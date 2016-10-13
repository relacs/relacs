/*
  ephys/capacitycompensation.h
  Lissajous figures for checking the capacity compensation of the amplifier.

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

#ifndef _RELACS_EPHYS_CAPACITYCOMPENSATION_H_
#define _RELACS_EPHYS_CAPACITYCOMPENSATION_H_ 1

#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace ephys {


/*!
\class CapacityCompensation
\brief [RePro] Sine waves for checking the capacity compensation of the amplifier inside the cell.
\author Jan Benda
\version 2.3 (Oct 13, 2017)
\par Options
- \c amplitude=1nA: Amplitude of stimulus (\c number)
- \c duration=10ms: Duration of stimulus (\c number)
- \c frequency=1000Hz: Frequency of sine-wave stimulus (\c number)
- \c showcycles=10: Number of cycles plotted (\c integer)
- \c pause=10ms: Duration of pause between pulses (\c number)
- \c average=10: Number of trials to be averaged (\c integer)
- \c skipspikes=true: Skip trials with detected spikes (\c boolean)
- \c dynamicrange=false: Dynamically adjust plot range (\c boolean)
- \c rate=0.01: Rate for adjusting plot ranges (\c number)
*/


class CapacityCompensation : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  CapacityCompensation( void );
  virtual void preConfig( void );
  virtual int main( void );


protected:

  MultiPlot P;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_CAPACITYCOMPENSATION_H_ */
