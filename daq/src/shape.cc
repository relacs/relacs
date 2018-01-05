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

Shape::Shape( Shape::ShapeType type, const string &name )
  : Type( type ),
    Name( name ) 
{
}


Shape::~Shape( void)
{
}


bool Shape::inside( const Point &p ) const
{
  return ( p >= boundingBoxMin() && p <= boundingBoxMax() );
}


bool Shape::below( const Point &p ) const
{
  return ( ( p >= boundingBoxMin() && p <= boundingBoxMax() ) ||
	   ( p.z() < boundingBoxMax().z() ) ); 
}


//******************************************

Zone::Zone( void )
  : Shape( Shape::Zone, "Zone" )
{
  Shapes.clear();
  Add.clear();
}


Zone::Zone( const Zone &z )
  : Shape( z.type(), z.name() ),
    Add( z.Add )
{
  Shapes.clear();
  for ( auto si=z.Shapes.begin(); si != z.Shapes.end(); ++si )
    Shapes.push_back( (*si)->copy() );
}


Zone::Zone( const Shape &s, const string &name )
  : Shape( Shape::Zone, name )
{
  Shapes.clear();
  Add.clear();
  Shapes.push_back( s.copy() );
  Add.push_back( true );
}


Zone::Zone( const deque<Shape*> &s, const string &name )
  : Shape( Shape::Zone, name )
{
  Shapes.clear();
  Add.clear();
  for ( auto si=s.begin(); si != s.end(); ++si ) {
    Shapes.push_back( (*si)->copy() );
    Add.push_back( true );
  }
}


Zone::~Zone( void )
{
  for ( auto si=Shapes.begin(); si != Shapes.end(); ++si )
    delete *si;
  Shapes.clear();
  Add.clear();
}


Shape *Zone::copy( void ) const
{
  return new Zone( *this );
}


void Zone::add( const Shape &s )
{
  Shapes.push_back( s.copy() );
  Add.push_back( true );
}


void Zone::subtract( const Shape &s )
{
  Shapes.push_back( s.copy() );
  Add.push_back( false );
}


void Zone::operator+=( const Shape &s )
{
  Shapes.push_back( s.copy() );
  Add.push_back( true );
}


void Zone::operator-=( const Shape &s )
{
  Shapes.push_back( s.copy() );
  Add.push_back( false );
}


Zone Zone::operator+( const Shape &s ) const
{
  Zone z( *this );
  z += s;
  return z;
}


Zone Zone::operator-( const Shape &s ) const
{
  Zone z( *this );
  z -= s;
  return z;
}


Point Zone::boundingBoxMin( void ) const
{
  Point p( 0.0, 0.0, 0.0 );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      p = (*si)->boundingBoxMin();
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai )
      p = p.min( (*si)->boundingBoxMin() );
  }
  return p;
}


Point Zone::boundingBoxMax( void ) const
{
  Point p( 0.0, 0.0, 0.0 );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      p = (*si)->boundingBoxMax();
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai )
      p = p.max( (*si)->boundingBoxMax() );
  }
  return p;
}


bool Zone::inside( const Point &p ) const
{
  bool ins = false;
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( (*si)->inside( p ) )
      ins = *ai;
  }
  return ins;
}


bool Zone::below( const Point &p ) const
{
  bool bel = false;
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai && (*si)->below( p ) )
      bel = true;
  }
  return bel;
}


//******************************************

Sphere::Sphere( void )
  : Shape( Shape::Sphere, "Sphere" )
{
}


Sphere::Sphere( const Sphere &s )
  : Shape( s.type(), s.name() ),
    Center( s.Center ),
    Radius( s.Radius )
{
}


Sphere::Sphere( const Point &center, double radius, const string &name )
  : Shape( Shape::Sphere, name ),
    Center( center ),
    Radius( radius )
{
}


Shape *Sphere::copy( void ) const
{
  return new Sphere( *this );
}


Point Sphere::boundingBoxMin( void ) const
{
  return Center - Point( Radius, Radius, Radius );
}


Point Sphere::boundingBoxMax( void ) const
{
  return Center + Point( Radius, Radius, Radius );
}


bool Sphere::inside( const Point &p ) const
{
  return ( Center.distance( p ) <= Radius );
}


bool Sphere::below( const Point &p ) const
{
  Point a( Center );
  a.z() = 0.0;
  Point b( p );
  b.z() = 0.0;
  return ( inside( p ) || ( a.distance( b ) <= Radius && p.z() < Center.z() ) );
}


//******************************************

Cuboid::Cuboid( void )
  : Shape( Shape::Cuboid, "Cuboid" )
{
}


Cuboid::Cuboid( const Cuboid &c )
  : Shape( c.type(), c.name() ),
    Corner( c.Corner ),
    Size( c.Size )
{
}


Cuboid::Cuboid( const Point &start, 
		double length, double width, double height, const string &name )
  : Shape( Shape::Cuboid, name ),
    Corner( start )
{
  Size[0] = length;
  Size[1] = width;
  Size[2] = height;
}


Cuboid::Cuboid( const Point &start, const Point &end, const string &name )
  : Shape( Shape::Cuboid, name ),
    Corner( start ),
    Size( end - start )
{
}


Shape *Cuboid::copy( void ) const
{
  return new Cuboid( *this );
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
