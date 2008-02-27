/*
  euler.cc
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include "odealgorithm.h"
using namespace std;

typedef vector< double > dvector;
typedef vector< dvector > dmatrix;

class LinearFirstOrder
{
public:
  LinearFirstOrder( void ) : Tau( 1.0 ) {};
  LinearFirstOrder( double tau ) : Tau( tau ) {};
  LinearFirstOrder( const LinearFirstOrder &l ) : Tau( l.Tau ) {};

  void operator()( double x, const dvector &y, dvector &dydx ) const
  {
    dydx[0] = -y[0]/Tau;
  };

private:
  double Tau;
};


int main( void )
{
  double x1 = 0.0;
  double x2 = 10.0;
  double deltax = 0.1;
  int n = (int)floor( (x2-x1)/deltax ) + 1;
  dvector x( n );
  dvector ystart( 1, 1.0 );
  dmatrix y( 1 );
  for ( unsigned int k=0; k<y.size(); k++ )
    y[k].resize( n );

  midpointInt( x, y, ystart, x1, x2, deltax, LinearFirstOrder( 2.0 ) );

  for ( unsigned int k=0; k<x.size(); k++ )
    cout << x[k] << " " << y[0][k] << endl;

  return 0;
}
