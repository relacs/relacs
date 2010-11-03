/*
  auditory/fifield.cc
  Optimized measuring of f-I curves for a range of carrier frequencies.

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
#include <iomanip>
#include <relacs/tablekey.h>
#include <relacs/auditory/session.h>
#include <relacs/auditory/fifield.h>
using namespace relacs;

namespace auditory {


FIField::FIField( void )
{
  setName( "FIField" );
  setVersion( "1.4" );
  setDate( "Oct 1, 2008" );

  // parameter:
  MinFreq = 2500.0;
  MaxFreq = 40000.0;
  FreqStep = 1000.0;
  FreqFac = 1.189207115;
  FreqStr = "";
  FreqShuffle = RangeLoop::Up;
  FreqIncrement = 0;
  ReducedFreqIncrement = 0;
  ReducedFreqNumber = 10;
  MinRateFrac = 0.3;

  // add some parameter as options:
  insertLabel( "Frequencies", "Intensities" ).setStyle( OptWidget::TabLabel );
  insertLabel( "Range", "Intensities" );
  insertNumber( "freqmin", "Intensities", "Minimum stimulus frequency", MinFreq, 2000.0, 80000.0, 1000.0, "Hz", "kHz" );
  insertNumber( "freqmax", "Intensities", "Maximum stimulus frequency", MaxFreq, 2000.0, 80000.0, 1000.0, "Hz", "kHz" );
  insertNumber( "freqstep", "Intensities", "Frequency step", FreqStep, 0.0, 10000.0, 100.0, "Hz", "kHz" );
  insertNumber( "freqfac", "Intensities", "Frequency factor", FreqFac, 0.0, 100.0, 0.01 ).setFormat( "%.9f" );
  insertText( "freqrange", "Intensities", "Frequency range", FreqStr ).setUnit( "kHz" );
  insertSelection( "freqshuffle", "Intensities", "Order of frequencies", RangeLoop::sequenceStrings() );
  insertInteger( "freqincrement", "Intensities", "Initial increment for frequencies", FreqIncrement, -1000, 1000, 1 );
  insertLabel( "Optimization", "Intensities" );
  insertInteger( "reducedfreqincrement", "Intensities", "Reduce frequency range at frequency increments below", ReducedFreqIncrement, 0, 1000, 1 );
  insertInteger( "reducedfreqnumber", "Intensities", "Number of frequencies used for the reduced frequency range", ReducedFreqNumber, 0, 1000, 1 );
  insertNumber( "minratefrac", "setbest", "Fraction of maximum rate required for f-I curve analysis", MinRateFrac, 0.0, 1.0, 0.1, "1", "%" );
  Options::erase( "usethresh" );
  Options::erase( "usesat" );
  Options::erase( "manualskip" );
  Options::erase( "waveform" );
  Options::erase( "carrierfreq" );
  Options::erase( "usebestfreq" );

  addTypeStyle( OptWidget::Bold, Parameter::Label );

  UseThresh = false;
  UseSaturation = false;
  Waveform = 0;
  UseBestFreq = false;

  // variables:
  FieldData.clear();
  BestFICurve.clear();
  BestOnFICurve.clear();
  BestSSFICurve.clear();
  FICurveStops = 2;

  // plot:
  P.lock();
  P[0].setLMarg( 5.0 );
  P[0].setRMarg( 1.0 );
  P[0].setXLabel( "Frequency [kHz]" );
  P[0].setYLabel( "Threshold [dB SPL]" );
  P[1].setLMarg( 4.0 );
  P[1].setRMarg( 2.0 );
  P[1].setXLabel( "Intensity [dB SPL]" );
  P[1].setYLabel( "Firing rate [Hz]" );
  P.unlock();

  // header and keys:
  FIFieldHeader.addInteger( "index2" ).setFlags( 1 );
  FIFieldHeader.addText( "side" ).setFlags( 1 );
  FIFieldHeader.addInteger( "best side" );
  FIFieldHeader.addNumber( "best frequency", "Hz", "%.0f" );
  FIFieldHeader.addNumber( "best threshold", "dB SPL", "%.1f" );
  FIFieldHeader.addNumber( "best slope", "Hz/dB", "%.1f" );
  FIFieldHeader.addNumber( "best intensity", "dB SPL", "%.1f" );
  FIFieldHeader.addNumber( "best rate", "Hz", "%.1f" );
  FIFieldHeader.addNumber( "best saturation", "dB SPL", "%.1f" );
  FIFieldHeader.addNumber( "best maximum rate", "Hz", "%.1f" );
  FIFieldHeader.addInteger( "best nfit" );
  FIFieldHeader.addText( "session time" ).setFlags( 1 );
  FIFieldHeader.addLabel( "status:" ).setFlags( 1 );
  Header.erase( "status:" );
}


FIField::~FIField( void )
{
}


int FIField::main( void )
{
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 ) {
    warning( "No spike trace!" );
    return Failed;
  }

  // get options:
  settings().delFlags( 16 );
  settings().setTypeFlags( 32, -Parameter::Blank );
  MinFreq = number( "freqmin" );
  MaxFreq = number( "freqmax" );
  FreqStep = number( "freqstep" );
  FreqFac = number( "freqfac" );
  FreqStr = text( "freqrange" );
  FreqShuffle = RangeLoop::Sequence( index( "freqshuffle" ) );
  FreqIncrement = integer( "freqincrement" );
  ReducedFreqIncrement = integer( "reducedfreqincrement" );
  ReducedFreqNumber = integer( "reducedfreqnumber" );
  MinIntensity = number( "intmin" );
  MaxIntensity = number( "intmax" );
  IntensityStep = number( "intstep" );
  UseThresh = boolean( "usethresh" );
  IntShuffle = RangeLoop::Sequence( index( "intshuffle" ) );
  IntIncrement = integer( "intincrement" );
  SlopeIntIncrement = integer( "slopeintincrement" );
  MinRateSlope = number( "minrateslope" );
  MaxRateFrac = number( "MaxRateFac" );
  IntensityExtension = number( "extint" );
  SingleRepeat = integer( "singlerepeat" );
  IntBlockRepeat = integer( "blockrepeat" );
  IntRepeat = integer( "repeat" );
  Ramp = number( "ramp" );
  Side = index( "side" );
  Duration = number( "duration" );
  PreWidth = number( "prewidth" );
  Pause = number( "pause" );
  SkipPause = boolean( "skippause" );
  SkipSilentIncrement = integer( "silentintincrement" );
  MaxSilent = integer( "maxsilent" );
  SilentFactor = number( "silentfactor" );
  ResetSilent = index( "resetsilent" );
  RateDt = number( "ratedt" );
  PeakWidth = number( "peakwidth" );
  SSWidth = number( "sswidth" );
  MinRateFrac = number( "minratefrac" );
  SetBest = boolean( "setbest" );
  SetCurves = index( "setcurves" );

  if ( PreWidth > Pause )
    Pause = PreWidth;
  if ( Side > 1 )
    Side = metaData( "Cell" ).index( "best side" );
  if ( SSWidth > Duration )
    SSWidth = Duration;

  // plot trace:
  tracePlotSignal( 2.0*PreWidth+Duration, PreWidth );

  // frequency:
  if ( ! FreqStr.empty() )
    FrequencyRange.set( FreqStr, 1000.0 );
  else if ( FreqFac > 0.0 )
    FrequencyRange.setLog( MinFreq, MaxFreq, FreqFac );
  else
    FrequencyRange.set( MinFreq, MaxFreq, FreqStep );
  FrequencyRange.setIncrement( FreqIncrement );
  FrequencyRange.setSequence( FreqShuffle );
  FrequencyRange.reset();
  CarrierFrequency = *FrequencyRange;

  // intensity:
  IntensityRange.set( MinIntensity, MaxIntensity, IntensityStep,
		      IntRepeat, IntBlockRepeat, SingleRepeat );
  if ( IntIncrement <= 0 )
    IntensityRange.setLargeIncrement();
  else
    IntensityRange.setIncrement( IntIncrement );
  IntensityRange.setSequence( IntShuffle );
  IntensityRange.reset();
  Intensity = *IntensityRange;

  if ( ResetSilent == 2 )
    resetSilentActivity();

  // plot:
  P.lock();
  P[0].clear();
  P[0].setXFallBackRange( 0.001*FrequencyRange.minValue(), 0.001*FrequencyRange.maxValue() );
  P[0].setXRange( Plot::AutoMinScale, Plot::AutoMinScale );
  P[0].setYFallBackRange( MinIntensity, MaxIntensity );
  P[0].setYRange( Plot::AutoMinScale, Plot::AutoMinScale );
  P[1].clear();
  P[1].setXFallBackRange( MinIntensity, MaxIntensity );
  P[1].setXRange( Plot::AutoMinScale, Plot::AutoMinScale );
  P[1].setYFallBackRange( 0.0, 100.0 );
  P[1].setYRange( 0.0, Plot::AutoScale );
  P.unlock();

  // results:
  vector< FIData > results( IntensityRange.size(), FIData( -PreWidth, Duration+Pause-PreWidth, RateDt ) );
  BestFICurve.reserve( IntensityRange.size() );
  BestOnFICurve.reserve( IntensityRange.size() );
  BestSSFICurve.reserve( IntensityRange.size() );
  MaxPlotRate = 50.0;
  MaxRate = 0.0;
  Threshold.reset();
  FIFieldHeader.setDefaults();

  FieldData.resize( FrequencyRange.size(), ThreshData() );
  for ( int k=0; k<FrequencyRange.size(); k++ )
    FieldData[k].Frequency = FrequencyRange.value( k );
  BestIndex = -1;

  // stimulus:
  Signal.setTrace( Speaker[ Side ] );
  applyOutTrace( Signal );  // to get maximum sampling rate!
  Signal.sineWave( CarrierFrequency, Duration, 1.0, Ramp );
  MeanIntensity = 3.0103;
  Signal.back() = 0;
  Signal.setDelay( 0.0 );
  Signal.setError( OutData::Unknown );

  for ( int k=0; ! Signal.success(); k++ ) {

    if ( k > 40 ) {
      warning( "Could not establish valid intensity!<br>Signal error: <b>" +
	       Signal.errorText() + "</b>.<br>Exit now!" );
      Signal.free();
      FieldData.clear();
      BestFICurve.free();
      BestOnFICurve.free();
      BestSSFICurve.free();
      return Failed;
    }

    Signal.setIntensity( Intensity );
    testWrite( Signal );

    if ( Signal.underflow() ) {
      printlog( "start() -> attenuator underflow: " + Str( Signal.intensity() ) );
      IntensityRange.setSkipBelow( IntensityRange.pos() );
      ++IntensityRange;
      Intensity = *IntensityRange;
    }
    else if ( Signal.overflow() ) {
      printlog( "start() -> attenuator overflow: " + Str( Signal.intensity() ) );
      IntensityRange.setSkipAbove( IntensityRange.pos() );
      ++IntensityRange;
      Intensity = *IntensityRange;
    }

  }

  Str s = "Frequency <b>" + Str( *FrequencyRange * 0.001 ) + " kHz</b>";
  s += ",  Increment <b>" + Str( FrequencyRange.currentIncrement() ) + "</b>";
  s += ",  Intensity <b>" + Str( *IntensityRange ) + " dB SPL</b>";
  s += ",  Loop <b>" + Str( IntensityRange.count()+1 ) + "</b>";
  message( s );

  int ds = loop( results );
  save();
  Signal.free();
  FieldData.clear();
  BestFICurve.free();
  BestOnFICurve.free();
  BestSSFICurve.free();
  writeZero( Speaker[ Side ] );
  return ds;
}


void FIField::saveHeader( const string &file )
{
  // create file:
  ofstream df( addPath( file ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  df << '\n';
  FIFieldHeader.save( df, "# ", -1, 1 );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 32, false, true );
  df << '\n';
}


void FIField::saveThreshold( const string &file )
{
  // create file:
  ofstream df( addPath( file ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  FIFieldHeader.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 32, false, true );
  df << '\n';
  TableKey key;
  key.addLabel( "threshold" );
  key.addNumber( "f_c", "kHz", "%6.3f" );
  key.addNumber( "I_th", "dB SPL", "%5.1f" );
  key.addNumber( "s.d.", "dB SPL", "%5.1f" );
  key.addNumber( "slope", "Hz/dB", "%5.1f" );
  key.addNumber( "s.d.", "Hz/dB", "%5.1f" );
  key.addLabel( "rate" );
  key.addNumber( "I_r", "dB SPL", "%5.1f" );
  key.addNumber( "s.d.", "dB SPL", "%5.1f" );
  key.addNumber( "r", "Hz", "%5.1f" );
  key.addLabel( "saturation" );
  key.addNumber( "I_max", "dB SPL", "%5.1f" );
  key.addNumber( "s.d.", "dB SPL", "%5.1f" );
  key.addNumber( "f_max", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.saveKey( df, true, true );

  // write data:
  double rate = metaData( "Cell" ).number( "best rate" );
  for ( unsigned int k=0; k<FieldData.size(); k++ ) {
    if ( FieldData[k].Measured ) {
      key.save( df, 0.001*FieldData[k].Frequency, 0 );
      key.save( df, FieldData[k].Threshold );
      key.save( df, FieldData[k].ThresholdSD );
      key.save( df, FieldData[k].Slope );
      key.save( df, FieldData[k].SlopeSD );
      key.save( df, FieldData[k].RateIntensity );
      key.save( df, FieldData[k].RateIntensitySD );
      key.save( df, rate );
      key.save( df, FieldData[k].Saturation );
      key.save( df, FieldData[k].SaturationSD );
      key.save( df, FieldData[k].MaxRate );
      key.save( df, FieldData[k].MaxRateSD );
      df << '\n';
    }
  }
  df << '\n' << '\n';
}


void FIField::save( void )
{
  if ( aborted() ) {
    FieldData[FrequencyRange.pos()].Measured = false;
  }

  Options &mo = metaData( "Cell" );

  if ( SetBest && BestIndex >= 0 ) {
    auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
    string ss = Side == 1 ? "right" : "left";

    // f-I curve parameter:
    mo.setNumber( ss + " frequency", FieldData[BestIndex].Frequency );
    mo.setNumber( ss + " threshold", FieldData[BestIndex].Threshold, FieldData[BestIndex].ThresholdSD );
    mo.setNumber( ss + " slope", FieldData[BestIndex].Slope, FieldData[BestIndex].SlopeSD );
    mo.setNumber( ss + " intensity", FieldData[BestIndex].RateIntensity, FieldData[BestIndex].RateIntensitySD );
    mo.setNumber( ss + " saturation", FieldData[BestIndex].Saturation, FieldData[BestIndex].SaturationSD );
    mo.setNumber( ss + " maxrate", FieldData[BestIndex].MaxRate, FieldData[BestIndex].MaxRateSD );

    // best side:
    as->updateBestSide();    

    // best side parameter:
    if ( Side == mo.index( "best side" ) ) {
      mo.setNumber( "best frequency", FieldData[BestIndex].Frequency );
      mo.setNumber( "best threshold", FieldData[BestIndex].Threshold, FieldData[BestIndex].ThresholdSD );
      mo.setNumber( "best slope", FieldData[BestIndex].Slope, FieldData[BestIndex].SlopeSD );
      mo.setNumber( "best intensity", FieldData[BestIndex].RateIntensity, FieldData[BestIndex].RateIntensitySD );
      mo.setNumber( "best maxrate", FieldData[BestIndex].MaxRate, FieldData[BestIndex].MaxRateSD );
      mo.setNumber( "best saturation", FieldData[BestIndex].Saturation, FieldData[BestIndex].SaturationSD );
    }

    // threshold curve:
    MapD tm;
    for ( unsigned int k=0; k<FieldData.size(); k++ )
      if ( FieldData[k].Measured && FieldData[k].Threshold > 0.0 ) {
	tm.push( FieldData[k].Frequency, FieldData[k].Threshold );
      }
    as->addThreshCurve( tm, Side );

    // f-I curves:
    if ( SetCurves == 1 || SetCurves == 3 ) {
      as->addFICurve( BestFICurve, Side, FieldData[BestIndex].Frequency );
    }
    else if ( SetCurves == 2 || SetCurves == 3 ) {
      as->addOnFICurve( BestOnFICurve, Side, FieldData[BestIndex].Frequency );
      as->addSSFICurve( BestSSFICurve, Side, FieldData[BestIndex].Frequency );
    }

  }

  FIFieldHeader.setInteger( "index2", totalRuns()-1 );
  FIFieldHeader.setInteger( "side", Side );
  FIFieldHeader.setInteger( "best side", mo.index( "best side" ) );
  if ( BestIndex >= 0 ) {
    FIFieldHeader.setNumber( "best frequency", FieldData[BestIndex].Frequency );
    FIFieldHeader.setNumber( "best threshold", FieldData[BestIndex].Threshold, FieldData[BestIndex].ThresholdSD );
    FIFieldHeader.setNumber( "best slope", FieldData[BestIndex].Slope, FieldData[BestIndex].SlopeSD );
    FIFieldHeader.setNumber( "best intensity", FieldData[BestIndex].RateIntensity, FieldData[BestIndex].RateIntensitySD );
    FIFieldHeader.setNumber( "best saturation", FieldData[BestIndex].Saturation, FieldData[BestIndex].SaturationSD );
    FIFieldHeader.setNumber( "best maximum rate", FieldData[BestIndex].MaxRate, FieldData[BestIndex].MaxRateSD );
    FIFieldHeader.setInteger( "best nfit", FieldData[BestIndex].N );
  }
  FIFieldHeader.setNumber( "best rate", mo.number( "best rate" ) );
  FIFieldHeader.setText( "session time", sessionTimeStr() );

  saveThreshold( "fifield.dat" );
}


void FIField::plot( const vector< FIData > &results )
{
  P.lock();

  // threshold
  P[0].clear();
  if ( BestIndex >= 0 && FieldData[BestIndex].Frequency >= FrequencyRange.minValue() )
    P[0].plotVLine( 0.001*FieldData[BestIndex].Frequency, Plot::White, 2 );
  if ( BestIndex >= 0 && FieldData[BestIndex].Threshold >= IntensityRange.minValue() )
    P[0].plotHLine( FieldData[BestIndex].Threshold, Plot::White, 2 );
  MapD tm, im, mm;
  for ( unsigned int k=0; k<FieldData.size(); k++ )
    if ( FieldData[k].Measured && FieldData[k].Threshold > 0.0 ) {
      tm.push( FieldData[k].Frequency, FieldData[k].Threshold );
      im.push( FieldData[k].Frequency, FieldData[k].RateIntensity );
      mm.push( FieldData[k].Frequency, FieldData[k].Saturation );
    }
  P[0].plot( mm, 0.001, Plot::Red, 2, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P[0].plot( im, 0.001, Plot::Orange, 2, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );
  P[0].plot( tm, 0.001, Plot::Yellow, 2, Plot::Solid, Plot::Circle, 6, Plot::Yellow, Plot::Yellow );

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
  P[1].plot( om, 1.0, Plot::Green, 3, Plot::Solid, Plot::Circle, 6, Plot::Green, Plot::Green );
  P[1].plot( sm, 1.0, Plot::Red, 3, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P[1].plot( cm, 1.0, Plot::Orange, 3, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );
  int c = IntensityRange.pos();
  MapD am;
  am.push( results[c].Intensity, results[c].PreRate );
  am.push( results[c].Intensity, results[c].OnRate );
  am.push( results[c].Intensity, results[c].SSRate );
  am.push( results[c].Intensity, results[c].MeanRate );
  P[1].plot( am, 1.0, Plot::Transparent, 3, Plot::Solid, Plot::Circle, 8, Plot::Yellow, Plot::Transparent );

  P.draw();

  P.unlock();
}


RePro::DoneState FIField::next( vector< FIData > &results, bool msg ) 
{
  bool sb = SetBest;
  SetBest = false;
  analyzeFICurve( results, MinRateFrac*MaxRate );
  SetBest = sb;

  // update threshold:
  if ( Threshold.Measured ) {
    FieldData[ FrequencyRange.pos() ] = Threshold;
    if ( MaxRate < Threshold.MaxRate )
      MaxRate = Threshold.MaxRate;
  }
  
  DoneState ds = FICurve::next( results, false );

  if ( ds == Failed || ds == Completed ) {

    // save data:
    if ( ds == Completed ) {
      if ( FrequencyRange.loop() == 0 ) {
	FIFieldHeader.setInteger( "index2", totalRuns() );
	FIFieldHeader.setInteger( "side", Side );
	FIFieldHeader.setText( "session time", sessionTimeStr() );
	saveHeader( "fifieldspikes.dat" );
	saveHeader( "fifieldrates.dat" );
	saveHeader( "fifieldficurves.dat" );
      }
      setHeader();
      Header.setInteger( "index1", FrequencyRange.loop() );
      saveSpikes( "fifieldspikes.dat", results );
      saveRates( "fifieldrates.dat", results );
      saveFICurve( "fifieldficurves.dat", results );

      // update bestindex:
      double bestthresh = IntensityRange.maxValue();
      BestIndex = -1;
      for ( unsigned int k=0; k<FieldData.size(); k++ )
	if ( FieldData[k].Measured &&
	     FieldData[k].Threshold > 0.0 &&
	     FieldData[k].Threshold < bestthresh ) {
	  BestIndex = k;
	  bestthresh = FieldData[k].Threshold;
	}
      if ( BestIndex == FrequencyRange.pos() ) {
	// store f-I curves:
	BestFICurve.clear();
	BestOnFICurve.clear();
	BestSSFICurve.clear();
	for ( unsigned int k=0; k<results.size(); k++ ) {
	  if ( results[k].Spikes.size() > 0 ) {
	    BestFICurve.push( results[k].Intensity, results[k].MeanRate );
	    BestOnFICurve.push( results[k].Intensity, results[k].OnRate );
	    BestSSFICurve.push( results[k].Intensity, results[k].SSRate );
	  }
	}
	
      }
    }

    if ( softStop() > 1 ) {
      return Completed;
    }

    ++FrequencyRange;

    if ( FrequencyRange.finishedBlock() ) {
      if ( softStop() > 0 ) {
	return Completed;
      }
      if ( BestIndex >= 0 && FreqShuffle > 1 ) {
	FrequencyRange.update( BestIndex );
      }
      if ( FrequencyRange.currentIncrement() <= ReducedFreqIncrement &&
	   BestIndex >= 0 ) {
	int l = BestIndex - ReducedFreqNumber/2;
	int r = l + ReducedFreqNumber + 1;
	FrequencyRange.setSkipBelow( l );
	FrequencyRange.setSkipAbove( r );
	FrequencyRange.setSkipBetween( l+1, r-1, false );
	FrequencyRange.update( FreqShuffle > 1 ? BestIndex : -1 );
      }
    }

    if ( ! FrequencyRange ) {

      // clear results:
      results.clear();
      results.resize( IntensityRange.size(), FIData( -PreWidth, Duration+Pause-PreWidth, RateDt ) );
      MaxPlotRate = 50.0;
      Threshold.reset();

      // clear intensities:
      if ( IntIncrement <= 0 )
	IntensityRange.setLargeIncrement();
      else
	IntensityRange.setIncrement( IntIncrement );
      IntensityRange.setSequence( IntShuffle );
      IntensityRange.reset();
      Intensity = *IntensityRange;

      // new signal:
      CarrierFrequency = *FrequencyRange;
      Signal.free();
      Signal.setTrace( Speaker[ Side ] );
      applyOutTrace( Signal );  // to get maximum sampling rate!
      Signal.sineWave( CarrierFrequency, Duration, 1.0, Ramp );
      Signal.back() = 0;
      Signal.setDelay( 0.0 );
      Signal.setError( OutData::Unknown );

      for ( int k=0; ! Signal.success() && ! Signal.busy(); k++ ) {
	
	if ( k > 40 ) {
	  warning( "Could not establish valid intensity!<br>Signal error: <b>" +
		   Signal.errorText() + "</b>,<br><b>" +
		   Str( k ) + "</b> tries.<br>Exit now! " + Str( k ) );
	  return Failed;
	}
	
	Signal.setIntensity( Intensity );
	testWrite( Signal );
	
	if ( Signal.underflow() ) {
	  printlog( "next() -> attenuator underflow: " + Str( Signal.intensity() ) );
	  IntensityRange.setSkipBelow( IntensityRange.pos() );
	  ++IntensityRange;
	  Intensity = *IntensityRange;
	}
	else if ( Signal.overflow() ) {
	  printlog( "next() -> attenuator overflow: " + Str( Signal.intensity() ) );
	  IntensityRange.setSkipAbove( IntensityRange.pos() );
	  ++IntensityRange;
	  Intensity = *IntensityRange;
	}
	
      }

    }
    else {
      return Completed;
    }

  }

  if ( msg ) {
    Str s = "Frequency <b>" + Str( *FrequencyRange * 0.001 ) + " kHz</b>";
    s += ",  Increment <b>" + Str( FrequencyRange.currentIncrement() ) + "</b>";
    s += ",  Intensity <b>" + Str( *IntensityRange ) + " dB SPL</b>";
    s += ",  Loop <b>" + Str( IntensityRange.count()+1 ) + "</b>";
    message( s );
  }

  return Continue;
}


addRePro( FIField );

}; /* namespace auditory */

#include "moc_fifield.cc"
