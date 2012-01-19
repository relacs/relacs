/*
  xarray.cc
  

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

#include <cmath>
#include <iostream>
#include <vector>
#include <relacs/array.h>
using namespace std;
using namespace relacs;


int main( int argc, char **argv )
{
  ArrayD a( 4 );
  ArrayF b( 4 );
  vector< double > c( 4 );
  Array< int > d( 4 );

  for ( int k=0; k<a.size(); k++ ) {
    a[k] = k;
    b[k] = a.size() - k;
    c[k] = 10 + k;
    d[k] = 1 + k;
  }

  cout << "a:\n" << a << '\n';
  cout << "b:\n" << b << '\n';
  cout << "d:\n" << d << '\n';

  cout << "a += b\n";
  a += b;
  cout << "a:\n" << a << '\n';

  cout << "a += c\n";
  a += c;
  cout << "a:\n" << a << '\n';

  cout << "a += d\n";
  a += d;
  cout << "a:\n" << a << '\n';

  cout << "d += c\n";
  d += c;
  cout << "d:\n" << d << '\n';

  cout << "a += 1.0\n";
  a += 1.0;
  cout << "a:\n" << a << '\n';

  float f = -9.0;
  cout << "a += f\n";
  a += f;
  cout << "a:\n" << a << '\n';

  long double l = 2.0;
  cout << "a /= l\n";
  a /= l;
  cout << "a:\n" << a << '\n';

  int i = 4;
  cout << "a += i\n";
  a += i;
  cout << "a:\n" << a << '\n';

  double x = 1.4;
  cout << "d += x\n";
  d += x;
  cout << "d:\n" << d << '\n';





  for ( int k=0; k<a.size(); k++ ) {
    a[k] = k;
    b[k] = a.size() - k;
    c[k] = 10 + k;
    d[k] = 1 + k;
  }

  cout << "a:\n" << a << '\n';
  cout << "b:\n" << b << '\n';
  cout << "d:\n" << d << '\n';

  cout << "b = a + 2.0\n";
  b = a + 2.0;
  cout << "b:\n" << b << '\n';

  cout << "b = 2.0 + a\n";
  b = 2.0 + a;
  cout << "b:\n" << b << '\n';

  cout << "a = a + a\n";
  a = a + a;
  cout << "a:\n" << a << '\n';

  cout << "a = a + b\n";
  a = a + b;
  cout << "a:\n" << a << '\n';

  cout << "a = b + a\n";
  a = b + a;
  cout << "a:\n" << a << '\n';

  cout << "a = a + c\n";
  a = a + c;
  cout << "a:\n" << a << '\n';

  cout << "a = c + a\n";
  a = c + a;
  cout << "a:\n" << a << '\n';


  float fa[5];
  Array<double> xx( a );
  xx.assign( a );
  xx.assign( b );
  xx.assign( c );
  double fd[6];
  Array<double> yy( b );

  /*
  Array y( x );
  //  x.resize( 15 );
  //  x[5] -= 23.0;

  cout << "x==y " << ( x == y ) << endl;
  cout << "x!=y " << ( x != y ) << endl;
  cout << "x<y  " << ( x < y ) << endl;
  cout << "x<=y " << ( x <= y ) << endl;
  cout << "x>y  " << ( x > y ) << endl;
  cout << "x>=y " << ( x >= y ) << endl;
  */

  return 0;
}
