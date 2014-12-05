/*
  daqflex/daqflexanalogoutput.h
  Interface for accessing analog output of a DAQFlex board from Measurement Computing.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  RELACS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <QMutexLocker>
#include <relacs/str.h>
#include <relacs/daqflex/daqflexanalogoutput.h>
using namespace std;
using namespace relacs;

namespace daqflex {


DAQFlexAnalogOutput::DAQFlexAnalogOutput( void )
  : AnalogOutput( "DAQFlexAnalogOutput", DAQFlexAnalogIOType )
{
  IsPrepared = false;
  NoMoreData = true;
  DAQFlexDevice = NULL;
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;
  ChannelValues = 0;
}


DAQFlexAnalogOutput::DAQFlexAnalogOutput( DAQFlexCore &device, const Options &opts ) 
  : AnalogOutput( "DAQFlexAnalogOutput", DAQFlexAnalogIOType )
{
  IsPrepared = false;
  NoMoreData = true;
  DAQFlexDevice = NULL;
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;
  ChannelValues = 0;
  open( device, opts );
}


DAQFlexAnalogOutput::~DAQFlexAnalogOutput( void )
{
  close();
}


int DAQFlexAnalogOutput::open( DAQFlexCore &daqflexdevice, const Options &opts )
{
  if ( isOpen() )
    return -5;

  Info.clear();
  Settings.clear();
  if ( &daqflexdevice == NULL )
    return InvalidDevice;

  DAQFlexDevice = &daqflexdevice;
  if ( !DAQFlexDevice->isOpen() )
    return NotOpen;

  // is AO supported?
  if ( DAQFlexDevice->maxAOChannels() == 0 ) {
    cerr << "Device " << DAQFlexDevice->deviceName() << " does not support anaolog output.\n";
    return InvalidDevice;
  }

  // set basic device infos:
  setDeviceName( DAQFlexDevice->deviceName() );
  setDeviceVendor( DAQFlexDevice->deviceVendor() );
  setDeviceFile( DAQFlexDevice->deviceFile() );

  // initialize ranges:
  BipolarRange.clear();
  UnipolarRange.clear();

  Str response = DAQFlexDevice->sendMessage( "?AO{0}:RANGE" );
  if ( DAQFlexDevice->success() && response.size() > 16 ) {
    // Analog output ranges:
    // 1608GX_2AO: BIP10V
    // 202, 205: UNI5V
    // 1208FS, 1408FS: UNI5V
    // 2408-2AO:  BIP10V
    // 7204: UNI4.096V
    bool uni = ( response[12] == 'U' );
    double range = response.number( 0.0, 15 );
    if ( range <= 1e-6 ) {
      cerr << "Failed to read out analog output range from device " << DAQFlexDevice->deviceName() << "\n";
      return InvalidDevice;
    }
    if ( uni )
      UnipolarRange.push_back( range );
    else
      BipolarRange.push_back( range );
  }
  else {
    cerr << "Failed to retrieve analog output range from device " << DAQFlexDevice->deviceName() << ". Error: " << DAQFlexDevice->errorStr() << "\n";
    return InvalidDevice;
  }

  // set default output values for channels:
  ChannelValues = new float[channels()];
  for ( int k=0; k<channels(); k++ )
    ChannelValues[k] = 0.0;

  // delays:
  vector< double > delays;
  opts.numbers( "delays", delays, "s" );
  setDelays( delays );

  reset();

  // clear flags:
  IsPrepared = false;
  NoMoreData = true;

  setInfo();

  return 0;
}


int DAQFlexAnalogOutput::open( Device &device, const Options &opts )
{
  return open( dynamic_cast<DAQFlexCore&>( device ), opts );
}


bool DAQFlexAnalogOutput::isOpen( void ) const
{
  lock();
  bool o = ( DAQFlexDevice != NULL && DAQFlexDevice->isOpen() );
  unlock();
  return o;
}


void DAQFlexAnalogOutput::close( void )
{
  if ( ! isOpen() )
    return;

  reset();

  // clean up stored channel values:
  if ( ChannelValues != 0 )
    delete[] ChannelValues;
  ChannelValues = 0;

  // clear flags:
  DAQFlexDevice = NULL;
  IsPrepared = false;
  NoMoreData = true;

  Info.clear();
}


int DAQFlexAnalogOutput::channels( void ) const
{
  if ( !isOpen() )
    return -1;
  return DAQFlexDevice->maxAOChannels();
}


int DAQFlexAnalogOutput::bits( void ) const
{
  if ( !isOpen() )
    return -1;
  return (int)( log( DAQFlexDevice->maxAOData()+2.0 )/ log( 2.0 ) );
}


double DAQFlexAnalogOutput::maxRate( void ) const
{
  return DAQFlexDevice->maxAORate();
}


int DAQFlexAnalogOutput::maxRanges( void ) const
{
  unsigned int n = BipolarRange.size();
  if ( n < UnipolarRange.size() )
    return UnipolarRange.size();
  else
    return n;
}


double DAQFlexAnalogOutput::unipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)UnipolarRange.size()) )
    return -1.0;
  return UnipolarRange[index];
}


double DAQFlexAnalogOutput::bipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)BipolarRange.size()) )
    return -1.0;
  return BipolarRange[index];
}


int DAQFlexAnalogOutput::directWrite( OutList &sigs )
{
  // no signals:
  if ( sigs.size() == 0 )
    return -1;

  QMutexLocker locker( mutex() );

  for ( int k=0; k<sigs.size(); k++ ) {

    // we use only the largest range:
    sigs[k].setGainIndex( 0 );
    if ( BipolarRange.size() > 0 ) {
      sigs[k].setMinVoltage( -BipolarRange[0] );
      sigs[k].setMaxVoltage( BipolarRange[0] );
      if ( ! sigs[k].noLevel() )
	sigs[k].multiplyScale( BipolarRange[0] );
    }
    else {
      sigs[k].setMinVoltage( 0.0 );
      sigs[k].setMaxVoltage( UnipolarRange[0] );
      if ( ! sigs[k].noLevel() )
	sigs[k].multiplyScale( UnipolarRange[0] );
    }

    double maxboardvolt = sigs[k].maxVoltage();
    double minval = sigs[k].minValue();
    double maxval = sigs[k].maxValue();
    double gain = DAQFlexDevice->maxAOData()/(maxval-minval);
    double scale = sigs[k].scale();
    if ( ! sigs[k].noLevel() )
      scale *= maxboardvolt;

    // apply range:
    float v = sigs[k].size() > 0 ? sigs[k][0] : 0.0;
    if ( v > maxval )
      v = maxval;
    else if ( v < minval )
      v = minval;
    v *= scale;
    unsigned short data = (unsigned short)( (v-minval)*gain );

    // write data:
    string response = DAQFlexDevice->sendMessage( "AO{" + Str( sigs[k].channel() ) + "}:VALUE=" + Str( data ) );
    //    if ( retval < 1 )
    //      sigs[k].addErrorStr( "DAQFlex direct write failed" );

    ChannelValues[sigs[k].channel()] = sigs[k].size() > 0 ? sigs[k][0] : 0.0;

  }

  return ( sigs.success() ? 0 : -1 );
}


template < typename T >
int DAQFlexAnalogOutput::convert( char *cbuffer, int nbuffer )
{
  if ( nbuffer < (int)sizeof( T ) )
    return 0;

  // conversion polynomials and scale factors:
  double minval[ Sigs.size() ];
  double maxval[ Sigs.size() ];
  double gain[ Sigs.size() ];
  double scale[ Sigs.size() ];
  //  const Calibration* calib[Sigs.size()];
  T zeros[ Sigs.size() ];
  for ( int k=0; k<Sigs.size(); k++ ) {
    minval[k] = Sigs[k].minValue();
    maxval[k] = Sigs[k].maxValue();
    gain[k] = DAQFlexDevice->maxAOData()/(maxval[k]-minval[k]);
    scale[k] = Sigs[k].scale();
    // calib[k] = (const Calibration *)Sigs[k].gainData();
    // XXX calibration?
    float v = ChannelValues[Sigs[k].channel()];
    if ( v > maxval[k] )
      v = maxval[k];
    else if ( v < minval[k] ) 
      v = minval[k];
    v *= scale[k];
    zeros[k] = (unsigned short)( (v-minval[k])*gain[k] );
  }

  // buffer pointer:
  T *bp = (T*)cbuffer;
  int maxn = nbuffer/sizeof( T )/Sigs.size();
  int n = 0;

  // convert data and multiplex into buffer:
  for ( int i=0; i<maxn && Sigs[0].deviceWriting(); i++ ) {
    for ( int k=0; k<Sigs.size(); k++ ) {
      if ( Sigs[k].deviceCount() < 0 ) {
	*bp = zeros[k];
	Sigs[k].incrDeviceIndex();
	if ( Sigs[k].deviceIndex() >= Sigs[k].deviceDelay() )
	  Sigs[k].incrDeviceCount();
      }
      else {
	float v = Sigs[k].deviceValue();
	if ( v > maxval[k] )
	  v = maxval[k];
	else if ( v < minval[k] )
	  v = minval[k];
	v *= scale[k];
	// XXX calibration?
	*bp = (unsigned short)( (v-minval[k])*gain[k] );
	if ( Sigs[k].deviceIndex() >= Sigs[k].size() )
	  Sigs[k].incrDeviceCount();
      }
      ++bp;
      ++n;
    }
  }

  // memorize last values:
  for ( int k=0; k<Sigs.size(); k++ ) {
    if ( Sigs[k].deviceCount() >= 0 && Sigs[k].deviceIndex() > 0 )
      ChannelValues[Sigs[k].channel()] = Sigs[k][Sigs[k].deviceIndex()-1];
    else if ( Sigs[k].deviceCount() > 0 && Sigs[k].deviceIndex() == 0 )
      ChannelValues[Sigs[k].channel()] = Sigs[k].back();
  }

  return n * sizeof( T );
}


int DAQFlexAnalogOutput::testWriteDevice( OutList &sigs )
{
  int retVal = 0;

  double buffertime = sigs[0].interval( 0xffff/sigs.size() );
  if ( buffertime < 0.001 ) {
    sigs.addError( DaqError::InvalidBufferTime );
    retVal = -1;
  }

  if ( sigs.size() > channels() ) {
    sigs.addError( DaqError::InvalidChannel );
    retVal = -1;
  }

  return retVal;
}


int DAQFlexAnalogOutput::prepareWrite( OutList &sigs )
{
  if ( !isOpen() )
    return -1;

  lock();

  if ( Buffer != 0 ) { // should not be necessary!
    delete [] Buffer;
    Buffer = 0;
    cerr << "DAQFlexAnalogOutput::prepareWrite() warning: Buffer was not freed!\n";
  }
  if ( NBuffer != 0 ) { // should not be necessary!
    cerr << "DAQFlexAnalogOutput::prepareWrite() warning: NBuffer=" << NBuffer << " is not zero!\n";
    NBuffer = 0;
  }

  unlock();

  reset();

  // no signals:
  if ( sigs.size() == 0 )
    return -1;

  {
    QMutexLocker locker( mutex() );

    /*
    // clear out board buffer.
    // this is not nice, since it actually writes data!
    DAQFlexDevice->sendMessage( "AOSCAN:LOWCHAN=0" );
    DAQFlexDevice->sendMessage( "AOSCAN:HIGHCHAN=0" );
    DAQFlexDevice->sendMessage( "AOSCAN:RATE=" + Str( maxRate(), "%g" ) );
    DAQFlexDevice->sendMessage( "AOSCAN:SAMPLES=0" );
    DAQFlexDevice->sendCommand( "AOSCAN:START" );
    double dummy = 1.0;
    for ( int k=0; k<100000; k++ )
    dummy *= (double)k/(double)(k+1);
    DAQFlexDevice->sendCommand( "AOSCAN:STOP" );
    */

    // copy and sort signal pointers:
    OutList ol;
    ol.add( sigs );
    ol.sortByChannel();

    // setup channels:
    DAQFlexDevice->sendMessage( "AOSCAN:LOWCHAN=" + Str( sigs[0].channel() ) );
    DAQFlexDevice->sendMessage( "AOSCAN:HIGHCHAN=" + Str( sigs.back().channel() ) );
    for( int k = 0; k < sigs.size(); k++ ) {
      // minimum and maximum values:
      double min = sigs[k].requestedMin();
      double max = sigs[k].requestedMax();
      if ( min == OutData::AutoRange || max == OutData::AutoRange ) {
	float smin = 0.0;
	float smax = 0.0;
	minMax( smin, smax, sigs[k] );
	if ( min == OutData::AutoRange )
	  min = smin;
	if ( max == OutData::AutoRange )
	  max = smax;
      }
      // we use only the largest range and there is only one range:
      sigs[k].setGainIndex( 0 );
      if ( BipolarRange.size() > 0 ) {
	sigs[k].setMinVoltage( -BipolarRange[0] );
	sigs[k].setMaxVoltage( BipolarRange[0] );
	if ( ! sigs[k].noLevel() )
	  sigs[k].multiplyScale( BipolarRange[0] );
      }
      else {
	sigs[k].setMinVoltage( 0.0 );
	sigs[k].setMaxVoltage( UnipolarRange[0] );
	if ( ! sigs[k].noLevel() )
	  sigs[k].multiplyScale( UnipolarRange[0] );
      }
      // check for signal overflow/underflow:
      if ( sigs[k].noLevel() ) {
	if ( min < sigs[k].minValue() )
	  sigs[k].addError( DaqError::Underflow );
	else if ( max > sigs[k].maxValue() )
	  sigs[k].addError( DaqError::Overflow );
      }
      else {
	if ( max > 1.0+1.0e-8 )
	  sigs[k].addError( DaqError::Overflow );
	else if ( min < -1.0-1.0e-8 )
	  sigs[k].addError( DaqError::Underflow );
      }

      // allocate gain factor:
      char *gaindata = sigs[k].gainData();
      if ( gaindata != NULL )
	delete [] gaindata;
      gaindata = new char[sizeof(Calibration)];
      sigs[k].setGainData( gaindata );
      Calibration *gainp = (Calibration *)gaindata;

      // get calibration:
      string response = DAQFlexDevice->sendMessage( "?AO{" + Str( sigs[k].channel() ) + "}:SLOPE" );
      gainp->Slope = Str( response.erase( 0, 12 ) ).number();
      response = DAQFlexDevice->sendMessage( "?AO{" + Str( sigs[k].channel() ) + "}:OFFSET" );
      gainp->Offset = Str( response.erase( 0, 13 ) ).number();
      /*
	gainp->Slope *= 2.0*max/DAQFlexDevice->maxAIData();
	gainp->Offset *= 2.0*max/DAQFlexDevice->maxAIData();
	gainp->Offset -= max;
      */

    }

    if ( ! ol.success() )
      return -1;

    int delayinx = ol[0].indices( ol[0].delay() );
    for ( int k=0; k<ol.size(); k++ )
      ol[k].deviceReset( delayinx );

    // setup acquisition:
    DAQFlexDevice->sendMessage( "AOSCAN:RATE=" + Str( sigs[0].sampleRate(), "%g" ) );
    if ( sigs[0].continuous() ) {
      Samples = 0;
      DAQFlexDevice->sendMessage( "AOSCAN:SAMPLES=0" );
    }
    else {
      Samples = sigs.deviceBufferSize();
      DAQFlexDevice->sendMessage( "AOSCAN:SAMPLES=" + Str( Samples ) );
    }

    // set buffer size:
    if ( DAQFlexDevice->aoFIFOSize() > 0 ) {
      BufferSize = sigs.size()*DAQFlexDevice->aoFIFOSize()*2;
      int nbuffer = sigs.deviceBufferSize()*2;
      int outps = DAQFlexDevice->outPacketSize();
      if ( BufferSize > nbuffer ) {
	BufferSize = nbuffer;
	if ( BufferSize < outps )
	BufferSize = outps;
      }
      else
	BufferSize = (BufferSize/outps+1)*outps; // round up to full package size
      if ( BufferSize > 0xfffff )
	BufferSize = 0xfffff;
    }
    else {
      BufferSize = sigs.deviceBufferSize()*2;
    }
    if ( BufferSize <= 0 )
      sigs.addError( DaqError::InvalidBufferTime );

    setSettings( ol, BufferSize );

    if ( ! ol.success() )
      return -1;

    Sigs = ol;
    Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

    if ( DAQFlexDevice->aoFIFOSize() <= 0 ) {
      // no FIFO and bulk transfer:
      convert<unsigned short>( Buffer, BufferSize );
    }

  }  // unlock MutexLocker
  int r = 1;
  if ( DAQFlexDevice->aoFIFOSize() > 0 ) {
    r = writeData();
    if ( r < 0 )
      return -1;
  }

  lock();
  IsPrepared = Sigs.success();
  NoMoreData = ( r == 0 );
  unlock();

  return 0;
}


int DAQFlexAnalogOutput::startWrite( QSemaphore *sp )
{
  QMutexLocker locker( mutex() );

  if ( !IsPrepared || Sigs.empty() ) {
    cerr << "AO not prepared or no signals!\n";
    return -1;
  }
  if ( DAQFlexDevice->aoFIFOSize() > 0 )
    DAQFlexDevice->sendCommand( "AOSCAN:START" );
  int r = NoMoreData ? 0 : 1;
  startThread( sp );
  return r;
}


int DAQFlexAnalogOutput::writeData( void )
{
  QMutexLocker locker( mutex() );

  if ( Sigs.empty() )
    return -1;

  if ( DAQFlexDevice->aoFIFOSize() <= 0 ) {
    unsigned short *bp = (unsigned short *)Buffer;
    for ( int k=0; k<Sigs.size(); k++ ) {
      unsigned short val = *(bp+NBuffer/2);
      string cmd = "AO{" + Str( Sigs[k].channel() ) + "}:VALUE=" + Str( val );
      DAQFlexDevice->sendMessage( cmd );
      NBuffer += 2;
    }
    // no more data:
    if ( NBuffer >= BufferSize ) {
      if ( Buffer != 0 )
	delete [] Buffer;
      Buffer = 0;
      BufferSize = 0;
      NBuffer = 0;
      return 0;
    }
    else
      return Sigs.size();
  }

  // device stopped?
  if ( IsPrepared ) {
    string response = DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );
    if ( response.find( "UNDERRUN" ) != string::npos ) {
      Sigs.addError( DaqError::OverflowUnderrun );
      return -1;
    }
  }

  if ( Sigs[0].deviceWriting() ) {
    // convert data into buffer:
    int bytesConverted = convert<unsigned short>( Buffer+NBuffer, BufferSize-NBuffer );
    NBuffer += bytesConverted;
  }

  if ( ! Sigs[0].deviceWriting() && NBuffer == 0 )
    return 0;

  // transfer buffer to device:
  int outps = DAQFlexDevice->outPacketSize();
  int bytesToWrite = (NBuffer/outps)*outps;
  if ( bytesToWrite > DAQFlexDevice->aoFIFOSize() * 2 )
    bytesToWrite = DAQFlexDevice->aoFIFOSize() * 2;
  else if ( NBuffer <= DAQFlexDevice->aoFIFOSize() * 2 )
    bytesToWrite = NBuffer;
  if ( bytesToWrite <= 0 )
    bytesToWrite = NBuffer;
  int timeout = (int)::ceil( 10.0 * 1000.0*Sigs[0].interval( bytesToWrite/2/Sigs.size() ) ); // in ms
  int bytesWritten = 0;
  //  cerr << "BULK START " << bytesToWrite << " TIMEOUT=" << timeout << "ms" << '\n';
  int ern = DAQFlexDevice->writeBulkTransfer( (unsigned char*)(Buffer), bytesToWrite,
					      &bytesWritten, timeout );

  int elemWritten = 0;
  if ( bytesWritten > 0 ) {
    memmove( Buffer, Buffer+bytesWritten, NBuffer-bytesWritten );
    NBuffer -= bytesWritten;
    elemWritten += bytesWritten / 2;
  }

  if ( ern == 0 ) {
    // no more data:
    if ( ! Sigs[0].deviceWriting() && NBuffer <= 0 ) {
      if ( Buffer != 0 )
	delete [] Buffer;
      Buffer = 0;
      BufferSize = 0;
      NBuffer = 0;
      return 0;
    }
  }
  else {
    // error:
    switch( ern ) {

    case LIBUSB_ERROR_PIPE:
      Sigs.addError( DaqError::OverflowUnderrun );
      return -1;

    case LIBUSB_ERROR_BUSY:
      Sigs.addError( DaqError::Busy );
      return -1;

    case LIBUSB_ERROR_TIMEOUT:
      cerr << "timeout in writing data\n";
      return -1;

    default:
      Sigs.addErrorStr( "Lib USB Error" );
      Sigs.addError( DaqError::Unknown );
      return -1;
    }
  }

  return elemWritten;
}


int DAQFlexAnalogOutput::reset( void )
{
  if ( ! isOpen() )
    return NotOpen;

  lock();
  DAQFlexDevice->sendCommand( "AOSCAN:STOP" );
  // clear underrun condition:
  DAQFlexDevice->sendMessage( "AOSCAN:RESET" );
  DAQFlexDevice->clearWrite();
  DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );
  unlock();

  stopWrite();

  lock();

  Sigs.clear();
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  Settings.clear();
  IsPrepared = false;

  unlock();

  return 0;
}


AnalogOutput::Status DAQFlexAnalogOutput::status( void ) const
{
  Status r = Idle;
  lock();
  string response = DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );
  if ( response.find( "RUNNING" ) != string::npos )
    r = Running;
  if ( response.find( "UNDERRUN" ) != string::npos ) {
    Sigs.addError( DaqError::OverflowUnderrun );
    r = Underrun;
  }
  unlock();
  return r;
}


bool DAQFlexAnalogOutput::prepared( void ) const 
{ 
  lock();
  bool ip = IsPrepared;
  unlock();
  return ip;
}


bool DAQFlexAnalogOutput::noMoreData( void ) const
{
  lock();
  bool nmd = NoMoreData;
  unlock();
  return nmd;
}


}; /* namespace daqflex */
