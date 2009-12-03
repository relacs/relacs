/*
  acoustic/calibspeakers.cc
  Calibrating attenuation for loudspeakers.

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
#include <relacs/acoustic/calibspeakers.h>
using namespace relacs;

namespace acoustic {


CalibSpeakers::CalibSpeakers( void )
  : RePro( "CalibSpeakers", "CalibSpeakers", "Acoustic",
	   "Jan Benda", "1.1", "Aug 12, 2008" ),
    Traces(),
    P( 2, 2, true, this )    
{
  // add some parameter as options:
  addText( "frequencyrange", "Frequency range (Hz)", "2000..40000..1000" );
  addNumber( "intmin", "Minimum sound intensity", 60.0, 0.0, 200.0, 5.0, "dB SPL" );
  addNumber( "intrange", "Sound intensity range", 40.0, 0.0, 200.0, 5.0, "dB SPL" );
  addNumber( "intstep", "Sound intensity step", 5.0, 0.0, 200.0, 1.0, "dB SPL" );
  addSelection( "outtrace", "Output trace", "Speaker-1" );
  addSelection( "intrace", "Input trace", "Sound-1" );
  addBoolean( "reset", "Reset calibration for each frequency?", false );
  addBoolean( "clear", "Clear calibration table?", true );
  addNumber( "duration", "Duration of stimulus", 0.4, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "skip", "Skip initial stimulus", 0.01, 0.0, 10.0, 0.001, "seconds", "ms" );
  addNumber( "pause", "Pause", 0.0, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "scale", "Scale for V/Pa", 1.0, 0.0, 10000.0, 0.05 );

  // plot:
  P.lock();
  P[0].setXLabel( "Requested intensity [dB SPL]" );
  P[0].setYLabel( "Measured intensity [dB SPL]" );
  P[1].setXLabel( "Frequency [kHz]" );
  P[1].setYLabel( "Offset [dB SPL]" );
  P.unlock();
}


CalibSpeakers::~CalibSpeakers( void )
{
}


void CalibSpeakers::config( void )
{
  setText( "outtrace", loudspeakerTraceNames() );
  setToDefault( "outtrace" );
  setText( "intrace", soundTraceNames() );
  setToDefault( "intrace" );
}


int CalibSpeakers::main( void )
{
  // get options:
  Settings = *this;
  Settings.setTypeFlags( 16, -Parameter::Blank );
  RangeLoop frequencyrange( text( "frequencyrange" ) );
  double minintensity = number( "intmin" );
  double intensityrange = number( "intrange" );
  double intensitystep = number( "intstep" );
  int outtrace = outTraceIndex( text( "outtrace", 0 ) );
  int intrace = traceIndex( text( "intrace", 0 ) );
  bool reset = boolean( "reset" );
  bool clear = boolean( "clear" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  double skip = number( "skip" );
  double soundpressurescale = number( "scale" );

  // input trace:
  if ( intrace < 0 || intrace >= traces().size() ) {
    warning( "Input trace " + Str( intrace ) + 
	     " does not exist!" );
    return Failed;
  }

  // attenuators:
  TraceSpec ot( outTrace( outtrace ) );
  LoudSpeaker *LAtt = dynamic_cast<LoudSpeaker*>( attenuator( ot.device(), ot.channel() ) );
  if ( LAtt == 0 ) {
    warning( "No Attenuator on " + ot.traceName() + 
	     " (channel " + Str( ot.channel() ) + 
	     " of device " + Str( ot.device() ) + 
	     ") found!" );
    return Failed;
  }
  LAtt->setSamplingRate( trace( intrace ).sampleRate() );
  if ( clear )
    LAtt->clear();

  // plot trace:
  plotToggle( true, true, duration, 0.0 );

  // plot:
  P.lock();
  P[0].clear();
  P[0].setXRange( minintensity, minintensity + intensityrange );
  P[1].clear();
  P[1].setXRange( 0.001*frequencyrange.minValue(), 0.001*frequencyrange.maxValue() );
  P[1].setAutoScaleY();
  P.unlock();

  MapD oldoffsets;  // need reserve!
  for ( frequencyrange.reset(); ! frequencyrange; ++frequencyrange ) {
    double freq = *frequencyrange;
    double g, o;
    LAtt->gain( g, o, freq );
    oldoffsets.push( freq, o );
  }

  frequencyrange.reset();
  double frequency = *frequencyrange;
  double intensity = minintensity;
  MapD intensities;  // need reserve!
  MapD offsets;  // need reserve!
  ArrayD gains;  // need reserve!

  double origgain = 1.0;
  double origoffset = 0.0;
  double freq = frequency;
  LAtt->gain( origgain, origoffset, freq );
  double fitgain = 1.0;
  double fitoffset = 0.0;
  if ( reset )
    LAtt->reset( frequency );
  else
    LAtt->setGain( origgain, origoffset, frequency );

  // stimulus:
  OutData signal;
  signal.setTrace( outtrace );
  applyOutTrace( signal );
  signal.sineWave( frequency, duration, 1.0, 0.001 );
  signal.ramp( 0.001 );
  signal.setDelay( 0.0 );
  signal.setError( OutData::Unknown );

  // try stimulus:
  for ( int k=0; ! signal.success(); k++ ) {

    if ( k > 40 ) {
      warning( "Could not establish valid intensity for carrier frequency " +
	       Str( frequency ) + " Hz !<br>" );
      LAtt->setGain( origgain, origoffset, frequency );
      return Failed;
    }

    signal.setIntensity( intensity );
    testWrite( signal );

    if ( signal.underflow() ) {
      printlog( "start() -> attenuator underflow: " + Str( signal.intensity() ) );
      minintensity += intensitystep;
      intensity = minintensity;
    }
    else if ( signal.overflow() ) {
      printlog( "start() -> attenuator overflow: " + Str( signal.intensity() ) );
      minintensity -= intensitystep;
      intensity = minintensity;
    }

  }

  // output stimulus:
  write( signal );
  sleep( duration + pause );

  int nosignaltries = 0;

  while ( ! interrupt() ) {

    // adjust analog input gains:
    double max = trace( intrace ).maxAbs( trace( intrace ).signalTime(),
					  trace( intrace ).signalTime()+duration );
    for ( int gaintries = 0; gaintries < MaxGainTries; gaintries++ ) {
      // check signal amplitude:
      if ( max < 0.95 * trace( intrace ).maxValue() &&
	   max > 0.3 * trace( intrace ).maxValue() )
	break;
      adjustGain( trace( intrace ), 1.5 * max );
      //      activateGains();
      // output signal again:
      write( signal );
      sleep( duration + pause );
      if ( interrupt() ) {
	saveOffsets( offsets, gains );
	LAtt->save();
	writeZero( outtrace );
	return Aborted;
      }
      max = trace( intrace ).maxAbs( trace( intrace ).signalTime(), 
				     trace( intrace ).signalTime()+duration );
    }

    // signal amplitude has proper range?
    int error = 0;
    if ( max > 0.95 * trace( intrace ).maxValue() ) {
      error = 1;
      printlog( "write() -> gain error: " + Str( error ) );
    }
    else if ( max < 0.3 * trace( intrace ).maxValue() ) {
      error = 2;
      printlog( "write() -> gain error: " + Str( error ) );
      nosignaltries++;
      if ( nosignaltries > MaxNoSignalTries ) {
	warning( "<b>No signal!</b><br>Stop now." );
	return Failed;
      }
    }

    if ( error == 0 ) {
      nosignaltries = 0;
      analyze( intrace, duration, skip, frequency, soundpressurescale,
	       intensity, intensities, fitgain, fitoffset );
      Str s = "Frequency <b>" + Str( frequency ) + " Hz</b>";
      s += ":  Tried <b>" + Str( signal.intensity(), 0, 3, 'g' ) + "dB SPL</b>";
      s += ",  Measured <b>" + Str( intensities.y().back(), 0, 3, 'g' ) + "dB SPL</b>";
      message( s );
      plot( minintensity, intensityrange, intensities, fitgain, fitoffset,
	    oldoffsets, offsets );
    }

    // next stimulus:
    signal.clearError();
    do {

      if ( signal.underflow() ||
	   error == 2 ) {
	if ( signal.underflow() )
	  printlog( "read() -> attenuator underflow: " + Str( signal.intensity() ) );
	minintensity += intensitystep;
      }

      intensity += intensitystep;
      if ( intensity > minintensity + intensityrange ||
	   signal.overflow() ||
	   error == 1 ) {
	if ( signal.overflow() )
	  printlog( "read() -> attenuator overflow: " + Str( signal.intensity() ) );
	saveIntensities( frequency, intensities, fitgain, fitoffset );
	// set new gain and offset:
	if ( intensities.size() > 2 ) {
	  double gain, offset;
	  double freq = frequency;
	  LAtt->gain( gain, offset, freq );
	  offset = offset - fitoffset * gain / fitgain;
	  gain = gain / fitgain;
	  LAtt->setGain( gain, offset, frequency );
	  // isn't this too much of a push? (intensities.size() > 2 )
	  offsets.push( frequency, offset );
	  gains.push( gain );
	  Str s = "new gain = " + Str( gain );
	  s += ",  new offset = " + Str( offset );
	  message( s );
	  P.lock();
	  P[1].clear();
	  P[1].plot( oldoffsets, 0.001, Plot::Orange, 2, Plot::Solid );
	  P[1].plot( offsets, 0.001, Plot::Red, 4, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
	  P.unlock();
	  P.draw();
	}

	// next frequency:
	minintensity = number( "intmin" );
	intensity = minintensity;
	intensities.clear();
	++frequencyrange;
	if ( ! frequencyrange ) {
	  frequency = *frequencyrange;
	  P.lock();
	  P[0].clear();
	  P.unlock();

	  double freq = frequency;
	  LAtt->gain( origgain, origoffset, freq );
	  fitgain = 1.0;
	  fitoffset = 0.0;
	  if ( reset )
	    LAtt->reset( frequency );
	  else
	    LAtt->setGain( origgain, origoffset, frequency );

	  // stimulus:
	  signal.free();
	  signal.setTrace( outtrace );
	  applyOutTrace( signal );
	  signal.sineWave( frequency, duration, 1.0, 0.001 );
	  signal.ramp( 0.001 );
	  signal.setDelay( 0.0 );
	}
	else {
	  saveOffsets( offsets, gains );
	  LAtt->save();
	  writeZero( outtrace );
	  return Completed;
	}
      }

      signal.setIntensity( intensity );
      testWrite( signal );

    } while ( ! signal.success() );

    write( signal );
    sleep( duration + pause );

  }

  saveOffsets( offsets, gains );
  LAtt->save();
  writeZero( outtrace );
  return Aborted;
}


void CalibSpeakers::saveIntensities( double frequency, const MapD &intensities,
				     double fitgain, double fitoffset )
{
  // create file:
  ofstream df( addPath( "calibintensities.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  time_t dt = ::time( 0 );
  Str date = "%d.%m.%02Y";
  date.format( ::localtime( &dt ) );
  Str time = "%h:%02M";
  time.format( ::localtime( &dt ) );
  Options header;
  header.addNumber( "frequency", frequency, "Hz" );
  header.addText( "date", date );
  header.addText( "time", time );
  header.addLabel( "settings:" );
  header.save( df, "# " );
  Settings.save( df, "#   ", -1, 16, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "intens", "dB SPL", "%6.2f" );
  key.addNumber( "measured", "dB SPL", "%6.2f" );
  key.addNumber( "fit", "dB SPL", "%6.2f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<intensities.size(); k++ ) {
    key.save( df, intensities.x(k), 0 );
    key.save( df, intensities.y(k), 1 );
    key.save( df, fitgain*intensities.x(k)+fitoffset, 1 );
    df << '\n';
  }
  df << '\n' << '\n';
}


void CalibSpeakers::saveOffsets( const MapD &offsets, const ArrayD &gains )
{
  if ( offsets.empty() )
    return;

  // create file:
  ofstream df( addPath( "caliboffsets.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  time_t dt = ::time( 0 );
  Str date = "%d.%m.%02Y";
  date.format( ::localtime( &dt ) );
  Str time = "%h:%02M";
  time.format( ::localtime( &dt ) );
  Options header;
  header.addText( "date", date );
  header.addText( "time", time );
  header.addLabel( "settings:" );
  header.save( df, "# " );
  Settings.save( df, "#   ", -1, 16, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "freq", "Hz", "%7.0f" );
  key.addNumber( "offset", "dB SPL", "%6.2f" );
  key.addNumber( "gain", "1", "%6.3f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<offsets.size(); k++ ) {
    key.save( df, offsets.x(k), 0 );
    key.save( df, offsets.y(k), 1 );
    key.save( df, gains[k], 2 );
    df << '\n';
  }
  df << '\n' << '\n';
}


void CalibSpeakers::plot( double minintensity, double intensityrange,
			  const MapD &intensities,
			  double fitgain, double fitoffset,
			  const MapD &oldoffsets, const MapD &offsets )
{
  double min = minintensity;
  double max = minintensity + intensityrange;
  P.lock();
  P[0].clear();
  //  P[0].setYRange( 0.0, max*fitgain+fitoffset );
  P[0].plotLine( min, min, max, max, Plot::Blue, 4 );
  P[0].plotLine( min, min*fitgain+fitoffset, max, max*fitgain+fitoffset, Plot::Yellow, 2 );
  P[0].plot( intensities, 1.0, Plot::Transparent, 1, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );

  P[1].clear();
  P[1].plot( oldoffsets, 0.001, Plot::Orange, 2, Plot::Solid );
  P[1].plot( offsets, 0.001, Plot::Red, 4, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );

  P.unlock();

  P.draw();
}


void CalibSpeakers::analyze( int intrace, double duration, double skip,
			     double frequency, double soundpressurescale,
			     double intensity, MapD &intensities,
			     double &fitgain, double &fitoffset )
{
  // signal amplitude:
  int si = trace( intrace ).signalIndex() + trace( intrace ).indices( skip );
  double periods = floor( frequency * 0.001 );
  if ( periods < 1.0 )
    periods = 1.0;
  int wi = trace( intrace ).indices( periods/frequency );
  int fi = trace( intrace ).signalIndex() + trace( intrace ).indices( duration - 0.004 ) - wi;
  double p = 0.0;
  for ( int n=1; si < fi; n++ ) {
    double sd = trace( intrace ).stdev( si, si+wi );
    p += ( sd - p )/n;
    si += wi;
  }
  //  p = trace( intrace ).stdev( si, fi );

  // PEAK amplitude:
  //  double amplitude = 20.0 * log10( p * sqrt( 2.0 ) / 2.0e-5 / soundpressurescale );
  // RMS amplitude:
  double amplitude = 20.0 * log10( p / 2.0e-5 / soundpressurescale );

  // store data:
  intensities.push( intensity, amplitude );

  if ( intensities.size() > 1 ) {
    // fit straight line:
    intensities.lineFit( fitoffset, fitgain );
  }
}


addRePro( CalibSpeakers );

}; /* namespace acoustic */

#include "moc_calibspeakers.cc"
