/*
  analogoutput.cc
  Interface for accessing analog output of a data-aquisition board.

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
#include <relacs/outdata.h>
#include <relacs/analogoutput.h>

using namespace std;

namespace relacs {


AnalogOutput::AnalogOutput( void )
  : Device( AnalogOutputType ),
    AnalogOutputSubType( 0 ),
    ExternalReference( -1.0 ),
    Run( false ),
    Semaphore( 0 ),
    WriteSleepMS( 0 )
{
}


AnalogOutput::AnalogOutput( int aotype )
  : Device( AnalogOutputType ),
    AnalogOutputSubType( aotype ),
    ExternalReference( -1.0 ),
    Run( false ),
    Semaphore( 0 ),
    WriteSleepMS( 0 )
{
}


AnalogOutput::AnalogOutput( const string &deviceclass, int aotype )
  : Device( deviceclass, AnalogOutputType ),
    AnalogOutputSubType( aotype ),
    ExternalReference( -1.0 ),
    Run( false ),
    Semaphore( 0 ),
    WriteSleepMS( 0 )
{
}


AnalogOutput::~AnalogOutput( void )
{
}


int AnalogOutput::open( const string &device )
{
  Info.clear();
  Settings.clear();
  setDeviceFile( device );
  return InvalidDevice;
}


int AnalogOutput::open( Device &device )
{
  Info.clear();
  Settings.clear();
  setDeviceFile( device.deviceIdent() );
  return InvalidDevice;
}


int AnalogOutput::reset( void )
{
  Settings.clear();
  return 0;
}


long AnalogOutput::index( void ) const
{
  return -1;
}


double AnalogOutput::delay( int channel ) const
{
  if ( Delays.empty() )
    return 0.0;
  else if ( channel < 0 || channel > channels() || (int)Delays.size() < channels() )
    return Delays[0];
  else
    return Delays[channel];
}


void AnalogOutput::setDelays( const vector<double> &delays )
{
  Delays = delays;
  setInfo();
}


void AnalogOutput::setDelay( double delay )
{
  Delays.clear();
  Delays.push_back( delay );
  setInfo();
}


int AnalogOutput::getAISyncDevice( const vector< AnalogInput* > &ais ) const
{
  return -1;
}


bool AnalogOutput::useAIRate( void ) const
{
  return false;
}


void AnalogOutput::take( const vector< AnalogOutput* > &aos,
			 vector< int > &aoinx,
			 vector< bool > &aorate )
{
}


void AnalogOutput::addTraces( vector< TraceSpec > &traces, int deviceid ) const
{
}


int AnalogOutput::matchTraces( vector< TraceSpec > &traces ) const
{
  return 0;
}


void AnalogOutput::setErrorStr( const string &strg ) const
{
  Device::setErrorStr( strg );
}


void AnalogOutput::addErrorStr( const string &strg ) const
{
  Device::addErrorStr( strg );
}


void AnalogOutput::setErrorStr( int errnum ) const
{
  Device::setErrorStr( errnum );
}


void AnalogOutput::addErrorStr( int errnum ) const
{
  Device::addErrorStr( errnum );
}


void AnalogOutput::setErrorStr( const OutList &sigs ) const
{
  if ( sigs.failed() )
    Device::setErrorStr( sigs.errorText() );
  else
    clearError();
}


void AnalogOutput::addErrorStr( const OutList &sigs ) const
{
  if ( sigs.failed() )
    Device::addErrorStr( sigs.errorText() );
}


void AnalogOutput::setInfo( void )
{
  Info.clear();
  Device::addInfo();
  if ( isOpen() ) {
    Info.addInteger( "channels", channels() );
    Info.addInteger( "bits", bits() );
    Info.addNumber( "max sampling rate", 0.001*maxRate(), " kHz" );
    Info.addNumber( "delay", 1000.0*delay( 0 ), " ms" );
  }
}


void AnalogOutput::setSettings( const OutList &sigs, int writebuffer )
{
  Settings.clear();
  for ( int k=0; k<sigs.size(); k++ )
    Settings.addInteger( "channel", sigs[k].channel() );
  Settings.addBoolean( "continuous", sigs[0].continuous() );
  Settings.addInteger( "startsource", sigs[0].startSource() );
  Settings.addNumber( "delay", 1000.0*sigs[0].delay(), "ms" );
  Settings.addNumber( "sampling rate", 0.001*sigs[0].sampleRate(), "kHz" );
}


int AnalogOutput::analogOutputType( void ) const
{
  return AnalogOutputSubType;
}


void AnalogOutput::setAnalogOutputType( int aotype )
{
  AnalogOutputSubType = aotype;
}


void AnalogOutput::setWriteSleep( int ms )
{
  WriteSleepMS = ms;
}


int AnalogOutput::writeSleep( void ) const
{
  return WriteSleepMS;
}


double AnalogOutput::externalReference( void ) const
{
  return ExternalReference;
}


void AnalogOutput::setExternalReference( double extr )
{
  ExternalReference = extr;
}


int AnalogOutput::testWrite( OutList &sigs )
{
  sigs.clearError();
  testWriteData( sigs );
  int r = testWriteDevice( sigs );
  return r;
}


int AnalogOutput::testWriteData( OutList &sigs )
{
  // no data:
  if ( sigs.size() == 0 )
    return -1;

  // device open:
  if ( ! isOpen() )
    sigs.addError( DaqError::DeviceNotOpen );

  // no data:
  for ( int k=0; k<sigs.size(); k++ ) {
    if ( sigs[k].size() <= 0 )
      sigs[k].addError( DaqError::NoData );
  }

  // multiple devices, startsource, delay, sampling rate, continuous,
  // update times, buffer sizes:
  for ( int k=1; k<sigs.size(); k++ ) {
    if ( sigs[k].device() != sigs[0].device() ) {
      sigs[k].addError( DaqError::MultipleDevices );
      sigs[k].setDevice( sigs[0].device() );
    }
    if ( sigs[k].startSource() != sigs[0].startSource() ) {
      sigs[k].addError( DaqError::MultipleStartSources ); 
      sigs[k].setStartSource( sigs[0].startSource() );
    }
    if ( fabs( sigs[k].delay() - sigs[0].delay() ) > 1e-7 ) {
      sigs[k].addError( DaqError::MultipleDelays ); 
      sigs[k].setDelay( sigs[0].delay() );
    }
    if ( fabs( sigs[k].sampleRate() - sigs[0].sampleRate() ) > 0.1 ) {
      sigs[k].addError( DaqError::MultipleSampleRates ); 
      sigs[k].setSampleRate( sigs[0].sampleRate() );
    }
    if ( sigs[k].continuous() != sigs[0].continuous() ) {
      sigs[k].addError( DaqError::MultipleContinuous ); 
      sigs[k].setContinuous( sigs[0].continuous() );
    }
    if ( sigs[k].restart() != sigs[0].restart() ) {
      sigs[k].addError( DaqError::MultipleRestart ); 
      sigs[k].setRestart( sigs[0].restart() );
    }
    if ( sigs[k].size() != sigs[0].size() ) {
      sigs[k].addError( DaqError::MultipleBuffersizes );
    }
  }

  // start source:
  if( sigs[0].startSource() < 0 ) {
    sigs.addError( DaqError::InvalidStartSource );
    sigs.setStartSource( 0 );
  }

  // delay:
  if( sigs[0].delay() < 0.0 ) {
    sigs.addError( DaqError::InvalidDelay );
    sigs.setDelay( 0.0 );
  }

  // sample rate:
  double maxrate = maxRate();
  if( sigs[0].sampleRate() < 1.0 ) {
    for ( int k=0; k<sigs.size(); k++ ) {
      sigs[k].addError( DaqError::InvalidSampleRate );
      sigs[k].setSampleRate( 1.0 );
    }
  }
  else if( sigs[0].sampleRate() > maxrate ) {
    for ( int k=0; k<sigs.size(); k++ ) {
      sigs[k].addError( DaqError::InvalidSampleRate );
      sigs[k].setSampleRate( maxrate );
    }
  }

  // channel configuration:
  for ( int k=0; k<sigs.size(); k++ ) {
    // check channel number:
    if( sigs[k].channel() < 0 ) {
      sigs[k].addError( DaqError::InvalidChannel );
      sigs[k].setChannel( 0 );
    }
    else if( sigs[k].channel() >= channels() ) {
      sigs[k].addError( DaqError::InvalidChannel );
    }
  }
  
  // multiple channels:
  for ( int k=0; k<sigs.size(); k++ ) {
    for ( int i=k+1; i<sigs.size(); i++ ) {
      if ( sigs[k].channel() == sigs[i].channel() ) {
	sigs[k].addError( DaqError::MultipleChannels );
	sigs[i].addError( DaqError::MultipleChannels );
      }
    }
  }

  return sigs.failed() ? -1 : 0;
}


AnalogOutput::Status AnalogOutput::status( void ) const
{
  lock();
  Status s = statusUnlocked();
  unlock();
  return s;
}


bool AnalogOutput::running( void ) const
{
  return QThread::isRunning();
}


void AnalogOutput::startThread( QSemaphore *sp, bool error )
{
  if ( sp != 0 ) {
    if ( error )
      sp->release( 1000 );
    else {
      Semaphore = sp;
      Run = true;
      start( QThread::HighPriority );
    }
  }
}


void AnalogOutput::run( void )
{
  lock();
  // fill in data:
  do {
    int r = writeData();
    // error:
    if ( r < -1 ) {
      bool rd = Run;
      unlock();
      int rr = reset();
      if ( rr != 0 )
	addErrorStr( getErrorStr( rr ) );
      if ( Semaphore != 0 )
	Semaphore->release( rd ? 1000 : 1 );
      Semaphore = 0;
      lock();
      Run = false;
      unlock();
      return;
    }
    // finished:
    if ( r < 0 || ! Run )
      break;
    // the sleep is needed to allow for other processes to wake up and to acquire the lock!
    if ( r > WriteSleepMS )
      r = WriteSleepMS;
    SleepWait.wait( mutex(), r );
  } while ( Run );

  // wait for device to finish writing:
  do {
    Status r = statusUnlocked();
    if ( r == Underrun ) {
      unlock();
      int rr = reset();
      if ( rr != 0 )
	addErrorStr( getErrorStr( rr ) );
      if ( Semaphore != 0 )
	Semaphore->release( 1000 );
      Semaphore = 0;
      lock();
      Run = false;
      unlock();
      return;
    }
    if ( r != Running || ! Run )
      break;
    SleepWait.wait( mutex(), 1 );
  } while ( Run );

  Run = false;
  unlock();
  if ( Semaphore != 0 ) {
    Semaphore->release( 1 );
    lock();
    Semaphore = 0;
    unlock();
  }
}


void AnalogOutput::stopWrite( void )
{
  lock();
  Run = false;
  unlock();
  SleepWait.wakeAll();
  wait();

  lock();
  Semaphore = 0;
  unlock();
}


}; /* namespace relacs */

