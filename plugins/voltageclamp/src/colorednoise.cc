/*
  voltageclamp/colorednoise.cc
  Colored Noise stimulus with exponential decaying/increasing power

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

#include <relacs/voltageclamp/colorednoise.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>
#include <relacs/fitalgorithm.h>
#include <relacs/random.h>

using namespace relacs;

namespace voltageclamp {


ColoredNoise::ColoredNoise( void )
  : PNSubtraction( "ColoredNoise", "voltageclamp", "Lukas Sonnenberg", "1.0", "Apr 24, 2020" )
{
    addNumber( "duration", "Stimulus duration", 3, 0.001, 100000.0, 0.001, "s", "s" );
    addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "s", "ms" );
    addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
    addNumber( "holdingpotential", "Holding potential", -100.0, -200.0, 200.0, 1.0, "mV" );
    addNumber( "V0", "V0", -60.0, -200.0, 200.0, 0.1, "mV" );
    addNumber( "frequencyconstant", "Frequency Constant", 800.0, 1.0, 10000.0, 1.0, "Hz" );
    addNumber( "maxamplitude", "Maximum Amplitude", 110.0, 0.0, 200.0, 1.0, "mV" );

    setWidget( &P );
}


int ColoredNoise::main( void )
{
  // get options:
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential = number( "holdingpotential" );
  double V0 = number( "V0" );
  double frequencyconstant = number( "frequencyconstant" );
  double maxamplitude = number( "maxamplitude" );

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

//  P.lock();
//  P.resize( 2, 2, true );
//  P[0].setXLabel( "Time [ms]" );
//  P[0].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]"  );
//  P[1].setXLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]"  );
//  P[1].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]" );
////  P[1].setYLabel( "conductance [\u03BCS]" );
////  P[1].setY2Tics( 0.0, 10.0 );
//
//  P[0].clearData();
//  P[1].clearData();
//  P[1].setXRange( V0 - 1.05 * maxamplitude, V0 + 1.05 * maxamplitude );
//  P.unlock();


  // colored noise parameters
  ArrayD expParam( 3, 1.0 );
  expParam[0] = 1;
  expParam[1] = frequencyconstant;
  expParam[2] = 0;

  double t0 = -0.002;

  for ( int Count=0; ( repeats <= 0 || Count < repeats ) && softStop() == 0; Count++ ) {
    //stimulus
    OutData signal;
    signal.setTrace(PotentialOutput[0]);
    signal.constWave(duration, -1.0, V0);

    SampleDataD f( signal.size() / 2 );
    f /= signal.size() * signal.stepsize();
    SampleDataD f2( signal.size() / 2 );
    f2( signal.size() / 2 );
    for (int k=0; k<f2.size(); k++) {
      f2[k] = -f[f.size()-k];
    };
    f.append( f2 );

    OutData signal2;
    signal2.setTrace(PotentialOutput[0]);
    signal2.constWave(duration, -1.0, 0.0);
    for (int k=0; k<f.size(); k++) {
      signal2[k] = expFunc2( abs( f[k] ), expParam ) * (rnd() - 0.5);
    };
    hcFFT( signal2 );
    signal2 *= maxamplitude / 0.5;

    OutData signal3;
    signal3.setTrace(PotentialOutput[0]);
    signal3.constWave(0.1, -1.0, holdingpotential );

    signal = signal + signal2;
    signal.append( signal3 );

    // nix options
    Parameter &p1 = signal.description().addNumber( "maxamplitude", maxamplitude, "mV" );
    Parameter &p2 = signal.description().addNumber( "frequencyconstant", frequencyconstant, "Hz" );
    Parameter &p3 = signal.description().addNumber( "V_base", V0, "mV" );
    Parameter &p4 = signal.description()["Intensity"];
    signal.setMutable( p1 );
    signal.setMutable( p2 );
    signal.setMutable( p3 );
    signal.setMutable( p4 );
    Options opts = signal.description();

    SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, t0, duration, t0 );

    SampleDataD potentialtrace( t0, duration, trace(SpikeTrace[0]).stepsize(), 0.0 );
    trace(SpikeTrace[0]).copy(signalTime(), potentialtrace);

//    //plot
//    P.lock();
//    //trace
//    P[0].plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
//
//    //IV
//    P[1].plot( potentialtrace, currenttrace, Plot::Yellow, 3, Plot::Solid );
//    P[1].setYRange( P[0].yminRange(), P[0].ymaxRange() );


    if (interrupt()) {
      break;



    };

  }


  return Completed;
}


addRePro( ColoredNoise, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_colorednoise.cc"
