/*
  patchclamp/membraneresistance.cc
  Measures membrane resistance, capacitance, and time constant with current pulses

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
#include <relacs/fitalgorithm.h>
#include <relacs/tablekey.h>
#include <relacs/patchclamp/membraneresistance.h>
using namespace relacs;

namespace patchclamp {


MembraneResistance::MembraneResistance( void )
  : RePro( "MembraneResistance", "MembraneResistance", "Patch-clamp",
	   "Jan Benda", "1.0", "Nov 12, 2009" ),
    P( this ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    VFac( 1.0 ),
    IFac( 1.0 )
{
  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addSelection( "outtrace", "Output trace", "Speaker-1" );
  addNumber( "amplitude", "Amplitude of output signal", -1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 10, 0, 10000, 1 );
  addNumber( "sswidth", "Window length for steady-state analysis", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addBoolean( "nossfit", "Fix steady-state potential for fit", true );
  addBoolean( "plotstdev", "Plot standard deviation of membrane potential", true );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.unlock();
}


void MembraneResistance::config( void )
{
  setText( "intrace", spikeTraceNames() );
  setToDefault( "intrace" );
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
}


void MembraneResistance::notify( void )
{
  int outtrace = index( "outtrace" );
  IUnit = outTrace( outtrace ).unit();
  setUnit( "amplitude", IUnit );
  IFac = Parameter::changeUnit( 1.0, IUnit, "nA" );

  int intrace = index( "intrace" );
  VUnit = trace( intrace ).unit();
  VFac = Parameter::changeUnit( 1.0, VUnit, "mV" );
}


int MembraneResistance::main( void )
{
  // get options:
  int intrace = traceIndex( text( "intrace", 0 ) );
  int outtrace = index( "outtrace" );
  Amplitude = number( "amplitude" );
  Duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double sswidth = number( "sswidth" );
  if ( pause < 2.0*Duration ) {
    warning( "Pause must be at least two times the stimulus duration!" );
    return Failed;
  }
  bool nossfit = boolean( "nossfit" );

  double samplerate = trace( intrace ).sampleRate();

  // don't print repro message:
  noMessage();

  // init:
  MeanTrace = SampleDataD( -0.5*Duration, 2.0*Duration, 1/samplerate, 0.0 );
  SquareTrace = MeanTrace;
  StdevTrace = MeanTrace;
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
  ExpOn = SampleDataD( 0.0, Duration, 1/samplerate, 0.0 );
  ExpOff = SampleDataD( Duration, 2.0*Duration, 1/samplerate, 0.0 );

  // plot trace:
  plotToggle( true, true, 2.0*Duration, 0.5*Duration );

  // plot:
  P.lock();
  P.setXRange( -500.0*Duration, 2000.0*Duration );
  P.setYLabel( trace( intrace ).ident() + " [" + VUnit + "]" );
  /*
  P.setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		  Plot::Center, -90.0 );
  */
  P.unlock();

  // signal:
  OutData signal( Duration, 1.0/samplerate );
  signal = Amplitude;
  signal.setIdent( "const" );
  signal.back() = 0.0;
  signal.setTrace( outtrace );

  // write stimulus:
  sleep( pause );
  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {

    Str s = "Amplitude <b>" + Str( Amplitude ) + " " + IUnit +"</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    message( s );

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }

    sleep( Duration );
    if ( interrupt() ) {
      if ( count > 0 )
	break;
      else
	return Aborted;
    }

    timeStamp();
    analyzeOn( trace( intrace ), Duration, count, sswidth, nossfit );

    sleepOn( Duration + 0.01 );
    if ( interrupt() ) {
      if ( count > 0 )
	break;
      else
	return Aborted;
    }

    analyzeOff( trace( intrace ), Duration, count, sswidth, nossfit );
    plot();
    sleepOn( pause );
    if ( interrupt() ) {
      if ( count > 0 )
	break;
      else
	return Aborted;
    }

  }

  save();

  return Completed;
}


void MembraneResistance::analyzeOn( const InData &trace, 
				    double duration, int count,
				    double sswidth, bool nossfit )
{
  // update averages:
  int inx = trace.signalIndex() - MeanTrace.index( 0.0 );
  for ( int k=0; k<MeanTrace.index( duration ); k++ ) {
    double v = trace[inx+k];
    MeanTrace[k] += (v - MeanTrace[k])/(count+1);
    SquareTrace[k] += (v*v - SquareTrace[k])/(count+1);
    StdevTrace[k] = sqrt( SquareTrace[k] - MeanTrace[k]*MeanTrace[k] );
  }

  // resting potential:
  VRest = MeanTrace.mean( -duration, 0.0 );
  VRestsd = MeanTrace.stdev( -duration, 0.0 );

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
  if ( fabs( VPeak - VSS ) <= VSSsd ) {
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
  ArrayD p( 3, 1.0 );
  p[0] = VRest-VSS;
  p[1] = -0.01;
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


void MembraneResistance::analyzeOff( const InData &trace, 
				     double duration, int count,
				     double sswidth, bool nossfit )
{
  // update averages:
  int inx = trace.signalIndex() - MeanTrace.index( 0.0 );
  for ( int k=MeanTrace.index( duration ); k<MeanTrace.size(); k++ ) {
    double v = trace[inx+k];
    MeanTrace[k] += (v - MeanTrace[k])/(count+1);
    SquareTrace[k] += (v*v - SquareTrace[k])/(count+1);
    StdevTrace[k] = sqrt( SquareTrace[k] - MeanTrace[k]*MeanTrace[k] );
  }

  // fit exponential to offset:
  int inxon0 = MeanTrace.index( 0.0 );
  int inxon1 = VPeakInx;
  ArrayD p( 3, 1.0 );
  p[0] = VSS-VRest;
  p[1] = -0.01;
  p[2] = VRest;
  ArrayI pi( 3, 1 );
  if ( nossfit ) {
    pi[2] = 0;
    inxon1 = MeanTrace.index( duration );
  }
  ArrayD u( 3, 1.0 );
  double ch = 0.0;
  int inxoff0 = MeanTrace.index( duration );
  int inxoff1 = inxoff0 + inxon1 - inxon0;
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
  P.setTitle( "R=" + Str( RMOn, 0, 1, 'f' ) +
	      " MOhm,  C=" + Str( CMOn, 0, 1, 'f' ) +
	      " pF,  tau=" + Str( TauMOn, 0, 1, 'f' ) + " ms" );
  P.plotVLine( 0, Plot::White, 2 );
  P.plotVLine( 1000.0*Duration, Plot::White, 2 );
  P.plot( MeanTrace, 1000.0, Plot::Red, 3, Plot::Solid );
  if ( boolean( "plotstdev" ) ) {
      P.plot( MeanTrace+StdevTrace, 1000.0, Plot::Orange, 1, Plot::Solid );
      P.plot( MeanTrace-StdevTrace, 1000.0, Plot::Orange, 1, Plot::Solid );
    }
  P.plot( ExpOn, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P.plot( ExpOff, 1000.0, Plot::Yellow, 2, Plot::Solid );
  P.unlock();
  P.draw();
}


void MembraneResistance::save( void )
{
  saveData();
  saveTrace();
  saveExpFit();
}


void MembraneResistance::saveData( void )
{
  TableKey datakey;
  datakey.addLabel( "Stimulus" );
  datakey.addNumber( "I", IUnit, "%6.1f", Amplitude );
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

  ofstream df;
  if ( completeRuns() <= 0 ) {
    df.open( addPath( "membraneresistance.dat" ).c_str() );
    datakey.saveKey( df );
  }
  else
    df.open( addPath( "membraneresistance.dat" ).c_str(),
	     ofstream::out | ofstream::app );

  datakey.saveData( df );
}


void MembraneResistance::saveTrace( void )
{
  ofstream df( addPath( "membraneresistancetrace.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  settings().save( df, "# " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "t", "ms", "%6.1f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.addNumber( "s.d.", VUnit, "%6.2f" );
  datakey.saveKey( df );

  for ( int k=0; k<MeanTrace.size(); k++ ) {
    datakey.save( df, 1000.0*MeanTrace.pos( k ), 0 );
    datakey.save( df, MeanTrace[k] );
    datakey.save( df, StdevTrace[k] );
    df << '\n';
  }
  
  df << "\n\n";
}


void MembraneResistance::saveExpFit( void )
{
  ofstream df( addPath( "membraneresistanceexpfit.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  settings().save( df, "# " );
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


addRePro( MembraneResistance );

}; /* namespace patchclamp */

#include "moc_membraneresistance.cc"
