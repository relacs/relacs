/*
  efield/eodmodel.h
  EOD with interruptions or chirps

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

#ifndef _RELACS_EFIELD_EODMODEL_H_
#define _RELACS_EFIELD_EODMODEL_H_ 1

#include <relacs/model.h>
#include <relacs/efield/traces.h>
using namespace relacs;

namespace efield {


/*!
\class EODModel
\brief [Model] EOD with interruptions or chirps
\author Jan Benda
\version 1.2 (Oct 30, 2013)
\par Options
- \c eodtype=Sine: EOD type (\c string)
- \c localamplitude=1mV/cm: EOD Amplitude for local electrode (\c number)
- \c globalamplitude=1mV/cm: EOD Amplitude for global electrode (\c number)
- \c frequency=1000Hz: Frequency (\c number)
- \c freqsd=10Hz: Standard deviation of frequency modulation (\c number)
- \c freqtau=1000s: Timescale of frequency modulation (\c number)
- \c interrupt=false: Add interruptions (\c boolean)
- \c interruptduration=100ms: Duration of interruption (\c number)
- \c interruptamplitude=0%: Amplitude fraction of interruption (\c number)
- \c localstimulusgain=1: Gain for additive stimulus component to local electrode (\c number)
- \c globalstimulusgain=0: Gain for additive stimulus component to global electrode (\c number)
- \c stimulusgain=1: Gain for stimulus recording channel (\c number)
*/


class EODModel : public Model, public Traces
{

public:

  EODModel( void );
  virtual void main( void );
  virtual void process( const OutData &source, OutData &dest );

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_EODMODEL_H_ */
