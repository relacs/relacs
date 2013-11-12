/*
  model.cc
  Base class of all models used by Simulate.

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

#include <QAction>
#include <relacs/relacswidget.h>
#include <relacs/model.h>

namespace relacs {


Model::Model( const string &name, const string &pluginset,
	      const string &author,
	      const string &version, const string &date )
  : RELACSPlugin( "Model: " + name, RELACSPlugin::Plugins,
		  name, pluginset, author, version, date ),
    Data( 0 ),
    Signals( 0 ),
    SignalMutex(),
    InterruptLock()
{
  Thread = new ModelThread( this );
  InterruptModel = false;
  Restarted = false;
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
  return trace >=0 && trace < (int)Data.size() ? Data[trace].Name : "";
}


double Model::deltat( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].DeltaT : 0.0;
}


double Model::time( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].DeltaT * Data[trace].Buffer.size() : 0.0;
}


float Model::scale( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].Scale : 1.0;
}


double Model::load( void ) const
{
  return AveragedLoad;
}


void Model::setDeltat( int trace, double deltat )
{
  if ( trace < 0 ) {
    for ( unsigned int k=0; k<Data.size(); k++ )
      Data[k].DeltaT = deltat;
  }
  else
    Data[trace].DeltaT = deltat;
}


void Model::add( const string &name, double deltat, 
		 double scale, int nbuffer )
{
  Data.push_back( InTrace( name, deltat, scale, nbuffer ) );
}


void Model::clearData( void )
{
  for ( unsigned int k=0; k<Data.size(); k++ )
    Data[k].clear();
}


void Model::clear( void )
{
  clearData();
  Data.clear();
}


void Model::push( int trace, float val )
{
  if ( trace == 0 ) {
    PushCount++;
    if ( PushCount >= MaxPush ) {
      PushCount = 0;
      double dt = deltat( 0 ) * Data[0].Buffer.size() - elapsed();
      double l = 1.0 - dt / MaxPushTime;
      AveragedLoad = AveragedLoad * (1.0 - AverageRatio ) + l * AverageRatio;
      long st = (long)::rint( 1000.0 * dt );
      if ( st > 0 ) {
	SleepWait.wait( &SignalMutex, st );
      }
    }
  }
  Data[trace].Buffer.push( val );
}


void Model::main( void )
{
}


void Model::process( const OutData &source, OutData &dest )
{
  dest = source;
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
  return Signals[trace].LastSignal;
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


void Model::start( void )
{
  clearData();
  InterruptModel = false;
  Restarted = true;
  AveragedLoad = 0;
  MaxPush = deltat( 0 ) > 0.0 ? (int)::ceil( 0.01 / deltat( 0 ) ) : 100;
  MaxPushTime = MaxPush * deltat( 0 );
  PushCount = 0;
  Signals.clear();
  SimTime.start();
  Thread->start( QThread::HighestPriority );
}


void Model::restart( void )
{
  InterruptModel = false;
  Restarted = true;
  Thread->start( QThread::HighestPriority );
}


void Model::run( void )
{
  setSettings();
  SignalMutex.lock();
  main();
  SignalMutex.unlock();
}


void Model::stop( void )
{
  if ( Thread->isRunning() ) {
    // tell the Model to interrupt:
    InterruptLock.lock();
    InterruptModel = true;
    InterruptLock.unlock();
    
    // wake up the Model from sleeping:
    SleepWait.wakeAll();

    Thread->wait();
  }
}


double Model::add( OutData &signal )
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
  }

  // trace still busy?
  if ( Signals[signal.trace()].Offset > ct && ! signal.priority() ) {
    signal.setError( signal.Busy );
    SignalMutex.unlock();
    return -1.0;
  }

  Signals[signal.trace()].Buffer.clear();
  process( signal, Signals[signal.trace()].Buffer );

  // current time:
  ct = elapsed();
  double bt = time( 0 );
  if ( ct < bt )
    ct = bt;
  Signals[signal.trace()].Onset = ct + Signals[signal.trace()].Buffer.delay();
  Signals[signal.trace()].Offset = ct + Signals[signal.trace()].Buffer.totalDuration();
  double onset = Signals[signal.trace()].Onset;
  SignalMutex.unlock();
  return onset;
}


double Model::add( OutList &sigs )
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
    Signals[sigs[k].trace()].Buffer.clear();
    process( sigs[k], Signals[sigs[k].trace()].Buffer );
  }

  // current time:
  ct = elapsed();
  double bt = time( 0 );
  if ( ct < bt )
    ct = bt;
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
  SignalMutex.unlock();
}


void Model::clearSignals( void )
{
  SignalMutex.lock();
  for ( deque< OutTrace >::iterator sp = Signals.begin(); sp != Signals.end(); ++sp ) {
    sp->Onset = 0.0;
    sp->Offset = 0.0;
    sp->Buffer.clear();
  }
  SignalMutex.unlock();
}


double Model::elapsed( void ) const
{
  return 0.001 * SimTime.elapsed();
}


bool Model::restarted( void )
{
  bool r = Restarted;
  Restarted = false;
  return r; 
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

