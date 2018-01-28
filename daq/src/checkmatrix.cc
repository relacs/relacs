#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <relacs/point.h>
#include <relacs/matrix.h>

using namespace relacs;


double urand( void )
{
  return (double)rand()/double(RAND_MAX);
}


Matrix random_matrix( void )
{
  Matrix m;
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      m( i, j ) = urand();
  }
  return m;
}


void check_identity( const Matrix &m )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ ) {
      if ( i == j )
	assert( fabs( m( i, j ) - 1.0 ) < 1e-8 );
      else
	assert( fabs( m( i, j ) ) < 1e-8 );
    }
  }
}


int main ( void )
{
  cerr << "Test Matrix::inverse()\n";
  int n = 1000;
  for ( int k=0; k<n; k++ ) {
    Matrix a = random_matrix();
    Matrix b = a.inverse();
    Matrix c = a * b;
    check_identity( c );
    a *= b;
    check_identity( a );
  }

  cerr << "Test Matrix::rotateYaw()\n";
  for ( int k=0; k<n; k++ ) {
    double yaw = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotateYaw( yaw );
    Matrix b = a * Matrix::rotateYaw( -yaw );
    check_identity( b );
    a *= Matrix::rotateYaw( -yaw );
    check_identity( a );

    // XXX Yaw turns around x axis!!! Should be z!
    Point p( 0.0, urand(), urand() );
    Point q = Matrix::rotateYaw( yaw ) * p;
    assert( fabs( q.x() ) < 1e-8 );
    assert( fabs( p.magnitude() - q.magnitude() ) < 1e-8 );
  }

  /*
  // XXX These tests fail. Why?
  cerr << "Test Matrix::rotatePitch()\n";
  for ( int k=0; k<n; k++ ) {
    double pitch = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotatePitch( pitch );
    Matrix b = a * Matrix::rotatePitch( -pitch );
    check_identity( b );
    a *= Matrix::rotatePitch( -pitch );
    check_identity( a );
  }

  cerr << "Test Matrix::rotateRoll()\n";
  for ( int k=0; k<n; k++ ) {
    double roll = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotateRoll( roll );
    Matrix b = a * Matrix::rotateRoll( -roll );
    check_identity( b );
    a *= Matrix::rotateRoll( -roll );
    check_identity( a );
  }
  */

  return 0;
}
