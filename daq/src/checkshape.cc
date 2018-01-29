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

const int nlines = 1000;
const double linefac = 0.001;


double urand( void )
{
  return (double)rand()/double(RAND_MAX);
}


int main ( void )
{
  cerr << "Test Sphere\n";
  Sphere sphr;
  for ( int k=0; k<npoints; k++ ) {
    Point p( 3.0*(urand()-0.5), 3.0*(urand()-0.5), 3.0*(urand()-0.5) );
    bool pinside = ( p.magnitude() <= 1.0 );
    assert( pinside == sphr.insideShape( p ) );
    Point q( 3.0*(urand()-0.5), 3.0*(urand()-0.5), 3.0*(urand()-0.5) );
    while ( p.distance( q ) < 0.5 )
      q = Point( 3.0*(urand()-0.5), 3.0*(urand()-0.5), 3.0*(urand()-0.5) );
    bool qinside = ( q.magnitude() <= 1.0 );
    assert( qinside == sphr.insideShape( q ) );

    Point ip1;
    Point ip2;
    sphr.intersectionPointsShape( p, q, ip1, ip2 );
    assert( ip1.isNone() == ip2.isNone() );
    if ( ip1.isNone() ) {
      // no intersections:
      for ( int i=1; i<nlines; i++ ) {
	Point ip = p + linefac*i * (q - p);
	assert( ! sphr.insideShape( ip ) );
      }
    }
    else {
      // intersection points are inside:
      assert( fabs( ip1.magnitude() - 1.0 ) < epsilon );
      assert( fabs( ip2.magnitude() - 1.0 ) < epsilon );
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
	assert( sphr.insideShape( ip ) );
	// outside intersection points:
	if ( sphr.insideShape( p ) ) {
	  ip = p + linefac*i * (ip1 - p);
	  assert( sphr.insideShape( ip ) );
	}
	else {
	  Point op = p + linefac*i * (ip1 - p);
	  assert( ! sphr.insideShape( op ) );
	}
	if ( sphr.insideShape( q ) ) {
	  Point iq = q + linefac*i * (ip2 - q);
	  assert( sphr.insideShape( iq ) );
	}
	else {
	  Point oq = q + linefac*i * (ip2 - q);
	  assert( ! sphr.insideShape( oq ) );
	}
      }
    }
  }

  cerr << "Test Cylinder\n";
  Cylinder cyln;
  for ( int k=0; k<npoints; k++ ) {
    Point p( 3.0*(urand()-0.5), 3.0*(urand()-0.5), 3.0*(urand()-0.5) );
    Point pp( p );
    pp.x() = 0.0;
    bool pinside = ( (pp.magnitude() <= 1.0) && (p.x() >= 0.0) && (p.x() <= 1.0) );
    assert( pinside == cyln.insideShape( p ) );
    Point q( 3.0*(urand()-0.5), 3.0*(urand()-0.5), 3.0*(urand()-0.5) );
    while ( p.distance( q ) < 0.5 )
      q = Point( 3.0*(urand()-0.5), 3.0*(urand()-0.5), 3.0*(urand()-0.5) );
    Point qq( q );
    qq.x() = 0.0;
    bool qinside = ( (qq.magnitude() <= 1.0) && (q.x() >= 0.0) && (q.x() <= 1.0) );
    assert( qinside == cyln.insideShape( q ) );

    Point ip1;
    Point ip2;
    cyln.intersectionPointsShape( p, q, ip1, ip2 );
    assert( ip1.isNone() == ip2.isNone() );
    if ( ip1.isNone() ) {
      // no intersections:
      for ( int i=1; i<nlines; i++ ) {
	Point ip = p + linefac*i * (q - p);
	assert( ! cyln.insideShape( ip ) );
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
	assert( cyln.insideShape( ip ) );
	// outside intersection points:
	if ( cyln.insideShape( p ) ) {
	  ip = p + linefac*i * (ip1 - p);
	  assert( cyln.insideShape( ip ) );
	}
	else {
	  Point op = p + linefac*i * (ip1 - p);
	  assert( ! cyln.insideShape( op ) );
	}
	if ( cyln.insideShape( q ) ) {
	  Point iq = q + linefac*i * (ip2 - q);
	  assert( cyln.insideShape( iq ) );
	}
	else {
	  Point oq = q + linefac*i * (ip2 - q);
	  assert( ! cyln.insideShape( oq ) );
	}
      }
    }
  }

  return 0;
}
