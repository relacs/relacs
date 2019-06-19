/*
  acquire.cc
  Read and write data from/to the data aqcuisition board.

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

#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <QReadLocker>
#include <QWriteLocker>
#include <relacs/acquire.h>

namespace relacs {


const string Acquire::SyncModeStrs[3] = {
  "simple restart of analog input together with analog output",
  "quick restart of analog input together with analog output",
  "continuous analog input, analog output reads out analog input counter" };

const TraceSpec Acquire::DummyTrace;


Acquire::Acquire( void )
{
  AI.clear();
  AdjustFlag = 0;

  AO.clear();
  LastDevice = -1;
  LastWrite = -1.0;
  LastDuration = 0.0;
  LastDelay = 0.0;
  SignalTime = -1.0;
  OutData::setAcquire( this );
  SignalEvents = 0;
  RestartEvents = 0;

  SyncMode = NoSync;

  Att.clear();
}


Acquire::~Acquire()
{
  OutData::setAcquire( 0 );
  clear();
}


int Acquire::addInput( AnalogInput *ai )
{
  if ( ai == 0 )
    return -1;

  if ( ! ai->isOpen() )
    return -2;

  QWriteLocker locker( &ReadMutex );

  AI.push_back( AIData( ai ) );

  return 0;
}


int Acquire::inputsSize( void ) const
{ 
  QReadLocker locker( &ReadMutex );
  int as = AI.size();
  return as;
}


int Acquire::inputIndex( const string &ident ) const
{
  QReadLocker locker( &ReadMutex );
  for ( unsigned int k=0; k < AI.size(); k++ ) {
    if ( AI[k].AI->deviceIdent() == ident )
      return k;
  }
  return -1;
}


const AnalogInput *Acquire::inputDevice( int deviceindex ) const
{
  QReadLocker locker( &ReadMutex );
  if ( deviceindex < 0 || deviceindex >= (int)AI.size() )
    return 0;

  return AI[deviceindex].AI;
}


const InList &Acquire::inputTraces( int deviceindex ) const
{
  QReadLocker locker( &ReadMutex );
  if ( deviceindex < 0 )
    deviceindex = 0;
  if ( deviceindex >= (int)AI.size() )
    deviceindex = AI.size()-1;

  return AI[deviceindex].Traces;
}


void Acquire::clearInputs( void )
{
  stopRead();
  QWriteLocker locker( &ReadMutex );
  AI.clear();
  InTraces.clear();
}


void Acquire::closeInputs( void )
{
  stopRead();
  QWriteLocker locker( &ReadMutex );
  for ( unsigned int k=0; k < AI.size(); k++ ) {
    if ( AI[k].AI->isOpen() )
      AI[k].AI->close();
    AI[k].Traces.clear();
    AI[k].Gains.clear();
  }
  AI.clear();
  InTraces.clear();
}


int Acquire::addOutput( AnalogOutput *ao )
{
  if ( ao == 0 )
    return -1;

  if ( ! ao->isOpen() )
    return -2;

  QWriteLocker locker( &WriteMutex );

  AO.push_back( AOData( ao ) );

  return 0;
}


int Acquire::outputsSize( void ) const
{
  QReadLocker locker( &WriteMutex );
  int as = AO.size();
  return as;
}


int Acquire::outputIndex( const string &ident ) const
{
  QReadLocker locker( &WriteMutex );
  for ( unsigned int k=0; k < AO.size(); k++ ) {
    if ( AO[k].AO->deviceIdent() == ident )
      return k;
  }
  return -1;
}


void Acquire::clearOutputs( void )
{
  stopWrite();
  QWriteLocker locker( &WriteMutex );
  AO.clear();
}


void Acquire::closeOutputs( void )
{
  stopWrite();
  QWriteLocker locker( &WriteMutex );
  for ( unsigned int k=0; k < AO.size(); k++ ) {
    if ( AO[k].AO->isOpen() )
      AO[k].AO->close();
    AO[k].Signals.clear();
  }
  AO.clear();
}


void Acquire::setSignalDelay( int device, double delay )
{
  QWriteLocker locker( &WriteMutex );
  if ( device >= 0 && device < (int)AO.size() && AO[device].AO != 0 ) {
    AO[device].AO->setDelay( delay );
  }
}


int Acquire::addAttLine( Attenuate *att, const string &device, int channel )
{
  if ( att == 0 )
    return -1;

  if ( ! att->isOpen() )
    return -2;

  if ( ! device.empty() )
    att->setAODevice( device );
  if ( channel >= 0 )
    att->setAOChannel( channel );

  // get id of analog output device:
  QWriteLocker locker( &WriteMutex );
  int id = -1;
  for ( unsigned int k=0; k<AO.size(); k++ ) {
    if ( AO[k].AO->deviceIdent() == att->aoDevice() ) {
      id = k;
      break;
    }
  }

  att->init();

  Att.push_back( AttData( att, id ) );

  return 0;
}


int Acquire::attLinesSize( void ) const
{
  QReadLocker locker( &WriteMutex );
  int as = Att.size();
  return as;
}


void Acquire::clearAttLines( void )
{
  QWriteLocker locker( &WriteMutex );
  Att.clear();
}


void Acquire::closeAttLines( void )
{
  QWriteLocker locker( &WriteMutex );
  for ( unsigned int k=0; k < Att.size(); k++ )
    if ( Att[k].Att->isOpen() )
      Att[k].Att->close();
  Att.clear();
}


void Acquire::addOutTrace( const string &name,
			   int device, int channel, 
			   double scale, const string &unit, double value,
			   double maxrate, const string &modality )
{
  if ( device >= 0 && device < (int)AO.size() && maxrate > AO[device].AO->maxRate() )
    maxrate = AO[device].AO->maxRate();
  OutTraces.push_back( TraceSpec( OutTraces.size(), name, device, channel,
				  scale, unit, value, maxrate, modality ) );
}


void Acquire::addOutTraces( void )
{
  for ( unsigned int k=0; k < AO.size(); k++ ) {
    AO[k].AO->addTraces( OutTraces, k );
  }
}


int Acquire::outTracesSize( void ) const
{
  return OutTraces.size();
}


int Acquire::outTraceIndex( const string &name ) const
{
  for ( unsigned int k=0; k<OutTraces.size(); k++ ) {
    if ( !OutTraces[k].traceName().empty() &&
	 OutTraces[k].traceName() == name )
      return k;
  }
  return -1;
}


string Acquire::outTraceName( int index ) const
{
  if ( index >= 0 && index < outTracesSize() )
    return OutTraces[index].traceName();
  else
    return "";
}


Attenuate *Acquire::outTraceAttenuate( int index )
{
  if ( index >= 0 && index < outTracesSize() ) {
    for ( unsigned int a=0; a<Att.size(); a++ ) {
      if ( ( Att[a].Id == OutTraces[index].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[index].channel() ) )
	return Att[a].Att;
    }
  }
  return 0;
}


const Attenuate *Acquire::outTraceAttenuate( int index ) const
{
  if ( index >= 0 && index < outTracesSize() ) {
    for ( unsigned int a=0; a<Att.size(); a++ ) {
      if ( ( Att[a].Id == OutTraces[index].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[index].channel() ) )
	return Att[a].Att;
    }
  }
  return 0;
}


const TraceSpec &Acquire::outTrace( int index ) const
{
  if ( index >= 0 && index < outTracesSize() )
    return OutTraces[ index ];
  else
    return DummyTrace;
}


const TraceSpec &Acquire::outTrace( const string &name ) const
{
  int inx = outTraceIndex( name );
  if ( inx >= 0 && inx < outTracesSize() )
    return OutTraces[ inx ];
  else
    return DummyTrace;
}


int Acquire::applyOutTrace( OutData &signal ) const
{
  if ( signal.trace() < 0 && signal.traceName().empty() ) {
    signal.addError( DaqError::InvalidTrace );
    return -1;
  }

  int inx = -1;
  if ( ! signal.traceName().empty() )
    inx = outTraceIndex( signal.traceName() );
  else
    inx = signal.trace();
  if ( inx < 0 || inx >= (int)OutTraces.size() ) {
    signal.addError( DaqError::InvalidTrace );
    return -1;
  }

  signal.setTrace( inx, "" );
  return OutTraces[inx].apply( signal ) * 3;
}


int Acquire::applyOutTrace( OutList &signal ) const
{
  int r = 0;
  for ( int k=0; k<signal.size(); k++ ) {
    int rr = applyOutTrace( signal[k] );
    if ( rr < 0 )
      r = -1;
  }
  return r;
}


void Acquire::clearOutTraces( void )
{
  OutTraces.clear();
}


void Acquire::addStimulusEvents( InList &data, EventList &events )
{
  EventData e( 6000, 0.0, 0.0, data[0].sampleInterval(),
	       false, true );  // as the width we store the signal durations
  e.setMode( StimulusEventMode );
  e.setCyclic();
  e.setSource( 0 );
  e.setIdent( "Stimulus" );
  e.setWidthName( "duration" );
  e.setWidthUnit( "s" );
  e.setWidthFormat( "%9.4f" );
  e.setWriteBufferCapacity( 1000 );
  QWriteLocker locker( &ReadMutex );
  events.push( e );
  SignalEvents = &events.back();
}


void Acquire::addRestartEvents( InList &data, EventList &events )
{
  EventData e( 6000, 0.0, 0.0, data[0].sampleInterval(),
	       false, false );
  e.setMode( RestartEventMode );
  e.setCyclic();
  e.setSource( 0 );
  e.setIdent( "Restart" );
  e.setWriteBufferCapacity( 1000 );
  QWriteLocker locker( &ReadMutex );
  events.push( e );
  RestartEvents = &events.back();
}


void Acquire::inTraces( vector< TraceSpec > &traces )
{
  for ( unsigned int k=0; k < AI.size(); k++ ) {
    // the standard physical input channels:
    for ( int c=0; c<AI[k].AI->channels(); c++ ) {
      ostringstream name;
      name << "device " << k << " channel " << c;
      traces.push_back( TraceSpec( traces.size(), name.str(), k, c ) );
    }
  }
  // additional input variables:
  for ( unsigned int k=0; k < AI.size(); k++ )
    AI[k].AI->addTraces( traces, 0 );
}


void Acquire::clear( void )
{
  clearInputs();
  clearOutputs();
  clearAttLines();
  clearOutTraces();
}


void Acquire::close( void )
{
  closeInputs();
  closeOutputs();
  closeAttLines();
  clearOutTraces();
}


void Acquire::initSync( void )
{
  QWriteLocker readlocker( &ReadMutex );
  QWriteLocker writelocker( &WriteMutex );

  // set synchronization mode:
  SyncMode = NoSync;

  // create lists of analog input and output devices:
  vector< AnalogInput* > ais;
  ais.reserve( AI.size() );
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    ais.push_back( AI[i].AI );
    AI[i].AIDevice = -1;
    AI[i].AIRate = false;
  }
  vector< AnalogOutput* > aos;
  aos.reserve( AO.size() );
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    aos.push_back( AO[i].AO );
    AO[i].AISyncDevice = -1;
    AO[i].AISyncRate = false;
    AO[i].AIDevice = -1;
    AO[i].AIRate = false;
    AO[i].AODevice = -1;
    AO[i].AORate = false;
  }
  
  // can analog output be synchronized with running analog input?
  SyncMode = AISync;
  for ( unsigned int i=0; i<aos.size(); i++ ) {
    AO[i].AISyncDevice = aos[i]->getAISyncDevice( ais );
    if ( AO[i].AISyncDevice < 0 )
      SyncMode = NoSync;
    else
      AO[i].AISyncRate = aos[i]->useAIRate();
  }

  // lists of taken device indices:
  vector< int > aiinx;
  aiinx.reserve( AI.size() );
  vector< int > aoinx;
  aoinx.reserve( AO.size() );
  vector< bool > airate;
  airate.reserve( AI.size() );
  vector< bool > aorate;
  aorate.reserve( AO.size() );
  
  // init combined start of other devices:
  for ( unsigned int i=0; i<ais.size(); i++ ) {
    aiinx.clear();
    aoinx.clear();
    airate.clear();
    aorate.clear();
    ais[i]->take( ais, aos, aiinx, aoinx, airate, aorate );
    for ( unsigned int j=0; j<aiinx.size(); j++ ) {
      if ( AI[aiinx[j]].AIDevice < 0 ) {
	AI[aiinx[j]].AIDevice = i;
	AI[aiinx[j]].AIRate = airate[j];
      }
    }
    for ( unsigned int j=0; j<aoinx.size(); j++ ) {
      if ( AO[aoinx[j]].AIDevice < 0 ) {
	AO[aoinx[j]].AIDevice = i;
	AO[aoinx[j]].AIRate = aorate[j];
      }
    }
  }
  for ( unsigned int i=0; i<aos.size(); i++ ) {
    aoinx.clear();
    aorate.clear();
    aos[i]->take( aos, aoinx, aorate );
    for ( unsigned int j=0; j<aoinx.size(); j++ ) {
      if ( AO[aoinx[j]].AODevice < 0 ) {
	AO[aoinx[j]].AODevice = i;
	AO[aoinx[j]].AORate = aorate[j];
      }
    }
  }

  // check for combined start of other devices:
  if ( SyncMode == NoSync ) {
    SyncMode = StartSync;
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( AO[i].AIDevice < 0 ) {
	SyncMode = NoSync;
	break;
      }
    }
    for ( unsigned int i=1; i<AI.size(); i++ ) {
      if ( AI[i].AIDevice < 0 ) {
	SyncMode = NoSync;
	break;
      }
    }
  }
}


Acquire::SyncModes Acquire::syncMode( void ) const
{
  return SyncMode;
}


string Acquire::syncModeStr( void ) const
{
  return SyncModeStrs[ SyncMode ];
}


int Acquire::testRead( InList &data )
{
  QReadLocker locker( &ReadMutex );

  //  cerr << "Acquire::testRead( InList& )\n";

  bool success = true;

  // clear error flags:
  data.clearError();

  // setup device datas:
  vector< InList > Traces( AI.size(), InList() );

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
      data[k].setDevice( AI.size()-1 );
      success = false;
    }
    else {
      // add data to device:
      Traces[data[k].device()].add( &data[k] );
    }
  }

  // priority, busy:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    // multiple priorities?
    for ( int k=1; k<Traces[i].size(); k++ ) {
      if ( Traces[i][k].priority() != 
	   Traces[i][0].priority() ) {
	Traces[i][0].addError( DaqError::MultiplePriorities );
	Traces[i][k].addError( DaqError::MultiplePriorities );
	Traces[i][k].setPriority( Traces[i][0].priority() );
	success = false;
      }
    }
    // device still busy?
    if ( Traces[i].size() > 0 &&
	 AI[i].AI->running() ) {
      if ( ! Traces[i][0].priority() ) {
	Traces[i].addError( DaqError::Busy );
	success = false;
      }
    }
  }

  // error?
  if ( ! success )
    return -1;

  // test reading from daq boards:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( Traces[i].size() > 0 &&
	 AI[i].AI->testRead( Traces[i] ) != 0 )
      success = false;
  }

  //  cerr << "Acquire::testRead( InList& )" << success << "\n";
  return success ? 0 : -1;
}


int Acquire::read( InList &data )
{
  //  cerr << "Acquire::read( InList& )\n";
  QWriteLocker readlocker( &ReadMutex );

  bool success = true;

  // clear error flags:
  data.clearError();

  // clear device datas:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    AI[i].Traces.clear();
    AI[i].Gains.clear();
  }
  InTraces.clear();
  PreviousTime = 0.0;
  NumEmptyData = 0;
  SignalTime = -1.0;

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
      data[k].setDevice( AI.size()-1 );
      success = false;
    }
    else {
      // add data to device:
      AI[data[k].device()].Traces.add( &data[k] );
      AI[data[k].device()].Gains.push_back( -1 );
      InTraces.add( &data[k] );
    }
  }

  // check model traces:
  if ( AI.size() > 0 && AI[0].AI->matchTraces( data ) < 0 ) {
    if ( ! data.failed() )
      data.addErrorStr( "unable to match model input traces" );
    success = false;
  }
  QWriteLocker writelocker( &WriteMutex );
  if ( AO.size() > 0 && AO[0].AO->matchTraces( OutTraces ) < 0 ) {
    /*
    if ( ! OutTraces.failed() )
      OutTraces.addErrorStr( "unable to match model output traces" );
    */
    data.addErrorStr( "unable to match model output traces" );
    success = false;
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
    if ( AI[i].AI->running() &&
	 AI[i].Traces.size() > 0 ) {
      if ( AI[i].Traces[0].priority() )
	AI[i].AI->stop();
      else {
	AI[i].Traces.addError( DaqError::Busy );
	success = false;
      }
    }
  }

  // error?
  if ( ! success )
    return -1;

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
	 AI[i].AI->prepareRead( AI[i].Traces ) != 0 ) {
      success = false;
      if ( AI[i].Traces.success() )
	AI[i].Traces.setError( DaqError::Unknown );
    }
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AI.size(); i++ )
      AI[i].AI->reset();
    return -1;
  }

  // clear analog input semaphore:
  if ( AISemaphore.available() > 0 )
    AISemaphore.acquire( AISemaphore.available() );

  // mark restart:
  InTraces.setRestart();
  if ( RestartEvents != 0 )
    RestartEvents->push( InTraces[0].restartTime() );

  // start reading from daq boards:
  vector< int > aistarted;
  aistarted.reserve( AI.size() );
  QWaitCondition *readwait = &ReadWait;
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 ) {
      bool started = false;
      for ( unsigned int k=0; k<aistarted.size(); k++ ) {
	if ( aistarted[k] == AI[i].AIDevice ) {
	  started = true;
	  break;
	}
      }
      if ( ! started ) {
	if ( AI[i].AI->startRead( &AISemaphore, &ReadMutex, readwait ) != 0 )
	  success = false;
	else {
	  aistarted.push_back( i );
	  readwait = 0; // we will wait on the first device only
	}
      }
    }
  }
  
  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AI.size(); i++ ) {
      AI[i].AI->stop();
      AI[i].AI->reset();
    }
    return -1;
  }

  // set fixed rates in analog output traces:
  for ( int k=0; k < outTracesSize(); k++ ) {
    if ( AO[OutTraces[k].device()].AISyncRate &&
	 AO[OutTraces[k].device()].AISyncDevice >= 0 &&
	 AO[OutTraces[k].device()].AISyncDevice < (int)AI.size() && 
	 AI[AO[OutTraces[k].device()].AISyncDevice].Traces.size() > 0 )
      OutTraces[k].setFixedSampleRate( AI[AO[OutTraces[k].device()].AISyncDevice].Traces[0].sampleRate() );
    if ( AO[OutTraces[k].device()].AIRate &&
	 AO[OutTraces[k].device()].AIDevice >= 0 &&
	 AO[OutTraces[k].device()].AIDevice < (int)AI.size() && 
	 AI[AO[OutTraces[k].device()].AIDevice].Traces.size() > 0 )
      OutTraces[k].setFixedSampleRate( AI[AO[OutTraces[k].device()].AIDevice].Traces[0].sampleRate() );
  }

  LastDevice = -1;
  LastWrite = -1.0;

  //  cerr << "Acquire::read( InList& ) finished\n";
  return 0;
}


string Acquire::readError( void ) const
{
  QReadLocker locker( &WriteMutex );
  return InTraces.errorText();
}


int Acquire::stopRead( void )
{
  bool success = true;
  double si = 0.0;

  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 &&
	 AI[i].AI->running() ) {
      if ( AI[i].AI->stop() != 0 )
	success = false;
      if ( si < AI[i].Traces[0].sampleInterval() )
	si = AI[i].Traces[0].sampleInterval();
    }
  }

  // sleep for two sampleintervals:
  usleep( long( 2000000.0 * si ) );

  // wake up all jobs sleeping on AISemaphore:
  AISemaphore.release( 1000 );

  return success ? 0 : -1;
}


int Acquire::restartRead( void )
{
  {
    QWriteLocker readlocker( &ReadMutex );
    bool success = true;

    // get shortest recording:
    double t = -1.0;
    for ( int i=0; i<InTraces.size(); i++ ) {
      InTraces[i].clearError();
      if ( t < 0.0 || InTraces[i].length() < t )
	t = InTraces[i].length();
    }

    // make all data the same length and set restart time:
    int m = 0;
    for ( int i=0; i<InTraces.size(); i++ ) {
      int n = InTraces[i].indices( t );
      int nd = InTraces[i].size() - n;
      if ( nd > 0 ) {
	m += nd;
	InTraces[i].resize( n );
      }
    }
    if ( m >= InTraces.size() )
      cerr << "Acquire::restartRead(): truncated " << m << " of " << InTraces.size() << "input traces\n";
    PreviousTime = InTraces.currentTimeRaw();
    NumEmptyData = 0;
    InTraces.setRestart();
    if ( RestartEvents != 0 )
      RestartEvents->push( InTraces[0].restartTime() );

    // reset and prepare reading from daq boards:
    for ( unsigned int i=0; i<AI.size(); i++ ) {
      if ( AI[i].Traces.size() > 0 &&
	   AI[i].AI->prepareRead( AI[i].Traces ) != 0 ) {
	success = false;
	if ( AI[i].Traces.success() )
	  AI[i].Traces.setError( DaqError::Unknown );
      }
    }

    // error?
    if ( ! success ) {
      for ( unsigned int i=0; i<AI.size(); i++ )
	AI[i].AI->reset();
      return -1;
    }

    // clear analog input semaphore:
    if ( AISemaphore.available() > 0 )
      AISemaphore.acquire( AISemaphore.available() );

    // start reading from daq boards:
    vector< int > aistarted;
    aistarted.reserve( AI.size() );
    QWaitCondition *readwait = &ReadWait;
    for ( unsigned int i=0; i<AI.size(); i++ ) {
      if ( AI[i].Traces.size() > 0 ) {
	bool started = false;
	for ( unsigned int k=0; k<aistarted.size(); k++ ) {
	  if ( aistarted[k] == AI[i].AIDevice ) {
	    started = true;
	    break;
	  }
	}
	if ( ! started ) {
	  if ( AI[i].AI->startRead( &AISemaphore, &ReadMutex, readwait ) != 0 )
	    success = false;
	  else {
	    aistarted.push_back( i );
	    readwait = 0; // we will wait on the first device only
	  }
	}
      }
    }
  
    if ( ! success )
      return -1;
  }

  {
    // set fixed rates in analog output traces:
    QWriteLocker writelocker( &WriteMutex );
    QWriteLocker readlocker( &ReadMutex );
    for ( int k=0; k < outTracesSize(); k++ ) {
      if ( AO[OutTraces[k].device()].AISyncRate &&
	   AO[OutTraces[k].device()].AISyncDevice >= 0 &&
	   AO[OutTraces[k].device()].AISyncDevice < (int)AI.size() && 
	   AI[AO[OutTraces[k].device()].AISyncDevice].Traces.size() > 0 )
	OutTraces[k].setFixedSampleRate( AI[AO[OutTraces[k].device()].AISyncDevice].Traces[0].sampleRate() );
      if ( AO[OutTraces[k].device()].AIRate &&
	   AO[OutTraces[k].device()].AIDevice >= 0 &&
	   AO[OutTraces[k].device()].AIDevice < (int)AI.size() && 
	   AI[AO[OutTraces[k].device()].AIDevice].Traces.size() > 0 )
	OutTraces[k].setFixedSampleRate( AI[AO[OutTraces[k].device()].AIDevice].Traces[0].sampleRate() );
    }
  }

  return 0;
}


int Acquire::restartRead( vector< AOData* > &aod, bool directao,
			  bool updategains )
{
  //  cerr << currentTime() << " Acquire::restartRead() begin \n";

  bool success = true;
  bool finished = true;

  // stop reading:
  if ( stopRead() != 0 )
    success = false;

  // WriteMutex gets locked from the calling functions.
  QWriteLocker locker( &ReadMutex );

  // get data and shortest recording:
  double t = -1.0;
  for ( int i=0; i<InTraces.size(); i++ ) {
    InTraces[i].clearError();
    if ( t < 0.0 || InTraces[i].length() < t )
      t = InTraces[i].length();
  }

  // make all data the same length and set restart time:
  int m = 0;
  for ( int i=0; i<InTraces.size(); i++ ) {
    int n = InTraces[i].indices( t );
    int nd = InTraces[i].size() - n;
    if ( nd > 0 ) {
      m += nd;
      InTraces[i].resize( n );
    }
  }
  if ( m >= InTraces.size() )
    cerr << "Acquire::restartRead(): truncated " << m << " of " << InTraces.size() << "input traces\n";
  PreviousTime = InTraces.currentTimeRaw();
  NumEmptyData = 0;
  InTraces.setRestart();
  if ( RestartEvents != 0 )
    RestartEvents->push( InTraces[0].restartTime() );

  // set signal start:
  if ( aod.size() > 0 && t >= 0.0 ) {
    LastWrite = t;
    // XXX How to deal with several daq boards with different delays?
    if ( AO.size() > 0 && AO[0].Signals.size() > 0 )
      LastWrite += AO[0].AO->delay( AO[0].Signals[0].channel() );
  }

  // set new gain indices:
  bool gainchanged = false;
  if ( updategains ) {
    for ( unsigned int i=0; i<AI.size(); i++ ) {

      // clear adjust-flag:
      AI[i].Traces.delMode( AdjustFlag );
      
      // set gainindices in data:
      for ( unsigned int k=0; k<AI[i].Gains.size(); k++ ) {
	if ( AI[i].Gains[k] >= 0 ) {
	  AI[i].Traces[k].setGainIndex( AI[i].Gains[k] );
	  AI[i].Traces[k].addMode( AdjustFlag );
	  AI[i].Gains[k] = -1;
	  gainchanged = true;
	}
      }
    }

  }

  // prepare reading from daq boards:
  if ( gainchanged ) {
    for ( unsigned int i=0; i<AI.size(); i++ ) {
      if ( AI[i].Traces.size() > 0 &&
	   AI[i].AI->prepareRead( AI[i].Traces ) != 0 ) {
	success = false;
	if ( AI[i].Traces.success() )
	  AI[i].Traces.setError( DaqError::Unknown );
      }
    }
  }

  if ( ! success ) {
    for ( unsigned int i=0; i<aod.size(); i++ ) {
      if ( ! aod[i]->Signals.failed() )
	aod[i]->Signals.addError( DaqError::Unknown );
    }
    LastWrite = -1.0;
    return -1;
  }

  // map aod devices to AO devices:
  vector< int > aoinx;
  aoinx.reserve( aod.size() );
  for ( unsigned int i=0; i<aod.size(); i++ ) {
    for ( unsigned int k=0; k<AO.size(); k++ ) {
      if ( aod[i] == &AO[k] ) {
	aoinx.push_back( k );
	break;
      }
    }
  }

  // clear analog input semaphore:
  if ( AISemaphore.available() > 0 )
    AISemaphore.acquire( AISemaphore.available() );

  // clear analog output semaphore:
  if ( AOSemaphore.available() > 0 )
    AOSemaphore.acquire( AOSemaphore.available() );
  QSemaphore *aos = &AOSemaphore;

  // direct analog output:
  if ( directao ) {
    for ( unsigned int i=0; i<aod.size(); i++ ) {
      if ( aod[i]->AO->directWrite( aod[i]->Signals ) != 0 )
	success = false;
    }
    aos = 0;
  }

  // no analog output:
  if ( aod.size() == 0 )
    aos = 0;

  // start reading from daq boards:
  vector< int > aistarted;
  aistarted.reserve( AI.size() );
  QWaitCondition *readwait = &ReadWait;
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 ) {
      bool started = false;
      for ( unsigned int k=0; k<aistarted.size(); k++ ) {
	if ( aistarted[k] == AI[i].AIDevice ) {
	  started = true;
	  break;
	}
      }
      if ( ! started ) {
	int r = AI[i].AI->startRead( &AISemaphore, &ReadMutex, readwait, aos );
	if ( r < 0 )
	  success = false;
	else {
	  if ( r > 0 )
	    finished = false;
	  aistarted.push_back( i );
	  readwait = 0; // we will wait on the first device only
	}
      }
    }
  }
    
  if ( ! success ) {
    for ( unsigned int i=0; i<aod.size(); i++ ) {
      if ( ! aod[i]->Signals.failed() )
	aod[i]->Signals.addError( DaqError::Unknown );
    }
    LastWrite = -1.0;
    return -1;
  }
    
  // start writing streaming signals:
  if ( ! directao ) {
    vector< int > aostarted;
    aostarted.reserve( aod.size() );
    for ( unsigned int i=0; i<aod.size(); i++ ) {
      bool started = false;
      for ( unsigned int k=0; k<aistarted.size(); k++ ) {
	if ( aistarted[k] == aod[i]->AIDevice ) {
	  started = true;
	  break;
	}
      }
      if ( ! started ) {
	for ( unsigned int k=0; k<aostarted.size(); k++ ) {
	  if ( aostarted[k] == aod[i]->AODevice ) {
	    started = true;
	    break;
	  }
	}
      }
      if ( ! started ) {
	int r = aod[i]->AO->startWrite( &AOSemaphore );
	if ( r < 0 )
	  success = false;
	else {
	  if ( r > 0 )
	    finished = false;
	  aostarted.push_back( aoinx[ i ] );
	}
      }
    }
  }
    
  if ( ! success ) {
    for ( unsigned int i=0; i<aod.size(); i++ ) {
      if ( ! aod[i]->Signals.failed() )
	aod[i]->Signals.addError( DaqError::Unknown );
    }
    LastWrite = -1.0;
    return -1;
  }

  // set fixed rates in analog output traces:
  for ( int k=0; k < outTracesSize(); k++ ) {
    if ( AO[OutTraces[k].device()].AISyncRate &&
	 AO[OutTraces[k].device()].AISyncDevice >= 0 &&
	 AO[OutTraces[k].device()].AISyncDevice < (int)AI.size() && 
	 AI[AO[OutTraces[k].device()].AISyncDevice].Traces.size() > 0 )
      OutTraces[k].setFixedSampleRate( AI[AO[OutTraces[k].device()].AISyncDevice].Traces[0].sampleRate() );
    if ( AO[OutTraces[k].device()].AIRate &&
	 AO[OutTraces[k].device()].AIDevice >= 0 &&
	 AO[OutTraces[k].device()].AIDevice < (int)AI.size() && 
	 AI[AO[OutTraces[k].device()].AIDevice].Traces.size() > 0 )
      OutTraces[k].setFixedSampleRate( AI[AO[OutTraces[k].device()].AIDevice].Traces[0].sampleRate() );
  }

  // cerr << currentTime() << " Acquire::restartRead() -> acquisition restarted " << success << "\n";

  return finished ? 0 : 1;
}


int Acquire::waitForRead( void )
{
  int nais = 0;
  {
    // number of analog input devices used for reading:
    QReadLocker locker( &ReadMutex );
    for ( unsigned int i=0; i<AI.size(); i++ ) {
      if ( ! AI[i].Traces.empty() && AI[i].AI->running() )
	nais++;
    }
  }

  // wait for the threads to finish:
  AISemaphore.acquire( nais );

  AISemaphore.acquire( AISemaphore.available() );

  ReadMutex.lockForRead();
  bool failed =  InTraces.failed();
  ReadMutex.unlock();
  if ( failed ) {
    // error:
    stopRead();
    return -1;
  }

  return 0;
}


int Acquire::getRawData( InList &data, EventList &events, double &signaltime,
			 double mintracetime, double prevsignal )
{
  QReadLocker locker( &ReadMutex );
  bool interrupted = false;
  if ( mintracetime > 0.0 ) {
    // do wee need to wait for a new signal?
    if ( prevsignal >= -1.0 ) {
      while ( InTraces.success() &&
	      SignalTime <= prevsignal &&
	      isReadRunning() ) { 
	ReadWait.wait( &ReadMutex );
	if ( ! isWriteRunning() )
	  break;
      }
      if ( SignalTime > prevsignal )
	mintracetime += SignalTime;
      else
	mintracetime = 0.0;
    }
    
    // do we need to wait for more data?
    while ( InTraces.success() &&
	    InTraces.currentTime() < mintracetime &&
	    isReadRunning() ) {
      ReadWait.wait( &ReadMutex );
    }
    
    interrupted = ( InTraces.currentTime() < mintracetime || mintracetime == 0.0 );
  }

  // set signal time:
  signaltime = SignalTime;
  // update raw data:
  data.updateRaw();
  events.updateRaw();
  // check data:
  bool error = InTraces.failed();
  return error ? -1 : ( interrupted ? 0 : 1 );
}


int Acquire::waitForData( double &signaltime )
{
  ReadMutex.lockForWrite();
  /*
  // XXX the writes restart are not a problem, as long as ai is really restarted.
  // XXX The problem (waiting for ever) arises, when analog input fails.
  // XXX We can only check this, when write() does not restart analog input!
  // XXX For this we woud need an additional mutex, since stopRead cannot be within the ReadMutex lock.
  // check whether all threads are really running:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( ! AI[i].Traces.empty() && ! AI[i].AI->running() ) {
      ReadMutex.unlock();
      stopRead();
      AI[i].Traces.setErrorStr( "analog input not running" );
      return -1;
    }
  }
  */
  ReadWait.wait( &ReadMutex );
  bool finished = ( InTraces.currentTimeRaw() <= PreviousTime );
  // XXX the following construct is needed, because ReadWait.wait() sometimes
  // returns immediately, without being waken up. (with daqflex/relacs.cfg)
  // XXX Maybe we should figure out, why this happens...
  // From Qt doc: If mutex is not in a locked state, wait() returns immediately.
  if ( finished ) {
    NumEmptyData++;
    if ( NumEmptyData < 3 ) {
      finished = false;
      //      cerr << "! warning in Acquire::updateRawData() -> indicated empty data\n";
    }
  }
  else
    NumEmptyData = 0;
  signaltime = getSignal();
  // set signal time:
  if ( signaltime >= 0.0 ) {
    SignalTime = signaltime;
    InTraces.setSignalTime( SignalTime );
    if ( SignalEvents != 0 )
      SignalEvents->setSignalTime( SignalTime );
    if ( RestartEvents != 0 )
      RestartEvents->setSignalTime( SignalTime );
  }
  PreviousTime = InTraces.currentTimeRaw();
  // check data:
  bool failed = InTraces.failed();
  ReadMutex.unlock();
  return failed ? -1 : ( finished ? 0 : 1 );
}


void Acquire::lockRead( void )
{
  ReadMutex.lockForRead();
}


void Acquire::unlockRead( void )
{
  ReadMutex.unlock();
}


bool Acquire::isReadRunning( void ) const
{
  QReadLocker locker( &ReadMutex );
  bool isrunning = true;
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( ! AI[i].Traces.empty() && ! AI[i].AI->running() ) {
      isrunning = false;
      break;
    }
  }
  return isrunning;
}


int Acquire::adjustFlag( void ) const
{
  return AdjustFlag;
}


void Acquire::setAdjustFlag( int flag )
{
  AdjustFlag = flag;
}


int Acquire::maxVoltages( const InData &data, vector<double> &ranges ) const
{
  QReadLocker locker( &ReadMutex );

  ranges.clear();

  // get device:
  int di = data.device();

  // data without device?
  if ( di < 0 || di >= (int)AI.size() )
    return DaqError::NoDevice;

  // device not open?
  if ( ! AI[di].AI->isOpen() )
    return DaqError::DeviceNotOpen;

  // the device:
  AnalogInput *ai = AI[di].AI;

  // fill in ranges:
  ranges.reserve( ai->maxRanges() );
  ranges.clear();
  if ( data.channel() < InData::ParamChannel ) {
    if ( data.unipolar() ) {
      for ( int k=0; k<ai->maxRanges() && ai->unipolarRange( k ) > 0; k++ )
	ranges.push_back( ai->unipolarRange( k ) );
    }
    else {
      for ( int k=0; k<ai->maxRanges() && ai->bipolarRange( k ) > 0; k++ )
	ranges.push_back( ai->bipolarRange( k ) );
    }
  }
  else {
    ranges.push_back( data.maxVoltage() );
  }

  return 0;
}


int Acquire::maxValues( const InData &data, vector<double> &ranges ) const
{
  QReadLocker locker( &ReadMutex );

  ranges.clear();

  // get device:
  int di = data.device();

  // data without device?
  if ( di < 0 || di >= (int)AI.size() )
    return DaqError::NoDevice;

  // device not open?
  if ( ! AI[di].AI->isOpen() )
    return DaqError::DeviceNotOpen;

  // the device:
  AnalogInput *ai = AI[di].AI;

  // fill in ranges:
  ranges.reserve( ai->maxRanges() );
  ranges.clear();
  if ( data.channel() < InData::ParamChannel ) {
    if ( data.unipolar() ) {
      for ( int k=0; k<ai->maxRanges() && ai->unipolarRange( k ) > 0; k++ )
	ranges.push_back( ai->unipolarRange( k ) * data.scale() );
    }
    else {
      for ( int k=0; k<ai->maxRanges() && ai->bipolarRange( k ) > 0; k++ )
	ranges.push_back( ai->bipolarRange( k ) * data.scale() );
    }
  }
  else {
    ranges.push_back( data.maxValue() );
  }

  return 0;
}


int Acquire::setGain( const InData &data, int gainindex )
{
  QWriteLocker locker( &ReadMutex );

  // get device:
  int di = data.device();

  // data without device?
  if ( di < 0 || di >= (int)AI.size() )
    return DaqError::NoDevice;

  // device not open?
  if ( ! AI[di].AI->isOpen() )
    return DaqError::DeviceNotOpen;

  // the device:
  AIData &ai = AI[di];

  int newindex = -1;

  // gainindex exist?
  if ( gainindex >= 0 && gainindex < ai.AI->maxRanges() &&
       ( ( data.unipolar() &&
	   ai.AI->unipolarRange( gainindex ) > 0.0 ) || 
	 ( !data.unipolar() &&
	   ai.AI->bipolarRange( gainindex ) > 0.0 ) ) ) {
    newindex = gainindex;
  }

  // no gain:
  if ( newindex < 0 )
    return DaqError::InvalidGain;

  // set gain:
  for ( int k=0; k<ai.Traces.size(); k++ ) {
    if ( ai.Traces[k].channel() == data.channel() ) {
      if ( ai.Traces[k].gainIndex() == newindex )
	ai.Gains[k] = -1;
      else
	ai.Gains[k] = newindex;
    }
  }

  return 0;
}


int Acquire::adjustGain( const InData &data, double maxvalue )
{
  QWriteLocker locker( &ReadMutex );

  // get device:
  int di = data.device();

  // data without device?
  if ( di < 0 || di >= (int)AI.size() )
    return DaqError::NoDevice;

  // device not open?
  if ( ! AI[di].AI->isOpen() )
    return DaqError::DeviceNotOpen;

  // the device:
  AIData &ai = AI[di];

  // find appropriate gain:
  int newindex = ai.AI->gainIndex( data.unipolar(), data.getVoltage( maxvalue ) );

  // no gain:
  if ( newindex < 0 )
    newindex = ai.AI->minGainIndex( data.unipolar() );
  if ( newindex < 0 )
    return DaqError::InvalidGain;

  // set gain:
  for ( int k=0; k<ai.Traces.size(); k++ ) {
    if ( ai.Traces[k].channel() == data.channel() ) {
      if ( ai.Traces[k].gainIndex() == newindex )
	ai.Gains[k] = -1;
      else
	ai.Gains[k] = newindex;
    }
  }

  return 0;
}


int Acquire::adjustGain( const InData &data,
			 double minvalue, double maxvalue )
{
  QWriteLocker locker( &ReadMutex );

  // get device:
  int di = data.device();

  // data without device?
  if ( di < 0 || di >= (int)AI.size() )
    return DaqError::NoDevice;

  // device not open?
  if ( ! AI[di].AI->isOpen() )
    return DaqError::DeviceNotOpen;

  // the device:
  AIData &ai = AI[di];

  int newindex = -1;

  // current gain index:
  int gainindex = data.gainIndex();
    
  // set new gain:
  if ( maxvalue > data.maxValue() ) {
    // overflow:
    for ( gainindex--; 
	  gainindex >= 0 &&
	    ( ( data.unipolar() && ai.AI->unipolarRange( gainindex ) <= 0.0 ) || 
	      ( !data.unipolar() && ai.AI->bipolarRange( gainindex ) <= 0.0 ) ); 
	  gainindex-- );
    if ( gainindex >= 0 )
      newindex = gainindex;
  }
  else {
    // find lower limit:
    int nextindex;
    for ( nextindex = gainindex+1; 
	  nextindex < ai.AI->maxRanges() &&
	    ( ( data.unipolar() && ai.AI->unipolarRange( nextindex ) <= 0.0 ) || 
	      ( !data.unipolar() && ai.AI->bipolarRange( nextindex ) <= 0.0 ) ); 
	  nextindex++ );
    if ( nextindex < ai.AI->maxRanges() ) {
      double lowerlimit = 0.0;
      if ( data.unipolar() )
	lowerlimit = data.maxValue() *
	  ai.AI->unipolarRange( nextindex ) /
	  ai.AI->unipolarRange( gainindex );
      else
	lowerlimit = data.maxValue() *
	  ai.AI->bipolarRange( nextindex ) /
	  ai.AI->bipolarRange( gainindex );
      if ( minvalue < lowerlimit )
	newindex = nextindex;
    }
  }

  // no gain:
  if ( newindex < 0 )
    return DaqError::InvalidGain;

  // set gain:
  for ( int k=0; k<ai.Traces.size(); k++ ) {
    if ( ai.Traces[k].channel() == data.channel() ) {
      if ( ai.Traces[k].gainIndex() == newindex )
	ai.Gains[k] = -1;
      else
	ai.Gains[k] = newindex;
    }
  }

  return 0;
}


bool Acquire::gainChanged( void ) const
{
  QReadLocker locker( &ReadMutex );
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    for ( unsigned int k=0; k<AI[i].Gains.size(); k++ ) {
      if ( AI[i].Gains[k] >= 0 )
	return true;
    }
  }
  return false;
}


int Acquire::activateGains( void )
{
  // clear adjust-flags:
  ReadMutex.lockForWrite();
  InTraces.delMode( AdjustFlag );
  ReadMutex.unlock();

  if ( ! gainChanged() )
    return 0;

  QWriteLocker locker( &WriteMutex );
  vector< AOData* > aod( 0 );
  return restartRead( aod, false, true );
}


int Acquire::testWrite( OutData &signal )
{
  QReadLocker locker( &WriteMutex );

  signal.clearError();

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

  // error?
  if ( signal.failed() )
    return -1;

  // device still busy?
  if ( AO[di].AO->status() == AnalogOutput::Running && ! signal.priority() )
    signal.addError( DaqError::Busy );

  // set intensity or level:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == di ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      if ( signal.noIntensity() && signal.noLevel() )
	signal.addError( DaqError::NoIntensity );
      else if ( signal.noIntensity() ) {
	double level = signal.level();
	int ra = Att[a].Att->testAttenuate( level );
	signal.setLevel( level );
	signal.addAttError( ra );
      }
      else {
	double intens = signal.intensity();
	int ra = 0;
	if ( intens == OutData::MuteIntensity ) 
	  ra = Att[a].Att->testMute();
	else {
	  double level = 0.0;
	  ra = Att[a].Att->testWrite( intens, signal.carrierFreq(), level );
	  signal.setIntensity( intens );
	  signal.setLevel( level );
	}
	signal.addAttError( ra );
      }
    }
  }

  if ( signal.failed() )
    return -1;

  // write to daq board:
  OutList ol( &signal );  
  AO[di].AO->testWrite( ol );

  // XXX check carrier frequency???

  return signal.success() ? 0 : -1;
}


int Acquire::testWrite( OutList &signal )
{
  QReadLocker locker( &WriteMutex );

  //  cerr << "Acquire::testWrite( OutList& )\n";

  bool success = true;

  signal.clearError();

  if ( signal.size() == 0 ) {
    signal.addError( DaqError::NoData );
    return -1;
  }

  // get device ids and sort signal per device:
  vector< int > dis;
  dis.reserve( signal.size() );
  vector< OutList > dsignals;
  dsignals.reserve( signal.size() );
  for ( int k=0; k<signal.size(); k++ ) {
    int di = -1;
    for ( unsigned int i=0; i<dis.size(); i++ ) {
      if ( signal[k].device() == dis[i] ) {
	di = i;
	break;
      }
    }
    if ( di >= 0 )
      dsignals[di].add( &signal[k] );
    else {
      di = dis.size();
      dis.push_back( signal[k].device() );
      dsignals.push_back( OutList() );
      dsignals[di].add( &signal[k] );
    }
  }

  // no device?
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    if ( dis[i] < 0 ) {
      dsignals[i].addError( DaqError::NoDevice );
      dsignals[i].setDevice( 0 );
      success = false;
    }
    else if ( dis[i] >= (int)AO.size() ) {
      dsignals[i].addError( DaqError::NoDevice );
      dsignals[i].setDevice( AO.size()-1 );
      success = false;
    }
  }

  // error?
  if ( ! success )
    return -1;

  // error?
  if ( ! success )
    return -1;

  // busy?
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    // multiple priorities?
    for ( int k=1; k<dsignals[i].size(); k++ ) {
      if ( dsignals[i][k].priority() != 
	   dsignals[i][0].priority() ) {
	dsignals[i][0].addError( DaqError::MultiplePriorities );
	dsignals[i][k].addError( DaqError::MultiplePriorities );
	dsignals[i][k].setPriority( dsignals[i][0].priority() );
	success = false;
      }
    }
    // device still busy?
    if ( dis[i] >= 0 && AO[dis[i]].AO->status() == AnalogOutput::Running && 
	 ! dsignals[i][0].priority() ) {
      dsignals[i].addError( DaqError::Busy );
      success = false;
    }
  }

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

  // set intensities or levels:
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    for ( int k=0; k<dsignals[i].size(); k++ ) {
      for ( unsigned int a=0; a<Att.size(); a++ ) {
	if ( Att[a].Id == dis[i] &&
	     Att[a].Att->aoChannel() == dsignals[i][k].channel() ) {
	  if ( dsignals[i][k].noIntensity() && dsignals[i][k].noLevel() ) {
	    dsignals[i][k].addError( DaqError::NoIntensity );
	    success = false;
	  }
	  else if ( dsignals[i][k].noIntensity() ) {
	    double level = dsignals[i][k].level();
	    int ra = Att[a].Att->testAttenuate( level );
	    dsignals[i][k].setLevel( level );
	    dsignals[i][k].addAttError( ra );
	  }
	  else {
	    double intens = dsignals[i][k].intensity();
	    int ra = 0;
	    if ( intens == OutData::MuteIntensity )
	      ra = Att[a].Att->testMute();
	    else {
	      double level = 0.0;
	      ra = Att[a].Att->testWrite( intens, dsignals[i][k].carrierFreq(), level );
	      dsignals[i][k].setIntensity( intens );
	      dsignals[i][k].setLevel( level );
	    }
	    if ( ra != 0 ) {
	      dsignals[i][k].addAttError( ra );
	      success = false;
	    }
	  }
	}
      }
    }
  }

  // error?
  if ( ! success )
    return -1;

  // test writing to daq boards:
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    if ( AO[dis[i]].AO->testWrite( dsignals[i] ) != 0 )
      success = false;
  }

  // XXX check carrier frequencies???

  return success ? 0 : -1;
}


int Acquire::write( OutData &signal, bool setsignaltime )
{
  QWriteLocker locker( &WriteMutex );

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

  // error?
  if ( signal.failed() )
    return -1;

  // device still busy?
  if ( AO[di].AO->status() == AnalogOutput::Running ) {
    if ( signal.priority() ) {
      AO[di].AO->stop();
      AO[di].AO->reset();
    }
    else
      signal.addError( DaqError::Busy );
  }

  // error?
  if ( signal.failed() )
    return -1;

  // add signal to device:
  for ( unsigned int i=0; i<AO.size(); i++ )
    AO[i].Signals.clear();
  AO[di].Signals.add( &signal );
  AO[di].AO->clearError();

  // set intensity or level:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == di ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      double level = 0.0;
      double fac = 1.0;
      if ( signal.noIntensity() && signal.noLevel() ) {
	signal.addError( DaqError::NoIntensity );
	break;
      }
      else if ( signal.noIntensity() ) {
	level = signal.level();
	int ra = Att[a].Att->attenuate( level );
	signal.setLevel( level );
	signal.addAttError( ra );
	if ( level == Attenuate::MuteAttenuationLevel )
	  fac = 0.0;
      }
      else {
	double intens = signal.intensity();
	int ra = Att[a].Att->write( intens, signal.carrierFreq(), level );
	signal.setIntensity( intens );
	signal.setLevel( level );
	signal.addAttError( ra );
	if ( intens == OutData::MuteIntensity || level == Attenuate::MuteAttenuationLevel )
	  fac = 0.0;
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
  AO[di].AO->testWrite( AO[di].Signals );

  // error?
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  AO[di].AO->prepareWrite( AO[di].Signals );

  // error?
  if ( signal.failed() ) {
    AO[di].AO->reset();
    AO[di].Signals.clear();
    return -1;
  }

  bool finished = true;

  // clear analog output semaphore:
  if ( AOSemaphore.available() > 0 )
    AOSemaphore.acquire( AOSemaphore.available() );

  // start writing to daq board:
  if ( gainChanged() ||
       signal.restart() ||
       SyncMode == NoSync || SyncMode == StartSync ) {
    if ( SyncMode == AISync )
      cerr << "Acquire::startWrite() -> called restartRead() because of gainChanged="
	   << gainChanged() << " or signal restart=" << signal.restart() << '\n';
    vector< AOData* > aod( 1, &AO[di] );
    if ( restartRead( aod, false, true ) > 0 )
      finished = false;
  }
  else {
    // clear adjust-flags:
    InTraces.delMode( AdjustFlag );
    if ( AO[di].AO->startWrite( &AOSemaphore ) > 0 )
      finished = false;
  }

  // error?
  if ( signal.failed() ) {
    AO[di].AO->stop();
    AO[di].AO->reset();
    AO[di].Signals.clear();
    return -1;
  }

  if ( setsignaltime ) {
    LastDevice = di;
    LastDuration = signal.duration();
    LastDelay = signal.delay();
  }

  return finished ? 0 : 1;
}


int Acquire::write( OutList &signal, bool setsignaltime )
{
  QWriteLocker locker( &WriteMutex );

  bool success = true;
  signal.clearError();

  if ( signal.size() == 0 ) {
    signal.addError( DaqError::NoData );
    return -1;
  }

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
	if ( signal[k0].priority() ) {
	  AO[i].AO->stop();
	  AO[i].AO->reset();
	}
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

  // error?
  if ( signal.failed() )
    return -1;

  // add signals to devices:
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    AO[i].Signals.clear();
    AO[i].AO->clearError();
  }
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
	    break;
	  }
	  else if ( AO[i].Signals[k].noIntensity() ) {
	    level = AO[i].Signals[k].level();
	    int ra = Att[a].Att->attenuate( level );
	    AO[i].Signals[k].setLevel( level );
	    if ( ra != 0 ) {
	      AO[i].Signals[k].addAttError( ra );
	      success = false;
	    }
	    if ( level == Attenuate::MuteAttenuationLevel )
	      fac = 0.0;
	  }
	  else {
	    double intens = AO[i].Signals[k].intensity();
	    int ra = Att[a].Att->write( intens, AO[i].Signals[k].carrierFreq(), level );
	    AO[i].Signals[k].setIntensity( intens );
	    AO[i].Signals[k].setLevel( level );
	    if ( ra != 0 ) {
	      AO[i].Signals[k].addAttError( ra );
	      success = false;
	    }
	    if ( intens == OutData::MuteIntensity || level == Attenuate::MuteAttenuationLevel )
	      fac = 0.0;
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

  bool finished = true;

  // clear analog output semaphore:
  if ( AOSemaphore.available() > 0 )
    AOSemaphore.acquire( AOSemaphore.available() );

  // start writing to daq boards:
  if ( gainChanged() ||
       signal[0].restart() ||
       SyncMode == NoSync || SyncMode == StartSync ) {
    if ( SyncMode == AISync )
      cerr << "Acquire::startWrite() -> called restartRead() because of gainChanged="
	   << gainChanged() << " or signal restart=" << signal[0].restart() << '\n';
    vector< AOData* > aod;
    aod.reserve( AO.size() );
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( AO[i].Signals.size() > 0 )
	aod.push_back( &AO[i] );
    }
    int r = restartRead( aod, false, true );
    if ( r < 0 )
      success = false;
    else if ( r > 0 )
      finished = false;
  }
  else {
    // clear adjust-flags:
    InTraces.delMode( AdjustFlag );
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( AO[i].Signals.size() > 0 ) {
	int r = AO[i].AO->startWrite( &AOSemaphore );
	if ( r < 0 )
	  success = false;
	else if ( r > 0 )
	  finished = false;
      }
    }
  }
  
  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->stop();
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    return -1;
  }

  if ( setsignaltime ) {
    LastDevice = signal[0].device();
    LastDuration = signal[0].duration();
    LastDelay = signal[0].delay();
  }

  return finished ? 0 : 1;
}


int Acquire::waitForWrite( void )
{
  int naos = 0;
  {
    QReadLocker locker( &WriteMutex );
    // number of analog output devices used for writing:
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( ! AO[i].Signals.empty() && AO[i].AO->running() )
	naos++;
    }
  }
  if ( naos == 0 )
    return 0;

  // wait for the threads to finish:
  AOSemaphore.acquire( naos );

  AOSemaphore.acquire( AOSemaphore.available() );

  WriteMutex.lockForRead();
  bool success = true;
  // check:
  for ( unsigned int i = 0; i<AO.size(); i++ ) {
    if ( ! AO[i].Signals.empty() && 
	 ( AO[i].AO->failed() || AO[i].Signals.failed() ) )
      success = false;
  }
  WriteMutex.unlock();

  if ( ! success ) {
    // error:
    stopWrite();
    return -1;
  }

  return 0;
}


bool Acquire::isWriteRunning( void ) const
{
  QReadLocker locker( &WriteMutex );
  bool isrunning = true;
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    if ( ! AO[i].Signals.empty() && ! AO[i].AO->running() ) {
      isrunning = false;
      break;
    }
  }
  return isrunning;
}


void Acquire::lockWrite( void )
{
  WriteMutex.lockForWrite();
}


void Acquire::unlockWrite( void )
{
  WriteMutex.unlock();
}


int Acquire::directWrite( OutData &signal, bool setsignaltime )
{
  QWriteLocker locker( &WriteMutex );

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

  // error?
  if ( signal.failed() )
    return -1;

  // device still busy?
  if ( AO[di].AO->status() == AnalogOutput::Running ) {
    if ( signal.priority() ) {
      AO[di].AO->stop();
      AO[di].AO->reset();
    }
    else
      signal.addError( DaqError::Busy );
  }

  // error?
  if ( signal.failed() )
    return -1;

  // add signal to device:
  for ( unsigned int i=0; i<AO.size(); i++ )
    AO[i].Signals.clear();
  AO[di].Signals.add( &signal );
  AO[di].AO->clearError();

  // set intensity or level:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == di ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      double level = 0.0;
      double fac = 1.0;
      if ( signal.noIntensity() && signal.noLevel() ) {
	signal.addError( DaqError::NoIntensity );
	break;
      }
      else if ( signal.noIntensity() ) {
	level = signal.level();
	int ra = Att[a].Att->attenuate( level );
	signal.setLevel( level );
	signal.addAttError( ra );
	if ( level == Attenuate::MuteAttenuationLevel )
	  fac = 0.0;
      }
      else {
	double intens = signal.intensity();
	int ra = Att[a].Att->write( intens, signal.carrierFreq(), level );
	signal.setIntensity( intens );
	signal.setLevel( level );
	signal.addAttError( ra );
	if ( intens == OutData::MuteIntensity || level == Attenuate::MuteAttenuationLevel )
	  fac = 0.0;
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

  // error?
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  // start writing to daq board:
  if ( gainChanged() ||
       signal.restart() ||
       SyncMode == NoSync || SyncMode == StartSync ) {
    if ( SyncMode == AISync )
      cerr << "Acquire::directWrite() -> called restartRead() because of gainChanged="
	   << gainChanged() << " or signal restart=" << signal.restart() << '\n';
    vector< AOData* > aod( 1, &AO[di] );
    restartRead( aod, true, true );
  }
  else {
    // clear adjust-flags:
    InTraces.delMode( AdjustFlag );
    AO[di].AO->directWrite( AO[di].Signals );
  }

  // error?
  if ( signal.failed() ) {
    AO[di].AO->stop();
    AO[di].AO->reset();
    AO[di].Signals.clear();
    return -1;
  }

  if ( setsignaltime ) {
    LastDevice = di;
    LastDuration = signal.duration();
    LastDelay = signal.delay();
  }

  //  cerr << "Acquire::write( OutData& ) end\n";

  return 0;
}


int Acquire::directWrite( OutList &signal, bool setsignaltime )
{
  QWriteLocker locker( &WriteMutex );

  if ( signal.size() == 0 ) {
    signal.addError( DaqError::NoData );
    return -1;
  }

  bool success = true;
  signal.clearError();

  // set trace:
  applyOutTrace( signal );

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
	if ( signal[k0].priority() ) {
	  AO[i].AO->stop();
	  AO[i].AO->reset();
	}
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

  // error?
  if ( signal.failed() )
    return -1;

  // add signals to devices:
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    AO[i].Signals.clear();
    AO[i].AO->clearError();
  }
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
	    break;
	  }
	  else if ( AO[i].Signals[k].noIntensity() ) {
	    level = AO[i].Signals[k].level();
	    int ra = Att[a].Att->attenuate( level );
	    AO[i].Signals[k].setLevel( level );
	    if ( ra != 0 ) {
	      AO[i].Signals[k].addAttError( ra );
	      success = false;
	    }
	    if ( level == Attenuate::MuteAttenuationLevel )
	      fac = 0.0;
	  }
	  else {
	    double intens = AO[i].Signals[k].intensity();
	    int ra = Att[a].Att->write( intens, AO[i].Signals[k].carrierFreq(), level );
	    AO[i].Signals[k].setIntensity( intens );
	    AO[i].Signals[k].setLevel( level );
	    if ( ra != 0 ) {
	      AO[i].Signals[k].addAttError( ra );
	      success = false;
	    }
	    if ( intens == OutData::MuteIntensity || level == Attenuate::MuteAttenuationLevel )
	      fac = 0.0;
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

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ )
      AO[i].Signals.clear();
    return -1;
  }

  // start writing to daq boards:
  if ( gainChanged() ||
       signal[0].restart() ||
       SyncMode == NoSync || SyncMode == StartSync ) {
    if ( SyncMode == AISync )
      cerr << "Acquire::directWrite() -> called restartRead() because of gainChanged="
	   << gainChanged() << " or signal restart=" << signal[0].restart() << '\n';
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
    // clear adjust-flags:
    InTraces.delMode( AdjustFlag );
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( AO[i].Signals.size() > 0 ) {
	if ( AO[i].AO->directWrite( AO[i].Signals ) != 0 )
	  success = false;
      }
    }
  }
  
  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      AO[i].AO->stop();
      AO[i].AO->reset();
      AO[i].Signals.clear();
    }
    return -1;
  }

  if ( setsignaltime ) {
    LastDevice = signal[0].device();
    LastDuration = signal[0].duration();
    LastDelay = signal[0].delay();
  }

  return 0;
}


int Acquire::writeZero( int channel, int device )
{
  QWriteLocker locker( &WriteMutex );

  // check ao device:
  if ( device < 0 || device >= (int)AO.size() )
    return -1;

  // device still busy?
  if ( AO[device].AO->status() == AnalogOutput::Running ) {
    AO[device].AO->stop();
    AO[device].AO->reset();
  }

  OutData signal( 1, 0.0001 );
  signal.setChannel( channel, device );
  signal[0] = 0.0;
  OutList sigs( signal );

  // write to daq board:
  AO[device].AO->directWrite( sigs );

  // error?
  if ( ! signal.success() ) {
    cerr << currentTime() << " ! error in Acquire::writeZero( int, int ) -> "
	 << signal.errorText() << "\n";
    return -1;
  }


  return 0;
}


int Acquire::writeZero( int index )
{
  if ( index < 0 || index >= outTracesSize() )
    return -1;
  else
    return writeZero( OutTraces[index].channel(),
		      OutTraces[index].device() );
}


int Acquire::writeZero( const string &trace )
{
  int index = outTraceIndex( trace );
  return writeZero( index );
}


double Acquire::signalTime( void ) const
{
  QReadLocker locker( &WriteMutex );
  return SignalTime;
}


string Acquire::writeError( void ) const
{
  QReadLocker locker( &WriteMutex );
  string es = "";
  for ( unsigned int i = 0; i<AO.size(); i++ ) {
    if ( AO[i].AO->failed() ) {
      string aoes = AO[i].AO->errorStr();
      if ( ! aoes.empty() ) {
	if ( ! es.empty() )
	  es += ", ";
	es += aoes;
      }
    }
  }
  return es;
}


int Acquire::stopWrite( void )
{
  QReadLocker locker( &WriteMutex );
  bool success = true;

  for ( unsigned int i = 0; i<AO.size(); i++ ) {
    if ( AO[i].AO->stop() != 0 )
      success = false;
    if ( AO[i].AO->reset() != 0 )
      success = false;
  }
  // wake up all jobs sleeping on AOSemaphore:
  AOSemaphore.release( 1000 );

  return success ? 0 : -1;
}


double Acquire::minLevel( int trace ) const
{
  QReadLocker locker( &WriteMutex );
  if ( trace < 0 || trace > outTracesSize() )
    return OutData::NoLevel;

  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == OutTraces[trace].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[trace].channel() ) ) {
      if ( Att[a].Att->noAttenuator() )
	return -200.0;
      else
	return Att[a].Att->attenuator()->minLevel();
    }
  }

  return OutData::NoLevel;
}


double Acquire::minLevel( const string &trace ) const
{
  return minLevel( outTraceIndex( trace ) );
}


double Acquire::maxLevel( int trace ) const
{
  QReadLocker locker( &WriteMutex );
  if ( trace < 0 || trace > outTracesSize() )
    return OutData::NoLevel;

  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == OutTraces[trace].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[trace].channel() ) ) {
      if ( Att[a].Att->noAttenuator() )
	return 100.0;
      else
	return Att[a].Att->attenuator()->maxLevel();
    }
  }

  return OutData::NoLevel;
}


double Acquire::maxLevel( const string &trace ) const
{
  return maxLevel( outTraceIndex( trace ) );
}


void Acquire::levels( int trace, vector<double> &l ) const
{
  QReadLocker locker( &WriteMutex );
  l.clear();
  if ( trace < 0 || trace > outTracesSize() )
    return;

  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == OutTraces[trace].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[trace].channel() ) ) {
      if ( Att[a].Att->noAttenuator() ) {
	l.reserve( 600 );
	for ( int k=0; k<600; k++ )
	  l.push_back( -100.0 + k*0.5 );
      }
      else
	Att[a].Att->attenuator()->levels( l );
      break;
    }
  }
}


void Acquire::levels( const string &trace, vector<double> &l ) const
{
  levels( outTraceIndex( trace ), l );
}


double Acquire::minIntensity( int trace, double frequency ) const
{
  QReadLocker locker( &WriteMutex );
  if ( trace < 0 || trace > outTracesSize() )
    return OutData::NoIntensity;

  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == OutTraces[trace].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[trace].channel() ) ) {
      return Att[a].Att->minIntensity( frequency );
    }
  }

  return OutData::NoIntensity;
}


double Acquire::minIntensity( const string &trace, double frequency ) const
{
  return minIntensity( outTraceIndex( trace ), frequency );
}


double Acquire::maxIntensity( int trace, double frequency ) const
{
  QReadLocker locker( &WriteMutex );
  if ( trace < 0 || trace > outTracesSize() )
    return OutData::NoIntensity;

  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == OutTraces[trace].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[trace].channel() ) ) {
      return Att[a].Att->maxIntensity( frequency );
    }
  }

  return OutData::NoIntensity;
}


double Acquire::maxIntensity( const string &trace, double frequency ) const
{
  return maxIntensity( outTraceIndex( trace ), frequency );
}


void Acquire::intensities( int trace, vector<double> &ints, double frequency ) const
{
  QReadLocker locker( &WriteMutex );
  ints.clear();
  if ( trace < 0 || trace > outTracesSize() )
    return;

  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == OutTraces[trace].device() ) && 
	 ( Att[a].Att->aoChannel() == OutTraces[trace].channel() ) ) {
      Att[a].Att->intensities( ints, frequency );
      break;
    }
  }
}


void Acquire::intensities( const string &trace, vector<double> &ints, double frequency ) const
{
  intensities( outTraceIndex( trace ), ints, frequency );
}


double Acquire::getSignal( void )
{
  // ReadMutex is already locked.

  double signaltime = -1.0;

  if ( SyncMode == AISync ) {
    if ( LastDevice < 0 )
      return -1.0;
    // get signal time:
    long inx = AO[LastDevice].AO->index();
    if ( inx < 0 )
      return -1.0;
    int d = AO[LastDevice].AISyncDevice;
    if ( d < 0 || AI[d].Traces.empty() )
      return -1.0;
    double prevsignaltime = AI[d].Traces[0].signalTime();
    AI[d].Traces[0].setSignalIndex( AI[d].Traces[0].restartIndex() + inx );
    signaltime = AI[d].Traces[0].signalTime();
    if ( prevsignaltime >= 0.0 &&
	 signaltime <= prevsignaltime ) // got no new signal start yet
      return -1.0;
  }
  else {
    if ( LastWrite < 0.0 )
      return -1.0;
    signaltime = LastWrite + LastDelay;
  }

  if ( SignalEvents != 0 )
    SignalEvents->push( signaltime, 0.0, LastDuration );

  LastDevice = -1;
  LastWrite = -1.0;

  return signaltime;
}


void Acquire::stop( void )
{
  stopRead();
  stopWrite();
}


string Acquire::currentTime( void )
{
  char s[200];
  time_t ct = time( NULL );
  strftime( s, 200, "%H:%M:%S", localtime( &ct ) );
  return s;
}


}; /* namespace relacs */

