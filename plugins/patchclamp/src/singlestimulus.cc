/*
  patchclamp/singlestimulus.cc
  Output of a single stimulus stored in a file.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
  : RePro( "SingleStimulus", "Single Stimulus", "Patch-clamp",
	   "Jan Benda", "1.2", "Oct 13, 2008" ),
    SP( 3, this ),
    P( 2, this )
{
  AmplitudeUnit = "nA";
  Offset = 0.0;
  Amplitude = 1.0;
  Duration = 0.0;
  Repeats = 10;
  SkipWin = 0.1;
  Sigma1 = 0.002;
  Sigma2 = 0.02;

  // options:
  addLabel( "Waveform" ).setStyle( OptWidget::TabLabel );
  addSelection( "waveform", "Stimulus waveform", "From file|Const|Sine|Rectangular|Triangular|Sawup|Sawdown|Whitenoise|OUnoise" );
  addText( "stimfile", "Stimulus file", "" ).setStyle( OptWidget::BrowseExisting ).setActivation( "waveform", "From file" );
  addNumber( "stimampl", "Amplitude factor (standard deviation) of stimulus file", 0.0, 0.0, 1.0, 0.01 ).setActivation( "waveform", "From file" );
  addNumber( "amplitude", "Amplitude of stimulus", Amplitude, 0.0, 130.0, 1.0, AmplitudeUnit ).setActivation( "waveform", "Const", false );;
  addNumber( "freq", "Frequency of waveform", 10.0, 0.0, 10000.0, 1.0, "Hz" ).setActivation( "waveform", "From file|Const", false );
  addNumber( "dutycycle", "Duty-cycle of rectangular waveform", 0.5, 0.0, 1.0, 0.05, "1", "%" ).setActivation( "waveform", "Rectangular" );
  addInteger( "seed", "Seed for random number generation", 0 ).setActivation( "waveform", "Whitenoise|OUnoise" );;
  addNumber( "duration", "Maximum duration of stimulus", Duration, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "ramp", "Ramp of stimulus", 0.002, 0.0, 10.0, 0.001, "seconds", "ms" );
  addLabel( "Stimulus" ).setStyle( OptWidget::TabLabel );
  addNumber( "offset", "Stimulus mean", Offset, -2000.0, 2000.0, 5.0, AmplitudeUnit );
  addSelection( "offsetbase", "Stimulus mean relative to", "absolute|threshold|previous" );
  addNumber( "repeats", "Number of stimulus presentations", Repeats, 1, 10000, 1, "times" );
  addNumber( "pause", "Duration of pause between stimuli", 1.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addSelection( "outtrace", "Output trace", "V-1" );
  addLabel( "Offset - search" ).setStyle( OptWidget::TabLabel );
  addBoolean( "userate", "Search offset for target firing rate", false );
  addNumber( "rate", "Target firing rate", 100.0, 0.0, 1000.0, 10.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "ratetol", "Tolerance for target firing rate", 5.0, 0.0, 1000.0, 1.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "offsetstep", "Initial offset step", 8.0, 0.0, 100.0, 1.0, AmplitudeUnit ).setActivation( "userate", "true" );
  addNumber( "searchrepeats", "Number of search stimulus presentations", 2, 1, 100, 1, "times" ).setActivation( "userate", "true" );
  addNumber( "silentrate", "Ignore response below", 0.0, 0.0, 1000.0, 5.0, "Hz" ).setActivation( "userate", "true" );
  addInteger( "maxsilent", "Number of stimulus presentations if response is below silentrate", 1, 1, 1000, 1 ).setActivation( "userate", "true" );
  addBoolean( "skippause", "Skip pause if response is below silentrate", true ).setActivation( "userate", "true" );
  addInteger( "maxsearch", "Stop search if response does not change for more than", 1, 1, 1000, 1 ).setUnit( "intensities" ).setActivation( "userate", "true" );
  addSelection( "method", "Method", "Bisect|Interpolate" ).setActivation( "userate", "true" );
  addNumber( "minslope", "Minimum slope required for interpolation", 4.0, 0.0, 100.0, 0.5, "Hz/"+AmplitudeUnit ).setActivation( "userate", "true" );
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
  addSelection( "storelevel", "Save", "all|generated|noise|none" ).setUnit( "stimuli" );

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
  SP[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		      Plot::Center, -90.0 );
  SP[2].setOrigin( 0.7, 0.1 );
  SP[2].setSize( 0.3, 0.8 );
  SP[2].setLMarg( 7.0 );
  SP[2].setRMarg( 1.5 );
  SP[2].setTMarg( 3.0 );
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
  P[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P.unlock();

}


SingleStimulus::~SingleStimulus( void )
{
}


void SingleStimulus::config( void )
{
  /*
  string its = "";
  const InList &il = traces();
  for ( int k=0; k<il.size(); k++ ) {
    if ( k > 0 )
      its += '|';
    its += il[k].ident();
  }
  setText( "intrace", its );
  setToDefault( "intrace" );
  */
  
  string ots = "";
  for ( int k=0; k<outTracesSize(); k++ ) {
    if ( k > 0 )
      ots += '|';
    ots += outTraceName( k );
  }
  setText( "outtrace", ots );
  setToDefault( "outtrace" );
}


void SingleStimulus::readConfig( StrQueue &sq )
{
  for ( int k=0; k<sq.size(); k++ ) {
    if ( sq[k].find( "outtrace" ) >= 0 )
      Options::read( sq[k] );
  }
  RePro::readConfig( sq );
}


void SingleStimulus::notify( void )
{
  int outtrace = index( "outtrace" );
  AmplitudeUnit = outTrace( outtrace ).unit();
  setUnit( "amplitude", AmplitudeUnit );
  setUnit( "offset", AmplitudeUnit );
  setUnit( "offsetstep", AmplitudeUnit );
  setUnit( "minslope", "Hz/"+AmplitudeUnit );
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
  WaveForm = (WaveForms)index( "waveform" );
  Str stimfile = text( "stimfile" );
  PeakAmplitudeFac = number( "stimampl" );
  Frequency = number( "freq" );
  DutyCycle = number( "dutycycle" );
  Seed = integer( "seed" );
  Amplitude = number( "amplitude" );
  double offset = number( "offset" );
  int offsetbase = index( "offsetbase" );
  Repeats = integer( "repeats" );
  Duration = number( "duration" );
  double pause = number( "pause" );
  Ramp = number( "ramp" );
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

  if ( offsetbase == 1 )
    Offset = offset + metaData().number( "best threshold" );	// get "best thresh" from FICurve (via the session widget)
  else if ( offsetbase == 2 )
    Offset = offset + metaData().number( "best offset" );	// get "best offset" from FICurve (via the session widget)
  else if ( offsetbase == 3 )
    Offset += 0.0;
  else
    Offset = offset;

  bool sameduration = ( Duration == searchduration );
  bool storedstimulus = false;

  OutData signal;

  // search for offset that evokes the target firing rate:
  if ( userate ) {

    // stimulus:
    signal.setTrace( outtrace );
    applyOutTrace( signal );
    int r = createStimulus( signal, stimfile, searchduration, sameduration );
    if ( r < 0 )
      return Failed;
    storedstimulus = sameduration;
    
    // plot trace:
    plotToggle( true, true, searchduration, 0.0 );
    
    P.hide();
    SP.show();
    SP.clearPlots();
    SP[0].setTitle( "Search target firing rate " + Str( targetrate ) + " Hz" );
    SP[1].setYLabel( "Stimulus [" + AmplitudeUnit + "]" );
    SP[2].setXLabel( "Offset [" + AmplitudeUnit + "]" );
    SP.draw();

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
    
      EventList spikes;

      for ( int counter=0; counter<searchrepeats; counter++ ) {
	
	// message:
	Str s = "Search rate <b>" + Str( targetrate ) + " Hz</b>";
	s += ",  <b>" + StimulusLabel + "</b>";
	s += ":  Offset: <b>" + Str( Offset, 0, 0, 'f' ) + " " + AmplitudeUnit + "</b>";
	s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( searchrepeats ) + "</b>";
	message( s );

	// output:
	write( signal );
	if ( ! signal.success() )
	  break;
	sleep( searchduration );
	if ( interrupt() ) {
	  writeZero( outtrace );
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
	  double threshold = metaData().number( "best threshold" );
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
	    SP[1].setXRange( 1000.0*SkipWin, 1000.0*searchduration );
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
	  SP.unlock();
	  SP.draw();
	}

	if ( rate >= silentrate ) {
	  sleep( searchpause );
	  if ( interrupt() ) {
	    writeZero( outtrace );
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
	      writeZero( outtrace );
	      return Aborted;
	    }
	  }
	  if ( counter+1 >= maxsilent )
	    break;
	}
      
	if ( softStop() > 1 ) {
	  writeZero( outtrace );
	  return Failed;
	}

      }

      double rate = spikes.rate( SkipWin, searchduration );
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
	    writeZero( outtrace );
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
	      writeZero( outtrace );
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
	    writeZero( outtrace );
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
	else if ( signal.error() ) {
	  warning( "Output of stimulus failed!<br>Signal error: <b>" +
		   signal.errorText() + "</b><br>Exit now!" );
	  writeZero( outtrace );
	  return Failed;
	}
	else {
	  warning( "Could not establish firing rate!" );
	  writeZero( outtrace );
	  return Failed;
	}
      }

    }

  }

  // stimulus:
  if ( ! sameduration || ! storedstimulus ) {
    signal.setTrace( outtrace );
    applyOutTrace( signal );
    int r = createStimulus( signal, stimfile, Duration, true );
    if ( r < 0 ) {
      writeZero( outtrace );
      return Failed;
    }
    signal += Offset;
  }

  // check signal:
  testWrite( signal );

  // plot trace:
  plotToggle( true, true, Duration, 0.0 );

  // setup plots:
  SP.hide();
  P.show();
  P.lock();
  P.clearPlots();
  P[0].setTitle( "Mean firing rate =    Hz" );
  P[1].setYLabel( "Stimulus [" + AmplitudeUnit + "]" );
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
    Str s =  "<b>" + StimulusLabel + "</b>";
    s += ",  Offset: <b>" + Str( Offset, 0, 1, 'f' ) + " " + AmplitudeUnit + "</b>";
    s += ",  Amplitude: <b>" + Str( Amplitude, 0, 1, 'f' ) + " " + AmplitudeUnit + "</b>";
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
      writeZero( outtrace );
      return Failed;
    }

    sleep( Duration + ( pause > 0.01 ? 0.01 : pause ) );

    if ( interrupt() ) {
      save( spikes, rate1, rate2 );
      writeZero( outtrace );
      return Aborted;
    }
    
    analyze( spikes, rate1, rate2 );
    plot( spikes, rate1, rate2, signal );
    
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
      writeZero( outtrace );
      return Completed;
    }
    
  }
  
  save( spikes, rate1, rate2 );
  writeZero( outtrace );
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
  header.addText( "outtrace" );
  header.addNumber( "offset", Offset, AmplitudeUnit, "%.1f" );
  header.addNumber( "amplitude", Amplitude, AmplitudeUnit, "%.1f" );
  header.addNumber( "amplfac", PeakAmplitudeFac, "", "%.3f" );
  header.addNumber( "duration", 1000.0*Duration, "ms", "%.1f" );
  header.addText( "envelope", StoreFile );
  header.addText( "session time", sessionTimeStr() ); 
  header.addLabel( "settings:" );

  saveSpikes( header, spikes );
  saveRate( header, rate1, rate2 );
}


void SingleStimulus::plot( const EventList &spikes, const SampleDataD &rate1,
			   const SampleDataD &rate2, const OutData &signal )
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
  double threshold = metaData().number( "best threshold" );
  double ymin = Offset - PeakAmplitude;
  double ymax = Offset + PeakAmplitude;
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
  P[1].plotHLine( Offset, Plot::White, 2 );
  if ( threshold > 0.0 )
    P[1].plotHLine( threshold, Plot::Yellow, 2 );
  P[1].plot( signal, 1000.0, Plot::Green, 2 );
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
				    double &duration, bool storesignal )
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
	double deltat = 1.0;
	while ( deltat >= 1.0/Frequency )
	  deltat *= 0.1;
	if ( WaveForm == Whitenoise )
	  wave.whiteNoise( duration, deltat, 0.0, Frequency, rand );
	else if ( WaveForm == OUnoise )
	  wave.ouNoise( duration, deltat, 1.0/Frequency, rand );
	PeakAmplitudeFac = 0.3;
	header.addInteger( "random seed", int( seed ) );
      }
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
      wave = 2.0*wave - 1.0;
    }

    if ( StoreLevel == Noise && 
	 ( WaveForm == Whitenoise || WaveForm == OUnoise ) ) 
      store = true;
    if ( StoreLevel == Generated ) 
      store = true;
  }
  else {
    // constant:
    wave = SampleDataD( 0.0, duration, 0.001, 0.0 );
    PeakAmplitudeFac = 1.0;
    header.addText( "waveform", "const" );
    if ( StoreLevel == Generated ) 
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
  /*
    int c = ::relacs::clip( -1.0, 1.0, wave );
    double cp = 100.0*double(c)/wave.size();
    if ( cp > 0.0 )
      info( "Clipped " + Str( cp, 0, 3, 'g' ) + "% of the stimulus waveform.", 4.0 );
   */

  Str labelpattern = "$(waveform)$(filename)$(frequency)$(random seed) $(dutycycle)";
  StimulusLabel = translate( labelpattern, header );
  if ( wavename.empty() )
    wavename = StimulusLabel;

  signal.clear();
  signal = wave;
  signal *= Amplitude;
  signal.ramp( Ramp );
  PeakAmplitude = Amplitude / PeakAmplitudeFac;

  signal.setIdent( "signal=" + wavename + ", amplitude=" + Str( Amplitude ) + AmplitudeUnit );
  header.addText( "amplitude", Str( Amplitude ) + AmplitudeUnit );
  header.addText( "amplitudefactor", Str( PeakAmplitude, 0, 3, 'g' ) );
  if ( storesignal && store ) {
    Str filepattern = "$(waveform)$(filename)$(frequency)$(random seed)$(dutycycle)r$(ramp)$(duration)$(amplitude)$(amplitudefactor).dat";
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
      signal.save( of, 7, 5 );
      printlog( "wrote " + StoreFile );;
    }
  }
  
  return 0;
}


addRePro( SingleStimulus );

}; /* namespace patchclamp */

#include "moc_singlestimulus.cc"
