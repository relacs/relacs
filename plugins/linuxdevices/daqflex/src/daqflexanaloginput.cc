/*
  daqflex/daqflexanaloginput.h
  Interface for accessing analog input of a DAQFlex board from Measurement Computing.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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
#include <relacs/daqflex/daqflexanaloginput.h>
using namespace std;
using namespace relacs;

namespace daqflex {


DAQFlexAnalogInput::DAQFlexAnalogInput( void )
  : AnalogInput( "DAQFlexAnalogInput", DAQFlexAnalogIOType )
{
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
  TakeAO = true;
  DAQFlexAO = 0;

  initOptions();
}


DAQFlexAnalogInput::DAQFlexAnalogInput( DAQFlexCore &device, const Options &opts )
  : DAQFlexAnalogInput()
{
  Options::read(opts);
  addBoolean( "takeao", "Start analog output in a single instruction", true );
}


DAQFlexAnalogInput::~DAQFlexAnalogInput( void )
{
  close();
}

void DAQFlexAnalogInput::initOptions()
{
  AnalogInput::initOptions();

  addBoolean("takeao", "dummy description", true);
}


int DAQFlexAnalogInput::open( DAQFlexCore &daqflexdevice )
{
  if ( isOpen() )
    return -5;

  Info.clear();
  Settings.clear();

  DAQFlexDevice = &daqflexdevice;
  if ( !DAQFlexDevice->isOpen() ) {
    setErrorStr( "Daqflex core device " + DAQFlexDevice->deviceName() + " is not open." );
    return NotOpen;
  }

  // set basic device infos:
  setDeviceName( DAQFlexDevice->deviceName() );
  setDeviceVendor( DAQFlexDevice->deviceVendor() );
  setDeviceFile( DAQFlexDevice->deviceFile() );

  {
    QMutexLocker corelocker( DAQFlexDevice->mutex() );
    // initialize ranges:
    BipolarRange.clear();
    BipolarRangeCmds.clear();
    double checkrange[17] = { 20.0, 10.0, 5.0, 4.0, 2.5, 2.5, 2.0, 1.25, 1.25, 1.0, 0.625, 0.3125, 0.15625, 0.14625, 0.078125, 0.073125, -1.0 };
    string checkstr[16] = { "BIP20V", "BIP10V", "BIP5V", "BIP4V", "BIP2PT5V", "BIP2.5V", "BIP2V", "BIP1PT25V", "BIP1.25V", "BIP1V", "BIP625.0E-3V", "BIP312.5E-3V", "BIP156.25E-3V", "BIP146.25E-3V", "BIP78.125E-3V", "BIP73.125E-3V" };
    for ( int i = 0; i < 20 && checkrange[i] > 0.0; i++ ) {
      string message = "AI{0}:RANGE=" + checkstr[i];
      DAQFlexDevice->sendMessageUnlocked( message );
      if ( DAQFlexDevice->success() ) {
	string response = DAQFlexDevice->sendMessageUnlocked( "?AI{0}:RANGE" );
	if ( DAQFlexDevice->success() && response == message ) {
	  BipolarRange.push_back( checkrange[i] );
	  BipolarRangeCmds.push_back( checkstr[i] );
	}
      }
    }
    if ( BipolarRange.size() == 0 ) {
      if ( DAQFlexDevice->error() == DAQFlexCore::ErrorLibUSBIO ) {
	setErrorStr( "Error in initializing DAQFlexAnalogInput device: no input ranges found. Error: " +
		     DAQFlexDevice->daqflexErrorStr() + ". Check the USB cable/connection!" );
	return ReadError;
      }
      // retrieve single supported range:
      Str response = DAQFlexDevice->sendMessageUnlocked( "?AI{0}:RANGE" );
      if ( DAQFlexDevice->success() && response.size() > 16 ) {
	bool uni = ( response[12] == 'U' );
	double range = response.number( 0.0, 15 );
	if ( range <= 1e-6 || uni ) {
	  setErrorStr( "Failed to read out analog input range from device " + DAQFlexDevice->deviceName() );
	  return InvalidDevice;
	}
	BipolarRange.push_back( range );
	BipolarRangeCmds.push_back( response.right( 12 ) );
      }
      else {
	setErrorStr( "Failed to retrieve analog input range from device " + DAQFlexDevice->deviceName() +
		     ". Error: " + DAQFlexDevice->daqflexErrorStr() );
	return InvalidDevice;
      }
    }
  }
  
  // clear flags:
  IsPrepared = false;
  IsRunning = false;
  TotalSamples = 0;
  CurrentSamples = 0;
  ReadBufferSize = 2 * DAQFlexDevice->aiFIFOSize();

  // For debugging:
  TakeAO = boolean( "takeao", true );
  DAQFlexAO = 0;

  setInfo();

  return 0;
}


int DAQFlexAnalogInput::open( Device &device )
{
  return open( dynamic_cast<DAQFlexCore&>( device ) );
}


bool DAQFlexAnalogInput::isOpen( void ) const
{
  lock();
  bool o = ( DAQFlexDevice != NULL && DAQFlexDevice->isOpen() );
  unlock();
  return o;
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
  TakeAO = true;
  DAQFlexAO = 0;

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

  return retVal;
}


int DAQFlexAnalogInput::prepareRead( InList &traces )
{
  if ( !isOpen() ) {
    traces.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  QMutexLocker ailocker( mutex() );

  Settings.clear();
  IsPrepared = false;
  Traces = 0;
  TraceIndex = 0;

  // init internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  // 2 times the updatetime ...
  BufferSize = 2 * traces.size() * traces[0].indices( traces[0].updateTime() ) * 2;
  // ... as a multiple of the packet size:
  int inps = DAQFlexDevice->inPacketSize();
  BufferSize = (BufferSize/inps+1)*inps;
  Buffer = new char[BufferSize];
  BufferN = 0;

  {
    QMutexLocker corelocker( DAQFlexDevice->mutex() );

    // setup acquisition:
    DAQFlexDevice->sendMessageUnlocked( "AISCAN:XFRMODE=BLOCKIO" );
    if ( DAQFlexDevice->failed() ) {
      traces.setErrorStr( DAQFlexDevice->daqflexErrorStr() );
      return -1;
    } 
    DAQFlexDevice->sendMessageUnlocked( "AISCAN:RATE=" + Str( traces[0].sampleRate(), "%g" ) );
    if ( DAQFlexDevice->failed() ) {
      traces.setErrorStr( DAQFlexDevice->daqflexErrorStr() );
      return -1;
    } 
    DAQFlexDevice->setAISampleRate( traces[0].sampleRate() );
    if ( traces[0].continuous() ) {
      DAQFlexDevice->sendMessageUnlocked( "AISCAN:SAMPLES=0" );
      TotalSamples = 0;
    }
    else {
      DAQFlexDevice->sendMessageUnlocked( "AISCAN:SAMPLES=" + Str( traces[0].size() ) );
      TotalSamples = traces[0].size() * traces.size();
    }
    if ( DAQFlexDevice->failed() ) {
      traces.setErrorStr( DAQFlexDevice->daqflexErrorStr() );
      return -1;
    } 
    CurrentSamples = 0;

    // setup channels:
    DAQFlexDevice->sendMessageUnlocked( "AISCAN:QUEUE=ENABLE" );
    if ( DAQFlexDevice->failed() ) {
      traces.setErrorStr( DAQFlexDevice->daqflexErrorStr() );
      return -1;
    } 
    DAQFlexDevice->sendMessageUnlocked( "AIQUEUE:CLEAR" );
    if ( DAQFlexDevice->failed() ) {
      traces.setErrorStr( DAQFlexDevice->daqflexErrorStr() );
      return -1;
    } 
    for( int k = 0; k < traces.size(); k++ ) {
      // DAQFlexDevice->sendMessageUnlocked( "?AIQUEUE:COUNT" ); USE THIS AS QUEUE Element

      // delay:
      if ( traces[k].delay() > 0.0 ) {
	traces[k].addError( DaqError::InvalidDelay );
	traces[k].addErrorStr( "delays are not supported by DAQFlex analog input!" );
	traces[k].setDelay( 0.0 );
      }

      // XXX 7202, 7204 do not have AIQUEUE! channels need to be in a sequence!
      string aiq = "AIQUEUE{" + Str( k ) + "}:";

      // channel:
      DAQFlexDevice->sendMessageUnlocked( aiq + "CHAN=" + Str( traces[k].channel() ) );
      if ( DAQFlexDevice->failed() ) {
	traces[k].setErrorStr( DAQFlexDevice->daqflexErrorStr() );
	return -1;
      } 

      // reference:
      // XXX 20X: Has only SE CHMODE! Cannot be set.
      // XXX 7202, 1608FS: Has no CHMODE
      // XXX 7204: Has only AI:CHMODE
      // XXX 1208-FS, 1408FS do not have CHMODE FOR AIQUEUE! But AI:CHMODE
      switch ( traces[k].reference() ) {
      case InData::RefCommon:
	DAQFlexDevice->sendMessageUnlocked( aiq + "CHMODE=SE" );
	break;
      case InData::RefDifferential:
	DAQFlexDevice->sendMessageUnlocked( aiq + "CHMODE=DIFF" );
	break;
      case InData::RefGround:
	DAQFlexDevice->sendMessageUnlocked( aiq + "CHMODE=SE" );
	break;
      default:
	traces[k].addError( DaqError::InvalidReference );
      }
      if ( DAQFlexDevice->failed() ) {
	traces[k].setErrorStr( DAQFlexDevice->daqflexErrorStr() );
	return -1;
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
	  if ( BipolarRange.size() > 1 ) {
	    string message = aiq + "RANGE=" + BipolarRangeCmds[traces[k].gainIndex()];
	    string response = DAQFlexDevice->sendMessageUnlocked( message );
	    if ( DAQFlexDevice->failed() || response.empty() )
	      traces[k].addError( DaqError::InvalidGain );
	  }
	  if ( traces[k].success() ) {
	    // get calibration:
	    string response = DAQFlexDevice->sendMessageUnlocked( "?AI{" + Str( traces[k].channel() ) + "}:SLOPE" );
	    gainp->Slope = Str( response.erase( 0, 12 ) ).number();
	    response = DAQFlexDevice->sendMessageUnlocked( "?AI{" + Str( traces[k].channel() ) + "}:OFFSET" );
	    gainp->Offset = Str( response.erase( 0, 13 ) ).number();
	    gainp->Slope *= 2.0*max/DAQFlexDevice->maxAIData();
	    gainp->Offset *= 2.0*max/DAQFlexDevice->maxAIData();
	    gainp->Offset -= max;
	    if ( DAQFlexDevice->failed() ) {
	      traces[k].setErrorStr( DAQFlexDevice->daqflexErrorStr() );
	      return -1;
	    } 
	  }
	}
      }
    }
  }

  if ( traces.failed() )
    return -1;

  if ( traces.success() ) {
    traces.setReadTime( traces[0].interval( ReadBufferSize/2/traces.size() ) );
    traces.setUpdateTime( traces[0].interval( BufferSize/2/traces.size() ) );
    setSettings( traces, BufferSize, ReadBufferSize );
    Traces = &traces;
    IsPrepared = true;
    return 0;
  }
  else
    return -1;
}


int DAQFlexAnalogInput::startRead( QSemaphore *sp, QReadWriteLock *datamutex,
				   QWaitCondition *datawait, QSemaphore *aosp )
{
  cerr << "STARTREAD\n";
  QMutexLocker locker( mutex() );
  if ( !IsPrepared || Traces == 0 ) {
    cerr << "AI not prepared or no traces!\n";
    return -1;
  }

  bool tookao = ( TakeAO && aosp != 0 && DAQFlexAO != 0 && DAQFlexAO->prepared() );

  if ( tookao ) {
    if ( DAQFlexAO->useAIRate() )
      DAQFlexDevice->sendCommands( "AOSCAN:START", "AISCAN:START" );
    else
      DAQFlexDevice->sendCommands( "AISCAN:START", "AOSCAN:START" );
  }
  else
    DAQFlexDevice->sendCommand( "AISCAN:START" );
  if ( DAQFlexDevice->failed() ) {
    setErrorStr( DAQFlexDevice->daqflexErrorStr() );
    return -1;
  } 

  bool finished = true;
  TraceIndex = 0;
  IsRunning = true;
  startThread( sp, datamutex, datawait );
  if ( tookao ) {
    DAQFlexAO->startThread( aosp );
    finished = DAQFlexAO->noMoreData();
  }
  return finished ? 0 : 1;
}


int DAQFlexAnalogInput::readData( void )
{
  //  cerr << "DAQFlex::readData() start\n";
  QMutexLocker locker( mutex() );

  if ( Traces == 0 || Buffer == 0 || ! IsRunning )
    return -2;

  int readn = 0;
  int buffern = BufferN*2;
  int inps = DAQFlexDevice->inPacketSize();
  int maxn = ((BufferSize - buffern)/inps)*inps;
  if ( maxn > ReadBufferSize )
    maxn = ReadBufferSize;
  if ( maxn <= 0 )
    return 0;

  // read data:
  int timeout = (int)::ceil( 10.0 * 1000.0*(*Traces)[0].interval( maxn/2/Traces->size() ) ); // in ms
  DAQFlexCore::DAQFlexError ern = DAQFlexCore::Success;
  ern = DAQFlexDevice->readBulkTransfer( (unsigned char*)(Buffer + buffern),
					 maxn, &readn, timeout );

  // store data:
  if ( readn > 0 ) {
    buffern += readn;
    BufferN = buffern / 2;
    readn /= 2;
    CurrentSamples += readn;
  }

  if ( ern == DAQFlexCore::Success || ern == DAQFlexCore::ErrorLibUSBTimeout ) {
    string status = DAQFlexDevice->sendMessage( "?AISCAN:STATUS" );
    if ( status != "AISCAN:STATUS=RUNNING" ) {
      if ( status == "AISCAN:STATUS=OVERRUN" ) {
	Traces->addError( DaqError::OverflowUnderrun );
	return -2;
      }
      else {
	cerr << "DAQFlexAnalogInput::readData() -> analog input not running anymore\n";
	// This error occurs on to fast sampling, but at lower sampling it looks like it can be ignored....
	// Traces->addErrorStr( "analog input not running anymore" );
	// Traces->addError( DaqError::Unknown );
      }
      // return -2;
    }
    /*
    // no more data to be read:
    XXX This does not make any sense, since IsRunning is always true!!!
    if ( readn <= 0 && !IsRunning ) {
      if ( IsRunning && ( TotalSamples <=0 || CurrentSamples < TotalSamples ) ) {
	Traces->addErrorStr( deviceFile() + " - buffer-overflow " );
	Traces->addError( DaqError::OverflowUnderrun );
	return -2;
      }
      return -1;
    }
    */
  }
  else {
    // error:
    cerr << "READBULKTRANSFER error=" << DAQFlexDevice->daqflexErrorStr( ern )
	 << " readn=" << readn << '\n';

    switch( ern ) {

    case DAQFlexCore::ErrorLibUSBOverflow:
    case DAQFlexCore::ErrorLibUSBPipe:
      Traces->addError( DaqError::OverflowUnderrun );
      return -2;

    case DAQFlexCore::ErrorLibUSBBusy:
      Traces->addError( DaqError::Busy );
      return -2;

    case DAQFlexCore::ErrorLibUSBNoDevice:
      Traces->addError( DaqError::NoDevice );
      return -2;

    default:
      Traces->addErrorStr( DAQFlexDevice->daqflexErrorStr( ern ) );
      Traces->addError( DaqError::Unknown );
      return -2;
    }
  }

  return readn;
}


int DAQFlexAnalogInput::convertData( void )
{
  QMutexLocker locker( mutex() );

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

  if ( ! IsRunning )
    return 0;

  lock();
  {
    QMutexLocker corelocker( DAQFlexDevice->mutex() );
    DAQFlexDevice->sendControlTransfer( "AISCAN:STOP" );
    DAQFlexDevice->sendMessageUnlocked( "AISCAN:RESET" );
  }
  unlock();

  stopRead();

  lock();
  IsRunning = false;
  unlock();

  return 0;
}


int DAQFlexAnalogInput::reset( void )
{
  if ( !isOpen() )
    return NotOpen;

  QMutexLocker locker( mutex() );
  {
    QMutexLocker corelocker( DAQFlexDevice->mutex() );

    if ( IsRunning )
      DAQFlexDevice->sendControlTransfer( "AISCAN:STOP" );

    DAQFlexDevice->sendMessageUnlocked( "AISCAN:RESET" );

    // clear overrun condition:
    DAQFlexDevice->clearRead();
  }

  // flush:
  int numbytes = 0;
  int status = 0;
  do {
    const int nbuffer = DAQFlexDevice->inPacketSize()*4;
    unsigned char buffer[nbuffer];
    status = DAQFlexDevice->readBulkTransfer( buffer, nbuffer, &numbytes, 200 );
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

  IsPrepared = false;
  IsRunning = false;
  Traces = 0;
  TraceIndex = 0;
  
  return 0;
}


bool DAQFlexAnalogInput::running( void ) const
{
  lock();
  string response = DAQFlexDevice->sendMessage( "?AISCAN:STATUS" );
  unlock();
  return ( response.find( "RUNNING" ) != string::npos && AnalogInput::running() );
}


void DAQFlexAnalogInput::take( const vector< AnalogInput* > &ais,
			       const vector< AnalogOutput* > &aos,
			       vector< int > &aiinx, vector< int > &aoinx,
			       vector< bool > &airate, vector< bool > &aorate )
{
  DAQFlexAO = 0;

  if ( TakeAO ) {
    TakeAO = false;
    // check for analog output device:
    for ( unsigned int k=0; k<aos.size(); k++ ) {
      if ( aos[k]->analogOutputType() == DAQFlexAnalogIOType &&
	   aos[k]->deviceFile() == deviceFile() &&
	   DAQFlexDevice->aoFIFOSize() > 0 ) {
	DAQFlexAO = dynamic_cast< DAQFlexAnalogOutput* >( aos[k] );
	aoinx.push_back( k );
	aorate.push_back( DAQFlexAO->useAIRate() );
	TakeAO = true;
	break;
      }
    }
  }
}


}; /* namespace daqflex */
