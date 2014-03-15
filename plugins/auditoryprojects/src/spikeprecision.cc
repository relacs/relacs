/*
  auditoryprojects/spikeprecision.cc
  Assess spike precision in locust auditory receptors

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
#include <relacs/kernel.h>
#include <relacs/spectrum.h>
#include <relacs/tablekey.h>
#include <relacs/translate.h>
#include <relacs/auditory/session.h>
#include <relacs/auditoryprojects/spikeprecision.h>
using namespace relacs;

namespace auditoryprojects {


SpikePrecision::SpikePrecision( void )
  : RePro( "SpikePrecision", "auditoryprojects",
	   "Samuel Glauser, Jan Benda", "1.5", "Jan 10, 2008" )
{
  // default values for the options & other variables:
  CarrierFrequency	= 0.0;
  Side 			= 0;		// left ("0") or right ("1") speaker
  Amplitude 		= 4.0;		// amplitude of envelope
  Duration 		= 0.5;
  StimRepetition	= 10;
  Intensity		= 80.0;		// stimulus intensity
  SkipWindow            = 0.2;   // inital portion of stimulus that is not used for analysis
  Sigma1                = 0.001;
  Sigma2                = 0.02;
  FreqRange.clear();

  // options:
  newSection( "Stimulus" );
  addText( "freqrange", "Frequency range", "" ).setUnit( "Hz" );
  addNumber( "amplitude", "Amplitude of envelope", Amplitude, 0.0, 130.0, 1.0, "dB", "dB" );
  addNumber( "intensity", "Stimulus intensity", Intensity, 0.0, 200.0, 5.0, "dB SPL", "dB SPL" );
  addSelection( "intensitybase", "Stimulus intensity relative to", "SPL|threshold|rate|previous" );
  addNumber( "repeats", "Number of stimulus presentations", StimRepetition, 1, 100, 1, "times", "times" );
  addNumber( "duration", "Duration of stimulus", Duration, 0.01, 100.0, 0.01, "seconds", "ms" );
  addNumber( "pause", "Duration of pause between stimuli", 1.0, 0.01, 1000.0, 0.1, "seconds", "ms" );
  addNumber( "carrierfreq", "Carrier frequency", CarrierFrequency, -40000.0, 40000.0, 500.0, "Hz", "kHz" );
  addBoolean( "usebestfreq", "Relative to the cell's best frequency", true );
  addSelection( "side", "Speaker", "left|right|best" );
  newSection( "Waveform" );
  addSelection( "waveform", "Type of amplitude modulation", "sine|rectangular|triangular|sawup|sawdown|noise gap|noise cutoff" );
  addNumber( "dutycycle", "Duty-cycle of rectangular waveform", 0.5, 0.0, 1.0, 0.05, "1", "%" ).setActivation( "waveform", "rectangular" );
  addText( "stimfile", "Stimulus file", "" ).setStyle( OptWidget::BrowseExisting ).setActivation( "waveform", "noise gap|noise cutoff" );
  addNumber( "stimampl", "Amplitude factor (standard deviation) of stimulus file", 0.0, 0.0, 1.0, 0.01 ).setActivation( "waveform", "noise gap|noise cutoff" );
  addNumber( "relfreqgap", "Relative width of frequency gap", 0.2, 0.0, 1.0, 0.01, "1", "%" ).setActivation( "waveform", "noise gap" );
  addNumber( "absfreqgap", "Absolute width of frequency gap", 10.0, 0.0, 1000.0, 5.0, "Hz" ).setActivation( "waveform", "noise gap" );
  addBoolean( "rescale", "Rescale filtered noise-gap stimuli", true ).setActivation( "waveform", "noise gap" );
  addNumber( "ramp2", "Ramp for rectangles and saw tooths", 0.0, 0.0, 1.0, 0.001, "seconds", "ms" ).setActivation( "waveform", "rectangular|sawup|sawdown" );
  addNumber( "ramp", "Ramp at beginning and end of stimulus", 0.002, 0.0, 1.0, 0.001, "seconds", "ms" );
  newSection( "Intensity - search" );
  addBoolean( "userate", "Search intensity for target firing rate", true );
  addNumber( "rate", "Target firing rate", 100.0, 0.0, 1000.0, 10.0, "Hz", "Hz" ).setActivation( "userate", "true" );
  addNumber( "ratetol", "Tolerance for target firing rate", 5.0, 0.0, 1000.0, 1.0, "Hz", "Hz" ).setActivation( "userate", "true" );
  addNumber( "intensitystep", "Initial intensity step", 8.0, 0.0, 100.0, 1.0, "dB", "dB" ).setActivation( "userate", "true" );
  addNumber( "searchrepeats", "Number of search stimulus presentations", 2, 1, 100, 1, "times", "times" ).setActivation( "userate", "true" );
  newSection( "Analysis" );
  addNumber( "skipwin", "Initial portion of stimulus not used for analysis", SkipWindow, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "sigma1", "Standard deviation of rate smoothing kernel 1", Sigma1, 0.0, 1.0, 0.0001, "seconds", "ms" );
  addNumber( "sigma2", "Standard deviation of rate smoothing kernel 2", Sigma2, 0.0, 1.0, 0.001, "seconds", "ms" );
  newSubSection( "Save stimuli" );
  addSelection( "storemode", "Save stimuli in", "session|repro|custom" ).setUnit( "path" );
  addText( "storepath", "Save stimuli in custom directory", "" ).setStyle( OptWidget::BrowseDirectory ).setActivation( "storemode", "custom" );

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
  P.resize( 4 );
  P.setCommonXRange( 0, 1 );
  P[0].setOrigin( 0.0, 0.5 );
  P[0].setSize( 0.7, 0.5 );
  P[0].setLMarg( 7.0 );
  P[0].setRMarg( 1.5 );
  P[0].setTMarg( 3.5 );
  P[0].setBMarg( 1.0 );
  P[0].noXTics();
  P[0].setYLabel( "Firing rate [Hz]" );
  P[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 0.7, 0.5 );
  P[1].setLMarg( 7.0 );
  P[1].setRMarg( 1.5 );
  P[1].setTMarg( 0.5 );
  P[1].setBMarg( 5.0 );
  P[1].setXLabel( "Time [ms]" );
  P[1].setYLabel( "Stimulus [dB]" );
  P[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P.setCommonXRange( 2, 3 );
  P[2].setOrigin( 0.7, 0.5 );
  P[2].setSize( 0.3, 0.5 );
  P[2].setLMarg( 7.0 );
  P[2].setRMarg( 1.5 );
  P[2].setTMarg( 3.5 );
  P[2].setBMarg( 1.0 );
  P[2].setYLabel( "Correlation" );
  P[2].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[2].noXTics();
  P[3].setOrigin( 0.7, 0.0 );
  P[3].setSize( 0.3, 0.48 );
  P[3].setLMarg( 7.0 );
  P[3].setRMarg( 1.5 );
  P[3].setTMarg( 0.5 );
  P[3].setBMarg( 5.0 );
  P[3].setXLabel( "Frequency [Hz]" );
  P[3].setYLabel( "Firing rate [Hz]" );
  P[3].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P.unlock();
  Stack->addWidget( &P );
  Stack->setCurrentWidget( &P );
}


// Destructor - delete things here you have constructed in the repro constructor:
SpikePrecision::~SpikePrecision( void )
{
}


int SpikePrecision::main( void )
{
  // get options:
  string freqrangestr   = allText( "freqrange" );
  Amplitude 		= number( "amplitude" );
  double intensity	= number( "intensity" );
  int intensitybase	= index( "intensitybase" );
  WaveForm              = (WaveForms)index( "waveform" );
  string stimfile       = text( "stimfile" );
  PeakAmplitudeFac      = number( "stimampl" );
  DutyCycle             = number( "dutycycle" );
  RelFreqGap            = number( "relfreqgap" );
  AbsFreqGap            = number( "absfreqgap" );
  Rescale               = number( "rescale" );
  StimRepetition	= integer( "repeats" );
  Duration 		= number( "duration" );
  double pause 	        = number( "pause" );
  Ramp                  = number( "ramp" );
  Ramp2                 = number( "ramp2" );
  CarrierFrequency 	= number( "carrierfreq" );
  bool usebestfreq	= boolean( "usebestfreq" );
  Side 			= index( "side" );
  bool userate   	= boolean( "userate" );
  double targetrate	= number( "rate" );
  double ratetolerance	= number( "ratetol" );
  double minintensitystep = 0.5;  // get this from the attenuator!!
  double intensitystep  = number( "intensitystep" );
  int searchrepeats	= integer( "searchrepeats" );
  SkipWindow	        = number( "skipwin" );
  Sigma1	        = number( "sigma1" );
  Sigma2	        = number( "sigma2" );
  StoreModes storemode  = (StoreModes)index( "storemode" );
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
  StoreFile = "";

  // setup variables:
  if ( Amplitude > Intensity ) {
    warning( "Amplitude > Intensity" );
    return Failed;
  }
  if ( Side > 1 )
    Side = metaData().index( "Cell>best side" );
  string sidestr = Side > 0 ? "right" :  "left";
  if ( usebestfreq ) {
    double cf = metaData().number( "Cell>" + sidestr + " frequency" );
    if ( cf > 0.0 )
      CarrierFrequency = cf;
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
    for ( k=0;
	  k < thresh.size() && thresh.x(k) < CarrierFrequency;
	  k++ );
    double intthresh = 0.0;
    if ( k == 0 && ::fabs( thresh.x( k ) - CarrierFrequency ) < 1.0 )
      intthresh = thresh.y( k );
    else if ( k >= thresh.size() && ::fabs( thresh.x( thresh.size()-1 ) - CarrierFrequency ) < 1.0 )
      intthresh = thresh.y( thresh.size()-1 );
    else if ( k == 0 || k >= thresh.size() ) {
      warning( "Can not get the neuron's threshold intensity. <br>Requested carrier frequency of " +
	       Str( 0.001*CarrierFrequency ) +
	       " kHz is outside the measured range of the threshold curve." );
      return Failed;
    }
    else
      intthresh = thresh.y(k) + ( CarrierFrequency - thresh.x(k) )*( thresh.y(k) - thresh.y(k-1) )/( thresh.x(k) - thresh.x(k-1) );
    if ( intensitybase == 1 )  // relative to threshold
      Intensity = intensity + intthresh;
    else if ( intensitybase == 2 )  // relative to target rate intensity
      Intensity = intensity
	+ intthresh + metaData().number( "Cell>" + sidestr + " intensity" )
	- metaData().number( "Cell>" + sidestr + " threshold" );
  }
  else if ( intensitybase == 3 )  // relative to previous intensity
    Intensity += 0.0;
  else  // dB SPL
    Intensity = intensity;

  settings().setValueTypeFlags( 16, -Parameter::Section );

  // setup frequency range:
  FreqRange.set( freqrangestr );

  // plot trace:
  tracePlotSignal( Duration+0.01 );

  OutData signal;
  SampleDataD amdb;
  vector < EnvelopeFrequencyData > results;

  // search for intensity that evokes the target firing rate:
  if ( userate ) {
    // setup plot:
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
      MapD fic = as->ssFICurve( Side, CarrierFrequency );
      if ( fic.empty() )
	fic = as->fICurve( Side, CarrierFrequency );
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

    // create stimulus:
    int r = createStimulus( signal, amdb,
			    WaveForm == Noisegap || WaveForm == Noisecutoff ? 0.0 : targetrate,
			    stimfile, false );
    amdb -= Intensity;
    if ( r < 0 )
      return Failed;
    
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
	s += ",  <b>" + StimulusLabel + "</b>";
	s += ":  Intensity: <b>" + Str( Intensity, 0, 0, 'f' ) + " dB SPL</b>";
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
	spikes.push( events( SpikeEvents[0] ), signalTime(),
		     signalTime() + Duration );
	double meanrate = spikes.back().rate( SkipWindow, Duration );
	SampleDataD rate2( 0.0, Duration, 0.0005 );
	spikes.rate( rate2, GaussKernel( Sigma2 ) );
	
	// plot:
	{
	  SP.lock();
	  // spikes and firing rate:
	  SP[0].clear();
	  if ( ! SP[0].zoomedXRange() && ! SP[1].zoomedXRange() )
	    SP[0].setXRange( 1000.0*SkipWindow, 1000.0*Duration );
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
	  double threshold = metaData().number( "Cell>best threshold" );
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
	    SP[1].setXRange( 1000.0*SkipWindow, 1000.0*Duration );
	  if ( ! SP[1].zoomedYRange() )
	    SP[1].setYRange( ymin - 1.0, ymax + 1.0 );
	  SP[1].plot( amdb+Intensity, 1000.0, Plot::Green, 2 );
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
	  if ( meanrate+50.0 > maxf )
	    maxf = ::ceil((meanrate+50.0)/20.0)*20.0;
	  if ( maxf < targetrate ) 
	    maxf = targetrate;
	  if ( ! SP[2].zoomedYRange() )
	    SP[2].setYRange( 0.0, maxf );
	  SP[2].plotHLine( targetrate, Plot::White, 2 );
	  SP[2].plot( rates, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Red, Plot::Red );
	  MapD cr;
	  cr.push( Intensity, meanrate );
	  SP[2].plot( cr, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Yellow, Plot::Yellow );
	  SP.draw();
	  SP.unlock();
	}
      
	if ( softStop() > 0 ) {
	  writeZero( Speaker[ Side ] ); 
	  return Failed;
	}

	if ( meanrate > 0.0 ) {
	  sleep( pause );
	  if ( interrupt() ) {
	    writeZero( Speaker[ Side ] ); 
	    return Aborted;
	  }
	  // adjust gain of daq board:
	  adjust( trace( SpikeTrace[0] ), signalTime(),
		  signalTime() + Duration, 0.8 );
	  //	  activateGains();
	}
	else
	  break;

      }

      double rate = spikes.rate( SkipWindow, Duration );
      rates.push( Intensity, rate );

      // new intensity:
      if ( signal.success() && 
	   ( fabs( rate - targetrate ) < ratetolerance ||
	     intensitystep < minintensitystep ) )
	break;
      else {
	if ( ( signal.success() && rate < targetrate ) || signal.underflow() ) {
	  if ( Intensity < maxint )
	    intensitystep *= 0.5;
	  Intensity += intensitystep;
	  if ( Intensity > maxint )
	    maxint = Intensity;
	}
	else if ( ( signal.success() && rate > targetrate ) || signal.overflow() ) {
	  if ( Intensity > minint )
	    intensitystep *= 0.5;
	  Intensity -= intensitystep;
	  if ( Intensity < minint )
	    minint = Intensity;
	}
	else {
	  warning( "Output of stimulus failed!<br>Signal error: <b>" +
		   signal.errorText() + "</b>" +
		   "<br>Exit now!" );
	  writeZero( Speaker[ Side ] ); 
	  return Failed;
	}
      }

    }

  }

  results.resize( FreqRange.size(), EnvelopeFrequencyData( Duration, 0.0005 ) );

  // setup plots:
  postCustomEvent( 11 );
  P.lock();
  P.clearPlots();
  P[0].setTitle( "Mean firing rate =    Hz" );
  P.setDrawBackground();
  P.draw();
  P.unlock();

  // loop through frequencies:
  for ( FreqRange.reset(); ! FreqRange; ++FreqRange ) {

    Frequency = *FreqRange;

    int r = createStimulus( signal, amdb, Frequency, stimfile, true );
    if ( r < 0 ) {
      writeZero( Speaker[ Side ] ); 
      return Failed;
    }
    signal.setIntensity( Intensity + PeakAmplitude );

    testWrite( signal );
    if ( ! signal.success() ) {
      if ( signal.overflow() )
	warning( "Unable to play stimulus!<p>Requested intensity of <b>" + 
		 Str( Intensity + PeakAmplitude ) + 
		 "dB SPL</b> exceeds maximum possible output intensity of <b>" + 
		 Str( signal.intensity() ) + 
		 "dB SPL</b>!<p>Exit RePro now!" );
      else
	warning( "Unable to play stimulus!<p>Signal error: <b>" + signal.errorText() + "</b><p>Exit RePro now!" );
      writeZero( Speaker[ Side ] ); 
      return Failed;
    }
    
    for ( int counter=0; counter<StimRepetition; counter++ ) {

      // message:
      Str s = "<b>" + StimulusLabel + "</b>";
      s += ":  Intensity: <b>" + Str( Intensity, 0, 0, 'f' ) + " dB SPL</b>";
      s += ",  Frequency: <b>" + Str( Frequency ) + " Hz</b>";
      s += ",  Amplitude: <b>" + Str( Amplitude ) + " dB</b>";
      s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( StimRepetition ) + "</b>";
      message( s );

      // output:
      for ( int k=0; k<10; k++ ) {
	write( signal );
	if ( signal.success() )
	  break;
      }
      if ( ! signal.success() )
	break;
      sleep( pause );
      if ( interrupt() ) {
	save( results );
	writeZero( Speaker[ Side ] ); 
	return Aborted;
      }

      // adjust gain of daq board:
      adjust( trace( SpikeTrace[0] ), signalTime(), 
	      signalTime() + Duration, 0.8 );
      //      activateGains();

      analyze( results );
      plot( amdb, results );
      
      if ( softStop() > 1 ) {
	save( results );
	writeZero( Speaker[ Side ] ); 
	return Completed;
      }

    }

    if ( softStop() > 0 ) {
      save( results );
      writeZero( Speaker[ Side ] ); 
      return Completed;
    }

  }

  save( results );
  writeZero( Speaker[ Side ] ); 
  return Completed;
}


// saving the spike trace for data analysis:
void SpikePrecision::saveSpikes( const vector < EnvelopeFrequencyData > &results )
{
  // create file:
  ofstream df( addPath( "SP_SpikeTrace" + Str( totalRuns(), 2, '0' ) + ".dat" ).c_str(), ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  df << "#                 Intensity: " << Intensity << "dB SPL\n";
  df << "# Intensity above threshold: " << Intensity - metaData().number( "Cell>best threshold" ) << "dB\n";
  df << "#                      Side: " << Side << " (0 = left speaker, 1 = right speaker)\n";
  df << "#         Carrier Frequency: " << CarrierFrequency << "Hz\n";
  df << "#                     Macro: " << macroName() << "\n";
  stimulusData().save( df, "# ", 0, Options::FirstOnly );
  settings().save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  // write data:
  int n = 0;
  for ( unsigned int k=0; k<results.size(); k++ ) {
    if ( results[k].Spikes.size() > 0 ) {
      df << "#         index0: " << n++ << '\n';
      df << "#      Frequency: " << results[k].Frequency << "Hz\n";
      df << "#       Envelope: " << results[k].Envelope << '\n';
      results[k].Spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
      df << '\n';
    }
  }
  df << '\n';
}


// saving the firing rates for data analysis:
void SpikePrecision::saveRates( const vector < EnvelopeFrequencyData > &results )
{
  // create file:
  ofstream df( addPath( "SP_FiringRate" + Str( totalRuns(), 2, '0' ) + ".dat" ).c_str(), ofstream::out | ofstream::app );

  if ( ! df.good() )
    return;

  // write header and key:
  df << "#                 Intensity: " << Intensity << "dB SPL\n";
  df << "# Intensity above threshold: " << Intensity - metaData().number( "Cell>best threshold" ) << "dB\n";
  df << "#                      Side: " << Side << " (0 = left speaker, 1 = right speaker)\n";
  df << "#         Carrier Frequency: " << CarrierFrequency << "Hz\n";
  df << "#                     Macro: " << macroName() << "\n";
  stimulusData().save( df, "# ", 0, Options::FirstOnly );
  settings().save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%5.1f" );
  key.addNumber( "f1", "Hz", "%5.1f" );
  key.addNumber( "f2", "Hz", "%5.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  // write data:
  int n = 0;
  for ( unsigned int k=0; k<results.size(); k++ ) {
    if ( results[k].Spikes.size() > 0 ) {
      df << "#           index0: " << n++ << '\n';
      df << "#        Frequency: " << results[k].Frequency << "Hz\n";
      df << "#         Envelope: " << results[k].Envelope << '\n';
      for (int j=0; j<results[k].Rate1.size(); j++) {
	key.save( df, results[k].Rate1.pos(j) * 1000.0, 0 );
	key.save( df, results[k].Rate1[j] );
	key.save( df, results[k].Rate2[j] );
	df << '\n';
      }
      df << "\n\n";
    }
  }
}


void SpikePrecision::save( const vector < EnvelopeFrequencyData > &results )
{
  if ( results.empty() )
    return;

  saveSpikes( results );
  saveRates( results );
}


void SpikePrecision::plot( const SampleDataD &amdb,
			   const vector < EnvelopeFrequencyData > &results )
{
  const EnvelopeFrequencyData &efd = results[FreqRange.pos()]; // rename current results to &efd

  P.lock();

  // spikes and firing rate:
  P[0].clear();
  P[0].setTitle( "Mean firing rate = " + Str( efd.SSRate, 0, 0, 'f' ) + "Hz" );
  if ( ! P[0].zoomedXRange() && ! P[1].zoomedXRange() )
    P[0].setXRange( 1000.0*SkipWindow, 1000.0*Duration );
  if ( ! P[0].zoomedYRange() )
    P[0].setYRange( 0.0, Plot::AutoScale );
  int maxspikes	= (int)rint( 20.0 / SpikeTraces );
  if ( maxspikes < 4 )
    maxspikes = 4;
  int offs = (int)efd.Spikes.size() > maxspikes ? efd.Spikes.size() - maxspikes : 0;
  double delta = StimRepetition > 0 && StimRepetition < maxspikes ? 1.0/StimRepetition : 1.0/maxspikes;
  for ( int i=offs, j=0; i<efd.Spikes.size(); i++ ) {
    j++;
    P[0].plot( efd.Spikes[i], 0, 0.0, 1000.0, 1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp, delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
  }
  P[0].plot( efd.Rate1, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P[0].plot( efd.Rate2, 1000.0, Plot::Orange, 2, Plot::Solid );

  // stimulus:
  double threshold = metaData().number( "Cell>best threshold" );
  double ymin = Intensity - PeakAmplitude;
  double ymax = Intensity + PeakAmplitude;
  if ( threshold > 0.0 ) {
    if ( ymin > threshold )
      ymin = threshold;
    if ( ymax < threshold )
      ymax = threshold;
  }
  P[1].clear();
  if ( ! P[0].zoomedXRange() && ! P[1].zoomedXRange() )
    P[1].setXRange( 1000.0*SkipWindow, 1000.0*Duration );
  if ( ! P[1].zoomedYRange() )
    P[1].setYRange( ymin - 1.0, ymax + 1.0 );
  P[1].plot( amdb, 1000.0, Plot::Green, 2 );
  if ( Frequency > 0.0 )
    P[1].plotHLine( Intensity, Plot::White, 2 );
  if ( threshold > 0.0 )
    P[1].plotHLine( threshold, Plot::Yellow, 2 );

  // precision and mean firing rate:
  MapD cd, rd;
  cd.reserve( results.size() );
  rd.reserve( results.size() );
  for ( unsigned int k=0; k<results.size(); k++ ) {
    if ( results[k].Spikes.size() > 1 ) {
      cd.push( results[k].Frequency, results[k].Correlation );
      rd.push( results[k].Frequency, results[k].SSRate );
    }
  }
  MapD ccd( 1, results[FreqRange.pos()].Frequency, results[FreqRange.pos()].Correlation );
  MapD crd( 1, results[FreqRange.pos()].Frequency, results[FreqRange.pos()].SSRate );

  // precision:
  P[2].clear();
  if ( ! P[2].zoomedYRange() )
    P[2].setYRange( -0.2, 1.0 );
  if ( ! P[2].zoomedXRange() && ! P[3].zoomedXRange() )
    P[2].setXRange( FreqRange.minValue(), FreqRange.maxValue() );
  P[2].plotHLine( 0.0, Plot::White, 2 );
  P[2].plot( cd, 1.0, Plot::Blue, 2, Plot::Solid, Plot::Circle, 6, Plot::Blue, Plot::Blue );
  if ( results[FreqRange.pos()].Spikes.size() > 1 )
    P[2].plot( ccd, 1.0, Plot::Transparent, 2, Plot::Solid, Plot::Circle, 6, Plot::Yellow, Plot::Transparent );

  // mean firing rate:
  P[3].clear();
  if ( ! P[3].zoomedYRange() )
    P[3].setYRange( 0.0, Plot::AutoScale );
  if ( ! P[2].zoomedXRange() && ! P[3].zoomedXRange() )
    P[3].setXRange( FreqRange.minValue(), FreqRange.maxValue() );
  P[3].plot( rd, 1.0, Plot::Red, 2, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  if ( results[FreqRange.pos()].Spikes.size() > 1 )
    P[3].plot( crd, 1.0, Plot::Transparent, 2, Plot::Solid, Plot::Circle, 6, Plot::Yellow, Plot::Transparent );

  P.draw();

  P.unlock();
}


void SpikePrecision::analyze( vector < EnvelopeFrequencyData > &results )
{
  const EventData &spikes = events( SpikeEvents[0] );

  // spikes:
  results[FreqRange.pos()].Spikes.push( spikes, signalTime(),
					signalTime() + Duration + 0.1 );
  int trial1 = results[FreqRange.pos()].Spikes.size() - 1;
  int trial2 = trial1;
  results[FreqRange.pos()].Frequency = Frequency;
  results[FreqRange.pos()].Intensity = Intensity;
  results[FreqRange.pos()].SSRate = results[FreqRange.pos()].Spikes.rate( SkipWindow, Duration );
  results[FreqRange.pos()].Correlation = results[FreqRange.pos()].Spikes.correlation( SkipWindow, Duration, GaussKernel( 0.001 ), 0.0001 );
  results[FreqRange.pos()].Spikes.back().addRate( results[FreqRange.pos()].Rate1, trial1, GaussKernel( Sigma1 ) );
  results[FreqRange.pos()].Spikes.back().addRate( results[FreqRange.pos()].Rate2, trial2, GaussKernel( Sigma2 ) );
  results[FreqRange.pos()].Envelope = StoreFile;
}


int SpikePrecision::createStimulus( OutData &signal, SampleDataD &amdb,
				    double frequency, const Str &file,
				    bool store )
{
  Options header;
  string waveforms[7] = { "Sine", "Rectangular", "Triangular", "Saw-up", "Saw-down", "Noise-gap", "Noise-cutoff" };
    
  header.addText( "waveform", waveforms[WaveForm] );
  header.addText( "frequency", Str( Frequency ) + "Hz" );
  header.addText( "amplitude", Str( Amplitude ) + "dB" );
  if ( Duration < 1.0 )
    header.addText( "duration", Str( 1000.0*Duration ) + "ms" );
  else
    header.addText( "duration", Str( Duration ) + "s" );

  amdb = SampleDataD( 0.0, Duration, 0.0001 );
  PeakAmplitude = Amplitude;
  if ( frequency <= 0.0 ) {
    // const stimulus:
    amdb = -Amplitude;
  }
  else {
    if ( WaveForm == Rectangular ) {
      amdb = Amplitude * 2.0 * ( rectangle( amdb.range(), 1.0/frequency, DutyCycle/frequency, Ramp2 ) - 1.0 );
      header.addText( "dutycycle", Str( 100.0*DutyCycle ) + "%" );
      header.addText( "ramp2", Str( 1000.0*Ramp2 ) + "ms" );
    }
    else if ( WaveForm == Triangular )
      amdb = Amplitude * 2.0 * ( triangle( amdb.range(), 1.0/frequency ) - 1.0 );
    else if ( WaveForm == Sawup ) {
      amdb = Amplitude * 2.0 * ( sawUp( amdb.range(), 1.0/frequency, Ramp2 ) - 1.0 );
      header.addText( "ramp2", Str( 1000.0*Ramp2 ) + "ms" );
    }
    else if ( WaveForm == Sawdown ) {
      amdb = Amplitude * 2.0 * ( sawDown( amdb.range(), 1.0/frequency, Ramp2 ) - 1.0 );
      header.addText( "ramp2", Str( 1000.0*Ramp2 ) + "ms" );
    }
    else if ( WaveForm == Noisegap ) {
      SampleDataD wave;
      wave.load( file );
      if ( wave.empty() ) {
	warning( "Cannot open or empty file <b>" + file + "</b>!" );
	return -1;
      }
      if ( wave.length() > Duration )
	wave.resize( wave.indices( Duration ) );
      double peakamplitudefac = PeakAmplitudeFac;
      if ( frequency > 0.0 ) {
	SampleDataD filter( 0.0, 1.0/wave.stepsize(), 1.0/wave.length(), 1.0 );
	double minfreq = RelFreqGap > 0.0 ? (1.0 - 0.5*RelFreqGap)*frequency : frequency - 0.5*AbsFreqGap;
	double maxfreq = RelFreqGap > 0.0 ? (1.0 + 0.5*RelFreqGap)*frequency : frequency + 0.5*AbsFreqGap;
	header.addText( "gap", Str( minfreq ) + "-" + Str( maxfreq ) + "Hz" );
	if ( RelFreqGap > 0.0 ) {
	  header.addText( "relative gap", Str( 100.0*RelFreqGap ) + "%" );
	  header.addText( "relfreqgap", "rg" + Str( 100.0*RelFreqGap ) + "%" );
	}
	else {
	  header.addText( "absolute gap", Str( AbsFreqGap ) + "Hz" );
	  header.addText( "absfreqgap", "ag" + Str( AbsFreqGap ) + "Hz" );
	}
	int mininx = filter.index( minfreq );
	if ( mininx < 0 )
	  mininx = 0;
	int maxinx = filter.index( maxfreq );
	if ( maxinx >= filter.size() )
	  maxinx = filter.size()-1;
	for ( int k=mininx; k <= maxinx; k++ )
	  filter[k] = 0.0;
	if ( Rescale )
	  wave.freqFilter( filter, true );
	else {
	  wave.freqFilter( filter, false );
	  peakamplitudefac = ::relacs::rms( wave );
	}
      }
      if ( peakamplitudefac <= 0.0 )
	peakamplitudefac = ::relacs::rms( wave );

      /*
	wave.save( "wavef.dat" );

	SampleDataD spec( 4096, 0.0, 0.5/(wave.stepsize()*4096) );
	::relacs::psd( wave.begin(), wave.end(), 
	spec.begin(), spec.end(), 
	true, ::relacs::hanning );
	spec /= ::relacs::max( spec );
	spec.log10();
	spec *= 10.0;
	spec.save( "waves.dat", 10, 8 );
      */

      int c = ::relacs::clip( -1.0, 1.0, wave );
      double cp = 100.0*double(c)/wave.size();
      if ( cp > 1.0 )
	warning( "Clipped " + Str( cp, 0, 3, 'g' ) + "% of the stimulus waveform.", 4.0 );
      PeakAmplitude = Amplitude / peakamplitudefac;
      amdb = PeakAmplitude * ( wave - 1.0 );
      header.addText( "file", file );
      header.addText( "filename", "-" + file.longName() + "-" );
      header.addText( "intensityfactor", Str( ::pow (10.0, -PeakAmplitude / 20.0 ), 0, 3, 'g' ) );
    }
    else if ( WaveForm == Noisecutoff ) {
      SampleDataD wave;
      wave.load( file );
      if ( wave.empty() ) {
	warning( "Cannot open or empty file <b>" + file + "</b>!" );
	return -1;
      }
      if ( wave.length() > Duration )
	wave.resize( wave.indices( Duration ) );
      double peakamplitudefac = PeakAmplitudeFac;
      if ( frequency > 0.0 ) {
	SampleDataD filter( 0.0, 1.0/wave.stepsize(), 1.0/wave.length(), 1.0 );
	int maxinx = filter.index( frequency );
	if ( maxinx >= filter.size() )
	  maxinx = filter.size();
	for ( int k=maxinx; k < filter.size(); k++ )
	  filter[k] = 0.0;
	wave.freqFilter( filter, true );
      }
      if ( peakamplitudefac <= 0.0 )
	peakamplitudefac = ::relacs::rms( wave );
      /*
	wave.save( "wavef.dat" );

	SampleDataD spec( 4096, 0.0, 0.5/(wave.stepsize()*4096) );
	::relacs::psd( wave.begin(), wave.end(), 
	spec.begin(), spec.end(), 
	true, ::relacs::hanning );
	spec /= ::relacs::max( spec );
	spec.log10();
	spec *= 10.0;
	spec.save( "waves.dat", 10, 8 );
      */
      int c = ::relacs::clip( -1.0, 1.0, wave );
      double cp = 100.0*double(c)/wave.size();
      if ( cp > 1.0 )
	warning( "Clipped " + Str( cp, 0, 3, 'g' ) + "% of the stimulus waveform.", 4.0 );
      PeakAmplitude = Amplitude / peakamplitudefac;
      amdb = PeakAmplitude * ( wave - 1.0 );
      header.addText( "file", file );
      header.addText( "filename", "-" + file.longName() + "-" );
      header.addText( "intensityfactor", Str( ::pow (10.0, -PeakAmplitude / 20.0 ), 0, 3, 'g' ) );
    }
    else
      amdb = Amplitude * ( sin( amdb.range(), frequency ) - 1.0 );
  }
  OutData am( amdb );
  for ( int k=0; k<am.size(); k++ )
    am[k] = ::pow (10.0, amdb[k] / 20.0 );
  am.ramp( Ramp );
  header.addText( "ramp", Str( 1000.0*Ramp ) + "ms" );
  Str labelpattern = "$(waveform)$(filename) $(dutycycle)$(ramp2)$(relfreqgap)$(absfreqgap)";
  StimulusLabel = translate( labelpattern, header );
  if ( store ) {
    Str filepattern = "$(waveform)$(filename)$(frequency)$(dutycycle)$(ramp2)$(relfreqgap)$(absfreqgap)r$(ramp)$(duration)$(amplitude)$(intensityfactor).dat";
    StoreFile = StorePath + translate( filepattern, header );
    ifstream cf( StoreFile.c_str() );
    if ( ! cf ) {
      ofstream of( StoreFile.c_str() );
      header.erase( "filename" );
      header.erase( "relfreqgap" );
      header.erase( "absfreqgap" );
      header.save( of, "# " );
      of << '\n';
      of << "#Key\n";
      of << "# t    x\n";
      of << "# s    1\n";
      am.save( of, 7, 5 );
      printlog( "wrote " + StoreFile );;
    }
  }
  signal.free();
  signal.setTrace( Speaker[ Side ] );
  signal.fill( am, CarrierFrequency );
  signal.setIdent( "amplitude=" + Str( Amplitude ) + "dB, frequency=" + Str( frequency ) + "Hz" );
  amdb += Intensity + PeakAmplitude;
  return 0;
}


void SpikePrecision::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    P.lock();
    P.setDrawBackground();
    P.unlock();
    Stack->setCurrentWidget( &P );
  }
  else if ( qce->type() == QEvent::User+12 ) {
    P.lock();
    P.setDrawBackground();
    P.unlock();
    Stack->setCurrentWidget( &SP );
  }
  else
    RePro::customEvent( qce );
}


addRePro( SpikePrecision, auditoryprojects );

}; /* namespace auditoryprojects */

#include "moc_spikeprecision.cc"
