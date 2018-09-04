/*
  voltageclamp/membranetest.cc
  get resistance of patch clamp

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

#include <relacs/voltageclamp/membranetest.h>
using namespace relacs;

namespace voltageclamp {


membranetest::membranetest( void )
  : RePro( "membranetest", "voltageclamp", "Lukas Sonnenberg", "1.0", "Sep 03, 2018" ) {
  // add some options:
//  newSection("Stimulus");
  addNumber("holdingpotential", "Holding Potential", -100.0, -1000.0, 1000.0, 1.0, "mV");
  addNumber("amplitude", "Amplitude of output signal", 20.0, -1000.0, 1000.0, 0.1, "mV");
  addNumber("duration", "Duration of output", 0.02, 0.001, 1000.0, 0.001, "sec", "ms");
  addNumber("pause", "Duration of pause bewteen outputs", 0.08, 0.001, 1000.0, 0.001, "sec", "ms");
  addInteger("repeats", "Repetitions of stimulus", 10, 0, 10000, 1).setStyle(OptWidget::SpecialInfinite);
  addBoolean("infinite", "Infinite repetitions of membranetest", true);
  //  newSection("Analysis");
//  addNumber( "sswidth", "Window length for steady-state analysis", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
//  addBoolean("plotstdev", "Plot standard deviation of current", true);
//  addSelection("setdata", "Set results to the session variables", "rest only|always|never");
//  addText( "checkoutput", "Outputs that need to be at their default value", "Current-1" ).setActivation( "setdata", "rest only" );

  // plot:
  P.lock();
  P.setXLabel("Time [ms]");
  P.unlock();
  setWidget(&P);
}

int membranetest::main( void )
{
//  OutParams.clear();
  // get options:
  double holdingpotential = number( "holdingpotential" );
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  bool infinite = boolean( "infinite" );
  double pause = number( "pause" );
//  int repeats = integer( "repeats" );

//  double sswidth = number( "sswidth" );
//  texts( "checkoutput", OutParams );

  if ( pause < 2.0*duration ) {
    warning("Pause must be at least two times the stimulus duration!");
    return Failed;
  }

  // don't print repro message:
  noMessage();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );

  // init:
//  DoneState state = Completed;
//  SampleDataD MeanPotential = SampleDataF( -0.5*duration, 2.0*duration, 1/samplerate, 0.0 );
//  SampleDataD SquarePotential = MeanPotential;
//  SampleDataD StdevPotential = MeanPotential;
//  TraceIndices.clear();
//  MeanTraces.clear();
//  SquareTraces.clear();
//
//  for ( int j=0; j<traces().size(); j++ ) {
//    if (j != SpikeTrace[0] && ::fabs(trace(j).stepsize() - trace(SpikeTrace[0]).stepsize()) / trace(SpikeTrace[0]).stepsize() < 1e-8) {
//      TraceIndices.push_back(j);
//      MeanTraces.push_back(MeanVoltage);
//      SquareTraces.push_back(MeanVoltage);
//    }
//  }

//  ExpOn = SampleDataF( 0.0, duration, 1/samplerate, 0.0 );
//  ExpOff = SampleDataF( duration, 2.0*duration, 1/samplerate, 0.0 );

  // plot:
  P.lock();
  P.clear();
//  P.setXRange( -500.0*Duration, 2000.0*Duration );
//  P.setYLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]" );
  P.draw();
  P.unlock();

  // signal:
  OutData signal;
  signal.setTrace( PotentialOutput[0] );
  signal.pulseWave( duration, -1.0, holdingpotential + amplitude, holdingpotential );
//  signal.setIdent( "I=" + Str( DCCurrent + Amplitude ) + IUnit );

  // sleep:
  sleepWait( pause );
  if ( interrupt() )
    return Aborted;

  write(holdingsignal);
  sleep(pause);

  if (infinite) {
    while (not interrupt()) {
      stimulus(signal);
    };
    return Aborted;
  }
  else {
    stimulus(signal);
    if (interrupt()) {
      return Aborted;
    };
  };
  return Completed;
}


void membranetest::stimulus(OutData &signal) {
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );

  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  SampleDataF MeanPot = SampleDataF(-duration,2*duration,1/samplerate,0.0);
  SampleDataF MeanSqPot = SampleDataF(-duration,2*duration,1/samplerate,0.0);
  SampleDataF StdPot = SampleDataF(-duration,2*duration,1/samplerate,0.0);

  for ( int Count=0; ( repeats <= 0 || Count < repeats ) && softStop() == 0; Count++ ) {
    write(signal);

    sleep(pause);

    SampleDataF currenttrace(-duration, 2 * duration, trace(CurrentTrace[0]).stepsize(), 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace);

    for (int i = 0; i < currenttrace.size(); i++) {
      MeanPot[i] += (currenttrace[i] - MeanPot[i]) / (Count + 1);
      MeanSqPot[i] += (currenttrace[i] * currenttrace[i] - MeanSqPot[i]) / (Count + 1);
      StdPot[i] = sqrt(MeanSqPot[i] - MeanPot[i] * MeanPot[i]);
    };

    if (interrupt()) {
      break;
    };

  }
  // plot
  P.lock();
  P.clear();
  P.plot(MeanSqPot, 1000.0, Plot::Yellow, 2, Plot::Solid);
  P.plot(MeanSqPot+StdPot, 1000.0, Plot::Red, 2, Plot::Solid);
  P.plot(MeanSqPot-StdPot, 1000.0, Plot::Red, 2, Plot::Solid);
  P.draw();
  P.unlock();


}

addRePro( membranetest, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_membranetest.cc"
