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
#include <relacs/str.h>
#include <relacs/ephys/capacitycompensation.h>
using namespace relacs;

namespace ephys {


CapacityCompensation::CapacityCompensation( void )
  : RePro( "CapacityCompensation", "ephys", "Jan Benda", "2.0", "Feb 17, 2015" )
{
  // add some options:
  addNumber( "amplitude", "Amplitude of stimulus", 1.0, -1000.0, 1000.0, 0.1 );
  addNumber( "duration", "Duration of stimulus", 0.1, 0.01, 10000.0, 0.01, "sec", "ms" );
  addNumber( "frequency", "Frequency of sine-wave stimulus", 1000.0, 1.0, 10000.0, 10.0, "Hz" );
  addInteger( "showcycles", "Number of cycles plotted", 10, 1, 10000, 1 );
  addNumber( "pause", "Duration of pause between pulses", 0.1, 0.01, 10000.0, 0.01, "sec", "ms" );
  addInteger( "average", "Number of trials to be averaged", 10, 1, 1000000 );

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

  // don't print repro message:
  noMessage();

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
  double tmin = duration - showcycles/frequency;
  double tmax = duration;
  P.lock();
  P.resize( 2, 2, true );
  P.setCommonYRange( 0, 1 );
  P[0].setXRange( 1000.0*(tmax-showcycles/frequency), 1000.0*tmax );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P[1].setXRange( dccurrent-1.1*amplitude, dccurrent+1.1*amplitude );
  if ( CurrentTrace[0] >= 0 )
    P[1].setXLabel( trace( CurrentTrace[0] ).ident() + " [" + trace( CurrentTrace[0] ).unit() + "]" );
  else
    P[1].setXLabel( outTrace( outtrace ).traceName() + " [" + outTrace( outtrace ).unit() + "]" );
  P[1].setYLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P.unlock();

  // plot trace:
  tracePlotSignal( duration, 0.0 );

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

    // get input trace:
    SampleDataF input;
    if ( CurrentTrace[0] >=0 ) {
      SampleDataF data( tmin, tmax, trace( CurrentTrace[0] ).stepsize(), 0.0F );
      trace( CurrentTrace[0] ).copy( signalTime(), data );
      if ( ::fabs( trace( CurrentTrace[0] ).stepsize() - intrace.stepsize() ) < 1e-6 )
	input = data;
      else
	input.interpolate( data, tmin, intrace.stepsize() );
    }
    else {
      input.sin( tmin, tmax, intrace.stepsize(), frequency );
      input *= amplitude;
    }

    // average:
    SampleDataF inaverage( tmin, tmax, intrace.stepsize(), 0.0F );
    indatatraces.push_back( input );
    if ( indatatraces.size() > naverage )
      indatatraces.pop_front();
    for ( int k=0; k<inaverage.size(); k++ ) {
      for ( unsigned int j=0; j<indatatraces.size(); j++ )
	inaverage[k] += ( indatatraces[j][k] - inaverage[k] )/(j+1);
    }

    // get output trace:
    SampleDataF output( tmin, tmax, intrace.stepsize(), 0.0F );
    intrace.copy( signalTime(), output );

    // average:
    SampleDataF outaverage( tmin, tmax, intrace.stepsize(), 0.0F );
    outdatatraces.push_back( output );
    if ( outdatatraces.size() > naverage )
      outdatatraces.pop_front();
    for ( int k=0; k<outaverage.size(); k++ ) {
      for ( unsigned int j=0; j<outdatatraces.size(); j++ )
	outaverage[k] += ( outdatatraces[j][k] - outaverage[k] )/(j+1);
    }

    // voltage range:
    float min = 0.0;
    float max = 0.0;
    minMax( min, max, outaverage );
    double mean = 0.5*(min+max);
    double range = 1.1*0.5*(max- min);
    min = mean - range;
    max = mean + range;
    if ( ymin == 0.0 && ymax == 0.0 ) {
      ymin = min;
      ymax = max;
    }
    else {
      double rate = 0.001;
      ymin += ( min - ymin )*rate;
      ymax += ( max - ymax )*rate;
      if ( ymax < max )
	ymax = max;
      if ( ymin > min )
	ymin = min;
    }

    // plot:
    P.lock();
    P[0].clear();
    P[0].setYRange( ymin, ymax );
    double offs = 0.5*(ymax+ymin);
    double ampl = 0.5*(ymax-ymin)/amplitude/1.1;
    P[0].plot( offs+ampl*(inaverage-dccurrent), 1000.0, Plot::Red, 2, Plot::Solid );
    P[0].plot( outaverage, 1000.0, Plot::Yellow, 2, Plot::Solid );
    P[1].clear();
    P[1].setYRange( ymin, ymax );
    P[1].plot( inaverage.array(), outaverage.array(), Plot::Yellow, 2, Plot::Solid );
    SampleDataF inp;
    inaverage.copy( duration-showcycles/frequency, duration, inp );
    SampleDataF outp;
    outaverage.copy( duration-showcycles/frequency, duration, outp );
    P[1].plot( inp.array(), outp.array(), Plot::Red, 3, Plot::Solid );
    P.draw();
    P.unlock();
  }

  directWrite( dcsignal );
  return Completed;
}


addRePro( CapacityCompensation, ephys );

}; /* namespace ephys */

#include "moc_capacitycompensation.cc"
