/*
  model.cc
  Base class of all models used by Simulate.

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

#include <QAction>
#include <relacs/relacswidget.h>
#include <relacs/model.h>

namespace relacs {


Model::Model( const string &name, const string &pluginset,
	      const string &author,
	      const string &version, const string &date )
  : RELACSPlugin( "Model: " + name, RELACSPlugin::Plugins,
		  name, pluginset, author, version, date ),
    DataMutex( 0 ),
    DataWait( 0 ),
    Signals( 0 ),
    SignalChannels( 0 ),
    SignalValues( 0 ),
    SignalMutex(),
    InterruptLock()
{
  Thread = new ModelThread( this );
  InterruptModel = false;
  AveragedLoad = 0;
  AverageRatio = 0.01;
}


Model::~Model( void )
{
}


int Model::traces( void ) const
{
  return Data.size();
}


string Model::traceName( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].ident() : "";
}


double Model::deltat( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].sampleInterval() : 0.0;
}


double Model::time( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].currentTime() : 0.0;
}


float Model::scale( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].scale() : 1.0;
}


double Model::load( void ) const
{
  return AveragedLoad;
}


void Model::push( int trace, float val )
{
  if ( trace == 0 ) {
    double t = Data[0].currentTime();
    if ( AIDevice != 0 ) {
      // compute dynamic clamp model:
      SignalMutex.lock();
      for ( unsigned int k=0; k<SignalValues.size(); k++ )
	SignalValues[k] = signal( t, k );
      AIDevice->model( Data, SignalChannels, SignalValues );
      for ( unsigned int k=0; k<SignalValues.size(); k++ )
	Signals[k].ModelValue = SignalValues[k];
      SignalMutex.unlock();
    }
    PushCount++;
    if ( PushCount >= MaxPush ) {
      PushCount = 0;
      double dt = t - elapsed();
      double l = 1.0 - dt / MaxPushTime;
      AveragedLoad = AveragedLoad * (1.0 - AverageRatio ) + l * AverageRatio;
      SignalMutex.lock();
      bool released = false;
      for ( unsigned int k=0; k<Signals.size(); k++ ) {
	if ( ! Signals[k].Finished && t > Signals[k].Offset ) {
	  Signals[k].Finished = true;
	  if ( ! released ) {
	    SignalsWait.release( 1 );
	    released = true;
	  }
	}
      }
      SignalMutex.unlock();
      long st = (long)::rint( 1000.0 * dt );
      if ( st <= 0 )
	st = 1;
      DataWait->wakeAll();
      InputWait.wait( DataMutex, st );
    }
  }
  Data[trace].push( val );
}


void Model::waitOnSignals( void )
{
  SignalsWait.acquire( SignalsWait.available() );
  bool wait = false;
  SignalMutex.lock();
  for ( unsigned int k=0; k<Signals.size(); k++ ) {
    if ( ! Signals[k].Finished ) {
      wait = true;
      break;
    }
  }
  SignalMutex.unlock();
  if ( wait )
    SignalsWait.acquire( 1 );
}


void Model::main( void )
{
}


void Model::process( const OutData &source, OutData &dest )
{
  dest = source;
  if ( source.level() != OutData::NoLevel )
    dest *= ::pow( 10.0, -source.level()/20.0 );
}


void Model::notify( void )
{
  if ( Thread->isRunning() ) {
    stop();
    restart();
  }
}


Options Model::metaData( void )
{
  return *this;
}


double Model::signal( double t, int trace ) const 
{
  if ( Signals.empty() || trace < 0 || trace >= (int)Signals.size() )
    return 0.0;

  if ( Signals[trace].Onset <= t && Signals[trace].Offset >= t ) {
    t -= Signals[trace].Onset;
    int inx = Signals[trace].Buffer.index( t );
    if ( inx < 0 )
      inx = 0;
    else if ( inx >= Signals[trace].Buffer.size() )
      inx = Signals[trace].Buffer.size()-1;
    Signals[trace].LastSignal = Signals[trace].Buffer[inx];
  }
  return Signals[trace].LastSignal + Signals[trace].ModelValue;
}


bool Model::interrupt( void ) const
{
  InterruptLock.lock();
  bool ir = InterruptModel; 
  InterruptLock.unlock();
  return ir;
}


bool Model::isRunning( void ) const
{
  return Thread->isRunning();
}


void Model::start( InList &data, AnalogInput *aidevice,
		   QReadWriteLock *datamutex, QWaitCondition *datawait )
{
  Data.clear();
  for ( int k=0; k<data.size(); k++ )
    Data.add( &data[k] );
  DataMutex = datamutex;
  DataWait = datawait;
  AIDevice = aidevice;
  InterruptModel = false;
  AveragedLoad = 0;
  MaxPush = deltat( 0 ) > 0.0 ? (int)::ceil( 0.01 / deltat( 0 ) ) : 100;
  MaxPushTime = MaxPush * deltat( 0 );
  PushCount = 0;
  Signals.clear();
  SignalChannels.clear();
  SignalValues.clear();
  SimTime.start();
  Thread->start( QThread::HighPriority );
}


void Model::restart( void )
{
  DataMutex->lockForWrite();
  Data.setRestart();
  DataMutex->unlock();

  InterruptModel = false;
  Thread->start( QThread::HighPriority );
}


void Model::run( void )
{
  setSettings();
  DataMutex->lockForWrite();
  main();
  DataMutex->unlock();
}


void Model::stop( void )
{
  if ( Thread->isRunning() ) {
    // tell the Model to interrupt:
    SignalsWait.release( 1 );
    InterruptLock.lock();
    InterruptModel = true;
    InterruptLock.unlock();
    
    // wake up the Model from sleeping:
    InputWait.wakeAll();

    Thread->wait();
  }
}


double Model::add( OutData &signal, bool wait )
{
  // current time:
  double ct = elapsed();

  if ( signal.trace() < 0 ) {
    signal.setError( signal.InvalidTrace );
    return -1.0;
  }

  SignalMutex.lock();

  if ( signal.trace() >= (int)Signals.size() ) {
    // add new signals:
    Signals.resize( signal.trace()+1 );
    SignalChannels.resize( signal.trace()+1 );
    SignalValues.resize( signal.trace()+1 );
  }

  // trace still busy?
  if ( Signals[signal.trace()].Offset > ct && ! signal.priority() ) {
    signal.setError( signal.Busy );
    SignalMutex.unlock();
    return -1.0;
  }

  SignalChannels[signal.trace()] = signal.channel();
  SignalValues[signal.trace()] = 0.0;
  Signals[signal.trace()].Buffer.clear();
  process( signal, Signals[signal.trace()].Buffer );
  Signals[signal.trace()].Finished = ! wait;

  // current time:
  ct = elapsed();
  double bt = time( 0 );
  if ( ct <= bt + 10.0*deltat( 0 ) )
    ct = bt + 10.0*deltat( 0 );
  Signals[signal.trace()].Onset = ct + Signals[signal.trace()].Buffer.delay();
  Signals[signal.trace()].Offset = ct + Signals[signal.trace()].Buffer.totalDuration();
  double onset = Signals[signal.trace()].Onset;
  SignalMutex.unlock();
  return onset;
}


double Model::add( OutList &sigs, bool wait )
{
  // current time:
  double ct = elapsed();

  for ( int k=0; k<sigs.size(); k++ ) {
    if ( sigs[k].trace() < 0 )
      sigs[k].setError( OutData::InvalidTrace );
  }

  if ( sigs.failed() )
    return -1.0;

  SignalMutex.lock();

  for ( int k=0; k<sigs.size(); k++ ) {
    if ( sigs[k].trace() >= (int)Signals.size() ) {
      // add new signals:
      Signals.resize( sigs[k].trace()+1 );
      SignalChannels.resize( sigs[k].trace()+1 );
      SignalValues.resize( sigs[k].trace()+1 );
    }
  }

  // trace still busy?
  for ( int k=0; k<sigs.size(); k++ ) {
    if ( Signals[sigs[k].trace()].Offset > ct && ! sigs[k].priority() )
      sigs[k].setError( OutData::Busy );
  }

  if ( sigs.failed() ) {
    SignalMutex.unlock();
    return -1.0;
  }

  // transer signals to buffer:
  for ( int k=0; k<sigs.size(); k++ ) {
    SignalChannels[sigs[k].trace()] = sigs[k].channel();
    SignalValues[sigs[k].trace()] = 0.0;
    Signals[sigs[k].trace()].Buffer.clear();
    process( sigs[k], Signals[sigs[k].trace()].Buffer );
    Signals[sigs[k].trace()].Finished = ! wait;
  }

  // current time:
  ct = elapsed();
  double bt = time( 0 );
  if ( ct <= bt + 10.0*deltat( 0 ) )
    ct = bt + 10.0*deltat( 0 );
  for ( int k=0; k<sigs.size(); k++ ) {
    Signals[sigs[k].trace()].Onset = ct + Signals[sigs[k].trace()].Buffer.delay();
    Signals[sigs[k].trace()].Offset = ct + Signals[sigs[k].trace()].Buffer.totalDuration();
  }
  double onset = Signals[sigs[0].trace()].Onset;
  SignalMutex.unlock();

  return onset;
}


void Model::stopSignals( void )
{
  // current time:
  double ct = elapsed();
  SignalMutex.lock();
  for ( deque< OutTrace >::iterator sp = Signals.begin(); sp != Signals.end(); ++sp ) {
    if ( sp->Onset >= ct ) {
      sp->Onset = 0.0;
      sp->Offset = 0.0;
      sp->Buffer.clear();
    }
    else if ( sp->Offset > ct )
      sp->Offset = ct;
  }
  SignalsWait.release( 1 );
  SignalMutex.unlock();
}


void Model::clearSignals( void )
{
  SignalMutex.lock();
  for ( deque< OutTrace >::iterator sp = Signals.begin(); sp != Signals.end(); ++sp ) {
    sp->Onset = 0.0;
    sp->Offset = 0.0;
    sp->Buffer.clear();
    sp->Finished = true;
  }
  SignalsWait.release( 1 );
  SignalMutex.unlock();
}


double Model::elapsed( void ) const
{
  return 0.001 * SimTime.elapsed();
}


void Model::addActions( QMenu *menu, bool doxydoc )
{
  menu->addAction( string( name() + " Dialog..." ).c_str(),
		   this, SLOT( dialog() ) );
  menu->addAction( string( name() + " Help..." ).c_str(),
		   this, SLOT( help() ) );
  if ( widget() != 0 )
    menu->addAction( string( name() + " Screenshot" ).c_str(),
		     this, SLOT( saveWidget() ) );
  if ( doxydoc )
    menu->addAction( string( name() + " Doxygen" ).c_str(),
		     this, SLOT( saveDoxygenOptions() ) );
}


ModelThread::ModelThread( Model *m )
  : QThread( m ),
    M( m )
{
}


void ModelThread::run( void )
{
  M->run();
}


}; /* namespace relacs */

