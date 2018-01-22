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

Shape::Shape( Shape::ShapeType type, const string &name )
  : Type( type ),
    Name( name ),
    Trans( Point::Origin ),
    Trafo(),
    InvTrafo()
{
}


Shape::Shape( Shape::ShapeType type, const string &name,
	      const Matrix &trafo, const Point &trans  )
  : Type( type ),
    Name( name ),
    Trans( trans ),
    Trafo( trafo )
{
  InvTrafo = Trafo.inverse();
}


Shape::Shape( const Shape &s )
  : Type( s.Type ),
    Name( s.Name ),
    Trans( s.Trans ),
    Trafo( s.Trafo ),
    InvTrafo( s.InvTrafo )
{
}


Shape::~Shape( void)
{
}


void Shape::translateX( double x )
{
  Trans.x() += x;
}


void Shape::translateY( double y )
{
  Trans.y() += y;
}


void Shape::translateZ( double z )
{
  Trans.z() += z;
}


void Shape::translate( const Point &p )
{
  Trans += p;
}


void Shape::scaleX( double xscale )
{
  Trans.x() *= xscale;
  Trafo *= Matrix::scaleX( xscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scaleY( double yscale )
{
  Trans.y() *= yscale;
  Trafo *= Matrix::scaleY( yscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scaleZ( double zscale )
{
  Trans.z() *= zscale;
  Trafo *= Matrix::scaleZ( zscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scale( double xscale, double yscale, double zscale )
{
  Trans.x() *= xscale;
  Trans.y() *= yscale;
  Trans.z() *= zscale;
  Trafo *= Matrix::scale( xscale, yscale, zscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scale( const Point &scale )
{
  Trans *= scale;
  Trafo *= Matrix::scale( scale );
  InvTrafo = Trafo.inverse();
}


void Shape::scale( double scale )
{
  Trans *= scale;
  Trafo *= Matrix::scale( scale );
  InvTrafo = Trafo.inverse();
}


void Shape::rotateYaw( double yaw )
{
  Matrix m = Matrix::rotateYaw( yaw );
  Trans *= m;
  Trafo *= m;
  InvTrafo = Trafo.inverse();
}


void Shape::rotatePitch( double pitch )
{
  Matrix m = Matrix::rotatePitch( pitch );
  Trans *= m;
  Trafo *= m;
  InvTrafo = Trafo.inverse();
}


void Shape::rotateRoll( double roll )
{
  Matrix m = Matrix::rotateRoll( roll );
  Trans *= m;
  Trafo *= m;
  InvTrafo = Trafo.inverse();
}


void Shape::rotate( double yaw, double pitch, double roll )
{
  Matrix m = Matrix::rotate( yaw, pitch, roll );
  Trans *= m;
  Trafo *= m;
  InvTrafo = Trafo.inverse();
}


Point Shape::transform( const Point &p ) const
{
  return Trafo * p + Trans;
}


Point Shape::inverseTransform( const Point &p ) const
{
  return InvTrafo * ( p - Trans );
}


bool Shape::inside( const Point &p ) const
{
  return insideShape( inverseTransform( p ) );
}


bool Shape::below( const Point &p ) const
{
  return ( ( p >= boundingBoxMin() && p <= boundingBoxMax() ) ||
	   ( p.z() < boundingBoxMax().z() ) ); 
}


void Shape::intersectionPoints( const Point &pos1, const Point &pos2,
				Point &ip1, Point &ip2 ) const
{
  ip1 = Point::None;
  ip2 = Point::None;
  // XXX make sure pos1 != pos2 ???
  intersectionPointsShape( inverseTransform( pos1 ), inverseTransform( pos2 ), ip1, ip2 );
  if ( ! ip1.isNone() )
    ip1 = transform( ip1 );
  if ( ! ip2.isNone() )
    ip2 = transform( ip2 );
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
  : Shape( z ),
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


void Zone::push( const Shape &s, bool add )
{
  Shapes.push_back( s.copy() );
  Add.push_back( add );
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


const Shape *Zone::operator[]( const string &name ) const
{
  for ( auto si = Shapes.begin(); si != Shapes.end(); ++si ) {
    if ( (*si)->name() == name )
      return *si;
  }
  return 0;
}


Shape *Zone::operator[]( const string &name )
{
  for ( auto si = Shapes.begin(); si != Shapes.end(); ++si ) {
    if ( (*si)->name() == name )
      return *si;
  }
  return 0;
}


void Zone::clear( void )
{
  for ( auto si = Shapes.begin(); si != Shapes.end(); ++si )
    delete *si;
  Shapes.clear();
  Add.clear();
}


Point Zone::boundingBoxMin( void ) const
{
  Point p( trans() );
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
  Point p( trans() );
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


bool Zone::insideShape( const Point &p ) const
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


void Zone::intersectionPointsShape( const Point &pos1, const Point &pos2,
				    Point &ip1, Point &ip2 ) const
{
  /*
  bool ins = false;
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    Point ipp1;
    Point ipp2;
    (*si)->intersectionPoints( pos1, pos2, ipp1, ipp2 );
    // XXX do something 
  }
  */
}


ostream &Zone::print( ostream &str ) const
{
  str << "Zone \"" << name() << "\" at " << trans().toString() 
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
  : Shape( s )
{
}


Sphere::Sphere( const Point &center, double radius, const string &name )
  : Shape( Shape::Sphere, name )
{
  scale( radius );
  translate( center );
}


Shape *Sphere::copy( void ) const
{
  return new Sphere( *this );
}


double Sphere::radius( void ) const
{
  Point p0 = transform( Point::Origin );
  Point px = transform( Point( 1.0, 0.0, 0.0 ) );
  return p0.distance( px );
}


Point Sphere::boundingBoxMin( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( -1.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( -1.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( -1.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( -1.0, -1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, -1.0 ) ) );
  return min( pts );
}


Point Sphere::boundingBoxMax( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( -1.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( -1.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( -1.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( -1.0, -1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, -1.0 ) ) );
  return max( pts );
}


bool Sphere::insideShape( const Point &p ) const
{
  return ( Point::Origin.distance( p ) <= 1.0 );
}


void Sphere::intersectionPointsShape( const Point &pos1, const Point &pos2,
				      Point &ip1, Point &ip2 ) const
{
  // distance of straight line from pos1 to pos2 must equal the radius 1.0:
  double p1 = pos1.magnitude();
  Point dpos = pos2 - pos1;
  double dp = dpos.magnitude();
  double dpp = pos1.dot( dpos );
  double sq = ::sqrt( dpp*dpp - dp*dp*(p1*p1-1.0) );
  double a1 = -dpp-sq;
  double a2 = -dpp+sq;
  if ( a1 >= 0 && a1 <= 1.0 )
    ip1 = pos1 + a1 * dpos;
  if ( a2 >= 0 && a2 <= 1.0 )
    ip2 = pos1 + a2 * dpos;
}


ostream &Sphere::print( ostream &str ) const
{
  str << "Sphere \"" << name() << "\" at " << trans().toString()
      << " with radius " << Str( radius(), 0, 3, 'f' ) << '\n';
  return str;
}


//******************************************

Cylinder::Cylinder( void )
  : Shape( Shape::Cylinder, "cylinder" )
{
}


Cylinder::Cylinder( const Cylinder &c )
  : Shape( c )
{
}


Cylinder::Cylinder( const Point &anchor, double radius, double length, const string &name )
  : Shape( Shape::Cylinder, name )
{
  scale( length, radius, radius );
  translate( anchor );
}


Shape *Cylinder::copy( void ) const
{
  return new Cylinder( *this );
}


double Cylinder::radius( void ) const
{
  Point p0 = transform( Point::Origin );
  Point py = transform( Point( 0.0, 1.0, 0.0 ) );
  return p0.distance( py );
}


double Cylinder::length( void ) const
{
  Point p0 = transform( Point::Origin );
  Point px = transform( Point( 1.0, 0.0, 0.0 ) );
  return p0.distance( px );
}


Point Cylinder::boundingBoxMin( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( 0.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 0.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 0.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 0.0, -1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, -1.0 ) ) );
  return min( pts );
}


Point Cylinder::boundingBoxMax( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( 0.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 0.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 0.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 0.0, -1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, -1.0 ) ) );
  pts.push_back( transform( Point( 1.0, -1.0, -1.0 ) ) );
  return max( pts );
}


bool Cylinder::insideShape( const Point &p ) const
{
  Point px( 0.0, p.y(), p.z() );
  return ( Point::Origin.distance( px ) <= 1.0 &&
	   p.x() >= 0.0 && p.x() <= 1.0 );
}


void Cylinder::intersectionPointsShape( const Point &pos1, const Point &pos2,
					Point &ip1, Point &ip2 ) const
{
  // distance of straight line from pos1 to pos2 projected onto x = 0
  // must equal the radius 1.0:
  Point pp1( pos1 );
  pp1.x() = 0.0;
  Point pp2( pos2 );
  pp2.x() = 0.0;
  double p1 = pp1.magnitude();
  Point dpos = pp2 - pp1;
  double dp = dpos.magnitude();
  double dpp = pp1.dot( dpos );
  double sq = ::sqrt( dpp*dpp - dp*dp*(p1*p1-1.0) );
  double a1 = -dpp-sq;
  double a2 = -dpp+sq;
  // check if intersections are in the right x-range:
  if ( a1 >= 0 && a1 <= 1.0 ) {
    ip1 = pp1 + a1 * dpos;
    if ( ip1.x() < 0.0 || ip1.x() > 1.0 )
      ip1 = Point::None;
  }
  if ( a2 >= 0 && a2 <= 1.0 ) {
    ip2 = pp1 + a2 * dpos;
    if ( ip2.x() < 0.0 || ip2.x() > 1.0 )
      ip2 = Point::None;
  }
}


ostream &Cylinder::print( ostream &str ) const
{
  str << "Cylinder \"" << name() << "\" at " << trans().toString()
      << " with length " << Str( length(), 0, 3, 'f' )
      << " and radius " << Str( radius(), 0, 3, 'f' ) << '\n';
  return str;
}


//******************************************

Cuboid::Cuboid( void )
  : Shape( Shape::Cuboid, "cuboid" )
{
}


Cuboid::Cuboid( const Cuboid &c )
  : Shape( c )
{
}


Cuboid::Cuboid( const Point &anchor, 
		double length, double width, double height, const string &name )
  : Shape( Shape::Cuboid, name )
{
  scale( length, width, height );
  translate( anchor );
}


Cuboid::Cuboid( const Point &anchor, const Point &end, const string &name )
  : Shape( Shape::Cuboid, name )
{
  scale( end - anchor );
  translate( anchor );
}


Cuboid::Cuboid( const Point &anchor, const Point &px, const Point &py, const Point &pz,
		const string &name )
  : Shape( Shape::Cuboid, name )
{
  Point ppx = px - anchor;
  Point ppy = py - anchor;
  Point ppz = pz - anchor;
  scale( ppx.magnitude(), ppy.magnitude(), ppz.magnitude() );
  rotatePitch( asin( ppx.z()/ppx.magnitude() ) );
  rotateYaw( atan2( ppx.y(), ppx.x() ) );
  rotateRoll( asin( ppy.z()/ppy.magnitude() ) );
  translate( anchor );
}


Shape *Cuboid::copy( void ) const
{
  return new Cuboid( *this );
}


double Cuboid::length( void ) const
{
  Point p0 = transform( Point::Origin );
  Point px = transform( Point( 1.0, 0.0, 0.0 ) );
  return p0.distance( px );
}


double Cuboid::width( void ) const
{
  Point p0 = transform( Point::Origin );
  Point py = transform( Point( 0.0, 1.0, 0.0 ) );
  return p0.distance( py );
}


double Cuboid::height( void ) const
{
  Point p0 = transform( Point::Origin );
  Point pz = transform( Point( 0.0, 0.0, 1.0 ) );
  return p0.distance( pz );
}


Point Cuboid::boundingBoxMin( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( 0.0, 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( 1.0, 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, 1.0, 0.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, 0.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 0.0, 1.0 ) ) );
  pts.push_back( transform( Point( 0.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 1.0 ) ) );
  return min( pts );
}


Point Cuboid::boundingBoxMax( void ) const
{
  deque<Point> pts;
  pts.push_back( transform( Point( 0.0, 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( 1.0, 0.0, 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, 1.0, 0.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 0.0 ) ) );
  pts.push_back( transform( Point( 0.0, 0.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 0.0, 1.0 ) ) );
  pts.push_back( transform( Point( 0.0, 1.0, 1.0 ) ) );
  pts.push_back( transform( Point( 1.0, 1.0, 1.0 ) ) );
  return max( pts );
}


bool Cuboid::insideShape( const Point &p ) const
{
  return ( p >= Point::Origin && p <= Point::Ones );
}


void Cuboid::intersectionPointsShape( const Point &pos1, const Point &pos2,
				      Point &ip1, Point &ip2 ) const
{
  // XXX make sure dpos is not zero!
  Point dpos = pos2 - pos1;
  Point a0 = - pos1 / dpos;
  Point a1 = ( 1.0 - pos1) / dpos;
  for ( int k=0; k<3; k++ ) {
    if ( a0[k] >= 0 && a0[k] <= 1.0 ) {
      Point ip = pos1 + a0[k] * dpos;
      // XXX check whether ip1 was already set?
      if ( ip[(k+1)%3] >= 0.0 && ip[(k+1)%3] <= 1.0 )
	ip1 = ip;
    }
    if ( a1[k] >= 0 && a1[k] <= 1.0 ) {
      Point ip = pos1 + a1[k] * dpos;
      // XXX check whether ip1 was already set?
      if ( ip[(k+1)%3] >= 0.0 && ip[(k+1)%3] <= 1.0 )
	ip2 = ip;
    }
  }
}


ostream &Cuboid::print( ostream &str ) const
{
  str << "Cuboid \"" << name() << "\" at " << trans().toString()
      << " with size " << Str( length(), 0, 3, 'f' ) << ", "
      << Str( width(), 0, 3, 'f' ) << ", "
      << Str( height(), 0, 3, 'f' ) << '\n';
  return str;
}


}; /* namespace relacs */
