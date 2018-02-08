#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <relacs/point.h>
#include <relacs/matrix.h>

using namespace relacs;


const double epsilon = 1e-8;

const int n = 1000;

const double minscale = 1e-8;


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


Matrix random_transformation( void )
{
  int trafotype = rand()%6;
  if ( trafotype < 3 ) {
    double scale = 1.0;
    do { scale = 4.0*(urand()-0.5); } while ( fabs(scale) < minscale );
    if ( trafotype == 0 )
      return Matrix::scaleX( scale );
    else if ( trafotype == 1 )
      return Matrix::scaleY( scale );
    else
      return Matrix::scaleZ( scale );
  }
  else {
    double angle = (2.0*urand()-1.0)*M_PI;
    if ( trafotype == 4 )
      return Matrix::rotateZ( angle );
    else if ( trafotype == 5 )
      return Matrix::rotateY( angle );
    else
      return Matrix::rotateX( angle );
  }
}


void check_equality( const Matrix &a, const Matrix &b )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      assert( fabs( a( i, j ) - b( i, j ) ) < epsilon );
  }
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
  cerr << "Test assignments\n";
  for ( int k=0; k<n; k++ ) {
    Matrix a = random_matrix();
    Matrix b( a );
    check_equality( a, b );
    Matrix c;
    c = a;
    check_equality( a, c );
    Matrix d;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	d( i, j ) = a( i, j );
    }
    check_equality( a, d );
    Matrix e;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	e[i][j] = a( i, j );
    }
    check_equality( a, e );
    Matrix f;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	f( i, j ) = a[i][j];
    }
    check_equality( a, f );
    Matrix g;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	g[i][j] = a[i][j];
    }
    check_equality( a, g );
  }

  cerr << "Test Matrix::inverse()\n";
  for ( int k=0; k<n; k++ ) {
    Matrix a = random_matrix();
    Matrix b = a.inverse();
    Matrix c = a * b;
    check_identity( c );
    Matrix d = b * a;
    check_identity( d );
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
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
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
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
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
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
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

  cerr << "Test commutativity of the scale matrices:\n";
  for ( int k=0; k<n; k++ ) {
    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Matrix s[3] = { Matrix::scaleX( 4.0*urand() + 0.01 ),
		    Matrix::scaleY( 4.0*urand() + 0.01 ),
		    Matrix::scaleZ( 4.0*urand() + 0.01 ) };
    Point q0 = (s[0]*s[1]*s[2])*p;
    for ( int k=0; k<3; k++ ) {
      for ( int j=1; j<3; j++ ) {
	int jj = (k+j)%3; 
	for ( int i=0; i<3; i++ ) {
	  if ( i != k && i != (k+j)%3 ) {
	    Matrix t1 = s[k];
	    t1 *= s[jj];
	    t1 *= s[i];
	    Point q1 = t1 * p;
	    Matrix t2 = s[i];
	    t2 *= s[jj];
	    t2 *= s[k];
	    Point q2 = t2 * p;
	    Matrix t3 = s[k]*s[jj]*s[i];
	    Point q3 = t3 * p;
	    Point q4 = (s[k]*s[jj]*s[i])*p;
	    assert( q0 == q1 );
	    assert( q0 == q2 );
	    assert( q0 == q3 );
	    assert( q0 == q4 );
	  }
	}
      }
    }
  }

  cerr << "Test Matrix::rotateZ()\n";
  Point p = Point::Ones;
  for ( int k = 0; k<=4; k++ ) {
    Point q = Matrix::rotateZ( 2.0*M_PI/4.0*k ) * p;
    assert( q.z() == 1.0 );
    assert( fabs( q.y() - (2*(((k+2)/2)%2)-1) ) < epsilon );
    assert( fabs( q.x() - (2*(((k+3)/2)%2)-1) ) < epsilon );
  }
  for ( int k=0; k<n; k++ ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotateZ( angle );
    Matrix b = Matrix::rotateZ( -angle ) * a;
    check_identity( b );
    Matrix c = a * a.inverse();
    check_identity( c );
    Matrix d = a * a.transpose();
    check_identity( d );
    a *= Matrix::rotateZ( -angle );
    check_identity( a );

    Point p( urand(), urand(), 0.0 );
    Point q = Matrix::rotateZ( angle ) * p;
    assert( fabs( q.z() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
    q = Matrix::rotateZ( angle ) * Point::UnitX;
    double anglee = atan2( q.y(), q.x() );
    assert( fabs( angle - anglee ) < epsilon );
  }

  cerr << "Test Matrix::rotateY()\n";
  p = Point::Ones;
  for ( int k = 0; k<=4; k++ ) {
    Point q = Matrix::rotateY( 2.0*M_PI/4.0*k ) * p;
    assert( q.y() == 1.0 );
    assert( fabs( q.x() - (2*(((k+3)/2)%2)-1) ) < epsilon );
    assert( fabs( q.z() - (2*(((k+2)/2)%2)-1) ) < epsilon );
  }
  for ( int k=0; k<n; k++ ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotateY( angle );
    Matrix b = Matrix::rotateY( -angle ) * a;
    check_identity( b );
    Matrix c = a * a.inverse();
    check_identity( c );
    Matrix d = a * a.transpose();
    check_identity( d );
    a *= Matrix::rotateY( -angle );
    check_identity( a );

    Point p( urand(), 0.0, urand() );
    Point q = Matrix::rotateY( angle ) * p;
    assert( fabs( q.y() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
    q = Matrix::rotateY( angle ) * Point::UnitX;
    double anglee = atan2( q.z(), q.x() );
    assert( fabs( angle - anglee ) < epsilon );
  }

  cerr << "Test Matrix::rotateX()\n";
  p = Point::Ones;
  for ( int k = 0; k<=4; k++ ) {
    Point q = Matrix::rotateX( 2.0*M_PI/4.0*k ) * p;
    assert( q.x() == 1.0 );
    assert( fabs( q.y() - (2*(((k+3)/2)%2)-1) ) < epsilon );
    assert( fabs( q.z() - (2*(((k+2)/2)%2)-1) ) < epsilon );
  }
  for ( int k=0; k<n; k++ ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    Matrix a = Matrix::rotateX( angle );
    Matrix b = Matrix::rotateX( -angle ) * a;
    check_identity( b );
    Matrix c = a * a.inverse();
    check_identity( c );
    Matrix d = a * a.transpose();
    check_identity( d );
    a *= Matrix::rotateX( -angle );
    check_identity( a );

    Point p( 0.0, urand(), urand() );
    Point q = Matrix::rotateX( angle ) * p;
    assert( fabs( q.x() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
    q = Matrix::rotateX( angle ) * Point::UnitY;
    double anglee = atan2( q.z(), q.y() );
    assert( fabs( angle - anglee ) < epsilon );
  }

  cerr << "Test two sequential transformations\n";
  for ( int k=0; k<n; k++ ) {
    p = Point( urand(), urand(), urand() );
    Matrix t1 = random_transformation();
    Matrix t2 = random_transformation();
    Point q0 = t2 * t1 * p;
    Point q1 = (t2 * t1)*p;
    Point q2 = p;
    q2 *= t1;
    q2 *= t2;
    Point q3 = p;
    q3 *= t2 * t1;
    assert( (q1 - q0).magnitude() < epsilon );
    assert( (q2 - q0).magnitude() < epsilon );
    assert( (q3 - q0).magnitude() < epsilon );
  }

  cerr << "Test three sequential transformations\n";
  for ( int k=0; k<n; k++ ) {
    p = Point( urand(), urand(), urand() );
    Matrix t1 = random_transformation();
    Matrix t2 = random_transformation();
    Matrix t3 = random_transformation();
    Point q0 = t3 * t2 * t1 * p;
    Point q1 = (t3 * t2 * t1)*p;
    Point q2 = p;
    q2 *= t1;
    q2 *= t2;
    q2 *= t3;
    Point q3 = p;
    q3 *= t3 * t2 * t1;
    assert( (q1 - q0).magnitude() < epsilon );
    assert( (q2 - q0).magnitude() < epsilon );
    assert( (q3 - q0).magnitude() < epsilon );
  }

  /*
  p = Point::UnitX;
  cerr << p;

  Point q = Matrix::rotateZ( 0.25*M_PI ) * p;
  cerr << q;

  q = Matrix::rotateY( 0.25*M_PI ) * p;
  cerr << q;
  cerr << '\n';

  q = (  Matrix::rotateZ( 0.25*M_PI ) * Matrix::rotateY( 0.25*M_PI ) ) * p;  // scaleX by two then rotate
  cerr << q;
  q = p;  // scale first, then rotate
  q *= Matrix::rotateY( 0.25*M_PI );
  q *= Matrix::rotateZ( 0.25*M_PI );
  cerr << q;

  q = (  Matrix::rotateY( 0.25*M_PI ) * Matrix::rotateZ( 0.25*M_PI ) ) * p; // rotate, then scale global X
  cerr << q;
  cerr << '\n';

  return 0;

  q = ( Matrix::scaleY( 4.0 ) *  Matrix::rotateZ( 0.25*M_PI ) * Matrix::scaleX( 2.0 ) ) * p;  // scaleX by two then rotate then scaleY by four
  cerr << q;
  q = p;  // scale x first, then rotate, then scale Y
  q *= Matrix::scaleX( 2.0 );
  q *= Matrix::rotateZ( 0.25*M_PI );
  q *= Matrix::scaleY( 4.0 );
  cerr << q;

  q = (  Matrix::scaleX( 2.0 ) * Matrix::rotateZ( 0.25*M_PI ) * Matrix::scaleY( 4.0 ) ) * p; // scale Y, rotate, then scale global X
  cerr << q;
  */
}
