/*
  ephys/calibratesyncpulse.h
  Sets the width of the pulse for synchronizing an amplifier.

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

#ifndef _RELACS_EPHYS_CALIBRATESYNCPULSE_H_
#define _RELACS_EPHYS_CALIBRATESYNCPULSE_H_ 1

#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace ephys {


/*!
\class CalibrateSyncPulse
\brief [RePro] Sets the width of the pulse for synchronizing an amplifier.
\author Jan Benda
\version 1.0 (Oct 07, 2015)
*/


class CalibrateSyncPulse : public RePro, public Traces
{
  Q_OBJECT

public:

  CalibrateSyncPulse( void );
  virtual void preConfig( void );
  virtual int main( void );


protected :

  MultiPlot P;
  string IUnit;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_CALIBRATESYNCPULSE_H_ */
