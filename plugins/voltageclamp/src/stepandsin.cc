/*
  voltageclamp/stepandsin.cc
  Voltage Steps (short activation protocol) followed by a sinusoidal stimulus

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

#include <relacs/voltageclamp/stepandsin.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>
#include <relacs/multiplot.h>

using namespace relacs;

namespace voltageclamp {


StepAndSin::StepAndSin( void )
  : PNSubtraction( "StepAndSin", "voltageclamp", "Lukas Sonnenberg", "1.0", "Mai 08, 2020" )
{
  // add some options:
//  addSection( "StepAndSin" );
  newSection( "Stimulus" );

  addNumber( "actmintest", "Minimum testing Potential (act)", -120.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "actmaxtest", "Maximum testing Potential (act)", 60.0, -200.0, 200.0, 5.0, "mV" );
  addNumber( "stepsize", "Step testing potential (act)", 5.0, 0.0, 200.0, 1.0, "mV" );
  addNumber( "stepduration", "Duration for each Potential Step (act)", 0.02, 0.0, 1000.0, 0.001, "s", "ms" );
  addNumber( "sinduration", "Stimulus duration (sinsum)", 0.01, 0.001, 100000.0, 0.001, "s", "s" );
  addNumber( "sinVbase", "Vbase (sinsum)", -50.7, -200.0, 200.0, 0.1, "mV" );
  addText( "sinamplitudes", "Amplitudes (sinsum)", "60.31,  40.67, 30.12, 40.52" ).setUnit( "mV" );
  addText( "sinfrequencies", "Frequencies (sinsum)", "14.4, 86.7, 144.8, 392.8" ).setUnit( "Hz" );

  addNumber( "holdingpotential", "Holding potential", -120.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "s", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );

  setWidget( &P );
}


int StepAndSin::main( void )
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
  P[1].setXRange( -150, 100 );
  P.unlock();

  for ( int Count=0; ( repeats <= 0 || Count < repeats ) && softStop() == 0; Count++ ) {
    OutData signal;
    signal.setTrace( PotentialOutput[0] );
    signal.constWave( holdingpotential );
    OutData act = Act();
    OutData sinsum = Sins();
    OutData signal3;
    signal3.setTrace(PotentialOutput[0]);
    signal3.constWave( 0.01, -1.0, holdingpotential );

    signal.append( act );
    signal.append( sinsum );
    signal.append( signal3 );

    Options opts = signal.description();
    double t0 = -0.002;
    double dt = signal.stepsize();
    double dur = signal.size();
    double maxduration = dt*dur;
    SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, t0, maxduration, t0);
    if (interrupt()) {
      break;
    };

    SampleDataD potentialtrace(t0, maxduration, trace(SpikeTrace[0]).stepsize(), 0.0);
    trace( SpikeTrace[0] ).copy( signalTime(), potentialtrace );



//     plot
    // trace
    P.lock();
    P[0].clearData();
//    P[1].clearData();
    P[0].plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
    // IV
//    for ( int i=0; i<currenttrace.size(); i++ ) {
//      double x = potentialtrace[i];
//      double y = currenttrace[i];
//      P[1].plotPoint( x, Plot::First, y, Plot::First, 1, Plot::Dot, 1, Plot::First, Plot::Yellow, Plot::Solid );
//    }
//    P[1].setYRange( min(currenttrace)*1.05, max(currenttrace)*1.05);
    P.draw();
    P.unlock();
  }
  return Completed;
}



OutData StepAndSin::Act() {
  double holdingpotential = number( "holdingpotential" );
  double actmintest = number( "actmintest" );
  double actmaxtest = number( "actmaxtest" );
  double stepsize = number( "stepsize" );
  double stepduration = number( "stepduration" );

  OutData signal;
  signal.setTrace( PotentialOutput[0] );
  signal.constWave( stepduration, -1.0, holdingpotential );

  // back to holdingpotential
  OutData signal2;
  signal2.setTrace( PotentialOutput[0] );
  signal2.constWave( stepduration, -1.0, holdingpotential );

  for (double step = actmintest; step <= actmaxtest; step += stepsize) {
    //activation step
    OutData signal1;
    signal1.setTrace( PotentialOutput[0] );
    signal1.pulseWave( stepduration, -1.0, step, holdingpotential );

    //append activation and inactivation step
    signal.append( signal1 );
    signal.append( signal2 );
    }
  return signal;
}


OutData StepAndSin::Sins() {
  double sinduration = number( "sinduration" );
  double holdingpotential = number( "holdingpotential" );
  double sinVbase = number( "sinVbase" );
  Str amplitude_strings = allText( "sinamplitudes" );
  Str frequency_strings = allText( "sinfrequencies" );
  std::vector<double> sinamplitudes;
  std::vector<double> sinfreqs;
  amplitude_strings.range( sinamplitudes, ",", ":" );
  frequency_strings.range( sinfreqs, ",", ":");

  //stimulus
  OutData signal;
  signal.setTrace(PotentialOutput[0]);
  signal.constWave( sinduration, -1.0, sinVbase );

  //back to holdingpotential
  OutData signal3;
  signal3.setTrace(PotentialOutput[0]);
  signal3.constWave( 0.01, -1.0, holdingpotential );


  for ( unsigned j = 0; j < sinamplitudes.size(); j++ ) {
    OutData signal2;
    signal2.setTrace(PotentialOutput[0]);
    signal2.sineWave( sinduration*1.5, -1.0, sinfreqs[j], 0.0, sinamplitudes[j] );
//    for ( int i=0; i<signal.size(); i++ ) {
//      signal[i] += signal2[i];
//    }
    signal = signal + signal2;
  }
  OutData signal2;
  signal2.setTrace(PotentialOutput[0]);
  signal2.constWave( sinduration, -1.0, 0.0 );
  for( int j = 0; j<signal2.size(); j++ ) {
    signal2[j] = signal[j];
  }

  signal2.append( signal3 );
  return signal;
}


addRePro( StepAndSin, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_stepandsin.cc"
