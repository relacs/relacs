/*
  patchclampprojects/synapticresistance.h
  Artificial synaptic conductance with current pulses to measure resistance.

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

#ifndef _RELACS_PATCHCLAMPPROJECTS_SYNAPTICRESISTANCE_H_
#define _RELACS_PATCHCLAMPPROJECTS_SYNAPTICRESISTANCE_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclampprojects {


/*!
\class SynapticResistance
\brief [RePro] Artificial synaptic conductance with current pulses to measure resistance.
\author Jan Benda
\version 1.0 (Apr 05, 2014)

Periodically inject current pulses for monitoring the cell's resistance.
At the same time an artificial synaptic conductance can be injected via dynamic clamp.

This %RePro reproduces the dynamic clamp experiment of figure 1 in 
Andrew A. Sharp, Michael B. O’Neil, L. F. Abbott, and Eve Marder (1993):
Dynamic Clamp: Computer-Generated Conductances in Real Neurons. J Neurophysiol (96), 992-995.

\par Options
- \c Current pulses
    - \c pulseamplitude=-1nA: Amplitude of current pulses (\c number)
    - \c pulseduration=100ms: Duration of current pulses (\c number)
    - \c pulseperiod=1000ms: Period of current pulses (\c number)
    - \c before=1000ms: Time before onset of synaptic current (\c number)
    - \c after=1000ms: Time after end of synaptic current (\c number)
    - \c pause=1000ms: Duration of pause between outputs (\c number)
    - \c repeats=10: Repetitions of stimulus (\c integer)
- \c Synaptic current
    - \c enableconductance=false: Inject synaptic current (\c boolean)
    - \c conductancetau=1000ms: Synaptic time-constant (\c number)
    - \c conductanceamplitude=1nS: Amplitude of synaptic conductance (\c number)
    - \c reversalpotential=0mV: Reversal potential of synaptic conductance (\c number)
*/


class SynapticResistance : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  SynapticResistance( void );
  virtual void preConfig( void );
  virtual int main( void );
  void save( const SampleDataF &meantrace, const SampleDataF &stdevtrace,
	     const SampleDataF &meancurrent,
	     const string &vunit, const string &iunit );


private:

  Plot P;

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_SYNAPTICRESISTANCE_H_ */
