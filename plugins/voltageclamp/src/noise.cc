/*
  voltageclamp/noise.cc
  Give voltageclamp white noise signal with cutoff frequencies

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

#include <relacs/tablekey.h>
#include <relacs/voltageclamp/noise.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>

using namespace relacs;

namespace voltageclamp {


Noise::Noise( void )
  : PNSubtraction( "Noise", "voltageclamp", "Lukas Sonnenberg", "1.0", "Dec 10, 2019" ) {
  // options:
  addNumber( "holdingpotential", "Holdingpotential", -100.0, -200.0, 200.0, 1.0, "mV", "mV" );
  addNumber( "offset", "Offset", 50.0, -200.0, 200.0, 1.0, "mV", "mV" );
  addNumber( "amplitude", "Amplitude", 50.0, 0.0, 100000.0, 1.0, "mV", "mV" );
  addNumber( "fmin", "Minimum frequency", 0.0, 0.0, 10000000.0, 100.0, "Hz", "Hz" );
  addNumber( "fmax", "Maximum frequency", 1000.0, 0.0, 10000000.0, 100.0, "Hz", "Hz" );
  addNumber( "duration", "Duration of noise stimulus", 1.0, 0.0, 10000.0, 0.1, "s" );
  addNumber( "pause", "Length of pause inbetween successive stimuli", 1.0, 0.0, 10000.0, 0.1, "s" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1).setStyle(OptWidget::SpecialInfinite );

//  setWidget( &P );
}

int Noise::main( void )
{
  // get options:
  double holdingpotential = number( "holdingpotential" );
  double offset = number( "offset" );
  double amplitude = number( "amplitude" );
//  double clip = number( "clip" );
//  double intensity = number( "intensity" );
  double fmin = number( "fmin" );
  double fmax = number( "fmax" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );

  // set amplifier to VC mode
  ephys::AmplifierControl *ampl = dynamic_cast< ephys::AmplifierControl* >( control( "AmplifierControl" ) );
  ampl ->activateVoltageClampMode();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );
  write( holdingsignal );
  sleep( pause );
  double samplerate = holdingsignal.sampleRate();

  if ( fmax > 0.5*samplerate+1.0e-8 ) {
    warning("Maximum frequency " + Str(fmax) +
            "Hz must be less than or equal to half the sampling rate " +
            Str(samplerate) + "Hz!");
    return Failed;
  }
  if ( fmin >= fmax ) {
    warning("Minimum frequency " + Str(fmin) +
            "Hz must be smaller than maximum frequency " + Str(fmax) + "Hz");
    return Failed;
  }

  // don't print repro message:
  noMessage();



  for ( int Count=0;
        ( repeats <= 0 || Count < repeats ) && softStop() == 0;
        Count++ ) {
    Str s = "Holding potential <b>" + Str(holdingpotential, "%.1f") + " mV</b>";
    s += ",  Frequency <b>" + Str( fmin, "%.0f" ) + " - " + Str( fmax, "%.0f" ) + " Hz</b>";
    s += ",  Loop <b>" + Str( Count+1 ) + "</b>";
    message(s);

    OutData signal;
    signal.setTrace( PotentialOutput[0] );
    signal.bandNoiseWave( duration, -1.0, fmin, fmax, 1.0 );
    signal = signal/max(signal);
    signal = signal * amplitude + holdingpotential + offset;

    Parameter &f0 = signal.description().addNumber( "fmin", fmin, "Hz" );
    Parameter &f1 = signal.description().addNumber( "fmax", fmax, "Hz" );
    Parameter &off = signal.description().addNumber( "offset", offset, "mV" );

    signal.setMutable( f0 );
    signal.setMutable( f1 );
    signal.setMutable( off );
    Options opts = signal.description();

    double mintime = -0.002;
    double maxtime = 0.01;
    double t0 = -0.002;
    SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, mintime, maxtime, t0 );

    if (interrupt()) {
      break;
    }
  }

  return Completed;
}


addRePro( Noise, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_noise.cc"
