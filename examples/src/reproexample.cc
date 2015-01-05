/*
  examples/reproexample.cc
  A simple example showing how to program your own RePro.

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

#include <relacs/examples/reproexample.h>
using namespace relacs;

namespace examples {


ReProExample::ReProExample( void )
  : RePro( "ReProExample", "examples",
	   "Jan Benda", "1.0", "July 8, 2008" )
{
  // add some parameter as options:
  addNumber( "duration", "Duration", 0.1, 0.01, 1000.0, 0.02, "sec", "ms" );
}


ReProExample::~ReProExample( void )
{
}


int ReProExample::main( void )
{
  // get options:
  double duration = number( "duration" );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotContinuous( duration );

  sleep( duration );

  return Completed;
}


addRePro( ReProExample, examples );


}; /* namespace examples */


#include "moc_reproexample.cc"

