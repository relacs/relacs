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
  JARAverageTime = 0.5;
  ChirpAverageTime = 0.02;
  SaveCycles = 200;
  SineWave = false;

  // add some parameter as options:
  addNumber( "duration", "Signal duration",
	     Duration, 1.0, 1000.0, 1.0, "seconds" );
  addNumber( "pause", "Pause between signals",
	      Pause, 1.0, 1000.0, 1.0, "seconds" );
  addNumber( "deltafstep", "Delta f steps", DeltaFStep, 1.0, 1000.0, 1.0, "Hz" );
  addNumber( "deltafmax", "Maximum delta f", DeltaFMax, -1000.0, 1000.0, 2.0, "Hz" );
  addNumber( "deltafmin", "Minimum delta f", DeltaFMin, -1000.0, 1000.0, 2.0, "Hz" );
  addNumber( "contraststep", "Contrast steps", ContrastStep, 0.01, 1.0, 0.05, "1", "%", "%.0f" );
  addNumber( "contrastmax", "Maximum contrast", ContrastMax, 0.01, 1.0, 0.05, "1", "%", "%.0f" );
  addInteger( "repeats", "Repeats", Repeats, 0, 1000, 2 );
  addNumber( "jaraverage", "Time for measuring EOD rate",
	      JARAverageTime, 0.01, 1000.0, 0.02, "seconds", "ms" );
  addNumber( "chirpaverage", "Time for measuring chirp data",
	      ChirpAverageTime, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addBoolean( "sinewave", "Use sine wave", SineWave );
  
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
  EOD1Events = 0;
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


void JAR::createSignal( const InData &data, const EventData &events )
{
  if ( Signal != 0 )
    delete Signal;

  Signal = new OutData;

  if ( SineWave ) {
    StimulusRate = FishRate + DeltaF;
    Signal->setSampleRate( 20.0 * StimulusRate );
    // get the actual set sampling rate.
    testWrite( *Signal );
    Signal->clearError();
    // create sine wave:
    Signal->sineWave( StimulusRate, ReadCycles * 1.0/StimulusRate );
    Signal->setIdent( "sinewave" );
    IntensityGain = 0.5;
  }
  else {
    // extract an EOD waveform:
    double t1 = events.back( ReadCycles );
    double t2 = events.back();
    data.copy( t1, t2, *Signal );
    double g = Signal->maximize( 0 );
    Signal->setSampleRate( data.sampleRate() * ( FishRate + DeltaF ) / FishRate );
    Signal->setCarrierFreq( FishRate + DeltaF );
    Signal->setIdent( "EOD" );
    StimulusRate = ReadCycles/Signal->duration();
    double maxamplitude = data.maxValue() - data.minValue();
    IntensityGain = 0.5 * maxamplitude / FishAmplitude2 / g;
  }
  Signal->repeat( (int)floor( Duration/Signal->duration() ) );
  //  Signal->setStartSource( 2 );
  Signal->setTrace( GlobalEField );
  Str s = "C=" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%";
  s += ", Df=" + Str( DeltaF, 0, 1, 'f' ) + "Hz";
  //  Signal->setIdent( s );
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
  JARAverageTime = number( "jaraverage" );
  ChirpAverageTime = number( "chirpaverage" );
  SineWave = boolean( "sinewave" );

  // plot trace:
  plotToggle( true, false, 1.0, 0.0 );

  // data:
  ContrastCount = 0;
  Contrasts.resize( int( ContrastMax/ContrastStep ) );
  for ( unsigned int i=0; i<Contrasts.size(); i++ )
    Contrasts[i] = (i+1)*ContrastStep;
  Contrast = Contrasts[ContrastCount];
  DeltaFRange.set( DeltaFMin, DeltaFMax, DeltaFStep );
  DeltaFRange.random();
  DeltaFRange.reset();
  DeltaF = *DeltaFRange;
  Intensity = 0.0;
  Count = 0;
  Response.resize( Contrasts.size() );
  for ( unsigned int i=0; i<Response.size(); i++ ) {
    Response[i].resize( DeltaFRange.size() );
    for ( unsigned int j=0; j<Response[i].size(); j++ ) {
      Response[i][j].clear();
      Response[i][j].reserve( Repeats );
    }
  }
  MeanResponse.resize( Contrasts.size() );
  for ( unsigned int i=0; i<MeanResponse.size(); i++ ) {
    MeanResponse[i].resize( DeltaFRange.size() );
  }
  EODFrequency.clear();
  EODAmplitude.clear();
  EODTransAmpl.clear();
  EODBeatPhase.clear();
  EODPhases.clear();
  JARChirpEvents.clear();
  Chirps.reserve( int( 100.0*(Duration+Pause) ) );

  // plot:
  P[0].setXRange( 0.0, Duration );
  P[1].setXRange( DeltaFRange.minValue() - 0.5 * DeltaFStep, 
		  DeltaFRange.maxValue() + 0.5 * DeltaFStep );
  P[2].setXRange( DeltaFRange.minValue() - 0.5 * DeltaFStep, 
		  DeltaFRange.maxValue() + 0.5 * DeltaFStep );

  // transdermal EOD:
  const EventData &eod2 = events( LocalEODEvents[0] );

  // EOD rate:
  FishRate = eod2.frequency( ReadCycles );
  if ( FishRate <= 0.0 ) {
    warning( "Not enough EOD cycles recorded!", 5.0 );
    stop();
    return Failed;
  }

  // trigger:
  //  setupTrigger( data, events );

  // EOD amplitude:
  FishAmplitude1 = eodAmplitude( trace( EODTrace ), events( EODEvents ),
				 events( EODEvents ).back() - 0.5, events( EODEvents ).back() );
  FishAmplitude2 = eodAmplitude( trace( LocalEODTrace[0] ), eod2,
				 eod2.back() - 0.5, eod2.back() );

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( trace( LocalEODTrace[0] ).currentTime()-0.1,
						  trace( LocalEODTrace[0] ).currentTime() );
  if ( val2 > 0.0 ) {
    adjustGain( trace( LocalEODTrace[0] ), ( 1.0 + ContrastMax + 0.1 ) * val2 );
    activateGains();
  }

  // create signal:
  createSignal( trace( LocalEODTrace[0] ), eod2 );

  // stimulus intensity:
  Intensity = Contrast * FishAmplitude2 * IntensityGain;
  Signal->setIntensity( Intensity );
  detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", 0.7*Signal->intensity() );

  // output signal:
  write( *Signal );
  if ( !Signal->success() ) {
    string s = "Output of stimulus failed!<br>Error code is <b>";
    s += Signal->errorText() + "</b>";
    warning( s );
    stop();
    return Failed;
  }

  // meassage: 
  Str s = "Contrast: <b>" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%</b>";
  s += "  Delta F:  <b>" + Str( DeltaF, 0, 1, 'f' ) + "Hz</b>";
  s += "  Loop:  <b>" + Str( Count+1 ) + "</b>";
  message( s );

  sleep( Duration + Pause );


  for ( ; ; ) {

    testWrite( *Signal );
    // signal failed?
    if ( !Signal->success() ) {
      if ( Signal->busy() ) {
	warning( "Output still busy!<br> Probably missing trigger.<br> Output of this signal software-triggered.", 4.0 );
	Signal->setStartSource( 0 );
	Signal->setPriority();
	write( *Signal );
	sleep( Signal->duration() + Pause );
	// trigger:
	// setupTrigger( data, events );
      }
      else if ( Signal->error() == Signal->OverflowUnderrun ) {
	warning( "Analog output overrun error!<br> Try again.", 4.0 );
	write( *Signal );
	sleep( Signal->duration() + Pause );
      }
      else {
	string s = "Output of stimulus failed!<br>Error code is <b>";
	s += Str( Signal->error() ) + ": " + Signal->errorStr() + "</b>";
	warning( s );
	stop();
	return Failed;
      }
    }

    // analyze:
    analyze();
    plot();
    saveEODFreq();
    saveChirps();
    saveChirpTraces();
    saveChirpEOD();
    FileIndex++;

    // keep track:
    ++DeltaFRange;
    if ( ! !DeltaFRange ) {
      DeltaFRange.reset();
      if ( softStop() ) {
	save();
	stop();
	return Completed;
      }
      ContrastCount++;
      if ( ContrastCount >= (int)Contrasts.size() ) {
	ContrastCount = 0;
	Count++;
	if ( Count >= Repeats ) {
	  Count = 0;
	  save();
	  stop();
	  return Completed;
	}
      }
    }
    Contrast = Contrasts[ContrastCount];
    DeltaF = *DeltaFRange;

    // create signal:
    createSignal( trace( LocalEODTrace[0] ), events( LocalEODEvents[0] ) );

    // stimulus intensity:
    Intensity = Contrast * FishAmplitude2 * IntensityGain;
    Signal->setIntensity( Intensity );
    detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", 0.7*Signal->intensity() );

    // output signal:
    write( *Signal );
    if ( !Signal->success() ) {
      string s = "Output of stimulus failed!<br>Error code is <b>";
      s += Str( Signal->error() ) + ": " + Signal->errorStr() + "</b>";
      warning( s, 2.0 );
      stop();
      return Failed;
    }

    // meassage: 
    Str s = "Contrast: <b>" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%</b>";
    s += "  Delta F:  <b>" + Str( DeltaF, 0, 1, 'f' ) + "Hz</b>";
    s += "  Loop:  <b>" + Str( Count+1 ) + "</b>";
    message( s );

    sleep( Duration + Pause );

  }


  return Completed;
}


void JAR::stop( void )
{
  P.clearPlots();
  delete Signal;
  Signal = 0;
  Contrasts.clear();
  DeltaFRange.clear();
  Response.clear();
  MeanResponse.clear();
  EODFrequency.clear();
  EODAmplitude.clear();
  EODTransAmpl.clear();
  EODBeatPhase.clear();
  EODPhases.clear();
  EOD1Events = 0;
  JARChirpEvents.clear();
  Chirps.clear();
  writeZero( 0 );
}


void JAR::save( void )
{
  Options header;
  header.addInteger( "Index", totalRuns()-1 );
  header.addText( "Waveform", SineWave ? "Sine-Wave" : "Fish-EOD" );
  header.addNumber( "EOD Rate", FishRate, "Hz", "%.1f" );
  header.addNumber( "EOD Amplitude", FishAmplitude1, EOD1Unit, "%.2f" );
  header.addNumber( "Trans. Amplitude", FishAmplitude2, EOD2Unit, "%.2f" );
  header.addText( "RePro Time", reproTimeStr() );
  header.addText( "Session Time", sessionTimeStr() );
  header.addLabel( "settings:" );

  saveJAR( header );
  saveMeanJAR( header );
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
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
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
	key.save( df, v );
	key.save( df, var > 0.0 ? sqrt( var ) : 0.0 );
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
  key.addNumber( "time", "s", "%10.5f" );
  key.addNumber( "EOD", EOD2Unit, "%7.4f" );
  key.saveKey( df );

  // write data into file:
  for ( int index=0; index < Signal->size(); ++index ) {
    key.save( df, Signal->pos( index ), 0 );
    key.save( df, (*Signal)[index] ); 
    df << '\n';
  }
  df << "\n\n";
}


void JAR::saveEODFreq( void )
{
  /*
  ofstream df( addPath( SineWave ? "jareodtracess.dat" : "jareodtraces.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  df << "#            Index: %d\n", FileIndex );
  df << "#          Delta f: %.1fHz\n", DeltaF );
  df << "#         Contrast: %.1f%%\n", 100.0 * Contrast );
  df << "#         Duration: %.3fsec\n", Duration );
  df << "#            Pause: %.3fsec\n", Pause );
  df << "#         Waveform: %s\n", SineWave ? "Sine-Wave" : "Fish-EOD" );
  df << "#     True Delta f: %.1fHz\n", TrueDeltaF );
  df << "#    True Contrast: %.1f%%\n", 100.0 * TrueContrast );
  df << "#  EOD Rate before: %.1fHz\n", FirstRate );
  df << "#  EOD Rate at end: %.1fHz\n", LastRate );
  df << "#              JAR: %.1fHz\n", LastRate - FirstRate );
  df << "#    EOD Amplitude: %.2f%s\n", FishAmplitude1, EOD1Unit.c_str() );
  df << "# Trans. Amplitude: %.2f%s\n", FishAmplitude2, EOD2Unit.c_str() );
  df << "#       RePro Time: %s\n", reproTimeStr().c_str() );
  df << "#     Session Time: %s\n", sessionTimeStr().c_str() );
  df << '\n';

  // write key:
  df << "#Key\n" );
  df << "# %-9s  %-5s  %-6s  %-5s\n",
	   "time", "freq", "ampl", "beat" );
  df << "# %-9s  %-5s  %-6s  %-5s\n",
	   "sec", "Hz", EOD2Unit.c_str(), "1" );
  df << "# %-9d  %-5d  %-6d  %-5d\n", 1, 2, 3, 4 );

  // write data into file:
  for ( int k=0;
	k<EODFrequency.size() && k<EODTransAmpl.size() && k<EODBeatPhase.size();
	k++ )
    df << "  %9.5f  %5.1f  %6.4f  %5.3f\n",
	     EODFrequency.x(k), EODFrequency.y(k),
	     EODTransAmpl.y(k), EODBeatPhase.y(k) );
  df << "\n\n";
  */
}


void JAR::saveChirps( void )
{
  /*
  ofstream df( addPath( SineWave ? "jarchirpss.dat" : "jarchirps.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  df << "#            Index: %d\n", FileIndex );
  df << "#          Delta f: %.1fHz\n", DeltaF );
  df << "#         Contrast: %.1f%%\n", 100.0 * Contrast );
  df << "#         Duration: %.3fsec\n", Duration );
  df << "#            Pause: %.3fsec\n", Pause );
  df << "#         Waveform: %s\n", SineWave ? "Sine-Wave" : "Fish-EOD" );
  df << "#     True Delta f: %.1fHz\n", TrueDeltaF );
  df << "#    True Contrast: %.1f%%\n", 100.0 * TrueContrast );
  df << "#  EOD Rate before: %.1fHz\n", FirstRate );
  df << "#  EOD Rate at end: %.1fHz\n", LastRate );
  df << "#              JAR: %.1fHz\n", LastRate - FirstRate );
  df << "#    EOD Amplitude: %.2f%s\n", FishAmplitude1, EOD1Unit.c_str() );
  df << "# Trans. Amplitude: %.2f%s\n", FishAmplitude2, EOD2Unit.c_str() );
  df << "#       RePro Time: %s\n", reproTimeStr().c_str() );
  df << "#     Session Time: %s\n", sessionTimeStr().c_str() );
  df << '\n';

  // write key:
  df << "#Key\n" );
  df << "# %-53s  %s\n", "chirp", "beat" );
  df << "# %-9s  %-5s  %-5s  %-6s  %-6s  %-5s  %-5s  %-5s  %-5s  %-5s  %-6s  %-6s  %-7s  %-5s\n",
	   "time", "size", "width", "ampl", "adiff", "dip", "phase", "phase", "loc", "df", "ampl-", "ampl+", "jump", "rjump" );
  df << "# %-9s  %-5s  %-5s  %-6s  %-6s  %-5s  %-5s  %-5s  %-5s  %-5s  %-6s  %-6s  %-7s  %-5s\n",
	   "sec", "Hz", "ms", EOD2Unit.c_str(), EOD2Unit.c_str(), "%",
	   "1", "1", "1", "Hz", EOD2Unit.c_str(), EOD2Unit.c_str(), EOD2Unit.c_str(), "%" );
  df << "# %-9d  %-5d  %-5d  %-6d  %-6d  %-5d  %-5d  %-5d  %-5d  %-5d  %-6d  %-6d  %-7d  %-5d\n",
	   1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 );

  // write data into file:
  if ( Chirps.size() == 0 ) {
    df << "  %9s  %5s  %5s  %6s  %6s  %5s  %5s  %5s  %5s  %5s  %6s  %6s  %7s  %5s\n",
	     "-0", "-0", "-0", "-0", "-0", "-0", "-0", "-0", "-0", "-0", "-0", "-0", "-0", "-0" );
  }
  else {
    for ( unsigned int k=0; k<Chirps.size(); k++ )
      df << "  %9.5f  %5.1f  %5.1f  %6.4f  %6.4f  %5.1f  %5.3f  %5.3f  %5.3f  %5.1f  %6.4f  %6.4f  %7.4f  %5.1f\n",
	       Chirps[k].Time, Chirps[k].Size, Chirps[k].Width, 
	       Chirps[k].EODAmpl, Chirps[k].Amplitude,
	       100.0 * Chirps[k].Amplitude / Chirps[k].EODAmpl,
	       Chirps[k].Phase, 
	       Chirps[k].BeatPhase, Chirps[k].BeatLoc, Chirps[k].Deltaf,
	       Chirps[k].BeatBefore, Chirps[k].BeatAfter,
	       Chirps[k].BeatAfter - Chirps[k].BeatBefore,
	       100.0 * ( Chirps[k].BeatAfter - Chirps[k].BeatBefore ) / ( Chirps[k].BeatAfter + Chirps[k].BeatBefore ) );
  }
  df << "\n\n";
  */
}


void JAR::saveChirpTraces( void )
{
  /*
  ofstream df( addPath( SineWave ? "jarchirptracess.dat" : "jarchirptraces.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  df << "#            Index: %d\n", FileIndex );
  df << "#          Delta f: %.1fHz\n", DeltaF );
  df << "#         Contrast: %.1f%%\n", 100.0 * Contrast );
  df << "#         Duration: %.3fsec\n", Duration );
  df << "#            Pause: %.3fsec\n", Pause );
  df << "#         Waveform: %s\n", SineWave ? "Sine-Wave" : "Fish-EOD" );
  df << "#     True Delta f: %.1fHz\n", TrueDeltaF );
  df << "#    True Contrast: %.1f%%\n", 100.0 * TrueContrast );
  df << "#  EOD Rate before: %.1fHz\n", FirstRate );
  df << "#  EOD Rate at end: %.1fHz\n", LastRate );
  df << "#              JAR: %.1fHz\n", LastRate - FirstRate );
  df << "#    EOD Amplitude: %.2f%s\n", FishAmplitude1, EOD1Unit.c_str() );
  df << "# Trans. Amplitude: %.2f%s\n", FishAmplitude2, EOD2Unit.c_str() );
  df << "#       RePro Time: %s\n", reproTimeStr().c_str() );
  df << "#     Session Time: %s\n", sessionTimeStr().c_str() );
  df << '\n';

  // write key:
  df << "#Key\n" );
  df << "# %-7s  %-5s  %-6s  %-5s  %-6s  %-5s\n", 
	   "time", "size", "ampl", "phase", "ampl2", "beat" );
  df << "# %-7s  %-5s  %-6s  %-5s  %-6s  %-5s\n", 
	   "sec", "Hz", EOD1Unit.c_str(), "1", EOD2Unit.c_str(), "1" );
  df << "# %-7d  %-5d  %-6d  %-5d  %-6d  %-5d\n", 1, 2, 3, 4, 5, 6 );

  // write data into file:
  if ( Chirps.size() == 0 ) {
    df << "  %7s  %5s  %6s  %5s  %6s  %5s\n",
	     "-0", "-0", "-0", "-0", "-0", "-0" );
  }
  else {
    int j = 0;
    for ( unsigned int k=0; k<Chirps.size(); k++ ) {
      for ( ; j<EODFrequency.size(); j++ )
	if ( EODFrequency.x(j) >= Chirps[k].Time )
	  break;
      for ( int i = -SaveCycles+1; i < SaveCycles; i++ ) {
	if ( j+i >= 0 && j+i < EODFrequency.size() ) {
	  df << "  %7.2f  %5.1f  %6.4f  %5.3f  %6.4f  %5.3f\n",
		   1000.0*(EODFrequency.x(j+i) - Chirps[k].Time),
		   EODFrequency.y(j+i), EODAmplitude.y(j+i), 
		   EODPhases.y(j+i), EODTransAmpl.y(j+i),
		   EODBeatPhase.y(j+i) );
	}
      }
      df << '\n';
    }
  }
  df << "\n\n";
  */
}


void JAR::saveChirpEOD( void )
{
  /*
  ofstream df( addPath( SineWave ? "jarchirpeods.dat" : "jarchirpeod.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  df << "#            Index: %d\n", FileIndex );
  df << "#          Delta f: %.1fHz\n", DeltaF );
  df << "#         Contrast: %.1f%%\n", 100.0 * Contrast );
  df << "#         Duration: %.3fsec\n", Duration );
  df << "#            Pause: %.3fsec\n", Pause );
  df << "#         Waveform: %s\n", SineWave ? "Sine-Wave" : "Fish-EOD" );
  df << "#     True Delta f: %.1fHz\n", TrueDeltaF );
  df << "#    True Contrast: %.1f%%\n", 100.0 * TrueContrast );
  df << "#  EOD Rate before: %.1fHz\n", FirstRate );
  df << "#  EOD Rate at end: %.1fHz\n", LastRate );
  df << "#              JAR: %.1fHz\n", LastRate - FirstRate );
  df << "#    EOD Amplitude: %.2f%s\n", FishAmplitude1, EOD1Unit.c_str() );
  df << "# Trans. Amplitude: %.2f%s\n", FishAmplitude2, EOD2Unit.c_str() );
  df << "#       RePro Time: %s\n", reproTimeStr().c_str() );
  df << "#     Session Time: %s\n", sessionTimeStr().c_str() );
  df << '\n';

  // write key:
  df << "#Key\n" );
  df << "# %-6s  %-5s\n", "time", "EOD" );
  df << "# %-6s  %-5s\n", "ms", trace( EODTrace ).unit().c_str() );

  // write data into file:
  if ( Chirps.size() == 0 ) {
    df << "  %6s  %7s\n", "-0", "-0" );
  }
  else {
    for ( long k=0; k<JARChirpEvents.size(); k++ ) {
      long ee = EOD1Events->next( JARChirpEvents[k] );
      InDataIterator f = trace( EODTrace ).begin( (*EOD1Events)[ ee - SaveCycles/4 ] );
      InDataIterator l = trace( EODTrace ).begin( (*EOD1Events)[ ee + SaveCycles/4 ] );
      for ( InDataIterator i = f; i < l && !i; ++i ) {
	df << "  %6.2f  %7.4f\n",
		 1000.0 * ( i.time() - JARChirpEvents[k] ), *i );
      }
      df << '\n';
    }
  }
  df << "\n\n";
  */
}


void JAR::plot( void )
{
  // eod frequency with chirp events:
  P[0].clear();
  Str s;
  s = "Delta f = " + Str( TrueDeltaF, 0, 0, 'f' ) + "Hz";
  s += ", Contrast = " + Str( 100.0 * TrueContrast, 0, 0, 'f' ) + "%, ";
  s += SineWave ? "Sine Wave" : "Fish EOD";
  P[0].setTitle( s );
  P[0].plot( EODFrequency, 1.0, Plot::Green, 2, Plot::Solid );
  P[0].plot( JARChirpEvents, 2, 0.0, 1.0, 0.9, Plot::Graph, 
	     1, Plot::Circle, 5, Plot::Pixel, Plot::Yellow, Plot::Yellow );

  P[1].clear();
  P[1].plotHLine( 0.0 );
  MapD m;
  m.reserve( Repeats * DeltaFRange.size() );
  // jars:
  for ( unsigned int j=0; j<Response[ContrastCount].size(); j++ )
    for ( unsigned int k=0; k<Response[ContrastCount][j].size(); k++ ) {
      m.push( DeltaFRange.value( j ),
	      Response[ContrastCount][j][k].Jar );
    }
  P[1].plot( m, 1.0, Plot::Yellow, 0, Plot::Solid, Plot::Circle, 6, Plot::Yellow, Plot::Yellow );
  // current jar:
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
    // chirp frequency:
    cm.clear();
    cm.reserve( 2*SaveCycles+10 );
    for ( int i = -SaveCycles; i<SaveCycles; i++ ) {
      if ( j+i >= 0 && j+i < EODFrequency.size() ) {
	cm.push( 1000.0*(EODFrequency.x(j+i) - Chirps[k].Time),
		 EODFrequency.y(j+i) - LastRate );
      }
    }
    P[3].plot( cm, 1.0, Plot::Yellow, 2, Plot::Solid );
    // chirp phase:
    cm.clear();
    cm.reserve( 2*SaveCycles+10 );
    for ( int i = -SaveCycles; i<SaveCycles; i++ ) {
      if ( j+i >= 0 && j+i < EODPhases.size() ) {
	cm.push( 1000.0*(EODPhases.x(j+i) - Chirps[k].Time),
		 40.0 * EODPhases.y(j+i) );
      }
    }
    P[3].plot( cm, 1.0, Plot::Red, 2, Plot::Solid );
  }

  P.draw();
}


void JAR::analyze( void )
{
  const EventData &eod1 = events( EODEvents );
  const EventData &eod2 = events( LocalEODEvents[0] );
  const EventData &sige = events( GlobalEFieldEvents );
  EOD1Events = &eod1;

  // EOD rate:
  FishRate = eod1.frequency( eod1.signalTime() - 0.5, eod1.signalTime() );

  // Delta F:
  TrueDeltaF = sige.frequency( ReadCycles ) - FishRate;

  // EOD amplitude:
  FishAmplitude1 = eodAmplitude( trace( EODTrace ), eod1,
				 eod1.back() - 0.5, eod1.back() );
  FishAmplitude2 = eodAmplitude( trace( LocalEODTrace[0] ), eod2,
				 eod2.back() - 0.5, eod2.back() );

  // contrast:
  TrueContrast = beatContrast( trace( LocalEODTrace[0] ), events( LocalBeatPeakEvents[0] ),
			       events( LocalBeatTroughEvents[0] ),
			       eod2.signalTime(), eod2.signalTime()+Duration, 0.1*Duration );

  // mean rate before stimulus:
  FirstRate = eod1.frequency( eod1.signalTime() - JARAverageTime, eod1.signalTime() );

  // mean rate at end of stimulus:
  LastRate = eod1.frequency( eod1.signalTime() + Signal->duration() - JARAverageTime,
			     eod1.signalTime() + Signal->duration() );

  // JAR:
  double jar = LastRate - FirstRate;

  // EOD trace:
  EOD1Unit = trace( EODTrace ).unit();
  EOD2Unit = trace( LocalEODTrace[0] ).unit();

  EventIterator first1 = eod1.begin( eod1.signalTime() );
  EventIterator last1 = eod1.begin( eod1.signalTime() + 1.5 * Duration );
  if ( last1 >= eod1.end() )
    last1 = eod1.end() - 2;

  // EOD frequency:
  EventFrequencyIterator findex;
  EODFrequency.clear();
  EODFrequency.reserve( last1 - first1 + 2 );
  EODPhases.clear();
  EODPhases.reserve( last1 - first1 + 2 );
  for ( findex=first1; findex < last1; ++findex ) {
    EODFrequency.push( findex.time() - eod1.signalTime(), *findex );
    EODPhases.push( findex.time() - eod1.signalTime(), 0.0 );
  }

  // EOD amplitude:
  EventSizeIterator sindex;
  EODAmplitude.clear();
  EODAmplitude.reserve( last1 - first1 + 2 );
  for ( sindex=first1; sindex < last1; ++sindex ) {
    EODAmplitude.push( sindex.time() - eod1.signalTime(), *sindex );
  }

  // EOD transdermal amplitude:
  EventIterator first2 = eod2.begin( eod2.signalTime() );
  EventIterator last2 = eod2.end() - 2; 
  EODTransAmpl.clear();
  EODTransAmpl.reserve( last2 - first2 + 2 );
  for ( sindex = first2; sindex < last2; ++sindex ) {
    EODTransAmpl.push( sindex.time() - eod2.signalTime(), *sindex );
  }

  // Beat phase:
  EODBeatPhase.clear();
  EODBeatPhase.reserve( last1 - first1 + 2 );
  for ( EventIterator index = first1; index < last1; ++index ) {
    long ti = sige.next( *index );
    if ( ti >= sige.size() )
      break;
    double t1 = sige[ ti ];
    int pi = eod2.previous( t1 );
    double t0 = eod2[ pi ];
    double phase = ( t1 - t0 ) / ( eod2[ pi + 1 ] - t0 );
    EODBeatPhase.push( index.time() - eod1.signalTime(), phase );
  }

  // store results:
  ResponseData d( FileIndex, FirstRate, LastRate, jar, TrueContrast );
  Response[ContrastCount][DeltaFRange.pos()].push_back( d );

  // Chirps:
  JARChirpEvents.assign( events( ChirpEvents ),
			 events( ChirpEvents ).signalTime(),
			 events( ChirpEvents ).signalTime() + Duration );
  Chirps.clear();
  for ( int k=0; k < JARChirpEvents.size(); k++ ) {

    // time:
    double time = JARChirpEvents[k] - JARChirpEvents.signalTime();
    EventFrequencyIterator event = eod1.begin( JARChirpEvents[k] );
    last1 = eod1.end() - 1;

    // size:
    double size = JARChirpEvents.eventSize( k );

    // width:
    double width = JARChirpEvents.eventWidth( k );

    // mean rate before chirp:
    double meanrate = eod1.frequency( JARChirpEvents[k] - 0.7 * width - ChirpAverageTime,
				      JARChirpEvents[k] - 0.7 * width );

    // current deltaf:
    double cdeltaf = StimulusRate - meanrate;

    // mean amplitude before chirp:
    double meanampl = eod1.meanSize( JARChirpEvents[k] - 0.7 * width - ChirpAverageTime,
				     JARChirpEvents[k] - 0.7 * width );

    // find maximum amplitude difference:
    EventSizeIterator sindex;
    double ampl = 0;
    for ( sindex = event - 0.7*width; sindex < event + 0.7*width; ++sindex )
      if ( ampl < fabs( *sindex - meanampl ) )
	ampl = fabs( *sindex - meanampl );

    // phase shift:
    // time of an eod zero crossing before the chirp:
    double eodtime = eod1.previousTime( event.time() - 0.7 * width );
    // mean EOD interval before the chirp:
    double meaninterv = 1.0 / meanrate;

    // time course of chirp phase shift:
    EventIterator index = event - SaveCycles;
    double ophase = -0.3;
    for ( int j = index.index() - eod1.next( eod1.signalTime() );
	  !index && index < event + SaveCycles && 
	    j < EODPhases.size();
	  ++index, ++j ) {
      double t = *index;
      EODPhases.x(j) = t - eod1.signalTime();
      double t0 = floor( ( t - eodtime ) / meaninterv ) * meaninterv + eodtime;
      double phase = ( t - t0 ) / meaninterv;
      phase -= rint( phase - ophase );
      EODPhases.y(j) = -phase;
      ophase = phase;
    } 

    // mean phase before:
    index = event - 0.7*width;
    int inx = index.index() - eod1.next( eod1.signalTime() );
    double meanphasel = 0.0;
    for ( int j=0; j < 6 && inx-j >= 0 && inx-j < EODPhases.size(); j++ ) {
      meanphasel += ( EODPhases.y(inx-j) - meanphasel ) / (j+1);
      --index;
    }

    // mean phase after:
    index = event + 0.7*width;
    inx = index.index() - eod1.next( eod1.signalTime() );
    double meanphaser = 0.0;
    for ( int j=0; j < 6 && inx+j >= 0 && inx+j < EODPhases.size(); j++ ) {
      meanphaser += ( EODPhases.y(inx+j) - meanphaser ) / (j+1);
      --index;
    }

    // mean chirp phase shift:
    double meanphase = meanphaser - meanphasel;

    // beat phase:
    double beatphase = 0.0;
    index = sige.begin( event.time() ) - 2;
    for ( int n=0; n < 4 && index < sige.end(); n++, ++index ) {
      double t1 = *index;
      double t0 = floor( ( t1 - eodtime ) / meaninterv ) * meaninterv + eodtime;
      double phase = ( t1 - t0 ) / meaninterv;
      beatphase += ( phase - beatphase ) / (n+1);
    }

    // beat location:
    double beatloc = cdeltaf <= 0.0 ? beatphase : 1.0 - beatphase;

    // beat amplitude right before chirp:
    double beatbefore = eod2.meanSize( JARChirpEvents[k] - 0.6 * width - 4.0 * meaninterv,
				       JARChirpEvents[k] - 0.6 * width );
    // beat amplitude right after chirp:
    double beatafter = eod2.meanSize( JARChirpEvents[k] + 0.6 * width,
				      JARChirpEvents[k] + 0.6 * width + 4.0 * meaninterv );

    // stor results:
    ChirpData d( time, size, 1000.0*width, meanampl, ampl, meanphase, 
		 beatphase, beatloc, cdeltaf, beatbefore, beatafter );
    Chirps.push_back( d );

    Response[ContrastCount][DeltaFRange.pos()].back().addChirp( size, 1000.0*width, ampl/meanampl, meanphase );
    MeanResponse[ContrastCount][DeltaFRange.pos()].addChirp( size, 1000.0*width, ampl/meanampl, meanphase );
  }

  // store results:
  MeanResponse[ContrastCount][DeltaFRange.pos()].addJAR( TrueContrast,
							 FirstRate, LastRate,
							 jar, Chirps.size() );

}


addRePro( JAR );

}; /* namespace efield */

#include "moc_jar.cc"
