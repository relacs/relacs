/*
  photoreceptors/pulses.h
  Single and double light or current pulses.

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

#ifndef _RELACS_PHOTORECEPTORS_PULSES_H_
#define _RELACS_PHOTORECEPTORS_PULSES_H_ 1

#include <relacs/repro.h>
#include <relacs/base/traces.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace photoreceptors {


/*!
\class Pulses
\brief [RePro] Single and double light or current pulses.
\author Jan Grewe
\version 1.0 (May 07, 2015)
*/


  class Pulses : public RePro, public base::Traces, public ephys::Traces
{
  Q_OBJECT

public:

  Pulses( void );
  virtual int main( void );

};


}; /* namespace photoreceptors */

#endif /* ! _RELACS_PHOTORECEPTORS_PULSES_H_ */
