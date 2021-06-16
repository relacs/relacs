/*
  efish/threefish.cc
  Beats of three fish.

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

#include <vector>
#include <relacs/map.h>
#include <relacs/outdata.h>
#include <relacs/kernel.h>
#include <relacs/rangeloop.h>
#include <relacs/efish/threefish.h>
using namespace relacs;

namespace efish {


ThreeFish::ThreeFish( void )
  : RePro( "ThreeFish", "efish", "Jan Benda", "1.0", "Jun 15, 2021" )
{
  newSection( "Stimulus" );
  addNumber( "duration1", "Duration of signal", 1.0, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "deltaf1min", "Minimum delta f (beat frequency) of first fish", 0.0, -10000.0, 10000.0, 5.0, "Hz" );
  addNumber( "deltaf1max", "Maximum delta f (beat frequency) of first fish", 100.0, -10000.0, 10000.0, 5.0, "Hz" );
  addNumber( "deltaf1step", "Increment delta f (beat frequency) of first fish", 10.0, 0.0, 1000.0, 1.0, "Hz" );
  addNumber( "contrast1", "Contrast of first fish", 0.1, 0.0, 1.0, 0.01, "", "%" );

  addNumber( "duration2", "Duration of second fish", 1.0, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "offset2", "Offset of second fish", 0.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "deltaf2min", "Minimum delta f (beat frequency) of second fish", 0.0, -10000.0, 10000.0, 5.0, "Hz" );
  addNumber( "deltaf2max", "Maximum delta f (beat frequency) of second fish", 100.0, -10000.0, 10000.0, 5.0, "Hz" );
  addNumber( "deltaf2step", "Increment delta f (beat frequency) of second fish", 10.0, 0.0, 1000.0, 1.0, "Hz" );
  addNumber( "contrast2", "Contrast of second fish", 0.1, 0.0, 1.0, 0.01, "", "%" );
  addSelection( "shuffle", "Order of delta f's", RangeLoop::sequenceStrings() );
  addInteger( "increment", "Initial increment for delta f's", -1, -1000, 1000, 1 );
  addInteger( "repeats", "Repeats", 10, 1, 100000, 2 );
  addNumber( "pause", "Pause between signals", 0.1, 0.0, 1000.0, 0.01, "seconds", "ms" );
  newSection( "Analysis" );
  addNumber( "before", "Spikes recorded before stimulus", 0.1, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addNumber( "after", "Spikes recorded after stimulus", 0.1, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addNumber( "sigma", "Standard deviation of rate smoothing kernel", 0.01, 0.0, 1.0, 0.0001, "seconds", "ms" );

  // plot:
  setWidget( &P );
}


int ThreeFish::fishEOD(double &rate, double &amplitude)
{
  // EOD rate:
  if ( EODEvents >= 0 )
    rate = events( EODEvents ).frequency( currentTime() - 0.5, currentTime() );
  else if ( LocalEODEvents[0] >= 0 )
    rate = events( LocalEODEvents[0] ).frequency( currentTime() - 0.5, currentTime() );
  else {
    warning( "No EOD present or not enough EOD cycles recorded!" );
    return 1;
  }

  // EOD amplitude:
  double ampl = eodAmplitude( trace( LocalEODTrace[0] ),
			      currentTime() - 0.5, currentTime() );
  if ( ampl <= 1.0e-8 ) {
    warning( "No EOD amplitude on local EOD electrode!" );
    return 1;
  }
  else
    amplitude = ampl;
  return 0;
}


int ThreeFish::makeEOD(double fishrate, double deltaf, double duration,
		       double phase, OutData &eod)
{
  double stimulusrate = deltaf;
  stimulusrate += fishrate;
  if ( stimulusrate < 1.0e-8 ) {
    warning( "Stimulus frequency is negative!" );
    return 1;
  }
  if ( fabs( deltaf ) > 1e-6 ) {
    double p = rint( stimulusrate / fabs( deltaf ) ) / stimulusrate;
    if ( stimulusrate < fabs( deltaf ) )
      p = 1.0/fabs( deltaf );
    int n = (int)::rint( duration / p );
    if ( n < 1 )
      n = 1;
    duration = n*p;
  }
  eod.setTrace( GlobalEField );
  eod.sineWave( duration, eod.minSampleInterval(), stimulusrate, phase );
  return 0;
}


int ThreeFish::main( void )
{
  // get options:
  double duration1 = number( "duration1" );
  double deltaf1min = number( "deltaf1min" );
  double deltaf1max = number( "deltaf1max" );
  double deltaf1step = number( "deltaf1step" );
  double contrast1 = number( "contrast1" );
  double duration2 = number( "duration2" );
  double offset2 = number( "offset2" );
  double deltaf2min = number( "deltaf2min" );
  double deltaf2max = number( "deltaf2max" );
  double deltaf2step = number( "deltaf2step" );
  double contrast2 = number( "contrast2" );
  RangeLoop::Sequence deltafshuffle = RangeLoop::Sequence( index( "shuffle" ) );
  int increment = integer( "increment" );
  int repeats = integer( "repeats" );
  double pause = number( "pause" );
  double before = number( "before" );
  double after = number( "after" );
  double sigma = number( "sigma" );

  // check EODs:
  if ( LocalEODTrace[0] < 0 || LocalEODEvents[0] < 0 ) {
    warning( "Local EOD recording with EOD events required!" );
    return Failed;
  }

  // plot trace:
  tracePlotSignal( duration1, 0.0 );

  // clear output lines:
  writeZero( GlobalAMEField );
  sleep( 0.5 );

  // EOD:
  double fishrate = 0.0;
  double fishamplitude = 0.0;
  if ( fishEOD(fishrate, fishamplitude) )
    return Failed;

  // adjust transdermal EOD:
  adjustGain( trace( LocalEODTrace[0] ),
	      ( 1.0 + contrast1 + contrast2 ) * 1.1 * fishamplitude );
  detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold",
    min(contrast1, contrast2)*fishamplitude );

  // delta f ranges:
  RangeLoop dfrange1;  
  dfrange1.set( deltaf1min, deltaf1max, deltaf1step, 1, 1, 1 );
  dfrange1.setIncrement( increment );
  dfrange1.setSequence( deltafshuffle );

  RangeLoop dfrange2;  
  dfrange2.set( deltaf2min, deltaf2max, deltaf2step, 1, 1, repeats );
  dfrange2.setIncrement( increment );
  dfrange2.setSequence( deltafshuffle );

  // data:
  vector< MapD > amtraces;
  amtraces.reserve( repeats );
  EventList spikes;
  SampleDataD spikerate( -before, duration1+after, 0.0005 );
  double maxrate = 100.0;
      
  // plot:
  P.lock();
  P.resize( 2 );
  P[0].setOrigin( 0.0, 0.0 );
  P[0].setSize( 1.0, 0.4 );
  P[0].setLMarg( 6 );
  P[0].setRMarg( 2 );
  P[0].setBMarg( 3 );
  P[0].setTMarg( 1 );
  P[0].setXLabel( "[s]" );
  P[0].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::FirstAxis, 
		     Plot::Left, 0.0 );
  P[0].setXTics();
  if ( duration1 <= 1.0 )
    P[0].setXRange( 0.0, duration1 );
  else
    P[0].setXRange( 0.0, 1.0 );
  P[0].setYLabel( "AM [" + trace( LocalEODTrace[0] ).unit() + "]" );
  P[0].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
		     Plot::Center, -90.0 );
  P[0].setAutoScaleY();
  P[0].setYTics( );

  P[1].setOrigin( 0.0, 0.4 );
  P[1].setSize( 1.0, 0.6 );
  P[1].setLMarg( 6 );
  P[1].setRMarg( 2 );
  P[1].setBMarg( 1 );
  P[1].setTMarg( 1 );
  P[1].setXLabel( "" );
  P[1].noXTics();
  P[1].setYLabel( "Firing rate [Hz]" );
  P[1].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, 
		     Plot::Center, -90.0 );
  P[1].setYRange( 0.0, 100.0 );
  P[1].setYTics( );
  P[1].clear();
  if ( duration1 <= 1.0 )
    P[1].setXRange( 0.0, duration1 );
  else
    P[1].setXRange( 0.0, 1.0 );
  P.setCommonXRange( 0, 1 );
  P.draw();
  P.unlock();

  timeStamp();

  for ( dfrange1.reset(); ! dfrange1 && softStop() == 0; ++dfrange1 ) {
    double deltaf1 = *dfrange1;
    if ( fabs( deltaf1) < 1e-6 )
      continue;
    OutData fish1;
    if ( makeEOD(fishrate, deltaf1, duration1, 0.0, fish1) ) {
      stop();
      return Failed;
    }
    fish1 *= contrast1/(contrast1 + contrast2);
    fish1.description().insertNumber( "DeltaF", "Phase", deltaf1, "Hz" );
    fish1.description().insertNumber( "Contrast", "Frequency", 100.0*contrast1, "%" );
    fish1.description()["Frequency"].addFlags( OutData::Mutable );
    fish1.description()["DeltaF"].addFlags( OutData::Mutable );

    for ( dfrange2.reset(); ! dfrange2 && softStop() == 0; ++dfrange2 ) {
      double deltaf2 = *dfrange2;
      if ( fabs( deltaf2) < 1e-6 )
	continue;
      OutData fish2;
      if ( makeEOD(fishrate, deltaf2, duration2, 0.25*6.28318, fish2) ) {
	stop();
	return Failed;
      }
      fish2 *= contrast2/(contrast1 + contrast2);
      double offs = offset2;
      if ( fabs( deltaf1 ) > 1e-6 && offs > 1e-6 ) {
	int n = ::round( offs * deltaf1 );
	if ( n < 1 )
	  n = 1;
	offs = n / deltaf1;
      }
      fish2.setOffset( offs );
      fish2.description().insertNumber( "DeltaF", "Phase", deltaf2, "Hz" );
      fish2.description().insertNumber( "Contrast", "Frequency", 100.0*contrast2, "%" );
      fish2.description()["Frequency"].addFlags( OutData::Mutable );
      fish2.description()["DeltaF"].addFlags( OutData::Mutable );

      OutData signal( fish1 );
      signal += fish2;
      signal.setDelay( before );
      signal.clearError();

      // stimulus intensity:
      double intensity = (contrast1+contrast2) * fishamplitude;
      signal.setIntensity( intensity );
      
      // meassage: 
      Str s = "Delta F1: <b>" + Str( deltaf1, 0, 1, 'f' ) + "Hz</b>";
      s += "  Contrast1: <b>" + Str( 100.0 * contrast1, 0, 5, 'g' ) + "%</b>";
      s += "  Delta F2: <b>" + Str( deltaf2, 0, 1, 'f' ) + "Hz</b>";
      s += "  Contrast2: <b>" + Str( 100.0 * contrast2, 0, 5, 'g' ) + "%</b>";
      s += "  Loop: <b>" + Str( dfrange2.count() ) + "</b>";
      message( s );

      write( signal );

      sleepOn( signal.duration() + pause );
      if ( interrupt() ) {
	//save();
	stop();
	return Aborted;
      }
      timeStamp();

      // analyze:
      if ( dfrange2.finishedSingle() ) {
	amtraces.clear();
	spikes.clear();
	maxrate = 100.0;
      }
      spikes.push( events( SpikeEvents[0] ), signalTime()-before,
		   signalTime()+duration1+after, signalTime() );
      spikes.rate( spikerate, GaussKernel( sigma ) );
      double maxr = max( spikerate );
      if ( maxr+100.0 > maxrate )
	maxrate = ::ceil((maxr+100.0)/20.0)*20.0;

      // EOD transdermal amplitude:
      const EventData &localeod = events( LocalEODEvents[0] );
      EventSizeIterator pindex = localeod.begin( signalTime() );
      EventSizeIterator plast = localeod.begin( signalTime() + duration1 );
      amtraces.push_back( MapD() );
      amtraces.back().reserve( plast - pindex + 1 );
      for ( ; pindex < plast; ++pindex )
	amtraces.back().push( pindex.time() - signalTime(), *pindex ); 

      plot( amtraces, spikes, spikerate, maxrate, repeats );

      fishEOD(fishrate, fishamplitude);
    }
  }

  stop();
  return Completed;
}


void ThreeFish::plot( const vector< MapD > &amtraces, const EventList &spikes,
		      const SampleDataD &spikerate, double maxrate, int repeats )
{
  P.lock();
  // stimulus:
  P[0].clear();
  for ( unsigned int i=0; i<amtraces.size()-1; i++ )
    P[0].plot( amtraces[i], 1.0, Plot::DarkGreen, 2, Plot::Solid );
  P[0].plot( amtraces.back(), 1.0, Plot::Green, 2, Plot::Solid );

  // rate and spikes:
  int maxspikes = 20;
  if ( SpikeEvents[0] >= 0 ) {
    P[1].clear();
    if ( ! P[1].zoomedYRange() )
      P[1].setYRange( 0.0, maxrate );
    int j = 0;
    double delta = repeats > 0 && repeats < maxspikes ? 1.0/repeats : 1.0/maxspikes;
    int offs = (int)spikes.size() > maxspikes ? spikes.size() - maxspikes : 0;
    for ( int i=offs; i<spikes.size(); i++ ) {
      j++;
      P[1].plot( spikes[i], 1.0,
		 1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp,
		 delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
    }
    P[1].plot( spikerate, 1.0, Plot::Yellow, 2, Plot::Solid );
  }
  P.draw();
  P.unlock();
}


void ThreeFish::stop( void )
{
  P.lock();
  P.clearPlots();
  P.unlock();
}


addRePro( ThreeFish, efish );

}; /* namespace efish */

#include "moc_threefish.cc"
