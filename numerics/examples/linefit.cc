/*
  linefit.cc
  

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
#include <relacs/array.h>
#include <relacs/fitalgorithm.h>
#include <relacs/random.h>
using namespace std;
using namespace relacs;


int main( void )
{
  // function:
  double m = 2.0;
  double b = -1.0;

  // generate data:
  double xmin = 0.0;
  double xmax = 10.0;
  double sig = 0.1;
  int n=40;
  ArrayD x( n ), y( n ), s( n );
  for ( int k=0; k<n; k++ ) {
    x[k] = rnd.gaussian()*(xmax-xmin)/4+0.5*(xmin+xmax);
    y[k] = m*x[k] + b + sig*rnd.gaussian();
    s[k] = sig;
  }

  // fit:
  double br=0.0;
  double bru=0.0;
  double mr=0.0;
  double mru=0.0;
  double chisq=0.0;
  lineFit( x, y, s, br, bru, mr, mru, chisq );

  cout << "m=" << m << " mr=" << mr << " mru=" << mru << '\n';
  cout << "b=" << b << " br=" << br << " bru=" << bru << '\n';
  cout << "chisq = " << chisq << '\n';

  return 0;
}
