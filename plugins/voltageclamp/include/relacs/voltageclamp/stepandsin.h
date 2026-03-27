/*
  voltageclamp/stepandsin.h
  Voltage Steps (short activation protocol) followed by a sinusoidal stimulus

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

#ifndef _RELACS_VOLTAGECLAMP_STEPANDSIN_H_
#define _RELACS_VOLTAGECLAMP_STEPANDSIN_H_ 1


#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/voltageclamp/pnsubtraction.h>
#include <relacs/sampledata.h>

using namespace relacs;

namespace voltageclamp {


/*!
\class StepAndSin
\brief [RePro] Voltage Steps (short activation protocol) followed by a sinusoidal stimulus
\author Lukas Sonnenberg
\version 1.0 (Mai 08, 2020)
*/


class StepAndSin : public PNSubtraction
{
  Q_OBJECT

public:

  StepAndSin( void );
  virtual int main( void );

private:

  OutData Act();
  OutData Sins();

protected:

  MultiPlot P;

};


}; /* namespace voltageclamp */

#endif /* ! _RELACS_VOLTAGECLAMP_STEPANDSIN_H_ */
