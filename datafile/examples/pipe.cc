/*
  pipe.cc
  

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

#include <iostream>
#include <fstream>
#include <string>
using namespace std;


int main( int argc, char *argv[] )
{
  // redirect cin:
  ifstream sf;
  if ( argc > 1 ) {
    sf.open( argv[1] );
    streambuf *sb = sf.rdbuf();
    cin.rdbuf( sb );
  }

  // redirect cout:
  ofstream df;
  if ( argc > 2 ) {
    df.open( argv[2] );
    streambuf *sb = df.rdbuf();
    cout.rdbuf( sb );
  }

  string s;
  while ( ! getline( cin, s ).eof() )
    cout << "piped: " << s << endl;

  if ( sf.is_open() )
    sf.close();
  if ( df.is_open() )
    df.close();

  return 0;
}
