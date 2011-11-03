/*
  base/simplemodel.cc
  A toy model for testing.

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

#include <relacs/random.h>
#include <relacs/base/simplemodel.h>
using namespace relacs;

namespace base {


SimpleModel::SimpleModel( void )
  : Model( "SimpleModel", "base", "Jan Benda", "1.1", "Jan 31, 2008" )
{
  // define options:
  addSelection( "model", "The model", "Noise|Sine|Stimulus" );
  addNumber( "gain", "Gain", 1.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "freq", "Frequency", 1000.0, 0.0, 10000000.0, 10.0, "Hz", "Hz", "%.1f" ).setActivation( "model", "Sine", true );
}


SimpleModel::~SimpleModel( void )
{
}


void SimpleModel::main( void )
{
  // read out options:
  int model = index( "model" );
  double gain = number( "gain" );
  double freq = number( "freq" );

  // integrate:
  if ( model == 1 ) {
    while ( ! interrupt() ) {
      push( 0, gain * ::sin( 6.28318530717959*freq*time( 0 ) ) );
    }
  }
  else if ( model == 2 ) {
    while ( ! interrupt() ) {
      push( 0, gain*signal( time( 0 ) ) );
    }
  }
  else {
    while ( ! interrupt() ) {
      push( 0, gain*rnd.gaussian() );
    }
  }
}


addModel( SimpleModel, base );

}; /* namespace base */
