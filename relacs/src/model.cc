/*
  ../include/relacs/model.cc
  Base class of all models used by Simulate.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <qaction.h>
#include <relacs/relacswidget.h>
#include <relacs/model.h>


Model::Model( const string &name, const string &titles, 
	      const string &author, const string &version,
	      const string &date )
  : RELACSPlugin( "Model: " + name, RELACSPlugin::Plugins,
		  name, titles, author, version, date ),
    Data()
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


float Model::gain( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].Gain : 1.0;
}


float Model::scale( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].Scale : 1.0;
}


float Model::offset( int trace ) const
{
  return trace >=0 && trace < (int)Data.size() ? Data[trace].Offset : 0.0;
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
		 double offs, double scale, double gain, int nbuffer )
{
  Data.push_back( InTrace( device, channel, deltat, offs, scale, gain, nbuffer ) );
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
  SignalMutex.unlock();
  if ( trace == 0 ) {
    PushCount++;
    if ( PushCount >= MaxPush ) {
      PushCount = 0;
      double dt = deltat( 0 ) * Data[0].Buffer.size() - elapsed();
      double l = 1.0 - dt / MaxPushTime;
      AveragedLoad = AveragedLoad * (1.0 - AverageRatio ) + l * AverageRatio;
      long st = (long)::rint( 1000.0 * dt );
      if ( st > 0 ) {
	QThread::msleep( st );
      }
    }
  }
  Data[trace].Buffer.push( val );
  SignalMutex.lock();
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
  if ( running() ) {
    stop();
    restart();
  }
}


double Model::signal( double t ) const 
{
  if ( ! Signals.empty() && Signals[0].Onset <= t && Signals[0].Offset >= t ) {
    t -= Signals[0].Onset;
    return Signals[0].Buffer[ t ];
  }
  else
    return 0.0;
}


double Model::signalInterpolated( double t ) const 
{
  if ( ! Signals.empty() && Signals[0].Onset <= t && Signals[0].Offset >= t ) {
    t -= Signals[0].Onset;
    return Signals[0].Buffer( t );
  }
  else
    return 0.0;
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
  Signals.reserve( 10 );
  Signals.clear();
  SignalEnd = -1.0;
  SignalMutex.lock();
  SimTime.start();
  QThread::start();
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
  main();
}


void Model::stop( void )
{
  if ( running() ) {
    // tell the Model to interrupt:
    InterruptLock.lock();
    InterruptModel = true;
    InterruptLock.unlock();
    
#ifdef USEWAITCONDITION
    
    // wake up the Model from sleeping:
    SleepWait.wakeAll();
    
#endif

    wait();

    SignalMutex.unlock();
  }
}


double Model::add( OutData &signal )
{
  // current time:
  double ct = elapsed();

  SignalMutex.lock();

  // device still busy?
  if ( SignalEnd > ct && ! signal.priority() ) {
    signal.setError( signal.Busy );
    SignalMutex.unlock();
    return -1.0;
  }

  // add signal:
  SignalEnd = -1.0;
  Signals.clear();
  Signals.push_back( OutTrace() );
  process( signal, Signals.back().Buffer );

  // current time:
  ct = elapsed();
  double bt = time( 0 );
  if ( ct < bt )
    ct = bt;
  Signals.back().Onset = ct + Signals.back().Buffer.delay();
  Signals.back().Offset = ct + Signals.back().Buffer.totalDuration();
  SignalEnd = ct + Signals.back().Buffer.totalDuration();
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
  if ( Signals.back().Onset >= ct )
    Signals.clear();
  else if ( Signals.back().Offset > ct )
    Signals.back().Offset = ct;
  SignalMutex.unlock();
}


void Model::clearSignals( void )
{
  SignalMutex.lock();
  SignalEnd = -1.0;
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


void Model::addActions( QPopupMenu *menu )
{
  QAction *action;

  action = new QAction( this );
  action->setMenuText( string( title() + " Dialog..." ).c_str() );
  //  action->setAccel( ALT + Key_S );
  connect( action, SIGNAL( activated() ),
	   this, SLOT( dialog() ) );
  action->addTo( menu );

  action = new QAction( this );
  action->setMenuText( string( title() + " Help..." ).c_str() );
  connect( action, SIGNAL( activated() ),
	   this, SLOT( help() ) );
  action->addTo( menu );
}

