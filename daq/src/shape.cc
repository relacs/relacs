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


void Shape::rotateZ( double angle )
{
  Matrix m = Matrix::rotateZ( angle );
  Trans *= m;
  Trafo *= m;
  InvTrafo = Trafo.inverse();
}


void Shape::rotateY( double angle )
{
  Matrix m = Matrix::rotateY( angle );
  Trans *= m;
  Trafo *= m;
  InvTrafo = Trafo.inverse();
}


void Shape::rotateX( double angle )
{
  Matrix m = Matrix::rotateX( angle );
  Trans *= m;
  Trafo *= m;
  InvTrafo = Trafo.inverse();
}


void Shape::rotate( double anglez, double angley, double anglex )
{
  Matrix m = Matrix::rotate( anglez, angley, anglex );
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


Point Shape::boundingBoxMin( void ) const
{
  return boundingBoxMin( Trafo, Trans );
}


Point Shape::boundingBoxMax( void ) const
{
  return boundingBoxMax( Trafo, Trans );
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
  // check whether the two points are the same:
  Point dpos = pos2 - pos1;
  if ( dpos.magnitude() < 1e-16 )
    return;
  // find intersections:
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


Point Zone::boundingBoxMin( const Matrix &trafo, const Point &trans ) const
{
  Point p( Point::None );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Matrix m = trafo * (*si)->trafo();
      Point t = trafo * (*si)->trans() + trans;
      p = (*si)->boundingBoxMin( m, t );
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Matrix m = trafo * (*si)->trafo();
      Point t = trafo * (*si)->trans() + trans;
      p = p.min( (*si)->boundingBoxMin( m, t ) );
    }
  }
  return p;
}


Point Zone::boundingBoxMax( const Matrix &trafo, const Point &trans ) const
{
  Point p( Point::None );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Matrix m = trafo * (*si)->trafo();
      Point t = trafo * (*si)->trans() + trans;
      p = (*si)->boundingBoxMax( m, t );
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Matrix m = trafo * (*si)->trafo();
      Point t = trafo * (*si)->trans() + trans;
      p = p.max( (*si)->boundingBoxMax( m, t ) );
    }
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
  ip1 = Point::None;
  ip2 = Point::None;
  Point dpos = pos2 - pos1;
  double dpmsq = dpos.magnitude();
  dpmsq *= dpmsq;
  double a1 = NAN;
  double a2 = NAN;
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    Point ipp1;
    Point ipp2;
    (*si)->intersectionPoints( pos1, pos2, ipp1, ipp2 );
    // find position on path:
    if ( ! ipp1.isNone() && ! ipp2.isNone() ) {
      double aa1 = dpos.dot( ipp1 - pos1 )/dpmsq;
      double aa2 = dpos.dot( ipp2 - pos1 )/dpmsq;
      if ( *ai ) {
	// expand intersection path:
	if ( ::isnan( a1 ) || aa1 < a1 ) {
	  a1 = aa1;
	  ip1 = ipp1;
	}
	if ( ::isnan( a2 ) || aa2 > a2 ) {
	  a2 = aa2;
	  ip2 = ipp2;
	}
      }
      else if ( ! ::isnan( a1 ) ) {
	// shrink intersection path:
	if ( aa1 <= a1 && aa2 >= a2 ) {
	  a1 = NAN;
	  a2 = NAN;
	  ip1 = Point::None;
	  ip2 = Point::None;
	}
	else if ( aa1 > a1 && aa2 > a2 ) {
	  a1 = aa1;
	  ip1 = ipp1;
	}
	else if ( aa1 < a1 && aa2 < a2 ) {
	  a2 = aa2;
	  ip2 = ipp2;
	}
      }
    }
  }
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
  Point px = transform( Point::UnitX );
  return p0.distance( px );
}


Point Sphere::boundingBoxMin( const Matrix &trafo, const Point &trans ) const
{
  /*
  Matrix itrafo = trafo.inverse();
  Point px = itrafo * Point::UnitX;
  px.normalize();
  Point py = itrafo * Point::UnitY;
  py.normalize();
  Point pz = itrafo * Point::UnitZ;
  pz.normalize();
  */
  Point px = Point::UnitX;
  Point py = Point::UnitY;
  Point pz = Point::UnitZ;
  deque<Point> pts;
  pts.push_back( trafo*( px + py + pz ) );
  pts.push_back( trafo*( px - py + pz ) );
  pts.push_back( trafo*( px + py - pz ) );
  pts.push_back( trafo*( px - py - pz ) );
  pts.push_back( trafo*( - px + py + pz ) );
  pts.push_back( trafo*( - px - py + pz ) );
  pts.push_back( trafo*( - px + py - pz ) );
  pts.push_back( trafo*( - px - py - pz ) );
  return min( pts ) + trans;
}


Point Sphere::boundingBoxMax( const Matrix &trafo, const Point &trans ) const
{
  /*
  Matrix itrafo = trafo.inverse();
  Point px = itrafo * Point::UnitX;
  px.normalize();
  Point py = itrafo * Point::UnitY;
  py.normalize();
  Point pz = itrafo * Point::UnitZ;
  pz.normalize();
  */
  Point px = Point::UnitX;
  Point py = Point::UnitY;
  Point pz = Point::UnitZ;
  deque<Point> pts;
  pts.push_back( trafo*( px + py + pz ) );
  pts.push_back( trafo*( px - py + pz ) );
  pts.push_back( trafo*( px + py - pz ) );
  pts.push_back( trafo*( px - py - pz ) );
  pts.push_back( trafo*( - px + py + pz ) );
  pts.push_back( trafo*( - px - py + pz ) );
  pts.push_back( trafo*( - px + py - pz ) );
  pts.push_back( trafo*( - px - py - pz ) );
  return max( pts ) + trans;
}


bool Sphere::insideShape( const Point &p ) const
{
  return ( Point::Origin.distance( p ) <= 1.0 );
}


void Sphere::intersectionPointsShape( const Point &pos1, const Point &pos2,
				      Point &ip1, Point &ip2 ) const
{
  ip1 = Point::None;
  ip2 = Point::None;
  // distance of straight line from pos1 to pos2 must equal the radius 1.0:
  double p1 = pos1.dot( pos1 );
  Point dpos = pos2 - pos1;
  double dp = dpos.dot( dpos );
  double dpp = pos1.dot( dpos );
  double discr = dpp*dpp - dp*(p1-1.0);
  if ( discr <= 0.0 )
    return;
  double sq = ::sqrt( discr );
  double a1 = (-dpp-sq)/dp;
  double a2 = (-dpp+sq)/dp;
  // check whether both pos1 and pos2 are not outside and at the same side (no intersection):
  if ( a1 <= 1.0 && a2 >= 0.0 ) {
    ip1 = pos1 + a1 * dpos;
    ip2 = pos1 + a2 * dpos;
  }
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
  Point py = transform( Point::UnitY );
  return p0.distance( py );
}


double Cylinder::length( void ) const
{
  Point p0 = transform( Point::Origin );
  Point px = transform( Point::UnitX );
  return p0.distance( px );
}


Point Cylinder::boundingBoxMin( const Matrix &trafo, const Point &trans ) const
{
  /*
  // transform base circle to world coordinates:
  Point py = trafo*Point::UnitY+trans;
  Point pz = trafo*Point::UnitZ+trans;
  // normal vector to plane of base circle:
  Point nv = py.cross( pz ).normalized();
  // vector in x-y plane (perpendicular to nv and z):
  Point qy = nv.cross( Point::UnitZ ).normalized();
  // vector perpendicular to nv and qy:
  Point qz = nv.cross( qy ).normalized();
  // transform qy and qz back to shape coordinates:
  Matrix itrafo = trafo.inverse();
  Point qqy = itrafo * qy;
  Point qqz = itrafo * qz;
  */
  Point qqy = Point::UnitY;
  Point qqz = Point::UnitZ;
  deque<Point> pts;
  pts.push_back( trafo*Point( 0.0, qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 0.0, -qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 0.0, qqy.y(), -qqz.z() ) );
  pts.push_back( trafo*Point( 0.0, -qqy.y(), -qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, -qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, qqy.y(), -qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, -qqy.y(), -qqz.z() ) );
  return min( pts ) + trans;
}


Point Cylinder::boundingBoxMax( const Matrix &trafo, const Point &trans ) const
{
  /*
  // transform base circle to world coordinates:
  Point py = trafo*Point::UnitY+trans;
  Point pz = trafo*Point::UnitZ+trans;
  // normal vector to plane of base circle:
  Point nv = py.cross( pz ).normalized();
  // vector in x-y plane (perpendicular to nv and z):
  Point qy = nv.cross( Point::UnitZ ).normalized();
  // vector perpendicular to nv and qy:
  Point qz = nv.cross( qy ).normalized();
  // transform qy and qz back to shape coordinates:
  Matrix itrafo = trafo.inverse();
  Point qqy = itrafo * qy;
  Point qqz = itrafo * qz;
  */
  Point qqy = Point::UnitY;
  Point qqz = Point::UnitZ;
  deque<Point> pts;
  pts.push_back( trafo*Point( 0.0, qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 0.0, -qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 0.0, qqy.y(), -qqz.z() ) );
  pts.push_back( trafo*Point( 0.0, -qqy.y(), -qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, -qqy.y(), qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, qqy.y(), -qqz.z() ) );
  pts.push_back( trafo*Point( 1.0, -qqy.y(), -qqz.z() ) );
  return max( pts ) + trans;
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
  ip1 = Point::None;
  ip2 = Point::None;
  int ps = 0;
  Point ips[2];
  double as[2];
  // intersections with infinite cylinder:
  // distance of straight line from pos1 to pos2 projected onto x = 0
  // must equal the radius 1.0:
  Point dpos = pos2 - pos1;
  Point pos1x( pos1 );
  pos1x.x() = 0.0;
  Point pos2x( pos2 );
  pos2x.x() = 0.0;
  double px1 = pos1x.dot( pos1x );
  Point dposx = pos2x - pos1x;
  double dpx = dposx.dot( dposx );
  double dppx = pos1x.dot( dposx );
  double discr = dppx*dppx - dpx*(px1-1.0);
  if ( discr <= 0.0 )
    return;
  double sq = ::sqrt( discr );
  double a1 = (-dppx-sq)/dpx;
  double a2 = (-dppx+sq)/dpx;
  // check whether both pos1 and pos2 are not outside and at the same side (no intersection):
  if ( a1 <= 1.0 && a2 >= 0.0 ) {
    Point ipa1 = pos1 + a1 * dpos;
    Point ipa2 = pos1 + a2 * dpos;
    // check if intersections are in the right x-range:
    if ( ipa1.x() >= 0.0 && ipa1.x() <= 1.0 ) {
      ips[ps] = ipa1;
      as[ps] = a1;
      ps++;
    }
    if ( ipa2.x() >= 0.0 && ipa2.x() <= 1.0 ) {
      ips[ps] = ipa2;
      as[ps] = a2;
      ps++;
    }
  }
  if ( ps < 2 && fabs( dpos[0] ) > 1e-8 ) {
    // intersections with plane perpendicular to x-axis at x=0 and x=1:
    double b1 = - pos1[0] / dpos[0];
    double b2 = ( 1.0 - pos1[0]) / dpos[0];
    Point ipb1 = pos1 + b1 * dpos;
    Point ipb2 = pos1 + b2 * dpos;
    if ( ipb1.distance( Point::Origin ) <= 1.0 ) {
      ips[ps] = ipb1;
      as[ps] = b1;
      ps++;
    }
    if ( ps < 2  && ipb2.distance( Point::UnitX ) <= 1.0 ) {
      ips[ps] = ipb2;
      as[ps] = b2;
      ps++;
    }
  }
  // no intersections:
  if ( ps < 2 )
    return;
  // return intersections in the right order:
  if ( as[0] < as[1] ) {
    if ( as[0] <= 1.0 && as[1] >= 0.0 ) {
      ip1 = ips[0];
      ip2 = ips[1];
    }
  }
  else {
    if ( as[1] <= 1.0 && as[0] >= 0.0 ) {
      ip1 = ips[1];
      ip2 = ips[0];
    }
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
  rotateY( asin( ppx.z()/ppx.magnitude() ) );
  rotateZ( atan2( ppx.y(), ppx.x() ) );
  rotateX( asin( ppy.z()/ppy.magnitude() ) );
  translate( anchor );
}


Shape *Cuboid::copy( void ) const
{
  return new Cuboid( *this );
}


double Cuboid::length( void ) const
{
  Point p0 = transform( Point::Origin );
  Point px = transform( Point::UnitX );
  return p0.distance( px );
}


double Cuboid::width( void ) const
{
  Point p0 = transform( Point::Origin );
  Point py = transform( Point::UnitY );
  return p0.distance( py );
}


double Cuboid::height( void ) const
{
  Point p0 = transform( Point::Origin );
  Point pz = transform( Point::UnitZ );
  return p0.distance( pz );
}


Point Cuboid::boundingBoxMin( const Matrix &trafo, const Point &trans ) const
{
  deque<Point> pts;
  pts.push_back( trafo*Point::Origin+trans );
  pts.push_back( trafo*Point::UnitX+trans );
  pts.push_back( trafo*Point::UnitY+trans );
  pts.push_back( trafo*Point::UnitZ+trans );
  pts.push_back( trafo*(Point::UnitX+Point::UnitY)+trans );
  pts.push_back( trafo*(Point::UnitX+Point::UnitZ)+trans );
  pts.push_back( trafo*(Point::UnitY+Point::UnitZ)+trans );
  pts.push_back( trafo*Point::Ones+trans );
  return min( pts );
}


Point Cuboid::boundingBoxMax( const Matrix &trafo, const Point &trans ) const
{
  deque<Point> pts;
  pts.push_back( trafo*Point::Origin+trans );
  pts.push_back( trafo*Point::UnitX+trans );
  pts.push_back( trafo*Point::UnitY+trans );
  pts.push_back( trafo*Point::UnitZ+trans );
  pts.push_back( trafo*(Point::UnitX+Point::UnitY)+trans );
  pts.push_back( trafo*(Point::UnitX+Point::UnitZ)+trans );
  pts.push_back( trafo*(Point::UnitY+Point::UnitZ)+trans );
  pts.push_back( trafo*Point::Ones+trans );
  return max( pts );
}


bool Cuboid::insideShape( const Point &p ) const
{
  return ( p >= Point::Origin && p <= Point::Ones );
}


void Cuboid::intersectionPointsShape( const Point &pos1, const Point &pos2,
				      Point &ip1, Point &ip2 ) const
{
  ip1 = Point::None;
  ip2 = Point::None;
  int ps = 0;
  Point ips[2];
  double as[2];
  Point dpos = pos2 - pos1;
  for ( int k=0; k<3; k++ ) {
    // line not perpendicular to k-th dimension?
    if ( ::fabs( dpos[k] ) > 1e-8 ) {
      // intersection with plane perpendicular to k-th dimension at 0:
      double a0 = - pos1[k] / dpos[k];
      Point ipp0 = pos1 + a0 * dpos;
      ipp0[k] = 0.0;  // we need an exact zero here
      // intersection with plane perpendicular to k-th dimension at 1:
      double a1 = ( 1.0 - pos1[k]) / dpos[k];
      Point ipp1 = pos1 + a1 * dpos;
      ipp1[k] = 1.0;  // we need an exact one here
      // check whether intersections are part of the cuboid surface,
      // i.e. whether the other two coordinates are between 0 and 1:
      if ( ps < 2 && ipp0 >= Point::Origin && ipp0 <= Point::Ones ) {
	ips[ps] = ipp0;
	as[ps] = a0;
	ps++;
      }
      if ( ps < 2 && ipp1 >= Point::Origin && ipp1 <= Point::Ones ) {
	ips[ps] = ipp1;
	as[ps] = a1;
	ps++;
      }
    }
  }
  // no intersections:
  if ( ps < 2 )
    return;
  // return intersections in the right order:
  if ( as[0] < as[1] ) {
    if ( as[0] <= 1.0 && as[1] >= 0.0 ) {
      ip1 = ips[0];
      ip2 = ips[1];
    }
  }
  else {
    if ( as[1] <= 1.0 && as[0] >= 0.0 ) {
      ip1 = ips[1];
      ip2 = ips[0];
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
