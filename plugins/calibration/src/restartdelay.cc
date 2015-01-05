/*
  calibration/restartdelay.cc
  Measures delays due to restart of the data acquisition

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

#include <cmath>
#include <relacs/sampledata.h>
#include <relacs/fitalgorithm.h>
#include <relacs/calibration/restartdelay.h>
using namespace relacs;

namespace calibration {


RestartDelay::RestartDelay( void )
  : RePro( "RestartDelay", "calibration", "Jan Benda", "1.4", "Feb 18, 2014" )
{
  // add some options:
  addNumber( "duration", "Duration of analysis window", 0.1, 0.01, 1000.0, 0.02, "sec", "ms" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addSelection( "intrace", "Input trace", "V-1" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "" );
  P.unlock();
  setWidget( &P );
}


void RestartDelay::preConfig( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
}


int RestartDelay::main( void )
{
  // get options:
  double duration = number( "duration" );
  int repeats = integer( "repeats" );
  int intrace = index( "intrace" );

  double deltat = 0.0;

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotSignal( 2.0*duration, 1.0*duration );

  // plot:
  P.lock();
  P.setXRange( -1000.0*duration, 1000.0*duration );
  P.unlock();

  OutData signal;
  signal.setTrace( 0 );
  signal.constWave( 0.0 );
  signal.setRestart();
  signal.mute();

  sleep( duration );

  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;
    analyze( trace( intrace ), duration, count, deltat );
    if ( count % 10 == 0 )
      message( "Average restart delay: <b>" + Str( 1000.0*deltat, 0, 3, 'f' ) + " ms</b>" );
    sleepWait( 5.0*duration );
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;
  }

  return Completed;
}


int RestartDelay::analyze( const InData &data, double duration,
			   int count, double &deltat )
{
  // get data:
  SampleDataF d( -duration, duration, data.sampleInterval() );
  int d2 = d.index( 0.0 );
  for ( int k=0, j=data.restartIndex()-d2;
	k<d.size() && j<data.size();
	k++, j++ ) {
    d[k] = data[j];
  }

  // estimate parameter:
  float min = 0.0;
  float max = 0.0;
  minMax( min, max, d );
  double offs = 0.5*(min+max);
  double ampl = 0.5*(max-min);
  // detect zero crossings:
  // this gets worse with noise on the input!
  int nz = 0;
  double t0 = 0.0;
  double t1 = 0.0;
  for ( int k=1; k<d2; k++ ) {
    if ( d[k-1] < offs && d[k] >= offs ) {
      nz++;
      if ( nz == 1 )
	t0 = d.pos( k );
      t1 = d.pos( k );
    }
  }
  if ( nz <= 1 ) {
    return 1;
  }
  double freq = double( nz-1 ) / ( t1 - t0 );
  double phase = t1*freq;
  while ( phase < 0.0 )
    phase += 1.0;
  phase *= 2.0*M_PI;
  
  // fit sine wave:
  SampleDataF s( d );
  s = 0.1;
  ArrayD p( 4 );
  p[0] = offs;
  p[1] = ampl;
  p[2] = freq;
  p[3] = phase;
  ArrayI pi1( 4, 1 );  // freq and phase
  pi1[0] = 0;
  pi1[1] = 0;
  ArrayI pi2( 4, 0 );  // phase only
  pi2[3] = 1;
  ArrayD uncert( 4 );
  double chisq;

  marquardtFit( d.range().begin(), d.range().begin()+d2,
		d.begin(), d.begin()+d2,
		s.begin(), s.begin()+d2,
		sineFuncDerivs, p, pi1, uncert, chisq );
  /*
  marquardtFit( d.range().begin(), d.range().begin()+d2,
		d.begin(), d.begin()+d2,
		s.begin(), s.begin()+d2,
		sineFuncDerivs, p, pi2, uncert, chisq );
  */
  double phase1 = p[3];
  SampleDataF s1( -duration, 0.0, d.stepsize() );
  for ( int k=0; k<s1.size(); k++ )
    s1[k] = sineFunc( s1.pos( k ), p );

  // detect zero crossings:
  for ( int k=d2+1; k<d.size(); k++ ) {
    if ( d[k-1] < offs && d[k] >= offs ) {
      phase = d.pos( k ) * freq;
      break;
    }
  }
  while ( phase > 1.0 )
    phase -= 1.0;
  phase *= 2.0*M_PI;
  p[3] = phase;
  marquardtFit( d.range().begin()+d2, d.range().end(),
		d.begin()+d2, d.end(),
		s.begin()+d2, s.end(),
		sineFuncDerivs, p, pi2, uncert, chisq );
  double phase2 = p[3];
  SampleDataF s2( 0.0, duration, d.stepsize() );
  for ( int k=0; k<s2.size(); k++ )
    s2[k] = sineFunc( s2.pos( k ), p );

  freq = p[2];
  while ( phase2 < phase1-0.1*2.0*M_PI )
    phase2 += 2.0*M_PI;
  double dt = ( phase2 - phase1 ) / 2.0 / M_PI / freq;
  deltat += ( dt - deltat ) / ( count+1 );

  // plot:
  P.lock();
  P.clear();
  P.setTitle( "f=" + Str( freq, 0, 0, 'f' ) + "Hz, dt="
	      + Str( 1000.0*dt, 0, 3, 'f' ) + "ms, mean(dt) ="
	      + Str( 1000.0*deltat, 0, 3, 'f' ) + "ms, n="
	      + Str( count+1 ) );
  P.plotVLine( 0.0, Plot::White, 2 );
  P.plot( d, 1000.0, Plot::Green, 4, Plot::Solid );
  P.plot( s1, 1000.0, Plot::Red, 2, Plot::Solid );
  P.plot( s2, 1000.0, Plot::Red, 2, Plot::Solid );
  P.draw();
  P.unlock();

  return 0;
}


addRePro( RestartDelay, calibration );

}; /* namespace calibration */

#include "moc_restartdelay.cc"
