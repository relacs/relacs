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
  : RePro( "StimulusDelay", "calibration", "Jan Benda", "2.0", "Mar 4, 2014" )
{
  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addSelection( "outtrace", "Output trace", "V-1" );
  addNumber( "samplerate", "Sampling rate of output", 10000.0, 1000.0, 1000000.0, 1000.0, "Hz", "kHz" );
  addNumber( "duration", "Duration of output", 0.01, 0.001, 1000.0, 0.001, "sec", "ms" );
  addNumber( "pause", "Pause between outputs", 0.05, 0.001, 1.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addSelection( "setdelay", "Set signal delay of analog output device", "none|minimum|mean" );

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
  int setdelay = index( "setdelay" );

  double meandeltat = 0.0;
  double meansquaredeltat = 0.0;
  double mindeltat = 10000.0;
  double maxdeltat = -10000.0;

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( 2.0*duration, 0.6*duration );

  OutData signal;
  signal.setTrace( outtrace );
  signal.pulseWave( duration, 1.0/samplerate, 1.0, 0.0 );
  //  signal.mute();
  signal.setIdent( "one" );

  if ( setdelay > 0 )
    setSignalDelay( signal.device(), 0.0 );

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
    analyze( trace( intrace ), duration, pause, count,
	     meandeltat, meansquaredeltat, mindeltat, maxdeltat );
    double stddeltat = ::sqrt( meansquaredeltat - meandeltat*meandeltat );
    if ( count % 10 == 0 )
      message( "Stimulus delay: average=<b>" + Str( 1000.0*meandeltat, 0, 3, 'f' ) +
	       " +/- " + Str( 1000.0*stddeltat, 0, 3, 'f' ) +
	       " ms</b>, minimum=<b>" + Str( 1000.0*mindeltat, 0, 3, 'f' ) + " ms</b>" );
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;
  }

  if ( setdelay == 1 )
    setSignalDelay( signal.device(), mindeltat );
  else if ( setdelay == 2 )
    setSignalDelay( signal.device(), meandeltat );
  return Completed;
}


int StimulusDelay::analyze( const InData &data, double duration,
			    double pause, int count,
			    double &meandeltat, double &meansquaredeltat,
			    double &mindeltat, double &maxdeltat )
{
  // find transition:
  double twin = duration;
  if ( pause < twin )
    twin = pause;
  double max0 = data.max( signalTime()-twin, signalTime() );
  double max1 = data.max( signalTime(), signalTime()+twin );
  double thresh = 0.5*(max0+max1);
  double dt = 0.0;
  for ( int k=data.index( signalTime()-twin ); 
	k<data.index( signalTime()+twin );
	k++ ) {
    if ( data[k] > thresh ) {
      dt = data.pos( k ) - signalTime();
      break;
    }
  }

  meandeltat += ( dt - meandeltat ) / ( count+1 );
  meansquaredeltat += ( dt*dt - meansquaredeltat ) / ( count+1 );

  if ( dt > maxdeltat )
    maxdeltat = dt;
  if ( dt < mindeltat )
    mindeltat = dt;

  // get data:
  double mdt = 1.2*maxdeltat;
  if ( mdt < 0.001 )
    mdt = 0.001;
  SampleDataF d( -0.0001, mdt, data.sampleInterval() );
  data.copy( signalTime(), d );

  // plot:
  P.lock();
  P.clear();
  P.setTitle( "dt=" + Str( 1000.0*dt, 0, 3, 'f' ) + "ms, mean(dt)="
	      + Str( 1000.0*meandeltat, 0, 3, 'f' ) + "ms, n="
	      + Str( count+1 ) );
  double dy = 0.1*(max1-max0);
  P.setYRange( max0-dy, max1+dy );
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
