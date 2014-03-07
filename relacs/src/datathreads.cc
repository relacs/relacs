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
    int r = RW->AQ->readData();
    if ( r < 0 ) {
      RW->printlog( "! error in reading acquired data: " + RW->IL.errorText() );
      RW->AQ->restartRead();
    }
    else if ( r == 0 ) {
      RW->printlog( "ReadThread -> finished reading data" );
      RunMutex.lock();
      Run = false;
      RunMutex.unlock();
      break;
    }
    RW->writeLockData();
    RW->AQ->convertData();
    RW->unlockData();
    RW->ReadDataWait.wakeAll();
    RunMutex.lock();
    rd = Run;
    RunMutex.unlock();
  } while ( rd );
  string errors = RW->AQ->readError();
  if ( ! errors.empty() )
    RW->printlog( "! error in transferring analog input data: " + errors );
}


WriteThread::WriteThread( RELACSWidget *rw )
  : RW( rw )
{
}


void WriteThread::start( void )
{
  QThread::start( HighPriority );
}


void WriteThread::run( void )
{
  RW->AQ->waitForWrite();
}


}; /* namespace relacs */


