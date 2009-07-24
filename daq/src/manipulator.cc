/*
  manipulator.cc
  Virtual class for controlling a Manipulator. 

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/manipulator.h>

namespace relacs {


Manipulator::Manipulator( void ) 
  : Device( ManipulatorType )
{
}


Manipulator::Manipulator( const string &deviceclass )
  : Device( deviceclass, ManipulatorType )
{
}


Manipulator::~Manipulator( void )
{
}


int Manipulator::stepX( double x )
{
  return 0;
}


int Manipulator::stepY( double y )
{
  return 0;
}


int Manipulator::stepZ( double z )
{
  return 0;
}


int Manipulator::clearX( void )
{
  return 0;
}


int Manipulator::clearY( void )
{
  return 0;
}


int Manipulator::clearZ( void )
{
  return 0;
}


int Manipulator::clear( void )
{
  return clearX() + ( clearY() << 1 ) + ( clearZ() << 2 );
}


int Manipulator::homeX( void )
{
  return 0;
}


int Manipulator::homeY( void )
{
  return 0;
}


int Manipulator::homeZ( void )
{
  return 0;
}


int Manipulator::home( void )
{
  return homeX() + ( homeY() << 1 ) + ( homeZ() << 2 );
}


double Manipulator::posX( void ) const
{
  return 0.0;
}


double Manipulator::posY( void ) const
{
  return 0.0;
}


double Manipulator::posZ( void ) const
{
  return 0.0;
}


int Manipulator::setAmplX( double posampl, double negampl )
{
  return 0;
}


int Manipulator::setAmplY( double posampl, double negampl )
{
  return 0;
}


int Manipulator::setAmplZ( double posampl, double negampl )
{
  return 0;
}


double Manipulator::minAmplX( void ) const
{
  return 0.0;
}


double Manipulator::maxAmplX( void ) const
{
  return 0.0;
}


double Manipulator::minAmplY( void ) const
{
  return minAmplX();
}


double Manipulator::maxAmplY( void ) const
{
  return maxAmplX();
}


double Manipulator::minAmplZ( void ) const
{
  return minAmplX();
}


double Manipulator::maxAmplZ( void ) const
{
  return maxAmplX();
}


}; /* namespace relacs */

