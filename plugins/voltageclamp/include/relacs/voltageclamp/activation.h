/*
  voltageclamp/activation.h
  Activation protocol

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

#ifndef _RELACS_VOLTAGECLAMP_ACTIVATION_H_
#define _RELACS_VOLTAGECLAMP_ACTIVATION_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace voltageclamp {


/*!
\class Activation
\brief [RePro] Activation protocol
\author Jan Benda & Lukas Sonnenberg
\version 1.0 (Aug 09, 2018)
*/


class Activation : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  Activation( void );
  virtual int main( void );


protected:

  Plot P;

};


}; /* namespace voltageclamp */

#endif /* ! _RELACS_VOLTAGECLAMP_ACTIVATION_H_ */
