/*
  statstests.cc
  

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

#include <cmath>
#include <algorithm>
#include <relacs/stats.h>
#ifdef HAVE_LIBGSL
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_gamma.h>
#endif
#include <relacs/statstests.h>


double alphaNormal( double x )
{
 return 0.5 * (1.0 + ::erf( x / ::sqrt( 2.0 ) ) );
}


double alphaBinomial( int k, int n, double p )
{
  if ( k<0 )
    return 0.0;
  if ( k > n )
    k = n;
  if ( p == 1.0 )
    return k<n ? 0.0 : 1.0;

  double pq = p/(1-p);
  double bp = ::pow( 1-p, n );
  double s = bp;
  for ( int j=1; j<=k; j++ ) {
    bp *= pq * double(n+1-j)/double(j);
    s += bp;
  }
  return s;
}


int positiveSign( const ArrayD &data, double median )
{
  int c = 0;
  for ( int k=0; k<data.size(); k++ ) {
    if ( data[k] - median > 0.0 )
      c++;
  }
  return c;
}


void signTest( const ArrayD &data, double median, int tail, int &n, double &p )
{
  if ( data.empty()  ) {
    n = 0;
    p = 1.0;
    return;
  }

  int nn = data.size();
  int np = positiveSign( data, median );
  int nm = nn - np;
  double sn = ::sqrt(nn);
  double zp = (np-nm)/sn;
  double zm = -zp;
  double pp = nn > 100 ? alphaNormal( zp ) : alphaBinomial( np, nn, 0.5 );
  double pm = nn > 100 ? alphaNormal( zm ) : alphaBinomial( nm, nn, 0.5 );
  if ( tail > 0 ) {
    n = nm;
    p = pm;
  }
  else if ( tail < 0 ) {
    n = np;
    p = pp;
  }
  else {
    n = nm < np ? nm : np;
    p = 2.0 * ( nm < np ? pm : pp );
    if ( nm == np )
      p = 2.0 * ( pm < 1.0 - pm ? pm : 1.0 - pm ); // ????
  }


}


bool absLess( double a, double b )
{
  return ( fabs( a ) < fabs( b ) );
}


double rankSumWilcoxon( const ArrayD &xdata, const ArrayD &ydata, int &n )
{
  if ( xdata.size() != ydata.size() ) {
    n = 0;
    return 0.0;
  }

  // sort absolute values of differrences:
  ArrayD diff = ydata - xdata;
  std::sort( diff.begin(), diff.end(), absLess );
  // skip zeros for ranks:
  int f=0;
  for ( f=0; f<diff.size() && fabs( diff[f] ) == 0.0; f++ );
  ArrayD rdata( diff, f );
  n = rdata.size();
  numerics::rank( rdata );
  // compute rank sum of positive differences:
  double Wp = 0.0;
  for ( int k=0; k<rdata.size(); k++ )
    if ( diff[f+k] > 0.0 )
      Wp += rdata[k];
  return Wp;
}


double alphaWilcoxon( double w, int n )
{
  // the total number of possible outcomes is 2^n:
  long npos = 1 << n;

  // loop over all possible distributions of signs as bit-strings:
  int count = 0;
  for ( long i=0; i < npos; i++ ) { 
    // compute sum of ranks of the set bits:
    int ranksum = 0;
    for ( int j=0; j<n; j++ ) {
      if ( (i >> j) & 1)
	ranksum += j + 1;
    }
    // count if sum of ranks is smaller than or equal to w:
    if ( ranksum <= w )
      count++;
  }
  double p = double( count ) / double( npos );
  return p;
}


double zWilcoxon( double w, int n )
{
  double z = (w - n * (n + 1) / 4.0) / ::sqrt (n * (n + 1.0) * (2.0 * n + 1.0) / 24.0);
  return z;
}


void wilcoxonTest( const ArrayD &xdata, const ArrayD &ydata, int tail,
		   double &w, double &p )
{
  if ( xdata.size() != ydata.size() ) {
    w = 0;
    p = 1.0;
    return;
  }

  int nn = xdata.size();
  double Wp = rankSumWilcoxon( xdata, ydata, nn );
  double Wm = 0.5*nn*(nn+1.0) - Wp;
  double zp = zWilcoxon( Wp, nn );
  double zm = zWilcoxon( Wm, nn );
  double pp = nn > 20 ? alphaNormal( zp ) : alphaWilcoxon( Wp, nn );
  double pm = nn > 20 ? alphaNormal( zm ) : alphaWilcoxon( Wm, nn );
  if ( tail > 0 ) {
    w = Wm;
    p = pm;
  }
  else if ( tail < 0 ) {
    w = Wp;
    p = pp;
  }
  else {
    w = Wm < Wp ? Wm : Wp;
    p = 2.0 * ( Wm < Wp ? pm : pp );
  }
}


/*
U-Test statistics:
(from http://lib.stat.cmu.edu/apstat/62 )
see also Cheung1997

c AS 62 generates the frequencies for the Mann-Whitney U-statistic.
c Users are much more likely to need the distribution function.
c Code to return the distribution function has been added at the end
c of AS 62 by Alan Miller.   Remove the C's in column 1 to activate it.
c
      SUBROUTINE UDIST(M, N, FRQNCY, LFR, WORK, LWRK, IFAULT)
C
C     ALGORITHM AS 62  APPL. STATIST. (1973) VOL.22, NO.2
C
C     The distribution of the Mann-Whitney U-statistic is generated for
C     the two given sample sizes
C
      INTEGER M, N, LFR, LWRK, IFAULT
      REAL FRQNCY(LFR), WORK(LWRK)
C
C     Local variables
C
      INTEGER MINMN, MN1, MAXMN, N1, I, IN, L, K, J
      REAL ZERO, ONE, SUM
      DATA ZERO /0.0/, ONE /1.0/
C
C     Check smaller sample size
C
      IFAULT = 1
      MINMN = MIN(M, N)
      IF (MINMN .LT. 1) RETURN
C
C     Check size of results array
C
      IFAULT = 2
      MN1 = M * N + 1
      IF (LFR .LT. MN1) RETURN
C
C     Set up results for 1st cycle and return if MINMN = 1
C
      MAXMN = MAX(M, N)
      N1 = MAXMN + 1
      DO 1 I = 1, N1
    1 FRQNCY(I) = ONE
      IF (MINMN .EQ. 1) GO TO 4
C
C     Check length of work array
C
      IFAULT = 3
      IF (LWRK .LT. (MN1 + 1) / 2 + MINMN) RETURN
C
C     Clear rest of FREQNCY
C
      N1 = N1 + 1
      DO 2 I = N1, MN1
    2 FRQNCY(I) = ZERO
C
C     Generate successively higher order distributions
C
      WORK(1) = ZERO
      IN = MAXMN
      DO 3 I = 2, MINMN
        WORK(I) = ZERO
        IN = IN + MAXMN
        N1 = IN + 2
        L = 1 + IN / 2
        K = I
C
C     Generate complete distribution from outside inwards
C
        DO 3 J = 1, L
          K = K + 1
          N1 = N1 - 1
          SUM = FRQNCY(J) + WORK(J)
          FRQNCY(J) = SUM
          WORK(K) = SUM - FRQNCY(N1)
          FRQNCY(N1) = SUM
    3 CONTINUE
C
    4 IFAULT = 0
C
C     Code to overwrite the frequency function with the distribution
C     function.   N.B. The frequency in FRQNCY(1) is for U = 0, and
C     that in FRQNCY(I) is for U = I - 1.
C
C     SUM = ZERO
C     DO 10 I = 1, MN1
C       SUM = SUM + FRQNCY(I)
C       FRQNCY(I) = SUM
C  10 CONTINUE
C     DO 20 I = 1, MN1
C  20 FRQNCY(I) = FRQNCY(I) / SUM
C
      RETURN
      END

*/


double pearsonTest( double r, int n )
{
  double df = n - 2.0;
  const double tiny = 1.0e-20;
  double t = r * ::sqrt( df / ( (1.0-r+tiny) * (1.0+r+tiny) ) );
  return incBeta( 0.5*df, 0.5, df/(df+t*t) );
}


double lngamma( double xx )
{
  static double cof[6] = { 76.18009172947146, -86.50532032941677,
			   24.01409824083091, -1.231739572450155,
			   0.1208650973866179e-2, -0.5395239384953e-5 };
  double x = xx;
  double y = x;
  double tmp = x+5.5;
  tmp -= (x+0.5) * ::log(tmp);
  double ser = 1.000000000190015;
  for ( int j=0; j<=5; j++ ) 
    ser += cof[j]/++y;
  return -tmp + ::log( 2.5066282746310005*ser/x );
}


int gcf( double *gammcf, double a, double x, double *gln )
{
  const int itmax = 100;
  const double zero = 5.0e-16;
  const double fpmin = 1.0e-30;

  *gammcf = 0.0;
  *gln = lngamma( a );
  double b = x+1.0-a;
  double c = 1.0/fpmin;
  double d = 1.0/b;
  double h = d;
  for ( int i=0; i<itmax; i++ ) {
    double an = -i*(i-a);
    b += 2.0;
    d = an*d+b;
    if ( ::fabs(d) < fpmin ) 
      d = fpmin;
    c = b+an/c;
    if ( ::fabs(c) < fpmin )
      c = fpmin;
    d = 1.0/d;
    double del = d*c;
    h *= del;
    if ( ::fabs( del-1.0 ) < zero ) {
      *gammcf = ::exp( -x + a * ::log(x) - (*gln) )*h;
      return 0;
    }
  }
  // a too large, itmax too small:
  *gammcf = 0.0;
  return 1;
}


int gser( double *gamser, double a, double x, double *gln )
{
  const int itmax = 100;
  const double zero = 5.0e-16;

  *gln = lngamma( a );
  if ( x == 0.0 ) {
    *gamser = 0.0;
    return 0;
  }
  else {
    double ap = a;
    double sum = 1.0/a;
    double del = sum; 
    for ( int n=0; n < itmax; n++ ) {
      ++ap;
      del *= x/ap;
      sum += del;
      if ( ::fabs(del) < ::fabs(sum) * zero ) {
	*gamser = sum * ::exp( -x + a * ::log(x) - (*gln) );
	return 0;
      }
    }
    // a too large, itmax too small:
    *gamser = 0.0;
    return 1;
  }
}


double gammaP( double a, double x )
{
  if ( x < 0.0 ) 
    cerr << "Invalid arguments in function gammaP: x < 0.0\n";
  if ( a <= 0.0 ) 
    cerr << "Invalid arguments in function gammaP: a <= 0.0\n";

#ifdef HAVE_LIBGSL

  gsl_sf_result result;
  int r = gsl_sf_gamma_inc_P_e( a, x, &result );
  if ( r == GSL_SUCCESS )
    return result.val;
  else {
    cerr << "Error in gammaP, GSL: " << gsl_strerror( r ) << "\n";
    return 1.0;
  }

#else

  if ( x < (a+1.0) ) {
    double gamser = 0.0;
    double gln = 0.0;
    if ( gser( &gamser, a, x, &gln ) ) {
      cerr << "Error in gammaP, gser: a too large, iterations exceeded\n";
      return 1.0;
    }
    return gamser;
  } 
  else {
    double gammcf = 0.0;
    double gln = 0.0;
    if ( gcf( &gammcf, a, x, &gln ) ) {
      cerr << "Error in gammaP, gcf: a too large, iterations exceeded\n";
      return 0.0;
    }
    return 1.0 - gammcf;
  }

#endif

}


double gammaQ( double a, double x )
{
  if ( x < 0.0 ) 
    cerr << "Invalid arguments in function gammaQ: x < 0.0\n";
  if ( a <= 0.0 ) 
    cerr << "Invalid arguments in function gammaQ: a <= 0.0\n";

#ifdef HAVE_LIBGSL
  
  gsl_sf_result result;
  int r = gsl_sf_gamma_inc_Q_e( a, x, &result );
  if ( r == GSL_SUCCESS )
    return result.val;
  else {
    cerr << "Error in gammaQ, GSL: " << gsl_strerror( r ) << "\n";
    return 0.0;
  }

#else

  if ( x < (a+1.0) ) {
    double gamser = 0.0;
    double gln = 0.0;
    if ( gser( &gamser, a, x, &gln ) ) {
      cerr << "Error in gammaQ, gser: a too large, iterations exceeded\n";
      return 0.0;
    }
    return 1.0 - gamser;
  } 
  else {
    double gammcf = 0.0;
    double gln = 0.0;
    if ( gcf( &gammcf, a, x, &gln ) ) {
      cerr << "Error in gammaQ, gcf: a too large, iterations exceeded\n";
      return 1.0;
    }
    return gammcf;
  }

#endif

}


double betacf( double a, double b, double x )
{
  const int itmax = 100;
  const double fpmin = 1.0e-30;
  const double zero = 5.0e-16;

  double qab = a+b;
  double qap = a+1.0;
  double qam = a-1.0;
  double c = 1.0;
  double d = 1.0-qab*x/qap;
  if ( ::fabs( d ) < fpmin ) 
    d = fpmin;
  d = 1.0/d;
  double h = d;
  int m=1;
  for ( m=1; m<=itmax; m++ ) {
    int m2 = 2*m;
    double aa = m*(b-m)*x/((qam+m2)*(a+m2));
    d=1.0+aa*d;
    if ( ::fabs( d ) < fpmin ) 
      d=fpmin;
    c=1.0+aa/c;
    if ( ::fabs( c ) < fpmin ) 
      c=fpmin;
    d=1.0/d;
    h *= d*c;
    aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
    d=1.0+aa*d;
    if ( ::fabs( d ) < fpmin ) 
      d=fpmin;
    c=1.0+aa/c;
    if ( ::fabs( c ) < fpmin ) 
      c=fpmin;
    d=1.0/d;
    double del = d*c;
    h *= del;
    if ( ::fabs( del-1.0 ) < zero ) 
      break;
  }
  if ( m > itmax ) {
    cerr << "Error in incBeta, betacf: a or b too large, iterations exceeded\n";
    return 0.0;
  }
  return h;
}


double incBeta( double a, double b, double x )
{
  if ( x < 0.0 || x > 1.0 ) 
    cerr << "Invalid arguments in function incBeta: x < 0.0 || x > 1.0\n";
  if ( a <= 0.0 ) 
    cerr << "Invalid arguments in function incBeta: a <= 0.0\n";
  if ( b <= 0.0 ) 
    cerr << "Invalid arguments in function incBeta: b <= 0.0\n";

#ifdef HAVE_LIBGSL
  
  gsl_sf_result result;
  int r = gsl_sf_beta_inc_e( a, b, x, &result );
  if ( r == GSL_SUCCESS )
    return result.val;
  else {
    cerr << "Error in incBeta, GSL: " << gsl_strerror( r ) << "\n";
    return 0.0;
  }

#else

  double bt = 0.0;
  if ( x != 0.0 && x != 1.0 ) 
    bt = ::exp( lngamma( a+b ) - lngamma( a ) - lngamma( b ) + 
		a * ::log( x ) + b * ::log( 1.0-x ) );
  if ( x < (a+1.0)/(a+b+2.0) )
    return bt * betacf( a, b, x ) / a;
  else
    return 1.0 - bt * betacf( b, a, 1.0-x ) / b;

#endif

}


#ifdef HAVE_LIBGSL

// set a non-aborting error handler:

void GSLSilentHandler( const char *reason, const char *file, 
		       int line, int gsl_errno )
{
  /*
  cerr << "Error in GSL-library: " 
       << reason << " " 
       << gsl_strerror( gsl_errno ) << '\n';
  */
}


class GSLSetErrorHandler
{

public:

  GSLSetErrorHandler( void )
  {
    OldHandler = gsl_set_error_handler( &GSLSilentHandler );
  }
  ~GSLSetErrorHandler( void )
  {
    gsl_set_error_handler( OldHandler );
  }

private:

  gsl_error_handler_t *OldHandler;

};

GSLSetErrorHandler gslseterrorhandler;

#endif
