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
}


void ReadThread::start( void )
{
  QThread::start( HighPriority );
}


void ReadThread::run( void )
{
  while ( true ) {
    int r = RW->AQ->waitForRead();
    if ( r < 0 ) {
      RW->printlog( "! error in reading acquired data: " + RW->IL.errorText() );
      RW->AQ->restartRead( &RW->DataMutex, &RW->ReadDataWait );
    }
    else
      break;
  };
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
  // XXX Check for errors!
}


}; /* namespace relacs */


