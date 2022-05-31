/*
  voltageclamp/pnsubtraction.cc
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

#include <relacs/fitalgorithm.h>
#include <cmath>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <algorithm> // for copy() and assign()
#include <iterator> // for back_inserter
#include <relacs/randomstring.h>
#include <relacs/ephys/amplifiercontrol.h>
#include <relacs/spectrum.h>
#include <relacs/sampledata.h>
#include <iostream>



using namespace relacs;

namespace voltageclamp {


PNSubtraction::PNSubtraction( const string &name,
	 const string &pluginset,
	 const string &author,
	 const string &version,
	 const string &date)
  : RePro( name, pluginset, author, version, date )
{
  // add some options:
  newSection( "QualityControl" );
  addSection( "P/N Subtraction" );
  addNumber( "pn", "p/N", -4, -100, 100, 1 );

  addSection( "Chirp Prepulse" );
  addBoolean( "qualitycontrol", "Quality control", true );
  addSelection( "chirpmethod", "Kind of frequency sweep", "linear|logarithmic"); 
  addNumber( "pulseduration", "Pulse duration", 0.1, 0.0, 1000.0, 0.001, "sec", "ms").setActivation( "qualitycontrol", "true" );
  addNumber( "f0", "minimum pulse frequency", 10.0, 1.0, 1000.0, 1.0, "Hz", "Hz" ).setActivation( "qualitycontrol", "true" );
  addNumber( "f1", "maximum pulse frequency", 500.0, 1.0, 5000.0, 1.0, "Hz", "Hz" ).setActivation( "qualitycontrol", "true" );
  addNumber( "phi", "phase of cosine", 270.0, 0.0, 360.0, 0.1, "degrees", "degrees" ).setActivation( "qualitycontrol", "true" );
//  addNumber( "PCS_derivativekernelwidth", "derivative kernel width", 1.0, 1.0, 1000.0, 1.0 ).setActivation( "qualitycontrol", "true" );

  addSection( "WhiteNoise Prepulse" );
  addBoolean( "qualitycontrol_whitenoise", "Quality control whitenoise", true );
  addNumber( "noiseduration_wn", "Noise duration", 0.1, 0.0, 1000.0, 0.001, "sec", "ms").setActivation( "qualitycontrol_whitenoise", "true" );
  addNumber( "holdingpotential_wn", "Noise holding potential", -120.0, -200.0, 200.0, 1.0, "mV" ).setActivation( "qualitycontrol_whitenoise", "true" );
  addNumber( "fmin", "minimum noise frequency", 10.0, 0.0, 1000.0, 1.0, "Hz", "Hz" ).setActivation( "qualitycontrol_whitenoise", "true" );
  addNumber( "fmax", "maximum noise frequency", 500.0, 0.0, 5000.0, 1.0, "Hz", "Hz" ).setActivation( "qualitycontrol_whitenoise", "true" );
  addNumber( "noisestd", "Noise standard deviation", 10.0, 1.0, 200.0, 1.0, "mV" ).setActivation( "qualitycontrol_whitenoise", "true" );

  //  addNumber( "PCS_derivativekernelwidth", "derivative kernel width", 1.0, 1.0, 1000.0, 1.0 ).setActivation( "qualitycontrol", "true" );

}

int PNSubtraction::main( void )
{
  return Completed;
}

SampleDataD PNSubtraction::PN_sub( OutData signal, Options &opts, double &holdingpotential, double &pause, double &mintime, double &maxtime, double &t0) {
  int pn = number( "pn" );
  double samplerate = signal.sampleRate();

  bool qualitycontrol = boolean( "qualitycontrol" );
  double pulseduration = number( "pulseduration" );
  double f0 = number( "f0" );
  double f1 = number( "f1" );
  double phi = number( "phi" );
  std::string chirpmethod = text( "chirpmethod" );

  bool qualitycontrol_whitenoise = boolean( "qualitycontrol_whitenoise");
  double noiseduration = number( "noiseduration_wn" );
  double holdingpotential_wn = number( "holdingpotential_wn" );
  double fmin = number( "fmin" );
  double fmax = number( "fmax" );
  double noisestd = number( "noisestd" );

  // assign random id for later connection between qualitycontrol, pn and traces
  std::string randomId = randomString(40);

  // add p/n option to signal
  Parameter &pn1 = opts.addNumber( "pn", pn );
  Parameter &qc1 = opts.addBoolean( "qualitycontrol", qualitycontrol );
  Parameter &rid = opts.addText( "TraceId", randomId );

  signal.setMutable( pn1 );
  signal.setMutable( qc1 );
  signal.setMutable( rid );
  signal.setDescription( opts );

  // don't print repro message:
  noMessage();

  SampleDataD pn_trace(mintime, signal.rangeBack(), 1 / samplerate);
  if ( pn != 0 ) {
    // give pn stimulus
    OutData pn_signal = signal;
    pn_signal.setTrace(PotentialOutput[0]);
    pn_signal = holdingpotential + (signal - holdingpotential) / pn;
    pn_signal.description().setType("stimulus/PNSubtraction");

    for (int i = 0; i < ::abs(pn); i++) {
      write(pn_signal);
      if (signal.error())
        return false;
      sleep(pause);

      if (interrupt()) {
        break;
      };

      SampleDataD currenttrace(mintime, maxtime, trace(CurrentTrace[0]).stepsize(), 0.0);
      trace(CurrentTrace[0]).copy(signalTime(), currenttrace);

      pn_trace += currenttrace;
    };
//  pn_trace -= pn_trace.mean(signalTime() + t0 - 0.001, signalTime() + t0);

    if (interrupt()) {
      return pn_trace;
    };
  };
  
  // make short quality assuring test-pulse
//  ArrayD I;
//  ArrayD Vp;

  double stepduration = 0.010;
  if ( qualitycontrol_whitenoise ) {
    OutData wn_signal1;
    wn_signal1.setTrace( PotentialOutput[0] );
    wn_signal1.constWave( stepduration, -1.0, -100 );

    OutData wn_signal2;
    wn_signal2.setTrace( PotentialOutput[0] );
    wn_signal2.constWave( stepduration, -1.0, -170 );

    OutData wn_signal3;
    wn_signal3.setTrace( PotentialOutput[0] );
    wn_signal3.constWave( stepduration, -1.0, -135 );

    // WhitenoiseStimulus
    //potential base
    OutData wn_signal4;
    wn_signal4.setTrace( PotentialOutput[0] );
    wn_signal4.constWave( noiseduration, -1.0, holdingpotential_wn );

    OutData wn_signal5;
    wn_signal5.setTrace( PotentialOutput[0] );
    wn_signal5.constWave( 0.010, -1.0, holdingpotential );

    // get next power of two
    int power = 1;
    while(power < wn_signal4.size()) {
      power *= 2;
    }

    //frequency range
    SampleDataD f( power/2 );
    for (int k=0; k<f.size(); k++) {
      f[k] = k / (power * wn_signal4.stepsize());
    };
    SampleDataD f2( power/2 );
    for (int k=0; k<f2.size(); k++) {
      f2[k] = -f[f.size()-k];
    };
    f.append( f2 );

    //draw random numbers on fourier space and transfer to time space
    SampleDataD data( power );
    for ( int k=0; k<data.size(); k++ ) {
      data[k] = (rnd.gaussian());
    }
    hcFFT( data );
    double datastd = 0.0;
    for ( int k=0; k<data.size(); k++ ) {
      datastd += data[k]*data[k] / (data.size() - 1);
    }
    datastd = sqrt(datastd);
    data *= noisestd/datastd;

    // get stimulus of prefered length
    for (int k=0; k<wn_signal4.size(); k++) {
      wn_signal4[k] += data[k];
    }

    wn_signal1.append( wn_signal2 );
    wn_signal1.append( wn_signal3 );
    wn_signal1.append( wn_signal4 );
    wn_signal1.append( wn_signal5 );

    wn_signal1.description().setType( "stimulus/QualityControl_WhiteNoise" );
    Options opts_wn = wn_signal1.description();
    Parameter &wn_rid = opts_wn.addText( "TraceId", randomId );
    Parameter &wn_f0 = opts_wn.addNumber( "fmin", fmin );
    Parameter &wn_f1 = opts_wn.addNumber( "fmax", fmax );
    Parameter &wn_dur = opts_wn.addNumber( "noiseduration_wn", fmax );
    wn_signal1.setMutable( wn_rid );
    wn_signal1.setMutable( wn_f0 );
    wn_signal1.setMutable( wn_f1 );
    wn_signal1.setMutable( wn_dur );
    wn_signal1.setDescription( opts_wn );


    write(wn_signal1);
//    if (interrupt()) {
//      break;
//    };
    sleep(pause);
  }


  if ( qualitycontrol ) {
    OutData qc_signal1;
    qc_signal1.setTrace( PotentialOutput[0] );
    qc_signal1.constWave( stepduration, -1.0, -100 );

    OutData qc_signal2;
    qc_signal2.setTrace( PotentialOutput[0] );
    qc_signal2.constWave( stepduration, -1.0, -170 );
    
    OutData qc_signal3;
    qc_signal3.setTrace( PotentialOutput[0] );
    qc_signal3.constWave( stepduration, -1.0, holdingpotential-20.0 );
    
    OutData qc_signal4;
    qc_signal4.setTrace( PotentialOutput[0] );
    
    cerr << chirpmethod << "\n";
    if ( chirpmethod == "linear" ) {    
      cerr << "entered if" << "\n";  
      qc_signal4.sweepWave( pulseduration, -1.0, f0, f1, 20.0, 0.0 );
    }
    else if ( chirpmethod == "logarithmic" ) { 
      cerr << "entered else if" << "\n";
      qc_signal4.constWave( pulseduration, -1.0, -100 );
      double phase = 0.0;
      for (int i=0; i<pulseduration*samplerate; i++ ) {
          phase += 2 * 3.14159265358979323846 * f0 * std::pow(f1/f0, (i/samplerate)/pulseduration) / samplerate;  
          cerr << i << ", " << phase << "\n";
          qc_signal4[i] = cos(phase + 3.14159265358979323846/180*phi) * 20.0;
      }
    }

    qc_signal4 += holdingpotential - 20.0;
//    qc_signal4 = qc_signal4 + holdingpotential - 20;

    OutData qc_signal5;
    qc_signal5.setTrace( PotentialOutput[0] );
    qc_signal5.constWave( 0.010, -1.0, holdingpotential );

    qc_signal1.append( qc_signal2 );
    qc_signal1.append( qc_signal3 );
    qc_signal1.append( qc_signal4 );
    qc_signal1.append( qc_signal5 );

    qc_signal1.description().setType( "stimulus/QualityControl" );
    Options opts_qc = qc_signal1.description();
    Parameter &qc_rid = opts_qc.addText( "TraceId", randomId );
    Parameter &qc_f0 = opts_qc.addNumber( "f1", f0 );
    Parameter &qc_f1 = opts_qc.addNumber( "f0", f1 );
    Parameter &qc_dur = opts_qc.addNumber( "pulseduration", f1 );
    qc_signal1.setMutable( qc_rid );
    qc_signal1.setMutable( qc_f0 );
    qc_signal1.setMutable( qc_f1 );
    qc_signal1.setMutable( qc_dur );
    qc_signal1.setDescription( opts_qc );

    write(qc_signal1);
//    if (interrupt()) {
//      break;
//    };
    sleep(pause);

    SampleDataD currenttrace(0.0, 3 * stepduration + pulseduration, trace(CurrentTrace[0]).stepsize(), 0.0 );
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );
    SampleDataD potentialtrace(0.0, 3 * stepduration + pulseduration, trace(SpikeTrace[0]).stepsize(), 0.0 );
    trace(SpikeTrace[0]).copy(signalTime(), potentialtrace );

    PCS_currenttrace = currenttrace;
    PCS_potentialtrace = potentialtrace;
  };

  signal.description().setType( "stimulus/Trace" );

  write(signal);
//  if (interrupt()) {
//    break;
//  };
  sleep(pause);

  SampleDataD currenttrace( mintime-0.01, maxtime+0.01, trace(CurrentTrace[0]).stepsize(), 0.0);
  trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

  SampleDataD potentialtrace( mintime-0.01, maxtime+0.01, trace(SpikeTrace[0]).stepsize(), 0.0);
  trace(SpikeTrace[0]).copy(signalTime(), potentialtrace );



  if ( qualitycontrol ) {
    dt = PCS_currenttrace.stepsize();
    Vp = PCS_potentialtrace.array();
    dVp = dxdt( Vp, dt );
    d2Vp = dxdt( dVp, dt );
    I = PCS_currenttrace.array();
    dI = dxdt( I, dt );

    ArrayD param = pcsFitLeak( stepduration );
    pcsFitCapacitiveCurrents( param, stepduration );
    pcsFitAllParams( param, stepduration );
    cerr << param << "\n";
    double a = param[0];
    double b = param[1];
    double c = param[2];
    double d = param[3];
    double e = param[4];

    gL = (d*b - a*b*c) / (a*a*b - a*c + d);
    Rs = a*a / (a*c - d);
    Cm = (a*c - d) * (a*c - d) / (a*a*c - a*d - a*a*a*b);
    Cp = d / a;
    EL = e / b;
    cerr << "gL=" << gL << ", Rs=" << Rs << ", Cm=" << Cm << ", Cp=" << Cp << ", EL=" << EL << "\n";

    ArrayD dpotential_p = dxdt( potentialtrace, dt );
    ArrayD d2potential_p = dxdt( dpotential_p, dt );
    ArrayD dcurrent = dxdt( currenttrace, dt );
    currenttrace = currenttrace + a*dcurrent - b*potentialtrace - c*dpotential_p - d*d2potential_p + e;
  }
  else if ( pn != 0 )
  {
    currenttrace -= pn / ::abs(pn) * pn_trace;// - currenttrace.mean(signalTime() + t0 - 0.001, signalTime() + t0);
    currenttrace -= currenttrace.mean(-samplerate / 500, 0);
  };
  return currenttrace;
};


//ArrayD PNSubtraction::dxdt( const ArrayD &x, const double &dt ) {
//  ArrayD dx( x.size() );
//  for ( int i = 1; i<(x.size()-1); i++ ) {
//    dx[i] = (x[i + 1] - x[i - 1]) / (2 * dt);
//  }
//  dx[0] = 2 * dx[1] - dx[2];
//  dx[x.size() - 1] = 2 * dx[x.size() - 2] - dx[x.size() - 3];
//  return dx;
//}


ArrayD PNSubtraction::dxdt( const ArrayD &x, const double &dt ) {
  double kernelsize = 1;//number( "PCS_derivativekernelwidth");
  ArrayD x2( x.size() );
  ArrayD dx( x.size() );

  // convolve x with ArrayD (kernelsize, 1/kernelsize)
  if ( kernelsize > 1.0 ) {
    int khalf = floor(kernelsize / 2.0);
    for ( int i = khalf; i < (x.size() - khalf); i++ ) {
      double s = 0.0;
      for ( int j = (i-khalf); j < (i+khalf); j++ ) {
        s += x[j];
      }
      x2[i] = s/kernelsize;
    }
    for ( int i = 0; i < khalf; i++ ) {
      int firstidx = khalf - i - 1;
      int lastidx = x2.size() - khalf + i;
      x2[firstidx] = 2*x2[firstidx+1] - x2[firstidx+2];
      x2[lastidx] =  2*x2[lastidx -1] - x2[lastidx -2];
    }
  }
  else {
    x2 = x;
  }
  // derive smoothed trace
  for ( int i = 1; i<(x.size()-1); i++ ) {
    dx[i] = (x2[i + 1] - x2[i - 1]) / (2 * dt);
  }
  dx[0] = 2 * dx[1] - dx[2];
  dx[x.size() - 1] = 2 * dx[x.size() - 2] - dx[x.size() - 3];
  return dx;
};


double PNSubtraction::passiveMembraneFuncDerivs( double t, const ArrayD &p, ArrayD &dfdp ) {

  double a = p[0];
  double b = p[1];
  double c = p[2];
  double d = p[3];
  double e = p[4];

  int idx = t/dt;
  double y = I[idx] + a * dI[idx] - b * Vp[idx] - c * dVp[idx] - d * d2Vp[idx] + e;
  dfdp[0] = dI[idx];
  dfdp[1] = -Vp[idx];
  dfdp[2] = -dVp[idx];
  dfdp[3] = -d2Vp[idx];
  dfdp[4] = 1.0;
//  cerr << "y="<< y << "\n";
//  double y = 0.0;
  return y;
};


ArrayD PNSubtraction::pcsFitLeak( double stepduration ) {
   //first step
  int idx00 = 0.5 * stepduration / dt;
  int idx01 = 1.0 * stepduration / dt - 2;
  //second step
  int idx10 = 1.5 * stepduration / dt;
  int idx11 = 2.0 * stepduration / dt - 2;
  //third step
  int idx20 = 2.5 * stepduration / dt;
  int idx21 = 3.0 * stepduration / dt - 2;

  ArrayD time( idx01-idx00 + idx11-idx10 + idx21-idx20 );
  ArrayD y( idx01-idx00 + idx11-idx10 + idx21-idx20 );

  //fill arrays
  int i = -1;
  for ( int j = idx00; j < idx01; j++) {
    i += 1;
    time[i] = j*dt;
    y[i] = 0.0;
  }
  for ( int j = idx10; j < idx11; j++) {
    i += 1;
    time[i] = j*dt;
    y[i] = 0.0;
  }
  for ( int j = idx20; j < idx21; j++) {
    i += 1;
    time[i] = j*dt;
    y[i] = 0.0;
  }


//  // Fit leak current
  ArrayD param(5, 0.0);
  param[1] = gL / (1 + gL * Rs);
  param[4] = 0.0;//EL * gL / (1 + gL * Rs);
  ArrayI pf(5, 0);
  pf[1] = 1;
  pf[4] = 1;
  ArrayD err( time.size(), 1.0);
  ArrayD uncert(5, 0.0);
  double chisq = 0.0;

  ArrayD dparam(5, 0.0);
  marquardtfit( time, y, err, param, pf, uncert, chisq );
  return param;
};


void PNSubtraction::pcsFitCapacitiveCurrents( ArrayD &param, double &stepduration ) {
  double pulseduration = number( "pulseduration" );
  // chirp index
  int idx30 = 7/2 * stepduration / dt;
  int idx31 = idx30 + pulseduration / dt - 2;
  ArrayD time( idx31 - idx30 );
  ArrayD y( idx31 - idx30 );
  //fill arrays
  int i = -1;
  for ( int j = idx30; j < idx31; j++) {
    i += 1;
    time[i] = j * dt;
    y[i] = 0.0;
  }
////  // Fit leak current
  param[0] = Cm * Rs / (1 + gL * Rs);
  param[2] = Cp + Cm / (1 + gL * Rs);
  param[3] = Cm * Cp * Rs / (1 + gL * Rs);

  ArrayI pf(5, 0);
  pf[0] = 1;
  pf[2] = 1;
  pf[3] = 1;
  ArrayD err( time.size(), 1.0);
  ArrayD uncert(5, 0.0);
  double chisq = 0.0;

  marquardtfit( time, y, err, param, pf, uncert, chisq );
};



void PNSubtraction::pcsFitAllParams( ArrayD &param, double &stepduration ) {
  double pulseduration = number( "pulseduration" );
  //first step
  int idx00 = 1 / 2 * stepduration / dt;
  int idx01 = 2 * stepduration / dt - 2;
  //second step
  int idx10 = 3 / 2 * stepduration / dt;
  int idx11 = 3 * stepduration / dt - 2;
  //third
  int idx20 = 5 / 2 * stepduration / dt;
  int idx21 = 4 * stepduration / dt - 2;
  //chirp index
  int idx30 = 7 / 2 * stepduration / dt;
  int idx31 = idx30 + pulseduration / dt - 2;

//
  ArrayD time( idx01-idx00 + idx11-idx10 + idx21-idx20 + idx31-idx30 );
  ArrayD y( idx01-idx00 + idx11-idx10 + idx21-idx20 + idx31-idx30 );
//
  //fill arrays
  int i = -1;
  for ( int j = idx00; j < idx01; j++) {
    i += 1;
    time[i] = j*dt;
    y[i] = 0.0;
  }
  for ( int j = idx10; j < idx11; j++) {
    i += 1;
    time[i] = j*dt;
    y[i] = 0.0;
  }
  for ( int j = idx20; j < idx21; j++) {
    i += 1;
    time[i] = j*dt;
    y[i] = 0.0;
  }
  for ( int j = idx30; j < idx31; j++) {
    i += 1;
    time[i] = j * dt;
    y[i] = 0.0;
  }
////  // Fit leak current
  ArrayI pf(5, 1);
  pf[4] = 0;
  ArrayD err( time.size(), 1.0);
  ArrayD uncert(5, 0.0);
  double chisq = 0.0;

  marquardtfit( time, y, err, param, pf, uncert, chisq );
  };


}; /* namespace voltageclamp */

#include "moc_pnsubtraction.cc"
