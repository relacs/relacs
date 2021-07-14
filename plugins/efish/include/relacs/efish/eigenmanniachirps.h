/*
  efish/eigenmanniachirps.h
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

#ifndef _RELACS_EFISH_EIGENMANNIACHIRPS_H_
#define _RELACS_EFISH_EIGENMANNIACHIRPS_H_ 1

#include <relacs/repro.h>
#include <relacs/efield/traces.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/eodtools.h>

using namespace relacs;

namespace efish {


/*!
\class EigenmanniaChirps
\brief [RePro] Repro for stimulation with the Eigenmannia-like chirps, i.e. incomplete and complete interruptions. To be used for chripchamber as well as ephys experiments.
\author Jan Grewe
\version 1.2 (July 13, 2021)
\par Options
- \c General settings
    - \c name="": Name of the Repro run, auto generated if empty (\c string)
    - \c eodmodel=sinewave: The model used for EOD generation, realistic creates an Eigenmannia like signal (\c string)
    - \c repeats=10: Number of stimulus repeats (\c integer)
    - \c pause=0.5s: Pause between repeats in seconds (\c number)
    - \c inverted=false: Inverts the signal to have the fish appearing oriented in a different way (\c boolean)
    - \c signaltype=all: Type of signal, whether it drives all, only ampullary, or only tuberous pathways (\c string)
    - \c filtercf=8Hz: Corner frequency of low pass filter for ampullary only stimuli. (\c number)
    - \c fakefish=0Hz: Fake a receiver fish with the given frequency, set to zero to use the real one (\c number)
- \c Beat parameter
    - \c duration=1.0s: Target duration of the stimulu, may be extended because we use full EOD cycles (\c number)
    - \c deltaf=20Hz: Difference frequency between reveiver and sender (\c number)
    - \c contrast=20%: Contrast of fish (\c number)
- \c Chirps
    - \c chirptype=TypeA: Type of chirp TypeA or TypeB (\c string)
    - \c chirpdelay=1.0s: Minimum time until first chirp occurs (\c number)
    - \c chirpduration=1EOD: Duration of the chirp in EODs (\c integer)
    - \c chirprate=1.0Hz: Rate with which the sender generate chirps (\c number)
*/

enum class EODModel{ SINE = 0, REALISTIC = 1 };
enum class SignalContent{ FULL = 0, NO_DC = 1, NO_AM = 2};
enum class ChirpType{ TYPE_A = 0, TYPE_B =1};

class EigenmanniaEOD {

public:
  EigenmanniaEOD();
  EigenmanniaEOD( const EODModel eod_model );
  EigenmanniaEOD( const EODModel eod_model, double sampling_interval );

  SampleDataD getEOD( const double eodf, double &duration, 
                      const double phase_shift = 0.0, bool full_cycles = true ) const;

  static constexpr double pi() { return std::acos(-1); }
  double phaseShift( const double eodf, double threshold = 0.0, bool rising_fank = true ) const;

private: 
  std::vector<double> harmonic_group_amplitudes = {1.0, 0.25, 0.0, 0.01};
  std::vector<double> harmonic_group_phases = {0.0, -0.48 * pi(), 0.0, 0.0};
  double sampling_interval;
  EODModel eod_model;
};


class EigenChirp {

public:
  EigenChirp( const double sampling_interval );
  EigenChirp( const double sampling_interval, const EODModel model );

  virtual SampleDataD getWaveform( const double eodf, const double chirp_duration, SignalContent signal ) const = 0;
  
  bool createStartStopSignals( double eodf, double &threshold, SampleDataD &start_eod, SampleDataD &stop_eod, SampleDataD &middle_eod ) const;

  void eodModel( EODModel model );
  EODModel eodModel( void ) const;
  
protected:
  double sampling_interval;
  EODModel eod_model;
};


class TypeAChirp : public EigenChirp {

public:
  TypeAChirp( const double sampling_interval );
  TypeAChirp( const double sampling_interval, const EODModel model );

  SampleDataD getWaveform( const double eodf, const double chirp_duration, SignalContent signal ) const;
  
};


class TypeBChirp : public EigenChirp {

public:
  TypeBChirp( const double sampling_interval );
  TypeBChirp( const double sampling_interval, const EODModel model );

  SampleDataD getWaveform( const double eodf, const double chirp_duration, SignalContent signal ) const;
  
};


class EigenmanniaChirps : 
        public RePro, 
        public ephys::Traces,
        public efield::Traces, 
        public efield::EODTools
{
  Q_OBJECT

public:

  EigenmanniaChirps( void );
  virtual int main( void );
private:
  string name;
  double stimulus_duration;
  double chirp_rate;
  double chirp_delay;
  double eodf;
  double fakefish;
  int chirp_duration;
  double deltaf;
  double sampling_interval;
  double receiver_amplitude;
  double stimulus_contrast;
  double pause;
  double filter_corner_freq;
  int repeats;
  bool inverted;
  
  EODModel eod_model_type;
  ChirpType chirp_type;
  SignalContent signal_content;

  OutData stimData;
  OutList outList;
  int readOptions( void );
  bool createStimulus( void );
  int fishEOD(double pause, double &rate, double &amplitude);

  string toString( SignalContent content );
  string toString( ChirpType type );
  string toString( EODModel );
};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_EIGENMANNIACHIRPS_H_ */
