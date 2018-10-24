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


PNSubtraction::PNSubtraction( const string &name,
	 const string &pluginset,
	 const string &author,
	 const string &version,
	 const string &date)
  : RePro( name, pluginset, author, version, date )
{
  // add some options:
  addNumber( "pn", "p/N", -4, -100, 100, 1 );
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
}


int PNSubtraction::main( void )
{
  return Completed;
}

SampleDataD PNSubtraction::PN_sub( OutData &signal, double &holdingpotential, double &pause, double &mintime, double &maxtime, double &t0 ) {
  int pn = number( "pn" );
  double samplerate = signal.sampleRate();

  if ( pn == 0 ) {
    write(signal);
    sleep(pause);

    SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

    return currenttrace;
  };

  // don't print repro message:
  noMessage();

  OutData pn_signal;
  pn_signal.setTrace( PotentialOutput[0] );
  pn_signal = holdingpotential + (signal - holdingpotential)/pn;

  SampleDataD pn_trace( mintime, pn_signal.rangeBack(), 1/samplerate );

  for ( int i = 0; i<::abs(pn); i++ ) {
    write(pn_signal);
    sleep(pause);

    if (interrupt()) {
      break;
    };

    SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

    pn_trace += currenttrace;// - currenttrace[0];

  };
//  pn_trace -= pn_trace.mean(signalTime() + t0 - 0.001, signalTime() + t0);

  if (interrupt()) {
    return pn_trace;
  };

  write(signal);
  sleep(pause);

  SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
  trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

  currenttrace -= pn/::abs(pn)*pn_trace;// - currenttrace.mean(signalTime() + t0 - 0.001, signalTime() + t0);
  currenttrace -= currenttrace(t0);//currenttrace.mean(signalTime() + t0 - 0.001, signalTime() + t0);

//  return pn_trace;
  return currenttrace;
};

}; /* namespace voltageclamp */

#include "moc_pnsubtraction.cc"
