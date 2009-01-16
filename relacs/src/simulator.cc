/*
  simulator.cc
  Simulation of Acquire

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

#include <relacs/model.h>
#include <relacs/simulator.h>

namespace relacs {


Simulator::Simulator( void )
  : Acquire(),
    Sim( 0 )
{
}


Simulator::Simulator( Model &sim )
  : Acquire(),
    Sim( &sim )
{
}


Simulator::~Simulator( void )
{
}


Model *Simulator::model( void )
{
  return Sim;
}


void Simulator::setModel( Model *sim )
{
  Sim = sim;
}


void Simulator::clearModel( void )
{
  Sim = 0;
}


int Simulator::read( InList &data )
{
  //  cerr << "Simulator::read( InList& )\n";

  // no simulation model?
  if ( Sim == 0 ) {
    for ( int k=0; k<data.size(); k++ )
      data[k].setError( DaqError::NoDevice );
    return -1;
  }

  bool success = true;

  for ( signed k=0; k<data.size(); k++ )
    data[k].clearError();

  // clear device datas:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    AI[i].Traces.clear();
    AI[i].Gains.clear();
  }

  // sort data to devices:
  for ( int k=0; k<data.size(); k++ ) {
    // no device?
    if ( data[k].device() < 0 ) {
      continue;
      /*
      data[k].addError( DaqError::NoDevice );
      data[k].setDevice( 0 );
      success = false;
      */
    }
    else if ( data[k].device() >= (int)AI.size() ) {
      data[k].addError( DaqError::NoDevice );
      data[k].setDevice( AO.size()-1 );
      success = false;
    }
    else {
      // add data to device:
      AI[data[k].device()].Traces.add( &data[k] );
      AI[data[k].device()].Gains.push_back( -1 );
    }
  }

  // priority, busy:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    // multiple priorities?
    for ( int k=1; k<AI[i].Traces.size(); k++ ) {
      if ( AI[i].Traces[k].priority() != 
	   AI[i].Traces[0].priority() ) {
	AI[i].Traces[0].addError( DaqError::MultiplePriorities );
	AI[i].Traces[k].addError( DaqError::MultiplePriorities );
	AI[i].Traces[k].setPriority( AI[i].Traces[0].priority() );
	success = false;
      }
    }
    // device still busy?
    if ( Sim->running() &&
	 AI[i].Traces.size() > 0 ) {
      if ( AI[i].Traces[0].priority() ) {
	Sim->stop();
	AI[i].AI->reset();
      }
      else {
	for ( int k=0; k<AI[i].Traces.size(); k++ )
	  AI[i].Traces[k].addError( DaqError::Busy );
	success = false;
      }
    }
  }

  // error?
  if ( ! success )
    return -1;

  for ( unsigned int i=0; i<AI.size(); i++ ) {
    AI[i].Traces.setReadTime( BufferTime );
    AI[i].Traces.setUpdateTime( UpdateTime );
  }

  // test reading from daq boards:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 &&
	 AI[i].AI->testRead( AI[i].Traces ) != 0 )
      success = false;
  }

  // error?
  if ( ! success )
    return -1;

  // prepare reading from daq boards:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 &&
	 AI[i].AI->prepareRead( AI[i].Traces ) != 0 )
      success = false;
  }

  // error?
  if ( ! success )
    return -1;

  // start reading from daq boards:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 && 
	 AI[i].AI->startRead() != 0 )
      success = false;
  }

  // error?
  if ( ! success )
    return -1;

  Sim->clear();

  // set ranges:
  for ( int k=0; k<data.size(); k++ ) {
    if ( data[k].device() >= 0 ) {
      AIData &ai = AI[ data[k].device() ];
      if ( data[k].unipolar() ) {
	double max = ai.AI->unipolarRange( data[k].gainIndex() );
	data[k].setMaxVoltage( max );
	data[k].setMinVoltage( 0.0 );
      }
      else {
	double max = ai.AI->bipolarRange( data[k].gainIndex() );
	data[k].setMaxVoltage( max );
	data[k].setMinVoltage( -max );
      }
      long bs = data[k].indices( data[k].updateTime() );
      if ( bs <= 0 || bs > data[k].capacity() )
	bs = data[k].capacity();
      else
	bs *= 6;
      Sim->add( data[k].device(), data[k].channel(), 
		data[k].sampleInterval(), data[k].scale(), bs );
    }
  }

  // start simulation:
  Sim->start();
  LastWrite = -1.0;
  Data.clear();
  Data.reserve( data.size() );
  for ( int k=0; k<data.size(); k++ ) {
    Data.add( &data[k] );
  }  
  SyncMode = AISync;

  SoftReset = false;
  HardReset = false;

  return 0;
}


int Simulator::readData( void )
{
  return 0;
}


int Simulator::convertData( void )
{
  for ( int k=0; k<Data.size(); k++ ) {
    if ( Data[k].device() >= 0 ) {
      // clear input error state:
      Data[k].clearError();

      // mark restart:
      if ( Sim->restarted() ) {
	LastWrite = -1.0;
	Data[k].setRestart();
      }

      // read data from simulation:
      while ( Sim->size( k ) > 0 )
	Data[k].push( Sim->pop( k ) );
    }
  }

  return 0;
}


int Simulator::stopRead( void )
{
  if ( Sim != 0 )
    Sim->stop();
  return 0;
}


int Simulator::restartRead( vector< AnalogOutput* > &aos, bool updategains )
{
  bool success = true;

  // set restart index:
  for ( unsigned int i=0; i<AI.size(); i++ )
    AI[i].Traces.setRestart();

  // set new gain indices:
  int t = 0;
  if ( updategains ) {
    for ( unsigned int i=0; i<AI.size(); i++ ) {

      AIData &ai = AI[i];

      // clear adjust-flag:
      ai.Traces.delMode( AdjustFlag );
      
      // set gainindices in data:
      for ( unsigned int k=0; k<ai.Gains.size(); k++ ) {
	if ( ai.Gains[k] >= 0 ) {
	  ai.Traces[k].setGainIndex( ai.Gains[k] );
	  if ( AdjustFlag > 0 )
	    ai.Traces[k].addMode( AdjustFlag );
	  ai.Gains[k] = -1;
	  // set gain:
	  if ( ai.Traces[k].unipolar() ) {
	    double max = ai.AI->unipolarRange( ai.Traces[k].gainIndex() );
	    ai.Traces[k].setMaxVoltage( max );
	    ai.Traces[k].setMinVoltage( 0.0 );
	  }
	  else {
	    double max = ai.AI->bipolarRange( ai.Traces[k].gainIndex() );
	    ai.Traces[k].setMaxVoltage( max );
	    ai.Traces[k].setMinVoltage( -max );
	  }
	  t++;
	}
      }
    }
  }

  return success ? 0 : -1;
}


int Simulator::convert( OutData &signal )
{
  signal.clearError();
  signal.setManualConvert();
  return 0;
}


int Simulator::write( OutData &signal )
{
  signal.clearError();

  if ( Sim == 0 )
    signal.addError( OutData::NoDevice );

  // set trace:
  applyOutTrace( signal );

  // get ao device:
  int di = signal.device();
  if ( di < 0 ) {
    signal.addError( OutData::NoDevice );
    signal.setDevice( 0 );
  }
  else if ( di >= (int)AO.size() ) {
    signal.addError( OutData::NoDevice );
    signal.setDevice( AO.size()-1 );
  }

  // error?
  if ( signal.failed() )
    return -1;

  // set intensity:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == signal.device() ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      double intens = signal.intensity();
      int ra = Att[a].Att->write( intens, signal.carrierFreq() );
      signal.setIntensity( intens );
      signal.addAttError( ra );
    }
    //    else
    //    Att[a].Att->mute();
  }

  // error?
  if ( signal.failed() )
    return -1;

  // test writing to daq board:
  OutList ol( &signal );  
  AO[di].AO->testWrite( ol );

  // error?
  if ( signal.failed() )
    return -1;

  // prepare writing to daq board:
  AO[di].AO->prepareWrite( ol );

  // error?
  if ( signal.failed() )
    return -1;

  // start writing to daq board:
  if ( gainChanged() ||
       signal.restart() ||
       SyncMode == NoSync || SyncMode == StartSync || SyncMode == TriggerSync ) {
    vector< AnalogOutput* > aos( 1, AO[di].AO );
    restartRead( aos, true );
  }
  else
    AO[di].AO->startWrite();

  // error?
  if ( signal.failed() )
    return -1;

  double st = Sim->add( signal );
  // device still busy?
  if ( st < 0.0 ) {
    if ( signal.priority() ) {
      Sim->stopSignal();
      st = Sim->add( signal );
    }
    else
      signal.addError( OutData::Busy );
  }
  if ( st >= 0.0 ) {
    LastWrite = st;
    LastDuration = signal.duration();
    LastDelay = 0.0;  // this is already contained in st!
  }

  // error?
  if ( signal.failed() )
    return -1;

  return 0;
}


int Simulator::writeData( void )
{
  return 0;
}


int Simulator::stopWrite( void )
{
  if ( Sim != 0 )
    Sim->clearSignals();
  return 0;
}


void Simulator::readSignal( InList &data, EventList &events )
{
  if ( LastWrite < 0.0 )
    return;

  double sigtime = LastWrite + LastDelay;

  // set signal time in input traces:
  data.setSignalTime( sigtime );

  // set signal time in events:
  events.setSignalTime( sigtime );
  
  // add signal time to stimulus events:
  for ( int k=0; k<events.size(); k++ ) {
    if ( (events[k].mode() & StimulusEventMode) == 0 )
      break;
    if ( events[k].empty() || events[k].back() < sigtime )
      events[k].push( sigtime, 0.0, LastDuration );
    else if ( ! events[k].empty() && events[k].back() >= sigtime ) {
      cerr << currentTime()
	   << " ! error in Simulator::readSignal() -> signalTime " << sigtime
	   << " < back() " << events[k].back() 
	   << " Current " << data[0].currentIndex()
	   << " Restart " << data[0].restartIndex() << '\n';
    }
  }

  LastWrite = -1.0;

  // restart time:
  double restarttime = data[0].restartTime();
  for ( int k=0; k<events.size(); k++ ) {
    if ( (events[k].mode() & RestartEventMode) > 0 ) {
      if ( events[k].empty() || events[k].back() < restarttime ) {
	events[k].push( restarttime );
      }
      else if ( ! events[k].empty() && events[k].back() > restarttime ) {
	cerr << currentTime()
	     << " ! error in Simulator::readSignal() -> restartTime " << restarttime
	     << " < back() " << events[k].back() 
	     << " Current " << data[0].currentIndex()
	     << " Restart " << data[0].restartIndex() << '\n';
      }
      break;
    }
  }
  
}


void Simulator::stop( void )
{
  stopWrite();
  stopRead();
  if ( Sim != 0 )
    Sim->clearSignals();
}


}; /* namespace relacs */

