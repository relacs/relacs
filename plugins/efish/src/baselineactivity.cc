/*
  efish/baselineactivity.cc
  ISI statistics and EOD locking of baseline activity.

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
#include <relacs/stats.h>
#include <relacs/kernel.h>
#include <relacs/tablekey.h>
#include <relacs/optwidget.h>
#include <relacs/detector.h>
#include <relacs/filter.h>
#include <relacs/efish/baselineactivity.h>
using namespace relacs;

namespace efish {


BaselineActivity::BaselineActivity( void )
  : RePro( "BaselineActivity", "efish", "Jan Benda", "2.2", "Oct 23, 2014" )
{
  // parameter:
  Duration = 0.3;
  Repeats = 0;
  BeatStep = 0.01;  // should not be smaller than the factor in BeatDetector::adjust()!
  ChirpStep = 2.0;
  ChirpMin = 10.0;

  // add some parameter as options:
  newSection( "Timing" ).setStyle( OptWidget::Bold );
  addNumber( "duration", "Duration", Duration, 0.01, 100.0, 0.05, "sec", "ms" );
  addInteger( "repeats", "Repeats", Repeats, 0, 100000, 2 ).setStyle( OptWidget::SpecialInfinite );
  newSection( "Analysis" ).setStyle( OptWidget::Bold );
  addNumber( "isimax", "Maximum interspike-interval", 0.02, 0.001, 1.0, 0.001, "sec", "ms" );
  addNumber( "isistep", "Interspike-interval resolution", 0.0002, 0.0001, 0.01, 0.0002, "sec", "ms" );
  addNumber( "ratedt", "Resolution of firing rate", 0.00005, 0.00001, 0.01, 0.00002, "s", "ms" );
  addNumber( "ratetmax", "Maximum time for firing rate", 0.002, 0.00001, 0.01, 0.00002, "s", "ms" );
  newSection( "Files" ).setStyle( OptWidget::Bold );
  addNumber( "eodduration", "EOD duration", 2.0, 0.1, 10.0, 0.1, "sec", "ms" );
  addBoolean( "saveeodtrace", "Save EOD trace", false );
  addBoolean( "saveeodtimes", "Save EOD times", false );
  newSection( "Control" ).setStyle( OptWidget::Bold );
  addText( "auto", "Automatically set detector parameter?", "never|once|always", 0, OptWidget::SelectText );
  addBoolean( "adjust", "Adjust input gain?", true );

  // variables:
  EODPeriod = 0.0;
  EODRate = 0.0;
  FirstSignal = 0.0;
  LastSignal = 0.0;
  SearchDuration = 0.0;
  LastEODInx = 0;

  setWidget( &P );
}


BaselineActivity::~BaselineActivity( void )
{
}


int BaselineActivity::main( void )
{
  // get options:
  Duration = number( "duration" );
  Repeats = integer( "repeats" );
  double isimax = number( "isimax" );
  double isistep = number( "isistep" );
  double ratedeltat = number( "ratedt" );
  double ratetmax = number( "ratetmax" );
  double eodduration = number( "eodduration" );
  bool saveeodtrace = boolean( "saveeodtrace" );
  bool saveeodtimes = boolean( "saveeodtimes" );
  int autodetect = index( "auto" );
  bool adjustg = boolean( "adjust" );

  // checks:
  if ( LocalEODTrace[0] < 0 ) {
    warning( "need local recording of the EOD Trace." );
    return Failed;
  }
  if ( LocalEODEvents[0] < 0 ) {
    warning( "need EOD events of local recording of the EOD Trace." );
    return Failed;
  }

  // don't print repro message:
  if ( Repeats <= 0 )
    noMessage();

  // don't save files:
  if ( Repeats <= 0 )
    noSaving();

  // plot trace:
  tracePlotContinuous( Duration );

  // plot:
  P.lock();
  P.resize( SpikeTraces*2+NerveTraces, 2, ( SpikeTraces < 2 ) );
  double pdx = 0.5;
  double pdy = 1.0;
  if ( SpikeTraces > 1 ) {
    pdx = 1.0 / SpikeTraces;
    pdy = 1.0 / ( 2 + NerveTraces );
  }
  else {
    pdx = 0.5;
    pdy = 1.0 / ( SpikeTraces + NerveTraces );
  }
  int n=0;
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Str ns = "";
      if ( SpikeTraces > 1 )
	ns = " " + Str( n+1, 0 );
      if ( SpikeTraces > 1 )
	P[2*n].setOrigin( n*pdx, ( NerveTraces + 1 )*pdy );
      else
	P[2*n].setOrigin( 0.0, NerveTraces*pdy );
      P[2*n].setSize( pdx, pdy );
      P[2*n].setTitle( "ISI Histogram" + ns );
      P[2*n].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
      P[2*n].setXLabel( "ISI [ms]" );
      P[2*n].noXGrid();
      P[2*n].setYRange( 0.0, Plot::AutoScale );
      P[2*n].setYFallBackRange( 0.0, 10.0 );
      P[2*n].setMinYTics( 1.0 );
      P[2*n].setYLabel( "" );
      P[2*n].setLMarg( 5.0 );

      if ( SpikeTraces > 1 )
	P[2*n+1].setOrigin( n*pdx, NerveTraces*pdy );
      else
	P[2*n+1].setOrigin( pdx, NerveTraces*pdy );
      P[2*n+1].setSize( pdx, pdy );
      P[2*n+1].setTitle( "EOD Firing Rate [Hz]" + ns );
      P[2*n+1].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
      P[2*n+1].setXLabel( "Time [ms]" );
      P[2*n+1].setYRange( 0.0, Plot::AutoMinScale );
      P[2*n+1].setYFallBackRange( 0.0, 100.0 );
      P[2*n+1].setMinYTics( 100.0 );
      P[2*n+1].setYLabel( "" );
      P[2*n+1].setLMarg( 5.0 );

      n++;
    }
  }
  if ( NerveTrace[0] >= 0 ) {
    P[2*n].setOrigin( 0.0, 0.0 );
    P[2*n].setSize( 1.0, pdy );
    P[2*n].setTitle( "Nerve Potential" );
    P[2*n].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
    P[2*n].setXLabel( "Time [ms]" );
    P[2*n].setAutoScaleY();
    P[2*n].setYLabel( "" );
    P[2*n].setLMarg( 5.0 );
    P[2*n].setRMarg( 1.0 );
    P[2*n].setBMarg( 5.5 );
  }
  P.unlock();

  // data:
  FirstSignal = 0.0;
  LastSignal = 0.0;
  SearchDuration = 0.0;
  LastEODInx = 0;

  EventList spikes( MaxTraces );
  vector< vector < ArrayD > > eodspikes;
  eodspikes.resize( MaxTraces );
  vector< SampleDataD > isih;
  vector< SampleDataD > spikerate;
  vector< int > trials;
  for ( int k=0; k<MaxTraces; k++ ) {
    spikes[k].clear();
    eodspikes[k].clear();
    trials.push_back( 0 );
    if ( SpikeEvents[k] >= 0 ) {
      isih.push_back( SampleDataD( 0.0, 2.0*isimax, isistep, 0.0 ) );
      spikerate.push_back( SampleDataD( 0.0, ratetmax, ratedeltat, 0.0 ) );
      if ( Repeats <= 0 ) {
	spikes[k].reserve( int( Duration * 1000.0 ) );
	eodspikes[k].reserve( int( Duration * 1000.0 ) );
      }
      else {
	spikes[k].reserve( int( Duration * Repeats * 1000.0 ) );
	eodspikes[k].reserve( int( Duration * Repeats * 1000.0 ) );
      }
    }
    else {
      isih.push_back( SampleDataD() );
      spikerate.push_back( SampleDataD() );
    }
  }
  EventData eodtimes;
  if ( Repeats <= 0 )
    eodtimes.reserve( int( Duration * 1500.0 ) );
  else {
    eodtimes.reserve( int( Duration * Repeats * 1500.0 ) );
  }
  SampleDataD eodcycle( 0.0, ratetmax, ratedeltat );
  EODPeriod = 0.0;
  EODRate = 0.0;
  MapD nerveamplp;
  MapD nerveamplt;
  MapD nerveamplm;
  SampleDataD nerveampls;
  if ( NerveTrace[0] >= 0 ) {
    nerveamplp.reserve( eodtimes.capacity() );
    nerveamplt.reserve( eodtimes.capacity() );
    nerveamplm.reserve( eodtimes.capacity() );
  }
  nerveamplp.clear();
  nerveamplt.clear();
  nerveamplm.clear();

  if ( autodetect > 0 && Repeats <= 0 ) {
    // setup Beat detector:
    if ( LocalBeatPeakEvents[0] >= 0 &&
	 ( totalRuns() <= 0 || autodetect > 1 ) ) {
      double beatthresh = BeatStep * trace( LocalEODTrace[0] ).maxValue();
      lockDetectorEvents( LocalBeatPeakEvents[0] );
      detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "minthresh", beatthresh );
      detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", beatthresh );
      unlockDetectorEvents( LocalBeatPeakEvents[0] );
    }
    // setup Chirp detector:
    if ( ChirpEvents >= 0 &&
	 ( totalRuns() <= 0 || autodetect > 1 ) ) {
      lockDetectorEvents( ChirpEvents );
      detectorEventsOpts( ChirpEvents ).setNumber( "minthresh", ChirpMin );
      detectorEventsOpts( ChirpEvents ).setNumber( "threshold", ChirpMin );
      unlockDetectorEvents( ChirpEvents );
    }
  }

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

  // trigger:
  // XXX  setupTrigger( traces(), events() );

  for ( int count=0;
	( Repeats <= 0 || count < Repeats ) && softStop() == 0;
	count++ ) {

    // message:
    if ( Repeats == 0 && count%100 == 0 )
      message( "Search ..." );
    else if ( Repeats > 0 )
      message( "Search Loop <b>" + Str( count ) + "</b>: <b>"
	       + Str( SearchDuration, "%.1f" ) + "<b> s" );

    // store current recording time:
    if ( Repeats <= 0 || count == 0 ) {
      FirstSignal = currentTime();
      LastSignal = currentTime();
      SearchDuration = 0.0;
    }

    // sleep:
    sleep( Duration );
    if ( interrupt() )
      break;

    // adjust gain of daq board:
    if ( adjustg ) {
      for ( int k=0; k<MaxTraces; k++ ) {
	if ( SpikeTrace[k] >= 0 )
	  adjust( trace( SpikeTrace[k] ), Duration, 0.8 );
      }
      if ( NerveTrace[0] >= 0 )
	adjust( trace( NerveTrace[0] ), Duration, 0.8 );
      if ( EODTrace >= 0 ) {
	double val1 = trace( EODTrace ).maxAbs( currentTime()-0.1,
						currentTime() );
	if ( val1 > 0.0 )
	  adjustGain( trace( EODTrace ), val1 );
      }
      double val2 = trace( LocalEODTrace[0] ).maxAbs( currentTime()-0.1,
						      currentTime() );
      if ( val2 > 0.0 )
	adjustGain( trace( LocalEODTrace[0] ), 1.25 * val2 );

      activateGains();
    }


    LastSignal = FirstSignal + SearchDuration;
    SearchDuration = currentTime() - FirstSignal;

    // analyze:
    analyze( autodetect, eodcycle, eodtimes, eodspikes,
	     spikes, isih, spikerate, trials,
	     nerveamplp, nerveamplt, nerveamplm, nerveampls );
    plot( eodcycle, eodspikes, isih, spikerate, nerveamplm );

  }

  setMessage();
  save( saveeodtrace, eodduration, saveeodtimes, eodtimes, eodcycle,
	spikes, isih, spikerate, nerveamplp, nerveamplt, nerveamplm, nerveampls );

  return Completed;
}


void BaselineActivity::saveSpikes( int trace, const Options &header, const EventList &spikes )
{
  // create file:
  ofstream df( addPath( "basespikes" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%8.2f" );
  key.saveKey( df, true, false );

  // write data into file:
  if ( spikes[trace].size() > 0 ) {
    for ( int j=0; j<spikes[trace].size(); j++ ) {
      key.save( df, 1000.0 * spikes[trace][j], 0 );
      df << '\n';
    }
  }
  else
    df << "  -0\n";
  df << "\n\n";
}


void BaselineActivity::saveISIH( int trace, const Options &header,
				 const vector<SampleDataD> &isih )
{
  // create file:
  ofstream df( addPath( "baseisih" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%5.2f" );
  key.addNumber( "eod", "1", "%6.3f" );
  key.addNumber( "n", "1", "%6.0f" );
  key.addNumber( "p", "1", "%5.3f" );
  key.saveKey( df, true, false );

  // normalization factor:
  double norm = 0.0;
  for ( int j=0; j<isih[trace].size(); j++ )
    norm += isih[trace][j];

  // write data into file:
  for ( int j=0; j<isih[trace].size(); j++ ) {
    key.save( df, 1000.0 * isih[trace].pos( j ), 0 );
    key.save( df, isih[trace].pos( j ) * EODRate, 1 );
    key.save( df, isih[trace][j] );
    key.save( df, isih[trace][j]/norm );
    df << '\n';
  }

  df << "\n\n";
}


void BaselineActivity::saveRate( int trace, const Options &header,
				 const vector<SampleDataD> &spikerate,
				 const SampleDataD &eodcycle )
{
  // create file:
  ofstream df( addPath( "baserate" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%5.3f" );
  key.addNumber( "rate", "Hz", "%5.1f" );
  key.addNumber( "ampl", EOD2Unit, "%6.3f" );
  key.saveKey( df, true, false );

  // write data into file:
  for ( int j=0; j<spikerate[trace].size(); j++ ) {
    key.save( df, 1000.0 * spikerate[trace].pos( j ), 0 );
    key.save( df, spikerate[trace][j] );
    key.save( df, eodcycle[j] );
    df << '\n';
  }

  df << "\n\n";
}


void BaselineActivity::saveNerve( const Options &header, const MapD &nerveamplp,
				  const MapD &nerveamplt, const MapD &nerveamplm,
				  const SampleDataD &nerveampls )
{
  {
    // create file:
    ofstream df( addPath( "basenerveampl.dat" ).c_str(),
		 ofstream::out | ofstream::app );
    if ( ! df.good() )
      return;

    // write header and key:
    header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    TableKey key;
    key.newSection( "peak" );
    key.addNumber( "time", "ms", "%9.2f" );
    key.addNumber( "ampl", "uV", "%6.1f" );
    key.newSection( "trough" );
    key.addNumber( "time", "ms", "%9.2f" );
    key.addNumber( "ampl", "uV", "%6.1f" );
    key.newSection( "average" );
    key.addNumber( "time", "ms", "%9.2f" );
    key.addNumber( "ampl", "uV", "%6.1f" );
    key.saveKey( df, true, true );

    // write data into file:
    if ( nerveamplm.empty() ) {
      key.save( df, 0.0, 0 );
      key.save( df, 0.0 );
      key.save( df, 0.0 );
      key.save( df, 0.0 );
      key.save( df, 0.0 );
      key.save( df, 0.0 );
      df << '\n';
    }
    else {
      for ( int k=0; k<nerveamplp.size() &&
	      k<nerveamplt.size() &&
	      k<nerveamplm.size(); k++ ) {
	key.save( df, 1000.0 * nerveamplp.x(k), 0 );
	key.save( df, nerveamplp.y(k) );
	key.save( df, 1000.0 * nerveamplt.x(k), 0 );
	key.save( df, nerveamplt.y(k) );
	key.save( df, 1000.0 * nerveamplm.x(k), 0 );
	key.save( df, nerveamplm.y(k) );
	df << '\n';
      }
    }
    df << "\n\n";
  }

  {
    // create file:
    ofstream df( addPath( "basenervesmoothampl.dat" ).c_str(),
		 ofstream::out | ofstream::app );
    if ( ! df.good() )
      return;

    // write header and key:
    header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    TableKey key;
    key.addNumber( "time", "ms", "%9.2f" );
    key.addNumber( "ampl", "uV", "%7.3f" );
    key.saveKey( df, true, true );

    // write data into file:
    if ( nerveampls.empty() ) {
      key.save( df, 0.0, 0 );
      key.save( df, 0.0 );
      df << '\n';
    }
    else {
      for ( int k=0; k<nerveampls.size(); k++ ) {
	key.save( df, 1000.0 * nerveampls.pos(k), 0 );
	key.save( df, nerveampls[k] );
	df << '\n';
      }
    }
    df << "\n\n";
  }
}


void BaselineActivity::saveEODTrace( const Options &header, double eodduration )
{
  // create file:
  ofstream df( addPath( "baseeodtrace.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.2f" );
  key.addNumber( "EOD", EOD2Unit, "%6.3f" );
  key.saveKey( df, true, false );

  // write data into file:
  const InData &leod = trace( LocalEODTrace[0] );
  int inx = leod.indices( eodduration );
  if ( leod.size()-inx < leod.minIndex() )
    inx = leod.size() - leod.minIndex();
  for ( int k=0; k<inx; k++ ) {
    key.save( df, 1000.0 * leod.interval( k ), 0 );
    key.save( df, leod[leod.size()-inx+k] );
    df << '\n';
  }

  df << "\n\n";
}


void BaselineActivity::saveEODTimes( const Options &header, const EventData &eodtimes )
{
  // create file:
  ofstream df( addPath( "baseeodtimes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%8.2f" );
  key.saveKey( df, true, false );

  // write data into file:
  for ( int k=0; k<eodtimes.size(); k++ ) {
    key.save( df, 1000.0 * eodtimes[k], 0 );
    df << '\n';
  }

  df << "\n\n";
}


void BaselineActivity::save( bool saveeodtrace, double eodduration,
			     bool saveeodtimes,
			     const EventData &eodtimes,
			     const SampleDataD &eodcycle,
			     const EventList &spikes,
			     const vector<SampleDataD> &isih,
			     const vector<SampleDataD> &spikerate,
			     const MapD &nerveamplp,
			     const MapD &nerveamplt,
			     const MapD &nerveamplm,
			     const SampleDataD &nerveampls )
{
  if ( Repeats <= 0 )
    return;

  Options header;
  header.addInteger( "index", totalRuns() );
  header.addInteger( "trace", -1 );
  header.addNumber( "EOD rate", EODRate, "Hz", "%.1f" );
  header.addNumber( "EOD period", 1000.0 * EODPeriod, "ms", "%.3f" );
  header.addNumber( "duration", SearchDuration, "sec", "%.3f" );
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Str ns( k+1 );
      header.addNumber( "firing frequency"+ns, FRate[k], "Hz", "%.1f" );
      header.addNumber( "CV"+ns, CV[k], "", "%.3f" );
      header.addNumber( "p-value"+ns, PValue[k], "", "%.3f" );
    }
  }
  header.addText( "session time", sessionTimeStr() );
  header.newSection( settings() );

  for ( int trace=0; trace<MaxTraces; trace++ ) {
    if ( SpikeEvents[trace] >= 0 ) {
      header.setInteger( "trace", trace );
      saveSpikes( trace, header, spikes );
      saveISIH( trace, header, isih );
      saveRate( trace, header, spikerate, eodcycle );
    }
  }
  if ( NerveTrace[0] >= 0 ) {
    header.setInteger( "trace", 0 );
    saveNerve( header, nerveamplp, nerveamplt, nerveamplm, nerveampls );
  }

  if ( saveeodtrace )
    saveEODTrace( header, eodduration );
  if ( saveeodtimes )
    saveEODTimes( header, eodtimes );
}


void BaselineActivity::plot( const SampleDataD &eodcycle,
			     const vector< vector< ArrayD > > &eodspikes,
			     const vector<SampleDataD> &isih,
			     const vector<SampleDataD> &spikerate,
			     const MapD &nerveamplm )
{
  P.lock();

  int n=0;
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      P[2*n].clear();
      if ( ! P[2*n].zoomedXRange() )
	P[2*n].setXRange( 0.0, 500.0 * isih[k].rangeBack() );
      for ( double x=EODPeriod; x < 0.5*isih[k].rangeBack(); x += EODPeriod )
	P[2*n].plotVLine( 1000.0 * x );
      P[2*n].plot( isih[k], 1000.0, Plot::Transparent, 0, Plot::Solid, Plot::Box, 0, Plot::Red, Plot::Red );

      P[2*n+1].clear();
      double tmax = spikerate[k].rangeBack();
      if ( tmax > EODPeriod )
	tmax = EODPeriod;
      if ( ! P[2*n+1].zoomedXRange() )
	P[2*n+1].setXRange( 0.0, 1000.0 * tmax );

      double ss = 1.0/eodspikes[k].size();
      if ( ss < 0.01 )
	ss = 0.01;
      int ns = (int)ceil( 1.0/ss );
      int js = 0;
      if ( (int)eodspikes[k].size() > ns )
	js = eodspikes[k].size() - ns;
      int i = 0;
      for ( unsigned int j = js; j < eodspikes[k].size(); j++ ) {
	i++;
	P[2*n+1].plot( eodspikes[k][j], 1000.0, 1.0 - i*ss, Plot::Graph, 2, Plot::StrokeUp,
		       ss, Plot::Graph, Plot::Red, Plot::Red );
      }

      P[2*n+1].plot( spikerate[k], 1000.0, Plot::Yellow, 2, Plot::Solid );

      P[2*n+1].plot( eodcycle, 1000.0, Plot::Green, 2, Plot::Solid );
      P[2*n+1].back().setAxis( Plot::X1Y2 );

      n++;
    }
  }

  if ( NerveTrace[0] >= 0 ) {
    P[2*n].clear();
    double l = nerveamplm.x().back();
    if ( ! P[2*n].zoomedXRange() )
      P[2*n].setXRange( 1000.0*(l-Duration), 1000.0*l );
    P[2*n].plot( nerveamplm, 1000.0, Plot::Cyan, 1, Plot::Solid );
  }

  P.draw();

  P.unlock();
}


void BaselineActivity::analyzeSpikes( const EventData &se,
				      EventData &eodtimes,
				      int k,
				      vector< vector< ArrayD > > &eodspikes,
				      EventList &spikes,
				      vector<SampleDataD> &isih,
				      vector<SampleDataD> &spikerate,
				      vector<int> &trials )
{
  // interspike interval histogram:
  if ( Repeats <= 0 )
    isih[k] = 0.0;
  se.addIntervalHistogram( LastSignal, FirstSignal+SearchDuration, isih[k] );
  double sd = 0.0;
  double isi = se.interval( FirstSignal, FirstSignal+SearchDuration, &sd );
  CV[k] = isi > 1.0e-6 ? sd / isi : 0.0;

  // EOD spike rate:
  if ( Repeats <= 0 ) {
    trials[k] = 0;
    spikerate[k] = 0.0;
  }
  for ( int j=LastEODInx; j<eodtimes.size(); j++ ) {
    se.addRate( spikerate[k], trials[k], 0.0, eodtimes[j] + FirstSignal );
  }

  // spikes:
  if ( Repeats <= 0 )
    spikes[k].clear();
  spikes[k].append( se, LastSignal, FirstSignal+SearchDuration, FirstSignal );

  // eod spikes:
  if ( Repeats <= 0 )
    eodspikes[k].clear();
  for ( int j=LastEODInx; j<eodtimes.size(); j++ ) {
    double lat = se.latency( eodtimes[j] + FirstSignal );
    if ( lat >= 0.0 && lat < EODPeriod )
      eodspikes[k].push_back( ArrayD( 1, lat ) );
  }

  // Firing Rate:
  Str ns(k+1);
  FRate[k] = se.rate( FirstSignal, FirstSignal+SearchDuration );
  controlOpts( "Session" ).setNumber( "firingrate"+ns, FRate[k] );
  PValue[k] = EODRate > 0.0 ? FRate[k]/EODRate : 0.0;
  controlOpts( "Session" ).setNumber( "pvalue"+ns, PValue[k] );

  if ( Repeats > 0 ) {
    lockMetaData();
    metaData().setNumber( "Cell>Firing Rate"+ns, FRate[k] );
    metaData().setNumber( "Cell>P-Value"+ns, PValue[k] );
    unlockMetaData();
  }
}


void BaselineActivity::analyze( int autodetect,
				SampleDataD &eodcycle,
				EventData &eodtimes,
				vector< vector< ArrayD > > &eodspikes,
				EventList &spikes,
				vector<SampleDataD> &isih,
				vector<SampleDataD> &spikerate,
				vector<int> &trials,
				MapD &nerveamplp, MapD &nerveamplt,
				MapD &nerveamplm, SampleDataD &nerveampls )
{
  const EventData &localeod = events( LocalEODEvents[0] );
  const InData &localeodtrace = trace( LocalEODTrace[0] );

  // EOD period & rate:
  EODRate = localeod.frequency( FirstSignal, FirstSignal+SearchDuration );
  EODPeriod = 1.0/EODRate;
  if ( Repeats > 0 ) {
    lockMetaData();
    metaData().setNumber( "Subject>EOD Frequency", EODRate );
    unlockMetaData();
  }
  double eodampl = eodAmplitude( localeodtrace, LastSignal, FirstSignal+SearchDuration );

  // EOD times
  if ( Repeats <= 0 ) {
    eodtimes.clear();
    LastEODInx = 0;
  }
  eodtimes.append( localeod, LastSignal, FirstSignal+SearchDuration, FirstSignal );

  // EOD cycle:
  EOD2Unit = localeodtrace.unit();
  if ( !eodtimes.empty() )
    localeodtrace.copy( eodtimes.previousTime( SearchDuration - 2.0*eodcycle.rangeBack(),
					       eodtimes[0] ) + FirstSignal, eodcycle );
  else
    eodcycle = 0.0;

  for ( int k=0; k<MaxTraces; k++ )
    if ( SpikeEvents[k] >= 0 )
      analyzeSpikes( events( SpikeEvents[k] ), eodtimes, k, eodspikes,
		     spikes, isih, spikerate, trials );
  if ( NerveTrace[0] >= 0 ) {
    const InData &nd = trace( NerveTrace[0] );
    // nerve amplitudes:
    // peak and trough amplitudes:
    static double threshold = 0.001;
    InData::const_iterator firstn = nd.begin( FirstSignal );
    InData::const_iterator lastn = nd.end();
    InDataTimeIterator firstntime = nd.timeBegin( FirstSignal );
    EventList peaktroughs( 2, (int)rint(5000.0*Duration), true );
    nerveamplp.clear();
    nerveamplt.clear();
    nerveamplm.clear();
    nerveampls.clear();
    nerveamplp.reserve( (int)rint(5000.0*SearchDuration) );
    nerveamplt.reserve( (int)rint(5000.0*SearchDuration) );
    nerveamplm.reserve( (int)rint(5000.0*SearchDuration) );
    threshold = nd.stdev( FirstSignal, FirstSignal+4.0/EODRate );
    if ( threshold < 1.0e-8 )
      threshold = 0.001;
    D.init( firstn, lastn, firstntime );
    D.peakTrough( firstn, lastn, peaktroughs,
		  threshold, threshold, threshold, NerveAcceptEOD );
    // store amplitudes:
    for ( int k=0; k<peaktroughs[0].size(); k++ )
      nerveamplp.push( peaktroughs[0][k] - FirstSignal,
		       peaktroughs[0].eventSize( k ) );
    for ( int k=0; k<peaktroughs[1].size(); k++ )
      nerveamplt.push( peaktroughs[1][k] - FirstSignal,
		       peaktroughs[1].eventSize( k ) );
    // averaged amplitude:
    if ( peaktroughs[0].size() > 1 ) {
      double left = FirstSignal;
      double st = (peaktroughs[0].back() - peaktroughs[0].front())/double(peaktroughs[0].size()-1);
      for ( int k=0; k<nerveamplp.size(); k++ ) {
	nerveamplm.push( left-FirstSignal, nd.mean( left, left+st ) );
	left += st;
      }
      // smoothed averaged amplitude:
      nerveampls = SampleDataD( 0.0, SearchDuration, 0.0001 );
      for ( int k=0; k<nerveampls.size(); k++ ) {
	double t = FirstSignal + nerveampls.pos( k );
	nerveampls[k] = nd.mean( t, t+st );
      }
    }
  }

  if ( autodetect > 1 && Repeats <= 0 ) {
    // setup Beat detector:
    lockDetectorEvents( LocalBeatPeakEvents[0] );
    if ( events( LocalBeatPeakEvents[0] ).count( currentTime() - Duration ) > 0 ) {
      /*
      double beatthresh = detectorEventsOpts( LocalBeatPeakEvents[0] ).number( "minthresh" );
      beatthresh += BeatStep * localeodtrace.maxValue();
      detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "minthresh", beatthresh );
      */
      double beatthresh = detectorEventsOpts( LocalBeatPeakEvents[0] ).number( "threshold" );
      beatthresh += BeatStep * localeodtrace.maxValue();
      detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "minthresh", beatthresh );
      detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "threshold", beatthresh );
    }
    else {
      double beatthresh = detectorEventsOpts( LocalBeatPeakEvents[0] ).number( "minthresh" );
      beatthresh -= BeatStep * localeodtrace.maxValue();
      if ( beatthresh >= 10.0*BeatStep * eodampl &&
	   beatthresh >= detectorEventsOpts( LocalBeatPeakEvents[0] ).minimum( "minthresh" ) )
	detectorEventsOpts( LocalBeatPeakEvents[0] ).setNumber( "minthresh", beatthresh );
    }
    unlockDetectorEvents( LocalBeatPeakEvents[0] );
    // setup Chirp detector:
    if ( ChirpEvents >= 0 ) {
      lockDetectorEvents( ChirpEvents );
      if ( events( ChirpEvents ).count(  currentTime() - Duration ) > 0 ) {
	double chirpmax = detectorEventsOpts( ChirpEvents ).number( "maxthresh" );
	double chirpthresh = detectorEventsOpts( ChirpEvents ).number( "threshold" );
	chirpthresh += ChirpStep;
	if ( chirpthresh < chirpmax )
	  detectorEventsOpts( ChirpEvents ).setNumber( "threshold", chirpthresh );
      }
      else {
	double chirpthresh = detectorEventsOpts( ChirpEvents ).number( "threshold" );
	chirpthresh -= ChirpStep;
	if ( chirpthresh >= ChirpMin )
	  detectorEventsOpts( ChirpEvents ).setNumber( "threshold", chirpthresh );
      }
      unlockDetectorEvents( ChirpEvents );
    }
  }

  LastEODInx = eodtimes.size();
}


addRePro( BaselineActivity, efish );

}; /* namespace efish */

#include "moc_baselineactivity.cc"
