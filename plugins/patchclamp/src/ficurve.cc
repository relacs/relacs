/*
  patchclamp/ficurve.cc
  f-I curve measured in current-clamp

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
#include <relacs/tablekey.h>
#include <relacs/patchclamp/ficurve.h>
using namespace relacs;

namespace patchclamp {


FICurve::FICurve( void )
  : RePro( "FICurve", "FICurve", "patchclamp",
	   "Jan Benda", "1.0", "Feb 17, 2010" ),
    P( 2, 2, true, this ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    VFac( 1.0 ),
    IFac( 1.0 ),
    IInFac( 1.0 )
{
  // add some options:
  addLabel( "Stimuli" );
  addSelection( "outcurrent", "Output trace", "Current-1" );
  addNumber( "imin", "Minimum injected current", 0.0, -1000.0, 1000.0, 0.001 );
  addBoolean( "iminrelthresh", "Minimum current is relative to threshold", false );
  addNumber( "imax", "Maximum injected current", 1.0, -1000.0, 1000.0, 0.001 );
  addNumber( "istep", "Minimum step-size of current", 0.001, 0.001, 1000.0, 0.001 );
  addBoolean( "userm", "Use membrane resistance for estimating istep from vstep", false );
  addNumber( "vstep", "Minimum step-size of voltage", 1.0, 0.001, 10000.0, 0.1 ).setActivation( "userm", "true" );
  addLabel( "Timing" );
  addNumber( "duration", "Duration of current output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "delay", "Delay before current pulses", 0.1, 0.001, 1.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause between current pulses", 0.4, 0.001, 1.0, 0.001, "sec", "ms" );
  addSelection( "shuffle", "Sequence of currents", RangeLoop::sequenceStrings() );
  addSelection( "ishuffle", "Initial sequence of currents for first repetition", RangeLoop::sequenceStrings() );
  addInteger( "iincrement", "Initial increment for currents", 0, 0, 1000, 1 );
  addInteger( "singlerepeat", "Number of immediate repetitions of a single stimulus", 1, 1, 10000, 1 );
  addInteger( "blockrepeat", "Number of repetitions of a fixed intensity increment", 10, 1, 10000, 1 );
  addInteger( "repeat", "Number of repetitions of the whole V-I curve measurement", 1, 1, 10000, 1 );
  addLabel( "Analysis" );
  addSelection( "involtage", "Input voltage trace", "V-1" );
  addSelection( "incurrent", "Input current trace", "Current-1" );
  addNumber( "fmax", "Maximum firing rate", 100.0, 0.0, 2000.0, 1.0, "Hz" );
  addNumber( "sswidth", "Window length for steady-state analysis", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addTypeStyle( OptWidget::TabLabel, Parameter::Label );
}


void FICurve::config( void )
{
  setText( "involtage", spikeTraceNames() );
  setToDefault( "involtage" );
  setText( "incurrent", currentTraceNames() );
  setToDefault( "incurrent" );
  setText( "outcurrent", currentOutputNames() );
  setToDefault( "outcurrent" );
}


void FICurve::notify( void )
{
  int involtage = index( "involtage" );
  if ( involtage >= 0 && SpikeTrace[involtage] >= 0 ) {
    VUnit = trace( SpikeTrace[involtage] ).unit();
    VFac = Parameter::changeUnit( 1.0, VUnit, "mV" );
    setUnit( "vstep", VUnit );
  }

  int outcurrent = index( "outcurrent" );
  if ( outcurrent >= 0 && CurrentOutput[outcurrent] >= 0 ) {
    IUnit = outTrace( CurrentOutput[outcurrent] ).unit();
    setUnit( "imin", IUnit );
    setUnit( "imax", IUnit );
    setUnit( "istep", IUnit );
    IFac = Parameter::changeUnit( 1.0, IUnit, "nA" );
  }

  int incurrent = index( "incurrent" );
  if ( incurrent >= 0 && CurrentTrace[incurrent] >= 0 ) {
    string iinunit = trace( CurrentTrace[incurrent] ).unit();
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
  int involtage = index( "involtage" );
  int incurrent = traceIndex( text( "incurrent", 0 ) );
  int outcurrent = outTraceIndex( text( "outcurrent", 0 ) );
  double imin = number( "imin" );
  bool iminrelthresh = boolean( "iminrelthresh" );
  double imax = number( "imax" );
  double istep = number( "istep" );
  bool userm = boolean( "userm" );
  double vstep = number( "vstep" );
  RangeLoop::Sequence shuffle = RangeLoop::Sequence( index( "shuffle" ) );
  RangeLoop::Sequence ishuffle = RangeLoop::Sequence( index( "ishuffle" ) );
  int iincrement = integer( "iincrement" );
  int singlerepeat = integer( "singlerepeat" );
  int blockrepeat = integer( "blockrepeat" );
  int repeat = integer( "repeat" );
  double duration = number( "duration" );
  double delay = number( "delay" );
  double pause = number( "pause" );
  double fmax = number( "fmax" );
  double sswidth = number( "sswidth" );
  if ( iminrelthresh ) {
    double ithresh = metaData( "Cell" ).number( "ithreshon" );
    if ( ithresh == 0.0 )
      ithresh = metaData( "Cell" ).number( "ithreshss" );
    imin += ithresh;
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
  if ( involtage < 0 || SpikeTrace[ involtage ] < 0 || SpikeEvents[ involtage ] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }
  if ( outcurrent < 0 ) {
    warning( "Invalid output current trace!" );
    return Failed;
  }
  if ( userm ) {
    double rm = metaData( "Cell" ).number( "rmss", "MOhm" );
    if ( rm <= 0 )
      rm = metaData( "Cell" ).number( "rm", "MOhm" );
    if ( rm <= 0 )
      warning( "Membrane resistance was not measured yet!" );
    else {
      Header.addNumber( "rm", rm, "MOhm" );
      vstep = Parameter::changeUnit( vstep, VUnit, "mV" ); 
      double ifac = Parameter::changeUnit( 1.0, "nA", IUnit ); 
      istep = ifac*vstep/rm;
    }
  }
  Header.addNumber( "imin", imin, IUnit );
  Header.addNumber( "istep", istep, IUnit );

  // don't print repro message:
  noMessage();

  // plot trace:
  plotToggle( true, true, 2.0*duration+delay, delay );

  // init:
  DoneState state = Completed;
  double samplerate = trace( SpikeTrace[involtage] ).sampleRate();
  Range.set( imin, imax, istep, repeat, blockrepeat, singlerepeat );
  if ( iincrement <= 0 )
    Range.setLargeIncrement();
  else
    Range.setIncrement( iincrement );
  Range.setSequence( ishuffle );
  int prevrepeat = 0;
  Results.clear();
  Results.resize( Range.size(), Data( delay, duration, 1.0/samplerate,
				      incurrent >= 0 ) );

  // plot:
  P.lock();
  P[0].setXLabel( "Time [ms]" );
  P[0].setXRange( -1000.0*delay, 1000.0*(duration+delay) );
  P[0].setYLabel( "Firing rate [Hz]" );
  P[0].setYFallBackRange( 0.0, 20.0 );
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[1].setXLabel( "Current [" + IUnit + "]" );
  P[1].setXRange( imin, imax );
  P[1].setYLabel( "Firing rate [Hz]" );
  P[1].setYFallBackRange( 0.0, 20.0 );
  P[1].setYRange( 0.0, Plot::AutoScale );
  P.unlock();

  // signal:
  OutData signal( duration, 1.0/samplerate );
  signal.setTrace( outcurrent );
  signal.setDelay( delay );
  double dccurrent = stimulusData().number( outTraceName( outcurrent ) );

  // write stimulus:
  sleep( pause );
  for ( Range.reset(); ! Range && softStop() == 0; ++Range ) {

    if ( prevrepeat < Range.currentRepetition() ) {
      if ( Range.currentRepetition() == 1 ) {
	Range.setSequence( shuffle );
	Range.update();
      }
      prevrepeat = Range.currentRepetition();
    }

    double amplitude = *Range;
    if ( fabs( amplitude ) < 1.0e-8 )
      amplitude = 0.0;

    Str s = "Current <b>" + Str( amplitude ) + " " + IUnit +"</b>";
    s += ",  Loop <b>" + Str( Range.count()+1 ) + "</b>";
    message( s );

    timeStamp();
    signal.setIdent( "I=" + Str( amplitude ) + IUnit );
    signal = amplitude;
    signal.back() = dccurrent;
    write( signal );
    if ( signal.failed() ) {
      if ( signal.overflow() ) {
	printlog( "Requested amplitude I=" + Str( amplitude ) + IUnit + "too high!" );
	for ( int k = Range.size()-1; k >= 0; k-- ) {
	  if ( Range[k] > signal.maxValue() )
	    Range.setSkip( k );
	  else
	    break;
	}
	Range.noCount();
	continue;
      }
      else if ( signal.underflow() ) {
	printlog( "Requested amplitude I=" + Str( amplitude ) + IUnit + "too small!" );
	for ( int k = 0; k < Range.size(); k++ ) {
	  if ( Range[k] < signal.minValue() )
	    Range.setSkip( k );
	  else
	    break;
	}
	Range.noCount();
	continue;
      }
      else {
	warning( signal.errorText() );
	return Failed;
      }
    }

    sleep( delay + 2.0*duration + 0.01 );
    if ( interrupt() ) {
      if ( Range.count() < 1 )
	state = Aborted;
      break;
    }

    Results[Range.pos()].I = amplitude;
    Results[Range.pos()].DC = dccurrent;
    Results[Range.pos()].analyze( Range.count(), trace( involtage ),
				  events( SpikeEvents[involtage] ), 
				  incurrent >= 0 ? &trace( incurrent ) : 0,
				  IInFac, delay, duration, sswidth );

    if ( Results[Range.pos()].SpikeCount <= 0.01 ) {
      Range.setSkipBelow( Range.pos() );
      Range.noCount();
    }
    if ( Results[Range.pos()].SSRate > fmax ) {
      Range.setSkipAbove( Range.pos() );
      Range.noCount();
    }

    plot( duration );
    sleepOn( duration + pause );
    if ( interrupt() ) {
      if ( Range.count() < 1 )
	state = Aborted;
      break;
    }
  }

  if ( state == Completed )
    save();

  return state;
}


void FICurve::plot( double duration )
{
  P.lock();

  // rate and spikes:
  const Data &data = Results[Range.pos()];
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
  P[1].setXRange( imin, imax );
  P[1].plot( rm, 1.0, Plot::Cyan, 3, Plot::Solid, Plot::Circle, 6, Plot::Cyan, Plot::Cyan );
  P[1].plot( om, 1.0, Plot::Green, 3, Plot::Solid, Plot::Circle, 6, Plot::Green, Plot::Green );
  P[1].plot( sm, 1.0, Plot::Red, 3, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P[1].plot( mm, 1.0, Plot::Orange, 3, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );
  int c = Range.pos();
  MapD am;
  am.push( Results[c].I, Results[c].PreRate );
  am.push( Results[c].I, Results[c].OnRate );
  am.push( Results[c].I, Results[c].SSRate );
  am.push( Results[c].I, Results[c].MeanRate );
  P[1].plot( am, 1.0, Plot::Transparent, 3, Plot::Solid, Plot::Circle, 8, Plot::Yellow, Plot::Transparent );

  P.unlock();
  P.draw();
}


void FICurve::save( void )
{
  saveData();
  saveRate();
  saveSpikes();
  saveTraces();
}


void FICurve::saveData( void )
{
  ofstream df( addPath( "ficurve-data.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addLabel( "Stimulus" );
  datakey.addNumber( "I", IUnit, "%6.3f" );
  datakey.addNumber( "IDC", IUnit, "%6.3f" );
  datakey.addNumber( "trials", "1", "%6.0f" );
  datakey.addLabel( "Firing rate" );
  datakey.addNumber( "f", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.addLabel( "Baseline" );
  datakey.addNumber( "f_b", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.addLabel( "Peak rate" );
  datakey.addNumber( "f_on", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.addNumber( "t_on", "ms", "%5.1f" );
  datakey.addLabel( "Steady-state" );
  datakey.addNumber( "f_ss", "Hz", "%5.1f" );
  datakey.addNumber( "s.d.", "Hz", "%5.1f" );
  datakey.addLabel( "Spike count" );
  datakey.addNumber( "count", "1", "%7.1f" );
  datakey.addNumber( "s.d.", "1", "%7.1f" );
  datakey.addLabel( "Latency" );
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
    datakey.save( df, Results[j].OnRate );
    datakey.save( df, Results[j].OnRateSD );
    datakey.save( df, Results[j].OnTime*1000.0 );
    datakey.save( df, Results[j].SSRate );
    datakey.save( df, Results[j].SSRateSD );
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

  Header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
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

  Header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
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

  Header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
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


FICurve::Data::Data( double delay, double duration, double stepsize,
		     bool current )
  : DC( 0.0 ),
    I( 0.0 ),
    VRest( 0.0 ),
    VRestsd( 0.0 ),
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
    SpikeCountSD( 0.0 ),
    Rate( -delay, 2.0*duration, 0.001, 0.0 ),
    RateSD( -delay, 2.0*duration, 0.001, 0.0 )
{
  if ( current )
    MeanCurrent = SampleDataD( -delay, duration, stepsize, 0.0 );
  else
    MeanCurrent.clear();
  Current.clear();
  Voltage.clear();
}


void FICurve::Data::analyze( int count, const InData &intrace,
			     const EventData &spikes,
			     const InData *incurrent, double iinfac,
			     double delay, double duration, double sswidth )
{
  // voltage trace:
  Voltage.push_back( SampleDataF( -delay, duration, intrace.stepsize(), 0.0F ) );
  intrace.copy( intrace.signalTime(), Voltage.back() );

  // current trace:
  if ( incurrent != 0 ) {
    Current.push_back( SampleDataF( -delay, duration, incurrent->stepsize(), 0.0F ) );
    incurrent->copy( incurrent->signalTime(), Current.back() );
    Current.back() *= iinfac;
    int inx = incurrent->signalIndex() - MeanCurrent.index( 0.0 );
    for ( int k=0; k<MeanCurrent.size() && inx+k<incurrent->size(); k++ ) {
      double c = iinfac*(*incurrent)[inx+k];
      MeanCurrent[k] += (c - MeanCurrent[k])/(count+1);
    }
  }

  // stimulus amplitude:
  if ( ! MeanCurrent.empty() ) {
    DC = MeanCurrent.mean( -delay, 0.0 );
    I = MeanCurrent.mean( 0.0, duration );
  }

  // spikes:
  double sigtime = spikes.signalTime();
  Spikes.push( spikes, sigtime-delay, sigtime+2.0*duration, sigtime );

  // firing frequency:
  Spikes.frequency( Rate, RateSD );

  PreRate = Spikes.rate( -delay, 0.0, &PreRateSD );
  SSRate = Spikes.frequency( duration-sswidth, duration, SSRateSD );
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


addRePro( FICurve );

}; /* namespace patchclamp */

#include "moc_ficurve.cc"
