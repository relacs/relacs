/*
  efield/jar.cc
  Measure the fishes JAR and chirp characteristics at different delta f's and beat contrasts.

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

#include <relacs/tablekey.h>
#include <relacs/base/linearattenuate.h>
#include <relacs/efield/jar.h>
using namespace relacs;

namespace efield {


JAR::JAR( void )
  : RePro( "JAR", "efield", "Jan Benda", "2.0", "Mar 09, 2010" )
{
  // parameter:
  ReadCycles = 100;
  Duration = 10.0;
  Pause = 20.0;
  DeltaFStep = 2.0;
  DeltaFMax = 12.0;
  DeltaFMin = -12.0;
  UseContrast = true;
  ContrastMin = 0.1;
  ContrastMax = 0.2;
  ContrastStep = 0.1;
  AmplMin = 1.0;
  AmplMax = 2.0;
  AmplStep = 1.0;
  Repeats = 200;
  Before = 1.0;
  After = 5.0;
  JARAverageTime = 0.5;
  ChirpAverageTime = 0.02;
  EODSaveTime = 1.0;
  SineWave = false;

  // add some parameter as options:
  addLabel( "Stimulation" );
  addNumber( "duration", "Signal duration", Duration, 1.0, 1000000.0, 1.0, "seconds" );
  addNumber( "pause", "Pause between signals", Pause, 1.0, 1000000.0, 1.0, "seconds" );
  addNumber( "ramp", "Duration of linear ramp", 0.5, 0, 10000.0, 0.1, "seconds" );
  addNumber( "deltafstep", "Delta f steps", DeltaFStep, 1.0, 10000.0, 1.0, "Hz" );
  addNumber( "deltafmax", "Maximum delta f", DeltaFMax, -10000.0, 10000.0, 2.0, "Hz" );
  addNumber( "deltafmin", "Minimum delta f", DeltaFMin, -10000.0, 10000.0, 2.0, "Hz" );
  addText( "deltafrange", "Range of delta f's", "" );
  addSelection( "amplsel", "Stimulus amplitude", "contrast|absolute" );
  addNumber( "contrastmax", "Maximum contrast", ContrastMax, 0.01, 1.0, 0.05, "1", "%", "%.0f" ).setActivation( "amplsel", "contrast" );
  addNumber( "contrastmin", "Minimum contrast", ContrastMin, 0.01, 1.0, 0.05, "1", "%", "%.0f" ).setActivation( "amplsel", "contrast" );
  addNumber( "contraststep", "Contrast steps", ContrastStep, 0.01, 1.0, 0.05, "1", "%", "%.0f" ).setActivation( "amplsel", "contrast" );
  addNumber( "amplmin", "Minimum amplitude", AmplMin, 0.1, 1000.0, 0.1, "mV/cm" ).setActivation( "amplsel", "absolute" );
  addNumber( "amplmax", "Maximum amplitude", AmplMax, 0.1, 1000.0, 0.1, "mV/cm" ).setActivation( "amplsel", "absolute" );
  addNumber( "amplstep", "Amplitude steps", AmplStep, 0.1, 1000.0, 0.1, "mV/cm" ).setActivation( "amplsel", "absolute" );
  addInteger( "repeats", "Repeats", Repeats, 0, 1000, 2 );
  addBoolean( "sinewave", "Use sine wave", SineWave );
  addLabel( "Analysis" );
  addNumber( "before", "Time before stimulation to be analyzed", Before, 0.0, 100000.0, 1.0, "seconds" );
  addNumber( "after", "Time after stimulation to be analyzed", After, 0.0, 100000.0, 1.0, "seconds" );
  addBoolean( "savetraces", "Save traces during pause", true );
  addNumber( "jaraverage", "Time for measuring EOD rate", JARAverageTime, 0.01, 10000.0, 0.02, "seconds", "ms" );
  addNumber( "chirpaverage", "Time for measuring chirp data", ChirpAverageTime, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "eodsavetime", "Duration of EOD to be saved", EODSaveTime, 0.0, 10000.0, 0.01, "seconds", "ms" );
  addTypeStyle( OptWidget::TabLabel, Parameter::Label );
  
  // variables:
  GlobalFishAmplitude = 0.0;
  LocalFishAmplitude = 0.0;
  FirstRate = 0.0;
  LastRate = 0.0;
  Intensity = 0.0;
  Contrast = ContrastStep;
  ContrastCount = 0;
  Contrasts.clear();
  DeltaF = 0.0;
  DeltaFRange.clear();
  FileIndex = 0;
  Count = 0; 
  Response.clear();
  MeanResponse.clear();
  EODFrequency.clear();
  EODAmplitude.clear();
  EODTransAmpl.clear();
  EODBeatPhase.clear();
  EODPhases.clear();
  PlotLabelIndex1 = -1;
  PlotLabelIndex2 = -1;
  JARChirpEvents.clear();
  Chirps.clear();

  // plot:
  P.lock();
  P.resize( 4 );
  P[0].setXLabel( "[sec]" );
  P[0].setYRange( Plot::AutoScale, Plot::AutoScale );
  P[0].setYLabel( "EOD [Hz]" );
  P[0].setLMarg( 6 );
  P[0].setRMarg( 1 );
  P[0].setTMarg( 3 );
  P[0].setBMarg( 4 );
  P[0].setOrigin( 0.0, 0.5 );
  P[0].setSize( 1.0, 0.5 );

  P[1].setXLabel( "Beat Df [Hz]" );
  P[1].setYFallBackRange( -10.0, 10.0 );
  P[0].setYRange( Plot::AutoScale, Plot::AutoScale );
  P[1].setYLabel( "JAR [Hz]" );
  P[1].setLMarg( 6 );
  P[1].setRMarg( 0 );
  P[1].setTMarg( 2.0 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 0.33, 0.5 );

  P[2].setXLabel( "Beat Df [Hz]" );
  P[2].setYRange( 0.0, 10.0 );
  P[2].setYLabel( "Chirps #" );
  P[2].setLMarg( 5 );
  P[2].setRMarg( 1 );
  P[2].setTMarg( 2.0 );
  P[2].setOrigin( 0.33, 0.0 );
  P[2].setSize( 0.33, 0.5 );

  P[3].setXLabel( "Time [ms]" );
  P[3].setXRange( -40.0, 40.0 );
  P[3].setYFallBackRange( 0.0, 50.0 );
  P[3].setYRange( Plot::AutoScale, Plot::AutoScale );
  P[3].setYLabel( "Chirps [Hz]" );
  P[3].setLMarg( 5 );
  P[3].setRMarg( 1 );
  P[3].setTMarg( 2.0 );
  P[3].setOrigin( 0.67, 0.0 );
  P[3].setSize( 0.33, 0.5 );
  P.unlock();
  setWidget( &P );
}


int JAR::main( void )
{
  // get options:
  Duration = number( "duration" );
  Pause = number( "pause" );
  double ramp = number( "ramp" );
  Repeats = integer( "repeats" );
  DeltaFStep = number( "deltafstep" );
  DeltaFMax = number( "deltafmax" );
  DeltaFMin = number( "deltafmin" );
  string deltafrange = text( "deltafrange" );
  UseContrast = ( index( "amplsel" ) == 0 );
  ContrastMin = number( "contrastmin" );
  ContrastMax = number( "contrastmax" );
  ContrastStep = number( "contraststep" );
  AmplMin = number( "amplmin" );
  AmplMax = number( "amplmax" );
  AmplStep = number( "amplstep" );
  Before = number( "before" );
  After = number( "after" );
  bool savetraces = boolean( "savetraces" );
  JARAverageTime = number( "jaraverage" );
  ChirpAverageTime = number( "chirpaverage" );
  EODSaveTime = number( "eodsavetime" );
  SineWave = boolean( "sinewave" );
  if ( After + Before > Pause )
    Pause = Before + After;

  if ( EODTrace < 0 ) {
    warning( "need a recording of the EOD Trace." );
    return Failed;
  }
  if ( EODEvents < 0 ) {
    warning( "need EOD events of the EOD Trace." );
    return Failed;
  }
  if ( LocalEODTrace[0] >= 0 && LocalEODEvents[0] < 0 ) {
    warning( "need EOD events of local EOD Trace." );
    return Failed;
  }
  if ( UseContrast && LocalEODTrace[0] < 0 ) {
    warning( "need local EOD for contrasts." );
    return Failed;
  }
  if ( !SineWave && LocalEODEvents[0] < 0 ) {
    warning( "need local EOD events for EOD waveform stimulus." );
    return Failed;
  }

  // check gain of attenuator:
  base::LinearAttenuate *latt = 
    dynamic_cast<base::LinearAttenuate*>( attenuator( outTraceName( GlobalEField ) ) );
  if ( latt != 0 && fabs( latt->gain() - 1.0 ) < 1.0e-8 ) {
    warning( "Attenuator gain is not set!" );
    return Failed;
  }

  // plot trace:
  tracePlotContinuous( 1.0 );

  // ranges:
  ContrastCount = 0;
  if ( UseContrast )
    Contrasts.assign( LinearRange( ContrastMin, ContrastMax, ContrastStep ) );
  else
    Contrasts.assign( LinearRange( AmplMin, AmplMax, AmplStep ) );
  DeltaFRange.clear();
  if ( deltafrange.empty() )
    DeltaFRange.set( DeltaFMin, DeltaFMax, DeltaFStep );
  else
    DeltaFRange.set( deltafrange );
  DeltaFRange.random();

  // data:
  Response.clear();
  Response.resize( Contrasts.size() );
  for ( unsigned int i=0; i<Response.size(); i++ ) {
    Response[i].resize( DeltaFRange.size() );
    for ( unsigned int j=0; j<Response[i].size(); j++ ) {
      Response[i][j].clear();
      Response[i][j].reserve( Repeats );
    }
  }
  MeanResponse.clear();
  MeanResponse.resize( Contrasts.size() );
  for ( unsigned int i=0; i<MeanResponse.size(); i++ ) {
    MeanResponse[i].clear();
    MeanResponse[i].resize( DeltaFRange.size() );
  }
  EODFrequency.clear();
  EODAmplitude.clear();
  EODTransAmpl.clear();
  EODBeatPhase.clear();
  EODPhases.clear();
  JARChirpEvents.clear();
  Chirps.clear();
  Chirps.reserve( int( 100.0*(Duration+Pause) ) );

  // plot:
  P.lock();
  P.clearPlots();
  P[0].setXRange( 0.0, Duration+After );
  P[1].setXRange( DeltaFRange.minValue() - 0.5 * DeltaFStep, 
		  DeltaFRange.maxValue() + 0.5 * DeltaFStep );
  P[2].setXRange( DeltaFRange.minValue() - 0.5 * DeltaFStep, 
		  DeltaFRange.maxValue() + 0.5 * DeltaFStep );
  P.draw();
  P.unlock();

  // EOD rate:
  if ( events( EODEvents ).frequency( JARAverageTime ) < 10.0 ) {
    warning( "Missing EOD!<br>Either no fish or threshold of EOD Detector too high.", 5.0 );
    return Failed;
  }
  FishRate = events( EODEvents ).frequency( ReadCycles );
  if ( FishRate <= 0.0 ) {
    warning( "Not enough EOD cycles recorded!", 5.0 );
    return Failed;
  }

  // trigger:
  //  setupTrigger( data, events );

  // EOD amplitude:
  if ( EODTrace >= 0 )
    GlobalFishAmplitude = eodAmplitude( trace( EODTrace ), currentTime() - 0.5,
					currentTime() );
  else
    GlobalFishAmplitude = 0.0;
  LocalFishAmplitude = 0.0;

  // adjust transdermal EOD:
  if ( LocalEODTrace[0] >= 0 ) {
    LocalFishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ),
				       currentTime() - 0.5, currentTime() );
    double absampl = trace( LocalEODTrace[0] ).maxAbs( currentTime()-0.1,
						       currentTime() );
    if ( absampl > 0.0 ) {
      if ( UseContrast )
	adjustGain( trace( LocalEODTrace[0] ), absampl + ( ContrastMax + 0.1 ) * LocalFishAmplitude );
      else
	adjustGain( trace( LocalEODTrace[0] ), absampl + AmplMax );
    }
  }

  for ( Count = 0;
	(Repeats <= 0 || Count < Repeats ) && softStop() == 0;
	Count++ ) {
    for ( ContrastCount = 0;
	  ContrastCount < (int)Contrasts.size() && softStop() <= 1;
	  ContrastCount++ ) {
      for ( DeltaFRange.reset(); !DeltaFRange && softStop() <= 2; ++DeltaFRange ) {

	setSaving( true );

	Contrast = Contrasts[ContrastCount];
	DeltaF = *DeltaFRange;
	
	// create signal:
	OutData signal;
	signal.setTrace( GlobalEField );
	applyOutTrace( signal );
	if ( SineWave ) {
	  StimulusRate = FishRate + DeltaF;
	  double p = 1.0;
	  if ( fabs( DeltaF ) > 0.01 )
	    p = rint( StimulusRate / fabs( DeltaF ) ) / StimulusRate;
	  else
	    p = 1.0/StimulusRate;
	  int n = (int)::rint( Duration / p );
	  if ( n < 1 )
	    n = 1;
	  signal.sineWave( n*p, -1.0, StimulusRate, 1.0, ramp );
	  signal.setIdent( "sinewave" );
	  IntensityGain = 1.0;
	}
	else if ( LocalEODEvents[0] >= 0 ) {
	  // extract an EOD waveform:
	  double t1 = events( LocalEODEvents[0] ).back( ReadCycles );
	  double t2 = events( LocalEODEvents[0] ).back();
	  trace( LocalEODTrace[0] ).copy( t1, t2, signal );
	  double g = signal.maximize( 0 );
	  signal.setSampleRate( trace( LocalEODTrace[0] ).sampleRate() * ( FishRate + DeltaF ) / FishRate );
	  signal.setCarrierFreq( FishRate + DeltaF );
	  signal.setIdent( "EOD" );
	  StimulusRate = ReadCycles/signal.duration();
	  double maxamplitude = trace( LocalEODTrace[0] ).maxValue() - trace( LocalEODTrace[0] ).minValue();
	  IntensityGain = maxamplitude / LocalFishAmplitude / g;
	  signal.repeat( (int)floor( Duration/signal.duration() ) );
	  signal.ramp( ramp );
	}
	Duration = signal.length();
	signal.setStartSource( 1 );
	signal.setDelay( Before );
	/*
	Str s = "C=" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%";
	s += ", Df=" + Str( DeltaF, 0, 1, 'f' ) + "Hz";
	signal.setIdent( s );
	*/
	
	// stimulus intensity:
	if ( UseContrast )
	  Intensity = Contrast * LocalFishAmplitude * IntensityGain;
	else
	  Intensity = Contrast * IntensityGain;
	signal.setIntensity( Intensity );
	if ( LocalBeatPeakEvents[0] >= 0 )
	  detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", 0.7*signal.intensity() );

	// output signal:
	write( signal );

	// signal failed?
	if ( signal.failed() ) {
	  if ( signal.busy() ) {
	    warning( "Output still busy!<br> Probably missing trigger.<br> Output of this signal software-triggered.", 4.0 );
	    signal.setStartSource( 0 );
	    signal.setPriority();
	    write( signal );
	    // trigger:
	    // setupTrigger( data, events );
	  }
	  else if ( signal.error() == signal.OverflowUnderrun ) {
	    warning( "Analog output overrun error!<br> Try again.", 4.0 );
	    write( signal );
	  }
	  else {
	    string s = "Output of stimulus failed!<br>Error code is <b>";
	    s += signal.errorText() + "</b>";
	    warning( s, 2.0 );
	    writeZero( GlobalEField );
	    return Failed;
	  }
	}
	
	// meassage: 
	Str s = "";
	if ( UseContrast )
	  s = "Contrast: <b>" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%</b>";
	else
	  s = "Amplitude: <b>" + Str( Contrast, "%g" ) + "mV/cm</b>";
	s += "  Delta F:  <b>" + Str( DeltaF, 0, 1, 'f' ) + "Hz</b>";
	s += "  Loop:  <b>" + Str( Count+1 ) + "</b>";
	int rc = DeltaFRange.remainingBlockCount() + 
	  (Contrasts.size()-ContrastCount-1)*DeltaFRange.size()*DeltaFRange.blockRepeat()*DeltaFRange.singleRepeat();
	if ( Repeats > 0 )
	  rc += Contrasts.size()*DeltaFRange.size()*DeltaFRange.blockRepeat()*DeltaFRange.singleRepeat()*(Repeats-Count-1);
	int rs = (Duration + Pause)*rc;
	double rm = floor( rs/60.0 );
	rs -= rm*60.0;
	double rh = floor( rm/60.0 );
	rm -= rh*60.0;
	string rt = "";
	if ( rh > 0.0 )
	  rt += Str( rh, 0, 0, 'f' ) + "h";
	rt += Str( rm, 2, 0, 'f', '0' ) + "min";
	rt += Str( rs, 2, 0, 'f', '0' ) + "sec";
	s += "  Remaining:  <b>" + rt + "</b>";
	message( s );
	
	sleep( Before + Duration + After );
	if ( interrupt() ) {
	  writeZero( GlobalEField );
	  save();
	  return Aborted;
	}
	
	// analyze:
	analyze();
	plot();
	saveTrace();
	FileIndex++;

	setSaving( savetraces );

	sleep( Pause - Before - After );
	if ( interrupt() ) {
	  writeZero( GlobalEField );
	  save();
	  return Aborted;
	}
	
      }
    }
  }
  writeZero( GlobalEField );
  
  save();
  return Completed;
}


void JAR::sessionStarted( void )
{
  FileIndex = 0;
}


void JAR::sessionStopped( bool saved )
{
  FileIndex = 0;
}


void JAR::save( void )
{
  unlockAll();
  Options header;
  header.addInteger( "Index", totalRuns()-1 );
  header.addText( "Waveform", SineWave ? "Sine-Wave" : "Fish-EOD" );
  header.addNumber( "EOD Rate", FishRate, "Hz", "%.1f" );
  header.addNumber( "EOD Amplitude", GlobalFishAmplitude, GlobalEODUnit, "%.2f" );
  if ( LocalFishAmplitude > 0.0 )
    header.addNumber( "Trans. Amplitude", LocalFishAmplitude, LocalEODUnit, "%.2f" );
  header.addText( "RePro Time", reproTimeStr() );
  header.addText( "Session Time", sessionTimeStr() );
  header.addLabel( "settings:" );

  saveJAR( header );
  saveMeanJAR( header );
  lockAll();
  saveEOD( header );
}


void JAR::saveJAR( const Options &header )
{
  // create file:
  ofstream df( addPath( SineWave ? "jars.dat" : "jar.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  settings().save( df, "#   " );
  df << '\n';

  // write key:
  TableKey key;
  key.addLabel( "stimulus" );
  if ( UseContrast )
    key.addNumber( "contrast", "%", "%8.1f" );
  else
    key.addNumber( "amplitude", "mV/cm", "%6.3f" );
  key.addNumber( "deltaf", "Hz", "%5.1f" );
  key.addNumber( "index", "1", "%5.0f" );
  key.addLabel( "jar" );
  if ( UseContrast )
    key.addNumber( "contrast", "%", "%6.1f" );
  key.addNumber( "first", "Hz", "%5.1f" );
  key.addNumber( "last", "Hz", "%5.1f" );
  key.addNumber( "jar", "Hz", "%5.1f" );
  key.addLabel( "chirps" );
  key.addNumber( "num", "1", "%f" );
  key.addNumber( "size", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.addNumber( "width", "ms", "%5.1f" );
  key.addNumber( "s.d.", "ms", "%5.1f" );
  key.addNumber( "ampl", "%", "%5.1f" );
  key.addNumber( "s.d.", "%", "%5.1f" );
  key.addNumber( "phase", "1", "%5.3f" );
  key.addNumber( "s.d.", "1", "%5.3f" );
  key.saveKey( df );

  // write data into file:
  for ( unsigned int i=0; i<Response.size(); i++ ) {
    df << '\n';
    for ( unsigned int j=0; j<Response[i].size(); j++ ) {
      for ( unsigned int k=0; k<Response[i][j].size(); k++ ) {
	key.save( df, UseContrast ? 100.0 * Contrasts[i] : Contrasts[i], 0 );
	key.save( df, DeltaFRange.value( j ) );
	key.save( df, Response[i][j][k].Index );
	if ( UseContrast )
	  key.save( df, 100.0 * Response[i][j][k].Contrast );
	key.save( df, Response[i][j][k].FirstRate );
	key.save( df, Response[i][j][k].LastRate );
	key.save( df, Response[i][j][k].Jar );
	key.save( df, Response[i][j][k].NChirps );
	double v, var;
	// chirp size:
	v = Response[i][j][k].Size;
	var = Response[i][j][k].SizeSq - v * v;
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	// chirp width:
	v = Response[i][j][k].Width;
	var = Response[i][j][k].WidthSq - v * v;
	key.save( df, v*1000.0 );
	key.save( df, var > 0.0 ? sqrt( var )*1000.0 : 0.0 );
	// chirp amplitude:
	v = Response[i][j][k].Amplitude;
	var = Response[i][j][k].AmplitudeSq - v * v;
	key.save( df, 100.0*v );
	key.save( df, 100.0 * ( var > 0.0 ? sqrt( var ) : 0.0 ) );
	// chirp phase:
	v = Response[i][j][k].Phase;
	var = Response[i][j][k].PhaseSq - v * v;
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	df << '\n';
      }
    }
  }
  df << "\n\n";
}


void JAR::saveMeanJAR( const Options &header )
{
  ofstream df( addPath( SineWave ? "jarmeans.dat" : "jarmean.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  settings().save( df, "#   " );
  df << '\n';

  // write key:
  TableKey key;
  key.addLabel( "stimulus" );
  if ( UseContrast )
    key.addNumber( "contrast", "%", "%8.1f" );
  else
    key.addNumber( "amplitude", "mV/cm", "%6.3f" );
  key.addNumber( "deltaf", "Hz", "%6.1f" );
  key.addNumber( "n", "1", "%3.0f" );
  key.addLabel( "jar" );
  if ( UseContrast )
    key.addNumber( "contrast", "%", "%8.1f" );
  key.addNumber( "s.d.", "%", "%5.1f" );
  key.addNumber( "first", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.addNumber( "last", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.addNumber( "jar", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.addLabel( "chirps" );
  key.addNumber( "total", "1", "%5.0f" );
  key.addNumber( "num", "1", "%5.1f" );
  key.addNumber( "s.d.", "1", "%5.1f" );
  key.addNumber( "size", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.addNumber( "width", "ms", "%5.1f" );
  key.addNumber( "s.d.", "ms", "%5.1f" );
  key.addNumber( "ampl", "%", "%5.1f" );
  key.addNumber( "s.d.", "%", "%5.1f" );
  key.addNumber( "phase", "1", "%5.3f" );
  key.addNumber( "s.d.", "1", "%5.3f" );
  key.saveKey( df );

  // write data into file:
  for ( unsigned int i=0; i<MeanResponse.size(); i++ ) {
    df << '\n';
    for ( unsigned int j=0; j<MeanResponse[i].size(); j++ ) {
      if ( MeanResponse[i][j].NJar > 0 ) {
	double var, v;
	key.save( df, UseContrast ? 100.0 * Contrasts[i] : Contrasts[i], 0 );
	key.save( df, DeltaFRange.value( j ) );
	key.save( df, MeanResponse[i][j].NJar );
	// measured contrast:
	if ( UseContrast ) {
	  v = MeanResponse[i][j].Contrast;
	  var = MeanResponse[i][j].ContrastSq - v * v;
	  key.save( df, 100.0 * v );
	  key.save( df, var > 0.0 ? 100.0 * sqrt( var ) : 0.0 );
	}
	// first rate:
	v = MeanResponse[i][j].First;
	var = MeanResponse[i][j].FirstSq - v * v;
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	// last:
	v = MeanResponse[i][j].Last;
	var = MeanResponse[i][j].LastSq - v * v;
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	// jar:
	v = MeanResponse[i][j].Jar;
	var = MeanResponse[i][j].JarSq - v * v;
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	// number of chirps:
	v = MeanResponse[i][j].Chirps;
	var = MeanResponse[i][j].ChirpsSq - v * v;
	key.save( df, MeanResponse[i][j].NChirps );
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	// chirp size:
	v = MeanResponse[i][j].Size;
	var = MeanResponse[i][j].SizeSq - v * v;
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	// chirp width:
	v = MeanResponse[i][j].Width;
	var = MeanResponse[i][j].WidthSq - v * v;
	key.save( df, v*1000.0 );
	key.save( df, var > 0.0 ? sqrt( var )*1000.0 : 0.0 );
	// chirp amplitude:
	v = MeanResponse[i][j].Amplitude;
	var = MeanResponse[i][j].AmplitudeSq - v * v;
	key.save( df, 100.0*v );
	key.save( df, 100.0 * ( var > 0.0 ? sqrt( var ) : 0.0 ) );
	// chirp phase:
	v = MeanResponse[i][j].Phase;
	var = MeanResponse[i][j].PhaseSq - v * v;
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
	df << '\n';
      }
    }
  }
  df << "\n\n";
}


void JAR::saveEOD( const Options &header )
{
  ofstream df( addPath( SineWave ? "jareods.dat" : "jareod.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  settings().save( df, "#   " );
  df << '\n';

  // write key:
  TableKey key;
  key.addNumber( "time", "s", "%9.5f" );
  key.addNumber( "EOD", GlobalEODUnit, "%.5g" );
  key.saveKey( df );

  // write data into file:
  for ( int k=trace( EODTrace ).size() - trace( EODTrace ).indices( EODSaveTime ) - 10;
	k < trace( EODTrace ).size() - 10;
	k++ ) {
    key.save( df, trace( EODTrace ).pos( k ), 0 );
    key.save( df, trace( EODTrace )[k] ); 
    df << '\n';
  }
  df << "\n\n";
}


void JAR::saveTrace( void )
{
  Options header;
  header.addInteger( "Index", FileIndex );
  header.addNumber( "Delta f", DeltaF, "Hz", "%.1f" );
  if ( UseContrast )
    header.addNumber( "Contrast", 100.0*Contrast, "%", "%.1f" );
  else
    header.addNumber( "Amplitude", Contrast, "mV/cm", "%.3f" );
  header.addNumber( "Duration", Duration, "sec", "%.3f" );
  header.addNumber( "Pause", Pause, "sec", "%.3f" );
  header.addText( "Waveform", SineWave ? "Sine-Wave" : "Fish-EOD" );
  header.addNumber( "True Delta f", TrueDeltaF, "Hz", "%.1f" );
  if ( TrueContrast > 0.0 )
    header.addNumber( "True Contrast", 100.0*TrueContrast, "Hz", "%.1f" );
  header.addNumber( "EOD Rate before", FirstRate, "Hz", "%.1f" );
  header.addNumber( "EOD Rate at end", LastRate, "Hz", "%.1f" );
  header.addNumber( "JAR", LastRate-FirstRate, "Hz", "%.1f" );
  header.addNumber( "EOD Amplitude", GlobalFishAmplitude, GlobalEODUnit, "%.2f" );
  if ( LocalFishAmplitude > 0.0 )
    header.addNumber( "Trans. Amplitude", LocalFishAmplitude, LocalEODUnit, "%.2f" );
  header.addText( "RePro Time", reproTimeStr() );
  header.addText( "Session Time", sessionTimeStr() );

  saveEODFreq( header );
  saveChirps( header );
  saveChirpTraces( header );
  saveChirpEOD( header );
}


void JAR::saveEODFreq( const Options &header )
{
  ofstream df( addPath( SineWave ? "jareodtracess.dat" : "jareodtraces.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  TableKey key;
  key.addNumber( "time", "s", "%9.5f" );
  key.addNumber( "freq", "Hz", "%5.1f" );
  if ( ! LocalEODUnit.empty() )
    key.addNumber( "ampl", LocalEODUnit, "%6.4f" );
  key.addNumber( "beat", "1", "%5.3f" );
  key.saveKey( df );

  // write data into file:
  for ( int k=0;
	k<EODFrequency.size() && k<EODBeatPhase.size();
	k++ ) {
    key.save( df, EODFrequency.x(k), 0 );
    key.save( df, EODFrequency.y(k) );
    if ( ! LocalEODUnit.empty() )
      key.save( df, EODTransAmpl.y(k) );
    key.save( df, EODBeatPhase.y(k) );
    df << '\n';
  }
  df << "\n\n";
}


void JAR::saveChirps( const Options &header )
{
  ofstream df( addPath( SineWave ? "jarchirpss.dat" : "jarchirps.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  TableKey key;
  key.addLabel( "chirp" );
  key.addNumber( "time", "s", "%10.5f" );
  key.addNumber( "size", "Hz", "%5.1f" );
  key.addNumber( "width", "ms", "%5.1f" );
  key.addNumber( "ampl", GlobalEODUnit, "%6.4f" );
  key.addNumber( "adiff", GlobalEODUnit, "%6.4f" );
  key.addNumber( "dip", "%", "%5.1f" );
  key.addNumber( "phase", "1", "%5.3f" );
  key.addLabel( "beat" );
  key.addNumber( "phase", "1", "%5.3f" );
  key.addNumber( "loc", "1", "%5.3f" );
  key.addNumber( "df", "Hz", "%5.1f" );
  if ( ! LocalEODUnit.empty() ) {
    key.addNumber( "ampl-", LocalEODUnit, "%6.4f" );
    key.addNumber( "ampl+", LocalEODUnit, "%6.4f" );
    key.addNumber( "jump", LocalEODUnit, "%7.4f" );
    key.addNumber( "rjump", "%", "%5.1f" );
  }
  key.saveKey( df );

  // write data into file:
  if ( Chirps.size() == 0 ) {
      key.save( df, "", 0 );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      if ( ! LocalEODUnit.empty() ) {
	key.save( df, "" );
	key.save( df, "" );
	key.save( df, "" );
	key.save( df, "" );
      }
      df << '\n';
  }
  else {
    for ( unsigned int k=0; k<Chirps.size(); k++ ) {
      key.save( df, Chirps[k].Time, 0 );
      key.save( df, Chirps[k].Size );
      key.save( df, Chirps[k].Width*1000.0 );
      key.save( df, Chirps[k].EODAmpl );
      key.save( df, Chirps[k].Amplitude );
      key.save( df, 100.0 * Chirps[k].Amplitude / Chirps[k].EODAmpl );
      key.save( df, Chirps[k].Phase );
      key.save( df, Chirps[k].BeatPhase );
      key.save( df, Chirps[k].BeatLoc );
      key.save( df, Chirps[k].Deltaf );
      if ( ! LocalEODUnit.empty() ) {
	key.save( df, Chirps[k].BeatBefore );
	key.save( df, Chirps[k].BeatAfter );
	key.save( df, Chirps[k].BeatAfter - Chirps[k].BeatBefore );
	key.save( df, 100.0 * ( Chirps[k].BeatAfter - Chirps[k].BeatBefore ) / ( Chirps[k].BeatAfter + Chirps[k].BeatBefore ) );
      }
      df << '\n';
    }
  }
  df << "\n\n";
}


void JAR::saveChirpTraces( const Options &header )
{
  ofstream df( addPath( SineWave ? "jarchirptracess.dat" : "jarchirptraces.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  TableKey key;
  key.addNumber( "time", "s", "%7.2f" );
  key.addNumber( "size", "Hz", "%5.1f" );
  key.addNumber( "ampl", GlobalEODUnit, "%6.4f" );
  key.addNumber( "phase", "1", "%5.3f" );
  if ( ! LocalEODUnit.empty() ) {
    key.addNumber( "ampl2", LocalEODUnit, "%6.4f" );
    key.addNumber( "beat", "1", "%5.3f" );
  }
  key.saveKey( df );

  // write data into file:
  if ( Chirps.size() == 0 ) {
    key.save( df, "", 0 );
    key.save( df, "" );
    key.save( df, "" );
    key.save( df, "" );
    key.save( df, "" );
    key.save( df, "" );
    df << '\n';
  }
  else {
    int j = 0;
    for ( unsigned int k=0; k<Chirps.size(); k++ ) {
      int wi = (int)::rint( Chirps[k].Width * Chirps[k].EODRate );
      for ( ; j<EODFrequency.size(); j++ )
	if ( EODFrequency.x(j) >= Chirps[k].Time )
	  break;
      for ( int i = -wi; i < wi; i++ ) {
	if ( j+i >= 0 && j+i < EODFrequency.size() ) {
	  key.save( df, 1000.0*(EODFrequency.x(j+i) - Chirps[k].Time), 0 );
	  key.save( df, EODFrequency.y(j+i) );
	  key.save( df, EODAmplitude.y(j+i) );
	  key.save( df, EODPhases.y(j+i) );
	  if ( ! LocalEODUnit.empty() ) {
	    key.save( df, EODTransAmpl.y(j+i) );
	    key.save( df, EODBeatPhase.y(j+i) );
	  }
	  df << '\n';
	}
      }
      df << '\n';
    }
  }
  df << "\n\n";
}


void JAR::saveChirpEOD( const Options &header )
{
  if ( EODTrace < 0 || EODEvents < 0 )
    return;

  ofstream df( addPath( SineWave ? "jarchirpeods.dat" : "jarchirpeod.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  TableKey key;
  key.addNumber( "time", "ms", "%6.2f" );
  key.addNumber( "EOD", trace( EODTrace ).unit(), "%7.4f" );
  key.saveKey( df );

  // write data into file:
  if ( Chirps.size() == 0 ) {
    key.save( df, "", 0 );
    key.save( df, "" );
    df << '\n';
  }
  else {
    for ( unsigned int k=0; k<Chirps.size(); k++ ) {
      double wt = Chirps[k].Width;
      double ect = events( EODEvents ).nextTime( Chirps[k].Time + signalTime() );
      InDataIterator ft = trace( EODTrace ).begin( ect-wt );
      InDataIterator lt = trace( EODTrace ).begin( ect+wt );
      InDataTimeIterator ti = trace( EODTrace ).timeBegin( ect-wt );
      for ( InDataIterator i = ft; i < lt && !i; ++i, ++ti ) {
	key.save( df, 1000.0 * ( *ti - Chirps[k].Time - signalTime() ), 0 );
	key.save( df, *i );
	df << '\n';
      }
      df << '\n';
    }
  }
  df << "\n\n";
}


void JAR::plot( void )
{
  P.lock();
  // eod frequency with chirp events:
  P[0].clear();
  Str s;
  s = "Delta f = " + Str( DeltaF, 0, 0, 'f' ) + "Hz";
  if ( UseContrast )
    s += ", Contrast = " + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%, ";
  else
    s += ", Amplitude = " + Str( Contrast ) + "mV/cm, ";
  s += SineWave ? "Sine Wave" : "Fish EOD";
  P[0].setTitle( s );
  P[0].plotVLine( Duration );
  P[0].plot( EODFrequency, 1.0, Plot::Green, 2, Plot::Solid );
  P[0].plot( JARChirpEvents, 2, 0.0, 1.0, 0.9, Plot::Graph, 
	     1, Plot::Circle, 5, Plot::Pixel, Plot::Yellow, Plot::Yellow );

  P[1].clear();
  P[1].plotHLine( 0.0 );
  MapD m;
  m.reserve( Repeats * DeltaFRange.size() );
  // jars:
  for ( unsigned int j=0; j<Response[ContrastCount].size(); j++ ) {
    for ( unsigned int k=0; k<Response[ContrastCount][j].size(); k++ ) {
      m.push( DeltaFRange.value( j ),
	      Response[ContrastCount][j][k].Jar );
    }
  }
  P[1].plot( m, 1.0, Plot::Yellow, 0, Plot::Solid, Plot::Circle, 6, Plot::Yellow, Plot::Yellow );
  // current jar:
  m.clear();
  m.resize( 1, *DeltaFRange, 
	    Response[ContrastCount][DeltaFRange.pos()][Count].Jar );
  P[1].plot( m, 1.0, Plot::Red, 0, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );

  P[2].clear();
  // chirps phase:
  m.clear();
  m.reserve( Response[ContrastCount].size() * 20 );
  for ( unsigned int j=0; j<Response[ContrastCount].size(); j++ )
    for ( unsigned int k=0; k<Response[ContrastCount][j].size(); k++ )
      if ( Response[ContrastCount][j][k].NChirps > 0 ) {
	m.push( DeltaFRange.value(j ) + 0.2 * DeltaFStep,
		10.0*Response[ContrastCount][j][k].Phase );
      }
  P[2].plot( m, 1.0, Plot::Blue, 0, Plot::Solid, Plot::Diamond, 6, Plot::Blue, Plot::Blue );
  // chirps:
  m.clear();
  m.reserve( Response[ContrastCount].size() * 20 );
  for ( unsigned int j=0; j<Response[ContrastCount].size(); j++ )
    for ( unsigned int k=0; k<Response[ContrastCount][j].size(); k++ ) {
      m.push( DeltaFRange.value( j ),
	      Response[ContrastCount][j][k].NChirps );
    }
  P[2].plot( m, 1.0, Plot::Yellow, 0, Plot::Solid, Plot::TriangleUp, 6, Plot::Yellow, Plot::Yellow );

  P[3].clear();
  MapD cm;
  int j = 0;
  for ( unsigned int k=0; k<Chirps.size(); k++ ) {
    for ( ; j<EODFrequency.size(); j++ )
      if ( EODFrequency.x(j) >= Chirps[k].Time )
	break;
    int wi = (int)::rint( Chirps[k].Width * Chirps[k].EODRate );
    // chirp frequency:
    cm.clear();
    cm.reserve( 2*wi+10 );
    for ( int i = -wi; i<wi; i++ ) {
      if ( j+i >= 0 && j+i < EODFrequency.size() ) {
	cm.push( 1000.0*(EODFrequency.x(j+i) - Chirps[k].Time),
		 EODFrequency.y(j+i) - Chirps[k].EODRate );
      }
    }
    P[3].plot( cm, 1.0, Plot::Yellow, 2, Plot::Solid );
    // chirp phase:
    cm.clear();
    cm.reserve( 2*wi+10 );
    for ( int i = -wi; i<wi; i++ ) {
      if ( j+i >= 0 && j+i < EODPhases.size() ) {
	cm.push( 1000.0*(EODPhases.x(j+i) - Chirps[k].Time),
		 40.0 * EODPhases.y(j+i) );
      }
    }
    P[3].plot( cm, 1.0, Plot::Red, 2, Plot::Solid );
  }

  P.draw();
  P.unlock();
}


void JAR::analyze( void )
{
  const EventData &eodglobal = events( EODEvents );
  const EventData &eodlocal = LocalEODEvents[0] >= 0 ? events( LocalEODEvents[0] ) : events( EODEvents );
  const EventData &sige = events( GlobalEFieldEvents );

  // EOD rate:
  double teod = signalTime();
  do {
    FishRate = eodglobal.frequency( teod - JARAverageTime, teod );
    teod -= JARAverageTime;
  } while ( FishRate < 1.0 );

  // Delta F:
  TrueDeltaF = sige.frequency( signalTime(), signalTime() +  JARAverageTime ) - FishRate;

  // EOD amplitude:
  GlobalFishAmplitude = eodAmplitude( trace( EODTrace ),
				      eodglobal.back() - JARAverageTime,
				      eodglobal.back() );
  if ( LocalEODTrace[0] >= 0 )
    LocalFishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ),
				       eodlocal.back() - JARAverageTime,
				       eodlocal.back() );

  // contrast:
  if ( LocalEODTrace[0] >= 0 )
    TrueContrast = beatContrast( trace(LocalEODTrace[0]),
				 signalTime()+0.1*Duration,
				 signalTime()+0.9*Duration,
				 fabs( DeltaF ) );
  else
    TrueContrast = 0.0;

  // mean rate before stimulus:
  FirstRate = eodglobal.frequency( signalTime() - JARAverageTime,
				   signalTime() );

  // mean rate at end of stimulus:
  LastRate = eodglobal.frequency( signalTime() + Duration - JARAverageTime,
				  signalTime() + Duration );

  // JAR:
  double jar = LastRate - FirstRate;

  // EOD trace:
  GlobalEODUnit = trace( EODTrace ).unit();
  if ( LocalEODTrace[0] >= 0 )
    LocalEODUnit = trace( LocalEODTrace[0] ).unit();
  else
    LocalEODUnit = "";

  EventIterator first1 = eodglobal.begin( signalTime() - Before );
  EventIterator last1 = eodglobal.begin( signalTime() + Duration + After );
  if ( last1 >= eodglobal.end() - 2 )
    last1 = eodglobal.end() - 2;

  // EOD frequency:
  EventFrequencyIterator findex;
  EODFrequency.clear();
  EODFrequency.reserve( last1 - first1 + 2 );
  EODPhases.clear();
  EODPhases.reserve( last1 - first1 + 2 );
  for ( findex=first1; findex < last1; ++findex ) {
    EODFrequency.push( findex.time() - signalTime(), *findex );
    EODPhases.push( findex.time() - signalTime(), 0.0 );
  }

  // EOD amplitude:
  EventSizeIterator sindex;
  EODAmplitude.clear();
  EODAmplitude.reserve( last1 - first1 + 2 );
  for ( sindex=first1; sindex < last1; ++sindex ) {
    EODAmplitude.push( sindex.time() - signalTime(), *sindex );
  }

  // EOD transdermal amplitude:
  EODTransAmpl.clear();
  if ( LocalEODEvents[0] >= 0 ) {
    EventIterator first2 = eodlocal.begin( signalTime() );
    EventIterator last2 = eodlocal.begin( signalTime() + Duration + After );
    EODTransAmpl.reserve( last2 - first2 + 2 );
    for ( sindex = first2; sindex < last2; ++sindex ) {
      EODTransAmpl.push( sindex.time() - signalTime(), *sindex );
    }
  }

  // Beat phase:
  EODBeatPhase.clear();
  EODBeatPhase.reserve( last1 - first1 + 2 );
  for ( EventIterator index = first1; index < last1; ++index ) {
    long ti = sige.next( *index );
    if ( ti >= sige.size() )
      break;
    double t1 = sige[ ti ];
    int pi = eodglobal.previous( t1 );
    double t0 = eodglobal[ pi ];
    if ( pi+1 >= eodglobal.size() )
      break;
    double phase = ( t1 - t0 ) / ( eodglobal[ pi + 1 ] - t0 );
    EODBeatPhase.push( index.time() - signalTime(), phase );
  }

  // store results:
  ResponseData d( FileIndex, FirstRate, LastRate, jar, TrueContrast );
  Response[ContrastCount][DeltaFRange.pos()].push_back( d );

  // Chirps:
  Chirps.clear();
  JARChirpEvents.clear();
  if ( ChirpEvents >= 0 ) {
    JARChirpEvents.assign( events( ChirpEvents ),
			   signalTime(), signalTime() + Duration + After );
    for ( int k=0; k < JARChirpEvents.size(); k++ ) {

      // time:
      double time = JARChirpEvents[k] - JARChirpEvents.signalTime();
      EventFrequencyIterator eventfp = eodglobal.begin( JARChirpEvents[k] + signalTime() );
      last1 = eodglobal.end() - 1;

      // size:
      double size = JARChirpEvents.eventSize( k );

      // width:
      double width = JARChirpEvents.eventWidth( k );

      // mean rate before chirp:
      double meanrate = eodglobal.frequency( JARChirpEvents[k] + signalTime() - 0.7 * width - ChirpAverageTime,
					     JARChirpEvents[k] + signalTime() - 0.7 * width );

      // current deltaf:
      double cdeltaf = StimulusRate - meanrate;

      // mean amplitude before chirp:
      double meanampl = eodglobal.meanSize( JARChirpEvents[k] + signalTime() - 0.7 * width - ChirpAverageTime,
					    JARChirpEvents[k] + signalTime() - 0.7 * width );

      // find maximum amplitude difference:
      EventSizeIterator sindex;
      double ampl = 0;
      for ( sindex = eventfp - 0.7*width; sindex < eventfp + 0.7*width; ++sindex )
	if ( ampl < fabs( *sindex - meanampl ) )
	  ampl = fabs( *sindex - meanampl );

      // phase shift:
      // time of an eod zero crossing before the chirp:
      double eodtime = eodglobal.previousTime( eventfp.time() - width );
      // mean EOD interval before the chirp:
      double meaninterv = 1.0 / meanrate;

      // time course of chirp phase shift:
      EventIterator gefi = eventfp - width - 10;
      double ophase = -0.3;
      for ( int j = gefi.index() - eodglobal.next( signalTime() );
	    !gefi && gefi < eventfp + width + 10 && 
	      j < EODPhases.size();
	    ++gefi, ++j ) {
	double t = *gefi;
	EODPhases.x(j) = t - signalTime();
	double t0 = floor( ( t - eodtime ) / meaninterv ) * meaninterv + eodtime;
	double phase = ( t - t0 ) / meaninterv;
	phase -= rint( phase - ophase );
	EODPhases.y(j) = -phase;
	ophase = phase;
      } 

      // mean phase before:
      gefi = eventfp - 0.7*width;
      int inx = gefi.index() - eodglobal.next( signalTime() );
      double meanphasel = 0.0;
      for ( int j=0; j < 6 && inx-j >= 0 && inx-j < EODPhases.size(); j++ ) {
	meanphasel += ( EODPhases.y(inx-j) - meanphasel ) / (j+1);
	--gefi;
      }

      // mean phase after:
      gefi = eventfp + 0.7*width;
      inx = gefi.index() - eodglobal.next( signalTime() );
      double meanphaser = 0.0;
      for ( int j=0; j < 6 && inx+j >= 0 && inx+j < EODPhases.size(); j++ ) {
	meanphaser += ( EODPhases.y(inx+j) - meanphaser ) / (j+1);
	--gefi;
      }

      // mean chirp phase shift:
      double meanphase = meanphaser - meanphasel;

      // beat phase:
      double beatphase = 0.0;
      gefi = sige.begin( eventfp.time() ) - 2;
      for ( int n=0; n < 4 && gefi < sige.end(); n++, ++gefi ) {
	double t1 = *gefi;
	double t0 = floor( ( t1 - eodtime ) / meaninterv ) * meaninterv + eodtime;
	double phase = ( t1 - t0 ) / meaninterv;
	beatphase += ( phase - beatphase ) / (n+1);
      }

      // beat location:
      double beatloc = cdeltaf <= 0.0 ? beatphase : 1.0 - beatphase;

      double beatbefore = 0.0;
      double beatafter = 0.0;
      if ( LocalEODEvents[0] >= 0 ) {
	// beat amplitude right before chirp:
	beatbefore = eodlocal.meanSize( JARChirpEvents[k] + signalTime() - 0.6 * width - 4.0 * meaninterv,
					JARChirpEvents[k] + signalTime() - 0.6 * width );
	// beat amplitude right after chirp:
	beatafter = eodlocal.meanSize( JARChirpEvents[k] + signalTime() + 0.6 * width,
				       JARChirpEvents[k] + signalTime() + 0.6 * width + 4.0 * meaninterv );
      }

      // store results:
      ChirpData d( time, size, width, meanrate, meanampl, ampl, meanphase, 
		   beatphase, beatloc, cdeltaf, beatbefore, beatafter );
      Chirps.push_back( d );
      
      Response[ContrastCount][DeltaFRange.pos()].back().addChirp( size, width, ampl/meanampl, meanphase );
      MeanResponse[ContrastCount][DeltaFRange.pos()].addChirp( size, width, ampl/meanampl, meanphase );
    }
  }
  
  // store results:
  MeanResponse[ContrastCount][DeltaFRange.pos()].addJAR( TrueContrast,
							 FirstRate, LastRate,
							 jar, Chirps.size() );
  
}


JAR::ResponseData::ResponseData( int i, double f, double l, double j,
				 double c ) 
  : Index( i ),
    FirstRate( f ),
    LastRate( l ),
    Jar( j ),
    Contrast( c ),
    NChirps( 0 ),
    Size( 0.0 ),
    SizeSq( 0.0 ),
    Width( 0.0 ),
    WidthSq( 0.0 ),
    Amplitude( 0.0 ),
    AmplitudeSq( 0.0 ),
    Phase( 0.0 ),
    PhaseSq( 0.0 )
{
}


void JAR::ResponseData::addChirp( double size, double width, double ampl,
				  double phase )
{
  NChirps++;
  Size += ( size - Size ) / NChirps;
  SizeSq += ( size*size - SizeSq ) / NChirps;
  Width += ( width - Width ) / NChirps;
  WidthSq += ( width*width - WidthSq ) / NChirps;
  Amplitude += ( ampl - Amplitude ) / NChirps;
  AmplitudeSq += ( ampl*ampl - AmplitudeSq ) / NChirps;
  Phase += ( phase - Phase ) / NChirps;
  PhaseSq += ( phase*phase - PhaseSq ) / NChirps;
}


JAR::MeanResponseData::MeanResponseData( void ) 
  : NJar( 0 ),
    Contrast( 0.0 ),
    ContrastSq( 0.0 ),
    First( 0.0 ),
    FirstSq( 0.0 ),
    Last( 0.0 ),
    LastSq( 0.0 ),
    Jar( 0.0 ),
    JarSq( 0.0 ),
    Chirps( 0.0 ),
    ChirpsSq( 0.0 ),
    NChirps( 0 ),
    Size( 0.0 ),
    SizeSq( 0.0 ),
    Width( 0.0 ),
    WidthSq( 0.0 ), 
    Amplitude( 0.0 ),
    AmplitudeSq( 0.0 ),
    Phase( 0.0 ),
    PhaseSq( 0.0 )
{
}


void JAR::MeanResponseData::addJAR( double contrast, double f, double l,
				    double jar, int chirps )
{
  NJar++;
  Contrast += ( contrast - Contrast ) / NJar;
  ContrastSq += ( contrast*contrast - ContrastSq ) / NJar;
  First += ( f - First ) / NJar;
  FirstSq += ( f*f - FirstSq ) / NJar;
  Last += ( l - Last ) / NJar;
  LastSq += ( l*l - LastSq ) / NJar;
  Jar += ( jar - Jar ) / NJar;
  JarSq += ( jar*jar - JarSq ) / NJar;
  Chirps += ( chirps - Chirps ) / NJar;
  ChirpsSq += ( chirps*chirps - ChirpsSq ) / NJar;
}


void JAR::MeanResponseData::addChirp( double size, double width, double ampl, double phase )
{
  NChirps++;
  Size += ( size - Size ) / NChirps;
  SizeSq += ( size*size - SizeSq ) / NChirps;
  Width += ( width - Width ) / NChirps;
  WidthSq += ( width*width - WidthSq ) / NChirps;
  Amplitude += ( ampl - Amplitude ) / NChirps;
  AmplitudeSq += ( ampl*ampl - AmplitudeSq ) / NChirps;
  Phase += ( phase - Phase ) / NChirps;
  PhaseSq += ( phase*phase - PhaseSq ) / NChirps;
}


JAR::ChirpData::ChirpData( double t, double s, double w, double er, double ea,
			   double a, double p, double bp, double bl,
			   double bf, double bb, double ba )
  : Time( t ),
    Size( s ),
    Width( w ),
    EODRate( er ),
    EODAmpl( ea ),
    Amplitude( a ),
    Phase( p ), 
    BeatPhase( bp ),
    BeatLoc( bl ),
    Deltaf ( bf ),
    BeatBefore( bb ),
    BeatAfter( ba )
{
}


addRePro( JAR );

}; /* namespace efield */

#include "moc_jar.cc"
