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

#include <cmath>
#include <relacs/str.h>
#include <relacs/shape.h>

namespace relacs {


//******************************************

Shape::Shape( Shape::ShapeType type, const string &name, const Point &anchor )
  : Type( type ),
    Name( name ),
    Anchor( anchor ),
    Yaw( 0.0 ),
    Pitch( 0.0 ),
    Roll( 0.0 )
{
}


Shape::~Shape( void)
{
}


Point Shape::transform( const Point &p ) const
{
  Point pp( cos(Pitch)*cos(Yaw)*p.x() +
	    cos(Pitch)*sin(Yaw)*p.y() -
	    sin(Pitch)*p.z(),
	    (sin(Roll)*sin(Pitch)*cos(Yaw)-cos(Roll)*sin(Pitch))*p.x() +
	    (sin(Roll)*sin(Pitch)*sin(Yaw)+cos(Roll)*cos(Pitch))*p.y() +
	    sin(Roll)*cos(Pitch)*p.z(),
	    (cos(Roll)*sin(Pitch)*cos(Yaw)+sin(Roll)*sin(Pitch))*p.x() +
	    (cos(Roll)*sin(Pitch)*sin(Yaw)-sin(Roll)*cos(Pitch))*p.y() +
	    cos(Roll)*cos(Pitch)*p.z() );
  return pp + anchor();
}


Point Shape::inverseTransform( const Point &p ) const
{
  Point pp = p - anchor();
  return Point( cos(Pitch)*cos(Yaw)*pp.x() +
		(sin(Roll)*sin(Pitch)*cos(Yaw)-cos(Roll)*sin(Yaw))*pp.y() +
		(cos(Roll)*sin(Pitch)*cos(Yaw)+sin(Roll)*sin(Yaw))*pp.z(),
		cos(Pitch)*sin(Yaw)*pp.x() +
		(sin(Roll)*sin(Pitch)*sin(Yaw)+cos(Roll)*cos(Yaw))*pp.y() +
		(cos(Roll)*sin(Pitch)*sin(Yaw)-sin(Roll)*cos(Yaw))*pp.z(),
		-sin(Pitch)*pp.x() +
		sin(Roll)*cos(Pitch)*pp.y() +
		cos(Roll)*cos(Pitch)*pp.z() );
}


ostream &operator<<( ostream &str, const Shape &s )
{
  return s.print( str );
}


//******************************************

Zone::Zone( void )
  : Shape( Shape::Zone, "zone" )
{
  Shapes.clear();
  Add.clear();
}


Zone::Zone( const Zone &z )
  : Shape( z.type(), z.name(), z.anchor() ),
    Add( z.Add )
{
  setAngles( z.yaw(), z.pitch(), z.roll() );
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
  Point p( anchor() );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      p = transform( (*si)->boundingBoxMin() );
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai )
      p = p.min( transform( (*si)->boundingBoxMin() ) );
  }
  return p;
}


Point Zone::boundingBoxMax( void ) const
{
  Point p( anchor() );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      p = transform( (*si)->boundingBoxMax() );
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai )
      p = p.max( transform( (*si)->boundingBoxMax() ) );
  }
  return p;
}


bool Zone::inside( const Point &p ) const
{
  Point pp = inverseTransform( p );
  bool ins = false;
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( (*si)->inside( pp ) )
      ins = *ai;
  }
  return ins;
}


bool Zone::below( const Point &p ) const
{
  Point pp = inverseTransform( p );
  bool bel = false;
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai && (*si)->below( pp ) )
      bel = true;
  }
  return bel;
}


ostream &Zone::print( ostream &str ) const
{
  str << "Zone \"" << name() << "\" at " << anchor().toString() 
      << " consisting of\n";
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai )
    str << " " << (*ai ? '+' : '-') << " " << *si;
  return str;
}



//******************************************

Sphere::Sphere( void )
  : Shape( Shape::Sphere, "sphere" )
{
}


Sphere::Sphere( const Sphere &s )
  : Shape( s.type(), s.name(), s.anchor() ),
    Radius( s.Radius )
{
  setAngles( s.yaw(), s.pitch(), s.roll() );
}


Sphere::Sphere( const Point &center, double radius, const string &name )
  : Shape( Shape::Sphere, name, center ),
    Radius( radius )
{
}


Shape *Sphere::copy( void ) const
{
  return new Sphere( *this );
}


Point Sphere::boundingBoxMin( void ) const
{
  return anchor() - Point( Radius, Radius, Radius );
}


Point Sphere::boundingBoxMax( void ) const
{
  return anchor() + Point( Radius, Radius, Radius );
}


bool Sphere::inside( const Point &p ) const
{
  return ( anchor().distance( p ) <= Radius );
}


bool Sphere::below( const Point &p ) const
{
  Point a( anchor() );
  a.z() = 0.0;
  Point b( p );
  b.z() = 0.0;
  return ( inside( p ) || ( a.distance( b ) <= Radius && p.z() < anchor().z() ) );
}


ostream &Sphere::print( ostream &str ) const
{
  str << "Sphere \"" << name() << "\" at " << anchor().toString() 
      << " of radius " << Str( Radius, 0, 3, 'f' ) << '\n';
  return str;
}


//******************************************

Cuboid::Cuboid( void )
  : Shape( Shape::Cuboid, "cuboid" )
{
}


Cuboid::Cuboid( const Cuboid &c )
  : Shape( c.type(), c.name(), c.anchor() ),
    Size( c.Size )
{
  setAngles( c.yaw(), c.pitch(), c.roll() );
}


Cuboid::Cuboid( const Point &anchor, 
		double length, double width, double height, const string &name )
  : Shape( Shape::Cuboid, name, anchor ),
    Size( length, width, height )
{
}


Cuboid::Cuboid( const Point &anchor, const Point &end, const string &name )
  : Shape( Shape::Cuboid, name, anchor ),
    Size( end - anchor )
{
}


Cuboid::Cuboid( const Point &anchor, const Point &px, const Point &py, const Point &pz,
		const string &name )
  : Shape( Shape::Cuboid, name, anchor ),
    Size( 0.0, 0.0, 0.0 )
{
  Size[0] = anchor.distance( px );
  Size[1] = anchor.distance( py );
  Size[2] = anchor.distance( pz );
  setPitch( asin( px.z()/px.magnitude() ) );
  setYaw( atan2( px.y(), px.x() ) );
  setRoll( asin( py.z()/py.magnitude() ) );
}


Shape *Cuboid::copy( void ) const
{
  return new Cuboid( *this );
}


Point Cuboid::boundingBoxMin( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( 0.0, 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( Size.x(), 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, Size.y(), 0.0 ) ) );
  pts.push_back( transform( Point( Size.x(), Size.y(), 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, 0.0, Size.z() ) ) );
  pts.push_back( transform( Point( Size.x(), 0.0, Size.z() ) ) );
  pts.push_back( transform( Point( 0.0, Size.y(), Size.z() ) ) );
  pts.push_back( transform( Point( Size.x(), Size.y(), Size.z() ) ) );
  return min( pts );
}


Point Cuboid::boundingBoxMax( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( 0.0, 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( Size.x(), 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, Size.y(), 0.0 ) ) );
  pts.push_back( transform( Point( Size.x(), Size.y(), 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, 0.0, Size.z() ) ) );
  pts.push_back( transform( Point( Size.x(), 0.0, Size.z() ) ) );
  pts.push_back( transform( Point( 0.0, Size.y(), Size.z() ) ) );
  pts.push_back( transform( Point( Size.x(), Size.y(), Size.z() ) ) );
  return max( pts );
}


bool Cuboid::inside( const Point &p ) const
{
  Point pp = inverseTransform( p );
  return ( pp >= Point( 0.0, 0.0, 0.0 ) && pp <= Size );
}


bool Cuboid::below( const Point &p ) const
{
  return ( ( p >= boundingBoxMin() && p <= boundingBoxMax() ) ||
	   ( p.z() < boundingBoxMax().z() ) ); 
}


ostream &Cuboid::print( ostream &str ) const
{
  str << "Cuboid \"" << name() << "\" at " << anchor().toString() 
      << " of size " << Size.toString() << '\n';
  return str;
}


}; /* namespace relacs */
