/*
  acoustic/echotest.cc
  Check for echoes

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

#include <deque>
#include <relacs/outdata.h>
#include <relacs/acoustic/echotest.h>
using namespace relacs;

namespace acoustic {


EchoTest::EchoTest( void )
  : RePro( "EchoTest", "acoustic", "Karin Fisch, Jan Benda", "1.0", "Jul 11, 2015" ),
    Traces()
{
  // add some options:
  addNumber( "duration", "Stimulus duration", 0.01, 0.0, 10.0, 0.001, "seconds", "ms" );
  addNumber( "frequency", "Carrier frequency", 0.0, -200000.0, 200000.0, 1000.0, "Hz", "kHz" );
  addNumber( "intensity", "Stimulus intensity", 80.0, -200.0, 200.0, 5.0, "dB" );
  addInteger( "repeats", "Repetitions of stimulus", 0, 0, 10000, 10 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "pause", "Duration of pause between stimuli", 0.1, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addSelection( "outtrace", "Output trace", "Speaker-1" );
  addSelection( "intrace", "Input trace", "Sound-1" );
  addInteger( "average", "Number of trials to be averages", 10, 1, 10000 );
  addNumber( "maxecho", "Maximum echo distance", 10.0, 0.0, 1000.0, 0.1, "m" );
  addNumber( "soundspeed", "Speed of sound", 343.0, 1.0, 100000.0, 1.0, "m/s" );
  // sound velocity : 343 m/s at 20 °C

  P.lock();
  P.setYLabel( "Echo" );
  P.setXLabel( "Distance [cm]" );
  P.unlock();
  setWidget( &P );
}


void EchoTest::preConfig( void )
{
  setText( "outtrace", loudspeakerTraceNames() );
  setToDefault( "outtrace" );
  setText( "intrace", soundTraceNames() );
  setToDefault( "intrace" );
}


int EchoTest::main( void )
{
  // get options:
  double duration = number( "duration" );
  double frequency = number( "frequency" );
  double intensity = number( "intensity" );
  double repeats = integer( "repeats" );
  double pause = number( "pause" );
  string outtrace = text( "outtrace", 0 );
  int intrace = traceIndex( text( "intrace", 0 ) );
  unsigned int naverage = integer( "average" );
  double maxecho = number( "maxecho" );
  double soundspeed = number( "soundspeed" );
  double maxtime = maxecho/soundspeed;

  // input trace:
  if ( intrace < 0 || intrace >= traces().size() ) {
    warning( "Input trace " + Str( intrace ) + 
	     " does not exist!" );
    return Failed;
  }
  const InData &indata = trace( intrace );

  P.lock();
  P.setTitle( "Speed of sound: " + Str( soundspeed, 0, 0, 'f' ) + "m/s" );
  P.unlock();

  // plot trace:
  tracePlotSignal( 2.0*maxtime );

  // envelope:
  OutData am;
  am.setTraceName( outtrace );
  am.triangleWave( duration, -1, duration );

  // signal:
  OutData signal;
  signal.setTraceName( outtrace );
  if ( ::fabs( frequency ) > 0.1 )
    signal.fill( am, frequency );
  else
    signal = am;
  signal.setIntensity( intensity );
  //  signal.save( "signal.dat" );
  
  deque<SampleDataF> datatraces;
  int state = Completed;

  // output stimulus:  
  for ( int count=0;
	( count<repeats || repeats == 0 ) && softStop() == 0;
	count++ ) {

    // output:
    write( signal );
    if ( signal.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
	       signal.errorText() + "</b>," +
	       "<br> Loop: <b>" + Str( count+1 ) + "</b>" +
	       "<br>Exit now!" );
      return Failed;
    }
    if ( interrupt() ) {
      if ( count == 0 )
        state = Aborted;
      break;
    }
    sleep( pause );

    // get trace:
    SampleDataF voltage( -0.5*duration, maxtime, indata.stepsize(), 0.0F );
    indata.copy( signalTime()+0.5*duration, voltage );

    // average:
    SampleDataF meanvoltage( -0.5*duration, maxtime, indata.stepsize(), 0.0F );
    datatraces.push_back( voltage );
    if ( datatraces.size() > naverage )
      datatraces.pop_front();
    for ( int k=0; k<meanvoltage.size(); k++ ) {
      for ( unsigned int j=0; j<datatraces.size(); j++ )
	meanvoltage[k] += ( datatraces[j][k] - meanvoltage[k] )/(j+1);
    }

    plot( meanvoltage, soundspeed );

    if ( interrupt() ) {
      if ( count == 0 )
        state = Aborted;
    break;
    }

  }

  writeZero( outtrace );
  return state;
}


  void EchoTest::plot( const SampleDataF &meanvoltage, double soundspeed )
{
  P.lock();
  P.clear();
  P.plotVLine( 0.0, Plot::White, 2 );
  P.plot( meanvoltage, soundspeed*100.0, Plot::Orange, 2, Plot::Solid );
  P.draw();
  P.unlock();
}


addRePro( EchoTest, acoustic );

}; /* namespace acoustic */

#include "moc_echotest.cc"
