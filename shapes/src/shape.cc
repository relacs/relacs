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
#include <relacs/shape.h>

namespace relacs {


//******************************************

Shape::Shape( Shape::ShapeType type, const string &name, int resolution )
  : Type( type ),
    Name( name ),
    Parent( 0 ),
    Trafo(),
    InvTrafo(),
    Resolution( resolution )
{
  Polygons.clear();
}


Shape::Shape( Shape::ShapeType type, const string &name,
	      int resolution, const Transform &trafo  )
  : Type( type ),
    Name( name ),
    Parent( 0 ),
    Trafo( trafo ),
    Resolution( resolution )
{
  InvTrafo = Trafo.inverse();
  Polygons.clear();
}


Shape::Shape( const Shape &s )
  : Polygons( s.Polygons ),
    Type( s.Type ),
    Name( s.Name ),
    Parent( s.Parent ),
    Trafo( s.Trafo ),
    InvTrafo( s.InvTrafo ),
    Resolution( s.Resolution )
{
}


Shape::~Shape( void)
{
}


void Shape::translateX( double x )
{
  Trafo.translateX( x );
  InvTrafo = Trafo.inverse();
}


void Shape::translateY( double y )
{
  Trafo.translateY( y );
  InvTrafo = Trafo.inverse();
}


void Shape::translateZ( double z )
{
  Trafo.translateZ( z );
  InvTrafo = Trafo.inverse();
}


void Shape::translate( double x, double y, double z )
{
  Trafo.translate( x, y, z );
  InvTrafo = Trafo.inverse();
}


void Shape::translate( const Point &p )
{
  Trafo.translate( p );
  InvTrafo = Trafo.inverse();
}


void Shape::scaleX( double xscale )
{
  Trafo.scaleX( xscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scaleY( double yscale )
{
  Trafo.scaleY( yscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scaleZ( double zscale )
{
  Trafo.scaleZ( zscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scale( double xscale, double yscale, double zscale )
{
  Trafo.scale( xscale, yscale, zscale );
  InvTrafo = Trafo.inverse();
}


void Shape::scale( const Point &scale )
{
  Trafo.scale( scale );
  InvTrafo = Trafo.inverse();
}


void Shape::scale( double scale )
{
  Trafo.scale( scale );
  InvTrafo = Trafo.inverse();
}


void Shape::rotateX( double angle )
{
  Trafo.rotateX( angle );
  InvTrafo = Trafo.inverse();
}


void Shape::rotateY( double angle )
{
  Trafo.rotateY( angle );
  InvTrafo = Trafo.inverse();
}


void Shape::rotateZ( double angle )
{
  Trafo.rotateZ( angle );
  InvTrafo = Trafo.inverse();
}


void Shape::rotate( double anglex, double angley, double anglez )
{
  Trafo.rotate( anglex, angley, anglez );
  InvTrafo = Trafo.inverse();
}


void Shape::rotate( const Point &axis, double angle )
{
  Trafo.rotate( axis, angle );
  InvTrafo = Trafo.inverse();
}


void Shape::transform( const Transform &trafo )
{
  Trafo *= trafo;
  InvTrafo = Trafo.inverse();
}


Point Shape::transform( const Point &p ) const
{
  return Trafo * p;
}


void Shape::setTransform( const Transform &trafo )
{
  Trafo = trafo;
  InvTrafo = Trafo.inverse();
}


void Shape::clearTransform( void )
{
  Trafo.clear();
  InvTrafo.clear();
}


Point Shape::inverseTransform( const Point &p ) const
{
  return InvTrafo * p;
}


int Shape::resolution( void ) const
{
  if ( Resolution <= 0 && Parent != 0 )
    return Parent->resolution();
  else
    return Resolution;
}


void Shape::updatePolygons( void ) const
{
  // generate polygons:
  resetPolygons();
  // transform polygons to world coordinates:
  Transform it = Trafo.inverse().transpose();
  for ( auto pi = Polygons.begin(); pi != Polygons.end(); ++pi )
    pi->apply( Trafo, it );
}


Point Shape::boundingBoxMin( void ) const
{
  return boundingBoxMin( Trafo );
}


Point Shape::boundingBoxMax( void ) const
{
  return boundingBoxMax( Trafo );
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
  : Shape( Shape::ZoneShape, "zone", 0 )
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


Zone::Zone( const string &name )
  : Shape( Shape::ZoneShape, name, 0 )
{
}


Zone::Zone( const Shape &s, const string &name )
  : Shape( Shape::ZoneShape, name, 0 )
{
  Shapes.clear();
  Add.clear();
  Shapes.push_back( s.copy() );
  Add.push_back( true );
}


Zone::Zone( const deque<Shape*> &s, const string &name )
  : Shape( Shape::ZoneShape, name, 0 )
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
  Shapes.back()->setParent( this );
}


void Zone::subtract( const Shape &s )
{
  Shapes.push_back( s.copy() );
  Add.push_back( false );
  Shapes.back()->setParent( this );
}


void Zone::push( const Shape &s, bool add )
{
  Shapes.push_back( s.copy() );
  Add.push_back( add );
  Shapes.back()->setParent( this );
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


bool Zone::added( int i ) const
{
  return Add[i];
}


bool Zone::added( const string &name ) const
{
  auto ai = Add.begin();
  for ( auto si = Shapes.begin(); si != Shapes.end(); ++si, ++ai ) {
    if ( (*si)->name() == name )
      return *ai;
  }
  return true;
}


bool Zone::subtracted( int i ) const
{
  return ! Add[i];
}


bool Zone::subtracted( const string &name ) const
{
  auto ai = Add.begin();
  for ( auto si = Shapes.begin(); si != Shapes.end(); ++si, ++ai ) {
    if ( (*si)->name() == name )
      return ! *ai;
  }
  return false;
}


void Zone::clear( void )
{
  for ( auto si = Shapes.begin(); si != Shapes.end(); ++si )
    delete *si;
  Shapes.clear();
  Add.clear();
}


void Zone::resetPolygons( void ) const
{
  Polygons.clear();
}


void Zone::updatePolygons( void ) const
{
  Polygons.clear();
  deque<Polygon> lines;
  // assemble polygons from shapes:
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    (*si)->updatePolygons();
    auto pe = Polygons.end();
    for ( auto pi=(*si)->polygons().begin(); pi != (*si)->polygons().end(); ++pi ) {
      //      cerr << *pi << '\n';
      if ( *ai ) {
	// do not include additive polygons that are inside the zone:
	bool inside = false;
	auto ssi = Shapes.begin();
	auto aai = Add.begin();
	for ( ; ssi != Shapes.end(); ++ssi, ++aai ) {
	  if ( ssi != si && pi->inside( **ssi ) )
	    inside = true;
	}
	if ( inside )
	  continue;
	// add polygon:
	Polygons.push_back( *pi );
      }
      else {
	// do not include polygons from subtractive shapes that are outside:
	bool outside = true;
	auto ssi = Shapes.begin();
	auto aai = Add.begin();
	for ( ; ssi != si; ++ssi, ++aai ) {
	  if ( ! pi->outside( **ssi ) ) { // XXX how to deal with subtractive shapes?
	    outside = false;
	    break;
	  }
	}
	if ( outside )
	  continue;
	Polygons.push_back( *pi );
	Polygons.back().flipNormal();
      }
      // check for intersections with other polygons:
      if ( Polygons.size() < 4 ) {
	pe = Polygons.end() - 1;
	for ( auto ppi = Polygons.begin(); ppi != pe; ++ppi ) {
	  //cerr << "intersect " << Polygons.size()-1 << " with " << ppi - Polygons.begin() << '\n';
	  Polygon line = Polygons.back().intersect( *ppi );
	  if ( line.size() > 0 )
	    lines.push_back( line );
	}
      }
    } // each polygon
  } // each shape

  for ( auto li = lines.begin(); li != lines.end(); ++li )
    Polygons.push_back( *li );

  // transform polygons to world coordinates:
  Transform it = trafo().inverse().transpose();
  for ( auto pi = Polygons.begin(); pi != Polygons.end(); ++pi )
    pi->apply( trafo(), it );
}


Point Zone::boundingBoxMin( const Transform &trafo ) const
{
  Point p( Point::None );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Transform m = trafo * (*si)->trafo();
      p = (*si)->boundingBoxMin( m );
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Transform m = trafo * (*si)->trafo();
      p = p.min( (*si)->boundingBoxMin( m ) );
    }
  }
  return p;
}


Point Zone::boundingBoxMax( const Transform &trafo ) const
{
  Point p( Point::None );
  if ( Shapes.empty() )
    return p;

  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Transform m = trafo * (*si)->trafo();
      p = (*si)->boundingBoxMax( m );
      break;
    }
  }
  for ( ; si != Shapes.end(); ++si, ++ai ) {
    if ( *ai ) {
      Transform m = trafo * (*si)->trafo();
      p = p.max( (*si)->boundingBoxMax( m ) );
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
	if ( std::isnan( a1 ) || aa1 < a1 ) {
	  a1 = aa1;
	  ip1 = ipp1;
	}
	if ( std::isnan( a2 ) || aa2 > a2 ) {
	  a2 = aa2;
	  ip2 = ipp2;
	}
      }
      else if ( ! std::isnan( a1 ) ) {
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
  str << "Zone \"" << name() << "\" consisting of\n";
  auto si = Shapes.begin();
  auto ai = Add.begin();
  for ( ; si != Shapes.end(); ++si, ++ai )
    str << " " << (*ai ? '+' : '-') << " " << *si;
  return str;
}



//******************************************

Sphere::Sphere( void )
  : Shape( Shape::Sphere, "sphere", 20 )
{
}


Sphere::Sphere( const Sphere &s )
  : Shape( s )
{
}


Sphere::Sphere( const string &name, int resolution )
  : Shape( Shape::Sphere, name, resolution )
{
}


Sphere::Sphere( const Point &center, double radius,
		const string &name, int resolution )
  : Shape( Shape::Sphere, name, resolution )
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


void Sphere::resetPolygons( void ) const
{
  Polygons.clear();

  int n = resolution();
  if ( n <= 0 )
    n = 20;
  int m = n/2;
  for ( int j=0; j<m; j++ ) {
    for ( int k=0; k<n; k++ ) {
      Polygon poly;
      double ck0 = ::cos( 2.0*M_PI*k/n );
      double sk0 = ::sin( 2.0*M_PI*k/n );
      double ck1 = ::cos( 2.0*M_PI*(k+1)/n );
      double sk1 = ::sin( 2.0*M_PI*(k+1)/n );
      double cj0 = ::cos( (1.0*j/m-0.5)*M_PI );
      double sj0 = ::sin( (1.0*j/m-0.5)*M_PI );
      double cj1 = ::cos( (1.0*(j+1)/m-0.5)*M_PI );
      double sj1 = ::sin( (1.0*(j+1)/m-0.5)*M_PI );
      poly.push( Point( ck0*cj0, sk0*cj0, sj0 ) );
      poly.push( Point( ck1*cj0, sk1*cj0, sj0 ) );
      poly.push( Point( ck1*cj1, sk1*cj1, sj1 ) );
      poly.push( Point( ck0*cj1, sk0*cj1, sj1 ) );
      poly.setNormal();
      Polygons.push_back( poly );
    }
  }
}


Point Sphere::boundingBoxMin( const Transform &trafo ) const
{
  /*
  Transform itrafo = trafo.inverse();
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
  return min( pts );
}


Point Sphere::boundingBoxMax( const Transform &trafo ) const
{
  /*
  Transform itrafo = trafo.inverse();
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
  return max( pts );
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
  str << "Sphere \"" << name() << "\" with radius " << radius() << '\n';
  return str;
}


//******************************************

Cylinder::Cylinder( void )
  : Shape( Shape::Cylinder, "cylinder", 20 )
{
}


Cylinder::Cylinder( const Cylinder &c )
  : Shape( c )
{
}


Cylinder::Cylinder( const string &name, int resolution )
  : Shape( Shape::Cylinder, name, resolution )
{
}


Cylinder::Cylinder( const Point &anchor, double radius, double length,
		    const string &name, int resolution )
  : Shape( Shape::Cylinder, name, resolution )
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


void Cylinder::resetPolygons( void ) const
{
  Polygons.clear();

  int n = resolution();
  if ( n <= 0 )
    n = 20;
  Polygon poly0;
  Polygon poly1;
  for ( int k=0; k<n; k++ ) {
    double c = ::cos( 2.0*M_PI*k/n );
    double s = ::sin( 2.0*M_PI*k/n );
    poly0.push( Point( 0.0, c, s ) );
    poly1.push( Point( 1.0, c, s ) );
  }
  poly0.setNormal( Point( -1.0, 0.0, 0.0 ) );
  poly1.setNormal( Point( +1.0, 0.0, 0.0 ) );
  Polygons.push_back( poly0 );
  Polygons.push_back( poly1 );
  for ( int k=0; k<n; k++ ) {
    Polygon poly;
    poly.push( poly0[k] );
    poly.push( poly0[(k+1)%n] );
    poly.push( poly1[(k+1)%n] );
    poly.push( poly1[k] );
    poly.setNormal();
    Polygons.push_back( poly );
  }
}


Point Cylinder::boundingBoxMin( const Transform &trafo ) const
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
  Transform itrafo = trafo.inverse();
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
  return min( pts );
}


Point Cylinder::boundingBoxMax( const Transform &trafo ) const
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
  Transform itrafo = trafo.inverse();
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
  str << "Cylinder \"" << name() << "\" with length "
      << length() << " and radius " << radius() << '\n';
  return str;
}


//******************************************

Cuboid::Cuboid( void )
  : Shape( Shape::Cuboid, "cuboid", 0 )
{
}


Cuboid::Cuboid( const Cuboid &c )
  : Shape( c )
{
}


Cuboid::Cuboid( const string &name )
  : Shape( Shape::Cuboid, name, 0 )
{
}


Cuboid::Cuboid( const Point &anchor, 
		double length, double width, double height, const string &name )
  : Shape( Shape::Cuboid, name, 0 )
{
  scale( length, width, height );
  translate( anchor );
}


Cuboid::Cuboid( const Point &anchor, const Point &end, const string &name )
  : Shape( Shape::Cuboid, name, 0 )
{
  scale( end - anchor );
  translate( anchor );
}


Cuboid::Cuboid( const Point &anchor, const Point &px, const Point &py, const Point &pz,
		const string &name )
  : Shape( Shape::Cuboid, name, 0 )
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


void Cuboid::corners( deque< Point > &pts,
		      const Transform &trafo ) const
{
  pts.clear();
  pts.push_back( trafo*Point::Origin );
  pts.push_back( trafo*Point::UnitX );
  pts.push_back( trafo*(Point::UnitX+Point::UnitY) );
  pts.push_back( trafo*Point::UnitY );
  pts.push_back( trafo*Point::UnitZ );
  pts.push_back( trafo*(Point::UnitX+Point::UnitZ) );
  pts.push_back( trafo*Point::Ones );
  pts.push_back( trafo*(Point::UnitY+Point::UnitZ) );
}


void Cuboid::corners( deque< Point > &pts ) const
{
  corners( pts, trafo() );
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


void Cuboid::resetPolygons( void ) const
{
  Polygons.clear();

  Polygon poly;
  poly.push( Point::Origin );
  poly.push( Point::UnitX );
  poly.push( Point::UnitX + Point::UnitY );
  poly.push( Point::UnitY );
  poly.setNormal( Point( 0.0, 0.0, -1.0 ) );
  Polygons.push_back( poly );

  poly.clear();
  poly.push( Point::UnitZ + Point::Origin );
  poly.push( Point::UnitZ + Point::UnitX );
  poly.push( Point::UnitZ + Point::UnitX + Point::UnitY );
  poly.push( Point::UnitZ + Point::UnitY );
  poly.setNormal( Point( 0.0, 0.0, 1.0 ) );
  Polygons.push_back( poly );

  poly.clear();
  poly.push( Point::Origin );
  poly.push( Point::UnitX );
  poly.push( Point::UnitZ + Point::UnitX );
  poly.push( Point::UnitZ + Point::Origin );
  poly.setNormal( Point( 0.0, -1.0, 0.0 ) );
  Polygons.push_back( poly );

  poly.clear();
  poly.push( Point::UnitX );
  poly.push( Point::UnitX + Point::UnitY );
  poly.push( Point::UnitZ + Point::UnitX + Point::UnitY );
  poly.push( Point::UnitZ + Point::UnitX );
  poly.setNormal( Point( 1.0, 0.0, 0.0 ) );
  Polygons.push_back( poly );

  poly.clear();
  poly.push( Point::UnitX + Point::UnitY );
  poly.push( Point::UnitY );
  poly.push( Point::UnitZ + Point::UnitY );
  poly.push( Point::UnitZ + Point::UnitX + Point::UnitY );
  poly.setNormal( Point( 0.0, 1.0, 0.0 ) );
  Polygons.push_back( poly );

  poly.clear();
  poly.push( Point::UnitY );
  poly.push( Point::Origin );
  poly.push( Point::UnitZ + Point::Origin );
  poly.push( Point::UnitZ + Point::UnitY );
  poly.setNormal( Point( -1.0, 0.0, 0.0 ) );
  Polygons.push_back( poly );
}


Point Cuboid::boundingBoxMin( const Transform &trafo ) const
{
  deque<Point> pts;
  corners( pts, trafo );
  return min( pts );
}


Point Cuboid::boundingBoxMax( const Transform &trafo ) const
{
  deque<Point> pts;
  corners( pts, trafo );
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
  str << "Cuboid \"" << name() << "\" with size "
      << length() << ", "
      << width() << ", "
      << height() << '\n';
  return str;
}


Point Cuboid::corner( void ) const
{
  return Point( length(), width(), height() );
}


void Cuboid::setCorner( const Point &corner )
{
  clearTransform();
  scale( corner );
}


void Cuboid::setLength( double length )
{
  Transform tm = trafo();
  tm(0, 0) = length;
  setTransform( tm );
}


void Cuboid::setWidth( double width )
{
  Transform tm = trafo();
  tm(1, 1) = width;
  setTransform( tm );
}


void Cuboid::setHeight( double height )
{
  Transform tm = trafo();
  tm(2, 2) = height;
  setTransform( tm );
}


}; /* namespace relacs */
