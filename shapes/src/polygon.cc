/*
  polygon.cc
  A polygon in 3D space.

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

#include <cmath>
#include <iomanip>
#include <relacs/transform.h>
#include <relacs/shape.h>
#include <relacs/polygon.h>


namespace relacs {


Polygon::Polygon( void )
  : Normal( Point::None )
{
  Points.clear();
}


Polygon::Polygon( const Polygon &p )
  : Points( p.Points ),
    Normal( p.Normal )
{
}


Point Polygon::normal( void ) const
{ 
  if ( Normal.isNone() && Points.size() > 2 ) {
    Point n = (Points[1] - Points[0]).cross( Points.back() - Points.front() );
    return n.normalized();
  }
  return Normal;
}


Polygon &Polygon::assign( const Polygon &p )
{
  Points = p.Points;
  Normal = p.Normal;
  return *this;
}


Polygon &Polygon::operator=( const Polygon &p )
{
  Points = p.Points;
  Normal = p.Normal;
  return *this;
}


void Polygon::flipNormal( void )
{
  Normal *= -1.0;
}


void Polygon::apply( const Transform &trafo, const Transform &invtransptrafo )
{
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi )
    *pi *= trafo;
  Normal *= invtransptrafo;
}


Point Polygon::center( void ) const
{
  if ( Points.empty() )
    return Point::None;

  Point c = Point::Origin;
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi )
    c += *pi;
  return c/Points.size();
}


void Polygon::project( const Transform &trafo, vector<double> &x, vector<double> &y ) const
{
  x.clear();
  y.clear();
  x.reserve( Points.size() );
  y.reserve( Points.size() );
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    Point p = trafo * (*pi);
    p.homDivide();
    x.push_back( p.x() );
    y.push_back( p.y() );
  }
}


bool Polygon::inside( const Shape &shape ) const
{
  if ( Points.empty() )
    return false;

  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( ! shape.inside( *pi ) )
      return false;
  }
  return true;
}


bool Polygon::insideShape( const Shape &shape ) const
{
  if ( Points.empty() )
    return false;

  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( ! shape.insideShape( *pi ) )
      return false;
  }
  return true;
}


bool Polygon::outside( const Shape &shape ) const
{
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( shape.inside( *pi ) )
      return false;
  }
  return true;
}


bool Polygon::outsideShape( const Shape &shape ) const
{
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( shape.insideShape( *pi ) )
      return false;
  }
  return true;
}


ostream &operator<<( ostream &str, const Polygon &p ) 
{
  int k=0;
  for ( auto pi = p.Points.begin(); pi != p.Points.end(); ++pi )
    str << k++ << " " << *pi << '\n';
  return str;
}


}; /* namespace relacs */
