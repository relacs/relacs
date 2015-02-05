/*
  dynclampaisim.cc
  Implementation of AnalogInput simulating an analog input device supporting analog ouput.

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

#include <relacs/relacsplugin.h>
#include <relacs/rtaicomedi/dynclampaisim.h>

namespace relacs {


DynClampAISim::DynClampAISim( void )
  : AISim()
{
}


DynClampAISim::~DynClampAISim( void )
{
}


int DynClampAISim::open( const string &device, const Options &opts )
{
  AISim::open( device, opts );
  setDeviceName( "Dynamic Clamp AI Simulation" );
  setInfo();
  return 0;
}


int DynClampAISim::open( Device &device, const Options &opts )
{
  AISim::open( device, opts );
  setDeviceName( "Dynamic Clamp AI Simulation" );
  setInfo();
  return 0;
}


addAnalogInput( DynClampAISim, rtaicomedi );

}; /* namespace relacs */

