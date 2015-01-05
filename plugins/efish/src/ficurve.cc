/*
  efish/ficurve.cc
  Measures f-I curves of electrosensory neurons.

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

#include <relacs/efish/ficurve.h>
using namespace relacs;

namespace efish {


FICurve::FICurve( void )
  : RePro( "FICurve", "efish", "Jan Benda", "1.6", "Nov 19, 2010" )
{
  // parameter:
  Duration = 0.4;
  PreDuration = 0.2;
  Pause = 1.0;
  Delay = 0.2;
  RateDeltaT = 0.001;
  OnsetTime = 0.05;
  SSTime = 0.05;
  IntIncrement = 2;
  RangeIntIncrement = 1;
  MinRate = 0.0;
  MinRateFrac = 0.0;
  MinRateSlope = 100.0;

  // add some parameter as options:
  newSection( "Test-Intensities" );
  addNumber( "duration", "Duration of test stimulus", Duration, 0.01, 1000.0, 0.05, "seconds", "ms" );
  addNumber( "maxintfac", "Maximum intensity of test stimulus", 1.6, 0.0, 2.0, 0.1, "1", "%" );
  addNumber( "minintfac", "Minimum intensity of test stimulus", 0.2, 0.0, 2.0, 0.1, "1", "%" );
  addInteger( "nints", "Number of intensities for test stimulus", 16, 2, 500, 1 );
  addInteger( "repeats", "Total number of repetitions of an f-I curve measurement", 10, 0, 100000, 2 ).setStyle( OptWidget::SpecialInfinite );
  addInteger( "blockrepeats", "Number of repetitions of a sequence of intensities", 1, 1, 100000, 2 );
  addInteger( "singlerepeats", "Number of immediate repetitions of an intensity", 1, 1, 100000, 2 );
  addSelection( "intshuffle", "Order of intensities", RangeLoop::sequenceStrings() );
  addInteger( "intincrement", "Initial increment", IntIncrement, -1000, 1000, 1 );
  newSection( "Pre-Intensities" );
  addNumber( "preduration", "Duration of adapting stimulus", PreDuration, 0.0, 1000.0, 0.05, "seconds", "ms" );
  addNumber( "maxpreintfac", "Maximum intensity of adapting stimulus", 1.4, 0.0, 2.0, 0.1, "1", "%" ).setActivation( "preduration", ">0" );
  addNumber( "minpreintfac", "Minimum intensity of adapting stimulus", 0.6, 0.0, 2.0, 0.1, "1", "%" ).setActivation( "preduration", ">0" );
  addInteger( "npreints", "Number of intensities for adapting stimuli", 5, 2, 500, 1 ).setActivation( "preduration", ">0" );
  addSelection( "preintshuffle", "Order of  adapting intensities", RangeLoop::sequenceStrings() ).setActivation( "preduration", ">0" );
  newSection( "Control" );
  addBoolean( "am", "Amplitude modulation", true );
  addNumber( "pause", "Pause between stimuli", Pause, 0.0, 1000.0, 0.05, "seconds", "ms" );
  addNumber( "delay", "Part of pause before stimulus", Delay, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "onsettime", "Onset rate occurs within", OnsetTime, 0.0, 1000.0, 0.002, "seconds", "ms" );
  addInteger( "rangeintincrement", "Optimize intensity range at increments below", RangeIntIncrement, 0, 1000, 1 );
  addNumber( "minrate", "Minimum required onset rate", MinRate, 0.0, 2000.0, 10.0, "Hz" );
  addNumber( "minratefrac", "Minimum required rate differences", MinRateFrac, 0.0, 1.0, 0.05, "1", "%" );
  //  addNumber( "minrateslope", "Minimum slope of f-I curve", MinRateSlope, 0.0, 1000.0, 10.0, "Hz/mV/cm" );
  addBoolean( "adjust", "Adjust input gain?", true );
  
  // variables:
  IntensityRange.clear();
  PreIntensityRange.clear();
  for ( int k=0; k<MaxTraces; k++ )
    Response[k].clear();
  Index = 0;

  // header for files:
  Header.addInteger( "index" );
  Header.addInteger( "trace" );
  Header.addNumber( "preintensity", "", "%.3f" );
  Header.addNumber( "EOD rate", "Hz", "%.1f" );
  Header.addNumber( "trans. amplitude", "", "%.2f" );
  Header.addText( "repro time" );
  Header.addText( "session time" );
  Header.newSection( "Settings" );

  // plot:
  setWidget( &P );
}


int FICurve::main( void )
{
  // get options:
  Duration = number( "duration" );
  double maxintensityfac = number( "maxintfac" );
  double minintensityfac = number( "minintfac" );
  int nintensities = integer( "nints" );
  int repeats = integer( "repeats" );
  int blockrepeats = integer( "blockrepeats" );
  int singlerepeats = integer( "singlerepeats" );
  RangeLoop::Sequence intshuffle = RangeLoop::Sequence( index( "intshuffle" ) );
  IntIncrement = integer( "intincrement" );
  PreDuration = number( "preduration" );
  double maxpreintensityfac = number( "maxpreintfac" );
  double minpreintensityfac = number( "minpreintfac" );
  int npreintensities = integer( "npreints" );
  RangeLoop::Sequence preintshuffle = RangeLoop::Sequence( index( "preintshuffle" ) );
  bool am = boolean( "am" );
  Pause = number( "pause" );
  Delay = number( "delay" );
  OnsetTime = number( "onsettime" );
  RangeIntIncrement = integer( "rangeintincrement" );
  MinRate = number( "minrate" );
  MinRateFrac = number( "minratefrac" );
  //  MinRateSlope = number( "minrateslope" );
  bool adjustg = boolean( "adjust" );

  // check EODs:
  if ( LocalEODTrace[0] < 0 || LocalEODEvents[0] < 0 ) {
    warning( "Local EOD recording with EOD events required!" );
    return Failed;
  }

  // EOD trace unit:
  EOD2Unit = trace( LocalEODTrace[0] ).unit();

  // EOD rate:
  FishRate = events( LocalEODEvents[0] ).frequency( currentTime() - 0.5, currentTime() );

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ),
				currentTime() - 0.5, currentTime() );

  // trigger:
  //  setupTrigger( data, events );

  // intensities:
  double maxint = maxintensityfac * FishAmplitude;
  if ( maxint > trace( LocalEODTrace[0] ).maxValue() )
    maxint = trace( LocalEODTrace[0] ).maxValue();
  IntensityRange.set( minintensityfac * FishAmplitude, maxint, nintensities );
  IntensityRange.setIncrement( IntIncrement );
  IntensityRange.setSingleRepeat( singlerepeats );
  IntensityRange.setBlockRepeat( blockrepeats );
  IntensityRange.setRepeat( repeats );
  IntensityRange.setSequence( intshuffle );

  // pre-intensities:
  if ( PreDuration > 0.0 ) {
    double maxpreint = maxpreintensityfac * FishAmplitude;
    if ( maxpreint > trace( LocalEODTrace[0] ).maxValue() )
      maxpreint = trace( LocalEODTrace[0] ).maxValue();
    PreIntensityRange.set( minpreintensityfac * FishAmplitude, maxpreint, npreintensities );
    PreIntensityRange.setSequence( preintshuffle );
  }
  else
    PreIntensityRange.set( FishAmplitude );

  // data:
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Response[k].resize( PreIntensityRange.size() );
      for ( unsigned int j = 0; j<Response[k].size(); j++ ) {
	Response[k][j].resize( IntensityRange.size() );
	for ( unsigned int i = 0; i<Response[k][j].size(); i++ ) {
	  Response[k][j][i].Spikes.reserve( IntensityRange.maxCount() );
	  Response[k][j][i].Rate = SampleDataD( -Delay-PreDuration,
						Duration+Pause-Delay, 
						RateDeltaT );
	}
      }
      MaxRate[k] = 100.0;
    }
  }

  // plot:
  P.lock();
  P.resize( 2*SpikeTraces, 2, false, Plot::Copy );

  // plot:
  int n=0;
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      P[2*n].clear();
      P[2*n].setLMarg( 7 );
      P[2*n].setRMarg( 2 );
      P[2*n].setBMarg( 2.5 );
      P[2*n].setTMarg( 1 );
      P[2*n].setXLabel( "[" + EOD2Unit + "]" );
      P[2*n].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
      P[2*n].setXRange( IntensityRange.minValue(), IntensityRange.maxValue() );
      P[2*n].setXTics();
      P[2*n].setYLabel( "Frequency [Hz]" );
      P[2*n].setYLabelPos( 1.5, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
      P[2*n].setAutoScaleY();
      P[2*n].setYTics( );
      
      P[2*n+1].clear();
      P[2*n+1].setLMarg( 7 );
      P[2*n+1].setRMarg( 2 );
      P[2*n+1].setBMarg( 3 );
      P[2*n+1].setTMarg( 1 );
      P[2*n+1].setXLabel( "[ms]" );
      P[2*n+1].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
      P[2*n+1].setXRange( -1000.0 * ( Delay + PreDuration ), 1000.0*Duration );
      P[2*n+1].setYLabel( "Frequency [Hz]" );
      P[2*n+1].setYLabelPos( 1.5, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
      P[2*n+1].setYRange( 0.0, 100.0 );
      P[2*n+1].setYTics( );

      if ( n > 0 ) {
	P.setCommonXRange( 2*(n-1), 2*n );
	P.setCommonYRange( 2*(n-1), 2*n );
	P.setCommonXRange( 2*(n-1)+1, 2*n+1 );
	P.setCommonYRange( 2*(n-1)+1, 2*n+1 );
      }

      n++;
    }
  }
  P.unlock();

  // plot trace:
  tracePlotSignal( Delay + PreDuration + Duration, Delay );

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( currentTime()-0.1,
						  currentTime() );
  if ( val2 > 0.0 )
    adjustGain( trace( LocalEODTrace[0] ), maxintensityfac * val2 );

  // create signal:
  OutData signal;
  signal.setTrace( am ? GlobalAMEField : GlobalEField );
  double si = 0.001;
  if ( signal.fixedSampleRate() )
    si = signal.minSampleInterval();
  signal.resize( 0.0, PreDuration + Duration, si, 0.0 );
  signal.setDelay( Delay );
  signal.setStartSource( 1 );
  signal.setIdent( "rectangle" );
  signal.setCarrierFreq( FishRate );

  // stimulus intensity:
  double maxintensity = FishAmplitude - IntensityRange.minValue();
  if ( IntensityRange.maxValue() - FishAmplitude > maxintensity )
    maxintensity = IntensityRange.maxValue() - FishAmplitude;
  maxintensity += IntensityRange.step();
  signal.setIntensity( maxintensity );

  for ( PreIntensityRange.reset();
	! PreIntensityRange && softStop() == 0;
	++PreIntensityRange ) {

    for ( IntensityRange.reset(  IntensityRange.pos( *PreIntensityRange ) );
	  ! IntensityRange && softStop() <= 1;
	  ++IntensityRange ) {

      // select intensities:
      if ( IntensityRange.finishedBlock() ) {
	if ( softStop() > 1 ) {
	  save();
	  stop();
	  return Completed;
	}
	selectRange();
	// selectSlopes();  // does not work yet, all code in this function is disabled.
      }

      // adaptation stimulus:
      PreIntensity = *PreIntensityRange;
      double stimuluspreintensity = PreIntensity - FishAmplitude;
      PreContrast = stimuluspreintensity / FishAmplitude;
      double y = stimuluspreintensity / maxintensity;
      if ( y > 1.0 ) {
	printlog( "! warning -> preintensity overflow: y=" + Str( y ) );
	y = 1.0;
      }
      else if ( y < -1.0 ) {
	printlog( "! warning  -> preintensity underflow: y=" + Str( y ) );
	y = -1.0;
      }

      // test stimulus:
      Intensity = *IntensityRange;
      double stimulusintensity = Intensity - FishAmplitude;
      Contrast = stimulusintensity / FishAmplitude;
      double x = stimulusintensity / maxintensity;
      if ( x > 1.0 ) {
	printlog( "! warning -> intensity overflow: x=" + Str( x ) );
	x = 1.0;
      }
      else if ( x < -1.0 ) {
	printlog( "! warning  -> intensity underflow: x=" + Str( x ) );
	x = -1.0;
      }

      // signal:
      signal.clear();
      if ( PreDuration > 0.0 ) {
	OutData presignal;
	presignal.constWave( PreDuration, signal.stepsize(), y, "PreStimulus" );
	signal.append( presignal );
      }
      OutData testsignal;
      testsignal.pulseWave( Duration, signal.stepsize(), x, 0.0, "TestStimulus" );
      signal.append( testsignal );

      // meassage: 
      Str s = "Contrast: <b>" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%</b>";
      s += "  Intensity: <b>" + Str( Intensity, 0, 3, 'f' ) + "mV/cm</b>";
      if ( PreDuration > 0.0 && fabs( PreIntensity - FishAmplitude ) > 1.0e-6 ) {
	s += "  PreContrast: <b>" + Str( 100.0 * PreContrast, 0, 0, 'f' ) + "%</b>";
	s += "  PreIntensity: <b>" + Str( PreIntensity, 0, 3, 'f' ) + "mV/cm</b>";
      }
      s += "  Loop: <b>" + Str( IntensityRange.count()+1 ) + "</b>";
      message( s );

      // output signal:
      write( signal );
      if ( !signal.success() ) {
	string s = "Output of stimulus failed!<br>Error code is <b>";
	s += Str( signal.error() ) + "</b>: <b>" + signal.errorText() + "</b>";
	warning( s, 4.0 );
	save();
	stop();
	return Failed;
      }

      sleep( Pause );
      if ( interrupt() ) {
	save();
	stop();
	return Aborted;
      }

      // signal failed?
      if ( !signal.success() ) {
	if ( signal.busy() ) {
	  warning( "Output still busy!<br> Probably missing trigger.<br> Output of this signal software-triggered.", 2.0 );
	  signal.setStartSource( 0 );
	  signal.setPriority();
	  write( signal );
	  sleep( Pause );
	  // trigger:
	  //      setupTrigger( data, events );
	}
	else if ( signal.error() == signal.OverflowUnderrun ) {
	  warning( "Analog Output Overrun Error!<br> Try again.", 2.0 );
	  write( signal );
	  sleep( Pause );
	}
	else {
	  string s = "Output of stimulus failed!<br>Error code is <b>";
	  s += signal.errorText() + "</b>";
	  warning( s, 4.0 );
	  save();
	  stop();
	  return Failed;
	}
	if ( interrupt() ) {
	  save();
	  stop();
	  return Aborted;
	}
      }

      // adjust input gains:
      if ( adjustg ) {
	for ( int k=0; k<MaxTraces; k++ )
	  if ( SpikeTrace[k] >= 0 )
	    adjust( trace( SpikeTrace[k] ), signalTime()+Duration,
		    signalTime()+Duration+Pause, 0.8 );
      }

      // analyze:
      analyze();
      plot();

    }

    save();

  }

  stop();

  return Completed;
}


void FICurve::stop( void )
{
  IntensityRange.clear();
  PreIntensityRange.clear();
  for ( int k=0; k<MaxTraces; k++ )
    if ( SpikeEvents[k] >= 0 )
      Response[k].clear();
  writeZero( GlobalAMEField );
}


void FICurve::sessionStarted( void )
{
  Index = 0;
  RePro::sessionStarted();
}


void FICurve::saveRate( int trace )
{
  // create file:
  ofstream df( addPath( "firate" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%6.1f" );
  key.addNumber( "rate", "Hz", "%5.1f" );
  key.saveKey( df, true, false );
      
  // write firing rate into file:
  int pinx = PreIntensityRange.pos();
  int n = 0;
  for ( int j=0; j<IntensityRange.size(); j++ )
    if ( Response[trace][pinx][j].Trial > 0 ) {
      df << '\n';
      df << "# index = " << n << '\n';
      df << "# trials = " << Response[trace][pinx][j].Trial << '\n';
      df << "# intensity = " << Str( IntensityRange.value( j ), "%.3f" ) << EOD2Unit << '\n';
      df << "# true intensity = " << Str( Response[trace][pinx][j].Intensity, "%.3f" ) << EOD2Unit << '\n';
      df << "# preintensity = " << Str( PreIntensityRange.value(), "%.3f" ) << EOD2Unit << '\n';
      df << "# true preintensity = " << Str( Response[trace][pinx][j].PreIntensity, "%.3f" ) << EOD2Unit << '\n';
      for ( int i=0; i<Response[trace][pinx][j].Rate.size(); i++ ) {
	key.save( df, 1000.0 * Response[trace][pinx][j].Rate.pos( i ), 0 );
	key.save( df, Response[trace][pinx][j].Rate[i] );
	df << '\n';
      }
      n++;
    }
  df << "\n\n";
}


void FICurve::saveSpikes( int trace )
{
  // create file:
  ofstream df( addPath( "fispikes" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;
  
  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%6.2f" );
  key.saveKey( df, true, false );
  
  // write spikes into file:
  int pinx = PreIntensityRange.pos();
  int n = 0;
  for ( int j=0; j<IntensityRange.size(); j++ ) {
    if ( Response[trace][pinx][j].Trial > 0 ) {
      df << '\n';
      df << "# index = " << n << '\n';
      df << "# intensity = " << Str( IntensityRange.value( j ), "%.3f" ) << EOD2Unit << '\n';
      df << "# true intensity = " << Str( Response[trace][pinx][j].Intensity, "%.3f" ) << EOD2Unit << '\n';
      df << "# preintensity = " << Str( PreIntensityRange.value(), "%.3f" ) << EOD2Unit << '\n';
      df << "# true preintensity = " << Str( Response[trace][pinx][j].PreIntensity, "%.3f" ) << EOD2Unit << '\n';
      Response[trace][pinx][j].Spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
      n++;
    }
  }
  df << "\n\n";
}


void FICurve::saveFICurves( int trace )
{
  // create file:
  ofstream df( addPath( "ficurves" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;
  
  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "inx", "1", "%5.0f" );
  key.addNumber( "n", "1", "%5.0f" );
  key.addNumber( "Ir", EOD2Unit, "%5.3f" );
  key.addNumber( "Im", EOD2Unit, "%5.3f" );
  key.addNumber( "f_0", "Hz", "%5.1f" );
  key.addNumber( "f_s", "Hz", "%5.1f" );
  key.addNumber( "f_r", "HZ", "%5.1f" );
  key.addNumber( "Ip", EOD2Unit, "%5.3f" );
  key.addNumber( "Ipm", EOD2Unit, "%5.3f" );
  key.addNumber( "f_p", "Hz", "%5.1f" );
  key.saveKey( df, true, false );
  
  // write firing curves into file:
  int pinx = PreIntensityRange.pos();
  int n = 0;
  for ( int j=0; j<IntensityRange.size(); j++ )
    if ( Response[trace][pinx][j].Trial > 0 ) {
      key.save( df, n, 0 );
      key.save( df, Response[trace][pinx][j].Trial );
      key.save( df, IntensityRange.value( j ) );
      key.save( df, Response[trace][pinx][j].Intensity );
      key.save( df, Response[trace][pinx][j].OnsetRate );
      key.save( df, Response[trace][pinx][j].SSRate );
      key.save( df, Response[trace][pinx][j].RestRate );
      key.save( df, PreIntensityRange.value( pinx ) );
      key.save( df, Response[trace][pinx][j].PreIntensity );
      key.save( df, Response[trace][pinx][j].PreRate );
      df << '\n';
      n++;
    }
  df << "\n\n";
}


void FICurve::save( void )
{
  // check for pending data:
  if ( IntensityRange.loop() <= 0 )
    return;

  Header.setInteger( "index", Index );
  Header.setUnit( "preintensity", EOD2Unit );
  Header.setNumber( "preintensity", PreIntensityRange.value() );
  Header.setNumber( "EOD rate", FishRate );
  Header.setUnit( "trans. amplitude", EOD2Unit );
  Header.setNumber( "trans. amplitude", FishAmplitude );
  Header.setText( "repro time", reproTimeStr() );
  Header.setText( "session time", sessionTimeStr() );
  Header.erase( "Settings" );
  Header.newSection( settings() );

  for ( int trace=0; trace<MaxTraces; trace++ ) {
    if ( SpikeEvents[trace] >= 0 ) {
      Header.setInteger( "trace", trace );
      saveRate( trace );
      saveSpikes( trace );
      saveFICurves( trace );
    }
  }

  Index++;
}


void FICurve::plot( void )
{
  P.lock();
  int n=0;
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {

      // f-I-curves:
      P[2*n].clear();
      if  (! P[2*n].zoomedYRange() )
	P[2*n].setYRange( 0.0, MaxRate[k] );
      P[2*n].plotVLine( FishAmplitude, Plot::White, 2, Plot::LongDash );
      if ( PreDuration > 0.0 )
	P[2*n].plotVLine( PreIntensity, Plot::White, 2 );
      P[2*n].plotVLine( Intensity, Plot::Yellow, 2 );
      int p = PreIntensityRange.pos();
      MapD m;
      m.reserve( IntensityRange.size() );
      // rest:
      for ( int j=0; j<IntensityRange.size(); j++ ) {
	if ( Response[k][p][j].Trial > 0 )
	  m.push( IntensityRange.value( j ), Response[k][p][j].RestRate );
      }
      P[2*n].plot( m, 1.0, Plot::Blue, 2, Plot::Solid );
      // pre steady-state:
      m.clear();
      m.reserve( IntensityRange.size() );
      for ( int j=0; j<IntensityRange.size(); j++ ) {
	if ( Response[k][p][j].Trial > 0 )
	  m.push( IntensityRange.value( j ), Response[k][p][j].PreRate );
      }
      P[2*n].plot( m, 1.0, Plot::DarkOrange, 2, Plot::Solid );
      // steady-state:
      m.clear();
      m.reserve( IntensityRange.size() );
      for ( int j=0; j<IntensityRange.size(); j++ ) {
	if ( Response[k][p][j].Trial > 0 )
	  m.push( IntensityRange.value( j ), Response[k][p][j].SSRate );
      }
      P[2*n].plot( m, 1.0, Plot::Red, 2, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
      // onset:
      for ( int i=0; i<PreIntensityRange.size(); i++ ) {
	if ( PreIntensityRange.count( i ) > 0 ||
	     PreIntensityRange.active( i ) ) {
	  m.clear();
	  m.reserve( IntensityRange.size() );
	  for ( int j=0; j<IntensityRange.size(); j++ ) {
	    if ( Response[k][i][j].Trial > 0 )
	      m.push( IntensityRange.value( j ), Response[k][i][j].OnsetRate );
	  }
	  int color = i == p ? Plot::Green : Plot::DarkGreen;
	  P[2*n].plot( m, 1.0, color, 2, Plot::Solid, Plot::Circle, 6, color, color );
	}
      }
      
      // rate and spikes:
      P[2*n+1].clear();
      if  (! P[2*n+1].zoomedYRange() )
	P[2*n+1].setYRange( 0.0, MaxRate[k] );
      P[2*n+1].plotVLine( 0.0, Plot::White, 2 );
      if ( PreDuration > 0.0 )
	P[2*n+1].plotVLine( -1000.0*PreDuration, Plot::White, 2 );
      for ( int i=0; i<Response[k][p][ IntensityRange.pos() ].Trial && i<10; i++ )
	P[2*n+1].plot( Response[k][p][ IntensityRange.pos() ].Spikes[i], 1000.0,
		       1.0 - (i+1)*0.1, Plot::Graph, 2, Plot::StrokeUp,
		       0.09, Plot::Graph, Plot::Red, Plot::Red );
      P[2*n+1].plot( Response[k][p][ IntensityRange.pos() ].Rate, 1000.0, Plot::Yellow, 2, Plot::Solid );

      n++;
    }
  }

  P.draw();

  P.unlock();  
}



void FICurve::analyzeSpikes( const EventData &se, int trace,
			     double truepreintensity, double trueintensity )
{
  ResponseData &response = Response[trace][PreIntensityRange.pos()][IntensityRange.pos()];

  // signal amplitude:
  response.PreIntensity = truepreintensity;
  response.Intensity = trueintensity;

  // spikes:
  response.Spikes.push( se, signalTime()-Delay-PreDuration, signalTime()+Duration+Pause, signalTime()+PreDuration );

  // firing frequency:
  se.addFrequency( response.Rate, response.Trial, signalTime()+PreDuration );

  int offs0 = response.Rate.index( -PreDuration );
  int offs1 = response.Rate.index( 0.0 );

  // rest rate:
  double rr = 0.0;
  for ( int j=0; j < offs0; j++ )
    rr += ( response.Rate[ j ] - rr ) / ( j+1 );
  response.RestRate = rr;

  // pre rate:
  double pr = rr;
  if ( PreDuration > 0.0 ) {
    int offs = offs1 - int( SSTime / RateDeltaT );
    if ( offs < offs0 )
      offs = offs0;
    pr = 0.0;
    for ( int j=0; j<SSTime/RateDeltaT; j++ )
      pr += ( response.Rate[ offs + j ] - pr ) / ( j+1 );
  }
  response.PreRate = pr;
  
  // peak firing rate:
  double maxr = pr;
  for ( int j=0; j<OnsetTime/RateDeltaT; j++ ) {
    double r = response.Rate[ offs1 + j ];
    if ( fabs( r - pr ) > fabs( maxr - pr ) )
      maxr = r;
  }
  response.OnsetRate = maxr;

  // steady -state firing rate:
  int offs = offs1 + int( (Duration-SSTime)/RateDeltaT );
  double sr = 0.0;
  for ( int j=0; j<SSTime/RateDeltaT; j++ )
    sr += ( response.Rate[ offs + j ] - sr ) / ( j+1 );
  response.SSRate = sr;
  
  maxr = max( response.Rate );
  if ( maxr > MaxRate[trace] )
    MaxRate[trace] = ::ceil(maxr/20.0)*20.0;
}


void FICurve::analyze( void )
{
  double bd = currentTime() - signalTime() - PreDuration - Duration;
  if ( bd > 0.5 )
    bd = 0.5;

  // EOD rate:
  FishRate = events( LocalEODEvents[0] ).frequency( currentTime() - bd, currentTime() );

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace( LocalEODTrace[0] ),
				currentTime() - bd, currentTime()  );

  // amplitude:
  double truepreintensity = eodAmplitude( trace( LocalEODTrace[0] ),
					  signalTime(), signalTime()+PreDuration );
  double trueintensity = eodAmplitude( trace( LocalEODTrace[0] ),
				       signalTime() + PreDuration, signalTime()+Duration );

  // spikes:
  for ( int k=0; k<MaxTraces; k++ )
    if ( SpikeEvents[k] >= 0 )
      analyzeSpikes( events( SpikeEvents[k] ), k, truepreintensity, trueintensity );
}


void FICurve::selectSlopes( void )
{
  /*
  if ( IntensityRange.currentIncrement() != IntIncrement )
    return;

  printlog( "FICurve::selectSlopes()" );

  int pinx = PreIntensityRange.pos();
  bool first = true;

  // check slopes and skip:
  for ( int k=0; k<IntensityRange.size(); k++ ) {

    // go to next measured rate:
    for ( ; k<IntensityRange.size() && IntensityRange.count( k ) == 0; k++ );

    // go to next measured rate:
    int j;
    for ( j=k+1; j<IntensityRange.size() && IntensityRange.count( j ) == 0; j++ );

    // no more data:
    if ( j >= IntensityRange.size() ) {
      if ( IntensityRange.skip( k ) || 
	   ( k>0 && IntensityRange.skip( k-1 ) ) ) {
	for ( j=k+1; j<IntensityRange.size(); j++ )
	  IntensityRange.setSkip( j );
      }
      break;
    }

    // calculate slope:
    double slope = ( Response[0][pinx][j].OnsetRate - Response[0][pinx][k].OnsetRate ) /
      ( IntensityRange.value( j ) - IntensityRange.value( k ) );

    printlog( "slope " + Str( k ) + " " + Str( j ) + " @" + Str( IntensityRange.value( k ) ) + " " + Str( IntensityRange.value( j ) ) + ": " +Str( slope ) );

    if ( fabs( slope ) < MinRateSlope ) {

      printlog( "  skipped" );

      // this measurement is not necessary, skip it:
      for ( int i=k+1; i<j; i++ )
	IntensityRange.setSkip( i );

      if ( first ) {
	for ( int i=0; i<k; i++ )
	  IntensityRange.setSkip( i );
      }

    }

    k = j-1;
    first = false;
  }
  */  
}


void FICurve::selectRange( void )
{
  if ( IntensityRange.currentIncrement() > RangeIntIncrement )
    return;

  int pinx = PreIntensityRange.pos();

  // maximum and minimum onset response:
  int k = IntensityRange.next( 0 );
  double min = Response[0][pinx][k].OnsetRate;
  int  mininx = k;
  double max = min;
  int  maxinx = k;
  for ( k=IntensityRange.next( k+1 );
	k<IntensityRange.size();
	k=IntensityRange.next( k+1 ) ) {
    if ( Response[0][pinx][k].OnsetRate > max ) {
      max = Response[0][pinx][k].OnsetRate;
      maxinx = k;
    }
    else if ( Response[0][pinx][k].OnsetRate < min ) {
      min = Response[0][pinx][k].OnsetRate;
      mininx = k;
    }
  }

  // no response:
  if ( mininx < maxinx ) {
    // monotonically increasing f-I curve:
    int skipinx = -1;
    for ( k=IntensityRange.next( 0 );
	  k<IntensityRange.size();
	  k=IntensityRange.next( k+1 ) ) {
      if ( Response[0][pinx][k].OnsetRate < MinRate )
	skipinx = k;
      else
	break;
    }
    if ( skipinx >= 0 ) {
      printlog( "  skip zeros 0 - " + Str( skipinx )
		+ " rate: " + Str( Response[0][pinx][skipinx].OnsetRate )
		+ " minrate: " + Str( MinRate ) );
      IntensityRange.setSkipBelow( skipinx );
    }
  }
  else {
    // monotonically decreasing f-I curve:
    int skipinx = -1;
    for ( k = IntensityRange.previous( IntensityRange.size()-1 );
	  k >= 0;
	  k = IntensityRange.previous( k-1 ) ) {
      if ( Response[0][pinx][k].OnsetRate < MinRate )
	skipinx = k;
      else
	break;
    }
    if ( skipinx >= 0 ) {
      printlog( "  skip zeros " + Str( skipinx ) + " - " + Str( IntensityRange.size()-1 )
		+ " rate: " + Str( Response[0][pinx][skipinx].OnsetRate )
		+ " minrate: " + Str( MinRate ) );
      IntensityRange.setSkipAbove( skipinx );
    }
  }

  // no change in response at right end for monotonically increasing f-I curve:
  if ( mininx < maxinx ) {
    int skipinx = -1;
    for ( k = IntensityRange.previous( IntensityRange.size()-1 );
	  k >= 0;
	  k = IntensityRange.previous( k-1 ) ) {
      if ( fabs( Response[0][pinx][k].OnsetRate - max ) < MinRateFrac*(max-min) )
	skipinx = k;
      else
	break;
    }
    if ( skipinx >= 0 ) {
      printlog( "  skip right " + Str( skipinx ) + " - " + Str( IntensityRange.size()-1 )
		+ " rate: " + Str( Response[0][pinx][skipinx].OnsetRate )
		+ " delta rate: " + Str( max-min ) );
      IntensityRange.setSkipAbove( skipinx );
    }
  }
  else {
    // no change in response at left end for monotonically decreasing f-I curve:
    int skipinx = -1;
    for ( k = IntensityRange.next( 0 );
	  k < IntensityRange.size();
	  k = IntensityRange.next( k+1 ) ) {
      if ( fabs( Response[0][pinx][k].OnsetRate - max ) < MinRateFrac*(max-min) )
	skipinx = k;
      else
	break;
    }
    if ( skipinx >= 0 ) {
      printlog( "  skip left 0 - " + Str( skipinx )
		+ " rate: " + Str( Response[0][pinx][skipinx].OnsetRate )
		+ " delta rate: " + Str( max-min ) );
      IntensityRange.setSkipBelow( skipinx );
    }
  }

}


addRePro( FICurve, efish );

}; /* namespace efish */

#include "moc_ficurve.cc"
