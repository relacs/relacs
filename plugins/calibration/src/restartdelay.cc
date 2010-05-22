/*
  calibration/restartdelay.cc
  Measures delays due to restart of the data acquisition

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

#include <cmath>
#include <relacs/sampledata.h>
#include <relacs/fitalgorithm.h>
#include <relacs/calibration/restartdelay.h>
using namespace relacs;

namespace calibration {


RestartDelay::RestartDelay( void )
  : RePro( "RestartDelay", "RestartDelay", "Calibration",
	   "Jan Benda", "1.3", "Feb 8, 2008" ),
    P( this, "restartdelayplot" )
{
  // add some options:
  addNumber( "duration", "Duration of analysis window", 0.1, 0.01, 1000.0, 0.02, "sec", "ms" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 );
  addSelection( "intrace", "Input trace", "V-1" );
  addNumber( "samplerate", "Sampling rate of zero output", 10000.0, 1000.0, 1000000.0, 1000.0, "Hz", "kHz" );

  // plot:
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "" );
  P.unlock();
}


void RestartDelay::config( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
}


int RestartDelay::main( void )
{
  // get options:
  double duration = number( "duration" );
  int repeats = integer( "repeats" );
  int intrace = traceIndex( text( "intrace", 0 ) );
  double samplerate = number( "samplerate" );

  double deltat = 0.0;

  // don't print repro message:
  noMessage();

  // plot trace:
  plotToggle( true, true, 2.0*duration, 1.0*duration );

  // plot:
  P.lock();
  P.setXRange( -1000.0*duration, 1000.0*duration );
  P.unlock();

  OutData signal( 2, 1.0/samplerate );
  signal = 0.0;
  signal.setDevice( trace( intrace ).device() );
  signal.setChannel( 0 );
  signal.setRestart();
  signal.mute();
  signal.setIdent( "zero" );

  sleep( duration );

  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    sleep( duration );
    if ( interrupt() )
      return count > 2 ? Completed : Aborted;
    analyze( trace( intrace ), duration, count, deltat );
    sleepWait( duration );
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
  int d2 = d.size()/2;
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

  unlockAll();
  
  // fit sine wave:
  SampleDataF s( d );
  s = 0.1;
  ArrayD p( 4 );
  p[0] = offs;
  p[1] = ampl;
  p[2] = freq;
  p[3] = 0.0;
  ArrayI pi1( 4, 0 );
  pi1[3] = 1;
  ArrayI pi2( 4, 1 );
  pi2[0] = 0;
  pi2[1] = 0;
  ArrayD uncert( 4 );
  double chisq;

  marquardtFit( d.range().begin(), d.range().begin()+d2,
		d.begin(), d.begin()+d2,
		s.begin(), s.begin()+d2,
		sineFuncDerivs, p, pi2, uncert, chisq );
  /*
  marquardtFit( d.range().begin(), d.range().begin()+d2,
		d.begin(), d.begin()+d2,
		s.begin(), s.begin()+d2,
		sineFuncDerivs, p, pi1, uncert, chisq );
  */
  double phase1 = p[3];
  SampleDataF s1( -duration, 0.0, d.stepsize() );
  for ( int k=0; k<s1.size(); k++ )
    s1[k] = sineFunc( s1.pos( k ), p );
  marquardtFit( d.range().begin()+d2, d.range().end(),
		d.begin()+d2, d.end(),
		s.begin()+d2, s.end(),
		sineFuncDerivs, p, pi1, uncert, chisq );
  double phase2 = p[3];
  SampleDataF s2( 0.0, duration, d.stepsize() );
  for ( int k=0; k<s2.size(); k++ )
    s2[k] = sineFunc( s2.pos( k ), p );

  freq = p[2];
  while ( phase2 < phase1 )
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
  P.unlock();
  P.draw();

  lockAll();

  return 0;
}


addRePro( RestartDelay );

}; /* namespace calibration */

#include "moc_restartdelay.cc"
