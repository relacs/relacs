/*
  xunit.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#include <cstdlib>
#include <iostream>
#include <relacs/str.h>
#include <relacs/parameter.h>
using namespace relacs;


int main( int argc, char *argv[] )
{
  if ( argc <  3 ) {
    cerr << "Usage:\n";
    cerr << "xunit 2.0ms ns\n";
    return 1;
  }

  Str s1 = argv[1];
  Str s2 = argv[2];

  double v1 = s1.number();
  string u1 = s1.unit();
  string u2 = s2;

  cout << v1 << u1 << " = "
       << Parameter::changeUnit( v1, u1, u2 ) << u2 << endl;

  return 0;
}
