/*
  efield/jar.cc
  Measure the fishes JAR and chirp characteristics at different delta f's and beat contrasts.

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

#include <relacs/tablekey.h>
#include <relacs/efield/jar.h>
using namespace relacs;

namespace efield {


JAR::JAR( void )
  : RePro( "JAR", "JAR", "efield", "Jan Benda", "2.0", "Mar 09, 2010" ),
    P( 4, 2, true, Plot::Copy, this )
{
  // parameter:
  ReadCycles = 100;
  Duration = 10.0;
  Pause = 20.0;
  DeltaFStep = 2.0;
  DeltaFMax = 12.0;
  DeltaFMin = -12.0;
  ContrastStep = 0.1;
  ContrastMax = 0.2;
  Repeats = 200;
  After = 5.0;
  JARAverageTime = 0.5;
  ChirpAverageTime = 0.02;
  EODSaveTime = 1.0;
  SineWave = false;

  // add some parameter as options:
  addLabel( "Stimulation" );
  addNumber( "duration", "Signal duration", Duration, 1.0, 10000.0, 1.0, "seconds" );
  addNumber( "pause", "Pause between signals", Pause, 1.0, 10000.0, 1.0, "seconds" );
  addNumber( "deltafstep", "Delta f steps", DeltaFStep, 1.0, 10000.0, 1.0, "Hz" );
  addNumber( "deltafmax", "Maximum delta f", DeltaFMax, -10000.0, 10000.0, 2.0, "Hz" );
  addNumber( "deltafmin", "Minimum delta f", DeltaFMin, -10000.0, 10000.0, 2.0, "Hz" );
  addNumber( "contraststep", "Contrast steps", ContrastStep, 0.01, 1.0, 0.05, "1", "%", "%.0f" );
  addNumber( "contrastmax", "Maximum contrast", ContrastMax, 0.01, 1.0, 0.05, "1", "%", "%.0f" );
  addInteger( "repeats", "Repeats", Repeats, 0, 1000, 2 );
  addBoolean( "sinewave", "Use sine wave", SineWave );
  addLabel( "Analysis" );
  addNumber( "after", "Time after stimulation to be analyzed", After, 0.0, 10000.0, 1.0, "seconds", "ms" );
  addNumber( "jaraverage", "Time for measuring EOD rate", JARAverageTime, 0.01, 10000.0, 0.02, "seconds", "ms" );
  addNumber( "chirpaverage", "Time for measuring chirp data", ChirpAverageTime, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "eodsavetime", "Duration of EOD to be saved", EODSaveTime, 0.0, 10000.0, 0.01, "seconds", "ms" );
  addTypeStyle( OptWidget::Bold, Parameter::Label );
  
  // variables:
  FishAmplitude1 = 0.0;
  FishAmplitude2 = 0.0;
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
  //  P[0].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::X2Axis, Plot::Left );
  P[0].setXLabel( "sec" );
  //  P[0].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::Y1Axis, 
  //		     Plot::Left, 0.0 );
  P[0].setYRange( Plot::AutoScale, Plot::AutoScale );
  P[0].setYLabel( "Hz" );
  //  P[0].setYLabelPos( -0.55, Plot::Y1Axis, 0.5, Plot::Graph, 
  //		     Plot::Center, -90.0 );
  P[0].setLMarg( 5 );
  P[0].setRMarg( 1 );
  P[0].setTMarg( 3 );
  P[0].setBMarg( 4 );
  P[0].setOrigin( 0.0, 0.55 );
  P[0].setSize( 1.0, 0.45 );

  P[1].setXLabel( "Df/Hz" );
  //  P[1].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::Y1Axis, 
  //		     Plot::Left, 0.0 );
  P[1].setYFallBackRange( -10.0, 10.0 );
  P[1].setYRange( -10.0, 10.0 );
  P[1].setYLabel( "JAR" );
  //  P[1].setYLabelPos( -0.55, Plot::Y1Axis, 0.5, Plot::Graph, 
  //		     Plot::Center, -90.0 );
  P[1].setTMarg( 3 );
  P[1].setLMarg( 5 );
  P[1].setRMarg( 1 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 0.33, 0.55 );

  P[2].setXLabel( "Df/Hz" );
  //  P[2].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::Y1Axis, 
  //		     Plot::Left, 0.0 );
  P[2].setYRange( 0.0, 10.0 );
  P[2].setYLabel( "Chirps" );
  //  P[2].setYLabelPos( -0.55, Plot::Y1Axis, 0.5, Plot::Graph, 
  //		     Plot::Center, -90.0 );
  P[2].setLMarg( 5 );
  P[2].setRMarg( 1 );
  P[2].setTMarg( 3 );
  P[2].setOrigin( 0.33, 0.0 );
  P[2].setSize( 0.33, 0.55 );

  P[3].setXLabel( "ms" );
  //  P[3].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::Y1Axis, 
  //		     Plot::Left, 0.0 );
  P[3].setXRange( -40.0, 40.0 );
  P[3].setYRange( Plot::AutoScale, Plot::AutoScale );
  P[3].setYLabel( "Hz" );
  //  P[3].setYLabelPos( -0.55, Plot::Y1Axis, 0.5, Plot::Graph, 
  //		     Plot::Center, -90.0 );
  P[3].setLMarg( 5 );
  P[3].setRMarg( 1 );
  P[3].setTMarg( 3 );
  P[3].setOrigin( 0.67, 0.0 );
  P[3].setSize( 0.33, 0.55 );
}


int JAR::main( void )
{
  // get options:
  Duration = number( "duration" );
  Pause = number( "pause" );
  Repeats = integer( "repeats" );
  DeltaFStep = number( "deltafstep" );
  DeltaFMax = number( "deltafmax" );
  DeltaFMin = number( "deltafmin" );
  ContrastStep = number( "contraststep" );
  ContrastMax = number( "contrastmax" );
  After = number( "after" );
  JARAverageTime = number( "jaraverage" );
  ChirpAverageTime = number( "chirpaverage" );
  EODSaveTime = number( "eodsavetime" );
  SineWave = boolean( "sinewave" );
  if ( After > Pause )
    After = Pause;

  // plot trace:
  plotToggle( true, false, 1.0, 0.0 );

  // ranges:
  ContrastCount = 0;
  Contrasts.resize( int( ContrastMax/ContrastStep ) );
  for ( unsigned int i=0; i<Contrasts.size(); i++ )
    Contrasts[i] = (i+1)*ContrastStep;
  DeltaFRange.clear();
  DeltaFRange.set( DeltaFMin, DeltaFMax, DeltaFStep );
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
  P[0].setXRange( 0.0, Duration+After );
  P[1].setXRange( DeltaFRange.minValue() - 0.5 * DeltaFStep, 
		  DeltaFRange.maxValue() + 0.5 * DeltaFStep );
  P[2].setXRange( DeltaFRange.minValue() - 0.5 * DeltaFStep, 
		  DeltaFRange.maxValue() + 0.5 * DeltaFStep );

  // EOD rate:
  FishRate = events( LocalEODEvents[0] ).frequency( ReadCycles );
  if ( FishRate <= 0.0 ) {
    warning( "Not enough EOD cycles recorded!", 5.0 );
    return Failed;
  }

  // trigger:
  //  setupTrigger( data, events );

  // EOD amplitude:
  FishAmplitude1 = eodAmplitude( trace( EODTrace ), events( EODEvents ),
				 events( EODEvents ).back() - 0.5, events( EODEvents ).back() );
  FishAmplitude2 = eodAmplitude( trace( LocalEODTrace[0] ), events( LocalEODEvents[0] ),
				 events( LocalEODEvents[0] ).back() - 0.5, events( LocalEODEvents[0] ).back() );

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( trace( LocalEODTrace[0] ).currentTime()-0.1,
						  trace( LocalEODTrace[0] ).currentTime() );
  if ( val2 > 0.0 ) {
    adjustGain( trace( LocalEODTrace[0] ), ( 1.0 + ContrastMax + 0.1 ) * val2 );
    activateGains();
  }

  for ( Count = 0;
	(Repeats <= 0 || Count < Repeats ) && softStop() == 0;
	Count++ ) {
    for ( ContrastCount = 0;
	  ContrastCount < (int)Contrasts.size() && softStop() <= 1;
	  ContrastCount++ ) {
      for ( DeltaFRange.reset(); !DeltaFRange && softStop() <= 2; ++DeltaFRange ) {

	Contrast = Contrasts[ContrastCount];
	DeltaF = *DeltaFRange;
	
	// create signal:
	OutData signal;
	signal.setTrace( GlobalEField );
	applyOutTrace( signal );
	if ( SineWave ) {
	  StimulusRate = FishRate + DeltaF;
	  double p = rint( StimulusRate / fabs( DeltaF ) ) / StimulusRate;
	  int n = (int)::rint( Duration / p );
	  if ( n < 1 )
	    n = 1;
	  signal.sineWave( StimulusRate, n*p );
	  signal.setIdent( "sinewave" );
	  IntensityGain = 0.5;
	}
	else {
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
	  IntensityGain = 0.5 * maxamplitude / FishAmplitude2 / g;
	  signal.repeat( (int)floor( Duration/signal.duration() ) );
	}
	Duration = signal.length();
	signal.setStartSource( 1 );
	signal.setDelay( 0.01 );
	/*
	Str s = "C=" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%";
	s += ", Df=" + Str( DeltaF, 0, 1, 'f' ) + "Hz";
	signal.setIdent( s );
	*/
	
	// stimulus intensity:
	Intensity = Contrast * FishAmplitude2 * IntensityGain;
	signal.setIntensity( Intensity );
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
	Str s = "Contrast: <b>" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%</b>";
	s += "  Delta F:  <b>" + Str( DeltaF, 0, 1, 'f' ) + "Hz</b>";
	s += "  Loop:  <b>" + Str( Count+1 ) + "</b>";
	message( s );
	
	sleep( Duration + After );
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

	sleep( Pause - After );
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
  header.addNumber( "EOD Amplitude", FishAmplitude1, GlobalEODUnit, "%.2f" );
  header.addNumber( "Trans. Amplitude", FishAmplitude2, LocalEODUnit, "%.2f" );
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
  key.addNumber( "contrast", "%", "%8.1f" );
  key.addNumber( "deltaf", "Hz", "%5.1f" );
  key.addNumber( "index", "1", "%5.0f" );
  key.addLabel( "jar" );
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
	key.save( df, 100.0 * Contrasts[i], 0 );
	key.save( df, DeltaFRange.value( j ) );
	key.save( df, Response[i][j][k].Index );
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
  key.addNumber( "contrast", "%", "%8.1f" );
  key.addNumber( "deltaf", "Hz", "%6.1f" );
  key.addNumber( "n", "1", "%3.0f" );
  key.addLabel( "jar" );
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
	key.save( df, 100.0 * Contrasts[i], 0 );
	key.save( df, DeltaFRange.value( j ) );
	key.save( df, MeanResponse[i][j].NJar );
	// measured contrast:
	v = MeanResponse[i][j].Contrast;
	var = MeanResponse[i][j].ContrastSq - v * v;
	key.save( df, 100.0 * v );
	key.save( df, var > 0.0 ? 100.0 * sqrt( var ) : 0.0 );
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
  key.addNumber( "EOD", LocalEODUnit, "%.5g" );
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
  header.addNumber( "Contrast", 100.0*Contrast, "Hz", "%.1f" );
  header.addNumber( "Duration", Duration, "sec", "%.3f" );
  header.addNumber( "Pause", Pause, "sec", "%.3f" );
  header.addText( "Waveform", SineWave ? "Sine-Wave" : "Fish-EOD" );
  header.addNumber( "True Delta f", TrueDeltaF, "Hz", "%.1f" );
  header.addNumber( "True Contrast", 100.0*TrueContrast, "Hz", "%.1f" );
  header.addNumber( "EOD Rate before", FirstRate, "Hz", "%.1f" );
  header.addNumber( "EOD Rate at end", LastRate, "Hz", "%.1f" );
  header.addNumber( "JAR", LastRate-FirstRate, "Hz", "%.1f" );
  header.addNumber( "EOD Amplitude", FishAmplitude1, GlobalEODUnit, "%.2f" );
  header.addNumber( "Trans. Amplitude", FishAmplitude2, LocalEODUnit, "%.2f" );
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
  key.addNumber( "ampl", LocalEODUnit, "%6.4f" );
  key.addNumber( "beat", "1", "%5.3f" );
  key.saveKey( df );

  // write data into file:
  for ( int k=0;
	k<EODFrequency.size() && k<EODTransAmpl.size() && k<EODBeatPhase.size();
	k++ ) {
    key.save( df, EODFrequency.x(k), 0 );
    key.save( df, EODFrequency.y(k) );
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
  key.addNumber( "ampl", LocalEODUnit, "%6.4f" );
  key.addNumber( "adiff", LocalEODUnit, "%6.4f" );
  key.addNumber( "dip", "%", "%5.1f" );
  key.addNumber( "phase", "1", "%5.3f" );
  key.addLabel( "beat" );
  key.addNumber( "phase", "1", "%5.3f" );
  key.addNumber( "loc", "1", "%5.3f" );
  key.addNumber( "df", "Hz", "%5.1f" );
  key.addNumber( "ampl-", LocalEODUnit, "%6.4f" );
  key.addNumber( "ampl+", LocalEODUnit, "%6.4f" );
  key.addNumber( "jump", LocalEODUnit, "%7.4f" );
  key.addNumber( "rjump", "%", "%5.1f" );
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
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
      key.save( df, "" );
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
      key.save( df, Chirps[k].BeatBefore );
      key.save( df, Chirps[k].BeatAfter );
      key.save( df, Chirps[k].BeatAfter - Chirps[k].BeatBefore );
      key.save( df, 100.0 * ( Chirps[k].BeatAfter - Chirps[k].BeatBefore ) / ( Chirps[k].BeatAfter + Chirps[k].BeatBefore ) );
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
  key.addNumber( "ampl2", LocalEODUnit, "%6.4f" );
  key.addNumber( "beat", "1", "%5.3f" );
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
      int wi = (int)::rint( 0.7 * Chirps[k].Width * Chirps[k].EODRate );
      for ( ; j<EODFrequency.size(); j++ )
	if ( EODFrequency.x(j) >= Chirps[k].Time )
	  break;
      for ( int i = -wi; i < wi; i++ ) {
	if ( j+i >= 0 && j+i < EODFrequency.size() ) {
	  key.save( df, 1000.0*(EODFrequency.x(j+i) - Chirps[k].Time), 0 );
	  key.save( df, EODFrequency.y(j+i) );
	  key.save( df, EODAmplitude.y(j+i) );
	  key.save( df, EODPhases.y(j+i) );
	  key.save( df, EODTransAmpl.y(j+i) );
	  key.save( df, EODBeatPhase.y(j+i) );
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
    const EventData &eodev = events( EODEvents );
    for ( unsigned int k=0; k<Chirps.size(); k++ ) {
      double wt = 0.7 * Chirps[k].Width;
      double ect = eodev.nextTime( Chirps[k].Time + eodev.signalTime() );
      InDataIterator ft = trace( EODTrace ).begin( ect-wt );
      InDataIterator lt = trace( EODTrace ).begin( ect+wt );
      InDataTimeIterator ti = trace( EODTrace ).timeBegin( ect-wt );
      for ( InDataIterator i = ft; i < lt && !i; ++i, ++ti ) {
	key.save( df, 1000.0 * ( *ti - Chirps[k].Time - eodev.signalTime() ), 0 );
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
  s += ", Contrast = " + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%, ";
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
    int wi = (int)::rint( 0.7 * Chirps[k].Width * Chirps[k].EODRate );
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

  P.unlock();
  P.draw();
}


void JAR::analyze( void )
{
  const EventData &eodglobal = events( EODEvents );
  const EventData &eodlocal = events( LocalEODEvents[0] );
  const EventData &sige = events( GlobalEFieldEvents );
  double sigtime = eodglobal.signalTime();

  // EOD rate:
  FishRate = eodglobal.frequency( sigtime - JARAverageTime, sigtime );

  // Delta F:
  TrueDeltaF = sige.frequency( sigtime, sigtime +  JARAverageTime ) - FishRate;

  // EOD amplitude:
  FishAmplitude1 = eodAmplitude( trace( EODTrace ), eodglobal,
				 eodglobal.back() - JARAverageTime,
				 eodglobal.back() );
  FishAmplitude2 = eodAmplitude( trace( LocalEODTrace[0] ), eodlocal,
				 eodlocal.back() - JARAverageTime,
				 eodlocal.back() );

  // contrast:
  TrueContrast = beatContrast( trace( LocalEODTrace[0] ),
			       events( LocalBeatPeakEvents[0] ),
			       events( LocalBeatTroughEvents[0] ),
			       sigtime,
			       sigtime+Duration,
			       0.1*Duration );

  // mean rate before stimulus:
  FirstRate = eodglobal.frequency( sigtime - JARAverageTime,
				   sigtime );

  // mean rate at end of stimulus:
  LastRate = eodglobal.frequency( sigtime + Duration - JARAverageTime,
				  sigtime + Duration );

  // JAR:
  double jar = LastRate - FirstRate;

  // EOD trace:
  GlobalEODUnit = trace( EODTrace ).unit();
  LocalEODUnit = trace( LocalEODTrace[0] ).unit();

  EventIterator first1 = eodglobal.begin( sigtime );
  EventIterator last1 = eodglobal.begin( sigtime + Duration + After );
  if ( last1 >= eodglobal.end() - 2 )
    last1 = eodglobal.end() - 2;

  // EOD frequency:
  EventFrequencyIterator findex;
  EODFrequency.clear();
  EODFrequency.reserve( last1 - first1 + 2 );
  EODPhases.clear();
  EODPhases.reserve( last1 - first1 + 2 );
  for ( findex=first1; findex < last1; ++findex ) {
    EODFrequency.push( findex.time() - sigtime, *findex );
    EODPhases.push( findex.time() - sigtime, 0.0 );
  }

  // EOD amplitude:
  EventSizeIterator sindex;
  EODAmplitude.clear();
  EODAmplitude.reserve( last1 - first1 + 2 );
  for ( sindex=first1; sindex < last1; ++sindex ) {
    EODAmplitude.push( sindex.time() - sigtime, *sindex );
  }

  // EOD transdermal amplitude:
  EventIterator first2 = eodlocal.begin( sigtime );
  EventIterator last2 = eodlocal.begin( sigtime + Duration + After );
  EODTransAmpl.clear();
  EODTransAmpl.reserve( last2 - first2 + 2 );
  for ( sindex = first2; sindex < last2; ++sindex ) {
    EODTransAmpl.push( sindex.time() - sigtime, *sindex );
  }

  // Beat phase:
  EODBeatPhase.clear();
  EODBeatPhase.reserve( last1 - first1 + 2 );
  for ( EventIterator index = first1; index < last1; ++index ) {
    long ti = sige.next( *index );
    if ( ti >= sige.size() )
      break;
    double t1 = sige[ ti ];
    int pi = eodlocal.previous( t1 );
    double t0 = eodlocal[ pi ];
    double phase = ( t1 - t0 ) / ( eodlocal[ pi + 1 ] - t0 );
    EODBeatPhase.push( index.time() - sigtime, phase );
  }

  // store results:
  ResponseData d( FileIndex, FirstRate, LastRate, jar, TrueContrast );
  Response[ContrastCount][DeltaFRange.pos()].push_back( d );

  // Chirps:
  JARChirpEvents.assign( events( ChirpEvents ),
			 sigtime, sigtime + Duration + After );
  Chirps.clear();
  for ( int k=0; k < JARChirpEvents.size(); k++ ) {

    // time:
    double time = JARChirpEvents[k] - JARChirpEvents.signalTime();
    EventFrequencyIterator event = eodglobal.begin( JARChirpEvents[k] + sigtime );
    last1 = eodglobal.end() - 1;

    // size:
    double size = JARChirpEvents.eventSize( k );

    // width:
    double width = JARChirpEvents.eventWidth( k );

    // mean rate before chirp:
    double meanrate = eodglobal.frequency( JARChirpEvents[k] + sigtime - 0.7 * width - ChirpAverageTime,
					   JARChirpEvents[k] + sigtime - 0.7 * width );

    // current deltaf:
    double cdeltaf = StimulusRate - meanrate;

    // mean amplitude before chirp:
    double meanampl = eodglobal.meanSize( JARChirpEvents[k] + sigtime - 0.7 * width - ChirpAverageTime,
					  JARChirpEvents[k] + sigtime - 0.7 * width );

    // find maximum amplitude difference:
    EventSizeIterator sindex;
    double ampl = 0;
    for ( sindex = event - 0.7*width; sindex < event + 0.7*width; ++sindex )
      if ( ampl < fabs( *sindex - meanampl ) )
	ampl = fabs( *sindex - meanampl );

    // phase shift:
    // time of an eod zero crossing before the chirp:
    double eodtime = eodglobal.previousTime( event.time() - 0.7 * width );
    // mean EOD interval before the chirp:
    double meaninterv = 1.0 / meanrate;

    // time course of chirp phase shift:
    EventIterator gefi = event - 0.7*width - 10;
    double ophase = -0.3;
    for ( int j = gefi.index() - eodglobal.next( sigtime );
	  !gefi && gefi < event + 0.7*width + 10 && 
	    j < EODPhases.size();
	  ++gefi, ++j ) {
      double t = *gefi;
      EODPhases.x(j) = t - sigtime;
      double t0 = floor( ( t - eodtime ) / meaninterv ) * meaninterv + eodtime;
      double phase = ( t - t0 ) / meaninterv;
      phase -= rint( phase - ophase );
      EODPhases.y(j) = -phase;
      ophase = phase;
    } 

    // mean phase before:
    gefi = event - 0.7*width;
    int inx = gefi.index() - eodglobal.next( sigtime );
    double meanphasel = 0.0;
    for ( int j=0; j < 6 && inx-j >= 0 && inx-j < EODPhases.size(); j++ ) {
      meanphasel += ( EODPhases.y(inx-j) - meanphasel ) / (j+1);
      --gefi;
    }

    // mean phase after:
    gefi = event + 0.7*width;
    inx = gefi.index() - eodglobal.next( sigtime );
    double meanphaser = 0.0;
    for ( int j=0; j < 6 && inx+j >= 0 && inx+j < EODPhases.size(); j++ ) {
      meanphaser += ( EODPhases.y(inx+j) - meanphaser ) / (j+1);
      --gefi;
    }

    // mean chirp phase shift:
    double meanphase = meanphaser - meanphasel;

    // beat phase:
    double beatphase = 0.0;
    gefi = sige.begin( event.time() ) - 2;
    for ( int n=0; n < 4 && gefi < sige.end(); n++, ++gefi ) {
      double t1 = *gefi;
      double t0 = floor( ( t1 - eodtime ) / meaninterv ) * meaninterv + eodtime;
      double phase = ( t1 - t0 ) / meaninterv;
      beatphase += ( phase - beatphase ) / (n+1);
    }

    // beat location:
    double beatloc = cdeltaf <= 0.0 ? beatphase : 1.0 - beatphase;

    // beat amplitude right before chirp:
    double beatbefore = eodlocal.meanSize( JARChirpEvents[k] + sigtime - 0.6 * width - 4.0 * meaninterv,
					   JARChirpEvents[k] + sigtime - 0.6 * width );
    // beat amplitude right after chirp:
    double beatafter = eodlocal.meanSize( JARChirpEvents[k] + sigtime + 0.6 * width,
					  JARChirpEvents[k] + sigtime + 0.6 * width + 4.0 * meaninterv );

    // stor results:
    ChirpData d( time, size, width, meanrate, meanampl, ampl, meanphase, 
		 beatphase, beatloc, cdeltaf, beatbefore, beatafter );
    Chirps.push_back( d );

    Response[ContrastCount][DeltaFRange.pos()].back().addChirp( size, width, ampl/meanampl, meanphase );
    MeanResponse[ContrastCount][DeltaFRange.pos()].addChirp( size, width, ampl/meanampl, meanphase );
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
