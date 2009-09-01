/*
  control.cc
  Base class for widgets that control hardware or analyze data.

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

#include <QAction>
#include <relacs/str.h>
#include <relacs/relacswidget.h>
#include <relacs/control.h>

namespace relacs {


Control::Control( const string &name, const string &title,
		  const string &pluginset, const string &author, 
		  const string &version, const string &date )
  : RELACSPlugin( "Control: " + name, RELACSPlugin::Plugins,
		  name, title, pluginset, author, version, date )
{
  Interrupt = false;
}


Control::~Control( void )
{
}


void Control::setName( const string &name )
{
  ConfigDialog::setName( name );
  setConfigIdent( "Control: " + name );
}


void Control::initialize( void )
{
}


void Control::initDevices( void )
{
}


void Control::addActions( QMenu *menu )
{
  QAction *action;

  action = new QAction( this );
  action->setMenuText( string( title() + " Dialog..." ).c_str() );
  connect( action, SIGNAL( activated() ),
	   this, SLOT( dialog() ) );
  action->addTo( menu );

  action = new QAction( this );
  action->setMenuText( string( title() + " Help..." ).c_str() );
  connect( action, SIGNAL( activated() ),
	   this, SLOT( help() ) );
  action->addTo( menu );
}


void Control::keyPressEvent( QKeyEvent *e )
{
  QWidget::keyPressEvent( e );
}


void Control::keyReleaseEvent( QKeyEvent *e )
{
  QWidget::keyReleaseEvent( e );
}


void Control::run( void )
{
  timeStamp();
  lockAll();
  main();
  unlockAll();
}


void Control::main( void )
{
}


bool Control::interrupt( void ) const
{
  InterruptLock.lock();
  bool ir = Interrupt; 
  InterruptLock.unlock();
  return ir;
}


bool Control::waitOnData( double time )
{
  unlockAll();
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  RW->DataSleepWait.wait( t );
  lockAll();
  return interrupt();
}


bool Control::waitOnReProSleep( double time )
{
  unlockAll();
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  RW->ReProSleepWait.wait( t );
  lockAll();
  return interrupt();
}


bool Control::waitOnReProFinished( double time )
{
  unlockAll();
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  RW->ReProAfterWait.wait( t );
  lockAll();
  return interrupt();
}


bool Control::waitOnSessionStart( double time )
{
  unlockAll();
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  RW->SessionStartWait.wait( t );
  lockAll();
  return interrupt();
}


bool Control::waitOnSessionPrestop( double time )
{
  unlockAll();
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  RW->SessionPrestopWait.wait( t );
  lockAll();
  return interrupt();
}


bool Control::waitOnSessionStop( double time )
{
  unlockAll();
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  RW->SessionStopWait.wait( t );
  lockAll();
  return interrupt();
}


bool Control::sleep( double t )
{
  // sleep:
  unlockAll();
  if ( t > 0.0 ) {
    unsigned long ms = (unsigned long)(1.0e3*t);
    if ( t < 0.001 || ms < 1 )
      QThread::usleep( (unsigned long)(1.0e6*t) );
    else
      SleepWait.wait( ms );
  }

  lockAll();

  // interrupt Control:
  return interrupt();
}


void Control::timeStamp( void )
{
  SleepTime.start();
}


bool Control::sleepOn( double t )
{
  double st = 0.001 * SleepTime.elapsed();
  return Control::sleep( t - st );
}


void Control::start( void )
{
  QThread::start();
}


void Control::requestStop( void )
{
  // terminate thread:
  if ( running() ) {
    InterruptLock.lock();
    Interrupt = true;
    InterruptLock.unlock();

    // wake up the Control from sleeping:
    SleepWait.wakeAll();
  }
}



}; /* namespace relacs */

#include "moc_control.cc"

