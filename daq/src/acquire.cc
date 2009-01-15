/*
  acquire.cc
  Read and write data from/to the data aqcuisition board.

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

#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <relacs/acquire.h>

namespace relacs {


const string Acquire::SyncModeStrs[5] = {
  "simple restart of analog input together with analog output",
  "quick restart of analog input together with analog output",
  "analog input and analog output are started by common trigger signal",
  "continuous analog input, counter device is used for time-stamping analog output",
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

  SyncMode = NoSync;

  BufferTime = 0.01;
  UpdateTime = 0.1;

  Att.clear();
}


Acquire::~Acquire()
{
  clear();
}


int Acquire::addInput( AnalogInput *ai, int defaulttype )
{
  if ( ai == 0 )
    return -1;

  if ( ! ai->isOpen() )
    return -2;

  if ( defaulttype < 0 )
    return -3;

  AI.push_back( AIData( ai, defaulttype ) );

  return 0;
}


int Acquire::inputsSize( void ) const
{ 
  return AI.size();
}


const AnalogInput *Acquire::inputDevice( int deviceindex ) const
{
  if ( deviceindex < 0 || deviceindex >= (int)AI.size() )
    return 0;

  return AI[deviceindex].AI;
}


const InList &Acquire::inputTraces( int deviceindex ) const
{
  if ( deviceindex < 0 )
    deviceindex = 0;
  if ( deviceindex >= (int)AI.size() )
    deviceindex = AI.size()-1;

  return AI[deviceindex].Traces;
}


void Acquire::clearInputs( void )
{
  stopRead();
  AI.clear();
}


void Acquire::closeInputs( void )
{
  stopRead();
  for ( unsigned int k=0; k < AI.size(); k++ ) {
    if ( AI[k].AI->isOpen() )
      AI[k].AI->close();
    AI[k].Traces.clear();
    AI[k].Gains.clear();
  }
  AI.clear();
}


int Acquire::addOutput( AnalogOutput *ao )
{
  if ( ao == 0 )
    return -1;

  if ( ! ao->isOpen() )
    return -2;

  AO.push_back( AOData( ao ) );

  return 0;
}


int Acquire::outputsSize( void ) const
{
  return AO.size();
}


int Acquire::outputIndex( const string &ident ) const
{
  for ( unsigned int k=0; k < AO.size(); k++ ) {
    if ( AO[k].AO->deviceIdent() == ident )
      return k;
  }
  return -1;
}


void Acquire::clearOutputs( void )
{
  stopWrite();
  AO.clear();
}


void Acquire::closeOutputs( void )
{
  stopWrite();
  for ( unsigned int k=0; k < AO.size(); k++ ) {
    if ( AO[k].AO->isOpen() )
      AO[k].AO->close();
    AO[k].Signals.clear();
  }
  AO.clear();
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
  int id = -1;
  for ( unsigned int k=0; k<AO.size(); k++ ) {
    if ( AO[k].AO->deviceIdent() == att->aoDevice() ) {
      id = k;
      break;
    }
  }

  Att.push_back( AttData( att, id ) );

  return 0;
}


int Acquire::attLinesSize( void ) const
{
  return Att.size();
}


void Acquire::clearAttLines( void )
{
  Att.clear();
}


void Acquire::closeAttLines( void )
{
  for ( unsigned int k=0; k < Att.size(); k++ )
    if ( Att[k].Att->isOpen() )
      Att[k].Att->close();
  Att.clear();
}


void Acquire::addOutTrace( const string &name,
			   int device, int channel, 
			   double scale, const string &unit,
			   bool reglitch, double maxrate, double signaldelay )
{
  OutTraces.push_back( TraceSpec( OutTraces.size(), name, device, channel,
				  scale, unit,
				  reglitch, maxrate, signaldelay ) );
}


void Acquire::addOutTraces( void )
{
  for ( unsigned int k=0; k < AO.size(); k++ ) {
    AO[k].AO->addTraces( OutTraces, k );
  }
  for ( unsigned int i=0; i<OutTraces.size(); i++ ) {
    cerr << "OUTTRACE " << i << " name " << OutTraces[i].traceName()
	 << " device=" << OutTraces[i].device()
	 << " channel=" << OutTraces[i].channel() << endl;
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
  if ( signal.trace() < 0 && signal.traceName().empty() )
    return 0;

  int inx = -1;
  if ( ! signal.traceName().empty() )
    inx = outTraceIndex( signal.traceName() );
  else
    inx = signal.trace();
  if ( inx < 0 ) {
    signal.addError( DaqError::InvalidTrace );
    return -1;
  }

  return OutTraces[inx].apply( signal );  
}


int Acquire::applyOutTrace( OutList &signal ) const
{
  int r = 0;
  for ( int k=0; k<signal.size(); k++ ) {
    int rr = applyOutTrace( signal[k] );
    if ( rr < 0 )
      r = rr;
  }
  return r;
}


void Acquire::clearOutTraces( void )
{
  OutTraces.clear();
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
  AI[0].AI->addTraces( traces, 0 );
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


void Acquire::init( void )
{
  SyncMode = NoSync;

  // create lists of analog input and output devices:
  vector< AnalogInput* > ais;
  ais.reserve( AI.size() );
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    ais.push_back( AI[i].AI );
    AI[i].AIDevice = -1;
  }
  vector< AnalogOutput* > aos;
  aos.reserve( AO.size() );
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    aos.push_back( AO[i].AO );
    AO[i].AISyncDevice = -1;
    AO[i].AIDevice = -1;
    AO[i].AODevice = -1;
  }
  
  // can analog output be synchronized with running analog input?
  SyncMode = AISync;
  for ( unsigned int i=0; i<aos.size(); i++ ) {
    AO[i].AISyncDevice = aos[i]->getAISyncDevice( ais );
    if ( AO[i].AISyncDevice < 0 )
      SyncMode = NoSync;
  }

  // lists of taken device indices:
  vector< int > aiinx;
  aiinx.reserve( AI.size() );
  vector< int > aoinx;
  aoinx.reserve( AO.size() );
  
  // init combined start of other devices:
  for ( unsigned int i=0; i<ais.size(); i++ ) {
    aiinx.clear();
    aoinx.clear();
    ais[i]->take( ais, aos, aiinx, aoinx );
    for ( unsigned int j=0; j<aiinx.size(); j++ ) {
      if ( AI[aiinx[j]].AIDevice < 0 )
	AI[aiinx[j]].AIDevice = i;
    }
    for ( unsigned int j=0; j<aoinx.size(); j++ ) {
      if ( AO[aoinx[j]].AIDevice < 0 )
	AO[aoinx[j]].AIDevice = i;
    }
  }
  for ( unsigned int i=0; i<aos.size(); i++ ) {
    aoinx.clear();
    aos[i]->take( aos, aoinx );
    for ( unsigned int j=0; j<aoinx.size(); j++ ) {
      if ( AO[aoinx[j]].AODevice < 0 )
	AO[aoinx[j]].AODevice = i;
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


double Acquire::bufferTime( void ) const
{
  return BufferTime;
}


void Acquire::setBufferTime( double time )
{
  BufferTime = time;
}


double Acquire::updateTime( void ) const
{
  return UpdateTime;
}


void Acquire::setUpdateTime( double time )
{
  UpdateTime = time;
}


int Acquire::testRead( InList &data )
{
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
      data[k].addError( DaqError::NoDevice );
      data[k].setDevice( 0 );
      success = false;
    }
    else if ( data[k].device() >= (int)AI.size() ) {
      data[k].addError( DaqError::NoDevice );
      data[k].setDevice( AO.size()-1 );
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

  // request buffer sizes:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    AI[i].Traces.setReadTime( BufferTime );
    AI[i].Traces.setUpdateTime( UpdateTime );
  }

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

  bool success = true;

  // clear error flags:
  data.clearError();

  // clear device datas:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    AI[i].Traces.clear();
    AI[i].Gains.clear();
  }

  // sort data to devices:
  for ( int k=0; k<data.size(); k++ ) {
    // no device?
    if ( data[k].device() < 0 ) {
      data[k].addError( DaqError::NoDevice );
      data[k].setDevice( 0 );
      success = false;
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

  // check model traces:
  if ( AI.size() > 0 && AI[0].AI->matchTraces( data ) < 0 ) {
    data.addErrorStr( "unable to match model input traces" );
    success = false;
  }
  if ( AO.size() > 0 && AO[0].AO->matchTraces( OutTraces ) < 0 ) {
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
	AI[i].AI->reset();
      else {
	AI[i].Traces.addError( DaqError::Busy );
	success = false;
      }
    }
  }

  // error?
  if ( ! success )
    return -1;

  // request buffer size:
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
  if ( ! success ) {
    for ( unsigned int i=0; i<AI.size(); i++ )
      AI[i].AI->reset();
    return -1;
  }

  // prepare dynamic clamp output:
  // XXXX OutList sigs = OutTraces; All potential output channels should be initialized!
  //AO[]->prepareWrite( sigs );

  // start reading from daq boards:
  vector< int > aistarted;
  aistarted.reserve( AI.size() );
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
	if ( AI[i].AI->startRead() != 0 )
	  success = false;
	else
	  aistarted.push_back( i );
      }
    }
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AI.size(); i++ )
      AI[i].AI->reset();
    return -1;
  }


  // start dynamic clamp output:
  //AO->startWrite();

  LastDevice = -1;
  LastWrite = -1.0;

  SoftReset = false;
  HardReset = false;

  //  cerr << "Acquire::read( InList& ) finished\n";
  return 0;
}


int Acquire::readData( void )
{
  //  cerr << "Acquire::readData( void )\n";

  bool error = false;
  bool finished = true;

  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 ) {

      AI[i].Traces.clearError();

      // get error from previous data input:
      int aie = AI[i].AI->error();
      if ( aie != 0 ) {
	AI[i].Traces.addDaqError( aie );
	error = true;
      }

      // read data from daq boards:
      if ( AI[i].AI->readData() < 0 ) {
	if ( AI[i].Traces.failed() )
	  error = true;
      }
      else
	finished = false;
 
    }
  }

  // restart:    
  if ( error ) {
    vector< AOData* > aod( 0 );
    restartRead( aod, false );
    return -1;
  }

  //  cerr << "Acquire::readData( void ) finished\n";
  return finished ? 0 : 1;
}


int Acquire::convertData( void )
{
  //  cerr << "Acquire::convertData( void )\n";

  bool success = true;

  for ( unsigned int i=0; i<AI.size(); i++ ) {
    if ( AI[i].Traces.size() > 0 ) {
      int r = AI[i].AI->convertData();
      if ( r < 0 )
	success = false;
     }
  }

  return success ? 0 : -1;
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

  return success ? 0 : -1;
}


int Acquire::restartRead( vector< AOData* > &aod, bool updategains )
{
  //  cerr << currentTime() << " Acquire::restartRead() begin \n";

  bool success = true;

  for ( unsigned int i=0; i<AI.size(); i++ ) {
    int n = AI[i].AI->readData();
    if ( n < 0 && AI[i].Traces.failed() )
      success = false;
  }

  // stop reading:
  if ( stopRead() != 0 )
    success = false;

  // get data and shortest recording:
  double t = -1.0;
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    int n = AI[i].AI->readData();
    if ( n < 0 && AI[i].Traces.failed() )
      success = false;
    AI[i].AI->convertData();
    for ( int k=0; k<AI[i].Traces.size(); k++ ) {
      if ( t < 0.0 || AI[i].Traces[k].length() < t )
	t = AI[i].Traces[k].length();
    }
  }

  // make all data the same length and set restart time:
  for ( unsigned int i=0; i<AI.size(); i++ ) {
    int m = 0;
    for ( int k=0; k<AI[i].Traces.size(); k++ ) {
      int n = AI[i].Traces[k].indices( t );
      m += AI[i].Traces[k].size() - n;
      AI[i].Traces[k].resize( n );
    }
    cerr << "truncated " << m << '\n';
    AI[i].Traces.setRestart();
  }

  // set signal start:
  if ( aod.size() > 0 && t >= 0.0 )
    LastWrite = t;

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
	   AI[i].AI->prepareRead( AI[i].Traces ) != 0 )
	success = false;
    }
  }

  if ( ! success )
    return -1;

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

  // start reading from daq boards:
  vector< int > aistarted;
  aistarted.reserve( AI.size() );
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
	if ( AI[i].AI->startRead() != 0 )
	  success = false;
	else
	  aistarted.push_back( i );
      }
    }
  }
    
  if ( ! success )
    return -1;
    
  // start writing signals:
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
      if ( aod[i]->AO->startWrite() != 0 )
	success = false;
      else
	aostarted.push_back( aoinx[ i ] );
    }
  }

  //  cerr << currentTime() << " Acquire::restartRead() -> acquisition restarted " << success << "\n";

  return success ? 0 : -1;
}


int Acquire::adjustFlag( void ) const
{
  return AdjustFlag;
}


void Acquire::setAdjustFlag( int flag )
{
  AdjustFlag = flag;
}


int Acquire::setGain( const InData &data, int gainindex )
{
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
  int maxindex = -1;
  int newindex = -1;
  for ( int k = ai.AI->maxRanges() - 1; k > 0; k-- ) {
    // this gain exist?
    if ( ( data.unipolar() && ai.AI->unipolarRange( k ) > 0 ) || 
	 ( !data.unipolar() && ai.AI->bipolarRange( k ) > 0 ) ) {
      if ( maxindex < 0 )
	maxindex = k;
      // check it:
      if ( data.unipolar() ) {
	if ( ai.AI->unipolarRange( k ) * data.scale() >= maxvalue ) {
	  newindex = k;
	  break;
	}
      }
      else {
	if ( ai.AI->bipolarRange( k ) * data.scale() >= maxvalue ) {
	  newindex = k;
	  break;
	}
      }
    }
  }

  // no gain:
  if ( newindex < 0 )
    newindex = maxindex;
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
	    ( data.unipolar() && ai.AI->unipolarRange( gainindex ) <= 0.0 ) || 
	    ( !data.unipolar() && ai.AI->bipolarRange( gainindex ) <= 0.0 ); 
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
  if ( ! gainChanged() )
    return 0;

  vector< AOData* > aod( 0 );
  return restartRead( aod, true );
}


int Acquire::testWrite( OutData &signal )
{
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

  // get error from previous signal output:
  if ( AO[di].Signals.size() == 1 &&
       &AO[di].Signals[0] == &signal ) {
    int aoe = AO[di].AO->error();
    if ( aoe != 0 ) {
      signal.addDaqError( aoe );
      return -1;
    }
  }

  // device still busy?
  if ( AO[di].AO->running() && ! signal.priority() )
    signal.addError( DaqError::Busy );

  // set intensity:
  for ( unsigned int a=0; a<Att.size(); a++ ) {
    if ( ( Att[a].Id == di ) && 
	 ( Att[a].Att->aoChannel() == signal.channel() ) ) {
      if ( signal.noIntensity() )
	signal.addError( DaqError::NoIntensity );
      else {
	double intens = signal.intensity();
	int ra = 0;
	if ( intens == OutData::MuteIntensity ) 
	  ra = Att[a].Att->testMute();
	else {
	  ra = Att[a].Att->testWrite( intens, signal.carrierFreq() );
	  signal.setIntensity( intens );
	}
	signal.addAttError( ra );
      }
    }
  }

  // request buffer size:
  signal.setWriteTime( BufferTime );

  // write to daq board:
  OutList ol( &signal );  
  AO[di].AO->testWrite( ol );

  // XXX check carrier frequency???

  return signal.success() ? 0 : -1;
}


int Acquire::testWrite( OutList &signal )
{
  //  cerr << "Acquire::testWrite( OutList& )\n";

  bool success = true;

  signal.clearError();

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
      dsignals[di].reserve( signal.size() - k );
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

  // get error from previous signal output:
  bool same = true;
  for ( unsigned int i=0; i<dis.size() && same; i++ ) {
    if ( AO[dis[i]].Signals.size() != dsignals[i].size() ) {
      same = false;
      break;
    }
    for ( int k=0; k<dsignals[i].size(); k++ ) {
      if ( &AO[dis[i]].Signals[k] != &dsignals[i][k] ) {
	same = false;
	break;
      }
    }
  }
  if ( same ) {
    for ( unsigned int i=0; i<dis.size(); i++ ) {
      int aoe = AO[dis[i]].AO->error();
      if ( aoe != 0 ) {
	dsignals[i].addDaqError( aoe );
	success = false;
      }
    }
  }

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
    if ( dis[i] >= 0 && AO[dis[i]].AO->running() && 
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

  // set intensities:
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    for ( int k=0; k<dsignals[i].size(); k++ ) {
      for ( unsigned int a=0; a<Att.size(); a++ ) {
	if ( Att[a].Id == dis[i] &&
	     Att[a].Att->aoChannel() == dsignals[i][k].channel() ) {
	  if ( dsignals[i][k].noIntensity() ) {
	    dsignals[i][k].addError( DaqError::NoIntensity );
	    success = false;
	  }
	  else {
	    double intens = dsignals[i][k].intensity();
	    int ra = 0;
	    if ( intens == OutData::MuteIntensity )
	      ra = Att[a].Att->testMute();
	    else {
	      ra = Att[a].Att->testWrite( intens, dsignals[i][k].carrierFreq() );
	      dsignals[i][k].setIntensity( intens );
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

  // request buffer size:
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    AO[i].Signals.setWriteTime( BufferTime );
  }

  // test writing to daq boards:
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    if ( AO[dis[i]].AO->testWrite( dsignals[i] ) != 0 )
      success = false;
  }

  // XXX check carrier frequencies???

  return success ? 0 : -1;
}


int Acquire::convert( OutData &signal )
{
  //  cerr << "Acquire::convert( OutData& ) -> " << signal.ident() << '\n';

  signal.clearError();

  // get ao device:
  int di = signal.device();
  if ( di < 0 ) {
    signal.addError( DaqError::NoDevice );
    return -1;
  }
  else if ( di >= (int)AO.size() ) {
    signal.addError( DaqError::NoDevice );
    return -1;
  }

  // do conversion:
  signal.setManualConvert();
  OutList ol( &signal );
  return AO[di].AO->convertData( ol );
}


int Acquire::convert( OutList &signal )
{
  //  cerr << "Acquire::convert( OutList& )\n";

  bool success = true;

  signal.clearError();

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
      dsignals[di].reserve( signal.size() - k );
      dsignals[di].add( &signal[k] );
    }
  }

  // no device?
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    if ( dis[i] < 0 ) {
      dsignals[i].addError( DaqError::NoDevice );
      success = false;
    }
    else if ( dis[i] >= (int)AO.size() ) {
      dsignals[i].addError( DaqError::NoDevice );
      success = false;
    }
  }

  if ( ! success )
    return -1;

  // do conversion:
  for ( unsigned int i=0; i<dis.size(); i++ ) {
    int r = AO[dis[i]].AO->convertData( dsignals[i] );    
    if ( r < 0 )
      success = false;
    dsignals[i].setManualConvert();
  }

  return success ? 0 : -1;
}


int Acquire::write( OutData &signal )
{
  //  cerr << "Acquire::write( OutData& ) -> " << signal.ident() << '\n';

  // set trace:
  applyOutTrace( signal );

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
      if ( signal.noIntensity() )
	signal.addError( DaqError::NoIntensity );
      else {
	double intens = signal.intensity();
	int ra = 0;
	if ( intens == OutData::MuteIntensity )
	  ra = Att[a].Att->mute();
	else {
	  ra = Att[a].Att->write( intens, signal.carrierFreq() );
	  signal.setIntensity( intens );
	}
	signal.addAttError( ra );
      }
    }
    else
      Att[a].Att->mute();
  }

  // request buffer size:
  signal.setWriteTime( BufferTime );

  // test writing to daq board:
  Signal.clear();
  Signal.add( &signal );
  AO[di].AO->testWrite( Signal );

  // error?
  if ( signal.failed() ) {
    AO[di].Signals.clear();
    return -1;
  }

  // convert data and prepare writing to daq board:
  if ( signal.autoConvert() || signal.deviceBuffer() == NULL ) {
    AO[di].AO->convertData( Signal );
  }
  AO[di].Signals[0].deviceBufferReset();

  AO[di].AO->prepareWrite( Signal );

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
    vector< AOData* > aod( 1, &AO[di] );
    restartRead( aod, true );
  }
  else
    AO[di].AO->startWrite();

  // error?
  if ( signal.failed() ) {
    AO[di].AO->reset();
    AO[di].Signals.clear();
    return -1;
  }

  LastDevice = di;
  //  LastWrite = 0.0; this is set in restartRead
  LastDuration = signal.duration();
  LastDelay = signal.delay();

  //  cerr << "Acquire::write( OutData& ) end\n";

  return 0;
}


int Acquire::write( OutList &signal )
{
  //  cerr << "Acquire::write( OutList& )\n";

  bool success = true;

  // set trace:
  applyOutTrace( signal );

  signal.clearError();

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
  }

  // error?
  if ( ! success ) {
    for ( unsigned int i=0; i<AO.size(); i++ )
      AO[i].Signals.clear();
    return -1;
  }

  // set intensities:
  bool usedatt[Att.size()];
  for ( unsigned int a=0; a<Att.size(); a++ )
    usedatt[a] = false;
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    for ( int k=0; k<AO[i].Signals.size(); k++ ) {
      for ( unsigned int a=0; a<Att.size(); a++ ) {
	if ( Att[a].Id == (int)i &&
	     Att[a].Att->aoChannel() == AO[i].Signals[k].channel() ) {
	  usedatt[a] = true;
	  if ( AO[i].Signals[k].noIntensity() ) {
	    AO[i].Signals[k].addError( DaqError::NoIntensity );
	    success = false;
	  }
	  else {
	    double intens = AO[i].Signals[k].intensity();
	    int ra = 0;
	    if ( intens == OutData::MuteIntensity )
	      ra = Att[a].Att->mute();
	    else {
	      ra = Att[a].Att->write( intens, AO[i].Signals[k].carrierFreq() );
	      AO[i].Signals[k].setIntensity( intens );
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

  // request buffer size:
  for ( unsigned int i=0; i<AO.size(); i++ )
    AO[i].Signals.setWriteTime( BufferTime );

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

  // convert data:
  bool ac = signal[0].autoConvert();
  // for manual convert there must be at least one non empty buffer per device:
  for ( unsigned int i=0; !ac && i<AO.size(); i++ ) {
    if ( AO[i].Signals.size() > 0 ) {
      bool e = true;
      for ( int k=0; k<AO[i].Signals.size(); k++ ) { 
	if ( AO[i].Signals[k].deviceBuffer() != NULL ) {
	  e = false;
	  break;
	}
      }
      if ( e )
	ac = true;
    }
  }
  if ( ac ) {
    for ( unsigned int i=0; i<AO.size(); i++ ) {
      if ( AO[i].Signals.size() > 0 &&
	   AO[i].AO->convertData( AO[i].Signals ) != 0 )
	success = false;
    }
  }
  // XXX hier muss noch was passieren:
  //    AO[di].Signals[0].deviceBufferReset();

  // prepare writing to daq boards:
  vector< AOData* > aod;
  aod.reserve( AO.size() );
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    if ( AO[i].Signals.size() > 0 ) {
      aod.push_back( &AO[i] );
      if ( aod.back()->AO->prepareWrite( AO[i].Signals ) != 0 )
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
    if ( restartRead( aod, true ) != 0 )
      success = false;
  }
  else {
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

  LastDevice = signal[0].device();
  //  LastWrite = 0.0; this is set in restartRead
  LastDuration = signal[0].duration();// XXX
  LastDelay = signal[0].delay();

  return 0;
}


int Acquire::writeData( void )
{
  bool finished = true;
  bool error = false;
  for ( unsigned int i=0; i<AO.size(); i++ ) {
    if ( ! AO[i].Signals.empty() ) {
      if ( AO[i].Signals[0].deviceBufferMaxPop() > 0 && AO[i].AO->running() ) {
	finished = false;
	int r = AO[i].AO->writeData();
	if ( r < 0 )
	  error = true;
      }
      else {
	if ( AO[i].Signals[0].autoConvert() ) {
	  AO[i].Signals.freeDeviceBuffer();
	}
	AO[i].Signals.clear();
      }
    }
  }
  return error ? -1 : ( finished ? 0 : 1 );
}


int Acquire::writeZero( int channel, int device )
{
  OutData signal( 1, 0.0001 );
  signal.setChannel( channel, device );
  signal[0] = 0.0;

  // get ao device:
  int di = signal.device();
  if ( di < 0 || di >= (int)AO.size() )
    signal.addError( DaqError::NoDevice );

  // device still busy?
  if ( AO[di].AO->running() )
    AO[di].AO->reset();

  // error?
  if ( ! signal.success() ) {
    cerr << currentTime() << " ! error in Acquire::writeZero( int, int ) -> "
	 << signal.errorStr() << "\n";
    return -signal.error();
  }

  // write to daq board:
  Signal.clear();
  Signal.add( &signal );
  AO[di].AO->convertData( Signal );
  AO[di].AO->prepareWrite( Signal );
  AO[di].AO->startWrite();

  // error?
  if ( ! signal.success() ) {
    cerr << currentTime() << " ! error in Acquire::writeZero( int, int ) -> "
	 << signal.errorStr() << "\n";
    return -signal.error();
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


int Acquire::stopWrite( void )
{
  bool success = true;

  for ( unsigned int i = 0; i<AO.size(); i++ ) {
    if ( AO[i].AO->isOpen() ) {
      if ( AO[i].AO->reset() != 0 )
	success = false;
      else
	AO[i].Signals.clear();
    }
  }

  return success ? 0 : -1;
}


void Acquire::readSignal( InList &data, EventList &events )
{
  double sigtime = -1.0;

  //  cerr << "Acquire::readSignal()\n";

  if ( SyncMode == CounterSync || SyncMode == AISync ) {
    if ( LastDevice < 0 )
      return;
    // get signal time:
    long inx = AO[LastDevice].AO->index();
    if ( inx < 0 )
      return;
    int d = AO[LastDevice].AISyncDevice;
    if ( d < 0 || AI[d].Traces.empty() )
      return;
    AI[d].Traces[0].setSignalIndex( inx );
    sigtime = AI[d].Traces[0].signalTime();
  }
  else {
    if ( LastWrite < 0.0 )
      return;
    sigtime = LastWrite + LastDelay;
  }

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
	   << " ! error in Acquire::readSignal() -> signalTime " << sigtime
	   << " < back() " << events[k].back() 
	   << ", current time = " << data[0].currentTime()
	   << ", restart time = " << data[0].restartTime() << '\n';
    }
  }

  LastDevice = -1;
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
	     << " ! error in Acquire::readSignal() -> restartTime " << restarttime
	     << " < back() " << events[k].back() 
	     << ", current time = " << data[0].currentTime()
	     << ", restart time = " << data[0].restartTime() << '\n';
      }
      break;
    }
  }
}


void Acquire::setReset( void )
{
  HardReset = true; 
}


void Acquire::stop( void )
{
  stopRead();
  stopWrite();
}


void Acquire::reset( void )
{
}


string Acquire::currentTime( void )
{
  char s[200];
  time_t ct = time( NULL );
  strftime( s, 200, "%H:%M:%S", localtime( &ct ) );
  return s;
}


}; /* namespace relacs */

