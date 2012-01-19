/*
  patchclamp/simple.cc
  Simple RePro for testing dynamic clamp

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

#include <relacs/tablekey.h>
#include <relacs/patchclamp/simple.h>
using namespace relacs;

namespace patchclamp {


Simple::Simple( void )
  : RePro( "Simple", "patchclamp", "Jan Benda", "0.0", "Feb 14, 2008" )
{
  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addSelection( "outtrace", "Output trace", "Current-1" );
  addNumber( "amplitude", "Amplitude of output signal", 1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addSelection( "stimulus", "Stimulus type", "constant|ramp|sine 1p|sine 2p|zero" );
  addBoolean( "samerate", "Use sampling rate of input", true );
  addNumber( "samplerate", "Sampling rate of output", 1000.0, 0.0, 10000000.0, 1000.0, "Hz", "kHz" ).setActivation( "samerate", "false" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 );
  addBoolean( "save", "Save recorded traces", false );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "" );
  P.unlock();
  setWidget( &P );
}


void Simple::config( void )
{
  setText( "intrace", spikeTraceNames() );
  setToDefault( "intrace" );
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
}


void Simple::notify( void )
{
  int outtrace = index( "outtrace" );
  if ( outtrace >= 0 && outtrace < outTracesSize() )
    setUnit( "amplitude", outTrace( outtrace ).unit() );
}


int Simple::main( void )
{
  // get options:
  int intrace = traceIndex( text( "intrace", 0 ) );
  int outtrace = index( "outtrace" );
  string unit = outTrace( outtrace ).unit();
  double amplitude = number( "amplitude" );
  int stimulustype = index( "stimulus" );
  bool samerate = boolean( "samerate" );
  double samplerate = number( "samplerate" );
  if ( samerate )
    samplerate = trace( intrace ).sampleRate();
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  bool save = boolean( "save" );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( 2.5*duration, 0.5*duration );

  // plot:
  P.lock();
  P.setXRange( -1000.0*duration, 1000.0*duration );
  P.unlock();

  // signal:
  OutData signal( duration, 1.0/samplerate );
  if ( stimulustype == 0 ) {
    signal = amplitude;
    signal.setIdent( "const" );
  }
  else if ( stimulustype == 1 ) {
    for ( int k=0; k<signal.size(); k++ )
      signal[k] = amplitude*k/signal.size();
    signal.setIdent( "ramp" );
  }
  else if ( stimulustype == 2 ) {
    for ( int k=0; k<signal.size(); k++ )
      signal[k] = amplitude * ::sin( 2.0*M_PI*k/signal.size() );
    signal.setIdent( "sin1p" );
  }
  else if ( stimulustype == 3 ) {
    for ( int k=0; k<signal.size(); k++ )
      signal[k] = amplitude * ::sin( 4.0*M_PI*k/signal.size() );
    signal.setIdent( "sin2p" );
  }
  else {
    signal = 0.0;
    signal.setIdent( "zero" );
  }
  signal.back() = 0.0;
  signal.setTrace( outtrace );

  // save:
  ofstream df;
  TableKey key;
  if ( save ) {
    df.open( addPath( "simpletrace.dat" ).c_str(),
	     ofstream::out | ofstream::app );
    if ( ! df.good() )
      save = false;
    else {
      // write header and key:
      Options header;
      header.addInteger( "index", totalRuns() );
      header.addText( "session time", sessionTimeStr() ); 
      header.addLabel( "settings:" );
      header.save( df, "# " );
      Options::save( df, "#   ", -1, 0, false, true );
      df << '\n';
      key.addNumber( "t", "ms", "%7.2f" );
      key.addNumber( trace( intrace ).ident(), trace( intrace ).unit(), "%7.2f" );
      key.saveKey( df, true, false );
    }
  }

  // write stimulus:
  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {

    Str s = "Amplitude <b>" + Str( amplitude ) + " " + unit +"</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    message( s );

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    sleep( duration );
    if ( interrupt() ) {
      //      writeZero( outtrace );
      return count > 2 ? Completed : Aborted;
    }

    //    analyze( trace( intrace ), duration, count );
    sleep( pause );

    if ( save ) {
      const InData &data = trace( intrace );
      for ( int k=data.index( signalTime()-0.5*duration );
	    k<data.index( signalTime()+2.0*duration ); k++ ) {
	key.save( df, 1000.0*(data.pos( k ) - signalTime()), 0 );
	key.save( df, data[k] );
	df << '\n';
      }
      df << '\n';
    }

    if ( interrupt() ) {
      if ( save ) {
	df << '\n';
	df.close();
      }
      //      writeZero( outtrace );
      return count > 2 ? Completed : Aborted;
    }

  }

  if ( save ) {
    df << '\n';
    df.close();
  }
  //  writeZero( outtrace );
  return Completed;
}


addRePro( Simple, patchclamp );

}; /* namespace patchclamp */

#include "moc_simple.cc"

