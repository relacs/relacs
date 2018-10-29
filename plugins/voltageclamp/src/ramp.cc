/*
  voltageclamp/ramp.cc
  Ramp protocol

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

#include <relacs/str.h>
#include <relacs/voltageclamp/ramp.h>
#include <relacs/voltageclamp/pnsubtraction.h>
using namespace relacs;

namespace voltageclamp {


Ramp::Ramp( void )
  : PNSubtraction( "Ramp", "voltageclamp", "Lukas Sonnenberg", "1.0", "Aug 23, 2018" )
{
  // add some options:
  addNumber( "pause", "Duration of pause bewteen outputs", 0.4, 0.001, 1000.0, 0.001, "sec", "ms" );
  addInteger( "repeats", "Repetitions of stimulus", 1, 0, 10000, 1 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "holdingpotential", "Holding potential", -100.0, -200.0, 200.0, 1.0, "mV" );

  addNumber( "mintest", "Minimum of ramp", -100.0, -200.0, 200.0, 5.0, "mV");
  addNumber( "maxtest", "Maximum of ramp", 80.0, -200.0, 200.0, 5.0, "mV");

  addText("srange", "slopes of ramp", "800.0, 400.0, 200.0, 100.0, 50.0, 25.0, 12.5, 6.25").setUnit( "mV/s" );

  P.lock();
  P.setXLabel( "Time [ms]" );
  P.setYLabel( "Current [nA]" );

  P.unlock();
  setWidget( &P );
}



int Ramp::main( void )
{
  // get options:
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  double holdingpotential = number( "holdingpotential" );
  double mintest = number( "mintest" );
  double maxtest = number( "maxtest" );
  Str srange = allText( "srange" );

  /*
   RePro *rp = repro( "Activation" );
   if ( rp == 0 ) {
      cerr <<  "Activation RePro not found\n";
      }
  if ( rp->completeRuns() <= 0 )
      cerr <<  "Activation RePro not run yet\n";
   Activation *ac = dynamic_cast<Activation*>(rp);
   double tau = ac->TimeConstant;



   addText( "vrange", "Voltage range", "-100, -80" ).setUnit( "mV" );

   #include <str.h>
   Str rangestr = allText( "vrange" );
   vector<double> vrange;
   rangestr.range(vrange, ",", ":");

   for ( unsigned int k=0; k<vrange.size(); k++ )
   vrange[k]

   */

  std::vector <double> slopesteps;
  srange.range(slopesteps, ",", ":" );
  slopesteps = slopesteps/1000;

  // don't print repro message:
  noMessage();

  // holding potential:
  OutData holdingsignal;
  holdingsignal.setTrace( PotentialOutput[0] );
  holdingsignal.constWave( holdingpotential );
  holdingsignal.setIdent( "VC=" + Str( holdingpotential ) + "mV" );

  // clear plot
  P.clearData();
  P.lock();
  P.setXRange(-2., (maxtest - mintest)/min(slopesteps));
  P.unlock();

  // write stimulus:
  write( holdingsignal );
  sleep( pause );

  for ( int Count=0;
        ( repeats <= 0 || Count < repeats ) && softStop() == 0; Count++ ) {

    for ( unsigned i=0; i<slopesteps.size(); i++) {
      Str s = "Holding potential <b>" + Str(holdingpotential, "%.1f") + " mV</b>";
      s += ", slope step <b>" + Str(slopesteps[i]*1000, "%.2f") + " mV/s</b>";
      s += ",  Loop <b>" + Str(Count + 1) + "</b>";
      message(s);

      // stimulus
      double duration = (maxtest - mintest)/slopesteps[i]/1000;
      cerr << maxtest-mintest << ", " << slopesteps[i] << ", " << duration << "\n";
      OutData signal;
      signal.setTrace(PotentialOutput[0]);
      signal.rampWave(duration, -1.0, mintest, maxtest);

      OutData signal2;
      signal2.setTrace(PotentialOutput[0]);
      signal2.pulseWave(0.1, -1.0, maxtest, holdingpotential);

      signal.append(signal2);

      // nix options
      Options opts;
      Parameter &p1 = opts.addNumber( "slope", slopesteps[i], "mV*s^-1" );
      signal.setMutable( p1 );
      signal.setDescription( opts );


      double mintime = -0.002;
      double maxtime = duration;

//      write(signal);
//      sleep(pause);
//      // plot
//      SampleDataF currenttrace(-.002, duration, trace(CurrentTrace[0]).stepsize(), 0.0);
//      trace(CurrentTrace[0]).copy(signalTime(), currenttrace);
      double t0 = 0.0;
      SampleDataD currenttrace = PN_sub( signal, opts, holdingpotential, pause, mintime, maxtime, t0 );

      if (interrupt()) {
        break;
      };

      P.lock();
      P.plot(currenttrace, 1000.0, Plot::Yellow, 2, Plot::Solid);
      P.draw();
      P.unlock();
    }
  }
  return Completed;
}


addRePro( Ramp, voltageclamp );

}; /* namespace voltageclamp */

#include "moc_ramp.cc"
