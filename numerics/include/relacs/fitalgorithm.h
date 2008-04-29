/*
  fitalgorithm.h
  

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

#ifndef _RELACS_FITALGORITHM_H_
#define _FITALGORITHM_H

#include <cmath>
#include <vector>
#include <relacs/array.h>
using namespace std;


namespace numerics {

static bool FitFlag = true;

  /*! 
    \a funcs is a function object with the signature
    void funcs( double x, ArrayD &y ) const
    that returns in \a y the values of the first \a y.size()
    basis functions at \a x.
    \return 0: success
    \return 1: no parameters to fit
    \return 2: not enough data points
    \return 16: error 1 in gaussJordan 
    \return 32: error 2 in gaussJordan 
   */
template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterS, typename BasisFunc >
int linearFit( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterY firsty, ForwardIterY lasty,
	       ForwardIterS firsts, ForwardIterS lasts,
	       BasisFunc &funcs, ArrayD &params, const ArrayI &paramfit,
	       ArrayD &uncert, double &chisq  );
template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename BasisFunc >
int linearFit( const ContainerX &x, const ContainerY &y, const ContainerS &s,
	       BasisFunc &funcs, ArrayD &params, const ArrayI &paramfit,
	       ArrayD &uncert, double &chisq );

  /*! Compute the chi squared distance between data values
      in array \a firsty through \a lasty at x-position
      \a firstx through \a lastx with corresponding 
      measurement errors \a firsts through \a lasts and
      the function \a f with parameter values \a params.
      \a f has the signature T f( S x, const ArrayD &params ),
      where \a T is the type of \a *firsty and \a S is the type of \a *firstx. */
template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterS, typename FitFunc >
double chisq( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts,
	      FitFunc &f, ArrayD &params );
  /*! Compute the chi squared distance between data values
      in container \a y at x-position \a x with corresponding 
      measurement errors \a s and
      the function \a f with parameter values \a params.
      \a f has the signature T f( S x, const ArrayD &params ),
      where \a T is the type of \a *firsty and \a S is the type of \a *firstx. */
template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
double chisq( const ContainerX &x, const ContainerY &y, const ContainerS &s,
	      FitFunc &f, ArrayD &params );

  /*! Compute the uncertainties (fit errors) in the parameter \a params
      of the function \a f that was fitted to the data
      in array \a firsty through \a lasty at x-position
      \a firstx through \a lastx with corresponding 
      measurement errors \a firsts through \a lasts.
      If \a uncert[i] > 0 this value is used as a stepsize 
      for computing the derivative with respect to \a param[i].
      If \a uncert[i] <= 0 the default value of 0.001 is used.
      \return 0: success
      \return 64: error 1 in gaussJordan
      \return 128: error 2 in gaussJordan
   */
template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterS, typename FitFunc >
int fitUncertainties( ForwardIterX firstx, ForwardIterX lastx,
		      ForwardIterY firsty, ForwardIterY lasty,
		      ForwardIterS firsts, ForwardIterS lasts,
		      FitFunc &f, const ArrayD &params, const ArrayI &paramfit,
		      ArrayD &uncert );
  /*! Compute the uncertainties (fit errors) in the parameter \a params
      of the function \a f that was fitted to the data
      in container \a y at x-position \a x with corresponding 
      measurement errors \a s.
      If \a uncert[i] > 0 this value is used as a stepsize 
      for computing the derivative with respect to \a param[i].
      If \a uncert[i] <= 0 the default value of 0.001 is used.
      \return 0: success
      \return 64: error 1 in gaussJordan
      \return 128: error 2 in gaussJordan
   */
template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
int fitUncertainties( const ContainerX &x, const ContainerY &y, const ContainerS &s,
		      FitFunc &f, const ArrayD &params,
		      const ArrayI &paramfit, ArrayD &uncert );

  /*! Find the parameter \a params that minimize the function \a f
      using the simplex method.
      \a f has the signature double f( const ArrayD &params ).
      \return 0: success
      \return 1: no parameters to fit
      \return 4: maximum number of iterations exceeded
   */
template < typename MinFunc >
int simplexMin( MinFunc &f, ArrayD &params, const ArrayI &paramfit,
		ArrayD &uncert, double &chi,
		int *iter=NULL, ostream *os=NULL,
		double chieps=0.01, int maxiter=300 );

  /*! Fit the function \a f with parameter \a params
      to the data in array \a firsty through \a lasty at x-position
      \a firstx through \a lastx with corresponding 
      measurement errors \a firsts through \a lasts
      using the simplex method.
      \a f has the signature T f( S x, const ArrayD &params ),
      where \a T is the type of \a *firsty and \a S is the type of \a *firstx.
      \return 0: success
      \return 1: no parameters to fit
      \return 2: not enough data points
      \return 4: maximum number of iterations exceeded
   */
template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterS, typename FitFunc >
int simplexFit( ForwardIterX firstx, ForwardIterX lastx,
		ForwardIterY firsty, ForwardIterY lasty,
		ForwardIterS firsts, ForwardIterS lasts,
		FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		ArrayD &uncert, double &chi,
		int *iter=NULL, ostream *os=NULL,
		double chieps=0.01, int maxiter=300 );
  /*! Fit the function \a f with parameter \a params
      to the data in container \a y at x-position \a x with corresponding 
      measurement errors \a s using the simplex method.
      \return 0: success
      \return 1: no parameters to fit
      \return 2: not enough data points
      \return 4: maximum number of iterations exceeded
   */
template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
int simplexFit( const ContainerX &x, const ContainerY &y, const ContainerS &s,
		FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		ArrayD &uncert, double &chisq,
		int *iter=NULL, ostream *os=NULL,
		double chieps=0.01, int maxiter=300 );


  /*! Fit the function \a f with parameter \a params
      to the data in array \a firsty through \a lasty at x-position
      \a firstx through \a lastx with corresponding 
      measurement errors \a firsts through \a lasts
      using the Levenberg-Marquardt method.
      \a f has the signature T f( S x, const ArrayD &params, ArrayD &dfdp ),
      where \a T is the type of \a *firsty and \a S is the type of \a *firstx.
      \a f returns the value of the functionat \a x 
      for the parameter values \a params and the derivatives
      with respect to the parameters at \a x in \a dfdp.
      \return 0: success
      \return 1: no parameters to fit
      \return 2: not enough data points
      \return 4: maximum number of iterations exceeded
      \return 8: maximum number of not successful iterations exceeded
      \return 16: error 1 in gaussJordan 1 
      \return 32: error 2 in gaussJordan 1 
      \return 64: error 1 in gaussJordan 2
      \return 128: error 2 in gaussJordan 2
   */
template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterS, typename FitFunc >
int marquardtFit ( ForwardIterX firstx, ForwardIterX lastx,
		   ForwardIterY firsty, ForwardIterY lasty,
		   ForwardIterS firsts, ForwardIterS lasts,
		   FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		   ArrayD &uncert, double &chi,
		   int *iter=NULL, ostream *os=NULL,
		   double chieps=0.0005, int maxiter=300 );
  /*! Fit the function \a f with parameter \a params
      to the data in container \a y at x-position \a x with corresponding 
      measurement errors \a s using the Levenberg-Marquardt method.
      \return 0: success
      \return 1: no parameters to fit
      \return 2: not enough data points
      \return 4: maximum number of iterations exceeded
      \return 8: maximum number of not successful iterations exceeded
      \return 16: error 1 in gaussJordan 1 
      \return 32: error 2 in gaussJordan 1 
      \return 64: error 1 in gaussJordan 2
      \return 128: error 2 in gaussJordan 2
   */
template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
int marquardtFit( const ContainerX &x, const ContainerY &y, const ContainerS &s,
		  FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		  ArrayD &uncert, double &chisq,
		  int *iter=NULL, ostream *os=NULL,
		  double chieps=0.0005, int maxiter=300 );

  /*! Returns \f[ p_0 \exp( x / p_1 ) + p_2 \f] */
double expFunc( double x, const ArrayD &p );
double expFuncDerivs( double x, const ArrayD &p, ArrayD &dfdp );
void expGuess( ArrayD &p, double y0, double x1, double y1,
	       double x2, double y2 );

  /*! Returns \f[ p_0 + p_1 \sin( 2 \pi p_2 x + p_3 ) \f] */
double sineFunc( double x, const ArrayD &p );
double sineFuncDerivs( double x, const ArrayD &p, ArrayD &dfdp );


  /*! 0: everything o.k.
      1: Singular Matrix-1 
      2: Singular Matrix-2 */
int gaussJordan( vector< ArrayD > &a, int n, ArrayD &b );

void covarSort( vector< ArrayD > &covar, const ArrayI &paramfit, int mfit );


template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterS, typename BasisFunc >
int linearFit( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterY firsty, ForwardIterY lasty,
	       ForwardIterS firsts, ForwardIterS lasts,
	       BasisFunc &funcs, ArrayD &params, const ArrayI &paramfit,
	       ArrayD &uncert, double &chisq  )
{
  uncert = -1.0;
  chisq = -1.0;

  ArrayD beta( params.size(), 0.0 );
  ArrayD afunc( params.size() );
  vector< ArrayD > covar( params.size(), ArrayD( params.size(), 0.0 ) );

  int mfit = 0;
  for ( int j=0; j<paramfit.size(); j++ ) {
    if ( paramfit[j] )
      mfit++;
  }
  if ( mfit == 0 )
    return 1;
  int nn = 0;
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  ForwardIterS iters = firsts;
  while ( iterx != lastx && itery != lasty && iters != lasts ) {
    ++nn;
    funcs( *iterx, afunc );
    double ym = *itery;
    if ( mfit < params.size() ) {
      for ( int j=0; j<params.size(); j++ ) {
	if ( !paramfit[j] )
	  ym -= params[j]*afunc[j];
      }
    }
    double sig2i = 1.0 / *iters / *iters;
    for ( int j=0, l=0; l<params.size(); l++ ) {
      if ( paramfit[l] ) {
	double wt = afunc[l]*sig2i;
	for ( int k=0, m=0; m<=l; m++ ) {
	  if ( paramfit[m] ) {
	    covar[j][k] += wt*afunc[m];
	    k++;
	  }
	}
	beta[j] += ym*wt;
	j++;
      }
    }
    ++iterx;
    ++itery;
    ++iters;
  }
  if ( nn <= mfit )
    return 2;
  for ( int j=1; j<mfit; j++ ) {
    for ( int k=0; k<j; k++ )
      covar[k][j] = covar[j][k];
  }
  int r = gaussJordan( covar, mfit, beta );
  if ( r > 0 )
    return 16*r;
  for ( int j=0, l=0; l<params.size(); l++ ) {
    if ( paramfit[l] ) {
      params[l] = beta[j];
      j++;
    }
  }
  chisq = 0.0;
  while ( firstx != lastx && firsty != lasty && firsts != lasts ) {
    funcs( *firstx, afunc );
    double sum = 0.0;
    for ( int j=0; j<params.size(); j++ )
      sum += params[j]*afunc[j];
    double ys = (*firsty - sum) / *firsts;
    chisq += ys*ys;
    ++firstx;
    ++firsty;
    ++firsts;
  }
  covarSort( covar, paramfit, mfit );
  for ( int i=0; i<params.size(); i++ )
    uncert[i] = ::sqrt( ::fabs( covar[i][i] ) );
  return 0;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename BasisFunc >
int linearFit( const ContainerX &x, const ContainerY &y, const ContainerS &s,
	       BasisFunc &funcs, ArrayD &params, const ArrayI &paramfit,
	       ArrayD &uncert, double &chisq )
{
  return linearFit( x.begin(), x.end(), y.begin(), y.end(), s.begin(), s.end(),
		    funcs, params, paramfit, uncert, chisq );
}


template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterS, typename FitFunc >
double chisq( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts,
	      FitFunc &f, ArrayD &params )
{
  FitFlag = true;   // new parameters
  double ch = 0.0;
  while ( (firstx != lastx) && (firsty != lasty) && (firsts != lasts) ) {
    double s = ( (*firsty) - f( *firstx, params ) ) / (*firsts);
    ch += s*s;
    ++firstx;
    ++firsty;
    ++firsts;
  }
  return ch;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
double chisq( const ContainerX &x, const ContainerY &y, const ContainerS &s,
	      FitFunc &f, ArrayD &params )
{
  return chisq( x.begin(), x.end(), y.begin(), y.end(), s.begin(), s.end(),
		f, params );
}


template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterS, typename FitFunc >
int fitUncertainties( ForwardIterX firstx, ForwardIterX lastx,
		      ForwardIterY firsty, ForwardIterY lasty,
		      ForwardIterS firsts, ForwardIterS lasts,
		      FitFunc &f, const ArrayD &params, const ArrayI &paramfit,
		      ArrayD &uncert )
{
  const double dp = 0.001;

  // numbers of parameters to be fitted:
  int mfit=0; 
  for ( int j=0; j<paramfit.size(); j++ ) {
    if ( paramfit[j] )
      mfit++;
    if ( uncert[j] <= 0.0 )
      uncert[j] = dp;
  }
  if ( mfit == 0 )
    return 0;

  // initialize:
  vector< ArrayD > alpha( params.size(), ArrayD( params.size(), 0 ) );
  ArrayD dyda( params.size() );
  ArrayD pp( params );

  while ( (firstx != lastx) && (firsty != lasty) && (firsts != lasts) ) {
    FitFlag = true;   // new parameters
    double y = f( (*firstx), pp );
    for ( int j=0, l=0; l<pp.size(); l++ ) {
      if ( paramfit[l] ) {
	FitFlag = true;   // new parameters
	pp[l] += uncert[l];
	double dydl = ( f( (*firstx), pp ) - y ) / uncert[l];
	pp[l] -= uncert[l];
	for ( int k=0, m=0; m<=l; m++ ) {
	  if ( paramfit[m] ) {
	    FitFlag = true;   // new parameters
	    pp[m] += uncert[m];
	    double dydm = ( f( (*firstx), pp ) - y ) / uncert[m];
	    pp[m] -= uncert[m];
	    alpha[j][k++] += (dydl/(*firsts))*(dydm/(*firsts));
	  }
	}
	j++;
      }
    }
    ++firstx;
    ++firsty;
    ++firsts;
  }

  // fill up alpha:
  for ( int j=1; j<mfit; j++ ) {
    for ( int k=0; k<j; k++ )
      alpha[k][j] = alpha[j][k];
  }

  ArrayD emptyb( 0 );
  int gjr = gaussJordan( alpha, mfit, emptyb );
  if ( gjr ) {
    uncert = 0;
    return 64 * gjr;
  }
  covarSort( alpha, paramfit, mfit );
  for ( int j=0; j<pp.size(); j++ )
    uncert[j] = ::sqrt( ::fabs( alpha[j][j] ) );
  
  return 0;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
int fitUncertainties( const ContainerX &x, const ContainerY &y, const ContainerS &s,
		      FitFunc &f, const ArrayD &params, const ArrayI &paramfit,
		      ArrayD &uncert )
{
  return fitUncertainties( x.begin(), x.end(), y.begin(), y.end(), s.begin(), s.end(),
			   f, params, paramfit, uncert );
}


template < typename MinFunc >
double simplexMinTry( vector< ArrayD > &p, ArrayD &y, ArrayD &psum,
	              int ihi, double fac, int mfit,
		      MinFunc &f, ArrayD &params, const ArrayI &paramfit )
{
  ArrayD ptry( params.size() );
  double fac1 = (1.0-fac)/mfit;
  double fac2 = fac1-fac;
  for ( int k=0, j=0; j<params.size(); j++ ) {
    if ( paramfit[j] )
      ptry[j] = psum[j]*fac1 - p[ihi][k++]*fac2;
    else
      ptry[j] = params[j];
  }
  double ytry = f( ptry );
  if ( ytry < y[ihi] ) {
    // accept new point:
    y[ihi] = ytry;
    for ( int k=0, j=0; j<params.size(); j++ ) {
      if ( paramfit[j] ) {
	psum[j] += ptry[j] - p[ihi][k];
	p[ihi][k] = ptry[j];
	k++;
      }
    }
  }
  return ytry;
}


template < typename MinFunc >
int simplexMin( MinFunc &f, ArrayD &params, const ArrayI &paramfit,
		ArrayD &uncert, double &chi,
		int *iter, ostream *os,
		double chieps, int maxiter )
{
  if ( iter != NULL )
    *iter = 0;
  // numbers of parameters to be fitted:
  int mfit=0; 
  for ( int j=0; j<paramfit.size(); j++ ) {
    if ( paramfit[j] )
      mfit++;
  }
  if ( mfit==0 ) {
    if ( os != NULL )
      *os << "exit: " << 1 << "\n\n";
    return 1;
  }
  // number of simplex points:
  int mpts = mfit+1;
  // parameter values for each point of the simplex:
  vector< ArrayD > p( mpts, ArrayD( mfit, 0 ) );
  // initialize first point of simplex:
  for ( int j=0, i=0; i<params.size(); i++ ) {
    if ( paramfit[i] )
      p[0][j++] = params[i];
  }
  // chi-squared values at each simplex point:
  ArrayD y( mpts, 0 );
  y[0] = f( params );
  ArrayD psum( params );
  // initialize remaining simplex:
  for ( int j=1; j<mpts; j++ ) {
    for ( int k=0, i=0; i<params.size(); i++ ) {
      if ( paramfit[i] ) {
	if ( k+1 == j )
	  p[j][k] = p[0][k] + uncert[i];
	else
	  p[j][k] = p[0][k];
	psum[i] = p[j][k];
	k++;
      }
    }
    y[j] = f( psum );
  }
  // get psum (sum of each simplex coordinate):
  for ( int k=0, j=0; j<params.size(); j++ ) {
    if ( paramfit[j] ) {
      double sum=0.0;
      for ( int i=0; i<mpts; i++ )
	sum += p[i][k];
      psum[j] = sum;
      k++;
    }
  }

  int ilo = 0;    // lowest point in simplex
  int ihi = 0;    // highest point
  int inhi = 0;   // second highest point in simplex

  // report:
  if ( os != NULL ) {
    *os << "Iter  Chi[low]      Chih/Chil-1 ";
    for ( int j=0; j<params.size(); j++ )
      *os << "  a[" << setw( 2 ) << j << "]       ";
    *os << '\n';
    *os << "   0           0.0           0.0";
    for ( int i=0, j=0; j<params.size(); j++ ) {
      if ( paramfit[j] )
	*os << "  " << setw( 12 ) << setprecision( 6 ) << p[0][i++];
      else
	*os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
    }
    *os << '\n';
  }

  int fiterror = 0;
  
  // iteration:
  int itercount = 0;
  for ( ; ; itercount++ ) {

    if ( iter != NULL )
      *iter = itercount;

    // analyse simplex:
    ilo = 0;                                   // lowest point in simplex
    inhi = 0;                                  // second highest point in simplex
    ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1); // highest point
    for ( int i=0; i<mpts; i++ ) {
      if ( y[i] <= y[ilo] )
	ilo = i;
      if ( y[i] > y[ihi] ) {
	inhi = ihi;
	ihi = i;
      }
      else if ( y[i] > y[inhi] && i != ihi )
	inhi = i;
    }

    // report state:
    if ( os != NULL ) {
      *os << setw( 4 ) << itercount
	 << "  " << setw( 12 ) << setprecision( 5 ) << y[ilo]
	 << "  "  << setw( 12 ) << setprecision( 5 ) << y[ihi]/y[ilo]-1.0;
      for ( int i=0, j=0; j<params.size(); j++ ) {
	if ( paramfit[j] )
	  *os << "  " << setw( 12 ) << setprecision( 6 ) << p[ilo][i++];
	else
	  *os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
      }
      *os << '\n';
    }

    // success?
    if ( fabs( y[ihi]/y[ilo] - 1.0 ) < chieps )
      break;

    // too many iterations?
    if ( itercount >= maxiter ) {
      fiterror = 4;
      break;
    }

    // flip highest point to other side of simplex:
    double ytry = simplexMinTry( p, y, psum, ihi, -1.0, mfit,
				 f, params, paramfit );
    if ( ytry <= y[ilo] ) {
      // extend flipped point by factor 2:
      ytry = simplexMinTry( p, y, psum, ihi, 2.0, mfit,
			    f, params, paramfit );
      // report action:
      if ( os != NULL ) {
	if ( ytry <= y[ihi] )
	  *os << "flipped highest point " << ihi << " by factor 2\n";
	else
	  *os << "flipped highest point " << ihi << "\n";
      }
    }
    else if ( ytry >= y[inhi] ) {
      double ysave = y[ihi];
      // move (possibly) flipped point closer to the other points:
      ytry = simplexMinTry( p, y, psum, ihi, 0.5, mfit,
			    f, params, paramfit );
      if ( ytry >= ysave ) {
	// failed to improve highest point:
	for ( int i=0; i<mpts; i++ ) {
	  if ( i != ilo ) {
	    // move all points closer to lowest point:
	    for ( int k=0, j=0; j<params.size(); j++ ) {
	      if ( paramfit[j] ) {
		p[i][k] = psum[j] = 0.5*(p[i][k]+p[ilo][k]);
		k++;
	      }
	    }
	    y[i] = f( psum );
	  }
	}
	// get psum (sum of each simplex coordinate):
	for ( int k=0, j=0; j<params.size(); j++ ) {
	  if ( paramfit[j] ) {
	    double sum=0.0;
	    for ( int i=0; i<mpts; i++ )
	      sum += p[i][k];
	    psum[j] = sum;
	    k++;
	  }
	}
	// report action:
	if ( os != NULL )
	  *os << "moved all points closer to the lowest point " << ilo << "\n";
      }
      else {
	// report action:
	if ( os != NULL )
	  *os << "moved highest point " << ihi << " closer to the other points\n";
      }
    }
    else {
      // report action:
      if ( os != NULL )
	*os << "flipped highest point " << ihi << "\n";
    }
  }

  // get fitted parameter:
  for ( int k=0, i=0; i<params.size(); i++ ) {
    if ( paramfit[i] )
      params[i] = p[ilo][k++];
  }
  chi = y[ilo];

  // report result:
  if ( os != NULL ) {
    *os << "exit: " << fiterror << '\n';
    *os << setw( 4 ) << itercount
       << "  " << setw( 12 ) << setprecision( 5 ) << chi
       << "  "  << setw( 12 ) << setprecision( 5 ) << y[ihi]/chi-1.0;
    for ( int j=0; j<params.size(); j++ )
      *os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
    *os << "\n\n";
  }

  return fiterror;
}


template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterS, typename FitFunc >
double simplexFitTry( vector< ArrayD > &p, ArrayD &y, ArrayD &psum,
	              int ihi, double fac, int mfit,
		      ForwardIterX firstx, ForwardIterX lastx,
		      ForwardIterY firsty, ForwardIterY lasty,
		      ForwardIterS firsts, ForwardIterS lasts,
		      FitFunc &f, ArrayD &params, const ArrayI &paramfit )
{
  ArrayD ptry( params.size() );
  double fac1 = (1.0-fac)/mfit;
  double fac2 = fac1-fac;
  for ( int k=0, j=0; j<params.size(); j++ ) {
    if ( paramfit[j] )
      ptry[j] = psum[j]*fac1 - p[ihi][k++]*fac2;
    else
      ptry[j] = params[j];
  }
  double ytry = chisq( firstx, lastx, firsty, lasty, firsts, lasts,
		       f, ptry );
  if ( ytry < y[ihi] ) {
    // accept new point:
    y[ihi] = ytry;
    for ( int k=0, j=0; j<params.size(); j++ ) {
      if ( paramfit[j] ) {
	psum[j] += ptry[j] - p[ihi][k];
	p[ihi][k] = ptry[j];
	k++;
      }
    }
  }
  return ytry;
}


template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterS, typename FitFunc >
int simplexFit( ForwardIterX firstx, ForwardIterX lastx,
		ForwardIterY firsty, ForwardIterY lasty,
		ForwardIterS firsts, ForwardIterS lasts,
		FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		ArrayD &uncert, double &chi,
		int *iter, ostream *os,
		double chieps, int maxiter )
{
  if ( iter != NULL )
    *iter = 0;
  // numbers of parameters to be fitted:
  int mfit=0; 
  for ( int j=0; j<paramfit.size(); j++ ) {
    if ( paramfit[j] )
      mfit++;
  }
  if ( mfit==0 ) {
    if ( os != NULL )
      *os << "exit: " << 1 << "\n\n";
    return 1;
  }
  if ( lastx - firstx <= mfit ) {
    if ( os != NULL )
      *os << "exit: " << 2 << "\n\n";
    return 2;
  }
  // number of simplex points:
  int mpts = mfit+1;
  // parameter values for each point of the simplex:
  vector< ArrayD > p( mpts, ArrayD( mfit, 0 ) );
  // initialize first point of simplex:
  for ( int j=0, i=0; i<params.size(); i++ ) {
    if ( paramfit[i] )
      p[0][j++] = params[i];
  }
  // chi-squared values at each simplex point:
  ArrayD y( mpts, 0 );
  y[0] = chisq( firstx, lastx, firsty, lasty, firsts, lasts, f, params );
  ArrayD psum( params );
  // initialize remaining simplex:
  for ( int j=1; j<mpts; j++ ) {
    for ( int k=0, i=0; i<params.size(); i++ ) {
      if ( paramfit[i] ) {
	if ( k+1 == j )
	  p[j][k] = p[0][k] + uncert[i];
	else
	  p[j][k] = p[0][k];
	psum[i] = p[j][k];
	k++;
      }
    }
    y[j] = chisq( firstx, lastx, firsty, lasty, firsts, lasts, f, psum );
  }
  // get psum (sum of each simplex coordinate):
  for ( int k=0, j=0; j<params.size(); j++ ) {
    if ( paramfit[j] ) {
      double sum=0.0;
      for ( int i=0; i<mpts; i++ )
	sum += p[i][k];
      psum[j] = sum;
      k++;
    }
  }

  int ilo = 0;    // lowest point in simplex
  int ihi = 0;    // highest point
  int inhi = 0;   // second highest point in simplex

  // report:
  if ( os != NULL ) {
    *os << "Iter  Chi[low]      Chih/Chil-1 ";
    for ( int j=0; j<params.size(); j++ )
      *os << "  a[" << setw( 2 ) << j << "]       ";
    *os << '\n';
    *os << "   0           0.0           0.0";
    for ( int i=0, j=0; j<params.size(); j++ ) {
      if ( paramfit[j] )
	*os << "  " << setw( 12 ) << setprecision( 6 ) << p[0][i++];
      else
	*os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
    }
    *os << '\n';
  }

  int fiterror = 0;
  
  // iteration:
  int itercount = 0;
  for ( ; ; itercount++ ) {

    if ( iter != NULL )
      *iter = itercount;

    // analyse simplex:
    ilo = 0;                                   // lowest point in simplex
    inhi = 0;                                  // second highest point in simplex
    ihi = y[0]>y[1] ? (inhi=1,0) : (inhi=0,1); // highest point
    for ( int i=0; i<mpts; i++ ) {
      if ( y[i] <= y[ilo] )
	ilo = i;
      if ( y[i] > y[ihi] ) {
	inhi = ihi;
	ihi = i;
      }
      else if ( y[i] > y[inhi] && i != ihi )
	inhi = i;
    }

    // report state:
    if ( os != NULL ) {
      *os << setw( 4 ) << itercount
	 << "  " << setw( 12 ) << setprecision( 5 ) << y[ilo]
	 << "  "  << setw( 12 ) << setprecision( 5 ) << y[ihi]/y[ilo]-1.0;
      for ( int i=0, j=0; j<params.size(); j++ ) {
	if ( paramfit[j] )
	  *os << "  " << setw( 12 ) << setprecision( 6 ) << p[ilo][i++];
	else
	  *os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
      }
      *os << '\n';
    }

    // success?
    if ( fabs( y[ihi]/y[ilo] - 1.0 ) < chieps )
      break;

    // too many iterations?
    if ( itercount >= maxiter ) {
      fiterror = 4;
      break;
    }

    // flip highest point to other side of simplex:
    double ytry = simplexFitTry( p, y, psum, ihi, -1.0, mfit,
				 firstx, lastx, firsty, lasty, firsts, lasts,
				 f, params, paramfit );
    if ( ytry <= y[ilo] ) {
      // extend flipped point by factor 2:
      ytry = simplexFitTry( p, y, psum, ihi, 2.0, mfit,
			    firstx, lastx, firsty, lasty, firsts, lasts,
			    f, params, paramfit );
      // report action:
      if ( os != NULL ) {
	if ( ytry <= y[ihi] )
	  *os << "flipped highest point " << ihi << " by factor 2\n";
	else
	  *os << "flipped highest point " << ihi << "\n";
      }
    }
    else if ( ytry >= y[inhi] ) {
      double ysave = y[ihi];
      // move (possibly) flipped point closer to the other points:
      ytry = simplexFitTry( p, y, psum, ihi, 0.5, mfit,
			    firstx, lastx, firsty, lasty, firsts, lasts,
			    f, params, paramfit );
      if ( ytry >= ysave ) {
	// failed to improve highest point:
	for ( int i=0; i<mpts; i++ ) {
	  if ( i != ilo ) {
	    // move all points closer to lowest point:
	    for ( int k=0, j=0; j<params.size(); j++ ) {
	      if ( paramfit[j] ) {
		p[i][k]=psum[j]=0.5*(p[i][k]+p[ilo][k]);
		k++;
	      }
	    }
	    y[i] = chisq( firstx, lastx, firsty, lasty, firsts, lasts, 
			  f, psum );
	  }
	}
	// get psum (sum of each simplex coordinate):
	for ( int k=0, j=0; j<params.size(); j++ ) {
	  if ( paramfit[j] ) {
	    double sum=0.0;
	    for ( int i=0; i<mpts; i++ )
	      sum += p[i][k];
	    psum[j] = sum;
	    k++;
	  }
	}
	// report action:
	if ( os != NULL )
	  *os << "moved all points closer to the lowest point " << ilo << "\n";
      }
      else {
	// report action:
	if ( os != NULL )
	  *os << "moved highest point " << ihi << " closer to the other points\n";
      }
    }
    else {
      // report action:
      if ( os != NULL )
	*os << "flipped highest point " << ihi << "\n";
    }
  }

  // get fitted parameter:
  for ( int k=0, i=0; i<params.size(); i++ ) {
    if ( paramfit[i] )
      params[i] = p[ilo][k++];
  }
  chi = y[ilo];

  // report result:
  if ( os != NULL ) {
    *os << "exit: " << fiterror << '\n';
    *os << setw( 4 ) << itercount
       << "  " << setw( 12 ) << setprecision( 5 ) << chi
       << "  "  << setw( 12 ) << setprecision( 5 ) << y[ihi]/chi-1.0;
    for ( int j=0; j<params.size(); j++ )
      *os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
    *os << "\n\n";
  }

  return fiterror;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
int simplexFit( const ContainerX &x, const ContainerY &y, const ContainerS &s,
		FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		ArrayD &uncert, double &chisq,
		int *iter, ostream *os,
		double chieps, int maxiter )
{
  return simplexFit( x.begin(), x.end(), y.begin(), y.end(), s.begin(), s.end(),
		     f, params, paramfit, uncert, chisq, iter, os, chieps, maxiter );
}


/*! \todo alpha[k] = 0.5*Rho... oder = Rho... ?????  check in Numerical Recipes! */
template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterS, typename FitFunc >
void marquardtCof( ForwardIterX firstx, ForwardIterX lastx,
		   ForwardIterY firsty, ForwardIterY lasty,
		   ForwardIterS firsts, ForwardIterS lasts,
		   FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		   int mfit, double &chisq,
		   vector< ArrayD > &alpha, ArrayD &beta )
{
  // initialize:
  for ( int j=0; j<mfit; j++ ) {
    alpha[j] = 0.0;
    beta[j] = 0.0;
  }
  chisq = 0.0;
  ArrayD dyda( params.size() );

  FitFlag = true;   // new parameters
  while ( (firstx != lastx) && (firsty != lasty) && (firsts != lasts) ) {
    double s = ( (*firsty) - f( *firstx, params, dyda ) ) / (*firsts);
    chisq += s*s;
    for ( int j=0, l=0; l<params.size(); l++ ) {
      if ( paramfit[l] ) {
	for ( int k=0, m=0; m<=l; m++ ) {
	  if ( paramfit[m] )
	    alpha[j][k++] += 0.5*(dyda[l]/(*firsts))*(dyda[m]/(*firsts));
	}
	beta[j] += s*dyda[l]/(*firsts);
	j++;
      }
    }
    ++firstx;
    ++firsty;
    ++firsts;
  }

  // fill up alpha:
  for ( int j=1; j<mfit; j++ ) {
    for ( int k=0; k<j; k++ )
      alpha[k][j] = alpha[j][k];
  }
}


template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterS, typename FitFunc >
int marquardtFit ( ForwardIterX firstx, ForwardIterX lastx,
		   ForwardIterY firsty, ForwardIterY lasty,
		   ForwardIterS firsts, ForwardIterS lasts,
		   FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		   ArrayD &uncert, double &chi,
		   int *iter, ostream *os,
		   double chieps, int maxiter )
{
  const double chigood = 1.0e-8;
  const int maxsearch = 4;
  const int miniter = 30;
  //  const double lambdastart = 0.01;
  const double lambdastart = 1.0;
  const double lambdafac = 10.0;

  // initialize:
  for ( int k=0; k<params.size(); k++ )
    uncert[k] = HUGE_VAL;
  if ( iter != NULL )
    *iter = 0;
  chi = HUGE_VAL;
  // numbers of parameters to be fitted:
  int mfit = 0;
  for ( int j=0; j<paramfit.size(); j++ ) {
    if ( paramfit[j] )
      mfit++;
  }
  if ( mfit==0 ) {
    if ( os != NULL )
      *os << "exit: " << 1 << "\n\n";
    return 1;
  }
  if ( lastx - firstx <= mfit ) {
    if ( os != NULL )
      *os << "exit: " << 2 << "\n\n";
    return 2;
  }

  double alambda = lambdastart;
  double chisq = 0.0;
  vector< ArrayD > alpha( params.size(), ArrayD( params.size(), 0 ) );
  vector< ArrayD > covar( params.size(), ArrayD( params.size(), 0 ) );
  ArrayD beta( params.size() );
  ArrayD oneda( params.size() );
  ArrayD da( params.size() );
  ArrayD atry( params );
  ArrayD emptyb( 0 );
  marquardtCof( firstx, lastx, firsty, lasty, firsts, lasts, 
		f, params, paramfit, mfit, chisq, alpha, beta );
  double ochisq = chisq;

  // report start values:
  if ( os != NULL ) {
    *os << "Iter  S   alambda     ChiSq         1-oChi/Chi  ";
    for ( int j=0; j<params.size(); j++ )
      *os << "  a[" << setw( 2 ) << j << "]       ";
    *os << '\n';
    *os << setw( 4 ) << 0 << "  "
       << setw( 2 ) << 0 << "  "
       << setw( 10 ) << setprecision( 3 ) << alambda << "  "
       << setw( 12 ) << setprecision( 5 ) << chisq << "  "
       << setw( 12 ) << setprecision( 5 ) << ::fabs(1.0-ochisq/chisq);
    for ( int j=0; j<params.size(); j++ )
      *os << "  " << setw( 12 ) << setprecision( 6 ) << atry[j];
    *os << '\n';
  }

  // iterate until maxsearch successfull iterations or
  // miniter unsuccsessful successive iterations or
  // more than maxiter iterations are done:
  int notbetter = 0;
  int iteration=0;
  for ( int search=0;
        (search<maxsearch) && iteration<=maxiter && notbetter < miniter;
        iteration++ ) {
    // calculate matrix elements:
    for ( int j=0; j<mfit; j++ ) {
      for ( int k=0; k<mfit; k++ )
	covar[j][k] = alpha[j][k];
      covar[j][j] = alpha[j][j]*(1.0+alambda);
      oneda[j] = beta[j];
    }
    // solve marix:
    int gjr = gaussJordan( covar, mfit, oneda );
    if ( gjr ) {
      if ( iter != NULL )
	*iter = iteration;
      if ( os != NULL )
	*os << "exit from gaussJordan: " << gjr << "\n\n";
      return 16*gjr;
    }

    for ( int j=0; j<mfit; j++ )
      da[j] = oneda[j];
    for ( int j=0, l=0; l<params.size(); l++ ) {
      if ( paramfit[l] )
	atry[l] = params[l] + da[j++];
    }
    marquardtCof( firstx, lastx, firsty, lasty, firsts, lasts,
		  f, atry, paramfit, mfit, chisq, covar, da );

    // report current iteration step:
    if ( os != NULL )	{
      *os << setw( 4 ) << iteration << "  "
	 << setw( 2 ) << search << "  "
	 << setw( 10 ) << setprecision( 3 ) << alambda << "  "
	 << setw( 12 ) << setprecision( 5 ) << chisq << "  "
	 << setw( 12 ) << setprecision( 5 ) << ::fabs(1.0-ochisq/chisq);
      for ( int j=0; j<params.size(); j++ )
	*os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
      *os << '\n';
    }

    // success?
    if ( chisq < ochisq + chigood ) {
      if ( fabs(1.0-ochisq/chisq) < chieps )
	search++;
      alambda /= lambdafac;
      ochisq=chisq;
      for ( int j=0; j<mfit; j++ ) {
	for ( int k=0; k<mfit; k++ )
	  alpha[j][k] = covar[j][k];
	beta[j] = da[j];
      }
      params = atry;
      notbetter = 0;
    }
    else {
      alambda *= lambdafac*lambdafac;
      chisq = ochisq;
      notbetter++;
    }
  }

  if ( iter != NULL )
    *iter = iteration;

  if ( notbetter >= miniter ) {
    if ( os != NULL )
      *os << "exit: " << 8 << "\n\n";
    return 8;
  }

  // calculate uncertainties:
  for ( int j=0; j<mfit; j++ )
    covar[j] = alpha[j];
  int gjr = gaussJordan( covar, mfit, emptyb );
  if ( gjr ) {
    if ( os != NULL )
      *os << "exit from final gaussJordan: " << gjr << "\n\n";
    return 64 * gjr;
  }
  covarSort( covar, paramfit, mfit );
  for ( int j=0; j<params.size(); j++ )
    uncert[j] = ::sqrt( ::fabs( covar[j][j] ) );

  chi = chisq;

  int fiterror = iteration > maxiter ? 4 : 0;
  if ( os != NULL )
    *os << "exit: " << fiterror << "\n\n";
  return fiterror;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerS, typename FitFunc >
int marquardtFit( const ContainerX &x, const ContainerY &y, const ContainerS &s,
		  FitFunc &f, ArrayD &params, const ArrayI &paramfit,
		  ArrayD &uncert, double &chisq,
		  int *iter, ostream *os,
		  double chieps, int maxiter )
{
  return marquardtFit( x.begin(), x.end(), y.begin(), y.end(), s.begin(), s.end(),
		       f, params, paramfit, uncert, chisq, iter, os, chieps, maxiter );
}

};

#endif /* ! _RELACS_FITALGORITHM_H_ */
