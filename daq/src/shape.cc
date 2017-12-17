/*
  shape.cc
  Shapes in 3D space.

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

#include <relacs/shape.h>

namespace relacs {


//******************************************

Shape::Shape( void )
{
}


//******************************************

Cuboid::Cuboid( void )
{
}


Cuboid::Cuboid( const Cuboid &c )
  : Corner( c.Corner ),
    Size( c.Size )
{
}


Cuboid::Cuboid( const Point &start, 
		double length, double width, double height)
  : Corner( start )
{
  Size[0] = length;
  Size[1] = width;
  Size[2] = height;
}


Cuboid::Cuboid( const Point &start, const Point &end )
  : Corner( start ),
    Size( end - start )
{
}


bool Cuboid::inside( const Point &p ) const
{
  // the point is outside the x range of the object:
  if ( p.x() < Corner.x() || p.x() > Corner.x() + Size.x() )
    return false;

  // the point is outside of the y range of the object:
  if ( p.y() < Corner.y() || p.y() > Corner.y() + Size.y() )
    return false;

  // the point is outside of the z range of the object:
  if ( p.z() < Corner.z() || p.z() > Corner.z() + Size.z() )
    return false;

  return true;
}

bool Cuboid::below( const Point &p ) const
{
  // the point is outside the x range of the object:
  if ( p.x() < Corner.x() || p.x() > Corner.x() + Size.x() )
    return false;

  // the point is outside of the y range of the object:
  if ( p.y() < Corner.y() || p.y() > Corner.y() + Size.y() )
    return false;

  // the point is above the lowest z of the object:
  if( p.z() < Corner.z() + Size.z() )
    return false;

  return true;
}


Point Cuboid::boundingBoxMin( void ) const
{
  return Corner;
}


Point Cuboid::boundingBoxMax( void ) const
{
  return Corner + Size;
}


}; /* namespace relacs */
