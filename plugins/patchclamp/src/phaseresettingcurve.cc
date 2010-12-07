/*
  patchclampprojects/phaseresettingcurve.h
  Measures phase-resetting curves of spiking neurons.

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

#include <algorithm>
#include <relacs/tablekey.h>
#include <relacs/map.h>
#include <relacs/sampledata.h>
#include <relacs/random.h>
#include <relacs/patchclamp/phaseresettingcurve.h>
using namespace relacs;

namespace patchclamp {


PhaseResettingCurve::PhaseResettingCurve( void )
  : RePro( "PhaseResettingCurve", "patchclamp",
	   "Jan Benda", "1.0", "Nov 25, 2010" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    IInFac( 1.0 )
{
  // add some options:
  addSelection( "dcamplitudesrc", "Set dc-current to", "DC|custom|DC" );
  addNumber( "dcamplitude", "Amplitude of dc-current", 0.0, 0.0, 1000.0, 0.01 ).setActivation( "dcamplitudesrc", "custom" );
  addNumber( "amplitude", "Test-pulse amplitude", 0.1, -10000.0, 10000.0, 0.01 );
  addNumber( "duration", "Duration of test-pulse", 0.001, 0.0, 1000.0, 0.001, "seconds", "ms" );
  addInteger( "nperiods", "Number of ISIs between test-pulses", 5, 0, 1000000 );
  addInteger( "repeats", "Number of test-pulses", 100, 0, 1000000 );
  addNumber( "rateduration", "Time for initial estimate of firing rate", 1.0, 0.0, 100000.0, 0.01, "seconds", "ms" );
  addInteger( "averageisis", "Average ISI over", 10, 1, 100000, 1, "test-pulses" );

  PrevDCAmplitude = 0.0;

  P.lock();
  P.setLMarg( 7.0 );
  P.setRMarg( 2.0 );
  P.setTMarg( 1.0 );
  P.setBMarg( 5.0 );
  P.setXLabel( "Phase" );
  P.setXRange( 0.0, 1.0 );
  P.setYLabel( "Phase response" );
  P.setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		  Plot::Center, -90.0 );
  P.setYRange( Plot::AutoScale, Plot::AutoScale );    
  P.unlock();
  setWidget( &P );
}


void PhaseResettingCurve::config( void )
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


int PhaseResettingCurve::main( void )
{
  // get options:
  int dcamplitudesrc = index( "dcamplitudesrc" );
  double dcamplitude = number( "dcamplitude" );
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  int nperiods = integer( "nperiods" );
  int repeats = integer( "repeats" );
  double rateduration = number( "rateduration" );
  int averageisis = integer( "averageisis" );

  double orgdcamplitude = stimulusData().number( outTraceName( 0 ) );
  if ( dcamplitudesrc == 1 ) // dc
    dcamplitude = orgdcamplitude;

  if ( SpikeTrace[ 0 ] < 0 || SpikeEvents[ 0 ] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }

  // setup plots:
  P.lock();
  P.clear();
  P.resetRanges();
  P.draw();
  P.unlock();

  // original dc stimulus:
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
  double baseperiod = 1.0/meanrate;
  
  // plot trace:
  tracePlotSignal( 2.0*nperiods*baseperiod, nperiods*baseperiod );

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
  const EventData &spikes = events( SpikeEvents[0] );
  double meanperiod = baseperiod;
  int n = repeats>0?repeats:10000;
  ArrayD periods;
  periods.reserve( n );
  ArrayD meanperiods;
  meanperiods.reserve( n );
  ArrayD perturbedperiods;
  perturbedperiods.reserve( n );
  MapD prctimes;
  prctimes.reserve( n );
  MapD prcphases;
  prcphases.reserve( n );
  SampleDataD medianprc;
  SampleDataF voltage( -2.5*baseperiod, 2.5*baseperiod, data.stepsize(), 0.0 );
  SampleDataF current;
  if ( CurrentTrace[0] >= 0 ) {
    current.resize( -2.5*baseperiod, 2.5*baseperiod,
		    trace( CurrentTrace[0] ).stepsize() );
  }

  // files:
  ofstream tf;
  TableKey tracekey;
  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "ReProIndex", reproCount() );
  header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  header.addNumber( "firing rate", meanrate, "Hz", "%0.1f" );
  header.addNumber( "period", 1000.0*baseperiod, "ms", "%0.2f" );
  openTraceFile( tf, tracekey, header );

  for ( int n=1; (repeats == 0 || n<=repeats ) && softStop() <= 0; n++ ) {

    if ( repeats == 0 )
      message( "<b>Measure</b> phase resetting at <b>" + Str( meanrate, 0, 0, 'f' ) + " Hz</b>: <b>" + Str( n ) + "</b>" );
    else
      message( "<b>Measure</b> phase resetting at <b>" + Str( meanrate, 0, 0, 'f' ) + " Hz</b>: <b>" + Str( n ) + "</b> from <b>" + Str( repeats ) + "</b>" );

    // stimulus:
    write( signal );
    sleep( ( nperiods + rnd.uniform() )*baseperiod );
    while ( spikes.count( signalTime(), currentTime() ) < nperiods &&
	    ! interrupt() )
      sleep( 0.5*nperiods*baseperiod );
    if ( interrupt() )
      break;

    // analyze:
    int psi = spikes.previous( signalTime() );
    if ( psi-nperiods/2 < 0 || psi+1 >= spikes.size() )
      continue;
    double period = (spikes[psi] - spikes[psi-nperiods/2])/(nperiods/2);
    periods.push( period );
    if ( period > 0.5*meanperiod && period < 1.5*meanperiod )
      meanperiod += (period - meanperiod)/double(averageisis);
    meanperiods.push( meanperiod );
    perturbedperiods.push( spikes[psi+1] - spikes[psi] );
    double t = signalTime() - spikes[psi];
    double dt = spikes[psi] + meanperiod - spikes[psi+1];
    prctimes.push( t, dt );
    double p = t/meanperiod;
    double dp = dt/meanperiod;
    prcphases.push( p, dp );

    // voltage trace:
    data.copy( spikes[psi-1], voltage );

    // current trace:
    if ( CurrentTrace[0] >= 0 )
      trace( CurrentTrace[0] ).copy( spikes[psi-1], current );

    unlockAll();

    // recalculate phase resetting:
    if ( n%10 == 0 || n == repeats ) {
      for ( int k=perturbedperiods.size()>3*averageisis?
	      perturbedperiods.size()-3*averageisis:0;
	    k<perturbedperiods.size();
	    k++ ) {
	prctimes.y(k) = meanperiod - perturbedperiods[k];
	prcphases.x(k) = prctimes.x(k)/meanperiod;
	prcphases.y(k) = prctimes.y(k)/meanperiod;
      }
    }
    // median phase resetting:
    if ( prcphases.size() >= 20 ) {
      double binsize = 0.2;
      if ( prcphases.size() > 100 )
	binsize = 0.05;
      else if ( prcphases.size() > 50 )
	binsize = 0.1;
      medianprc = SampleDataD( 0.5*binsize, 1.0, binsize, 0.0 );
      for ( int k=0; k<medianprc.size(); k++ ) {
	ArrayD m;
	m.reserve( prcphases.size() );
	for ( int j=0; j<prcphases.size(); j++ ) {
	  if ( prcphases.x( j ) >= medianprc.pos( k )-0.5*binsize &&
	       prcphases.x( j ) < medianprc.pos( k )+0.5*binsize &&
	       prcphases.y( j ) < prcphases.x( j ) )
	    m.push( prcphases.y( j ) );
	}
	sort( m.begin(), m.end() );
	medianprc[k] = median( m );
      }
    }
    else
      medianprc.clear();

    // plot:
    P.lock();
    P.clear();
    if ( ! P.zoomedYRange() ) {
      double min=0.0, max=0.0;
      minMax( min, max, prcphases.y() );
      P.setYRange( min, max );
    }
    P.plotHLine( 0.0, Plot::White, 2 );
    SampleDataD line;
    line.line( 0.0, 1.0, 0.01, 1.0, -1.0 );
    P.plot( line, 1.0, Plot::Yellow, 2 );
    P.plot( prcphases, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Blue, Plot::Blue );
    MapD cp;
    cp.push( p, dp );
    P.plot( cp, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 16, Plot::Cyan, Plot::Cyan );
    if ( medianprc.size() > 0 )
      P.plot( medianprc, 1.0, Plot::Red, 2, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
    P.draw();
    P.unlock();

    // save:
    saveTrace( tf, tracekey, n, voltage, current, period, t, dt, p, dp );

    lockAll();

  }

  tf << '\n';
  saveData( header, periods, meanperiods, perturbedperiods,
	    prctimes, prcphases );
  if ( medianprc.size() > 0 )
    savePRC( header, medianprc );

  // back to initial dc-current:
  directWrite( orgdcsignal );

  return Completed;
}


void PhaseResettingCurve::openTraceFile( ofstream &tf, TableKey &tracekey,
					 const Options &header )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( CurrentTrace[0] >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.3f" );
  tf.open( addPath( "phaseresettingcurve-traces.dat" ).c_str(),
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


void PhaseResettingCurve::saveTrace( ofstream &tf, TableKey &tracekey,
				     int index,
				     const SampleDataF &voltage,
				     const SampleDataF &current, double T,
				     double t, double dt, double p, double dp )
{
  tf << "# index: " << index << '\n';
  tf << "# T: " << Str( 1000.0*T, 0, 2, 'f' ) << "ms\n";
  tf << "# t: " << Str( 1000.0*t, 0, 2, 'f' ) << "ms\n";
  tf << "# dt: " << Str( 1000.0*dt, 0, 2, 'f' ) << "ms\n";
  tf << "# p: " << Str( p, 0, 3, 'f' ) << "\n";
  tf << "# dp: " << Str( dp, 0, 3, 'f' ) << "\n";
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


void PhaseResettingCurve::saveData( const Options &header,
				    const ArrayD &periods,
				    const ArrayD &meanperiods,
				    const ArrayD &perturbedperiods,
				    const MapD &prctimes,
				    const MapD &prcphases )
{
  ofstream df( addPath( "phaseresettingcurve-data.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "T", "ms", "%7.2f" );
  datakey.addNumber( "Tmean", "ms", "%7.2f" );
  datakey.addNumber( "Tpert", "ms", "%7.2f" );
  datakey.addNumber( "t", "ms", "%7.2f" );
  datakey.addNumber( "dt", "ms", "%7.2f" );
  datakey.addNumber( "p", "1", "%7.4f" );
  datakey.addNumber( "dp", "1", "%7.4f" );
  datakey.saveKey( df );

  for ( int k=0; k<prctimes.size(); k++ ) {
    datakey.save( df, 1000.0*periods[k], 0 );
    datakey.save( df, 1000.0*meanperiods[k] );
    datakey.save( df, 1000.0*perturbedperiods[k] );
    datakey.save( df, 1000.0*prctimes.x( k ) );
    datakey.save( df, 1000.0*prctimes.y( k ) );
    datakey.save( df, prcphases.x( k ) );
    datakey.save( df, prcphases.y( k ) );
    df << '\n';
  }
  
  df << "\n\n";
}


void PhaseResettingCurve::savePRC( const Options &header,
				   const SampleDataD &medianprc )
{
  ofstream df( addPath( "phaseresettingcurve-prc.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "p", "1", "%7.4f" );
  datakey.addNumber( "dp", "1", "%7.4f" );
  datakey.saveKey( df );

  for ( int k=0; k<medianprc.size(); k++ ) {
    datakey.save( df, medianprc.pos( k ), 0 );
    datakey.save( df, medianprc[k] );
    df << '\n';
  }
  
  df << "\n\n";
}


addRePro( PhaseResettingCurve );

}; /* namespace patchclamp */

#include "moc_phaseresettingcurve.cc"
