/*
  model.cc
  Base class of all models used by Simulate.

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


int Model::device( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].Device : -1;
}


int Model::channel( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].Channel : -1;
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


void Model::add( int device, int channel, double deltat, 
		 double scale, int nbuffer )
{
  Data.push_back( InTrace( device, channel, deltat, scale, nbuffer ) );
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
	SignalMutex.unlock();
	QThread::msleep( st );
	SignalMutex.lock();
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
  if ( isRunning() ) {
    stop();
    restart();
  }
}


double Model::signal( double t, int trace ) const 
{
  if ( Signals.empty() )
    return 0.0;

  int inx = -1;
  for ( unsigned int k=0; k<Signals.size(); k++ ) {
    if ( Signals[k].Buffer.trace() == trace ) {
      inx = k;
      break;
    }
  }
  if ( inx < 0 ) {
    if ( trace < 0 )
      inx = 0;
    else
      return 0.0;
  }

  if ( Signals[inx].Onset <= t && Signals[inx].Offset >= t ) {
    t -= Signals[inx].Onset;
    int tinx = Signals[inx].Buffer.index( t );
    if ( tinx < 0 )
      tinx = 0;
    else if ( tinx >= Signals[inx].Buffer.size() )
      tinx = Signals[inx].Buffer.size()-1;
    Signals[inx].LastSignal = Signals[inx].Buffer[tinx];
  }
  return Signals[inx].LastSignal;
}


bool Model::interrupt( void ) const
{
  InterruptLock.lock();
  bool ir = InterruptModel; 
  InterruptLock.unlock();
  return ir;
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
  QThread::start( QThread::HighestPriority );
}


void Model::restart( void )
{
  InterruptModel = false;
  Restarted = true;
  SignalMutex.lock();
  QThread::start();
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
  if ( isRunning() ) {
    // tell the Model to interrupt:
    InterruptLock.lock();
    InterruptModel = true;
    InterruptLock.unlock();
    
    // wake up the Model from sleeping:
    SleepWait.wakeAll();

    wait();
  }
}


double Model::add( OutData &signal )
{
  // current time:
  double ct = elapsed();

  SignalMutex.lock();

  // find signal on same trace:
  for ( unsigned int k=0; k<Signals.size(); k++ ) {
    if ( Signals[k].Buffer.trace() == signal.trace() ) {
      // trace still busy?
      if ( Signals[k].Offset > ct && ! signal.priority() ) {
	signal.setError( signal.Busy );
	SignalMutex.unlock();
	return -1.0;
      }
      Signals.erase( Signals.begin()+k );
      break;
    }
  }

  // add signal:
  Signals.push_back( OutTrace() );
  process( signal, Signals.back().Buffer );

  // current time:
  ct = elapsed();
  double bt = time( 0 );
  if ( ct < bt )
    ct = bt;
  Signals.back().Onset = ct + Signals.back().Buffer.delay();
  Signals.back().Offset = ct + Signals.back().Buffer.totalDuration();
  SignalMutex.unlock();
  return Signals.back().Onset;
}


void Model::stopSignal( void )
{
  // current time:
  double ct = elapsed();

  SignalMutex.lock();
  if ( Signals.empty() ) {
    SignalMutex.unlock();
    return;
  }
  for ( deque< OutTrace >::iterator sp = Signals.begin(); sp != Signals.end(); ) {
    if ( sp->Onset >= ct ) {
      sp = Signals.erase( sp );
      continue;
    }
    else if ( sp->Offset > ct )
      sp->Offset = ct;
    ++sp;
  }
  SignalMutex.unlock();
}


void Model::clearSignals( void )
{
  SignalMutex.lock();
  Signals.clear();
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


void Model::addActions( QMenu *menu )
{
  menu->addAction( string( name() + " Dialog..." ).c_str(),
		   (RELACSPlugin*)this, SLOT( dialog() ) );
  menu->addAction( string( name() + " Help..." ).c_str(),
		   (RELACSPlugin*)this, SLOT( help() ) );
}


}; /* namespace relacs */

