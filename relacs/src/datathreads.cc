/*
  datathreads.cc
  The threads for reading and writing to the data acquisition boards.

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

#include <relacs/relacswidget.h>
#include <relacs/acquire.h>
#include <relacs/datathreads.h>

namespace relacs {


ReadThread::ReadThread( RELACSWidget *rw )
  : RW( rw )
{
  Run = false;
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
  Run = false;
  RunMutex.unlock();
  if ( rd )
    QThread::wait();
}


void ReadThread::run( void )
{
  bool rd = true;

  do {
    RW->writeLockAI();
    int r = RW->AQ->readData();
    RW->unlockAI();
    if ( r < 0 ) {
      RW->printlog( "! error in reading acquired data: " + RW->IL.errorText() );
      RW->writeLockAI();
      RW->AQ->restartRead();
      RW->unlockAI();
    }
    else if ( r == 0 ) {
      RW->printlog( "ReadThread -> finished reading data" );
      RunMutex.lock();
      Run = false;
      RunMutex.unlock();
      break;
    }
    RW->writeLockData();
    RW->writeLockAI();
    RW->AQ->convertData();
    RW->unlockAI();
    RW->unlockData();
    RW->ReadDataWait.wakeAll();
    RunMutex.lock();
    rd = Run;
    RunMutex.unlock();
  } while ( rd );
}


WriteThread::WriteThread( RELACSWidget *rw )
  : RW( rw )
{
  Run = false;
}


void WriteThread::start( void )
{
  RunMutex.lock();
  Run = true;
  RunMutex.unlock();
  QThread::start( HighestPriority );
}


void WriteThread::stop( void )
{
  RunMutex.lock();
  bool rd = Run;
  Run = false;
  RunMutex.unlock();
  if ( rd )
    QThread::wait();
}


void WriteThread::run( void )
{
  bool rd = true;

  do {
    int r = RW->AQ->writeData();
    if ( r < 0 ) {
      RW->AQ->stopWrite();
      RW->printlog( "! error in transferring analog output data. Stop analog output." );
      // error message:
      QCoreApplication::postEvent( RW, new QEvent( QEvent::Type( QEvent::User+3 ) ) );
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


