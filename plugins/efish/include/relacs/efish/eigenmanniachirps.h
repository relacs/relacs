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

class EigenmanniaChirps : 
        public RePro, 
        public efield::Traces, 
        public efield::EODTools
{
  Q_OBJECT

public:

  EigenmanniaChirps( void );
  virtual int main( void );
private:
  constexpr double pi() { return std::acos(-1); }
  double stimulus_samplerate = 20000;
  double duration;
  double chirp_rate;
  double eodf;
  double chrip_duration;
  double deltaf;
  EODModel eod_model;
  std::vector<double> harmonic_group_amplitudes = {1.0, 0.25, 0.0, 0.01};
  std::vector<double> harmonic_group_phases = {0.0, };
  OutData stimData;
  OutList outList;

  void createStimulus( void );
  SampleDataD eigenmanniaEOD( const double &freq, const double &duration, const double &samplerate );
  bool estimateEodFrequency( double &fisheodf );
};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_EIGENMANNIACHIRPS_H_ */
