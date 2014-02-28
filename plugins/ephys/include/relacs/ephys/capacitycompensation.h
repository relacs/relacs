/*
  ephys/capacitycompensation.h
  Lissajous figures for checking the capacity compensation of the amplifier.

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

#ifndef _RELACS_EPHYS_CAPACITYCOMPENSATION_H_
#define _RELACS_EPHYS_CAPACITYCOMPENSATION_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace ephys {


/*!
\class CapacityCompensation
\brief [RePro] Lissajous figures for checking the capacity compensation of the amplifier.
\author Jan Benda
\version 1.0 (Feb 28, 2014)
\par Options
- \c amplitude=1: Amplitude of stimulus (\c number)
- \c duration=200ms: Duration of stimulus (\c number)
- \c frequency=100Hz: Frequency of sine-wave stimulus (\c number)
- \c skipcycles=10: Number of initial cycles to be skipped (\c integer)
- \c pause=100ms: Duration of pause between pulses (\c number)
*/


class CapacityCompensation : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  CapacityCompensation( void );
  virtual void preConfig( void );
  virtual int main( void );


protected:

  Plot P;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_CAPACITYCOMPENSATION_H_ */
