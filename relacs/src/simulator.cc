/*
  simulator.cc
  Simulation of Acquire

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
  for ( unsigned int i=0; i<AI.size(); i++ )
    AI[i].Traces.setRestart();

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
	bs *= 100;
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


int Simulator::restartRead( vector< AOData* > &aos, bool directao,
			    bool updategains )
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


int Simulator::setupWrite( OutData &signal )
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
  if ( signal.failed() )
    return -1;

  // set intensity:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == signal.device() ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      if ( signal.noIntensity() && signal.noLevel() )
	signal.addError( DaqError::NoIntensity );
      else if ( signal.noIntensity() ) {
	double level = signal.level();
	int ra = Att[a].Att->attenuate( level );
	signal.setLevel( level );
	signal.addAttError( ra );
      }
      else {
	double intens = signal.intensity();
	int ra = 0;
	if ( intens == OutData::MuteIntensity )
	  ra = Att[a].Att->mute();
	else {
	  double level = 0.0;
	  ra = Att[a].Att->write( intens, signal.carrierFreq(), level );
	  signal.setIntensity( intens );
	  signal.setLevel( level );
	}
	signal.addAttError( ra );
      }
    }
    else
      Att[a].Att->mute();
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

  return 0;
}


int Simulator::startWrite( OutData &signal )
{
  int di = signal.device();

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
  if ( signal.failed() ) {
    LastWrite = -1.0;
    return -1;
  }

  return 0;
}


int Simulator::setupWrite( OutList &signal )
{
  cerr << "ERROR: Simulator::setupWrite( OutList& ) not yet impemented!\n";
  return -1;
}


int Simulator::startWrite( OutList &signal )
{
  cerr << "ERROR: Simulator::startWrite( OutList& ) not yet impemented!\n";
  return -1;
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


int Simulator::directWrite( OutData &signal )
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
  if ( AO[di].AO->running() ) {
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
      if ( signal.noIntensity() && signal.noLevel() )
	signal.addError( DaqError::NoIntensity );
      else if ( signal.noLevel() ) {
	double intens = signal.intensity();
	int ra = 0;
	if ( intens == OutData::MuteIntensity )
	  ra = Att[a].Att->mute();
	else {
	  double level = 0.0;
	  ra = Att[a].Att->write( intens, signal.carrierFreq(), level );
	  signal.setIntensity( intens );
	  signal.setLevel( level );
	}
	signal.addAttError( ra );
      }
      else {
	double level = signal.level();
	int ra = Att[a].Att->attenuate( level );
	signal.setLevel( level );
	signal.addAttError( ra );
      }
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
  if ( signal.failed() ) {
    LastWrite = -1.0;
    return -1;
  }

  return 0;
}


int Simulator::directWrite( OutList &signal )
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
      if ( AO[i].AO->running() ) {
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
	  if ( AO[i].Signals[k].noIntensity() && AO[i].Signals[k].noLevel() ) {
	    AO[i].Signals[k].addError( DaqError::NoIntensity );
	    success = false;
	  }
	  else if ( AO[i].Signals[k].noIntensity() ) {
	    double level = AO[i].Signals[k].level();
	    int ra = Att[a].Att->attenuate( level );
	    AO[i].Signals[k].setLevel( level );
	    AO[i].Signals[k].addAttError( ra );
	  }
	  else {
	    double intens = AO[i].Signals[k].intensity();
	    int ra = 0;
	    if ( intens == OutData::MuteIntensity )
	      ra = Att[a].Att->mute();
	    else {
	      double level = 0.0;
	      ra = Att[a].Att->write( intens, AO[i].Signals[k].carrierFreq(), level );
	      AO[i].Signals[k].setIntensity( intens );
	      AO[i].Signals[k].setLevel( level );
	    }
	    if ( ra != 0 ) {
	      AO[i].Signals[k].addAttError( ra );
	      success = false;
	    }
	  }
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
      Sim->stopSignal();
      st = Sim->add( signal[0] );
    }
    else
      signal.addError( OutData::Busy );
  }
  if ( st >= 0.0 ) {
    LastWrite = st;
    LastDuration = signal[0].duration();
    LastDelay = 0.0;  // this is already contained in st!
  }

  // error?
  if ( signal.failed() ) {
    LastWrite = -1.0;
    return -1;
  }

  return 0;
}


int Simulator::writeReset( bool channels, bool params )
{
  int retval = 0;

  Sim->stopSignal();

  OutList sigs;

  for ( unsigned int i=0; i<AO.size(); i++ ) {

    AO[i].AO->reset();

    for ( unsigned int k=0; k<OutTraces.size(); k++ ) {
      // this is a channel at the device that should be reset:
      if ( OutTraces[k].device() == (int)i &&
	   ( ( channels && OutTraces[k].channel() < 1000 ) ||
	     ( params &&  OutTraces[k].channel() >= 1000 ) ) ) {
	OutData sig( 0.0 );
	sig.setTrace( OutTraces[k].trace() );
	if ( OutTraces[k].apply( sig ) < 0 )
	  cerr << "! error: Acquire::writeReset() -> wrong match\n";
	sig.setIdent( "init" );
	sigs.push( sig );
      }
    }
    
  }

  directWrite( sigs );

  return retval;
}


int Simulator::writeZero( int channel, int device )
{
  // check ao device:
  if ( device < 0 || device >= (int)AO.size() )
    return -1;

  // device still busy?
  Sim->stopSignal();
  AO[device].AO->reset();

  OutData signal( 1, 0.0001 );
  signal.setChannel( channel, device );
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


bool Simulator::readSignal( double &signaltime, InList &data, EventList &events )
{
  if ( LastWrite < 0.0 )
    return false;

  double sigtime = LastWrite + LastDelay;

  signaltime = sigtime;

  // set signal time in input traces:
  data.setSignalTime( sigtime );

  // set signal time in events:
  events.setSignalTime( sigtime );
  
  // add signal time to stimulus events:
  for ( int k=0; k<events.size(); k++ ) {
    if ( (events[k].mode() & StimulusEventMode) > 0 ) {
      if ( events[k].empty() || events[k].back() < sigtime )
	events[k].push( sigtime, 0.0, LastDuration );
      else if ( ! events[k].empty() && events[k].back() >= sigtime ) {
	cerr << currentTime()
	     << " ! error in Simulator::readSignal() -> signalTime " << sigtime
	     << " < back() " << events[k].back() 
	     << " Current " << data[0].currentIndex()
	     << " Restart " << data[0].restartIndex() << '\n';
      }
      break;
    }
  }

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

