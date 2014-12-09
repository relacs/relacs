/*
  ephys/capacitycompensation.cc
  Lissajous figures for checking the capacity compensation of the amplifier.

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
#include <relacs/ephys/capacitycompensation.h>
using namespace relacs;

namespace ephys {


CapacityCompensation::CapacityCompensation( void )
  : RePro( "CapacityCompensation", "ephys", "Jan Benda", "1.0", "Feb 28, 2014" )
{
  // add some options:
  addNumber( "amplitude", "Amplitude of stimulus", 1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of stimulus", 0.2, 0.01, 10000.0, 0.01, "sec", "ms" );
  addNumber( "frequency", "Frequency of sine-wave stimulus", 100.0, 1.0, 10000.0, 1.0, "Hz" );
  addInteger( "skipcycles", "Number of initial cycles to be skipped", 10, 0, 10000, 1 );
  addNumber( "pause", "Duration of pause between pulses", 0.1, 0.01, 10000.0, 0.01, "sec", "ms" );

  // plot:
  setWidget( &P );
}


void CapacityCompensation::preConfig( void )
{
  if ( CurrentTrace[0] >= 0 )
    setUnit( "amplitude", trace( CurrentTrace[0] ).unit() );
  else if ( CurrentOutput[0] >= 0 )
    setUnit( "amplitude", outTrace( CurrentOutput[0] ).unit() );
  else
    setUnit( "amplitude", outTrace( 0 ).unit() );
}


int CapacityCompensation::main( void )
{
  // get options:
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  double frequency = number( "frequency" );
  int skipcycles = integer( "skipcycles" );
  double pause = number( "pause" );

  // don't print repro message:
  noMessage();

  // in- and outtrace:
  const InData &intrace = trace( SpikeTrace[0] >= 0 ? SpikeTrace[0] : 0 );
  int outtrace = CurrentOutput[0] >= 0 ? CurrentOutput[0] : 0;

  // plot:
  double ymin = 0.0;
  double ymax = 0.0;
  double tmin = skipcycles/frequency;
  double tmax = duration;
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
    P.setXLabel( outTrace( outtrace ).traceName() + " [" + outTrace( outtrace ).unit() + "]" );
  P.setYLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P.unlock();

  // plot trace:
  tracePlotSignal( duration, 0.0 );

  // signal:
  double samplerate = intrace.sampleRate();
  OutData signal;
  signal.setTrace( outtrace );
  signal.sineWave( duration, 1.0/samplerate, frequency, 0.0, amplitude, 0.0 );

  // message:
  Str s = "Amplitude <b>" + Str( amplitude ) + " nA</b>";
  s += ",  Frequency <b>" + Str( frequency, "%.0f" ) + " Hz</b>";
  s += ",  Duration <b>" + Str( 1000.0*duration, "%.0f" ) + " ms</b>";
  message( s );
    
  // write stimulus:
  while ( ! interrupt() && softStop() == 0 ) {

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    if ( interrupt() )
      break;
    sleep( pause );

    // get input trace:
    SampleDataF input;
    if ( CurrentTrace[0] >=0 ) {
      SampleDataF data( tmin, tmax, trace( CurrentTrace[0] ).stepsize(), 0.0F );
      trace( CurrentTrace[0] ).copy( signalTime(), data );
      if ( ::fabs( trace( CurrentTrace[0] ).stepsize() - intrace.stepsize() ) < 1e-6 )
	input = data;
      else
	input.interpolate( data, tmin, intrace.stepsize() );
    }
    else {
      input.sin( tmin, tmax, intrace.stepsize(), frequency );
      input *= amplitude;
    }

    // get output trace:
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
    P.plot( input.array(), output.array(), Plot::Yellow, 2, Plot::Solid );
    SampleDataF inp;
    input.copy( duration-1.0/frequency, duration, inp );
    SampleDataF outp;
    output.copy( duration-1.0/frequency, duration, outp );
    P.plot( inp.array(), outp.array(), Plot::Red, 3, Plot::Solid );
    P.draw();
    P.unlock();
  }

  writeZero( outtrace );
  return Completed;
}


addRePro( CapacityCompensation, ephys );

}; /* namespace ephys */

#include "moc_capacitycompensation.cc"
