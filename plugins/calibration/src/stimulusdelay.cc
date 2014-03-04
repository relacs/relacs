/*
  calibration/stimulusdelay.cc
  Measures delays between actual and reported onset of a stimulus

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

#include <cmath>
#include <relacs/sampledata.h>
#include <relacs/calibration/stimulusdelay.h>
using namespace relacs;

namespace calibration {


StimulusDelay::StimulusDelay( void )
  : RePro( "StimulusDelay", "calibration", "Jan Benda", "1.3", "Feb 18, 2014" )
{
  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addSelection( "outtrace", "Output trace", "Speaker-1" );
  addNumber( "samplerate", "Sampling rate of output", 10000.0, 1000.0, 1000000.0, 1000.0, "Hz", "kHz" );
  addNumber( "duration", "Duration of output", 0.01, 0.001, 1.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Pause between outputs", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "" );
  P.unlock();
  setWidget( &P );
}


void StimulusDelay::preConfig( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
}


int StimulusDelay::main( void )
{
  // get options:
  int intrace = index( "intrace" );
  int outtrace = index( "outtrace" );
  double samplerate = number( "samplerate" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );

  double deltat = 0.0;
  double maxdeltat = 0.001;

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( 2.0*duration, 0.6*duration );

  OutData signal;
  signal.setTrace( outtrace );
  signal.pulseWave( duration, 1.0/samplerate, 1.0, 0.0 );
  //  signal.mute();
  signal.setIdent( "one" );

  sleep( pause );
  timeStamp();

  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    sleepOn( duration+pause );
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;
    timeStamp();
    analyze( trace( intrace ), duration, pause, count, deltat, maxdeltat );
    if ( count % 10 == 0 )
      message( "Average stimulus delay: <b>" + Str( 1000.0*deltat, 0, 3, 'f' ) + " ms</b>" );
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;
  }

  return Completed;
}


int StimulusDelay::analyze( const InData &data, double duration,
			    double pause, int count, double &deltat, double &maxdeltat )
{
  // find transition:
  double max0 = data.max( signalTime()-duration, signalTime() );
  double max1 = data.max( signalTime(), signalTime()+duration );
  double thresh = 0.5*(max0+max1);
  double dt = 0.0;
  for ( int k=data.index( signalTime()-duration ); 
	k<data.index( signalTime()+duration );
	k++ ) {
    if ( data[k] > thresh ) {
      dt = data.pos( k ) - signalTime();
      break;
    }
  }

  deltat += ( dt - deltat ) / ( count+1 );

  if ( 1.2*dt > maxdeltat )
    maxdeltat = 1.2*dt;

  // get data:
  SampleDataF d( -0.0001, maxdeltat, data.sampleInterval() );
  data.copy( signalTime(), d );

  // plot:
  P.lock();
  P.clear();
  P.setTitle( "dt=" + Str( 1000.0*dt, 0, 3, 'f' ) + "ms, mean(dt)="
	      + Str( 1000.0*deltat, 0, 3, 'f' ) + "ms, n="
	      + Str( count+1 ) );
  P.plotVLine( 0.0, Plot::White, 2 );
  if ( d.size() < 30 )
    P.plot( d, 1000.0, Plot::Green, 2, Plot::Solid, Plot::Circle, 10, Plot::Green, Plot::Green );
  else
    P.plot( d, 1000.0, Plot::Green, 2, Plot::Solid );
  P.draw();
  P.unlock();

  return 0;
}


addRePro( StimulusDelay, calibration )

}; /* namespace calibration */

#include "moc_stimulusdelay.cc"
