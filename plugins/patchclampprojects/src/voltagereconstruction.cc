/*
  patchclampprojects/voltagereconstruction.cc
  Reconstructs the membrane voltage inbetween the spikes from latency measurements.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#include <list>
#include <relacs/tablekey.h>
#include <relacs/map.h>
#include <relacs/sampledata.h>
#include <relacs/random.h>
#include <relacs/patchclampprojects/voltagereconstruction.h>
using namespace relacs;

namespace patchclampprojects {


VoltageReconstruction::VoltageReconstruction( void )
  : RePro( "VoltageReconstruction", "patchclampprojects",
	   "Jan Benda, Ales Skorjanc", "1.1", "Nov 25, 2010" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    IInFac( 1.0 )
{
  // add some options:
  addSelection( "dcamplitudesrc", "Set initial dc-current to", "DC|custom|DC" );
  addNumber( "dcamplitude", "Initial amplitude of dc-current", 0.0, 0.0, 1000.0, 0.01 ).setActivation( "dcamplitudesrc", "custom" );
  addNumber( "amplitude", "Test-pulse amplitude", 0.1, 0.0, 1000.0, 0.01 );
  addNumber( "duration", "Duration of test-pulse", 0.005, 0.0, 1000.0, 0.001, "seconds", "ms" );
  addInteger( "repeats", "Number of test-pulses", 100, 0, 1000000 );
  addNumber( "rateduration", "Time for initial estimate of firing rate", 1.0, 0.0, 100000.0, 0.01, "seconds", "ms" );

  // setup plot:
  P.lock();
  P.setLMarg( 7.0 );
  P.setRMarg( 7.0 );
  P.setTMarg( 1.0 );
  P.setBMarg( 5.0 );
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "Latency [ms]" );
  P.setY2Label( "Membrane voltage [" + VUnit + "]" );
  P.setY2Tics();
  P.setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		  Plot::Center, -90.0 );
  P.unlock();
  setWidget( &P );
}


void VoltageReconstruction::config( void )
{
  if ( SpikeTrace[0] >= 0 )
    VUnit = trace( SpikeTrace[0] ).unit();
  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "dcamplitude", IUnit );
    setUnit( "amplitude", IUnit );
  }
  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int VoltageReconstruction::main( void )
{
  // get options:
  int dcamplitudesrc = index( "dcamplitudesrc" );
  double dcamplitude = number( "dcamplitude" );
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  int repeats = number( "repeats" );
  double rateduration = number( "rateduration" );

  double orgdcamplitude = stimulusData().number( outTraceName( 0 ) );
  if ( dcamplitudesrc == 1 ) // dc
    dcamplitude = orgdcamplitude;

  if ( SpikeTrace[ 0 ] < 0 || SpikeEvents[ 0 ] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }

  OutData orgdcsignal( orgdcamplitude );
  orgdcsignal.setTrace( CurrentOutput[0] );
  orgdcsignal.setIdent( "DC=" + Str( orgdcamplitude ) + IUnit );
  orgdcsignal.addDescription( "stimulus/value" );
  orgdcsignal.description().addNumber( "Intensity", orgdcamplitude, IUnit );

  // measure firing rate:
  for ( int n=0; ; n++ ) {
    message( "<b>Measure</b> mean firing rate " + Str( '.', n ) );
    sleep( rateduration-n*1.0 > 1.0 ? 1.0 : rateduration-n*1.0 );
    if ( rateduration - n*1.0 <= 1.0  )
      break;
  }
  if ( interrupt() ) {
    directWrite( orgdcsignal );
    return Aborted;
  }
  double meanrate = events( SpikeEvents[0] ).rate( currentTime() - rateduration,
						   currentTime() );
  if ( meanrate < 0.01 ) {
    warning( "Not enough spikes evoked by dc current!" );
    directWrite( orgdcsignal );
    return Failed;
  }
  double period = 1.0/meanrate;
  
  // plot trace:
  tracePlotSignal( rateduration, 0.5*rateduration );

  // setup plots:
  P.lock();
  P.clear();
  P.setXRange( 0.0, 1000.0*period );    
  P.setYRange( Plot::AutoScale, 0.0 );    
  P.unlock();

  // stimulus:
  OutData signal( duration, trace( SpikeTrace[0] ).stepsize() );
  signal = dcamplitude + amplitude;
  signal.back() = dcamplitude;
  signal.setTrace( CurrentOutput[0] );
  signal.setIdent( "I=" + Str( dcamplitude + amplitude ) + IUnit );
  signal.addDescription( "stimulus/pulse" );
  signal.description().addNumber( "Intensity", dcamplitude + amplitude, IUnit );
  signal.description().addNumber( "IntensityOffset", dcamplitude, IUnit );
  signal.description().addNumber( "Duration", 1000.0*duration, "ms" );

  // data:
  const InData &data = trace( SpikeTrace[0] );
  MapD latencies;
  latencies.reserve( repeats>0?repeats:10000 );
  list< SampleDataF > voltages;
  SampleDataF meanvoltage( -1.5*period, 4.5*period, data.stepsize(), 0.0 );
  SampleDataF voltagesq( meanvoltage );
  SampleDataF voltagesd( meanvoltage );
  SampleDataF current;
  if ( CurrentTrace[0] >= 0 ) {
    current.resize( -1.5*period, 4.5*period,
		    trace( CurrentTrace[0] ).stepsize() );
  }

  // header:
  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "ReProIndex", reproCount() );
  header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  header.addNumber( "firing rate", meanrate, "Hz", "%0.1f" );
  header.addNumber( "period", 1000.0*period, "ms", "%0.2f" );

  // files:
  ofstream tf;
  TableKey tracekey;

  DoneState state = Completed;
  for ( int count=0;
	(repeats == 0 || count<repeats ) && softStop() <= 0;
	count++ ) {

    if ( repeats == 0 )
      message( "<b>Measure</b> latencies at <b>" + Str( meanrate, 0, 0, 'f' ) + " Hz</b>: <b>" + Str( count ) + "</b>" );
    else
      message( "<b>Measure</b> latencies at <b>" + Str( meanrate, 0, 0, 'f' ) + " Hz</b>: <b>" + Str( count ) + "</b> from <b>" + Str( repeats ) + "</b>" );

    // stimulus:
    write( signal );
    sleep( ( 5.0 + rnd.uniform() )*period );
    if ( interrupt() ) {
      if ( count == 0 )
	state = Aborted;
      break;
    }

    // analyze latencies:
    const EventData &spikes = events( SpikeEvents[0] );
    int psi = spikes.previous( signalTime() );
    if ( psi+1 >= spikes.size() )
      continue;
    double x = signalTime() - spikes[psi];
    double y = spikes[psi+1] - signalTime();
    latencies.push( 1000.0*x, -1000.0*y );

    // voltage trace:
    voltages.push_back( SampleDataF( meanvoltage.range() ) );
    data.copy( spikes[psi-1], voltages.back() );
    meanvoltage.averageAdd( voltages.back(), count+1, voltagesq, voltagesd );
    if ( voltages.size() > 5 )
      voltages.pop_front();

    // current trace:
    if ( CurrentTrace[0] >= 0 )
      trace( CurrentTrace[0] ).copy( spikes[psi-1], current );

    // plot:
    P.lock();
    P.clear();
    P.setY2Range( Plot::AutoScale, meanvoltage.max( 0.5*period, period ) );
    for ( list< SampleDataF >::const_iterator p = voltages.begin();
	  p != voltages.end();
	  ++p ) {
      P.plot( *p, 1000.0, Plot::Yellow, 2 );
      P.back().setAxis( Plot::X1Y2 );
    }
    if ( count > 4 ) {
      P.plot( meanvoltage+voltagesd, 1000.0, Plot::Orange, 2 );
      P.back().setAxis( Plot::X1Y2 );
      P.plot( meanvoltage-voltagesd, 1000.0, Plot::Orange, 2 );
      P.back().setAxis( Plot::X1Y2 );
    }
    P.plot( meanvoltage, 1000.0, Plot::Red, 4 );
    P.back().setAxis( Plot::X1Y2 );
    P.plot( latencies, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Blue, Plot::Blue );
    MapD cl;
    cl.push( 1000.0*x, -1000.0*y );
    P.plot( cl, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 16, Plot::Cyan, Plot::Cyan );
    P.draw();
    P.unlock();

    // save:
    if ( count == 0 )
      openTraceFile( tf, tracekey, header );
    saveTrace( tf, tracekey, count, voltages.back(), current, x, y );
  }

  if ( state == Completed ) {
    unlockAll();
    tf << '\n';
    saveMeanTrace( header, meanvoltage, voltagesd );
    saveData( header, latencies );
    lockAll();
  }

  // back to initial dc-current:
  directWrite( orgdcsignal );

  return state;
}


void VoltageReconstruction::openTraceFile( ofstream &tf, TableKey &tracekey,
					   const Options &header )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( CurrentTrace[0] >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.3f" );
  tf.open( addPath( "voltagereconstruction-traces.dat" ).c_str(),
	   ofstream::out | ofstream::app );
  header.save( tf, "# " );
  tf << "# status:\n";
  stimulusData().save( tf, "#   " );
  tf << "# settings:\n";
  settings().save( tf, "#   " );
  tf << '\n';
  tracekey.saveKey( tf, true, false );
  tf << '\n';
}


void VoltageReconstruction::saveTrace( ofstream &tf, const TableKey &tracekey,
				       int index,
				       const SampleDataF &voltage,
				       const SampleDataF &current,
				       double x, double y )
{
  tf << "# index: " << index << '\n';
  tf << "# x: " << Str( 1000.0*x, 0, 2, 'f' ) << "ms\n";
  tf << "# y: " << Str( 1000.0*y, 0, 2, 'f' ) << "ms\n";
  if ( ! current.empty() ) {
    for ( int k=0; k<voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.pos( k ), 0 );
      tracekey.save( tf, voltage[k] );
      tracekey.save( tf, current[k] );
      tf << '\n';
    }
  }
  else {
    for ( int k=0; k<voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.pos( k ), 0 );
      tracekey.save( tf, voltage[k] );
      tf << '\n';
    }
  }
  tf << '\n';
}


void VoltageReconstruction::saveMeanTrace( const Options &header,
					   const SampleDataF &voltage,
					   const SampleDataF &voltagesd )
{
  ofstream df( addPath( "voltagereconstruction-meantrace.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "t", "ms", "%6.2f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.addNumber( "s.d.", VUnit, "%6.2f" );
  datakey.saveKey( df );

  for ( int k=0; k<voltage.size(); k++ ) {
    datakey.save( df, 1000.0*voltage.pos( k ), 0 );
    datakey.save( df, voltage[k] );
    datakey.save( df, voltagesd[k] );
    df << '\n';
  }
  
  df << "\n\n";
}


void VoltageReconstruction::saveData( const Options &header,
				      const MapD &latencies )
{
  ofstream df( addPath( "voltagereconstruction-data.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "x", "ms", "%7.2f" );
  datakey.addNumber( "y", "ms", "%7.2f" );
  datakey.saveKey( df );

  for ( int k=0; k<latencies.size(); k++ ) {
    datakey.save( df, latencies.x( k ), 0 );
    datakey.save( df, latencies.y( k ) );
    df << '\n';
  }
  
  df << "\n\n";
}


addRePro( VoltageReconstruction );

}; /* namespace patchclampprojects */

#include "moc_voltagereconstruction.cc"
