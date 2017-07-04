/*
  auditory/ficurve.cc
  Optimized measuring of f-I curves.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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
#include <iomanip>
#include <relacs/array.h>
#include <relacs/map.h>
#include <relacs/stats.h>
#include <relacs/str.h>
#include <relacs/tablekey.h>
#include <relacs/auditory/session.h>
#include <relacs/auditory/ficurve.h>
using namespace relacs;

namespace auditory {


FICurve::FICurve( void )
  : RePro( "FICurve", "auditory", "Jan Benda", "1.6", "Jul 18, 2014" )
{
  // parameter:
  MinIntensity = 30.0;
  MaxIntensity = 100.0;
  IntensityStep = 1.0;
  UseThresh = false;
  UseSaturation = false;
  IntShuffle = RangeLoop::Up;
  IntIncrement = 0;
  SlopeIntIncrement = 2;
  MinRateSlope = 0.0;
  MaxRateFrac = 1.0;
  IntensityExtension = 0.0;
  SingleRepeat = 6;
  IntBlockRepeat = 1;
  IntRepeat = 1;
  CarrierFrequency = 5000.0;
  UseBestFreq = false;
  Waveform = 0;
  Ramp = 0.002;
  Side = 0;
  Duration = 0.4;
  PreWidth = 0.05;
  Pause = 0.4;
  SkipPause = true;
  UseSilent = false;
  MaxBaseRate = 10.0;
  SkipSilentIncrement = true;
  MaxSilent = 100;
  SilentFactor = 3.0;
  ResetSilent = 1;
  RateDt = 0.001;
  PeakWidth = 0.1;
  SSWidth = 0.05;
  PlotOnSSRate = true;
  AdjustGain = true;
  SetBest = true;
  SetCurves = 1;

  // add some parameter as options:
  newSection( "Intensities" );
  addNumber( "intmin", "Minimum stimulus intensity", MinIntensity, -200.0, 200.0, 5.0, "dB SPL" ).setActivation( "useprevints", "no" );
  addNumber( "intmax", "Maximum stimulus intensity", MaxIntensity, 0.0, 200.0, 5.0, "dB SPL" ).setActivation( "useprevints", "no" );
  addNumber( "intstep", "Sound intensity step", IntensityStep, 0.0, 200.0, 1.0, "dB SPL" ).setActivation( "useprevints", "no" );
  addBoolean( "usethresh", "Relative to the cell's threshold", UseThresh ).setActivation( "useprevints", "no" );
  addBoolean( "usesat", "Maximum intensity relative to the cell's best saturation", UseSaturation ).setActivation( "useprevints", "no" );
  addSelection( "useprevints", "Re-use intensities from the previous run", "no|all|selected" );
  addSelection( "intshuffle", "Order of intensities", RangeLoop::sequenceStrings() );
  addInteger( "intincrement", "Initial increment for intensities", IntIncrement, -1000, 1000, 1 );
  addInteger( "singlerepeat", "Number of immediate repetitions of a single stimulus", SingleRepeat, 1, 10000, 1 );
  addInteger( "blockrepeat", "Number of repetitions of a fixed intensity increment", IntBlockRepeat, 1, 10000, 1 );
  addInteger( "repeats", "Number of repetitions of the whole f-I curve measurement", IntRepeat, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addBoolean( "manualskip", "Show buttons for manual selection of intensities", false );
  newSection( "Waveform" );
  addSelection( "waveform", "Waveform of stimulus", "sine|noise" );
  addNumber( "carrierfreq", "Frequency of carrier", CarrierFrequency, -40000.0, 40000.0, 500.0, "Hz", "kHz" );
  addBoolean( "usebestfreq", "Relative to the cell's best frequency", UseBestFreq );
  addNumber( "ramp", "Ramp of stimulus", Ramp, 0.0, 10.0, 0.001, "seconds", "ms" );
  addNumber( "duration", "Duration of stimulus", Duration, 0.0, 1000.0, 0.05, "seconds", "ms" );
  addNumber( "pause", "Pause", Pause, 0.0, 1000.0, 0.05, "seconds", "ms" );
  addSelection( "side", "Speaker", "left|right|best" );
  newSection( "Optimization" );
  newSubSection( "Baseline activity" );
  addBoolean( "usesilent", "Use measured baseline activity", UseSilent );
  addNumber( "maxbaserate", "Maximum baseline activity", MaxBaseRate, 0.0, 1000.0, 5.0, "Hz" ).setActivation( "usesilent", "false" );
  addInteger( "maxsilent", "Maximum trials used for baseline activity", MaxSilent, 0, 1000, 1 );
  addSelection( "resetsilent", "Reset estimation of baseline activity at", "Never|Session|RePro" );
  addNumber( "silentfactor", "Weight for standard deviation of baseline activity", SilentFactor, 0.0, 100.0, 0.5 ).setActivation( "usesilent", "true" );
  newSubSection( "No response" );
  addBoolean( "skippause", "Skip pause if there is no response", SkipPause );
  addInteger( "silentintincrement", "Skip all stimuli below not responding ones<br> at intensity increments below", SkipSilentIncrement, 0, 1000, 1 );
  newSubSection( "Slope" );
  addInteger( "slopeintincrement", "Optimize slopes at intensity increments below", SlopeIntIncrement, 0, 1000, 1 );
  addNumber( "minrateslope", "Minimum slope of firing rate", MinRateSlope, 0.0, 1000.0, 1.0, "Hz/dB" );
  addNumber( "maxratefrac", "Fraction of maximum firing rate above which slopes are optimized", MaxRateFrac, 0.0, 1.0, 0.1, "1", "%" );
  addNumber( "extint", "Extend intensity range by", IntensityExtension, 0.0, 100.0, 1.0, "dB SPL" );
  newSection( "Analysis" );
  addNumber( "ratedt", "Bin width for firing rate", RateDt, 0.0, 1.0, 0.0005, "seconds", "ms" );
  addNumber( "prewidth", "Window length for baseline firing rate", PreWidth, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "peakwidth", "Window length for peak firing rate", PeakWidth, 0.0, 10.0, 0.01, "seconds", "ms" );
  addNumber( "sswidth", "Window length for steady-state firing rate", SSWidth, 0.0, 10.0, 0.01, "seconds", "ms" );
  addBoolean( "plotonssrate", "Plot onset- and steady-state rate", PlotOnSSRate );
  addBoolean( "adjust", "Adjust input gain", AdjustGain );
  addBoolean( "setbest", "Set results to the session variables", SetBest );
  addSelection( "setcurves", "F-I curves to be passed to session", "none|mean rate|onset + steady-state|mean + onset + steady-state" );
  addFlags( 1 );

  // variables:
  NSilent = 0;
  SilentRate = 0.0;
  LastSilentRate = 0.0;
  SilentRateSq = 0.0;
  SilentRateSD = 0.0;
  MaxSilentRate = 0.0;
  Intensity = 0.0;
  MinCarrierFrequency = 2000.0;
  FICurveStops = 0;
  PlotIntensitySelection = false;

  // plot:
  P.lock();
  P.setDataMutex( mutex() );
  P.resize( 2, 2, true );
  P[0].setLMarg( 5.0 );
  P[0].setRMarg( 1.0 );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( "Firing rate [Hz]" );
  P[1].setLMarg( 4.0 );
  P[1].setRMarg( 2.0 );
  P[1].setXLabel( "Intensity [dB SPL]" );
  P[1].setYLabel( "Firing rate [Hz]" );
  P.unlock();
  setWidget( &P );

  // header and keys:
  Header.addInteger( "index1" );
  Header.addNumber( "carrier frequency", "kHz", "%.3f" );
  Header.addText( "side" );
  Header.addNumber( "intmin", "dB SPL", "%.1f" );
  Header.addNumber( "intmax", "dB SPL", "%.1f" );
  Header.addNumber( "threshold", "dB SPL", "%.1f" );
  Header.addNumber( "slope", "Hz/dB", "%.2f" );
  Header.addNumber( "intensity", "dB SPL", "%.1f" );
  Header.addNumber( "rate", "Hz", "%.1f" );
  Header.addNumber( "saturation", "dB SPL", "%.1f" );
  Header.addNumber( "maxrate", "Hz", "%.1f" );
  Header.addInteger( "nfit" );
  Header.addNumber( "silent rate", "Hz", "%.1f" );
  Header.addText( "session time" );
  Header.newSection( "Status" );
  Header.newSection( "Settings" );
}


FICurve::~FICurve( void )
{
}


int FICurve::main( void )
{
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 ) {
    warning( "No spike trace!" );
    return Failed;
  }

  // get options:
  settings().setValueTypeFlags( 16, -Parameter::Section );
  MinIntensity = number( "intmin" );
  MaxIntensity = number( "intmax" );
  IntensityStep = number( "intstep" );
  UseThresh = boolean( "usethresh" );
  UseSaturation = boolean( "usesat" );
  int useprevints = index( "useprevints" );
  IntShuffle = RangeLoop::Sequence( index( "intshuffle" ) );
  IntIncrement = integer( "intincrement" );
  SlopeIntIncrement = integer( "slopeintincrement" );
  MinRateSlope = number( "minrateslope" );
  MaxRateFrac = number( "maxratefrac" );
  IntensityExtension = number( "extint" );
  SingleRepeat = integer( "singlerepeat" );
  IntBlockRepeat = integer( "blockrepeat" );
  IntRepeat = integer( "repeats" );
  bool manualskip = boolean( "manualskip" );
  Waveform = index( "waveform" );
  Ramp = number( "ramp" );
  CarrierFrequency = number( "carrierfreq" );
  UseBestFreq = boolean( "usebestfreq" );
  Side = index( "side" );
  Duration = number( "duration" );
  PreWidth = number( "prewidth" );
  Pause = number( "pause" );
  SkipPause = boolean( "skippause" );
  SkipSilentIncrement = integer( "silentintincrement" );
  UseSilent = boolean( "usesilent" );
  MaxBaseRate = number( "maxbaserate" );
  MaxSilent = integer( "maxsilent" );
  SilentFactor = number( "silentfactor" );
  ResetSilent = index( "resetsilent" );
  RateDt = number( "ratedt" );
  PeakWidth = number( "peakwidth" );
  SSWidth = number( "sswidth" );
  PlotOnSSRate = boolean( "plotonssrate" );
  AdjustGain = boolean( "adjust" );
  SetBest = boolean( "setbest" );
  SetCurves = index( "setcurves" );

  if ( PreWidth > Pause )
    Pause = PreWidth;

  lockMetaData();
  if ( Side > 1 )
    Side = metaData().index( "Cell>best side" );
  string sidestr = Side > 0 ? "right" :  "left";
  if ( UseBestFreq ) {
    double cf = metaData().number( "Cell>" + sidestr + " frequency" );
    if ( cf > 0.0 )
      CarrierFrequency += cf;
  }
  if ( UseThresh || UseSaturation ) {
    auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
    if ( as == NULL ) {
      warning( "Can not get the neuron's threshold intensity. <br>No auditory::Session-plugin found." );
      return Failed;
    }
    MapD thresh = as->threshCurve( Side );
    if ( thresh.empty() ) {
      warning( "Can not get the neuron's threshold intensity. <br>No threshold curve was measured so far.");
      return Failed;
    }
    int k=0;
    for ( k=0;
	  k < thresh.size() && thresh.x(k) < CarrierFrequency;
	  k++ );
    double intthresh = 0.0;
    if ( k == 0 && ::fabs( thresh.x( k ) - CarrierFrequency ) < 1.0 )
      intthresh = thresh.y( k );
    else if ( k >= thresh.size() && ::fabs( thresh.x( thresh.size()-1 ) - CarrierFrequency ) < 1.0 )
      intthresh = thresh.y( thresh.size()-1 );
    else if ( k == 0 || k >= thresh.size() ) {
      warning( "Can not get the neuron's threshold intensity. <br>Requested carrier frequency of " +
	       Str( 0.001*CarrierFrequency ) +
	       " kHz is outside the measured range of the threshold curve." );
      return Failed;
    }
    else
      intthresh = thresh.y(k) + ( CarrierFrequency - thresh.x(k) )*( thresh.y(k) - thresh.y(k-1) )/( thresh.x(k) - thresh.x(k-1) );
    double ith = 0.0;
    if ( UseThresh )
      ith = intthresh;
    double isat = ith;
    if ( UseSaturation )
      isat = intthresh + metaData().number( "Cell>" + sidestr + " saturation" ) - metaData().number( "Cell>" + sidestr + " threshold" );
    MinIntensity += ith;
    MaxIntensity += isat;
  }
  if ( SSWidth > Duration )
    SSWidth = Duration;
  unlockMetaData();

  // plot trace:
  tracePlotSignal( 2.0*PreWidth+Duration, PreWidth );

  // intensity:
  if ( useprevints > 0 ) {
    if ( useprevints > 1 )
      IntensityRange.purge();
    MinIntensity = IntensityRange.minValue();
    MaxIntensity = IntensityRange.maxValue();
    IntensityStep = IntensityRange.step();
  }
  else {
    IntensityRange.clear();
    IntensityRange.set( MinIntensity, MaxIntensity, IntensityStep );
  }
  IntensityRange.setRepeat( IntRepeat );
  IntensityRange.setBlockRepeat( IntBlockRepeat );
  IntensityRange.setSingleRepeat( SingleRepeat );
  IntensityRange.setIncrement( IntIncrement );
  IntensityRange.setSequence( IntShuffle );
  IntensityRange.reset( -1, ( useprevints != 1 ) );
  Intensity = *IntensityRange;

  if ( ResetSilent == 2 )
    resetSilentActivity();

  // plot:
  P.lock();
  P[0].clear();
  P[0].setXRange( -1000.0*PreWidth, 1000.0*(Duration+PreWidth) );
  P[0].setYFallBackRange( 0.0, 100.0 );
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[1].clear();
  P[1].setXRange( MinIntensity, MaxIntensity );
  P[1].setYFallBackRange( 0.0, 100.0 );
  P[1].setYRange( 0.0, Plot::AutoScale );
  if ( manualskip ) {
    P[1].setUserMouseTracking( true );
    connect( &P[1], SIGNAL( userMouseEvent( Plot::MouseEvent& ) ),
	     this, SLOT( plotMouseEvent( Plot::MouseEvent& ) ) );
  }
  PlotIntensitySelection = false;
  P.unlock();

  // results:
  vector< FIData > results( IntensityRange.size(), FIData( -PreWidth, Duration+Pause-PreWidth, RateDt ) );
  MaxPlotRate = 50.0;
  Threshold.reset();

  // stimulus:
  Signal.setTrace( Speaker[ Side ] );
  if ( Waveform == 1 ) {
    Signal.bandNoiseWave( Duration, -1.0, MinCarrierFrequency, CarrierFrequency, 0.3, 0, Ramp );
    MeanIntensity = 6.0206; // stdev=0.5
    MeanIntensity = 10.458; // stdev = 0.3
  }
  else {
    Signal.sineWave( Duration, -1.0, CarrierFrequency, 0.0, 1.0, Ramp );
    MeanIntensity = 3.0103;
  }
  Signal.back() = 0;
  Signal.setDelay( 0.0 );
  Signal.setError( OutData::Unknown );

  for ( int k=0; ! Signal.success(); k++ ) {

    if ( k > 40 ) {
      warning( "Could not establish valid intensity!<br>Signal error: <b>" +
	       Signal.errorText() + "</b>.<br>Exit now!" );
      Signal.free();
      if ( manualskip ) {
	P.lock();
	P[1].setUserMouseTracking( false );
	disconnect( &P[1], SIGNAL( userMouseEvent( Plot::MouseEvent& ) ),
		    this, SLOT( plotMouseEvent( Plot::MouseEvent& ) ) );
	P.unlock();
      }
      return Failed;
    }

    Signal.setIntensity( Intensity + MeanIntensity );
    testWrite( Signal );

    if ( Signal.underflow() ) {
      printlog( "attenuator underflow: increased intensity from " +
		Str( Intensity + MeanIntensity ) + " to " + Str( Signal.intensity() ) );
      IntensityRange.setSkipBelow( IntensityRange.pos() );
      IntensityRange.noCount();
      ++IntensityRange;
      Intensity = *IntensityRange;
    }
    else if ( Signal.overflow() ) {
      printlog( "attenuator overflow: reduced intensity from " +
		Str( Intensity + MeanIntensity ) + " to " + Str( Signal.intensity() ) );
      IntensityRange.setSkipAbove( IntensityRange.pos() );
      IntensityRange.noCount();
      ++IntensityRange;
      Intensity = *IntensityRange;
    }

  }

  Str s = "Frequency <b>" + Str( CarrierFrequency * 0.001 ) + " kHz</b>";
  s += ",  Intensity <b>" + Str( *IntensityRange ) + " dB SPL</b>";
  s += ",  Increment <b>" + Str( IntensityRange.currentIncrement() ) + "</b>";
  s += ",  Loop <b>" + Str( IntensityRange.count()+1 ) + "</b>";
  message( s );

  int ds = loop( results );
  save( results );
  Signal.free();
  writeZero( Speaker[ Side ] );
  if ( manualskip ) {
    P.lock();
    P[1].setUserMouseTracking( false );
    disconnect( &P[1], SIGNAL( userMouseEvent( Plot::MouseEvent& ) ),
		this, SLOT( plotMouseEvent( Plot::MouseEvent& ) ) );
    P.unlock();
  }
  return ds;
}


int FICurve::loop( vector< FIData > &results )
{
  sleep( Pause );
  if ( interrupt() )
    return Aborted;

  DoneState ds = Continue;

  do {
    write( Signal );
    if ( ! Signal.success() ) {
      warning( "Output of signal failed!<br>Signal error <b>" +
	       Signal.errorText() + "</b>.<br>Exit now!" );
      return Failed;
    }
    if ( interrupt() )
      ds = Aborted;
    else {
      silentActivity();

      // pause if there were spikes:
      if ( ! SkipPause ||
	   events( SpikeEvents[0] ).rate( signalTime(), signalTime() + Duration ) > MaxSilentRate ) {
	sleep( Pause );
      }
      
      // if there were some spikes, adjust analog input gain:
      if ( AdjustGain &&
	   events( SpikeEvents[0] ).count( signalTime(), signalTime() + Duration ) > 0 )
	adjust( trace( SpikeTrace[0] ), signalTime(), signalTime() + Duration, 0.8 );

      // analyze:
      analyze( results );
      plot( results );
      
      // set next stimulus:
      ds = next( results );
      if ( Signal.empty() )
	ds = Failed;
    }

  } while ( ds == Continue );

  return ds;
}


void FICurve::init( void )
{
  if ( ResetSilent == 1 )
    resetSilentActivity();
}


void FICurve::saveSpikes( const string &file, const vector< FIData > &results )
{
  // create file:
  ofstream df( addPath( file ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  // write data:
  int n = 0;
  for ( unsigned int k=IntensityRange.next( 0 ); 
	k<results.size(); 
	k=IntensityRange.next( ++k ) ) {
    df << "#         index0: " << n++ << '\n';
    df << "#      intensity: " << results[k].Intensity << "dB SPL\n";
    df << "# true intensity: " << results[k].TrueIntensity << "dB SPL\n";
    results[k].Spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
    df << '\n';
  }
  df << '\n';
}


void FICurve::saveRates( const string &file, const vector< FIData > &results )
{
  // create file:
  ofstream df( addPath( file ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.addNumber( "f", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  // write data:
  int n = 0;
  for ( unsigned int k=IntensityRange.next( 0 ); 
	k<results.size(); 
	k=IntensityRange.next( ++k ) ) {
    df << "#         index0: " << n++ << '\n';
    df << "#      intensity: " << results[k].Intensity << "dB SPL\n";
    df << "# true intensity: " << results[k].TrueIntensity << "dB SPL\n";
    if  ( results[k].Rate.size() != results[k].RateSD.size() )
      printlog( "! warning saveRates() -> Rate.size() " + Str( results[k].Rate.size() )
	   + " != RateSD.size() " + Str( results[k].RateSD.size() ) );
    for ( int j=0; j<results[k].Rate.size(); j++ ) {
      key.save( df, 1000.0*results[k].Rate.pos( j ), 0 );
      key.save( df, results[k].Rate[j] );
      // XXX:
      if ( j<results[k].RateSD.size() )
	key.save( df, results[k].RateSD[j] );
      df << '\n';
    }
    df << '\n';
  }
  df << '\n';
}


void FICurve::saveFICurve( const string &file, const vector< FIData > &results )
{
  // create file:
  ofstream df( addPath( file ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.newSection( "stimulus" );
  key.addNumber( "I_r", "dB SPL", "%5.1f" );
  key.addNumber( "I", "dB SPL", "%5.1f" );
  key.addNumber( "n", "1", "%3.0f" );
  key.newSection( "firing rate" );
  key.addNumber( "f", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.newSection( "baseline" );
  key.addNumber( "f_b", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.newSection( "peak rate" );
  key.addNumber( "f_on", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.addNumber( "t_on", "ms", "%5.1f" );
  key.newSection( "steady-state" );
  key.addNumber( "f_ss", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.newSection( "spike count" );
  key.addNumber( "count", "1", "%7.1f" );
  key.addNumber( "s.d.", "1", "%7.1f" );
  key.newSection( "latency" );
  key.addNumber( "latency", "ms", "%6.1f" );
  key.addNumber( "s.d.", "ms", "%6.1f" );
  key.saveKey( df, true, true );

  // write data:
  for ( unsigned int k=IntensityRange.next( 0 ); 
	k<results.size(); 
	k=IntensityRange.next( ++k ) ) {
    key.save( df, results[k].Intensity, 0 );
    key.save( df, results[k].TrueIntensity );
    key.save( df, results[k].Spikes.size() );
    key.save( df, results[k].MeanRate );
    key.save( df, results[k].MeanRateSD );
    key.save( df, results[k].PreRate );
    key.save( df, results[k].PreRateSD );
    key.save( df, results[k].OnRate );
    key.save( df, results[k].OnRateSD );
    key.save( df, results[k].OnTime );
    key.save( df, results[k].SSRate );
    key.save( df, results[k].SSRateSD );
    key.save( df, results[k].Count );
    key.save( df, results[k].CountSD );
    key.save( df, 1000.0*results[k].Latency );
    key.save( df, 1000.0*results[k].LatencySD );
    df << '\n';
  }
  df << '\n' << '\n';
}


void FICurve::analyzeFICurve( const vector< FIData > &results, double minrate )
{
  Threshold.Frequency = CarrierFrequency;

  // maximum firing rate:
  double max = 0.0;
  int mk = 0;
  for ( unsigned int k=IntensityRange.next( 0 ); 
	k<results.size(); 
	k = IntensityRange.next( ++k ) ) {
    if ( results[k].MeanRate > max )
      max = results[k].MeanRate;
    mk = k;
  }
  int mrk = 1;
  double maxrate = max;
  double maxratesd = results[mk].MeanRateSD;
  int lmk = IntensityRange.previous( mk-1 );
  if ( lmk >= 0 ) {
    maxrate += ( results[lmk].MeanRate - maxrate ) / (++mrk);
    maxratesd += ( results[lmk].MeanRateSD - maxratesd ) / mrk;
  }
  unsigned int rmk = IntensityRange.next( mk+1 );
  if ( rmk < results.size() ) {
    maxrate += ( results[rmk].MeanRate - maxrate ) / (++mrk);
    maxratesd += ( results[rmk].MeanRateSD - maxratesd ) / mrk;
  }
  Threshold.MaxRate = maxrate;
  Threshold.MaxRateSD = maxratesd;

  if ( maxrate <= MaxSilentRate || maxrate < minrate ) {
    //    printlog( "analyzeFICurve() -> all rates below MaxSilentRate or minrate" );
    Threshold.reset();
    return;
  }

  // half maximum:
  double hmax = 0.5*( SilentRate + max );
  unsigned int h = 0;
  for ( h=IntensityRange.next( 0 ); 
	h<results.size(); 
	h = IntensityRange.next( ++h ) ) {
    if ( results[h].MeanRate >= hmax )
      break;
  }

  // go to the right:
  double rmax = 0.75 * ( max - SilentRate ) + SilentRate;
  unsigned int r = 0;
  for ( r=IntensityRange.next( h ); 
	r<results.size(); 
	r = IntensityRange.next( ++r ) ) {
    if ( results[r].MeanRate > rmax )
      break;
  }
  
  // go to the left:
  double lmax = 0.25 * ( max - SilentRate ) + SilentRate;
  int l = 0;
  for ( l=IntensityRange.previous( h ); 
	l >= 0; 
	l = IntensityRange.previous( --l ) ) {
    if ( results[l].MeanRate < lmax )
      break;
  }
  // down to SilentRate:
  int p = l;
  for ( l=IntensityRange.previous( --l ); 
	l >= 0; 
	l = IntensityRange.previous( --l ) ) {
    if ( results[l].MeanRate <= SilentRate ||
	 results[l].MeanRate >= results[p].MeanRate ) {
      l = p;
      break;
    }
    p = l;
  }
  if ( r > results.size() )
    r = results.size();
  if ( l < 0 )
    l = 0;

  // number of data points:
  int n=0;
  for ( unsigned int k=IntensityRange.next( l ); 
	k < results.size() && k < r; 
	k = IntensityRange.next( ++k ) ) {
    n++;
  }

  // enlarge right margin (at least two data points are needed):
  for ( ; n<2 && r<results.size(); n++ ) {
    r = IntensityRange.next( ++r );
  }

  if ( n < 2 ) {
    // no data:
    //    printlog( "analyzeFICurve() -> no data to fit" );
    Threshold.reset();
  }
  else {
    // analyze:
    ArrayD ri;
    ri.reserve( results.size() );
    ArrayD rm;
    rm.reserve( results.size() );
    ArrayD rs;
    rs.reserve( results.size() );
    for ( unsigned int k=l; 
	  k<results.size() && k<r; 
	  k=IntensityRange.next( ++k ) ) {
      ri.push( IntensityRange.value( k ) );
      rm.push( results[k].MeanRate );
      rs.push( results[k].MeanRateSD + 1.0 );  // add one Hertz to avoid zero SD!
    }
    double shift = ri.mean(); 
    ri -= shift;
    // fit:
    double s=0.0, us=0.0, b=0.0, ub=0.0, ch=0.0;
    lineFit( ri, rm, rs, b, ub, s, us, ch );
    // fit succesfull?
    if ( ch >= 0.0 ) {
      double as = fabs( s );
      // slope:
      Threshold.Slope = s;
      Threshold.SlopeSD = us;
      // threshold:
      if ( UseSilent ) {
	Threshold.Threshold = ( SilentRate - b ) / s;
	Threshold.ThresholdSD = ( SilentRateSD + ub + fabs( Threshold.Threshold * us ) ) / as;
      }
      else {
	Threshold.Threshold = ( MaxBaseRate - b ) / s;
	Threshold.ThresholdSD = ( ub + fabs( Threshold.Threshold * us ) ) / as;
      }

      Threshold.Threshold += shift;
      // intensity at rate:
      lockMetaData();
      Threshold.RateIntensity = ( metaData().number( "Cell>best rate" ) - b ) / s;
      unlockMetaData();
      Threshold.RateIntensitySD = ( ub + fabs( Threshold.RateIntensity * us ) ) / as;
      Threshold.RateIntensity += shift;
      // saturation:
      Threshold.Saturation = ( Threshold.MaxRate - b ) / s;
      Threshold.SaturationSD = ( Threshold.MaxRateSD + ub + fabs( Threshold.Saturation * us ) ) / as;
      Threshold.Saturation += shift;
      // number of data points used for fit:
      Threshold.N = rm.size();
      Threshold.Measured = true;
      
      // threshold should be near left margin:
      int sii = SlopeIntIncrement;
      if ( sii <= 0 )
	sii = 2;
      if ( fabs( Threshold.Threshold - IntensityRange.value( l ) ) > 
	   fabs( IntensityRange.value( l + 2*sii ) - IntensityRange.value( l ) ) ) {
	printlog( "analyzeFICurve() -> suspicious threshold" );
      }
    }

  }

}


void FICurve::setHeader( void )
{
  Header.setInteger( "index1", totalRuns() );
  Header.setNumber( "carrier frequency", 0.001*CarrierFrequency );
  Header.setInteger( "side", Side );
  Header.setNumber( "intmin", IntensityRange.minValue() );
  Header.setNumber( "intmax", IntensityRange.maxValue() );
  Header.setNumber( "threshold", Threshold.Threshold, Threshold.ThresholdSD );
  Header.setNumber( "slope", Threshold.Slope, Threshold.SlopeSD );
  Header.setNumber( "intensity", Threshold.RateIntensity, Threshold.RateIntensitySD );
  lockMetaData();
  Header.setNumber( "rate", metaData().number( "Cell>best rate" ) );
  unlockMetaData();
  Header.setNumber( "saturation", Threshold.Saturation, Threshold.SaturationSD );
  Header.setNumber( "maxrate", Threshold.MaxRate, Threshold.MaxRateSD );
  Header.setInteger( "nfit", Threshold.N );
  Header.setNumber( "silent rate", SilentRate, SilentRateSD );
  Header.setText( "session time", sessionTimeStr() );
}


void FICurve::updateSession( const vector< FIData > &results )
{
  lockMetaData();
  if ( SetBest ) {
    auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
    Options &mo = metaData().section( "Cell properties" );
    string ss = Side == 1 ? "right" : "left";

    mo.unsetNotify();

    // f-I curve parameter:
    if ( Waveform == 1 ) {
      string ns = ss + " noise";
      mo.setNumber( ns + " threshold", Threshold.Threshold, Threshold.ThresholdSD );
      mo.setNumber( ns + " slope", Threshold.Slope, Threshold.SlopeSD );
      mo.setNumber( ns + " intensity", Threshold.RateIntensity, Threshold.RateIntensitySD );
      mo.setNumber( ns + " saturation", Threshold.Saturation, Threshold.SaturationSD );
      mo.setNumber( ns + " maxrate", Threshold.MaxRate, Threshold.MaxRateSD );
    }
    else if ( Waveform == 0 ) {
      if ( ::fabs( CarrierFrequency - mo.number( ss + " frequency" ) ) < 1.0 ) {
	mo.setNumber( ss + " threshold", Threshold.Threshold, Threshold.ThresholdSD );
	mo.setNumber( ss + " slope", Threshold.Slope, Threshold.SlopeSD );
	mo.setNumber( ss + " intensity", Threshold.RateIntensity, Threshold.RateIntensitySD );
	mo.setNumber( ss + " saturation", Threshold.Saturation, Threshold.SaturationSD );
	mo.setNumber( ss + " maxrate", Threshold.MaxRate, Threshold.MaxRateSD );
      }
    }

    // determine best side:
    as->updateBestSide();

    /*
    // best side parameter:
    if ( Side == mo.index( "best side" ) &&
	 ( ( Waveform == 1 ) ||
	   ( Waveform == 0 &&
	     ::fabs( CarrierFrequency - mo.number( "best frequency" ) ) < 1.0 ) ) ) {
      mo.setNumber( "best threshold", Threshold.Threshold, Threshold.ThresholdSD );
      mo.setNumber( "best slope", Threshold.Slope, Threshold.SlopeSD );
      mo.setNumber( "best intensity", Threshold.RateIntensity, Threshold.RateIntensitySD );
      mo.setNumber( "best saturation", Threshold.Saturation, Threshold.SaturationSD );
      mo.setNumber( "best maxrate", Threshold.MaxRate, Threshold.MaxRateSD );
    }
    */

    // f-I curves:
    MapD om, sm, fm;
    for ( unsigned int k=0;
	  k<results.size();
	  k=IntensityRange.next( ++k ) ) {
      om.push( IntensityRange.value( k ), results[k].OnRate );
      sm.push( IntensityRange.value( k ), results[k].SSRate );
      fm.push( IntensityRange.value( k ), results[k].MeanRate );
    }
    if ( SetCurves == 1 || SetCurves == 3 ) {
      as->addFICurve( fm, Side, CarrierFrequency );
    }
    if ( SetCurves == 2 || SetCurves == 3 ) {
      as->addOnFICurve( om, Side, CarrierFrequency );
      as->addSSFICurve( sm, Side, CarrierFrequency );
    }

    mo.unsetNotify();
    mo.callNotifies();
  }
  unlockMetaData();
}


void FICurve::save( const vector< FIData > &results )
{
  analyzeFICurve( results );

  setHeader();
  Header.erase( "Status" );
  lockStimulusData();
  Header.newSection( stimulusData() );
  unlockStimulusData();
  Header.erase( "Settings" );
  Header.newSection( settings(), 16 );
  updateSession( results );

  saveSpikes( "fispikes.dat", results );
  saveRates( "firates.dat", results );
  saveFICurve( "ficurve.dat", results );
}


void FICurve::plotIntensitySelection( void )
{
  ArrayD sa; sa.reserve( IntensityRange.size() );
  ArrayD ua; ua.reserve( IntensityRange.size() );
  for ( int k=0; k<IntensityRange.size(); k++ ) {
    double x = IntensityRange.value( k );
    if ( k == 0 )
      x += 0.01 * IntensityStep;
    if ( k == IntensityRange.size()-1 )
      x -= 0.01 * IntensityStep;
    if ( IntensityRange.skip( k ) )
      sa.push( x );
    else
      ua.push( x);
  }
  P[1].plot( ua, 1.0, 0.95, Plot::Graph, 0, Plot::Diamond,
	     0.7*IntensityStep, Plot::FirstX, Plot::Green, Plot::Green );
  P[1].plot( sa, 1.0, 0.95, Plot::Graph, 0, Plot::Diamond,
	     0.7*IntensityStep, Plot::FirstX, Plot::Red, Plot::Red );
}


void FICurve::plot( const vector< FIData > &results )
{
  const FIData &fid = results[IntensityRange.pos()];

  // rate and spikes:
  P.lock();
  P[0].clear();
  P[0].plotVLine( 0.0, Plot::White, 2 );
  P[0].plotVLine( 1000.0*Duration, Plot::White, 2 );
  int maxspikes = (int)rint( 20.0 / SpikeTraces );
  if ( maxspikes < 4 )
    maxspikes = 4;
  int j = 0;
  int r = IntensityRange.maxCount();
  double delta = r > 0 && r < maxspikes ? 1.0/r : 1.0/maxspikes;
  int offs = (int)fid.Spikes.size() > maxspikes ? fid.Spikes.size() - maxspikes : 0;
  for ( int i=offs; i<fid.Spikes.size(); i++ ) {
    j++;
    P[0].plot( fid.Spikes[i], 0, 0.0, 1000.0,
	       1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp,
	       delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
  }
  P[0].plot( fid.Rate, 1000.0, Plot::Yellow, 2, Plot::Solid );

  // f-I curve:
  P[1].clear();
  MapD om, sm, cm, bm;
  for ( unsigned int k=0; k<results.size(); k++ ) {
    if ( results[k].Spikes.size() > 0 ) {
      bm.push( IntensityRange.value( k ), results[k].PreRate );
      om.push( IntensityRange.value( k ), results[k].OnRate );
      sm.push( IntensityRange.value( k ), results[k].SSRate );
      cm.push( IntensityRange.value( k ), results[k].MeanRate );
    }
  }
  P[1].plot( bm, 1.0, Plot::Cyan, 3, Plot::Solid, Plot::Circle, 6, Plot::Cyan, Plot::Cyan );
  if ( PlotOnSSRate ) {
    P[1].plot( om, 1.0, Plot::Green, 3, Plot::Solid, Plot::Circle, 6, Plot::Green, Plot::Green );
    P[1].plot( sm, 1.0, Plot::Red, 3, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  }
  P[1].plot( cm, 1.0, Plot::Orange, 3, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );
  int c = IntensityRange.pos();
  MapD am;
  am.push( results[c].Intensity, results[c].PreRate );
  if ( PlotOnSSRate ) {
    am.push( results[c].Intensity, results[c].OnRate );
    am.push( results[c].Intensity, results[c].SSRate );
  }
  am.push( results[c].Intensity, results[c].MeanRate );
  P[1].plot( am, 1.0, Plot::Transparent, 3, Plot::Solid, Plot::Circle, 8, Plot::Yellow, Plot::Transparent );

  if ( PlotIntensitySelection )
    plotIntensitySelection();

  P.draw();

  P.unlock();
}


void FICurve::resetSilentActivity( void )
{
  NSilent = 0;
  SilentRate = 0.0;
  LastSilentRate = 0.0;
  SilentRateSq = 0.0;
  SilentRateSD = 0.0;
  MaxSilentRate = UseSilent ? 0.0 : MaxBaseRate;
}


void FICurve::silentActivity( void )
{
  if ( SpikeEvents[0] < 0 )
    return;

  const EventData &spikes = events( SpikeEvents[0] );

  // rate:
  double rate = spikes.rate( signalTime()-PreWidth, signalTime() );

  // update statistics:
  if ( NSilent < MaxSilent ) {
    NSilent++;
    SilentRate += ( rate - SilentRate ) / NSilent;
    SilentRateSq += ( rate*rate - SilentRateSq ) / NSilent;
  }
  else {
    SilentRate += ( rate - LastSilentRate )  / MaxSilent;
    SilentRateSq += ( rate*rate - LastSilentRate*LastSilentRate )  / MaxSilent;
  }
  LastSilentRate = rate;

  // update standard deviation and maximum:
  if ( UseSilent ) {
    SilentRateSD = sqrt( fabs( SilentRateSq - SilentRate*SilentRate ) );
    MaxSilentRate = SilentRate + SilentFactor*SilentRateSD;
  }
  else {
    MaxSilentRate = MaxBaseRate;
  }

  // update session:
  lockMetaData();
  metaData().setNumber( "Cell>silent rate", SilentRate, SilentRateSD );
  unlockMetaData();
}


void FICurve::analyze( vector< FIData > &results )
{
  if ( SpikeEvents[0] < 0 )
    return;

  const EventData &spikes = events( SpikeEvents[0] );
  FIData &fid = results[IntensityRange.pos()];

  // spikes:
  fid.Spikes.push( spikes, signalTime()-PreWidth,
		   signalTime()+Duration + Pause, signalTime() );

  // firing frequency:
  fid.Spikes.frequency( fid.Rate, fid.RateSD );

  fid.PreRate = fid.Spikes.rate( -PreWidth, 0.0, &fid.PreRateSD );
  fid.SSRate = fid.Spikes.frequency( Duration-SSWidth, Duration, fid.SSRateSD );
  fid.MeanRate = fid.Spikes.rate( 0.0, Duration, &fid.MeanRateSD );
  fid.Count = fid.Spikes.count( 0.0, Duration, &fid.CountSD );

  fid.OnRate = fid.PreRate;
  fid.OnRateSD = fid.PreRateSD;
  fid.OnTime = 0.0;
  for ( int k=fid.Rate.index( 0.0 ); k<fid.Rate.index( PeakWidth ) && k<fid.Rate.size(); k++ ) {
    if ( fabs( fid.Rate[k] - fid.PreRate ) > fabs( fid.OnRate - fid.PreRate ) ) {
      fid.OnRate = fid.Rate[k];
      fid.OnRateSD = fid.RateSD[k];
      fid.OnTime = fid.Rate.pos( k );
    }
  }

  if ( fid.OnRate+50.0 > fid.MaxPlotRate ) {
    fid.MaxPlotRate = ::ceil((fid.OnRate+50.0)/20.0)*20.0;
  }
  if ( MaxPlotRate < fid.MaxPlotRate )
    MaxPlotRate = fid.MaxPlotRate;

  // latency:
  fid.Latency = fid.Spikes.latency( 0.0, &fid.LatencySD );

  // intensities:
  fid.Intensity = *IntensityRange;
  fid.TrueIntensity = Signal.intensity() - MeanIntensity;

  // skip intensities:
  if ( IntensityRange.currentIncrement() <= SkipSilentIncrement &&
       fid.MeanRate <= MaxSilentRate &&
       fid.Spikes.size() >= SingleRepeat*IntBlockRepeat ) {
    bool skip = true;
    for ( int k=IntensityRange.next( 0 ); 
	  k<IntensityRange.pos(); 
	  k = IntensityRange.next( ++k ) ) {
      if ( results[k].MeanRate > MaxSilentRate ) {
	printlog( "mean rate " + Str( results[k].MeanRate, "%.1f" ) + "Hz > MaxSilentRate " + Str( MaxSilentRate, "%.1f" ) + "Hz" ); 
	skip = false;
	break;
      }
    }
    if ( skip )
      IntensityRange.setSkipBelow( IntensityRange.pos() );
  }

}


RePro::DoneState FICurve::next( vector< FIData > &results, bool msg ) 
{
  if ( softStop() > 1+FICurveStops ) {
    printlog( "exit f-I curve immediately by softStop()=" + Str( softStop() ) + ", FICurveStops=" + Str( FICurveStops ) );
    return Completed;
  }

  // next stimulus:
  Signal.setError( OutData::Unknown );  // unknown
  for ( int tk=0; ! Signal.success() && ! Signal.busy(); tk++ ) {

    if ( tk > 40 ) {
      warning( "Could not establish valid intensity!<br>Signal error <b>" +
	       Signal.errorText() + "</b>,<br><b>" +
	       Str( tk ) + "</b> tries.<br>Exit now!" );
      return Failed;
    }

    Signal.clearError();

    ++IntensityRange;

    if ( IntensityRange.finishedBlock() && softStop() > FICurveStops ) {
      printlog( "exit f-I curve after block by softStop()=" + Str( softStop() ) + ", FICurveStops=" + Str( FICurveStops ) );
      return Completed;
    }

    // check slopes:
    if ( MinRateSlope > 0.0 && 
	 MaxRateFrac < 1.0 &&
	 IntensityRange.finishedBlock() &&
	 IntensityRange.currentIncrement() <= SlopeIntIncrement ) {

      // skip measurements only in the saturated range:
      // find maximum response:
      double max=0.0;
      for ( unsigned int k=IntensityRange.next( 0 ); 
	    k<results.size(); 
	    k = IntensityRange.next( ++k ) )
	if ( results[k].MeanRate > max )
	  max = results[k].MeanRate;

      // get first data-point above MaxRateFrac*max:
      unsigned int f = 0;
      for ( f=IntensityRange.next( 0 ); 
	    f<results.size(); 
	    f = IntensityRange.next( ++f ) ) {
	if ( results[f].Spikes.size() > 0 && 
	     results[f].MeanRate > MaxRateFrac*max && 
	     results[f].MeanRate > MaxSilentRate ) {
	  // extend range:
	  double maxint = IntensityRange[f] + IntensityExtension;
	  for ( ; f<(unsigned int)IntensityRange.size(); ++f ) {
	    if ( IntensityRange[f] >= maxint ) {
	      f = IntensityRange.next( f );
	      break;
	    }
	  }
	  break;
	}
      }
	  
      // check slopes and skip:
      for ( unsigned int k=f; 
	    k<results.size(); 
	    k = IntensityRange.next( ++k ) ) {
	int l = IntensityRange.previous( k-1 );
	int r = IntensityRange.next( k+1 );
	if ( l < 0 ) {
	  // extrapolate to the left:
	  if ( r < IntensityRange.size() ) {
	    double slope = ( results[r].MeanRate - results[k].MeanRate ) / 
	      ( IntensityRange.value( r ) - IntensityRange.value( k ) );
	    if ( fabs( slope ) < MinRateSlope )
	      IntensityRange.setSkipBelow( k-1 );
	  }
	}
	else if ( r >= int(results.size()) ) {
	  // extrapolate to the right:
	  if ( l >= 0 ) {
	    double slope = ( results[k].MeanRate - results[l].MeanRate ) / 
	      ( IntensityRange.value( k ) - IntensityRange.value( l ) );
	    if ( fabs( slope ) < MinRateSlope )
	      IntensityRange.setSkipAbove( k+1 );
	  }
	}
	else {
	  double slope = ( results[r].MeanRate - results[k].MeanRate ) / 
	    ( IntensityRange.value( r ) - IntensityRange.value( k ) );
	  if ( fabs( slope ) < MinRateSlope )
	    IntensityRange.setSkipBetween( k+1, r-1 );
	}
      }

      IntensityRange.update();
    }

    if ( ! IntensityRange )
      Intensity = *IntensityRange;
    else {
      printlog( "exit f-I curve because of no remaining valid intensity." );
      return Completed;
    }

    Signal.setIntensity( Intensity + MeanIntensity );
    testWrite( Signal );

    if ( Signal.underflow() ) {
      printlog( "next() -> attenuator underflow: increased intensity from " +
		Str( Intensity + MeanIntensity ) + " to " + Str( Signal.intensity() ) );
      IntensityRange.setSkipBelow( IntensityRange.pos() );
      IntensityRange.noCount();
      ++IntensityRange;
      Intensity = *IntensityRange;
    }
    else if ( Signal.overflow() ) {
      printlog( "next() -> attenuator overflow: reduced intensity from " +
		Str( Intensity + MeanIntensity ) + " to " + Str( Signal.intensity() ) );
      IntensityRange.setSkipAbove( IntensityRange.pos() );
      IntensityRange.noCount();
      ++IntensityRange;
      Intensity = *IntensityRange;
    }

  }

  if ( msg ) {
    Str s = "Frequency <b>" + Str( CarrierFrequency * 0.001 ) + " kHz</b>";
    s += ",  Intensity <b>" + Str( *IntensityRange ) + " dB SPL</b>";
    s += ",  Increment <b>" + Str( IntensityRange.currentIncrement() ) + "</b>";
    s += ",  Loop <b>" + Str( IntensityRange.count()+1 ) + "</b>";
    message( s );
  }

  return Continue;
}


class PlotMouseEvent : public QEvent
{

public:

  PlotMouseEvent( const Plot::MouseEvent &me )
    : QEvent( Type( User+11 ) ),
      ME( me )
  {
  }

  Plot::MouseEvent ME;
};


void FICurve::plotMouseEvent( Plot::MouseEvent &me )
{
  if ( ! tryLock( 5 ) ) {
    QCoreApplication::postEvent( this, new PlotMouseEvent( me ) );
    return;
  }
  P[1].lock();
  if ( me.xCoor() == Plot::First && me.yCoor() == Plot::First &&
       me.yPos() > P[1].yminRange() + 0.9*(P[1].ymaxRange() - P[1].yminRange()) ) {
    bool changed = false;
    if ( me.left() && me.released() ) {
      int inx = (int)::round( ( me.xPos() - MinIntensity ) / IntensityStep );
      if ( inx >= IntensityRange.size() )
	inx = IntensityRange.size() - 1;
      if ( inx < 0 )
	inx = 0;
      if ( me.shift() )
	IntensityRange.setSkipBelow( inx, ! IntensityRange.skip( inx ) );
      else if ( me.control() )
	IntensityRange.setSkipAbove( inx, ! IntensityRange.skip( inx ) );
      else
	IntensityRange.setSkip( inx, ! IntensityRange.skip( inx ) );
      changed = true;
    }
    if ( ! PlotIntensitySelection || changed ) {
      plotIntensitySelection();
      P.draw();
    }
    PlotIntensitySelection = true;
    me.setUsed();
  }
  else
    PlotIntensitySelection = false;
  P[1].unlock();
  unlock();
}


void FICurve::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    PlotMouseEvent *pme = dynamic_cast<PlotMouseEvent*>( qce );
    plotMouseEvent( pme->ME );
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( FICurve, auditory );

}; /* namespace auditory */

#include "moc_ficurve.cc"
