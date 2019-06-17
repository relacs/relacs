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
#include <algorithm> // for copy() and assign()
#include <iterator> // for back_inserter

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
  addBoolean("qualitycontrol", "Quality control", true);
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
}


int PNSubtraction::main( void )
{
  return Completed;
}

SampleDataD PNSubtraction::PN_sub( OutData signal, Options &opts, double &holdingpotential, double &pause, double &mintime, double &maxtime, double &t0) {
  int pn = number( "pn" );
  double samplerate = signal.sampleRate();
  bool qualitycontrol = boolean( "qualitycontrol" );

  // add p/n option to signal
  Parameter &pn1 = opts.addNumber( "pn", pn );
  Parameter &qc1 = opts.addBoolean( "qualitycontrol", qualitycontrol );
  signal.setMutable( pn1 );
  signal.setMutable( qc1 );
  signal.setDescription( opts );

  // don't print repro message:
  noMessage();

  // make short quality assuring test-pulse
  if ( qualitycontrol ) {
    OutData quality_signal1;
    quality_signal1.setTrace( PotentialOutput[0] );
    quality_signal1.constWave( 0.010, -1.0, holdingpotential );

    OutData quality_signal2;
    quality_signal2.setTrace( PotentialOutput[0] );
    quality_signal2.pulseWave( 0.010, -1.0, holdingpotential-20, holdingpotential );


//    OutData quality_signal1;
//    quality_signal1.setTrace(PotentialOutput[0]);
//    quality_signal1.constWave(.010, -1.0, holdingpotential);
//
//    OutData quality_signal2;
//    quality_signal2.setTrace(PotentialOutput[0]);
//    quality_signal2.constWave(.010, -1.0, holdingpotential-20);
//
//    OutData quality_signal3;
//    quality_signal3.setTrace(PotentialOutput[0]);
//    quality_signal3.constWave(.010, -1.0, holdingpotential);
//
//    quality_signal1.append(quality_signal2);
//    quality_signal1.append(quality_signal3);

    quality_signal1.description().setType( "stimulus/Qualitycontrol" );
//    quality_signal1.setIdent( signal.ident() );
//    quality_signal1.setDescription( signal.description() );
//
//    cerr << "ident:" << signal.ident() << endl;
//    cerr << "ident2:" << quality_signal1.ident() << endl;
//    cerr << "descr:" << signal.description() << endl;
//    cerr << "qualityname:" << quality_signal1.description().type() << endl;

    write(quality_signal1);
    sleep(pause);
  };

  // skip prepulses if pn==0
  if ( pn == 0 ) {
    write(signal);
    sleep(pause);

    SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

    return currenttrace;
  };

  // give stimulus
  OutData pn_signal = signal;
  pn_signal.setTrace( PotentialOutput[0] );
  pn_signal = holdingpotential + (signal - holdingpotential)/pn;
  SampleDataD pn_trace( mintime, pn_signal.rangeBack(), 1/samplerate );
  pn_signal.description().setType( "stimulus/PNSubatraction" );

//  cerr << "pnname:"  << pn_signal.description().type() << endl;
//  cerr << "signalname:" << signal.description().type() << endl;


  for ( int i = 0; i<::abs(pn); i++ ) {
    write(pn_signal);
    sleep(pause);

    if (interrupt()) {
      break;
    };

    SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace );

    pn_trace += currenttrace;

  };
//  pn_trace -= pn_trace.mean(signalTime() + t0 - 0.001, signalTime() + t0);

  if (interrupt()) {
    return pn_trace;
  };

  signal.description().setType( "stimulus/Trace" );
  write(signal);
  sleep(pause);

  SampleDataD currenttrace( mintime, maxtime, trace(CurrentTrace[0]).stepsize() , 0.0);
  trace(CurrentTrace[0]).copy(signalTime(), currenttrace );


  currenttrace -= pn/::abs(pn)*pn_trace;// - currenttrace.mean(signalTime() + t0 - 0.001, signalTime() + t0);
  currenttrace -= currenttrace.mean( -samplerate/500, 0);

//  return pn_trace;
  return currenttrace;
};

}; /* namespace voltageclamp */

#include "moc_pnsubtraction.cc"
