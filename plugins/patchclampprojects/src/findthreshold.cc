/*
  patchclampprojects/findthreshold.cc
  Finds the current threshold.

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

#include <fstream>
#include <relacs/tablekey.h>
#include <relacs/patchclampprojects/findthreshold.h>
using namespace relacs;

namespace patchclampprojects {


FindThreshold::FindThreshold( void )
  : RePro( "FindThreshold", "patchclampprojects", "Jan Benda", "1.0", "Feb 08, 2010" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    IInFac( 1.0 )
{
  // add some options:
  addLabel( "Stimuli" );
  addSelection( "amplitudesrc", "Use initial amplitude from", "custom|DC|threshold" );
  addNumber( "startamplitude", "Initial amplitude of current stimulus", 0.0, 0.0, 1000.0, 0.01 ).setActivation( "amplitudesrc", "custom" );
  addNumber( "startamplitudestep", "Initial size of amplitude steps used for searching threshold", 0.1, 0.0, 1000.0, 0.001 );
  addNumber( "amplitudestep", "Final size of amplitude steps used for oscillating around threshold", 0.01, 0.0, 1000.0, 0.001 );
  addNumber( "minspikecount", "Minimum required spike count for each trial", 1.0, 0.0, 10000.0, 1.0 );
  addBoolean( "resetcurrent", "Reset current to zero after each stimulus", false );
  addLabel( "Timing" );
  addSelection( "durationsel", "Set duration of stimulus", "in milliseconds|as multiples of membrane time constant" );
  addNumber( "duration", "Duration of stimulus", 0.1, 0.0, 1000.0, 0.001, "sec", "ms" ).setActivation( "durationsel", "in milliseconds" );
  addNumber( "durationfac", "Duration of stimulus", 1.0, 0.0, 1000.0, 0.1, "tau_m" ).setActivation( "durationsel", "as multiples of membrane time constant" );
  addNumber( "searchpause", "Duration of pause between outputs during search", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "pause", "Duration of pause between outputs", 1.0, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "savetime", "Length of trace to be saved and analyzed", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "skiptime", "Initial time skipped from spike-count analysis", 0.0, 0.0, 1000.0, 0.01, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 );
  addTypeStyle( OptWidget::TabLabel, Parameter::Label );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
  setWidget( &P );
}


void FindThreshold::config( void )
{
  if ( SpikeTrace[0] >= 0 )
    VUnit = trace( SpikeTrace[0] ).unit();
  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "startamplitude", IUnit );
    setUnit( "startamplitudestep", IUnit );
    setUnit( "amplitudestep", IUnit );
  }
  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int FindThreshold::main( void )
{
  int durationsel = index( "durationsel" );
  double duration = number( "duration" );
  double durationfac = number( "durationfac" );
  double searchpause = number( "searchpause" );
  double measurepause = number( "pause" );
  double savetime = number( "savetime" );
  double skiptime = number( "skiptime" );
  int repeats = integer( "repeats" );
  int amplitudesrc = index( "amplitudesrc" );
  double amplitude = number( "startamplitude" );
  double amplitudestep = number( "startamplitudestep" );
  double finalamplitudestep = number( "amplitudestep" );
  double minspikecount = number( "minspikecount" );
  double orgdcamplitude = stimulusData().number( outTraceName( CurrentOutput[0] ) );
  bool resetcurrent = boolean( "resetcurrent" );
  if ( amplitudesrc == 1 )
    amplitude = orgdcamplitude;
  else if ( amplitudesrc == 2 ) {
    amplitude = metaData( "Cell" ).number( "ithreshss" );
    if ( amplitude == 0.0 )
      amplitude = metaData( "Cell" ).number( "ithreshon" );
  }

  if ( amplitudestep < finalamplitudestep ) {
    warning( "startamplitudestep must be larger than amplitudestep!" );
    return Failed;
  }

  if ( SpikeTrace[0] < 0 || SpikeEvents[0] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }
  if ( CurrentOutput[0] < 0 ) {
    warning( "Invalid output current trace!" );
    return Failed;
  }
  double membranetau = metaData( "Cell" ).number( "taum" );
  if ( durationsel == 1 ) {
    if ( membranetau <= 0.0 ) {
      warning( "Membrane time constant was not measured yet!" );
      return Failed;
    }
    duration = durationfac*membranetau;
  }
  if ( savetime <= 0.0 || savetime > duration+searchpause )
    savetime = duration+searchpause;
  if ( skiptime > duration || skiptime > savetime ) {
    warning( "skiptime too long!" );
    return Failed;
  }
  double pause = searchpause;

  double samplerate = trace( SpikeTrace[0] ).sampleRate();

  // don't print repro message:
  noMessage();

  // init:
  bool record = false;
  bool search = true;
  DoneState state = Completed;
  Results.clear();
  Amplitudes.clear();
  Amplitudes.reserve( 100 );
  Spikes.clear();
  Spikes.reserve( repeats > 0 ? repeats : 100 );
  SpikeCount = 0;
  TrialCount = 0;
  TableKey tracekey;
  ofstream tf;

  // header:
  Header.clear();
  Header.addInteger( "index", completeRuns() );
  Header.addInteger( "ReProIndex", reproCount() );
  Header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  Header.addNumber( "duration", 1000.0*duration, "ms", "%0.1f" );

  // plot trace:
  tracePlotSignal( 0.5*duration+savetime, 0.5*duration );

  // plot:
  P.lock();
  P.setXRange( 0.0, 1000.0*savetime );
  P.setYLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]" );
  P.unlock();

  // signal:
  OutData signal( duration, 1.0/samplerate );
  signal.setTrace( CurrentOutput[0] );
  signal.addDescription( "stimulus/pulse" );
  signal.description().addNumber( "Intensity", 0.0, IUnit );
  if ( resetcurrent )
    signal.description().addNumber( "IntensityOffs", 0.0, IUnit );
  signal.description().addNumber( "Duration", 1000.0*duration, "ms" );

  // dc signal:
  OutData dcsignal( orgdcamplitude );
  dcsignal.setTrace( CurrentOutput[0] );
  dcsignal.setIdent( "DC=" + Str( orgdcamplitude ) + IUnit );
  dcsignal.addDescription( "stimulus/value" );
  dcsignal.description().addNumber( "Intensity", orgdcamplitude, IUnit );

  // write stimulus:
  sleep( pause );
  if ( interrupt() )
    return Aborted;
  for ( int count=1; softStop() == 0; count++ ) {

    timeStamp();

    Str s;
    if ( ! record )
      s = "<b>Search threshold: </b>";
    else
      s = "<b>Measure threshold: </b>";
    s += "Amplitude <b>" + Str( amplitude ) + " " + IUnit +"</b>, ";
    s += "Step <b>" + Str( amplitudestep ) + " " + IUnit +"</b>";
    if ( record )
      s += ",  Loop <b>" + Str( count ) + "</b>";
    message( s );

    // signal:
    signal = amplitude;
    signal.setIdent( "I=" + Str( amplitude ) + IUnit );
    signal.description().setNumber( "Intensity", amplitude, IUnit );
    if ( resetcurrent )
      signal.back() = 0.0;
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      if ( ! record || count <= 1 )
	state = Failed;
      directWrite( dcsignal );
      break;
    }

    // sleep:
    sleep( duration + 0.5*pause );
    if ( interrupt() ) {
      if ( ! record || count <= 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }

    // analyze, plot, and save:
    analyze( amplitude, duration, savetime, skiptime );
    if ( record )
      saveTrace( tf, tracekey, count-1 );
    plot( record, duration );

    // change stimulus amplitude:
    if ( Results.back().SpikeCount >= minspikecount )
      amplitude -= amplitudestep;
    else
      amplitude += amplitudestep;
    if ( fabs( amplitude ) < 1.0e-8 )
      amplitude = 0.0;

    // switch modes:
    if ( ! record ) {
      // find threshold:
      if ( Results.size() > 1 &&
	   ( ( Results[Results.size()-2].SpikeCount < minspikecount &&
	       Results[Results.size()-1].SpikeCount >= minspikecount ) ||
	     ( Results[Results.size()-2].SpikeCount >= minspikecount &&
	       Results[Results.size()-1].SpikeCount < minspikecount ) ) ) {
	if ( amplitudestep <= finalamplitudestep ) {
	  amplitudestep = finalamplitudestep;
	  pause = measurepause;
	  count = 0;
	  Results.clear();
	  SpikeCount = 0;
	  TrialCount = 0;
	  Amplitudes.clear();
	  Amplitudes.reserve( repeats > 0 ? repeats : 1000 );
	  Latencies.clear();
	  Latencies.reserve( repeats > 0 ? repeats : 1000 );
	  Spikes.clear();
	  Spikes.reserve( repeats > 0 ? repeats : 100 );
	  if ( search )
	    search = false;
	  else {
	    record = true;
	    openFiles( tf, tracekey );
	  }
	}
	else {
	  amplitudestep *= 0.5;
	  amplitudestep = ceil(amplitudestep/finalamplitudestep)*finalamplitudestep;
	}
      }
    }
    TrialCount = count;

    sleepOn( duration + pause );

    if ( record && repeats > 0 && count >= repeats )
      break;

  }

  tf << '\n';
  if ( record && TrialCount > 0 )
    save();
  Results.clear();
  Latencies.clear();
  Amplitudes.clear();
  Spikes.clear();
  return state;
}


void FindThreshold::analyze( double amplitude, double duration,
			     double savetime, double skiptime )
{
  if ( Results.size() >= 20 )
    Results.pop_front();

  if ( CurrentTrace[0] >= 0 ) {
    Results.push_back( Data( savetime, trace( SpikeTrace[0] ),
			     trace( CurrentTrace[0] ) ) );
    Results.back().Current *= IInFac;
  }
  else
    Results.push_back( Data( savetime, trace( SpikeTrace[0] ) ) );
  Results.back().Amplitude = amplitude;
  events( SpikeEvents[0] ).copy( signalTime(), signalTime() + savetime,
				 signalTime(), Results.back().Spikes );
  Results.back().SpikeCount = Results.back().Spikes.count( skiptime, savetime );

  if ( Results.back().SpikeCount > 0 ) {
    SpikeCount++;
    Latencies.push( Results.back().Spikes.latency( 0.0 ) );
  }
  Amplitudes.push( Results.back().Amplitude );
  Spikes.push( Results.back().Spikes );
}


void FindThreshold::openFiles( ofstream &tf, TableKey &tracekey )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( CurrentTrace[0] >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.1f" );
  if ( completeRuns() <= 0 )
    tf.open( addPath( "findthreshold-traces.dat" ).c_str() );
  else
    tf.open( addPath( "findthreshold-traces.dat" ).c_str(),
             ofstream::out | ofstream::app );
  Header.save( tf, "# " );
  tf << "# status:\n";
  stimulusData().save( tf, "#   " );
  tf << "# settings:\n";
  settings().save( tf, "#   " );
  tf << '\n';
  tracekey.saveKey( tf, true, false );
  tf << '\n';
}


void FindThreshold::saveTrace( ofstream &tf, TableKey &tracekey, int index )
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


void FindThreshold::save( void )
{
  double asd = 0.0;
  double am = Amplitudes.mean( asd );
  Header.addNumber( "amplitude", am, IUnit, "%0.3f" );
  Header.addNumber( "amplitude s.d.", asd, IUnit, "%0.3f" );
  Header.addInteger( "trials", TrialCount );
  Header.addInteger( "spikes", SpikeCount );
  Header.addNumber( "prob", 100.0*(double)SpikeCount/(double)TrialCount, "%", "%0.1f" );
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  Header.addNumber( "latency", 1000.0*lm, "ms", "%0.2f" );
  Header.addNumber( "latency s.d.", 1000.0*lsd, "ms", "%0.2f" );

  saveSpikes();
  saveData();
}


void FindThreshold::saveSpikes( void )
{
  ofstream sf;
  if ( completeRuns() <= 0 )
    sf.open( addPath( "findthreshold-spikes.dat" ).c_str() );
  else
    sf.open( addPath( "findthreshold-spikes.dat" ).c_str(),
             ofstream::out | ofstream::app );

  Header.save( sf, "# " );
  sf << "# status:\n";
  stimulusData().save( sf, "#   " );
  sf << "# settings:\n";
  settings().save( sf, "#   " );
  sf << '\n';

  TableKey spikekey;
  spikekey.addNumber( "t", "ms", "%7.2f" );
  spikekey.saveKey( sf, true, false );
  sf << '\n';

  for ( int k=0; k<Spikes.size(); k++ ) {
    sf << "#       index: " << Str( k ) << '\n';
    sf << "#   amplitude: " << Str( Results.back().Amplitude ) << IUnit << '\n';
    sf << "# spike count: " << Str( Results.back().SpikeCount ) << '\n';
    Spikes[k].saveText( sf, 1000.0, 7, 2, 'f', "-0" );
    sf << '\n';
  }

  sf << '\n';
}


void FindThreshold::saveData( void )
{
  TableKey datakey;
  datakey.addLabel( "Data" );
  datakey.addNumber( "duration", "ms", "%6.1f", Header.number( "duration" ) );
  double asd = 0.0;
  double am = Amplitudes.mean( asd );
  datakey.addNumber( "amplitude", IUnit, "%7.3f", am );
  datakey.addNumber( "s.d.", IUnit, "%7.3f", asd );
  datakey.addNumber( "trials", "1", "%6.0f", (double)TrialCount );
  datakey.addNumber( "spikes", "1", "%6.0f", (double)SpikeCount );
  datakey.addNumber( "prob", "%", "%5.1f", 100.0*(double)SpikeCount/(double)TrialCount );
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  datakey.addNumber( "latency", "ms", "%6.2f", 1000.0*lm );
  datakey.addNumber( "s.d.", "ms", "%6.2f", 1000.0*lsd );
  datakey.addLabel( "Traces" );
  datakey.add( stimulusData() );

  ofstream df;
  if ( completeRuns() <= 0 ) {
    df.open( addPath( "findthreshold-data.dat" ).c_str() );
    datakey.saveKey( df );
  }
  else
    df.open( addPath( "findthreshold-data.dat" ).c_str(),
             ofstream::out | ofstream::app );

  datakey.saveData( df );

  metaData( "Cell" ).setNumber( "ithreshss", am );
}


void FindThreshold::plot( bool record, double duration )
{
  P.lock();
  P.clear();
  double am = Amplitudes.mean();
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  if ( record )
    P.setTitle( "p=" + Str( 100.0*(double)SpikeCount/(double)TrialCount, 0, 0, 'f' ) +
		"%,  latency=(" + Str( 1000.0*lm, 0, 0, 'f' ) +
		"+/-" + Str( 1000.0*lsd, 0, 0, 'f' ) +
		") ms, amplitude=" + Str( am, 0, 2, 'f' ) + " " + IUnit );
  else
    P.setTitle( "" );
  P.plotVLine( 0, Plot::White, 2 );
  P.plotVLine( 1000.0*duration, Plot::White, 2 );
  for ( unsigned int k=0; k<Results.size()-1; k++ ) {
    SampleDataD vtrace = Results[k].Voltage;
    vtrace += 10.0*(Results.size() - k - 1);
    P.plot( vtrace, 1000.0, Plot::Orange, 2, Plot::Solid );
  }
  if ( ! Results.empty() )
    P.plot( Results.back().Voltage, 1000.0, Plot::Yellow, 4, Plot::Solid );
  P.draw();
  P.unlock();
}


FindThreshold::Data::Data( double savetime, const InData &voltage,
			   const InData &current )
  : Spikes( 10 )
{
  Amplitude = 0.0;
  Voltage.resize( 0.0, savetime, voltage.stepsize(), 0.0 );
  voltage.copy( voltage.signalTime(), Voltage );
  Current.resize( 0.0, savetime, current.stepsize(), 0.0 );
  current.copy( current.signalTime(), Current );
  SpikeCount = 0;
}


FindThreshold::Data::Data( double savetime, const InData &voltage )
  : Spikes( 10 )
{
  Amplitude = 0.0;
  Voltage.resize( 0.0, savetime, voltage.stepsize(), 0.0 );
  voltage.copy( voltage.signalTime(), Voltage );
  Current.clear();
  SpikeCount = 0;
}


addRePro( FindThreshold );

}; /* namespace patchclampprojects */

#include "moc_findthreshold.cc"
