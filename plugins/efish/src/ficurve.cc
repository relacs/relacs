/*
  efish/ficurve.cc
  Measures f-I curves of electrosensory neurons.

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

#include <relacs/efish/ficurve.h>
using namespace relacs;

namespace efish {


FICurve::FICurve( void )
  : RePro( "FICurve", "FICurve", "efish",
	   "Jan Benda", "1.4", "Mar 17, 2010" ),
    P( this )    
{
  // parameter:
  Duration = 0.4;
  PreDuration = 0.2;
  Pause = 1.0;
  Delay = 0.2;
  RateDeltaT = 0.001;
  OnsetTime = 0.05;
  SSTime = 0.05;
  NSkip = 2;
  MediumResolution = 2;
  MinRateSlope = 100.0;

  // add some parameter as options:
  addLabel( "Timing" );
  addNumber( "duration", "Duration of test stimulus", Duration, 0.01, 1000.0, 0.05, "seconds", "ms" );
  addNumber( "preduration", "Duration of adapting stimulus", PreDuration, 0.0, 1000.0, 0.05, "seconds", "ms" );
  addNumber( "pause", "Pause between stimuli", Pause, 0.0, 1000.0, 0.05, "seconds", "ms" );
  addNumber( "delay", "Part of pause before stimulus", Delay, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addLabel( "Intensities" );
  addNumber( "maxintfac", "Maximum intensity", 1.6, 0.0, 2.0, 0.1, "1", "%" );
  addNumber( "minintfac", "Minimum intensity", 0.2, 0.0, 2.0, 0.1, "1", "%" );
  addInteger( "nints", "Number of intensities", 16, 2, 500, 1 );
  addNumber( "maxpreintfac", "Maximum preintensity", 1.4, 0.0, 2.0, 0.1, "1", "%" );
  addNumber( "minpreintfac", "Minimum preintensity", 0.6, 0.0, 2.0, 0.1, "1", "%" );
  addInteger( "npreints", "Number of preintensities", 5, 2, 500, 1 );
  addBoolean( "am", "Amplitude modulation", true );
  addLabel( "Control" );
  addInteger( "repeats", "Repeats", 10, 0, 100000, 2 );
  addInteger( "blockrepeats", "Block repeats", 1, 1, 100000, 2 );
  addInteger( "singlerepeats", "Single repeats", 1, 1, 100000, 2 );
  addInteger( "medres", "Medium resolution", MediumResolution, 1, 100, 1 );
  addInteger( "nskip", "N skip", NSkip, 1, 10, 1 );
  addNumber( "minrateslope", "Minimum slope of f-I curve", MinRateSlope, 0.0, 1000.0, 10.0, "Hz/mV/cm" );
  addTypeStyle( OptWidget::Bold, Parameter::Label );
  
  // variables:
  IntensityRange.clear();
  PreIntensityRange.clear();
  for ( int k=0; k<MaxSpikeTraces; k++ )
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
  Header.addLabel( "settings:" );
}


int FICurve::main( void )
{
  // get options:
  Duration = number( "duration" );
  PreDuration = number( "preduration" );
  Pause = number( "pause" );
  Delay = number( "delay" );
  int repeats = integer( "repeats" );
  int blockrepeats = integer( "blockrepeats" );
  int singlerepeats = integer( "singlerepeats" );
  double maxintensityfac = number( "maxintfac" );
  double minintensityfac = number( "minintfac" );
  int nintensities = integer( "nints" );
  double maxpreintensityfac = number( "maxpreintfac" );
  double minpreintensityfac = number( "minpreintfac" );
  int npreintensities = integer( "npreints" );
  bool am = boolean( "am" );
  MediumResolution = integer( "medres" );
  NSkip = integer( "nskip" );
  MinRateSlope = number( "minrateslope" );

  // check EODs:
  if ( LocalEODTrace[0] < 0 || LocalEODEvents[0] < 0 ) {
    warning( "Local EOD recording with EOD events required!" );
    return Failed;
  }

  // EOD rate:
  FishRate = events( LocalEODEvents[0] ).frequency( events( LocalEODEvents[0] ).back() - 0.5,
						    events( LocalEODEvents[0] ).back() );

  // EOD amplitude:
  FishAmplitude = events( LocalEODEvents[0] ).meanSize( events( LocalEODEvents[0] ).back() - 0.5,
							events( LocalEODEvents[0] ).back() );

  // trigger:
  //  setupTrigger( data, events );

  // adjust EOD2 gain:
  adjustGain( trace( LocalEODTrace[0] ), 1.05*maxintensityfac *
	      trace( LocalEODTrace[0] ).maxAbs( trace( LocalEODTrace[0] ).currentTime()-0.2,
						trace( LocalEODTrace[0] ).currentTime() ) );

  // intensities:
  double maxint = maxintensityfac * FishAmplitude;
  if ( maxint > trace( LocalEODTrace[0] ).maxValue() )
    maxint = trace( LocalEODTrace[0] ).maxValue();
  IntensityRange.set( minintensityfac * FishAmplitude, maxint, nintensities );
  IntensityRange.setIncrement( MediumResolution*2 );
  IntensityRange.setSingleRepeat( singlerepeats );
  IntensityRange.setBlockRepeat( blockrepeats );
  IntensityRange.setRepeat( repeats );
  IntensityRange.alternateOutUp();

  // pre-intensities:
  if ( PreDuration > 0.0 ) {
    double maxpreint = maxpreintensityfac * FishAmplitude;
    if ( maxpreint > trace( LocalEODTrace[0] ).maxValue() )
      maxpreint = trace( LocalEODTrace[0] ).maxValue();
    PreIntensityRange.set( minpreintensityfac * FishAmplitude, maxpreint, npreintensities );
    PreIntensityRange.alternateOutDown();
  }
  else
    PreIntensityRange.set( FishAmplitude );

  // data:
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
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
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      P[2*n].clear();
      P[2*n].setLMarg( 7 );
      P[2*n].setRMarg( 2 );
      P[2*n].setBMarg( 3 );
      P[2*n].setTMarg( 1 );
      P[2*n].setXLabel( "mV/cm" );
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
      P[2*n+1].setXLabel( "ms" );
      P[2*n+1].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
      P[2*n+1].setXRange( -1000.0 * ( Delay + PreDuration ), 1000.0*Duration );
      P[2*n+1].setYLabel( "Frequency [Hz]" );
      P[2*n+1].setYLabelPos( 1.5, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
      P[2*n+1].setYRange( 0.0, 100.0 );
      P[2*n+1].setYTics( );

      n++;
    }
  }
  P.unlock();

  // plot trace:
  plotToggle( true, true, Delay + PreDuration + Duration, Delay );

  // adjust transdermal EOD:
  double val2 = trace( LocalEODTrace[0] ).maxAbs( trace( LocalEODTrace[0] ).currentTime()-0.1,
						  trace( LocalEODTrace[0] ).currentTime() );
  if ( val2 > 0.0 )
    adjustGain( trace( LocalEODTrace[0] ), maxintensityfac * val2 );

  // create signal:
  OutData signal( PreDuration + Duration, 0.001 );
  signal.setDelay( Delay );
  //  signal.setStartSource( 2 );
  signal.setTrace( am ? GlobalAMEField : GlobalEField );
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
	  ! IntensityRange;
	  ++IntensityRange ) {

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
      int k=0;
      if ( PreDuration > 0.0 ) {
	for ( ; signal.pos( k ) < PreDuration && k<signal.size(); k++ )
	  signal[k] = y;
      }
      for ( ; k<signal.size(); k++ )
	signal[k] = x;
      signal.back() = 0.0;
      /*
	string ss = "I0=" + Str( PreIntensity );
	ss += ", T0=" + Str( PreDuration, 0, 0, 'f' ) + "ms";
	ss += ", I1=" + Str( Intensity );
	ss += ", T1=" + Str( 1000.0*Duration, 0, 0, 'f' ) + "ms";
	signal.setComment( ss );
      */

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

      // meassage: 
      Str s = "Contrast: <b>" + Str( 100.0 * Contrast, 0, 0, 'f' ) + "%</b>";
      s += "  Intensity: <b>" + Str( Intensity, 0, 3, 'f' ) + "mV/cm</b>";
      if ( PreDuration > 0.0 && fabs( PreIntensity - FishAmplitude ) > 1.0e-6 ) {
	s += "  PreContrast: <b>" + Str( 100.0 * PreContrast, 0, 0, 'f' ) + "%</b>";
	s += "  PreIntensity: <b>" + Str( PreIntensity, 0, 3, 'f' ) + "mV/cm</b>";
      }
      s += "  Loop: <b>" + Str( IntensityRange.count()+1 ) + "</b>";
      message( s );

      sleep( signal.duration() + Pause );
      if ( interrupt() ) {
	stop();
	save();
	return Aborted;
      }

      testWrite( signal );
      // signal failed?
      if ( !signal.success() ) {
	if ( signal.busy() ) {
	  warning( "Output still busy!<br> Probably missing trigger.<br> Output of this signal software-triggered.", 2.0 );
	  signal.setStartSource( 0 );
	  signal.setPriority();
	  write( signal );
	  sleep( signal.duration() + Pause );
	  // trigger:
	  //      setupTrigger( data, events );
	}
	else if ( signal.error() == signal.OverflowUnderrun ) {
	  warning( "Analog Output Overrun Error!<br> Try again.", 2.0 );
	  write( signal );
	  sleep( signal.duration() + Pause );
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
	  stop();
	  save();
	  return Aborted;
	}
      }

      // adjust input gains:
      for ( int k=0; k<MaxSpikeTraces; k++ )
	if ( SpikeTrace[k] >= 0 )
	  adjust( trace( SpikeTrace[k] ), trace( SpikeTrace[k] ).signalTime()+Duration,
		  trace( SpikeTrace[k] ).signalTime()+Duration+Pause, 0.8 );

      // analyze:
      analyze();
      plot();

      // select intensities:
      if ( IntensityRange.lastSingle() )
	selectRange();
      if ( IntensityRange.finishedBlock() ) {
	if ( softStop() > 1 ) {
	  saveData();
	  save();
	  stop();
	  return Completed;
	}
	//    selectSlopes();
      }

    }

    saveData();

  }

  save();
  stop();

  return Completed;
}


void FICurve::stop( void )
{
  IntensityRange.clear();
  PreIntensityRange.clear();
  for ( int k=0; k<MaxSpikeTraces; k++ )
    if ( SpikeEvents[k] >= 0 )
      Response[k].clear();
  writeZero( GlobalAMEField );
}


void FICurve::sessionStarted( void )
{
  Index = 0;
}


void FICurve::saveRate( int trace )
{
  // create file:
  ofstream df( addPath( "firate" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# " );
  settings().save( df, "#   " );
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
  Header.save( df, "# " );
  settings().save( df, "#   " );
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
  Header.save( df, "# " );
  settings().save( df, "#   " );
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


void FICurve::saveData( void )
{
  Header.setInteger( "index", Index );
  Header.setUnit( "preintensity", EOD2Unit );
  Header.setNumber( "preintensity", PreIntensityRange.value() );
  Header.setNumber( "EOD rate", FishRate );
  Header.setUnit( "trans. amplitude", EOD2Unit );
  Header.setNumber( "trans. amplitude", FishAmplitude );
  Header.setText( "repro time", reproTimeStr() );
  Header.setText( "session time", sessionTimeStr() );

  for ( int trace=0; trace<MaxSpikeTraces; trace++ ) {
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
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {

      // f-I-curves:
      P[2*n].clear();
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
  P.unlock();  
  P.draw();
}



void FICurve::analyzeSpikes( const EventData &se, int trace,
			     double truepreintensity, double trueintensity )
{
  ResponseData &response = Response[trace][PreIntensityRange.pos()][IntensityRange.pos()];

  // signal amplitude:
  response.PreIntensity = truepreintensity;
  response.Intensity = trueintensity;

  // spikes:
  double sigtime = se.signalTime();
  response.Spikes.push( se, sigtime-Delay-PreDuration, sigtime+Duration+Pause, sigtime+PreDuration );

  // firing frequency:
  se.addFrequency( response.Rate, response.Trial, sigtime+PreDuration );

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
  const EventData &eod2 = events( LocalEODEvents[0] );

  // EOD trace unit:
  EOD2Unit = trace( LocalEODTrace[0] ).unit();

  // amplitude:
  double truepreintensity = eod2.meanSize( eod2.signalTime(), eod2.signalTime()+PreDuration );
  //  double trueprecontrast = ( truepreintensity - FishAmplitude ) / FishAmplitude;
  double trueintensity = eod2.meanSize( eod2.signalTime() + PreDuration, eod2.signalTime()+Duration );
  //  double truecontrast = ( trueintensity - FishAmplitude ) / FishAmplitude;

  double bd = eod2.back() - eod2.signalTime() - PreDuration - Duration;
  if ( bd > 0.5 )
    bd = 0.5;

  // EOD rate:
  FishRate = eod2.frequency( eod2.back() - bd, eod2.back() );

  // EOD amplitude:
  FishAmplitude = eod2.meanSize( eod2.back() - bd, eod2.back() );

  // spikes:
  for ( int k=0; k<MaxSpikeTraces; k++ )
    if ( SpikeEvents[k] >= 0 )
      analyzeSpikes( events( SpikeEvents[k] ), k, truepreintensity, trueintensity );
}


void FICurve::selectSlopes( void )
{
  if ( IntensityRange.currentIncrement() != MediumResolution )
    return;

  cerr << "FICurve::selectSlopes()" << endl;

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

    cerr << "slope " << k << " " << j << " @" << IntensityRange.value( k ) << " " << IntensityRange.value( j ) << ": " << slope << endl;

    if ( fabs( slope ) < MinRateSlope ) {

    cerr << "  skipped" << endl;

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
  
}


void FICurve::selectRange( void )
{
  if ( IntensityRange.currentIncrement() <= MediumResolution )
    return;

  int pinx = PreIntensityRange.pos();
  int iinx = IntensityRange.pos();

  cerr << "FICurve::selectRange() -> " << iinx << endl;

  int sinx = IntensityRange.pos( *PreIntensityRange );

  // no response:
  if ( Response[0][pinx][iinx].OnsetRate < 2.0/OnsetTime ) {

    // at left end:
    if ( iinx < sinx ) {
      cerr << "  skip zeros " << 0 << " - " << iinx
	   << " rate: " << Response[0][pinx][iinx].OnsetRate
	   << " minrate: " << 2.0/OnsetTime << endl;
      for ( int k=0; k <= iinx; k++ )
	IntensityRange.setSkip( k );
    }
    // at right end:
    else {
      cerr << "  skip zeros " << iinx << " - " << IntensityRange.size()-1
	   << " rate: " << Response[0][pinx][iinx].OnsetRate
	   << " minrate: " << 2.0/OnsetTime << endl;
      for ( int k=iinx; k < IntensityRange.size(); k++ )
	IntensityRange.setSkip( k );
    }

  }

  // maximum and minimum response:
  int k=IntensityRange.next( 0 );
  double min = Response[0][pinx][k].OnsetRate;
  double max = min;
  for ( k=IntensityRange.next( k+1 );
	k<IntensityRange.size();
	k=IntensityRange.next( k+1 ) ) {
    if ( Response[0][pinx][k].OnsetRate > max )
      max = Response[0][pinx][k].OnsetRate;
    else if ( Response[0][pinx][k].OnsetRate < min )
      min = Response[0][pinx][k].OnsetRate;
  }

  // no change in response:
  // at left end:
  if ( iinx < sinx ) {
    k = iinx;
    for ( int n=1; n<NSkip; n++ ) {
      int j = k;
      k = IntensityRange.next( k+1 );
      if ( k >= sinx )
	break;
      if ( fabs( Response[0][pinx][k].OnsetRate - Response[0][pinx][iinx].OnsetRate ) >
	   0.15*(max-min) )
	break;
      if ( n >= NSkip-1 ) {
	cerr << "  skip left " << 0 << " - " << j
	     << " rate: " << Response[0][pinx][j].OnsetRate
	     << " delta rate: " << max-min << endl;
	for ( int i=0; i <= j; i++ )
	  IntensityRange.setSkip( i );
      }
    }
  }
  // at right end:
  else {
    k = iinx;
    for ( int n=1; n<NSkip; n++ ) {
      int j = k;
      k = IntensityRange.previous( k-1 );
      if ( k <= sinx )
	break;
      if ( fabs( Response[0][pinx][k].OnsetRate - Response[0][pinx][iinx].OnsetRate ) >
	   0.15*(max-min) )
	break;
      if ( n >= NSkip-1 ) {
	cerr << "  skip right " << j << " - " << IntensityRange.size()-1
	     << " rate: " << Response[0][pinx][j].OnsetRate
	     << " delta rate: " << max-min << endl;
	for ( int i=j; i<IntensityRange.size(); i++ )
	  IntensityRange.setSkip( i );
      }
    }
  }

}


addRePro( FICurve );

}; /* namespace efish */

#include "moc_ficurve.cc"
