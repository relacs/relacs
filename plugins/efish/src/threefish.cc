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
#include <relacs/tablekey.h>
#include <relacs/outdata.h>
#include <relacs/kernel.h>
#include <relacs/rangeloop.h>
#include <relacs/efish/threefish.h>
using namespace relacs;

namespace efish {


ThreeFish::ThreeFish( void )
  : RePro( "ThreeFish", "efish", "Jan Benda", "1.2", "Jun 16, 2021" )
{
  newSection( "Stimulus" );
  addBoolean( "relativeeods", "stimulus frequencies are given relative to the fish's eodf", true );
  addBoolean( "absfreqs", "stimulus frequencies are given as absolute frequencies", true ).setActivation( "relativeeods", "false" );

  addNumber( "duration1", "Duration of signal", 1.0, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "deltaf1min", "Minimum delta f (beat frequency) of first fish", 0.0, -100.0, 100.0, 0.01, "EODf" ).setActivation( "relativeeods", "true" );
  addNumber( "deltaf1max", "Maximum delta f (beat frequency) of first fish", 0.5, -100.0, 100.0, 0.01, "EODf" ).setActivation( "relativeeods", "true" );
  addNumber( "deltaf1step", "Increment delta f (beat frequency) of first fish", 0.1, 0.0, 100.0, 0.01, "EODf" ).setActivation( "relativeeods", "true" );
  addNumber( "deltaf1minabs", "Minimum delta f (beat frequency) of first fish", 10.0, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "false" );
  addNumber( "deltaf1maxabs", "Maximum delta f (beat frequency) of first fish", 20, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "false" );
  addNumber( "deltaf1stepabs", "Increment delta f (beat frequency) of first fish", 10, 0.0, 500.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "false" );
  addNumber( "freq1min", "Minimum frequency of first fish", 10.0, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "true" );
  addNumber( "freq1max", "Maximum frequency of first fish", 20, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "true" );
  addNumber( "freq1step", "Increment frequency of first fish", 10, 0.0, 500.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "true" );
  addNumber( "contrast1", "Contrast of first fish", 0.1, 0.0, 1.0, 0.01, "", "%" );

  addNumber( "duration2", "Duration of second fish", 1.0, 0.01, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "delay2", "Delay of second fish", 0.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "deltaf2min", "Minimum delta f (beat frequency) of second fish", 0.0, -100.0, 100.0, 0.01, "EODf" ).setActivation( "relativeeods", "true" );
  addNumber( "deltaf2max", "Maximum delta f (beat frequency) of second fish", 0.5, -100.0, 100.0, 0.01, "EODf" ).setActivation( "relativeeods", "true" );
  addNumber( "deltaf2step", "Increment delta f (beat frequency) of second fish", 0.1, 0.0, 100.0, 0.01, "EODf" ).setActivation( "relativeeods", "true" );
  addNumber( "deltaf2minabs", "Minimum delta f (beat frequency) of second fish", 10.0, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "false" );
  addNumber( "deltaf2maxabs", "Maximum delta f (beat frequency) of second fish", 20, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "false" );
  addNumber( "deltaf2stepabs", "Increment delta f (beat frequency) of second fish", 10, 0.0, 500.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "false" );
  addNumber( "freq2min", "Minimum frequency of second fish", 10.0, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "true" );
  addNumber( "freq2max", "Maximum frequency of second fish", 20, 0.0, 5000.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "true" );
  addNumber( "freq2step", "Increment frequency of second fish", 10, 0.0, 500.0, 0.01, "Hz" ).setActivation( "relativeeods", "false" ).addActivation( "absfreqs", "true" );
  
  addNumber( "contrast2", "Contrast of second fish", 0.1, 0.0, 1.0, 0.01, "", "%" );
  addSelection( "shuffle", "Order of delta f's", RangeLoop::sequenceStrings() );
  addInteger( "increment", "Initial increment for delta f's", -1, -1000, 1000, 1 );
  addInteger( "repeats", "Repeats", 10, 1, 100000, 2 );
  addNumber( "pause", "Pause between signals", 0.5, 0.0, 1000.0, 0.01, "seconds", "ms" );
  newSection( "Analysis" );
  addNumber( "before", "Spikes recorded before stimulus", 0.1, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addNumber( "after", "Spikes recorded after stimulus", 0.1, 0.0, 1000.0, 0.005, "seconds", "ms" );
  addNumber( "sigma", "Standard deviation of rate smoothing kernel", 0.01, 0.0, 1.0, 0.0001, "seconds", "ms" );

  // plot:
  setWidget( &P );
}


int ThreeFish::fishEOD(double pause, double &rate, double &amplitude)
{
  // EOD rate:
  if ( EODEvents >= 0 )
    rate = events( EODEvents ).frequency( currentTime() - pause, currentTime() );
  else if ( LocalEODEvents[0] >= 0 )
    rate = events( LocalEODEvents[0] ).frequency( currentTime() - pause, currentTime() );
  else {
    warning( "No EOD present or not enough EOD cycles recorded!" );
    return 1;
  }

  // EOD amplitude:
  double ampl = eodAmplitude( trace( LocalEODTrace[0] ),
			      currentTime() - pause, currentTime() );
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
  bool releods = boolean( "relativeeods" );
  bool absfreqs = boolean( "absfreqs" );

  double deltaf1min, deltaf1max, deltaf1step;
  double deltaf2min, deltaf2max, deltaf2step;

  if ( releods ) {
    deltaf1min = number( "deltaf1min" );
    deltaf1max = number( "deltaf1max" );
    deltaf1step = number( "deltaf1step" );

    deltaf2min = number( "deltaf2min" );
    deltaf2max = number( "deltaf2max" );
    deltaf2step = number( "deltaf2step" );
  } else {
    if ( absfreqs ) {
      deltaf1min = number( "freq1min" );
      deltaf1max = number( "freq1max" );
      deltaf1step = number( "freq1step" );

      deltaf2min = number( "freq2min" );
      deltaf2max = number( "freq2max" );
      deltaf2step = number( "freq2step" );
    } else {
      deltaf1min = number( "deltaf1minabs" );
      deltaf1max = number( "deltaf1maxabs" );
      deltaf1step = number( "deltaf1stepabs" );

      deltaf2min = number( "deltaf2minabs" );
      deltaf2max = number( "deltaf2maxabs" );
      deltaf2step = number( "deltaf2stepabs" );
    }
  }  
  double duration1 = number( "duration1" );
  double contrast1 = number( "contrast1" );

  double duration2 = number( "duration2" );
  double delay2 = number( "delay2" );
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

  if ( pause < after ) {
    warning( "Pause too short!", 4.0 );
    pause = after;
  }

  if ( delay2 + duration2 > duration1 ) {
    warning( "Signal of second fish too long!", 4.0 );
    duration2 = duration1 - delay2;
  }

  // plot trace:
  tracePlotSignal( duration1, 0.0 );

  // clear output lines:
  writeZero( GlobalAMEField );
  sleep( pause );

  // EOD:
  double fishrate = 0.0;
  double fishamplitude = 0.0;
  if ( fishEOD(pause, fishrate, fishamplitude) )
    return Failed;

  // adjust transdermal EOD:
  adjustGain( trace( LocalEODTrace[0] ),
	      ( 1.0 + contrast1 + contrast2 ) * 1.1 * fishamplitude );
  detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold",
    min(contrast1, contrast2)*fishamplitude );

  // delta f ranges:
  cerr << "Deltafs: " <<  deltaf1min << "," << deltaf1max << "\t" <<  deltaf1min << "," << deltaf1max << endl;

  RangeLoop dfrange1;  
  dfrange1.set( deltaf1min, deltaf1max, deltaf1step, 1, 1, 1 );
  dfrange1.setIncrement( increment );
  dfrange1.setSequence( deltafshuffle );

  RangeLoop dfrange2;  
  dfrange2.set( deltaf2min, deltaf2max, deltaf2step, 1, 1, repeats );
  dfrange2.setIncrement( increment );
  dfrange2.setSequence( deltafshuffle );

  // data:
  double deltaf1 = 0.0;
  double deltaf2 = 0.0;
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
    for ( dfrange2.reset(); ! dfrange2 && softStop() == 0; ++dfrange2 ) {
      // stimulus:
      cerr << *dfrange1 << endl;
      cerr << *dfrange2 << endl;
      if ( releods )
        deltaf1 = *dfrange1 * fishrate;
      else {
        if ( absfreqs )
          deltaf1 = *dfrange1;
        else
          deltaf1 = *dfrange1 + fishrate;
      }
      if ( fabs( deltaf1 ) < 1e-6 )
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
      fish1.description()["Duration"].addFlags( OutData::Mutable );

      if ( releods )
        deltaf2 = *dfrange2 * fishrate;
      else {
        if ( absfreqs )
          deltaf2 = *dfrange2;
        else
          deltaf2 = *dfrange2 + fishrate;
      }
      if ( fabs( deltaf2) < 1e-6 )
	      continue;
      OutData fish2;
      if ( makeEOD(fishrate, deltaf2, duration2, 0.25*6.28318, fish2) ) {
	      stop();
	      return Failed;
      }
      fish2 *= contrast2/(contrast1 + contrast2);
      double offs = delay2;
      if ( fabs( deltaf1 ) > 1e-6 && offs > 1e-6 ) {
	      int n = ::round( offs * deltaf1 );
	      if ( n < 1 )
	        n = 1;
	      offs = n / deltaf1;
      }
      fish2.setOffset( offs );
      fish2.description().setName("fish2");
      fish2.description().insertNumber( "DeltaF", "Phase", deltaf2, "Hz" );
      fish2.description().insertNumber( "Contrast", "Frequency", 100.0*contrast2, "%" );
      fish2.description()["Frequency"].addFlags( OutData::Mutable );
      fish2.description()["DeltaF"].addFlags( OutData::Mutable );
      fish2.description()["Duration"].addFlags( OutData::Mutable );
      cerr << "Stimulus: " <<  deltaf1 << "\t" << deltaf2 << endl;
      OutData signal( fish1 );
      signal += fish2;
      signal.setDelay( before );
      signal.clearError();

      // stimulus intensity:
      double intensity = (contrast1+contrast2) * fishamplitude;
      signal.setIntensity( intensity );
      
      // message: 
      Str s = "Delta F1: <b>" + Str( deltaf1, 0, 1, 'f' ) + "Hz</b>";
      s += "  Contrast1: <b>" + Str( 100.0 * contrast1, 0, 5, 'g' ) + "%</b>";
      s += "  Delta F2: <b>" + Str( deltaf2, 0, 1, 'f' ) + "Hz</b>";
      s += "  Contrast2: <b>" + Str( 100.0 * contrast2, 0, 5, 'g' ) + "%</b>";
      s += "  Loop: <b>" + Str( dfrange2.count() ) + "</b>";
      message( s );

      write( signal );

      sleepOn( signal.duration() + pause );
      if ( interrupt() ) {
	save( fishrate, fishamplitude, deltaf1, deltaf2,
	      spikes, spikerate, amtraces );
	stop();
	return Aborted;
      }
      timeStamp();

      // analyze:
      analyze( amtraces, spikes, spikerate, maxrate, duration1, before, after, sigma );

      plot( amtraces, spikes, spikerate, maxrate, repeats );

      if ( dfrange2.lastSingle() ) {
	save( fishrate, fishamplitude, deltaf1, deltaf2,
	      spikes, spikerate, amtraces );
	amtraces.clear();
	spikes.clear();
	maxrate = 100.0;
      }

      fishEOD(pause, fishrate, fishamplitude);
    }
  }

  save( fishrate, fishamplitude, deltaf1, deltaf2,
	spikes, spikerate, amtraces );
  stop();
  return Completed;
}


void ThreeFish::analyze( vector< MapD > &amtraces, EventList &spikes,
			 SampleDataD &spikerate, double &maxrate,
			 double duration, double before, double after, double sigma )
{
  spikes.push( events( SpikeEvents[0] ), signalTime()-before,
	       signalTime()+duration+after, signalTime() );
  spikes.rate( spikerate, GaussKernel( sigma ) );
  double maxr = max( spikerate );
  if ( maxr+100.0 > maxrate )
    maxrate = ::ceil((maxr+100.0)/20.0)*20.0;

  // EOD transdermal amplitude:
  const EventData &localeod = events( LocalEODEvents[0] );
  EventSizeIterator pindex = localeod.begin( signalTime() );
  EventSizeIterator plast = localeod.begin( signalTime() + duration );
  amtraces.push_back( MapD() );
  amtraces.back().reserve( plast - pindex + 1 );
  for ( ; pindex < plast; ++pindex )
    amtraces.back().push( pindex.time() - signalTime(), *pindex ); 
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


void ThreeFish::saveRate( const Options &header, const SampleDataD &spikerate )
{
  // create file:
  ofstream df( addPath( "threefish-rate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%9.2f" );
  key.addNumber( "rate", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int j=0; j<spikerate.size(); j++ ) {
    key.save( df, 1000.0 * spikerate.pos( j ), 0 );
    key.save( df, spikerate[j] );
    df << '\n';
  }
  df << "\n\n";
}


void ThreeFish::saveSpikes( const Options &header, const EventList &spikes )
{
  // create file:
  ofstream df( addPath( "threefish-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  TableKey key;
  key.addNumber( "time", "ms", "%9.2f" );
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<spikes.size(); k++ ) {
    df << '\n';
    df << "# trial: " << k << '\n';
    if ( spikes[k].empty() ) {
      df << "  -0" << '\n';
    }
    else {
      for ( int j=0; j<spikes[k].size(); j++ ) {
	key.save( df, 1000.0 * spikes[k][j], 0 );
	df << '\n';
      }
    }
  }
  df << "\n\n";
}


void ThreeFish::saveAmpl( const Options &header, const vector< MapD > &amtraces )
{
  // create file:
  ofstream df( addPath( "threefish-ams.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  TableKey key;
  key.addNumber( "time", "ms", "%9.2f", 3 );
  key.addNumber( "ampl", trace( LocalEODTrace[0] ).unit(), "%5.3f", 3 );
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  key.saveKey( df, true, false, 1 );

  // write data:
  for ( unsigned int k=0; k<amtraces.size(); k++ ) {
    df << '\n';
    df << "# trial: " << k << '\n';
    for ( int j=0; j<amtraces[k].size(); j++ ) {
      key.save( df, 1000.0*amtraces[k].x(j), 0 );
      key.save( df, amtraces[k].y(j), 1 );
      df << '\n';
    }
  }
  df << "\n\n";
}


void ThreeFish::save( double fishrate, double fishamplitude, double deltaf1, double deltaf2,
		      const EventList &spikes, const SampleDataD &spikerate,
		      const vector< MapD > &amtraces )
{
  if ( spikes.empty() )
    return;
  
  Options header;
  header.addInteger( "index", totalRuns() );
  header.addNumber( "EOD rate", "Hz", "%.1f" ).setNumber( fishrate );
  header.addNumber( "EOD amplitude", trace( LocalEODTrace[0] ).unit(), "%.2f" ).setNumber( fishamplitude );
  header.addNumber( "Deltaf1", "Hz", "%.1f" ).setNumber( deltaf1 );
  header.addNumber( "Deltaf2", "Hz", "%.1f" ).setNumber( deltaf2 );
  header.addText( "session time", sessionTimeStr() );
  header.newSection( settings() );
  
  saveSpikes( header, spikes );
  saveRate( header, spikerate );
  saveAmpl( header, amtraces );
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
