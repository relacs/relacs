/*
  photoreceptors/pulses.cc
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

#include <relacs/photoreceptors/pulses.h>
using namespace relacs;

namespace photoreceptors {


Pulses::Pulses( void )
  : RePro( "Pulses", "photoreceptors", "Jan Grewe", "1.0", "May 07, 2015" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "duration", "Duration", 1.0, 0.0, 1.0, 0.1, "s", "s", "%.2f" ).setFlags( 1 );
  newSection( "LED" );
  addNumber( "led_intensity", "Light intensity", 1.0, 0.0, 10.0, 0.05, "V" ).setFlags( 1 );
  addNumber( "led_duration", "Light pulse duration", 0.25, 0, 1.0, 0.01, "s" ).setFlags( 1 );
  addNumber( "led_onset", "Start time of light pulse", 0.5, 0.0, 1.0, 0.01, "s" ).setFlags( 1 );
  newSection( "Current" );
  addNumber( "current_intensity", "Current", -0.25, -10.0, 0.0, 0.01, "nA" ).setFlags( 1 );
  addNumber( "current_duration", "Current pulse duration", 0.25, 0.0, 1.0, 0.01, "s" ).setFlags( 1 );
  addNumber( "current_onset", "Start time of current pulse", 0.0, 0.0, 1.0, 0.01, "s" ).setFlags( 1 );
  
  setConfigSelectMask( 1+8 );
  setDialogSelectMask( 1 ); 
}


int Pulses::main( void )
{
  noMessage();
  // get options:
  // double duration = number( "duration" );
  return Completed;
}


addRePro( Pulses, photoreceptors );

}; /* namespace photoreceptors */

#include "moc_pulses.cc"
