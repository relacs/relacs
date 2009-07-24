/*
  datathreads.cc
  The threads for reading and writing to the data acquisition boards. 

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

#include <relacs/relacswidget.h>
#include <relacs/datathreads.h>

namespace relacs {


ReadThread::ReadThread( RELACSWidget *rw )
  : RW( rw )
{
}


void ReadThread::start( void )
{
  RunMutex.lock();
  Run = true;
  RunMutex.unlock();
  QThread::start( HighestPriority );
}


void ReadThread::stop( void )
{
  RunMutex.lock();
  bool rd = Run;
  RunMutex.unlock();
  if ( rd ) {
    RunMutex.lock();
    Run = false;
    RunMutex.unlock();
    QThread::wait();
  }
}


void ReadThread::run( void )
{
  bool rd = true;

  double readinterval = RW->IL[0].readTime();
  signed long ri = (unsigned long)::rint( 1000.0*readinterval );
  QThread::msleep( ri/4 );
  do {
    RW->lockAI();
    int r = RW->AQ->readData();
    RW->unlockAI();
    RW->ReadDataWait.wakeAll();
    if ( r < 0 ) {
      RW->printlog( "! error in reading acquired data: " + RW->IL.errorText() );
      RunMutex.lock();
      Run = false;
      RunMutex.unlock();
      break;
    }
    QThread::msleep( ri );
    RunMutex.lock();
    rd = Run;
    RunMutex.unlock();
  } while ( rd );
}


WriteThread::WriteThread( RELACSWidget *rw )
  : RW( rw )
{
}


void WriteThread::start( double writeinterval )
{
  WriteInterval = writeinterval;
  RunMutex.lock();
  Run = true;
  RunMutex.unlock();
  QThread::start( HighestPriority );
}


void WriteThread::stop( void )
{
  RunMutex.lock();
  bool rd = Run;
  RunMutex.unlock();
  if ( rd ) {
    RunMutex.lock();
    Run = false;
    RunMutex.unlock();
    QThread::wait();
  }
}


  void WriteThread::run( void )
{
  bool rd = true;

  signed long wi = (unsigned long)::rint( 1000.0*WriteInterval );
  do {
    QThread::msleep( wi );
    RW->lockSignals();
    int r = RW->AQ->writeData();
    RW->unlockSignals();
    if ( r < 0 ) {
      RW->lockSignals();
      RW->AQ->stopWrite();
      RW->unlockSignals();
      RW->printlog( "! error in writing data. Stop analog output." );
      // error message:
      QApplication::postEvent( RW, new QCustomEvent( QEvent::User+3 ) );
    }
    if ( r <= 0 ) {
      RunMutex.lock();
      Run = false;
      RunMutex.unlock();
      break;
    }
    RunMutex.lock();
    rd = Run;
    RunMutex.unlock();
  } while ( rd );
}


}; /* namespace relacs */


