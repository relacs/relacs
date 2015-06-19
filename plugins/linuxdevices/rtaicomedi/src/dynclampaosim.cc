/*
  dynclampaosim.cc
  Implementation of AnalogOutput simulating an analog output device supporting analog ouput.

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
#include <relacs/rtaicomedi/dynclampaosim.h>

namespace relacs {


DynClampAOSim::DynClampAOSim( void ) 
  : AOSim()
{
}


DynClampAOSim::~DynClampAOSim( void )
{
}


int DynClampAOSim::open( const string &device )
{
  AOSim::open( device );
  dynclampmodelsim::generateLookupTables();
  setDeviceName( "Dynamic Clamp AO Simulation" );
  // publish information about the analog input device:
  setInfo();
  vector< TraceSpec > traces;
  traces.clear();
  addTraces( traces, 0 );
  for ( unsigned int k=0; k<traces.size(); k++ ) {
    if ( traces[k].channel() >= PARAM_CHAN_OFFSET )
      Info.addText( "Model parameter", traces[k].traceName() );
  }
  return 0;
}


int DynClampAOSim::open( Device &device )
{
  AOSim::open( device );
  dynclampmodelsim::generateLookupTables();
  setDeviceName( "Dynamic Clamp AO Simulation" );
  setInfo();
  return 0;
}


int DynClampAOSim::testWriteDevice( OutList &sigs )
{
  QMutexLocker locker( mutex() );

  // start source:
  if ( sigs[0].startSource() < 0 || sigs[0].startSource() >= 5 ) {
    sigs.setStartSource( 0 );
    sigs.addError( DaqError::InvalidStartSource );
  }

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // channel configuration:
  for ( int k=0; k<ol.size(); k++ ) {
    ol[k].delError( DaqError::InvalidChannel );
    // check channel number:
    if ( ol[k].channel() < 0 ) {
      ol[k].addError( DaqError::InvalidChannel );
      ol[k].setChannel( 0 );
    }
    else if ( ol[k].channel() >= channels() && ol[k].channel() < PARAM_CHAN_OFFSET ) {
      ol[k].addError( DaqError::InvalidChannel );
      ol[k].setChannel( channels()-1 );
    }
  }

  for ( int k=0; k<ol.size(); k++ ) {

    // parameter signals don't have references and gains:
    if ( ol[k].channel() >= PARAM_CHAN_OFFSET )
      continue;

    // check channel:
    if ( ol[k].channel() < 0 || ol[k].channel() >= channels() ) {
      ol[k].addError( DaqError::InvalidChannel );
      return -1;
    }

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
    sigs[k].setMinVoltage( -10.0 );
    sigs[k].setMaxVoltage( 10.0 );
    if ( ! sigs[k].noLevel() )
      sigs[k].multiplyScale( 10.0 );
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

  }

  if ( ol.failed() )
    return -1;

  return 0;
}


bool DynClampAOSim::useAIRate( void ) const
{
  return true;
}


void DynClampAOSim::addTraces( vector< TraceSpec > &traces, int deviceid ) const
{
  dynclampmodelsim::addAOTraces( traces, deviceid );
}


int DynClampAOSim::matchTraces( vector< TraceSpec > &traces ) const
{
  return dynclampmodelsim::matchAOTraces( traces );
}


addAnalogOutput( DynClampAOSim, rtaicomedi );

}; /* namespace relacs */

