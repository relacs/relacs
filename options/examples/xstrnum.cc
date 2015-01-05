/*
  xstrnum.cc
  

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
#include <relacs/str.h>
using namespace std;
using namespace relacs;


int main( int argc, char *argv[] )
{ 
  if ( argc <  2 ) {
    cerr << "Usage:\n";
    cerr << "xstrnum some_text_starting_with_a_number\n";
    return 1;
  }

  Str ss( argv[1] );

  ss.setSpace( Str::WhiteSpace );

  int n;
  cout << "number: " << ss.number( -1.0, 0, &n );
  cout << ", n=" << n << endl;
  cout << " error: " << ss.error( -1.0, 0, &n );
  cout << ", n=" << n << endl;
  cout << "  unit: " << ss.unit( "no unit", 0, &n );
  cout << ", n=" << n << endl;

  double e = -1.0;
  cout << "number: " << ss.number( e, -1.0, 0, &n );
  cout << " error: " << e << ", n=" << n << endl;

  string unit = "no unit";
  e = -1.0;
  cout << "number: " << ss.number( e, unit, -1.0, 0, &n );
  cout << " error: " << e << " unit: " << unit << ", n=" << n << endl;

  return 0;
}

