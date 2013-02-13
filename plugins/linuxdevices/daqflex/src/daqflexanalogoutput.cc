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


  // WORKS ONLY FOR 2ms updatetimes! XXXX Check what special buffersize this results in! That makes 512 Bytes - the packet size! XXX


DAQFlexAnalogOutput::DAQFlexAnalogOutput( void ) 
  : AnalogOutput( "DAQFlexAnalogOutput", DAQFlexAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  DAQFlexDevice = NULL;
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;
}


DAQFlexAnalogOutput::DAQFlexAnalogOutput( DAQFlexCore &device, const Options &opts ) 
  : AnalogOutput( "DAQFlexAnalogOutput", DAQFlexAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
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
  ErrorState = 0;
  IsPrepared = false;

  setInfo();
  
  return 0;
}


int DAQFlexAnalogOutput::open( Device &device, const Options &opts )
{
  return open( dynamic_cast<DAQFlexCore&>( device ), opts );
}


bool DAQFlexAnalogOutput::isOpen( void ) const 
{ 
  return ( DAQFlexDevice != NULL && DAQFlexDevice->isOpen() );
}


void DAQFlexAnalogOutput::close( void ) 
{
  if ( ! isOpen() )
    return;

  reset();

  // clear flags:
  DAQFlexDevice = NULL;
  IsPrepared = false;

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

  for ( int k=0; k<sigs.size(); k++ ) {

    double minval = sigs[k].minValue();
    double maxval = sigs[k].maxValue();
    double scale = sigs[k].scale();
    
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
  const Calibration* calib[Sigs.size()];
  T zeros[ Sigs.size() ];
  for ( int k=0; k<Sigs.size(); k++ ) {
    minval[k] = Sigs[k].minValue();
    maxval[k] = Sigs[k].maxValue();
    scale[k] = Sigs[k].scale();
    calib[k] = (const Calibration *)Sigs[k].gainData();
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
	//	cerr << "V=" << v << " data=" << *bp << '\n';
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
  if ( buffertime < sigs[0].writeTime() ) {
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

  reset();

  // no signals:
  if ( sigs.size() == 0 )
    return -1;

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

    sigs[k].setGainIndex( 0 );
    sigs[k].setMinVoltage( -BipolarRange[0] );
    sigs[k].setMaxVoltage( BipolarRange[0] );
    if ( ! sigs[k].noIntensity() )
      sigs[k].setScale( BipolarRange[0] );

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
  int bi = sigs[0].indices( 10.0 * sigs[0].writeTime() );
  if ( bi <= 0 )
    bi = 100;
  BufferSize = sigs.size()*bi*2;
  int nbuffer = sigs.deviceBufferSize()*2;
  if ( BufferSize > nbuffer )
    BufferSize = nbuffer;
  int outps = DAQFlexDevice->outPacketSize();
  BufferSize = (BufferSize/outps+1)*outps; // round up to full package size
  if ( BufferSize < outps )
    BufferSize = outps;
  if ( BufferSize > 0xfffff )
    sigs.addError( DaqError::InvalidBufferTime );
  if ( BufferSize <= 0 )
    sigs.addError( DaqError::NoData );

  setSettings( ol, BufferSize );

  if ( ! ol.success() )
    return -1;

  Sigs = ol;
  if ( Buffer != 0 ) { // should not be necessary!
    delete [] Buffer;
    cerr << "DAQFlexAnalogOutput::prepareWrite() warning: Buffer was not freed!\n";
  }
  if ( NBuffer != 0 ) { // should not be necessary!
    cerr << "DAQFlexAnalogOutput::prepareWrite() warning: NBuffer=" << NBuffer << " is not zero!\n";
    NBuffer = 0;
  }
  Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

  cerr << "STARTWRITE SCALE=" << Sigs[0].scale() << '\n';
  fillWriteBuffer( 0 );

  IsPrepared = ol.success();

  return 0;
}


int DAQFlexAnalogOutput::startWrite( void )
{
  if ( !IsPrepared || Sigs.empty() ) {
    cerr << "AO not prepared or no signals!\n";
    return -1;
  }
  DAQFlexDevice->sendMessage( "AOSCAN:START" );
  fillWriteBuffer( 1 );
  return 0;
}


int DAQFlexAnalogOutput::writeData( void )
{
  if ( Sigs.empty() )
    return -1;

  // device stopped?
  string response = DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );
  if ( response.find( "UNDERRUN" ) != string::npos ) {
    ErrorState = 1;
    Sigs.addError( DaqError::OverflowUnderrun );
    return -1;
  }

  return fillWriteBuffer( 2 );
}


int DAQFlexAnalogOutput::reset( void ) 
{ 
  Sigs.clear();
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  if ( !isOpen() )
    return NotOpen;

  DAQFlexDevice->sendMessage( "AOSCAN:STOP" );
  // clear underrun condition:
  DAQFlexDevice->sendControlTransfer( "AOSCAN:RESET", false );
  libusb_clear_halt( DAQFlexDevice->deviceHandle(), 
		     DAQFlexDevice->endpointIn() );
  DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );

  Settings.clear();
  ErrorState = 0;
  IsPrepared = false;

  return 0;
}


bool DAQFlexAnalogOutput::running( void ) const
{   
  string response = DAQFlexDevice->sendMessage( "?AOSCAN:STATUS" );
  return ( response.find( "RUNNING" ) != string::npos );
}


int DAQFlexAnalogOutput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */
}


int DAQFlexAnalogOutput::fillWriteBuffer( int stage )
{
  if ( !isOpen() ) {
    Sigs.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  ErrorState = 0;

  if ( ! Sigs[0].deviceWriting() ) {
    Sigs.addError( DaqError::NoData );
    return -1;
  }

  int maxntry = 4;
  if ( stage==0 ) {
    // XXX figure out FIFO size!
    maxntry = 100000/BufferSize;
    if ( maxntry <= 0 )
      maxntry = 1;
  }
  
  int ern = 0;
  int elemWritten = 0;
  int outps = DAQFlexDevice->outPacketSize();
  
  // try to write twice
  for ( int tryit = 0;
	tryit < maxntry && Sigs[0].deviceWriting(); 
	tryit++ ) {
    
    // convert data into buffer:
    int bytesConverted = convert<unsigned short>( Buffer+NBuffer, BufferSize-NBuffer );
    NBuffer += bytesConverted;
    if ( NBuffer <= 0 )
      break;

    // transfer buffer to device:
    /*
    int bytesToWrite = (NBuffer/outps)*outps; //  XXX
    if ( bytesToWrite <= 0 )
      bytesToWrite = NBuffer;
    */
    int bytesToWrite = NBuffer; //  XXX
    //    if ( stage==0 && bytesToWrite > 4*4096 ) // XXX figure out FIFO size!
    //      bytesToWrite = 4*4096;
    int bytesWritten = 0;
    ern = libusb_bulk_transfer( DAQFlexDevice->deviceHandle(),
				DAQFlexDevice->endpointOut(),
				(unsigned char*)(Buffer), bytesToWrite,
				&bytesWritten, 50 );

    if ( bytesWritten > 0 ) {
      memmove( Buffer, Buffer+bytesWritten, NBuffer-bytesWritten );
      NBuffer -= bytesWritten;
      elemWritten += bytesWritten / 2;
    }
    if ( ern != 0 )
      break;
  }

  if ( ern == 0 ) {
    // no more data:
    if ( ! Sigs[0].deviceWriting() ) {
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
      ErrorState = 1;
      Sigs.addError( DaqError::OverflowUnderrun );
      return -1;

    case LIBUSB_ERROR_BUSY:
      ErrorState = 2;
      Sigs.addError( DaqError::Busy );
      return -1;

    case LIBUSB_ERROR_TIMEOUT:
      ErrorState = 0;
      break;

    default:
      ErrorState = 2;
      Sigs.addErrorStr( "Lib USB Error" );
      Sigs.addError( DaqError::Unknown );
      return -1;
    }
  }
  
  return elemWritten;
}


}; /* namespace daqflex */
