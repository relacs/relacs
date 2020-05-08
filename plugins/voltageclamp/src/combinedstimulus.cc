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
#include <relacs/sampledata.h>
#include <relacs/spectrum.h>
#include <cmath>
#include <iostream>
#include <relacs/multiplot.h>

using namespace relacs;

namespace voltageclamp {


CombinedStimulus::CombinedStimulus( void )
  : PNSubtraction( "CombinedStimulus", "voltageclamp", "Lukas Sonnenberg", "1.0", "May 01, 2020" )
{
  addNumber( "noiseduration", "duration (ColoredNoise)", 3, 0.001, 100000.0, 0.001, "s", "s" );
  addNumber( "noiseVbase", "Base Potential (ColoredNoise)", -60.0, -200.0, 200.0, 0.1, "mV" );
  addNumber( "noisefrequencyconstant", "Frequency Constant (ColoredNoise)", 800.0, 0.001, 10000.0, 0.001, "Hz" );
  addNumber( "noisestd", "Standard Deviation (ColoredNoise)", 80.0, 1.0, 200.0, 1.0, "mV" );
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
  double noiseVbase = number( "noiseVbase" );
  double noisemaxamplitude = number( "noisemaxamplitude" );

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

  // clear plot and set Range
  string IUnit = trace( CurrentTrace[0] ).unit();
  string VUnit = trace( SpikeTrace[0]).unit();

  P.lock();
  P.resize( 2, 2, true );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]"  );
  P[1].setXLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]"  );
  P[1].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]" );

  P[0].clearData();
  P[1].clearData();
  P[1].setXRange( noiseVbase - noisemaxamplitude*1.05, noiseVbase + noisemaxamplitude*1.05 );
  P.unlock();

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
    SampleDataD potentialtrace(t0, maxduration, trace(SpikeTrace[0]).stepsize(), 0.0);
    trace( SpikeTrace[0] ).copy( signalTime(), potentialtrace );

    // plot
    // trace
    P[0].plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
    // IV
    for ( int i=0; i<currenttrace.size(); i++ ) {
      double x = potentialtrace[i];
      double y = currenttrace[i];
      P[1].plotPoint( x, Plot::First, y, Plot::First, 1, Plot::Dot, 1, Plot::First, Plot::Yellow, Plot::Solid );
    }
    P[1].setYRange( min(currenttrace)*1.05, max(currenttrace)*1.05);
    P.draw();
    P.unlock();


  }
  return Completed;
}


OutData CombinedStimulus::ColoredNoise() {
  double holdingpotential = number( "holdingpotential" );
  double noiseduration = number( "noiseduration" );
  double noiseVbase = number( "noiseVbase" );
  double noisefrequencyconstant = number( "noisefrequencyconstant" );
  double noisestd = number( "noisestd" );
  double noisemaxamplitude = number( "noisemaxamplitude" );

  // colored noise parameters
  ArrayD expParam( 3, 1.0 );
  expParam[0] = 1;
  expParam[1] = noisefrequencyconstant;
  expParam[2] = 0;

  //potential base
  OutData signal;
  signal.setTrace( PotentialOutput[0] );
  signal.constWave( noiseduration, -1.0, noiseVbase );

  // get next power of two
  int power = 1;
  while(power < signal.size()) {
    power *=2;
  }
  //frequency range
  SampleDataD f( power );
  for (int k=0; k<f.size(); k++) {
    f[k] = k / (power * signal.stepsize());
  };
  SampleDataD f2( power );
  for (int k=0; k<f2.size(); k++) {
    f2[k] = -f[f.size()-k];
  };
  f.append( f2 );

  //draw random numbers on fourier space and transfer to time space
  SampleDataD data( power );
  for ( int k=0; k<data.size(); k++ )
    data[k] = expFunc2( abs( f[k] ), expParam ) * (rnd() - 0.5);
  hcFFT( data );
  double datastd = 0.0;
  for ( int k=0; k<data.size(); k++ ) {
    datastd += data[k]*data[k] / (data.size() - 1);
  }
  datastd = sqrt(datastd);
  data *= noisestd/datastd;

    //go back to holdingpotential
  OutData signal3;
  signal3.setTrace(PotentialOutput[0]);
  signal3.constWave(0.0001, -1.0, holdingpotential );

  //put stimulus pieces together
  for (int k=0; k<signal.size(); k++) {
    if (data[k] > noisemaxamplitude) {
      signal[k] = noiseVbase + noisemaxamplitude;
    }
    else if (data[k] < -noisemaxamplitude) {
      signal[k] = noiseVbase - noisemaxamplitude;
    }
    else {
      signal[k] += data[k];
    }
  }
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
