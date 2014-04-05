/*
  patchclampprojects/synapticresistance.cc
  Artificial synaptic conductance with current pulses to measure resistance.

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

#include <relacs/sampledata.h>
#include <relacs/tablekey.h>
#include <relacs/patchclampprojects/synapticresistance.h>
using namespace relacs;

namespace patchclampprojects {


SynapticResistance::SynapticResistance( void )
  : RePro( "SynapticResistance", "patchclampprojects", "Jan Benda", "1.0", "Apr 05, 2014" )
{
  // add some options:
  newSection( "Current pulses" );
  addNumber( "pulseamplitude", "Amplitude of current pulses", -1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "pulseduration", "Duration of current pulses", 0.1, 0.001, 100000.0, 0.01, "s", "ms" );
  addNumber( "pulseperiod", "Period of current pulses", 1.0, 0.001, 10000.0, 0.1, "s", "ms" );
  addNumber( "before", "Time before onset of synaptic current", 1.0, 0.0, 10000.0, 0.1, "s", "ms" );
  addNumber( "after", "Time after end of synaptic current", 1.0, 0.0, 10000.0, 0.1, "s", "ms" );
  addNumber( "pause", "Duration of pause between outputs", 1.0, 0.0, 10000.0, 0.1, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  newSection( "Synaptic current" );
  addBoolean( "enableconductance", "Inject synaptic current", false );
  addNumber( "conductancetau", "Synaptic time-constant", 1.0, 0.0, 100000.0, 0.1, "s", "ms" );
  addNumber( "conductanceamplitude", "Amplitude of synaptic conductance", 1.0, 0.0, 1.0e8, 1.0, "nS" ).setActivation( "enableconductance", "true" );
  addNumber( "reversalpotential", "Reversal potential of synaptic conductance", 0.0, -1000.0, 1000.0, 5.0, "mV" ).setActivation( "enableconductance", "true" );

  // plot:
  setWidget( &P );
}


void SynapticResistance::preConfig( void )
{
  if ( CurrentOutput[0] >= 0 )
    setUnit( "pulseamplitude", outTrace( CurrentOutput[0] ).unit() );
}


int SynapticResistance::main( void )
{
  // get options:
  double pulseamplitude = number( "pulseamplitude" );
  double pulseduration = number( "pulseduration" );
  double pulseperiod = number( "pulseperiod" );
  double before = number( "before" );
  double after = number( "after" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  bool enableconductance = boolean( "enableconductance" );
  double conductancetau = number( "conductancetau" );
  double conductanceamplitude = number( "conductanceamplitude" );
  double reversalpotential = number( "reversalpotential" );
  if ( SpikeTrace[0] < 0 ) {
    warning( "Invalid input voltage trace!" );
    return Failed;
  }
  if ( CurrentOutput[0] < 0 ) {
    warning( "Invalid output current trace!" );
    return Failed;
  }
  if ( enableconductance && outTraceIndex( "g" ) < 0 ) {
    warning( "No conductance output available!" );
    return Failed;
  }

  // init:
  DoneState state = Completed;
  double duration = 10.0*conductancetau + after;
  const InData &intrace = trace( SpikeTrace[0] );
  SampleDataF meantrace = SampleDataF( -before, duration, intrace.stepsize(), 0.0 );
  SampleDataF squaretrace = meantrace;
  SampleDataF stdevtrace = meantrace;
  SampleDataF meancurrent;
  string vunit = intrace.unit();
  string iunit = "";
  if ( CurrentTrace[0] >= 0 ) {
    meancurrent = meantrace;
    iunit = outTrace( CurrentOutput[0] ).unit();
  }

  // don't print repro message:
  noMessage();

  // plot:
  P.lock();
  P.clear();
  P.setXLabel( "Time [ms]" );
  P.setXRange( -1000.0*before, 1000.0*duration );
  P.setYLabel( intrace.ident() + " [" + vunit + "]" );
  P.draw();
  P.unlock();

  // reversal potential:
  if ( enableconductance ) {
    OutData esignal;
    esignal.setTraceName( "E" );
    esignal.constWave( reversalpotential );
    esignal.setIdent( "E=" + Str( reversalpotential ) + "mV" );
    directWrite( esignal );
  }

  // dc signal:
  lockStimulusData();
  double dccurrent = stimulusData().number( outTraceName( CurrentOutput[0] ) );
  unlockStimulusData();
  OutData dcsignal;
  dcsignal.setTrace( CurrentOutput[0] );
  dcsignal.constWave( dccurrent );
  dcsignal.setIdent( "DC=" + Str( dccurrent ) + iunit );

  OutList signal;
  // current pulses:
  OutData sig;
  signal.push( sig );
  signal.back().setTrace( CurrentOutput[0] );
  signal.back().rectangleWave( before + duration, -1.0, pulseperiod, pulseduration,
			       0.0, pulseamplitude );
  signal.back() += dccurrent;
  signal.back().setIdent( "rectanglepulses" );
  // synaptic conductance:
  if ( enableconductance ) {
    signal.push( sig );
    signal.back().setTraceName( "g" );
    signal.back().alphaWave( before+duration, -1.0, conductancetau, ::exp( 1.0 )*conductanceamplitude, before );
    signal.back().setIdent( "gsynaptic" );
  }

  // sleep:
  sleepWait( pause );
  if ( interrupt() )
    return Aborted;

  // plot trace:
  tracePlotSignal( before+duration, 0.0 );

  // write out stimulus:
  for ( int count=0;
        ( repeats <= 0 || count < repeats ) && softStop() == 0;
        count++ ) {

    timeStamp();

    Str s = "Conductance <b>" + Str( conductanceamplitude ) + " nS</b>";
    s += ", Timee-constant <b>" + Str( 1000.0*conductancetau ) + " ms</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    message( s );

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      directWrite( dcsignal );
      return Failed;
    }
    if ( interrupt() ) {
      if ( count < 1 )
        state = Aborted;
      directWrite( dcsignal );
      break;
    }

    // analyze:
    int inx = intrace.signalIndex() - meantrace.index( 0.0 );
    for ( int k=0; k<meantrace.size() && inx+k<intrace.size(); k++ ) {
      double v = intrace[inx+k];
      meantrace[k] += (v - meantrace[k])/(count+1);
      squaretrace[k] += (v*v - squaretrace[k])/(count+1);
      stdevtrace[k] = sqrt( squaretrace[k] - meantrace[k]*meantrace[k] );
      if ( CurrentTrace[0] >= 0 )
	meancurrent[k] += (trace( CurrentTrace[0] )[inx+k] - meancurrent[k])/(count+1);
    }

    // plot:
    P.lock();
    P.clear();
    P.plotVLine( 0, Plot::White, 2 );
    P.plot( meantrace+stdevtrace, 1000.0, Plot::Orange, 1, Plot::Solid );
    P.plot( meantrace-stdevtrace, 1000.0, Plot::Orange, 1, Plot::Solid );
    P.plot( meantrace, 1000.0, Plot::Red, 3, Plot::Solid );
    P.draw();
    P.unlock();

    // pause:
    sleepOn( duration+pause );
    if ( interrupt() ) {
      if ( count < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }

  }

  if ( state == Completed )
    save( meantrace, stdevtrace, meancurrent, vunit, iunit );

  return state;
}


void SynapticResistance::save( const SampleDataF &meantrace, const SampleDataF &stdevtrace,
			       const SampleDataF &meancurrent,
			       const string &vunit, const string &iunit )
{
  ofstream df( addPath( "synapticresistance-trace.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "ReProIndex", reproCount() );
  header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  lockStimulusData();
  header.newSection( stimulusData() );
  unlockStimulusData();
  header.newSection( settings() );
  header.save( df, "# " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "t", "ms", "%6.2f" );
  datakey.addNumber( "V", vunit, "%6.2f" );
  datakey.addNumber( "s.d.", vunit, "%6.2f" );
  if ( !meancurrent.empty() )
    datakey.addNumber( "I", iunit, "%6.3f" );
  datakey.saveKey( df );

  for ( int k=0; k<meantrace.size(); k++ ) {
    datakey.save( df, 1000.0*meantrace.pos( k ), 0 );
    datakey.save( df, meantrace[k] );
    datakey.save( df, stdevtrace[k] );
    if ( ! meancurrent.empty() )
	datakey.save( df, meancurrent[k] );
    df << '\n';
  }
  df << "\n\n";
}


addRePro( SynapticResistance, patchclampprojects );

}; /* namespace patchclampprojects */

#include "moc_synapticresistance.cc"
