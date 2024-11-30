/*
  voltageclamp/summary.cc
  summary over other repros to plot steady-states and time constants

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
#include <relacs/voltageclamp/summary.h>
#include <relacs/voltageclamp/activation.h>
#include <relacs/voltageclamp/inactivation.h>
#include <relacs/voltageclamp/recovery.h>
#include <relacs/str.h>

using namespace relacs;

namespace voltageclamp {


Summary::Summary( void )
  : RePro( "Summary", "voltageclamp", "Lukas Sonnenberg", "1.0", "Sep 07, 2018" )
{
  // add some options:
  addBoolean("plotall", "Keep old Plots", true);
  addNumber("taumax", "maximum of time constant plot", 15.0, "ms");
//  addText("color_g_act","Color of activation curve", "Yellow")
//  addText("color_I_inact","Color of inactivation curve", "Green")
//  addText("color_tau_act","Color of inactivation time constant from activation protocol", "Magenta")
//  addText("color_tau_rec","Color of inactivation time constant from recovery protocol", "Cyan")
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );

  P.lock();
  P.resize( 2, 2, true );
  P[0].setXLabel( "Potential [mV]" );
  P[0].setYLabel( "Current [nA]" );
  P[0].setY2Label( "I/Imax");
  P[0].setY2Range(0,1);
  P[0].setY2Tics(0.0, 0.2);

  P[1].setXLabel( "Potential [mV]" );
  P[1].setYLabel( "\u03c4 [ms]");

  P.unlock();
  setWidget( &P );
}


int Summary::main( void )
{
  double taumax = number("taumax");

//  Inactivation inac();
  bool plotall = boolean( "plotall" );
  if ( !plotall ) {
    //clear plot
    P[0].clearData();
    P[1].clearData();
  };

  //get pointers to RePros
  RePro* rp_ac = repro( "Activation[voltageclamp]" );
  RePro* rp_inac = repro( "Inactivation[voltageclamp]" );
  RePro* rp_rec = repro( "Recovery[voltageclamp]" );

  //plot important curves if the RePro is already completed
  if ((rp_ac != 0) && (rp_ac->completeRuns() > 0)) {
    plotactivation( rp_ac );
  };

  if ((rp_inac != 0) && (rp_inac->completeRuns() > 0)) {
    plotinactivation( rp_inac );
  };

  if ((rp_rec != 0) && (rp_rec->completeRuns() > 0)) {
    plotrecovery( rp_rec );
  };

  while ( ! interrupt() && softStop() == 0 )
    sleep( 0.2 );

  P.lock();
  P[1].setYRange(0.0, taumax);
  P.draw();
  P.unlock();

  return Completed;
}

void Summary::plotactivation( RePro* rp_ac ) {
  Activation* ac = dynamic_cast<Activation*>( rp_ac );
  vector<double> g_act = ac->g_act;
  vector<double> tau_inac = ac->tau;
  vector<double> potential = ac->potential;

  P.lock();
  P[0].plot(potential, g_act, Plot::Yellow, 2.0, Plot::Solid);
  P[0].setYRange(0,max(g_act));

  P[1].plot(potential, tau_inac, Plot::Magenta, 2.0, Plot::Solid);
  P.draw();
  P.unlock();


}


void Summary::plotinactivation( RePro* rp_inac )
{
  Inactivation* inac = dynamic_cast<Inactivation*>( rp_inac );
  vector<double> I_inact = inac->inact;
  vector<double> potential = inac->potential;
  vector<double> I_I_min(I_inact.size());

  for (unsigned int i=0; i<I_inact.size(); i++) {
    I_I_min[i] = I_inact[i]/min(I_inact);
  };

  P.lock();
  P[0].plot(potential, I_I_min, Plot::Green, 2.0, Plot::Solid);
  P[0].back().setAxis( Plot::X1Y2 );
  P.draw();
  P.unlock();
}

void Summary::plotrecovery( RePro* rp_rec )
{
  Recovery* rec = dynamic_cast<Recovery*>( rp_rec );
  vector<double> tau_rec = rec->tau;
  vector<double> potential = rec->potential;

  P.lock();
  P[1].plot(potential, tau_rec, Plot::Cyan, 2.0, Plot::Solid);
  P.draw();
  P.unlock();
}


addRePro( Summary, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_summary.cc"
