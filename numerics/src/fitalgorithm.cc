/*
  fitalgorithm.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
#include <algorithm>
#include <relacs/fitalgorithm.h>

namespace relacs {


  /*! \a a is m x m matrix with m < n,
      \a b is n-dim or 0-dim vector. 
      returns:
      0: everything o.k. 
      1: Singular Matrix-1 
      2: Singular Matrix-2 */
int gaussJordan( vector< ArrayD > &a, int n, ArrayD &b )
{
  vector< int > indxc( n, 0 );
  vector< int > indxr( n, 0 );
  vector< int > ipiv( n, 0 );
  bool bvec = ( b.size() > 0 && b.size() >= n );
  int icol = 0;
  int irow = 0;
  for ( int i=0; i<n; i++ ) {
    double big = 0.0;
    for ( int j=0; j<n; j++) {
      if ( ipiv[j] != 1 ) {
	for ( int k=0; k<n; k++ ) {
	  if ( ipiv[k] == 0 ) {
	    if ( fabs(a[j][k]) >= big ) {
	      big = fabs( a[j][k] );
	      irow = j;
	      icol = k;
	    }
	  }
	  else if ( ipiv[k] > 1 )
	    return 1;    // singular matrix 1
	}
      }
    }
    ++( ipiv[icol] );
    if (irow != icol) {
      for ( int l=0; l<n; l++ ) 
	swap( a[irow][l], a[icol][l] );
      if ( bvec )
	swap( b[irow], b[icol] );
    }
    indxr[i] = irow;
    indxc[i] = icol;
    if ( a[icol][icol] == 0.0 ) 
      return 2;    /* Singular Matrix-2 */
    double pivinv = 1.0/a[icol][icol];
    a[icol][icol] = 1.0;
    for ( int l=0; l<n; l++) 
      a[icol][l] *= pivinv;
    if ( bvec )
      b[icol] *= pivinv;
    for ( int ll=0; ll<n; ll++ )
      if ( ll != icol ) {
	double dum = a[ll][icol];
	a[ll][icol] = 0.0;
	for ( int l=0; l<n; l++ ) 
	  a[ll][l] -= a[icol][l]*dum;
	if ( bvec )
	  b[ll] -= b[icol]*dum;
      }
  }
  for ( int l=n-1; l>=0; l-- ) {
    if (indxr[l] != indxc[l])
      for ( int k=0; k<n; k++ )
	swap(a[k][indxr[l]],a[k][indxc[l]]);
  }
  return 0;
}


void covarSort( vector< ArrayD > &covar, const ArrayI &paramfit, int mfit )
{
  for ( int i=mfit; i<paramfit.size(); i++ ) {
    for ( int j=0; j<=i; j++ )
      covar[i][j] = covar[j][i] = 0.0;
  }
  int k = mfit;
  for ( int j=paramfit.size()-1; j>=0; j--) {
    if ( paramfit[j] ) {
      k--;
      for ( int i=0; i<paramfit.size(); i++ )
	swap( covar[i][k], covar[i][j] );
      for ( int i=0; i<paramfit.size(); i++ )
	swap( covar[k][i], covar[j][i] );
    }
  }
}


double expFunc( double x, const ArrayD &p )
{
  return p[0] * ::exp( x / p[1] ) + p[2];
}


double expFuncDerivs( double x, const ArrayD &p, ArrayD &dfdp )
{
  double ex = ::exp( x / p[1] );
  double y = p[0] * ex+p[2];
  dfdp[0] = ex;
  dfdp[1] = -p[0] * ex * ( x / p[1] ) / p[1];
  dfdp[2] = 1.0;
  return y;
}


void expGuess( ArrayD &p, double y0, double x1, double y1,
	       double x2, double y2 )
{
  p[2] = y0;
  p[1] = (x1-x2)/::log(::fabs((y1-y0)/(y2-y0)));
  p[0] = (y2-y0)*::exp(-x2/p[1]);
}


double sineFunc( double x, const ArrayD &p )
{
  return p[0] + p[1]*::sin( 2.0*M_PI*p[2]*x + p[3] );
}


double sineFuncDerivs( double x, const ArrayD &p, ArrayD &dfdp )
{
  double t = 2.0*M_PI*x;
  double a = t*p[2] + p[3];
  double s = ::sin( a );
  double c = p[1]*::cos( a );

  dfdp[0] = 1.0;
  dfdp[1] = s;
  dfdp[2] = c*t;
  dfdp[3] = c;

  return p[0]+p[1]*s;
}


}; /* namespace relacs */

