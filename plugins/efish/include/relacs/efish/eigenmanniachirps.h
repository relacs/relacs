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
\version 1.0 (May 11, 2020)
*/

enum class EODModel{ SINE = 0, REALISTIC = 1 };
enum class SignalContent{ FULL = 0, NO_DC = 1}; // , NO_AM = 2};
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


class TypeAChirp {

public:
  TypeAChirp( const double sampling_interval );
  TypeAChirp( const double sampling_interval, const EODModel model );

  SampleDataD getWaveform( const double eodf, const double chirp_duration, SignalContent signal ) const;
  
  void eodModel( EODModel model );
  EODModel eodModel( void ) const;
  
private:
  double sampling_interval;
  EODModel eod_model;

};


class TypeBChirp {

public:
  TypeBChirp( const double sampling_interval );
  TypeBChirp( const double sampling_interval, const EODModel model );

  SampleDataD getWaveform( const double eodf, const double chirp_duration, SignalContent signal ) const;
  
  void eodModel( EODModel model );
  EODModel eodModel( void ) const;
  
private:
  double sampling_interval;
  EODModel eod_model;
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
  int repeats;
  
  EODModel eod_model;
  ChirpType chirp_type;
  SignalContent signal_content;

  OutData stimData;
  OutList outList;
  int readOptions( void );
  bool createStimulus( void );
  int fishEOD(double pause, double &rate, double &amplitude);
  bool estimateEodFrequency( double &fisheodf );

  string toString( SignalContent content );
  string toString( ChirpType type );
};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_EIGENMANNIACHIRPS_H_ */
