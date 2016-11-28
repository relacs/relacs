/*
  patchclamp/singlestimulus.cc
  Output of a single stimulus stored in a file.

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
#include <ctype.h>
#include <relacs/optwidget.h>
#include <relacs/tablekey.h>
#include <relacs/translate.h>
#include <relacs/stats.h>
#include <relacs/kernel.h>
#include <relacs/random.h>
#include <relacs/patchclamp/singlestimulus.h>
using namespace relacs;

namespace patchclamp {


SingleStimulus::SingleStimulus( void )
  : RePro( "SingleStimulus", "patchclamp", "Jan Benda", "1.8", "Feb 26, 2015" )
{
  WaveForm = Sine;
  IUnit = "nA";
  Amplitude = 1.0;

  // options:
  newSection( "Waveform" );
  addSelection( "waveform", "Stimulus waveform", "From file|Const|Sine|Rectangular|Triangular|Sawup|Sawdown|Alpha|Whitenoise|OUnoise|Sweep" );
  addText( "stimfile", "Stimulus file", "" ).setStyle( OptWidget::BrowseExisting ).setActivation( "waveform", "From file" );
  addNumber( "stimampl", "Amplitude factor (standard deviation) of stimulus file", 0.0, 0.0, 1.0, 0.01 ).setActivation( "waveform", "From file" );
  addNumber( "amplitude", "Amplitude of stimulus", Amplitude, 0.0, 10000.0, 1.0, IUnit ).setActivation( "waveform", "Const", false );;
  addSelection( "freqsel", "Specify", "frequency|period|number of periods" ).setActivation( "waveform", "From file|Const|Sweep", false );
  addNumber( "freq", "Frequency of waveform", 10.0, 0.0, 1000000.0, 1.0, "Hz" ).setActivation( "freqsel", "frequency" ).addActivation( "waveform", "From file|Const|Sweep", false );
  addNumber( "period", "Period of waveform", 0.1, 0.0, 1000000.0, 0.001, "s", "ms" ).setActivation( "freqsel", "period" ).addActivation( "waveform", "From file|Const|Sweep", false );
  addNumber( "numperiods", "Number of periods", 1.0, 0.0, 1000000.0, 1.0 ).setActivation( "freqsel", "number of periods" ).addActivation( "waveform", "From file|Const|Sweep", false );
  addSelection( "pulsesel", "Specify", "pulse duration|duty-cycle" ).setActivation( "waveform", "Rectangular" );
  addNumber( "pulseduration", "Pulse duration", 0.01, 0.0, 10000.0, 0.001, "s", "ms" ).setActivation( "pulsesel", "pulse duration" ).addActivation( "waveform", "Rectangular" );
  addNumber( "dutycycle", "Duty-cycle", 0.5, 0.0, 1.0, 0.05, "1", "%" ).setActivation( "pulsesel", "duty-cycle" ).addActivation( "waveform", "Rectangular" );
  addNumber( "tau", "Time-constant", 1.0, 0.0, 100000.0, 0.001, "s", "ms" ).setActivation( "waveform", "Alpha" );
  addInteger( "seed", "Seed for random number generation", 0 ).setActivation( "waveform", "Whitenoise|OUnoise" );;
  addNumber( "startfreq", "Start sweep with frequency", 1.0, 0.0, 100000.0, 1.0, "Hz", "Hz", "%.2f" ).setActivation( "waveform", "Sweep" );
  addNumber( "endfreq", "End sweep with frequency", 100.0, 0.0, 100000.0, 1.0, "Hz", "Hz", "%.2f" ).setActivation( "waveform", "Sweep" );
  addNumber( "cycleramp", "Ramp for each cycle", 0.0, 0.0, 10.0, 0.001, "seconds", "ms" ).setActivation( "waveform", "Rectangular|Sawup|Sawdown" );
  addNumber( "duration", "Maximum duration of stimulus", 1.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "ramp", "Ramp for stimulus onset and offset", 0.0, 0.0, 10.0, 0.001, "seconds", "ms" );
  newSection( "Stimulus" );
  addNumber( "offset", "Stimulus mean", 0.0, -2000.0, 2000.0, 5.0, IUnit );
  addSelection( "offsetbase", "Stimulus mean relative to", "absolute|amplitude|current|threshold|previous" );
  addBoolean( "samerate", "Use sampling rate of input", true ).setActivation( "waveform", "From file", false );
  addNumber( "samplerate", "Sampling rate of output", 1000.0, 0.0, 10000000.0, 1000.0, "Hz", "kHz" ).setActivation( "samerate", "true", false );
  addNumber( "repeats", "Number of stimulus presentations", 10, 0, 10000, 1, "times" ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "pause", "Duration of pause between stimuli", 1.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "before", "Time before stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "after", "Time after stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
  addSelection( "outtrace", "Output trace", "V-1" );
  newSection( "Offset - search" );
  addBoolean( "userate", "Search offset for target firing rate", false );
  addNumber( "rate", "Target firing rate", 100.0, 0.0, 1000.0, 10.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "ratetol", "Tolerance for target firing rate", 5.0, 0.0, 1000.0, 1.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "offsetstep", "Initial offset step", 8.0, 0.0, 100.0, 1.0, IUnit ).setActivation( "userate", "true" );
  addNumber( "searchrepeats", "Number of search stimulus presentations", 2, 1, 100, 1, "times" ).setActivation( "userate", "true" );
  addNumber( "silentrate", "Ignore response below", 0.0, 0.0, 1000.0, 5.0, "Hz" ).setActivation( "userate", "true" );
  addInteger( "maxsilent", "Number of stimulus presentations if response is below silentrate", 1, 1, 1000, 1 ).setActivation( "userate", "true" );
  addBoolean( "skippause", "Skip pause if response is below silentrate", true ).setActivation( "userate", "true" );
  addInteger( "maxsearch", "Stop search if response does not change for more than", 1, 1, 1000, 1 ).setUnit( "intensities" ).setActivation( "userate", "true" );
  addSelection( "method", "Method", "Bisect|Interpolate" ).setActivation( "userate", "true" );
  addNumber( "minslope", "Minimum slope required for interpolation", 4.0, 0.0, 100.0, 0.5, "Hz/"+IUnit ).setActivation( "userate", "true" );
  addNumber( "searchduration", "Maximum duration of stimulus", 0.0, 0.0, 1000.0, 0.01, "seconds", "ms" ).setActivation( "userate", "true" );
  addNumber( "searchpause", "Duration of pause between stimuli", 0.0, 0.0, 1000.0, 0.01, "seconds", "ms" ).setActivation( "userate", "true" );
  newSection( "Analysis" );
  addNumber( "skipwin", "Initial portion of stimulus not used for analysis", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "sigma", "Standard deviation of rate smoothing kernel", 0.01, 0.0, 1.0, 0.0001, "seconds", "ms" );
  addBoolean( "storevoltage", "Save voltage trace", true );
  addSelection( "plot", "Plot shows", "Current voltage trace|Mean voltage trace|Current and mean voltage trace|Firing rate" );
  newSubSection( "Save stimuli" );
  addSelection( "storemode", "Save stimuli in", "session|repro|custom" ).setUnit( "path" );
  addText( "storepath", "Save stimuli in custom directory", "" ).setStyle( OptWidget::BrowseDirectory ).setActivation( "storemode", "custom" );
  addSelection( "storelevel", "Save", "all|generated|noise|none" ).setUnit( "stimuli" );

  // setup plots:
  Stack = new QStackedLayout;
  setLayout( Stack );

  SP.lock();
  SP.resize( 3 );
  SP.setCommonXRange( 0, 1 );
  SP[0].setOrigin( 0.0, 0.5 );
  SP[0].setSize( 0.7, 0.5 );
  SP[0].setLMarg( 7.0 );
  SP[0].setRMarg( 1.5 );
  SP[0].setTMarg( 3.5 );
  SP[0].noXTics();
  SP[0].setYLabel( "Firing rate [Hz]" );
  SP[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP[1].setOrigin( 0.0, 0.0 );
  SP[1].setSize( 0.7, 0.5 );
  SP[1].setLMarg( 7.0 );
  SP[1].setRMarg( 1.5 );
  SP[1].setTMarg( 0.5 );
  SP[1].setBMarg( 5.0 );
  SP[1].setXLabel( "Time [ms]" );
  SP[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP[2].setOrigin( 0.7, 0.1 );
  SP[2].setSize( 0.3, 0.8 );
  SP[2].setLMarg( 7.0 );
  SP[2].setRMarg( 1.5 );
  SP[2].setTMarg( 2.0 );
  SP[2].setBMarg( 5.0 );
  SP[2].setYLabel( "Firing rate [Hz]" );
  SP[2].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP.unlock();
  Stack->addWidget( &SP );

  P.lock();
  P.resize( 2 );
  P.setCommonXRange( 0, 1 );
  P[0].setOrigin( 0.0, 0.5 );
  P[0].setSize( 1.0, 0.5 );
  P[0].setLMarg( 7.0 );
  P[0].setRMarg( 1.5 );
  P[0].setTMarg( 3.5 );
  P[0].setBMarg( 1.0 );
  P[0].noXTics();
  P[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 1.0, 0.48 );
  P[1].setLMarg( 7.0 );
  P[1].setRMarg( 1.5 );
  P[1].setTMarg( 0.5 );
  P[1].setBMarg( 5.0 );
  P[1].setXLabel( "Time [ms]" );
  P[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P.unlock();
  Stack->addWidget( &P );
  Stack->setCurrentWidget( &P );

}


SingleStimulus::~SingleStimulus( void )
{
}


void SingleStimulus::preConfig( void )
{
  if ( SpikeTrace[0] >= 0 )
    VUnit = trace( SpikeTrace[0] ).unit();
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


void SingleStimulus::notify( void )
{
  int outtrace = index( "outtrace" );
  if ( outtrace >= 0 && outtrace < outTracesSize() ) {
    IUnit = outTrace( outtrace ).unit();
    setUnit( "amplitude", IUnit );
    setUnit( "offset", IUnit );
    setUnit( "offsetstep", IUnit );
    setUnit( "minslope", "Hz/"+IUnit );
  }
}


int SingleStimulus::main( void )
{
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 ) {
    warning( "No spike trace!" );
    return Failed;
  }

  // store options:
  settings().setValueTypeFlags( 16, -Parameter::Section );

  // get options:
  WaveForm = (WaveForms)index( "waveform" );
  Str stimfile = text( "stimfile" );
  PeakAmplitudeFac = number( "stimampl" );
  int freqsel = index( "freqsel" );
  Frequency = number( "freq" );
  double period = number( "period" );
  double numperiods = number( "numperiods" );
  int pulsesel = index( "pulsesel" );
  PulseDuration = number( "pulseduration" );
  Tau = number( "tau" );
  DutyCycle = number( "dutycycle" );
  Seed = integer( "seed" );
  StartFreq = number( "startfreq" );
  EndFreq = number( "endfreq" );
  Amplitude = number( "amplitude" );
  double offset = number( "offset" );
  int offsetbase = index( "offsetbase" );
  bool samerate = boolean( "samerate" );
  double samplerate = number( "samplerate" );
  int repeats = integer( "repeats" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  double cycleramp = number( "cycleramp" );
  double ramp = number( "ramp" );
  int outtrace = index( "outtrace" );
  bool userate = boolean( "userate" );
  double targetrate = number( "rate" );
  double ratetolerance = number( "ratetol" );
  double minoffsetstep = 0.5;  // get this from the attenuator!! XXX
  double offsetstep = number( "offsetstep" );
  int searchrepeats = integer( "searchrepeats" );
  double silentrate = number( "silentrate" );
  int maxsilent = integer( "maxsilent" );
  bool skippause = boolean( "skippause" );
  int maxsearch = integer( "maxsearch" );
  int searchmethod = index( "method" );
  double minslope = number( "minslope" );
  double searchduration = number( "searchduration" );
  if ( searchduration <= 0.0 )
    searchduration = duration;
  double searchpause = number( "searchpause" );
  if ( searchpause <= 0.0 )
    searchpause = pause;
  double skipwin = number( "skipwin" );
  double sigma = number( "sigma" );
  double before = number( "before" );
  if ( before > pause )
    before = pause;
  double after = number( "after" );
  if ( after > pause )
    after = pause;
  bool storevoltage = boolean( "storevoltage" );
  int plotmode = index( "plot" );
  StoreModes storemode = (StoreModes)index( "storemode" );
  if ( storemode == SessionPath )
    StorePath = addPath( "" );
  else if ( storemode == ReProPath )
    StorePath = reproPath();
  else if ( storemode == CustomPath ) {
    StorePath = text( "storepath" );
    if ( ! StorePath.empty() )
      StorePath.provideSlash();
  }
  else
    StorePath = "";
  StoreLevel = (StoreLevels)index( "storelevel" );
  StoreFile = "";

  lockStimulusData();
  double dccurrent = stimulusData().number( outTraceName( outtrace ) );
  unlockStimulusData();
  if ( offsetbase == 1 ) // amplitude
    Offset = offset + Amplitude;
  else if ( offsetbase == 2 ) // current
    Offset = offset + dccurrent;
  else if ( offsetbase == 3 ) { // threshold
    lockMetaData();
    Offset = offset + metaData().number( "Cell>ithreshss" );
    if ( fabs( Offset - offset ) < 1.0e-8 )
      Offset = offset + metaData().number( "Cell>ithreshon" );
    unlockMetaData();
  }
  else if ( offsetbase == 4 )  // previous
    Offset += 0.0;
  else   // absolute
    Offset = offset;

  // check stimulus duration:
  if ( WaveForm >= Sine && WaveForm <= OUnoise ) {
    if ( duration < 1.0e-8 ) {
      warning( "The stimulus duration must be greater than zero!" );
      return Aborted;
    }
  }

  // stimulus frequency:
  if ( freqsel == 1 ) { // period
    if ( period < 1.0e-8 ) {
      warning( "The period must be greater than zero!" );
      return Aborted;
    }
    Frequency = 1.0/period;
  }
  else if ( freqsel == 2 ) // number of periods
    Frequency = numperiods/duration;

  // pulse duration:
  if ( pulsesel == 1 )
    PulseDuration = DutyCycle/Frequency;
  else
    DutyCycle = -1.0;

  bool sameduration = ( duration == searchduration );
  bool storedstimulus = false;

  // signal:
  OutData signal;
  signal.setTrace( outtrace );
  if ( samerate )
    samplerate = trace( eventInputTrace( SpikeEvents[0] ) ).sampleRate();
  else if ( samplerate <= 0.0 )
    samplerate = signal.maxSampleRate();

  // dc signal:
  OutData dcsignal;
  dcsignal.setTrace( outtrace );
  dcsignal.constWave( dccurrent );
  dcsignal.setIdent( "DC=" + Str( dccurrent ) + IUnit );

  // search for offset that evokes the target firing rate:
  if ( userate ) {

    // stimulus:
    int r = createStimulus( signal, stimfile, searchduration,
			    1.0/samplerate, ramp, cycleramp,
			    skipwin, sameduration );
    if ( r < 0 )
      return Failed;
    storedstimulus = sameduration;
    
    // plot trace:
    tracePlotSignal( searchduration );
    
    postCustomEvent( 12 );
    SP.lock();
    SP.clearPlots();
    SP[0].setTitle( "Search target firing rate " + Str( targetrate ) + " Hz" );
    SP[1].setYLabel( "Stimulus [" + IUnit + "]" );
    SP[2].setXLabel( "Offset [" + IUnit + "]" );
    SP.draw();
    SP.unlock();

    MapD rates;
    rates.reserve( 20 );

    // get f-I curve:
    /* XXX
    auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
    if ( as == 0 )
      warning( "No auditory session!", 4.0 );
    else {
      MapD fic = as->ssFICurve();
      if ( fic.empty() )
	fic = as->fICurve();
      // find first guess for offset (default is supplied by the user!):
      for ( int k=fic.size()-1; k >= 0; k-- ) {
	if ( fic[k] <= targetrate ) {
	  if ( k+1 < fic.size() &&
	       ::fabs( fic[k+1] - targetrate ) < ::fabs( fic[k] - targetrate ) )
	    Offset = fic.x(k+1);
	  else
	    Offset = fic.x(k);
	  break;
	}
      }
    }

    */

    // check signal:
    signal += Offset;
    
    double minint = Offset;
    double maxint = Offset;
    double maxf = 50.0;

    double offsetincr = 0.0;

    // search offset:
    while ( true ) {

      Offset += offsetincr;
      signal += offsetincr;
      signal.back() = dccurrent;
    
      EventList spikes;

      for ( int counter=0; counter<searchrepeats; counter++ ) {
	
	// message:
	Str s = "Search rate <b>" + Str( targetrate ) + " Hz</b>";
	s += ",  <b>" + StimulusLabel + "</b>";
	s += ":  Offset: <b>" + Str( Offset, 0, 0, 'f' ) + " " + IUnit + "</b>";
	s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( searchrepeats ) + "</b>";
	message( s );

	// output:
	write( signal );
	if ( ! signal.success() )
	  break;
	if ( interrupt() ) {
	  directWrite( dcsignal );
	  return Aborted;
	}

	// analyze:
	spikes.push( events( SpikeEvents[0] ), signalTime(),
		     signalTime()+searchduration );
	double rate = spikes.back().rate( skipwin, searchduration );
	double meanrate = spikes.rate( skipwin, searchduration );
	SampleDataD ratepsth( 0.0, searchduration, 0.0005 );
	spikes.rate( ratepsth, GaussKernel( sigma ) );
	
	// plot:
	{
	  SP.lock();
	  // spikes and firing rate:
	  SP[0].clear();
	  if ( ! SP[0].zoomedXRange() && ! SP[1].zoomedXRange() )
	    SP[0].setXRange( 1000.0*skipwin, 1000.0*searchduration );
	  if ( ! SP[0].zoomedYRange() )
	    SP[0].setYRange( 0.0, Plot::AutoScale );
	  int maxspikes	= (int)rint( 20.0 / searchrepeats );
	  if ( maxspikes < 4 )
	    maxspikes = 4;
	  int offs = (int)spikes.size() > maxspikes ? spikes.size() - maxspikes : 0;
	  double delta = searchrepeats > 0 && searchrepeats < maxspikes ? 1.0/searchrepeats : 1.0/maxspikes;
	  for ( int i=offs, j=0; i<spikes.size(); i++ ) {
	    j++;
	    SP[0].plot( spikes[i], 0, 0.0, 1000.0, 1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp, delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
	  }
	  SP[0].plot( ratepsth, 1000.0, Plot::Orange, 2, Plot::Solid );

	  // stimulus:
	  lockMetaData();
	  double threshold = metaData().number( "Cell>best threshold" );
	  unlockMetaData();
	  double ymin = Offset - PeakAmplitude;
	  double ymax = Offset + PeakAmplitude;
	  if ( threshold > 0.0 ) {
	    if ( ymin > threshold )
	      ymin = threshold;
	    if ( ymax < threshold )
	      ymax = threshold;
	  }
	  SP[1].clear();
	  if ( ! SP[0].zoomedXRange() && ! SP[1].zoomedXRange() )
	    SP[1].setXRange( 1000.0*skipwin, 1000.0*searchduration );
	  if ( ! SP[1].zoomedYRange() )
	    SP[1].setYRange( ymin - 1.0, ymax + 1.0 );
	  SP[1].plot( signal, 1000.0, Plot::Green, 2 );
	  if ( Frequency > 0.0 )
	    SP[1].plotHLine( Offset, Plot::White, 2 );
	  if ( threshold > 0.0 )
	    SP[1].plotHLine( threshold, Plot::Yellow, 2 );

	  // firing rate versus stimulus offset:
	  SP[2].clear();
	  double mini = minint;
	  double maxi = maxint;
	  if ( maxi - mini < offsetstep ) {
	    mini -= 0.5*offsetstep;
	    maxi += 0.5*offsetstep;
	  }
	  if ( ! SP[2].zoomedXRange() )
	    SP[2].setXRange( mini, maxi );
	  if ( rate+50.0 > maxf )
	    maxf = ::ceil((rate+50.0)/20.0)*20.0;
	  if ( maxf < targetrate ) 
	    maxf = targetrate;
	  if ( ! SP[2].zoomedYRange() )
	    SP[2].setYRange( 0.0, maxf );
	  SP[2].plotHLine( targetrate, Plot::White, 2 );
	  SP[2].plot( rates, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Red, Plot::Red );
	  MapD mr;
	  mr.push( Offset, meanrate );
	  SP[2].plot( mr, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Red, Plot::Red );
	  MapD cr;
	  cr.push( Offset, rate );
	  SP[2].plot( cr, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Yellow, Plot::Yellow );
	  SP.draw();
	  SP.unlock();
	}

	if ( rate >= silentrate ) {
	  sleep( searchpause );
	  if ( interrupt() ) {
	    directWrite( dcsignal );
	    return Aborted;
	  }
	}
	else {
	  if ( ! skippause ) {
	    sleep( searchpause );
	    if ( interrupt() ) {
	      directWrite( dcsignal );
	      return Aborted;
	    }
	  }
	  if ( counter+1 >= maxsilent )
	    break;
	}
      
	if ( softStop() > 1 ) {
	  directWrite( dcsignal );
	  return Failed;
	}

      }

      double rate = spikes.rate( skipwin, searchduration );
      int rinx = 0;
      if ( signal.success() )
	rinx = rates.insert( Offset, rate );

      if ( softStop() > 0 )
	break;

      // new offset:
      offsetincr = 0.0;
      if ( signal.success() && 
	   ( fabs( rate - targetrate ) < ratetolerance ||
	     offsetstep < minoffsetstep ) ) {
	// ready:
	break;
      }
      else {
	// cell lost?
	if ( signal.success() && rate < silentrate &&
	     rinx > 0 && rates.y( rinx-1 ) > 2.0*silentrate ) {
	  if ( ( rinx < rates.size()-1 && 
		 rates.y( rinx+1 ) > 2.0*silentrate ) ||
	       rates.y( rinx-1 ) > silentrate + 0.3*(targetrate - silentrate ) ) {
	    info( "Cell probably lost!" );
	    directWrite( dcsignal );
	    return Failed;
	  }
	}
	if ( ( signal.success() && rate < targetrate ) || signal.underflow() ) {
	  // saturation?
	  if ( rinx == rates.size()-1 && rinx > 0 &&
	       ::relacs::max( rates.y() ) > silentrate ) {
	    int satcount = 0;
	    for ( int k = rinx-1; k >= 0; k-- )
	      if ( fabs( rates.y( k ) - rate ) < ratetolerance )
		satcount++;
	    if ( satcount >= maxsearch ) {
	      info( "Target firing rate probably too high!" );
	      directWrite( dcsignal );
	      return Failed;
	    }
	  }
	  // rate below target rate:
	  if ( Offset < maxint )
	    offsetstep *= 0.5;
	  if ( searchmethod == 1 && rates.size() > 1 ) {
	    int sinx = rinx;
	    if ( sinx+1 >= rates.size() )
	      sinx--;
	    double slope = (rates.x(sinx+1)-rates.x(sinx))/(rates.y(sinx+1)-rates.y(sinx));
	    if ( 1.0/slope > minslope )
	      offsetincr = slope*(targetrate - rates.y(rinx)) + rates.x(rinx) - Offset;
	    else
	      offsetincr = offsetstep;
	  }
	  else
	    offsetincr = offsetstep;
	  if ( Offset + offsetincr > maxint )
	    maxint = Offset + offsetincr;
	}
	else if ( ( signal.success() && rate > targetrate ) || signal.overflow() ) {
	  // overflow:
	  if ( rates.size() > 0 && Offset > rates.x().back() &&
	       offsetstep < minoffsetstep ) {
	    info( "Offset needed for targetrate is too high!" );
	    directWrite( dcsignal );
	    return Failed;
	  }
	  // rate above target rate:
	  if ( Offset > minint )
	    offsetstep *= 0.5;
	  if ( searchmethod == 1 && rates.size() > 1 ) {
	    int sinx = rinx-1;
	    if ( sinx < 0 )
	      sinx++;
	    double slope = (rates.x(sinx+1)-rates.x(sinx))/(rates.y(sinx+1)-rates.y(sinx));
	    if ( 1.0/slope > minslope )
	      offsetincr = slope*(targetrate - rates.y(rinx)) + rates.x(rinx) - Offset;
	    else
	      offsetincr = -offsetstep;
	  }
	  else
	    offsetincr = -offsetstep;
	  if ( Offset + offsetincr < minint )
	    minint = Offset + offsetincr;
	}
	else if ( signal.failed() ) {
	  warning( "Output of stimulus failed!<br>Signal error: <b>" +
		   signal.errorText() + "</b><br>Exit now!" );
	  directWrite( dcsignal );
	  return Failed;
	}
	else {
	  warning( "Could not establish firing rate!" );
	  directWrite( dcsignal );
	  return Failed;
	}
      }

    }

  }

  // stimulus:
  if ( ! sameduration || ! storedstimulus ) {
    int r = createStimulus( signal, stimfile, duration, 
			    1.0/samplerate, ramp, cycleramp,
			    skipwin, true );
    if ( r < 0 ) {
      directWrite( dcsignal );
      return Failed;
    }
    signal += Offset;
    signal.back() = dccurrent;
  }

  // plot trace:
  tracePlotSignal( duration );

  // setup plots:
  postCustomEvent( 11 );
  P.lock();
  P.clearPlots();
  P[0].setXRange( -1000.0*before, 1000.0*(duration+after) );
  if ( plotmode < 3 ) {
    P[0].setYLabel( "Voltage [" + VUnit + "]" );
    P[0].setYRange( Plot::AutoScale, Plot::AutoScale );
  }
  else {
    P[0].setTitle( "CV =    , Mean firing rate =    Hz" );
    P[0].setYLabel( "Firing rate [Hz]" );
    P[0].setYRange( 0.0, Plot::AutoScale );
  }
  P[1].setXRange( -1000.0*before, 1000.0*(duration+after) );
  P[1].setYLabel( "Stimulus [" + IUnit + "]" );
  P.draw();
  P.unlock();

  // files:
  ofstream tf;
  TableKey tracekey;
  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "ReProIndex", reproCount() );
  header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  header.addNumber( "offset", Offset, IUnit, "%g" );
  header.addNumber( "amplitude", Amplitude, IUnit, "%g" );
  header.addNumber( "amplfac", PeakAmplitudeFac, "", "%.3f" );
  header.addNumber( "duration", 1000.0*duration, "ms", "%.1f" );
  header.addText( "envelope", StoreFile );
  lockStimulusData();
  header.newSection( stimulusData() );
  unlockStimulusData();
  header.newSection( settings() );

  // variables:
  EventList spikes;
  double meanrate = 0.0;
  double cvrate = 0.0;
  SampleDataD rate( -before, duration+after, 0.001, 0.0 );
  SampleDataF voltage( -before, duration+after, trace( SpikeTrace[0] ).stepsize(), 0.0 );
  SampleDataF meanvoltage( -before, duration+after, trace( SpikeTrace[0] ).stepsize(), 0.0 );
  SampleDataF current;
  SampleDataF meancurrent;
  if ( CurrentTrace[0] >= 0 ) {
    current.resize( -before, duration+after, trace( CurrentTrace[0] ).stepsize(), 0.0 );
    meancurrent.resize( -before, duration+after, trace( CurrentTrace[0] ).stepsize(), 0.0 );
  }
  int state = Completed;

  timeStamp();

  // stimulus loop:  
  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    
    // message:
    Str s =  "<b>" + StimulusLabel + "</b>";
    s += ",  Offset: <b>" + Str( Offset, 0, 1, 'f' ) + " " + IUnit + "</b>";
    s += ",  Amplitude: <b>" + Str( Amplitude, 0, 5, 'g' ) + " " + IUnit + "</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    if ( repeats > 0 )
      s += " of <b>" + Str( repeats ) + "</b>";
    message( s );
    
    // output:
    write( signal );
    if ( signal.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
	       signal.errorText() + "</b>," +
	       "<br> Loop: <b>" + Str( count+1 ) + "</b>" +
	       "<br>Exit now!" );
      directWrite( dcsignal );
      return Failed;
    }

    sleep( after );

    if ( interrupt() ) {
      if ( count == 0 )
	state = Aborted;
      break;
    }

    // voltage trace:
    trace( SpikeTrace[0] ).copy( signalTime(), voltage );
    for ( int k=0; k<voltage.size(); k++ )
      meanvoltage[k] += ( voltage[k] - meanvoltage[k] )/(count+1);

    // current trace:
    if ( CurrentTrace[0] >= 0 ) {
      trace( CurrentTrace[0] ).copy( signalTime(), current );
      for ( int k=0; k<current.size(); k++ )
	meancurrent[k] += ( current[k] - meancurrent[k] )/(count+1);
    }
    
    analyze( spikes, rate, meanrate, cvrate, duration, skipwin, sigma, before, after );
    plot( spikes, rate, signal, voltage, meanvoltage, meanrate, cvrate, duration, repeats );
    if ( storevoltage ) {
      if ( count == 0 )
	openTraceFile( tf, tracekey, header );
      saveTrace( tf, tracekey, count, voltage, current );
    }
    
    sleepOn( duration + pause );
    if ( interrupt() )
      break;
    timeStamp();
    
  }

  directWrite( dcsignal );
  
  if ( state == Completed ) {
    header.clearSections();
    header.addNumber( "firing rate", meanrate, "Hz", "%.1f" );
    header.addNumber( "CV", cvrate, "", "%.3f" );
    if ( storevoltage ) {
      tf << '\n';
      saveMeanTrace( header, tracekey, meanvoltage, meancurrent );
    }
    saveRate( header, rate, sigma );
    saveSpikes( header, spikes );
  }

  return state;
}


void SingleStimulus::openTraceFile( ofstream &tf, TableKey &tracekey,
				    const Options &header )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( CurrentTrace[0] >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.3f" );
  Str waveform = settings().text( "waveform" );
  if ( waveform == "From file" )
    waveform = "file";
  waveform.lower();
  tf.open( addPath( "stimulus-" + waveform + "-traces.dat" ).c_str(),
	   ofstream::out | ofstream::app );
  header.save( tf, "# ", 0, FirstOnly );
  tf << '\n';
  tracekey.saveKey( tf, true, false );
  tf << '\n';
}


void SingleStimulus::saveTrace( ofstream &tf, TableKey &tracekey, int index,
				const SampleDataF &voltage, const SampleDataF &current )
{
  tf << "# index: " << index << '\n';
  if ( ! current.empty() ) {
    for ( int k=0; k<voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.pos( k ), 0 );
      tracekey.save( tf, voltage[k] );
      tracekey.save( tf, current[k] );
      tf << '\n';
    }
  }
  else {
    for ( int k=0; k<voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.pos( k ), 0 );
      tracekey.save( tf, voltage[k] );
      tf << '\n';
    }
  }
  tf << '\n';
}


void SingleStimulus::saveMeanTrace( Options &header, TableKey &tracekey,
				    const SampleDataF &meanvoltage,
				    const SampleDataF &meancurrent )
{
  // create file:
  Str waveform = settings().text( "waveform" );
  if ( waveform == "From file" )
    waveform = "file";
  waveform.lower();
  ofstream df( addPath( "stimulus-" + waveform + "-meantraces.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, FirstOnly );
  df << '\n';
  tracekey.saveKey( df, true, false );

  // write data:
  if ( ! meancurrent.empty() ) {
    for ( int k=0; k<meanvoltage.size(); k++ ) {
      tracekey.save( df, meanvoltage.pos( k ) * 1000.0, 0 );
      tracekey.save( df, meanvoltage[k] );
      tracekey.save( df, meancurrent[k] );
      df << '\n';
    }
  }
  else {
    for ( int k=0; k<meanvoltage.size(); k++ ) {
      tracekey.save( df, meanvoltage.pos( k ) * 1000.0, 0 );
      tracekey.save( df, meanvoltage[k] );
      df << '\n';
    }
  }
  df << "\n\n";
}


void SingleStimulus::saveSpikes( Options &header, const EventList &spikes )
{
  // create file:
  Str waveform = settings().text( "waveform" );
  if ( waveform == "From file" )
    waveform = "file";
  waveform.lower();
  ofstream df( addPath( "stimulus-" + waveform + "-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );

  // write data:
  spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
  df << '\n';
}


void SingleStimulus::saveRate( Options &header, const SampleDataD &rate, double sigma )
{
  // create file:
  Str waveform = settings().text( "waveform" );
  if ( waveform == "From file" )
    waveform = "file";
  waveform.lower();
  ofstream df( addPath( "stimulus-" + waveform + "-rate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.addNumber( "r" + Str( 1000.0*sigma ) + "ms", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<rate.size(); k++ ) {
    key.save( df, rate.pos( k ) * 1000.0, 0 );
    key.save( df, rate[k] );
    df << '\n';
  }
  df << "\n\n";
}


void SingleStimulus::plot( const EventList &spikes, const SampleDataD &rate,
			   const OutData &signal, const SampleDataF &voltage,
			   const SampleDataF &meanvoltage, double meanrate,
			   double cvrate, double duration, int repeats )
{
  int plotmode = index( "plot" );
  P.lock();
  P[0].clear();
  P[0].plotVLine( 0.0, Plot::White, 2 );
  P[0].plotVLine( 1000.0*duration, Plot::White, 2 );
  if ( plotmode < 3 ) {
    if ( meanrate > 0.0 ) {
      if ( cvrate >= 0.0 )
	P[0].setTitle( "CV = " + Str( cvrate, 0, 2, 'f' ) + ", Mean firing rate = " + Str( meanrate, 0, 0, 'f' ) + "Hz" );
      else
	P[0].setTitle( "Mean firing rate = " + Str( meanrate, 0, 0, 'f' ) + "Hz" );
    }
    else
      P[0].setTitle( "" );
    P[0].setYLabel( "Voltage [" + VUnit + "]" );
    if ( ! P[0].zoomedYRange() )
      P[0].setYRange( Plot::AutoScale, Plot::AutoScale );
    if ( plotmode == 0 || plotmode == 2 )
      P[0].plot( voltage, 1000.0, Plot::Yellow, 2, Plot::Solid );
    if ( plotmode == 1 || plotmode == 2 )
      P[0].plot( meanvoltage, 1000.0, Plot::Red, 2, Plot::Solid );
  }
  else {
    // spikes and firing rate:
    if ( cvrate >= 0.0 )
      P[0].setTitle( "CV = " + Str( cvrate, 0, 2, 'f' ) + ", Mean firing rate = " + Str( meanrate, 0, 0, 'f' ) + "Hz" );
    else
      P[0].setTitle( "Mean firing rate = " + Str( meanrate, 0, 0, 'f' ) + "Hz" );
    P[0].setYLabel( "Firing rate [Hz]" );
    if ( ! P[0].zoomedYRange() )
      P[0].setYRange( 0.0, Plot::AutoScale );
    int maxspikes	= (int)rint( 20.0 / SpikeTraces );
    if ( maxspikes < 4 )
      maxspikes = 4;
    int offs = (int)spikes.size() > maxspikes ? spikes.size() - maxspikes : 0;
    double delta = repeats > 0 && repeats < maxspikes ? 1.0/repeats : 1.0/maxspikes;
    for ( int i=offs, j=0; i<spikes.size(); i++ ) {
      j++;
      P[0].plot( spikes[i], 0, 0.0, 1000.0, 1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp, delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
    }
    P[0].plot( rate, 1000.0, Plot::Yellow, 2, Plot::Solid );
  }

  // stimulus:
  lockMetaData();
  double threshold = metaData().number( "Cell>best threshold" );
  unlockMetaData();
  double ymin = Offset - PeakAmplitude;
  double ymax = Offset + PeakAmplitude;
  if ( threshold > 0.0 ) {
    if ( ymin > threshold )
      ymin = threshold;
    if ( ymax < threshold )
      ymax = threshold;
  }
  P[1].clear();
  if ( ! P[1].zoomedYRange() )
    P[1].setYRange( ymin - 0.05*PeakAmplitude, ymax + 0.05*PeakAmplitude );
  P[1].plotHLine( Offset, Plot::White, 2 );
  if ( threshold > 0.0 )
    P[1].plotHLine( threshold, Plot::Yellow, 2 );
  P[1].plotVLine( 0.0, Plot::White, 2 );
  P[1].plotVLine( 1000.0*duration, Plot::White, 2 );
  P[1].plot( signal, 1000.0, Plot::Green, 2 );

  P.draw();

  P.unlock();
}


void SingleStimulus::analyze( EventList &spikes, SampleDataD &rate, double &meanrate,
			      double &cvrate, double duration, double skipwin, double sigma,
			      double before, double after )
{
  if ( SpikeEvents[0] < 0 )
    return;

  // spikes:
  spikes.push( events( SpikeEvents[0] ), signalTime()-before,
	       signalTime() + duration+after, signalTime() );
  int trial = spikes.size()-1;

  meanrate = spikes.rate( skipwin, duration );
  double stdisi = 0.0;
  double meanisi = spikes.interval( skipwin, duration, stdisi );
  if ( meanisi > 0.0 )
    cvrate = stdisi/meanisi;
  else
    cvrate = -1.0;
  spikes.back().addRate( rate, trial, GaussKernel( sigma ) );
}


int SingleStimulus::createStimulus( OutData &signal, const Str &file,
				    double &duration, double deltat,
				    double ramp, double cycleramp,
				    double skipwin, bool storesignal )
{
  string wavename;
  bool store = false;
  Options header;

  signal.clear();
  if ( WaveForm == File ) {
    // load stimulus from file:
    signal.load( file );
    if ( signal.empty() ) {
      warning( "Unable to load stimulus from file " + file );
      return -1;
    }
    if ( duration > 0.0 && signal.length() > duration )
      signal.resize( signal.indices( duration ) );
    duration = signal.length();
    if ( PeakAmplitudeFac <= 0.0 )
      PeakAmplitudeFac = ::relacs::rms( signal );
    wavename = file.notdir();
    header.addText( "file", file );
    header.addText( "filename", file.longName() + "-" );
    if ( duration < 1.0 )
      header.addText( "duration", Str( 1000.0*duration ) + "ms" );
    else
      header.addText( "duration", Str( duration ) + "s" );
  }
  else if ( WaveForm >= Sine && WaveForm <= OUnoise ) {
    string waveforms[8] = { "Sine", "Rectangular", "Triangular", "Saw-up", "Saw-down", "Alpha", "Whitenoise", "OU-noise" };
    header.addText( "waveform", waveforms[WaveForm-2] );
    header.addNumber( "frequency", Frequency, "Hz" );
    if ( WaveForm == Sine || WaveForm == Alpha || WaveForm == Whitenoise || WaveForm == OUnoise ) {
      if ( WaveForm == Sine ) {
	PeakAmplitudeFac = 1.0;
	signal.sineWave( duration, deltat, Frequency );
      }
      else if ( WaveForm == Alpha ) {
	PeakAmplitudeFac = 1.0;
	signal.alphaWave( duration, deltat, 1.0/Frequency, Tau );
	header.addNumber( "tau", 1000.0*Tau, "ms" );
      }
      else {
	unsigned long seed = Seed;
	if ( WaveForm == Whitenoise )
	  signal.noiseWave( duration, deltat, Frequency, 1.0, &seed );
	else if ( WaveForm == OUnoise )
	  signal.ouNoiseWave( duration, deltat, 1.0/Frequency, 1.0, &seed );
	PeakAmplitudeFac = 0.3;
	header.addInteger( "random seed", int( seed ) );
      }
    }
    else {
      PeakAmplitudeFac = 1.0;
      if ( WaveForm == Rectangular ) {
	signal.rectangleWave( duration, deltat, 1.0/Frequency, PulseDuration, cycleramp );
	if ( DutyCycle >= 0.0 )
	  header.addNumber( "dutycycle", 100.0*DutyCycle, "%" );
	else
	  header.addNumber( "pulseduration", 1000.0*PulseDuration, "ms" );
      }
      else if ( WaveForm == Triangular )
	signal.triangleWave( duration, deltat, 1.0/Frequency );
      else if ( WaveForm == Sawup )
	signal.sawUpWave( duration, deltat, 1.0/Frequency, cycleramp );
      else if ( WaveForm == Sawdown )
	signal.sawDownWave( duration, deltat, 1.0/Frequency, cycleramp );
      signal = 2.0*signal - 1.0;
      if ( WaveForm != Triangular )
	header.addNumber( "cycleramp", 1000.0*cycleramp, "ms" );
    }

    if ( StoreLevel == Noise && 
	 ( WaveForm == Whitenoise || WaveForm == OUnoise ) ) 
      store = true;
    if ( StoreLevel == Generated ) 
      store = true;
  }
  else if ( WaveForm == Sweep ) {
    // frequency sweep:
    header.addText( "waveform", "Sweep" );
    header.addNumber( "startfrequency", StartFreq, "Hz" );
    header.addNumber( "endfrequency", EndFreq, "Hz" );
    PeakAmplitudeFac = 1.0;
    signal.sweepWave( duration, deltat, StartFreq, EndFreq );
    if ( StoreLevel == Generated ) 
      store = true;
  }
  else {
    // constant:
    signal.pulseWave( duration, deltat, 0.0, 0.0 );
    PeakAmplitudeFac = 1.0;
    header.addText( "waveform", "const" );
    if ( StoreLevel == Generated ) 
      store = true;
  }
  if ( StoreLevel == All ) 
    store = true;

  // check wave:
  if ( duration <= skipwin ) {
    warning( "Stimulus duration <b>" + Str( 1000.0* duration, 0, 0, 'f' ) + "ms</b>" +
	     " is smaller than analysis window!" );
    return -1;
  }
  /*
    int c = ::relacs::clip( -1.0, 1.0, signal );
    double cp = 100.0*double(c)/signal.size();
    if ( cp > 0.0 )
      info( "Clipped " + Str( cp, 0, 3, 'g' ) + "% of the stimulus waveform.", 4.0 );
   */

  Str labelpattern = "$(waveform)$(filename)$(frequency)$(random seed) $(pulseduration)$(dutycycle)$(tau)";
  StimulusLabel = translate( labelpattern, header );
  if ( wavename.empty() )
    wavename = StimulusLabel;

  signal *= Amplitude;
  signal.ramp( ramp );
  header.addNumber( "ramp", 1000.0*ramp, "ms" );
  PeakAmplitude = Amplitude / PeakAmplitudeFac;

  signal.setIdent( "signal=" + wavename + ", amplitude=" + Str( Amplitude ) + IUnit );
  header.addNumber( "amplitude", Amplitude, IUnit );
  header.addNumber( "amplitudefactor", PeakAmplitude );
  if ( storesignal && store ) {
    Str filepattern = "$(waveform)$(filename)$(frequency)$(random seed)$(pulseduration)$(dutycycle)$(tau)r$(ramp)$(duration)$(amplitude)$(amplitudefactor).dat";
    StoreFile = StorePath + translate( filepattern, header );
    ifstream cf( StoreFile.c_str() );
    if ( ! cf ) {
      ofstream of( StoreFile.c_str() );
      header.erase( "filename" );
      header.save( of, "# ", 0, FirstOnly );
      of << '\n';
      of << "#Key\n";
      of << "# t   x\n";
      of << "# s   1\n";
      signal.save( of, 7, 5 );
      printlog( "wrote " + StoreFile );;
    }
  }
  
  return 0;
}


void SingleStimulus::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    if ( ! P.tryLock( 2 ) ) {
      // we do not get the lock for the session now,
      // so we repost the event to a later time.
      postCustomEvent( 11 );
      return;
    }
    P.setDrawBackground();
    P.unlock();
    Stack->setCurrentWidget( &P );
  }
  else if ( qce->type() == QEvent::User+12 ) {
    if ( ! P.tryLock( 2 ) ) {
      // we do not get the lock for the session now,
      // so we repost the event to a later time.
      postCustomEvent( 12 );
      return;
    }
    P.setDrawBackground();
    P.unlock();
    Stack->setCurrentWidget( &SP );
  }
  else
    RePro::customEvent( qce );
}


addRePro( SingleStimulus, patchclamp );

}; /* namespace patchclamp */

#include "moc_singlestimulus.cc"
