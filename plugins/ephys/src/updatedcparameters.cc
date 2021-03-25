/*
  ephys/updatedcparameters.cc
  short pulse to estimate membrane parameters, updates dynclamp parameters accordingly

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

#include <relacs/ephys/updatedcparameters.h>
using namespace relacs;

namespace ephys {


UpdateDCParameters::UpdateDCParameters( void )
  : RePro( "UpdateDCParameters", "ephys", "Lukas Sonnenberg", "1.0", "Mar 25, 2021" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
}


int UpdateDCParameters::main( void )
{
  // get options:
  // double duration = number( "duration" );
  return Completed;
}


addRePro( UpdateDCParameters, ephys );

}; /* namespace ephys */

#include "moc_updatedcparameters.cc"
