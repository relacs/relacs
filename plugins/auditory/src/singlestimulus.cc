/*
  auditory/singlestimulus.cc
  Output of a single stimulus stored in a file.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/auditory/session.h>
#include <relacs/auditory/singlestimulus.h>
using namespace relacs;

namespace auditory {


SingleStimulus::SingleStimulus( void )
  : RePro( "SingleStimulus", "auditory", "Jan Benda", "1.4", "Jul 19, 2014" )
{
  Intensity = 50.0;
  Amplitude = 1.0;
  Duration = 0.0;
  CarrierFreq = 5000.0;
  Repeats = 10;
  SkipWin = 0.1;
  Sigma1 = 0.002;
  Sigma2 = 0.02;

  // options:
  newSection( "Waveform" );
  addSelection( "type", "Type of stimulus", "Wave|Envelope|AM" );
  addSelection( "waveform", "Stimulus waveform", "From file|Const|Sine|Rectangular|Triangular|Sawup|Sawdown|Whitenoise|OUnoise" );
  addText( "stimfile", "Stimulus file", "" ).setStyle( OptWidget::BrowseExisting ).setActivation( "waveform", "From file" );
  addBoolean( "stimscale", "Scale stimulus to maximum amplitudes", false ).setActivation( "waveform", "From file" );
  addNumber( "stimhighcut", "Cutoff frequency of high-pass filter applied to stimulus", 0.0, 0.0, 1000000.0, 10.0, "Hz", "Hz" ).setActivation( "waveform", "From file" );
  addNumber( "stimampl", "Amplitude factor (standard deviation) of stimulus file", 0.0, 0.0, 1.0, 0.01 ).setActivation( "waveform", "From file" );
  addNumber( "amplitude", "Amplitude of stimulus", Amplitude, 0.0, 130.0, 1.0, "dB" ).setActivation( "type", "AM" );
  addSelection( "freqsel", "Specify", "frequency|period|number of periods" ).setActivation( "waveform", "From file|Const|Sweep", false );
  addNumber( "freq", "Frequency of waveform", 10.0, 0.0, 1000000.0, 1.0, "Hz" ).setActivation( "freqsel", "frequency" ).addActivation( "waveform", "From file|Const|Sweep", false );
  addNumber( "period", "Period of waveform", 0.1, 0.0, 1000000.0, 0.001, "s", "ms" ).setActivation( "freqsel", "period" ).addActivation( "waveform", "From file|Const|Sweep", false );
  addNumber( "numperiods", "Number of periods", 1.0, 0.0, 1000000.0, 1.0 ).setActivation( "freqsel", "number of periods" ).addActivation( "waveform", "From file|Const|Sweep", false );
  addSelection( "pulsesel", "Specify", "pulse duration|duty-cycle" ).setActivation( "waveform", "Rectangular" );
  addNumber( "pulseduration", "Pulse duration", 0.01, 0.0, 10000.0, 0.001, "s", "ms" ).setActivation( "pulsesel", "pulse duration" ).addActivation( "waveform", "Rectangular" );
  addNumber( "dutycycle", "Duty-cycle", 0.5, 0.0, 1.0, 0.05, "1", "%" ).setActivation( "pulsesel", "duty-cycle" ).addActivation( "waveform", "Rectangular" );
  addInteger( "seed", "Seed for random number generation", 0 ).setActivation( "waveform", "Whitenoise|OUnoise" );
  addNumber( "duration", "Maximum duration of stimulus", Duration, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "ramp", "Ramp of stimulus", 0.002, 0.0, 10.0, 0.001, "seconds", "ms" );
  newSection( "Stimulus" );
  addNumber( "intensity", "Stimulus intensity", Intensity, -200.0, 200.0, 5.0, "dB" );
  addSelection( "intensitybase", "Stimulus intensity relative to", "SPL|threshold|rate|previous" );
  addNumber( "repeats", "Number of stimulus presentations", Repeats, 0, 10000, 1, "times" ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "pause", "Duration of pause between stimuli", 1.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addSelection( "side", "Speaker", "left|right|best" );
  newSubSection( "Carrier" );
  addNumber( "carrierfreq", "Carrier frequency", CarrierFreq, -40000.0, 40000.0, 500.0, "Hz", "kHz" );
  addBoolean( "usebestfreq", "Relative to the cell's best frequency", true );
  newSection( "Intensity - search" );
  addBoolean( "userate", "Search intensity for target firing rate", true );
  addNumber( "rate", "Target firing rate", 100.0, 0.0, 1000.0, 10.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "ratetol", "Tolerance for target firing rate", 5.0, 0.0, 1000.0, 1.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "intensitystep", "Initial intensity step", 8.0, 0.0, 100.0, 1.0, "dB" ).setActivation( "userate", "true" );
  addNumber( "searchrepeats", "Number of search stimulus presentations", 2, 1, 100, 1, "times" ).setActivation( "userate", "true" );
  addNumber( "silentrate", "Ignore response below", 0.0, 0.0, 1000.0, 5.0, "Hz" ).setActivation( "userate", "true" );
  addInteger( "maxsilent", "Number of stimulus presentations if response is below silentrate", 1, 1, 1000, 1 ).setActivation( "userate", "true" );
  addBoolean( "skippause", "Skip pause if response is below silentrate", true ).setActivation( "userate", "true" );
  addInteger( "maxsearch", "Stop search if response does not change for more than", 1, 1, 1000, 1 ).setUnit( "intensities" ).setActivation( "userate", "true" );
  addSelection( "method", "Method", "Bisect|Interpolate" ).setActivation( "userate", "true" );
  addNumber( "minslope", "Minimum slope required for interpolation", 4.0, 0.0, 100.0, 0.5, "Hz/dB" ).setActivation( "userate", "true" );
  addNumber( "searchduration", "Maximum duration of stimulus", 0.0, 0.0, 1000.0, 0.01, "seconds", "ms" ).setActivation( "userate", "true" );
  addNumber( "searchpause", "Duration of pause between stimuli", 0.0, 0.0, 1000.0, 0.01, "seconds", "ms" ).setActivation( "userate", "true" );
  newSection( "Analysis" );
  addNumber( "skipwin", "Initial portion of stimulus not used for analysis", SkipWin, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "sigma1", "Standard deviation of rate smoothing kernel 1", Sigma1, 0.0, 1.0, 0.0001, "seconds", "ms" );
  addNumber( "sigma2", "Standard deviation of rate smoothing kernel 2", Sigma2, 0.0, 1.0, 0.001, "seconds", "ms" );
  addNumber( "before", "Time before stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "after", "Time after stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
  addBoolean( "adjust", "Adjust input gain", true );
  newSubSection( "Save stimuli" );
  addSelection( "storemode", "Save stimuli in", "session|repro|custom" ).setUnit( "path" );
  addText( "storepath", "Save stimuli in custom directory", "" ).setStyle( OptWidget::BrowseDirectory ).setActivation( "storemode", "custom" );
  addSelection( "storelevel", "Save", "all|am+generated|generated|noise|none" ).setUnit( "stimuli" );

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
  SP[0].setBMarg( 1.0 );
  SP[0].noXTics();
  SP[0].setYRange( 0.0, Plot::AutoScale );
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
  SP[1].setYLabel( "Stimulus [dB]" );
  SP[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP[2].setOrigin( 0.7, 0.1 );
  SP[2].setSize( 0.3, 0.8 );
  SP[2].setLMarg( 7.0 );
  SP[2].setRMarg( 1.5 );
  SP[2].setTMarg( 2.0 );
  SP[2].setBMarg( 5.0 );
  SP[2].setXLabel( "Intensity [dB SPL]" );
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
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[0].setYLabel( "Firing rate [Hz]" );
  P[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 1.0, 0.5 );
  P[1].setLMarg( 7.0 );
  P[1].setRMarg( 1.5 );
  P[1].setTMarg( 0.5 );
  P[1].setBMarg( 5.0 );
  P[1].setXLabel( "Time [ms]" );
  P[1].setYLabel( "Stimulus [dB]" );
  P[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P.unlock();
  Stack->addWidget( &P );
  Stack->setCurrentWidget( &P );

}


SingleStimulus::~SingleStimulus( void )
{
}


int SingleStimulus::main( void )
{
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 ) {
    warning( "No spike trace!" );
    return Failed;
  }

  // get options:
  WaveType = (WaveTypes)index( "type" );
  WaveForm = (WaveForms)index( "waveform" );
  Str stimfile = text( "stimfile" );
  StimHighCut = boolean( "stimhighcut" );
  StimScale = boolean( "stimscale" );
  PeakAmplitudeFac = number( "stimampl" );
  int freqsel = index( "freqsel" );
  Frequency = number( "freq" );
  double period = number( "period" );
  double numperiods = number( "numperiods" );
  int pulsesel = index( "pulsesel" );
  PulseDuration = number( "pulseduration" );
  DutyCycle = number( "dutycycle" );
  Seed = integer( "seed" );
  Amplitude = number( "amplitude" );
  double intensity = number( "intensity" );
  int intensitybase = index( "intensitybase" );
  Repeats = integer( "repeats" );
  Duration = number( "duration" );
  double pause = number( "pause" );
  Ramp = number( "ramp" );
  CarrierFreq = number( "carrierfreq" );
  bool usebestfreq = boolean( "usebestfreq" );
  Side = index( "side" );
  bool userate = boolean( "userate" );
  double targetrate = number( "rate" );
  double ratetolerance = number( "ratetol" );
  double minintensitystep = 0.5;  // get this from the attenuator!!
  double intensitystep = number( "intensitystep" );
  int searchrepeats = integer( "searchrepeats" );
  double silentrate = number( "silentrate" );
  int maxsilent = integer( "maxsilent" );
  bool skippause = boolean( "skippause" );
  int maxsearch = integer( "maxsearch" );
  int searchmethod = index( "method" );
  double minslope = number( "minslope" );
  double searchduration = number( "searchduration" );
  if ( searchduration <= 0.0 )
    searchduration = Duration;
  double searchpause = number( "searchpause" );
  if ( searchpause <= 0.0 )
    searchpause = pause;
  SkipWin = number( "skipwin" );
  Sigma1 = number( "sigma1" );
  Sigma2 = number( "sigma2" );
  double before = number( "before" );
  if ( before > pause )
    before = pause;
  double after = number( "after" );
  if ( after > pause )
    after = pause;
  bool adjustgain = boolean( "adjust" );
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

  // stimulus frequency:
  if ( freqsel == 1 ) { // period
    if ( period < 1.0e-8 ) {
      warning( "The period must be greater than zero!" );
      return Aborted;
    }
    Frequency = 1.0/period;
  }
  else if ( freqsel == 2 ) // number of periods
    Frequency = numperiods/Duration;

  // pulse duration:
  if ( pulsesel == 1 )
    PulseDuration = DutyCycle/Frequency;
  else
    DutyCycle = -1.0;

  lockMetaData();
  if ( Side > 1 )
    Side = metaData().index( "Cell>best side" );
  string sidestr = Side > 0 ? "right" :  "left";
  if ( usebestfreq ) {
    double cf = metaData().number( "Cell>" + sidestr + " frequency" );
    if ( cf > 0.0 )
      CarrierFreq += cf;
  }
  unlockMetaData();
  if ( fabs( CarrierFreq ) < 1e-7 ) {
    warning( "Carrier frequency is set to zero!" );
    return Failed;
  }
  if ( intensitybase == 1 || intensitybase == 2 ) {
    auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
    if ( as == NULL ) {
      warning( "Can not get the neuron's threshold intensity. <br>No auditory::Session-plugin found." );
      return Failed;
    }
    MapD thresh = as->threshCurve( Side );
    if ( thresh.empty() ) {
      warning( "Can not get the neuron's threshold intensity. <br>No threshold curve was measured so far.");
      return Failed;
    }
    int k=0;
    for ( k=0; k < thresh.size() && thresh.x(k) < CarrierFreq; k++ );
    double intthresh = 0.0;
    if ( k == 0 && ::fabs( thresh.x( k ) - CarrierFreq ) < 1.0 )
      intthresh = thresh.y( k );
    else if ( k >= thresh.size() && ::fabs( thresh.x( thresh.size()-1 ) - CarrierFreq ) < 1.0 )
      intthresh = thresh.y( thresh.size()-1 );
    else if ( k == 0 || k >= thresh.size() ) {
      warning( "Can not get the neuron's threshold intensity. <br>Requested carrier frequency of " +
	       Str( 0.001*CarrierFreq ) +
	       " kHz is outside the measured range of the threshold curve." );
      return Failed;
    }
    else
      intthresh = thresh.y(k) + ( CarrierFreq - thresh.x(k) )*( thresh.y(k) - thresh.y(k-1) )/( thresh.x(k) - thresh.x(k-1) );
    if ( intensitybase == 1 )  // relative to threshold
      Intensity = intensity + intthresh;
    else if ( intensitybase == 2 ) {  // relative to target rate intensity
      lockMetaData();
      Intensity = intensity
	+ intthresh + metaData().number( "Cell>" + sidestr + " intensity" )
	- metaData().number( "Cell>" + sidestr + " threshold" );
      unlockMetaData();
    }
  }
  else if ( intensitybase == 3 )  // relative to previous intensity
    Intensity += 0.0;
  else  // dB SPL
    Intensity = intensity;

  string wavetypes[3] = { "Wave", "Envelope", "AM" };

  bool sameduration = ( Duration == searchduration );
  bool storedstimulus = false;

  OutData signal;

  // search for intensity that evokes the target firing rate:
  if ( userate ) {

    // stimulus:
    int r = createStimulus( signal, stimfile, searchduration, sameduration );
    if ( r < 0 )
      return Failed;
    storedstimulus = sameduration;
    
    // plot trace:
    tracePlotSignal( searchduration );
    
    postCustomEvent( 12 );
    SP.lock();
    SP.clearPlots();
    SP[0].setTitle( "Search target firing rate " + Str( targetrate ) + " Hz" );
    SP.setDrawBackground();
    SP.draw();
    SP.unlock();

    MapD rates;
    rates.reserve( 20 );

    // get f-I curve:
    auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
    if ( as == 0 )
      warning( "No auditory session!", 4.0 );
    else {
      MapD fic = as->ssFICurve( Side, CarrierFreq );
      if ( fic.empty() )
	fic = as->fICurve( Side, CarrierFreq );
      // find first guess for intensity (default is supplied by the user!):
      for ( int k=fic.size()-1; k >= 0; k-- ) {
	if ( fic[k] <= targetrate ) {
	  if ( k+1 < fic.size() &&
	       ::fabs( fic[k+1] - targetrate ) < ::fabs( fic[k] - targetrate ) )
	    Intensity = fic.x(k+1);
	  else
	    Intensity = fic.x(k);
	  break;
	}
      }
    }

    // check intensity:
    signal.setIntensity( Intensity + PeakAmplitude );
    testWrite( signal );
    Intensity = signal.intensity() - PeakAmplitude;
    
    double minint = Intensity;
    double maxint = Intensity;
    double maxf = 50.0;

    // search intensity:
    while ( true ) {

      signal.setIntensity( Intensity + PeakAmplitude );
    
      EventList spikes;

      for ( int counter=0; counter<searchrepeats; counter++ ) {
	
	// message:
	Str s = "Search rate <b>" + Str( targetrate ) + " Hz</b>";
	s += ",  " + wavetypes[WaveType] + "<b>" + StimulusLabel + "</b>";
	s += ":  Intensity: <b>" + Str( Intensity, 0, 0, 'f' ) + " dB SPL</b>";
	s += " at <b>" + Str( 0.001*CarrierFreq, 0, 1, 'f' ) + " kHz</b>";
	s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( searchrepeats ) + "</b>";
	message( s );

	// output:
	write( signal );
	if ( ! signal.success() )
	  break;
	if ( interrupt() ) {
	  writeZero( Speaker[ Side ] );
	  return Aborted;
	}

	// analyze:
	spikes.push( events( SpikeEvents[0] ), signalTime(), signalTime()+searchduration );
	double rate = spikes.back().rate( SkipWin, searchduration );
	double meanrate = spikes.rate( SkipWin, searchduration );
	SampleDataD rate2( 0.0, searchduration, 0.0005 );
	spikes.rate( rate2, GaussKernel( Sigma2 ) );
	
	// plot:
	{
	  SP.lock();
	  // spikes and firing rate:
	  SP[0].clear();
	  if ( ! SP[0].zoomedXRange() && ! SP[1].zoomedXRange() )
	    SP[0].setXRange( 1000.0*SkipWin, 1000.0*searchduration );
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
	  SP[0].plot( rate2, 1000.0, Plot::Orange, 2, Plot::Solid );

	  // stimulus:
	  lockMetaData();
	  double threshold = metaData().number( "Cell>best threshold" );
	  unlockMetaData();
	  double ymin = Intensity - PeakAmplitude;
	  double ymax = Intensity + PeakAmplitude;
	  if ( threshold > 0.0 ) {
	    if ( ymin > threshold )
	      ymin = threshold;
	    if ( ymax < threshold )
	      ymax = threshold;
	  }
	  SP[1].clear();
	  if ( ! SP[0].zoomedXRange() && ! SP[1].zoomedXRange() )
	    SP[1].setXRange( 1000.0*SkipWin, 1000.0*searchduration );
	  if ( ! SP[1].zoomedYRange() )
	    SP[1].setYRange( ymin - 1.0, ymax + 1.0 );
	  SP[1].plot( AMDB+Intensity+PeakAmplitude, 1000.0, Plot::Green, 2 );
	  if ( Frequency > 0.0 )
	    SP[1].plotHLine( Intensity, Plot::White, 2 );
	  if ( threshold > 0.0 )
	    SP[1].plotHLine( threshold, Plot::Yellow, 2 );

	  // firing rate versus stimulus intensity:
	  SP[2].clear();
	  double mini = minint;
	  double maxi = maxint;
	  if ( maxi - mini < intensitystep ) {
	    mini -= 0.5*intensitystep;
	    maxi += 0.5*intensitystep;
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
	  mr.push( Intensity, meanrate );
	  SP[2].plot( mr, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Red, Plot::Red );
	  MapD cr;
	  cr.push( Intensity, rate );
	  SP[2].plot( cr, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Yellow, Plot::Yellow );
	  SP.draw();
	  SP.unlock();
	}

	if ( rate >= silentrate ) {
	  sleep( searchpause );
	  if ( interrupt() ) {
	    writeZero( Speaker[ Side ] );
	    return Aborted;
	  }
	  // adjust gain of daq board:
	  if ( adjustgain ) {
	    adjust( trace( SpikeTrace[0] ), signalTime(), 
		    signalTime() + searchduration, 0.8 );
	  }
	}
	else {
	  if ( ! skippause ) {
	    sleep( searchpause );
	    if ( interrupt() ) {
	      writeZero( Speaker[ Side ] );
	      return Aborted;
	    }
	  }
	  if ( counter+1 >= maxsilent )
	    break;
	}
      
	if ( softStop() > 1 ) {
	  writeZero( Speaker[ Side ] );
	  return Failed;
	}

      }

      double rate = spikes.rate( SkipWin, searchduration );
      int rinx = 0;
      if ( signal.success() )
	rinx = rates.insert( Intensity, rate );

      if ( softStop() > 0 )
	break;

      // new intensity:
      if ( signal.success() && 
	   ( fabs( rate - targetrate ) < ratetolerance ||
	     intensitystep < minintensitystep ) ) {
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
	    writeZero( Speaker[ Side ] );
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
	      writeZero( Speaker[ Side ] );
	      return Failed;
	    }
	  }
	  // rate below target rate:
	  if ( Intensity < maxint )
	    intensitystep *= 0.5;
	  if ( searchmethod == 1 && rates.size() > 1 ) {
	    int sinx = rinx;
	    if ( sinx+1 >= rates.size() )
	      sinx--;
	    double slope = (rates.x(sinx+1)-rates.x(sinx))/(rates.y(sinx+1)-rates.y(sinx));
	    if ( 1.0/slope > minslope )
	      Intensity = slope*(targetrate - rates.y(rinx)) + rates.x(rinx);
	    else
	      Intensity += intensitystep;
	  }
	  else
	    Intensity += intensitystep;
	  if ( Intensity > maxint )
	    maxint = Intensity;
	}
	else if ( ( signal.success() && rate > targetrate ) || signal.overflow() ) {
	  // overflow:
	  if ( rates.size() > 0 && Intensity > rates.x().back() &&
	       intensitystep < minintensitystep ) {
	    info( "Intensity needed for targetrate is too high!" );
	    writeZero( Speaker[ Side ] );
	    return Failed;
	  }
	  // rate above target rate:
	  if ( Intensity > minint )
	    intensitystep *= 0.5;
	  if ( searchmethod == 1 && rates.size() > 1 ) {
	    int sinx = rinx-1;
	    if ( sinx < 0 )
	      sinx++;
	    double slope = (rates.x(sinx+1)-rates.x(sinx))/(rates.y(sinx+1)-rates.y(sinx));
	    if ( 1.0/slope > minslope )
	      Intensity = slope*(targetrate - rates.y(rinx)) + rates.x(rinx);
	    else
	      Intensity -= intensitystep;
	  }
	  else
	    Intensity -= intensitystep;
	  if ( Intensity < minint )
	    minint = Intensity;
	}
	else if ( signal.error() ) {
	  warning( "Output of stimulus failed!<br>Signal error: <b>" +
		   signal.errorText() + "</b><br>Exit now!" );
	  writeZero( Speaker[ Side ] );
	  return Failed;
	}
	else {
	  warning( "Could not establish firing rate!" );
	  writeZero( Speaker[ Side ] );
	  return Failed;
	}
      }

    }

  }

  // stimulus:
  if ( ! sameduration || ! storedstimulus ) {
    int r = createStimulus( signal, stimfile, Duration, true );
    if ( r < 0 ) {
      writeZero( Speaker[ Side ] );
      return Failed;
    }
  }
  AMDB += Intensity + PeakAmplitude;

  // check intensity:
  signal.setIntensity( Intensity + PeakAmplitude );
  testWrite( signal );
  Intensity = signal.intensity() - PeakAmplitude;

  // plot trace:
  if ( Duration < 2.0 )
    tracePlotSignal( Duration );
  else
    tracePlotContinuous( 2.0 );

  // setup plots:
  postCustomEvent( 11 );
  P.lock();
  P.clearPlots();
  P[0].setTitle( "Mean firing rate =    Hz" );
  P.setDrawBackground();
  P.draw();
  P.unlock();

  // variables:
  EventList spikes;
  MeanRate = 0.0;
  SampleDataD rate1( -before, Duration+after, 0.001, 0.0 );
  SampleDataD rate2( -before, Duration+after, 0.001, 0.0 );

  timeStamp();

  // output stimulus:  
  for ( int counter=0; ( Repeats <= 0 || counter<Repeats ) && softStop() == 0; counter++ ) {
    
    // message:
    Str s =  wavetypes[WaveType] + ": <b>" + StimulusLabel + "</b>";
    s += ",  Intensity: <b>" + Str( Intensity, 0, 1, 'f' ) + " dB SPL</b>";
    s += " at <b>" + Str( 0.001*CarrierFreq, 0, 1, 'f' ) + " kHz</b>";
    if ( WaveType == AM )
      s += ",  Amplitude: <b>" + Str( Amplitude, 0, 1, 'f' ) + " dB</b>";
    s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( Repeats ) + "</b>";
    message( s );
    
    // output:
    write( signal );
    if ( signal.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
	       signal.errorText() + "</b>," +
	       "<br> Loop: <b>" + Str( counter+1 ) + "</b>" +
	       "<br>Exit now!" );
      writeZero( Speaker[ Side ] );
      return Failed;
    }

    sleep( pause > 0.01 ? 0.01 : pause );

    if ( interrupt() ) {
      save( spikes, rate1, rate2 );
      writeZero( Speaker[ Side ] );
      return Aborted;
    }
    
    analyze( spikes, rate1, rate2, before, after );
    plot( spikes, rate1, rate2 );
    
    // adjust gain of daq board:
    if ( adjustgain ) {
      adjust( trace( SpikeTrace[0] ), signalTime(),
	      signalTime() + Duration, 0.8 );
    }

    sleepOn( Duration + pause );
    timeStamp();
    
  }
  
  save( spikes, rate1, rate2 );
  writeZero( Speaker[ Side ] );
  return Completed;
}


void SingleStimulus::saveSpikes( Options &header, const EventList &spikes )
{
  // create file:
  ofstream df( addPath( "singlestimulus-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );

  // write data:
  spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
  df << '\n';
}


void SingleStimulus::saveRate( Options &header, const SampleDataD &rate1,
			       const SampleDataD &rate2 )
{
  // create file:
  ofstream df( addPath( "singlestimulus-rate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.addNumber( "r" + Str( 1000.0*Sigma1 ) + "ms", "Hz", "%5.1f" );
  key.addNumber( "r" + Str( 1000.0*Sigma2 ) + "ms", "Hz", "%5.1f" );
  key.addNumber( "I", "dB SPL", "%7.2f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<rate1.size(); k++ ) {
    double t = rate1.pos( k );
    key.save( df, t * 1000.0, 0 );
    key.save( df, rate1[k] );
    key.save( df, rate2[k] );
    if ( t < 0.0 || t > AMDB.rangeBack() )
      key.save( df, 0.0 );
    else
      key.save( df, AMDB.interpolate( rate1.pos( k ) ) );
    df << '\n';
  }
  df << "\n\n";
}


void SingleStimulus::save( const EventList &spikes, const SampleDataD &rate1,
			   const SampleDataD &rate2 )
{
  Options header;
  header.addInteger( "index1", totalRuns() );
  header.addNumber( "carrier frequency", 0.001*CarrierFreq, "kHz", "%.3f" );
  header.addInteger( "side", Side );
  header.addNumber( "intensity", Intensity, "dB SPL", "%.1f" );
  header.addNumber( "amplitude", Amplitude, "dB", "%.1f" );
  header.addNumber( "amplfac", PeakAmplitudeFac, "", "%.3f" );
  header.addNumber( "duration", 1000.0*Duration, "ms", "%.1f" );
  header.addText( "envelope", StoreFile );
  header.addText( "session time", sessionTimeStr() ); 
  lockStimulusData();
  header.newSection( stimulusData() );
  unlockStimulusData();
  header.newSection( settings() );

  saveSpikes( header, spikes );
  saveRate( header, rate1, rate2 );
}


void SingleStimulus::plot( const EventList &spikes, const SampleDataD &rate1,
			   const SampleDataD &rate2 )
{
  P.lock();
  // spikes and firing rate:
  P[0].clear();
  P[0].setTitle( "Mean firing rate = " + Str( MeanRate, 0, 0, 'f' ) + "Hz" );
  if ( ! P[0].zoomedXRange() && ! P[1].zoomedXRange() )
    P[0].setXRange( 1000.0*SkipWin, 1000.0*Duration );
  if ( ! P[0].zoomedYRange() )
    P[0].setYRange( 0.0, Plot::AutoScale );
  int maxspikes	= (int)rint( 20.0 / SpikeTraces );
  if ( maxspikes < 4 )
    maxspikes = 4;
  int offs = (int)spikes.size() > maxspikes ? spikes.size() - maxspikes : 0;
  double delta = Repeats > 0 && Repeats < maxspikes ? 1.0/Repeats : 1.0/maxspikes;
  for ( int i=offs, j=0; i<spikes.size(); i++ ) {
    j++;
    P[0].plot( spikes[i], 0, 0.0, 1000.0, 1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp, delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
  }
  P[0].plot( rate1, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P[0].plot( rate2, 1000.0, Plot::Orange, 2, Plot::Solid );

  // stimulus:
  lockMetaData();
  double threshold = metaData().number( "Cell>best threshold" );
  unlockMetaData();
  double ymin = Intensity - PeakAmplitude;
  double ymax = Intensity + PeakAmplitude;
  if ( WaveType == Envelope )
    ymin = AMDB.min( Ramp, Duration-Ramp );
  else if ( WaveType == Wave ) {
    ymin = AMDB.min( Ramp, Duration-Ramp );
    ymax = AMDB.max( Ramp, Duration-Ramp );
    if ( ymax < Intensity )
      ymax = Intensity;
    if ( ymin > Intensity )
      ymin = Intensity;
  }
  if ( threshold > 0.0 ) {
    if ( ymin > threshold )
      ymin = threshold;
    if ( ymax < threshold )
      ymax = threshold;
  }
  P[1].clear();
  if ( ! P[0].zoomedXRange() && ! P[1].zoomedXRange() )
    P[1].setXRange( 1000.0*SkipWin, 1000.0*Duration );
  /*
  P[1].setYFallBackRange( ymin - 1.0, ymax + 1.0 );
  if ( ! P[1].zoomedYRange() )
    P[1].setYRange( Plot::AutoMinScale, Plot::AutoMinScale );
  */
  if ( ! P[1].zoomedYRange() )
    P[1].setYRange( ymin - 1.0, ymax + 1.0 );
  P[1].plotHLine( Intensity, Plot::White, 2 );
  if ( threshold > 0.0 )
    P[1].plotHLine( threshold, Plot::Yellow, 2 );
  P[1].plot( AMDB, 1000.0, Plot::Green, 2 );

  P.draw();

  P.unlock();
}


void SingleStimulus::analyze( EventList &spikes, SampleDataD &rate1,
			      SampleDataD &rate2, double before, double after )
{
  if ( SpikeEvents[0] < 0 )
    return;

  // spikes:
  spikes.push( events( SpikeEvents[0] ), signalTime()-before,
	       signalTime() + Duration + after, signalTime() );
  int trial1 = spikes.size()-1;
  int trial2 = spikes.size()-1;

  MeanRate = spikes.rate( SkipWin, Duration );
  spikes.back().addRate( rate1, trial1, GaussKernel( Sigma1 ) );
  spikes.back().addRate( rate2, trial2, GaussKernel( Sigma2 ) );
}


int SingleStimulus::createStimulus( OutData &signal, const Str &file,
				    double &duration, bool stoream )
{
  OutData wave;
  wave.setTrace( Speaker[ Side ] );

  string wavename;
  bool store = false;
  Options header;

  if ( WaveForm == File ) {
    // load stimulus from file:
    wave.load( file, file );
    if ( wave.empty() ) {
      warning( "Unable to load stimulus from file " + file );
      return -1;
    }
    if ( duration > 0.0 && wave.length() > duration )
      wave.resize( wave.indices( duration ) );
    duration = wave.length();
    if ( StimScale )
      wave /= maxAbs( wave );
    if ( StimHighCut > 0.0 ) {
      double fac = wave.stepsize()*StimHighCut;
      double x = wave[0];
      for ( int k=0; k<wave.size(); k++ ) {
	x += ( wave[k] - x )*fac;
	wave[k] -= x;
      }
    }
    if ( PeakAmplitudeFac <= 0.0 )
      PeakAmplitudeFac = ::relacs::rms( wave );
    int c = ::relacs::clip( -1.0, 1.0, wave );
    double cp = 100.0*double(c)/wave.size();
    if ( cp > 0.0 )
      info( "Clipped " + Str( cp, 0, 3, 'g' ) + "% of the stimulus waveform.", 4.0 );
    wavename = file.notdir();
    header.addText( "file", file );
    header.addText( "filename", file.longName() + "-" );
    if ( Duration < 1.0 )
      header.addText( "duration", Str( 1000.0*duration ) + "ms" );
    else
      header.addText( "duration", Str( duration ) + "s" );
  }
  else if ( WaveForm >= Sine && WaveForm <= OUnoise ) {
    string waveforms[7] = { "Sine", "Rectangular", "Triangular", "Saw-up", "Saw-down", "Whitenoise", "OU-noise" };
    header.addText( "waveform", waveforms[WaveForm-2] );
    header.addText( "frequency", Str( Frequency ) + "Hz" );
    if ( WaveForm == Sine || WaveForm == Whitenoise || WaveForm == OUnoise ) {
      if ( WaveForm == Sine ) {
	PeakAmplitudeFac = 1.0;
	wave.sineWave( duration, -1.0, Frequency );
      }
      else {
	unsigned long seed = Seed;
	if ( WaveForm == Whitenoise )
	  wave.noiseWave( duration, -1.0, Frequency, 1.0, &seed );
	else if ( WaveForm == OUnoise )
	  wave.ouNoiseWave( duration, -1.0, 1.0/Frequency, 1.0, &seed );
	PeakAmplitudeFac = 0.3;
	wave *= PeakAmplitudeFac;
	int c = ::relacs::clip( -1.0, 1.0, wave );
	double cp = 100.0*double(c)/wave.size();
	if ( cp > 0.0 )
	  info( "Clipped " + Str( cp, 0, 3, 'g' ) + "% of the stimulus waveform.", 4.0 );
	header.addInteger( "random seed", int( seed ) );
      }
      if ( WaveType == Envelope )
	wave = 0.5*(wave+1.0);
    }
    else {
      PeakAmplitudeFac = 1.0;
      if ( WaveForm == Rectangular ) {
	wave.rectangleWave( duration, -1.0, 1.0/Frequency, PulseDuration, Ramp );
	if ( DutyCycle >= 0.0 )
	  header.addText( "dutycycle", Str( 100.0*DutyCycle ) + "%" );
	else
	  header.addText( "pulseduration", Str( 1000.0*PulseDuration ) + "ms" );
      }
      else if ( WaveForm == Triangular )
	wave.triangleWave( duration, -1.0, 1.0/Frequency );
      else if ( WaveForm == Sawup )
	wave.sawUpWave( duration, -1.0, 1.0/Frequency, Ramp );
      else if ( WaveForm == Sawdown )
	wave.sawDownWave( duration, -1.0, 1.0/Frequency, Ramp );
      if ( WaveType != Envelope )
	wave = 2.0*wave - 1.0;
    }

    if ( StoreLevel == Noise && 
	 ( WaveForm == Whitenoise || WaveForm == OUnoise ) ) 
      store = true;
    if ( StoreLevel == AMGenerated || StoreLevel == Generated ) 
      store = true;
  }
  else {
    // constant:
    WaveType = Envelope;
    wave.pulseWave( duration, 0.001, 1.0, 0.0 );
    PeakAmplitudeFac = 1.0;
    header.addText( "waveform", "const" );
    if ( StoreLevel == AMGenerated || StoreLevel == Generated ) 
      store = true;
  }
  header.addText( "ramp", Str( 1000.0*Ramp ) + "ms" );
  if ( StoreLevel == All ) 
    store = true;

  // check wave:
  if ( duration <= SkipWin ) {
    warning( "Stimulus duration <b>" + Str( 1000.0* duration, 0, 0, 'f' ) + "ms</b>" +
	     " is smaller than analysis window!" );
    return -1;
  }

  Str labelpattern = "$(waveform)$(filename)$(frequency)$(random seed) $(dutycycle)";
  StimulusLabel = translate( labelpattern, header );
  if ( wavename.empty() )
    wavename = StimulusLabel;

  signal.clear();
  signal.setTrace( Speaker[ Side ] );
  if ( WaveType == AM ) {
    if ( StoreLevel == AMGenerated ) 
      store = true;
    PeakAmplitude = Amplitude / PeakAmplitudeFac;
    AMDB = PeakAmplitude * ( wave - 1.0 );
    OutData am( AMDB );
    for ( int k=0; k<am.size(); k++ )
      am[k] = ::pow (10.0, AMDB[k] / 20.0 );
    signal.fill( am, CarrierFreq );
    signal.setIdent( "am=" + wavename + ", amplitude=" + Str( Amplitude ) + "dB" );
    header.addText( "amplitude", Str( Amplitude ) + "dB" );
    header.addText( "intensityfactor", Str( ::pow (10.0, -PeakAmplitude / 20.0 ), 0, 3, 'g' ) );
    if ( stoream && store ) {
      Str filepattern = "$(waveform)$(filename)$(frequency)$(random seed)$(dutycycle)r$(ramp)$(duration)$(amplitude)$(intensityfactor).dat";
      StoreFile = StorePath + translate( filepattern, header );
      ifstream cf( StoreFile.c_str() );
      if ( ! cf ) {
	ofstream of( StoreFile.c_str() );
	header.erase( "filename" );
	header.save( of, "# " );
	of << '\n';
	of << "#Key\n";
	of << "# t   x\n";
	of << "# s   1\n";
	am.save( of, 7, 5 );
	printlog( "wrote " + StoreFile );
      }
    }
  }
  else if ( WaveType == Envelope ) {
    if ( ::relacs::min( wave ) < 0.0 ) {
      warning( "This envelope contains negative values!" );
      return -1;
    }
    wave.ramp( Ramp );
    PeakAmplitude = -20.0 * ::log10( PeakAmplitudeFac );
    Amplitude = 0.0;
    AMDB = wave;
    for ( int k=0; k<AMDB.size(); k++ ) {
      AMDB[k] = 20.0 * ::log10( wave[k] );
      if ( AMDB[k] < -60.0 )
	AMDB[k] = -60.0;
    }
    signal.fill( wave, CarrierFreq );
    signal.setIdent( "envelope=" + wavename );
    header.addText( "intensityfactor", Str( PeakAmplitudeFac, 0, 3, 'g' ) );
    if ( stoream && store ) {
      Str filepattern = "$(waveform)$(filename)$(frequency)$(random seed)$(dutycycle)r$(ramp)$(duration)$(intensityfactor).dat";
      StoreFile = StorePath + translate( filepattern, header );
      ifstream cf( StoreFile.c_str() );
      if ( ! cf ) {
	ofstream of( StoreFile.c_str() );
	header.erase( "filename" );
	header.save( of, "# " );
	of << '\n';
	of << "#Key\n";
	of << "# t   x\n";
	of << "# s   1\n";
	wave.save( of, 7, 5 );
	printlog( "wrote " + StoreFile );
      }
    }
  }
  else { // Wave
    // this should go into OutData::fixSample():
    if ( wave.stepsize() < signal.minSampleInterval() )
      signal.interpolate( wave, 0.0, signal.minSampleInterval() );
    else
      signal = wave;
    signal.setCarrierFreq( CarrierFreq );
    signal.setIdent( "wave=" + wavename );
    PeakAmplitude = -20.0 * ::log10( PeakAmplitudeFac );
    static const double EnvTau = 0.0002;
    AMDB = wave;
    double x = wave[0]*wave[0];
    for ( int k=0; k<AMDB.size(); k++ ) {
      double v = AMDB[k];
      x += ( v*v - x )*AMDB.stepsize()/EnvTau;
      AMDB[k] = sqrt( 2.0*x );
      AMDB[k] = 20.0 * ::log10( AMDB[k] );
      if ( AMDB[k] < -60.0 )
	AMDB[k] = -60.0;
    }
  }
  signal.ramp( Ramp );
  
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


addRePro( SingleStimulus, auditory );

}; /* namespace auditory */

#include "moc_singlestimulus.cc"
