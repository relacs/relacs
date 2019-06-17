/*
  calibration/attenuatorcheck.cc
  Checks performance of attenuator device

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
#include <relacs/rangeloop.h>
#include <relacs/tablekey.h>
#include <relacs/calibration/attenuatorcheck.h>
using namespace relacs;

namespace calibration {


AttenuatorCheck::AttenuatorCheck( void )
  : RePro( "AttenuatorCheck", "calibration", "Jan Benda", "1.2", "Feb 15, 2014" )
{
  InFac = 1.0;

  // add some options:
  addSelection( "outtrace", "Output trace", "V-1" );
  addSelection( "intrace", "Input trace", "V-1" );
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "s" );
  addSelection( "type", "Measurement type", "attenuation|noise" );
  addNumber( "frequency", "Frequency of stimulus", 50.0, 10.0, 100000.0, 10.0, "Hz", "Hz" ).setActivation( "type", "attenuation" );
  addNumber( "amplitude", "Amplitude of stimulus", 1.0, -1.0, 1.0, 0.1, "Vmax", "Vmax" ).setActivation( "type", "noise" );
  addNumber( "minlevel", "Minimum attenuation level", 0.0, -1000.0, 1000.0, 0.1, "dB", "dB" );
  addNumber( "maxlevel", "Maximum attenuation level", 100.0, -1000.0, 1000.0, 0.1, "dB", "dB" );
  addNumber( "dlevel", "Increment of attenuation level", 1.0, 0.0, 1000.0, 0.1, "dB", "dB" );
  addBoolean( "adjust", "Adjust input gain?", true );

  setWidget( &P );
}


void AttenuatorCheck::preConfig( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
}


void AttenuatorCheck::notify( void )
{
  int outtrace = index( "outtrace" );
  if ( outtrace >= 0 && outtrace < outTracesSize() ) {
    OutName = outTrace( outtrace ).traceName();
  }

  int intrace = index( "intrace" );
  if ( intrace >= 0 && intrace < traces().size() ) {
    InName = trace( intrace ).ident();
    InUnit = trace( intrace ).unit();
    InFac = Parameter::changeUnit( 1.0, InUnit, "V" );
  }
}


int AttenuatorCheck::main( void )
{
  // get options:
  int outtrace = index( "outtrace" );
  int intrace = index( "intrace" );
  double duration = number( "duration" );
  int type = index( "type" );
  double frequency = number( "frequency" );
  double amplitude = number( "amplitude" );
  double minlevel = number( "minlevel" );
  double maxlevel = number( "maxlevel" );
  double dlevel = number( "dlevel" );
  bool adjust = boolean( "adjust" );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( duration, 0.0 );

  P.lock();
  P.setXRange( minlevel, Plot::AutoScale );
  P.setXLabel( "Attenuation level [dB]" );
  if ( type == 1 ) {
    P.setYLabel( "Standard deviation [" + InUnit + "]" );
    P.setYRange( 0.0, Plot::AutoScale );
  }
  else {
    P.setYLabel( "Gain [dB]" );
    P.setYRange( Plot::AutoScale, Plot::AutoScale );
  }
  P.unlock();

  //  ouptut signal:
  OutData signal;
  signal.setTrace( outtrace );
  if ( type == 1 )
    signal.pulseWave( duration, -1.0, amplitude, 0.0 );
  else
    signal.sineWave( duration, -1.0, frequency, 0.0, 1.0 );

  // input gain setting:
  int orggain = trace( intrace ).gainIndex();
  if ( adjust )
    setGain( trace( intrace ), 0 );

  RangeLoop levelrange( minlevel, maxlevel, dlevel, 1, 1, 1 );

  MapD levels;
  levels.reserve( levelrange.size() );

  for ( levelrange.reset(); ! levelrange && softStop() == 0; ++levelrange ) {

    double level = *levelrange;

    message( "Test attenuation level <b>" + Str( level, "%.1f" ) + " dB</b>" );

    signal.setNoIntensity();
    signal.setLevel( level );
    write( signal );
    if ( signal.failed() ) {
      if ( signal.error() & OutData::AttOverflow )
	continue;
      else if ( signal.error() & OutData::AttUnderflow )
	break;
      else {
	if ( adjust )
	  setGain( trace( intrace ), orggain );
	return Failed;
      }
    }
    if ( interrupt() )
      return Aborted;

    // analyze:
    double val = 0.0;
    double d = signal.length();
    double stdev = trace( intrace ).stdev( signalTime() + 0.05*d, signalTime()+0.95*d );
    if ( type == 1 )
      val = stdev;
    else
      val = 20.0*::log10( InFac * stdev * ::sqrt( 2.0 ) );
    levels.push( level, val );
    if ( adjust )
      adjustGain( trace( intrace ),
		  trace( intrace ).maxAbs( signalTime() + 0.05*d, signalTime()+0.95*d ) );

    P.lock();
    P.clear();
    P.plot( levels, 1.0, Plot::Red, 2, Plot::Solid,
	    Plot::Circle, 10, Plot::Red, Plot::Red );
    if ( type == 0 && levels.size() > 2 ) {
      P.plotLine( levels.x().front(), levels.y().front(),
		  levels.x().back(), levels.y().back(),
		  Plot::Yellow, 2 );
    }
    P.draw();
    P.unlock();

    sleep( 0.01 );
  }

  // save data:
  string filename = type == 1 ? "attenuatorcheck-noise.dat" : "attenuatorcheck-gain.dat";
  ofstream df( addPath( filename ).c_str(),
	       ofstream::out | ofstream::app );
  lockStimulusData();
  stimulusData().save( df, "# ", 0, Options::FirstOnly );
  unlockStimulusData();
  settings().save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey datakey;
  datakey.addNumber( "level", "dB", "%7.2f" );
  if ( type == 1 )
    datakey.addNumber( "stdev", InUnit, "%7.4f" );
  else
    datakey.addNumber( "gain", "dB", "%8.4f" );
  datakey.saveKey( df );
  for ( int k=0; k<levels.size(); k++ ) {
    datakey.save( df, levels.x( k ), 0 );
    datakey.save( df, levels.y( k ) );
    df << '\n';
  }
  df << "\n\n";

  if ( adjust )
    setGain( trace( intrace ), orggain );
  return Completed;
}


addRePro( AttenuatorCheck, calibration );

}; /* namespace calibration */

#include "moc_attenuatorcheck.cc"
