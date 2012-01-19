/*
  xmoments.cc
  check mean, stdev, skewness, and kurtosis for Gaussian and Exponential distributions.

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

#include <cstdio>
#include <vector>
#include <relacs/stats.h>
#include <relacs/array.h>
using namespace relacs;


void moments( const ArrayD &x )
{
  /*
  double a = mean( x );
  double v = variance( x );
  double s = stdev( x );
  double sk = skewness( x );
  double kt = kurtosis( x );
  */

  double a = x.mean();
  double v = x.variance();
  double s = x.stdev();
  double sk = x.skewness();
  double kt = x.kurtosis();

  cout << "mean: " << a << '\n';
  cout << "variance: " << v << '\n';
  cout << "standard deviation: " << s << '\n';
  cout << "skewness: " << sk << '\n';
  cout << "kurtosis: " << kt << '\n';
}


int main( int argc, char **argv )
{
  ArrayD x;
  const int n = 1000000;

  cout << "\nUniform distribution (s=0, k=-6/5):\n";
  x.rand( n, rnd );
  moments( x );

  cout << "\nNormal distribution (s=0, k=0):\n";
  for ( ArrayD::iterator p = x.begin(); p != x.end(); ++p )
    *p = rnd.gaussian();
  //  x.randNorm( n, rnd );
  moments( x );

  cout << "\nExponential distribution (s=2, k=6):\n";
  for ( ArrayD::iterator p = x.begin(); p != x.end(); ++p )
    *p = rnd.exponential();
  moments( x );

  cout << "\nGamma distribution a=4 (s=1, k=3/2):\n";
  for ( ArrayD::iterator p = x.begin(); p != x.end(); ++p )
    *p = rnd.gamma( 4.0 );
  moments( x );

  return 0;
}
