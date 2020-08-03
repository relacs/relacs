/*
  voltageclamp/pnsubtraction.h
  P-N Subtraction for removing stimulus artifacts in voltageclamp recordings

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

#ifndef _RELACS_VOLTAGECLAMP_PNSUBTRACTION_H_
#define _RELACS_VOLTAGECLAMP_PNSUBTRACTION_H_ 1

#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/fitalgorithm.h>

using namespace relacs;

namespace voltageclamp {


/*!
\class PNSubtraction
\brief [RePro] P-N Subtraction for removing stimulus artifacts in voltageclamp recordings
\author Lukas Sonnenberg
\version 1.0 (Oct 08, 2018)
*/


class PNSubtraction : public RePro, public ephys::Traces
{
  Q_OBJECT

friend class Activation;

public:

  PNSubtraction( const string &name,
	 const string &pluginset="",
	 const string &author="unknown",
	 const string &version="unknown",
	 const string &date=__DATE__ );
  //PNSubtraction( void );
  virtual int main( void );

  SampleDataD PN_sub( OutData signal, Options &opts, double &holdingpotential, double &pause, double &mintime, double &maxtime, double &t0);

private:
  ArrayD dxdt( const ArrayD &x, const double &dt, const double &kernelsize );
  ArrayD pcsFitLeak( double stepduration );
  void pcsFitCapacitiveCurrents( ArrayD &params, double &stepduration );
  void pcsFitAllParams( ArrayD &param, double &stepduration );
  void analyzeCurrentPulse( SampleDataD voltagetrace, double I0 );
  double linearFuncDerivs( double x, const ArrayD &p, ArrayD &dfdp ) ;
  double passiveMembraneFuncDerivs( double t, const ArrayD &p, ArrayD &dfdp );


  SampleDataD PCS_potentialtrace;
  SampleDataD PCS_currenttrace;
  double EL = 0.0;
  double gL = 1e-5;
  double Cm = 1e-6;
  double Cp = 1e-9;
  double Rs = 1.0;
  double tau;
  double dt;
  ArrayD Vp;
  ArrayD dVp;
  ArrayD d2Vp;
  ArrayD I;
  ArrayD dI;



  template < typename ForwardIterX, typename ForwardIterY,
          typename ForwardIterS > //, typename FitFunc >
  void marquardtcof( ForwardIterX firstx, ForwardIterX lastx,
                     ForwardIterY firsty, ForwardIterY lasty,
                     ForwardIterS firsts, ForwardIterS lasts,
//                     FitFunc &f,
                     ArrayD &params, const ArrayI &paramfit,
                     int mfit, double &chisq,
                     vector< ArrayD > &alpha, ArrayD &beta )
  {
//    FitFunc f = passiveMembraneFuncDerivs;
    // initialize:
    for ( int j=0; j<mfit; j++ ) {
      alpha[j] = 0.0;
      beta[j] = 0.0;
    }
    chisq = 0.0;
    ArrayD dyda( params.size() );

    FitFlag = true;   // new parameters
    while ( (firstx != lastx) && (firsty != lasty) && (firsts != lasts) ) {
      double s = ( (*firsty) - passiveMembraneFuncDerivs( *firstx, params, dyda ) ) / (*firsts);
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
          typename ForwardIterS >
  int marquardtfit ( ForwardIterX firstx, ForwardIterX lastx,
                     ForwardIterY firsty, ForwardIterY lasty,
                     ForwardIterS firsts, ForwardIterS lasts,
//                     FitFunc &f,
                     ArrayD &params, const ArrayI &paramfit,
                     ArrayD &uncert, double &chi)
  {
    int *iter = NULL;
//    ostream os = NULL;
//    double chieps = 0.0005;
//    int maxiter = 300;

//    FitFunc f = passiveMembraneFuncDerivs;

    const double chigood = 1.0e-8;
    const int maxsearch = 4;
    const int miniter = 30;
    //  const double lambdastart = 0.01;
    const double lambdastart = 1.0;
    const double lambdafac = 10.0;
    int maxiter = 300;
    double chieps = 0.0005;

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
//    if ( mfit==0 ) {
//      if ( os != NULL )
//        *os << "exit: " << 1 << "\n\n";
//      return 1;
//    }
//    if ( lastx - firstx <= mfit ) {
//      if ( os != NULL )
//        *os << "exit: " << 2 << "\n\n";
//      return 2;
//    }

    double alambda = lambdastart;
    double chisq = 0.0;
    vector< ArrayD > alpha( params.size(), ArrayD( params.size(), 0 ) );
    vector< ArrayD > covar( params.size(), ArrayD( params.size(), 0 ) );
    ArrayD beta( params.size() );
    ArrayD oneda( params.size() );
    ArrayD da( params.size() );
    ArrayD atry( params );
    ArrayD emptyb( 0 );
    marquardtcof( firstx, lastx, firsty, lasty, firsts, lasts, params, paramfit, mfit, chisq, alpha, beta );
    double ochisq = chisq;

    // report start values:
//    if ( os != NULL ) {
//      *os << "Iter  S   alambda     ChiSq         1-oChi/Chi  ";
//      for ( int j=0; j<params.size(); j++ )
//        *os << "  a[" << setw( 2 ) << j << "]       ";
//      *os << '\n';
//      *os << setw( 4 ) << 0 << "  "
//          << setw( 2 ) << 0 << "  "
//          << setw( 10 ) << setprecision( 3 ) << alambda << "  "
//          << setw( 12 ) << setprecision( 5 ) << chisq << "  "
//          << setw( 12 ) << setprecision( 5 ) << ::fabs(1.0-ochisq/chisq);
//      for ( int j=0; j<params.size(); j++ )
//        *os << "  " << setw( 12 ) << setprecision( 6 ) << atry[j];
//      *os << '\n';
//    }

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
//        if ( os != NULL )
//          *os << "exit from gaussJordan: " << gjr << "\n\n";
        return 16*gjr;
      }

      for ( int j=0; j<mfit; j++ )
        da[j] = oneda[j];
      for ( int j=0, l=0; l<params.size(); l++ ) {
        if ( paramfit[l] )
          atry[l] = params[l] + da[j++];
      }
      marquardtcof( firstx, lastx, firsty, lasty, firsts, lasts, atry, paramfit, mfit, chisq, covar, da );

      // report current iteration step:
//      if ( os != NULL )	{
//        *os << setw( 4 ) << iteration << "  "
//            << setw( 2 ) << search << "  "
//            << setw( 10 ) << setprecision( 3 ) << alambda << "  "
//            << setw( 12 ) << setprecision( 5 ) << chisq << "  "
//            << setw( 12 ) << setprecision( 5 ) << ::fabs(1.0-ochisq/chisq);
//        for ( int j=0; j<params.size(); j++ )
//          *os << "  " << setw( 12 ) << setprecision( 6 ) << params[j];
//        *os << '\n';
//      }

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
//      if ( os != NULL )
//        *os << "exit: " << 8 << "\n\n";
      return 8;
    }

    // calculate uncertainties:
    for ( int j=0; j<mfit; j++ )
      covar[j] = alpha[j];
    int gjr = gaussJordan( covar, mfit, emptyb );
    if ( gjr ) {
//      if ( os != NULL )
//        *os << "exit from final gaussJordan: " << gjr << "\n\n";
      return 64 * gjr;
    }
    covarSort( covar, paramfit, mfit );
    for ( int j=0; j<params.size(); j++ )
      uncert[j] = ::sqrt( ::fabs( covar[j][j] ) );

    chi = chisq;

    int fiterror = iteration > maxiter ? 4 : 0;
//    if ( os != NULL )
//      *os << "exit: " << fiterror << "\n\n";
    return fiterror;
  }


  template < typename ContainerX, typename ContainerY,
          typename ContainerS >
  int marquardtfit( const ContainerX &x, const ContainerY &y, const ContainerS &s,
//                    FitFunc &f,
                    ArrayD &params, const ArrayI &paramfit,
                    ArrayD &uncert, double &chisq)
  {
    return marquardtfit( x.begin(), x.end(), y.begin(), y.end(), s.begin(), s.end(),
//                         &f,
                         params, paramfit, uncert, chisq);//, iter, os, chieps, maxiter );
  }




//    double stepduration;

	//	double tau;


};


}; /* namespace voltageclamp */

#endif /* ! _RELACS_VOLTAGECLAMP_PNSUBTRACTION_H_ */
