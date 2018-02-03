#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <relacs/point.h>

using namespace relacs;


const double epsilon = 1e-8;

const int n = 1000;

const double minscale = 1e-8;


double urand( void )
{
  return (double)rand()/double(RAND_MAX);
}


Point random_point( void )
{
  Point p;
  for ( int j=0; j<3; j++ )
    p[j] = 5.0*(urand()-0.5);
  return p;
}


void check_equality( const Point &a, const Point &b )
{
  for ( int j=0; j<3; j++ )
    assert( fabs( a[j] - b[j] ) < epsilon );
}


int main ( void )
{
  cerr << "Test assignments\n";
  for ( int k=0; k<n; k++ ) {
    Point a = random_point();
    Point b( a );
    check_equality( a, b );
    Point c;
    c = a;
    check_equality( a, c );
    Point d;
    for ( int j=0; j<3; j++ )
      d[j] = a[j];
    check_equality( a, d );
  }

  cerr << "Test normalization\n";
  for ( int k=0; k<n; k++ ) {
    Point a = random_point();
    Point b = a.normalized();
    assert( fabs( b.magnitude() - 1.0 ) < epsilon );
    a.normalize();
    assert( fabs( a.magnitude() - 1.0 ) < epsilon );
  }

  cerr << "Test addition/subtraction and distance\n";
  for ( int k=0; k<n; k++ ) {
    Point a = random_point();
    double am = a.magnitude();
    Point b = random_point();
    double bm = b.magnitude();
    Point c = a + b;
    assert( fabs( c.distance( b ) - am ) < epsilon );
    assert( fabs( c.distance( a ) - bm ) < epsilon );
    Point d = a - b;
    assert( fabs( a.distance( b ) - d.magnitude() ) < epsilon );
  }
}
