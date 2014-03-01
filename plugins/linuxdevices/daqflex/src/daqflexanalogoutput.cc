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

  // set basic device infos:
  setDeviceName( DAQFlexDevice->deviceName() );
  setDeviceVendor( DAQFlexDevice->deviceVendor() );
  setDeviceFile( DAQFlexDevice->deviceFile() );

  // initialize ranges:
  BipolarRange.clear();
  BipolarRange.push_back( 10.0 );

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
  return BipolarRange.size();
}


double DAQFlexAnalogOutput::unipolarRange( int index ) const
{
  return -1.0;
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

  lock();

  const double maxboardvolt = 10.0;  // XXX is this really the same for all boards?

  for ( int k=0; k<sigs.size(); k++ ) {

    // XXX what about output gain settings?

    double minval = sigs[k].minValue();
    double maxval = sigs[k].maxValue();
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
    // XXX    unsigned short data = comedi_from_physical( v, polynomial );
    unsigned short data = (unsigned short)((v+10.0)/20.0 * 0xffff);

    // write data:
    string response = DAQFlexDevice->sendMessage( "AO{" + Str( sigs[k].channel() ) + "}:VALUE=" + Str( data ) );
    //    if ( retval < 1 )
    //      sigs[k].addErrorStr( "DAQFlex direct write failed" );

  }

  unlock();

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
  double scale[ Sigs.size() ];
  //  const Calibration* calib[Sigs.size()];
  T zeros[ Sigs.size() ];
  for ( int k=0; k<Sigs.size(); k++ ) {
    minval[k] = Sigs[k].minValue();
    maxval[k] = Sigs[k].maxValue();
    scale[k] = Sigs[k].scale();
    // calib[k] = (const Calibration *)Sigs[k].gainData();
    // XXX calibration?
    zeros[k] = (unsigned short)( (10.0/20.0) * 0xffff );
    // XXX    zeros[k] = comedi_from_physical( 0.0, calib[k] );
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
	*bp = (unsigned short)( ((v+10.0)/20.0) * 0xffff );
	if ( Sigs[k].deviceIndex() >= Sigs[k].size() )
	  Sigs[k].incrDeviceCount();
      }
      ++bp;
      ++n;
    }
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

  lock();

  /*
  // clear out board buffer.
  // this is not nice, since it actually writes data!
  DAQFlexDevice->sendMessage( "AOSCAN:LOWCHAN=0" );
  DAQFlexDevice->sendMessage( "AOSCAN:HIGHCHAN=0" );
  DAQFlexDevice->sendMessage( "AOSCAN:RATE=" + Str( maxRate(), "%g" ) );
  DAQFlexDevice->sendMessage( "AOSCAN:SAMPLES=0" );
  DAQFlexDevice->sendMessage( "AOSCAN:START" );
  double dummy = 1.0;
  for ( int k=0; k<100000; k++ )
    dummy *= (double)k/(double)(k+1);
  DAQFlexDevice->sendMessage( "AOSCAN:STOP" );
  */

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // setup channels:
  DAQFlexDevice->sendMessage( "AOSCAN:LOWCHAN=" + Str( sigs[0].channel() ) );
  DAQFlexDevice->sendMessage( "AOSCAN:HIGHCHAN=" + Str( sigs.back().channel() ) );
  for( int k = 0; k < sigs.size(); k++ ) {

    // we use only the largest range:
    sigs[k].setGainIndex( 0 );
    sigs[k].setMinVoltage( -BipolarRange[0] );
    sigs[k].setMaxVoltage( BipolarRange[0] );
    if ( ! sigs[k].noLevel() )
      sigs[k].multiplyScale( BipolarRange[0] );

    // XXX Check for signal overflow/underflow!

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

  if ( ! ol.success() ) {
    unlock();
    return -1;
  }

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
  if ( BufferSize <= 0 )
    sigs.addError( DaqError::InvalidBufferTime );

  setSettings( ol, BufferSize );

  if ( ! ol.success() ) {
    unlock();
    return -1;
  }

  Sigs = ol;
  Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

  //  cerr << "STARTWRITE SCALE=" << Sigs[0].scale() << '\n';
  int r = fillWriteBuffer();
  IsPrepared = ol.success();

  if ( r < 0 ) {
    unlock();
    return -1;
  }

  NoMoreData = ( r == 0 );

  unlock();

  return 0;
}


int DAQFlexAnalogOutput::startWrite( void )
{
  lock();

  if ( !IsPrepared || Sigs.empty() ) {
    cerr << "AO not prepared or no signals!\n";
    unlock();
    return -1;
  }
  DAQFlexDevice->sendMessage( "AOSCAN:START" );
  int r = NoMoreData ? 0 : 1;
  unlock();
  return r;
}


int DAQFlexAnalogOutput::writeData( void )
{
  lock();
  if ( Sigs.empty() ) {
    unlock();
    return -1;
  }

  // device stopped?
  string response = DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );
  if ( response.find( "UNDERRUN" ) != string::npos ) {
    Sigs.addError( DaqError::OverflowUnderrun );
    unlock();
    return -1;
  }

  int r = fillWriteBuffer();

  unlock();

  return r;
}


int DAQFlexAnalogOutput::reset( void )
{
  bool o = isOpen();

  lock();

  Sigs.clear();
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  if ( !o )
    return NotOpen;

  DAQFlexDevice->sendMessage( "AOSCAN:STOP" );
  // clear underrun condition:
  DAQFlexDevice->sendControlTransfer( "AOSCAN:RESET", false );
  libusb_clear_halt( DAQFlexDevice->deviceHandle(),
		     DAQFlexDevice->endpointIn() );
  DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );

  Settings.clear();
  IsPrepared = false;

  unlock();

  return 0;
}


bool DAQFlexAnalogOutput::running( void ) const
{
  lock();
  string response = DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );
  bool r = ( response.find( "RUNNING" ) != string::npos );
  unlock();
  return r;
}


int DAQFlexAnalogOutput::fillWriteBuffer( void )
{
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
  if ( bytesToWrite <= 0 )
    bytesToWrite = NBuffer;
  int timeout = (int)::ceil( 1000.0*Sigs[0].interval( bytesToWrite/2/Sigs.size() ) ); // in ms
  int bytesWritten = 0;
  //    cerr << "BULK START " << bytesToWrite << '\n';
  int ern = libusb_bulk_transfer( DAQFlexDevice->deviceHandle(),
				  DAQFlexDevice->endpointOut(),
				  (unsigned char*)(Buffer), bytesToWrite,
				  &bytesWritten, timeout );
  //    cerr << "BULK END " << bytesWritten << " ern=" << ern << '\n';

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
      break;

    default:
      Sigs.addErrorStr( "Lib USB Error" );
      Sigs.addError( DaqError::Unknown );
      return -1;
    }
  }

  return elemWritten;
}


}; /* namespace daqflex */
