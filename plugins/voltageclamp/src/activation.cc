/*
  voltageclamp/activation.cc
  Activation protocol

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

#include <relacs/voltageclamp/activation.h>
using namespace relacs;

namespace voltageclamp {


Activation::Activation( void )
  : RePro( "Activation", "voltageclamp", "Jan Benda & Lukas Sonnenberg", "1.0", "Aug 09, 2018" )
{
  // add some options:
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "holdingpotential", "Holding potential", -100.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "testingpotential", "Testing potential", 0.0, -200.0, 200.0, 1.0, "mV" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "Current [nA]" );
  P.unlock();
  setWidget( &P );
}


int Activation::main( void )
{
  // get options:
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential = number( "holdingpotential" );
  double testingpotential = number( "testingpotential" );

  // don't print repro message:
  noMessage();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );

  // stimulus:
  OutData signal;
  signal.setTrace( PotentialOutput[0] );
  // signal.pulseWave( duration, -1.0,  testingpotential, holdingpotential );
  signal.constWave( duration, -1.0,  testingpotential );

  OutData signal2;
  signal2.setTrace( PotentialOutput[0] );
  signal2.pulseWave( duration, -1.0,  20.0, holdingpotential );

  signal.append( signal2 );

  // write stimulus:
  write( holdingsignal );
  sleep( pause );
  for ( int Count=0;
	( repeats <= 0 || Count < repeats ) && softStop() == 0;
	Count++ ) {

    Str s = "Holding potential <b>" + Str( holdingpotential, "%.1f" ) + " mV</b>";
    s += ", Testing potential <b>" + Str( testingpotential, "%.1f" ) + " mV</b>";
    s += ",  Loop <b>" + Str( Count+1 ) + "</b>";
    message( s );

    write( signal );
    sleep( pause );

    SampleDataF currenttrace( -0.002, 0.01, trace( CurrentTrace[0] ).stepsize(), 0.0 );
    trace( CurrentTrace[0] ).copy( signalTime(), currenttrace );
    P.lock();
    P.clearData();
    P.plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
    P.draw();
    P.unlock();
    
  }
  return Completed;
}


addRePro( Activation, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_activation.cc"
