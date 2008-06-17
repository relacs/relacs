/*
  xtests.cc
  

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
#include <relacs/random.h>
#include <relacs/statstests.h>
using namespace std;
using namespace relacs;


int main( void ) 
{
  /*
  // Binomial distribution:
  int n=10;
  double p = 0.3;
  for ( int k=0; k<=n; k++ )
    cout << k << " " << alphaBinomial( k, n, p ) << endl;;
  */

  // Binomial symmetry:
  int n=20;
  double p = 0.8;
  for ( int k=0; k<=n; k++ )
    cout << k << " " << alphaBinomial( k, n, p ) << " " << 1.0-alphaBinomial( n-k-1, n, 1.0-p ) << endl;

  /*
  // Wilcoxon symmetry:
  int maxw = n*(n+1)/2;
  for ( int w=0; w<=maxw; w++ )
    cout << w << " " << alphaWilcoxon( w, n ) << " " << 1.0-alphaWilcoxon( maxw-w-1, n ) << endl;
  */

  /*
  // Wilcoxon ditribution:
  int n=10;
  double s = 2.0;
  for ( double r=-5.0; r<=5.0; r+=1.0 ) {
    ArrayD xdata( n );
    ArrayD ydata( n );
    for ( int k=0; k<n; k++ ) {
      double a = rnd.gaussian();
      double b = rnd.gaussian();
      xdata[k] = a;
      ydata[k] = a + r+s*b;
    }
    int nn = 0;
    double wp = wWilcoxon( xdata, ydata, nn );
    double pp = alphaWilcoxon( wp, nn );
    double wm = nn*(nn+1.0)*0.5 - wp;
    double pm = alphaWilcoxon( wm, nn );
    double p = 2.0 * ( wm < wp ? pm : pp );
    cout << r << " " << wp << " " << pp << " " << wm << " " << pm << " " << p << endl;
  }
  */

  return 0;
}
