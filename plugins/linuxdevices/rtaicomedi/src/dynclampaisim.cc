/*
  dynclampaisim.cc
  Implementation of AnalogInput simulating an analog input device supporting analog ouput.

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

#include <relacs/relacsplugin.h>
#include <relacs/rtaicomedi/dynclampmodelsim.h>
#include <relacs/rtaicomedi/dynclampaisim.h>

namespace relacs {


DynClampAISim::DynClampAISim( void )
  : AISim()
{
  statusInput.clear();
  statusInputNames.clear();
  statusInputUnits.clear();
#ifdef ENABLE_INTERVALS
  intervalstatusinx = statusInput.size();
  statusInputNames.push_back( "Interval" );
  statusInputUnits.push_back( "s" );
  statusInput.push_back( 0.0 );
#endif
#ifdef ENABLE_AITIME
  aitimestatusinx = statusInput.size();
  statusInputNames.push_back( "AI-time" );
  statusInputUnits.push_back( "s" );
  statusInput.push_back( 1.2e-6 );
#endif
#ifdef ENABLE_AIACQUISITIONTIME
  aiacquisitiontimestatusinx = statusInput.size();
  statusInputNames.push_back( "AI-acquisition-time" );
  statusInputUnits.push_back( "s" );
  statusInput.push_back( 1.0e-6 );
#endif
#ifdef ENABLE_AICONVERSIONTIME
  aiconversiontimestatusinx = statusInput.size();
  statusInputNames.push_back( "AI-conversion-time" );
  statusInputUnits.push_back( "s" );
  statusInput.push_back( 0.1e-6 );
#endif
#ifdef ENABLE_AOTIME
  aotimestatusinx = statusInput.size();
  statusInputNames.push_back( "AO-time" );
  statusInputUnits.push_back( "s" );
  statusInput.push_back( 0.6e-6 );
#endif
#ifdef ENABLE_MODELTIME
  modeltimestatusinx = statusInput.size();
  statusInputNames.push_back( "Model-time" );
  statusInputUnits.push_back("s" );
  statusInput.push_back( 5e-6 );
#endif
#ifdef ENABLE_COMPUTATION
  outputstatusinx = statusInput.size();
#endif
}


DynClampAISim::~DynClampAISim( void )
{
}

int DynClampAISim::open( const string &device )
{
  AISim::open( device );
  string es = dynclampmodelsim::initStatus( statusInput, statusInputNames, statusInputUnits );
  setDeviceName( "Dynamic Clamp AI Simulation" );
  // publish information about the analog input device:
  setInfo();
  vector< TraceSpec > traces;
  traces.clear();
  addTraces( traces, 0 );
  for ( unsigned int k=0; k<traces.size(); k++ )
    Info.addText( "Model output", traces[k].traceName() );
  if ( ! es.empty() ) {
    setErrorStr( es );
    return -1;
  }
  return 0;
}


int DynClampAISim::open( Device &device )
{
  AISim::open( device );
  string es = dynclampmodelsim::initStatus( statusInput, statusInputNames, statusInputUnits );
  setDeviceName( "Dynamic Clamp AI Simulation" );
  setInfo();
  if ( ! es.empty() ) {
    setErrorStr( es );
    return -1;
  }
  return 0;
}


int DynClampAISim::testReadDevice( InList &traces )
{
  QMutexLocker locker( mutex() );

  // start source:
  if ( traces[0].startSource() < 0 || traces[0].startSource() >= 5 ) {
    traces.setStartSource( 0 );
    traces.addError( DaqError::InvalidStartSource );
  }

  for( int k = 0; k < traces.size(); k++ ) {

    // check delays:
    if ( traces[k].delay() > 0.0 ) {
      traces[k].addError( DaqError::InvalidDelay );
      traces[k].addErrorStr( "delays are not supported for analog input!" );
      traces[k].setDelay( 0.0 );
    }

  }

  if ( traces.failed() )
    return -1;

  return 0;
}


int DynClampAISim::prepareRead( InList &traces )
{
  dynclampmodelsim::initModel( traces[0].stepsize() );
#ifdef ENABLE_INTERVALS
  statusInput[intervalstatusinx] = traces[0].sampleInterval();
#endif
#ifdef ENABLE_AITIME
  statusInput[aitimestatusinx] = traces.size()*1.2e-6;
#endif
  return AISim::prepareRead( traces );
}


void DynClampAISim::model( InList &data,
			   const vector< int > &aochannels, vector< float > &aovalues )
{
#ifdef ENABLE_COMPUTATION
  dynclampmodelsim::computeModel( data, aochannels, aovalues,
				  outputstatusinx, statusInput );
#else
  for ( unsigned int k=0; k<aochannels.size(); k++ )
    aovalues[k] = 0.0;
#endif
  for ( int k=0; k<data.size(); k++ ) {
    if ( data[k].channel() >= 2*PARAM_CHAN_OFFSET ) {
      data[k].push( statusInput[ data[k].channel()-2*PARAM_CHAN_OFFSET ]*data[k].scale() );
    }
  }
}


void DynClampAISim::addTraces( vector< TraceSpec > &traces, int deviceid ) const
{
  dynclampmodelsim::addAITraces( traces, deviceid );
  int channel = 2*PARAM_CHAN_OFFSET;
  for ( unsigned int k=0; k<statusInput.size(); k++ ) {
    traces.push_back( TraceSpec( traces.size(), statusInputNames[k],
				 deviceid, channel++, 1.0, statusInputUnits[k] ) );
  }
}


int DynClampAISim::matchTraces( InList &traces ) const
{
  return dynclampmodelsim::matchAITraces( traces, statusInputNames, statusInputUnits );
}


addAnalogInput( DynClampAISim, rtaicomedi );

}; /* namespace relacs */

