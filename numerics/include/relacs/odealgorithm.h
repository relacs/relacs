/*
  odealgorithm.h
  

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

#ifndef _RELACS_ODEALGORITHM_H_
#define _RELACS_ODEALGORITHM_H_ 1

#include <cmath>

namespace relacs {


template < class Derivs >
void eulerStep( double x, double *y, double *dydx, int n,
		double deltax, Derivs &f );
  /*! Calculates a single Euler forward step for the set of ordinary
      differential equations dy/dx = f(y(x),x).
      \parameter \a YVector an array type containing numbers.
                 needs int YVector::size(),
                 YVector::iterator, YVector::const_iterator
      \parameter \a Derivs a functor with a function 
                 f( XValue x, const YVector &y, YVector &dydx )
                 returning the derivative f(y,x) in \a dydx.
      \parameter \a x 
      \parameter \a y, the size of \a y determines the dimension of the system.
      \parameter \a dydx workspace for keeping the derivative.
      \parameter \a deltax the step size
      \parameter \a f the class calculating the derivative
      \return \a y is updated.
   */
template < class YVector, class Derivs >
void eulerStep( double x, YVector &y, YVector &dydx,
		double deltax, Derivs &f );
  /*! \a x1 must be smaller than \a x2. \a deltax must be positive. */
template < class XVector, class YVector, class YMatrix, class Derivs >
int eulerInt( XVector &x, YMatrix &y, const YVector &ystart,
	      double x1, double x2, double deltax, Derivs &f );

template < class Derivs >
void midpointStep( double x, double *y, double *dydx, int n,
		   double deltax, Derivs &f );
  /*! \a dydx and \a yt need to be arrays of the same size as \a y
      and are used as workspace. */
template < class YVector, class Derivs >
void midpointStep( double x, YVector &y, YVector &dydx, YVector &yt,
		   double deltax, Derivs &f );
template < class XVector, class YVector, class YMatrix, class Derivs >
int midpointInt( XVector &x, YMatrix &y, const YVector &ystart,
		 double x1, double x2, double deltax, Derivs &f );

template < class Derivs >
void rk4Step( double x, double *y, double *dydx, int n,
	      double deltax, Derivs &f );


template < class Derivs >
void eulerStep( double x, double *y, double *dydx, int n,
		double deltax, Derivs &f )
{
  f( x, y, dydx, n );
  for ( int k=0; k<n; k++ )
    y[k] += deltax*dydx[k];
}


template < class YVector, class Derivs >
void eulerStep( double x, YVector &y, YVector &dydx,
		double deltax, Derivs &f )
{
  f( x, y, dydx );
  typename YVector::iterator yi = y.begin();
  typename YVector::const_iterator dydxi = dydx.begin();
  for ( ; yi != y.end(); ++yi, ++dydxi )
    *yi += *dydxi * deltax;
}

 
template < class XVector, class YVector, class YMatrix, class Derivs >
int eulerInt( XVector &x, YMatrix &y, const YVector &ystart,
	      double x1, double x2, double deltax, Derivs &f )
{
  // boundaries:
  if ( x2 <= x1 ) {
    x.clear();
    y.clear();
    return 2; // empty range
  }

  // stepsize:
  if ( deltax <= 0.0 ||
       x1+deltax <= x1 ) {
    x.clear();
    y.clear();
    return 1; // step size too small
  }

  // dimension:
  int m = (int)ystart.size();
  if ( (int)y.size() < m ) {
    x.clear();
    y.clear();
    return 4; // not the right dimension
  }

  // steps:
  int n = (int)::floor( (x2-x1)/deltax ) + 1;
  if ( n > (int)x.size() )
    n = (int)x.size();
  if ( n > (int)y[0].size() )
    n = (int)y[0].size();

  // initial conditions:
  typename XVector::iterator xi = x.begin();
  double xx = x1;
  *xi = xx;
  ++xi;
  typename YMatrix::value_type::iterator yi[ m ];
  typename YMatrix::iterator yii = y.begin();
  typename YVector::const_iterator ysi = ystart.begin();
  for ( int k=0; k<m; ++k, ++yii, ++ysi ) {
    yi[k] = yii->begin();
    *yi[k] = *ysi;
  }
  YVector yy( ystart );
  YVector dydx( ystart.size() );

  // iteration:
  for ( int i=1; i<n; ++i, ++xi ) {
    f( xx, yy, dydx );
    xx = x1 + i * deltax;
    *xi = xx;
    typename YVector::const_iterator dydxi = dydx.begin();
    typename YVector::iterator yyi = yy.begin();
    for ( int k=0; k<m; ++k, ++dydxi, ++yyi ) {
      *yyi += *dydxi * deltax;
      *(++yi[k]) = *yyi;
    }
  }

  return 0;
}


template < class Derivs >
void midpointStep( double x, double *y, double *dydx, int n,
		   double deltax, Derivs &f )
{
  f( x, y, dydx, n );
  double yt[n];
  for ( int i=0; i<n; i++ ) 
    yt[i] = y[i]+ 0.5*deltax*dydx[i];
  f( x+0.5*deltax, yt, dydx, n );
  for ( int i=0; i<n; i++ ) 
    y[i] += deltax*dydx[i];
}


template < class YVector, class Derivs >
void midpointStep( double x, YVector &y, YVector &dydx, YVector &yt,
		   double deltax, Derivs &f )
{
  f( x, y, dydx );
  typename YVector::iterator yi = y.begin();
  typename YVector::const_iterator dydxi = dydx.begin();
  typename YVector::iterator yti = yt.begin();
  for ( ; yi != y.end(); ++yi, ++dydxi, ++yti ) 
    *yti = *yi + *dydxi * 0.5 * deltax;

  f( x + 0.5 * deltax, yt, dydx );
  yi = y.begin();
  dydxi = dydx.begin();
  for ( ; yi != y.end(); ++yi, ++dydxi ) 
    *yi += *dydxi * deltax;
}


template < class XVector, class YVector, class YMatrix, class Derivs >
int midpointInt( XVector &x, YMatrix &y, const YVector &ystart,
		 double x1, double x2, double deltax, Derivs &f )
{
  // boundaries:
  if ( x2 <= x1 ) {
    x.clear();
    y.clear();
    return 2; // empty range
  }

  // stepsize:
  if ( deltax <= 0.0 ||
       x1+deltax <= x1 ) {
    x.clear();
    y.clear();
    return 1; // step size too small
  }

  // dimension:
  int m = (int)ystart.size();
  if ( (int)y.size() < m ) {
    x.clear();
    y.clear();
    return 4; // not the right dimension
  }

  // steps:
  int n = (int)::floor( (x2-x1)/deltax ) + 1;
  if ( n > (int)x.size() )
    n = (int)x.size();
  if ( n > (int)y[0].size() )
    n = (int)y[0].size();

  // initial conditions:
  typename XVector::iterator xi = x.begin();
  double xx = x1;
  *xi = xx;
  ++xi;
  typename YMatrix::value_type::iterator yi[ m ];
  typename YMatrix::iterator yii = y.begin();
  typename YVector::const_iterator ysi = ystart.begin();
  for ( int k=0; k<m; ++k, ++yii, ++ysi ) {
    yi[k] = yii->begin();
    *yi[k] = *ysi;
  }
  YVector yy( ystart );
  YVector dydx( ystart.size() );
  YVector yt( ystart.size() );

  // iteration:
  for ( int i=1; i<n; ++i, ++xi ) {
    f( xx, yy, dydx );
    xx = x1 + i * deltax;
    *xi = xx;
    typename YVector::iterator yyi = yy.begin();
    typename YVector::const_iterator dydxi = dydx.begin();
    typename YVector::iterator yti = yt.begin();
    for ( ; yyi != yy.end(); ++yyi, ++dydxi, ++yti ) 
      *yti = *yyi + *dydxi * 0.5 * deltax;
    f( xx - 0.5 * deltax, yt, dydx );
    yyi = yy.begin();
    dydxi = dydx.begin();
    for ( int k=0; k<m; ++k, ++dydxi, ++yyi ) {
      *yyi += *dydxi * deltax;
      *(++yi[k]) = *yyi;
    }
  }

  return 0;
}


template < class Derivs >
void rk4Step( double x, double *y, double *dydx, int n,
	      double deltax, Derivs &f )
{

  double hh  = deltax/2.0;
  double h6 = deltax/6.0;
  double xh = x+hh;
  double dym[n];
  double dyt[n];
  double yt[n];

  f( x, y, dydx, n );
  for ( int k=0; k<n; k++ ) 
    yt[k] = y[k]+hh*dydx[k];  
  f( xh, yt, dyt, n );
  for ( int k=0; k<n; k++ )
    yt[k] = y[k]+hh*dyt[k];  
  f( xh, yt, dym, n );
  for ( int k=0; k<n; k++ ) {
    yt[k] = y[k]+deltax*dym[k];
    dym[k] += dyt[k];  
  }
  f( x+deltax, yt, dyt, n );
  for ( int k=0; k<n; k++ )
    y[k] += h6*(dydx[k]+dyt[k]+2.0*dym[k]);
}


#ifdef NOTHING


void RK4Step( double *y, double *dydx, int nvar, double x, double h, double *yout,
	      derivsfunc derivs )
{
  int i;
  double xh,hh,h6;
  double *dym,*dyt,*yt;
  
  dym=dvector(nvar);
  dyt=dvector(nvar);
  yt=dvector(nvar);
  hh=h*0.5;
  h6=h/6.0;
  xh=x+hh;
  for (i=0;i<nvar;i++) 
    yt[i]=y[i]+hh*dydx[i];
  derivs(xh,yt,dyt,nvar);
  for (i=0;i<nvar;i++)
    yt[i]=y[i]+hh*dyt[i];
  derivs(xh,yt,dym,nvar);
  for (i=0;i<nvar;i++) 
    {
      yt[i]=y[i]+h*dym[i];
      dym[i] += dyt[i];
    }
  derivs(x+h,yt,dyt,nvar);
  for (i=0;i<nvar;i++)
    yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.0*dym[i]);
  free_dvector(yt);
  free_dvector(dyt);
  free_dvector(dym);
}


int RK4Int( REAL *xd, REAL **yd, int nvar, int maxstep, int *kount,
	    double *ystart, double x1, double x2, double deltax, 
	    derivsfunc derivs )
{
  int i,k;
  double x;
  double *v, *vout, *dv;
  
  maxstep--;
  if ( deltax <= 0.0 )
    deltax=(x2-x1)/maxstep;
  xd[0]=x1;
  x=x1;
  if ((REAL)(x+deltax) == x) 
    {
      if ( kount != NULL )
	*kount = 0;
      return( 1 ); /* Step size too small */
    }
  v=dvector(nvar);
  vout=dvector(nvar);
  dv=dvector(nvar);
  for (i=0;i<nvar;i++) 
    {
      v[i]=ystart[i];
      yd[i][0]=v[i];
    }
  for (k=0; k<maxstep && x<x2; k++) 
    {
      derivs(x,v,dv,nvar);
      RK4Step(v,dv,nvar,x,deltax,vout,derivs);
      x += deltax;
      xd[k+1]=x;
      for (i=0;i<nvar;i++) 
	{
	  v[i]=vout[i];
	  yd[i][k+1]=v[i];
	}
    }
  if ( kount != NULL )
    *kount = k+1;
  free_dvector(dv);
  free_dvector(vout);
  free_dvector(v);
  if ( x<x2 )
    return( 8 );
  else
    return( 0 );
}


void rkck( double *y, double *dydx, int n, double x, double h, double *yout,
	   double *yerr, derivsfunc derivs )
{
  int i;
  static double a2=0.2,a3=0.3,a4=0.6,a5=1.0,a6=0.875,b21=0.2,
    b31=3.0/40.0,b32=9.0/40.0,b41=0.3,b42 = -0.9,b43=1.2,
    b51 = -11.0/54.0, b52=2.5,b53 = -70.0/27.0,b54=35.0/27.0,
    b61=1631.0/55296.0,b62=175.0/512.0,b63=575.0/13824.0,
    b64=44275.0/110592.0,b65=253.0/4096.0,c1=37.0/378.0,
    c3=250.0/621.0,c4=125.0/594.0,c6=512.0/1771.0,
    dc5 = -277.00/14336.0;
  double dc1=c1-2825.0/27648.0,dc3=c3-18575.0/48384.0,
    dc4=c4-13525.0/55296.0,dc6=c6-0.25;
  double *ak2,*ak3,*ak4,*ak5,*ak6,*ytemp;
  
  ak2=dvector(n);
  ak3=dvector(n);
  ak4=dvector(n);
  ak5=dvector(n);
  ak6=dvector(n);
  ytemp=dvector(n);
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+b21*h*dydx[i];
  derivs(x+a2*h,ytemp,ak2,n);
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b31*dydx[i]+b32*ak2[i]);
  derivs(x+a3*h,ytemp,ak3,n);
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b41*dydx[i]+b42*ak2[i]+b43*ak3[i]);
  derivs(x+a4*h,ytemp,ak4,n);
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b51*dydx[i]+b52*ak2[i]+b53*ak3[i]+b54*ak4[i]);
  derivs(x+a5*h,ytemp,ak5,n);
  for (i=0;i<n;i++)
    ytemp[i]=y[i]+h*(b61*dydx[i]+b62*ak2[i]+b63*ak3[i]+b64*ak4[i]+b65*ak5[i]);
  derivs(x+a6*h,ytemp,ak6,n);
  for (i=0;i<n;i++)
    yout[i]=y[i]+h*(c1*dydx[i]+c3*ak3[i]+c4*ak4[i]+c6*ak6[i]);
  for (i=0;i<n;i++)
    yerr[i]=h*(dc1*dydx[i]+dc3*ak3[i]+dc4*ak4[i]+dc5*ak5[i]+dc6*ak6[i]);
  free_dvector(ytemp);
  free_dvector(ak6);
  free_dvector(ak5);
  free_dvector(ak4);
  free_dvector(ak3);
  free_dvector(ak2);
}


int rkqs( double *y, double *dydx, int n, double *x, double htry, double eps,
	  double *yscal, double *hdid, double *hnext, derivsfunc derivs )
  /* returns 0: o.k.  1: stepsize undeflow */
{
#define SAFETY 0.9
#define PGROW -0.2
#define PSHRNK -0.25
#define ERRCON 1.89e-4

  int i;
  double errmax,h,htemp,xnew;
  double *yerr,*ytemp;
  int r = 0;

  yerr=dvector(n);
  ytemp=dvector(n);
  h=htry;
  for (;;)
    {
      rkck(y,dydx,n,*x,h,ytemp,yerr,derivs);
      errmax=0.0;
      for (i=0;i<n;i++)
	errmax=MAX(errmax,fabs(yerr[i]/yscal[i]));
      errmax /= eps;
      if (errmax <= 1.0)
	break;
      htemp=SAFETY*h*pow(errmax,PSHRNK);
      h=(h >= 0.0 ? MAX(htemp,0.1*h) : MIN(htemp,0.1*h));
      xnew=(*x)+h;
      if (xnew == *x)
	r = 1;	/* stepsize underflow */
    }
  if (errmax > ERRCON)
    *hnext=SAFETY*h*pow(errmax,PGROW);
  else
    *hnext=5.0*h;
  if ( (*hnext) > odehmax )
    (*hnext) = odehmax;
  *x += (*hdid=h);
  for (i=0;i<n;i++)
    y[i]=ytemp[i];
  free_dvector(ytemp);
  free_dvector(yerr);
  return( r );

#undef SAFETY
#undef PGROW
#undef PSHRNK
#undef ERRCON
}


double odeeps=1.0e-4;
double odeh1=0.1;       /* initial step size factor */
double odehmin=1.0e-7;  /* minimal step size factor */
double odehmax=1.0e30;  /* maximal step size factor */
int odenok;             /* number of steps with accepted step size */
int odenbad;            /* number of steps with changed step size */


int OdeInt( REAL *xd, REAL **yd, int nvar, int kmax, int *kount, 
	    double *ystart, double x1, double x2, double deltax, 
	    derivsfunc derivs )
{
#define TINY 1.0e-3

#define DSIGN(a,b) ((b) >= 0.0 ? fabs(a) : -fabs(a))

  int nstp, i;
  double dxsav, xsav, x, hnext, hdid, h;
  double *yscal, *y, *dydx;
  int r=0;

  if ( kmax <= 0 )
    return( r |= 2 );
  if ( deltax > 0.0 )
    odehmax = deltax;
  yscal=dvector(nvar);
  y=dvector(nvar);
  dydx=dvector(nvar);
  x=x1;
  h=DSIGN(odeh1,x2-x1);
  odenok = odenbad = (*kount) = 0;
  for (i=0;i<nvar;i++) 
    y[i]=ystart[i];
  dxsav = fabs(x2-x1)/kmax;
  xsav=x-dxsav*2.0;
  for (nstp=0;nstp<kmax;nstp++)
    {
      derivs(x,y,dydx,nvar);
      for (i=0;i<nvar;i++)
        yscal[i]=fabs(y[i])+fabs(dydx[i]*h)+TINY;
      if ( (*kount) < kmax-1 && fabs(x-xsav) > fabs(dxsav) )
        {
	  xd[(*kount)]=x;
	  for (i=0;i<nvar;i++) 
	    yd[i][(*kount)]=y[i];
	  (*kount)++;
	  xsav=x;
        }
      if ((x+h-x2)*(x+h-x1) > 0.0)
        h=x2-x;
      r |= rkqs( y, dydx, nvar, &x, h, odeeps, yscal, &hdid, &hnext, derivs );
      if (hdid == h)
        ++odenok;
      else
        ++odenbad;
      if ((x-x2)*(x2-x1) >= 0.0)
        {
	  for (i=0;i<nvar;i++) 
	    ystart[i]=y[i];
	  xd[(*kount)]=x;
	  for (i=0;i<nvar;i++) 
	    yd[i][(*kount)]=y[i];
	  (*kount)++;
	  free_dvector(dydx);
	  free_dvector(y);
	  free_dvector(yscal);
	  return( r );
        }
      if (fabs(hnext) < odehmin)
	{
	  r |= 4;    /* step size too small */
	  h = odehmin;
	}
      h=hnext;
    }
  return( r |= 8 );  /* too many steps */

#undef DSIGN
#undef TINY
}

#endif


}; /* namespace relacs */

#endif /* ! _RELACS_ODEALGORITHM_H_ */
