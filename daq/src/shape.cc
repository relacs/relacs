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
  : StartPoint( c.StartPoint ),
    Length( c.Length ),
    Width( c.Width ),
    Height( c.Height )
{
}


Cuboid::Cuboid( const Point &start, 
		double length, double width, double height)
  : Shape()
{
  StartPoint = start;
  Length = length;
  Width = width;
  Height = height;
}


Cuboid::Cuboid( const Point &start, const Point &end )
  : Shape()
{
  StartPoint = start;
  Length = end.x() - start.x();
  Width = end.y() - start.y();
  Height = end.z() - start.z();
}


Cuboid::Cuboid( const Point &start, const Point &depth, const Point &length,
		const Point &width) 
  : Shape()
{
  StartPoint = start;
  Height = depth.z() - start.z();
  Width = width.y() - start.y();
  Length = length.x() - start.x();
}


bool Cuboid::point_inside( const Point &p ) const
{
  // the point is outside the x range of the object:
  if ( p.x() < StartPoint.x() || p.x() > StartPoint.x() + Length )
    return false;

  // the point is outside of the y range of the object:
  if ( p.y() < StartPoint.y() || p.y() > StartPoint.y() + Width )
    return false;

  // the point is outside of the z range of the object:
  if ( p.z() < StartPoint.z() || p.z() > StartPoint.z() + Height )
    return false;

  return true;
}

bool Cuboid::point_below( const Point &p ) const
{
  // the point is outside the x range of the object:
  if ( p.x() < StartPoint.x() || p.x() > StartPoint.x() + Length )
    return false;

  // the point is outside of the y range of the object:
  if ( p.y() < StartPoint.y() || p.y() > StartPoint.y() + Width )
    return false;

  // the point is above the lowest z of the object:
  if( p.z() < StartPoint.z() + Height )
    return false;

  return true;
}


bool Cuboid::point_safe( const Point &p ) const
{
  return ( point_inside( p ) || point_below( p ) );
}


bool Cuboid::extreme_x( double &biggest, double &lowest ) const
{
  biggest = StartPoint.x() + Length;
  lowest  = StartPoint.x();
  return true;
}

bool Cuboid::extreme_y( double &biggest, double &lowest ) const
{
  biggest = StartPoint.y() + Width;
  lowest  = StartPoint.y();
  return true;
}

bool Cuboid::extreme_z( double &biggest, double &lowest ) const
{
  biggest = StartPoint.z() + Height;
  lowest  = StartPoint.z();
  return true;
}


}; /* namespace relacs */
