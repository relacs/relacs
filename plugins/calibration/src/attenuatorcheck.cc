/*
  calibration/attenuatorcheck.cc
  Checks performance of attenuator device

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/rangeloop.h>
#include <relacs/calibration/attenuatorcheck.h>
using namespace relacs;

namespace calibration {


AttenuatorCheck::AttenuatorCheck( void )
  : RePro( "AttenuatorCheck", "calibration", "Jan Benda", "1.0", "Jun 20, 2013" )
{
  // add some options:
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "frequency", "Frequency of stimulus", 100.0, 10.0, 100000.0, 10.0, "Hz", "Hz" );
  addNumber( "minlevel", "Minimum attenuation level", 0.0, -1000.0, 1000.0, 0.1, "dB", "dB" );
  addNumber( "maxlevel", "Maximum attenuation level", 100.0, -1000.0, 1000.0, 0.1, "dB", "dB" );
  addNumber( "dlevel", "Increment of attenuation level", 1.0, 0.0, 1000.0, 0.1, "dB", "dB" );

  setWidget( &P );
}


int AttenuatorCheck::main( void )
{
  // get options:
  double duration = number( "duration" );
  double frequency = number( "frequency" );
  double minlevel = number( "minlevel" );
  double maxlevel = number( "maxlevel" );
  double dlevel = number( "dlevel" );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( duration, 0.0 );

  P.lock();
  P.setXRange( minlevel, Plot::AutoScale );
  P.setXLabel( "Attenuation level [dB]" );
  P.setYLabel( "Log10 signal amplitude" );
  P.unlock();

  OutData signal;
  signal.setTrace( 0 );
  signal.sineWave( duration, -1.0, frequency );
  adjustGain( trace( 0 ), 10.0 );

  RangeLoop levelrange( minlevel, maxlevel, dlevel, 1, 1, 1 );

  MapD amplitudes;
  amplitudes.reserve( levelrange.size() );

  for ( levelrange.reset(); ! levelrange && softStop() == 0; ++levelrange ) {

    double level = *levelrange;
    signal.setNoIntensity();
    signal.setLevel( level );
    write( signal );
    if ( signal.failed() ) {
      if ( signal.error() & OutData::AttOverflow )
	break;
      else if ( signal.error() & OutData::AttUnderflow )
	continue;
      else
	return Failed;
    }
    message( "Test attenuation level <b>" + Str( level, "%.1f" ) + " dB</b>" );

    sleep( signal.length() );
    if ( interrupt() )
      return Aborted;

    // analyze:
    double ampl = trace( 0 ).stdev( signalTime(), signalTime()+signal.length() ) * ::sqrt( 2.0 );
    amplitudes.push( level, ::log10( ampl ) );
    adjustGain( trace( 0 ), ampl );

    P.lock();
    P.clear();
    P.plot( amplitudes, 1.0, Plot::Red, 2, Plot::Solid,
	    Plot::Circle, 10, Plot::Red, Plot::Red );
    if ( amplitudes.size() > 2 ) {
      P.plotLine( amplitudes.x().front(), amplitudes.y().front(),
		  amplitudes.x().back(), amplitudes.y().back(),
		  Plot::Yellow, 2 );
    }
    P.draw();
    P.unlock();
  }

  adjustGain( trace( 0 ), 10.0 );
  return Completed;
}


addRePro( AttenuatorCheck, calibration );

}; /* namespace calibration */

#include "moc_attenuatorcheck.cc"
