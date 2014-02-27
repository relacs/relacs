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
  : RePro( "BridgeTest", "patchclamp", "Jan Benda", "2.0", "Feb 27, 2014" )
{
  // add some options:
  addSelection( "type", "Type of measurement", "Pulse|Lissajous" );
  addNumber( "amplitude", "Amplitude of stimulus", 1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of stimulus", 0.005, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "frequency", "Frequency of sine-wave stimulus", 100.0, 1.0, 10000.0, 1.0, "Hz" ).setActivation( "type", "Lissajous" );
  addInteger( "skipcycles", "Number of initial cycles to be skipped", 10, 0, 10000, 1 ).setActivation( "type", "Lissajous" );
  addNumber( "pause", "Duration of pause between pulses", 0.01, 0.001, 1.0, 0.001, "sec", "ms" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "Voltage [mv]" );
  P.unlock();
  setWidget( &P );
}


void BridgeTest::preConfig( void )
{
  if ( CurrentTrace[0] >= 0 )
    setUnit( "amplitude", trace( CurrentTrace[0] ).unit() );
  else if ( CurrentOutput[0] >= 0 )
    setUnit( "amplitude", outTrace( CurrentOutput[0] ).unit() );
}


int BridgeTest::main( void )
{
  // get options:
  int type = index( "type" );
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  double frequency = number( "frequency" );
  int skipcycles = integer( "skipcycles" );
  double pause = number( "pause" );

  // don't print repro message:
  noMessage();

  // plot:
  double ymin = 0.0;
  double ymax = 0.0;
  double tmin = 0.0;
  double tmax = duration;
  if ( type == 0 ) {
    tmin = -0.5*duration;
    if ( tmin < -pause )
      tmin = -pause;
    tmax = 3.5*duration;
    if ( tmax > duration+pause )
      tmax = duration+pause;
    P.lock();
    P.setXRange( 1000.0*tmin, 1000.0*tmax );
    P.setXLabel( "Time [ms]" );
    P.setYLabel( trace( SpikeTrace[0] ).ident() + " [" + trace( SpikeTrace[0] ).unit() + "]" );
    P.unlock();
  }
  else {
    tmin = skipcycles/frequency;
    if ( duration - tmin < tmin ) {
      duration = 2.0*tmin;
      tmax = duration;
      warning( "Duration too small. Set to at least " + Str( 1000.0*duration, "%.0f" ) + "ms !", 4.0 );
    }
    P.lock();
    P.setXRange( -1.1*amplitude, 1.1*amplitude );
    if ( CurrentTrace[0] >= 0 )
      P.setXLabel( trace( CurrentTrace[0] ).ident() + " [" + trace( CurrentTrace[0] ).unit() + "]" );
    else
      P.setXLabel( outTrace( CurrentOutput[0] ).traceName() + " [" + outTrace( CurrentOutput[0] ).unit() + "]" );
    P.setYLabel( trace( SpikeTrace[0] ).ident() + " [" + trace( SpikeTrace[0] ).unit() + "]" );
    P.unlock();
  }

  // plot trace:
  if ( type > 0 )
    tracePlotSignal( duration, 0.0 );
  else
    tracePlotSignal( 3.0*duration, 0.5*duration );

  // signal:
  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  OutData signal;
  signal.setTrace( CurrentOutput[0] );
  if ( type > 0 )
    signal.sineWave( duration, 1.0/samplerate, frequency, amplitude, 0.0 );
  else
    signal.pulseWave( duration, 1.0/samplerate, amplitude, 0.0 );

  // message:
  Str s = "Amplitude <b>" + Str( amplitude ) + " nA</b>";
  if ( type > 0 )
    s += ",  Frequency <b>" + Str( frequency, "%.0f" ) + " Hz</b>";
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
      writeZero( CurrentOutput[0] );
      return Completed;
    }

    // get trace:
    SampleDataF input;
    if ( type > 0 ) {
      if ( CurrentTrace[0] >=0 ) {
	SampleDataF data( tmin, tmax, trace( CurrentTrace[0] ).stepsize(), 0.0F );
	trace( CurrentTrace[0] ).copy( signalTime(), data );
	if ( ::fabs( trace( CurrentTrace[0] ).stepsize() - trace( SpikeTrace[0] ).stepsize() ) < 1e-6 )
	  input = data;
	else
	  input.interpolate( data, tmin, trace( SpikeTrace[0] ).stepsize() );
      }
      else {
	input.sin( tmin, tmax, trace( SpikeTrace[0] ).stepsize(), frequency );
	input *= amplitude;
      }
    }
    SampleDataF output( tmin, tmax, trace( SpikeTrace[0] ).stepsize(), 0.0F );
    trace( SpikeTrace[0] ).copy( signalTime(), output );
    float min = 0.0;
    float max = 0.0;
    minMax( min, max, output );
    if ( ymin == 0.0 && ymax == 0.0 ) {
      ymin = min;
      ymax = max;
    }
    else {
      double rate = 0.1;
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
    if ( type > 0 ) {
      P.plot( input.array(), output.array(), Plot::Yellow, 2, Plot::Solid );
      SampleDataF inp;
      input.copy( duration-1.0/frequency, duration, inp );
      SampleDataF outp;
      output.copy( duration-1.0/frequency, duration, outp );
      P.plot( inp.array(), outp.array(), Plot::Red, 3, Plot::Solid );
    }
    else {
      P.plotVLine( 0.0, Plot::White, 2 );
      P.plotVLine( 1000.0*duration, Plot::White, 2 );
      P.plot( output, 1000.0, Plot::Green, 2, Plot::Solid );
    }
    P.draw();
    P.unlock();
  }

  writeZero( CurrentOutput[0] );
  return Completed;
}


addRePro( BridgeTest, patchclamp );

}; /* namespace patchclamp */

#include "moc_bridgetest.cc"
