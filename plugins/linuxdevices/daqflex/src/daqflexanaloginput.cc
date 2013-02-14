/*
  daqflex/daqflexanaloginput.h
  Interface for accessing analog input of a DAQFlex board from Measurement Computing.

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
#include <relacs/daqflex/daqflexanaloginput.h>
using namespace std;
using namespace relacs;

namespace daqflex {


DAQFlexAnalogInput::DAQFlexAnalogInput( void )
  : AnalogInput( "DAQFlexAnalogInput", DAQFlexAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  DAQFlexDevice = NULL;
  Traces = 0;
  ReadBufferSize = 0;
  BufferSize = 0;
  BufferN = 0;
  Buffer = NULL;
  TraceIndex = 0;
  TotalSamples = 0;
  CurrentSamples = 0;
}


DAQFlexAnalogInput::DAQFlexAnalogInput( DAQFlexCore &device, const Options &opts )
  : AnalogInput( "DAQFlexAnalogInput", DAQFlexAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  DAQFlexDevice = NULL;
  Traces = 0;
  ReadBufferSize = 0;
  BufferSize = 0;
  BufferN = 0;
  Buffer = NULL;
  TraceIndex = 0;
  TotalSamples = 0;
  CurrentSamples = 0;
  open( device, opts );
}


DAQFlexAnalogInput::~DAQFlexAnalogInput( void )
{
  close();
}


int DAQFlexAnalogInput::open( DAQFlexCore &daqflexdevice, const Options &opts )
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
  double checkrange[6] = { 10.0, 5.0, 2.0, 1.0, 0.5, -1.0 };
  for ( int i = 0; i < 20 && checkrange[i] > 0.0; i++ ) {
    string message = "AI{0}:RANGE=BIP" + Str( checkrange[i], "%g" ) + "V";
    DAQFlexDevice->sendMessage( message );
    string response = DAQFlexDevice->sendMessage( "?AI{0}:RANGE" );
    if ( response == message )
      BipolarRange.push_back( checkrange[i] );
  }

  reset();

  // clear flags:
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  TotalSamples = 0;
  CurrentSamples = 0;
  ReadBufferSize = 2 * DAQFlexDevice->aiFIFOSize();

  setInfo();

  return 0;
}


int DAQFlexAnalogInput::open( Device &device, const Options &opts )
{
  return open( dynamic_cast<DAQFlexCore&>( device ), opts );
}


bool DAQFlexAnalogInput::isOpen( void ) const
{
  return ( DAQFlexDevice != NULL && DAQFlexDevice->isOpen() );
}


void DAQFlexAnalogInput::close( void )
{
  if ( ! isOpen() )
    return;

  reset();

  // clear flags:
  DAQFlexDevice = NULL;
  IsPrepared = false;
  TraceIndex = 0;
  TotalSamples = 0;
  CurrentSamples = 0;

  Info.clear();
}


int DAQFlexAnalogInput::channels( void ) const
{
  if ( !isOpen() )
    return -1;
  return DAQFlexDevice->maxAIChannels();
}


int DAQFlexAnalogInput::bits( void ) const
{
  if ( !isOpen() )
    return -1;
  return (int)( log( DAQFlexDevice->maxAIData()+2.0 )/ log( 2.0 ) );
}


double DAQFlexAnalogInput::maxRate( void ) const
{
  return DAQFlexDevice->maxAIRate();
}


int DAQFlexAnalogInput::maxRanges( void ) const
{
  return BipolarRange.size();
}


double DAQFlexAnalogInput::unipolarRange( int index ) const
{
  return -1.0;
}


double DAQFlexAnalogInput::bipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)BipolarRange.size()) )
    return -1.0;
  return BipolarRange[index];
}


int DAQFlexAnalogInput::testReadDevice( InList &traces )
{
  int retVal = 0;

  for ( int k=0; k<traces.size(); k++ ) {
    if ( traces[k].gainIndex() < 0 ) {
      traces[k].addError( DaqError::InvalidGain );
      traces[k].setGainIndex( 0 );
    }
    if ( traces[k].unipolar() ) {
      traces[k].addError( DaqError::InvalidGain );
      traces[k].setUnipolar( false );
    }
    if ( traces[k].gainIndex() >= (int)BipolarRange.size() ) {
      traces[k].addError( DaqError::InvalidGain );
      traces[k].setGainIndex( BipolarRange.size()-1 );
    }
  }

  // check read buffer size:
  int readbufsize = traces.size() * traces[0].indices( traces[0].readTime() ) * 2;
  if ( readbufsize > ReadBufferSize ) {
    traces.addError( DaqError::InvalidBufferTime );
    traces.setReadTime( ReadBufferSize/traces.size()/2/traces[0].sampleRate() );
    retVal = -1;
  }

  // check update buffer size:
  int bufsize = traces.size() * traces[0].indices( traces[0].updateTime() ) * 2;
  if ( bufsize < readbufsize ) {
    traces.addError( DaqError::InvalidUpdateTime );
    retVal = -1;
  }

  return retVal;
}


int DAQFlexAnalogInput::prepareRead( InList &traces )
{
  if ( !isOpen() )
    return -1;

  reset();

  // init internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  BufferSize = 2 * traces.size() * traces[0].indices( traces[0].updateTime() ) * 2;
  int inps = DAQFlexDevice->inPacketSize();
  BufferSize = (BufferSize/inps+1)*inps;
  Buffer = new char[BufferSize];
  BufferN = 0;

  // setup acquisition:
  DAQFlexDevice->sendMessage( "AISCAN:XFRMODE=BLOCKIO" );
  DAQFlexDevice->sendMessage( "AISCAN:RATE=" + Str( traces[0].sampleRate(), "%g" ) );
  if ( traces[0].continuous() ) {
    DAQFlexDevice->sendMessage( "AISCAN:SAMPLES=0" );
    TotalSamples = 0;
  }
  else {
    DAQFlexDevice->sendMessage( "AISCAN:SAMPLES=" + Str( traces[0].size() ) );
    TotalSamples = traces[0].size() * traces.size();
  }
  CurrentSamples = 0;

  // setup channels:
  DAQFlexDevice->sendMessage( "AISCAN:QUEUE=ENABLE" );
  DAQFlexDevice->sendMessage( "AIQUEUE:CLEAR" );
  for( int k = 0; k < traces.size(); k++ ) {
    // DAQFlexDevice->sendMessage( "?AIQUEUE:COUNT" ); USE THIS AS QUEUE Element

    // delay:
    if ( traces[k].delay() > 0.0 ) {
      traces[k].addError( DaqError::InvalidDelay );
      traces[k].addErrorStr( "delays are not supported by DAQFlex!" );
      traces[k].setDelay( 0.0 );
    }

    string aiq = "AIQUEUE{" + Str( k ) + "}:";

    // channel:
    DAQFlexDevice->sendMessage( aiq + "CHAN=" + Str( traces[k].channel() ) );

    // reference:
    switch ( traces[k].reference() ) {
    case InData::RefCommon:
      DAQFlexDevice->sendMessage( aiq + "CHMODE=SE" );
      break;
    case InData::RefDifferential:
      DAQFlexDevice->sendMessage( aiq + "CHMODE=DIFF" );
      break;
    case InData::RefGround:
      DAQFlexDevice->sendMessage( aiq + "CHMODE=SE" );
      break;
    default:
      traces[k].addError( DaqError::InvalidReference );
    }

    // allocate gain factor:
    char *gaindata = traces[k].gainData();
    if ( gaindata != NULL )
      delete [] gaindata;
    gaindata = new char[sizeof(Calibration)];
    traces[k].setGainData( gaindata );
    Calibration *gainp = (Calibration *)gaindata;

    // ranges:
    if ( traces[k].unipolar() ) {
      traces[k].addError( DaqError::InvalidGain );
    }
    else {
      double max = BipolarRange[traces[k].gainIndex()];
      if ( max < 0 )
	traces[k].addError( DaqError::InvalidGain );
      else {
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( -max );
	string message = aiq + "RANGE=BIP" + Str( max, "%g" ) + "V";
	DAQFlexDevice->sendMessage( message );
	// get calibration:
	string response = DAQFlexDevice->sendMessage( "?AI{" + Str( traces[k].channel() ) + "}:SLOPE" );
	gainp->Slope = Str( response.erase( 0, 12 ) ).number();
	response = DAQFlexDevice->sendMessage( "?AI{" + Str( traces[k].channel() ) + "}:OFFSET" );
	gainp->Offset = Str( response.erase( 0, 13 ) ).number();
	gainp->Slope *= 2.0*max/DAQFlexDevice->maxAIData();
	gainp->Offset *= 2.0*max/DAQFlexDevice->maxAIData();
	gainp->Offset -= max;
      }
    }
  }
  //    DAQFlexDevice->sendMessage( "?AIQUEUE:COUNT" );

  if ( traces.failed() )
    return -1;

  if ( traces.success() ) {
    setSettings( traces, BufferSize, ReadBufferSize );
    Traces = &traces;
  }

  IsPrepared = traces.success();

  return traces.success() ? 0 : -1;
}


int DAQFlexAnalogInput::startRead( void )
{
  if ( !IsPrepared || Traces == 0 ) {
    cerr << "AI not prepared or no traces!\n";
    return -1;
  }
  DAQFlexDevice->sendMessage( "AISCAN:START" );
  IsRunning = true;
  return 0;
}


int DAQFlexAnalogInput::readData( void )
{
  //  cerr << "DAQFlex::readData() start\n";
  if ( Traces == 0 || Buffer == 0 || ! IsRunning )
    return -1;

  ErrorState = 0;
  bool failed = false;
  int readn = 0;
  int buffern = BufferN*2;
  int inps = DAQFlexDevice->inPacketSize();
  int maxn = ((BufferSize - buffern)/inps)*inps;
  if ( maxn <= 0 && ! IsRunning )
    maxn = BufferSize - buffern;

  // try to read twice:
  for ( int tryit = 0; tryit < 2 && ! failed && maxn > 0; tryit++ ) {

    // read data:
    int m = 0;
    int err = libusb_bulk_transfer( DAQFlexDevice->deviceHandle(),
				    DAQFlexDevice->endpointIn(),
				    (unsigned char*)(Buffer + buffern),
				    maxn, &m, 100 );

    if ( err != 0 && err != LIBUSB_ERROR_TIMEOUT ) {
      Traces->addErrorStr( "LibUSB error " + Str( err ) );
      if ( err == LIBUSB_ERROR_OVERFLOW ) {
	ErrorState = 1;
	Traces->addError( DaqError::OverflowUnderrun );
      }
      failed = true;
      cerr << " DAQFlexAnalogInput::readData(): libUSB error " << err << "\n";
    }
    else if ( m > 0 ) {
      buffern += m;
      readn += m;
      maxn = ((BufferSize - buffern)/inps)*inps;
    }

  }

  BufferN = buffern / 2;
  readn /= 2;
  CurrentSamples += readn;

  if ( failed )
    return -1;

  // no more data to be read:
  if ( readn <= 0 && !running() ) {
    if ( IsRunning && ( TotalSamples <=0 || CurrentSamples < TotalSamples ) ) {
      Traces->addErrorStr( deviceFile() + " - buffer-overflow " );
      Traces->addError( DaqError::OverflowUnderrun );
      ErrorState = 1;
      cerr << " DAQFlexAnalogInput::readData(): no data and not running\n";
    }
    return -1;
  }

  return readn;
}


int DAQFlexAnalogInput::convertData( void )
{
  if ( Traces == 0 || Buffer == 0 )
    return -1;

  // conversion factors and scale factors:
  double scale[Traces->size()];
  const Calibration* calib[Traces->size()];
  for ( int k=0; k<Traces->size(); k++ ) {
    scale[k] = (*Traces)[k].scale();
    calib[k] = (const Calibration *)(*Traces)[k].gainData();
  }

  // trace buffer pointers and sizes:
  float *bp[Traces->size()];
  int bm[Traces->size()];
  int bn[Traces->size()];
  for ( int k=0; k<Traces->size(); k++ ) {
    bp[k] = (*Traces)[k].pushBuffer();
    bm[k] = (*Traces)[k].maxPush();
    bn[k] = 0;
  }

  // type cast for device buffer:
  unsigned short *db = (unsigned short *)Buffer;

  for ( int k=0; k<BufferN; k++ ) {
    // convert:
    *bp[TraceIndex] = (float) db[k]*calib[TraceIndex]->Slope + calib[TraceIndex]->Offset;
    *bp[TraceIndex] *= scale[TraceIndex];
    // update pointers:
    bp[TraceIndex]++;
    bn[TraceIndex]++;
    if ( bn[TraceIndex] >= bm[TraceIndex] ) {
      (*Traces)[TraceIndex].push( bn[TraceIndex] );
      bp[TraceIndex] = (*Traces)[TraceIndex].pushBuffer();
      bm[TraceIndex] = (*Traces)[TraceIndex].maxPush();
      bn[TraceIndex] = 0;
    }
    // next trace:
    TraceIndex++;
    if ( TraceIndex >= Traces->size() )
      TraceIndex = 0;
  }

  // commit:
  for ( int c=0; c<Traces->size(); c++ )
    (*Traces)[c].push( bn[c] );

  int n = BufferN;
  BufferN = 0;

  return n;
}


int DAQFlexAnalogInput::stop( void )
{
  if ( !isOpen() )
    return NotOpen;
  DAQFlexDevice->sendMessage( "AISCAN:STOP" );
  IsRunning = false;
  return 0;
}


int DAQFlexAnalogInput::reset( void )
{
  int retVal = stop();
  // clear overrun condition:
  DAQFlexDevice->sendControlTransfer( "AISCAN:RESET" , false );
  libusb_clear_halt( DAQFlexDevice->deviceHandle(),
		     DAQFlexDevice->endpointIn() );

  // flush:
  int numbytes = 0;
  int status = 0;
  do {
    const int nbuffer = DAQFlexDevice->inPacketSize()*4;
    unsigned char buffer[nbuffer];
    status = libusb_bulk_transfer( DAQFlexDevice->deviceHandle(),
				   DAQFlexDevice->endpointIn(),
				   buffer, nbuffer, &numbytes, 200 );
  } while ( numbytes > 0 && status == 0 );

  // free internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = NULL;
  BufferSize = 0;
  BufferN = 0;
  TotalSamples = 0;
  CurrentSamples = 0;

  Settings.clear();

  ErrorState = 0;
  IsPrepared = false;
  Traces = 0;
  TraceIndex = 0;

  return retVal;
}


bool DAQFlexAnalogInput::running( void ) const
{
  string response = DAQFlexDevice->sendMessage( "?AISCAN:STATUS", false );
  return ( response.find( "RUNNING" ) != string::npos );
}


int DAQFlexAnalogInput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */
}


}; /* namespace daqflex */
