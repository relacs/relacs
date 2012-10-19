/*
  auditoryprojects/isoresponse.cc
  Measures f-I-curves for a superposition of two sine waves.

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
#include <iomanip>
#include <cmath>
#include <relacs/stats.h>
#include <relacs/map.h>
#include <relacs/str.h>
#include <relacs/tablekey.h>
#include <relacs/auditory/session.h>
#include <relacs/auditoryprojects/isoresponse.h>
using namespace relacs;

namespace auditoryprojects {


IsoResponse::IsoResponse( void )
  : RePro( "IsoResponse", "auditoryprojects",
	   "Alexander Wolf", "1.0", "Jan 10, 2008" )
{

  pi=4.0*atan(1.0);
  A0=0.02; // air pressure in mPa

  // parameter:

  UseBestFrequency = 0;
  DistBestFrequency = 1000.0;
  DistFrequencies = 2000.0;
  Frequency1 = 4000.0;
  Frequency2 = 10000.0;
  MinIntensity = 30.0;
  MaxIntensity = 100.0;
  FinalResolution = 1.0;
  EstimResolution = 5.0;

  ShortStim = 1;
  Duration = 0.2;
  Pause = 0.3;
  OnWidth = 0.01;
  SSWidth = 0.10;

  Repeats = 12;
  EstRepeats = 3;

  IntensityUp = 5;    
  Switch_high = 0;    //switch=0 => percentage
  FRhigh_per  = 0.05;          //MaxFrequency-5%
  FRhigh_fix  = 500.0;
  Switch_low  = 0;     //switch=1 => fixed value
  FRlow_per   = 0.5;
  FRlow_fix   = 150.0;             //150 Hz
  MinFR = 30.0;

  IrsNumber = 5;
  IrsPoints = 33;


  // add some parameter as options:

  addSection( "Stimulus" ).setStyle( OptWidget::Bold | OptWidget::TabLabel );

  addSubSection( "Frequencies" ).setStyle( OptWidget::Bold );
  addText( "use_best_freq", "Frequency reference", "absolute|best|isothresh" ).setStyle( Parameter::SelectText );
  addNumber( "dist_best_freq", "Distance of f1 and f2 from best Frequency (for 'best')", DistBestFrequency, 500.0, 20000.0, 500.0, "Hz", "kHz*3/pi" );
  addNumber( "dist_freq", "Distance between f1 and f2 (for 'isothresh')", DistFrequencies, 500.0, 20000.0, 500.0, "Hz", "kHz*3/pi" );
  addNumber( "f1", "1st Frequency", Frequency1, 2000.0, 40000.0, 500.0, "Hz", "kHz" );
  addNumber( "f2", "2nd Frequency", Frequency2, 2000.0, 40000.0, 500.0, "Hz", "kHz*3/pi" );

  addSubSection( "Intensities" ).setStyle( OptWidget::Bold );
  addNumber( "intmin", "Minimum stimulus intensity", MinIntensity, 0.0, 200.0, 5.0, "dB SPL" );
  addNumber( "intmax", "Maximum stimulus intensity", MaxIntensity, 0.0, 200.0, 5.0, "dB SPL" );
  addNumber( "intres", "Final intensity resolution", FinalResolution, 0.5, 10.0, 0.5, "dB SPL" );
  addNumber( "estres", "Resolution for estimation of FR-range", EstimResolution, 1.0, 20.0, 1.0, "dB SPL" );
  addText( "intshuffle", "Order of intensities", RangeLoop::sequenceStrings() ).setStyle( Parameter::SelectText );

  addSubSection( "Stimulus Structure" ).setStyle( OptWidget::Bold );
  addBoolean( "short_stim", "Measure Onset Response only", ShortStim );
  addNumber( "duration", "Stimulus duration", Duration, 0.0, 10.0, 0.005, "seconds", "ms" );
  addNumber( "pause", "Pause between stimuli", Pause, 0.0, 10.0, 0.01, "seconds", "ms" );
  addNumber( "onwidth", "Window length for onset firing rate", OnWidth, 0.0, 10.0, 0.005, "seconds", "ms" );
  addNumber( "sswidth", "Window length for steady-state firing rate", SSWidth, 0.0, 10.0, 0.01, "seconds", "ms" );


  addSection( "Analysis" ).setStyle( OptWidget::Bold | OptWidget::TabLabel );
  addInteger( "repeats", "Number of repeats for each stimulus",  Repeats, 1, 100, 1 );
  addInteger( "estrepeats", "Repeats for stimulus while estimating f-I-curve",  EstRepeats, 1, 100, 1 );
  addText( "side", "Speaker", "best|left|right" ).setStyle( Parameter::SelectText );

  addSubSection( "Analysis Bounds" ).setStyle( OptWidget::Bold );
  addNumber( "intup", "Increases upper intensity bound by: ", IntensityUp, 0.0, 20.0, 1.0, "dB SPL" );
  addBoolean( "switch_high", "Higher bound FR fixed", Switch_high );
  addNumber( "per_high", "Percentage of max FR", FRhigh_per , 0.0, 1.0, 0.01, "1", "%" );
  addNumber( "fix_high", "Absolute value for bound of upper FR", FRhigh_fix , 0.0, 800.0, 10, "Hz" );
  addBoolean( "switch_low", "Lower bound FR fixed", Switch_low );
  addNumber( "per_low", "Percentage of max FR", FRlow_per , 0.0, 1.0, 0.01, "1", "%" );
  addNumber( "fix_low", "Absolute value for lower bound of FR", FRlow_fix , 0.0, 800.0, 10, "Hz" );
  addNumber( "minfr", "Minimum firing rater response", MinFR , 0.0, 800.0, 10, "Hz" );

  addSubSection( "Iso-Response-Sets" ).setStyle( OptWidget::Bold );
  addInteger( "isonumber", "Number of Iso-Response-Sets evaluated", IrsNumber, 1, 100, 1 );
  addInteger( "isopoints", "Number of data points on each IRS", IrsPoints, 2, 100, 1 );


  // variables:
  AmplitudeRelation.clear();
  AmplFraction = 0.0;
  AmplRatio = 0.0;
  BestFreq = 0.0;
  Flow = 0.0;
  Fup = 0.0;
  IntCorrection = 0.0;
  IntensityRange.clear();
  Intensity = 0.0;
  MaxRate = 0.0;
  MaxRatePos = 0;
  Scaling = 0.0;
  State = 0;

  Results.clear();
  IsoResults.clear();

  // plot:
  P.lock();
  P.resize( 3, 3, true );
  P.unlock();
  setWidget( &P );

  // header:
  Header.addInteger( "run" );
  Header.addInteger( "index" );
  Header.addNumber( "alpha", "pi/2", "%.3f" );
  Header.addNumber( "intcorr", "dB", "%.1f" );
  Header.addNumber( "scaling" );
  Header.addNumber( "best", "Hz", "%.1f" );
  Header.addNumber( "freq1", "Hz", "%.1f" );
  Header.addNumber( "freq2", "Hz", "%.1f" );
}


IsoResponse::~IsoResponse( void )
{
}

void IsoResponse::createSignal( OutData &signal, double a1, double a2 )
{
  signal.resize( 0.0, Duration, 1.0/2.0e5, 0 );
  signal.setTrace( Speaker[ Side ] );
  applyOutTrace( signal );
  double n1=a1/(a1+a2);
  double n2=a2/(a1+a2);
  double o1=2.0*pi*Frequency1;
  double o2=2.0*pi*Frequency2;
  for( int k=0; k<signal.size(); k++ ) {
    double t = signal.pos( k );
    signal[k] = n1*sin(o1*t) + n2*sin(o2*t);
  }
  signal.ramp( 0.002 );
  signal.setIdent( "n1=" + Str( n1, 0, 3, 'f' ) + 
		    ",n2=" + Str( n2, 0, 3, 'f' ) + 
		    "f1=" + Str( Frequency1, 0, 1, 'f' ) + 
		    "Hz,f2=" + Str( Frequency2, 0, 1, 'f' ) + "Hz" );
  IntCorrection =  10.0*log10( 2.0/(n1*n1+n2*n2) );
  IsoResults[AmplitudeRelation.pos()].IntCor=IntCorrection;
  signal.setCarrierFreq( 0.5*(Frequency1+Frequency2) );
}


int IsoResponse::findIsoFreq( void )
{
  double del = DistFrequencies;
  auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
  MapD thresh = as->threshCurve();

  // index of minimum threshold:
    
  int inx     = minIndex( thresh );
  /*int inx = 0;
    double imin = thresh.y(0);
    for( int i=1; i < thresh.size(); i++) {
    if( thresh.y(i)<imin ) {
    inx = i;
    imin = thresh.y(i);
    } 
    }*/

  if ( inx==0 || inx==thresh.size()-1 ) {
    printlog( "Thresh size:  " + Str( thresh.size() ) + "   Min: " + Str( inx ) );
    warning( "Threshold curve not well determined: run 'FIField' again" );
    return Failed;
  }
  int ind_low = inx-1;
  int ind_up  = inx+1;
  double fup  =  thresh.x( ind_up );
  double flow =  thresh.x( ind_low );
  double iup  =  thresh.y( ind_up );
  double ilow =  thresh.y( ind_low );
    
  //find points around right distance
  while ( fup-flow < del ) {
    printlog( "fup: " + Str( fup ) + " flow: " + Str( flow ) + " iup: " + Str( iup ) + " ilow: " + Str( ilow ) );
    if ( ilow <= iup ) {
      if ( ind_low == 0) {
	warning( "Threshold curve not wide enough: run 'FIField' again" );
	return Failed;
      }
      ind_low--; 
      while( thresh.y( ind_low ) < ilow ) {
	if ( ind_low == 0) {
	  warning( "Threshold curve not wide enough: run 'FIField' again" );
	  return Failed;
	}
	ind_low--;
      }
      flow = thresh.x( ind_low ); 
      ilow =  thresh.y( ind_low );
    }
    else {
      if ( ind_up == thresh.size()-1 ) {
	warning( "Threshold curve not wide enough: run 'FIField' again" );
	return Failed;
      }
      ind_up++; 
      while( thresh.y( ind_up ) < iup ) {
	if ( ind_up == thresh.size()-1) {
	  warning( "Threshold curve not wide enough: run 'FIField' again" );
	  return Failed;
	}
	ind_up++;
      }
      fup = thresh.x( ind_up ); 
      iup = thresh.y( ind_up );
    }
    printlog( "fup: " + Str( fup ) + " flow: " + Str( flow ) + " iup: " + Str( iup ) + " ilow: " + Str( ilow ) );
  }

  //interpolate from previously found points
  if( iup > ilow ) {
    double iup0 =  thresh.y(ind_up-1);
    double fup0 =  thresh.x(ind_up-1);
    double m1 = (iup - iup0)/(fup - fup0);
    double n1 = (fup*iup0 - fup0*iup)/(fup-fup0);
    double fstar = (ilow-n1)/m1;
    double ilow0 = 0;
    double flow0 = 0;

    if( fstar-flow < del && ind_low > 0 ) {
      int i = 1;
      while ( ind_low >= i && thresh.y(ind_low-i) <= ilow ) i++;
      if ( ind_low < i ) {                 //no valid lower point found
	ilow0 = thresh.y(ind_low+1);       //interpolation downwards
	flow0 = thresh.y(ind_low+1);
      }
      else{
	ilow0 = thresh.y(ind_low-i);       //interpolation upwards
	flow0 = thresh.x(ind_low-i);
      }
    }
    else {
      ilow0 = thresh.y(ind_low+1);
      flow0 = thresh.x(ind_low+1);
    }
    double m2 = (ilow - ilow0)/(flow - flow0);
    double n2 = (flow*ilow0 - flow0*ilow)/(flow-flow0);
    flow = (n2-n1-m1*del)/(m1-m2);
  }

  else if( iup < ilow ) {
    double ilow0 =  thresh.y(ind_low+1);
    double flow0 =  thresh.x(ind_low+1);
    double m1 = (ilow - ilow0)/(flow - flow0);
    double n1 = (flow*ilow0 - flow0*ilow)/(flow-flow0);
    double fstar = (iup-n1)/m1;
    double iup0 = 0;
    double fup0 = 0;
      
    if( fup-fstar < del && ind_up < thresh.size()-1 ) {
      int i = 1;
      while ( thresh.size()-1 >= i+ind_up && thresh.y(ind_up+i) < iup ) i++;
      if ( thresh.size() == i+ind_up ) { 
	iup0 =  thresh.y(ind_up-1);
	fup0 =  thresh.x(ind_up-1);
      }
      else{
	iup0 = thresh.y(ind_up+i);
	fup0 = thresh.x(ind_up+i);
      }
    }
    else {
      iup0 =  thresh.y(ind_up-1);
      fup0 =  thresh.x(ind_up-1);
    }
    double m2 = (iup - iup0)/(fup - fup0);
    double n2 = (fup*iup0 - fup0*iup)/(fup-fup0);
    flow = (n2-n1-m1*del)/(m1-m2);
  }

  else {
    double ilow0 =  thresh.y(ind_low+1);
    double flow0 =  thresh.x(ind_low+1);
    double m1    = (ilow - ilow0)/(flow - flow0);
    double n1    = (flow*ilow0 - flow0*ilow)/(flow-flow0);
    double iup0  =  thresh.y(ind_up-1);
    double fup0  =  thresh.x(ind_up-1);
    double m2    = (iup - iup0)/(fup - fup0);
    double n2    = (fup*iup0 - fup0*iup)/(fup-fup0);
    flow = (n2-n1-m1*del)/(m1-m2);
  }
  Frequency1 = flow;
  Frequency2 = flow+del;
  return Continue;
}


int IsoResponse::main( void )
{  
  BestFreq = metaData( "Cell" ).number( "best frequency" );
  // get options:
  UseBestFrequency = index( "use_best_freq" );
  if( UseBestFrequency==0 ) {
    Frequency1 = number( "f1" );
    Frequency2 =  number( "f2" ) * 3.0/pi;
  }
  else  if( UseBestFrequency==1 ) {
    if ( BestFreq<= 0.0 ) {
      warning( "Best Frequency smaller 0: Check settings or run 'FIField'" );
      return Failed;
    }
    DistBestFrequency = number( "dist_best_freq" ) * 3.0/pi;
    Frequency1 = BestFreq-DistBestFrequency;
    Frequency2 = BestFreq+DistBestFrequency;
  }
  else {
    DistFrequencies = number( "dist_freq" ) * 3.0/pi;
    int r = findIsoFreq();
    if ( r == Failed )
      return Failed;
  }
  Str st = "BestFrequency: " + Str( BestFreq ) + "Hz, Frequency1: " + Str( Frequency1 ) + "Hz, Frequency2:  " + Str( Frequency2 ) + "Hz";
  info( st, 5.0 ); 

  MinIntensity = number( "intmin" );
  MaxIntensity =  number( "intmax" );
  FinalResolution = number( "intres" );
  EstimResolution =  number( "estres" );
  IntShuffle = RangeLoop::Sequence( index( "intshuffle" ) );

  ShortStim = number( "short_stim" );
  Duration = number( "duration" );
  Pause = number( "pause" );
  SSWidth = number( "sswidth" );
  OnWidth = number( "onwidth" );

  Repeats = integer( "repeats" );
  EstRepeats = integer( "estrepeats" );
  Side = index( "side" );

  IntensityUp = number("intup");
  Switch_high = boolean( "switch_high" );
  FRhigh_per  = number( "per_high" );
  FRhigh_fix  = number( "fix_high" );
  Switch_low  = boolean( "switch_low" );
  FRlow_per   = number( "per_low" );
  FRlow_fix   = number( "fix_low" );
  MinFR   = number( "minfr" );

  IrsNumber = integer( "isonumber" );
  IrsPoints = integer( "isopoints" );

  FIso.resize( IrsNumber );

  if ( Side > 1 )
    Side = metaData( "Cell" ).index( "best side" );

  // Warnings
  if ( Switch_high && Switch_low && FRlow_fix>=FRhigh_fix ) {
    warning( "Upper Frequency Bound greater than Lower Frequency Bound: Repro aborted - check settings" );
    return Failed;
  }
  else if ( !Switch_high && !Switch_low && FRlow_per>=FRhigh_per ) {
    warning( "Upper Frequency Bound greater than Lower Frequency Bound: Repro aborted - check settings" );
    return Failed;
  }
  if ( !ShortStim && Duration<=SSWidth ) {
    warning( "SSWidth greater than Duration: Repro aborted - check settings" );
    return Failed;
  }
   
  // plot trace:
  tracePlotSignal( Duration );

  // plot:
  P.lock();
  P[0].clear();
  P[0].setXRange( MinIntensity, MaxIntensity );
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[1].clear();
  P[1].setXRange( 0.0, Plot::AutoScale );
  P[1].setYRange( 0.0, Plot::AutoScale );
  P[2].clear();
  P[2].setXRange( 0.0, Plot::AutoScale );
  P[2].setYRange( 0.0, Plot::AutoScale );
  P.unlock();

  // intensity: 
  IntensityRange.clear();
  IntensityRange.set( MinIntensity, MaxIntensity, FinalResolution,
		      1, EstRepeats, 1 );
  IntensityRange.setLargeIncrement();
  IntensityRange.setSequence( IntShuffle );
  IntensityRange.reset();
  Intensity = *IntensityRange;

  Results.clear();
  Results.resize( IntensityRange.size() );

  // angle
  AmplitudeRelation.clear();
  AmplitudeRelation.set( 0.0, 1.0, 1.0/(IrsPoints-1.0), 1, 1, 1 );
  AmplitudeRelation.setLargeIncrement();
  AmplitudeRelation.alternateInUp(); 
  AmplitudeRelation.reset();
  AmplFraction = *AmplitudeRelation;

  IsoResults.clear();
  IsoResults.resize( AmplitudeRelation.size() );

  OutData signal;
  Flow = 0.0;
  Fup = 0.0;
  IntCorrection = 0.0;
  MaxRate = 0.0;
  MaxRatePos = 0;
  Scaling = 0.0;
  State = 0;

  for ( AmplitudeRelation.reset(); ! AmplitudeRelation; ++AmplitudeRelation, ++State ) {

    AmplFraction = *AmplitudeRelation;

    // create stimulus:
    if ( State == 0 ) 
      createSignal( signal, 1.0, 0.0 );  //creates signal with pure sine
    else if ( State == 1 ) 
      createSignal( signal, 0.0, 1.0 );
    else {
      if ( State == 2 )
	findScaling();
      AmplRatio = tan(pi/2.0*AmplFraction)*Scaling;
      createSignal( signal, 1.0, AmplRatio );
    }

    Results.clear();
    Results.resize( IntensityRange.size() );

    IntensityRange.setLargeIncrement();
    IntensityRange.setBlockRepeat( EstRepeats );

    for ( IntensityRange.reset(); ! IntensityRange; ++IntensityRange ) {

      Intensity = *IntensityRange;
      signal.setIntensity( Intensity+IntCorrection );
      testWrite( signal );
      if ( signal.underflow() ) {
	printlog( "read() -> attenuator underflow: " + Str( signal.intensity() ) );
	IntensityRange.setSkipBelow( IntensityRange.pos() );
	continue;
      }
      else if ( signal.overflow() ) {
	printlog( "read() -> attenuator overflow: " + Str( signal.intensity() ) );
	IntensityRange.setSkipAbove( IntensityRange.pos() );
	continue;
      }
      write( signal );
      
      Str s = "Intensity <b>" + Str( *IntensityRange ) + " dB SPL</b>";
      s += ",  Increment <b>" + Str( IntensityRange.currentIncrement() ) + "</b>";
      s += ",  Loop <b>" + Str( IntensityRange.count()+1 ) + "</b>";
      s += ",  Angle <b>" + Str( *AmplitudeRelation, 0, 5, 'g' ) + "*pi/2.0</b>" ;
      message( s );
      
      sleep( Duration + Pause );
      if ( interrupt() ) {
	Results.clear();
	writeZero( Speaker[ Side ] ); 
	return Aborted;
      }

      // adjust gain of daq board:
      if ( events( SpikeEvents[0] ).count( signalTime(),
					   signalTime() + Duration ) > 2 ) {
	adjust( trace( SpikeTrace[0] ), signalTime(),
		signalTime() + Duration, 0.8 );
	//	activateGains();
      }
      
      analyze( signal );
      
      plot( );

      if ( IntensityRange.finishedBlock() &&
	   IntensityRange.currentIncrement() <= (int)::rint( EstimResolution / FinalResolution ) ) {
	// printlog( "setFRBounds" );
	// if( State==0 ) 
	setFRBounds( MaxRate );
	setIBounds();
      }

    }

    // save data of f-I curve:
    Header.setInteger( "run", totalRuns() );
    Header.setInteger( "index", AmplitudeRelation.loop() );
    Header.setNumber( "alpha", AmplFraction );
    Header.setNumber( "intcorr", IntCorrection );
    Header.setNumber( "scaling", Scaling );
    Header.setNumber( "best", BestFreq );
    Header.setNumber( "freq1", Frequency1 );
    Header.setNumber( "freq2", Frequency2 );
    Header.addSection( "status" );
    Header.append( stimulusData() );
    Header.addSection( "settings" );
    Header.append( settings() );
    saveSpikes();
    saveRates();

    IsoResults[AmplitudeRelation.pos()].FinalResults=Results;
    plotIsoSets();

    if ( softStop() > 1 ) {
      Results.clear();
      writeZero( Speaker[ Side ] ); 
      return Completed;
    }

  }

  Results.clear();
  writeZero( Speaker[ Side ] ); 
  return Completed;
}


void IsoResponse::saveSpikes( void )
{
  // create file:
  ofstream df( addPath( "isoresponsespikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:

  Header.save( df, "# ", 0, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  // write data:
  int n = 0;
  for ( unsigned int k=0; k<Results.size(); k++ )
    if ( Results[k].Spikes.size() > 0 ) {
      df << "#         index0: " << n++ << '\n';
      df << "#      intensity: " << Results[k].Intensity << "dB SPL\n";
      df << "# true intensity: " << Results[k].TrueIntensity << "dB SPL\n";
      df << "#             a1: " << Results[k].A1 << "mPa\n";
      df << "#             a2: " << Results[k].A2 << "mPa\n";
      Results[k].Spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
      df << '\n';
    }
  df << '\n';
}


void IsoResponse::saveRates( void )
{
  // create file:
  ofstream df( addPath( "isoresponserates.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:

  Header.save( df, "# ", 0, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "I"   , "dB SPL", "%5.1f" );
  key.addNumber( "I_t" , "dB SPL", "%5.1f" );
  key.addNumber( "A1"  , "mPa", "%6.2f" );
  key.addNumber( "A2"  , "mPa", "%6.2f" );
  key.addNumber( "f_on", "Hz", "%5.1f" );
  key.addNumber( "sd"  , "Hz", "%5.1f" );
  key.addNumber( "f_s" , "Hz", "%5.1f" );
  key.addNumber( "sd"  , "Hz", "%5.1f" );
  key.addNumber( "f_m" , "Hz", "%5.1f" );
  key.addNumber( "sd"  , "Hz", "%5.1f" );
  key.addNumber( "trials", "1", "%5.0f" );
  key.saveKey( df, true, false );
  df << '\n';

  // write data:
  //  int n = 0;
  //cerr << "saveRates: State " << State <<  "   A1 "<< "     A2 " << endl;
  for ( unsigned int k=0; k<Results.size(); k++ )
    if ( Results[k].Spikes.size() > 0 ) {
      key.save( df, Results[k].Intensity, 0 );
      key.save( df, Results[k].TrueIntensity );
      key.save( df, Results[k].A1 );
      key.save( df, Results[k].A2 );
      //  cerr << Results[k].A1 << "       " << Results[k].A2 << endl;
      key.save( df, Results[k].OnRate );
      key.save( df, Results[k].OnRateSD );
      key.save( df, Results[k].SSRate );
      key.save( df, Results[k].SSRateSD );
      key.save( df, Results[k].MeanRate );
      key.save( df, Results[k].MeanRateSD );
      key.save( df, Results[k].Spikes.size() );
      df << '\n';
    }
  df << '\n';
}


void IsoResponse::plot( void )
{
  P.lock();

  // f-I curve:
  P[0].clear();
  MapD sm, cm, om;
  
  for ( unsigned int k=0; k<Results.size(); k++ ) {
    if ( Results[k].Spikes.size() > 0 ) {
      if ( !ShortStim ) sm.push( IntensityRange.value( k ), Results[k].SSRate );
      cm.push( IntensityRange.value( k ), Results[k].MeanRate );
      om.push( IntensityRange.value( k ), Results[k].OnRate );
    }
  }
  if ( !ShortStim ) 
    P[0].plot( sm, 1.0, Plot::Red,    3, Plot::Solid, Plot::Circle, 6, Plot::Red,    Plot::Red    );
  P[0].plot( cm, 1.0, Plot::Orange, 3, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );
  P[0].plot( om, 1.0, Plot::Blue,   3, Plot::Solid, Plot::Circle, 6, Plot::Blue,   Plot::Blue   );
  int c = IntensityRange.pos();
  MapD am;
  if ( !ShortStim ) am.push( Results[c].Intensity, Results[c].CurSSRate   );
  am.push( Results[c].Intensity, Results[c].CurMeanRate );
  am.push( Results[c].Intensity, Results[c].CurOnRate   );
  P[0].plot( am, 1.0, Plot::Transparent, 3, Plot::Solid, Plot::Circle, 8, Plot::Yellow, Plot::Transparent );

  P.draw();

  P.unlock();
}


void IsoResponse::plotIsoSets(  )
{
  double a1, a2;
  if ( !ShortStim )  //if ( SSWidth <= Duration )
    {
      /********************************************************************************************************/
      /*Plot Steady State Iso Sets*****************************************************************************/
      /********************************************************************************************************/
      IsoResults[AmplitudeRelation.pos()].IsoSets.clear();
      IsoResults[AmplitudeRelation.pos()].IsoSets.reserve( IrsNumber );
      
      //determine reference firing rates
      for (int l=0; l<IrsNumber; l++ )
	{    
	  double pressrate = -1.0;
	  double preint = -1.0;
	  int pk = -1;
	  for ( unsigned int k=0; 
		k<Results.size(); 
		k++ )
	    {
	      double currssrate = Results[k].SSRate;
	      double currint = Results[k].Intensity;
	      if ( currssrate >= 0.0 ) 
		{
		  if ( pressrate >= 0.0 &&
		       pressrate <= FIso[l] &&
		       currssrate > FIso[l] ) 
		    {
		      double slope = (currint - preint)/(currssrate - pressrate);
		      double irms = currint - slope*(currssrate-FIso[l]);
		      if ( State==0 ) {
			a1 = A0 * ::pow( 10.0, irms/20 )*sqrt(2.0);
			a2 = 0.0;
		      }
		      else if ( State==1 ) {
			a1 = 0.0;
			a2 = A0 * ::pow( 10.0, irms/20 )*sqrt(2.0);
		      }
		      else {
			a1 = A0 * sqrt( 2.0/(1+AmplRatio*AmplRatio) ) * ::pow( 10.0, irms/20 );
			a2 = a1*AmplRatio;
		      }
		      IsoResults[AmplitudeRelation.pos()].IsoSets.push( a1, a2 );
		      break; // ?????????????
		    }
		  pressrate = currssrate;
		  preint = currint;
		  pk = k;
		}
	    }
	}
      
      /*Three point spline*/

      P.lock();
      
      //plot iso-response-sets
      for (int l=0; l<IsoResults[AmplitudeRelation.pos()].IsoSets.size(); l++ ) {
	MapD p;
	p.push( IsoResults[AmplitudeRelation.pos()].IsoSets.x( l ), 
		IsoResults[AmplitudeRelation.pos()].IsoSets.y( l ) ); 
	int color = 3+l;
	P[2].plot( p, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 6, color, color );
      }
      
      P.draw();

      P.unlock();
    }
  /********************************************************************************************************/
  /*Plot Onset Iso Sets************************************************************************************/
  /********************************************************************************************************/
  IsoResults[AmplitudeRelation.pos()].OnIsoSets.clear();
  IsoResults[AmplitudeRelation.pos()].OnIsoSets.reserve( IrsNumber );

  //determine reference firing rates
  for (int l=0; l<IrsNumber; l++ )
    {    
      double pressrate = -1.0;
      double preint = -1.0;
      int pk = -1;
      for ( unsigned int k=0; 
	    k<Results.size(); 
	    k++ )
	{
	  double currssrate = Results[k].OnRate;
	  double currint = Results[k].Intensity;
	  if ( currssrate >= 0.0 ) 
	    {
	    if ( pressrate >= 0.0 &&
		 pressrate <= FIso[l] &&
		 currssrate > FIso[l] ) 
	      {
		double slope = (currint - preint)/(currssrate - pressrate);
		double irms = currint - slope*(currssrate-FIso[l]);
		if ( State==0 ) {
		  a1 = A0 * ::pow( 10.0, irms/20 )*sqrt(2.0);
		  a2 = 0.0;
		}
		else if ( State==1 ) {
		  a1 = 0.0;
		  a2 = A0 * ::pow( 10.0, irms/20 )*sqrt(2.0);
		}
		else {
		  a1 = A0*sqrt( 2.0/(1+AmplRatio*AmplRatio) ) * ::pow( 10.0, irms/20 );
		  a2 = a1*AmplRatio;
		}
		IsoResults[AmplitudeRelation.pos()].OnIsoSets.push( a1, a2 );
		break; 
	      }
	    pressrate = currssrate;
	    preint = currint;
	    pk = k;
	    }
	}
    }

  /*Three point spline*/

  P.lock();

  //plot iso-response-sets
  for (int l=0; l<IsoResults[AmplitudeRelation.pos()].OnIsoSets.size(); l++ ) {
    MapD p;
    p.push( IsoResults[AmplitudeRelation.pos()].OnIsoSets.x( l ), 
	    IsoResults[AmplitudeRelation.pos()].OnIsoSets.y( l ) ); 
    int color = 3+l;
    P[1].plot( p, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 6, color, color );
  }

  P.draw();

  P.unlock();
}



void IsoResponse::analyze( const OutData &signal )
{

  const EventData &spikes = events( SpikeEvents[0] );
  FIData &fid = Results[IntensityRange.pos()];

  // spikes:
  fid.Spikes.push( spikes, signalTime(),
		   signalTime() + Duration + Pause );

  // frequencies
  fid.OnRate = fid.Spikes.frequency( 0.0, OnWidth, fid.OnRateSD );
  fid.CurOnRate = fid.Spikes.back().frequency( 0.0, OnWidth );
  if ( !ShortStim ) { //if ( SSWidth <= Duration ) 
    fid.SSRate = fid.Spikes.frequency( Duration-SSWidth, Duration, fid.SSRateSD );
    fid.CurSSRate = fid.Spikes.back().frequency( Duration-SSWidth, Duration );
  }
  else {
    fid.SSRate = 0.0;
    fid.CurSSRate = 0.0;
  }
  fid.MeanRate = fid.Spikes.frequency( 0.0, Duration, fid.MeanRateSD );
  fid.CurMeanRate = fid.Spikes.back().frequency(  0.0, Duration );

  // intensities:
  fid.Intensity = *IntensityRange;
  fid.TrueIntensity = signal.intensity() - IntCorrection;

  // amplitudes
  if ( State==0 ) {
    fid.A1 = A0 * ::pow( 10.0, fid.Intensity/20 )*sqrt(2.0);
    fid.A2 = 0.0;
    //cerr << "analyze: State " << State <<  "   A1 " << fid.A1 << "    A2 " << fid.A2 << endl;
  }
  else if ( State==1 ) {
    fid.A1 = 0.0;
    fid.A2 = A0 * ::pow( 10.0, fid.Intensity/20 )*sqrt(2.0);
    //cerr << "analyze: State " << State <<  "   A1 " << fid.A1 << "    A2 " << fid.A2 << endl;
  }
  else {
    fid.A1 = A0*sqrt( 2.0/(1+AmplRatio*AmplRatio) ) * ::pow( 10.0, fid.Intensity/20 );
    fid.A2 = fid.A1*AmplRatio;
    //cerr << "analyze: State " << State <<  "   A1 " << fid.A1 << "    A2 " << fid.A2 << endl;
  }

  // skip low response:
  if ( fid.Spikes.frequency( 0.0, Duration ) < MinFR ) {
    IntensityRange.setSkip();
  }

  // maximum firing rate:
  double max = 0.0;
  int mk = 0;
  if ( ShortStim ) {
    for ( unsigned int k=IntensityRange.next( 0 ); 
	  k<Results.size(); 
	  k = IntensityRange.next( ++k ) ) 
      {
	if ( Results[k].OnRate > max ) {
	  max = Results[k].OnRate;
	  mk = k;
	}
      } //for
  } //if
  else {
    for ( unsigned int k=IntensityRange.next( 0 ); 
	  k<Results.size(); 
	  k = IntensityRange.next( ++k ) ) 
      {
	if ( Results[k].SSRate > max ) {
	  max = Results[k].SSRate;
	  mk = k;
	} //if
      }   //for
    }     //else

  MaxRate = max; 
  MaxRatePos =  mk;
}

void IsoResponse::setFRBounds( double max )
{
  // set bounds for firing rate
  if( Switch_high==0 ) {
    Fup=max*FRhigh_per; 
    printlog( "Fup: " + Str( Fup ) + " max: " + Str( max ) + " FRhigh_per: " + Str( FRhigh_per ) );
  }
  else {
    if( max>FRhigh_fix ) Fup=FRhigh_fix; 
    else { 
      Fup=max; 
      Str s = "Upper bound larger than max. firing rate: set to <b>" + Str( max ) + " Hz</b>";
      warning( s, 3.0 ); 
    }
  }
  if( Switch_low ==0 ) Flow=max*FRlow_per; 
  else {
    if( max<FRlow_fix ) {
      Flow=max/2.0;  //???warning
      Str s = "Lower bound larger than max. firing rate: set to <b>" + Str( max/2.0 ) + " Hz</b>";
      warning( s, 3.0 );
    }
    else Flow=FRlow_fix;
  }

  //set frequencies for iso-response-sets
  if( State == 0 ) {   
    if( IrsNumber==1 ) FIso[0]=Fup;
    else if( IrsNumber==2 ) {
      FIso[0]=Flow;
      FIso[1]=Fup;
    }
    else {
      int a=IrsNumber-1;
      double dist=(Fup-Flow)/a;
      FIso[0]=Flow;
      for( int k=1; k<a; k++ ) 
	FIso[k]=Flow+k*dist;
      FIso[a]=Fup;
    }
  }
}

void IsoResponse::setIBounds( )
{
  // set new intensity range

  int upbound=IntensityRange.size();
  int lowbound=0;
  unsigned int f = 0;
  if ( ShortStim ) {
    for ( f=IntensityRange.next( 0 ); 
	  f<Results.size(); 
	  f = IntensityRange.next( ++f ) )
      {
	if ( Results[f].Spikes.size() > 0 && 
	     Results[f].OnRate <= Flow ) {
	  lowbound=f;
	  printlog( "set lowbound to " + Str( lowbound ) + " " + Str( IntensityRange[f] ) + " " + Str( Flow ) + " " + Str( MaxRate ) );
	}
	if ( Results[f].Spikes.size() > 0 && 
	     Results[f].OnRate >= Fup ) {
	  upbound=f;
	  printlog( "set upbound to " + Str( upbound ) + " " + Str( IntensityRange[f] ) + " " + Str( Fup ) + " " + Str( MaxRate ) );
	  break;
	} //if
      }   //for
  }       //if
  else {
    for ( f=IntensityRange.next( 0 ); 
	  f<Results.size(); 
	  f = IntensityRange.next( ++f ) )
      {
	if ( Results[f].Spikes.size() > 0 && 
	     Results[f].SSRate <= Flow ) {
	  lowbound=f;
	  printlog( "set lowbound to " + Str( lowbound ) + " " + Str( IntensityRange[f] ) + " " + Str( Flow ) + " " + Str( MaxRate ) );
	}
	if ( Results[f].Spikes.size() > 0 && 
	     Results[f].SSRate >= Fup ) {
	  upbound=f;
	  printlog( "set upbound to " + Str( upbound ) + " " + Str( IntensityRange[f] ) + " " + Str( Fup ) + " " + Str( MaxRate ) );
	  break;
	} //if
      }   //for
  }       //else
  
  IntensityRange.setSkipBelow( lowbound-1 );
  IntensityRange.setSkipAbove( upbound+1+int(IntensityUp/FinalResolution) );  
  IntensityRange.setBlockRepeat( Repeats );
  IntensityRange.setIncrement( 1 );
  IntensityRange.update();
}


void IsoResponse::findScaling(  )
{
  Scaling=0.0;
  double refpoint [3] = {0.6*MaxRate, 0.7*MaxRate, 0.8*MaxRate};
  double i1 [3];
  double i2 [3];
  unsigned int l = 0;
  for (l=0; l<3; l++)
    {
      double pressrate = -1.0;
      double preint = -1.0;
      int pk = -1;
      for ( unsigned int k=0; 
	    k<IsoResults[0].FinalResults.size(); 
	    k++ )
	{
	  double currssrate;
	  if ( ShortStim == 1 ) currssrate = IsoResults[0].FinalResults[k].OnRate;
	  else                  currssrate = IsoResults[0].FinalResults[k].SSRate;
	  double currint = IsoResults[0].FinalResults[k].Intensity;
	  if ( currssrate >= 0.0 ) {
	    if ( pressrate >= 0.0 &&
		 pressrate <= refpoint[l] &&
		 currssrate > refpoint[l] ) {
	      double slope = (currint - preint)/(currssrate - pressrate);
	      i1[l]=slope*(refpoint[l] - currssrate) + currint;
	    }
	    pressrate = currssrate;
	    preint = currint;
	    pk = k;
	  }
	}
      pressrate = -1.0;
      preint = -1.0;
      pk = -1;
      for ( unsigned int k=0; 
	    k<IsoResults.back().FinalResults.size(); 
	    k++ )
	{
	  double currssrate;
	  if ( ShortStim == 1 ) currssrate = IsoResults.back().FinalResults[k].OnRate;
	  else                  currssrate = IsoResults.back().FinalResults[k].SSRate;
	  double currint = IsoResults.back().FinalResults[k].Intensity;
	  if ( currssrate >= 0.0 ) {
	    if ( pressrate >= 0.0 &&
		 pressrate <= refpoint[l] &&
		 currssrate > refpoint[l] ) {
	      double slope = (currint - preint)/(currssrate - pressrate);
	      i2[l]=slope*(refpoint[l] - currssrate) + currint;
	     }
	    pressrate = currssrate;
	    preint = currint;
	    pk = k;
	  }
	}
      Scaling += ::pow( 10.0, (i2[l]-i1[l])/20.0 )/3.0;
      printlog( "Scaling: " + Str( Scaling ) );
    }
}


addRePro( IsoResponse, auditoryprojects );

}; /* namespace auditoryprojects */

#include "moc_isoresponse.cc"
