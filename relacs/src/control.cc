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


Control::Control( const string &name, const string &pluginset,
		  const string &author, const string &version, 
		  const string &date )
  : RELACSPlugin( "Control: " + name, RELACSPlugin::Plugins,
		  name, pluginset, author, version, date )
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
  QAction* action = new QAction( (QWidget*)this );
  action->setText( string( name() + " Dialog..." ).c_str() );
  QWidget::connect( action, SIGNAL( triggered() ),
		    (QWidget*)this, SLOT( dialog() ) );
  menu->addAction( action );

  action = new QAction( (QWidget*)this );
  action->setText( string( name() + " Help..." ).c_str() );
  QWidget::connect( action, SIGNAL( triggered() ),
		    (QWidget*)this, SLOT( help() ) );
  menu->addAction( action );
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
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  unlockStimulusData();
  unlockMetaData();
  unlockData();
  RW->DataSleepWait.wait( mutex(), t );
  readLockData();
  lockMetaData();
  lockStimulusData();
  return interrupt();
}


bool Control::waitOnReProSleep( double time )
{
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  unlockStimulusData();
  unlockMetaData();
  unlockData();
  RW->ReProSleepWait.wait( mutex(), t );
  readLockData();
  lockMetaData();
  lockStimulusData();
  return interrupt();
}


bool Control::waitOnReProFinished( double time )
{
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  unlockStimulusData();
  unlockMetaData();
  unlockData();
  RW->ReProAfterWait.wait( mutex(), t );
  readLockData();
  lockMetaData();
  lockStimulusData();
  return interrupt();
}


bool Control::waitOnSessionStart( double time )
{
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  unlockStimulusData();
  unlockMetaData();
  unlockData();
  RW->SessionStartWait.wait( mutex(), t );
  readLockData();
  lockMetaData();
  lockStimulusData();
  return interrupt();
}


bool Control::waitOnSessionPrestop( double time )
{
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  unlockStimulusData();
  unlockMetaData();
  unlockData();
  RW->SessionPrestopWait.wait( mutex(), t );
  readLockData();
  lockMetaData();
  lockStimulusData();
  return interrupt();
}


bool Control::waitOnSessionStop( double time )
{
  unsigned long t = time == MAXDOUBLE ? ULONG_MAX : (unsigned long)::rint(1.0e3*time);
  unlockStimulusData();
  unlockMetaData();
  unlockData();
  RW->SessionStopWait.wait( mutex(), t );
  readLockData();
  lockMetaData();
  lockStimulusData();
  return interrupt();
}


bool Control::sleep( double t )
{
  // sleep:
  unlockStimulusData();
  unlockMetaData();
  unlockData();
  if ( t > 0.0 ) {
    unsigned long ms = (unsigned long)(1.0e3*t);
    if ( t < 0.001 || ms < 1 ) {
      lock();
      QThread::usleep( (unsigned long)(1.0e6*t) );
      unlock();
    }
    else
      SleepWait.wait( mutex(), ms );
  }
  readLockData();
  lockMetaData();
  lockStimulusData();

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
  if ( isRunning() ) {
    InterruptLock.lock();
    Interrupt = true;
    InterruptLock.unlock();

    // wake up the Control from sleeping:
    SleepWait.wakeAll();
  }
}



}; /* namespace relacs */

#include "moc_control.cc"

