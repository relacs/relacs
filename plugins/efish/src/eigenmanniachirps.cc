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

//************************************************************************************
//********                       Eigenamannia EOD                              *******
EigenmanniaEOD::EigenmanniaEOD():
    sampling_interval(1./20000), eod_model(EODModel::REALISTIC)
{}

EigenmanniaEOD::EigenmanniaEOD( const EODModel eod_model ):
    sampling_interval(1./20000), eod_model(eod_model)
{}

EigenmanniaEOD::EigenmanniaEOD( const EODModel eod_model, const double sampling_interval ):
    sampling_interval(sampling_interval), eod_model(eod_model)
{}

SampleDataD EigenmanniaEOD::getEOD( const double eodf, double &duration, const double phase, 
                                    bool full_cycles ) const {
  if ( full_cycles ) {
    duration = ceil(duration * eodf) / eodf; 
  }
  SampleDataD eod( 0.0, duration, 0.0 );
  if ( eod_model == EODModel::REALISTIC ) {
    for ( size_t i = 0; i < harmonic_group_amplitudes.size(); ++i ) {
      eod += ( sin( 0.0, duration, this->sampling_interval, (i+1) * eodf, harmonic_group_phases[i] + (i+1) * phase ) * harmonic_group_amplitudes[i] );
    }
  } else {
    eod += sin(0.0, duration, this->sampling_interval, eodf, phase);
  }
  return eod;
}

double EigenmanniaEOD::phaseShift( const double eodf, double threshold, bool rising_flank ) const {
  if (eod_model == EODModel::SINE) {
    return 0.0;
  }
  double eod_period = 1./eodf;
  double duration = 3. * eod_period;
  SampleDataD eod = getEOD( eodf, duration ); 

  double eod_min = min(eod);
  double eod_max = max(eod);
  if ( threshold < eod_min ) {
    threshold = eod_min + 2 * std::numeric_limits<double>::epsilon();
  } else if (threshold > eod_max ) {
    threshold = eod_max - 2 * std::numeric_limits<double>::epsilon() ;
  }

  EventData crossings;
  if ( rising_flank ) {
    rising( eod, crossings, threshold );
  } else {
    falling( eod, crossings, threshold );
  }
  // eod.save("eod_stub.dat");
  double shift = 0.0;
  if ( crossings.size() > 0 ) {
    shift = 2 * pi() * (crossings[0] - sampling_interval)/eod_period ;
  } else {
    cerr << "EigenmanniaEOD: invalid threshold, could not figure out the phase shift!\n";
  }
  return shift;
}


//************************************************************************************
//********                      Eigen Chirp Base class                         *******
EigenChirp::EigenChirp( const double sampling_interval) :
 sampling_interval(sampling_interval), eod_model(EODModel::REALISTIC)
  {}

EigenChirp::EigenChirp( const double sampling_interval, const EODModel eod_model ) :
    sampling_interval( sampling_interval ), eod_model( eod_model )
  {}

void EigenChirp::eodModel( EODModel model ) {
  this->eod_model = model;
}

EODModel EigenChirp::eodModel( void ) const {
  return this->eod_model;
}

bool EigenChirp::createStartStopSignals( double eodf, double &threshold, SampleDataD &start_eod,
                                         SampleDataD &stop_eod, SampleDataD &middle_eod ) const{
  EigenmanniaEOD eod( this->eod_model, this->sampling_interval );
  double eod_period = 1./eodf;
  double duration = 3. * eod_period;
  SampleDataD eod_signal = eod.getEOD( eodf, duration );
  
  double eod_min = min(eod_signal);
  double eod_max = max(eod_signal);
  double ystep = 0.125 * (eod_max - eod_min) * this->sampling_interval / eod_period;
  if ( threshold < eod_min ) {
    threshold = eod_min + ystep;
  } else if (threshold > eod_max ) {
    threshold = eod_max - ystep;
  }

  EventData crossings;
  falling( eod_signal, crossings, threshold );
  
  eod_signal.copy( 0.0, crossings[0], start_eod );
  eod_signal.copy( crossings[0], eod_period, stop_eod );
  eod_signal.copy( crossings[0], crossings[0] + eod_period, middle_eod );
  
  threshold = start_eod.back();
  return true;
}

//************************************************************************************
//********                         Type A Chirp                                *******
TypeAChirp::TypeAChirp( const double sampling_interval) :
  EigenChirp( sampling_interval ){}
    
TypeAChirp::TypeAChirp( const double sampling_interval, const EODModel eod_model ):
    EigenChirp( sampling_interval, eod_model ){}

SampleDataD TypeAChirp::getWaveform( const double eodf, const double chirp_duration, SignalContent signal ) const {
  /*
  Type A waveform consists of a initial (partial) eod and an interruption that can be either at the bottom if signal content is Full or Ampullary, Or at the zero line if Signal content is DC ONLY 
  */
  double threshold = signal == SignalContent::FULL ? -10.0 : 0.0;

  SampleDataD start, stop, middle;
  createStartStopSignals(eodf, threshold, start, stop, middle);
  SampleDataD chirp_data( static_cast<int>( floor( chirp_duration / sampling_interval ) ), 0.0,
                          sampling_interval, threshold );
  SampleDataD result = start;
  result = result.append( chirp_data );
  result = result.append( stop ); 
  // result.save("typea_chrip.dat");
  return result;
}


//************************************************************************************
//********                         Type B Chirp                                *******
TypeBChirp::TypeBChirp( const double sampling_interval) :
    EigenChirp(sampling_interval) {}

TypeBChirp::TypeBChirp( const double sampling_interval, const EODModel eod_model ) :
    EigenChirp( sampling_interval, eod_model ) {}


SampleDataD TypeBChirp::getWaveform( const double eodf, const double chirp_duration, SignalContent signal ) const {
  double threshold = signal == SignalContent::FULL ? -10.0 : 0.0;
  
  double eod_period = 1./eodf;
  int interruption_count = static_cast<int>( ceil(chirp_duration/(2*eod_period)) );

  SampleDataD start, stop, middle;
  createStartStopSignals(eodf, threshold, start, stop, middle);
  SampleDataD chirp_data( static_cast<int>( floor( eod_period / sampling_interval ) ), 0.0,
                          sampling_interval, threshold );
  SampleDataD result = start;
  result = result.append( chirp_data );
  for ( int i =0; i < interruption_count; ++i) {
    result.append( middle );
    result.append( chirp_data );
  }
  result = result.append( stop ); 
  result.save("typeb_chrip.dat");
  return result;
}


//************************************************************************************
//********                     EigenmanniaChirps RePro                         *******
EigenmanniaChirps::EigenmanniaChirps( void )
  : RePro( "EigenmanniaChirps", "efish", "Jan Grewe", "1.0", "May 11, 2020" ) {
  // add some options:
  newSection( "General settings" );
  addText( "name" , "Prefix used to identify the repro run, auto-generated if empty", "" );
  addSelection( "eodmodel", "Model for EOD creation.", "sinewave|realistic" );
  addInteger( "repeats", "Number of repeated trials with the same conditions.", 10, 0, 100000, 2 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "pause", "Minimal pause between trials.", 0.5, 0.0, 1000., 0.01, "s" );
  addBoolean( "inverted", "Flips the foreign signal upside down i.e. the fish is on the other side.", false );
  addSelection( "signaltype", "Type of signal, whether it drives all, only ampullary, or only tuberous pathways", "all|tuberous only|ampullary only" );
  addNumber( "filtercf", "Corner frequency of low pass filter for ampullary only stimuli.", 8, 0.1, 256., 0.5, "Hz" ).setActivation( "signaltype", "ampullary only" );
  addNumber( "fakefish", "Fake a receiver fish with the given frequency, set to zero to use the real one", 0.0, 0., 1500., 10., "Hz" );
  
  newSection( "Beat parameter" );
  addNumber( "duration", "Total trial duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "deltaf", "Difference frequency", 20., 0.1, 1000., 1.0, "Hz" );
  addNumber( "contrast", "Strength of sender relative to receiver.", 0.2, 0.0, 1.0, 0.01, "", "%" );
  
  newSection( "Chirps" );
  addSelection( "chirptype", "Type of chirp", "TypeA|TypeB" );
  addNumber( "chirpdelay", "Minimum time until the first chrip", 1.0, 0.0, 1000.0, 0.01, "s");
  addInteger( "chirpduration", "Chirp duration in multiple of EOD period", 1, 0, 1000, 1, "EOD" );
  addNumber( "chirprate", "Rate at which the fake fish generates chirps.", 1.0, 0.001, 100.0, 0.1, "Hz" );
}

int EigenmanniaChirps::fishEOD(double pause, double &rate, double &amplitude)
{
  // EOD rate:
  if ( EODEvents >= 0 )
    rate = events( EODEvents ).frequency( currentTime() - pause, currentTime() );
  else if ( LocalEODEvents[0] >= 0 )
    rate = events( LocalEODEvents[0] ).frequency( currentTime() - pause, currentTime() );
  else {
    warning( "No EOD present or not enough EOD cycles recorded!" );
    return 1;
  }

  // EOD amplitude:
  double ampl = eodAmplitude( trace( LocalEODTrace[0] ),
			      currentTime() - pause, currentTime() );
  if ( ampl <= 1.0e-8 ) {
    warning( "No EOD amplitude on local EOD electrode!" );
    return 1;
  }
  else
    amplitude = ampl;
  return 0;
}

string EigenmanniaChirps::toString( ChirpType type ) {
  string str = (type == ChirpType::TYPE_A ? "Type A" : "Type B");
  return str;
}

string EigenmanniaChirps::toString( SignalContent content ) {
  string str;
  if ( content == SignalContent::FULL ) {
    str = "all channels";
  } else if ( content == SignalContent::NO_DC ) {
    str = "tuberous only";
  } else {
    str = "ampullary only";
  }
  return str;
}

string EigenmanniaChirps::toString( EODModel model ) {
  string str;
  if ( model == EODModel::REALISTIC ){
    str = "realistic";
  } else {
    str = "sinewave";
  }
  return str;
}

bool EigenmanniaChirps::createStimulus( void ) {
  stimData.clear();  
  string ident = name.size() == 0 ? "Eigenmannia chirps" : name;
  double sender_eodf = eodf + deltaf;
  EigenmanniaEOD eod_model( eod_model_type );
  SampleDataD eod_waveform;
  SampleDataD first_eod_waveform;
  SampleDataD chirp_waveform;
  int chirp_count = static_cast<int>( floor( stimulus_duration * chirp_rate ) );
  double ici = stimulus_duration / chirp_count;
  double chirp_duration_s = chirp_duration * 1./sender_eodf;

  if ( ici < chirp_duration_s ) {
    return false; 
  }
  if ( chirp_count * chirp_duration_s >= stimulus_duration ) {
    return false;
  }

  if ( chirp_type == ChirpType::TYPE_A ){
    TypeAChirp chirp( sampling_interval, eod_model_type);
    chirp_waveform = chirp.getWaveform( sender_eodf, chirp_duration_s, signal_content );
  } else {
    TypeBChirp chirp( sampling_interval, eod_model_type);
    chirp_waveform = chirp.getWaveform( sender_eodf, chirp_duration_s, signal_content );
  }
  
  first_eod_waveform = eod_model.getEOD( sender_eodf, chirp_delay, 0.0, true );
  eod_waveform = eod_model.getEOD( sender_eodf, ici, 0.0, true );

  SampleDataD temp = first_eod_waveform;
  std::vector<double> chirp_times(chirp_count, 0.0);
  for ( int i = 0; i < chirp_count; ++i ){
    chirp_times[i] = temp.size() * sampling_interval + chirp_waveform.size() * sampling_interval / 2;
    temp.append( chirp_waveform );
    temp.append( eod_waveform );
  }

  temp /= max({max( temp ), abs( min( temp ) )});
  if ( inverted )
    temp *= -1;
  
  if ( signal_content == SignalContent::NO_AM ) {
    // low-pass filter the stimulus for ampullary only stimuli
    double tau = 1./(2 * EigenmanniaEOD::pi() * filter_corner_freq );
    SampleDataD y(temp.size(), 0.0, temp.stepsize(), 0.0);
    for (int i = 0; i < temp.size()-1; ++i) {
      y[i+1] = y[i] + temp.stepsize()/tau * (temp[i+1] - y[i]);
    }
    stimData = y;
  } else { 
    stimData = temp;
  }
  
  if ( GlobalEField == -1) {
    warning("Did not find a valid GlobalEField trace, check output trace configuration!");
  }
  stimData.setTrace( GlobalEField );
  stimData.setSampleInterval( sampling_interval );
  stimData.description().addNumber( "real duration", stimData.size() * sampling_interval, "s" ).addFlags( OutData::Mutable );
  stimData.description().addNumber( "real chirp duration", chirp_waveform.size() * sampling_interval, "s" ).addFlags( OutData::Mutable );
  stimData.description().addNumber( "contrast", stimulus_contrast).addFlags( OutData::Mutable );
  stimData.description().addNumber( "chirp duration", chirp_duration, "EOD").addFlags( OutData::Mutable );
  stimData.description().addText( "signal content", toString(signal_content) ).addFlags( OutData::Mutable );
  stimData.description().addText( "eod model", toString(eod_model_type) ).addFlags( OutData::Mutable );
  stimData.description().addNumber( "inverted", int(inverted) ).addFlags( OutData::Mutable );
  stimData.description().addNumber( "deltaf", deltaf, "Hz" ).addFlags( OutData::Mutable );
  Parameter &chirptime_p = stimData.description().addNumber( "ChirpTimes", 0.0, "s" ).addFlags( OutData::Mutable );
  chirptime_p.setNumber( chirp_times[0] );
  for ( size_t k=1; k<chirp_times.size(); k++ )
    chirptime_p.addNumber( chirp_times[k] );
  
  stimData.setIdent( ident );
  stimData.save("stimulus.dat");
  outList.push( stimData );
  return true;
}

int EigenmanniaChirps::readOptions( void ) {
  name = text( "name" );
  stimulus_duration = number( "duration", 0.0, "s" );
  deltaf = number( "deltaf", 0.0, "Hz" );
  chirp_duration = integer( "chirpduration" );
  chirp_rate = number( "chirprate", 0.0, "Hz" );
  chirp_delay = number( "chirpdelay", 0.0, "s" );
  pause = number( "pause", 0.0, "s" );
  sampling_interval = 1./20000;
  stimulus_contrast = number( "contrast" );
  fakefish = number("fakefish");
  repeats = static_cast<int>(number( "repeats" ));
  inverted = boolean( "inverted" );
  filter_corner_freq = number( "filtercf" );
  string model_selection = text( "eodmodel" );

  if (model_selection == "sinewave") {
    eod_model_type = EODModel::SINE;
  } else {
    eod_model_type = EODModel::REALISTIC;
  }
  
  string chirp_selection = text( "chirptype" );
  if ( chirp_selection == "TypeA") {
    chirp_type = ChirpType::TYPE_A;
  } else {
    chirp_type = ChirpType::TYPE_B;
  }
  
  string chirp_location = text( "signaltype" );
  if ( chirp_location == "all" ) {
    signal_content = SignalContent::FULL;
  } else if ( chirp_location == "tuberous only" ) {
    signal_content = SignalContent::NO_DC;
  } else{
    signal_content = SignalContent::NO_AM;
  }
  
  return 0;
}


int EigenmanniaChirps::main( void ) {
  // get options:
  int ret = readOptions();
  if ( ret != 0 )
    return Failed;
  if ( fakefish > 0.0 ) {
    eodf = fakefish;
    receiver_amplitude = 1.0;
  } else {
    if ( fishEOD(pause, eodf, receiver_amplitude) )
      return Failed;
  }
  bool stimulus_ok = createStimulus();
  if (!stimulus_ok) {
    return Failed;
  }
  int eod_trace = LocalEODTrace[0] >= 0 ? LocalEODTrace[0] : EODTrace;
  if ( eod_trace == -1 ) {
    warning( "There is no EOD Trace! Cannot estimate fish amplitude and frequency! Expecting a LocalEOD or EOD trace, check input configuration. Exiting!" );
    return Failed;
  }
  
  receiver_amplitude = eodAmplitude( trace(eod_trace), currentTime() - 0.5, currentTime() );
  
  // stimulus intensity:
  double intensity = stimulus_contrast * receiver_amplitude;
  stimData.setIntensity( intensity );
  
  // output signal:
  for (int i = 0; i < repeats && softStop() == 0 ; ++i) {
    Str s = "<b>" + toString( chirp_type ) + "</b>"; 
    s += "  Contrast: <b>" + Str( 100.0 * stimulus_contrast, 0, 5, 'g' ) + "%</b>";
    s += "  Delta F: <b>" + Str( deltaf, 0, 1, 'f' ) + "Hz</b>";
    s += "  Stim F: <b>" + Str( eodf + deltaf, 0, 1, 'f' ) + "Hz</b>";
    s += "  SignalType: <b>" + toString( signal_content ) + "</b>";
    s += "  Chirp duration: <b>" + Str( chirp_duration ) + "EODs</b>";
    message( s );

    write( stimData );
    if ( !stimData.success() ) {
      string s = "Output of stimulus failed!<br>Error code is <b>" + stimData.errorText() + "</b>";
      warning( s, 4.0 );
      for ( int i = 0; i < outList.size(); ++i )
	      writeZero( outList[i].trace() );
      return Failed;
    }
    sleep( pause );
    
    if (i == repeats - 1) 
      return Completed;
  }

  return Completed;
}


addRePro( EigenmanniaChirps, efish );

}; /* namespace efish */

#include "moc_eigenmanniachirps.cc"
