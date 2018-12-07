/*
  patchclamp/ficurve.cc
  f-I curve measured in current-clamp

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
#include <relacs/tablekey.h>
#include <relacs/patchclamp/ficurve.h>
using namespace relacs;

namespace patchclamp {


FICurve::FICurve( void )
  : RePro( "FICurve", "patchclamp", "Jan Benda", "2.0", "Nov 29, 2018" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    IInFac( 1.0 )
{
  IStep = 0.001;

  // add some options:
  newSection( "Stimuli" );
  addSelection( "ibase", "Currents are relative to", "zero|DC|threshold" );
  addNumber( "imin", "Minimum injected current", 0.0, -1000.0, 1000.0, 0.001 );
  addNumber( "imax", "Maximum injected current", 1.0, -1000.0, 1000.0, 0.001 );
  addNumber( "istep", "Minimum step-size of current", IStep, 0.001, 1000.0, 0.001 ).setActivation( "userm", "false" );
  addNumber( "optimizedimin", "Minimum current below firing threshold", 1000.0, 0.0, 1000.0, 0.001 );
  addBoolean( "manualskip", "Show buttons for manual selection of intensities", false );
  newSection( "Timing" );
  addNumber( "duration", "Duration of current output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "delay", "Delay before current pulses", 0.1, 0.001, 10.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause between current pulses", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addSelection( "ishuffle", "Initial sequence of currents for first repetition", RangeLoop::sequenceStrings() );
  addSelection( "shuffle", "Sequence of currents", RangeLoop::sequenceStrings() );
  addInteger( "iincrement", "Initial increment for currents", -1, -1000, 1000, 1 );
  addInteger( "singlerepeat", "Number of immediate repetitions of a single stimulus", 1, 1, 10000, 1 );
  addInteger( "blockrepeat", "Number of repetitions of a fixed intensity increment", 10, 1, 10000, 1 );
  addInteger( "repeats", "Number of repetitions of the whole f-I curve measurement", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  newSection( "Analysis" );
  addBoolean( "optimize", "Dynamically optimize range of injected currents", true);
  addNumber( "fmax", "Maximum firing rate", 200.0, 0.0, 2000.0, 1.0, "Hz" ).setActivation( "optimize", "true" );
  addNumber( "vmax", "Maximum steady-state potential", 0.0, -2000.0, 2000.0, 1.0, "mV" ).setActivation( "optimize", "true" );
  addInteger( "numpoints", "Number of points to measure below maximum firing rate", 0 ).setActivation( "optimize", "true" );
  addNumber( "sswidth", "Window length for steady-state analysis", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addBoolean( "ignorenoresponse", "Do not include trials without response in analysis", true );
  addInteger( "diffincrement", "Optimize range at current increments below", 0, 0, 10000 ).setActivation( "optimize", "true" );
  addNumber( "maxratediff", "Maximum difference between onset and steady-state firing rate for optimization", 100.0, 0.0, 1000.0, 1.0, "Hz" ).setActivation( "diffincrement", ">0" ).addActivation( "optimize", "true" );

  PlotRangeSelection = false;

  P.lock();
  P.resize( 2, 2, true );
  P.unlock();
  setWidget( &P );
}


void FICurve::preConfig( void )
{
  if ( SpikeTrace[0] >= 0 ) {
    VUnit = trace( SpikeTrace[0] ).unit();
  }

  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "imin", IUnit );
    setUnit( "imax", IUnit );
    setUnit( "istep", IUnit );
    setUnit( "optimizedimin", IUnit );
  }

  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int FICurve::main( void )
{
  Header.clear();
  Header.addInteger( "index", completeRuns() );
  Header.addInteger( "ReProIndex", reproCount() );
  Header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );

  // get options:
  int ibase = index( "ibase" );
  double imin = number( "imin" );
  double imax = number( "imax" );
  IStep = number( "istep" );
  double optimizedimin = number( "optimizedimin" );
  bool manualskip = boolean( "manualskip" );
  RangeLoop::Sequence shuffle = RangeLoop::Sequence( index( "shuffle" ) );
  RangeLoop::Sequence ishuffle = RangeLoop::Sequence( index( "ishuffle" ) );
  int iincrement = integer( "iincrement" );
  int singlerepeat = integer( "singlerepeat" );
  int blockrepeat = integer( "blockrepeat" );
  int repeat = integer( "repeats" );
  double duration = number( "duration" );
  double delay = number( "delay" );
  double pause = number( "pause" );
  bool optimize = boolean( "optimize" );
  double fmax = number( "fmax" );
  double vmax = number( "vmax" );
  int numpoints = integer( "numpoints" );
  double sswidth = number( "sswidth" );
  bool ignorenoresponse = boolean( "ignorenoresponse" );
  int diffincrement = number( "diffincrement" );
  double maxratediff = number( "maxratediff" );

  lockStimulusData();
  double dccurrent = stimulusData().number( outTraceName( CurrentOutput[0] ) );
  unlockStimulusData();
  if ( ibase == 1 ) {
    imin += dccurrent;
    imax += dccurrent;
  }
  else if ( ibase == 2 ) {
    lockMetaData();
    double ithresh = metaData().number( "Cell>ithreshon" );
    if ( ithresh == 0.0 )
      ithresh = metaData().number( "Cell>ithreshss" );
    unlockMetaData();
    imin += ithresh;
    imax += ithresh;
  }
  if ( imax <= imin ) {
    warning( "imin must be smaller than imax!" );
    return Failed;
  }
  if ( pause < duration ) {
    warning( "Pause must be at least as long as the stimulus duration!" );
    return Failed;
  }
  if ( pause < delay ) {
    warning( "Pause must be at least as long as the delay!" );
    return Failed;
  }
  if ( sswidth >= duration ) {
    warning( "sswidth must be smaller than stimulus duration!" );
    return Failed;
  }
  if ( SpikeTrace[0] < 0 || SpikeEvents[0 ] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }
  if ( CurrentOutput[0] < 0 ) {
    warning( "Invalid output current trace!" );
    return Failed;
  }
  Header.addNumber( "imin", imin, IUnit );
  Header.addNumber( "istep", IStep, IUnit );

  // don't print repro message:
  noMessage();

  // init:
  DoneState state = Completed;
  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  Range.set( imin, imax, IStep, repeat, blockrepeat, singlerepeat );
  Range.setIncrement( iincrement );
  Range.setSequence( ishuffle );
  int prevrepeat = 0;
  Results.clear();
  Results.resize( Range.size(), Data() );

  // plot:
  P.lock();
  P[0].clear();
  P[0].setXLabel( "Time [ms]" );
  P[0].setXRange( -1000.0*delay, 1000.0*(duration+delay) );
  P[0].setYLabel( "Firing rate [Hz]" );
  P[0].setYFallBackRange( 0.0, 20.0 );
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[1].clear();
  P[1].setXLabel( "Current [" + IUnit + "]" );
  P[1].setXRange( imin, imax );
  P[1].setYLabel( "Firing rate [Hz]" );
  P[1].setYFallBackRange( 0.0, 20.0 );
  P[1].setYRange( 0.0, Plot::AutoScale );
  if ( manualskip ) {
    P[1].setUserMouseTracking( true );
    connect( &P[1], SIGNAL( userMouseEvent( Plot::MouseEvent& ) ),
	     this, SLOT( plotMouseEvent( Plot::MouseEvent& ) ) );
  }
  P.unlock();
  PlotRangeSelection = false;

  // signal:
  OutData signal( duration, 1.0/samplerate );
  signal.setTrace( CurrentOutput[0] );
  signal.setDelay( delay );

  // dc signal:
  OutData dcsignal;
  dcsignal.setTrace( CurrentOutput[0] );
  dcsignal.constWave( dccurrent );
  dcsignal.setIdent( "DC=" + Str( dccurrent ) + IUnit );

  // initial pause:
  sleepWait( pause );
  if ( interrupt() ) {
    if ( manualskip ) {
      P.lock();
      P[1].setUserMouseTracking( false );
      disconnect( &P[1], SIGNAL( userMouseEvent( Plot::MouseEvent& ) ),
		  this, SLOT( plotMouseEvent( Plot::MouseEvent& ) ) );
      P.unlock();
    }
    return Aborted;
  }

  // plot trace:
  tracePlotSignal( 2.0*duration+delay, delay );

  // write stimulus:
  bool gotmin = false;
  bool gotmax = false;
  for ( Range.reset(); ! Range && softStop() == 0; ) {

    timeStamp();

    // reset sequence after first repetition:
    if ( prevrepeat < Range.currentRepetition() ) {
      if ( Range.currentRepetition() == 1 ) {
	Range.setSequence( shuffle );
	Range.update();
	if ( ! !Range ) {
	  directWrite( dcsignal );
	  break;
	}
      }
      prevrepeat = Range.currentRepetition();
    }

    // new stimulus:
    double amplitude = *Range;
    if ( fabs( amplitude ) < 1.0e-8 )
      amplitude = 0.0;

    Str s = "Increment <b>" + Str( Range.currentIncrementValue() ) + " " + IUnit + "</b>";
    s += ",  Current <b>" + Str( amplitude ) + " " + IUnit +"</b>";
    s += ",  Count <b>" + Str( Range.count()+1 ) + "</b>";
    message( s );

    signal.pulseWave( duration, 1.0/samplerate, amplitude, dccurrent );
    signal.setIdent( "I=" + Str( amplitude ) + IUnit );
    write( signal );
    if ( signal.failed() ) {
      if ( signal.overflow() ) {
	printlog( "Requested amplitude I=" + Str( amplitude ) + IUnit + "too high!" );
	for ( int k = Range.size()-1; k >= 0; k-- ) {
	  if ( Range[k] > signal.maxValue() || k == Range.pos() )
	    Range.setSkip( k );
	  else {
	    directWrite( dcsignal );
	    break;
	  }
	}
	Range.noCount();
	continue;
      }
      else if ( signal.underflow() ) {
	printlog( "Requested amplitude I=" + Str( amplitude ) + IUnit + "too small!" );
	for ( int k = 0; k < Range.size(); k++ ) {
	  if ( Range[k] < signal.minValue() || k == Range.pos() )
	    Range.setSkip( k );
	  else {
	    directWrite( dcsignal );
	    break;
	  }
	}
	Range.noCount();
	continue;
      }
      else {
	warning( signal.errorText() );
	directWrite( dcsignal );
	if ( manualskip ) {
	  P.lock();
	  P[1].setUserMouseTracking( false );
	  disconnect( &P[1], SIGNAL( userMouseEvent( Plot::MouseEvent& ) ),
		      this, SLOT( plotMouseEvent( Plot::MouseEvent& ) ) );
	  P.unlock();
	}
	return Failed;
      }
    }

    // sleep:
    sleep( duration + 0.01 );
    if ( interrupt() ) {
      if ( Range.totalCount() < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }

    // analyse:
    Results[Range.pos()].I = amplitude;
    Results[Range.pos()].DC = dccurrent;
    Results[Range.pos()].analyze( Range.count(), trace( 0 ),
				  events( SpikeEvents[0] ), 
				  CurrentTrace[0] >= 0 ? &trace( CurrentTrace[0] ) : 0,
				  IInFac, delay, duration, sswidth, ignorenoresponse );

    if ( optimize ) {
      // skip currents that evoke firing rates greater than fmax:
      if ( Results[Range.pos()].SSRate > fmax ) {
	Range.setSkipAbove( Range.pos() );
	gotmax = true;
	if ( gotmin && numpoints > 0 )
	  Range.setSkipNumber( numpoints );
	Range.noCount();
      }
      // skip currents causing depolarization block:
      if ( Results[Range.pos()].SSRate < 1.0/duration &&
	   Results[Range.pos()].VSS > vmax ) {
	Range.setSkipAbove( Range.pos() );
	gotmax = true;
	if ( gotmin && numpoints > 0 )
	  Range.setSkipNumber( numpoints );
	Range.noCount();
      }
      // skip currents too low to make the neuron fire:
      else if ( Results[Range.pos()].SpikeCount <= 0.01 ) {
	double maxcurrent = Range.value() - optimizedimin;
	Range.setSkipBelow( Range.pos( maxcurrent ) - 1 );
	gotmin = true;
	if ( gotmax && numpoints > 0 )
	  Range.setSkipNumber( numpoints );
      }
      // skip currents above large enough fon - fss differences:
      if ( Range.finishedBlock() &&
	   Range.currentIncrement() == diffincrement ) {
	int n = 0;
	for ( unsigned int k=Range.next( 0 );
	      k<Results.size();
	      k=Range.next( ++k ) ) {
	  if ( Results[k].OnRate - Results[k].SSRate > maxratediff ) {
	    n++;
	    if ( n > 1 ) {
	      printlog( "Skip currents above " + Str( Range[k] ) );
	      Range.setSkipAbove( Range.next( ++k ) );
	      break;
	    }
	  }
	  else
	    n = 0;
	}
      }
    }

    int cinx = Range.pos();
    ++Range;

    plot( duration, cinx );

    sleepOn( duration + pause );
    if ( interrupt() ) {
      if ( Range.totalCount() < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }
  }

  // save data:
  if ( state == Completed && Range.totalCount() > 0 )
    save();

  if ( manualskip ) {
    P.lock();
    P[1].setUserMouseTracking( false );
    disconnect( &P[1], SIGNAL( userMouseEvent( Plot::MouseEvent& ) ),
		this, SLOT( plotMouseEvent( Plot::MouseEvent& ) ) );
    P.unlock();
  }
  return state;
}


void FICurve::plotRangeSelection( void )
{
  ArrayD sa; sa.reserve( Range.size() );
  ArrayD ua; ua.reserve( Range.size() );
  for ( int k=0; k<Range.size(); k++ ) {
    double x = Range.value( k );
    if ( k == 0 )
      x += 0.01 * IStep;
    if ( k == Range.size()-1 )
      x -= 0.01 * IStep;
    if ( Range.skip( k ) )
      sa.push( x );
    else
      ua.push( x);
  }
  P[1].plot( ua, 1.0, 0.95, Plot::Graph, 0, Plot::Diamond,
	     0.7*IStep, Plot::FirstX, Plot::Green, Plot::Green );
  P[1].plot( sa, 1.0, 0.95, Plot::Graph, 0, Plot::Diamond,
	     0.7*IStep, Plot::FirstX, Plot::Red, Plot::Red );
}


void FICurve::plot( double duration, int inx )
{
  P.lock();

  // rate and spikes:
  const Data &data = Results[inx];
  P[0].clear();
  P[0].setTitle( "I=" + Str( data.I, 0, 2, 'f' ) + IUnit );
  P[0].plotVLine( 0, Plot::White, 2 );
  P[0].plotVLine( 1000.0*duration, Plot::White, 2 );
  int maxspikes = (int)rint( 20.0 / SpikeTraces );
  if ( maxspikes < 4 )
    maxspikes = 4;
  int j = 0;
  int r = Range.maxCount();
  double delta = r > 0 && r < maxspikes ? 1.0/r : 1.0/maxspikes;
  int offs = (int)data.Spikes.size() > maxspikes ? data.Spikes.size() - maxspikes : 0;
  for ( int i=offs; i<data.Spikes.size(); i++ ) {
    j++;
    P[0].plot( data.Spikes[i], 0, 0.0, 1000.0,
	       1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp,
	       delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
  }
  P[0].plot( data.Rate, 1000.0, Plot::Yellow, 2, Plot::Solid );

  // f-I curves:
  P[1].clear();
  MapD om, mm, sm, rm;
  double imin = Results[Range.next( 0 )].I;
  double imax = imin;
  for ( unsigned int k=Range.next( 0 );
	k<Results.size();
	k=Range.next( ++k ) ) {
    imax = Results[k].I;
    rm.push( Results[k].I, Results[k].PreRate );
    om.push( Results[k].I, Results[k].OnRate );
    sm.push( Results[k].I, Results[k].SSRate );
    mm.push( Results[k].I, Results[k].MeanRate );
  }
  if ( ! P[1].zoomedXRange() && imax > imin+1.0e-8 )
    P[1].setXRange( imin, imax );
  P[1].plot( rm, 1.0, Plot::Cyan, 3, Plot::Solid, Plot::Circle, 6, Plot::Cyan, Plot::Cyan );
  P[1].plot( om, 1.0, Plot::Green, 3, Plot::Solid, Plot::Circle, 6, Plot::Green, Plot::Green );
  P[1].plot( sm, 1.0, Plot::Red, 3, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P[1].plot( mm, 1.0, Plot::Orange, 3, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );
  MapD am;
  am.push( Results[inx].I, Results[inx].PreRate );
  am.push( Results[inx].I, Results[inx].OnRate );
  am.push( Results[inx].I, Results[inx].SSRate );
  am.push( Results[inx].I, Results[inx].MeanRate );
  P[1].plot( am, 1.0, Plot::Transparent, 3, Plot::Solid, Plot::Circle, 8, Plot::Yellow, Plot::Transparent );

  if ( PlotRangeSelection )
    plotRangeSelection();

  P.draw();

  P.unlock();
}


void FICurve::save( void )
{
  message( "<b>Saving ...</b>" );
  tracePlotContinuous();
  lockStimulusData();
  Header.newSection( stimulusData() );
  unlockStimulusData();
  Header.newSection( settings() );
  saveData();
  saveRate();
  saveSpikes();
  saveTraces();
  message( "<b>FICurve finished.</b>" );
}


void FICurve::saveData( void )
{
  ofstream df( addPath( "ficurve-data.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Header.save( df, "# ", 0, FirstOnly );
  df << '\n';

  TableKey datakey;
  datakey.newSection( "Stimulus" );
  datakey.addNumber( "I", IUnit, "%6.3f" );
  datakey.addNumber( "IDC", IUnit, "%6.3f" );
  datakey.addNumber( "trials", "1", "%6.0f" );
  datakey.newSection( "Firing rate" );
  datakey.addNumber( "f", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.newSection( "Baseline" );
  datakey.addNumber( "f_b", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.addNumber( "v_rest", VUnit, "%6.1f" );
  datakey.addNumber( "s.d.", VUnit, "%6.1f" );
  datakey.newSection( "Peak rate" );
  datakey.addNumber( "f_on", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.addNumber( "t_on", "ms", "%5.1f" );
  datakey.newSection( "Steady-state" );
  datakey.addNumber( "f_ss", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.addNumber( "v_rest", VUnit, "%6.1f" );
  datakey.addNumber( "s.d.", VUnit, "%6.1f" );
  datakey.newSection( "Spike count" );
  datakey.addNumber( "count", "1", "%7.1f" );
  datakey.addNumber( "s.d.", "1", "%7.1f" );
  datakey.newSection( "Latency" );
  datakey.addNumber( "latency", "ms", "%6.1f" );
  datakey.addNumber( "s.d.", "ms", "%6.1f" );
  datakey.saveKey( df );

  for ( unsigned int j=Range.next( 0 );
	j<Results.size();
	j=Range.next( ++j ) ) {
    datakey.save( df, Results[j].I, 0 );
    datakey.save( df, Results[j].DC );
    datakey.save( df, (double)Range.count( j ) );
    datakey.save( df, Results[j].MeanRate );
    datakey.save( df, Results[j].MeanRateSD );
    datakey.save( df, Results[j].PreRate );
    datakey.save( df, Results[j].PreRateSD );
    datakey.save( df, Results[j].VRest );
    datakey.save( df, Results[j].VRestSD );
    datakey.save( df, Results[j].OnRate );
    datakey.save( df, Results[j].OnRateSD );
    datakey.save( df, Results[j].OnTime*1000.0 );
    datakey.save( df, Results[j].SSRate );
    datakey.save( df, Results[j].SSRateSD );
    datakey.save( df, Results[j].VSS );
    datakey.save( df, Results[j].VSSSD );
    datakey.save( df, Results[j].SpikeCount );
    datakey.save( df, Results[j].SpikeCountSD );
    datakey.save( df, Results[j].Latency*1000.0 );
    datakey.save( df, Results[j].LatencySD*1000.0 );
    df << '\n';
  }
  df << "\n\n";
}


void FICurve::saveRate( void )
{
  ofstream df( addPath( "ficurve-rates.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Header.save( df, "# ", 0, FirstOnly );
  df << '\n';

  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.addNumber( "f", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );

  int inx = 0;
  for ( unsigned int j=Range.next( 0 );
	j<Results.size();
	j=Range.next( ++j ) ) {
    df << "#    index: " << Str( inx ) << '\n';
    df << "#   trials: " << Str( Range.count( j ) ) << '\n';
    df << "#        I: " << Str( Results[j].I ) << IUnit << '\n';
    df << "#       DC: " << Str( Results[j].DC ) << IUnit << '\n';
    df << "#  PreRate: " << Str( Results[j].PreRate ) << "Hz\n";
    df << "# MeanRate: " << Str( Results[j].MeanRate ) << "Hz\n";
    df << "#   OnRate: " << Str( Results[j].OnRate ) << "Hz\n";
    df << "#   SSRate: " << Str( Results[j].SSRate ) << "Hz\n";
    df << "#  Latency: " << Str( Results[j].Latency*1000.0 ) << "ms\n";
    df << '\n';
    key.saveKey( df, true, false );
    for ( int k=0; k<Results[j].Rate.size(); k++ ) {
      key.save( df, 1000.0*Results[j].Rate.pos( k ), 0 );
      key.save( df, Results[j].Rate[k] );
      key.save( df, Results[j].RateSD[k] );
      df << '\n';
    }
    df << "\n\n";
    inx++;
  }
  df << '\n';
}


void FICurve::saveSpikes( void )
{
  ofstream df( addPath( "ficurve-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Header.save( df, "# ", 0, FirstOnly );
  df << '\n';

  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  int inx = 0;
  for ( unsigned int j=Range.next( 0 );
	j<Results.size();
	j=Range.next( ++j ) ) {
    df << "#    index: " << Str( inx ) << '\n';
    df << "#   trials: " << Str( Range.count( j ) ) << '\n';
    df << "#        I: " << Str( Results[j].I ) << IUnit << '\n';
    df << "#       DC: " << Str( Results[j].DC ) << IUnit << '\n';
    df << "#  PreRate: " << Str( Results[j].PreRate ) << "Hz\n";
    df << "# MeanRate: " << Str( Results[j].MeanRate ) << "Hz\n";
    df << "#   OnRate: " << Str( Results[j].OnRate ) << "Hz\n";
    df << "#   SSRate: " << Str( Results[j].SSRate ) << "Hz\n";
    df << "#  Latency: " << Str( Results[j].Latency*1000.0 ) << "ms\n";
    df << '\n';
    Results[j].Spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
    df << '\n';
    inx++;
  }
  df << '\n';
}


void FICurve::saveTraces( void )
{
  ofstream df( addPath( "ficurve-traces.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Header.save( df, "# ", 0, FirstOnly );
  df << '\n';

  TableKey key;
  key.addNumber( "t", "ms", "%7.2f" );
  key.addNumber( "V", VUnit, "%6.1f" );
  if ( ! Results[Range.next( 0 )].Current.empty() )
    key.addNumber( "I", IUnit, "%6.3f" );
  key.saveKey( df, true, false );
  df << '\n';

  int inx = 0;
  for ( unsigned int j=Range.next( 0 );
	j<Results.size();
	j=Range.next( ++j ) ) {
    df << "#    index: " << Str( inx ) << '\n';
    df << "#   trials: " << Str( Range.count( j ) ) << '\n';
    df << "#        I: " << Str( Results[j].I ) << IUnit << '\n';
    df << "#       DC: " << Str( Results[j].DC ) << IUnit << '\n';
    df << "#  PreRate: " << Str( Results[j].PreRate ) << "Hz\n";
    df << "# MeanRate: " << Str( Results[j].MeanRate ) << "Hz\n";
    df << "#   OnRate: " << Str( Results[j].OnRate ) << "Hz\n";
    df << "#   SSRate: " << Str( Results[j].SSRate ) << "Hz\n";
    df << "#  Latency: " << Str( Results[j].Latency*1000.0 ) << "ms\n";
    df << '\n';
    if ( ! Results[j].Current.empty() ) {
      for ( unsigned int i=0; i<Results[j].Current.size() && i<Results[j].Voltage.size(); i++ ) {
	for ( int k=0; k<Results[j].Voltage[i].size(); k++ ) {
	  key.save( df, 1000.0*Results[j].Voltage[i].pos( k ), 0 );
	  key.save( df, Results[j].Voltage[i][k] );
	  key.save( df, Results[j].Current[i][k] );
	  df << '\n';
	}
	df << '\n';
      }
    }
    else {
      for ( unsigned int i=0; i<Results[j].Voltage.size(); i++ ) {
	for ( int k=0; k<Results[j].Voltage[i].size(); k++ ) {
	  key.save( df, 1000.0*Results[j].Voltage[i].pos( k ), 0 );
	  key.save( df, Results[j].Voltage[i][k] );
	  df << '\n';
	}
	df << '\n';
      }
    }
    df << '\n';
    inx++;
  }
  df << '\n';
}


FICurve::Data::Data( void )
  : DC( 0.0 ),
    I( 0.0 ),
    VRest( 0.0 ),
    VRestSQ( 0.0 ),
    VRestSD( 0.0 ),
    VSS( 0.0 ),
    VSSSQ( 0.0 ),
    VSSSD( 0.0 ),
    PreRate( 0.0 ),
    PreRateSD( 0.0 ),
    SSRate( 0.0 ),
    SSRateSD( 0.0 ),
    MeanRate( 0.0 ),
    MeanRateSD( 0.0 ),
    OnRate( 0.0 ),
    OnRateSD( 0.0 ),
    OnTime( 0.0 ),
    Latency( 0.0 ),
    LatencySD( 0.0 ),
    SpikeCount( 0.0 ),
    SpikeCountSD( 0.0 )
{
  Rate.clear();
  RateSD.clear();
  MeanCurrent.clear();
  Current.clear();
  Voltage.clear();
}


void FICurve::Data::analyze( int count, const InData &intrace,
			     const EventData &spikes,
			     const InData *incurrent, double iinfac,
			     double delay, double duration, double sswidth,
			     bool ignorenoresponse )
{
  // initialize:
  if ( Rate.empty() ) {
    Rate = SampleDataD( -delay, 2.0*duration, 0.001, 0.0 );
    RateSD = SampleDataD( -delay, 2.0*duration, 0.001, 0.0 );
    Spikes.reserve( 100 );
    if ( incurrent != 0 )
      MeanCurrent = SampleDataD( -delay, 2.0*duration, incurrent->stepsize(), 0.0 );
  }

  // voltage trace:
  Voltage.push_back( SampleDataF( -delay, 2.0*duration, intrace.stepsize(), 0.0F ) );
  intrace.copy( intrace.signalTime(), Voltage.back() );

  // current trace:
  if ( incurrent != 0 ) {
    Current.push_back( SampleDataF( -delay, 2.0*duration, incurrent->stepsize(), 0.0F ) );
    incurrent->copy( incurrent->signalTime(), Current.back() );
    Current.back() *= iinfac;
    int inx = incurrent->signalIndex() - MeanCurrent.index( 0.0 );
    for ( int k=0; k<MeanCurrent.size() && inx+k<incurrent->size(); k++ ) {
      double c = iinfac*(*incurrent)[inx+k];
      MeanCurrent[k] += (c - MeanCurrent[k])/(count+1);
    }
  }

  // resting potential:
  double vrest = Voltage.back().mean( -delay, 0.0 );
  VRest += (vrest - VRest)/(count+1);
  VRestSQ += (vrest*vrest - VRestSQ)/(count+1);
  VRestSD = sqrt( fabs(VRestSQ - VRest*VRest) );

  // steady-state potential:
  double vss = Voltage.back().mean( duration-sswidth, duration );
  VSS += (vss - VSS)/(count+1);
  VSSSQ += (vss*vss - VSSSQ)/(count+1);
  VSSSD = sqrt( fabs(VSSSQ - VSS*VSS) );

  // spikes:
  double sigtime = spikes.signalTime();
  Spikes.push( spikes, sigtime-delay, sigtime+2.0*duration, sigtime );

  // firing frequency:
  if ( ignorenoresponse ) {
    Spikes.frequency( Rate, RateSD );
    SSRate = Spikes.frequency( duration-sswidth, duration, SSRateSD );
  }
  else {
    Spikes.frequency( Rate, RateSD, 0.0 );
    SSRate = Rate.mean( duration-sswidth, duration );
    SSRateSD = RateSD.mean( duration-sswidth, duration );
  }

  PreRate = Spikes.rate( -delay, 0.0, &PreRateSD );
  MeanRate = Spikes.rate( 0.0, duration, &MeanRateSD );
  SpikeCount = Spikes.count( 0.0, duration, &SpikeCountSD );

  OnRate = PreRate;
  OnRateSD = PreRateSD;
  OnTime = 0.0;
  for ( int k=Rate.index( 0.0 ); k<Rate.index( duration-sswidth ) && k<Rate.size(); k++ ) {
    if ( fabs( Rate[k] - PreRate ) > fabs( OnRate - PreRate ) ) {
      OnRate = Rate[k];
      OnRateSD = RateSD[k];
      OnTime = Rate.pos( k );
    }
  }

  // latency:
  Latency = Spikes.latency( 0.0, &LatencySD );
}


void FICurve::keyPressEvent( QKeyEvent *qke )
{
  qke->ignore();
  switch ( qke->key()) {
  case Qt::Key_X:
    lock();
    Range.setSkipNocount();
    unlock();
    qke->accept();
    break;

  default:
    RePro::keyPressEvent( qke );

  }
}


class PlotMouseEvent : public QEvent
{

public:

  PlotMouseEvent( const Plot::MouseEvent &me )
    : QEvent( Type( User+11 ) ),
      ME( me )
  {
  }

  Plot::MouseEvent ME;
};


void FICurve::plotMouseEvent( Plot::MouseEvent &me )
{
  if ( ! tryLock( 5 ) ) {
    QCoreApplication::postEvent( this, new PlotMouseEvent( me ) );
    return;
  }
  P[1].lock();
  if ( me.xCoor() == Plot::First && me.yCoor() == Plot::First &&
       me.yPos() > P[1].yminRange() + 0.9*(P[1].ymaxRange() - P[1].yminRange()) ) {
    bool changed = false;
    if ( me.left() && me.released() ) {
      int inx = (int)::round( ( me.xPos() - Range.front() ) / IStep );
      if ( inx >= Range.size() )
	inx = Range.size() - 1;
      if ( inx < 0 )
	inx = 0;
      if ( me.shift() )
	Range.setSkipBelow( inx, ! Range.skip( inx ) );
      else if ( me.control() )
	Range.setSkipAbove( inx, ! Range.skip( inx ) );
      else
	Range.setSkip( inx, ! Range.skip( inx ) );
      Range.update();
      changed = true;
    }
    if ( ! PlotRangeSelection || changed ) {
      plotRangeSelection();
      P.draw();
    }
    PlotRangeSelection = true;
    me.setUsed();
  }
  else
    PlotRangeSelection = false;
  P[1].unlock();
  unlock();
}


void FICurve::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    PlotMouseEvent *pme = dynamic_cast<PlotMouseEvent*>( qce );
    plotMouseEvent( pme->ME );
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( FICurve, patchclamp );

}; /* namespace patchclamp */

#include "moc_ficurve.cc"
