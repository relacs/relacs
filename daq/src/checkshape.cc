#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <relacs/point.h>
#include <relacs/matrix.h>
#include <relacs/shape.h>

using namespace relacs;


const double epsilon = 1e-8;

const int npoints = 10000;

const double maxrange = 3.0;
const double mindistance = 0.5;

const int nlines = 100;
const double linefac = 0.01;

const int ntrafos = 20;
const double minscale = 1e-8;


double urand( void )
{
  return (double)rand()/double(RAND_MAX);
}


double xrand( void )
{
  return maxrange*((double)rand()/double(RAND_MAX)-0.5);
}


double arand( void )
{
  return 2.0*M_PI*((double)rand()/double(RAND_MAX)-0.5);
}


void check_intersections( const Shape &shp )
{
  cerr << "  check intersectionsPoints():\n";
  for ( int k=0; k<npoints; k++ ) {
    // two random points:
    Point p( xrand(), xrand(), xrand() );
    Point q( xrand(), xrand(), xrand() );
    while ( p.distance( q ) < mindistance )
      q = Point( xrand(), xrand(), xrand() );

    Point ip1;
    Point ip2;
    shp.intersectionPoints( p, q, ip1, ip2 );
    assert( ip1.isNone() == ip2.isNone() );
    if ( ip1.isNone() ) {
      // no intersections:
      for ( int i=1; i<nlines; i++ ) {
	Point ip = p + linefac*i * (q - p);
	assert( ! shp.inside( ip ) );
      }
    }
    else {
      // intersection points are in the same direction:
      Point pq = q - p;
      Point ip21 = ip2 - ip1;
      Point rp = ip21/pq;
      rp /= rp.x();
      rp -= 1.0;
      assert( rp.magnitude() < epsilon );

      Point pip1 = ip1 - p;
      rp = pip1/pq;
      double a1 = rp.x();
      rp /= rp.x();
      rp -= 1.0;
      assert( rp.magnitude() < epsilon );

      Point pip2 = ip2 - p;
      rp = pip2/pq;
      double a2 = rp.x();
      rp /= rp.x();
      rp -= 1.0;
      assert( rp.magnitude() < epsilon );

      // intersection points are ordered:
      assert( a1 <= a2 );

      for ( int i=1; i<nlines; i++ ) {
	// inside intersection points:
	Point ip = ip1 + linefac*i * ip21;
	assert( shp.inside( ip ) );
	// outside intersection points:
	if ( shp.inside( p ) ) {
	  ip = p + linefac*i * (ip1 - p);
	  assert( shp.inside( ip ) );
	}
	else {
	  Point op = p + linefac*i * (ip1 - p);
	  assert( ! shp.inside( op ) );
	}
	if ( shp.inside( q ) ) {
	  Point iq = q + linefac*i * (ip2 - q);
	  assert( shp.inside( iq ) );
	}
	else {
	  Point oq = q + linefac*i * (ip2 - q);
	  assert( ! shp.inside( oq ) );
	}
      }
    }
  }
}


void check_transformed_intersections( Shape &shp )
{
  double scale = 1.0;
  for ( int k=0; k<ntrafos; k++ ) {
    Shape *s = shp.copy();
    cerr << "  transform scaleX() ...\n";
    do { scale = 4.0*(urand()-0.5); } while ( fabs(scale) < minscale );
    s->scaleX( scale );
    check_intersections( shp );
    cerr << "  transform rotateYaw() ...\n";
    s->rotateYaw( arand() );
    check_intersections( shp );
    cerr << "  transform scaleY() ...\n";
    do { scale = 4.0*(urand()-0.5); } while ( fabs(scale) < minscale );
    s->scaleY( scale );
    check_intersections( shp );
    cerr << "  transform rotatePitch() ...\n";
    s->rotatePitch( arand() );
    check_intersections( shp );
    cerr << "  transform scaleZ() ...\n";
    do { scale = 4.0*(urand()-0.5); } while ( fabs(scale) < minscale );
    s->scaleZ( scale );
    check_intersections( shp );
    cerr << "  transform rotateRoll() ...\n";
    s->rotateRoll( arand() );
    check_intersections( shp );
  }
}


int main ( void )
{
  cerr << "Test Sphere:\n";
  Sphere sphr;
  cerr << "  check insideShape():\n";
  for ( int k=0; k<npoints; k++ ) {
    Point p( xrand(), xrand(), xrand() );
    bool inside = ( p.magnitude() <= 1.0 );
    assert( inside == sphr.insideShape( p ) );
  }
  check_intersections( sphr );
  check_transformed_intersections( sphr );

  cerr << "Test Cylinder:\n";
  Cylinder cyln;
  cerr << "  check insideShape():\n";
  for ( int k=0; k<npoints; k++ ) {
    Point p( xrand(), xrand(), xrand() );
    Point pp( p );
    pp.x() = 0.0;
    bool inside = ( (pp.magnitude() <= 1.0) && (p.x() >= 0.0) && (p.x() <= 1.0) );
    assert( inside == cyln.insideShape( p ) );
  }
  check_intersections( cyln );
  check_transformed_intersections( cyln );

  cerr << "Test Cuboid:\n";
  Cuboid cbd;
  cerr << "  check insideShape():\n";
  for ( int k=0; k<npoints; k++ ) {
    Point p( xrand(), xrand(), xrand() );
    bool inside = ( ( p >= Point::Origin ) && ( p <= Point::Ones ) );
    assert( inside == cbd.insideShape( p ) );
  }
  check_intersections( cbd );
  check_transformed_intersections( cbd );

  return 0;
}
