/*
  efield/calibefield.cc
  Calibrates an attenuator for electric field stimuli.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/efield/calibefield.h>
using namespace relacs;

namespace efield {


CalibEField::CalibEField( void )
  : RePro( "CalibEField", "efield",
	   "Jan Benda", "1.3", "Jan 07, 2010" )
{
  // add some parameter as options:
  addBoolean( "reset", "Reset calibration?", false );
  addBoolean( "am", "Amplitude modulation?", false );
  addNumber( "beatfreq", "Beat frequency to be used when fish EOD present", 20.0, 1.0, 100.0, 1.0, "Hz" );
  addNumber( "frequency", "Stimulus frequency to be used when no fish EOD is present", 600.0, 10.0, 1000.0, 5.0, "Hz" );
  addNumber( "duration", "Duration of stimulus", 0.4, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "pause", "Pause", 0.0, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "mincontrast", "Minimum contrast", 0.1, 0.01, 1.0, 0.05, "", "%" );
  addNumber( "maxcontrast", "Maximum contrast", 0.25, 0.01, 1.0, 0.05, "", "%" );
  addInteger( "numintensities", "Number of intensities (amplitudes) to be measured", 10, 4, 1000, 2 );
  addInteger( "repeats", "Maximum repeats", 3, 1, 100, 2 );

  // plot:
  setWidget( &P );    
}


CalibEField::~CalibEField( void )
{
}


int CalibEField::main( void )
{
  // get options:
  bool reset = boolean( "reset" );
  bool am = boolean( "am" );
  double frequency = number( "frequency" );
  double beatfrequency = number( "beatfreq" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  double maxcontrast = number( "maxcontrast" );
  double mincontrast = number( "mincontrast" );
  int numintensities = integer( "numintensities" );
  int repeats = integer( "repeats" );

  int outtrace = am ? GlobalAMEField : GlobalEField;

  if ( duration*beatfrequency < 4 ) {
    warning( "stimulus too short or beat frequency too low." );
    return Failed;
  }

  // attenuator:
  base::LinearAttenuate *latt = 
    dynamic_cast<base::LinearAttenuate*>( attenuator( outTraceName( outtrace ) ) );
  if ( latt == 0 ) {
    warning( "No Attenuator Plugin found!" );
    return Failed;
  }

  // check for analog input traces:
  if ( LocalEODTrace[0] < 0 ) {
    warning( "No local EOD recording available!" );
    return Failed;
  }
  if ( LocalEODEvents[0] < 0 ) {
    warning( "No local EOD events available!" );
    return Failed;
  }

  // data:
  const EventData &ee = events( LocalEODEvents[0] );
  bool fish = ( ee.count( ee.rangeBack() - 1.0 ) > 100 );
  message( fish ? "there IS a fish EOD" : "NO fish EOD" );
  double fishrate = 0.0;
  double fishamplitude = 0.0;
  if ( fish ) {
    fishrate = ee.frequency( ee.rangeBack() - 0.5 );
    if ( duration * beatfrequency < 6 ) {
      beatfrequency = ceil( 10.0 / duration );
      Str s = "Not enough beat periods! <br>You should set the beat frequency to "
	+ Str( beatfrequency ) + "Hz.";
      warning( s, 4.0 );
    }
    if ( am )
      frequency = beatfrequency;
    else
      frequency = fishrate + beatfrequency;

    // mean EOD amplitude:
    double fishupmax = ee.meanSize( ee.back() - 0.5 );
    double fishdownmax = fabs( meanTroughs( trace( LocalEODTrace[0] ),
					    ee.back() - 0.5, ee.back(),
					    0.2 * fishupmax ) );
    fishamplitude = fishupmax > fishdownmax ? fishupmax : fishdownmax;
    Str s = "true fish EOD amplitude = " + Str( fishamplitude );
    s += ", Up = " + Str( fishupmax );
    s += ", Down = " + Str( fishdownmax );
    message( s );
  }
  else if ( am ) {
    warning( "Need fish EOD for calibrating amplitude modulation!" );
    return Failed;
  }
  Amplitude = 0.0;
  int repeatcount = 0;
  LocalEODUnit = trace( LocalEODTrace[0] ).unit();
  Intensities.reserve( 100 );
  if ( reset ) {
    message( "reset gain" );
    latt->setGain( 0.1, 0.0 );
  }
  double origgain = latt->gain();
  double origoffset = latt->offset();
  FitGain = 1.0;
  FitOffset = 0.0;

  double maxsignal = 0.0;
  double maxintensity = 0.0;
  double minintensity = 0.0;
  if ( fish ) {
    maxintensity = maxcontrast * fishamplitude;
    minintensity = mincontrast * fishamplitude;
  }
  else {
    maxintensity = maxcontrast * trace( LocalEODTrace[0] ).maxValue();
    minintensity = mincontrast * trace( LocalEODTrace[0] ).maxValue();
  }
  double intensitystep = (maxintensity - minintensity)/(numintensities-1);
  double intensity = minintensity;
  int intensitycount = 0;

  // plot trace:
  tracePlotSignal( duration );

  // plot:
  P.lock();
  P.setXLabel( "Requested Intensity [" + LocalEODUnit + "]" );
  string ylabel = am ? "Measured AM Intensity" : "Measured EOD Intensity";
  ylabel += " [" + LocalEODUnit + "]";
  P.setYLabel( ylabel );
  P.unlock();

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( currentTime()-0.1,
						  currentTime() );
  if ( val2 > 0.0 )
    adjustGain( trace( LocalEODTrace[0] ), ( 1.0 + maxcontrast ) * val2 );

  // stimulus:
  OutData signal;
  signal.setTrace( outtrace );
  applyOutTrace( signal );
  signal.sineWave( frequency, duration, 1.0 );
  signal.back() = 0;

  while ( softStop() == 0 ) {

    // set intensity:
    if ( intensity > maxIntensity( outtrace ) ) {
      maxintensity = maxIntensity( outtrace );
      intensitystep = (maxintensity - minintensity)/(numintensities-1);
      intensity = minintensity + intensitycount*intensitystep;
    }
    else if ( intensity < minIntensity( outtrace ) ) {
      minintensity = minIntensity( outtrace );
      intensitystep = (maxintensity - minintensity)/(numintensities-1);
      intensity = minintensity + intensitycount*intensitystep;
    }
    signal.setIntensity( intensity );

    // write stimulus:
    write( signal );
    if ( signal.failed() ) {
      warning( "Failed to write out signal!" );
      latt->setGain( origgain, origoffset );
      stop( outtrace );
      return Failed;
    }

    sleep( duration + pause );
    if ( interrupt() ) {
      saveData( latt );
      stop( outtrace );
      return Aborted;
    }

    // adjust signal analog input gains:
    if ( !am && GlobalEFieldTrace >= 0 ) {
      double max = trace( GlobalEFieldTrace ).maxAbs( signalTime(), signalTime()+duration );
      if ( maxsignal < max ) {
	maxsignal = max;
	adjustGain( trace( GlobalEFieldTrace ), 1.1 * maxsignal );
      }
    }

    // setup signal eod detector:
    if ( GlobalEFieldTrace >= 0 && GlobalEFieldEvents >= 0 ) {
      double max = trace( GlobalEFieldTrace ).maxValue();
      detectorEventsOpts( GlobalEFieldEvents ).setMinMax( "threshold", 0.01 * max, max, 0.01*max );
      detectorEventsOpts( GlobalEFieldEvents ).setNumber( "threshold", 0.1 * max );
    }

    int r = analyze( duration, beatfrequency, mincontrast, maxcontrast,
		     intensity, fish );

    Str s = "Loop <b>" + Str( repeatcount+1 ) + "</b>";
    s += ":  Tried <b>" + Str( signal.intensity(), 0, 3, 'g' ) + LocalEODUnit + "</b>";
    s += ",  Measured <b>" + Str( Amplitude, 0, 3, 'g' ) + LocalEODUnit + "</b>";
    message( s );

    plot( maxintensity );

    // next stimulus:
    intensitycount++;
    if ( intensitycount >= numintensities || r > 0 ) {
      // response too strong?
      if ( r == 2 && intensitycount < 4 ) { 
	intensitystep *= 0.5;
	minintensity *= 0.5;
	maxintensity *= 0.5;
      }
      // response too weak?
      else if ( r == 1 || 
		( intensitycount >= 4 && fabs( FitGain ) < 1.0e-6 ) ) {
	intensitystep *= 2.0;
	minintensity *= 2.0;
	maxintensity *= 2.0;
      }
      // fit totally failed:
      else if ( FitGain <= 0.0 ) {
	warning( "Negative slope. <br>Exit." );
	latt->setGain( origgain, origoffset );
	saveData( latt );
	stop( outtrace );
	return Failed;
      }
      // set new parameter:
      else {
	double offset = latt->offset() - FitOffset * latt->gain() / FitGain;
	double gain = latt->gain() / FitGain;
	latt->setGain( gain, offset );
	Str s = "new gain = " + Str( gain );
	s += ",  new offset = " + Str( offset );
	message( s );
	repeatcount++;
	if ( repeatcount == repeats-1 ) {
	  // increase resolution:
	  duration *= 2.0;
	  signal.setTrace( outtrace );
	  applyOutTrace( signal );
	  signal.sineWave( frequency, duration, 1.0 );
	  signal.back() = 0;
	  numintensities *= 2;
	  intensitystep *= 0.5;
	  tracePlotSignal( duration + pause );
	}
	else {
	  // reset intensity range:
	  if ( fish ) {
	    maxintensity = maxcontrast * fishamplitude;
	    minintensity = mincontrast * fishamplitude;
	  }
	  else {
	    maxintensity = maxcontrast * trace( LocalEODTrace[0] ).maxValue();
	    minintensity = mincontrast * trace( LocalEODTrace[0] ).maxValue();
	  }
	}
	FitGain = 1.0;
	FitOffset = 0.0;
      }
      // ready?
      if ( repeatcount >= repeats ) {
	saveData( latt );
	stop( outtrace );
	return Completed;
      }
      intensitystep = (maxintensity - minintensity)/(numintensities-1);
      intensitycount = 0;
      intensity = minintensity;
      Intensities.clear();
    }
    else
      intensity += intensitystep;
  }

  saveData( latt );
  stop( outtrace );
  return Completed;
}


void CalibEField::stop( int outtrace )
{
  Intensities.free();
  writeZero( outtrace );
}


void CalibEField::saveData( const base::LinearAttenuate *latt )
{
  if ( Intensities.empty() )
    return;

  // create file:
  ofstream df( addPath( "calibrate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Options header;
  header.addInteger( "device", latt->aoDevice() );
  header.addInteger( "channel", latt->aoChannel() );
  header.addText( "session time", sessionTimeStr() );
  header.save( df, "# " );
  settings().save( df, "#   " );
  df << '\n';
  TableKey key;
  key.addNumber( "intens", LocalEODUnit, "%8.5g" );
  key.addNumber( "measured", LocalEODUnit, "%8.5g" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<Intensities.size(); k++ ) {
    key.save( df, Intensities.x(k), 0 );
    key.save( df, Intensities.y(k), 1 );
    df << '\n';
  }
  df << '\n' << '\n';
}


void CalibEField::plot( double maxx )
{
  P.lock();
  P.clear();
  P.setXRange( 0.0, maxx );
  P.setYRange( 0.0, maxx*FitGain+FitOffset );
  P.plotLine( 0.0, 0.0, maxx, maxx, Plot::Blue, 4 );
  P.plotLine( 0.0, FitOffset, maxx, maxx*FitGain+FitOffset, Plot::Yellow, 2 );
  P.plot( Intensities, 1.0, Plot::Transparent, 1, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P.draw();
  P.unlock();
}


int CalibEField::analyze( double duration, double beatfrequency,
			  double mincontrast, double maxcontrast,
			  double intensity, bool fish )
{
  const EventData &localeod = events( LocalEODEvents[0] );
  const InData &localeodtrace = trace( LocalEODTrace[0] );

  if ( fish ) {
    // fish EOD present:
    double uppermean = 0.0;
    double upperampl = 0.0;
    double lowermean = 0.0;
    double lowerampl = 0.0;
    beatAmplitudes( localeodtrace, localeod,
		    signalTime(), signalTime() + duration, 1.0/beatfrequency,
		    uppermean, upperampl, lowermean, lowerampl );
    
    Amplitude = sqrt( 2.0 ) * 0.5 * (upperampl + lowerampl);

    // range overflow?
    if ( uppermean+upperampl > 0.95 * localeodtrace.maxValue() ||
	 fabs(lowermean)+lowerampl > 0.95 * localeodtrace.maxValue() ) {
      Str s = "Beat range overflow: upperpeak = " + Str( uppermean+upperampl );
      s += ", lowerpeak = " + Str( fabs(lowermean)+lowerampl );
      s += ", maxValue = " + Str( localeodtrace.maxValue() );
      message( s );
      return 2;
    }
    // range underflow?
    if ( upperampl + lowerampl < 1.0e-6 ||
	 fabs(uppermean) + fabs(lowermean) < 1.0e-6 ) {
      Str s = "Beat range underflow: amplitudes = " + Str( 0.5*(upperampl + lowerampl) );
      s += ", means = " + Str( 0.5*(fabs(uppermean) + fabs(lowermean)) );
      s += ", maxValue = " + Str( localeodtrace.maxValue() );
      message( s );
      return 1;
    }

    double contrast = (upperampl + lowerampl)/(fabs(uppermean) + fabs(lowermean)); 

    // contrast overflow?
    if ( contrast > 1.1*maxcontrast ) {
      Str s = "Contrast overflow: "
	+ Str( (upperampl + lowerampl)/(fabs(uppermean) + fabs(lowermean)) );
      message( s );
      return 2;
    }

    // contrast underflow?
    if ( contrast < 0.9*mincontrast ) {
      Str s = "Contrast underflow: "
	+ Str( (upperampl + lowerampl)/(fabs(uppermean) + fabs(lowermean)) );
      message( s );
      return 1;
    }
  }
  else {
    // no fish EOD present:

    // mean EOD amplitude:
    double offset = 0.1 * duration;
    Amplitude = localeod.meanSize( signalTime() + offset,
				   signalTime() + duration - offset );
    
    // overflow?
    if ( Amplitude > 0.95 * localeodtrace.maxValue() ) {
      Str s = "Signal overflow: Amplitude = " + Str( Amplitude );
      s += ", maxValue = " + Str( localeodtrace.maxValue() );
      message( s );
      return 2;
    }
    // underflow?
    if ( Amplitude < 1.0e-6 ) {
      Str s = "Signal underflow: Amplitude = " + Str( Amplitude );
      message( s );
      return 1;
    }
  }

  // store data:
  Intensities.push( intensity, Amplitude );

  if ( Intensities.size() < 2 )
    return 0;

  // fit straight line:
  /*
  double b[2];
  Intensities->lineFit( b );
  FitOffset = b[0];
  FitGain = b[1];
  */
  double sxx=0.0, sxy=0.0;
  for ( int i=0; i<Intensities.size(); i++ ) {
    sxx += Intensities.x(i) * Intensities.x(i);
    sxy += Intensities.x(i) * Intensities.y(i);
  }
  FitOffset = 0.0;
  FitGain = sxy/sxx;

  return 0;
}


addRePro( CalibEField );

}; /* namespace efield */

#include "moc_calibefield.cc"
