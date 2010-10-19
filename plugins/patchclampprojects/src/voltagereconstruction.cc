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
#include <relacs/optwidget.h>
#include <relacs/tablekey.h>
#include <relacs/map.h>
#include <relacs/sampledata.h>
#include <relacs/random.h>
#include <relacs/patchclampprojects/voltagereconstruction.h>
using namespace relacs;

namespace patchclampprojects {


VoltageReconstruction::VoltageReconstruction( void )
  : RePro( "VoltageReconstruction", "patchclampprojects", "JanBenda, Ales Skorjanc", "1.0", "Oct 18, 2010" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    IInFac( 1.0 )
{
  // add some options:
  addLabel( "Pulse-stimulus" ).setStyle( OptWidget::TabLabel );
  addSelection( "startamplitudesrc", "Set initial dc-current to", "custom|DC|threshold|previous" );
  addNumber( "startamplitude", "Initial amplitude of dc-current", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "startamplitudesrc", "custom" );
  addNumber( "amplitude", "Test-pulse amplitude", 0.1, 0.0, 1000.0, 0.01 );
  addNumber( "duration", "Duration of test-pulse", 0.0, 0.0, 1000.0, 0.001, "seconds", "ms" );
  addInteger( "repeats", "Number of test-pulses", 100, 0, 1000000 );
  addLabel( "Rate - search" ).setStyle( OptWidget::TabLabel );
  addBoolean( "userate", "Search dc-current for target firing rate", false );
  addNumber( "rate", "Target firing rate", 100.0, 0.0, 1000.0, 10.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "ratetol", "Tolerance for target firing rate", 5.0, 0.0, 1000.0, 1.0, "Hz" ).setActivation( "userate", "true" );
  addNumber( "amplitudestep", "Initial size of dc-current steps used for searching target rate", 8.0, 0.0, 100.0, 1.0, IUnit ).setActivation( "userate", "true" );
  addNumber( "searchduration", "Duration of dc-current stimulus", 0.0, 0.0, 1000.0, 0.01, "seconds", "ms" ).setActivation( "userate", "true" );
  addNumber( "skipwin", "Initial portion of stimulus not used for analysis", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" ).setActivation( "userate", "true" );

  PrevDCAmplitude = 0.0;

  // setup plots:
  Stack = new QStackedLayout;
  setLayout( Stack );

  SP.lock();
  SP.setLMarg( 7.0 );
  SP.setRMarg( 1.5 );
  SP.setTMarg( 3.0 );
  SP.setBMarg( 5.0 );
  SP.setYLabel( "Firing rate [Hz]" );
  SP.setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		   Plot::Center, -90.0 );
  SP.unlock();
  Stack->addWidget( &SP );

  P.lock();
  P.setLMarg( 7.0 );
  P.setRMarg( 7.0 );
  P.setTMarg( 3.0 );
  P.setBMarg( 5.0 );
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "Latency [ms]" );
  P.setY2Label( "Membrane voltage [" + VUnit + "]" );
  P.setY2Tics();
  P.setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		  Plot::Center, -90.0 );
  P.unlock();
  Stack->addWidget( &P );
  Stack->setCurrentWidget( &P );
}


void VoltageReconstruction::config( void )
{
  if ( SpikeTrace[0] >= 0 )
    VUnit = trace( SpikeTrace[0] ).unit();
  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "startamplitude", IUnit );
    setUnit( "amplitude", IUnit );
    setUnit( "amplitudestep", IUnit );
  }
  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int VoltageReconstruction::main( void )
{
  // get options:
  int startamplitudesrc = index( "startamplitudesrc" );
  double startamplitude = number( "startamplitude" );
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  int repeats = number( "repeats" );
  bool userate = boolean( "userate" );
  double targetrate = number( "rate" );
  double ratetolerance = number( "ratetol" );
  double amplitudestep = number( "amplitudestep" );
  double searchduration = number( "searchduration" );
  double skipwin = number( "skipwin" );

  double orgdcamplitude = stimulusData().number( outTraceName( 0 ) );
  double dcamplitude = startamplitude;
  if ( startamplitudesrc == 1 ) // dc
    dcamplitude = orgdcamplitude;
  else if ( startamplitudesrc == 2 ) {  // thresh
    dcamplitude = metaData( "Cell" ).number( "ithreshss" );
    if ( dcamplitude == 0.0 )
      dcamplitude = metaData( "Cell" ).number( "ithreshon" );
  }
  else if ( startamplitudesrc == 3 )  // prev
    dcamplitude = PrevDCAmplitude;

  if ( SpikeTrace[ 0 ] < 0 || SpikeEvents[ 0 ] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }
  if ( userate && searchduration < 5.0/targetrate ) {
    warning( "Searchduration too small for requested targetrate!" );
    return Failed;
  }
  if ( userate && skipwin > 0.5*searchduration ) {
    warning( "Skipwin too large compared to searchduration!" );
    return Failed;
  }
  if ( userate && ratetolerance > 0.3*targetrate ) {
    warning( "Ratetolerance certainly too high!" );
    return Failed;
  }
  double silentrate = 1.0/(searchduration-skipwin);
  double minamplitudestep = 0.001;

  OutData orgdcsignal( orgdcamplitude );
  orgdcsignal.setTrace( CurrentOutput[0] );
  orgdcsignal.setIdent( "DC=" + Str( orgdcamplitude ) + IUnit );
  orgdcsignal.addDescription( "stimulus/value" );
  orgdcsignal.description().addNumber( "Intensity", orgdcamplitude, IUnit );

  // search for offset that evokes the target firing rate:
  if ( userate ) {
    
    // plot trace:
    plotToggle( true, true, searchduration, 0.0 );
    
    postCustomEvent( 12 );
    SP.lock();
    SP.setTitle( "Search target firing rate " + Str( targetrate ) + " Hz" );
    SP.setXLabel( "DC Current [" + IUnit + "]" );
    SP.draw();
    SP.unlock();

    MapD rates;
    rates.reserve( 20 );

    // stimulus:
    OutData signal( dcamplitude );
    signal.setTrace( CurrentOutput[0] );
    signal.setIdent( "DC=" + Str( dcamplitude ) + IUnit );
    signal.addDescription( "stimulus/value" );
    signal.description().addNumber( "Intensity", dcamplitude, IUnit );
    
    double minampl = dcamplitude;
    double maxampl = dcamplitude;
    double maxf = 50.0;

    // search dc-curent amplitude:
    while ( true ) {

      signal = dcamplitude;
      signal.setIdent( "DC=" + Str( dcamplitude ) + IUnit );
      signal.description().setNumber( "Intensity", dcamplitude, IUnit );
	
      // message:
      Str s = "<b>Search</b> rate <b>" + Str( targetrate ) + " Hz</b>";
      s += ":  DC = <b>" + Str( dcamplitude, 0, 0, 'f' ) + " " + IUnit + "</b>";
      message( s );

      // output:
      directWrite( signal );
      if ( ! signal.success() )
	break;
      sleep( searchduration );
      if ( interrupt() ) {
	directWrite( orgdcsignal );
	return Aborted;
      }

      // analyze:
      double meanrate = events( SpikeEvents[0] ).rate( signalTime() + skipwin,
						       signalTime() + searchduration );
	
      // plot:
      {
	SP.lock();
	// firing rate versus stimulus offset:
	SP.clear();
	double mina = minampl;
	double maxa = maxampl;
	if ( maxa - mina < amplitudestep ) {
	  mina -= 0.5*amplitudestep;
	  maxa += 0.5*amplitudestep;
	}
	if ( ! SP.zoomedXRange() )
	  SP.setXRange( mina, maxa );
	if ( meanrate+10.0 > maxf )
	  maxf = ::ceil((meanrate+10.0)/10.0)*10.0;
	if ( maxf < targetrate ) 
	  maxf = targetrate;
	if ( ! SP.zoomedYRange() )
	  SP.setYRange( 0.0, maxf );
	SP.plotHLine( targetrate, Plot::White, 2 );
	SP.plot( rates, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Red, Plot::Red );
	MapD cr;
	cr.push( dcamplitude, meanrate );
	SP.plot( cr, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Yellow, Plot::Yellow );
	SP.draw();
	SP.unlock();
      }

      int rinx = 0;
      if ( signal.success() )
	rinx = rates.insert( dcamplitude, meanrate );

      if ( softStop() > 0 )
	return Aborted;

      // new offset:
      if ( signal.success() && 
	   fabs( meanrate - targetrate ) < ratetolerance ) {
	// ready:
	break;
      }
      else {
	// cell lost?
	if ( signal.success() && meanrate < silentrate &&
	     rinx > 0 && rates.y( rinx-1 ) > 2.0*silentrate ) {
	  if ( ( rinx < rates.size()-1 && 
		 rates.y( rinx+1 ) > 2.0*silentrate ) ||
	       rates.y( rinx-1 ) > silentrate + 0.3*(targetrate - silentrate ) ) {
	    info( "Cell probably lost!" );
	    directWrite( orgdcsignal );
	    return Failed;
	  }
	}
	if ( ( signal.success() && meanrate < targetrate ) || signal.underflow() ) {
	  double maxr = 0.0;
	  if ( rates.y().maxIndex( maxr ) < rinx && maxr > 2.0*silentrate ) {
	    // depolarization block reached:
	    dcamplitude -= amplitudestep;
	    if ( dcamplitude < minampl )
	      minampl = dcamplitude;
	  }
	  else {
	    // rate below target rate:
	    if ( dcamplitude < maxampl )
	      amplitudestep *= 0.5;
	    dcamplitude += amplitudestep;
	    if ( dcamplitude > maxampl )
	      maxampl = dcamplitude;
	  }
	}
	else if ( ( signal.success() && meanrate > targetrate ) || signal.overflow() ) {
	  // overflow:
	  if ( rates.size() > 0 && dcamplitude > rates.x().back() &&
	       amplitudestep < minamplitudestep ) {
	    info( "dcamplitude needed for targetrate is too high!" );
	    directWrite( orgdcsignal );
	    return Failed;
	  }
	  // rate above target rate:
	  if ( dcamplitude > minampl )
	    amplitudestep *= 0.5;
	  dcamplitude -= amplitudestep;
	  if ( dcamplitude < minampl )
	    minampl = dcamplitude;
	}
	else if ( signal.failed() ) {
	  warning( "Output of stimulus failed!<br>Signal error: <b>" +
		   signal.errorText() + "</b><br>Exit now!" );
	  directWrite( orgdcsignal );
	  return Failed;
	}
	else {
	  warning( "Could not establish firing rate!" );
	  directWrite( orgdcsignal );
	  return Failed;
	}
      }

    }

  }

  // remember baseline current:
  PrevDCAmplitude = dcamplitude;

  // measure firing rate:
  for ( int n=0; ; n++ ) {
    message( "<b>Measure</b> mean firing rate " + Str( '.', n ) );
    sleep( 1.0 );
    if ( n*1.0 >= 3.0*searchduration )
      break;
  }
  if ( interrupt() ) {
    directWrite( orgdcsignal );
    return Aborted;
  }
  double meanrate = events( SpikeEvents[0] ).rate( currentTime() - 3.0*searchduration,
						   currentTime() );
  if ( meanrate < 0.01 ) {
    warning( "Not enough spikes evoked by dc current!" );
    directWrite( orgdcsignal );
    return Failed;
  }
  double period = 1.0/meanrate;
  
  // plot trace:
  plotToggle( true, false, searchduration, 0.0 );

  // setup plots:
  postCustomEvent( 11 );
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
  latencies.reserve( 10000 );
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
  openTraceFile( tf, tracekey, header );

  for ( int n=0; (repeats == 0 || n<repeats ) && softStop() <= 0; n++ ) {

    if ( repeats == 0 )
      message( "<b>Measure</b> latencies at <b>" + Str( meanrate, 0, 0, 'f' ) + " Hz</b>: <b>" + Str( n ) + "</b>" );
    else
      message( "<b>Measure</b> latencies at <b>" + Str( meanrate, 0, 0, 'f' ) + " Hz</b>: <b>" + Str( n ) + "</b> from <b>" + Str( repeats ) + "</b>" );

    // stimulus:
    write( signal );
    sleep( ( 5.0 + rnd.uniform() )*period );
    if ( interrupt() )
      break;

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
    meanvoltage.averageAdd( voltages.back(), n+1, voltagesq, voltagesd );
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
    if ( n > 4 ) {
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
    saveTrace( tf, tracekey, n, voltages.back(), current, x, y );
  }

  tf << '\n';
  saveMeanTrace( header, meanvoltage, voltagesd );
  saveData( header, latencies );

  // back to initial dc-current:
  directWrite( orgdcsignal );

  return Completed;
}


void VoltageReconstruction::openTraceFile( ofstream &tf, TableKey &tracekey,
					   const Options &header )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( CurrentTrace[0] >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.3f" );
  if ( completeRuns() <= 0 )
    tf.open( addPath( "voltagereconstruction-traces.dat" ).c_str() );
  else
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


void VoltageReconstruction::saveTrace( ofstream &tf, TableKey &tracekey,
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


void VoltageReconstruction::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    Stack->setCurrentWidget( &P );
    break;
  }
  case 12: {
    Stack->setCurrentWidget( &SP );
    break;
  }
  default:
    RELACSPlugin::customEvent( qce );
  }
}


addRePro( VoltageReconstruction );

}; /* namespace patchclampprojects */

#include "moc_voltagereconstruction.cc"
