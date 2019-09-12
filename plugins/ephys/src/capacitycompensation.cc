/*
  ephys/capacitycompensation.cc
  Sine waves for checking the capacity compensation of the amplifier inside the cell.

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

#include <relacs/sampledata.h>
#include <relacs/stats.h>
#include <relacs/tablekey.h>
#include <relacs/str.h>
#include <relacs/ephys/amplifiercontrol.h>
#include <relacs/ephys/capacitycompensation.h>
using namespace relacs;

namespace ephys {


CapacityCompensation::CapacityCompensation( void )
  : RePro( "CapacityCompensation", "ephys", "Jan Benda", "2.4", "May 22, 2017" )
{
  // add some options:
  addNumber( "amplitude", "Amplitude of stimulus", 1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of stimulus", 0.01, 0.01, 10000.0, 0.01, "sec", "ms" );
  addNumber( "frequency", "Frequency of sine-wave stimulus", 1000.0, 1.0, 10000.0, 10.0, "Hz" );
  addInteger( "showcycles", "Number of cycles plotted", 10, 1, 10000, 1 );
  addNumber( "pause", "Duration of pause between pulses", 0.01, 0.01, 10000.0, 0.01, "sec", "ms" );
  addInteger( "average", "Number of trials to be averaged", 10, 1, 1000000 );
  addBoolean( "skipspikes", "Skip trials with detected spikes", true );
  addBoolean( "dynamicrange", "Dynamically adjust plot range", false );
  addNumber( "rate", "Rate for adjusting plot ranges", 0.01, 0.0001, 0.1, 0.001 ).setActivation( "dynamicrange", "true" );
  addBoolean( "autobridge", "Switch to Bridge Mode automatically", false);
  // plot:
  setWidget( &P );
}


void CapacityCompensation::preConfig( void )
{
  if ( CurrentTrace[0] >= 0 )
    setUnit( "amplitude", trace( CurrentTrace[0] ).unit() );
  else if ( CurrentOutput[0] >= 0 )
    setUnit( "amplitude", outTrace( CurrentOutput[0] ).unit() );
  else
    setUnit( "amplitude", outTrace( 0 ).unit() );
}


int CapacityCompensation::main( void )
{
  // get options:
  double amplitude = number( "amplitude" );
  double duration = number( "duration" );
  double frequency = number( "frequency" );
  int showcycles = integer( "showcycles" );
  double pause = number( "pause" );
  unsigned int naverage = integer( "average" );
  bool skipspikes = boolean( "skipspikes" );
  bool dynamicrange = boolean( "dynamicrange" );
  double rate = number( "rate" );
  bool autobridge = boolean( "autobridge" );

  // don't print repro message:
  noMessage();

  // set amplifier to Bridge mode
  if ( autobridge ) {
    ephys::AmplifierControl *ampl = dynamic_cast< ephys::AmplifierControl * >( control("AmplifierControl"));
    if (ampl == 0) {
      warning("No amplifier found.");
      return Failed;
    }
    ampl->activateBridgeMode();
  };

  // in- and outtrace:
  const InData &intrace = trace( SpikeTrace[0] >= 0 ? SpikeTrace[0] : 0 );
  int outtrace = CurrentOutput[0] >= 0 ? CurrentOutput[0] : 0;
  deque< SampleDataF > indatatraces;
  deque< SampleDataF > outdatatraces;

  // dc current:
  double dccurrent = stimulusData().number( outTraceName( outtrace ) );
  OutData dcsignal;
  dcsignal.setTrace( outtrace );
  dcsignal.constWave( dccurrent );
  dcsignal.setIdent( "DC=" + Str( dccurrent ) + outTrace( outtrace ).unit() );

  // plot:
  double ymin = 0.0;
  double ymax = 0.0;
  if ( duration - showcycles/frequency < 2.0/frequency ) {
    duration = (showcycles+2.0)/frequency;
    warning( "Duration too small. Set to at least " + Str( 1000.0*duration, "%.0f" ) + "ms !", 4.0 );
  }
  double tmin = duration - (showcycles+1)/frequency;
  double tmax = duration - 1.0/frequency;
  P.lock();
  P.resize( 2, 2, true );
  P.setCommonYRange( 0, 1 );
  P[0].setXRange( 1000.0*tmin, 1000.0*tmax );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P[1].setXRange( -1.1*amplitude, 1.1*amplitude );
  if ( CurrentTrace[0] >= 0 )
    P[1].setXLabel( trace( CurrentTrace[0] ).ident() + " [" + trace( CurrentTrace[0] ).unit() + "]" );
  else
    P[1].setXLabel( outTrace( outtrace ).traceName() + " [" + outTrace( outtrace ).unit() + "]" );
  P[1].setYLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P.unlock();

  // plot trace:
  tracePlotSignal( duration, 0.0 );

  // results:
  SampleDataF inaverage( tmin, tmax, intrace.stepsize(), 0.0F );
  SampleDataF outaverage( tmin, tmax, intrace.stepsize(), 0.0F );
  double b = 0.0;
  double bu = 0.0;
  double m = 0.0;
  double mu = 0.0;
  double chisq = 0.0;
    
  // signal:
  double samplerate = intrace.sampleRate();
  OutData signal;
  signal.setTrace( outtrace );
  signal.sineWave( duration, 1.0/samplerate, frequency, 0.0, amplitude, 0.0 );
  signal += dccurrent;

  // message:
  Str s = "Amplitude <b>" + Str( amplitude ) + " nA</b>";
  s += ",  Frequency <b>" + Str( frequency, "%.0f" ) + " Hz</b>";
  s += ",  Duration <b>" + Str( 1000.0*duration, "%.0f" ) + " ms</b>";
  message( s );
    
  // write stimulus:
  while ( ! interrupt() && softStop() == 0 ) {

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    if ( interrupt() )
      break;
    sleep( pause );

    // check for spikes:
    if ( skipspikes && SpikeEvents[0] >= 0 ) {
      bool spikes = events( SpikeEvents[0] ).count( signalTime()+tmin,
						    signalTime()+tmax );
      if ( spikes )
	continue;
    }

    // get input trace:
    SampleDataF input;
    if ( CurrentTrace[0] >= 0 ) {
      SampleDataF data( tmin, tmax, trace( CurrentTrace[0] ).stepsize(), 0.0F );
      trace( CurrentTrace[0] ).copy( signalTime(), data );
      if ( ::fabs( trace( CurrentTrace[0] ).stepsize() - intrace.stepsize() ) < 1e-6 )
	input = data;
      else
	input.interpolate( data, tmin, intrace.stepsize() );
    }
    else {
      SampleDataF insine;
      insine.sin( 0.0, duration, intrace.stepsize(), frequency );
      insine *= amplitude;
      insine += dccurrent;
      insine.copy( tmin, tmax, input );
    }

    // current average:
    inaverage = 0.0F;
    indatatraces.push_back( input );
    if ( indatatraces.size() > naverage )
      indatatraces.pop_front();
    for ( int k=0; k<inaverage.size(); k++ ) {
      for ( unsigned int j=0; j<indatatraces.size(); j++ )
	inaverage[k] += ( indatatraces[j][k] - inaverage[k] )/(j+1);
    }
    inaverage -= mean( inaverage );

    // get output trace:
    SampleDataF output( tmin, tmax, intrace.stepsize(), 0.0F );
    intrace.copy( signalTime(), output );

    // voltage average:
    outaverage = 0.0F;
    outdatatraces.push_back( output );
    if ( outdatatraces.size() > naverage )
      outdatatraces.pop_front();
    for ( int k=0; k<outaverage.size(); k++ ) {
      for ( unsigned int j=0; j<outdatatraces.size(); j++ )
	outaverage[k] += ( outdatatraces[j][k] - outaverage[k] )/(j+1);
    }
    outaverage -= mean( outaverage );

    // subtract running average:
    int w = outaverage.indices( 1.0/frequency );
    for ( int k=0; k<outaverage.size(); k++ ) {
      int i = k - w/2;
      if ( i < 0 )
	i = 0;
      else if ( i+w >= outaverage.size() )
	i = outaverage.size() - w;
      outaverage[k] -= mean( outaverage.begin()+i, outaverage.begin()+i+w );
    }

    // linefit:
    lineFit( inaverage.array(), outaverage.array(), b, bu, m, mu, chisq );
    double x1 = -1.1*amplitude;
    double y1 = m*x1+b;
    double x2 = 1.1*amplitude;
    double y2 = m*x2+b;
    double rms = ::sqrt( chisq/inaverage.size() );

    // current and voltage range:
    float min = 0.0;
    float max = 0.0;
    minMax( min, max, inaverage );
    double camplitude = max - min;
    minMax( min, max, outaverage );
    if ( ymin == 0.0 && ymax == 0.0 ) {
      ymin = min;
      ymax = max;
    }
    else if ( dynamicrange ) {
      ymin += ( min - ymin )*rate;
      ymax += ( max - ymax )*rate;
      if ( ymax < max )
	ymax = max;
      if ( ymin > min )
	ymin = min;
    }
    else {
      if ( min < ymin )
	ymin = min;      
      if ( max > ymax )
	ymax = max;      
    }

    // plot:
    P.lock();
    P[0].clear();
    if ( ! P[0].zoomedYRange() )
      P[0].setYRange( ymin, ymax );
    double ampl = (ymax-ymin)/camplitude/1.1;
    P[0].plot( ampl*inaverage, 1000.0, Plot::Red, 2, Plot::Solid );
    P[0].plot( outaverage, 1000.0, Plot::Yellow, 2, Plot::Solid );
    P[1].clear();
    P[1].setTitle( "RMS = " + Str( rms, 0, 2, 'f' ) + " mV" );
    if ( ! P[1].zoomedYRange() )
      P[1].setYRange( ymin, ymax );
    P[1].plot( inaverage.array(), outaverage.array(), Plot::Yellow, 3, Plot::Solid );
    P[1].plotLine( x1, y1, x2, y2, Plot::Orange, 2 );
    P.draw();
    P.unlock();
  }

  directWrite( dcsignal );

  if ( outdatatraces.size() >= naverage )
    save( inaverage, intrace.ident(), intrace.unit(), outaverage,
	  b, bu, m, mu, chisq );

  return Completed;
}


void CapacityCompensation::save( const SampleDataF &inaverage, 
				 const string &inname, const string &inunit,
				 const SampleDataF &outaverage,
				 double b, double bu, double m, double mu, 
				 double chisq )
{
  string outname = "current";
  string outunit = "nA";
  if ( CurrentTrace[0] >= 0 ) {
    outname = trace( CurrentTrace[0] ).ident();
    outunit = trace( CurrentTrace[0] ).unit();
  }
  else if ( CurrentOutput[0] >= 0 ) {
    outname = outTrace( CurrentOutput[0] ).traceName();
    outunit = outTrace( CurrentOutput[0] ).unit();
  }

  ofstream df( addPath( "capacitycompensation-average.dat" ).c_str(),
               ofstream::out | ofstream::app );

  Options header;
  header.addNumber( "slope", m, mu, inunit + "/" + outunit, "%g" );
  header.addNumber( "intercept", b, bu, inunit, "%g" );
  header.addNumber( "chisq", chisq, inunit + "^2", "%g" );
  lockStimulusData();
  header.newSection( stimulusData() );
  unlockStimulusData();
  header.newSection( settings() );
  header.save( df, "# ", 0, FirstOnly );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "t", "ms", "%7.2f" );
  datakey.addNumber( inname, inunit, "%6.2f" );
  datakey.addNumber( outname, outunit, "%6.2f" );
  datakey.saveKey( df );

  for ( int k=0; k<inaverage.size(); k++ ) {
    datakey.save( df, 1000.0*inaverage.pos( k ), 0 );
    datakey.save( df, inaverage[k] );
    datakey.save( df, outaverage[k] );
    df << '\n';
  }
  
  df << "\n\n";
}


addRePro( CapacityCompensation, ephys );

}; /* namespace ephys */

#include "moc_capacitycompensation.cc"
