/*
  voltageclamp/combinedstimulus.cc
  combine colored noise with activation, inactivation, tail in a short stimulus

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

#include <relacs/voltageclamp/combinedstimulus.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>
#include <relacs/fitalgorithm.h>

using namespace relacs;

namespace voltageclamp {


CombinedStimulus::CombinedStimulus( void )
  : PNSubtraction( "CombinedStimulus", "voltageclamp", "Lukas Sonnenberg", "1.0", "May 01, 2020" )
{
  addNumber( "noiseduration", "duration (ColoredNoise)", 3, 0.001, 100000.0, 0.001, "s", "s" );
  addNumber( "noiseVbase", "Base Potential (ColoredNoise)", -60.0, -200.0, 200.0, 0.1, "mV" );
  addNumber( "noisefrequencyconstant", "Frequency Constant (ColoredNoise)", 800.0, 0.001, 10000.0, 0.001, "Hz" );
  addNumber( "noisemaxamplitude", "Maximum Amplitude (ColoredNoise)", 110.0, 0.0, 200.0, 1.0, "mV" );
  
  addNumber( "actmintest", "Minimum testing Potential (act/inact)", -120.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "actmaxtest", "Maximum testing Potential (act/inact)", 60.0, -200.0, 200.0, 5.0, "mV" );
  addNumber( "inactstep", "Inactivation testing Potential", 10.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "tailmintest", "Minimum testing Potential (tail)", -120.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "tailmaxtest", "Maximum testing potential (tail)", 0.0, -200.0, 200.0, 5.0, "mV" );
  addNumber( "tailstep", "Tail testing Potential", -10.0, -100.0, 200.0, 1.0, "mV" );
  addNumber( "tailduration", "Tail test step duration", 0.0007, 0.0, 100.0, 0.0001, "s", "ms" );
  addNumber( "stepsize", "Step testing potential (act/inact/tail)", 5.0, 0.0, 200.0, 1.0, "mV" );
  addNumber( "stepduration", "Duration for each Potential Step (act/inact/tail)", 0.02, 0.0, 1000.0, 0.001, "s", "ms" );

  addNumber( "pause", "Duration of pause between outputs", 0.4, 0.001, 1000.0, 0.001, "s", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "holdingpotential", "Holding potential", -100.0, -200.0, 200.0, 1.0, "mV" );

  setWidget( &P );
}

/////////////////////// STILL NEEDS PLOTS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ////////////////////////

int CombinedStimulus::main( void )
{
  // get options:
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential = number( "holdingpotential" );

  // don't print repro message:
  noMessage();

  // set amplifier to VC mode
  ephys::AmplifierControl *ampl = dynamic_cast< ephys::AmplifierControl* >( control( "AmplifierControl" ) );
  if ( ampl == 0 ) {
    warning( "No amplifier found." );
    return Failed;
  }
  ampl ->activateVoltageClampMode();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );

  // write holdingpotential:
  write( holdingsignal );
  sleep( pause );

  for ( int Count=0; ( repeats <= 0 || Count < repeats ) && softStop() == 0; Count++ ) {
    OutData signal;
    signal.setTrace( PotentialOutput[0] );
    signal.constWave( holdingpotential );
  // make short quality assuring test-pulse
    OutData noise = ColoredNoise();
    OutData act =   ActInact();
    OutData tail =  Tail();

    signal.append( noise );
    signal.append( act );
    signal.append( tail );

    Options opts = signal.description();
    double t0 = -0.002;
    double dt = signal.stepsize();
    double dur = signal.size();
    double maxduration = dt*dur;
    SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, t0, maxduration, t0);
  }
  return Completed;
}


OutData CombinedStimulus::ColoredNoise() {
  double holdingpotential = number( "holdingpotential" );
  double noiseduration = number( "noiseduration" );
  double noiseVbase = number( "noiseVbase" );
  double noisefrequencyconstant = number( "noisefrequencyconstant" );
  double noisemaxamplitude = number( "noisemaxamplitude" );

  // colored noise parameters
  ArrayD expParam( 3, 1.0 );
  expParam[0] = 1;
  expParam[1] = -noisefrequencyconstant;
  expParam[2] = 0;

  //potential base
  OutData signal;
  signal.setTrace( PotentialOutput[0] );
  signal.constWave( noiseduration, -1.0, noiseVbase );

  //frequency range
  SampleDataD f( signal.size() / 2 );
  f /= signal.size() * signal.stepsize();
  SampleDataD f2( signal.size() / 2 );
  f2( signal.size() / 2 );
  for (int k=0; k<f2.size(); k++) {
    f2[k] = -f[f.size()-k];
  };
  f.append( f2 );

  //draw random numbers on fourier space and transfer to time space
  OutData signal2;
  signal2.setTrace(PotentialOutput[0]);
  signal2.constWave(noiseduration, -1.0, 0.0);
  
  //''' set hardcoded cutoff frequency '''
  double cutoff = 20;
  for (int k=0; k<f.size(); k++) {
    if ( f[k] <= cutoff ) {
      signal2[k] = expFunc2( abs( f[k] ), expParam ) * (rnd() - 0.5);
    }
    else {
      signal2[k] = 0.0;
    }
  };
  hcFFT( signal2 );
  signal2 *= noisemaxamplitude / 0.5;

  //go back to holdingpotential
  OutData signal3;
  signal3.setTrace(PotentialOutput[0]);
  signal3.constWave(0.1, -1.0, holdingpotential );

  //put stimulus pieces together
  signal = signal + signal2;
  signal.append( signal3 );
  return signal;
}


OutData CombinedStimulus::ActInact() {
  double holdingpotential = number( "holdingpotential" );
  double actmintest = number( "actmintest" );
  double actmaxtest = number( "actmaxtest" );
  double inactstep = number( "inactstep" );
  double stepsize = number( "stepsize" );
  double stepduration = number( "stepduration" );

  OutData signal;
  signal.setTrace( PotentialOutput[0] );
  signal.constWave( stepduration, -1.0, holdingpotential );

  // inactivation step
  OutData signal2;
  signal2.setTrace( PotentialOutput[0] );
  signal2.pulseWave( stepduration, -1.0, inactstep, holdingpotential );

  // back to holdingpotential
  OutData signal3;
  signal3.setTrace( PotentialOutput[0] );
  signal3.constWave( stepduration, -1.0, holdingpotential );

  for (double step = actmintest; step <= actmaxtest; step += stepsize) {
    //activation step
    OutData signal1;
    signal1.setTrace( PotentialOutput[0] );
    signal1.pulseWave( stepduration, -1.0, step, inactstep );

    //append activation and inactivation step
    signal.append( signal1 );
    signal.append( signal2 );
    signal.append( signal3 );
  }
  return signal;
}


OutData CombinedStimulus::Tail() {
  double holdingpotential = number( "holdingpotential" );
  double tailmintest = number( "tailmintest" );
  double tailmaxtest = number( "tailmaxtest" );
  double tailstep = number( "tailstep" );
  double tailduration = number( "tailduration" );
  double stepsize = number( "stepsize" );
  double stepduration = number( "stepduration" );

  OutData signal;
  signal.setTrace( PotentialOutput[0] );
  signal.constWave( stepduration, -1.0, holdingpotential );

  // initial tail step
  OutData signal1;
  signal1.setTrace( PotentialOutput[0] );
  signal1.constWave( tailduration, -1.0, tailstep );

  OutData signal3;
  signal3.setTrace( PotentialOutput[0] );
  signal3.constWave( stepduration, -1.0, holdingpotential );

  for (double step = tailmintest; step <= tailmaxtest; step += stepsize) {
    //tail step
    OutData signal2;
    signal2.setTrace( PotentialOutput[0] );
    signal2.pulseWave( stepduration, -1.0, step, holdingpotential );

    //append initial and actual tail step
    signal.append( signal1 );
    signal.append( signal2 );
    signal.append( signal3 );
  }
  return signal;
}


addRePro( CombinedStimulus, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_combinedstimulus.cc"
