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
  : Model( "EODModel", "efield", "Jan Benda", "1.2", "Oct 30, 2013" )
{
  // define options:
  addSelection( "eodtype", "EOD type", "Sine|None|Sine|Apteronotus|Eigenmannia" );
  addNumber( "localamplitude", "EOD Amplitude for local electrode", 1.0, 0.0, 100000.0, 1.0, "mV/cm", "mV/cm", "%.2f" );
  addNumber( "globalamplitude", "EOD Amplitude for global electrode", 1.0, 0.0, 100000.0, 1.0, "mV/cm", "mV/cm", "%.2f" );
  addNumber( "frequency", "Frequency", 1000.0, 0.0, 10000000.0, 10.0, "Hz", "Hz", "%.1f" );
  addNumber( "freqsd", "Standard deviation of frequency modulation", 10.0, 0.0, 1000.0, 2.0, "Hz" );
  addNumber( "freqtau", "Timescale of frequency modulation", 1000.0, 0.5, 100000.0, 0.5, "s" );
  addBoolean( "interrupt", "Add interruptions", false );
  addNumber( "interruptduration", "Duration of interruption", 0.1, 0.0, 100.0, 0.1, "s", "ms" ).setActivation( "interrupt", "true" );
  addNumber( "interruptamplitude", "Amplitude fraction of interruption", 0.0, 0.0, 1.0, 0.5, "1", "%" ).setActivation( "interrupt", "true" );;
  addNumber( "localstimulusgain", "Gain for additive stimulus component to local electrode", 0.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "globalstimulusgain", "Gain for additive stimulus component to global electrode", 0.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "stimulusgain", "Gain for stimulus recording channel", 1.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
}


void EODModel::main( void )
{
  // read out options:
  int eodtype = index( "eodtype" );
  double localamplitude = number( "localamplitude" );
  double globalamplitude = number( "globalamplitude" );
  double frequency = 2.0*M_PI*number( "frequency" );
  double freqsd = 2.0*M_PI*number( "freqsd" );
  double freqtau = number( "freqtau" );
  // OU normalisation factor for noise term:
  double freqfac = ::sqrt( 2.0*freqtau/deltat( 0 ) );
  bool interrupteod = boolean( "interrupt" );
  double interruptionduration = number( "interruptduration" );
  double interruptionamplitude = number( "interruptamplitude" );
  double nextinterruption = interrupteod ? time( 0 ) + 2.0 : -100.0;
  double localstimulusgain = number( "localstimulusgain" );
  double globalstimulusgain = number( "globalstimulusgain" );
  double stimulusgain = number( "stimulusgain" );

  // init:
  double amps[traces()];
  double stimgains[traces()];
  for ( int k=0; k<traces(); k++ ) {
    amps[k] = 0.0;
    stimgains[k] = 0.0;
  }
  if ( EODTrace >= 0 ) {
    amps[EODTrace] = globalamplitude;
    stimgains[EODTrace] = globalstimulusgain;
  }
  for ( int k=0; k<LocalEODTraces; k++ ) {
    amps[LocalEODTrace[k]] = localamplitude;
    stimgains[LocalEODTrace[k]] = localstimulusgain;
  }
  for ( int j=0; j<FishEODTanks; j++ ) {
    for ( int k=0; k<FishEODTraces[j]; k++ ) {
      amps[FishEODTrace[j][k]] = globalamplitude;
      stimgains[FishEODTrace[j][k]] = globalstimulusgain;
    }
  }
  if ( GlobalEFieldTrace >= 0 ) {
    amps[GlobalEFieldTrace] = 0.0;
    stimgains[GlobalEFieldTrace] = stimulusgain;
  }
  for ( int k=0; k<LocalEFieldTraces; k++ ) {
    amps[LocalEFieldTrace[k]] = 0.0;
    stimgains[LocalEFieldTrace[k]] = stimulusgain;
  }
  for ( int k=0; k<FishEFieldTraces; k++ ) {
    amps[FishEFieldTrace[k]] = 0.0;
    stimgains[FishEFieldTrace[k]] = stimulusgain;
  }

  // integrate:
  Random rand;
  double eodf = 0.0;
  double phase = 0.0;
  while ( ! interrupt() ) {
    // O-U noise for EOD frequency:
    eodf += ( -eodf + freqfac*rand.gaussian() ) * deltat( 0 ) / freqtau;
    // phase of EOD frequency:
    phase += (frequency + freqsd * eodf) * deltat( 0 );
    if ( phase > 2.0*M_PI )
      phase -= 2.0*M_PI;
    // amplitude modulations:
    double ampfac = 1.0;
    if ( time( 0 ) > nextinterruption &&
	 time( 0 ) < nextinterruption + interruptionduration ) {
      ampfac = interruptionamplitude;
      if ( time( 0 ) > nextinterruption + interruptionduration - 2.0*deltat( 0 ) )
	nextinterruption += 4.0;
    }
    double v = 0.0;
    if ( eodtype == 1 )
      v = ::sin( phase );
    else if ( eodtype == 2 )
      v = ( ::sin( phase ) - 0.5*::sin( 2.0*phase ) ) / 1.3;
    else if ( eodtype == 3 )
      v = ( ::sin( phase ) + 0.25*::sin( 2.0*phase + 0.5*M_PI ) ) + 0.25;
    for ( int k=0; k<traces(); k++ ) {
      if ( trace( k ).source() == 0 ) {
	double x = v * amps[k]*ampfac;
	x += stimgains[k] * signal( time( 0 ) );
	push( k, x );
      }
    }
  }
}


void EODModel::process( const OutData &source, OutData &dest )
{
  dest = source;
  double intensfac = 1.0;
  if ( source.level() != OutData::NoLevel ) {
    intensfac = ( ::pow( 10.0, -source.level()/20.0 ) );
    bool scaled = false;
    if ( source.trace() == GlobalAMEField ) {
      intensfac /= 0.3;
      scaled = true;
    }
    for ( int k=0; ! scaled && k<LocalEFields; k++ ) {
      if ( source.trace() == LocalEField[k] ) {
	intensfac /= ( 0.4 + k*0.1 );
	break;
      }
    }
    for ( int k=0; ! scaled && k<LocalAMEFields; k++ ) {
      if ( source.trace() == LocalAMEField[k] ) {
	intensfac /= ( 0.2 + k*0.15 );
	break;
      }
    }
    for ( int k=0; ! scaled && k<FishEFields; k++ ) {
      if ( source.trace() == FishEField[k] ) {
	intensfac /= ( 0.5 + k*0.05 );
	break;
      }
    }
    if ( ! scaled )
      intensfac /= 0.4;
  }
  dest *= intensfac;
}


addModel( EODModel, efield );

}; /* namespace efield */

#include "moc_eodmodel.cc"
