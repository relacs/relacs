/*
  acoustic/echotest.cc
  Check for echoes

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

#include <relacs/acoustic/echotest.h>
#include <relacs/outdata.h>
#include <relacs/sampledata.h>
#include <relacs/plot.h>
using namespace relacs;

namespace acoustic {


EchoTest::EchoTest( void )
  : RePro( "EchoTest", "acoustic", "Karin Fisch", "1.0", "Jun 22, 2010" )
{

  double duration = 0.001;
  double responseDuration = 0.1;
  double samplerate = 200000.0;
  int repeats = 2000;
  double pause = 0.1;
  int lastTraces = 10;

  // add some options:
  addNumber( "duration", "Stimulus duration", duration, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "responseDuration", "Waiting for response - duration", responseDuration, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "samplerate", "Stimulus sampling rate", samplerate, 0.0, 1000000.0, 10000.0, "Hz", "kHz" );
  addNumber( "repetitions", "Number of stimulus presentations", repeats, 1, 10000, 1, "times" );
  addNumber( "pause", "Duration of pause between stimuli", pause, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "lastTraces", "Number of traces used for average plot", lastTraces, 1, 100, 1, "1", "1" );

  P.lock();
  P.setYLabel("mean response");
  P.setXLabel("distance [cm]");
  P.unlock();
  setWidget( &P );
}


int EchoTest::main( void )
{
  // get options:
  double duration = number( "duration" );
  double responseDuration = number( "responseDuration" );
  double samplerate = number( "samplerate" );
  double repeats = number( "repetitions" );
  double pause = number( "pause" );
  int lastTraces = number( "lastTraces" );

  OutData signal;
  signal = triangle(LinearRange(0.0, duration, 1.0/samplerate), duration);
  signal.save( "signal.dat" );
  signal.setTrace( 0 );

  vector<SampleDataF> traceVector;

  // output stimulus:  
  for ( int counter=0; counter<repeats && softStop() == 0; counter++ ) {

    // output:
    write( signal );
    if ( signal.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
	       signal.errorText() + "</b>," +
	       "<br> Loop: <b>" + Str( counter+1 ) + "</b>" +
	       "<br>Exit now!" );
      return Failed;
    }

    SampleDataF data( 0.0, responseDuration, trace( 0 ).stepsize() );
    trace(0).copy( signalTime(), data );
    traceVector.push_back(data);

    if ( (int)traceVector.size()>lastTraces) {

      SampleDataF meantrace;
      average( meantrace, traceVector );

      // sound velocity : 343 m/s für 20 °C
      meantrace.scaleStepsize((1.0/343.0)*0.01); // [cm]

      plotMeanResponse(meantrace);

      traceVector.erase(traceVector.begin());
    }

    sleep( duration + pause );
    if ( interrupt() )
      return Aborted;

  }

  return Completed;
}



void EchoTest::plotMeanResponse(SampleDataF &mean) {

  P.lock();
  P.clear();
  P.plot(mean, 1.0, Plot::Orange, 2, Plot::Solid );
  P.draw();
  P.unlock();
}



addRePro( EchoTest );

}; /* namespace acoustic */

#include "moc_echotest.cc"
