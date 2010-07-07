/*
  base/wait.cc
  Wait until a specified (date and) time.

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

#include <QDateTime>
#include <relacs/base/wait.h>
using namespace relacs;

namespace base {


Wait::Wait( void )
  : RePro( "Wait", "base", "Jan Benda", "1.0", "Jul 07, 2010" )
{
  // add some options:
  addDate( "date", "Date" );
  addTime( "time", "Time" );
  addBoolean( "savedata", "Save Data", false );
}


int Wait::main( void )
{
  // get options:
  int year, month, day;
  Options::date( "date", 0, year, month, day );
  int hour, minutes, seconds;
  Options::time( "time", 0, hour, minutes, seconds );
  QDateTime endtime( QDate( year, month, day ), QTime( hour, minutes, seconds ) );
  bool savedata = boolean( "savedata" );

  // don't print repro message:
  noMessage();

  // don't save files:
  if ( ! savedata )
    noSaving();

  message( "Time out at " + string( endtime.toString().toLatin1().data() ) );
  QDateTime currenttime = QDateTime::currentDateTime();
  while ( softStop() == 0 ) {
    currenttime = QDateTime::currentDateTime();
    if ( currenttime >= endtime )
      break;
    sleep( 0.1 );
    if ( interrupt() )
      return Aborted;
  }

  message( "Timed out at " + string( currenttime.toString().toLatin1().data() ) );
  return Completed;
}


addRePro( Wait );

}; /* namespace base */

#include "moc_wait.cc"
