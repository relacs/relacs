/*
  voltageclamp/inactivation.cc
  Inactivation protocol

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

#include <relacs/voltageclamp/inactivation.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>
using namespace relacs;

namespace voltageclamp {


Inactivation::Inactivation( void )
  : PNSubtraction( "Inactivation", "voltageclamp", "Lukas Sonnenberg", "1.0", "Aug 09, 2018" )
{
  newSection( "Stimulus" );

  addSection( "Stimulus" );
  // add some options:
  addNumber( "duration0", "Stimulus duration0", 0.01, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "duration1", "Stimulus duration1", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "duration2", "Stimulus duration2", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "holdingpotential0", "Holding potential0", -100.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "holdingpotential1", "Holding potential1", 30.0, -200.0, 200.0, 1.0, "mV" );

//  addNumber( "testingpotential", "Testing potential", -70.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "mintest", "Minimum testing potential", -100.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "maxtest", "Maximum testing potential", 80.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "teststep", "Step testing potential", 5.0, 0.0, 200.0, 1.0, "mV");

  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );

  // plot
  setWidget( &P );
}


int Inactivation::main( void )
{
  // get options:
  double duration0 = number( "duration0" );
  double duration1 = number( "duration1" );
  double duration2 = number( "duration2" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential0 = number( "holdingpotential0" );
  double holdingpotential1 = number( "holdingpotential1" );
  double mintest = number( "mintest" );
  double maxtest = number( "maxtest" );
  double teststep = number( "teststep" );

  int stepnum = (maxtest-mintest)/teststep+1;

  // don't print repro message:
  noMessage();

  // set amplifier to VC mode
  ephys::AmplifierControl *ampl = dynamic_cast< ephys::AmplifierControl* >( control( "AmplifierControl" ) );
  if ( ampl == 0 ) {
    warning( "No amplifier found." );
    return Failed;
  }
  ampl ->activateVoltageClampMode();

  // reset plot

  P.lock();
  string IUnit = trace( CurrentTrace[0] ).unit();
  string VUnit = trace( SpikeTrace[0]).unit();

  P.resize( 2, 2, true );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]"  );
  P[1].setXLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]"  );
  P[1].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]"  );

  P[0].clearData();
  P[1].clearData();
  P[1].setXRange(mintest,maxtest);
  P.unlock();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential0 );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential0 ) + "mV" );


  // write holdingpotential:
  write( holdingsignal );
  sleep( pause );

  for ( int Count=0;
	( repeats <= 0 || Count < repeats ) && softStop() == 0;
	Count++ ) {

    int i = -1;
    for ( int step=mintest;  step<=maxtest; step+=teststep) {
      i += 1;
      Str s = "Holding potential <b>" + Str(holdingpotential0, "%.1f") + " mV</b>";
      s += ", Testing potential <b>" + Str(step, "%.1f") + " mV</b>";
      s += ",  Loop <b>" + Str(Count + 1) + "</b>";
      message(s);

      // stimulus:
      OutData signal;
      signal.setTrace( PotentialOutput[0] );
      signal.constWave( duration0, -1.0,  holdingpotential0 );

      OutData signal1;
      signal1.setTrace( PotentialOutput[0] );
      signal1.constWave( duration1, -1.0, step );

      OutData signal2;
      signal2.setTrace( PotentialOutput[0] );
      signal2.pulseWave( duration2, -1.0,  holdingpotential1, holdingpotential0 );

      signal.append( signal1 );
      signal.append( signal2 );

      // nix options
      Options opts;
      Parameter &p1 = opts.addNumber( "step", step, "mV" );
      signal.setMutable( p1 );
      signal.setDescription( opts );


      double mintime = duration0+duration1-0.002;
      double maxtime = duration0+duration1+0.01;

//      write(signal);
//      sleep(pause);
//      // inactivation curve
//      SampleDataF currenttrace(-0.002 + duration0 + duration1, 0.01 + duration0 + duration1,
//                               trace(CurrentTrace[0]).stepsize(), 0.0);
//      trace(CurrentTrace[0]).copy(signalTime(), currenttrace);

      double t0 = duration0+duration1-0.001;
      SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential0, pause, mintime, maxtime, t0 );

      if (interrupt()) {
        break;
      };

      double dt = currenttrace.stepsize();

      double absmax = 0.0;
      int index = 0;
      int idx0 = .002/dt+5;

      std::vector <double> y(currenttrace.size()-idx0);
      for (int j=0; j<currenttrace.size()-idx0; j++) {
        y[j] = currenttrace[idx0+j];
      };

      absmax = min( y );
      index = minIndex( y ) + idx0;
      inact.push_back(absmax);
      potential.push_back(step);

      // plot
      P.lock();
      // trace
      P[0].plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
      P[0].plotPoint( (index*dt+duration0+duration1)*1000-2, Plot::First, absmax, Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
                      Plot::Magenta, Plot::Magenta );

        // inactivation curve
      P[1].setYRange(P[0].yminRange(),P[0].ymaxRange());
      P[1].plotPoint( step, Plot::First, absmax, Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
                      Plot::Magenta, Plot::Magenta );

      P.draw();
      P.unlock();
    }
  }
  return Completed;
}


addRePro( Inactivation, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_inactivation.cc"
