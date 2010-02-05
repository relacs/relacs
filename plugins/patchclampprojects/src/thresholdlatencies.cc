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
    P( this )
{
  // add some options:
  addSelection( "involtage", "Input voltage trace", "V-1" );
  addSelection( "incurrent", "Input current trace", "Current-1" );
  addSelection( "outcurrent", "Output trace", "Current-1" );
  addNumber( "duration", "Duration of output", 0.1, 0.0, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 1.0, 0.0, 1000.0, 0.01, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 );
  addNumber( "amplitudestep", "Size of amplitude steps used for oscillating around threshold", 0.1, 0.0, 1000.0, 0.01 );
  addNumber( "delay", "Time before stimullus onset", 0.05, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "savetracetime", "Length of trace to be saved and analyzed", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
}


void ThresholdLatencies::config( void )
{
  setText( "involtage", spikeTraceNames() );
  setToDefault( "involtage" );
  //  setText( "incurrent", spikeTraceNames() );
  //  setToDefault( "incurrent" );
  setText( "outcurrent", currentOutputNames() );
  setToDefault( "outcurrent" );
}


void ThresholdLatencies::notify( void )
{
  int outcurrent = index( "outcurrent" );
  IUnit = outTrace( outcurrent ).unit();
  setUnit( "amplitudestep", IUnit );
  //  IFac = Parameter::changeUnit( 1.0, IUnit, "nA" );

  int involtage = index( "involtage" );
  VUnit = trace( involtage ).unit();
  //  VFac = Parameter::changeUnit( 1.0, VUnit, "mV" );
}


int ThresholdLatencies::main( void )
{
  int involtage = index( "involtage" );
  int incurrent = traceIndex( text( "incurrent", 0 ) );
  int outcurrent = outTraceIndex( text( "outcurrent", 0 ) );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double amplitudestep = number( "amplitudestep" );
  double delay = number( "delay" );
  double savetracetime = number( "savetracetime" );
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
  Results.clear();

  // save:
  TableKey datakey;
  datakey.addNumber( "t", "ms", "%7.2f" );
  datakey.addNumber( "V", VUnit, "%6.1f" );
  if ( incurrent >= 0 )
    datakey.addNumber( "I", IUnit, "%6.1f" );
  ofstream df;
  if ( completeRuns() <= 0 )
    df.open( addPath( "thresholdlatencies.dat" ).c_str() );
  else
    df.open( addPath( "thresholdlatencies.dat" ).c_str(),
	     ofstream::out | ofstream::app );
  settings().save( df, "# " );
  df << '\n';
  datakey.saveKey( df );
  df << '\n';

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
  double amplitude = 2.0;

  // write stimulus:
  sleep( pause );
  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {

    timeStamp();

    Str s = "Amplitude <b>" + Str( amplitude ) + " " + IUnit +"</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    message( s );

    // signal:
    signal = amplitude;
    signal.setIdent( "const ampl=" + Str( amplitude ) + IUnit );
    signal.back() = 0.0;
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      if ( count > 0 )
	break;
      else {
	df << '\n';
	Results.clear();
	return Failed;
      }
    }

    sleep( savetracetime + 0.01 );
    if ( interrupt() ) {
      if ( count > 0 )
	break;
      else {
	df << '\n';
	Results.clear();
	return Aborted;
      }
    }

    analyze( involtage, incurrent,
	     delay, duration, savetracetime );
    save( df, datakey, incurrent );
    plot( duration );

    if ( Results.back().Spikes > 0 )
      amplitude -= amplitudestep;
    else
      amplitude += amplitudestep;

    sleepOn( delay + duration + pause );

  }

  df << '\n';
  Results.clear();

  return Completed;
}


void ThresholdLatencies::analyze( int involtage, int incurrent,
				  double delay, double duration, double savetime )
{
  if ( Results.size() >= 20 )
    Results.pop_front();

  double amplitude = 0.0; // XXX
  if ( incurrent >= 0 )
    Results.push_back( Data( delay, savetime, amplitude,
			     trace( SpikeTrace[involtage] ), trace( incurrent ) ) );
  else
    Results.push_back( Data( delay, savetime, amplitude, trace( SpikeTrace[involtage] ) ) );
  Results.back().Spikes = events( SpikeEvents[involtage] ).count( events( SpikeEvents[involtage] ).signalTime(),
								  events( SpikeEvents[involtage] ).signalTime() + savetime );
}


void ThresholdLatencies::save( ofstream &df, TableKey &datakey, int incurrent )
{
  df << "# amplitude: " << Str( Results.back().Amplitude ) << IUnit << '\n';
  df << "# spike count: " << Str( Results.back().Spikes ) << '\n';
  if ( incurrent >= 0 ) {
    for ( int k=0; k<Results.back().Voltage.size(); k++ ) {
      datakey.save( df, 1000.0*Results.back().Voltage.pos( k ), 0 );
      datakey.save( df, Results.back().Voltage[k] );
      datakey.save( df, Results.back().Current[k] );
      df << '\n';
    }
  }
  else {
    for ( int k=0; k<Results.back().Voltage.size(); k++ ) {
      datakey.save( df, 1000.0*Results.back().Voltage.pos( k ), 0 );
      datakey.save( df, Results.back().Voltage[k] );
      df << '\n';
    }
  }
  df << '\n';
}


void ThresholdLatencies::plot( double duration )
{
  P.lock();
  P.clear();
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
				double amplitude,
				const InData &voltage,
				const InData &current )
{
  Amplitude = amplitude;
  Voltage.resize( -delay, savetime-delay, voltage.stepsize(), 0.0 );
  voltage.copy( voltage.signalTime(), Voltage );
  Current.resize( -delay, savetime-delay, current.stepsize(), 0.0 );
  current.copy( current.signalTime(), Current );
  Spikes = 0;
}


ThresholdLatencies::Data::Data( double delay, double savetime,
				double amplitude,
				const InData &voltage )
{
  Amplitude = amplitude;
  Voltage.resize( -delay, savetime-delay, voltage.stepsize(), 0.0 );
  voltage.copy( voltage.signalTime(), Voltage );
  Current.clear();
  Spikes = 0;
}


addRePro( ThresholdLatencies );

}; /* namespace patchclampprojects */

#include "moc_thresholdlatencies.cc"
