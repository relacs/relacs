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
  : RePro( "MembraneResistance", "patchclamp", "Jan Benda", "1.2", "Sep 25, 2014" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    VFac( 1.0 ),
    IFac( 1.0 ),
    IInFac( 1.0 )
{
  // add some options:
  addLabel( "Stimulus" );
  addNumber( "amplitude", "Amplitude of output signal", -1.0, -1000.0, 1000.0, 0.1 ).setActivation( "userm", "false" );
  addBoolean( "userm", "Compute amplitude from vstep and estimated membrane resistance", false );
  addNumber( "vstep", "Steady-state voltage amplitude induced by output signal", -1.0, -1000.0, 1000.0, 0.1, "mV" ).setActivation( "userm", "true" );
  addNumber( "duration", "Duration of output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 );
  addLabel( "Analysis" );
  addNumber( "sswidth", "Window length for steady-state analysis", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addBoolean( "nossfit", "Fix steady-state potential for fit", true );
  addBoolean( "plotstdev", "Plot standard deviation of membrane potential", true );
  addSelection( "setdata", "Set results to the session variables", "rest only|always|never" );
  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
  setWidget( &P );
}


void MembraneResistance::config( void )
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
  if ( CurrentOutput[0] < 0 ) {
    warning( "Invalid output current trace!" );
    return Failed;
  }
  if ( userm ) {
    double rm = metaData( "Cell" ).number( "rm", 0.0, "MOhm" );
    if ( rm > 1.0e-8 ) {
      double g = stimulusData().number( "g", 0.0, "ns" );
      double r = 1.0/(0.001*g + 1.0/rm);
      Amplitude = vstep/r/IFac;
    }
  }

  double samplerate = trace( SpikeTrace[0] ).sampleRate();

  // don't print repro message:
  noMessage();

  // init:
  DoneState state = Completed;
  MeanVoltage = SampleDataF( -0.5*Duration, 2.0*Duration, 1/samplerate, 0.0 );
  SquareVoltage = MeanVoltage;
  StdevVoltage = MeanVoltage;
  TraceIndices.clear();
  MeanTraces.clear();
  SquareTraces.clear();
  for ( int j=0; j<traces().size(); j++ ) {
    if ( j != SpikeTrace[0] &&
	 ::fabs( trace( j ).stepsize() - trace( SpikeTrace[0] ).stepsize() )/trace( SpikeTrace[0] ).stepsize() < 1e-8 ) {
      TraceIndices.push_back( j );
      MeanTraces.push_back( MeanVoltage );
      SquareTraces.push_back( MeanVoltage );
    }
  }
  DCCurrent = stimulusData().number( outTraceName( CurrentOutput[0] ) );
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

  // plot:
  P.lock();
  P.setXRange( -500.0*Duration, 2000.0*Duration );
  P.setYLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]" );
  P.unlock();

  // signal:
  OutData signal( Duration, 1.0/samplerate );
  signal = DCCurrent + Amplitude;
  signal.setIdent( "I=" + Str( DCCurrent + Amplitude ) + IUnit );
  signal.back() = DCCurrent;
  signal.setTrace( CurrentOutput[0] );
  signal.addDescription( "stimulus/pulse" );
  signal.description().addNumber( "Intensity", DCCurrent + Amplitude, IUnit );
  signal.description().addNumber( "IntensityOffset", DCCurrent, IUnit );
  signal.description().addNumber( "Duration", 1000.0*Duration, "ms" );

  // dc signal:
  OutData dcsignal( DCCurrent );
  dcsignal.setTrace( CurrentOutput[0] );
  dcsignal.setIdent( "DC=" + Str( DCCurrent ) + IUnit );
  dcsignal.addDescription( "stimulus/value" );
  dcsignal.description().addNumber( "Intensity", DCCurrent, IUnit );

  // sleep:
  sleep( pause );
  if ( interrupt() )
    return Aborted;

  // plot trace:
  tracePlotSignal( 2.0*Duration, 0.5*Duration );

  // write stimulus:
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
    sleep( Duration + 0.01 );
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
  int inx = intrace.signalIndex() - MeanVoltage.index( 0.0 );
  for ( int k=0; k<MeanVoltage.index( duration ) && inx+k<intrace.size(); k++ ) {
    double v = intrace[inx+k];
    MeanVoltage[k] += (v - MeanVoltage[k])/(Count+1);
    SquareVoltage[k] += (v*v - SquareVoltage[k])/(Count+1);
    StdevVoltage[k] = sqrt( SquareVoltage[k] - MeanVoltage[k]*MeanVoltage[k] );
  }

  unlockAll();

  // resting potential:
  VRest = MeanVoltage.mean( -sswidth, 0.0 );
  VRestsd = MeanVoltage.stdev( -sswidth, 0.0 );

  // steady-state potential:
  VSS = MeanVoltage.mean( duration-sswidth, duration );
  VSSsd = MeanVoltage.stdev( duration-sswidth, duration );

  // membrane resitance:
  RMss = ::fabs( (VSS - VRest)/Amplitude )*VFac/IFac;
  if ( RMss <= 0.0 && RMss > 1.0e10 )
    RMss = 0.0;

  // peak potential:
  VPeak = VRest;
  VPeakInx = 0;
  if ( VSS > VRest )
    VPeakInx = MeanVoltage.maxIndex( VPeak, 0.0, duration-sswidth );
  else
    VPeakInx = MeanVoltage.minIndex( VPeak, 0.0, duration-sswidth );
  VPeaksd = StdevVoltage[VPeakInx];
  if ( fabs( VPeak - VSS ) <= 4.0*VSSsd || VPeakInx > MeanVoltage.index( duration-sswidth ) ) {
    VPeak = VSS;
    VPeaksd = VSSsd;
    VPeakInx = MeanVoltage.index( duration );
    VPeakTime = 0.0;
  }
  else
    VPeakTime = MeanVoltage.pos( VPeakInx );


  // fit exponential to onset:
  int inxon0 = MeanVoltage.index( 0.0 );
  int inxon1 = VPeakInx;
  // guess time constant:
  double tau = 0.01;
  for ( int k = inxon0; k<VPeakInx; k++ ) {
    if ( (MeanVoltage[k]-VSS)/(VRest-VSS) < 1.0/2.71828182845905 ) {
      tau = MeanVoltage.pos( k );
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
    inxon1 = MeanVoltage.index( duration );
  }
  ArrayD u( 3, 1.0 );
  double ch = 0.0;
  marquardtFit( MeanVoltage.range().begin()+inxon0, MeanVoltage.range().begin()+inxon1,
		MeanVoltage.begin()+inxon0, MeanVoltage.begin()+inxon1,
		StdevVoltage.begin()+inxon0, StdevVoltage.begin()+inxon1,
		expFuncDerivs, p, pi, u, ch );
  TauMOn = -1000.0*p[1];
  if ( TauMOn <= 0.0 && TauMOn > 1.0e5 )
    TauMOn = 0.0;
  RMOn = ::fabs( (p[2] - VRest)/Amplitude )*VFac/IFac;
  if ( RMOn <= 0.0 && RMOn > 1.0e10 ) {
    RMOn = 0.0;
    CMOn = 0.0;
  }
  else
    CMOn = TauMOn/RMOn*1000.0;
  for ( int k=0; k<ExpOn.size(); k++ )
    ExpOn[k] = expFunc( ExpOn.pos( k ), p );

  lockAll();
}


void MembraneResistance::analyzeOff( double duration,
				     double sswidth, bool nossfit )
{
  // update averages:
  const InData &intrace = trace( SpikeTrace[0] );
  int inx = intrace.signalIndex() - MeanVoltage.index( 0.0 );
  for ( int k=MeanVoltage.index( duration );
	k<MeanVoltage.size() && inx+k<intrace.size();
	k++ ) {
    double v = intrace[inx+k];
    MeanVoltage[k] += (v - MeanVoltage[k])/(Count+1);
    SquareVoltage[k] += (v*v - SquareVoltage[k])/(Count+1);
    StdevVoltage[k] = sqrt( SquareVoltage[k] - MeanVoltage[k]*MeanVoltage[k] );
  }

  for ( unsigned int j=0; j<TraceIndices.size(); j++ ) {
    const InData &intrace2 = trace( TraceIndices[j] );
    for ( int k=0; k<MeanTraces[j].size() && inx+k<intrace2.size(); k++ ) {
      double v = intrace2[inx+k];
      if ( TraceIndices[j] == CurrentTrace[0] )
	v *= IInFac;
      MeanTraces[j][k] += (v - MeanTraces[j][k])/(Count+1);
      SquareTraces[j][k] += (v*v - SquareTraces[j][k])/(Count+1);
    }
  }

  unlockAll();

  // fit exponential to offset:
  int inxon0 = MeanVoltage.index( 0.0 );
  int inxon1 = VPeakInx;
  int inxoff0 = MeanVoltage.index( duration );
  int inxoff1 = inxoff0 + inxon1 - inxon0;
  // guess time constant:
  double tau = 0.01;
  for ( int k = inxoff0; k<inxoff1; k++ ) {
    if ( (MeanVoltage[k]-VRest)/(VSS-VRest) < 1.0/2.71828182845905 ) {
      tau = MeanVoltage.interval( k - inxoff0 );
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
    inxon1 = MeanVoltage.index( duration );
  }
  ArrayD u( 3, 1.0 );
  double ch = 0.0;
  if ( inxoff1 > MeanVoltage.size() )
    inxoff1 = MeanVoltage.size();
  marquardtFit( MeanVoltage.range().begin()+inxon0, MeanVoltage.range().begin()+inxon1,
		MeanVoltage.begin()+inxoff0, MeanVoltage.begin()+inxoff1,
		StdevVoltage.begin()+inxoff0, StdevVoltage.begin()+inxoff1,
		expFuncDerivs, p, pi, u, ch );
  TauMOff = -1000.0*p[1];
  if ( TauMOff <= 0.0 && TauMOff > 1.0e5 )
    TauMOff = 0.0;
  RMOff = ::fabs( (VSS - p[2])/Amplitude )*VFac/IFac;
  if ( RMOff <= 0.0 && RMOff > 1.0e10 ) {
    RMOff = 0.0;
    CMOff = 0.0;
  }
  else
    CMOff = TauMOff/RMOff*1000.0;
  for ( int k=0; k<ExpOff.size(); k++ )
    ExpOff[k] = expFunc( ExpOff.pos( k ) - duration, p );

  lockAll();
}


void MembraneResistance::plot( void )
{
  P.lock();
  P.clear();
  P.setTitle( "R=" + Str( RMOn, 0, 0, 'f' ) +
	      " MOhm,  C=" + Str( CMOn, 0, 0, 'f' ) +
	      " pF,  tau=" + Str( TauMOn, 0, 0, 'f' ) + " ms" );
  P.plotVLine( 0, Plot::White, 2 );
  P.plotVLine( 1000.0*Duration, Plot::White, 2 );
  if ( boolean( "plotstdev" ) ) {
    P.plot( MeanVoltage+StdevVoltage, 1000.0, Plot::Orange, 1, Plot::Solid );
    P.plot( MeanVoltage-StdevVoltage, 1000.0, Plot::Orange, 1, Plot::Solid );
  }
  P.plot( MeanVoltage, 1000.0, Plot::Red, 3, Plot::Solid );
  P.plot( ExpOn, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P.plot( ExpOff, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P.draw();
  P.unlock();
}


void MembraneResistance::save( void )
{
  unlockAll();

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

  saveData();
  saveTrace( header );
  saveExpFit( header );

  bool setdata = ( settings().index( "setdata" ) <= 1 );
  if ( settings().index( "setdata" ) == 0 ) {
    // all outputs must be at 0:
    for ( int k=0; k<outTracesSize(); k++ ) {
      if ( fabs( stimulusData().number( outTraceName( k ) ) ) > 1.0e-6 ) {
	setdata = false;
	break;
      }
    }
  }

  lockAll();

  if ( setdata ) {
    metaData().setNumber( "Cell>vrest", 0.001*VRest, 0.001*VRestsd );
    metaData().setNumber( "Cell>rm", RMOn  );
    metaData().setNumber( "Cell>rmss", RMss  );
    metaData().setNumber( "Cell>cm", CMOn );
    metaData().setNumber( "Cell>taum", 0.001*TauMOn );
  }
}


void MembraneResistance::saveData( void )
{
  TableKey datakey;
  datakey.addLabel( "Stimulus" );
  datakey.addNumber( "dI", IUnit, "%6.3f", Amplitude );
  datakey.addNumber( "IDC", IUnit, "%6.3f", DCCurrent );
  datakey.addNumber( "trials", "1", "%6.0f", (double)Count );
  datakey.addNumber( "duration", "ms", "%6.1f", 1000.0*Duration );
  datakey.addLabel( "Rest" );
  datakey.addNumber( "Vrest", VUnit, "%6.1f", VRest );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", VRestsd );
  datakey.addLabel( "Steady-state" );
  datakey.addNumber( "Vss", VUnit, "%6.1f", VSS );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", VSSsd );
  datakey.addNumber( "R", "MOhm", "%6.1f", RMss );
  datakey.addLabel( "Peak" );
  datakey.addNumber( "Vpeak", VUnit, "%6.1f", VPeak );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", VPeaksd );
  datakey.addNumber( "tpeak", "ms", "%6.1f", 1000.0*VPeakTime );
  datakey.addLabel( "Onset" );
  datakey.addNumber( "R", "MOhm", "%6.1f", RMOn );
  datakey.addNumber( "C", "pF", "%6.1f", CMOn );
  datakey.addNumber( "tau", "ms", "%6.1f", TauMOn );
  datakey.addLabel( "Offset" );
  datakey.addNumber( "R", "MOhm", "%6.1f", RMOff );
  datakey.addNumber( "C", "pF", "%6.1f", CMOff );
  datakey.addNumber( "tau", "ms", "%6.1f", TauMOff );
  datakey.addLabel( "Sag" );
  datakey.addNumber( "Vsag", VUnit, "%6.1f", fabs( VPeak-VSS ) );
  datakey.addNumber( "s.d.", VUnit, "%6.1f", sqrt( VPeaksd*VPeaksd + VSSsd*VSSsd ) );
  datakey.addNumber( "relVsag", "%", "%6.1f", 100.0*fabs( (VPeak-VSS)/(VSS-VRest) ) );
  datakey.addNumber( "s.d.", "%", "%6.1f", 100.0*sqrt( VPeaksd*VPeaksd + fabs((VRest-VPeak)*VSSsd/(VSS-VRest))*fabs((VRest-VPeak)*VSSsd/(VSS-VRest)) )/fabs(VSS-VRest) );
  datakey.addLabel( "Status" );
  datakey.add( stimulusData() );

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
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "t", "ms", "%7.2f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.addNumber( "s.d.", VUnit, "%6.2f" );
  for ( unsigned int j=0; j<TraceIndices.size(); j++ ) {
    int i = TraceIndices[j];
    if ( i == CurrentTrace[0] ) {
      datakey.addNumber( "I", IUnit, "%6.3f" );
      datakey.addNumber( "s.d.", IUnit, "%6.3f" );
    }
    else {
      datakey.addNumber( trace( i ).ident(), trace( i ).unit(), "%7.3f" );
      datakey.addNumber( "s.d.", trace( i ).unit(), "%7.3f" );
    }
  }
  datakey.saveKey( df );

  for ( int k=0; k<MeanVoltage.size(); k++ ) {
    datakey.save( df, 1000.0*MeanVoltage.pos( k ), 0 );
    datakey.save( df, MeanVoltage[k] );
    datakey.save( df, StdevVoltage[k] );
    for ( unsigned int j=0; j<MeanTraces.size(); j++ ) {
      datakey.save( df, MeanTraces[j][k] );
      datakey.save( df, sqrt( SquareTraces[j][k] - MeanTraces[j][k]*MeanTraces[j][k] ) );
    }
    df << '\n';
  }
  
  df << "\n\n";
}


void MembraneResistance::saveExpFit( const Options &header )
{
  ofstream df( addPath( "membraneresistance-expfit.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addLabel( "Onset" );
  datakey.addNumber( "t", "ms", "%6.1f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.addLabel( "Offset" );
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
