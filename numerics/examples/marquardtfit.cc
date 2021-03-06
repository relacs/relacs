/*
  marquardtfit.cc
  

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
#include <fstream>
#include <relacs/array.h>
#include <relacs/basisfunction.h>
#include <relacs/fitalgorithm.h>
#include <relacs/random.h>
using namespace std;
using namespace relacs;


int main( void )
{
  // function:
  ArrayD c( 3 );
  c[0] = 1.0;
  c[1] = 2.0;
  c[2] = 3.0;

  // generate data:
  ofstream of( "data.dat" );
  double xmin = 0.0;
  double xmax = 5.0;
  double sig = 0.5;
  int n=40;
  ArrayD x( n ), y( n ), s( n );
  for ( int k=0; k<n; k++ ) {
    x[k] = rnd.gaussian()*(xmax-xmin)/4+0.5*(xmin+xmax);
    y[k] = expFunc( x[k], c ) + sig*rnd.gaussian();
    s[k] = sig;
    of << x[k] << "  " << y[k] << "  " << s[k] << '\n';
  }
  of.close();

  // fit:
  ArrayD p( 3, 1.0 );
  p[0] = 1.0;
  p[1] = 2.0;
  p[2] = 4.5;
  ArrayI pi( 3, 1 );
  pi[2] = 1;
  ArrayD u( 3, 1.0 );
  double ch = 0.0;
  int iter = 0;
  marquardtFit( x, y, s, expFuncDerivs, p, pi, u, ch, &iter, &cout );

  for ( int i=0; i<3; i++ )
    cout << "c[" << i << "] = " << p[i] << " +/- " << u[i] << '\n';
  cout << "chisq = " << ch << "  iter = " << iter << '\n';

  return 0;
}
