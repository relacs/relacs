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
using namespace relacs;

namespace efish {


/*!
\class EigenmanniaChirps
\brief [RePro] Repro for stimulation with the Eigenmannia-like chirps, i.e. incomplete and complete interruptions. To be used for chripchamber as well as ephys experiments.
\author Jan Grewe
\version 1.0 (May 11, 2020)
*/


class EigenmanniaChirps : public RePro
{
  Q_OBJECT

public:

  EigenmanniaChirps( void );
  virtual int main( void );

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_EIGENMANNIACHIRPS_H_ */
