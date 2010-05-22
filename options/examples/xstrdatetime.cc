/*
  xstrdatetime.cc
  

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

#include <iostream>
#include <relacs/str.h>
using namespace std;
using namespace relacs;


int main( int argc, char *argv[] )
{ 
  if ( argc <  2 ) {
    cerr << "Usage:\n";
    cerr << "xstrdatetime some_date_or_time_string\n";
    return 1;
  }

  Str ss( argv[1] );

  int year, month, day;
  int date = ss.date( year, month, day );
  int hour, minutes, seconds;
  int time = ss.time( hour, minutes, seconds );

  if ( date == 0 ) {
    cout << ss << " is a date\n";
    cout << "year =" << year << '\n';
    cout << "month=" << month << '\n';
    cout << "day  =" << day << '\n';
  }
  else if ( time == 0 ) {
    cout << ss << " is a time\n";
    cout << "hour   =" << hour << '\n';
    cout << "minutes=" << minutes << '\n';
    cout << "seconds=" << seconds << '\n';
  }
  else
    cout << ss << " is an invalid time (" << time << ") or date (" << date << ")\n";

  return 0;
}

