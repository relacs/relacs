/*
  efish/threefish.cc
  Beats of three fish

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

#include <relacs/efish/threefish.h>
using namespace relacs;

namespace efish {


ThreeFish::ThreeFish( void )
  : RePro( "ThreeFish", "efish", "Jan Benda", "1.0", "Jun 15, 2021" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
}


int ThreeFish::main( void )
{
  // get options:
  // double duration = number( "duration" );
  return Completed;
}


addRePro( ThreeFish, efish );

}; /* namespace efish */

#include "moc_threefish.cc"
