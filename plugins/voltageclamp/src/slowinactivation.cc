/*
  voltageclamp/slowinactivation.cc
  Slow inactivation of sodium channels with activation curve and time constant

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

#include <relacs/voltageclamp/slowinactivation.h>
#include <relacs/fitalgorithm.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/ephys/amplifiercontrol.h>
#include <math.h>

using namespace relacs;

namespace voltageclamp {


SlowInactivation::SlowInactivation( void )
  : PNSubtraction( "SlowInactivation", "voltageclamp", "Lukas Sonnenberg", "1.0", "Jul 25, 2020" )
{
  // add some options:
//  addBoolean( "autostim", "Automatic stimulus construction", true );
  addNumber( "mintest", "Minimum testing potential", -120.0, -200.0, 200.0, 1.0, "mV");
  addNumber( "maxtest", "Maximum testing potential", -10.0, -200.0, 200.0, 1.0, "mV");
  addNumber( "teststep", "Step testing potential", 5.0, 0.0, 200.0, 0.1, "mV");

  addNumber( "adaptationduration", "adaptation duration", 45.0, 0.001, 100000.0, 0.1, "s", "s" );
//  addNumber( "duration", "Stimulus duration", 45.0, 0.001, 100000.0, 0.1, "s", "s" );
  addText("trange", "Time steps", "0.0, 0.1, 0.3, 1.0, 3.0, 10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0").setUnit( "s" );
          // first time step must be 0.0, last time step is assumed to be duration
  addNumber( "pause", "Duration of pause bewteen outputs", 0.01, 0.001, 1000.0, 0.001, "s", "ms" );
  addNumber( "holdingpotential", "Holding potential", -120.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "adaptationpotential0", "adaptation potential0", -120.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "adaptationpotential1", "adaptation potential1", 10.0, -200.0, 200.0, 1.0, "mV" );

  addNumber( "sampleactpot", "activation potential", -10.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "sampledeactpot", "deactivation potential", -120.0, -200.0, 200.0, 1.0, "mV" );
  addNumber( "sampleacttime", "activation time", 0.003, 0, 0.1, 0.0001, "s", "ms" );
  addNumber( "sampledeacttime", "deactivation time", 0.017, 1.0, 0.0001, 1.0, "s", "ms" );

  addNumber( "switchpotential", "switch adaptation potential", -47.5, -200.0, 200.0, 0.1, "mV" );
  addInteger( "noverlap", "overlaping adaptation steps", 1, 0, 10, 1 );

  // plot
  setWidget( &P );
}


int SlowInactivation::main( void )
{
  // get options:
//  bool autostim = boolean( "autostim" );
  double mintest = number( "mintest" );
  double maxtest = number( "maxtest" );
  double teststep = number( "teststep" );

  double adaptationduration = number( "adaptationduration" );
//  double duration = number( "duration" );
  double holdingpotential = number( "holdingpotential" );
  double adaptationpotential0 = number( "adaptationpotential0" );
  double adaptationpotential1 = number( "adaptationpotential1" );

  double switchpotential = number( "switchpotential" );
  int noverlap = integer( "noverlap" );
  double pause = number( "pause" );

  double sampleactpot = number( "sampleactpot" );
  double sampledeactpot = number( "sampledeactpot" );
  double sampleacttime = number( "sampleacttime" );
  double sampledeacttime = number( "sampledeacttime" );
  double sampletime = sampleacttime + sampledeacttime;

  Str trange = allText( "trange" );
  std::vector<double> timesteps;
  trange.range(timesteps, ",", ":" );
//  int samplenum = (maxtest-mintest)/teststep+1;

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
  string VUnit = trace( SpikeTrace[0]).unit();

  P.lock();
  P.resize( 3, 3, true );
  P[0].setXLabel( "Time [s]" );
  P[0].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]"  );
  P[1].setXLabel( "Time [s]" );
  P[1].setYLabel( trace( CurrentTrace[0] ).ident() + " [" + IUnit + "]"  );
  P[2].setXLabel( trace( SpikeTrace[0] ).ident() + " [" + VUnit + "]"  );
  P[2].setYLabel( "Amplitude [" + IUnit + "]");

  P[0].clearData();
  P[1].clearData();
  P[2].clearData();

  P[0].setXRange( adaptationduration, timesteps[timesteps.size()-1]+sampletime + adaptationduration);
  P[1].setXRange( 0, timesteps[timesteps.size()-1]+sampletime );
  P[2].setXRange( mintest, maxtest );
  P[2].setYRange( -.02, 1.02 );
  P.unlock();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );

  // write holdingpotential:
  write( holdingsignal );
  sleep( pause );

  // stimulus preparations
  int N_adapt0 = noverlap; //ceil((switchpotential - mintest) / teststep + noverlap);
  int N_adapt1 = noverlap; //ceil((maxtest - switchpotential) / teststep + noverlap);
  int maxsteps = (maxtest - mintest) / teststep;
  int minidx1 = -noverlap;
  std::vector<double> potsteps(maxsteps);

  for ( int i=0; i<maxsteps; i++ ) {
    potsteps[i] = mintest + i*teststep;
    if ( potsteps[i] <= switchpotential ) {
      N_adapt0 += 1;
    }
    else {
      if ( minidx1 == -noverlap ) {
        minidx1 += i;
      }
      N_adapt1 += 1;
    }
  }

  int maxidx0 = N_adapt0 - 1;

  double maxpotential_adapt0 = mintest + teststep * N_adapt0;
//  double minpotential_adapt1 = maxtest - teststep * N_adapt1;

  std::vector<double> Iadapt0( N_adapt0 );
  std::vector<double> Iadapt1( N_adapt1 );

  double estimatedTime = (N_adapt0 + N_adapt1) * (adaptationduration + timesteps[timesteps.size()-1]) / 60;
  cerr << "Slow Inactivation stimulus will take approximately " + Str(estimatedTime, "%.1f") + "min to finish\n";

  OutData samplestim;
  samplestim.setTrace( PotentialOutput[0] );
  samplestim.constWave( sampledeacttime, -1.0, sampledeactpot );
  OutData samplestim1;
  samplestim1.setTrace( PotentialOutput[0] );
  samplestim1.constWave( sampleacttime, -1.0, sampleactpot );
  samplestim.append( samplestim1 );

  // stimulus0
//  int j = -1;
//  for ( double potstep=mintest; potstep<=maxpotential_adapt0; potstep+=teststep) {
//      j += 1;
  for ( int j=0; j<=maxidx0; j++ ) {
    double potstep = potsteps[j];
    Str s = "Holding potential <b>" + Str(holdingpotential, "%.1f") + " mV</b>";
    s += ", Testing potential <b>" + Str(potstep, "%.1f") + " mV</b>";
    s += ", Adaptation potential <b>" + Str(adaptationpotential0, "%.1f") + " mV </b>";
    message(s);
    cerr << potstep << "\n";

    OutData signal;
    signal.setTrace( PotentialOutput[0] );
    signal.constWave( adaptationduration, -1.0, adaptationpotential0 );

    signal.append( samplestim );

    for ( unsigned i=1; i<timesteps.size(); i++ ) {
      double timestep = timesteps[i] - timesteps[i-1] - sampletime;
      OutData signal1;
      signal1.setTrace( PotentialOutput[0] );
      signal1.constWave( timestep, -1.0, potstep );

      signal.append( signal1 );
      signal.append( samplestim );
    }

    // nix options
    Options opts;

    double t0 = 0.0;
    double mintime = 0.0;
    double maxtime = adaptationduration + timesteps[timesteps.size()-1] + sampletime;

    SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, mintime, maxtime, t0 );
    std::vector<double> minimas = getMinimas();
    Iadapt0[j] = minimas[ minimas.size() - 1 ];

    // Plot
    P.lock();
    P[0].plot( currenttrace, 1.0, Plot::Yellow, 2, Plot::Solid );
    for ( unsigned i=0; i<timesteps.size(); i++ ) {
      P[0].plotPoint( timesteps[i]*1.0 + adaptationduration + sampledeacttime, Plot::First, minimas[i], Plot::First, 0,
                      Plot::Circle, 5, Plot::Pixel, Plot::Red, Plot::Red );
      P[1].plotPoint( timesteps[i]*1.0, Plot::First, minimas[i], Plot::First, 0,
                      Plot::Circle, 5, Plot::Pixel, Plot::Yellow, Plot::Yellow );
    }
    P.draw();
    P.unlock();
    }

  // stimulus1
  int k = -1;
//  for ( double potstep=minpotential_adapt1; potstep<=maxtest; potstep+=teststep) {
  for ( int j=minidx1; j<maxsteps; j++ ) {
    k += 1;
    double potstep = potsteps[j];
    Str s = "Holding potential <b>" + Str(holdingpotential, "%.1f") + " mV</b>";
    s += ", Testing potential <b>" + Str(potstep, "%.1f") + " mV</b>";
    s += ", Adaptation potential <b>" + Str(adaptationpotential1, "%.1f") + " mV </b>";
    message(s);

    cerr << potstep << "\n";

    OutData signal;
    signal.setTrace( PotentialOutput[0] );
    signal.constWave( adaptationduration, -1.0, adaptationpotential1 );

    signal.append( samplestim );

    for ( unsigned i=1; i<timesteps.size(); i++ ) {
      double timestep = timesteps[i] - timesteps[i-1] - sampletime;
      OutData signal1;
      signal1.setTrace( PotentialOutput[0] );
      signal1.constWave( timestep, -1.0, potstep );

      signal.append( signal1 );
      signal.append( samplestim );
    }

    // nix options
    Options opts;

    double t0 = 0.0;
    double mintime = 0.0;
    double maxtime = adaptationduration + timesteps[timesteps.size()-1] + sampletime;
    SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, mintime, maxtime, t0 );
    std::vector<double> minimas = getMinimas();
    Iadapt1[k] = minimas[ minimas.size() - 1 ];

    // Plot
    P.lock();
    P[0].plot( currenttrace, 1.0, Plot::Green, 2, Plot::Solid );
    for ( unsigned i=0; i<timesteps.size(); i++ ) {
      P[0].plotPoint( timesteps[i]*1.0 + adaptationduration + sampledeacttime, Plot::First, minimas[i], Plot::First, 0,
                      Plot::Circle, 5, Plot::Pixel, Plot::Magenta, Plot::Magenta );
      P[1].plotPoint( timesteps[i]*1.0, Plot::First, minimas[i], Plot::First, 0,
                      Plot::Circle, 5, Plot::Pixel, Plot::Green, Plot::Green );
    }
    P.draw();
    P.unlock();
  }

  // write holdingpotential:
  write( holdingsignal );
  sleep( pause );

  //   Plot slowInactivation activation curve
  //  double absmin = 0.0;

  P.lock();
  double totalmin0 = min( Iadapt0 );
  double totalmin1 = min( Iadapt1 );
  double totalmin = 0.0;
  if (totalmin0 < totalmin1 ){
            totalmin = totalmin0;
    }
  else { totalmin = totalmin1; }

  int i0 = -1;
  for ( int j=0; j<=maxidx0; j++ ) {
    double potstep = potsteps[j];
    i0 += 1;
    P[2].plotPoint( potstep, Plot::First, Iadapt0[i0] / totalmin, Plot::First, 0,
                    Plot::Circle, 5, Plot::Pixel, Plot::Yellow, Plot::Yellow );
  }
  int i1 = -1;
  for ( int j=minidx1; j<=maxsteps; j++ ) {
    double potstep = potsteps[j];
    i1 += 1;
    P[2].plotPoint( potstep, Plot::First, Iadapt1[i1] / totalmin, Plot::First, 0,
                    Plot::Circle, 5, Plot::Pixel, Plot::Green, Plot::Green );
  }
  P.draw();
  P.unlock();

  return Completed;
}

std::vector<double> SlowInactivation::getMinimas( void )
{
  double adaptationduration = number( "adaptationduration" );
  double sampleacttime = number( "sampleacttime" );
  double sampledeacttime = number( "sampledeacttime" );
  double sampletime = sampleacttime + sampledeacttime;
  double waittime = 0.0003;

  Str trange = allText( "trange" );
  std::vector<double> timesteps;
  trange.range(timesteps, ",", ":" );

  std::vector<double> minimas(timesteps.size());

//  double time = adaptationduration;
  for ( unsigned i=1; i<timesteps.size(); i++ ) {
    double time = timesteps[i] + adaptationduration + sampledeacttime;
    SampleDataD currenttrace( time + waittime, time + sampleacttime - waittime, trace(CurrentTrace[0]).stepsize(), 0.0);
    trace(CurrentTrace[0]).copy(signalTime(), currenttrace);
    minimas[i] = min( currenttrace );
  }
  return minimas;
}



addRePro( SlowInactivation, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_slowinactivation.cc"
