/*
  voltageclamp/sinussum.cc
  Sum of sinus as VC protocol

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

#include <relacs/voltageclamp/sinussum.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>

using namespace relacs;

namespace voltageclamp {


SinusSum::SinusSum( void )
  : PNSubtraction( "SinusSum", "voltageclamp", "Lukas Sonnenberg", "1.0", "Sep 06, 2019" )
{
  // add some options:
  addNumber( "duration", "Stimulus duration", 0.01, 0.001, 100000.0, 0.001, "s", "s" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "s", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "holdingpotential", "Holding potential0", -100.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "V0", "V0", -50.7, -200.0, 200.0, 0.1, "mV" );
  addText("amplitudes", "Amplitudes", "60.31,  40.67, 30.12, 40.52").setUnit( "mV" );
  addText("frequencies", "Frequencies", "14.4, 86.7, 144.8, 392.8").setUnit( "Hz" );

  // plot
  P.lock();
  P.resize( 2, 2, true );
  P[0].setXLabel( "Time [s]" );
  P[0].setYLabel( "Current [nA]" );
  P[1].setXLabel( "Time [s]" );
  P[1].setYLabel( "Voltage [mV]");

  P.unlock();
  setWidget( &P );
}



int SinusSum::main( void )
{
  // get options:
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential = number( "holdingpotential" );
  double V0 = number( "V0" );

  Str amplitude_strings = allText( "amplitudes" );
  Str frequency_strings = allText( "frequencies" );

  std::vector<double> amplitudes;
  std::vector<double> freqs;
  amplitude_strings.range(amplitudes, ",", ":" );
  frequency_strings.range(freqs, ",", ":");

  // don't print repro message:
  noMessage();

  // set amplifier to VC mode
  ephys::AmplifierControl *ampl = dynamic_cast< ephys::AmplifierControl* >( control( "AmplifierControl" ) );
  if ( ampl == 0 ) {
    warning( "No amplifier found." );
    return Failed;
  }
  ampl ->activateVoltageClampMode();

  // reset plot
  P.lock();
  P[0].clearData();
  P[1].clearData();
  P[1].setXRange( 0.0, duration );
  P.unlock();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );

  // write holdingpotential:
  write( holdingsignal );
  sleep( pause );
  
  double t0 = 0.0;
  for ( int Count=0; ( repeats <= 0 || Count < repeats ) && softStop() == 0; Count++ ) {
    //stimulus
    OutData signal;
    signal.setTrace(PotentialOutput[0]);
    signal.constWave(duration, -1.0, V0);

    // nix options
    Parameter &p1 = signal.description().addText( "amplitudes", amplitude_strings);
    Parameter &p2 = signal.description().addText( "frequencies", frequency_strings );
    Parameter &p3 = signal.description()["Intensity"];
    signal.setMutable( p1 );
    signal.setMutable( p2 );
    signal.setMutable( p3 );
    Options opts = signal.description();

    for ( unsigned j = 0; j < amplitudes.size(); j++) {
      OutData signal2;
      signal2.setTrace(PotentialOutput[0]);
      signal2.sineWave( duration, -1.0, freqs[j], 0.0, 1.0 );
      signal = signal + amplitudes[j] * signal2;
    }

    SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, t0, duration, t0 );

    if (interrupt()) {
      return 0;
    };

    // plot
    P.lock();
    // trace
    P[0].plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
    P[1].plot( signal, 1000000.0, Plot::Yellow, 2, Plot::Solid);
    P.draw();
    P.unlock();
  }
  return Completed;
}


addRePro( SinusSum, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_sinussum.cc"
