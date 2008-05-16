/*
  temp.cc
  Example for TempDTM5080 temeprature measurement

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/misc/tempdtm5080.h>
using namespace misc;


int main( void )
{
  TempDTM5080 t( "/dev/ttyS0", 1 );
  cout << t << endl;
  for ( ; ; ) {
    cout << "temp: " << t.temperature() << '\n';
    sleep( 1 );
  }
  return 0;
}
