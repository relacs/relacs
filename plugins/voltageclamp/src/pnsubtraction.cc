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
    sleep(0.1);
    cerr << pulseamplitude << ", " << pulseduration << "c\n";

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

    Options opts_br = br_signal.description();
    Parameter &br_rid = opts_br.addText( "TraceId", randomId );
    Parameter &br_amp = opts_br.addNumber( "PulseAmplitude", pulseamplitude );
    Parameter &br_dur = opts_br.addNumber( "PulseDuration", pulseduration );
    br_signal.setMutable( br_rid );
    br_signal.setMutable( br_amp );
    br_signal.setMutable( br_dur );
    br_signal.setDescription( opts_br );

    write(br_signal);

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

  // skip prepulses if pn==0
  if ( pn == 0 ) {
    write(signal);
    if ( signal.error() )
      return false;
    sleep(pause);

    SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

    return currenttrace;
  };

  // give stimulus
  OutData pn_signal = signal;
  pn_signal.setTrace( PotentialOutput[0] );
  pn_signal = holdingpotential + (signal - holdingpotential)/pn;
  SampleDataD pn_trace( mintime, pn_signal.rangeBack(), 1/samplerate );
  pn_signal.description().setType( "stimulus/PNSubtraction" );

  for ( int i = 0; i<::abs(pn); i++ ) {
    write(pn_signal);
    if ( signal.error() )
      return false;
    sleep(pause);

    if (interrupt()) {
      break;
    };

    SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

    pn_trace += currenttrace;

  };
//  pn_trace -= pn_trace.mean(signalTime() + t0 - 0.001, signalTime() + t0);

  if (interrupt()) {
    return pn_trace;
  };

  signal.description().setType( "stimulus/Trace" );

  write(signal);
  sleep(pause);

  SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
  trace(CurrentTrace[0]).copy(signalTime(), currenttrace );


  currenttrace -= pn/::abs(pn)*pn_trace;// - currenttrace.mean(signalTime() + t0 - 0.001, signalTime() + t0);
  currenttrace -= currenttrace.mean( -samplerate/500, 0);

//  return pn_trace;
  return currenttrace;
};

}; /* namespace voltageclamp */

#include "moc_pnsubtraction.cc"
