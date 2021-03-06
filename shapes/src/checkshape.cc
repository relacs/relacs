#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <relacs/point.h>
#include <relacs/transform.h>
#include <relacs/shape.h>

using namespace relacs;


const double epsilon = 1e-8;

const int npoints = 10000;

const double maxrange = 3.0;
const double mindistance = 0.5;

const int nlines = 100;
const double linefac = 0.01;

const int ntrafos = 20;
const double minscale = 0.001;

const double offset = 0.1;


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


void random_transformation( Shape &shp )
{
  int trafotype = rand()%9;
  if ( trafotype < 3 ) {
    double scale = 1.0;
    do { scale = 4.0*(urand()-0.5); } while ( fabs(scale) < minscale );
    if ( trafotype == 0 )
      return shp.scaleX( scale );
    else if ( trafotype == 1 )
      return shp.scaleY( scale );
    else
      return shp.scaleZ( scale );
  }
  else if ( trafotype < 6 ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    if ( trafotype == 4 )
      return shp.rotateZ( angle );
    else if ( trafotype == 5 )
      return shp.rotateY( angle );
    else
      return shp.rotateX( angle );
  }
  else {
    double shift = 4.0*(urand()-0.5);
    if ( trafotype == 7 )
      return shp.translateX( shift );
    else if ( trafotype == 8 )
      return shp.translateY( shift );
    else
      return shp.translateZ( shift );
  }
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
      double pqm = pq.magnitude();
      Point ip21 = ip2 - ip1;
      assert( fabs( fabs( pq.dot(ip21) ) - pqm*ip21.magnitude() ) < epsilon );

      Point pip1 = ip1 - p;
      double pqp1 = pq.dot(pip1);
      assert( fabs( fabs( pqp1 ) - pqm*pip1.magnitude() ) < epsilon );
      double a1 = pqp1/pqm/pqm;

      Point pip2 = ip2 - p;
      double pqp2 = pq.dot(pip2);
      assert( fabs( fabs( pqp2 ) - pqm*pip2.magnitude() ) < epsilon );
      double a2 = pqp2/pqm/pqm;

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
  for ( int k=0; k<ntrafos; k++ ) {
    Shape *s = shp.copy();
    for ( int j=0; j<6; j++ ) {
      random_transformation( *s );
      check_intersections( *s );
    }
  }
}


void check_boundingbox( Shape &shp )
{
  Point bbmin = shp.boundingBoxMin();
  Point bbmax = shp.boundingBoxMax();
  for ( int k=0; k<npoints; k++ ) {
    Point p( (bbmax.x() - bbmin.x())*urand() + bbmin.x(),
	     (bbmax.y() - bbmin.y())*urand() + bbmin.y(),
	     (bbmax.z() - bbmin.z())*urand() + bbmin.z() );
    for ( int j=0; j<3; j++ ) {
      Point q = p;
      q[j] = bbmin[j] - offset;
      assert( ! shp.inside( q ));
      q[j] = bbmax[j] + offset;
      assert( ! shp.inside( q ));
    }
  }
}


void check_transformed_boundingbox( Shape &shp )
{
  cerr << "  check transformed bounding box():\n";
  for ( int k=0; k<ntrafos; k++ ) {
    Shape *s = shp.copy();
    for ( int j=0; j<6; j++ ) {
      random_transformation( *s );
      /*
      Matrix itrafo = s->trafo().inverse();
      Point px = itrafo * Point::UnitX;
      px.normalize();
      Point py = itrafo * Point::UnitY;
      py.normalize();
      Point pz = itrafo * Point::UnitZ;
      pz.normalize();
      Point qx = s->trafo() * px;
      Point qy = s->trafo() * py;
      Point qz = s->trafo() * pz;
      cerr << qx.dot( qy) << " " << qx.dot( qz ) << " " << qy.dot( qz ) << '\n';
      */
      check_boundingbox( *s );
    }
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
  cerr << "  check bounding box():\n";
  check_boundingbox( sphr );
  check_transformed_boundingbox( sphr );

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
  cerr << "  check bounding box():\n";
  check_boundingbox( cyln );
  check_transformed_boundingbox( cyln );

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
  check_boundingbox( cbd );
  check_transformed_boundingbox( cbd );

  return 0;
}
