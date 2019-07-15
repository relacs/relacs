/*
  voltageclamp/subthresholdinactivation.cc
  Estimation of inactivation time constants in the threshold regime

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

#include <relacs/fitalgorithm.h>
#include <relacs/voltageclamp/subthresholdinactivation.h>
#include <relacs/voltageclamp/pnsubtraction.h>

using namespace relacs;

namespace voltageclamp {


SubthresholdInactivation::SubthresholdInactivation( void )
  : PNSubtraction( "SubthresholdInactivation", "voltageclamp", "Lukas Sonnenberg", "1.0", "Aug 21, 2018" )
{
  // add some options:
  addNumber( "duration0", "Stimulus duration0", 0.01, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "duration1", "Stimulus duration1", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
//  addNumber( "duration2", "Stimulus duration2", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "holdingpotential0", "Holding potential0", -100.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "holdingpotential1", "Holding potential1", -100.0, -200.0, 200.0, 1.0, "mV" );

//  addList( "timesteps", "Time steps", "s", "ms" );

  addText("trange", "Time steps", "1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0, 500.0, 1000.0").setUnit( "ms" );

  addNumber( "mintest", "Minimum testing potential", -90.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "maxtest", "Maximum testing potential", -30.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "teststep", "Step testing potential", 5.0, 0.0, 200.0, 1.0, "mV");

  // plot
  P.lock();
  P.resize( 2, 2, true );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( "Current [nA]" );
  P[1].setXLabel( "Voltage [mV]" );
  P[1].setYLabel( "Time constant [ms]");

  P.unlock();
  setWidget( &P );
}



int SubthresholdInactivation::main( void )
{
  // get options:
  double duration0 = number( "duration0" );
  double duration1 = number( "duration1" );
//  double duration2 = number( "duration2" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential0 = number( "holdingpotential0" );
  double holdingpotential1 = number( "holdingpotential1" );
  double mintest = number( "mintest" );
  double maxtest = number( "maxtest" );
  double teststep = number( "teststep" );
  Str trange = allText( "trange" );


  std::vector<double> timesteps;
  trange.range(timesteps, ",", ":" );
  timesteps = timesteps/1000;
  int stepnum = (maxtest-mintest)/teststep+1;

  tau = std::vector<double> ();
  potential = std::vector<double> ();

  for ( double step=mintest;  step<=maxtest; step+=teststep ) {
    tau.push_back(0.0);
    potential.push_back(step);
  }

  // don't print repro message:
  noMessage();

  // reset plot
  P.lock();
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
    for ( int potstep=mintest;  potstep<=maxtest; potstep+=teststep) {
      i += 1;

      std::vector <double> absmax(timesteps.size());

      for ( unsigned j = 0; j < timesteps.size(); j++ ) {
        double timestep = timesteps[j];

        Str s = "Holding potential <b>" + Str(holdingpotential0, "%.1f") + " mV</b>";
        s += ", Testing potential <b>" + Str(potstep, "%.1f") + " mV</b>";
        s += ", Time step <b>" + Str(timestep*1000, "%.2f") + " ms</b>";
        s += ",  Loop <b>" + Str(Count + 1) + "</b>";
        message(s);

        // stimulus:
        OutData signal;
        signal.setTrace(PotentialOutput[0]);
        signal.constWave(duration0, -1.0, holdingpotential0);

//        OutData signal1;
//        signal1.setTrace(PotentialOutput[0]);
//        signal1.constWave(duration1, -1.0, holdingpotential1);

        OutData signal1;
        signal1.setTrace(PotentialOutput[0]);
        signal1.constWave(timestep, -1.0, potstep);

        OutData signal2;
        signal2.setTrace(PotentialOutput[0]);
        signal2.pulseWave(duration1, -1.0, holdingpotential1, holdingpotential0);

        signal.append(signal1);
        signal.append(signal2);
//        signal.append(signal3);

        // nix options
        Options opts;
        Parameter &p1 = opts.addNumber( "step", potstep, "mV" );
        Parameter &p2 = opts.addNumber( "timestep", timestep, "s" );
        signal.setMutable( p1 );
        signal.setMutable( p2 );
        signal.setDescription( opts );


        double mintime = duration0 + timestep;
        double maxtime = 0.02 + duration0 + timestep;

//        write(signal);
//        sleep(pause);
//        // minimas
//        SampleDataF currenttrace(mintime, maxtime,trace(CurrentTrace[0]).stepsize(), 0.0);
//        trace(CurrentTrace[0]).copy(signalTime(), currenttrace);
        double t0 = duration0+duration1;
        SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential0, pause, mintime, maxtime, t0 );

        if (interrupt()) {
          return 0;
        };

        double dt = currenttrace.stepsize();

        absmax[j] = min(currenttrace);
        int index = 0;
        index = minIndex(currenttrace);

        // plot
        P.lock();
        // trace
        P[0].plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
        P[0].plotPoint( (index * dt + duration0 + timestep) * 1000, Plot::First, absmax[j], Plot::First, 0,
                        Plot::Circle, 5, Plot::Pixel,
                        Plot::Magenta, Plot::Magenta);
        P.draw();
        P.unlock();
      }

      // fit exponential to absmax
      ArrayD param( 3, 1.0 );
      param[0] = -absmax[absmax.size()-1];
      param[1] = -1.0;
      param[2] = absmax[absmax.size()-1];
      ArrayD error( absmax.size(), 1.0 );
      ArrayD uncertainty( 3, 0.0 );
//    uncertainty[0] = 3000;
//    uncertainty[1] = 10;
//    uncertainty[2] = 3000;
      ArrayI paramfit( 3, 0 );
      paramfit[1] = 1;
      double chisq = 0.0;

      marquardtFit( timesteps,//.begin(), timesteps.end(),
                    absmax,//.begin(), absmax.end(),
                    error,//.begin(), error.end(),
                    expFuncDerivs, param, paramfit, uncertainty, chisq );
      tau[i] = -param[1]*1000;

      P.lock();
      P[1].plotPoint( potstep, Plot::First, tau[i], Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
                      Plot::Green, Plot::Green );
      P[1].setYRange(min(tau),max(tau));
      P.draw();
      P.unlock();

//    std::vector <double> expfun(timesteps.size());
//    for (unsigned k=0; k<timesteps.size(); k++) {
//      expfun[k] = expFunc( timesteps[k], param );
//    };

//    P.lock();
//    P[1].plot( timesteps, absmax, Plot::Magenta, 2, Plot::Solid );
//    P[1].plot( timesteps, expfun, Plot::Green, 2, Plot::Solid);
//    P[1].setXRange( 0.0, 0.1 );
//    P[1].setYRange( -1300.0, 0.0 );
//    P.draw();
//    P.unlock();

    }

  }
  return Completed;
}


addRePro( SubthresholdInactivation, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_subthresholdinactivation.cc"
