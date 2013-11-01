/*
  xbufferarray.cc
  

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

#include <iostream>
#include <fstream>
#include <relacs/bufferarray.h>
using namespace std;
using namespace relacs;


int main( void )
{
  // write file:
  {
    BufferArrayF buffer( 1000, 500 );
    long n = 0;
    ofstream fs( "test.dat" );
    for ( long k=0; k<100000; k++ ) {
      float val = k;
      buffer.push( val );
      if ( k%123 == 50 ) {
	int m = buffer.saveBinary( fs, n );
	n += m;
	//	cerr << m << '\n';
      }
    }
    int m = buffer.saveBinary( fs, n );
    n += m;
    //    cerr << m << '\n';
    cerr << "wrote " << n << " data elements.\n";
  }

  // read file:
  {
    ifstream fs( "test.dat" );
    float buffer[942];
    int n = 0;
    long k=0;
    while ( fs.good() ) {
      fs.read( (char *)buffer, 942*sizeof( float ) );
      int m = fs.gcount()/sizeof( float );
      n += m;
      for ( int j=0; j<m; j++ ) {
	float val = k;
	if ( buffer[j] != val )
	  cerr << "error at element " << val << ". Is " << buffer[j] << '\n';
	k++;
      }
    }
    cerr << "read  " << n << " data elements.\n";
  }

  return 0;
}
