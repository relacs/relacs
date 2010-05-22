/*
  attenuator.cc
  Interface for programming an attenuator.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#include <sstream>
#include <relacs/attenuator.h>
using namespace std;

namespace relacs {


Attenuator::Attenuator( void )
  : Device( AttenuatorType )
{
}


Attenuator::Attenuator( const string &deviceclass )
  : Device( deviceclass, AttenuatorType )
{
}


Attenuator::~Attenuator( void )
{
}


int Attenuator::mute( int di )
{
  double l = MuteAttenuationLevel;
  return attenuate( di, l );
}


int Attenuator::testMute( int di )
{
  double l = MuteAttenuationLevel;
  return testAttenuate( di, l );
}


void Attenuator::setInfo( void )
{
  Info.clear();
  Device::addInfo();
  Info.addInteger( "lines", lines() );
  Info.addNumber( "minimum attenuation level", minLevel(), "dB" );
  Info.addNumber( "maximum attenuation level", maxLevel(), "dB" );
}


}; /* namespace relacs */

