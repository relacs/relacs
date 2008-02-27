/*
  simple.h
  Simple RePro for testing dynamic clamp

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include "simple.h"


Simple::Simple( void )
  : RePro( "Simple", "Simple", "Jan Benda", "0.0", "Feb 14, 2008" ),
    P( this, "stimulusdelayplot" )
{
  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addSelection( "outtrace", "Output trace", "Speaker-1" );
  addNumber( "amplitude", "Amplitude of output signal", 1.0, 0.0, 1000.0, 1.0 );
  addNumber( "duration", "Duration of output", 0.1, 0.001, 1.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "" );
  P.unlock();
}


void Simple::config( void )
{
  string its = "";
  const InList &il = traces();
  for ( int k=0; k<il.size(); k++ ) {
    if ( k > 0 )
      its += '|';
    its += il[k].ident();
  }
  setText( "intrace", its );
  setToDefault( "intrace" );

  string ots = "";
  for ( int k=0; k<outTracesSize(); k++ ) {
    if ( k > 0 )
      ots += '|';
    ots += outTraceName( k );
  }
  setText( "outtrace", ots );
  setToDefault( "outtrace" );
}


void Simple::notify( void )
{
  int outtrace = index( "outtrace" );
  setUnit( "amplitude", outTrace( outtrace ).unit() );
}


int Simple::main( void )
{
  // get options:
  int intrace = traceIndex( text( "intrace", 0 ) );
  int outtrace = index( "outtrace" );
  string unit = outTrace( outtrace ).unit();
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );

  // don't print repro message:
  noMessage();

  // plot trace:
  plotToggle( true, true, 2.0*duration, 0.5*duration );

  // plot:
  P.setXRange( -1000.0*duration, 1000.0*duration );

  OutData signal( duration, 1.0/trace( intrace ).sampleRate() );
  signal = amplitude;
  signal.back() = 0;
  signal.setTrace( outtrace );
  signal.setIdent( "one" );

  sleep( pause );

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
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;

    //    analyze( trace( intrace ), duration, count, deltat );
    sleep( pause );
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;

  }

  return Completed;
}


addRePro( Simple );
