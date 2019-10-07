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
  addNumber( "pn", "p/N", -4, -100, 100, 1 );
  addBoolean( "qualitycontrol", "Quality control", true );
  addBoolean( "currentpulse", "Current pulse", true );
  addNumber( "pulseamplitude", "Pulse amplitude", -0.1, -1000.0, 1000.0, 0.01).setActivation( "currentpulse", "true" );
  addNumber( "pulseduration", "Pulse duration", 0.1, 0.0, 1000.0, 0.001, "sec", "ms").setActivation( "currentpulse", "true" );
}


int PNSubtraction::main( void )
{
  return Completed;
}

SampleDataD PNSubtraction::PN_sub( OutData signal, Options &opts, double &holdingpotential, double &pause, double &mintime, double &maxtime, double &t0) {
  int pn = number( "pn" );
  double samplerate = signal.sampleRate();
  bool qualitycontrol = boolean( "qualitycontrol" );
  bool currentpulse = boolean( "currentpulse" );
  double pulseamplitude = 0.0;
  double pulseduration = 0.0;
  if ( currentpulse ) {
    pulseamplitude = number( "pulseamplitude" );
    pulseduration = number( "pulseduration" );
  };

  // assign random id for later connection between qualitycontrol, pn and traces
  std::string randomId = randomString(40);

  // add p/n option to signal
  Parameter &pn1 = opts.addNumber( "pn", pn );
  Parameter &qc1 = opts.addBoolean( "qualitycontrol", qualitycontrol );
  Parameter &cp1 = opts.addBoolean( "currentpulse", currentpulse );
  Parameter &rid = opts.addText( "TraceId", randomId );

  signal.setMutable( pn1 );
  signal.setMutable( qc1 );
  signal.setMutable( cp1 );
  signal.setMutable( rid );
  signal.setDescription( opts );

  // don't print repro message:
  noMessage();

  // make short current pulse to estimate membrane capacity, time constant and resistance
  if ( currentpulse ) {
    // set amplifier to VC mode
    ephys::AmplifierControl *ampl = dynamic_cast< ephys::AmplifierControl* >( control( "AmplifierControl" ) );
    ampl ->activateVoltageClampMode();

    // set VC to holdingpotential for 100ms
    OutData hp_signal;
    hp_signal.setTrace( PotentialOutput[0] );
    hp_signal.constWave( 0.1, -1.0, holdingpotential );
    write(hp_signal);
    sleep(pause);

    SampleDataD currenttrace( 0.0, 0.1, trace(CurrentTrace[0]).stepsize(), 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );
    double I0 = currenttrace.mean( 0.07, 0.1 );

    // set amplifier to Bridge mode
    ampl ->activateBridgeMode();
    OutData br_hold;
    br_hold.setTrace( CurrentOutput[0] );
    br_hold.constWave( I0 );
    write(br_hold);
    sleep(pulseduration);

//    sleep(10); /////////////////// delete ///////////////////////////////////////////////////////////////////////////

    OutData br_signal;
    br_signal.setTrace( CurrentOutput[0] );
    br_signal.pulseWave( pulseduration, -1.0, I0, I0 + 2*pulseamplitude);

    OutData br_signal2;
    br_signal2.setTrace( CurrentOutput[0] );
    br_signal2.pulseWave( pulseduration, -1.0, I0 + 2*pulseamplitude, I0 + pulseamplitude);

    OutData br_signal3;
    br_signal3.setTrace( CurrentOutput[0] );
    br_signal3.pulseWave( pulseduration, -1.0, I0 + pulseamplitude, I0);

    OutData br_signal4;
    br_signal4.setTrace( CurrentOutput[0] );
    br_signal4.constWave( pulseduration, -1.0, I0 );

    br_signal.append( br_signal2 );
    br_signal.append( br_signal3 );
    br_signal.append( br_signal4 );

    br_signal.description().setType( "stimulus/CurrentPulse" );
    Options opts_br = br_signal.description();
    Parameter &br_rid = opts_br.addText( "TraceId", randomId );
    Parameter &br_amp = opts_br.addNumber( "PulseAmplitude", pulseamplitude );
    Parameter &br_dur = opts_br.addNumber( "PulseDuration", pulseduration );
    br_signal.setMutable( br_rid );
    br_signal.setMutable( br_amp );
    br_signal.setMutable( br_dur );
    br_signal.setDescription( opts_br );

    write(br_signal);

    SampleDataD potentialtrace = SampleDataF( 0.0, 4*pulseduration, 1/samplerate, 0.0 );
    trace(SpikeTrace[0]).copy(signalTime(), potentialtrace );

//    cerr << potentialtrace.min(0.0, 3*pulseduration) << ", " << potentialtrace.max(0.0, 3*pulseduration) << ", " << potentialtrace.size() << "\n";

    analyzeCurrentPulse( potentialtrace, I0 );

    // set amplifier back to VC mode
    ampl ->activateVoltageClampMode();
    write(hp_signal);
    sleep(pause);
  };

  // make short quality assuring test-pulse
  if ( qualitycontrol ) {
    OutData qc_signal1;
    qc_signal1.setTrace( PotentialOutput[0] );
    qc_signal1.constWave( 0.010, -1.0, holdingpotential );

    OutData qc_signal2;
    qc_signal2.setTrace( PotentialOutput[0] );
    qc_signal2.pulseWave( 0.010, -1.0, holdingpotential-20, holdingpotential );

    qc_signal1.append( qc_signal2 );

    qc_signal1.description().setType( "stimulus/QualityControl" );
    Options opts_qc = qc_signal1.description();
    Parameter &qc_rid = opts_qc.addText( "TraceId", randomId );
    qc_signal1.setMutable( qc_rid );
    qc_signal1.setDescription( opts_qc );

//    cerr << qc_signal1.description() << endl;

    write(qc_signal1);
    sleep(pause);
  };

//  // skip prepulses if pn==0
//  if ( pn == 0 ) {
//    write(signal);
//
//    if ( signal.error() )
//      return false;
//    sleep(pause);
//
//    SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
//    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );
//
//    return currenttrace;
//  };

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

  signal.description().setType( "stimulus/Trace" );

  write(signal);
  sleep(pause);

  SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize(), 0.0);
  trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

  if (currentpulse) {
    SampleDataD potentialtrace(mintime, maxtime, trace(SpikeTrace[0]).stepsize(), 0.0);
    trace(SpikeTrace[0]).copy(signalTime(), potentialtrace);

    SampleDataD I_L(mintime, maxtime, trace(SpikeTrace[0]).stepsize(), 0.0);
    SampleDataD dVdt(mintime, maxtime, trace(SpikeTrace[0]).stepsize(), 0.0);

    I_L = gL * (potentialtrace - EL);
    for (int i = 0; i < (dVdt.size() - 1); i++) {
      dVdt[i] = (potentialtrace[i + 1] - potentialtrace[i]) * samplerate;
    }
    dVdt[dVdt.size()-1] = 0.0;

    currenttrace += - I_L - Cm * dVdt;
  }
  else if ( pn != 0 )
  {
    currenttrace -= pn / ::abs(pn) * pn_trace;// - currenttrace.mean(signalTime() + t0 - 0.001, signalTime() + t0);
    currenttrace -= currenttrace.mean(-samplerate / 500, 0);
  };
  return currenttrace;
};


double currentPulseFuncDerivs(  double t, const ArrayD &p, ArrayD &dfdp ) {
  double dT = p[4];
  double tau = p[0];
  double V0 = p[1];
  double V1 = p[2];
  double V2 = p[3];
  double y = 0.0;

  double V11 = (V0 - V1) * ::exp( -dT / tau ) + V1;
  double V21 = (V11- V2) * ::exp( -dT / tau ) + V2;

  double ex1 = ::exp( - (t - 1*dT) / tau);
  double ex2 = ::exp( - (t - 2*dT) / tau);
  double ex3 = ::exp( - (t - 3*dT) / tau);

  if (t < dT ) {
    y = V0;
    dfdp[0] = 0.0;
    dfdp[1] = 1.0;
    dfdp[2] = 0.0;
    dfdp[3] = 0.0;
  }
  else if ( t < 2*dT ) {
    y = ( V0 - V1 ) * ex1 + V1;
    dfdp[0] = (t-dT) / (tau*tau) * (V0-V1) * ex1;
    dfdp[1] = ex1;
    dfdp[2] = - ex1 + 1.0;
    dfdp[3] = 0.0;
  }
  else if ( t < 3*dT ) {
    y = ( V11 - V2 ) * ex2 + V2;
    dfdp[0] = (t-1*dT) / (tau*tau) * (V0-V1) * ex1 +
              (t-2*dT) / (tau*tau) * (V1-V2) * ex2;
    dfdp[1] = ex1;
    dfdp[2] = - ex1 + ex2;
    dfdp[3] = - ex2 + 1.0;
  }
  else if ( t < 4*dT ) {
    y = ( V21 - V0 ) * ex3 + V0;
    dfdp[0] = (t-1*dT) / (tau*tau) * (V0-V1) * ex1 +
              (t-2*dT) / (tau*tau) * (V1-V2) * ex2 +
              (t-3*dT) / (tau*tau) * (V2-V0) * ex3;
    dfdp[1] = ex1 - ex3 + 1.0;
    dfdp[2] = - ex1 + ex2;
    dfdp[3] = - ex2 + ex3;
  };
  dfdp[4] = 0.0;
  return y;
};


double linearFuncDerivs( double x, const ArrayD &p, ArrayD &dfdp ) {
  double m = p[0];
  double b = p[1];
  double y = m * x + b;
  dfdp[0] = x;
  dfdp[1] = 1.0;
  return y;
};


void PNSubtraction::analyzeCurrentPulse( SampleDataD voltagetrace, double I0 ) {
  double pulseamplitude = number( "pulseamplitude" );
  double pulseduration = number( "pulseduration" );
  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  const InData &intrace = trace( SpikeTrace[0] );
  int dT = intrace.indices(pulseduration);

  // Fit exponentials to CurrentPulse
  ArrayD param( 5, 1.0 );
  param[0] = .05;
  param[1] = mean(voltagetrace.begin(), voltagetrace.begin()+10) + 1;
  param[2] = mean(voltagetrace.begin() + 2*dT - 10, voltagetrace.begin() + 2*dT ) + 1;
  param[3] = mean(voltagetrace.begin() + 3*dT - 10, voltagetrace.begin() + 3*dT ) + 3;
  param[4] = pulseduration;
  ArrayD error( voltagetrace.size(), 1.0 );
  ArrayD uncertainty( 5, 0.0 );
  ArrayI paramfit( 5, 1 );
  paramfit[4] = 0;
  double chisq = 0.0;

  marquardtFit( voltagetrace.range(), voltagetrace, error, currentPulseFuncDerivs,
          param, paramfit, uncertainty, chisq );

  // Fit leak current
  ArrayD I_leak( 3, 1.0 ); I_leak[0] = I0; I_leak[1] = I0 + 2*pulseamplitude; I_leak[2] = I0 + pulseamplitude;
  ArrayD V_leak( 3, 1.0 ); V_leak[0] = param[1]; V_leak[1] = param[2]; V_leak[2] = param[3];
  ArrayD p_leak( 2, 1.0 ); p_leak[0] = 0.1; p_leak[1] = 0.0;
  ArrayD err_leak( 2, 1.0 );
  ArrayD uncert_leak( 2, 0.0 );
  ArrayI pf_leak( 2, 1 );
  marquardtFit( V_leak, I_leak, err_leak, linearFuncDerivs, p_leak, pf_leak, uncert_leak, chisq );

  gL = p_leak[0];
  EL = p_leak[1];
  tau = param[0];
  Cm = tau * gL;
  cerr << "tau=" << param[0]*1000.0 << "ms, Cm=" << Cm*1000.0 << "pF\n";
};

}; /* namespace voltageclamp */

#include "moc_pnsubtraction.cc"
