/*
  simulator.cc
  Simulation of Acquire

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
      if ( data[k].source() <= 0 ) {
	// filters have device = -1 but should not cause errors!
	data[k].addError( DaqError::NoDevice );
	data[k].setDevice( 0 );
	success = false;
      }
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
    if ( Sim->isRunning() &&
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

  // mark restart:
  InTraces.setRestart();
  EventDataMutex.lock();
  if ( RestartEvents != 0 )
    RestartEvents->push( InTraces[0].restartTime() );
  EventDataMutex.unlock();

  // start reading from daq boards:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 && 
	 AI[i].AI->startRead( 0, &AIDataMutex, &AIWait, 0 ) != 0 )
      success = false;
  }

  // error?
  if ( ! success )
    return -1;

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
	bs *= 100;
    }
  }

  // start simulation:
  LastWrite = -1.0;
  SyncMode = AISync;

  Sim->start( data, &AIDataMutex, &AIWait );

  return 0;
}


int Simulator::stopRead( void )
{
  if ( Sim != 0 )
    Sim->stop();
  return 0;
}


int Simulator::restartRead( vector< AOData* > &aos, bool directao,
			    bool updategains )
{
  bool success = true;

  // set restart index:
  InTraces.setRestart();
  EventDataMutex.lock();
  if ( RestartEvents != 0 )
    RestartEvents->push( InTraces[0].restartTime() );
  EventDataMutex.unlock();

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


int Simulator::write( OutData &signal, bool setsignaltime )
{
  signal.clearError();

  if ( Sim == 0 )
    signal.addError( OutData::NoDevice );

  // set trace:
  applyOutTrace( signal );

  // error?
  if ( signal.failed() )
    return -1;

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
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  // clear device datas:
  for ( unsigned int i=0; i<AO.size(); i++ )
    AO[i].Signals.clear();

  // add data to device:
  AO[di].Signals.add( &signal );

  // set intensity:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == signal.device() ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      double level = 0.0;
      double fac = 1.0;
      if ( signal.noIntensity() && signal.noLevel() )
	signal.addError( DaqError::NoIntensity );
      else if ( signal.noIntensity() ) {
	level = signal.level();
	int ra = Att[a].Att->attenuate( level );
	signal.setLevel( level );
	signal.addAttError( ra );
      }
      else {
	double intens = signal.intensity();
	int ra = 0;
	if ( intens == OutData::MuteIntensity ) {
	  ra = Att[a].Att->mute();
	  fac = 0.0;
	}
	else {
	  ra = Att[a].Att->write( intens, signal.carrierFreq(), level );
	  signal.setIntensity( intens );
	  signal.setLevel( level );
	}
	signal.addAttError( ra );
      }
      if ( Att[a].Att->noAttenuator() ) {
	// without attenuator we need to scale the signal!
	if ( fac != 0.0 )
	  fac = ::pow( 10.0, -level/20.0 );
	signal.setScale( fac );
	// The scale factor will be multiplied by maxboardvolt in prepareWrite()!
      }
      else
	signal.setScale( 1.0 );
    }
    else
      Att[a].Att->mute();
  }

  // error?
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  // test writing to daq board:
  OutList ol( &signal );  
  AO[di].AO->testWrite( ol );

  // error?
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  // prepare writing to daq board:
  AO[di].AO->prepareWrite( ol );

  // error?
  if ( signal.failed() ) {
    AO[di].AO->reset();
    AO[di].Signals.clear();
    return -1;
  }

  // start writing to daq board:
  if ( gainChanged() ||
       signal.restart() ||
       SyncMode == NoSync || SyncMode == StartSync || SyncMode == TriggerSync ) {
    vector< AOData* > aos( 1, &AO[di] );
    restartRead( aos, false, true );
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
      Sim->stopSignals();
      st = Sim->add( signal );
    }
    else
      signal.addError( OutData::Busy );
  }
  if ( st >= 0.0 && setsignaltime ) {
    LastWrite = st;
    LastDuration = signal.duration();
    LastDelay = 0.0;  // this is already contained in st!
  }

  // error?
  if ( signal.failed() ) {
    AO[di].AO->reset();
    AO[di].Signals.clear();
    LastWrite = -1.0;
    return -1;
  }

  return 0;
}


int Simulator::write( OutList &signal, bool setsignaltime )
{
  bool success = true;
  signal.clearError();

  if ( Sim == 0 )
    signal.addError( OutData::NoDevice );

  // set trace:
  applyOutTrace( signal );

  // error?
  if ( signal.failed() )
    return -1;

  // get device ids and sort signal per device:
  for ( int k=0; k<signal.size(); k++ ) {
    // no device?
    if ( signal[k].device() < 0 ) {
      signal[k].addError( DaqError::NoDevice );
      signal[k].setDevice( 0 );
      success = false;
    }
    else if ( signal[k].device() >= (int)AO.size() ) {
      signal[k].addError( DaqError::NoDevice );
      signal[k].setDevice( AO.size()-1 );
      success = false;
    }
  }

  // error?
  if ( signal.failed() )
    return -1;

  // clear device datas:
  for ( unsigned int i=0; i<AO.size(); i++ )
    AO[i].Signals.clear();

  // add signals:
  for ( int k=0; k<signal.size(); k++ )
    AO[signal[k].device()].Signals.add( &signal[k] );

  // multiple delays:
  for ( int k=0; k<signal.size(); k++ ) {
    if ( signal[k].delay() != 
	 signal[0].delay() ) {
      signal[0].addError( DaqError::MultipleDelays );
      signal[k].addError( DaqError::MultipleDelays );
      signal[k].setDelay( signal[0].delay() );
      success = false;
    }
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ )
      AO[i].Signals.clear();
    return -1;
  }

  // set intensities or levels:
  bool usedatt[Att.size()];
  for ( unsigned int a=0; a<Att.size(); a++ )
    usedatt[a] = false;
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    for ( int k=0; k<AO[i].Signals.size(); k++ ) {
      for ( unsigned int a=0; a<Att.size(); a++ ) {
	if ( Att[a].Id == (int)i &&
	     Att[a].Att->aoChannel() == AO[i].Signals[k].channel() ) {
	  usedatt[a] = true;
	  double level = 0.0;
	  double fac = 1.0;
	  if ( AO[i].Signals[k].noIntensity() && AO[i].Signals[k].noLevel() ) {
	    AO[i].Signals[k].addError( DaqError::NoIntensity );
	    success = false;
	  }
	  else if ( AO[i].Signals[k].noIntensity() ) {
	    level = AO[i].Signals[k].level();
	    int ra = Att[a].Att->attenuate( level );
	    AO[i].Signals[k].setLevel( level );
	    AO[i].Signals[k].addAttError( ra );
	  }
	  else {
	    double intens = AO[i].Signals[k].intensity();
	    int ra = 0;
	    if ( intens == OutData::MuteIntensity ) {
	      ra = Att[a].Att->mute();
	      fac = 0.0;
	    }
	    else {
	      ra = Att[a].Att->write( intens, AO[i].Signals[k].carrierFreq(), level );
	      AO[i].Signals[k].setIntensity( intens );
	      AO[i].Signals[k].setLevel( level );
	    }
	    if ( ra != 0 ) {
	      AO[i].Signals[k].addAttError( ra );
	      success = false;
	    }
	  }
	  if ( Att[a].Att->noAttenuator() ) {
	    // without attenuator we need to scale the signal!
	    if ( fac != 0.0 )
	      fac = ::pow( 10.0, -level/20.0 );
	    AO[i].Signals[k].setScale( fac );
	    // The scale factor will be multiplied by maxboardvolt in prepareWrite()!
	  }
	  else
	    AO[i].Signals[k].setScale( 1.0 );
	}
      }
    }
  }
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ! usedatt[a] )
      Att[a].Att->mute();
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ )
      AO[i].Signals.clear();
    return -1;
  }

  // test writing to daq boards:
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    if ( AO[i].Signals.size() > 0 &&
	 AO[i].AO->testWrite( AO[i].Signals ) != 0 )
      success = false;
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    return -1;
  }

  // prepare writing to daq boards:
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    if ( AO[i].Signals.size() > 0 ) {
      if ( AO[i].AO->prepareWrite( AO[i].Signals ) != 0 )
	success = false;
    }
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    return -1;
  }

  // start writing to daq boards:
  if ( gainChanged() ||
       signal[0].restart() ||
       SyncMode == NoSync || SyncMode == StartSync || SyncMode == TriggerSync ) {
    vector< AOData* > aod;
    aod.reserve( AO.size() );
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( AO[i].Signals.size() > 0 )
	aod.push_back( &AO[i] );
    }
    if ( restartRead( aod, false, true ) != 0 )
      success = false;
  }
  else {
    // clear adjust-flags:
    for ( unsigned int i=0; i<AI.size(); i++ )
      AI[i].Traces.delMode( AdjustFlag );
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( AO[i].Signals.size() > 0 ) {
	if ( AO[i].AO->startWrite() != 0 )
	  success = false;
      }
    }
  }
  
  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    return -1;
  }

  double st = Sim->add( signal );
  // device still busy?
  if ( st < 0.0 ) {
    if ( signal[0].priority() ) {
      Sim->stopSignals();
      st = Sim->add( signal );
    }
    else
      signal.addError( OutData::Busy );
  }

  // successfull signal:
  if ( st >= 0.0 && setsignaltime ) {
    LastWrite = st;
    LastDuration = signal[0].duration();
    LastDelay = 0.0;  // this is already contained in st!
  }

  // error?
  if ( signal.failed() ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    LastWrite = -1.0;
    return -1;
  }

  return 0;
}


int Simulator::waitForWrite( void )
{
  if ( Sim != 0 )
    Sim->waitOnSignals();
  return 0;
}


int Simulator::stopWrite( void )
{
  if ( Sim != 0 )
    Sim->clearSignals();
  return 0;
}


int Simulator::directWrite( OutData &signal, bool setsignaltime )
{
  signal.clearError();

  // set trace:
  applyOutTrace( signal );

  // error?
  if ( signal.failed() )
    return -1;

  // get ao device:
  int di = signal.device();
  if ( di < 0 ) {
    signal.addError( DaqError::NoDevice );
    signal.setDevice( 0 );
  }
  else if ( di >= (int)AO.size() ) {
    signal.addError( DaqError::NoDevice );
    signal.setDevice( AO.size()-1 );
  }

  // device still busy?
  if ( AO[di].AO->status() == AnalogOutput::Running ) {
    if ( signal.priority() )
      AO[di].AO->reset();
    else
      signal.addError( DaqError::Busy );
  }

  // error?
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  // clear device datas:
  for ( unsigned int i=0; i<AO.size(); i++ )
    AO[i].Signals.clear();

  // add data to device:
  AO[di].Signals.add( &signal );

  // set intensity:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == di ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      double level = 0.0;
      double fac = 1.0;
      if ( signal.noIntensity() && signal.noLevel() )
	signal.addError( DaqError::NoIntensity );
      else if ( signal.noIntensity() ) {
	level = signal.level();
	int ra = Att[a].Att->attenuate( level );
	signal.setLevel( level );
	signal.addAttError( ra );
      }
      else {
	double intens = signal.intensity();
	int ra = 0;
	if ( intens == OutData::MuteIntensity ) {
	  ra = Att[a].Att->mute();
	  fac = 0.0;
	}
	else {
	  ra = Att[a].Att->write( intens, signal.carrierFreq(), level );
	  signal.setIntensity( intens );
	  signal.setLevel( level );
	}
	signal.addAttError( ra );
      }
      if ( Att[a].Att->noAttenuator() ) {
	// without attenuator we need to scale the signal!
	if ( fac != 0.0 )
	  fac = ::pow( 10.0, -level/20.0 );
	signal.setScale( fac );
	// The scale factor will be multiplied by maxboardvolt in directWrite()!
      }
      else
	signal.setScale( 1.0 );
    }
    else
      Att[a].Att->mute();
  }

  // start writing to daq board:
  if ( gainChanged() ||
       signal.restart() ||
       SyncMode == NoSync || SyncMode == StartSync || SyncMode == TriggerSync ) {
    vector< AOData* > aos( 1, &AO[di] );
    restartRead( aos, true, true );
  }
  else
    AO[di].AO->directWrite( AO[di].Signals );

  // error?
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  double st = Sim->add( signal );
  // device still busy?
  if ( st < 0.0 ) {
    if ( signal.priority() ) {
      Sim->stopSignals();
      st = Sim->add( signal );
    }
    else
      signal.addError( OutData::Busy );
  }
  if ( st >= 0.0 && setsignaltime ) {
    LastWrite = st;
    LastDuration = signal.duration();
    LastDelay = 0.0;  // this is already contained in st!
  }

  // error?
  if ( signal.failed() ) {
    AO[di].AO->reset();
    AO[di].Signals.clear();
    LastWrite = -1.0;
    return -1;
  }

  return 0;
}


int Simulator::directWrite( OutList &signal, bool setsignaltime )
{
  if ( signal.size() <= 0 )
    return 0;

  bool success = true;
  signal.clearError();

  // set trace:
  applyOutTrace( signal );

  // error?
  if ( signal.failed() )
    return -1;

  // get device ids and sort signal per device:
  for ( int k=0; k<signal.size(); k++ ) {
    // no device?
    if ( signal[k].device() < 0 ) {
      signal[k].addError( DaqError::NoDevice );
      signal[k].setDevice( 0 );
      success = false;
    }
    else if ( signal[k].device() >= (int)AO.size() ) {
      signal[k].addError( DaqError::NoDevice );
      signal[k].setDevice( AO.size()-1 );
      success = false;
    }
  }

  // busy:
  for ( int i=0; i<(int)AO.size(); i++ ) {
    // multiple priorities?
    int k0 = -1;
    for ( int k=0; k<signal.size(); k++ ) {
      if ( signal[k].device() == i ) {
	k0 = k;
      }
    }
    if ( k0 >= 0 ) {
      for ( int k=k0+1; k<signal.size(); k++ ) {
	if ( signal[k].device() == i &&
	     ( signal[k].priority() != signal[k0].priority() ) ) {
	  signal[k0].addError( DaqError::MultiplePriorities );
	  signal[k].addError( DaqError::MultiplePriorities );
	  signal[k].setPriority( signal[k0].priority() );
	  success = false;
	}
      }
      // device still busy?
      if ( AO[i].AO->status() == AnalogOutput::Running ) {
	if ( signal[k0].priority() )
	  AO[i].AO->reset();
	else {
	  for ( int k=k0; k<signal.size(); k++ ) {
	    if ( signal[k].device() == i )
	      signal[k].addError( DaqError::Busy );
	  }
	  success = false;
	}
      }
    }
  }

  // clear device datas:
  for ( unsigned int i=0; i<AO.size(); i++ )
    AO[i].Signals.clear();

  // add signals:
  for ( int k=0; k<signal.size(); k++ )
    AO[signal[k].device()].Signals.add( &signal[k] );

  // multiple delays:
  for ( int k=0; k<signal.size(); k++ ) {
    if ( signal[k].delay() != 
	 signal[0].delay() ) {
      signal[0].addError( DaqError::MultipleDelays );
      signal[k].addError( DaqError::MultipleDelays );
      signal[k].setDelay( signal[0].delay() );
      success = false;
    }
    if ( signal[k].delay() > 0 ) {
      signal[k].addError( DaqError::InvalidDelay );
      signal[k].setDelay( 0.0 );
      success = false;
    }
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ )
      AO[i].Signals.clear();
    return -1;
  }

  // set intensities or levels:
  bool usedatt[Att.size()];
  for ( unsigned int a=0; a<Att.size(); a++ )
    usedatt[a] = false;
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    for ( int k=0; k<AO[i].Signals.size(); k++ ) {
      for ( unsigned int a=0; a<Att.size(); a++ ) {
	if ( Att[a].Id == (int)i &&
	     Att[a].Att->aoChannel() == AO[i].Signals[k].channel() ) {
	  usedatt[a] = true;
	  double level = 0.0;
	  double fac = 1.0;
	  if ( AO[i].Signals[k].noIntensity() && AO[i].Signals[k].noLevel() ) {
	    AO[i].Signals[k].addError( DaqError::NoIntensity );
	    success = false;
	  }
	  else if ( AO[i].Signals[k].noIntensity() ) {
	    level = AO[i].Signals[k].level();
	    int ra = Att[a].Att->attenuate( level );
	    AO[i].Signals[k].setLevel( level );
	    AO[i].Signals[k].addAttError( ra );
	  }
	  else {
	    double intens = AO[i].Signals[k].intensity();
	    int ra = 0;
	    if ( intens == OutData::MuteIntensity ) {
	      ra = Att[a].Att->mute();
	      fac = 0.0;
	    }
	    else {
	      ra = Att[a].Att->write( intens, AO[i].Signals[k].carrierFreq(), level );
	      AO[i].Signals[k].setIntensity( intens );
	      AO[i].Signals[k].setLevel( level );
	    }
	    if ( ra != 0 ) {
	      AO[i].Signals[k].addAttError( ra );
	      success = false;
	    }
	  }
	  if ( Att[a].Att->noAttenuator() ) {
	    // without attenuator we need to scale the signal!
	    if ( fac != 0.0 )
	      fac = ::pow( 10.0, -level/20.0 );
	    AO[i].Signals[k].setScale( fac );
	    // The scale factor will be multiplied by maxboardvolt in directWrite()!
	  }
	  else
	    AO[i].Signals[k].setScale( 1.0 );
	}
      }
    }
  }
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ! usedatt[a] )
      Att[a].Att->mute();
  }

  // start writing to daq boards:
  if ( success ) {
    if ( gainChanged() ||
	 signal[0].restart() ||
	 SyncMode == NoSync || SyncMode == StartSync || SyncMode == TriggerSync ) {
      vector< AOData* > aod;
      aod.reserve( AO.size() );
      for ( unsigned int i=0; i<AO.size(); i++ ) {
	if ( AO[i].Signals.size() > 0 )
	  aod.push_back( &AO[i] );
      }
      if ( restartRead( aod, true, true ) != 0 )
	success = false;
    }
    else {
      for ( unsigned int i=0; i<AO.size(); i++ ) {
	if ( AO[i].Signals.size() > 0 ) {
	  if ( AO[i].AO->directWrite( AO[i].Signals ) != 0 )
	    success = false;
	}
      }
    }
  }
  
  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    return -1;
  }

  double st = Sim->add( signal[0] );
  // device still busy?
  if ( st < 0.0 ) {
    if ( signal[0].priority() ) {
      Sim->stopSignals();
      st = Sim->add( signal[0] );
    }
    else
      signal.addError( OutData::Busy );
  }
  if ( st >= 0.0 && setsignaltime ) {
    LastWrite = st;
    LastDuration = signal[0].duration();
    LastDelay = 0.0;  // this is already contained in st!
  }

  // error?
  if ( signal.failed() ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    LastWrite = -1.0;
    return -1;
  }

  return 0;
}


int Simulator::writeZero( int channel, int device )
{
  // check ao device:
  if ( device < 0 || device >= (int)AO.size() )
    return -1;

  // device still busy?
  Sim->stopSignals();
  AO[device].AO->reset();

  OutData signal( 1, 0.0001 );
  signal.setChannel( channel, device );
  for ( int k=0; k<(int)OutTraces.size(); k++ ) {
    if ( device == OutTraces[k].device() &&
	 channel == OutTraces[k].channel() ) {
      signal.setTrace( k );
      break;
    }
  }
  signal[0] = 0.0;
  OutList sigs( signal );

  // write to daq board:
  AO[device].AO->directWrite( sigs );
  Sim->add( sigs[0] );

  // error?
  if ( ! signal.success() ) {
    cerr << currentTime() << " ! error in Acquire::writeZero( int, int ) -> "
	 << signal.errorStr() << "\n";
    return -signal.error();
  }

  return 0;
}


bool Simulator::getSignal( void )
{
  if ( LastWrite < 0.0 )
    return false;

  double signaltime = LastWrite + LastDelay;

  if ( signaltime <= SignalTime ) 
    return false;

  EventDataMutex.lock();
  if ( NewSignalTime )
    cerr << currentTime()
	 << " ! error in Acquire::getSignal() -> NewSignalTime still true\n";
  SignalTime = signaltime;
  NewSignalTime = true;
  if ( SignalEvents != 0 )
    SignalEvents->push( SignalTime, 0.0, LastDuration );
  EventDataMutex.unlock();

  LastWrite = -1.0;
  return true;
}


void Simulator::stop( void )
{
  stopWrite();
  stopRead();
  if ( Sim != 0 )
    Sim->clearSignals();
}


}; /* namespace relacs */

