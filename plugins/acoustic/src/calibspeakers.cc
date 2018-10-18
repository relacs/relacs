/*
  acoustic/calibspeakers.cc
  Calibrating attenuation for loudspeakers.

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

#include <fstream>
#include <iomanip>
#include <relacs/str.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/acoustic/calibspeakers.h>
using namespace relacs;

namespace acoustic {


CalibSpeakers::CalibSpeakers( void )
  : RePro( "CalibSpeakers", "acoustic", "Jan Benda", "1.4", "Aug 3, 2017" ),
    Traces()
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
  addNumber( "duration", "Duration of stimulus", 0.5, 0.0, 100.0, 0.05, "seconds", "ms" );
  addNumber( "skip", "Initial time to skip for analysis", 0.01, 0.0, 100.0, 0.001, "seconds", "ms" );
  addNumber( "win", "Window for computing rms response", 0.001, 0.001, 100.0, 0.001, "seconds", "ms" );
  addBoolean( "fitslope", "Fit slope in addition to offset", false );
  addNumber( "ramp", "Ramp time of stimulus", 0.001, 0.0, 10.0, 0.001, "seconds", "ms" );
  addNumber( "pause", "Pause between stimuli", 0.0, 0.0, 100.0, 0.05, "seconds", "ms" );
  addNumber( "scale", "Scale for V/Pa", 1.0, 0.0, 10000.0, 0.05 );

  // plot:
  P.lock();
  P.resize( 2, 2, true );    
  P[0].setXLabel( "Requested intensity [dB SPL]" );
  P[0].setYLabel( "Measured intensity [dB SPL]" );
  P[1].setXLabel( "Frequency [kHz]" );
  P[1].setYLabel( "Offset [dB SPL]" );
  P.unlock();
  setWidget( &P );
}


CalibSpeakers::~CalibSpeakers( void )
{
}


void CalibSpeakers::preConfig( void )
{
  setText( "outtrace", loudspeakerTraceNames() );
  setToDefault( "outtrace" );
  setText( "intrace", soundTraceNames() );
  setToDefault( "intrace" );
}


int CalibSpeakers::main( void )
{
  // get options:
  settings().setValueTypeFlags( 16, -Parameter::Section );
  RangeLoop frequencyrange( allText( "frequencyrange" ) );
  double minintensity = number( "intmin" );
  double intensityrange = number( "intrange" );
  double intensitystep = number( "intstep" );
  string outtrace = text( "outtrace", 0 );
  int intrace = traceIndex( text( "intrace", 0 ) );
  bool reset = boolean( "reset" );
  bool clear = boolean( "clear" );
  double duration = number( "duration" );
  double ramp = number( "ramp" );
  double pause = number( "pause" );
  double skip = number( "skip" );
  double win = number( "win" );
  bool fitslope = boolean( "fitslope" );
  double soundpressurescale = number( "scale" );
  if ( frequencyrange.minValue() < 0.0 ) {
    warning( "Signal frequencies need to be positive!" );
    return Failed;
  }

  // input trace:
  if ( intrace < 0 || intrace >= traces().size() ) {
    warning( "Input trace " + Str( intrace ) + 
	     " does not exist!" );
    return Failed;
  }
  int intracesource = intrace;
  while ( trace( intracesource ).source() > 0 )
    intracesource = traceInputTrace( intracesource );

  // attenuators:
  LoudSpeaker *latt = dynamic_cast<LoudSpeaker*>( attenuator( outtrace ) );
  if ( latt == 0 ) {
    TraceSpec ot( outTrace( outtrace ) );
    warning( "No Attenuator on " + ot.traceName() + 
	     " (channel " + Str( ot.channel() ) + 
	     " of device " + Str( ot.device() ) + 
	     ") found!" );
    return Failed;
  }
  latt->setSamplingRate( trace( intrace ).sampleRate() );
  MapD oldoffsets;
  oldoffsets.reserve( frequencyrange.size() );
  for ( frequencyrange.reset(); ! frequencyrange; ++frequencyrange ) {
    double freq = *frequencyrange;
    double f = freq;
    double g, o;
    latt->gain( g, o, f );
    cerr << freq << "  " << g << "  " << o << '\n';
    oldoffsets.push( freq, o );
  }
  if ( clear )
    latt->clear();

  // plot trace:
  tracePlotSignal( 1.6*duration, 0.1*duration );

  // plot:
  P.lock();
  P[0].clear();
  P[0].setXRange( minintensity, minintensity + intensityrange );
  P[1].clear();
  P[1].setXRange( 0.001*frequencyrange.minValue(), 0.001*frequencyrange.maxValue() );
  P[1].setAutoScaleY();
  P.unlock();

  frequencyrange.reset();
  double frequency = *frequencyrange;
  double intensity = minintensity;
  MapD intensities;
  intensities.reserve( frequencyrange.size() );
  MapD offsets;
  offsets.reserve( frequencyrange.size() );
  ArrayD gains;
  gains.reserve( frequencyrange.size() );

  double origgain = 1.0;
  double origoffset = 0.0;
  double freq = frequency;
  latt->gain( origgain, origoffset, freq );
  double fitgain = 1.0;
  double fitoffset = 0.0;
  if ( reset )
    latt->reset( frequency );
  else
    latt->setGain( origgain, origoffset, frequency );

  // stimulus:
  OutData signal;
  signal.setTraceName( outtrace );
  signal.sineWave( duration, -1.0, frequency, 0.0, 1.0, ramp );
  signal.setError( OutData::Unknown );
  signal.setIntensity( intensity );
  testWrite( signal );
  if ( signal.underflow() || signal.overflow() ) {
    if ( signal.underflow() ) {
      printlog( "attenuator underflow: " + Str( signal.intensity() ) );
      minintensity += intensitystep;
      intensity = minintensity;
    }
    else {
      printlog( "attenuator overflow: " + Str( signal.intensity() ) );
      minintensity -= intensitystep;
      intensity = minintensity;
    }
    signal.setIntensity( intensity );
    testWrite( signal );
  }
  if ( signal.failed() ) {
    warning( "Failed to prepare stimulus for carrier frequency " +
	     Str( frequency ) + " Hz !<br>" );
    latt->setGain( origgain, origoffset, frequency );
    return Failed;
  }

  // output stimulus:
  write( signal );
  sleep( pause );

  int nosignaltries = 0;

  while ( ! interrupt() && softStop() == 0 ) {

    // adjust analog input gains:
    double max = 0.0;
    for ( int gaintries = 0; gaintries < MaxGainTries; gaintries++ ) {
      if ( signal.success() ) {
	// check signal amplitude:
	max = trace( intracesource ).maxAbs( signalTime(), signalTime()+duration );
	if ( max < 0.95 * trace( intracesource ).maxValue() &&
	     max > 0.1 * trace( intracesource ).maxValue() )
	  break;
	adjustGain( trace( intracesource ), 1.5 * max );
      }
      // output signal again:
      write( signal );
      sleep( pause );
      if ( interrupt() ) {
	saveOffsets( offsets, gains );
	latt->save();
	writeZero( outtrace );
	return Aborted;
      }
    }

    // signal amplitude has proper range?
    int error = 0;
    string errorstr = "";
    if ( max > 0.95 * trace( intrace ).maxValue() ) {
      error = 1;
      errorstr = "microphone signal too large";
    }
    else if ( max < 0.05 * trace( intrace ).maxValue() ) {
      error = 2;
      errorstr = "microphone signal too small";
      nosignaltries++;
      if ( nosignaltries > MaxNoSignalTries ) {
	warning( "<b>No signal!</b><br>Stop now." );
	return Failed;
      }
    }

    if ( error == 0 ) {
      nosignaltries = 0;
      analyze( intrace, duration, skip, win, fitslope, ramp, frequency, soundpressurescale,
	       signal.intensity(), intensities, fitgain, fitoffset );
      Str s = "Frequency <b>" + Str( frequency ) + " Hz</b>";
      s += ":  Tried <b>" + Str( signal.intensity(), 0, 3, 'g' ) + "dB SPL</b>";
      s += ",  Measured <b>" + Str( intensities.y().back(), 0, 3, 'g' ) + "dB SPL</b>";
      message( s );
      plot( minintensity, intensityrange, intensities, fitgain, fitoffset,
	    oldoffsets, offsets );
    }
    else {
      Str s = "Frequency <b>" + Str( frequency ) + " Hz</b>";
      s += ":  Tried <b>" + Str( signal.intensity(), 0, 3, 'g' ) + "dB SPL</b>";
      s += ", <b>" + errorstr + "</b>";
      message( s );
    }

    // next stimulus:
    signal.clearError();
    do {

      if ( signal.underflow() ||
	   error == 2 ) {
	if ( signal.underflow() )
	  printlog( "attenuator underflow: " + Str( signal.intensity() ) );
	minintensity += intensitystep;
      }

      intensity += intensitystep;
      if ( intensity > minintensity + intensityrange ||
	   signal.overflow() ||
	   error == 1 ) {
	if ( signal.overflow() )
	  printlog( "attenuator overflow: " + Str( signal.intensity() ) );
	saveIntensities( frequency, intensities, fitgain, fitoffset );
	// set new gain and offset:
	if ( intensities.size() > 2 ) {
	  double gain, offset;
	  double freq = frequency;
	  latt->gain( gain, offset, freq );
	  offset = offset - fitoffset * gain / fitgain;
	  gain = gain / fitgain;
	  latt->setGain( gain, offset, frequency );
	  offsets.push( frequency, offset );
	  gains.push( gain );
	  Str s = "new gain = " + Str( gain );
	  s += ",  new offset = " + Str( offset );
	  message( s );
	  P.lock();
	  P[1].clear();
	  P[1].plot( oldoffsets, 0.001, Plot::Orange, 2, Plot::Solid );
	  P[1].plot( offsets, 0.001, Plot::Red, 4, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
	  P.draw();
	  P.unlock();
	}

	// next frequency:
	minintensity = settings().number( "intmin" );
	intensity = minintensity;
	intensities.clear();
	++frequencyrange;
	if ( ! frequencyrange ) {
	  frequency = *frequencyrange;
	  P.lock();
	  P[0].clear();
	  P.unlock();

	  double freq = frequency;
	  latt->gain( origgain, origoffset, freq );
	  fitgain = 1.0;
	  fitoffset = 0.0;
	  if ( reset )
	    latt->reset( frequency );
	  else
	    latt->setGain( origgain, origoffset, frequency );

	  // stimulus:
	  signal.free();
	  signal.setTraceName( outtrace );
	  signal.sineWave( duration, -1.0, frequency, 0.0, 1.0, ramp );
	  signal.ramp( 0.001 );
	  signal.setDelay( 0.0 );
	}
	else {
	  saveOffsets( offsets, gains );
	  latt->save();
	  writeZero( outtrace );
	  return Completed;
	}
      }

      signal.setIntensity( intensity );
      testWrite( signal );

    } while ( ! signal.success() );

    write( signal );
    sleep( pause );

  }

  saveOffsets( offsets, gains );
  latt->save();
  writeZero( outtrace );
  return Aborted;
}


void CalibSpeakers::saveIntensities( double frequency, const MapD &intensities,
				     double fitgain, double fitoffset )
{
  // create file:
  ofstream df( addPath( "calibspeakers-intensities.dat" ).c_str(),
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
  header.newSection( settings(), 16 );
  header.save( df, "# ", 0, Options::FirstOnly );
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
  ofstream df( addPath( "calibspeakers-offsets.dat" ).c_str(),
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
  header.newSection( settings(), 16 );
  header.save( df, "# ", 0, Options::FirstOnly );
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
  P[0].setXRange( min, max );
  //  P[0].setYRange( 0.0, max*fitgain+fitoffset );
  //  P[0].plotLine( min, min, max, max, Plot::Blue, 4 );
  //  P[0].plotLine( min, min*fitgain+fitoffset, max, max*fitgain+fitoffset, Plot::Yellow, 2 );
  SampleDataD x;
  x.line( min, max, 1.0, 0.0, 1.0 );
  P[0].plot( x, 1.0, Plot::Blue, 4 );
  x.line( min, max, 1.0, fitoffset, fitgain );
  P[0].plot( x, 1.0, Plot::Yellow, 2 );
  P[0].plot( intensities, 1.0, Plot::Transparent, 1, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );

  P[1].clear();
  P[1].plot( oldoffsets, 0.001, Plot::Orange, 2, Plot::Solid );
  P[1].plot( offsets, 0.001, Plot::Red, 4, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );

  P.draw();

  P.unlock();
}


void CalibSpeakers::analyze( int intrace, double duration, double skip, double win, 
			     bool fitslope, double ramp, double frequency, 
			     double soundpressurescale,
			     double intensity, MapD &intensities,
			     double &fitgain, double &fitoffset )
{
  // signal amplitude:
  int si = trace( intrace ).index( signalTime() + skip );
  double periods = floor( win * frequency );
  if ( periods < 1.0 )
    periods = 1.0;
  int wi = trace( intrace ).indices( periods/frequency );
  int fi = trace( intrace ).index( signalTime() + duration - 4*ramp ) - wi;
  double p = 0.0;
  for ( int n=1; si < fi; n++ ) {
    double sd = trace( intrace ).stdev( si, si+wi );
    p += ( sd - p )/n;
    si += wi;
  }
  //  p = trace( intrace ).stdev( si, fi );

  const double hearingthreshold = 2.0e-5;
  // PEAK amplitude:
  //  double amplitude = 20.0 * log10( p * sqrt( 2.0 ) / soundpressurescale / hearingthreshold );
  // RMS amplitude:
  double amplitude = 20.0 * log10( p / soundpressurescale / hearingthreshold );

  // store data:
  intensities.push( intensity, amplitude );

  if ( intensities.size() > 1 ) {
    // fit straight line:
    double bu = 0.0;
    double mu = 0.0;
    double fitchisq = 0.0;
    int l = 0;
    int r = intensities.size();
    if ( fitslope )
      intensities.lineFit( l, r, fitoffset, bu, fitgain, mu, fitchisq );
    else {
      fitgain = 1.0;
      intensities.offsetFit( l, r, fitoffset, bu, fitgain, fitchisq );
    }
    fitchisq /= intensities.size();
    // improve fit by discarding measurements:
    int minn = intensities.size()/2;
    if ( minn < 4 )
      minn = 4;
    bool improved = true;
    while ( r-l > minn && improved ) {
      improved = false;
      double offset = 0.0;
      double gain = 0.0;
      double chisq = 0.0;
      l++;
      if ( fitslope )
	intensities.lineFit( l, r, offset, bu, gain, mu, chisq );
      else {
	gain = 1.0;
	intensities.offsetFit( l, r, offset, bu, gain, chisq );
      }
      chisq /= r-l;
      if ( (chisq-fitchisq)/fitchisq > -0.1 )
	l--;
      else {
	improved = true;
	fitoffset = offset;
	fitgain = gain;
      }
      if ( r-l > minn ) {
	r--;
	if ( fitslope )
	  intensities.lineFit( l, r, offset, bu, gain, mu, chisq );
	else {
	  gain = 1.0;
	  intensities.offsetFit( l, r, offset, bu, gain, chisq );
	}
	chisq /= r-l;
	if ( (chisq-fitchisq)/fitchisq > -0.1 )
	  r++;
	else {
	  improved = true;
	  fitoffset = offset;
	  fitgain = gain;
	}
      }
    }
    //    cerr << "size=" << intensities.size() << "  l=" << l << "  r=" << r << "  offs=" << fitoffset << "  gain=" << fitgain << '\n';
  }
}


addRePro( CalibSpeakers, acoustic );

}; /* namespace acoustic */

#include "moc_calibspeakers.cc"
