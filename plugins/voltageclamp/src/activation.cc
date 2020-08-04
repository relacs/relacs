/*
  voltageclamp/activation.cc
  Activation protocol

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
#include <relacs/voltageclamp/activation.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>

using namespace relacs;

namespace voltageclamp {

// !!!!!!!!!!!!!!!!!!!!!!PLOTTING ABSOLUTE MAXIMUM IS HARDCODED!!!!!!!!!!!!!!!!!!!!
Activation::Activation( void )
//  : RePro( "Activation", "voltageclamp", "Jan Benda & Lukas Sonnenberg", "1.0", "Aug 09, 2018" )
  : PNSubtraction( "Activation", "voltageclamp", "Jan Benda & Lukas Sonnenberg", "1.0", "Aug 09, 2018" )
{
  // add some options:
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "holdingpotential", "Holding potential", -100.0, -200.0, 200.0, 1.0, "mV" );

  addNumber( "mintest", "Minimum testing potential", -100.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "maxtest", "Maximum testing potential", 80.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "teststep", "Step testing potential", 5.0, 0.0, 200.0, 1.0, "mV");

  addNumber( "fitdelay", "Onset time of fit", .0005, 0.0001, 0.1, 0.0001, "s", "ms" );

  addNumber( "minrevpot", "minimum of reversal potential", -50.0, -200.0, 200.0, 10.0, "mV" );
  addNumber( "maxrevpot", "maximum of reversal potential", 200.0, -200.0, 200.0, 10.0, "mV" );

  setWidget( &P );
}


int Activation::main( void )
{
  // get options:
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential = number( "holdingpotential" );
  double mintest = number( "mintest" );
  double maxtest = number( "maxtest" );
  double teststep = number( "teststep" );
  double fitdelay = number( "fitdelay" );

  int stepnum = (maxtest-mintest)/teststep+1;
  std::vector<double> IV(stepnum);

  potential = std::vector<double>();
  tau = std::vector<double> ();
  g_act = std::vector<double> ();

  double I_min = std::numeric_limits<double>::infinity();
  V_min = std::numeric_limits<double>::infinity();
  t_min = std::numeric_limits<double>::infinity();

  int i3 = -1;
  for ( int step=mintest;  step<=maxtest; step+=teststep ) {
    i3 += 1;
    potential.push_back(step);
    tau.push_back(0.0);
    g_act.push_back(0.0);
  };

  // don't print repro message:
  noMessage();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );

  // clear plot and set Range
  string IUnit = trace( CurrentTrace[0] ).unit();
  string VUnit = trace( SpikeTrace[0]).unit();

  P.lock();
  P.resize( 2, 2, true );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]"  );
  P[1].setXLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]"  );
  P[1].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]" );
  P[1].setY2Label( "conductance [\u03BCS]" );
  P[1].setY2Tics( 0.0, 10.0 );

  P[0].clearData();
  P[1].clearData();
  P[1].setXRange(mintest,maxtest);
  P.unlock();

  // write stimulus:
  write( holdingsignal );
  sleep( pause );
  for ( int Count=0;
	( repeats <= 0 || Count < repeats ) && softStop() == 0;
	Count++ ) {

    int i = -1;
    for ( int step=mintest;  step<=maxtest; step+=teststep) {
      i += 1;
      Str s = "Holding potential <b>" + Str(holdingpotential, "%.1f") + " mV</b>";
      s += ", Testing potential <b>" + Str(step, "%.1f") + " mV</b>";
      s += ",  Loop <b>" + Str(Count + 1) + "</b>";
      message(s);

      // stimulus:
      OutData signal;

      signal.setTrace( PotentialOutput[0] );
      signal.pulseWave( duration, -1.0,  step, holdingpotential );
      signal.setIntensity( 0.0 );
      //Options opts;
//      Parameter &p1 = opts.addNumber( "step", step, "mV" );
      Parameter &p1 = signal.description().addNumber( "step", step, "mV" );
      Parameter &p2 = signal.description()["Intensity"];
//      Parameter &p3 = signal.description().addNumber( "IntensityOffset", holdingpotential, "mV" );

      signal.setMutable( p1 );
      signal.setMutable( p2 );
//      signal.setMutable( p3 );
      Options opts = signal.description();
      //signal.setDescription( opts );

      double mintime = -0.002;
      double maxtime = 0.01;

//      write( signal );
//      sleep( pause );
//      // get sample Data
//      double t0 = 0.0;
//      SampleDataF currenttrace(mintime, maxtime, trace(CurrentTrace[0]).stepsize(), t0 );
//      trace(CurrentTrace[0]).copy(signalTime(), currenttrace );
      double t0 = -0.002;

      SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, mintime, maxtime, t0 );
//      SampleDataD currenttrace(mintime, maxtime, trace(CurrentTrace[0]).stepsize(), 0.0 );
//      trace(CurrentTrace[0]).copy(signalTime(), currenttrace );
//      pn_trace = -pn_trace;
//      pn_trace -= pn_trace[0];
//      currenttrace -= currenttrace[0];


      if (interrupt()) {
        break;
      };

      double dt = currenttrace.stepsize();


      // IV
      double waittime = 0.0003;
      double absmax = 0.0;
      int index = 0;
      if ( -currenttrace.min(waittime, maxtime) >= currenttrace.max(waittime, maxtime) ){
        absmax = currenttrace.min(waittime, maxtime);
        index = currenttrace.minIndex(waittime, maxtime);
      }
      else {
        absmax = currenttrace.max(waittime, maxtime);
        index = currenttrace.maxIndex(waittime, maxtime);
      }
      IV[i] = absmax;

      cerr << "value = " << absmax <<  IUnit + ", at " << currenttrace.pos(index)*1000 << "ms\n";

      // fit tau to decaying activation curve
      int idx0 = index + fitdelay/dt;
      if ( idx0 > currenttrace.size()) {
        idx0 = index;
      };

      if ( I_min > absmax ) {
        I_min = absmax;
        V_min = step;
        t_min = currenttrace.pos(index);
        };

      std::vector<double> x(currenttrace.size() - idx0);
      std::vector<double> y(currenttrace.size() - idx0);

      for (int j=0; j<currenttrace.size()-idx0; j++ ) {
        x[j] = (j + idx0)*dt*1000 - 2;
        y[j] = currenttrace[j+idx0];
      };

      ArrayD param( 3, 1.0 );
      param[0] = 1.5*currenttrace[index];
      param[1] = -1.0;
      param[2] = currenttrace[currenttrace.size()-1];
      ArrayD error( currenttrace.size(), 1.0 );
      ArrayD uncertainty( 3, 0.0 );
      ArrayI paramfit( 3, 1 );
      double chisq = 0.0;

      int z = marquardtFit( x, y, error, expFuncDerivs, param, paramfit, uncertainty, chisq );
      if (z == 0) {
        tau[i] = -param[1];
      };

      std::vector <double> expfit(x.size());
      for (unsigned k=0; k<x.size(); k++) {
        expfit[k] = expFunc( x[k], param );
      };

      // plot
      P.lock();
      // trace
      P[0].plot( currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid );
      P[0].plotPoint( currenttrace.pos(index)*1000.0, Plot::First, absmax, Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
                      Plot::Magenta, Plot::Magenta );
      P[0].plot( x, expfit, Plot::Green, 2, Plot::Solid);


      // IV
      P[1].setYRange(P[0].yminRange(),P[0].ymaxRange());
      P[1].plotPoint( step, Plot::First, absmax, Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
                      Plot::Magenta, Plot::Magenta );

//      P[1].plot(currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid);
//      P[1].plot(pn_trace, 1000.0, Plot::Green, 2, Plot::Dotted);
//
//      P[1].plot(currenttrace - pn_trace, 1000.0, Plot::Red, 2, Plot::Dotted);

      P.draw();
      P.unlock();
    }

  double p_rev = pRev(IV);
  for ( unsigned i=0; i<potential.size(); i++) {
    g_act[i] = -IV[i]/(p_rev-potential[i]);
//    cerr << g_act[i] << "\n";
  };

//  g_act = -IV/(p_rev-potential);
  cerr << "reversal potential is " << p_rev << " mV" << "\n";

  P.lock();
  P[1].plotPoint( p_rev, Plot::First, 0.0, Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
                  Plot::Red, Plot::Red );
  P[1].plot(potential,g_act, Plot::Yellow, 1.0, Plot::Solid);
  P[1].back().setAxis( Plot::X1Y2 );
  P[1].setY2Range(0,max(g_act));
  P.unlock();


  }
  return Completed;
}

double Activation::pRev( const std::vector<double> &IV )
{
  // reversal potential
  // get options
  double minpot = number( "minrevpot" );
  double maxpot = number( "maxrevpot" );
  double mintest = number( "mintest" );
  double maxtest = number( "maxtest" );
  double teststep = number( "teststep" );
  int stepnum = (maxtest-mintest)/teststep+1;

  std::vector<double> potential(stepnum);
  int i = -1;
  for ( int step=mintest;  step<=maxtest; step+=teststep ) {
    i += 1;
    potential[i] = step;
  }

  // get IV in respective interval
  int idx1 = std::upper_bound( potential.begin(), potential.end(), minpot ) - potential.begin();
  int idx2 = std::upper_bound( potential.begin(), potential.end(), maxpot ) - potential.begin();
  std::vector<double> IV2(idx2-idx1);
  std::vector<double> potential2(idx2-idx1);
  int i2 = -1;
  for (int idx=idx1; idx<idx2; idx++) {
    i2++;
    IV2[i2] = IV[idx];
    potential2[i2] = potential[idx];
  };

  // find transition from negative to postitive
  int idx4 = std::upper_bound( IV2.begin(), IV2.end(), 0.0 ) - IV2.begin();
  int idx3 = idx4-1;

  // linear interpolation to find reversal potential (y=m*x+b)
  double m = (IV2[idx4]-IV2[idx3])/(potential2[idx4]-potential2[idx3]);
  double b = (IV2[idx4]*potential2[idx3] - IV2[idx3]*potential2[idx4])/(potential2[idx3]-potential2[idx4]);
  double p_rev = -b/m;
  return p_rev;
}



addRePro( Activation, voltageclamp );


}; /* namespace voltageclamp */

#include "moc_activation.cc"
