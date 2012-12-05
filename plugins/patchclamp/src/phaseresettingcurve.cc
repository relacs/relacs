/*
  patchclampprojects/phaseresettingcurve.h
  Measures phase-resetting curves of spiking neurons.

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
	   "Jan Benda", "1.1", "Dec 14, 2010" ),
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
  addInteger( "minsave", "Minimum number of test pulses required for saving data", 10, 1, 100000, 1 );

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
  int minsave = integer( "minsave" );

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
  orgdcsignal.description().addSection( "", "stimulus/value" );
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
  signal.description().addSection( "", "stimulus/pulse" );
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
  list< SampleDataF > voltage;
  list< SampleDataF > current;
  EventList prcspikes;
  prcspikes.reserve( repeats == 0 ? 1000 : repeats );

  // files:
  ofstream tf;
  TableKey tracekey;
  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "ReProIndex", reproCount() );
  header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  header.addNumber( "firing rate", meanrate, "Hz", "%0.1f" );
  header.addNumber( "period", 1000.0*baseperiod, "ms", "%0.2f" );
  header.addSection( "status" );
  header.append( stimulusData() );
  header.addSection( "settings" );
  header.append( settings() );

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
    voltage.push_back( SampleDataF( spikes[psi-2]-spikes[psi], spikes[psi+2]-spikes[psi], data.stepsize(), 0.0 ) );
    data.copy( spikes[psi], voltage.back() );

    // current trace:
    if ( CurrentTrace[0] >= 0 ) {
      current.push_back( SampleDataF( spikes[psi-2]-spikes[psi], spikes[psi+2]-spikes[psi], data.stepsize(), 0.0 ) );
      trace( CurrentTrace[0] ).copy( spikes[psi], current.back() );
    }

    // spikes:
    prcspikes.push( spikes, spikes[psi-nperiods/2]-baseperiod,
		    spikes[psi+nperiods/2]+baseperiod, signalTime() );

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
	       prcphases.x( j ) < medianprc.pos( k )+0.5*binsize )
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
    if ( prctimes.size() >= minsave ) {
      if ( prctimes.size() == minsave )
	openTraceFile( tf, tracekey, header );
      int inx = prctimes.size()-voltage.size();
      saveTrace( tf, tracekey, inx, voltage, current,
		 periods[inx], meanperiods[inx], perturbedperiods[inx],
		 prctimes.x(inx), prctimes.y(inx),
		 prcphases.x(inx), prcphases.y(inx) );
    }

    lockAll();

  }

  if ( prctimes.size() >= minsave ) {
    unlockAll();
    while ( voltage.size() > 0 ) {
      int inx = prctimes.size()-voltage.size();
      saveTrace( tf, tracekey, inx, voltage, current,
		 periods[inx], meanperiods[inx], perturbedperiods[inx],
		 prctimes.x(inx), prctimes.y(inx),
		 prcphases.x(inx), prcphases.y(inx) );
    }
    tf << '\n';
    saveSpikes( header, prcspikes, periods, meanperiods, perturbedperiods,
		prctimes, prcphases );
    saveData( header, periods, meanperiods, perturbedperiods,
	      prctimes, prcphases );
    savePRC( header, prcphases );
    lockAll();
  }

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
  tf << '\n';
  tracekey.saveKey( tf, true, false );
  tf << '\n';
}


void PhaseResettingCurve::saveTrace( ofstream &tf, TableKey &tracekey,
				     int index,
				     list< SampleDataF > &voltage,
				     list< SampleDataF > &current,
				     double T, double Tmean, double Tpert,
				     double t, double dt, double p, double dp )
{
  tf << "# index: " << index << '\n';
  tf << "# T: " << Str( 1000.0*T, 0, 2, 'f' ) << "ms\n";
  tf << "# Tmean: " << Str( 1000.0*Tmean, 0, 2, 'f' ) << "ms\n";
  tf << "# Tpert: " << Str( 1000.0*Tpert, 0, 2, 'f' ) << "ms\n";
  tf << "# t: " << Str( 1000.0*t, 0, 2, 'f' ) << "ms\n";
  tf << "# dt: " << Str( 1000.0*dt, 0, 2, 'f' ) << "ms\n";
  tf << "# p: " << Str( p, 0, 4, 'f' ) << "\n";
  tf << "# dp: " << Str( dp, 0, 4, 'f' ) << "\n";
  if ( ! current.empty() ) {
    for ( int k=0; k<voltage.front().size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.front().pos( k ), 0 );
      tracekey.save( tf, voltage.front()[k] );
      tracekey.save( tf, current.front()[k] );
      tf << '\n';
    }
    voltage.pop_front();
    current.pop_front();
  }
  else {
    for ( int k=0; k<voltage.front().size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.front().pos( k ), 0 );
      tracekey.save( tf, voltage.front()[k] );
      tf << '\n';
    }
    voltage.pop_front();
  }
  tf << '\n';
}


void PhaseResettingCurve::saveSpikes( const Options &header, const EventList &spikes,
				      const ArrayD &periods, const ArrayD &meanperiods,
				      const ArrayD &perturbedperiods,
				      const MapD &prctimes, const MapD &prcphases )
{
  ofstream df( addPath( "phaseresettingcurve-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << '\n';

  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  for ( int k=0; k<spikes.size(); k++ ) {
    df << "# index: " << k << '\n';
    df << "# T: " << Str( 1000.0*periods[k], 0, 2, 'f' ) << "ms\n";
    df << "# Tmean: " << Str( 1000.0*meanperiods[k], 0, 2, 'f' ) << "ms\n";
    df << "# Tpert: " << Str( 1000.0*perturbedperiods[k], 0, 2, 'f' ) << "ms\n";
    df << "# t: " << Str( 1000.0*prctimes.x(k), 0, 2, 'f' ) << "ms\n";
    df << "# dt: " << Str( 1000.0*prctimes.y(k), 0, 2, 'f' ) << "ms\n";
    df << "# p: " << Str( prcphases.x(k), 0, 4, 'f' ) << "\n";
    df << "# dp: " << Str( prcphases.y(k), 0, 4, 'f' ) << "\n";
    spikes[k].saveText( df, 1000.0, 7, 2, 'f', "-0" );
    df << '\n';
  }
  df << '\n';
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
				   const MapD &prcphases )
{
  // analyze:
  double binsize = 0.2;
  if ( prcphases.size() > 100 )
    binsize = 0.05;
  else if ( prcphases.size() > 50 )
    binsize = 0.1;
  SampleDataD medianprc( 0.5*binsize, 1.0, binsize, 0.0 );
  ArrayD lqprc( medianprc.size(), 0.0 );
  ArrayD uqprc( medianprc.size(), 0.0 );
  ArrayD minprc( medianprc.size(), 0.0 );
  ArrayD maxprc( medianprc.size(), 0.0 );
  ArrayD meanprc( medianprc.size(), 0.0 );
  ArrayD stdevprc( medianprc.size(), 0.0 );
  ArrayD nprc( medianprc.size(), 0.0 );
  for ( int k=0; k<medianprc.size(); k++ ) {
    ArrayD m;
    m.reserve( prcphases.size() );
    for ( int j=0; j<prcphases.size(); j++ ) {
      if ( prcphases.x( j ) >= medianprc.pos( k )-0.5*binsize &&
	   prcphases.x( j ) < medianprc.pos( k )+0.5*binsize )
	m.push( prcphases.y( j ) );
    }
    if ( m.size() > 0 ) {
      sort( m.begin(), m.end() );
      medianprc[k] = median( m );
      lqprc[k] = quantile( 0.25, m );
      uqprc[k] = quantile( 0.75, m );
      minprc[k] = m.front();
      maxprc[k] = m.back();
      meanprc[k] = meanStdev( stdevprc[k], m );
      nprc[k] = m.size();
    }
  }

  // save:
  ofstream df( addPath( "phaseresettingcurve-prc.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << '\n';

  TableKey datakey;
  datakey.addSection( "p" );
  datakey.addNumber( "bin", "1", "%7.4f" );
  datakey.addSection( "dp" );
  datakey.addNumber( "median", "1", "%7.4f" );
  datakey.addNumber( "1.quartile", "1", "%7.4f" );
  datakey.addNumber( "3.quartile", "1", "%7.4f" );
  datakey.addNumber( "min", "1", "%7.4f" );
  datakey.addNumber( "max", "1", "%7.4f" );
  datakey.addNumber( "mean", "1", "%7.4f" );
  datakey.addNumber( "s.d.", "1", "%7.4f" );
  datakey.addNumber( "n", "1", "%5.0f" );
  datakey.saveKey( df );

  for ( int k=0; k<medianprc.size(); k++ ) {
    datakey.save( df, medianprc.pos( k ), 0 );
    datakey.save( df, medianprc[k] );
    datakey.save( df, lqprc[k] );
    datakey.save( df, uqprc[k] );
    datakey.save( df, minprc[k] );
    datakey.save( df, maxprc[k] );
    datakey.save( df, meanprc[k] );
    datakey.save( df, stdevprc[k] );
    datakey.save( df, nprc[k] );
    df << '\n';
  }
  
  df << "\n\n";
}


addRePro( PhaseResettingCurve, patchclamp );

}; /* namespace patchclamp */

#include "moc_phaseresettingcurve.cc"
