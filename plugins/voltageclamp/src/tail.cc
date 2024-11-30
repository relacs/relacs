/*
  voltageclamp/tail.cc
  Tail current protocol

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

#include <relacs/repros.h>
#include <relacs/voltageclamp/tail.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/relacsplugin.h>
#include <relacs/ephys/amplifiercontrol.h>

using namespace relacs;

namespace voltageclamp {


Tail::Tail( void )
  : PNSubtraction( "Tail", "voltageclamp", "Lukas Sonnenberg", "1.0", "Aug 23, 2018" )
{
  // add some options:
  addSection( "TailOptions" );
  addNumber( "duration0", "Stimulus duration0", 0.01, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "duration2", "Stimulus duration2", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "holdingpotential0", "Holding potential0", -100.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "mintest", "Minimum testing potential", -100.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "maxtest", "Maximum testing potential", 80.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "teststep", "Step testing potential", 5.0, 0.0, 200.0, 1.0, "mV");

  addBoolean( "auto", "Auto", true );
  addNumber( "duration1", "Stimulus duration1", 0.0006, 0.0001, 100000.0, 0.0001, "s", "ms" ).setActivation( "auto", "false" );
  addNumber( "holdingpotential1", "Holding potential1", -5.0, -200.0, 200.0, 1.0, "mV" ).setActivation( "auto", "false" );
  addNumber( "beforeMin", "Time before potential minumum", 0.0, 0.0, 1.0, 0.00002, "s", "ms" ).setActivation( "auto", "true" );

  // plot
  setWidget( &P );
}



int Tail::main( void )
{
  // get options:
  double duration0 = number( "duration0" );
//  double duration1 = number( "duration1" );
  double duration2 = number( "duration2" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential0 = number( "holdingpotential0" );
//  double holdingpotential1 = number( "holdingpotential1" );
  double mintest = number( "mintest" );
  double maxtest = number( "maxtest" );
  double teststep = number( "teststep" );

  double duration1 = 0;  ////////// somehow I need these two, otherwise there is an error
  double holdingpotential1 = 0; /// and I really don't know why
  bool automatic = boolean( "auto" );
  if ( automatic ) {
    RePro* rp_ac = repro( "Activation[voltageclamp]" );
    double beforeMin = number( "beforeMin" );

    if ((rp_ac != 0) && (rp_ac->completeRuns() > 0)) {
      Activation* ac = dynamic_cast<Activation*>( rp_ac );
      duration1 = ac->t_min - beforeMin;
      holdingpotential1 = ac->V_min;
      cerr << "tmin=" << ac->t_min << "s, Imin=" << ac->V_min << "mV\n";
    }
    else {
      warning("Run the RePro 'Activation' before running 'Tail' or change the 'Auto' setting for this RePro.", 0.0);
      return 1;
    };

  }
  else {
    duration1 = number( "duration1" );
    holdingpotential1 = number( "holdingpotential1" );
  };

  cerr << "duration1=" << duration1 << "s, holdingpotential1=" << holdingpotential1 << "mV\n";

  int stepnum = (maxtest-mintest)/teststep+1;
  std::vector<double> inact(stepnum);


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
  string IUnit = trace( CurrentTrace[0] ).unit();
  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]" )  ;

  P.clearData();
  P.setXRange(duration0*1000.0, (duration0+duration1)*1000.0 + 2.0);
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
      signal1.constWave( duration1, -1.0, holdingpotential1 );

      OutData signal2;
      signal2.setTrace( PotentialOutput[0] );
      signal2.pulseWave( duration2, -1.0, step, holdingpotential0 );

      signal.append( signal1 );
      signal.append( signal2 );

      // nix options
      Options opts;
      Parameter &p1 = opts.addNumber( "step", step, "mV" );
      signal.setMutable( p1 );
      signal.setDescription( opts );


        double mintime = duration0;
      double maxtime = 0.002 + duration0 + duration1;

//      write(signal);
//      sleep(pause);
//
//      // inactivation curve
//      SampleDataF currenttrace(duration0, 0.002 + duration0 + duration1,
//                               trace(CurrentTrace[0]).stepsize(), 0.0);
//      trace(CurrentTrace[0]).copy(signalTime(), currenttrace);

      SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential0, pause, mintime, maxtime, duration0 );

      if (interrupt()) {
        break;
      };

      // plot
      P.lock();
      P.plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
      P.draw();
      P.unlock();
    }
  }
  return Completed;
}


addRePro( Tail, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_tail.cc"
