/*
  patchclampprojects/thresholdlatencies.cc
  Measures spike latencies in response to pulses close to the firing threshold.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <fstream>
#include <relacs/tablekey.h>
#include <relacs/patchclampprojects/thresholdlatencies.h>
using namespace relacs;

namespace patchclampprojects {


ThresholdLatencies::ThresholdLatencies( void )
  : RePro( "ThresholdLatencies", "ThresholdLatencies", "patchclampprojects",
	   "Jan Benda", "1.0", "Feb 04, 2010" ),
    P( this )
{
  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addSelection( "outtrace", "Output trace", "Current-1" );
  addNumber( "amplitude", "Amplitude of output signal", -1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
}


void ThresholdLatencies::config( void )
{
  setText( "intrace", spikeTraceNames() );
  setToDefault( "intrace" );
  setText( "outtrace", currentOutputNames() );
  setToDefault( "outtrace" );
}


void ThresholdLatencies::notify( void )
{
  /*
  int outtrace = index( "outtrace" );
  IUnit = outTrace( outtrace ).unit();
  setUnit( "amplitude", IUnit );
  IFac = Parameter::changeUnit( 1.0, IUnit, "nA" );

  int intrace = index( "intrace" );
  VUnit = trace( intrace ).unit();
  VFac = Parameter::changeUnit( 1.0, VUnit, "mV" );
  */
}


int ThresholdLatencies::main( void )
{
  int intrace = traceIndex( text( "intrace", 0 ) );
  int outtrace = index( "outtrace" );
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  if ( pause < 2.0*duration ) {
    warning( "Pause must be at least two times the stimulus duration!" );
    return Failed;
  }

  double samplerate = trace( intrace ).sampleRate();

  // don't print repro message:
  noMessage();

  // init:


  // plot trace:
  plotToggle( true, true, 2.0*duration, 0.5*duration );

  // plot:
  P.lock();
  P.setXRange( -500.0*duration, 2000.0*duration );
  //  P.setYLabel( trace( intrace ).ident() + " [" + VUnit + "]" );
  P.unlock();

  // signal:
  OutData signal( duration, 1.0/samplerate );
  signal = amplitude;
  signal.setIdent( "const" );
  signal.back() = 0.0;
  signal.setTrace( outtrace );

  // write stimulus:
  sleep( pause );
  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {

    /*
    Str s = "Amplitude <b>" + Str( amplitude ) + " " + IUnit +"</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    message( s );
    */

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }

    sleep( duration + pause );
    if ( interrupt() ) {
      if ( count > 0 )
	break;
      else
	return Aborted;
    }

    //    analyze( trace( intrace ), duration, count );
    plot();

  }

  save();

  return Completed;
}


addRePro( ThresholdLatencies );

}; /* namespace patchclampprojects */

#include "moc_thresholdlatencies.cc"
