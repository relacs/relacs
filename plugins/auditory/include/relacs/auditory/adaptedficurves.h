/*
  auditory/adaptedficurves.h
  First adapts the neuron to a background sound intensity and then measures f-I curves.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_AUDITORY_ADAPTEDFICURVES_H_
#define _RELACS_AUDITORY_ADAPTEDFICURVES_H_ 1

#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace auditory {


/*!
\class AdaptedFICurves
\brief [RePro] First adapts the neuron to a background sound intensity and then measures f-I curves.
\author Jan Benda
\version 1.0 (Jan 24, 2011)
*/


class AdaptedFICurves : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

  AdaptedFICurves( void );
  virtual int main( void );


protected:

  void analyze( EventList &spikes, SampleDataD &rate,
		double delay, double duration, double pause,
		int count, double sstime, double onsettime,
		const ArrayD &times );

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_ADAPTEDFICURVES_H_ */
