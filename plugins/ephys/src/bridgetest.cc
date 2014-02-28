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
#include <relacs/ephys/bridgetest.h>
using namespace relacs;

namespace ephys {


BridgeTest::BridgeTest( void )
  : RePro( "BridgeTest", "patchclamp", "Jan Benda", "2.0", "Feb 27, 2014" )
{
  // add some options:
  addNumber( "amplitude", "Amplitude of stimulus", 1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of stimulus", 0.01, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause between pulses", 0.1, 0.01, 1.0, 0.01, "sec", "ms" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "Voltage [mV]" );
  P.unlock();
  setWidget( &P );
}


void BridgeTest::preConfig( void )
{
  if ( CurrentOutput[0] >= 0 )
    setUnit( "amplitude", outTrace( CurrentOutput[0] ).unit() );
  else
    setUnit( "amplitude", outTrace( 0 ).unit() );
}


int BridgeTest::main( void )
{
  // get options:
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  double pause = number( "pause" );

  // don't print repro message:
  noMessage();

  // in- and outtrace:
  const InData &intrace = trace( SpikeTrace[0] >= 0 ? SpikeTrace[0] : 0 );
  int outtrace = CurrentOutput[0] >= 0 ? CurrentOutput[0] : 0;

  // plot:
  double ymin = 0.0;
  double ymax = 0.0;
  double tmin =  -0.5*duration;
  if ( tmin < -pause )
    tmin = -pause;
  double tmax = 3.5*duration;
  if ( tmax > duration+pause )
    tmax = duration+pause;
  P.lock();
  P.setXRange( 1000.0*tmin, 1000.0*tmax );
  P.setXLabel( "Time [ms]" );
  P.setYLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P.unlock();

  // plot trace:
  tracePlotSignal( 3.0*duration, 0.5*duration );

  // signal:
  double samplerate = intrace.sampleRate();
  OutData signal;
  signal.setTrace( outtrace );
  signal.pulseWave( duration, 1.0/samplerate, amplitude, 0.0 );

  // message:
  Str s = "Amplitude <b>" + Str( amplitude ) + " nA</b>";
  s += ",  Duration <b>" + Str( 1000.0*duration, "%.0f" ) + " ms</b>";
  message( s );
    
  // write stimulus:
  while ( true ) {

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    sleep( duration + pause );
    if ( interrupt() ) {
      writeZero( outtrace );
      return Completed;
    }

    // get trace:
    SampleDataF output( tmin, tmax, intrace.stepsize(), 0.0F );
    intrace.copy( signalTime(), output );
    float min = 0.0;
    float max = 0.0;
    minMax( min, max, output );
    double mean = 0.5*(min+max);
    double range = 1.1*0.5*(max- min);
    min = mean - range;
    max = mean + range;
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
    P.plot( output, 1000.0, Plot::Green, 2, Plot::Solid );
    P.draw();
    P.unlock();
  }

  writeZero( outtrace );
  return Completed;
}


addRePro( BridgeTest, patchclamp );

}; /* namespace patchclamp */

#include "moc_bridgetest.cc"
