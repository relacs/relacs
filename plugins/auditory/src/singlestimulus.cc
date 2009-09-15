/*
  auditory/singlestimulus.cc
  Output of a single stimulus stored in a file.

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
  : RePro( "SingleStimulus", "Single Stimulus", "Auditory",
	   "Jan Benda", "1.2", "Jan 10, 2008" ),
    SP( 3, this ),
    P( 2, this )
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
  addLabel( "Waveform" ).setStyle( OptWidget::TabLabel );
  addSelection( "type", "Type of stimulus", "Wave|Envelope|AM" );
  addSelection( "waveform", "Stimulus waveform", "From file|Const|Sine|Rectangular|Triangular|Sawup|Sawdown|Whitenoise|OUnoise" );
  addText( "stimfile", "Stimulus file", "" ).setStyle( OptWidget::BrowseExisting ).setActivation( "waveform", "From file" );
  addNumber( "stimampl", "Amplitude factor (standard deviation) of stimulus file", 0.0, 0.0, 1.0, 0.01 ).setActivation( "waveform", "From file" );
  addNumber( "amplitude", "Amplitude of stimulus", Amplitude, 0.0, 130.0, 1.0, "dB" ).setActivation( "type", "AM" );
  addNumber( "freq", "Frequency of waveform", 1.0, 0.0, 10000.0, 1.0, "Hz" ).setActivation( "waveform", "From file|Const", false );
  addNumber( "dutycycle", "Duty-cycle of rectangular waveform", 0.5, 0.0, 1.0, 0.05, "1", "%" ).setActivation( "waveform", "Rectangular" );
  addInteger( "seed", "Seed for random number generation", 0 ).setActivation( "waveform", "Whitenoise|OUnoise" );;
  addNumber( "duration", "Maximum duration of stimulus", Duration, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "ramp", "Ramp of stimulus", 0.002, 0.0, 10.0, 0.001, "seconds", "ms" );
  addLabel( "Stimulus" ).setStyle( OptWidget::TabLabel );
  addNumber( "intensity", "Stimulus intensity", Intensity, -200.0, 200.0, 5.0, "dB" );
  addSelection( "intensitybase", "Stimulus intensity relative to", "SPL|threshold|rate|previous" );
  addNumber( "repeats", "Number of stimulus presentations", Repeats, 1, 10000, 1, "times" );
  addNumber( "pause", "Duration of pause between stimuli", 1.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addSelection( "side", "Speaker", "left|right|best" );
  addLabel( "Carrier" );
  addNumber( "carrierfreq", "Carrier frequency", CarrierFreq, 2000.0, 40000.0, 500.0, "Hz", "kHz" ).setActivation( "usebestfreq", "false" );
  addBoolean( "usebestfreq", "Use the cell's best frequency", true );
  addLabel( "Intensity - search" ).setStyle( OptWidget::TabLabel );
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
  addLabel( "Analysis" ).setStyle( OptWidget::TabLabel );
  addNumber( "skipwin", "Initial portion of stimulus not used for analysis", SkipWin, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "sigma1", "Standard deviation of rate smoothing kernel 1", Sigma1, 0.0, 1.0, 0.0001, "seconds", "ms" );
  addNumber( "sigma2", "Standard deviation of rate smoothing kernel 2", Sigma2, 0.0, 1.0, 0.001, "seconds", "ms" );
  addBoolean( "adjust", "Adjust input gain", true );
  addLabel( "Save stimuli" );
  addSelection( "storemode", "Save stimuli in", "session|repro|custom" ).setUnit( "path" );
  addText( "storepath", "Save stimuli in custom directory", "" ).setStyle( OptWidget::BrowseDirectory ).setActivation( "storemode", "custom" );
  addSelection( "storelevel", "Save", "all|am+generated|generated|noise|none" ).setUnit( "stimuli" );

  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // setup plots:
  SP.hide();
  SP.lock();
  SP.setCommonXRange( 0, 1 );
  SP[0].setOrigin( 0.0, 0.5 );
  SP[0].setSize( 0.7, 0.5 );
  SP[0].setLMarg( 7.0 );
  SP[0].setRMarg( 1.5 );
  SP[0].setTMarg( 3.0 );
  SP[0].noXTics();
  SP[0].setYLabel( "Firing rate [Hz]" );
  SP[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP[1].setOrigin( 0.0, 0.0 );
  SP[1].setSize( 0.7, 0.48 );
  SP[1].setLMarg( 7.0 );
  SP[1].setRMarg( 1.5 );
  SP[1].setXLabel( "Time [ms]" );
  SP[1].setYLabel( "Stimulus [dB]" );
  SP[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP[2].setOrigin( 0.7, 0.1 );
  SP[2].setSize( 0.3, 0.8 );
  SP[2].setLMarg( 7.0 );
  SP[2].setRMarg( 1.5 );
  SP[2].setTMarg( 3.0 );
  SP[2].setXLabel( "Intensity [dB SPL]" );
  SP[2].setYLabel( "Firing rate [Hz]" );
  SP[2].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP.unlock();

  P.show();
  P.lock();
  P.setCommonXRange( 0, 1 );
  P[0].setOrigin( 0.0, 0.5 );
  P[0].setSize( 1.0, 0.5 );
  P[0].setLMarg( 8.0 );
  P[0].setRMarg( 2.0 );
  P[0].noXTics();
  P[0].setYLabel( "Firing rate [Hz]" );
  P[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 1.0, 0.48 );
  P[1].setLMarg( 8.0 );
  P[1].setRMarg( 2.0 );
  P[1].setXLabel( "Time [ms]" );
  P[1].setYLabel( "Stimulus [dB]" );
  P[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P.unlock();

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

  // store options:
  Settings = *this;
  Settings.setTypeFlags( 16, -Parameter::Blank );

  // get options:
  WaveType = (WaveTypes)index( "type" );
  WaveForm = (WaveForms)index( "waveform" );
  Str stimfile = text( "stimfile" );
  PeakAmplitudeFac = number( "stimampl" );
  Frequency = number( "freq" );
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

  if ( intensitybase == 1 )
    Intensity = intensity + metaData( "Cell" ).number( "best threshold" );	// get "best thresh" from FICurve (via the session widget)
  else if ( intensitybase == 2 )
    Intensity = intensity + metaData( "Cell" ).number( "best intensity" );	// get "best intensity" from FICurve (via the session widget)
  else if ( intensitybase == 3 )
    Intensity += 0.0;
  else
    Intensity = intensity;
  if ( usebestfreq ) {
    double cf = metaData( "Cell" ).number( "best frequency" );
    if ( cf > 0.0 )
      CarrierFreq = cf;
  }
  if ( Side > 1 )
    Side = metaData( "Cell" ).index( "best side" );

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
    plotToggle( true, true, searchduration, 0.0 );
    
    postCustomEvent( 11 );
    SP.lock();
    SP.clearPlots();
    SP[0].setTitle( "Search target firing rate " + Str( targetrate ) + " Hz" );
    SP.unlock();
    SP.draw();

    MapD rates;
    rates.reserve( 20 );

    // get f-I curve:
    auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
    if ( as == 0 )
      warning( "No auditory session!", 4.0 );
    else {
      MapD fic = as->ssFICurve();
      if ( fic.empty() )
	fic = as->fICurve();
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
	s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( searchrepeats ) + "</b>";
	message( s );

	// output:
	write( signal );
	if ( ! signal.success() )
	  break;
	sleep( searchduration );
	if ( interrupt() ) {
	  writeZero( Speaker[ Side ] );
	  return Aborted;
	}

	// analyze:
	const EventData &spk = events( SpikeEvents[0] );
	spikes.push( spk, spk.signalTime(), spk.signalTime()+searchduration );
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
	  double threshold = metaData( "Cell" ).number( "best threshold" );
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
	  SP.unlock();
	  SP.draw();
	}

	if ( rate >= silentrate ) {
	  sleep( searchpause );
	  if ( interrupt() ) {
	    writeZero( Speaker[ Side ] );
	    return Aborted;
	  }
	  // adjust gain of daq board:
	  if ( adjustgain ) {
	    adjust( trace( SpikeTrace[0] ), trace( SpikeTrace[0] ).signalTime(), 
		    trace( SpikeTrace[0] ).signalTime() + searchduration, 0.8 );
	    //	    activateGains();
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
  plotToggle( true, true, Duration, 0.0 );

  // setup plots:
  postCustomEvent( 10 );
  P.lock();
  P.clearPlots();
  P[0].setTitle( "Mean firing rate =    Hz" );
  P.unlock();
  P.draw();

  // variables:
  EventList spikes;
  MeanRate = 0.0;
  SampleDataD rate1( 0.0, Duration, 0.001, 0.0 );
  SampleDataD rate2( 0.0, Duration, 0.001, 0.0 );

  timeStamp();

  // output stimulus:  
  for ( int counter=0; counter<Repeats; counter++ ) {
    
    // message:
    Str s =  wavetypes[WaveType] + ": <b>" + StimulusLabel + "</b>";
    s += ",  Intensity: <b>" + Str( Intensity, 0, 1, 'f' ) + " dB SPL</b>";
    if ( WaveType == AM )
      s += ",  Amplitude: <b>" + Str( Amplitude, 0, 1, 'f' ) + " dB</b>";
    s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( Repeats ) + "</b>";
    message( s );
    
    // output:
    for ( int k=0; k<10; k++ ) {
      write( signal );
      if ( signal.success() )
	break;
      else
	sleep( 0.001 );
    }
    if ( signal.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
	       signal.errorText() + "</b>," +
	       "<br> Loop: <b>" + Str( counter+1 ) + "</b>" +
	       "<br>Exit now!" );
      writeZero( Speaker[ Side ] );
      return Failed;
    }

    sleep( Duration + ( pause > 0.01 ? 0.01 : pause ) );

    if ( interrupt() ) {
      save( spikes, rate1, rate2 );
      writeZero( Speaker[ Side ] );
      return Aborted;
    }
    
    analyze( spikes, rate1, rate2 );
    plot( spikes, rate1, rate2 );
    
    // adjust gain of daq board:
    if ( adjustgain ) {
      adjust( trace( SpikeTrace[0] ), trace( SpikeTrace[0] ).signalTime(),
	      trace( SpikeTrace[0] ).signalTime() + Duration, 0.8 );
      //      activateGains();
    }

    sleepOn( Duration + pause );
    timeStamp();
    
    if ( softStop() > 0 ) {
      save( spikes, rate1, rate2 );
      writeZero( Speaker[ Side ] );
      return Completed;
    }
    
  }
  
  save( spikes, rate1, rate2 );
  writeZero( Speaker[ Side ] );
  return Completed;
}


void SingleStimulus::saveSpikes( Options &header, const EventList &spikes )
{
  // create file:
  ofstream df( addPath( "stimulusspikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  Settings.save( df, "#   ", -1, 16, false, true );
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
  ofstream df( addPath( "stimulusrate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  Settings.save( df, "#   ", -1, 16, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.addNumber( "r" + Str( 1000.0*Sigma1 ) + "ms", "Hz", "%5.1f" );
  key.addNumber( "r" + Str( 1000.0*Sigma2 ) + "ms", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<rate1.size(); k++ ) {
    key.save( df, rate1.pos( k ) * 1000.0, 0 );
    key.save( df, rate1[k] );
    key.save( df, rate2[k] );
    df << '\n';
  }
  df << "\n\n";
}


void SingleStimulus::save( const EventList &spikes, const SampleDataD &rate1,
			   const SampleDataD &rate2 )
{
  Options header;
  header.addInteger( "index1", totalRuns()-1 );
  header.addNumber( "carrier frequency", 0.001*CarrierFreq, "kHz", "%.3f" );
  header.addInteger( "side", Side );
  header.addNumber( "intensity", Intensity, "dB SPL", "%.1f" );
  header.addNumber( "amplitude", Amplitude, "dB", "%.1f" );
  header.addNumber( "amplfac", PeakAmplitudeFac, "", "%.3f" );
  header.addNumber( "duration", 1000.0*Duration, "ms", "%.1f" );
  header.addText( "envelope", StoreFile );
  header.addText( "session time", sessionTimeStr() ); 
  header.addLabel( "settings:" );

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
  double threshold = metaData( "Cell" ).number( "best threshold" );
  double ymin = Intensity - PeakAmplitude;
  double ymax = Intensity + PeakAmplitude;
  if ( WaveType == Envelope )
    ymin = AMDB.min( Ramp, Duration-Ramp );
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
  P.unlock();

  P.draw();
}


void SingleStimulus::analyze( EventList &spikes, SampleDataD &rate1,
			      SampleDataD &rate2 )
{
  if ( SpikeEvents[0] < 0 )
    return;

  const EventData &spikeevents = events( SpikeEvents[0] );

  // spikes:
  spikes.push( spikeevents, spikeevents.signalTime(),
	       spikeevents.signalTime() + Duration );
  int trial1 = spikes.size()-1;
  int trial2 = spikes.size()-1;

  MeanRate = spikes.rate( SkipWin, Duration );
  spikes.back().addRate( rate1, trial1, GaussKernel( Sigma1 ) );
  spikes.back().addRate( rate2, trial2, GaussKernel( Sigma2 ) );
}


int SingleStimulus::createStimulus( OutData &signal, const Str &file,
				    double &duration, bool stoream )
{
  SampleDataD wave;
  string wavename;
  bool store = false;
  Options header;

  if ( WaveForm == File ) {
    // load stimulus from file:
    // open file:
    ifstream str( file.c_str() );
    if ( ! str.bad() ) {
      // skip header and read key:
      double tfac = 1.0;
      string s;
      while ( getline( str, s ) && 
	      ( s.empty() || s.find( '#' ) == 0 ) ) {
	if ( s.find( "#Key" ) == 0 ) {
	  for ( int k=0; 
		getline( str, s ) && 
		  ( s.empty() || s.find( '#' ) == 0 ); 
		k++ ) {
	    if ( k < 4 && s.find( "ms" ) != string::npos )
	      tfac = 0.001;
	  }
	  break;
	}
      }
      // load data:
      wave.load( str, "EMPTY", &s );

      if ( tfac != 0.0 )
	wave.range() *= tfac;
    }
    if ( wave.empty() ) {
      warning( "Unable to load stimulus from file " + file );
      return -1;
    }
    if ( duration > 0.0 && wave.length() > duration )
      wave.resize( wave.indices( duration ) );
    duration = wave.length();
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
	wave.sin( LinearRange( 0.0, duration, 0.001 ), Frequency );
      }
      else {
	Random rand;
	unsigned long seed = rand.setSeed( Seed );
	if ( WaveForm == Whitenoise )
	  wave.whiteNoise( duration, 0.001, 0.0, Frequency, rand );
	else if ( WaveForm == OUnoise )
	  wave.ouNoise( duration, 0.001, 1.0/Frequency, rand );
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
	wave.rectangle( LinearRange( 0.0, duration, 0.001 ), 1.0/Frequency, DutyCycle/Frequency, Ramp );
	header.addText( "dutycycle", Str( 100.0*DutyCycle ) + "%" );
      }
      else if ( WaveForm == Triangular )
	wave.triangle( LinearRange( 0.0, duration, 0.001 ), 1.0/Frequency );
      else if ( WaveForm == Sawup )
	wave.sawUp( LinearRange( 0.0, duration, 0.001 ), 1.0/Frequency, Ramp );
      else if ( WaveForm == Sawdown )
	wave.sawDown( LinearRange( 0.0, duration, 0.001 ), 1.0/Frequency, Ramp );
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
    wave = SampleDataD( 0.0, duration, 0.001, 1.0 );
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
  applyOutTrace( signal );
  if ( WaveType == AM ) {
    if ( StoreLevel == AMGenerated ) 
      store = true;
    AMDB = SampleDataD( wave.range() );
    PeakAmplitude = Amplitude / PeakAmplitudeFac;
    AMDB = PeakAmplitude * ( wave - 1.0 );
    SampleDataD am( AMDB.range() );
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
	printlog( "wrote " + StoreFile );;
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
    AMDB = SampleDataD( wave.range() );
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
	printlog( "wrote " + StoreFile );;
      }
    }
  }
  else { // Wave
    signal = wave;
    signal.setCarrierFreq( CarrierFreq );
    signal.setIdent( "wave=" + wavename );
  }
  signal.ramp( Ramp );
  
  return 0;
}


void SingleStimulus::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+10 ) {
    SP.hide();
    P.show();
  }
  else if ( qce->type() == QEvent::User+11 ) {
    P.hide();
    SP.show();
  }
  else
    RELACSPlugin::customEvent( qce );
}


addRePro( SingleStimulus );

}; /* namespace auditory */

#include "moc_singlestimulus.cc"
