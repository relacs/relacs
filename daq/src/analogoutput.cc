/*
  analogoutput.cc
  Interface for accessing analog output of a data-aquisition board.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <sstream>
#include <relacs/outdata.h>
#include <relacs/analogoutput.h>

using namespace std;

namespace relacs {


AnalogOutput::AnalogOutput( void )
  : Device( AnalogOutputType ),
    AnalogOutputSubType( 0 ),
    ExternalReference( -1.0 )
{
}


AnalogOutput::AnalogOutput( int aotype )
  : Device( AnalogOutputType ),
    AnalogOutputSubType( aotype ),
    ExternalReference( -1.0 )
{
}


AnalogOutput::AnalogOutput( const string &deviceclass, int aotype )
  : Device( deviceclass, AnalogOutputType ),
    AnalogOutputSubType( aotype ),
    ExternalReference( -1.0 )
{
}


AnalogOutput::~AnalogOutput( void )
{
}


int AnalogOutput::open( const string &device, const Options &opts )
{
  Info.clear();
  Settings.clear();
  setDeviceFile( device );
  return InvalidDevice;
}


int AnalogOutput::open( Device &device, const Options &opts )
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


void AnalogOutput::setInfo( void )
{
  Info.clear();
  Device::addInfo();
  if ( isOpen() ) {
    Info.addInteger( "channels", channels() );
    Info.addInteger( "bits", bits() );
    Info.addNumber( "max sampling rate", 0.001*maxRate(), " kHz" );
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
  if ( sigs[0].writeTime() > 0.0 && writebuffer >= 0 )
    Settings.addNumber( "write buffer time", 1000.0*sigs[0].writeTime(), "ms" );
  if ( writebuffer > 0 )
    Settings.addNumber( "write buffer size", writebuffer, "Byte" );
}


int AnalogOutput::analogOutputType( void ) const
{
  return AnalogOutputSubType;
}


void AnalogOutput::setAnalogOutputType( int aotype )
{
  AnalogOutputSubType = aotype;
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
    if ( sigs[k].delay() != sigs[0].delay() ) {
      sigs[k].addError( DaqError::MultipleDelays ); 
      sigs[k].setDelay( sigs[0].delay() );
    }
    if ( sigs[k].sampleRate() != sigs[0].sampleRate() ) {
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
    if ( sigs[k].writeTime() != sigs[0].writeTime() ) {
      sigs[k].addError( DaqError::MultipleBufferTimes ); 
      sigs[k].setWriteTime( sigs[0].writeTime() );
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


}; /* namespace relacs */

