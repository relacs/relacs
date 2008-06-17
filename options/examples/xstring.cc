/*
  xstring.cc
  

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
#include <vector>
#include <relacs/str.h>
using namespace std;
using namespace relacs;


int main( int argc, char *argv[] )
{ 
  if ( argc <  2 ) {
    cerr << "Usage:\n";
    cerr << "xstring some_text\n";
    return 1;
  }

  Str ss( argv[1] );
  //string a( argv[2] );
  //char *a = argv[2];
  //char a = argv[2][0];

  double v=0.25;
  ss.format( v, "fge" );
  cout << ss << endl;
  return 0;

  /*  
  vector< int > ri;
  ss.range( ri );
  for ( unsigned int k=0; k<ri.size(); k++ )
    cerr << ri[k] << endl;


  cout << '*' << ss << '*' << endl;
  cout << '*' << ss.field( 0, " ", "(", "#" ) << '*' << endl;
  cout << '*' << ss.field( 1, " ", "(", "#" ) << '*' << endl;
  cout << '*' << ss.field( 2, " ", "(", "#" ) << '*' << endl;
  cout << '*' << ss.field( 3, " ", "(", "#" ) << '*' << endl;
  */
  return 0;
}

