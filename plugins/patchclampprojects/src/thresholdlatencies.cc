/*
  patchclampprojects/thresholdlatencies.cc
  Measures spike latencies in response to pulses close to the firing threshold.

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

#include <fstream>
#include <relacs/tablekey.h>
#include <relacs/patchclampprojects/thresholdlatencies.h>
using namespace relacs;

namespace patchclampprojects {


ThresholdLatencies::ThresholdLatencies( void )
  : RePro( "ThresholdLatencies", "patchclampprojects", "Jan Benda", "1.2", "Nov 03, 2010" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    IInFac( 1.0 ),
    RecordNow( false )
{
  // add some options:
  newSection( "Test-Pulse" );
  addSelection( "durationsel", "Set duration of stimulus", "in milliseconds|as multiples of membrane time constant" );
  addNumber( "duration", "Duration of stimulus", 0.1, 0.0, 1000.0, 0.001, "sec", "ms" ).setActivation( "durationsel", "in milliseconds" );
  addNumber( "durationfac", "Duration of stimulus", 1.0, 0.0, 1000.0, 0.1, "tau_m" ).setActivation( "durationsel", "as multiples of membrane time constant" );
  addSelection( "startamplitudesrc", "Set initial amplitude to", "custom|DC|threshold|previous" );
  addNumber( "startamplitude", "Initial amplitude of current stimulus", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "startamplitudesrc", "custom" );
  addNumber( "startamplitudestep", "Initial size of amplitude steps used for searching threshold", 0.1, 0.0, 1000.0, 0.001 );
  addNumber( "amplitudestep", "Final size of amplitude steps used for oscillating around threshold", 0.01, 0.0, 1000.0, 0.001 );
  addSelection( "adjust", "Adjust", "DC|none|stimulus|DC" );
  addSection( "First Pre-Pulse" );
  addNumber( "preduration", "Duration of first pre-pulse stimulus", 0.0, 0.0, 1000.0, 0.001, "sec", "ms" );
  addSelection( "preamplitudesrc", "Set amplitude of first pre-pulse to", "custom|previous DC|threshold|VC|VC rest" ).setActivation( "preduration", ">0" );
  addNumber( "preamplitude", "Amplitude of first pre-pulse stimulus", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "preamplitudesrc", "custom" );
  addNumber( "prevcamplitude", "Voltage clamp value of first pre-pulse", 0.0, -1000.0, 1000.0, 0.1, "mV" ).setActivation( "preamplitudesrc", "VC|VC rest" );
  addNumber( "prevcgain", "Gain for voltage clamp of first pre-pulse", 0.0, -1000.0, 1000.0, 0.1, "mS" ).setActivation( "preamplitudesrc", "VC|VC rest" );
  addSelection( "prepulseramp", "Start first pre-pulse with a ramp", "none|linear|cosine" ).setActivation( "preduration", ">0" );
  addNumber( "prepulserampwidth", "Width of the ramp", 0.0, 0.0, 1000.0, 0.001, "sec", "ms" ).setActivation( "prepulseramp", "none", false );
  newSection( "Second Pre-Pulse" );
  addNumber( "pre2duration", "Duration of second pre-pulse stimulus", 0.0, 0.0, 1000.0, 0.001, "sec", "ms" );
  addSelection( "pre2amplitudesrc", "Set amplitude of second pre-pulse to", "custom|previous DC|threshold" ).setActivation( "pre2duration", ">0" );
  addNumber( "pre2amplitude", "Amplitude of second pre-pulse stimulus", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "pre2amplitudesrc", "custom" );
  newSection( "Post-Pulse" );
  addNumber( "postduration", "Duration of post-pulse stimulus", 0.0, 0.0, 1000.0, 0.001, "sec", "ms" );
  addSelection( "postamplitudesrc", "Set post-pulse amplitude to", "custom|previous DC|threshold" ).setActivation( "postduration", ">0" );
  addNumber( "postamplitude", "Amplitude of post-pulse stimulus", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "postamplitudesrc", "custom" );
  newSection( "Timing" );
  addNumber( "searchpause", "Duration of pause between outputs during search", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "pause", "Duration of pause between outputs", 1.0, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "delay", "Time before stimullus onset", 0.05, 0.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "savetracetime", "Length of trace to be saved and analyzed (from test-pulse on)", 0.5, 0.0, 1000.0, 0.01, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );

  PrevMeanTestAmplitude = 0.0;
  PrevMeanDCTestAmplitude = 0.0;
  PrevMeanDCAmplitude = 0.0;

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
  setWidget( &P );
}


void ThresholdLatencies::preConfig( void )
{
  if ( SpikeTrace[0] >= 0 )
    VUnit = trace( SpikeTrace[0] ).unit();
  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "startamplitude", IUnit );
    setUnit( "startamplitudestep", IUnit );
    setUnit( "amplitudestep", IUnit );
    setUnit( "preamplitude", IUnit );
    setUnit( "pre2amplitude", IUnit );
    setUnit( "postamplitude", IUnit );
  }
  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int ThresholdLatencies::main( void )
{
  int durationsel = index( "durationsel" );
  double duration = number( "duration" );
  double durationfac = number( "durationfac" );
  double searchpause = number( "searchpause" );
  double measurepause = number( "pause" );
  double delay = number( "delay" );
  double savetracetime = number( "savetracetime" );
  int repeats = integer( "repeats" );
  int startamplitudesrc = index( "startamplitudesrc" );
  double amplitude = number( "startamplitude" );
  double amplitudestep = number( "startamplitudestep" );
  double finalamplitudestep = number( "amplitudestep" );
  int adjust = index( "adjust" );
  double preduration = number( "preduration" );
  int preamplitudesrc = index( "preamplitudesrc" );
  double preamplitude = number( "preamplitude" );
  double prevcamplitude = number( "prevcamplitude" );
  double prevcgain = number( "prevcgain" );
  int prepulseramp = index( "prepulseramp" );
  double prepulserampwidth = number( "prepulserampwidth" );
  double pre2duration = number( "pre2duration" );
  int pre2amplitudesrc = index( "pre2amplitudesrc" );
  double pre2amplitude = number( "pre2amplitude" );
  double postduration = number( "postduration" );
  int postamplitudesrc = index( "postamplitudesrc" );
  double postamplitude = number( "postamplitude" );
  lockMetaData();
  double membranetau = metaData().number( "Cell>taum" );
  unlockMetaData();
  double pause = searchpause;
  if ( durationsel == 1 ) {
    if ( membranetau <= 0.0 ) {
      warning( "Membrane time constant was not measured yet!" );
      return Failed;
    }
    duration = durationfac*membranetau;
  }
  double orgdcamplitude = stimulusData().number( outTraceName( 0 ) );
  // test-pulse amplitude:
  lockMetaData();
  if ( startamplitudesrc == 1 ) // current dc
    amplitude = orgdcamplitude;
  else if ( startamplitudesrc == 2 ) {  // thresh
    amplitude = metaData().number( "Cell>ithreshss" );
    if ( amplitude == 0.0 )
      amplitude = metaData().number( "Cell>ithreshon" );
  }
  else if ( startamplitudesrc == 3 )  // prev
    amplitude = PrevMeanTestAmplitude;
  // pre-pulse amplitude:
  double prevc = false;
  if ( preamplitudesrc == 1 ) // previous dc
    preamplitude = PrevMeanDCAmplitude;
  else if ( preamplitudesrc == 2 ) { // thresh
    preamplitude = metaData().number( "Cell>ithreshss" );
    if ( preamplitude == 0.0 )
      preamplitude = metaData().number( "Cell>ithreshon" );
  }
  else if ( preamplitudesrc >= 3 ) {  // VC + VC rest
    prevc = ( preduration > 0.0 );
    preamplitude = 0.0;
  }
  // pre2-pulse amplitude:
  if ( pre2amplitudesrc == 1 ) // previous dc
    pre2amplitude = PrevMeanDCAmplitude;
  else if ( pre2amplitudesrc == 2 ) { // thresh
    pre2amplitude = metaData().number( "Cell>ithreshss" );
    if ( pre2amplitude == 0.0 )
      pre2amplitude = metaData().number( "Cell>ithreshon" );
  }
  // post-pulse amplitude:
  if ( postamplitudesrc == 1 ) // previous dc
    postamplitude = PrevMeanDCAmplitude;
  else if ( postamplitudesrc == 2 ) { // thresh
    postamplitude = metaData().number( "Cell>ithreshss" );
    if ( postamplitude == 0.0 )
      postamplitude = metaData().number( "Cell>ithreshon" );
  }
  unlockMetaData();

  if ( amplitudestep < finalamplitudestep ) {
    warning( "startamplitudestep must be larger than amplitudestep!" );
    return Failed;
  }
  if ( savetracetime < duration + postduration ) {
    warning( "savetracetime must be at least as long as the test- and post-pulse duration! Continue anyways.", 5.0 );
    savetracetime = duration + postduration;
  }
  if ( duration + postduration + searchpause < savetracetime ) {
    warning( "Test-pulse plus post-pulse duration plus searchpause must be at least as long as savetracetime! Continue anyways.", 5.0 );
    savetracetime = duration + postduration + searchpause;
  }
  if ( duration + postduration + measurepause < savetracetime ) {
    warning( "Test-pulse plus post-pulse duration plus pause must be at least as long as savetracetime! Continue anyways.", 5.0 );
    savetracetime = duration + postduration + measurepause;
  }
  if ( prepulseramp > 0 && preduration > 0.0 && preduration < prepulserampwidth ) {
    warning( "Width of pre-pulse ramp is longer than pre-pulse duration!<br>Continue with ramp set to pre-pulse width." );
    prepulserampwidth = preduration;
  }
  if ( preduration > 0.0 && prepulseramp > 0 && prepulserampwidth <= 0.0 ) {
    warning( "Width of pre-pulse ramp should be greater than zero!" );
    return Failed;
  }
  if ( SpikeTrace[ 0 ] < 0 || SpikeEvents[ 0 ] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }
  RecordNow = false;

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
  PreAmplitudes.clear();
  PreAmplitudes.reserve( repeats > 0 ? repeats : 100 );
  Pre2Amplitudes.clear();
  Pre2Amplitudes.reserve( repeats > 0 ? repeats : 100 );
  PostAmplitudes.clear();
  PostAmplitudes.reserve( repeats > 0 ? repeats : 100 );
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
  Header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  Header.addNumber( "duration", 1000.0*duration, "ms", "%0.1f" );

  // plot trace:
  tracePlotSignal( 1.5*savetracetime, 0.5*savetracetime );

  // plot:
  P.lock();
  P.clear();
  P.setXRange( -1000.0*(delay+preduration+pre2duration), 1000.0*(savetracetime-delay) );
  P.setYLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]" );
  P.unlock();

  // signal:
  OutData signal( preduration + pre2duration + duration + postduration, trace( SpikeTrace[0] ).stepsize() );
  signal.setTrace( CurrentOutput[0] );
  signal.setDelay( delay );
  if ( preduration > 0.0 ) {
    signal.description().newSection( "", "stimulus/pulse" );
    signal.description().addNumber( "TOffs", 0.0, "ms" );
    signal.description().addNumber( "Intensity", preamplitude, IUnit );
    signal.description().addNumber( "IntensityOffset", dcamplitude, IUnit );
    signal.description().addNumber( "Duration", 1000.0*preduration, "ms" );
  }
  if ( pre2duration > 0.0 ) {
    signal.description().newSection( "", "stimulus/pulse" );
    signal.description().addNumber( "TOffs", 1000.0*preduration, "ms" );
    signal.description().addNumber( "Intensity", pre2amplitude, IUnit );
    signal.description().addNumber( "IntensityOffset", dcamplitude, IUnit );
    signal.description().addNumber( "Duration", 1000.0*pre2duration, "ms" );
  }
  signal.description().newSection( "", "stimulus/pulse" );
  signal.description().addNumber( "TOffs", 1000.0*(preduration+pre2duration), "ms" );
  signal.description().addNumber( "Intensity", amplitude, IUnit );
  signal.description().addNumber( "IntensityOffset", dcamplitude, IUnit );
  signal.description().addNumber( "Duration", 1000.0*preduration, "ms" );
  if ( postduration > 0.0 ) {
    signal.description().newSection( "", "stimulus/pulse" );
    signal.description().addNumber( "TOffs", 1000.0*(preduration+duration), "ms" );
    signal.description().addNumber( "Intensity", postamplitude, IUnit );
    signal.description().addNumber( "IntensityOffset", dcamplitude, IUnit );
    signal.description().addNumber( "Duration", 1000.0*postduration, "ms" );
  }

  // VC signals:
  OutData vcsignal( preduration + pre2duration + duration + postduration, trace( SpikeTrace[0] ).stepsize() );
  vcsignal.setTraceName( "VC" );
  vcsignal.setDelay( delay );
  vcsignal.description().newSection( "", "stimulus/pulse" );
  vcsignal.description().addNumber( "TOffs", 0.0, "ms" );
  vcsignal.description().addNumber( "Intensity", prevcamplitude, "mV" );
  vcsignal.description().addNumber( "IntensityOffset", 0.0, "mV" );
  vcsignal.description().addNumber( "Duration", 1000.0*preduration, "ms" );

  OutData vcgainsignal( preduration + duration + postduration, trace( SpikeTrace[0] ).stepsize() );
  vcgainsignal.setTraceName( "VCgain" );
  vcgainsignal.setDelay( delay );
  vcgainsignal.description().newSection( "", "stimulus/pulse" );
  vcgainsignal.description().addNumber( "TOffs", 0.0, "ms" );
  vcgainsignal.description().addNumber( "Intensity", prevcgain, "mS" );
  vcgainsignal.description().addNumber( "IntensityOffset", 0.0, "mS" );
  vcgainsignal.description().addNumber( "Duration", 1000.0*preduration, "ms" );

  // all signals:
  OutList sigs;
  sigs.add( &signal );
  if ( prevc ) {
    sigs.add( &vcsignal );
    sigs.add( &vcgainsignal );
  }

  // DC signal:
  OutData dcsignal;
  dcsignal.setTrace( CurrentOutput[0] );
  dcsignal.constWave( dcamplitude );
  dcsignal.setIdent( "DC=" + Str( dcamplitude ) + IUnit );

  // measure resting potential:
  lockMetaData();
  double restvoltage = metaData().number( "Cell>vrest" );
  unlockMetaData();
  if ( prevc && preamplitudesrc == 4 ) {  // VC rest
    // zero DC current:
    dcsignal.constWave( 0.0 );
    dcsignal.setIdent( "DC=" + Str( 0.0 ) + IUnit );
    directWrite( dcsignal );
    sleep( 0.5 );
    if ( interrupt() )
      return Aborted;

    // measure resting potential:
    restvoltage = trace( SpikeTrace[0] ).mean( currentTime()-0.2,
					       currentTime() );
    prevcamplitude += restvoltage;
    message( "Voltage clamp to " + Str( prevcamplitude ) + " mV, resting potential is at "
	     + Str( restvoltage ) + " mV" );

    // back to DC:
    dcsignal.constWave( dcamplitude );
    dcsignal.setIdent( "DC=" + Str( dcamplitude ) + IUnit );
    directWrite( dcsignal );
  }

  // wait:
  sleepWait( pause );
  if ( interrupt() )
    return Aborted;

  // run:
  for ( int count=1; softStop() == 0; count++ ) {

    timeStamp();

    Str s;
    if ( ! record )
      s = "<b>Search threshold: </b>";
    else
      s = "<b>Measure response: </b>";
    if ( preduration > 0.0 )
      s += "Pre-pulse <b>" + Str( 1000.0*preduration, 0, 0, 'f' ) + " ms</b>, ";
    s += "DC amplitude <b>" + Str( dcamplitude ) + " " + IUnit +"</b>, ";
    s += "Amplitude <b>" + Str( amplitude ) + " " + IUnit +"</b>, ";
    s += "Step <b>" + Str( amplitudestep ) + " " + IUnit +"</b>";
    if ( record ) {
      s += ",  Loop <b>" + Str( count ) + "</b>";
      if ( repeats > 0 )
	s += " of <b>" + Str( repeats ) + "</b>";
    }
    message( s );

    // current membrane voltage:
    double dcvoltage = trace( SpikeTrace[0] ).mean( currentTime()-0.5*pause,
						    currentTime() );

    // signal:
    Options::section_iterator sdi = signal.description().sectionsBegin();
    // pre-pulse:
    if ( preduration > 0.0 && prepulseramp > 0 && ! prevc ) {
      int w = signal.indices( prepulserampwidth );
      if ( prepulseramp == 2 ) {
	// cosine ramp:
	for ( int k=0; k<w; k++ )
	  signal[k] = dcamplitude + (preamplitude-dcamplitude)*( 0.5 - 0.5 * ::cos( 3.14159265358979323846*double(k)/double(w) ) );
      }
      else {
	// linear ramp:
	for ( int k=0; k<w; k++ )
	  signal[k] = dcamplitude + (preamplitude-dcamplitude)*double(k)/double(w);
      }
      for ( int k=w; k<signal.index( preduration ); k++ )
	signal[k] = preamplitude;
      (*sdi)->setNumber( "Intensity", preamplitude, IUnit );
      (*sdi)->setNumber( "IntensityOffset", dcamplitude, IUnit );
      ++sdi;
    }
    else if ( preduration > 0.0 ) {
      for ( int k=0; k<signal.index( preduration ); k++ )
	signal[k] = preamplitude;
      (*sdi)->setNumber( "Intensity", preamplitude, IUnit );
      (*sdi)->setNumber( "IntensityOffset", dcamplitude, IUnit );
      ++sdi;
    }
    // pre2-pulse:
    if ( pre2duration > 0.0 ) {
      for ( int k=signal.index( preduration );
	    k<signal.index( preduration + pre2duration );
	    k++ )
	signal[k] = pre2amplitude;
      (*sdi)->setNumber( "Intensity", pre2amplitude, IUnit );
      (*sdi)->setNumber( "IntensityOffset", dcamplitude, IUnit );
      ++sdi;
    }
    // test-pulse:
    for ( int k=signal.index( preduration + pre2duration );
	  k<signal.index( preduration + pre2duration + duration );
	  k++ )
      signal[k] = amplitude;
    (*sdi)->setNumber( "Intensity", amplitude, IUnit );
    (*sdi)->setNumber( "IntensityOffset", dcamplitude, IUnit );
    ++sdi;
    // post-pulse:
    for ( int k=signal.index( preduration + pre2duration + duration );
	  k<signal.size();
	  k++ )
      signal[k] = postamplitude;
    if ( postduration > 0.0 ) {
      (*sdi)->setNumber( "Intensity", postamplitude, IUnit );
      (*sdi)->setNumber( "IntensityOffset", dcamplitude, IUnit );
    }
    signal.back() = dcamplitude;
    if ( preduration > 0.0 )
      signal.setIdent( "I=" + Str( amplitude ) + IUnit + "IP=" + Str( preamplitude ) + IUnit);
    else
      signal.setIdent( "I=" + Str( amplitude ) + IUnit );
    // vc signals:
    if ( prevc ) {
      vcsignal = 0.0;
      if ( prepulseramp > 0 ) {
	int w = vcsignal.indices( prepulserampwidth );
	if ( prepulseramp == 2 ) {
	  // cosine ramp:
	  for ( int k=0; k<w; k++ )
	    vcsignal[k] = dcvoltage + (prevcamplitude-dcvoltage)*( 0.5 - 0.5 * ::cos( 3.14159265358979323846*double(k)/double(w) ) );
	}
	else {
	  // linear ramp:
	  for ( int k=0; k<w; k++ )
	    vcsignal[k] = dcvoltage + (prevcamplitude-dcvoltage)*double(k)/double(w);
	}
	for ( int k=w; k<vcsignal.index( preduration ); k++ )
	  vcsignal[k] = prevcamplitude;
      }
      else {
	for ( int k=0; k<vcsignal.index( preduration ); k++ )
	  vcsignal[k] = prevcamplitude;
      }
      vcsignal.back() = dcvoltage;
      vcsignal.description().setNumber( "Intensity", prevcamplitude );
      vcgainsignal = 0.0;
      for ( int k=0; k<vcgainsignal.index( preduration ); k++ )
	vcgainsignal[k] = prevcgain;
      vcgainsignal.back() = 0.0;
      vcgainsignal.description().setNumber( "Intensity", prevcgain );
    }

    // write out signals:
    write( sigs );
    if ( sigs.failed() ) {
      warning( sigs.errorText() );
      if ( ! record || count <= 1 )
	state = Failed;
      break;
    }

    // sleep:
    sleep( savetracetime - duration - postduration + 0.01 );
    if ( interrupt() ) {
      if ( ! record || count <= 1 )
	state = Aborted;
      break;
    }

    // analyze, plot, and save:
    analyze( dcamplitude, preamplitude, prevc ? prevcamplitude : -1.0e38,
	     pre2amplitude, amplitude, postamplitude, delay, preduration,
	     pre2duration, duration, postduration, savetracetime, pause );
    if ( record ) {
      unlockAll();
      if ( TrialCount == 0 )
	openTraceFile( tf, tracekey );
      saveTrace( tf, tracekey, count-1 );
      lockAll();
    }
    plot( record, preduration, pre2duration, duration, postduration );

    if ( ! record || adjust == 1 ) {
      // change stimulus amplitude:
      if ( Results.back().SpikeCount > 0 )
	amplitude -= amplitudestep;
      else
	amplitude += amplitudestep;
      if ( fabs( amplitude ) < 1.0e-8 )
	amplitude = 0.0;
    }
    else if ( adjust == 2 ) {
      // change DC amplitude:
      if ( Results.back().SpikeCount > 0 ||
	   Results.back().BaseSpikeCount > 0 ) {
	dcamplitude -= amplitudestep;
	preamplitude -= amplitudestep;
	pre2amplitude -= amplitudestep;
	amplitude -= amplitudestep;
	postamplitude -= amplitudestep;
      }
      else {
	dcamplitude += amplitudestep;
	preamplitude += amplitudestep;
	pre2amplitude += amplitudestep;
	amplitude += amplitudestep;
	postamplitude += amplitudestep;
      }
      if ( fabs( amplitude ) < 1.0e-8 )
	amplitude = 0.0;
      if ( fabs( dcamplitude ) < 1.0e-8 )
	dcamplitude = 0.0;
      if ( fabs( preamplitude ) < 1.0e-8 )
	preamplitude = 0.0;
      if ( fabs( pre2amplitude ) < 1.0e-8 )
	pre2amplitude = 0.0;
      if ( fabs( postamplitude ) < 1.0e-8 )
	postamplitude = 0.0;
      dcsignal.constWave( dcamplitude );
      dcsignal.setIdent( "DC=" + Str( dcamplitude ) + IUnit );
      directWrite( dcsignal );
    }

    // switch modes:
    if ( ! record ) {
      // find threshold:
      if ( RecordNow ||
	   ( Results.size() > 1 &&
	     ( ( Results[Results.size()-2].SpikeCount <= 0 &&
		 Results[Results.size()-1].SpikeCount > 0 ) ||
	       ( Results[Results.size()-2].SpikeCount > 0 &&
		 Results[Results.size()-1].SpikeCount <= 0 ) ) ) ) {
	if ( RecordNow || amplitudestep <= finalamplitudestep ) {
	  if ( ! RecordNow )
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
	  PreAmplitudes.clear();
	  PreAmplitudes.reserve( repeats > 0 ? repeats : 1000 );
	  Pre2Amplitudes.clear();
	  Pre2Amplitudes.reserve( repeats > 0 ? repeats : 1000 );
	  PostAmplitudes.clear();
	  PostAmplitudes.reserve( repeats > 0 ? repeats : 1000 );
	  Latencies.clear();
	  Latencies.reserve( repeats > 0 ? repeats : 1000 );
	  SpikeCounts.clear();
	  SpikeCounts.reserve( repeats > 0 ? repeats : 100 );
	  Spikes.clear();
	  Spikes.reserve( repeats > 0 ? repeats : 1000 );
	  if ( search )
	    search = false;
	  else
	    record = true;
	  if ( RecordNow ) {
	    search = false;
	    record = true;
	  }
	}
	else {
	  amplitudestep *= 0.5;
	  amplitudestep = ceil(amplitudestep/finalamplitudestep)*finalamplitudestep;
	}
      }
    }
    TrialCount = count;

    sleepOn( preduration + pre2duration + duration + postduration + pause );
    if ( interrupt() ) {
      if ( ! record || count <= 1 )
	state = Aborted;
      break;
    }

    if ( record && repeats > 0 && count >= repeats )
      break;

  }

  bool usedc = fabs( orgdcamplitude ) > 1.0e-6;
  if ( record && TrialCount > 0 ) {
    unlockAll();
    tf << '\n';
    save( usedc );
    lockAll();
  }
  dcsignal.constWave( orgdcamplitude );
  dcsignal.setIdent( "DC=" + Str( orgdcamplitude ) + IUnit );
  directWrite( dcsignal );
  if ( prevc ) {
    writeZero( "VCgain" );
    writeZero( "VC" );
  }
  Results.clear();
  Latencies.clear();
  Amplitudes.clear();
  DCAmplitudes.clear();
  PreAmplitudes.clear();
  Pre2Amplitudes.clear();
  PostAmplitudes.clear();
  SpikeCounts.clear();
  Spikes.clear();
  return state;
}


void ThresholdLatencies::analyze( double dcamplitude, double preamplitude,
				  double prevcamplitude, double pre2amplitude,
				  double amplitude, double postamplitude,
				  double delay, double preduration,
				  double pre2duration, double duration,
				  double postduration, double savetime,
				  double pause )
{
  if ( Results.size() >= 20 )
    Results.pop_front();

  if ( CurrentTrace[0] >= 0 ) {
    Results.push_back( Data( delay, preduration+pre2duration, savetime,
			     trace( SpikeTrace[0] ), trace( CurrentTrace[0] ) ) );
    Results.back().Current *= IInFac;
  }
  else
    Results.push_back( Data( delay, preduration+pre2duration, savetime,
			     trace( SpikeTrace[0] ) ) );
  Results.back().DCAmplitude = dcamplitude;
  Results.back().Amplitude = amplitude;
  Results.back().PreAmplitude = preduration > 0.0 ? preamplitude : -1.0e38;
  Results.back().PreVCAmplitude = preduration > 0.0 ? prevcamplitude : -1.0e38;
  Results.back().Pre2Amplitude = pre2duration > 0.0 ? pre2amplitude : -1.0e38;
  Results.back().PostAmplitude = postduration > 0.0 ? postamplitude : -1.0e38;
  events( SpikeEvents[0] ).copy( signalTime() - delay,
				 signalTime() + preduration + pre2duration + savetime - delay,
				 signalTime() + preduration + pre2duration,
				 Results.back().Spikes );
  Results.back().SpikeCount = Results.back().Spikes.count( 0.0, savetime-delay );
  Results.back().BaseSpikeCount = events( SpikeEvents[0] ).count( signalTime()-pause, signalTime() );

  if ( Results.back().SpikeCount > 0 ) {
    SpikeCount++;
    Latencies.push( Results.back().Spikes.latency( 0.0 ) );
  }
  DCAmplitudes.push( Results.back().DCAmplitude );
  Amplitudes.push( Results.back().Amplitude );
  PreAmplitudes.push( Results.back().PreAmplitude );
  Pre2Amplitudes.push( Results.back().Pre2Amplitude );
  PostAmplitudes.push( Results.back().PostAmplitude );
  SpikeCounts.push( Results.back().SpikeCount );
  Spikes.push( Results.back().Spikes );
}


void ThresholdLatencies::openTraceFile( ofstream &tf, TableKey &tracekey )
{
  tracekey.addNumber( "t", "ms", "%7.2f" );
  tracekey.addNumber( "V", VUnit, "%6.1f" );
  if ( CurrentTrace[0] >= 0 )
    tracekey.addNumber( "I", IUnit, "%6.3f" );
  if ( completeRuns() <= 0 )
    tf.open( addPath( "thresholdlatencies-traces.dat" ).c_str() );
  else
    tf.open( addPath( "thresholdlatencies-traces.dat" ).c_str(),
             ofstream::out | ofstream::app );
  Header.save( tf, "# " );
  stimulusData().save( tf, "# ", 0, Options::FirstOnly );
  settings().save( tf, "# ", 0, Options::FirstOnly );
  tf << '\n';
  tracekey.saveKey( tf, true, false );
  tf << '\n';
}


void ThresholdLatencies::saveTrace( ofstream &tf, TableKey &tracekey, int index )
{
  tf << "#         index: " << Str( index ) << '\n';
  tf << "#   dcamplitude: " << Str( Results.back().DCAmplitude ) << IUnit << '\n';
  tf << "#     amplitude: " << Str( Results.back().Amplitude ) << IUnit << '\n';
  if ( Results.back().PreAmplitude > -1.0e38 )
    tf << "#  preamplitude: " << Str( Results.back().PreAmplitude ) << IUnit << '\n';
  if ( Results.back().Pre2Amplitude > -1.0e38 )
    tf << "#  pre2amplitude: " << Str( Results.back().Pre2Amplitude ) << IUnit << '\n';
  if ( Results.back().PostAmplitude > -1.0e38 )
    tf << "# postamplitude: " << Str( Results.back().PostAmplitude ) << IUnit << '\n';
  tf << "#   spike count: " << Str( Results.back().SpikeCount ) << '\n';
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
  Header.addNumber( "dcamplitude", bam, IUnit, "%0.3f" );
  Header.addNumber( "dcamplitude s.d.", basd, IUnit, "%0.3f" );
  double asd = 0.0;
  double am = Amplitudes.mean( asd );
  Header.addNumber( "amplitude", am, IUnit, "%0.3f" );
  Header.addNumber( "amplitude s.d.", asd, IUnit, "%0.3f" );
  double presd = 0.0;
  double prem = PreAmplitudes.mean( presd );
  if ( prem > -1e37 ) {
    Header.addNumber( "preamplitude", prem, IUnit, "%0.3f" );
    Header.addNumber( "preamplitude s.d.", presd, IUnit, "%0.3f" );
  }
  double pre2sd = 0.0;
  double pre2m = Pre2Amplitudes.mean( pre2sd );
  if ( pre2m > -1e37 ) {
    Header.addNumber( "pre2amplitude", pre2m, IUnit, "%0.3f" );
    Header.addNumber( "pre2amplitude s.d.", pre2sd, IUnit, "%0.3f" );
  }
  double postsd = 0.0;
  double postm = PostAmplitudes.mean( postsd );
  if ( postm > -1e37 ) {
    Header.addNumber( "postamplitude", postm, IUnit, "%0.3f" );
    Header.addNumber( "postamplitude s.d.", postsd, IUnit, "%0.3f" );
  }
  Header.addInteger( "trials", TrialCount );
  Header.addInteger( "spikes", SpikeCount );
  Header.addNumber( "prob", 100.0*(double)SpikeCount/(double)TrialCount, "%", "%0.1f" );
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  Header.addNumber( "latency", 1000.0*lm, "ms", "%6.2f" );
  Header.addNumber( "latency s.d.", 1000.0*lsd, "ms", "%6.2f" );

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
  stimulusData().save( sf, "# ", 0, Options::FirstOnly );
  settings().save( sf, "# ", 0, Options::FirstOnly );
  sf << '\n';

  TableKey spikekey;
  spikekey.addNumber( "t", "ms", "%7.2f" );
  spikekey.saveKey( sf, true, false );
  sf << '\n';

  for ( int k=0; k<Spikes.size(); k++ ) {
    sf << "#         index: " << Str( k ) << '\n';
    sf << "#   dcamplitude: " << Str( DCAmplitudes[k] ) << IUnit << '\n';
    sf << "#     amplitude: " << Str( Amplitudes[k] ) << IUnit << '\n';
    if ( PreAmplitudes[k] > -1e38 )
      sf << "#  preamplitude: " << Str( PreAmplitudes[k] ) << IUnit << '\n';
    if ( Pre2Amplitudes[k] > -1e38 )
      sf << "#  pre2amplitude: " << Str( Pre2Amplitudes[k] ) << IUnit << '\n';
    if ( PostAmplitudes[k] > -1e38 )
      sf << "#  postamplitude: " << Str( PostAmplitudes[k] ) << IUnit << '\n';
    sf << "#   spike count: " << Str( SpikeCounts[k] ) << '\n';
    Spikes[k].saveText( sf, 1000.0, 7, 2, 'f', "-0" );
    sf << '\n';
  }

  sf << '\n';
}


void ThresholdLatencies::saveData( bool dc )
{
  TableKey datakey;
  datakey.newSection( "Data" );
  datakey.addNumber( "duration", "ms", "%6.1f", Header.number( "duration" ) );
  double basd = 0.0;
  double bam = DCAmplitudes.mean( basd );
  datakey.addNumber( "dcamplitude", IUnit, "%7.3f", bam );
  datakey.addNumber( "s.d.", IUnit, "%7.3f", basd );
  double asd = 0.0;
  double am = Amplitudes.mean( asd );
  datakey.addNumber( "amplitude", IUnit, "%7.3f", am );
  datakey.addNumber( "s.d.", IUnit, "%7.3f", asd );
  double presd = 0.0;
  double prem = PreAmplitudes.mean( presd );
  if ( prem < 1e-37 ) {
    prem = -100.0;
    presd = -100.0;
  }
  datakey.addNumber( "preamplitude", IUnit, "%7.3f", prem );
  datakey.addNumber( "s.d.", IUnit, "%7.3f", presd );
  double pre2sd = 0.0;
  double pre2m = Pre2Amplitudes.mean( pre2sd );
  if ( pre2m < 1e-37 ) {
    pre2m = -100.0;
    pre2sd = -100.0;
  }
  datakey.addNumber( "pre2amplitude", IUnit, "%7.3f", pre2m );
  datakey.addNumber( "s.d.", IUnit, "%7.3f", pre2sd );
  double postsd = 0.0;
  double postm = PostAmplitudes.mean( postsd );
  if ( postm < 1e-37 ) {
    postm = -100.0;
    postsd = -100.0;
  }
  datakey.addNumber( "postamplitude", IUnit, "%7.3f", postm );
  datakey.addNumber( "s.d.", IUnit, "%7.3f", postsd );
  datakey.addNumber( "trials", "1", "%6.0f", (double)TrialCount );
  datakey.addNumber( "spikes", "1", "%6.0f", (double)SpikeCount );
  datakey.addNumber( "prob", "%", "%5.1f", 100.0*(double)SpikeCount/(double)TrialCount );
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  datakey.addNumber( "latency", "ms", "%6.2f", 1000.0*lm );
  datakey.addNumber( "s.d.", "ms", "%6.2f", 1000.0*lsd );
  datakey.newSection( "Traces" );
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
    PrevMeanDCTestAmplitude = am;
    PrevMeanDCAmplitude = bam;
  }
  else
    PrevMeanTestAmplitude = am;

}


void ThresholdLatencies::plot( bool record, double preduration, double pre2duration,
			       double duration, double postduration )
{
  P.lock();
  P.clear();
  double am = Amplitudes.mean();
  double lsd = 0.0;
  double lm = Latencies.mean( lsd );
  if ( record )
    P.setTitle( "p=" + Str( 100.0*(double)SpikeCount/(double)(TrialCount+1), 0, 0, 'f' ) +
		"%,  latency=(" + Str( 1000.0*lm, 0, 0, 'f' ) +
		"+/-" + Str( 1000.0*lsd, 0, 0, 'f' ) +
		") ms, amplitude=" + Str( am, 0, 2, 'f' ) + " " + IUnit );
  else
    P.setTitle( "" );
  if ( preduration > 0.0 )
    P.plotVLine( -1000.0*(preduration+pre2duration), Plot::White, 1 );
  if ( pre2duration > 0.0 )
    P.plotVLine( -1000.0*pre2duration, Plot::White, 1 );
  P.plotVLine( 0.0, Plot::White, 2 );
  P.plotVLine( 1000.0*duration, Plot::White, 2 );
  if ( postduration > 0.0 )
    P.plotVLine( 1000.0*(duration+postduration), Plot::White, 1 );
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


void ThresholdLatencies::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() == Qt::Key_M ) {
    lock();
    RecordNow = true;
    unlock();
    event->accept();
  }
  else
    RePro::keyPressEvent( event );
}


ThresholdLatencies::Data::Data( double delay, double predurations, double savetime,
				const InData &voltage,
				const InData &current )
  : Spikes( 10 )
{
  DCAmplitude = 0.0;
  Amplitude = 0.0;
  PreAmplitude = 0.0;
  Pre2Amplitude = 0.0;
  PostAmplitude = 0.0;
  Voltage.resize( -delay-predurations, savetime-delay, voltage.stepsize(), 0.0 );
  voltage.copy( signalTime() + predurations, Voltage );
  Current.resize( -delay-predurations, savetime-delay, current.stepsize(), 0.0 );
  current.copy( signalTime() + predurations, Current );
  SpikeCount = 0;
}


ThresholdLatencies::Data::Data( double delay, double predurations, double savetime,
				const InData &voltage )
  : Spikes( 10 )
{
  DCAmplitude = 0.0;
  Amplitude = 0.0;
  PreAmplitude = 0.0;
  Pre2Amplitude = 0.0;
  PostAmplitude = 0.0;
  Voltage.resize( -delay-predurations, savetime-delay, voltage.stepsize(), 0.0 );
  voltage.copy( signalTime() + predurations, Voltage );
  Current.clear();
  SpikeCount = 0;
}


addRePro( ThresholdLatencies, patchclampprojects );

}; /* namespace patchclampprojects */

#include "moc_thresholdlatencies.cc"
