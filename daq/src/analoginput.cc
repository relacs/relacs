/*
  analoginput.cc
  Interface for accessing analog input of a data-aquisition board.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
  : Device( Type ),
    AnalogInputType( 0 )
{
}


AnalogInput::AnalogInput( int aitype )
  : Device( Type ),
    AnalogInputType( aitype )
{
}


AnalogInput::AnalogInput( const string &deviceclass, int aitype )
  : Device( deviceclass, Type ),
    AnalogInputType( aitype )
{
}


AnalogInput::~AnalogInput( void )
{
}      


int AnalogInput::open( const string &device, long mode )
{
  clearSettings();
  setDeviceFile( device );
  return InvalidDevice;
}


int AnalogInput::open( Device &device, long mode )
{
  clearSettings();
  setDeviceFile( device.deviceIdent() );
  return InvalidDevice;
}


void AnalogInput::take( const vector< AnalogInput* > &ais,
			const vector< AnalogOutput* > &aos,
			vector< int > &aiinx, vector< int > &aoinx )
{
}


string AnalogInput::info( void ) const
{
  ostringstream ss;
  ss << ";channels: " << channels();
  ss << ";bits: " << bits();
  ss << ";max sampling rate: " << 0.001*maxRate() << " kHz";
  ss << ends;
  return Device::info() + ss.str();
}


void AnalogInput::setSettings( const InList &traces, int elemsize )
{
  ostringstream ss;

  for ( int k=0; k<traces.size(); k++ ) {
    ss << "channel: " << traces[k].channel()
       << ";gain: " << traces[k].gainIndex()
       << ";polarity: " << ( traces[k].unipolar() ? "unipolar" : "bipolar" )
       << ";reference: " << traces[k].referenceStr() << ';';
  }
  ss << "continuous: " << ( traces[0].continuous() ? "yes" : "no" );
  ss << ";startsource: " << traces[0].startSource();
  ss << ";delay: " << 1000.0*traces[0].delay() << "ms";
  ss << ";sampling rate: " << 0.001*traces[0].sampleRate() << "kHz";
  ss << ";read buffer time: " << traces[0].readTime() << "s";
  ss << ";read buffer size: " << traces.size()*traces[0].indices( traces[0].readTime() )*elemsize/1000 << "kB";
  ss << ";update buffer time: " << traces[0].updateTime() << "s";
  ss << ";update buffer size: " << traces.size()*traces[0].indices( traces[0].updateTime() )*elemsize/1000 << "kB";

  Device::setSettings( ss.str() );
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
  return AnalogInputType;
}


void AnalogInput::setAnalogInputType( int aitype )
{
  AnalogInputType = aitype;
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
  // buffer size, update times:
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
    if ( traces[k].updateTime() != traces[0].readTime() ) {
      traces[k].addError( DaqError::MultipleBufferTimes ); 
      traces[k].setReadTime( traces[0].readTime() );
    }
    if ( traces[k].updateTime() != traces[0].updateTime() ) {
      traces[k].addError( DaqError::MultipleUpdateTimes ); 
      traces[k].setUpdateTime( traces[0].updateTime() );
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
    else if( traces[k].channel() >= channels() ) {
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


}; /* namespace relacs */

