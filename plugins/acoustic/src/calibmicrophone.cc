/*
  acoustic/calibmicrophone.h
  Determine calibration factor for a microphone using a calibration sound source.

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
#include <iomanip>
#include <relacs/str.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/acoustic/calibmicrophone.h>
using namespace relacs;

namespace acoustic {


CalibMicrophone::CalibMicrophone( void )
  : RePro( "CalibMicrophone", "CalibMicrophone", "Acoustic",
	   "Jan Benda", "1.0", "Aug 11 2008" ),
    Traces(),
    MW( (QWidget*)this )
{
  // add some parameter as options:
  addNumber( "intensity", "Intensity of the sound source", 114.0, 0.0, 200.0, 1.0, "dB SPL" );
  addNumber( "frequency", "Frequency of the sound source", 1000.0, 0.0, 10000.0, 1.0, "Hz" );
  addSelection( "intrace", "Input trace", "Sound-1" );
  addNumber( "duration", "Duration of a single measurement", 1.0, 0.0, 1000.0, 0.5, "seconds" );
  addInteger( "repeat", "Number of measurements", 10, 1, 10000, 1 );
  Options::setFlags( 1 );

  addNumber( "amplitude", "rms amplitude", 0.0, 0.0, 10000.0, 0.05, "V" ).setFlags( 2 );
  addNumber( "amplitudestdev", "rms amplitude stdev", 0.0, 0.0, 10000.0, 0.05, "V" ).setFlags( 2 );
  addNumber( "scale", "Scale factor", 0.0, 0.0, 10000.0, 0.05, "V/Pa" ).setFlags( 2 );

  MW.assign( ((Options*)this), 0, 1+2, true, 0, mutex() );
  MW.setSpacing( 4 );
  MW.setMargin( 4 );

  setDialogSelectMask( 1 );
}


CalibMicrophone::~CalibMicrophone( void )
{
}


void CalibMicrophone::config( void )
{
  string its = "";
  for ( int k=0; k<MaxSoundTraces; k++ ) {
    if ( k<LeftSoundTraces )
      its += "|" + trace( LeftSoundTrace[k] ).ident();
    if ( k<RightSoundTraces )
      its += "|" + trace( RightSoundTrace[k] ).ident();
  }
  for ( int k=0; k<SoundTraces; k++ )
    its += "|" + trace( SoundTrace[k] ).ident();
  its.erase( 0, 1 );
  setText( "intrace", its );
  setToDefault( "intrace" );
}


int CalibMicrophone::main( void )
{
  // get options:
  double intensity = number( "intensity" );
  double frequency = number( "frequency" );
  int intrace = traceIndex( text( "intrace", 0 ) );
  double duration = number( "duration" );
  int repeats = integer( "repeat" );

  // input trace:
  if ( intrace < 0 || intrace >= traces().size() ) {
    warning( "Input trace " + Str( intrace ) + 
	     " does not exist!" );
    return Failed;
  }
  const InData &indata = trace( intrace );

  // plot trace:
  plotToggle( true, false, duration, 0.0 );

  double refpressure = 2.0e-5 * ::pow( 10.0, intensity/20.0 );

  ArrayD amplitudes;
  amplitudes.reserve( repeats );
  double amplitude = 0.0;
  double amplitudestdev = 0.0;
  double scale = 0.0;

  // autoscale gain:
  bool failed = true;
  for ( int k=0; k<20; k++ ) {
    sleep( duration );
    double maxampl = indata.maxAbs( indata.currentTime()-duration, indata.currentTime() );
    if ( maxampl < indata.maxValue() ) {
      adjustGain( indata, maxampl );
      failed = false;
      break;
    }
    adjust( indata, indata.currentTime()-duration, duration, 0.9 );
    activateGains();
  }
  if ( failed ) {
    warning( "unable to set the right gain!" );
    return Failed;
  }
  sleep( duration );

  // measure:
  for ( int k=0; k<repeats; k++ ) {
    sleep( duration );
    analyze( indata, frequency, duration, amplitudes );
    amplitude = amplitudes.mean( amplitudestdev );
    scale = amplitude / refpressure;

    Str s = "Measured rms=<b>" + Str( amplitude, 0, 4, 'g' ) + "+/-" + Str( amplitudestdev, 0, 4, 'g' ) + "V</b>"
      + ": scale=<b>" + Str( scale, 0, 5, 'g' ) + "V/Pa</b>";
    message( s );

    setNumber( "amplitude", amplitude );
    setNumber( "amplitudestdev", amplitudestdev );
    setNumber( "scale", scale );
    MW.updateValues( OptWidget::changedFlag() );
  }

  // XXX write result to calibspeaker repro!!!

  return Completed;
}


void CalibMicrophone::stop( void )
{
}


void CalibMicrophone::save( void )
{
}


void CalibMicrophone::analyze( const InData &intrace,
			       double frequency, double duration,
			       ArrayD &amplitudes )
{
  // signal amplitude:
  int nperiods = (int)::floor( duration*frequency ) - 1;
  if ( nperiods < 1 )
    nperiods = 1;
  double period = 1.0/frequency;
  double upto = intrace.currentTime() - period;
  double from = upto - nperiods*period;
  double p = intrace.stdev( from, upto );
  amplitudes.push( p );
}


addRePro( CalibMicrophone );

}; /* namespace acoustic */

#include "moc_calibmicrophone.cc"
