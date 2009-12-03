/*
  efield/calibefield.cc
  Calibrates an attenuator for electric field stimuli.

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
#include <relacs/efield/calibefield.h>
using namespace relacs;

namespace efield {


CalibEField::CalibEField( void )
  : RePro( "CalibEField", "CalibEField", "efield",
	   "Jan Benda", "1.3", "Dec 03, 2009" ),
    P( 1, 1, true, Plot::Copy, this )    
{
  // parameter:
  Reset = false;
  AM = false;
  Duration = 0.4;
  Frequency = 600.0;
  BeatFrequency = 20.0;
  Pause = 0.0;
  Repeats = 3;
  MaxContrast = 0.25;
  MaxIntensities = 10;
  MinIntensityFrac = 0.5;

  // add some parameter as options:
  addBoolean( "reset", "Reset calibration?", Reset );
  addBoolean( "am", "Amplitude modulation?", AM );
  addNumber( "frequency", "Stimulus frequency", Frequency, 10.0, 1000.0, 5.0, "Hz" );
  addNumber( "beatfreq", "Beat frequency", BeatFrequency, 1.0, 100.0, 1.0, "Hz" );
  addNumber( "duration", "Duration of stimulus", Duration, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "pause", "Pause", Pause, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "maxcontrast", "Maximum contrast", MaxContrast, 0.01, 1.0, 0.05, "", "%" );
  addInteger( "maxint", "Maximum number of intensities", MaxIntensities, 2, 1000, 2 );
  addNumber( "minintensity", "Minimum relative intensity", MinIntensityFrac, 0.01, 1.0, 0.05, "", "%" );
  addInteger( "repeats", "Maximum repeats", Repeats, 1, 100, 2 );

  // variables:
  Fish = false;
  OrigGain = 0.1;
  OrigOffset = 0.0;
  FitGain = 1.0;
  FitOffset = 0.0;
  FitFlag = 0;
  Amplitude = 0.0;
  IntensityCount = 0;
  EOD2Unit = "";
  Intensity = 1.0;
  Intensities.clear();
}


CalibEField::~CalibEField( void )
{
}


int CalibEField::main( void )
{
  // get options:
  Reset = boolean( "reset" );
  AM = boolean( "am" );
  Frequency = number( "frequency" );
  BeatFrequency = number( "beatfreq" );
  Duration = number( "duration" );
  Pause = number( "pause" );
  MaxContrast = number( "maxcontrast" );
  MaxIntensities = integer( "maxint" );
  MinIntensityFrac = number( "minintensity" );
  Repeats = integer( "repeats" );
  IntensitiesOffs = int( MaxIntensities * MinIntensityFrac / ( 1.0 - MinIntensityFrac ) );

  // plot:
  P[0].setXLabel( "Requested Intensity" );
  P[0].setYLabel( AM ? "Measured AM Intensity" : "Measured EOD Intensity" );

  // attenuator:
  LAtt = dynamic_cast<base::LinearAttenuate*>( attenuator( 0, AM ? 1 : 0 ) );
  if ( LAtt == 0 ) {
    warning( "No Attenuator found!" );
    return Failed;
  }

  // data:
  const EventData &ee = events( LocalEODEvents[0] );
  Fish = ( ee.meanSize() > 0.1 * trace( LocalEODTrace[0] ).maxValue() &&
	   ee.count( ee.rangeBack() - 1.5 ) > 100 );
  message( Fish ? "there IS a fish EOD" : "NO fish EOD" );
  if ( Fish ) {
    FishRate = ee.frequency( ee.rangeBack() - 0.5 );
    if ( Duration * BeatFrequency < 6 ) {
      BeatFrequency = ceil( 10.0 / Duration );
      Str s = "Not enough beat periods! <br>Set beat frequency to "
	+ Str( BeatFrequency ) + "Hz.";
      warning( s, 4.0 );
    }
    if ( AM ) {
      Frequency = BeatFrequency;
    }
    else {
      Frequency = FishRate + BeatFrequency;
    }
    // mean EOD amplitude:
    double fishupmax = ee.meanSize( ee.back() - 0.5 );
    double fishdownmax = fabs( meanTroughs( trace( LocalEODTrace[0] ),
					    ee.back() - 0.5, 0.5,
					    0.2 * fishupmax ) );
    FishAmplitude = fishupmax > fishdownmax ? fishupmax : fishdownmax;
    Str s = "true fish EOD amplitude = " + Str( FishAmplitude );
    s += ", Up = " + Str( fishupmax );
    s += ", Down = " + Str( fishdownmax );
    message( s );
  }
  else if ( AM ) {
    warning( "Need fish EOD for calibrating amplitude modulation!" );
    return Failed;
  }
  Amplitude = 0.0;
  RepeatCount = 0;
  EOD2Unit = trace( LocalEODTrace[0] ).unit();
  Intensities.reserve( 100 );
  if ( Reset ) {
    cerr << "reset gain" << endl;
    LAtt->setGain( 0.1, 0.0 );
  }
  OrigGain = LAtt->gain();
  OrigOffset = LAtt->offset();
  FitGain = 1.0;
  FitOffset = 0.0;
  FitFlag = 0;

  MaxSignal = 0.0;

  if ( Fish )
    IntensityStep = MaxContrast * FishAmplitude;
  else
    IntensityStep = trace( LocalEODTrace[0] ).maxValue();
  IntensityStep /= IntensitiesOffs + MaxIntensities  + 2;
  Intensity = IntensityStep * IntensitiesOffs;
  IntensityCount = 0;

  // plot trace:
  plotToggle( true, true, Duration, 0.0 );

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( trace( LocalEODTrace[0] ).currentTime()-0.1,
						  trace( LocalEODTrace[0] ).currentTime() );
  if ( val2 > 0.0 )
    adjustGain( trace( LocalEODTrace[0] ), ( 1.0 + MaxContrast ) * val2 );

  // stimulus:
  OutData signal;
  signal.sineWave( Frequency, Duration, 1.0, 0.001 );
  signal.back() = 0;
  signal.setDelay( 0 );
  signal.setChannel( AM ? 1 : 0 );

  for ( ; ; ) {

    for ( int k=0; ; k++ ) {

      // failed to get a succesfull testWrite():
      if ( k >= 40 ) {
	warning( "Could not establish valid intensity!" );
	LAtt->setGain( OrigGain, OrigOffset );
	stop();
	return Failed;
      }

      signal.setIntensity( Intensity );
      if ( Fish )
	detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", 0.5*Intensity );
      testWrite( signal );

      if ( signal.success() ) {
	break;
      }

      else if ( signal.overflow() ) {
	cerr << "CalibEField::main() -> attenuator overflow: " << signal.intensity() << endl;
	IntensityStep *= 0.5;
	Intensity = IntensityStep * IntensitiesOffs;
      }

      else if ( signal.underflow() ) {
	cerr << "CalibEField::main() -> attenuator underflow: " << signal.intensity() << endl;
	IntensityStep *= 2.0;
	Intensity = IntensityStep * IntensitiesOffs;
      }

      else if ( signal.failed() ) {
	cerr << "! error: CalibEField::main() -> failed" << endl;
      }

    }

    write( signal );

    sleep( Duration + Pause );
    if ( interrupt() ) {
      stop();
      return Aborted;
    }


    // adjust analog input gains:
    if ( !AM && EFieldSignalTrace[0] >= 0 ) {
      double max = trace( EFieldSignalTrace[0] ).maxAbs( trace( EFieldSignalTrace[0] ).signalTime(),
							 trace( EFieldSignalTrace[0] ).signalTime()+Duration );
      if ( MaxSignal < max )
	MaxSignal = max;
      adjustGain( trace( EFieldSignalTrace[0] ), 1.1 * MaxSignal );
    }

    // setup signal eod detector:
    if ( EFieldSignalTrace[0] >= 0 && EFieldSignalEvents[0] >= 0 ) {
      double max = trace( EFieldSignalTrace[0] ).maxValue();
      detectorEventsOpts( EFieldSignalEvents[0] ).setMinMax( "threshold", 0.01 * max, max, 0.01*max );
      detectorEventsOpts( EFieldSignalEvents[0] ).setNumber( "threshold", 0.1 * max );
    }

    analyze();

    Str s = "Loop <b>" + Str( RepeatCount+1 ) + "</b>";
    s += ":  Tried <b>" + Str( signal.intensity(), 0, 3, 'g' ) + EOD2Unit + "</b>";
    s += ",  Measured <b>" + Str( Amplitude, 0, 3, 'g' ) + EOD2Unit + "</b>";
    message( s );

    plot();

    // next stimulus:
    IntensityCount++;
    if ( IntensityCount >= MaxIntensities || (FitFlag & 3) ) {
      // overflow?
      if ( FitFlag & 2 ) { 
	cerr << "CalibEField::read() -> signal overflow: " << signal.intensity() << endl;
	if ( Fish && IntensityCount > 1 ) {
	  if ( Intensities.size() > MaxIntensities/2 && 
	       Intensities.size() > 2 )
	    IntensityStep = Intensities.x(Intensities.size()-2)/(IntensitiesOffs+MaxIntensities+2);
	  else
	    IntensityStep = Intensity/(IntensitiesOffs+MaxIntensities+2);
	}
	else
	  IntensityStep *= 0.5;
      }
      // underflow?
      else if ( (FitFlag & 1) && 
		( Intensities.size() < 2 || fabs( FitGain ) < 1.0e-6 ) ) {
	cerr << "CalibEField::read() -> signal underflow: " << signal.intensity() << endl;
	IntensityStep *= 2.0;
      }
      // fit totally failed:
      else if ( FitGain <= 0.0 ) {
	warning( "Negative slope. <br>Exit." );
	LAtt->setGain( OrigGain, OrigOffset );
	stop();
	return Failed;
      }
      // set new parameter:
      else {
	double offset = LAtt->offset() - FitOffset * LAtt->gain() / FitGain;
	double gain = LAtt->gain() / FitGain;
	LAtt->setGain( gain, offset );
	Str s = "new gain = " + Str( gain );
	s += ",  new offset = " + Str( offset );
	message( s );
	RepeatCount++;
	if ( RepeatCount == Repeats-1 ) {
	  // increase resolution:
	  Duration *= 2.0;
	  signal.setDelay( 0 );
	  signal.setChannel( AM ? 1 : 0 );
	  signal.sineWave( Frequency, Duration, 1.0, 0.001 );
	  signal.back() = 0;
	  MaxIntensities *= 2;
	  IntensitiesOffs *= 2;
	  plotToggle( true, true, Duration + Pause, 0.0 );
	}
	if ( Fish )
	  IntensityStep = MaxContrast * FishAmplitude;
	else
	  IntensityStep = trace( LocalEODTrace[0] ).maxValue();
	IntensityStep /= IntensitiesOffs + MaxIntensities + 2;
	FitGain = 1.0;
	FitOffset = 0.0;
      }
      // ready?
      if ( RepeatCount >= Repeats ) {
	stop();
	return Completed;
      }
      FitFlag = 0;
      IntensityCount = 0;
      Intensity = IntensityStep * IntensitiesOffs;
      Intensities.clear();
    }
    else
      Intensity += IntensityStep;
  }

  stop();
  return Completed;
}


void CalibEField::stop( void )
{
  Intensities.free();
  writeZero( AM ? 1 : 0 );
}


void CalibEField::saveData( void )
{
  // create file:
  ofstream df( addPath( "calibrate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Options header;
  header.addInteger( "device", LAtt->aoDevice() );
  header.addInteger( "channel", LAtt->aoChannel() );
  header.addText( "session time", sessionTimeStr() );
  header.save( df, "# " );
  Options::save( df, "#   " );
  df << '\n';
  TableKey key;
  key.addNumber( "intens", EOD2Unit, "%8.5g" );
  key.addNumber( "measured", EOD2Unit, "%8.5g" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<Intensities.size(); k++ ) {
    key.save( df, Intensities.x(k), 0 );
    key.save( df, Intensities.y(k), 1 );
    df << '\n';
  }
  df << '\n' << '\n';
}


void CalibEField::save( void )
{
  saveData();
}


void CalibEField::plot( void )
{
  double x = ( IntensitiesOffs + MaxIntensities + 2 ) * IntensityStep;
  P[0].clear();
  P[0].setXRange( 0.0, x );
  P[0].setYRange( 0.0, RepeatCount > 0 ? x : x*FitGain+FitOffset );
  P[0].plotLine( 0.0, 0.0, x, x, Plot::Blue, 4 );
  P[0].plotLine( 0.0, FitOffset, x, x*FitGain+FitOffset, Plot::Yellow, 2 );
  P[0].plot( Intensities, 1.0, Plot::Transparent, 1, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P.draw();
}


void CalibEField::analyze( void )
{
  double a = 0.0;
  const EventData &eod2 = events( LocalEODEvents[0] );

  if ( Fish ) {
    // fish EOD present:

    const EventData &bpe = events( LocalBeatPeakEvents[0] );
    const EventData &bte = events( LocalBeatTroughEvents[0] );

    // beat:
    if ( bpe.count( bpe.signalTime(), bpe.signalTime()+Duration ) < 4 ) {
      // no beat:
      double offset = 10.0/FishRate;
      double min, max;
      eod2.minMaxSize( eod2.signalTime() + offset,
		       eod2.signalTime() + Duration - offset,
		       min, max );
      Str s = "NO beat: min=" + Str( min );
      s += " max=" + Str( max );
      message( s );
      // overflow?
      if ( min > 0.9 * trace( LocalEODTrace[0] ).maxValue() ) {
	Str s = "EOD Overflow: max Value " + Str( trace( LocalEODTrace[0] ).maxValue() );
	message( s );
	FitFlag |= 2;
	return;
      }
      else {
	// underflow!
	Str s = "EOD Underflow: max Value " + Str( trace( LocalEODTrace[0] ).maxValue() );
	message( s );
	FitFlag |= 1;
	return;
      }
    }

    double offset = 1.0 * bpe.interval( bpe.signalTime(), bpe.signalTime()+Duration );

    // mean upper and lower beat peaks and troughs:
    double upperpeak;
    double uppertrough;
    double lowerpeak;
    double lowertrough;
    beatPeakTroughs( trace( LocalEODTrace[0] ), bpe, bte,
		     bpe.signalTime(), Duration, offset,
		     upperpeak, uppertrough, lowerpeak, lowertrough );
    upperpeak = fabs( upperpeak );
    uppertrough = fabs( uppertrough );
    lowerpeak = fabs( lowerpeak );
    lowertrough = fabs( lowertrough );

    // maximum amplitude:
    double max = upperpeak + lowerpeak;
    // minimum amplitude:
    double min = uppertrough + lowertrough;

    // overflow?
    if ( upperpeak > 0.95 * trace( LocalEODTrace[0] ).maxValue() ||
	 lowerpeak > 0.95 * trace( LocalEODTrace[0] ).maxValue() ) {
      Str s = "Beat Overflow: upperpeak = " + Str( upperpeak );
      s += ", lowerpeak = " + Str( lowerpeak );
      s += ", maxValue = " + Str( trace( LocalEODTrace[0] ).maxValue() );
      message( s );
      FitFlag |= 2;
      return;
    }
    // underflow?
    if ( max < 1.0e-7 ) {
      Str s = "Beat Underflow: upperpeak = " + Str( upperpeak );
      s += ", lowerpeak = " + Str( lowerpeak );
      s += ", maxValue = " + Str( trace( LocalEODTrace[0] ).maxValue() );
      message( s );
      FitFlag |= 1;
      return;
    }

    // contrast overflow?
    if ( max-min > MaxContrast * (max+min) &&
	 IntensityCount < MaxIntensities-2 ) {
      Str s = "Contrast Overflow: " + Str( (max-min)/(min+max) );
      message( s );
      FitFlag |= 2;
      return;
    }
 
    // signal amplitude:
    a = 0.25 * ( max - min );
  }
  else {
    // no fish EOD present:

    // mean EOD amplitude:
    double offset = 0.1 * Duration;
    a = eod2.meanSize( eod2.signalTime() + offset,
		       eod2.signalTime() + Duration - offset );
    
    // overflow?
    if ( a > 0.95 * trace( LocalEODTrace[0] ).maxValue() ) {
      Str s = "Signal Overflow: Amplitude = " + Str( a );
      s += ", maxValue = " + Str( trace( LocalEODTrace[0] ).maxValue() );
      message( s );
      FitFlag |= 2;
      return;
    }
    // underflow?
    if ( a < 1.0e-8 ) {
      Str s = "Signal Underflow: Amplitude = " + Str( a );
      message( s );
      FitFlag |= 1;
      return;
    }
  }

  // store data:
  Amplitude = a;
  Intensities.push( Intensity, a );

  if ( Intensities.size() < 2 )
    return;

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
}


addRePro( CalibEField );

}; /* namespace efield */

#include "moc_calibefield.cc"
