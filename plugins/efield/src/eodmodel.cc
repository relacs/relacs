/*
  efield/eodmodel.cc
  EOD with interruptions or chirps

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/random.h>
#include <relacs/efield/eodmodel.h>
using namespace relacs;

namespace efield {


EODModel::EODModel( void )
  : Model( "EODModel", "efield", "Jan Benda", "1.0", "Feb 03, 2013" )
{
  // define options:
  addSelection( "eodtype", "EOD type", "Sine|None|Sine|Apteronotus|Eigenmannia" );
  addNumber( "amplitude", "Amplitude", 1.0, 0.0, 100000.0, 1.0, "mV/cm", "mV/cm", "%.2f" );
  addNumber( "frequency", "Frequency", 1000.0, 0.0, 10000000.0, 10.0, "Hz", "Hz", "%.1f" );
  addNumber( "freqsd", "Standard deviation of frequency modulation", 10.0, 0.0, 1000.0, 2.0, "Hz" );
  addNumber( "freqtau", "Timescale of frequency modulation", 1000.0, 0.5, 100000.0, 0.5, "s" );
  addBoolean( "interrupt", "Add interruptions", false );
  addNumber( "interruptduration", "Duration of interruption", 0.1, 0.0, 100.0, 0.1, "s", "ms" ).setActivation( "interrupt", "true" );
  addNumber( "interruptamplitude", "Amplitude fraction of interruption", 0.0, 0.0, 1.0, 0.5, "1", "%" ).setActivation( "interrupt", "true" );;
  addNumber( "stimulusgain", "Gain of stimulus", 0.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
}


void EODModel::main( void )
{
  // read out options:
  int eodtype = index( "eodtype" );
  double amplitude = number( "amplitude" );
  double frequency = 2.0*M_PI*number( "frequency" );
  double freqsd = 2.0*M_PI*number( "freqsd" );
  double freqtau = number( "freqtau" );
  // OU normalisation factor for noise term:
  double freqfac = ::sqrt( 2.0*freqtau/deltat( 0 ) );
  bool interrupteod = boolean( "interrupt" );
  double interruptionduration = number( "interruptduration" );
  double interruptionamplitude = number( "interruptamplitude" );
  double nextinterruption = interrupteod ? time( 0 ) + 2.0 : -1.0;
  double stimulusgain = number( "stimulusgain" );

  // integrate:
  Random rand;
  double eodf = 0.0;
  double phase = 0.0;
  while ( ! interrupt() ) {
    // O-U noise for EOD frequency:
    eodf += ( -eodf + freqfac*rand.gaussian() ) * deltat( 0 ) / freqtau;
    // phase of EOD frequency:
    phase += (frequency + freqsd * eodf) * deltat( 0 );
    // amplitude modulations:
    double amp = amplitude;
    if ( time( 0 ) > nextinterruption &&
	 time( 0 ) < nextinterruption + interruptionduration ) {
      amp *= interruptionamplitude;
      if ( time( 0 ) > nextinterruption + interruptionduration - 2.0*deltat( 0 ) )
	nextinterruption += 4.0;
    }
    double v = 0.0;
    if ( eodtype == 1 )
      v = amp * ::sin( phase );
    else if ( eodtype == 2 )
      v = amp * ( ::sin( phase ) - 0.5*::sin( 2.0*phase ) );
    else if ( eodtype == 3 )
      v = amp * ( ::sin( phase ) + 0.25*::sin( 2.0*phase + 0.5*M_PI ) );
    v += stimulusgain * signal( time( 0 ) );
    push( 0, v );
  }
}


void EODModel::process( const OutData &source, OutData &dest )
{
  dest = source;
}


addModel( EODModel, efield );

}; /* namespace efield */

#include "moc_eodmodel.cc"
