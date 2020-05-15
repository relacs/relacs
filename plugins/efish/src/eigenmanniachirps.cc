/*
  efish/eigenmanniachirps.cc
  Repro for stimulation with the Eigenmannia-like chirps, i.e. incomplete and complete interruptions. To be used for chripchamber as well as ephys experiments.

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

#include <relacs/efish/eigenmanniachirps.h>
#include <numeric>

using namespace relacs;
using namespace std;
namespace efish {


EigenmanniaChirps::EigenmanniaChirps( void )
  : RePro( "EigenmanniaChirps", "efish", "Jan Grewe", "1.0", "May 11, 2020" ) {
  // add some options:
  newSection( "Beat parameter" );
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "deltaf", "Difference frequency", 20., 0.1, 1000., 1.0, "Hz" );
  addNumber( "fakefish", "Assume the presence of a fish with the given frequency, set to zero to use the real one", 0.0, 1., 1500., 10., "Hz" );
  addNumber( "contrast", "Beat contrast", 20., 0.01, 200.0, 1.0, "%" );
  addSelection( "eodmodel", "Model for EOD creation", "sinewave|harmonic group" );
  addInteger( "repeats", "Repeats", 10, 0, 100000, 2 ).setStyle( OptWidget::SpecialInfinite );

  newSection( "Chirps" );
  addSelection( "chirptype", "Type of chirp", "incomplete|complete" );
  addNumber( "chirpduration", "Duration of chirps, extended to integer multiples of the EOD period", 0.01, 0.001, 0.5, 0.001, "s", "ms" );
  addNumber( "chirprate", "Rate at which the fake fish generates chirps.", 1.0, 0.001, 100.0, 0.1, "Hz" );
  addSelection( "chirplocation", "Position in the EOD period in which the chrip should start, choose bottom to induce a DC shift", "bottom|center" );
   
  newSection( "EOD estimation" );
  addSelection( "intrace", "inputTrace" );
  addBoolean( "usepsd", "Use the power spectrum", true );
  addNumber( "mineodfreq", "Minimum expected EOD frequency", 100.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "maxeodfreq", "Maximum expected EOD frequency", 2000.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "eodfreqprec", "Precision of EOD frequency measurement", 1.0, 0.0, 100.0, 0.1, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "averagetime", "Time for computing EOD frequency", 2.0, 0.0, 100000.0, 1.0, "s" );  
}


bool EigenmanniaChirps::estimateEodFrequency( double &fisheodf ) {
  double averagetime = number( "averagetime" );
  fisheodf = number( "fakefish" );
  if ( fisheodf < 0.1 ) {
    if ( !boolean( "usepsd" ) ) {
      fisheodf = events( EODEvents ).frequency( currentTime() - averagetime, currentTime() );
      if ( EODEvents < 0 ) {
	      warning( "need EOD events of the EOD Trace." );
	      fisheodf = number( "fakefisheodf" );
	      return false;
      }
      return true;
    } else {
      double bigeod = 0.0;
      double bigeodf = 0.0; 
      double min_eodf = number( "mineodfreq" );
      double max_eodf = number( "maxeodfreq" );
      double eodf_prec = number( "eodfreqprec" );
      int intrace = index( "inputTrace" );
      if ( intrace == -1 ) {
        return false;
      }
      int nfft = 1;

      nfft = nextPowerOfTwo( (int)::ceil( 1.0/trace( intrace ).stepsize()/eodf_prec ) );
      eodf_prec = 1.0/trace( intrace ).stepsize()/nfft;
      if ( averagetime < 2.0/trace( intrace ).stepsize()/nfft ) {
	      averagetime = 2.0/trace( intrace ).stepsize()/nfft;
	      warning( "averagetime is too small for requested frequency resolution. I set it to " +
		    Str( averagetime ) + "s for now." );
      }

      SampleDataF data( 0.0, averagetime, trace( intrace ).stepsize() );
      trace( intrace ).copy( currentTime() - averagetime, data );
      SampleDataF power( nfft );
      rPSD( data, power );
      double threshold = power.max( min_eodf, max_eodf );
      EventData powerpeaks( 1000, true );
      peaks( power, powerpeaks, 0.2*threshold );
      double maxpower = 0.0;
      double maxfreq = 0.0;
      for ( int i=0; i<powerpeaks.size(); i++ ) {
	      if ( powerpeaks[i] >= min_eodf && powerpeaks[i] <= max_eodf ) {
	        if ( powerpeaks.eventSize( i ) > maxpower ) {
	          maxpower = powerpeaks.eventSize( i );
	          maxfreq = powerpeaks[i];
	        }
	      }
      }
      if ( bigeod < maxpower ) {
	      bigeod = maxpower;
	      bigeodf = maxfreq;
      }
      fisheodf = bigeodf;
      return true;
    }
  }
  return true;
}


SampleDataD EigenmanniaChirps::eigenmanniaEOD( const double &freq, const double &duration, const double &samplerate ) {
  double interval = 1./samplerate;
  SampleDataD eod( 0.0, duration, 0.0 );
  if ( eod_model == EODModel::REALISTIC ) {
    for ( size_t i = 0; i < harmonic_group_amplitudes.size(); ++i ) {
      eod += ( sin( 0.0, duration, interval, i * freq, harmonic_group_phases[i] ) * harmonic_group_amplitudes[i] );
    }
  } else {
    eod += sin(0.0, duration, interval, freq, 0.0);
  }
  return eod;
}


void EigenmanniaChirps::createStimulus( void ) {
  stimData.clear();
  duration = number( "duration" );
  deltaf = number( "deltaf" );
  string model_selection = text( "eodmodel" );
  if (model_selection == "sinewave") {
    eod_model = EODModel::SINE;
  } else {
    eod_model = EODModel::REALISTIC;
  }
  
  bool success = estimateEodFrequency( eodf );
  double fakefish_eodf = eodf + deltaf;

  stimData = eigenmanniaEOD( fakefish_eodf, duration, stimulus_samplerate );
  
  stimData.setTrace( GlobalEField );
  stimData.setSampleInterval( 1./stimulus_samplerate );
  // stimData.description().addText( "Type", "unrewarded" ).addFlags( OutData::Mutable );
  // stimData.description()["Frequency"].addFlags( OutData::Mutable );
  //stimData.setIdent( ident + "_unrewarded" );
  outList.push( stimData );
  
  //stimData.sineWave();
}


int EigenmanniaChirps::main( void ) {
  // get options:
  duration = number( "duration" );
  deltaf = number( "deltaf" );
  string model_selection = text( "eodmodel" );
  if (model_selection == "sinewave") {
    eod_model = EODModel::SINE;
  } else {
    eod_model = EODModel::REALISTIC;
  }
  return Completed;
}


addRePro( EigenmanniaChirps, efish );

}; /* namespace efish */

#include "moc_eigenmanniachirps.cc"
