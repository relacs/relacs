/*
  temp.cc
  Example for TempDTM5080 temeprature measurement

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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
#include <unistd.h>
#include <relacs/misc/tempdtm5080.h>
using namespace misc;


int main( void )
{
  Options opts;
  opts.addInteger( "probe", 1 );
  TempDTM5080 t( "/dev/ttyS0", opts );
  cout << t << endl;
  for ( ; ; ) {
    cout << "temp: " << t.temperature() << '\n';
    sleep( 1 );
  }
  return 0;
}
