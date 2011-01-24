/*
  auditory/adaptedficurves.cc
  First adapts the neuron to a background sound intensity and then measures f-I curves.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/auditory/adaptedficurves.h>
using namespace relacs;

namespace auditory {


AdaptedFICurves::AdaptedFICurves( void )
  : RePro( "AdaptedFICurves", "auditory", "Jan Benda", "1.0", "Jan 24, 2011" )
{
  // add some options:
  addLabel ( "Adaptation stimuli" );
  addNumber( "adaptint", "Sound intensity of adapting stimulus", 50.0, 0.0, 200.0, 0.5, "dB SPL" );
  addNumber( "adaptinit", "Duration of initial adaptation stimulus", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "adaptduration", "Duration of subsequent adaptation stimuli", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addLabel ( "Test stimuli" );
  addNumber( "intmin", "Minimum sound intensity of test stimulus", 50.0, 0.0, 200.0, 0.5, "dB SPL" );
  addNumber( "intmax", "Maximum sound intensity of test stimulus", 100.0, 0.0, 200.0, 0.5, "dB SPL" );
  addNumber( "intstep", "Sound-intensity steps of test stimulus", 50.0, 0.0, 200.0, 0.5, "dB SPL" );
  addNumber( "duration", "Duration of test stimuli", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addLabel ( "General" );
  addSelection( "side", "Speaker", "left|right|best" );
  addNumber( "carrierfreq", "Frequency of carrier", 5000.0, -40000.0, 40000.0, 500.0, "Hz", "kHz" );
  addBoolean( "usebestfreq", "Relative to the cell's best frequency", true );
  addNumber( "ramp", "Duration of ramps for all intenisty transitions", 0.001, 0.001, 1000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Pause between stimuli", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addInteger( "repetitions", "Number of repetitions of the stimulus", 10, 0, 10000, 1 );
}


int AdaptedFICurves::main( void )
{
  // get options:
  double adaptint = number( "adaptint" );
  double adaptinit = number( "adaptinit" );
  double adaptduration = number( "adaptduration" );
  double intmin = number( "intmin" );
  double intmax = number( "intmax" );
  double intstep = number( "intstep" );
  double duration = number( "duration" );
  bool side = index( "side" );
  double carrierfrequency = number( "carrierfreq" );
  bool usebestfreq = boolean( "usebestfreq" );
  double pause = number( "pause" );
  double ramp = number( "ramp" );
  double repetitions = integer( "repetitions" );

  if ( side > 1 )
    side = metaData( "Cell" ).index( "best side" );
  string sidestr = side > 0 ? "right" :  "left";
  if ( usebestfreq ) {
    double cf = metaData( "Cell" ).number( sidestr + " frequency" );
    if ( cf > 0.0 )
      carrierfrequency += cf;
  }

  // test intensities:
  RangeLoop intrange( intmin, intmax, intstep );
  intrange.alternateInUp();

  // amplitude modulation:
  SampleDataD am( 0.0, 10.0, 0.0005 );
  am.clear();
  int rn = am.indices( ramp );
  double xb = ::pow( 10.0, 0.05*(adaptint - intmax) );
  double x = 0.0;
  double x0 = 0.0;
  if ( rn > 0 )
    am.push( x );
  for ( intrange.reset(); ! intrange; ++intrange ) {
    // adapt:
    x0 = x;
    x = xb;
    for ( int j=1; j<=rn; j++ )
      am.push( (x-x0)*j/rn+x0 );
    for ( int j=0; j<am.indices( intrange.loop()==0 ? adaptinit : adaptduration ) - rn; j++ )
      am.push( x );
    // test:
    x0 = x;
    x = ::pow( 10.0, 0.05*(*intrange - intmax) );
    for ( int j=1; j<=rn; j++ )
      am.push( (x-x0)*j/rn+x0 );
    for ( int j=0; j<am.indices( duration ) - rn; j++ )
      am.push( x );
  }
  // final ramp:
  x0 = x;
  x = 0.0;
  for ( int j=1; j<=rn; j++ )
    am.push( (x-x0)*j/rn+x0 );

  // stimulus:
  OutData signal;
  signal.setTrace( Speaker[ side ] );
  signal.fill( am, carrierfrequency, "pulses" );

  int state = Completed;

  timeStamp();

  for ( int count=0;
        ( repetitions <= 0 || count < repetitions ) && softStop() == 0;
        count++ ) {

    // message:
    Str s = "Background intensity: <b>" + Str( adaptint, 0, 1, 'f' ) + " dB SPL</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    if ( repetitions > 0 )
      s += " of <b>" + Str( repetitions ) + "</b>";
    message( s );

    // output:
    write( signal );
    if ( signal.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
               signal.errorText() + "</b>," +
               "<br> Loop: <b>" + Str( count+1 ) + "</b>" +
               "<br>Exit now!" );
      writeZero( Speaker[ side ] );
      return Failed;
    }

    sleep( signal.length() + ( pause > 0.01 ? 0.01 : pause ) );

    if ( interrupt() ) {
      if ( count == 0 )
        state = Aborted;
      break;
    }

    //    analyze( spikes, rate );
    //    plot( spikes, rate, signal, voltage, plotmode );
 
    sleepOn( signal.length() + pause );
    if ( interrupt() ) {
      if ( count == 0 )
        state = Aborted;
      break;
    }
    timeStamp();
   
  }

  if ( state == Completed ) {
    unlockAll();
    //    saveRate( header, rate );
    //    saveSpikes( header, spikes );
    lockAll();
  }

  writeZero( Speaker[ side ] );
  return state;
}


addRePro( AdaptedFICurves );

}; /* namespace auditory */

#include "moc_adaptedficurves.cc"
