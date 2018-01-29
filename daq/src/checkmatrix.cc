#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <relacs/point.h>
#include <relacs/matrix.h>

using namespace relacs;


const double epsilon = 1e-8;

const int n = 1000;


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
	assert( fabs( m( i, j ) - 1.0 ) < epsilon );
      else
	assert( fabs( m( i, j ) ) < epsilon );
    }
  }
}


int main ( void )
{
  cerr << "Test Matrix::inverse()\n";
  for ( int k=0; k<n; k++ ) {
    Matrix a = random_matrix();
    Matrix b = a.inverse();
    Matrix c = a * b;
    check_identity( c );
    a *= b;
    check_identity( a );
  }

  cerr << "Test Matrix::scaleX()\n";
  for ( int k=0; k<n; k++ ) {
    double scale = 10.0*(urand()-0.5);
    if ( fabs( scale ) < epsilon )
      continue;
    Matrix a = Matrix::scaleX( scale );
    Matrix b = Matrix::scaleX( 1.0/scale ) * a;
    check_identity( b );
    a *= Matrix::scaleX( 1.0/scale );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Point q = Matrix::scaleX( scale ) * p;
    assert( fabs( p.x()*scale - q.x() ) < epsilon );
    assert( fabs( p.y() - q.y() ) < epsilon );
    assert( fabs( p.z() - q.z() ) < epsilon );
  }

  cerr << "Test Matrix::scaleY()\n";
  for ( int k=0; k<n; k++ ) {
    double scale = 10.0*(urand()-0.5);
    if ( fabs( scale ) < epsilon )
      continue;
    Matrix a = Matrix::scaleY( scale );
    Matrix b = Matrix::scaleY( 1.0/scale ) * a;
    check_identity( b );
    a *= Matrix::scaleY( 1.0/scale );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Point q = Matrix::scaleY( scale ) * p;
    assert( fabs( p.x() - q.x() ) < epsilon );
    assert( fabs( p.y()*scale - q.y() ) < epsilon );
    assert( fabs( p.z() - q.z() ) < epsilon );
  }

  cerr << "Test Matrix::scaleZ()\n";
  for ( int k=0; k<n; k++ ) {
    double scale = 10.0*(urand()-0.5);
    if ( fabs( scale ) < epsilon )
      continue;
    Matrix a = Matrix::scaleZ( scale );
    Matrix b = Matrix::scaleZ( 1.0/scale ) * a;
    check_identity( b );
    a *= Matrix::scaleZ( 1.0/scale );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Point q = Matrix::scaleZ( scale ) * p;
    assert( fabs( p.x() - q.x() ) < epsilon );
    assert( fabs( p.y() - q.y() ) < epsilon );
    assert( fabs( p.z()*scale - q.z() ) < epsilon );
  }

  cerr << "Test Matrix::rotateYaw()\n";
  for ( int k=0; k<n; k++ ) {
    double yaw = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotateYaw( yaw );
    Matrix b = Matrix::rotateYaw( -yaw ) * a;
    check_identity( b );
    a *= Matrix::rotateYaw( -yaw );
    check_identity( a );

    Point p( urand(), urand(), 0.0 );
    Point q = Matrix::rotateYaw( yaw ) * p;
    assert( fabs( q.z() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
  }

  cerr << "Test Matrix::rotatePitch()\n";
  for ( int k=0; k<n; k++ ) {
    double pitch = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotatePitch( pitch );
    Matrix b = Matrix::rotatePitch( -pitch ) * a;
    check_identity( b );
    a *= Matrix::rotatePitch( -pitch );
    check_identity( a );

    Point p( urand(), 0.0, urand() );
    Point q = Matrix::rotatePitch( pitch ) * p;
    assert( fabs( q.y() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
  }

  cerr << "Test Matrix::rotateRoll()\n";
  for ( int k=0; k<n; k++ ) {
    double roll = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotateRoll( roll );
    Matrix b = Matrix::rotateRoll( -roll ) * a;
    check_identity( b );
    a *= Matrix::rotateRoll( -roll );
    check_identity( a );

    Point p( 0.0, urand(), urand() );
    Point q = Matrix::rotateRoll( roll ) * p;
    assert( fabs( q.x() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
  }

  return 0;
}
