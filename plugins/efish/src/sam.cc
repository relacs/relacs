/*
  efish/sam.cc
  Measures responses to sinusoidal amplitude modulations.

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

#include <cmath>
#include <fstream>
#include <iomanip>
#include <relacs/detector.h>
#include <relacs/stats.h>
#include <relacs/str.h>
#include <relacs/optwidget.h>
#include <relacs/efish/sam.h>
using namespace relacs;

namespace efish {


SAM::SAM( void )
  : RePro( "SAM", "SAM", "efish",
	   "Jan Benda", "2.0", "Dec 01, 2009" ),
    P( this )
{
  // parameter:
  ReadCycles = 100;
  Duration = 1.0;
  Pause = 1.0;
  FreqAbs = false;
  DeltaF = 5.0;
  Contrast = 0.2;
  Repeats = 6;
  AM = true;
  SineWave = true;
  Skip = 0.5;
  RateN = 10;
  Before=0.0;
  After=0.0;

  // add some parameter as options:
  addLabel( "Stimulus" );
  addNumber( "duration", "Duration of signal",
	      Duration, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "pause", "Pause between signals",
	      Pause, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addSelection( "freqsel", "Stimulus frequency is", "relative to EOD|absolute" );
  addNumber( "deltaf", "Delta f (beat frequency)", DeltaF, -1000.0, 1000.0, 5.0, "Hz" );
  addNumber( "contrast", "Contrast", Contrast, 0.01, 1.0, 0.05, "", "%" );
  addInteger( "repeats", "Repeats", Repeats, 0, 100000, 2 );
  addBoolean( "am", "Amplitude modulation", AM ).setActivation( "freqsel", "relative to EOD" );
  addBoolean( "sinewave", "Use sine wave", SineWave );
  addLabel( "Analysis" );
  addNumber( "skip", "Skip", Skip, 0.0, 100.0, 0.1, "Periods" );
  addInteger( "ratebins", "Number of bins for firing rate", RateN, 2, 1000 );
  addNumber( "before", "Spikes recorded before stimulus",
	      Before, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addNumber( "after", "Spikes recorded after stimulus",
	      After, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addTypeStyle( OptWidget::Bold, Parameter::Label );
  
  // variables:
  Signal = 0;
  FishAmplitude = 0.0;
  Intensity = 0.0;
  EODTransAmpl.clear();
  AllEODTransAmpl.clear();
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    Spikes[k].clear();
    AllSpikes[k].clear();
    SpikeRate[k] = 0;
    Trials[k] = 0; 
  }
  Offset = 0;
  NerveAmplP.clear();
  NerveAmplT.clear();
  NerveAmplM.clear();
  AllNerveAmplP.clear();
  AllNerveAmplT.clear();
  AllNerveAmplM.clear();
  NerveMeanAmplP.clear();
  NerveMeanAmplT.clear();
  NerveMeanAmplM.clear();
  RateDeltaT = 0.005;   // seconds

  // header for files:
  Header.addInteger( "index" );
  Header.addInteger( "trace" );
  Header.addText( "waveform" );
  Header.addNumber( "true deltaf", "Hz", "%.1f" );
  Header.addNumber( "true contrast", "%", "%.1f" );
  Header.addNumber( "EOD rate", "Hz", "%.1f" );
  Header.addNumber( "trans. amplitude", "", "%.2f" );
  Header.addText( "session time" );
  Header.addLabel( "settings:" );

  // tablekeys:
  AmplKey.addNumber( "time", "ms", "%9.2f" );
  AmplKey.addNumber( "ampl", EOD2Unit, "%5.3f" );

  SpikesKey.addNumber( "time", "ms", "%9.2f" );

  NerveKey.addLabel( "peak" );
  NerveKey.addNumber( "time", "ms", "%9.2f" );
  NerveKey.addNumber( "ampl", "uV", "%6.1f" );
  NerveKey.addLabel( "trough" );
  NerveKey.addNumber( "time", "ms", "%9.2f" );
  NerveKey.addNumber( "ampl", "uV", "%6.1f" );
  NerveKey.addLabel( "average" );
  NerveKey.addNumber( "time", "ms", "%9.2f" );
  NerveKey.addNumber( "ampl", "uV", "%6.1f" );
}


int SAM::createSignal( const InData &data, const EventData &events )
{
  if ( Signal != 0 )
    delete Signal;

  Signal = new OutData;
  Signal->setTrace( AM ? GlobalAMEField : GlobalEField );
  applyOutTrace( *Signal );
  string ident = "";

  if ( AM ) {
    if ( SineWave ) {
      DeltaF = fabs( DeltaF );
      int n = (int)::rint( Duration * DeltaF );
      if ( n < 1 )
	n = 1;
      Signal->sineWave( DeltaF, n/DeltaF );
      ident = "SAM";
      IntensityGain = 0.5;
      TrueDeltaF = 1.0 / Signal->duration();
    }
    else {
      warning( "Non-Sinewave as AM not supported yet!" );
      return 1;
    }
  }
  else {
    if ( SineWave ) {
      double stimulusrate = DeltaF;
      if ( ! FreqAbs )
	stimulusrate += FishRate;
      double p = rint( stimulusrate / fabs( DeltaF ) ) / stimulusrate;
      int n = (int)::rint( Duration / p );
      if ( n < 1 )
	n = 1;
      Signal->sineWave( stimulusrate, n*p );
      ident = "sinewave";
      IntensityGain = 0.5;
    }
    else {
      // extract an EOD waveform:
      double t1 = events.back( ReadCycles+1 );
      double t2 = events.back( 1 );
      data.copy( t1, t2, *Signal );
      Signal->maximize( 0.5 );
      Signal->setSampleRate( data.sampleRate() * ( FishRate + DeltaF ) / FishRate );
      Signal->setCarrierFreq( FishRate + DeltaF );
      ident = "EOD";
      double maxamplitude = Signal->maxValue() - Signal->minValue();
      IntensityGain = 0.5 * maxamplitude;
    }
    Signal->repeat( (int)rint( Duration/Signal->duration() ) );
  }
  Duration = Signal->duration();
  Signal->setStartSource( 1 );
  Str s = ident + ", C=" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%";
  s += ", Df=" + Str( DeltaF, 0, 1, 'f' ) + "Hz";
  if ( AM )
    s += ", AM";
  Signal->setIdent( s );
  Signal->clearError();
  return 0;
}


int SAM::main( void )
{
  // get options:
  Duration = number( "duration" );
  Pause = number( "pause" );
  Repeats = integer( "repeats" );
  FreqAbs = ( index( "freqsel" ) == 1 );
  DeltaF = number( "deltaf" );
  Contrast = number( "contrast" );
  SineWave = boolean( "sinewave" );
  AM = boolean( "am" );
  Skip = number( "skip" );
  RateN = integer( "ratebins" );
  Before = number( "before" );
  After = number( "after" );

  if ( FreqAbs && DeltaF <= 0.0 ) {
    warning( "Delta f cannot be negative for absolute stimulus frequencies!\n" );
    return Failed;
  }
  if ( FreqAbs && AM )
    AM = false;

  // check EODs:
  if ( LocalEODTrace[0] < 0 || LocalEODEvents[0] < 0 ||
       LocalBeatPeakEvents[0] < 0 || LocalBeatTroughEvents[0] < 0) {
    warning( "Local EOD recording with EOD and beat events required!" );
    return Failed;
  }

  // check for SignalEvents:
  if ( !AM && GlobalEFieldEvents < 0 ) {
    warning( "Need stimulus recording for non AM-stimuli!" );
    return Failed;
  }

  // data:
  Intensity = 0.0;
  Period = fabs( 1.0/DeltaF );
  int beats = int( 2.0 * Duration * fabs( DeltaF ) );
  EODTransAmpl.reserve( beats );
  AllEODTransAmpl.reserve( int( 2000.0 * Duration ) );
  RateDeltaT = Period / RateN;
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Spikes[k].clear();
      Spikes[k].reserve( beats );
      AllSpikes[k].clear();
      SpikeRate[k] = new SampleDataD( -0.5*Period, 0.5*Period+RateDeltaT, RateDeltaT );
      Trials[k] = 0;
      MaxRate[k] = 20.0;
    }
  }
  if ( NerveTrace[0] >= 0 ) {
    Offset = 0;
    NerveAmplP.reserve( beats );
    NerveAmplT.reserve( beats );
    NerveAmplM.reserve( beats );
    AllNerveAmplP.reserve( (int)rint(1500.0*Duration) );
    AllNerveAmplT.reserve( (int)rint(1500.0*Duration) );
    AllNerveAmplM.reserve( (int)rint(1500.0*Duration) );
    NerveMeanAmplP = SampleDataD( -0.5*Period, 0.5*Period, 0.0005 );
    NerveMeanAmplT = SampleDataD( -0.5*Period, 0.5*Period, 0.0005 );
    NerveMeanAmplM = SampleDataD( -0.5*Period, 0.5*Period, 0.0005 );
  }

  // plot trace:
  plotToggle( true, true, Before+Duration+After, Before );
  
  // plot:
  P.lock();
  P.resize( (SpikeTraces+NerveTraces)*2, 2, false, Plot::Pointer );
  int n=0;
  for ( int k=0; k<MaxSpikeTraces + NerveTraces; k++ ) {
    if ( ( k<MaxSpikeTraces && SpikeEvents[k] >= 0 ) ||
	 ( k>=MaxSpikeTraces && NerveTrace[0] >= 0 ) ) {
      P[2*n].clear();
      P[2*n].setLMarg( 8.0 );
      P[2*n].setRMarg( 1.5 );
      P[2*n].setBMarg( 1.0 );
      P[2*n].setTMarg( 1.0 );
      P[2*n].setXLabel( "" );
      P[2*n].noXTics();
      P[2*n].setXRange( -500.0*Period, 500.0*Period );
      if ( k<MaxSpikeTraces ) { 
	P[2*n].setYLabel( "Firing Rate " + Str( k+1 ) + " [Hz]" );
	P[2*n].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
			     Plot::Center, -90.0 );
	P[2*n].setYRange( 0.0, 100.0 );
	P[2*n].setYTics( );
      }
      else {
	P[2*n].setYLabel( "Potential [mV]" );
	P[2*n].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
			     Plot::Center, -90.0 );
	P[2*n].setAutoScaleY();
	P[2*n].setYTics( );
      }

      P[2*n+1].clear();
      P[2*n+1].setLMarg( 8.0 );
      P[2*n+1].setRMarg( 1.5 );
      P[2*n+1].setBMarg( 3.0 );
      P[2*n+1].setTMarg( 0.2 );

      P[2*n+1].setXRange( -500.0*Period, 500.0*Period );
      P[2*n+1].setXLabel( "ms" );
      P[2*n+1].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::FirstAxis, 
			 Plot::Left, 0.0 );
      P[2*n+1].setXTics();
      P[2*n+1].setYLabel( "SAM" );
      P[2*n+1].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
			 Plot::Center, -90.0 );
      P[2*n+1].setAutoScaleY();
      P[2*n+1].setYTics( );

      n++;
    }
  }
  P.unlock();

  // EOD rate:
  FishRate = 0.0;
  if ( EODEvents >= 0 )
    FishRate = events( EODEvents ).frequency( ReadCycles );
  else if ( LocalEODEvents[0] >= 0 )
    FishRate = events( LocalEODEvents[0] ).frequency( ReadCycles );
  if ( FishRate <= 0.0 ) {
    warning( "No EOD present or not enough EOD cycles recorded!" );
    return Failed;
  }

  // trigger:
  // XXX  setupTrigger( traces(), events() );

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ), events( LocalEODEvents[0] ),
				events( LocalEODEvents[0] ).back() - 0.5, events( LocalEODEvents[0] ).back() );
  if ( FishAmplitude <= 1.0e-8 ) {
    warning( "No EOD amplitude on local EOD electrode!" );
    return Failed;
  }

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( trace( LocalEODTrace[0] ).currentTime()-0.1,
						  trace( LocalEODTrace[0] ).currentTime() );
  if ( val2 > 0.0 )
    adjustGain( trace( LocalEODTrace[0] ), ( 1.0 + Contrast ) * val2 );

  // create signal:
  if ( createSignal( trace( LocalEODTrace[0] ), events( LocalEODEvents[0] ) ) )
    return Failed;

  // clear output lines:
  writeZero( AM ? GlobalEField : GlobalAMEField );
  sleep( 0.01 );

  timeStamp();

  for ( Count=0;
	( Repeats <= 0 || Count < Repeats ) && softStop() == 0; 
	Count++ ) {

    // stimulus intensity:
    Intensity = Contrast * FishAmplitude * IntensityGain;
    Signal->setIntensity( Intensity );
    detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", 1.5*Signal->intensity() );

    // output signal:
    write( *Signal );
    if ( !Signal->success() ) {
      string s = "Output of stimulus failed!<br>Error code is <b>";
      s += Signal->errorText() + "</b>";
      warning( s, 2.0 );
      return Failed;
    }

    // meassage: 
    Str s = AM ? "SAM" : ( FreqAbs ? "Direct" : "EOD" );
      s += ":  Contrast: <b>" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%</b>";
    s += "  Delta F: <b>" + Str( DeltaF, 0, 1, 'f' ) + "Hz</b>";
    s += "  Loop: <b>" + Str( Count+1 ) + "</b>";
    message( s );

    sleep( Signal->duration() );
    if ( interrupt() ) {
      save();
      stop();
      return Aborted;
    }

    testWrite( *Signal );
    // signal failed?
    if ( !Signal->success() ) {
      if ( Signal->busy() ) {
	warning( "Output still busy!<br> Probably missing trigger.<br> Output of this signal software-triggered.", 4.0 );
	Signal->setStartSource( 0 );
	Signal->setPriority();
	write( *Signal );
	sleep( Signal->duration() );
	if ( interrupt() ) {
	  save();
	  stop();
	  return Aborted;
	}
	// trigger:
	// XXX setupTrigger( traces(), events() );
      }
      else if ( Signal->error() == Signal->OverflowUnderrun ) {
	warning( "Analog Output Underrun Error!<br> Try again.", 4.0 );
	write( *Signal );
	sleep( Signal->duration() );
	if ( interrupt() ) {
	  save();
	  stop();
	  return Aborted;
	}
      }
      else {
	string s = "Output of stimulus failed!<br>Error is <b>";
	s += Signal->errorText() + "</b>";
	warning( s );
	return Failed;
      }
      testWrite( *Signal );
      if ( !Signal->success() ) {
	string s = "Output of stimulus failed again!<br>Error code is <b>";
	s += Signal->errorText() + "</b>";
	warning( s );
	return Failed;
      }
    }

    sleepOn( Signal->duration() + Pause );
    if ( interrupt() ) {
      save();
      stop();
      return Aborted;
    }
    timeStamp();

    // adjust input gains:
    for ( int k=0; k<MaxSpikeTraces; k++ ) {
      if ( SpikeTrace[k] >= 0 )
	adjust( trace( SpikeTrace[k] ),
		trace( SpikeTrace[k] ).signalTime()+Duration,
		trace( SpikeTrace[k] ).signalTime()+Duration+Pause,
		0.8 );
    }
    if ( NerveTrace[0] >= 0 )
      adjust( trace( NerveTrace[0] ),
	      trace( NerveTrace[0] ).signalTime()+Duration,
	      trace( NerveTrace[0] ).signalTime()+Duration+Pause,
	      0.8 );
    if ( GlobalEFieldTrace >= 0 ) {
      double v = trace( GlobalEFieldTrace ).maxAbs( trace( GlobalEFieldTrace ).signalTime(),
						    trace( GlobalEFieldTrace ).signalTime()+Duration+Pause );
      adjustGain( trace( GlobalEFieldTrace ), 1.05 * v );
      detectorEventsOpts( GlobalEFieldEvents ).setNumber( "threshold", 0.5*v );
    }

    // analyze:
    analyze();
    plot();

    // save:
    if ( Repeats > 0 ) {
      if ( Count == 0 ) {
	Header.setInteger( "index", totalRuns() );
	Header.setText( "waveform", SineWave ? "Sine-Wave" : "Fish-EOD" );
	Header.setNumber( "true deltaf", TrueDeltaF );
	Header.setNumber( "true contrast", 100.0 * TrueContrast );
	Header.setNumber( "EOD rate", FishRate );
	Header.setUnit( "trans. amplitude", EOD2Unit );
	Header.setNumber( "trans. amplitude", FishAmplitude );
	Header.setText( "session time", sessionTimeStr() );
      }
      Header.setInteger( "trace", -1 );
      saveAmpl();
      saveAllAmpl();
      for ( int trace=0; trace<MaxSpikeTraces; trace++ ) {
	if ( SpikeEvents[trace] >= 0 ) {
	  Header.setInteger( "trace", trace );
	  saveSpikes( trace );
	  saveAllSpikes( trace );
	}
      }
      if ( NerveTrace[0] >= 0 ) {
	Header.setInteger( "trace", 0 );
	saveNerve();
	saveAllNerve();
      }

    }

  }

  save();
  stop();
  return Completed;
}


void SAM::stop( void )
{
  if ( Signal != 0 )
    delete Signal;
  Signal = 0;
  EODTransAmpl.clear();
  AllEODTransAmpl.free();
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Spikes[k].clear();
      Spikes[k].reserve( 0 );
      AllSpikes[k].clear();
      AllSpikes[k].reserve( 0 );
      delete SpikeRate[k];
    }
    SpikeRate[k] = 0;
  }
  if ( NerveTrace[0] >= 0 ) {
    NerveAmplP.clear();
    NerveAmplT.clear();
    NerveAmplM.clear();
    AllNerveAmplP.free();
    AllNerveAmplT.free();
    AllNerveAmplM.free();
    NerveMeanAmplP.clear();
    NerveMeanAmplT.clear();
    NerveMeanAmplM.clear();
  }
  writeZero( AM ? GlobalAMEField : GlobalEField );
}


void SAM::saveRate( int trace )
{
  // create file:
  ofstream df( addPath( "samrate" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# " );
  settings().save( df, "#   " );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%9.2f" );
  key.addNumber( "rate", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int n=0; n<SpikeRate[trace]->size(); n++ ) {
    key.save( df, 1000.0 * SpikeRate[trace]->pos( n ), 0 );
    key.save( df, (*SpikeRate[trace])[n] );
    df << '\n';
  }
  df << '\n' << '\n';
}


void SAM::saveAmpl( void )
{
  // create file:
  ofstream df( addPath( "samampl.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# " );
    settings().save( df, "#   " );
    df << '\n';
    AmplKey.setUnit( 1, EOD2Unit );
    AmplKey.saveKey( df, true, false );
  }

  // write data:
  for ( unsigned int i=0; i<EODTransAmpl.size(); i++ ) {
    df << '\n';
    df << "# trial: " << Count << '\n';
    df << "#  beat: " << i << '\n';
    for ( int j=0; j<EODTransAmpl[i].size(); j++ ) {
      AmplKey.save( df, 1000.0 * EODTransAmpl[i].x(j), 0 );
      AmplKey.save( df, EODTransAmpl[i].y(j), 1 );
      df << '\n';
    }
  }
}


void SAM::saveAllAmpl( void )
{
  // create file:
  ofstream df( addPath( "samallampl.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# " );
    settings().save( df, "#   " );
    df << '\n';
    AmplKey.setUnit( 1, EOD2Unit );
    AmplKey.saveKey( df, true, false );
  }

  // write data:
  df << '\n';
  df << "# trial: " << Count << '\n';
  for ( int j=0; j<AllEODTransAmpl.size(); j++ ) {
    AmplKey.save( df, 1000.0 * AllEODTransAmpl.x(j), 0 );
    AmplKey.save( df, AllEODTransAmpl.y(j), 1 );
    df << '\n';
  }
}


void SAM::saveSpikes( int trace )
{
  // create file:
  ofstream df( addPath( "samspikes" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# " );
    settings().save( df, "#   " );
    df << '\n';
    SpikesKey.saveKey( df, true, false );
  }

  // write data:
  for ( unsigned int i=0; i<Spikes[trace].size(); i++ ) {
    df << '\n';
    df << "# trial: " << Count << '\n';
    df << "#  beat: " << i << '\n';
    if ( Spikes[trace][i].size() > 0 ) {
      for ( unsigned int j=0; j<Spikes[trace][i].size(); j++ ) {
	SpikesKey.save( df, 1000.0 * Spikes[trace][i][j], 0 );
	df << '\n';
      }
    }
    else
      df << "  -0\n";
  }
}


void SAM::saveAllSpikes( int trace )
{
  // create file:
  ofstream df( addPath( "samallspikes" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# " );
    settings().save( df, "#   " );
    df << '\n';
    SpikesKey.saveKey( df, true, false );
  }
      
  // write data:
  df << '\n';
  df << "# trial: " << Count << '\n';
  if ( AllSpikes[trace].size() > 0 ) {
    for ( unsigned int j=0; j<AllSpikes[trace].size(); j++ ) {
      SpikesKey.save( df, 1000.0 * AllSpikes[trace][j], 0 );
      df << '\n';
    }
  }
  else 
    df << "  -0\n";

}


void SAM::saveNerve( void )
{
  // create file:
  ofstream df( addPath( "samnerveampl.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# " );
    settings().save( df, "#   " );
    df << '\n';
    NerveKey.saveKey( df, true, true );
  }

  // write data:
  for ( unsigned int i=Offset; i<NerveAmplP.size(); i++ ) {
    df << '\n';
    df << "# trial: " << Count << '\n';
    df << "#  beat: " << i << '\n';
    for ( int j=0; j<NerveAmplP[i].size(); j++ ) {
      NerveKey.save( df, 1000.0 * NerveAmplP[i].x( j ), 0 );
      NerveKey.save( df, NerveAmplP[i][j] );
      NerveKey.save( df, 1000.0 * NerveAmplT[i].x( j ) );
      NerveKey.save( df, NerveAmplT[i][j] );
      NerveKey.save( df, 1000.0 * NerveAmplM[i].x( j ) );
      NerveKey.save( df, NerveAmplM[i][j] );
      df << '\n';
    }
  }
}


void SAM::saveAllNerve( void )
{
  // create file:
  ofstream df( addPath( "samallnerveampl.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# " );
    settings().save( df, "#   " );
    df << '\n';
    NerveKey.saveKey( df, true, true );
  }
      
  // write data:
  df << '\n';
  df << "# trial: " << Count << '\n';
  for ( int j=0; j<AllNerveAmplP.size(); j++ ) {
    NerveKey.save( df, 1000.0 * AllNerveAmplP.x( j ), 0 );
    NerveKey.save( df, AllNerveAmplP[j] );
    NerveKey.save( df, 1000.0 * AllNerveAmplT.x( j ) );
    NerveKey.save( df, AllNerveAmplT[j] );
    NerveKey.save( df, 1000.0 * AllNerveAmplM.x( j ) );
    NerveKey.save( df, AllNerveAmplM[j] );
    df << '\n';
  }
}


void SAM::save( void )
{
  if ( Repeats <= 0 || Count <= 0 )
    return;

  for ( int trace=0; trace<MaxSpikeTraces; trace++ ) {
    if ( SpikeEvents[trace] >= 0 ) {
      Header.setInteger( "trace", trace );
      saveRate( trace );
    }
  }
}


void SAM::plot( void )
{
  P.lock();
  int n=0;
  for ( int k=0; k<MaxSpikeTraces + NerveTraces; k++ ) {
    if ( ( k<MaxSpikeTraces && SpikeEvents[k] >= 0 ) ||
	 ( k>=MaxSpikeTraces && NerveTrace[0] >= 0 ) ) {
      
      if ( k<MaxSpikeTraces ) {
	// rate and spikes:
	P[2*n].clear();
	P[2*n].setYRange( 0.0, MaxRate[k] );
	int j = 0;
	double delta = Spikes[k].size() < 20 ? 1.0/Spikes[k].size() : 0.05;
	int offs = Spikes[k].size() > 20 ? Spikes[k].size() - 20 : 0;
	for ( unsigned int i=offs; i<Spikes[k].size(); i++ ) {
	  j++;
	  P[2*n].plot( Spikes[k][i], 1000.0,
		       1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp,
		       delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
	}
	P[2*n].plot( *SpikeRate[k], 1000.0, Plot::Yellow, 2, Plot::Solid );
      }
      else {
	// nerve potential:
	P[2*n].clear();
	for ( unsigned int i=Offset; i<NerveAmplM.size(); i++ ) {
	  P[2*n].plot( NerveAmplM[i], 1000.0, Plot::Cyan, 1, Plot::Solid );
	}
	P[2*n].plot( NerveMeanAmplM, 1000.0, Plot::Magenta, 2, Plot::Solid );
      }

      // amplitude:
      P[2*n+1].clear();
      if ( ! EODTransAmpl.empty() ) {
	for ( unsigned int i=0; i < EODTransAmpl.size()-1; i++ )
	  P[2*n+1].plot( EODTransAmpl[i], 1000.0, Plot::DarkGreen, 2, Plot::Solid );
	P[2*n+1].plot( EODTransAmpl.back(), 1000.0, Plot::Green, 2, Plot::Solid );
      }

      n++;
    }
  }
  P.unlock();

  P.draw();
}


void SAM::analyzeSpikes( const EventData &se, int k, 
			 const EventData &beattimes )
{
  // spikes:
  Spikes[k].resize( beattimes.size() );
  for ( int i = 0; i<beattimes.size(); i++ ) {
    se.copy( beattimes[i]-0.5 * Period, beattimes[i]+0.5 * Period,
	     beattimes[i], Spikes[k][i] );
  }
  se.copy( se.signalTime()-Before, se.signalTime()+Duration+After,
	   se.signalTime(), AllSpikes[k] );
  
  // beat spike rate:
  for ( int j=0; j<beattimes.size(); j++ )
    se.addRate( *SpikeRate[k], Trials[k], 0.0, beattimes[j] );
  
  double max = ::relacs::max( *SpikeRate[k] );
  if ( max+100.0 > MaxRate[k] ) {
    MaxRate[k] = ::ceil((max+100.0)/20.0)*20.0;
  }
}


void SAM::analyze( void )
{
  const EventData &eod2 = events( LocalEODEvents[0] );

  // EOD trace unit:
  EOD2Unit = trace( LocalEODTrace[0] ).unit();

  // EOD rate:
  if ( EODEvents >= 0 )
    FishRate = events( EODEvents ).frequency( ReadCycles );
  else
    FishRate = events( LocalEODEvents[0] ).frequency( ReadCycles );

  // Delta F:
  if ( AM || FreqAbs )
    TrueDeltaF = DeltaF;
  else {
    if ( GlobalEFieldEvents >= 0 )
      TrueDeltaF = events( GlobalEFieldEvents ).frequency( ReadCycles ) - FishRate;
  }

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ), eod2,
				eod2.back() - 0.5, eod2.back() );

  // contrast:
  TrueContrast = beatContrast( trace( LocalEODTrace[0] ),
			       events( LocalBeatPeakEvents[0] ),
			       events( LocalBeatTroughEvents[0] ),
			       eod2.signalTime(), eod2.signalTime()+Duration,
			       0.1*Duration );

  // beat positions:
  EventData beattimes( EODTransAmpl.capacity() );

  if ( AM || FreqAbs ) {
    for ( double t = fabs(0.25/TrueDeltaF); t < Duration; t += fabs(1.0/TrueDeltaF) ) {
      double t0 = eod2.signalTime() + t;
      if ( ! events( ChirpEvents ).within( t0, 0.03 ) &&
	   t0 >= Skip * Period + eod2.signalTime() && 
	   t0 <= Signal->duration() - 2.0*Skip*Period + eod2.signalTime() ) {
	beattimes.push( t0 );
      }
    }
  }
  else if ( GlobalEFieldEvents >= 0 ) {
    double p0 = 0.0;
    double p1 = 0.0;
    double p2 = 0.0;
    //    ofstream df( "signal.dat" );
    const EventData &sige = events( GlobalEFieldEvents );
    for ( EventIterator index = sige.begin( sige.signalTime() );
	  index < sige.begin( eod2.signalTime() + Duration );
	  ++index ) {
      double t1 = *index;
      int pi = eod2.previous( t1 );
      double t0 = eod2[ pi ];
      double p = ( t1 - t0 ) / ( eod2[ pi + 1 ] - t0 );
      //      while ( p - p2 < -0.2 )
      //	p += 1.0;
      //df << t1 << "  " << p << endl;
      if ( fabs( p2 - p1 ) > 0.6 &&
	   fabs( p0 - p1 ) < 0.4 &&
	   fabs( p - p2 ) < 0.4 &&
      /*
XXX
1. p fuer alle index
2. Running linear savitzky golay ueber 1/8 periode
3. take phase
XXX
      if ( p - floor( p ) < 0.95 &&
	   p2 - floor( p2 ) > 0.95 &&
      */
	   ! events( ChirpEvents ).within( t0, 0.03 ) &&
	   t0 >= Skip * Period + eod2.signalTime() && 
	   t0 <= Signal->duration() - 2.0*Skip*Period + eod2.signalTime() ) {
	beattimes.push( eod2[ pi - 1 ] );
	// skip a quarter period:
	for ( int j=0; 
	      j < int( 0.25*(FishRate + DeltaF)/fabs(DeltaF) ) &&
		index < sige.begin( eod2.signalTime() + Duration ); 
	      j++ )
	  ++index;
      }
      p0 = p1;
      p1 = p2;
      p2 = p;
    }
  }

  // EOD transdermal amplitude:
  double dt = 0.25 * eod2.interval( ReadCycles );
  EODTransAmpl.resize( beattimes.size() );
  for ( int k = 0; k<beattimes.size(); k++ ) {
    int cycles = eod2.count( beattimes[k] - 0.5 * Period, Period );
    EODTransAmpl[k].clear();
    EODTransAmpl[k].reserve( cycles+10 );
    EventSizeIterator pindex = eod2.begin( beattimes[k] - 0.5 * Period );
    EventSizeIterator plast = eod2.begin( beattimes[k] + 0.5 * Period );
    for ( ; pindex < plast; ++pindex )
      EODTransAmpl[k].push( pindex.time() - beattimes[k] - dt, *pindex );
  }
  AllEODTransAmpl.clear();
  EventSizeIterator pindex = eod2.begin( eod2.signalTime() );
  EventSizeIterator plast = eod2.begin( eod2.signalTime() + Duration );
  for ( ; pindex < plast; ++pindex )
    AllEODTransAmpl.push( pindex.time() - eod2.signalTime(), *pindex );

  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 )
      analyzeSpikes( events( SpikeEvents[k] ), k, beattimes );
  }

  if ( NerveTrace[0] >= 0 ) {
    const InData &nd = trace( NerveTrace[0] );
    Offset = NerveAmplP.size();

    for ( int i = 0; i<beattimes.size(); i++ ) {
      // nerve amplitudes:
      // peak and trough amplitudes:
      double l = beattimes[i];
      double min = nd.min( l, l+4.0/FishRate );
      double max = nd.max( l, l+4.0/FishRate );
      double threshold = 0.5*(max-min);
      if ( threshold < 1.0e-8 )
	threshold = 0.001;
      EventList peaktroughs( 2, (int)rint(2000.0*Period), true );
      InData::const_iterator firstn = nd.begin( l-0.5*Period-1.0/FishRate );
      InData::const_iterator lastn = nd.begin( l+0.5*Period+10.0/FishRate );
      if ( lastn > nd.end() )
	lastn = nd.end();
      InDataTimeIterator firstt = nd.timeBegin( l-0.5*Period-1.0/FishRate );
      Detector< InData::const_iterator, InDataTimeIterator > D;
      D.init( firstn, lastn, firstt );
      D.peakTrough( firstn, lastn, peaktroughs,
		    threshold, threshold, threshold, 
		    NerveAcceptEOD );
      // store amplitudes:
      NerveAmplP.push_back( MapD() );
      NerveAmplT.push_back( MapD() );
      NerveAmplM.push_back( MapD() );
      NerveAmplP.back().reserve( peaktroughs[0].size() + 100 );
      NerveAmplT.back().reserve( peaktroughs[0].size() + 100 );
      NerveAmplM.back().reserve( peaktroughs[0].size() + 100 );
      for ( int k=0; k<peaktroughs[0].size() && k<peaktroughs[1].size(); k++ ) {
	NerveAmplP.back().push( peaktroughs[0][k] - l, 
				peaktroughs[0].eventSize( k ) );
	NerveAmplT.back().push( peaktroughs[1][k] - l, 
				peaktroughs[1].eventSize( k ) );
      }
      // averaged amplitude:
      double st = (peaktroughs[0].back() - peaktroughs[0].front())/double(peaktroughs[0].size()-1);
      int si = nd.indices( st );
      int inx = nd.indices( l-0.5*Period-1.0/FishRate );
      for ( int k=0; k<NerveAmplP.back().size(); k++ ) {
	NerveAmplM.back().push( nd.interval( inx )-l, 
				nd.mean( inx, inx+si ) );
	inx += si;
      }
    }

    // nerve mean amplitudes:
    average( NerveMeanAmplP, NerveAmplP );
    average( NerveMeanAmplT, NerveAmplT );
    average( NerveMeanAmplM, NerveAmplM );

    // nerve amplitudes:
    // peak and trough amplitudes:
    double l = nd.signalTime();
    double min = nd.min( l, l+4.0/FishRate );
    double max = nd.max( l, l+4.0/FishRate );
    double threshold = 0.5*(max-min);
    if ( threshold < 1.0e-8 )
      threshold = 0.001;
    EventList peaktroughs( 2, (int)rint(1500.0*Duration), true );
    InData::const_iterator firstn = nd.begin( l );
    InData::const_iterator lastn = nd.begin( l+Duration );
    if ( lastn > nd.end() )
      lastn = nd.end();
    InDataTimeIterator firstt = nd.timeBegin( l );
    Detector< InData::const_iterator, InDataTimeIterator > D;
    D.init( firstn, lastn, firstt );
    D.peakTrough( firstn, lastn, peaktroughs,
		  threshold, threshold, threshold, 
		  NerveAcceptEOD );
    // store amplitudes:
    AllNerveAmplP.reserve( peaktroughs[0].size() + 100 );
    AllNerveAmplT.reserve( peaktroughs[0].size() + 100 );
    AllNerveAmplM.reserve( peaktroughs[0].size() + 100 );
    for ( int k=0; k<peaktroughs[0].size() && k<peaktroughs[1].size(); k++ ) {
      AllNerveAmplP.push( peaktroughs[0][k] - l, 
			  peaktroughs[0].eventSize( k ) );
      AllNerveAmplT.push( peaktroughs[1][k] - l, 
			  peaktroughs[1].eventSize( k ) );
    }
    // averaged amplitude:
    double st = (peaktroughs[0].back() - peaktroughs[0].front())/double(peaktroughs[0].size()-1);
    int si = nd.indices( st );
    int inx = nd.indices( l );
    for ( int k=0; k<AllNerveAmplP.size(); k++ ) {
      AllNerveAmplM.push( nd.interval( inx )-l, nd.mean( inx, inx+si ) );
      inx += si;
    }
  }
}


addRePro( SAM );

}; /* namespace efish */

#include "moc_sam.cc"
