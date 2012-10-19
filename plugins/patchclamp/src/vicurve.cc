/*
  patchclamp/vicurve.cc
  V-I curve measured in current-clamp

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
#include <relacs/patchclamp/vicurve.h>
using namespace relacs;

namespace patchclamp {


VICurve::VICurve( void )
  : RePro( "VICurve", "patchclamp", "Jan Benda", "1.1", "Nov 03, 2010" ),
    VUnit( "mV" ),
    IUnit( "nA" ),
    VFac( 1.0 ),
    IFac( 1.0 ),
    IInFac( 1.0 )
{
  // add some options:
  addSection( "Stimuli" );
  addSelection( "ibase", "Currents are relative to", "zero|DC|threshold" );
  addNumber( "imin", "Minimum injected current", -1.0, -1000.0, 1000.0, 0.001 );
  addNumber( "imax", "Maximum injected current", 1.0, -1000.0, 1000.0, 0.001 );
  addNumber( "istep", "Minimum step-size of current", 0.001, 0.001, 1000.0, 0.001 ).setActivation( "userm", "false" );
  addBoolean( "userm", "Use membrane resistance for estimating istep from vstep", false );
  addNumber( "vstep", "Minimum step-size of voltage", 1.0, 0.001, 10000.0, 0.1 ).setActivation( "userm", "true" );
  addSection( "Timing" );
  addNumber( "duration", "Duration of current output", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "delay", "Delay before current pulses", 0.1, 0.001, 10.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Duration of pause between current pulses", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addSelection( "ishuffle", "Initial sequence of currents for first repetition", RangeLoop::sequenceStrings() );
  addSelection( "shuffle", "Sequence of currents", RangeLoop::sequenceStrings() );
  addInteger( "iincrement", "Initial increment for currents", -1, -1000, 1000, 1 );
  addInteger( "singlerepeat", "Number of immediate repetitions of a single stimulus", 1, 1, 10000, 1 );
  addInteger( "blockrepeat", "Number of repetitions of a fixed intensity increment", 10, 1, 10000, 1 );
  addInteger( "repeat", "Number of repetitions of the whole V-I curve measurement", 1, 0, 10000, 1 );
  addSection( "Analysis" );
  addNumber( "vmin", "Minimum value for membrane voltage", -100.0, -1000.0, 1000.0, 1.0 );
  addNumber( "sswidth", "Window length for steady-state analysis", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addNumber( "ton", "Timepoint of onset-voltage measurement", 0.01, 0.0, 100.0, 0.001, "sec", "ms" );
  addBoolean( "plotstdev", "Plot standard deviation of membrane potential", true );
  addValueTypeStyle( OptWidget::TabLabel, Parameter::Label );

  P.lock();
  P.resize( 2, 2, true );
  P.unlock();
  setWidget( &P );
}


void VICurve::config( void )
{
  if ( SpikeTrace[0] >= 0 ) {
    VUnit = trace( SpikeTrace[0] ).unit();
    VFac = Parameter::changeUnit( 1.0, VUnit, "mV" );
    setUnit( "vstep", VUnit );
    setUnit( "vmin", VUnit );
  }

  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "imin", IUnit );
    setUnit( "imax", IUnit );
    setUnit( "istep", IUnit );
    IFac = Parameter::changeUnit( 1.0, IUnit, "nA" );
  }

  if ( CurrentTrace[0] >= 0 ) {
    string iinunit = trace( CurrentTrace[0] ).unit();
    IInFac = Parameter::changeUnit( 1.0, iinunit, IUnit );
  }
}


int VICurve::main( void )
{
  Header.clear();
  Header.addInteger( "index", completeRuns() );
  Header.addInteger( "ReProIndex", reproCount() );
  Header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );

  // get options:
  int ibase = index( "ibase" );
  double imin = number( "imin" );
  double imax = number( "imax" );
  double istep = number( "istep" );
  bool userm = boolean( "userm" );
  double vstep = number( "vstep" );
  RangeLoop::Sequence shuffle = RangeLoop::Sequence( index( "shuffle" ) );
  RangeLoop::Sequence ishuffle = RangeLoop::Sequence( index( "ishuffle" ) );
  int iincrement = integer( "iincrement" );
  int singlerepeat = integer( "singlerepeat" );
  int blockrepeat = integer( "blockrepeat" );
  int repeat = integer( "repeat" );
  double duration = number( "duration" );
  double delay = number( "delay" );
  double pause = number( "pause" );
  double vmin = number( "vmin" );
  double ton = number( "ton" );
  double sswidth = number( "sswidth" );
  double dccurrent = stimulusData().number( outTraceName( CurrentOutput[0] ) );
  if ( ibase == 1 ) {
    imin += dccurrent;
    imax += dccurrent;
  }
  else if ( ibase == 2 ) {
    double ithresh = metaData( "Cell" ).number( "ithreshon" );
    if ( ithresh == 0.0 )
      ithresh = metaData( "Cell" ).number( "ithreshss" );
    imin += ithresh;
    imax += ithresh;
  }
  if ( imax <= imin ) {
    warning( "imin must be smaller than imax!" );
    return Failed;
  }
  if ( pause < duration ) {
    warning( "Pause must be at least as long as the stimulus duration!" );
    return Failed;
  }
  if ( pause < delay ) {
    warning( "Pause must be at least as long as the delay!" );
    return Failed;
  }
  if ( sswidth >= duration ) {
    warning( "sswidth must be smaller than stimulus duration!" );
    return Failed;
  }
  if ( SpikeTrace[0] < 0 || SpikeEvents[0] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }
  if ( userm ) {
    double rm = metaData( "Cell" ).number( "rmss", "MOhm" );
    if ( rm <= 0 )
      rm = metaData( "Cell" ).number( "rm", "MOhm" );
    if ( rm <= 0 )
      warning( "Membrane resistance was not measured yet!" );
    else {
      Header.addNumber( "rm", rm, "MOhm" );
      vstep = Parameter::changeUnit( vstep, VUnit, "mV" ); 
      double ifac = Parameter::changeUnit( 1.0, "nA", IUnit ); 
      istep = ifac*vstep/rm;
    }
  }
  Header.addNumber( "istep", istep, IUnit );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( 2.0*duration+delay, delay );

  // init:
  DoneState state = Completed;
  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  Range.set( imin, imax, istep, repeat, blockrepeat, singlerepeat );
  Range.setIncrement( iincrement );
  Range.setSequence( ishuffle );
  int prevrepeat = 0;
  Results.clear();
  Results.resize( Range.size(), Data() );

  // plot:
  P.lock();
  P[0].clear();
  P[0].setXLabel( "Time [ms]" );
  P[0].setXRange( -1000.0*delay, 2000.0*duration );
  P[0].setYLabel( "Membrane potential [" + VUnit + "]" );
  P[1].clear();
  P[1].setXLabel( "Current [" + IUnit + "]" );
  P[1].setXRange( imin, imax );
  P[1].setYLabel( "Membrane potential [" + VUnit + "]" );
  P.unlock();

  // signal:
  OutData signal( duration, 1.0/samplerate );
  signal.setTrace( CurrentOutput[0] );
  signal.setDelay( delay );
  signal.addDescription( "stimulus/pulse" );
  signal.description().addNumber( "Intensity", dccurrent, IUnit );
  signal.description().addNumber( "IntensityOffset", dccurrent, IUnit );
  signal.description().addNumber( "Duration", 1000.0*duration, "ms" );

  // dc signal:
  OutData dcsignal( dccurrent );
  dcsignal.setTrace( CurrentOutput[0] );
  dcsignal.setIdent( "DC=" + Str( dccurrent ) + IUnit );
  dcsignal.addDescription( "stimulus/value" );
  dcsignal.description().addNumber( "Intensity", dccurrent, IUnit );

  // write stimulus:
  sleep( pause );
  if ( interrupt() )
    return Aborted;
  for ( Range.reset(); ! Range && softStop() == 0; ) {

    if ( prevrepeat < Range.currentRepetition() ) {
      if ( Range.currentRepetition() == 1 ) {
	Range.setSequence( shuffle );
	Range.update();
	if ( ! !Range ) {
	  directWrite( dcsignal );
	  break;
	}
      }
      prevrepeat = Range.currentRepetition();
    }

    double amplitude = *Range;
    if ( fabs( amplitude ) < 1.0e-8 )
      amplitude = 0.0;

    Str s = "Increment <b>" + Str( Range.currentIncrementValue() ) + " " + IUnit + "</b>";
    s += ",  Current <b>" + Str( amplitude ) + " " + IUnit +"</b>";
    s += ",  Count <b>" + Str( Range.count()+1 ) + "</b>";
    message( s );

    timeStamp();
    signal.setIdent( "I=" + Str( amplitude ) + IUnit );
    signal = amplitude;
    signal.back() = dccurrent;
    signal.description().setNumber( "Intensity", amplitude, IUnit );
    write( signal );
    if ( signal.failed() ) {
      if ( signal.overflow() ) {
	printlog( "Requested amplitude I=" + Str( amplitude ) + IUnit + "too high!" );
	for ( int k = Range.size()-1; k >= 0; k-- ) {
	  if ( Range[k] > signal.maxValue() || k == Range.pos() )
	    Range.setSkip( k );
	  else {
	    directWrite( dcsignal );
	    break;
	  }
	}
	Range.noCount();
	continue;
      }
      else if ( signal.underflow() ) {
	printlog( "Requested amplitude I=" + Str( amplitude ) + IUnit + "too small!" );
	for ( int k = 0; k < Range.size(); k++ ) {
	  if ( Range[k] < signal.minValue() || k == Range.pos() )
	    Range.setSkip( k );
	  else {
	    directWrite( dcsignal );
	    break;
	  }
	}
	Range.noCount();
	continue;
      }
      else {
	warning( signal.errorText() );
	directWrite( dcsignal );
	return Failed;
      }
    }

    sleep( delay + 2.0*duration + 0.01 );
    if ( interrupt() ) {
      if ( Range.count() < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }

    Results[Range.pos()].I = amplitude;
    Results[Range.pos()].DC = dccurrent;
    Results[Range.pos()].analyze( Range.count(), trace( 0 ),
				  events( SpikeEvents[0] ), 
				  CurrentTrace[0] >= 0 ? &trace( CurrentTrace[0] ) : 0,
				  IInFac, delay, duration, ton, sswidth );

    if ( Results[Range.pos()].VSS < vmin ) {
      Range.setSkipBelow( Range.pos() );
      Range.noCount();
    }
    if ( Results[Range.pos()].SpikeCount.back() > 1.0 ) {
      Range.setSkipAbove( Range.pos() );
      Range.noCount();
    }

    int cinx = Range.pos();
    ++Range;

    plot( duration, cinx );
    sleepOn( duration + pause );
    if ( interrupt() ) {
      if ( Range.count() < 1 )
	state = Aborted;
      directWrite( dcsignal );
      break;
    }
  }

  if ( state == Completed )
    save();

  return state;
}


void VICurve::plot( double duration, int inx )
{
  P.lock();

  // membrane voltage:
  const Data &data = Results[inx];
  P[0].clear();
  P[0].setTitle( "I=" + Str( data.I, 0, 2, 'f' ) + IUnit );
  P[0].plotVLine( 0, Plot::White, 2 );
  P[0].plotVLine( 1000.0*duration, Plot::White, 2 );
  if ( boolean( "plotstdev" ) ) {
    P[0].plot( data.MeanTrace+data.StdevTrace, 1000.0, Plot::Orange, 1, Plot::Solid );
    P[0].plot( data.MeanTrace-data.StdevTrace, 1000.0, Plot::Orange, 1, Plot::Solid );
  }
  P[0].plot( data.MeanTrace, 1000.0, Plot::Red, 3, Plot::Solid );

  // V-I curves:
  P[1].clear();
  MapD om, pm, sm, rm;
  double imin = Results[Range.next( 0 )].I;
  double imax = imin;
  for ( unsigned int k=Range.next( 0 );
	k<Results.size();
	k=Range.next( ++k ) ) {
    imax = Results[k].I;
    rm.push( Results[k].I, Results[k].VRest );
    om.push( Results[k].I, Results[k].VOn );
    sm.push( Results[k].I, Results[k].VSS );
    pm.push( Results[k].I, Results[k].VPeak );
  }
  if ( ! P[1].zoomedXRange() && imax > imin+1.0e-8 )
    P[1].setXRange( imin, imax );
  P[1].plot( rm, 1.0, Plot::Cyan, 3, Plot::Solid, Plot::Circle, 6, Plot::Cyan, Plot::Cyan );
  P[1].plot( om, 1.0, Plot::Green, 3, Plot::Solid, Plot::Circle, 6, Plot::Green, Plot::Green );
  P[1].plot( sm, 1.0, Plot::Red, 3, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P[1].plot( pm, 1.0, Plot::Orange, 3, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );

  MapD am;
  am.push( Results[inx].I, Results[inx].VRest );
  am.push( Results[inx].I, Results[inx].VOn );
  am.push( Results[inx].I, Results[inx].VSS );
  am.push( Results[inx].I, Results[inx].VPeak );
  P[1].plot( am, 1.0, Plot::Transparent, 3, Plot::Solid, Plot::Circle, 8, Plot::Yellow, Plot::Transparent );

  P.draw();

  P.unlock();
}


void VICurve::save( void )
{
  message( "<b>Saving ...</b>" );
  tracePlotContinuous();
  unlockAll();
  for ( unsigned int j=Range.next( 0 );
	j<Results.size();
	j=Range.next( ++j ) ) {
    double spikecount = min( Results[j].SpikeCount );
    if ( spikecount > 0.0 ) {
      metaData( "Cell" ).setNumber( "ithreshon", Results[j].I );
      Header.addNumber( "Ithreshon", Results[j].I, IUnit );
      break;
    }
  }
  Header.addSection( "status" );
  Header.append( stimulusData() );
  Header.addSection( "settings" );
  Header.append( settings() );

  saveData();
  saveTrace();
  message( "<b>VICurve finished.</b>" );
  lockAll();
}


void VICurve::saveData( void )
{
  ofstream df( addPath( "vicurve-data.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Header.save( df, "# " );
  df << '\n';

  TableKey datakey;
  datakey.addSection( "Stimulus" );
  datakey.addNumber( "I", IUnit, "%6.3f" );
  datakey.addNumber( "IDC", IUnit, "%6.3f" );
  datakey.addNumber( "trials", "1", "%6.0f" );
  datakey.addSection( "Rest" );
  datakey.addNumber( "Vrest", VUnit, "%6.1f" );
  datakey.addNumber( "s.d.", VUnit, "%6.1f" );
  datakey.addSection( "Steady-state" );
  datakey.addNumber( "Vss", VUnit, "%6.1f" );
  datakey.addNumber( "s.d.", VUnit, "%6.1f" );
  datakey.addSection( "Peak" );
  datakey.addNumber( "Vpeak", VUnit, "%6.1f" );
  datakey.addNumber( "s.d.", VUnit, "%6.1f" );
  datakey.addNumber( "tpeak", "ms", "%6.1f" );
  datakey.addSection( "Onset" );
  datakey.addNumber( "Vpeak", VUnit, "%6.1f" );
  datakey.addNumber( "s.d.", VUnit, "%6.1f" );
  datakey.saveKey( df );

  for ( unsigned int j=Range.next( 0 );
	j<Results.size();
	j=Range.next( ++j ) ) {
    datakey.save( df, Results[j].I, 0 );
    datakey.save( df, Results[j].DC );
    datakey.save( df, (double)Range.count( j ) );
    datakey.save( df, Results[j].VRest );
    datakey.save( df, Results[j].VRestsd );
    datakey.save( df, Results[j].VSS );
    datakey.save( df, Results[j].VSSsd );
    datakey.save( df, Results[j].VPeak );
    datakey.save( df, Results[j].VPeaksd );
    datakey.save( df, 1000.0*Results[j].VPeakTime );
    datakey.save( df, Results[j].VOn );
    datakey.save( df, Results[j].VOnsd );
    df << '\n';
  }
  df << "\n\n";
}


void VICurve::saveTrace( void )
{
  ofstream df( addPath( "vicurve-trace.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Header.save( df, "# " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "t", "ms", "%6.2f" );
  datakey.addNumber( "V", VUnit, "%6.2f" );
  datakey.addNumber( "s.d.", VUnit, "%6.2f" );
  if ( ! Results[0].MeanCurrent.empty() )
    datakey.addNumber( "I", IUnit, "%6.3f" );

  int inx = 0;
  for ( unsigned int j=Range.next( 0 );
	j<Results.size();
	j=Range.next( ++j ) ) {
    df << "#  index: " << Str( inx ) << '\n';
    df << "# trials: " << Str( Range.count( j ) ) << '\n';
    df << "#      I: " << Str( Results[j].I ) << IUnit << '\n';
    df << "#     DC: " << Str( Results[j].DC ) << IUnit << '\n';
    df << "#  VRest: " << Str( Results[j].VRest ) << VUnit << '\n';
    df << "#    VOn: " << Str( Results[j].VOn ) << VUnit << '\n';
    df << "#  VPeak: " << Str( Results[j].VPeak ) << VUnit << '\n';
    df << "#    VSS: " << Str( Results[j].VSS ) << VUnit << '\n';
    df << '\n';
    datakey.saveKey( df );
    for ( int k=0; k<Results[j].MeanTrace.size(); k++ ) {
      datakey.save( df, 1000.0*Results[j].MeanTrace.pos( k ), 0 );
      datakey.save( df, Results[j].MeanTrace[k] );
      datakey.save( df, Results[j].StdevTrace[k] );
      if ( ! Results[j].MeanCurrent.empty() )
	datakey.save( df, Results[j].MeanCurrent[k] );
      df << '\n';
    }
    df << "\n\n";
    inx++;
  }
  df << '\n';
}


VICurve::Data::Data( void )
  : DC( 0.0 ),
    I( 0.0 ),
    VRest( 0.0 ),
    VRestsd( 0.0 ),
    VOn( 0.0 ),
    VOnsd( 0.0 ),
    VPeak( 0.0 ),
    VPeaksd( 0.0 ),
    VPeakTime( 0.0 ),
    VSS( 0.0 ),
    VSSsd( 0.0 )
{
  SpikeCount.clear();
  MeanTrace.clear();
  SquareTrace.clear();
  StdevTrace.clear();
  MeanCurrent.clear();
}


void VICurve::Data::analyze( int count, const InData &intrace,
			     const EventData &spikes,
			     const InData *incurrent, double iinfac,
			     double delay, double duration,
			     double ton, double sswidth )
{
  // initialize:
  if ( MeanTrace.empty() ) {
    double stepsize = intrace.stepsize();
    MeanTrace = SampleDataD( -delay, 2.0*duration, stepsize, 0.0 );
    SquareTrace = SampleDataD( -delay, 2.0*duration, stepsize, 0.0 );
    StdevTrace = SampleDataD( -delay, 2.0*duration, stepsize, 0.0 );
    SpikeCount.reserve( 100 );
    if ( incurrent != 0 )
      MeanCurrent = SampleDataD( -delay, 2.0*duration, stepsize, 0.0 );
  }

  // update averages:
  int inx = intrace.signalIndex() - MeanTrace.index( 0.0 );
  for ( int k=0; k<MeanTrace.size() && inx+k<intrace.size(); k++ ) {
    double v = intrace[inx+k];
    MeanTrace[k] += (v - MeanTrace[k])/(count+1);
    SquareTrace[k] += (v*v - SquareTrace[k])/(count+1);
    StdevTrace[k] = sqrt( SquareTrace[k] - MeanTrace[k]*MeanTrace[k] );
    if ( incurrent != 0 ) {
      double c = iinfac*(*incurrent)[inx+k];
      MeanCurrent[k] += (c - MeanCurrent[k])/(count+1);
    }
  }

  // resting potential:
  VRest = MeanTrace.mean( -delay, 0.0 );
  VRestsd = MeanTrace.stdev( -delay, 0.0 );

  // steady-state potential:
  VSS = MeanTrace.mean( duration-sswidth, duration );
  VSSsd = MeanTrace.stdev( duration-sswidth, duration );

  // onset potential:
  VOn = MeanTrace[ ton ];
  VOnsd = StdevTrace[ ton ];

  // peak potential:
  VPeak = VRest;
  int vpeakinx = 0;
  if ( VSS > VRest )
    vpeakinx = MeanTrace.maxIndex( VPeak, 0.0, duration-sswidth );
  else
    vpeakinx = MeanTrace.minIndex( VPeak, 0.0, duration-sswidth );
  VPeaksd = StdevTrace[vpeakinx];
  if ( fabs( VPeak - VSS ) <= VSSsd ) {
    VPeak = VSS;
    VPeaksd = VSSsd;
    VPeakTime = 0.0;
  }
  else
    VPeakTime = MeanTrace.pos( vpeakinx );

  // spikes:
  double sigtime = spikes.signalTime();
  SpikeCount.push( (double)spikes.count( sigtime, sigtime+duration ) );
}


addRePro( VICurve, patchclamp );

}; /* namespace patchclamp */

#include "moc_vicurve.cc"
