/*
  efish/filestimulus.cc
  Load a stimulus from a text file.

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

#include <cmath>
#include <fstream>
#include <iomanip>
#include <relacs/efish/filestimulus.h>
using namespace relacs;

namespace efish {


FileStimulus::FileStimulus( void )
  : RePro( "FileStimulus", "efish", "Jan Benda", "1.3", "Mar 25, 2010" )
{
  // parameter:
  SigStdev = 1.0;
  Duration = 0.0;
  Pause = 1.0;
  UseContrast = true;
  Contrast = 0.2;
  Amplitude = 1.0;
  AM = true;
  Repeats = 6;
  RateDeltaT = 0.01;
  PlotPsd = false;
  Before = 0.0;
  After = 0.0;
  NoiseAmpl = 0.0;
  NoiseContrast = 0.0;

  UpperCutoff = 0.0;
  LowerCutoff = 0.0;
  NoiseTau = 0.0;
  NoiseType = "White";

  // add some parameter as options:
  newSection( "Stimulus" );
  addText( "file", "Stimulus file", "" ).setStyle( OptWidget::BrowseExisting );
  addNumber( "sigstdev", "Standard deviation of signal", SigStdev, 0.01, 1.0, 0.05 );
  addNumber( "duration", "Duration of signal", Duration, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "pause", "Pause between signals", Pause, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addSelection( "amplsel", "Stimulus amplitude", "contrast|absolute" );
  addNumber( "contrast", "Contrast", Contrast, 0.0, 1.0, 0.01, "", "%" ).setActivation( "amplsel", "contrast" );
  addNumber( "amplitude", "Amplitude", Amplitude, 0.001, 1000.0, 0.1, "mV/cm" ).setActivation( "amplsel", "absolute" );
  addBoolean( "am", "Amplitude modulation", AM );
  addInteger( "repeats", "Repeats", Repeats, 0, 100000, 2 ).setStyle( OptWidget::SpecialInfinite );
  newSection( "Additional noise" );
  addSelection( "noisetype", "Type of noise", "none|Gaussian-White|Ornstein-Uhlenbeck");
  addNumber( "uppercutoff", "Upper cutoff frequency", UpperCutoff, 0.0, 10000.0, 1.0, "","Hz" ).setActivation("noisetype","White");
  addNumber( "lowercutoff", "Lower cutoff frequency", LowerCutoff, 0.0, 10000.0, 1.0, "","Hz" ).setActivation("noisetype","White");
  addNumber( "noisetau", "Time constant of the Ornstein-Uhlenbeck process",NoiseTau,0.0,500.0,0.01,"seconds","ms" ).setActivation("noisetype","Ornstein-Uhlenbeck");
  addNumber( "noisecontrast", "Contrast of the added noise",NoiseContrast, 0.0, 1.00, 0.005, "", "%" ).setActivation("amplsel", "contrast");
  addNumber( "noiseampl", "Amplitude of the additional noise", NoiseAmpl, 0.0, 100.00, 0.05, "mV/cm" ).setActivation( "amplsel", "absolute");
  newSection( "Analysis" );
  addNumber( "binwidth", "Bin width", RateDeltaT, 0.0001, 1.0, 0.002, "seconds", "ms" );
  addNumber( "before", "Spikes recorded before stimulus", Before, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addNumber( "after", "Spikes recorded after stimulus", After, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addBoolean( "plotpsd", "Plot power spectrum", PlotPsd );
  addSelection( "psdsize", "Number of data points for FFT", "256|64|128|256|512|1024|2048|4096|8192|16384|32768|65536|131072|262144|524288|1048576" );
  addBoolean( "adjust", "Adjust input gain?", true );

  // variables:
  FishAmplitude = 0.0;
  Intensity = 0.0;
  Count = 0;
  EODTransAmpl.clear();
  EFieldAmpl.clear();
  for ( int k=0; k<MaxTraces; k++ ) {
    Spikes[k].clear();
    Trials[k] = 0; 
    SpikeRate[k].clear();
    SpikeFrequency[k].clear();
    SpikePower[k].clear();
  }
  NerveAmplP.clear();
  NerveAmplT.clear();
  NerveAmplM.clear();
  NerveMeanAmplP.clear();
  NerveMeanAmplT.clear();
  NerveMeanAmplM.clear();
  NervePower.clear();

  // header for files:
  Header.addInteger( "index" );
  Header.addInteger( "trace" );
  Header.addNumber( "EOD rate", "Hz", "%.1f" );
  Header.addNumber( "trans. amplitude", "", "%.2f" );
  Header.addNumber( "true contrast", "%", "%.1f" );
  Header.addText( "file" );
  Header.addNumber( "duration", "sec", "%.3f" );
  Header.addText( "session time" );
  Header.newSection( "Settings" );

  // tablekeys:
  AmplKey.addNumber( "time", "ms", "%9.2f", 3 );
  AmplKey.addNumber( "ampl", LocalEODUnit, "%5.3f", 3 );

  EFieldKey.addNumber( "time", "ms", "%9.2f", 3 );
  EFieldKey.addNumber( "efield", "mV", "%7.1f", 3 );

  SpikesKey.addNumber( "time", "ms", "%9.2f" );

  NoiseKey.addNumber( "time", "ms", "%9.2f" );
  NoiseKey.addNumber( "signal+noise", "1", "%10.7f" );
  NoiseKey.addNumber( "signal", "1", "%10.7f" );
  NoiseKey.addNumber( "noise", "1", "%10.7f" );

  NerveKey.newSection( "peak" );
  NerveKey.addNumber( "time", "ms", "%9.2f" );
  NerveKey.addNumber( "ampl", "uV", "%6.1f" );
  NerveKey.newSection( "trough" );
  NerveKey.addNumber( "time", "ms", "%9.2f" );
  NerveKey.addNumber( "ampl", "uV", "%6.1f" );
  NerveKey.newSection( "average" );
  NerveKey.addNumber( "time", "ms", "%9.2f" );
  NerveKey.addNumber( "ampl", "uV", "%7.2f" );

  // plot:
  setWidget( &P );
}


int FileStimulus::main( void )
{
  // get options:
  Str file = text( "file" );
  SigStdev = number( "sigstdev" );
  Duration = number( "duration" );
  Pause = number( "pause" );
  Repeats = integer( "repeats" );
  UseContrast = ( index( "amplsel" ) == 0 );
  Contrast = number( "contrast" );
  Amplitude = number( "amplitude" );
  AM = boolean( "am" );
  RateDeltaT = number( "binwidth" );
  Before = number( "before" );
  After = number( "after" );
  PlotPsd = boolean( "plotpsd" );
  int specsize = integer( "psdsize" );
  bool adjustg = boolean( "adjust" );

  NoiseType = text( "noisetype" );
  UpperCutoff = number( "uppercutoff" );
  LowerCutoff = number( "lowercutoff" );
  NoiseTau = number( "noisetau" );
  NoiseAmpl = number( "noiseampl" );
  NoiseContrast = number ( "noisecontrast" );
  bool addNoise = NoiseType != "none";

  // checks:
  if ( LocalEODTrace[0] < 0 ) {
    warning( "need local recording of the EOD Trace." );
    return Failed;
  }
  if ( LocalEODEvents[0] < 0 ) {
    warning( "need EOD events of local recording of the EOD Trace." );
    return Failed;
  }
  
  if ( addNoise && UpperCutoff < LowerCutoff){
    warning( "Upper cutoff of additional noise smaller than lower cutoff! Lower cutoff set to zero!" );
    LowerCutoff = 0.0;
  }

  // plot trace:
  if ( Duration <= 1.0 )
    tracePlotSignal( Before+Duration+After, Before );
  else
    tracePlotContinuous( 1.0 );

  // plot:
  P.lock();
  int np = SpikeTraces + NerveTraces;
  double psdw = 0.0;
  if ( PlotPsd ) {
    np *= 2;
    psdw = 0.3;
  }
  P.resize( 1 + np );
  double stimheight = SpikeTraces > 1 ? 1.0/(1.0+SpikeTraces+NerveTraces) : 0.4;
  double rateheight = ( 1.0 - stimheight ) / ( SpikeTraces + NerveTraces );
  P[0].setOrigin( 0.0, 0.0 );
  P[0].setSize( 1.0-psdw, stimheight );
  P[0].setLMarg( 6 );
  P[0].setRMarg( 2 );
  P[0].setBMarg( 3 );
  P[0].setTMarg( 1 );
  P[0].setXLabel( "[s]" );
  P[0].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::FirstAxis, 
		     Plot::Left, 0.0 );
  P[0].setXTics();
  if ( Duration <= 1.0 )
    P[0].setXRange( -Before, Duration+After );
  else
    P[0].setXRange( 0.0, 1.0 );
  P[0].setYLabel( "Stimulus" );
  P[0].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
		     Plot::Center, -90.0 );
  P[0].setAutoScaleY();
  P[0].setYTics( );

  int n=0;
  for ( int k=0; k<SpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      n++;
      double h = stimheight + (n-1)*rateheight;
      if ( PlotPsd )
	h =  stimheight + (n-1)/2*rateheight;
      P[n].setOrigin( 0.0, h );
      P[n].setSize( 1.0-psdw, rateheight );
      P[n].setLMarg( 6 );
      P[n].setRMarg( 2 );
      P[n].setBMarg( 1 );
      P[n].setTMarg( 1 );
      P[n].setXLabel( "" );
      P[n].noXTics();
      P[n].setYLabel( "Firing rate [Hz]" );
      P[n].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
			 Plot::Center, -90.0 );
      P[n].setYRange( 0.0, 100.0 );
      P[n].setYTics( );
      P[n].clear();
      if ( Duration <= 1.0 )
	P[n].setXRange( -Before, Duration+After );
      else
	P[n].setXRange( 0.0, 1.0 );
      P.setCommonXRange( 0, n );
      if ( n > 1 )
	P.setCommonYRange( n-(PlotPsd?2:1), n );

      if ( PlotPsd ) {
	n++;
	P[n].setOrigin( 1.0-psdw, h );
	P[n].setSize( psdw, rateheight );
	P[n].setLMarg( 5 );
	P[n].setRMarg( 2 );
	P[n].setBMarg( 1 );
	P[n].setTMarg( 1 );
	P[n].setXRange( 0.0, 500.0 );
	if ( n > 2 ) {
	  P[n].setXLabel( "" );
	  P[n].noXTics();
	}
	else {
	  P[n].setXLabel( "Frequency [Hz]" );
	  P[n].setXTics();
	}
	P[n].setYLabel( "Power [dB]" );
	P[n].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
			   Plot::Center, -90.0 );
	P[n].setYTics( );
	P[n].clear();
	if ( n > 2 ) {
	  P.setCommonXRange( n-2, n );
	  P.setCommonYRange( n-2, n );
	}
      }
    }
  }
  if ( NerveTrace[0] >= 0 ) {
    n++;
    double h = stimheight + (n-1)*rateheight;
    if ( PlotPsd )
      h =  stimheight + (n-1)/2*rateheight;
    P[n].setOrigin( 0.0, h );
    P[n].setSize( 1.0-psdw, rateheight );
    P[n].setLMarg( 6 );
    P[n].setRMarg( 2 );
    P[n].setBMarg( 1 );
    P[n].setTMarg( 1 );
    P[n].setXLabel( "" );
    P[n].setYLabel( "Nerve Amplitude [uV]" );
    P[n].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
		       Plot::Center, -90.0 );
    P[n].setAutoScaleY();
    P[n].setYTics( );
    P[n].clear();
    if ( Duration <= 1.0 )
      P[n].setXRange( -Before, Duration+After );
    else
      P[n].setXRange( 0.0, 1.0 );
    P.setCommonXRange( 0, n );

    if ( PlotPsd ) {
      n++;
      P[n].setOrigin( 1.0-psdw, h );
      P[n].setSize( psdw, rateheight );
      P[n].setLMarg( 5 );
      P[n].setRMarg( 2 );
      P[n].setBMarg( 1 );
      P[n].setTMarg( 1 );
      P[n].setXRange( 0.0, 500.0 );
      if ( n > 2 ) {
	  P[n].setXLabel( "" );
	  P[n].noXTics();
      }
      else {
	P[n].setXLabel( "Frequency [Hz]" );
	P[n].setXTics();
      }
      P[n].setYLabel( "Power [dB]" );
      P[n].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
			 Plot::Center, -90.0 );
      P[n].setYTics( );
      P[n].clear();
      if ( n > 2 ) {
	P.setCommonXRange( n-2, n );
	P.setCommonYRange( n-2, n );
      }
    }
  }
  P.draw();
  P.unlock();

  // create signal:
  string filename = file.name();
  file.expandPath();
  OutData signal;
  signal.setTrace( AM ? GlobalAMEField : GlobalEField );
  setWaitMouseCursor();
  signal.load( file, filename );
  if ( signal.empty() ) {
    warning( "Cannot load stimulus file <b>" + file + "</b>!" );
    restoreMouseCursor();
    return Failed;
  }
  if ( Duration > 0.0 ) {
    if ( signal.length() >= Duration )
      signal.resize( signal.indices( Duration ) );
    else if ( signal.length() < Duration )
      warning( "Duration of signal " + Str( signal.length() ) + " sec is smaller than requested duration of " + Str( Duration ) + " sec!" );
  }
  signal.fixSample();
  signal.setTrace( AM ? GlobalAMEField : GlobalEField );
  signal.setStartSource( 1 );
  signal.setDelay( Before );
  signal.setIdent( filename );
  Duration = signal.duration();
  restoreMouseCursor();

  // data:
  Intensity = 0.0;
  if ( UseContrast )
    EODTransAmpl.reserve( Repeats>0 ? Repeats : 100 );
  else
    EFieldAmpl.reserve( Repeats>0 ? Repeats : 100 );

  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Spikes[k].clear();
      Spikes[k].reserve( Repeats>0 ? Repeats : 100 );
      Trials[k] = 0;
      SpikeRate[k] = SampleDataD( -Before, Duration+After, RateDeltaT );
      SpikeFrequency[k] = SampleDataD( -Before, Duration+After, RateDeltaT );
      MaxRate[k] = 20.0;
      SpikePower[k] = SampleDataD( specsize );
    }
  }
  if ( NerveTraces > 0 ) {
    NerveAmplP.reserve( Repeats>0 ? Repeats : 500 );
    NerveAmplT.reserve( Repeats>0 ? Repeats : 500 );
    NerveAmplM.reserve( Repeats>0 ? Repeats : 500 );
    NerveMeanAmplP = SampleDataD( -Before, Duration+After, 0.001 );
    NerveMeanAmplT = SampleDataD( -Before, Duration+After, 0.001 );
    NerveMeanAmplM = SampleDataD( -Before, Duration+After, 0.001 );
    NervePower = SampleDataD( specsize );
  }

  Header.erase( "Settings" );
  Header.newSection( settings() );

  // trigger:
  //  setupTrigger( events );

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ),
				currentTime() - Pause, currentTime() );

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( currentTime()-0.1,
						  currentTime() );
  if ( val2 > 0.0 ) {
    if ( UseContrast )
      adjustGain( trace( LocalEODTrace[0] ), ( 1.0 + Contrast / SigStdev ) * val2 );
    else
      adjustGain( trace( LocalEODTrace[0] ), val2 + Amplitude / SigStdev );
  }

  // stimulus intensity:
  if ( UseContrast )
    Intensity = Contrast * FishAmplitude / SigStdev;
  else
    Intensity = Amplitude / SigStdev;
  if ( ::fabs( Intensity ) < 1.0e-8 )
    signal.mute();
  else
    signal.setIntensity( Intensity );
  detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", 0.5*signal.intensity() );

  // reset all outputs:
  OutList sigs;
  for ( int k=0; k<EFields; k++ ) {
    OutData sig;
    sig.setTrace( EField[k] );
    sig.constWave( 0.0 );
    sig.setIdent( "init" );
    sig.mute();
    sigs.push( sig );
  }
  directWrite( sigs );
  sleep( 0.01 );
  
  // central loop over Repeats!
  for ( Count = 0;
	( Repeats <= 0 || Count < Repeats ) && softStop() == 0;
	Count++ ) {

    // create additive noise:
    OutData noisesignal( signal );
    OutData noise;
    double noisefac = 1.0;
    if ( addNoise ) {
      // noise intensity:
      double noisestdev = SigStdev;
      if ( UseContrast )
	noisestdev *= NoiseContrast/Contrast; 
      else
	noisestdev *= NoiseAmpl/Amplitude;
      // noise:
      noise.setTrace( AM ? GlobalAMEField : GlobalEField );
      if ( NoiseType == "White" ) {
	double cu = UpperCutoff;
	double cl = LowerCutoff;
	if ( cu > 2/signal.stepsize() ) {
	  string s = "Output of stimulus failed!<br>Error is <b>Upper cutoff beyond Nyquist frequency!</b>";
	  warning( s );
	  stop();
	  return Failed;  
	}
	noise.bandNoiseWave( signal.duration(), signal.stepsize(), cl, cu, noisestdev );
      }
      else {
	noise.ouNoiseWave( signal.duration(), signal.stepsize(), NoiseTau, noisestdev );
      }
      noisesignal += noise;
      noisefac = sqrt(SigStdev*SigStdev + noisestdev*noisestdev)*3.0;
      noisesignal /= noisefac;
      if ( ::fabs( Intensity ) < 1.0e-8 )
	noisesignal.mute();
      else
	noisesignal.setIntensity( Intensity*noisefac );
    }
    else {
      noise.clear();
    }
    int c = ::relacs::clip( -1.0, 1.0, noisesignal );
    printlog( "clipped " + Str( c ) + " from " + Str( noisesignal.size() ) + " data points.\n" );
    noisesignal.back() = 0.0;

    // message: 
    Str s = "Stimulus: <b>" + filename + "</b>";
    if ( UseContrast )
      s += "  Contrast: <b>" + Str( 100.0 * Contrast, 0, 5, 'g' ) + "%</b>";
    else
      s += "  Amplitude: <b>" + Str( Amplitude, 0, 3, 'g' ) + "mV/cm</b>";
    if ( addNoise )
      s += "  with <b>additive noise</b>";
    s += "  Loop: <b>" + Str( Count+1 ) + "</b>";
    message( s );

    // put out the signal:
    write( noisesignal );
    if ( !noisesignal.success() ) {
      string s = "Output of stimulus failed!<br>Error is <b>";
      s += noisesignal.errorText() + "</b>";
      warning( s );
      stop();
      return Failed;
    }
    
    sleep( Pause );
    if ( interrupt() ) {
      save();
      stop();
      return Aborted;
    }

    // signal failed?
    if ( !noisesignal.success() ) {
      if ( noisesignal.busy() ) {
	warning( "Output still busy! <br> Probably missing trigger. <br> Output of this signal software-triggered.", 4.0 );
	noisesignal.setStartSource( 0 );
	noisesignal.setPriority();
	write( noisesignal );
	sleep( Pause );
	if ( interrupt() ) {
	  save();
	  stop();
	  return Aborted;
	}
	// trigger:
	//	setupTrigger( events );
	continue;
      }
      else if ( noisesignal.error() == noisesignal.OverflowUnderrun ) {
	warning( "Analog Output Overrun Error! <br> Try again.", 4.0 );
	write( noisesignal );
	sleep( Pause );
	if ( interrupt() ) {
	  save();
	  stop();
	  return Aborted;
	}
	continue;
      }
      else {
	string s = "Output of stimulus failed!<br>Error is <b>";
	s += noisesignal.errorText() + "</b>";
	warning( s );
	stop();
	return Failed;
      }
    }

    // adjust input gains:
    if ( adjustg ) {
      for ( int k=0; k<MaxTraces; k++ )
	if ( SpikeTrace[k] >= 0 )
	  adjust( trace( SpikeTrace[k] ), signalTime()+Duration,
		  signalTime()+Duration+Pause, 0.8 );
    }
    if ( GlobalEFieldTrace >= 0 )
      adjustGain( trace( GlobalEFieldTrace ),
		  1.05 * trace( GlobalEFieldTrace ).maxAbs( signalTime(),
							    signalTime() + Duration ) );
    
    // analyze:
    analyze();
    plot();

    // save:
    if ( Repeats > 0 ) {
      if ( Count == 0 ) {
	Header.setInteger( "index", totalRuns() );
	Header.setNumber( "EOD rate", FishRate );
	Header.setUnit( "trans. amplitude", LocalEODUnit );
	Header.setNumber( "trans. amplitude", FishAmplitude );
	Header.setNumber( "true contrast", 100.0 * TrueContrast );
	Header.setText( "file", file );
	Header.setNumber( "duration", Duration );
	Header.setText( "session time", sessionTimeStr() );
      }
      Header.setInteger( "trace", -1 );
      saveAmpl();
      for ( int trace=0; trace<MaxTraces; trace++ ) {
	if ( SpikeEvents[trace] >= 0 ) {
	  Header.setInteger( "trace", trace );
	  saveSpikes( trace );
	  saveNoise( trace, Spikes[trace].size()-1, noisesignal, signal, noisefac, noise );
	}
      }
      if ( NerveTrace[0] >= 0 ) {
	Header.setInteger( "trace", 0 );
	saveNerve();
      }
    }
  }

  save();
  stop();
  return Completed;
}


void FileStimulus::stop( void )
{
  P.lock();
  P.clearPlots();
  P.unlock();
  EODTransAmpl.clear();
  EFieldAmpl.clear();
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Spikes[k].clear();
      SpikeRate[k].clear();
      SpikeFrequency[k].clear();
      SpikePower[k].clear();
    }
  }
  NerveAmplT.clear();
  NerveAmplP.clear();
  NerveAmplM.clear();
  NerveMeanAmplT.clear();
  NerveMeanAmplP.clear();
  NerveMeanAmplM.clear();
  NervePower.clear();
  writeZero( AM ? GlobalAMEField : GlobalEField );
}


void FileStimulus::saveRate( int trace )
{
  // create file:
  ofstream df( addPath( "stimrate" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%9.2f" );
  key.addNumber( "rate", "Hz", "%5.1f" );
  key.addNumber( "frequency", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int j=0; j<SpikeRate[trace].size(); j++ ) {
    key.save( df, 1000.0 * SpikeRate[trace].pos( j ), 0 );
    key.save( df, SpikeRate[trace][j] );
    key.save( df, SpikeFrequency[trace][j] );
    df << '\n';
  }
  df << '\n' << '\n';
}


void FileStimulus::savePower( int trace )
{
  // create file:
  ofstream df( addPath( "stimpowerspec" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "freq", "Hz", "%8.3f" );
  key.addNumber( "power", "Hz", "%10.5g" );
  key.saveKey( df, true, false );

  // write data:
  for ( int j=0; j<SpikePower[trace].size(); j++ ) {
    key.save( df, SpikePower[trace].pos( j ), 0 );
    key.save( df, SpikePower[trace][j] );
    df << '\n';
  }
  df << '\n' << '\n';
}


void FileStimulus::saveNerve( void )
{
  // create file:
  ofstream df( addPath( "stimnerveampl.dat" ).c_str(), ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    NerveKey.saveKey( df, true, true );
  }

  // write data:
  df << '\n';
  df << "# trial: " << Count << '\n';
  for ( int n=0; n<NerveAmplP[Count].size(); n++ ) {
    NerveKey.save( df, 1000.0 * NerveAmplT[Count].x( n ), 0 );
    NerveKey.save( df, NerveAmplT[Count][n] );
    NerveKey.save( df, 1000.0 * NerveAmplP[Count].x( n ) );
    NerveKey.save( df, NerveAmplP[Count][n] );
    NerveKey.save( df, 1000.0 * NerveAmplM[Count].x( n ) );
    NerveKey.save( df, NerveAmplM[Count][n] );
    df << '\n';
  }
}


void FileStimulus::saveNervePower( void )
{
  // create file:
  ofstream df( addPath( "stimnervepowerspec.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "freq", "Hz", "%8.3f" );
  key.addNumber( "power", "Hz", "%10.5g" );
  key.saveKey( df, true, false );

  // write data:
  for ( int j=0; j<NervePower.size(); j++ ) {
    key.save( df, NervePower.pos( j ), 0 );
    key.save( df, NervePower[j] );
    df << '\n';
  }
  df << '\n' << '\n';
}


void FileStimulus::saveAmpl( void )
{
  // create file:
  ofstream df( addPath( "stimampl.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    if ( UseContrast ) {
      AmplKey.setUnit( 1, LocalEODUnit );
      AmplKey.saveKey( df, true, false, 1 );
    }
    else {
      EFieldKey.setUnit( 1, trace( GlobalEFieldTrace ).unit() );
      EFieldKey.saveKey( df, true, false, 1 );
    }
  }

  // write data:
  if ( UseContrast ) {
    df << '\n';
    df << "# trial: " << EODTransAmpl.size()-1 << '\n';
    for ( int j=0; j<EODTransAmpl.back().size(); j++ ) {
      AmplKey.save( df, 1000.0*EODTransAmpl.back().x(j), 0 );
      AmplKey.save( df, EODTransAmpl.back().y(j), 1 );
      df << '\n';
    }
  }
  else {
    df << '\n';
    df << "# trial: " << EFieldAmpl.size()-1 << '\n';
    for ( int j=0; j<EFieldAmpl.back().size(); j++ ) {
      EFieldKey.save( df, 1000.0*EFieldAmpl.back().pos(j), 0 );
      EFieldKey.save( df, EFieldAmpl.back()[j], 1 );
      df << '\n';
    }
  }
}


void FileStimulus::saveSpikes( int trace )
{
  // create file:
  ofstream df( addPath( "stimspikes" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    SpikesKey.saveKey( df, true, false );
  }

  // write data:
  df << '\n';
  df << "# trial: " << Spikes[trace].size()-1 << '\n';
  if ( Spikes[trace].back().empty() ) {
    df << "  -0" << '\n';
  }
  else {
    for ( int j=0; j<Spikes[trace].back().size(); j++ ) {
      SpikesKey.save( df, 1000.0 * Spikes[trace].back()[j], 0 );
      df << '\n';
    }
  }

}


void FileStimulus::saveNoise( int trace, int trial, const OutData &noisesignal,
			      const OutData &signal, double noisefac, const OutData &noise )
{
  // create file:
  ofstream df( addPath( "stimadditivenoise" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;
  
  // write header and key:
  if ( Count == 0 ) {
    df << '\n' << '\n';
    Header.save( df, "# " );
    df << '\n';
    NoiseKey.saveKey( df, true, false );
  }
  
  // write data:
  df << '\n';
  df << "# trial: " << trial << '\n';
  if ( noise.empty() ) {
    for ( int j=0; j<noisesignal.size(); j++ ) {
      NoiseKey.save( df, 1000.0 * noisesignal.pos( j ), 0 );
      NoiseKey.save( df, noisesignal[j] );
      NoiseKey.save( df, signal[j]/noisefac );
      NoiseKey.save( df, 0.0 );
      df << '\n';
    }
  }
  else {
    for ( int j=0; j<noisesignal.size(); j++ ) {
      NoiseKey.save( df, 1000.0 * noisesignal.pos( j ), 0 );
      NoiseKey.save( df, noisesignal[j] );
      NoiseKey.save( df, signal[j]/noisefac );
      NoiseKey.save( df, noise[j]/noisefac );
      df << '\n';
    }
  }
}


void FileStimulus::save( void )
{
  if ( Repeats <= 0 )
    return;

  if ( NerveTrace[0] >= 0 )
    saveNervePower();
  for ( int trace=0; trace<MaxTraces; trace++ ) {
    if ( SpikeEvents[trace] >= 0 ) {
      Header.setInteger( "trace", trace );
      saveRate( trace );
      savePower( trace );
    }
  }
}


void FileStimulus::plot( void )
{
  P.lock();
  // amplitude:
  P[0].clear();
  if ( UseContrast ) {
    for ( unsigned int i=0; i<EODTransAmpl.size()-1; i++ )
      P[0].plot( EODTransAmpl[i], 1.0, Plot::DarkGreen, 2, Plot::Solid );
    P[0].plot( EODTransAmpl.back(), 1.0, Plot::Green, 2, Plot::Solid );
  }
  else {
    for ( unsigned int i=0; i<EFieldAmpl.size()-1; i++ )
      P[0].plot( EFieldAmpl[i], 1.0, Plot::DarkGreen, 2, Plot::Solid );
    P[0].plot( EFieldAmpl.back(), 1.0, Plot::Green, 2, Plot::Solid );
  }

  // rate and spikes:
  int maxspikes = (int)rint( 20.0 / SpikeTraces );
  if ( maxspikes < 4 )
    maxspikes = 4;
  int n=0;
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      n++;
      P[n].clear();
      if ( ! P[n].zoomedYRange() )
	P[n].setYRange( 0.0, MaxRate[k] );
      int j = 0;
      double delta = Repeats > 0 && Repeats < maxspikes ? 1.0/Repeats : 1.0/maxspikes;
      int offs = (int)Spikes[k].size() > maxspikes ? Spikes[k].size() - maxspikes : 0;
      for ( int i=offs; i<Spikes[k].size(); i++ ) {
	j++;
	P[n].plot( Spikes[k][i], 1.0,
		   1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp,
		   delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
      }
      P[n].plot( SpikeRate[k], 1.0, Plot::Yellow, 2, Plot::Solid );
      P[n].plot( SpikeFrequency[k], 1.0, Plot::Orange, 2, Plot::Solid );
      if ( PlotPsd ) {
	n++;
	P[n].clear();
	SampleDataD psd( SpikePower[k] );
	psd.decibel();
	P[n].plot( psd, 1.0, Plot::Yellow, 2, Plot::Solid );
      }
    }
  }

  // nerve:
  if ( NerveTrace[0] >= 0 ) {
    n++;
    P[n].clear();
    if ( ! P[n].zoomedYRange() )
      P[n].setAutoScaleY();
    for ( unsigned int i=0; i<NerveAmplM.size(); i++ )
      P[n].plot( NerveAmplM[i], 1000.0, Plot::Cyan, 1, Plot::Solid );
    P[n].plot( NerveMeanAmplM, 1000.0, Plot::Magenta, 2, Plot::Solid );
    if ( PlotPsd ) {
      n++;
      P[n].clear();
      SampleDataD psd( NervePower );
      psd.decibel();
      P[n].plot( psd, 1.0, Plot::Yellow, 2, Plot::Solid );
    }
  }

  P.draw();

  P.unlock();
}


void FileStimulus::analyzeSpikes( const EventData &se, int k )
{
  // spikes:
  Spikes[k].push( se, signalTime()-Before, signalTime()+Duration+After,
		  signalTime() );
  
  // spike rate:
  se.addRate( SpikeRate[k], Trials[k], 0.0, signalTime() );
  Trials[k]--;
  se.addFrequency( SpikeFrequency[k], Trials[k], signalTime() );
  
  double maxr = max( SpikeRate[k] );
  double maxf = max( SpikeFrequency[k] );
  if ( maxf > maxr )
    maxr = maxf;
  if ( maxr+100.0 > MaxRate[k] ) {
    MaxRate[k] = ::ceil((maxr+100.0)/20.0)*20.0;
  }

  // power spectrum:
  SampleDataD psd( SpikePower[k].size() );
  se.spectrum( signalTime(), signalTime()+Duration, 0.001, psd );
  for ( int j=0; j<psd.size(); j++ )
    SpikePower[k][j] += ( psd[j] - SpikePower[k][j] )/Trials[k];
  SpikePower[k].setRange( 0.0, psd.stepsize() );
}


void FileStimulus::analyze( void )
{
  const EventData &localeod = events( LocalEODEvents[0] );

  // EOD trace unit:
  LocalEODUnit = trace( LocalEODTrace[0] ).unit();

  // EOD rate:
  if ( EODEvents >= 0 )
    FishRate = events( EODEvents ).frequency( signalTime() + Duration,
					      signalTime() + Duration + Pause );
  else
    FishRate = localeod.frequency( signalTime() + Duration,
				   signalTime() + Duration + Pause );

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ),
				signalTime() + Duration, signalTime() + Duration + Pause );

  // contrast: (XXX this function needs too much time! XXX)
  TrueContrast = beatContrast( trace( LocalEODTrace[0] ),
			       signalTime()+0.1*Duration,
			       signalTime()+0.9*Duration,
			       0.01, Intensity/FishAmplitude );

  // EOD transdermal amplitude:
  if ( UseContrast ) {
    EventSizeIterator pindex = localeod.begin( signalTime() );
    EventSizeIterator plast = localeod.begin( signalTime() + Duration );
    EODTransAmpl.push_back( MapD() );
    EODTransAmpl.back().reserve( plast - pindex + 1 );
    for ( ; pindex < plast; ++pindex )
      EODTransAmpl.back().push( pindex.time() - signalTime(), *pindex ); 
  }
  else {
    const InData &globalefield = trace( GlobalEFieldTrace );
    EFieldAmpl.push_back( SampleDataF( 0.0, Duration, globalefield.stepsize() ) );
    globalefield.copy( signalTime(), EFieldAmpl.back() );
  }

  // spikes:
  for ( int k=0; k<MaxTraces; k++ )
    if ( SpikeEvents[k] >= 0 )
      analyzeSpikes( events( SpikeEvents[k] ), k );

  // nerve:
  if ( NerveTrace[0] >= 0 ) {
    const InData &nd = trace( NerveTrace[0] );
    // nerve amplitudes:
    // peak and trough amplitudes:
    double threshold = nd.stdev( signalTime(), signalTime()+4.0/FishRate );
    if ( threshold < 1.0e-8 )
      threshold = 0.001;
    EventList peaktroughs( 2, (int)rint(1500.0*(Before+Duration+After)), true );
    InData::const_iterator firstn = nd.begin( signalTime()-Before );
    InData::const_iterator lastn = nd.begin( signalTime()+Duration+After );
    if ( lastn > nd.end() )
      lastn = nd.end();
    Detector< InDataIterator, InDataTimeIterator > D;
    D.init( firstn, lastn, nd.timeBegin( signalTime()-Before ) );
    D.peakTrough( firstn, lastn, peaktroughs, threshold,
		  threshold, threshold, NerveAcceptEOD );
    // store amplitudes:
    NerveAmplP.push_back( MapD() );
    NerveAmplT.push_back( MapD() );
    NerveAmplM.push_back( MapD() );
    NerveAmplP.back().reserve( peaktroughs[0].size() );
    NerveAmplT.back().reserve( peaktroughs[0].size() );
    NerveAmplM.back().reserve( peaktroughs[0].size() );
    for ( int k=0; k<peaktroughs[0].size() && k<peaktroughs[1].size(); k++ ) {
      NerveAmplP.back().push( peaktroughs[0][k] - signalTime(),
			      peaktroughs[0].eventSize( k ) );
      NerveAmplT.back().push( peaktroughs[1][k] - signalTime(), 
			      peaktroughs[1].eventSize( k ) );
    }
    // averaged amplitude:
    double st = (peaktroughs[0].back() - peaktroughs[0].front())/double(peaktroughs[0].size()-1);
    double left = signalTime() - Before;
    for ( int k=0; k<NerveAmplP.back().size(); k++ ) {
      NerveAmplM.back().push( left-signalTime(), nd.mean( left, left+st ) );
      left += st;
    }
    // nerve mean amplitudes:
    average( NerveMeanAmplP, NerveAmplP );
    average( NerveMeanAmplT, NerveAmplT );
    average( NerveMeanAmplM, NerveAmplM );
    // power spectrum:
    SampleDataD psd( NervePower.size() );
    SampleDataD nerve( 0.0, Duration, 0.001 );
    nerve.interpolate( NerveAmplM.back() );
    nerve -= mean( nerve );
    rPSD( nerve, psd );
    for ( int j=0; j<psd.size(); j++ )
      NervePower[j] += ( psd[j] - NervePower[j] )/NerveAmplP.size();
    NervePower.setRange( 0.0, psd.stepsize() );
  }

}


addRePro( FileStimulus, efish );

}; /* namespace efish */

#include "moc_filestimulus.cc"
