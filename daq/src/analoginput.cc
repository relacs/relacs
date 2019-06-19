/*
  analoginput.cc
  Interface for accessing analog input of a data-aquisition board.

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

#include <cmath>
#include <sstream>
#include <relacs/indata.h>
#include <relacs/analoginput.h>

using namespace std;

namespace relacs {


AnalogInput::AnalogInput( void )
  : Device( AnalogInputType ),
    AnalogInputSubType( 0 ),
    Run( false ),
    Semaphore( 0 ),
    DataMutex( 0 ),
    DataWait( 0 ),
    ReadSleepMS( 0 )
{
}


AnalogInput::AnalogInput( int aitype )
  : Device( AnalogInputType ),
    AnalogInputSubType( aitype ),
    Run( false ),
    Semaphore( 0 ),
    DataMutex( 0 ),
    DataWait( 0 ),
    ReadSleepMS( 0 )
{
}


AnalogInput::AnalogInput( const string &deviceclass, int aitype )
  : Device( deviceclass, AnalogInputType ),
    AnalogInputSubType( aitype ),
    Run( false ),
    Semaphore( 0 ),
    DataMutex( 0 ),
    DataWait( 0 ),
    ReadSleepMS( 0 )
{
}


AnalogInput::~AnalogInput( void )
{
}      


int AnalogInput::open( const string &device)
{
  Info.clear();
  Settings.clear();
  setDeviceFile( device );
  return InvalidDevice;
}


int AnalogInput::open( Device &device)
{
  Info.clear();
  Settings.clear();
  setDeviceFile( device.deviceIdent() );
  return InvalidDevice;
}


void AnalogInput::model( InList &data,
			 const vector< int > &aochannels, vector< float > &aovalues )
{
  for ( unsigned int k=0; k<aovalues.size(); k++ )
    aovalues[k] = 0.0;
}


void AnalogInput::take( const vector< AnalogInput* > &ais,
			const vector< AnalogOutput* > &aos,
			vector< int > &aiinx, vector< int > &aoinx,
			vector< bool > &airate, vector< bool > &aorate )
{
}


void AnalogInput::setInfo( void )
{
  Info.clear();
  Device::addInfo();
  if ( isOpen() ) {
    Info.addInteger( "channels", channels() );
    Info.addInteger( "bits", bits() );
    Info.addNumber( "max sampling rate", 0.001*maxRate(), " kHz" );
  }
}


void AnalogInput::setSettings( const InList &traces, 
			       int fifobuffer, int pluginbuffer )
{
  Settings.clear();
  for ( int k=0; k<traces.size(); k++ ) {
    Settings.addInteger( "channel", traces[k].channel() );
    Settings.addInteger( "gain", traces[k].gainIndex() );
    Settings.addText( "polarity", traces[k].unipolar() ? "unipolar" : "bipolar" );
    Settings.addText( "reference", traces[k].referenceStr() );
  }
  Settings.addBoolean( "continuous", traces[0].continuous() );
  Settings.addInteger( "startsource", traces[0].startSource() );
  Settings.addNumber( "delay", 1000.0*traces[0].delay(), "ms" );
  Settings.addNumber( "sampling rate", 0.001*traces[0].sampleRate(), "kHz" );
  if ( fifobuffer > 0 )
    Settings.addNumber( "FIFO buffer size", fifobuffer, "Byte" );
  if ( pluginbuffer > 0 )
    Settings.addNumber( "plugin buffer size", pluginbuffer, "Byte" );
}


void AnalogInput::addTraces( vector< TraceSpec > &traces, int deviceid ) const
{
}


int AnalogInput::matchTraces( InList &traces ) const
{
  return 0;
}


int AnalogInput::analogInputType( void ) const
{
  return AnalogInputSubType;
}


void AnalogInput::setAnalogInputType( int aitype )
{
  AnalogInputSubType = aitype;
}


void AnalogInput::setReadSleep( unsigned long ms )
{
  ReadSleepMS = ms;
}


int AnalogInput::minGainIndex( bool unipolar ) const
{
  if ( ! isOpen() )
    return -1;

  int index = -1;
  if ( unipolar ) {
    for ( int k = 0; k<maxRanges(); k++ ) {
      if ( unipolarRange( k ) > 0 ) {
	index = k;
	break;
      }
    }
  }
  else {
    for ( int k = 0; k<maxRanges(); k++ ) {
      if ( bipolarRange( k ) > 0 ) {
	index = k;
	break;
      }
    }
  }
  return index;
}


int AnalogInput::maxGainIndex( bool unipolar ) const
{
  if ( ! isOpen() )
    return -1;

  int index = -1;
  if ( unipolar ) {
    for ( int k = maxRanges()-1; k >= 0; k-- ) {
      if ( unipolarRange( k ) > 0 ) {
	index = k;
	break;
      }
    }
  }
  else {
    for ( int k = maxRanges()-1; k >= 0; k-- ) {
      if ( bipolarRange( k ) > 0 ) {
	index = k;
	break;
      }
    }
  }
  return index;
}


int AnalogInput::gainIndex( bool unipolar, double maxvoltage ) const
{
  if ( ! isOpen() )
    return -1;

  // find appropriate gain:
  int newindex = -1;
  for ( int k = maxRanges()-1; k >= 0; k-- ) {
    if ( unipolar ) {
      if ( unipolarRange( k ) > 0 &&
	   unipolarRange( k ) >= maxvoltage ) {
	newindex = k;
	break;
      }
    }
    else {
      if ( bipolarRange( k ) > 0 &&
	   bipolarRange( k ) >= maxvoltage ) {
	newindex = k;
	break;
      }
    }
  }

  return newindex;
}


int AnalogInput::testRead( InList &traces )
{
  traces.clearError();
  testReadData( traces );
  int  r = testReadDevice( traces );
  return r;
}


int AnalogInput::testReadData( InList &traces )
{
  // no data:
  if ( traces.size() == 0 )
    return -1;

  // device open:
  if ( ! isOpen() )
    traces.addError( DaqError::DeviceNotOpen );

  // multiple devices, startsource, delay, sampling rate, continuous, 
  // buffer size:
  for ( int k=1; k<traces.size(); k++ ) {
    if ( traces[k].device() != traces[0].device() ) {
      traces[k].addError( DaqError::MultipleDevices );
      traces[k].setDevice( traces[0].device() );
    }
    if ( traces[k].startSource() != traces[0].startSource() ) {
      traces[k].addError( DaqError::MultipleStartSources ); 
      traces[k].setStartSource( traces[0].startSource() );
    }
    if ( traces[k].delay() != traces[0].delay() ) {
      traces[k].addError( DaqError::MultipleDelays ); 
      traces[k].setDelay( traces[0].delay() );
    }
    if ( fabs( traces[k].sampleRate() - traces[0].sampleRate() ) > 1.0e-8 ) {
      traces[k].addError( DaqError::MultipleSampleRates ); 
      traces[k].setSampleRate( traces[0].sampleRate() );
    }
    if ( traces[k].continuous() != traces[0].continuous() ) {
      traces[k].addError( DaqError::MultipleContinuous ); 
      traces[k].setContinuous( traces[0].continuous() );
    }
    if ( traces[k].capacity() != traces[0].capacity() ) {
      traces[k].addError( DaqError::MultipleBuffersizes );
    }
  }

  // start source:
  if( traces[0].startSource() < 0 ) {
    traces.addError( DaqError::InvalidStartSource );
    traces.setStartSource( 0 );
  }

  // delay:
  if( traces[0].delay() < 0.0 ) {
    traces.addError( DaqError::InvalidDelay );
    traces.setDelay( 0.0 );
  }

  // sample rate:
  double maxrate = maxRate();
  if( traces[0].sampleRate() < 1.0 ) {
    traces.addError( DaqError::InvalidSampleRate );
    traces.setSampleRate( 1.0 );
  }
  else if( traces[0].sampleRate() > maxrate ) {
    traces.addError( DaqError::InvalidSampleRate );
    traces.setSampleRate( maxrate );
  }

  // buffers size:
  for ( int k=0; k<traces.size(); k++ ) {
    if ( traces[k].capacity() <= 0 )
      traces[k].addError( DaqError::NoData );
  }

  // channel configuration:
  for ( int k=0; k<traces.size(); k++ ) {
    // check channel number:
    if( traces[k].channel() < 0 ) {
      traces[k].addError( DaqError::InvalidChannel );
      traces[k].setChannel( 0 );
    }
    else if( traces[k].channel() >= channels() &&
	     traces[k].channel() < InData::ParamChannel ) {
      traces[k].addError( DaqError::InvalidChannel );
    }
    // check reference:
    if ( traces[k].reference() != InData::RefDifferential &&
	 traces[k].reference() != InData::RefGround &&
	 traces[k].reference() != InData::RefCommon &&
	 traces[k].reference() != InData::RefOther ) {
      traces[k].addError( DaqError::InvalidReference );
      traces[k].setReference( InData::RefGround );
    } 
    // check gain index:
    if ( traces[k].channel() < InData::ParamChannel ) {
      if ( traces[k].gainIndex() < 0 ) {
	traces[k].addError( DaqError::InvalidGain );
	traces[k].setGainIndex( 0 );
      } 
      else if ( traces[k].gainIndex() >= maxRanges() ) {
	traces[k].addError( DaqError::InvalidGain );
	traces[k].setGainIndex( maxRanges()-1 );
      }
      // check and fix validity of gain index:
      if ( ( traces[k].unipolar() && 
	     unipolarRange( traces[k].gainIndex() ) < 0.0 ) ||
	   ( ! traces[k].unipolar() && 
	     bipolarRange( traces[k].gainIndex() ) < 0.0 ) ) {
	traces[k].addError( DaqError::InvalidGain );
	while ( ( ( traces[k].unipolar() && 
		    unipolarRange( traces[k].gainIndex() ) < 0.0 ) ||
		  ( ! traces[k].unipolar() && 
		    bipolarRange( traces[k].gainIndex() ) < 0.0 ) ) &&
		traces[k].gainIndex()+1 < maxRanges() ) {
	  traces[k].setGainIndex( traces[k].gainIndex()+1 );
	}
	while ( ( ( traces[k].unipolar() && 
		    unipolarRange( traces[k].gainIndex() ) < 0.0 ) ||
		  ( ! traces[k].unipolar() && 
		    bipolarRange( traces[k].gainIndex() ) < 0.0 ) ) &&
		traces[k].gainIndex()-1 >= 0 ) {
	  traces[k].setGainIndex( traces[k].gainIndex()-1 );
	}
      }
    }
  }
  
  // multiple channels:
  for ( int k=0; k<traces.size(); k++ ) {
    for ( int i=k+1; i<traces.size(); i++ ) {
      if ( traces[k].channel() == traces[i].channel() ) {
	traces[k].addError( DaqError::MultipleChannels );
	traces[i].addError( DaqError::MultipleChannels );
      }
    }
  }

  return traces.failed() ? -1 : 0;
}


bool AnalogInput::running( void ) const
{
  return QThread::isRunning();
}


void AnalogInput::startThread( QSemaphore *sp, QReadWriteLock *datamutex,
			       QWaitCondition *datawait, bool error )
{
  if ( sp != 0 ) {
    if ( error )
      sp->release( 1000 );
    else {
      Semaphore = sp;
      DataMutex = datamutex;
      DataWait = datawait;
      Run = true;
      start( QThread::HighPriority );
    }
  }
}


void AnalogInput::run( void )
{
  bool rd = true;
  lock();
  do {
    rd = Run;
    // get (remaining) data from the card:
    int r = readData();
    // error:
    if ( r < -1 ) {
      unlock();
      int rr = reset();
      if ( rr != 0 )
	addErrorStr( getErrorStr( rr ) );
      if ( DataWait != 0 )
	DataWait->wakeAll();
      if ( Semaphore != 0 )
	Semaphore->release( rd ? 1000 : 1 );
      Semaphore = 0;
      lock();
      Run = false;
      unlock();
      return;
    }
    // finished:
    if ( r < 0 ) {
      Run = false;
      if ( DataWait != 0 )
	DataWait->wakeAll();
      break;
    }
    // transfer data to the buffer:
    if ( r > 0 ) {
      unlock();   // DataMutex first, then device mutex -> otherwise dead lock!!!
      if ( DataMutex != 0 )
	DataMutex->lockForWrite();
      lock();
      convertData();
      unlock();
      if ( DataMutex != 0 )
	DataMutex->unlock();
      if ( DataWait != 0 )
	DataWait->wakeAll();
      lock();
    }
    if ( Run ) {
      // the sleep is needed to allow for other processes to wake up and acquire the lock!
      SleepWait.wait( mutex(), ReadSleepMS );
    }
  } while ( rd );
  unlock();

  if ( Semaphore != 0 ) {
    Semaphore->release( 1 );
    lock();
    Semaphore = 0;
    unlock();
  }
}


void AnalogInput::stopRead( void )
{
  // stop thread:
  lock();
  Run = false;
  unlock();
  SleepWait.wakeAll();
  wait();

  lock();
  Semaphore = 0;
  DataMutex = 0;
  DataWait = 0;
  unlock();
}


}; /* namespace relacs */

