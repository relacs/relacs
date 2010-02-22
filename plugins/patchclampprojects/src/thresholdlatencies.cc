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
  addSelection( "durationsel", "Set duration of stimulus", "in milliseconds|as multiples of membrane time constant" );
  addNumber( "duration", "Duration of stimulus", 0.1, 0.0, 1000.0, 0.001, "sec", "ms" ).setActivation( "durationsel", "in milliseconds" );
  addNumber( "durationfac", "Duration of stimulus", 1.0, 0.0, 1000.0, 0.1, "tau_m" ).setActivation( "durationsel", "as multiples of membrane time constant" );
  addNumber( "searchpause", "Duration of pause between outputs during search", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "pause", "Duration of pause between outputs", 1.0, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "delay", "Time before stimullus onset", 0.05, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "savetracetime", "Length of trace to be saved and analyzed", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 );
  addLabel( "Stimulus amplitudes" );
  addSelection( "amplitudesrc", "Use initial amplitude from", "custom|DC|threshold" );
  addNumber( "startamplitude", "Initial amplitude of current stimulus", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "amplitudesrc", "custom" );
  addNumber( "startamplitudestep", "Initial size of amplitude steps used for searching threshold", 0.1, 0.0, 1000.0, 0.001 );
  addNumber( "amplitudestep", "Final size of amplitude steps used for oscillating around threshold", 0.01, 0.0, 1000.0, 0.001 );
  addSelection( "adjust", "Adjust", "DC|none|stimulus|DC" );
  setFlags( 1 );
  addTypeStyle( OptWidget::Bold, Parameter::Label );
  setConfigSelectMask( 1 );
  setDialogSelectMask( 1 );

  // data exchange options:
  addLabel( "Used amplitudes" );
  addNumber( "stimulusamplitude", 0.0 );
  addNumber( "dcstimulusamplitude", 0.0 );
  addNumber( "dcamplitude", 0.0 );

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
    setUnit( "startamplitudestep", IUnit );
    setUnit( "amplitudestep", IUnit );
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
  int durationsel = index( "durationsel" );
  double duration = number( "duration" );
  double durationfac = number( "durationfac" );
  double searchpause = number( "searchpause" );
  double measurepause = number( "pause" );
  double delay = number( "delay" );
  double savetracetime = number( "savetracetime" );
  int repeats = integer( "repeats" );
  int amplitudesrc = index( "amplitudesrc" );
  double amplitude = number( "startamplitude" );
  double amplitudestep = number( "startamplitudestep" );
  double finalamplitudestep = number( "amplitudestep" );
  int adjust = index( "adjust" );
  double membranetau = metaData( "Cell" ).number( "taum" );
  double pause = searchpause;
  if ( durationsel == 1 ) {
    if ( membranetau <= 0.0 ) {
      warning( "Membrane time constant was not measured yet!" );
      return Failed;
    }
    duration = durationfac*membranetau;
  }
  double orgdcamplitude = stimulusData().number( outTraceName( outcurrent ) );
  if ( amplitudesrc == 1 )
    amplitude = orgdcamplitude;
  else if ( amplitudesrc == 2 )
    amplitude = metaData( "Cell" ).number( "ithreshss" );

  if ( amplitudestep < finalamplitudestep ) {
    warning( "startamplitudestep must be larger than amplitudestep!" );
    return Failed;
  }
  if ( savetracetime < duration ) {
    warning( "savetracetime must be at least as long as the stimulus duration!" );
    return Failed;
  }
  if ( delay + duration + searchpause < savetracetime ) {
    warning( "Stimulus duration plus searchpause plus delay must be at least as long as savetracetime!" );
    return Failed;
  }
  if ( delay + duration + measurepause < savetracetime ) {
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
  bool record = false;
  bool search = true;
  DoneState state = Completed;
  double dcamplitude = orgdcamplitude;
  Results.clear();
  Amplitudes.clear();
  Amplitudes.reserve( repeats > 0 ? repeats : 100 );
  DCAmplitudes.clear();
  DCAmplitudes.reserve( repeats > 0 ? repeats : 100 );
  Latencies.clear();
  Latencies.reserve( repeats > 0 ? repeats : 100 );
  SpikeCounts.clear();
  SpikeCounts.reserve( repeats > 0 ? repeats : 100 );
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
  Header.addNumber( "ReProTime", "s", "%0.3f", reproStartTime() );
  Header.addNumber( "duration", "ms", "%0.1f", 1000.0*duration );

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

  // DC signal:
  OutData dcsignal( dcamplitude );
  dcsignal.setTrace( outcurrent );

  // write stimulus:
  sleep( pause );
  for ( int count=1; softStop() == 0; count++ ) {

    timeStamp();

    Str s;
    if ( ! record )
      s = "<b>Search threshold: </b>";
    else
      s = "<b>Measure response: </b>";
    s += "DC amplitude <b>" + Str( dcamplitude ) + " " + IUnit +"</b>, ";
    s += "Amplitude <b>" + Str( amplitude ) + " " + IUnit +"</b>";
    if ( record )
      s += ",  Loop <b>" + Str( count ) + "</b>";
    message( s );

    // signal:
    signal = amplitude;
    signal.setIdent( "const ampl=" + Str( amplitude ) + IUnit );
    signal.back() = dcamplitude;
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
    analyze( involtage, incurrent, amplitude, dcamplitude,
	     delay, duration, savetracetime, pause );
    if ( record )
      saveTrace( tf, tracekey, count-1 );
    plot( record, duration );

    if ( ! record || adjust == 1 ) {
      // change stimulus amplitude:
      if ( Results.back().SpikeCount > 0 )
	amplitude -= amplitudestep;
      else
	amplitude += amplitudestep;
    }
    else if ( adjust == 2 ) {
      // change DC amplitude:
      if ( Results.back().SpikeCount > 0 ||
	   Results.back().BaseSpikeCount > 0 ) {
	dcamplitude -= amplitudestep;
	amplitude -= amplitudestep;
      }
      else {
	dcamplitude += amplitudestep;
	amplitude += amplitudestep;
      }
      dcsignal = dcamplitude;
      dcsignal.setIdent( "DC=" + Str( dcamplitude ) + IUnit );
      directWrite( dcsignal );
    }

    // switch modes:
    if ( ! record ) {
      // find threshold:
      if ( Results.size() > 1 &&
	   ( ( Results[Results.size()-2].SpikeCount <= 0 &&
	       Results[Results.size()-1].SpikeCount > 0 ) ||
	     ( Results[Results.size()-2].SpikeCount > 0 &&
	       Results[Results.size()-1].SpikeCount <= 0 ) ) ) {
	if ( amplitudestep <= finalamplitudestep ) {
	  amplitudestep = finalamplitudestep;
	  pause = measurepause;
	  count = 0;
	  Results.clear();
	  SpikeCount = 0;
	  TrialCount = 0;
	  Amplitudes.clear();
	  Amplitudes.reserve( repeats > 0 ? repeats : 1000 );
	  DCAmplitudes.clear();
	  DCAmplitudes.reserve( repeats > 0 ? repeats : 1000 );
	  Latencies.clear();
	  Latencies.reserve( repeats > 0 ? repeats : 1000 );
	  SpikeCounts.clear();
	  SpikeCounts.reserve( repeats > 0 ? repeats : 100 );
	  Spikes.clear();
	  Spikes.reserve( repeats > 0 ? repeats : 1000 );
	  if ( search )
	    search = false;
	  else {
	    record = true;
	    openTraceFile( tf, tracekey, incurrent );
	  }
	}
	else
	  amplitudestep *= 0.5;
      }
    }
    TrialCount = count;

    sleepOn( delay + duration + pause );

    if ( record && repeats > 0 && count >= repeats )
      break;

  }

  tf << '\n';
  if ( record && TrialCount > 0 )
    save( fabs( orgdcamplitude ) > 1.0e-6 );
  Results.clear();
  Latencies.clear();
  Amplitudes.clear();
  DCAmplitudes.clear();
  SpikeCounts.clear();
  Spikes.clear();
  return state;
}


void ThresholdLatencies::analyze( int involtage, int incurrent,
				  double amplitude, double dcamplitude,
				  double delay, double duration,
				  double savetime, double pause )
{
  if ( Results.size() >= 20 )
    Results.pop_front();

  if ( incurrent >= 0 ) {
    Results.push_back( Data( delay, savetime, trace( SpikeTrace[involtage] ),
			     trace( incurrent ) ) );
    Results.back().Current *= IInFac;
    Results.back().DCAmplitude = Results.back().Current.mean( -delay, 0.0 );
    Results.back().Amplitude = Results.back().Current.mean( 0.5*duration, duration );
  }
  else {
    Results.push_back( Data( delay, savetime, trace( SpikeTrace[involtage] ) ) );
    Results.back().DCAmplitude = dcamplitude;
    Results.back().Amplitude = amplitude;
  }
  double sigtime = events( SpikeEvents[involtage] ).signalTime();
  events( SpikeEvents[involtage] ).copy( sigtime - delay,
					 sigtime + savetime - delay,
					 sigtime,
					 Results.back().Spikes );
  Results.back().SpikeCount = Results.back().Spikes.count( 0.0, savetime-delay );
  Results.back().BaseSpikeCount = events( SpikeEvents[involtage] ).count( sigtime-pause, sigtime );

  if ( Results.back().SpikeCount > 0 ) {
    SpikeCount++;
    Latencies.push( Results.back().Spikes.latency( 0.0 ) );
  }
  DCAmplitudes.push( Results.back().DCAmplitude );
  Amplitudes.push( Results.back().Amplitude );
  SpikeCounts.push( Results.back().SpikeCount );
  Spikes.push( Results.back().Spikes );
}


void ThresholdLatencies::openTraceFile( ofstream &tf, TableKey &tracekey,
					int incurrent )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( incurrent >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.3f" );
  if ( completeRuns() <= 0 )
    tf.open( addPath( "thresholdlatencies-traces.dat" ).c_str() );
  else
    tf.open( addPath( "thresholdlatencies-traces.dat" ).c_str(),
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


void ThresholdLatencies::saveTrace( ofstream &tf, TableKey &tracekey, int index )
{
  tf << "#       index: " << Str( index ) << '\n';
  tf << "# dcamplitude: " << Str( Results.back().DCAmplitude ) << IUnit << '\n';
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


void ThresholdLatencies::save( bool dc )
{
  double basd = 0.0;
  double bam = DCAmplitudes.mean( basd );
  Header.addNumber( "dcamplitude", IUnit, "%7.3f", bam );
  Header.addNumber( "dcamplitude s.d.", IUnit, "%7.3f", basd );
  double asd = 0.0;
  double am = Amplitudes.mean( asd );
  Header.addNumber( "amplitude", IUnit, "%7.3f", am );
  Header.addNumber( "amplitude s.d.", IUnit, "%7.3f", asd );
  Header.addNumber( "trials", "1", "%6.0f", (double)TrialCount );
  Header.addNumber( "spikes", "1", "%6.0f", (double)SpikeCount );
  Header.addNumber( "prob", "%", "%5.1f", 100.0*(double)SpikeCount/(double)TrialCount );
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  Header.addNumber( "latency", "ms", "%6.2f", 1000.0*lm );
  Header.addNumber( "latency s.d.", "ms", "%6.2f", 1000.0*lsd );

  saveSpikes();
  saveData( dc );
}


void ThresholdLatencies::saveSpikes( void )
{
  ofstream sf;
  if ( completeRuns() <= 0 )
    sf.open( addPath( "thresholdlatencies-spikes.dat" ).c_str() );
  else
    sf.open( addPath( "thresholdlatencies-spikes.dat" ).c_str(),
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
    sf << "# dcamplitude: " << Str( DCAmplitudes[k] ) << IUnit << '\n';
    sf << "#   amplitude: " << Str( Amplitudes[k] ) << IUnit << '\n';
    sf << "# spike count: " << Str( SpikeCounts[k] ) << '\n';
    Spikes[k].saveText( sf, 1000.0, 7, 2, 'f', "-0" );
    sf << '\n';
  }

  sf << '\n';
}


void ThresholdLatencies::saveData( bool dc )
{
  TableKey datakey;
  datakey.addLabel( "Data" );
  datakey.addNumber( "duration", "ms", "%6.1f", Header.number( "duration" ) );
  double basd = 0.0;
  double bam = DCAmplitudes.mean( basd );
  datakey.addNumber( "dcamplitude", IUnit, "%7.3f", bam );
  datakey.addNumber( "s.d.", IUnit, "%7.3f", basd );
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
    df.open( addPath( "thresholdlatencies-data.dat" ).c_str() );
    datakey.saveKey( df );
  }
  else
    df.open( addPath( "thresholdlatencies-data.dat" ).c_str(),
             ofstream::out | ofstream::app );

  datakey.saveData( df );

  // make amplitudes public:
  if ( dc ) {
    setNumber( "dcstimulusamplitude", am );
    setNumber( "dcamplitude", bam );
  }
  else
    setNumber( "stimulusamplitude", am );

}


void ThresholdLatencies::plot( bool record, double duration )
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
  P.unlock();
  P.draw();
}


ThresholdLatencies::Data::Data( double delay, double savetime,
				const InData &voltage,
				const InData &current )
  : Spikes( 10 )
{
  DCAmplitude = 0.0;
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
  DCAmplitude = 0.0;
  Amplitude = 0.0;
  Voltage.resize( -delay, savetime-delay, voltage.stepsize(), 0.0 );
  voltage.copy( voltage.signalTime(), Voltage );
  Current.clear();
  SpikeCount = 0;
}


addRePro( ThresholdLatencies );

}; /* namespace patchclampprojects */

#include "moc_thresholdlatencies.cc"
