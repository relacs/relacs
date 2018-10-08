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
using namespace relacs;

namespace voltageclamp {


PNSubtraction::PNSubtraction( void )
  : RePro( "PNSubtraction", "voltageclamp", "Lukas Sonnenberg", "1.0", "Oct 08, 2018" )
{
  // add some options:
  addNumber( "pn", "p/N", -10, 10, 1, -4 );
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
}


int PNSubtraction::main( void )
{
  // get options:
  // double duration = number( "duration" );

  // don't print repro message:
  noMessage();

  return Completed;
}

SampleDataD PNSubtraction::PN_sub( OutData &signal, double &holdingpotential, double &pause ) {
  int pn = number( "pn" );
  double samplerate = signal.sampleRate();

  // don't print repro message:
  noMessage();

  OutData pn_signal;
  pn_signal.setTrace( PotentialOutput[0] );
  pn_signal = holdingpotential + (signal - holdingpotential)/pn;

  SampleDataD pn_trace( -0.002, pn_signal.rangeBack(), 1/samplerate );

  for ( int i = 0; i<::abs(pn); i++ ) {
    write(pn_signal);
    sleep(pause);

    SampleDataF currenttrace( -0.002, pn_signal.rangeBack(), trace(CurrentTrace[0]).stepsize() , 0.0);
    pn_trace += currenttrace/::abs(pn);

  };

  write(signal);
  sleep(pause);

  SampleDataF currenttrace( -0.002, pn_signal.rangeBack(), trace(CurrentTrace[0]).stepsize() , 0.0);
  currenttrace -= pn*pn_trace;

  return currenttrace;
};


addRePro( PNSubtraction, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_pnsubtraction.cc"
