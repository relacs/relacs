/*
  point.cc
  A point in 3D space.

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

#include <math.h>
#include <relacs/point.h>


namespace relacs {


Point::Point( void )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] = 0;
}


Point::Point( const Point &p )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] = p.Coords[k];
}


Point::Point( double x, double y, double z ) 
{
  Coords[0] = x;
  Coords[1] = y;
  Coords[2] = z;
}


void Point::set( double x, double y, double z )
{
  Coords[0] = x;
  Coords[1] = y;
  Coords[2] = z;
}


double Point::distance( const Point &p ) const
{
  double d = 0.0;
  for ( int k=0; k<Dim; k++ )
    d += ::pow( Coords[k] - p[k], 2.0 );
  return ::sqrt( d );
}


Point Point::abs_diff( const Point &p ) const
{
  Point q;
  for ( int k=0; k<Dim; k++ )
    q[k] = ::abs(Coords[k] - p[k]);
  return q;
}


Point Point::center( const Point &p ) const
{
  Point middle;
  for ( int k=0; k<Dim; k++ )
    middle[k] = 0.5*(Coords[k] + p[k]);
  return middle;
}


ostream &operator<<( ostream &str, const Point &p ) 
{
  str << "point: \t (x: " << p[0] << ", ";
  str << "y: " << p[1] << ", ";
  str << "z: " << p[2] << ")\n";
  return str;
}


}; /* namespace relacs */
