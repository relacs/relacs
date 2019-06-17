/*
  calibration/inputrangecheck.cc
  Checks calibration of each range (gain) of an analog input channel.

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

#include <relacs/rangeloop.h>
#include <relacs/tablekey.h>
#include <relacs/calibration/inputrangecheck.h>
using namespace relacs;

namespace calibration {


InputRangeCheck::InputRangeCheck( void )
  : RePro( "InputRangeCheck", "calibration", "Jan Benda", "1.2", "Jun 11, 2015" )
{
  // add some options:
  addSelection( "outtrace", "Output trace", "V-1" );
  addSelection( "intrace", "Input trace", "V-1" );
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Pause between stimuli", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addInteger( "amplnum", "Number of amplitudes to be tested", 10, 0, 100000 );

  setWidget( &P );
}


void InputRangeCheck::preConfig( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
}


void InputRangeCheck::notify( void )
{
  int outtrace = index( "outtrace" );
  if ( outtrace >= 0 && outtrace < outTracesSize() ) {
    OutName = outTrace( outtrace ).traceName();
    OutUnit = outTrace( outtrace ).unit();
  }

  int intrace = index( "intrace" );
  if ( intrace >= 0 && intrace < traces().size() ) {
    InName = trace( intrace ).ident();
    InUnit = trace( intrace ).unit();
  }
}


int InputRangeCheck::main( void )
{
  // get options:
  int outtrace = index( "outtrace" );
  int intrace = index( "intrace" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int amplnum = integer( "amplnum" );

  // don't print repro message:
  noMessage();

  // input gain setting:
  int orggain = trace( intrace ).gainIndex();

  // plot trace:
  tracePlotSignal( 1.3*duration, 0.2*duration );

  // init plot:
  P.lock();
  P.setXLabel( "Analog output " + OutName + " [" + OutUnit + "]" );
  P.setYLabel( "Analog input " + InName + " [" + InUnit + "]" );
  P.unlock();

  // ranges:
  vector<double> ranges;
  maxValues( trace( intrace ), ranges );
  for ( unsigned int r=0; r < ranges.size() && softStop() < 1; ++r ) {

    // init plot:
    P.lock();
    P.setTitle( "Input range " + Str( r ) + ": " +
		Str( -ranges[r] ) + " --- " + Str( ranges[r] ) + " " + InUnit );
    P.setXRange( -1.05*ranges[r], 1.05*ranges[r] );
    P.setYRange( -1.05*ranges[r], 1.05*ranges[r] );
    P.unlock();

    setGain( trace( intrace ), r );
    if ( activateGains() < 0 )
      return Failed;
    sleep( pause );

    RangeLoop amplrange( -ranges[r], ranges[r], amplnum, 1, 1, 1 );
    MapD ampls;
    ampls.reserve( amplrange.size() );
  
    for ( amplrange.reset(); ! amplrange && softStop() < 2; ++amplrange ) {

      double amplitude = *amplrange;
      if ( ::fabs( amplitude ) < 1.0e-8 )
	amplitude = 0.0;
      
      message( "Input range <b>" + Str( r ) + "</b>: Test amplitude <b>" + 
	       Str( amplitude ) + " " + InUnit + "</b>" );

      //  ouptut signal:
      OutData signal;
      signal.setTrace( outtrace );
      signal.pulseWave( duration, -1.0, amplitude, 0.0 );
      write( signal );
      if ( signal.failed() )
	return Failed;
      if ( interrupt() )
	return Aborted;

      // analyze:
      double d = signal.length();
      double val = trace( intrace ).mean( signalTime() + 0.3*d, signalTime()+0.95*d );
      ampls.push( amplitude, val );

      P.lock();
      P.clear();
      P.plotVLine( -ranges[r], Plot::White, 3 );
      P.plotVLine( ranges[r], Plot::White, 3 );
      P.plotVLine( 0.0, Plot::White, 3 );
      P.plotHLine( 0.0, Plot::White, 3 );
      P.plotLine( -ranges[r], -ranges[r], ranges[r], ranges[r], Plot::Yellow, 2 );
      P.plot( ampls, 1.0, Plot::Red, 2, Plot::Solid,
	      Plot::Circle, 10, Plot::Red, Plot::Red );
      P.draw();
      P.unlock();
      
      sleep( pause );
    }

    // save data:
    ofstream df( addPath( "inputrangecheck-data.dat" ).c_str(),
		 ofstream::out | ofstream::app );
    if ( r == 0 ) {
      df << '\n';
      settings().save( df, "# ", 0, Options::FirstOnly );
      df << '\n';
    }
    df << "# gainindex: " << Str( r ) << '\n';
    df << "# range: " << Str( ranges[r] ) << InUnit << '\n';
    df << '\n';
    TableKey datakey;
    datakey.addNumber( InName, InUnit, "%8.3f" );
    datakey.addNumber( OutName, OutUnit, "%8.3f" );
    datakey.addNumber( "diff", OutUnit, "%8.4f" );
    datakey.saveKey( df );
    double infac = Parameter::changeUnit( 1.0, InUnit, OutUnit );
    for ( int k=0; k<ampls.size(); k++ ) {
      datakey.save( df, ampls.x( k ), 0 );
      datakey.save( df, ampls.y( k ) );
      datakey.save( df, ampls.y( k ) - infac * ampls.x( k ) );
      df << '\n';
    }
    df << "\n";

  }

  setGain( trace( intrace ), orggain );
  if ( activateGains() < 0 )
    return Failed;
  return Completed;
}


addRePro( InputRangeCheck, calibration );

}; /* namespace calibration */

#include "moc_inputrangecheck.cc"
