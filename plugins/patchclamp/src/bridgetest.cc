/*
  patchclamp/bridgetest.cc
  Short current pulses for testing the bridge

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

#include <relacs/sampledata.h>
#include <relacs/str.h>
#include <relacs/patchclamp/bridgetest.h>
using namespace relacs;

namespace patchclamp {


BridgeTest::BridgeTest( void )
  : RePro( "BridgeTest", "patchclamp", "Jan Benda", "1.0", "Oct 05, 2012" )
{
  // add some options:
  addNumber( "amplitude", "Amplitude of current pulse", 1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of curent pulse", 0.005, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause between pulses", 0.01, 0.001, 1.0, 0.001, "sec", "ms" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "Voltage [mv]" );
  P.unlock();
  setWidget( &P );
}


int BridgeTest::main( void )
{
  // get options:
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  double pause = number( "pause" );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( 3.0*duration, 0.5*duration );

  // plot:
  double ymin = 0.0;
  double ymax = 0.0;
  double tmin = -0.5*duration;
  if ( tmin < -pause )
    tmin = -pause;
  double tmax = 3.5*duration;
  if ( tmax > duration+pause )
    tmax = duration+pause;
  P.lock();
  P.setXRange( 1000.0*tmin, 1000.0*tmax );
  P.unlock();

  // signal:
  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  OutData signal( duration, 1.0/samplerate );
  signal = amplitude;
  signal.setIdent( "const" );
  signal.back() = 0.0;
  signal.setTrace( CurrentOutput[0] );

  // write stimulus:
  while ( true ) {
    Str s = "Amplitude <b>" + Str( amplitude ) + " nA</b>";
    s += ",  Duration <b>" + Str( 1000.0*duration ) + " ms</b>";
    message( s );

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    sleep( duration + pause );
    if ( interrupt() )
      return Completed;

    // get trace:
    SampleDataF data( tmin, tmax, trace( SpikeTrace[0] ).stepsize(), 0.0F );
    trace( SpikeTrace[0] ).copy( signalTime(), data );
    float min = 0.0;
    float max = 0.0;
    minMax( min, max, data );
    if ( ymin == 0.0 && ymax == 0.0 ) {
      ymin = min;
      ymax = max;
    }
    else {
      double rate = 0.001;
      ymin += ( min - ymin )*rate;
      ymax += ( max - ymax )*rate;
      if ( ymax < max )
	ymax = max;
      if ( ymin > min )
	ymin = min;
    }

    // plot:
    P.lock();
    P.clear();
    P.setYRange( ymin, ymax );
    P.plotVLine( 0.0, Plot::White, 2 );
    P.plotVLine( 1000.0*duration, Plot::White, 2 );
    P.plot( data, 1000.0, Plot::Green, 2, Plot::Solid );
    P.draw();
    P.unlock();
  }


  //  writeZero( outtrace );
  return Completed;
}


addRePro( BridgeTest, patchclamp );

}; /* namespace patchclamp */

#include "moc_bridgetest.cc"
