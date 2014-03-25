/*
  patchclamp/membraneresistance.cc
  Measures membrane resistance, capacitance, and time constant with current pulses

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
#include <relacs/fitalgorithm.h>
#include <relacs/tablekey.h>
#include <relacs/patchclamp/membraneresistance.h>
using namespace relacs;

namespace patchclamp {


MembraneResistance::MembraneResistance( void )
  : RePro( "MembraneResistance", "patchclamp", "Jan Benda", "1.0", "Nov 12, 2009" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    VFac( 1.0 ),
    IFac( 1.0 ),
    IInFac( 1.0 )
{
  // add some options:
  newSection( "Stimulus" );
  addNumber( "amplitude", "Amplitude of output signal", -1.0, -1000.0, 1000.0, 0.1 ).setActivation( "userm", "false" );
  addBoolean( "userm", "Compute amplitude from vstep and estimated membrane resistance", false );
  addNumber( "vstep", "Steady-state voltage amplitude induced by output signal", -1.0, -1000.0, 1000.0, 0.1, "mV" ).setActivation( "userm", "true" );
  addNumber( "duration", "Duration of output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  newSection( "Analysis" );
  addNumber( "sswidth", "Window length for steady-state analysis", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addBoolean( "nossfit", "Fix steady-state potential for fit", true );
  addBoolean( "plotstdev", "Plot standard deviation of membrane potential", true );
  addSelection( "setdata", "Set results to the session variables", "rest only|always|never" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
  setWidget( &P );
}


void MembraneResistance::preConfig( void )
{
  if ( SpikeTrace[0] >= 0 ) {
    VUnit = trace( SpikeTrace[0] ).unit();
    VFac = Parameter::changeUnit( 1.0, VUnit, "mV" );
  }

  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "amplitude", IUnit );
    IFac = Parameter::changeUnit( 1.0, IUnit, "nA" );
  }

  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int MembraneResistance::main( void )
{
  // get options:
  Amplitude = number( "amplitude" );
  bool userm = boolean( "userm" );
  double vstep = number( "vstep" );
  Duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double sswidth = number( "sswidth" );
  if ( pause < 2.0*Duration ) {
    warning( "Pause must be at least two times the stimulus duration!" );
    return Failed;
  }
  bool nossfit = boolean( "nossfit" );
  if ( SpikeTrace[0] < 0 ) {
    warning( "Invalid input voltage trace!" );
    return Failed;
  }
  if ( userm ) {
    unlockMetaData();
    double rm = metaData().number( "Cell>rm", 0.0, "MOhm" );
    lockMetaData();
    if ( rm > 1.0e-8 ) {
      lockStimulusData();
      double g = stimulusData().number( "g", 0.0, "ns" );
      unlockStimulusData();
      double r = 1.0/(0.001*g + 1.0/rm);
      Amplitude = vstep/r/IFac;
    }
  }

  double samplerate = trace( SpikeTrace[0] ).sampleRate();

  // don't print repro message:
  noMessage();

  // init:
  DoneState state = Completed;
  MeanTrace = SampleDataF( -0.5*Duration, 2.0*Duration, 1/samplerate, 0.0 );
  SquareTrace = MeanTrace;
  StdevTrace = MeanTrace;
  if ( CurrentTrace[0] >= 0 )
    MeanCurrent = MeanTrace;
  else
    MeanCurrent.clear();
  lockStimulusData();
  DCCurrent = stimulusData().number( outTraceName( CurrentOutput[0] ) );
  unlockStimulusData();
  VRest = 0.0;
  VRestsd = 0.0;
  VSS = 0.0;
  VSSsd = 0.0;
  VPeak = 0.0;
  VPeaksd = 0.0;
  VPeakInx = 0;
  VPeakTime = 0.0;
  RMss = 0.0;
  RMOn = 0.0;
  CMOn = 0.0;
  TauMOn = 0.0;
  RMOff = 0.0;
  CMOff = 0.0;
  TauMOff = 0.0;
  ExpOn = SampleDataF( 0.0, Duration, 1/samplerate, 0.0 );
  ExpOff = SampleDataF( Duration, 2.0*Duration, 1/samplerate, 0.0 );

  // plot trace:
  tracePlotSignal( 2.0*Duration, 0.5*Duration );

  // plot:
  P.lock();
  P.setXRange( -500.0*Duration, 2000.0*Duration );
  P.setYLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]" );
  P.unlock();

  // signal:
  OutData signal;
  signal.setTrace( CurrentOutput[0] );
  signal.pulseWave( Duration, 1.0/samplerate, DCCurrent + Amplitude, DCCurrent );
  signal.setIdent( "I=" + Str( DCCurrent + Amplitude ) + IUnit );

  // dc signal:
  OutData dcsignal;
  dcsignal.setTrace( CurrentOutput[0] );
  dcsignal.constWave( DCCurrent );
  dcsignal.setIdent( "DC=" + Str( DCCurrent ) + IUnit );

  // write stimulus:
  sleepWait( pause );
  if ( interrupt() )
    return Aborted;
  for ( Count=0;
	( repeats <= 0 || Count < repeats ) && softStop() == 0;
	Count++ ) {

    Str s = "Amplitude <b>" + Str( Amplitude ) + " " + IUnit +"</b>";
    s += ",  Loop <b>" + Str( Count+1 ) + "</b>";
    message( s );

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      directWrite( dcsignal );
      return Failed;
    }
    if ( interrupt() ) {
      if ( Count < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }

    timeStamp();
    analyzeOn( Duration, sswidth, nossfit );

    sleepOn( Duration + 0.02 );
    if ( interrupt() ) {
      if ( Count < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }

    analyzeOff( Duration, sswidth, nossfit );
    plot();
    sleepOn( pause );
    if ( interrupt() ) {
      if ( Count < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }

  }

  if ( state == Completed )
    save();

  return state;
}


void MembraneResistance::analyzeOn( double duration,
				    double sswidth, bool nossfit )
{
  // update averages:
  const InData &intrace = trace( SpikeTrace[0] );
  int inx = intrace.signalIndex() - MeanTrace.index( 0.0 );
  for ( int k=0; k<MeanTrace.index( duration ) && inx+k<intrace.size(); k++ ) {
    double v = intrace[inx+k];
    MeanTrace[k] += (v - MeanTrace[k])/(Count+1);
    SquareTrace[k] += (v*v - SquareTrace[k])/(Count+1);
    StdevTrace[k] = sqrt( SquareTrace[k] - MeanTrace[k]*MeanTrace[k] );
    if ( CurrentTrace[0] >= 0 ) {
      double c = IInFac*trace( CurrentTrace[0] )[inx+k];
      MeanCurrent[k] += (c - MeanCurrent[k])/(Count+1);
    }
  }

  // resting potential:
  VRest = MeanTrace.mean( -sswidth, 0.0 );
  VRestsd = MeanTrace.stdev( -sswidth, 0.0 );

  // steady-state potential:
  VSS = MeanTrace.mean( duration-sswidth, duration );
  VSSsd = MeanTrace.stdev( duration-sswidth, duration );

  // membrane resitance:
  RMss = ::fabs( (VSS - VRest)/Amplitude )*VFac/IFac;

  // peak potential:
  VPeak = VRest;
  VPeakInx = 0;
  if ( VSS > VRest )
    VPeakInx = MeanTrace.maxIndex( VPeak, 0.0, duration-sswidth );
  else
    VPeakInx = MeanTrace.minIndex( VPeak, 0.0, duration-sswidth );
  VPeaksd = StdevTrace[VPeakInx];
  if ( fabs( VPeak - VSS ) <= 4.0*VSSsd || VPeakInx > MeanTrace.index( duration-sswidth ) ) {
    VPeak = VSS;
    VPeaksd = VSSsd;
    VPeakInx = MeanTrace.index( duration );
    VPeakTime = 0.0;
  }
  else
    VPeakTime = MeanTrace.pos( VPeakInx );


  // fit exponential to onset:
  int inxon0 = MeanTrace.index( 0.0 );
  int inxon1 = VPeakInx;
  // guess time constant:
  double tau = 0.01;
  for ( int k = inxon0; k<VPeakInx; k++ ) {
    if ( (MeanTrace[k]-VSS)/(VRest-VSS) < 1.0/2.71828182845905 ) {
      tau = MeanTrace.pos( k );
      break;
    }
  }
  ArrayD p( 3, 1.0 );
  p[0] = VRest-VSS;
  p[1] = -tau;
  p[2] = VSS;
  ArrayI pi( 3, 1 );
  if ( nossfit ) {
    pi[2] = 0;
    inxon1 = MeanTrace.index( duration );
  }
  ArrayD u( 3, 1.0 );
  double ch = 0.0;
  marquardtFit( MeanTrace.range().begin()+inxon0, MeanTrace.range().begin()+inxon1,
		MeanTrace.begin()+inxon0, MeanTrace.begin()+inxon1,
		StdevTrace.begin()+inxon0, StdevTrace.begin()+inxon1,
		expFuncDerivs, p, pi, u, ch );
  TauMOn = -1000.0*p[1];
  RMOn = ::fabs( (p[2] - VRest)/Amplitude )*VFac/IFac;
  CMOn = TauMOn/RMOn*1000.0;
  for ( int k=0; k<ExpOn.size(); k++ )
    ExpOn[k] = expFunc( ExpOn.pos( k ), p );
}


void MembraneResistance::analyzeOff( double duration,
				     double sswidth, bool nossfit )
{
  // update averages:
  const InData &intrace = trace( SpikeTrace[0] );
  int inx = intrace.signalIndex() - MeanTrace.index( 0.0 );
  for ( int k=MeanTrace.index( duration ); k<MeanTrace.size() && inx+k<intrace.size(); k++ ) {
    double v = intrace[inx+k];
    MeanTrace[k] += (v - MeanTrace[k])/(Count+1);
    SquareTrace[k] += (v*v - SquareTrace[k])/(Count+1);
    StdevTrace[k] = sqrt( SquareTrace[k] - MeanTrace[k]*MeanTrace[k] );
    if ( CurrentTrace[0] >= 0 ) {
      double c = IInFac*trace( CurrentTrace[0] )[inx+k];
      MeanCurrent[k] += (c - MeanCurrent[k])/(Count+1);
    }
  }

  // fit exponential to offset:
  int inxon0 = MeanTrace.index( 0.0 );
  int inxon1 = VPeakInx;
  int inxoff0 = MeanTrace.index( duration );
  int inxoff1 = inxoff0 + inxon1 - inxon0;
  // guess time constant:
  double tau = 0.01;
  for ( int k = inxoff0; k<inxoff1; k++ ) {
    if ( (MeanTrace[k]-VRest)/(VSS-VRest) < 1.0/2.71828182845905 ) {
      tau = MeanTrace.interval( k - inxoff0 );
      break;
    }
  }
  ArrayD p( 3, 1.0 );
  p[0] = VSS-VRest;
  p[1] = -tau;
  p[2] = VRest;
  ArrayI pi( 3, 1 );
  if ( nossfit ) {
    pi[2] = 0;
    inxon1 = MeanTrace.index( duration );
  }
  ArrayD u( 3, 1.0 );
  double ch = 0.0;
  if ( inxoff1 > MeanTrace.size() )
    inxoff1 = MeanTrace.size();
  marquardtFit( MeanTrace.range().begin()+inxon0, MeanTrace.range().begin()+inxon1,
		MeanTrace.begin()+inxoff0, MeanTrace.begin()+inxoff1,
		StdevTrace.begin()+inxoff0, StdevTrace.begin()+inxoff1,
		expFuncDerivs, p, pi, u, ch );
  TauMOff = -1000.0*p[1];
  RMOff = ::fabs( (VSS - p[2])/Amplitude )*VFac/IFac;
  CMOff = TauMOff/RMOff*1000.0;
  for ( int k=0; k<ExpOff.size(); k++ )
    ExpOff[k] = expFunc( ExpOff.pos( k ) - duration, p );
}


void MembraneResistance::plot( void )
{
  P.lock();
  P.clear();
  P.setTitle( "R=" + Str( RMOn, 0, 2, 'f' ) +
	      " MOhm,  C=" + Str( CMOn, 0, 1, 'f' ) +
	      " pF,  tau=" + Str( TauMOn, 0, 1, 'f' ) + " ms" );
  P.plotVLine( 0, Plot::White, 2 );
  P.plotVLine( 1000.0*Duration, Plot::White, 2 );
  if ( boolean( "plotstdev" ) ) {
    P.plot( MeanTrace+StdevTrace, 1000.0, Plot::Orange, 1, Plot::Solid );
    P.plot( MeanTrace-StdevTrace, 1000.0, Plot::Orange, 1, Plot::Solid );
  }
  P.plot( MeanTrace, 1000.0, Plot::Red, 3, Plot::Solid );
  P.plot( ExpOn, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P.plot( ExpOff, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P.draw();
  P.unlock();
}


void MembraneResistance::save( void )
{
  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "ReProIndex", reproCount() );
  header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
  header.addInteger( "trials", Count );
  header.addNumber( "Vrest", VRest, VUnit, "%0.1f" );
  header.addNumber( "Vss", VSS, VUnit, "%0.1f" );
  header.addNumber( "Rss", RMss, "MOhm", "%0.1f" );
  header.addNumber( "Vpeak", VPeak, VUnit, "%0.1f" );
  header.addNumber( "Tpeak", 1000.0*VPeakTime, "ms", "%0.1f" );
  header.addNumber( "Rm", RMOn, "MOhm", "%0.1f" );
  header.addNumber( "Cm", CMOn, "pF", "%0.1f" );
  header.addNumber( "Taum", TauMOn, "ms", "%0.1f" );
  header.addNumber( "Roff", RMOff, "MOhm", "%0.1f" );
  header.addNumber( "Coff", CMOff, "pF", "%0.1f" );
  header.addNumber( "Tauoff", TauMOff, "ms", "%0.1f" );
  header.addNumber( "Vsag", fabs( VPeak-VSS ), VUnit, "%0.1f" );
  header.addNumber( "relVsag", 100.0*fabs( (VPeak-VSS)/(VSS-VRest) ), "%", "%0.1f" );
  lockStimulusData();
  header.newSection( stimulusData() );
  unlockStimulusData();
  header.newSection( settings() );

  saveData();
  saveTrace( header );
  saveExpFit( header );

  bool setdata = ( settings().index( "setdata" ) <= 1 );
  if ( settings().index( "setdata" ) == 0 ) {
    // all outputs must be at 0:
    lockStimulusData();
    for ( int k=0; k<outTracesSize(); k++ ) {
      if ( fabs( stimulusData().number( outTraceName( k ) ) ) > 1.0e-6 ) {
	setdata = false;
	break;
      }
    }
    unlockStimulusData();
  }

  if ( setdata ) {
    lockMetaData();
    metaData().setNumber( "Cell>vrest", 0.001*VRest, 0.001*VRestsd );
    metaData().setNumber( "Cell>rm", RMOn  );
    metaData().setNumber( "Cell>rmss", RMss  );
    metaData().setNumber( "Cell>cm", CMOn );
    metaData().setNumber( "Cell>taum", 0.001*TauMOn );
    unlockMetaData();
  }
}


void MembraneResistance::saveData( void )
{
  TableKey datakey;
  datakey.newSection( "Stimulus" );
  datakey.addNumber( "dI", IUnit, "%6.3f", Amplitude );
  datakey.addNumber( "IDC", IUnit, "%6.3f", DCCurrent );
  datakey.addNumber( "trials", "1", "%6.0f", (double)Count );
  datakey.addNumber( "duration", "ms", "%6.1f", 1000.0*Duration );
  datakey.newSection( "Rest" );
  datakey.addNumber( "Vrest", VUnit, "%6.1f", VRest );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", VRestsd );
  datakey.newSection( "Steady-state" );
  datakey.addNumber( "Vss", VUnit, "%6.1f", VSS );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", VSSsd );
  datakey.addNumber( "R", "MOhm", "%6.1f", RMss );
  datakey.newSection( "Peak" );
  datakey.addNumber( "Vpeak", VUnit, "%6.1f", VPeak );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", VPeaksd );
  datakey.addNumber( "tpeak", "ms", "%6.1f", 1000.0*VPeakTime );
  datakey.newSection( "Onset" );
  datakey.addNumber( "R", "MOhm", "%6.1f", RMOn );
  datakey.addNumber( "C", "pF", "%6.1f", CMOn );
  datakey.addNumber( "tau", "ms", "%6.1f", TauMOn );
  datakey.newSection( "Offset" );
  datakey.addNumber( "R", "MOhm", "%6.1f", RMOff );
  datakey.addNumber( "C", "pF", "%6.1f", CMOff );
  datakey.addNumber( "tau", "ms", "%6.1f", TauMOff );
  datakey.newSection( "Sag" );
  datakey.addNumber( "Vsag", VUnit, "%6.1f", fabs( VPeak-VSS ) );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", sqrt( VPeaksd*VPeaksd + VSSsd*VSSsd ) );
  datakey.addNumber( "relVsag", "%", "%6.1f", 100.0*fabs( (VPeak-VSS)/(VSS-VRest) ) );
  datakey.addNumber( "s.d.", "%", "%6.1f", 100.0*sqrt( VPeaksd*VPeaksd + fabs((VRest-VPeak)*VSSsd/(VSS-VRest))*fabs((VRest-VPeak)*VSSsd/(VSS-VRest)) )/fabs(VSS-VRest) );
  datakey.newSection( "Status" );
  lockStimulusData();
  datakey.add( stimulusData() );
  unlockStimulusData();

  ofstream df;
  if ( completeRuns() <= 0 ) {
    df.open( addPath( "membraneresistance-data.dat" ).c_str() );
    datakey.saveKey( df );
  }
  else
    df.open( addPath( "membraneresistance-data.dat" ).c_str(),
	     ofstream::out | ofstream::app );

  datakey.saveData( df );
}


void MembraneResistance::saveTrace( const Options &header )
{
  ofstream df( addPath( "membraneresistance-trace.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "t", "ms", "%6.2f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.addNumber( "s.d.", VUnit, "%6.2f" );
  if ( ! MeanCurrent.empty() )
    datakey.addNumber( "I", IUnit, "%6.3f" );
  datakey.saveKey( df );

  for ( int k=0; k<MeanTrace.size(); k++ ) {
    datakey.save( df, 1000.0*MeanTrace.pos( k ), 0 );
    datakey.save( df, MeanTrace[k] );
    datakey.save( df, StdevTrace[k] );
    if ( ! MeanCurrent.empty() )
      datakey.save( df, MeanCurrent[k] );
    df << '\n';
  }
  
  df << "\n\n";
}


void MembraneResistance::saveExpFit( const Options &header )
{
  ofstream df( addPath( "membraneresistance-expfit.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << '\n';

  TableKey datakey;
  datakey.newSection( "Onset" );
  datakey.addNumber( "t", "ms", "%6.1f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.newSection( "Offset" );
  datakey.addNumber( "t", "ms", "%6.1f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.saveKey( df );

  for ( int k=0; k<ExpOn.size() &&  k<ExpOff.size(); k++ ) {
    datakey.save( df, 1000.0*ExpOn.pos( k ), 0 );
    datakey.save( df, ExpOn[k] );
    datakey.save( df, 1000.0*ExpOff.pos( k ) );
    datakey.save( df, ExpOff[k] );
    df << '\n';
  }
  
  df << "\n\n";
}


addRePro( MembraneResistance, patchclamp );

}; /* namespace patchclamp */

#include "moc_membraneresistance.cc"
