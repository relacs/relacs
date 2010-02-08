/*
  patchclampprojects/thresholdlatencies.cc
  Measures spike latencies in response to pulses close to the firing threshold.

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
#include <relacs/patchclampprojects/thresholdlatencies.h>
using namespace relacs;

namespace patchclampprojects {


ThresholdLatencies::ThresholdLatencies( void )
  : RePro( "ThresholdLatencies", "ThresholdLatencies", "patchclampprojects",
	   "Jan Benda", "1.0", "Feb 04, 2010" ),
    P( this ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    IInFac( 1.0 )
{
  // add some options:
  addLabel( "Traces" );
  addSelection( "involtage", "Input voltage trace", "V-1" );
  addSelection( "incurrent", "Input current trace", "Current-1" );
  addSelection( "outcurrent", "Output trace", "Current-1" );
  addLabel( "Timing" );
  addNumber( "duration", "Duration of output", 0.1, 0.0, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 1.0, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "delay", "Time before stimullus onset", 0.05, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "savetracetime", "Length of trace to be saved and analyzed", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 );
  addLabel( "Amplitudes" );
  addNumber( "startamplitude", "Initial amplitude of current stimulus", 1.0, 0.0, 1000.0, 0.01 );
  addNumber( "amplitudestep", "Size of amplitude steps used for oscillating around threshold", 0.1, 0.0, 1000.0, 0.01 );
  addLabel( "Background amplitude" );
  addSelection( "backamplitudesel", "Background amplitude", "none|relative|absolute" );
  addNumber( "backamplitudefrac", "Fraction of threshold", 0.9, 0.0, 1.0, 0.01, "1", "%" );
  addNumber( "backamplitudedecr", "Decrement below threshold", 0.1, 0.0, 1000.0, 0.01 );
  addInteger( "backrepeats", "Repetitions of stimulus for threshold estimation", 10, 0, 10000, 1 );

  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
}


void ThresholdLatencies::config( void )
{
  setText( "involtage", spikeTraceNames() );
  setToDefault( "involtage" );
  setText( "incurrent", currentTraceNames() );
  setToDefault( "incurrent" );
  setText( "outcurrent", currentOutputNames() );
  setToDefault( "outcurrent" );
}


void ThresholdLatencies::notify( void )
{
  int involtage = index( "involtage" );
  if ( involtage >= 0 && SpikeTrace[involtage] >= 0 ) {
    VUnit = trace( SpikeTrace[involtage] ).unit();
  }

  int outcurrent = index( "outcurrent" );
  if ( outcurrent >= 0 && CurrentOutput[outcurrent] >= 0 ) {
    IUnit = outTrace( CurrentOutput[outcurrent] ).unit();
    setUnit( "startamplitude", IUnit );
    setUnit( "amplitudestep", IUnit );
    setUnit( "backamplitudedecr", IUnit );
  }

  int incurrent = index( "incurrent" );
  if ( incurrent >= 0 && CurrentTrace[incurrent] >= 0 ) {
    string iinunit = trace( CurrentTrace[incurrent] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int ThresholdLatencies::main( void )
{
  int involtage = index( "involtage" );
  int incurrent = traceIndex( text( "incurrent", 0 ) );
  int outcurrent = outTraceIndex( text( "outcurrent", 0 ) );
  double duration = number( "duration" );
  double pause = number( "pause" );
  double delay = number( "delay" );
  double savetracetime = number( "savetracetime" );
  int repeats = integer( "repeats" );
  double amplitude = number( "startamplitude" );
  double amplitudestep = number( "amplitudestep" );
  int backamplitudesel = index( "backamplitudesel" );
  double backamplitudefrac = number( "backamplitudefrac" );
  double backamplitudedecr = number( "backamplitudedecr" );
  int backrepeats = integer( "backrepeats" );

  if ( savetracetime < duration ) {
    warning( "savetracetime must be at least as long as the stimulus duration!" );
    return Failed;
  }
  if ( delay + duration + pause < savetracetime ) {
    warning( "Stimulus duration plus pause plus delay must be at least as long as savetracetime!" );
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

  double samplerate = trace( SpikeTrace[involtage] ).sampleRate();

  // don't print repro message:
  noMessage();

  // init:
  /* 0: find threshold
     1: measure threshold
     2: find threshold with background
     3: record */
  int mode = backamplitudesel == 0 ? 2 : 0;
  bool record = false;
  DoneState state = Completed;
  double backamplitude = 0.0;
  Results.clear();
  Amplitudes.clear();
  Amplitudes.reserve( 100 );
  Latencies.clear();
  Latencies.reserve( 100 );
  SpikeCount = 0;
  TrialCount = 0;
  TableKey tracekey;
  ofstream tf;
  TableKey spikekey;
  ofstream sf;

  // plot trace:
  plotToggle( true, true, savetracetime, delay );

  // plot:
  P.lock();
  P.setXRange( -1000.0*delay, 1000.0*(savetracetime-delay) );
  P.setYLabel( trace( SpikeTrace[involtage] ).ident() + " [" + VUnit + "]" );
  P.unlock();

  // signal:
  OutData signal( duration, 1.0/samplerate );
  signal.setTrace( outcurrent );
  signal.setDelay( delay );

  // write stimulus:
  sleep( pause );
  for ( int count=1; softStop() == 0; count++ ) {

    timeStamp();

    Str s;
    if ( mode == 0 || mode == 2 )
      s = "<b>Search threshold: </b>";
    else if ( mode == 1 )
      s = "<b>Measure threshold: </b>";
    else if ( mode == 3 )
      s = "<b>Measure response: </b>";
    if ( mode >= 2 )
      s += "Background amplitude <b>" + Str( backamplitude ) + " " + IUnit +"</b>, ";
    s += "Amplitude <b>" + Str( amplitude ) + " " + IUnit +"</b>";
    if ( mode==1 || record )
      s += ",  Loop <b>" + Str( count ) + "</b>";
    message( s );

    // signal:
    signal = amplitude;
    signal.setIdent( "const ampl=" + Str( amplitude ) + IUnit );
    signal.back() = backamplitude;
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      if ( ! record || count <= 1 )
	state = Failed;
      break;
    }

    // sleep:
    sleep( savetracetime + 0.01 );
    if ( interrupt() ) {
      if ( ! record || count <= 1 )
	state = Aborted;
      break;
    }

    // analyze, plot, and save:
    analyze( involtage, incurrent, amplitude,
	     delay, duration, savetracetime );
    if ( record ) {
      saveTrace( tf, tracekey, count-1 );
      saveSpikes( sf, spikekey, count-1 );
    }
    plot( duration );

    if ( Results.back().SpikeCount > 0 )
      amplitude -= amplitudestep;
    else
      amplitude += amplitudestep;

    // switch modes:
    if ( ! record ) {
      int prevmode = mode;
      if ( mode == 1 ) {
	// threshold measurement:
	if ( count >= backrepeats ) {
	  mode++;
	  backamplitude = Amplitudes.mean();
	  if ( backamplitudesel == 1 )
	    backamplitude *= backamplitudefrac;
	  else
	    backamplitude -= backamplitudedecr;
	}
      }
      else {
	// find threshold:
	if ( Results.size() > 1 &&
	     ( ( Results[Results.size()-2].SpikeCount <= 0 &&
		 Results[Results.size()-1].SpikeCount > 0 ) ||
	       ( Results[Results.size()-2].SpikeCount > 0 &&
		 Results[Results.size()-1].SpikeCount <= 0 ) ) ) {
	  mode++;
	}
	if ( mode >= 3 ) {
	  record = true;
	  openFiles( tf, tracekey, sf, spikekey, incurrent );
	}
      }
      if ( prevmode != mode ) {
	// new mode, reset:
	count = 1;
	Results.clear();
	SpikeCount = 0;
	TrialCount = 0;
	Amplitudes.clear();
	Amplitudes.reserve( repeats > 0 ? repeats : 1000 );
	Latencies.clear();
	Latencies.reserve( repeats > 0 ? repeats : 1000 );
      }
    }
    TrialCount = count;

    sleepOn( delay + duration + pause );

    if ( record && repeats > 0 && count >= repeats )
      break;

  }

  tf << '\n';
  sf << '\n';
  if ( record && TrialCount > 0 )
    saveData();
  Results.clear();
  Latencies.clear();
  Amplitudes.clear();
  writeZero( outcurrent );
  return state;
}


void ThresholdLatencies::analyze( int involtage, int incurrent, double amplitude,
				  double delay, double duration, double savetime )
{
  if ( Results.size() >= 20 )
    Results.pop_front();

  if ( incurrent >= 0 ) {
    Results.push_back( Data( delay, savetime, trace( SpikeTrace[involtage] ),
			     trace( incurrent ) ) );
    Results.back().Current *= IInFac;
    Results.back().Amplitude = Results.back().Current.mean( 0.5*duration, duration );
  }
  else {
    Results.push_back( Data( delay, savetime, trace( SpikeTrace[involtage] ) ) );
    Results.back().Amplitude = amplitude;
  }
  events( SpikeEvents[involtage] ).copy( events( SpikeEvents[involtage] ).signalTime() - delay,
					 events( SpikeEvents[involtage] ).signalTime() + savetime - delay,
					 events( SpikeEvents[involtage] ).signalTime(),
					 Results.back().Spikes );
  Results.back().SpikeCount = Results.back().Spikes.count( 0.0, savetime-delay );

  if ( Results.back().SpikeCount > 0 ) {
    SpikeCount++;
    Latencies.push( Results.back().Spikes.latency( 0.0 ) );
  }
  Amplitudes.push( Results.back().Amplitude );
}


void ThresholdLatencies::openFiles( ofstream &tf, TableKey &tracekey,
				    ofstream &sf, TableKey &spikekey,
				    int incurrent )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( incurrent >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.1f" );
  if ( completeRuns() <= 0 )
    tf.open( addPath( "thresholdlatencies-traces.dat" ).c_str() );
  else
    tf.open( addPath( "thresholdlatencies-traces.dat" ).c_str(),
             ofstream::out | ofstream::app );
  settings().save( tf, "# " );
  tf << '\n';
  tracekey.saveKey( tf, true, false );
  tf << '\n';

  spikekey.addNumber( "t", "ms", "%7.2f" );
  if ( completeRuns() <= 0 )
    sf.open( addPath( "thresholdlatencies-spikes.dat" ).c_str() );
  else
    sf.open( addPath( "thresholdlatencies-spikes.dat" ).c_str(),
             ofstream::out | ofstream::app );
  settings().save( sf, "# " );
  sf << '\n';
  spikekey.saveKey( sf, true, false );
  sf << '\n';
}


void ThresholdLatencies::saveTrace( ofstream &tf, TableKey &tracekey, int index )
{
  tf << "#       index: " << Str( index ) << '\n';
  tf << "#   amplitude: " << Str( Results.back().Amplitude ) << IUnit << '\n';
  tf << "# spike count: " << Str( Results.back().SpikeCount ) << '\n';
  if ( ! Results.back().Current.empty() ) {
    for ( int k=0; k<Results.back().Voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*Results.back().Voltage.pos( k ), 0 );
      tracekey.save( tf, Results.back().Voltage[k] );
      tracekey.save( tf, Results.back().Current[k] );
      tf << '\n';
    }
  }
  else {
    for ( int k=0; k<Results.back().Voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*Results.back().Voltage.pos( k ), 0 );
      tracekey.save( tf, Results.back().Voltage[k] );
      tf << '\n';
    }
  }
  tf << '\n';
}


void ThresholdLatencies::saveSpikes( ofstream &sf, TableKey &spikekey, int index )
{
  sf << "#       index: " << Str( index ) << '\n';
  sf << "#   amplitude: " << Str( Results.back().Amplitude ) << IUnit << '\n';
  sf << "# spike count: " << Str( Results.back().SpikeCount ) << '\n';
  Results.back().Spikes.saveText( sf, 1000.0, 7, 2, 'f', "-0" );
  sf << '\n';
}


void ThresholdLatencies::saveData( void )
{
  TableKey datakey;
  double asd = 0.0;
  double am = Amplitudes.mean( asd );
  datakey.addNumber( "amplitude", IUnit, "%7.2f", am );
  datakey.addNumber( "s.d.", IUnit, "%7.2f", asd );
  datakey.addNumber( "trials", "1", "%6.0f", (double)TrialCount );
  datakey.addNumber( "spikes", "1", "%6.0f", (double)SpikeCount );
  datakey.addNumber( "prob", "%", "%5.1f", 100.0*(double)SpikeCount/(double)TrialCount );
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  datakey.addNumber( "latency", "ms", "%6.2f", 1000.0*lm );
  datakey.addNumber( "s.d.", "ms", "%6.2f", 1000.0*lsd );

  ofstream df;
  if ( completeRuns() <= 0 ) {
    df.open( addPath( "thresholdlatencies-data.dat" ).c_str() );
    datakey.saveKey( df );
  }
  else
    df.open( addPath( "thresholdlatencies-data.dat" ).c_str(),
             ofstream::out | ofstream::app );

  datakey.saveData( df );
}


void ThresholdLatencies::plot( double duration )
{
  P.lock();
  P.clear();
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  P.setTitle( "p=" + Str( 100.0*(double)SpikeCount/(double)TrialCount, 0, 0, 'f' ) +
	      "%,  latency=(" + Str( 1000.0*lm, 0, 0, 'f' ) +
	      "+/-" + Str( 1000.0*lsd, 0, 0, 'f' ) + ") ms" );
  P.plotVLine( 0, Plot::White, 2 );
  P.plotVLine( 1000.0*duration, Plot::White, 2 );
  for ( unsigned int k=0; k<Results.size()-1; k++ ) {
    SampleDataD vtrace = Results[k].Voltage;
    vtrace += 10.0*(Results.size() - k - 1);
    P.plot( vtrace, 1000.0, Plot::Orange, 2, Plot::Solid );
  }
  if ( ! Results.empty() )
    P.plot( Results.back().Voltage, 1000.0, Plot::Yellow, 4, Plot::Solid );
  P.unlock();
  P.draw();
}


ThresholdLatencies::Data::Data( double delay, double savetime,
				const InData &voltage,
				const InData &current )
  : Spikes( 10 )
{
  Amplitude = 0.0;
  Voltage.resize( -delay, savetime-delay, voltage.stepsize(), 0.0 );
  voltage.copy( voltage.signalTime(), Voltage );
  Current.resize( -delay, savetime-delay, current.stepsize(), 0.0 );
  current.copy( current.signalTime(), Current );
  SpikeCount = 0;
}


ThresholdLatencies::Data::Data( double delay, double savetime,
				const InData &voltage )
  : Spikes( 10 )
{
  Amplitude = 0.0;
  Voltage.resize( -delay, savetime-delay, voltage.stepsize(), 0.0 );
  voltage.copy( voltage.signalTime(), Voltage );
  Current.clear();
  SpikeCount = 0;
}


addRePro( ThresholdLatencies );

}; /* namespace patchclampprojects */

#include "moc_thresholdlatencies.cc"
