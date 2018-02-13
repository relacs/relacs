#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <relacs/point.h>
#include <relacs/transform.h>

using namespace relacs;


const double epsilon = 1e-8;

const int n = 1000;

const double minscale = 1e-8;


double urand( void )
{
  return (double)rand()/double(RAND_MAX);
}


Transform random_matrix( void )
{
  Transform m;
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      m( i, j ) = urand();
  }
  return m;
}


Transform random_transformation( void )
{
  Transform m;
  int trafotype = rand()%9;
  if ( trafotype < 3 ) {
    double scale = 1.0;
    do { scale = 4.0*(urand()-0.5); } while ( fabs(scale) < minscale );
    if ( trafotype == 0 )
      return m.scaleX( scale );
    else if ( trafotype == 1 )
      return m.scaleY( scale );
    else
      return m.scaleZ( scale );
  }
  else if ( trafotype < 6 ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    if ( trafotype == 4 )
      return m.rotateZ( angle );
    else if ( trafotype == 5 )
      return m.rotateY( angle );
    else
      return m.rotateX( angle );
  }
  else {
    double shift = 4.0*(urand()-0.5);
    if ( trafotype == 7 )
      return m.translateX( shift );
    else if ( trafotype == 8 )
      return m.translateY( shift );
    else
      return m.translateZ( shift );
  }
}


void check_equality( const Transform &a, const Transform &b )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      assert( fabs( a( i, j ) - b( i, j ) ) < epsilon );
  }
}


void check_identity( const Transform &m )
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
    Transform a = random_matrix();
    Transform b( a );
    check_equality( a, b );
    Transform c;
    c = a;
    check_equality( a, c );
    Transform d;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	d( i, j ) = a( i, j );
    }
    check_equality( a, d );
    Transform e;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	e[i][j] = a( i, j );
    }
    check_equality( a, e );
    Transform f;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	f( i, j ) = a[i][j];
    }
    check_equality( a, f );
    Transform g;
    for ( int i=0; i<3; i++ ) {
      for ( int j=0; j<3; j++ )
	g[i][j] = a[i][j];
    }
    check_equality( a, g );
  }

  cerr << "Test Transform::inverse()\n";
  for ( int k=0; k<n; k++ ) {
    Transform a = random_matrix();
    Transform b = a.inverse();
    Point p( urand(), urand(), urand() );
    Point q = a * p;
    q *= b;
    assert( (p-q).magnitude() < epsilon );
    Transform c = a * b;
    check_identity( c );
    Transform d = b * a;
    check_identity( d );
    a *= b;
    check_identity( a );
  }

  cerr << "Test Transform::transpose()\n";
  for ( int k=0; k<n; k++ ) {
    Transform a = random_matrix();
    Transform b = a.transpose();
    Transform c = b.transpose();
    check_equality( a, c );
    c = b.inverse();
    b = a.inverse();
    Transform d = b.transpose();
    check_equality( c, d );
    check_equality( a, a.transpose().transpose() );
    check_equality( a.inverse().transpose(), a.transpose().inverse() );
  }

  cerr << "Test Transform::translateX()\n";
  for ( int k=0; k<n; k++ ) {
    double shift = 10.0*(urand()-0.5);
    Transform a;
    a.translateX( shift );
    Transform b;
    b.translateX( -shift ) * a;
    check_identity( b );
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
    check_identity( b );
    Transform c;
    a *= c.translateX( -shift );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Transform d;
    Point q = d.translateX( shift ) * p;
    assert( fabs( p.x()+shift - q.x() ) < epsilon );
    assert( fabs( p.y() - q.y() ) < epsilon );
    assert( fabs( p.z() - q.z() ) < epsilon );
  }

  cerr << "Test Transform::translateY()\n";
  for ( int k=0; k<n; k++ ) {
    double shift = 10.0*(urand()-0.5);
    Transform a;
    a.translateY( shift );
    Transform b;
    b.translateY( -shift ) * a;
    check_identity( b );
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
    check_identity( b );
    Transform c;
    a *= c.translateY( -shift );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Transform d;
    Point q = d.translateY( shift ) * p;
    assert( fabs( p.x() - q.x() ) < epsilon );
    assert( fabs( p.y()+shift - q.y() ) < epsilon );
    assert( fabs( p.z() - q.z() ) < epsilon );
  }

  cerr << "Test Transform::translateZ()\n";
  for ( int k=0; k<n; k++ ) {
    double shift = 10.0*(urand()-0.5);
    Transform a;
    a.translateZ( shift );
    Transform b;
    b.translateZ( -shift ) * a;
    check_identity( b );
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
    check_identity( b );
    Transform c;
    a *= c.translateZ( -shift );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Transform d;
    Point q = d.translateZ( shift ) * p;
    assert( fabs( p.x() - q.x() ) < epsilon );
    assert( fabs( p.y() - q.y() ) < epsilon );
    assert( fabs( p.z()+shift - q.z() ) < epsilon );
  }

  cerr << "Test Transform::scaleX()\n";
  for ( int k=0; k<n; k++ ) {
    double scale = 10.0*(urand()-0.5);
    if ( fabs( scale ) < epsilon )
      continue;
    Transform a;
    a.scaleX( scale );
    Transform b;
    b = b.scaleX( 1.0/scale ) * a;
    check_identity( b );
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
    check_identity( b );
    Transform c;
    a *= c.scaleX( 1.0/scale );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Transform d;
    Point q = d.scaleX( scale ) * p;
    assert( fabs( p.x()*scale - q.x() ) < epsilon );
    assert( fabs( p.y() - q.y() ) < epsilon );
    assert( fabs( p.z() - q.z() ) < epsilon );
  }

  cerr << "Test Transform::scaleY()\n";
  for ( int k=0; k<n; k++ ) {
    double scale = 10.0*(urand()-0.5);
    if ( fabs( scale ) < epsilon )
      continue;
    Transform a;
    a.scaleY( scale );
    Transform b;
    b = b.scaleY( 1.0/scale ) * a;
    check_identity( b );
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
    check_identity( b );
    Transform c;
    a *= c.scaleY( 1.0/scale );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Transform d;
    Point q = d.scaleY( scale ) * p;
    assert( fabs( p.x() - q.x() ) < epsilon );
    assert( fabs( p.y()*scale - q.y() ) < epsilon );
    assert( fabs( p.z() - q.z() ) < epsilon );
  }

  cerr << "Test Transform::scaleZ()\n";
  for ( int k=0; k<n; k++ ) {
    double scale = 10.0*(urand()-0.5);
    if ( fabs( scale ) < epsilon )
      continue;
    Transform a;
    a.scaleZ( scale );
    Transform b;
    b = b.scaleZ( 1.0/scale ) * a;
    check_identity( b );
    b = a.inverse() * a;
    check_identity( b );
    b = a * a.inverse();
    check_identity( b );
    Transform c;
    a *= c.scaleZ( 1.0/scale );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Transform d;
    Point q = d.scaleZ( scale ) * p;
    assert( fabs( p.x() - q.x() ) < epsilon );
    assert( fabs( p.y() - q.y() ) < epsilon );
    assert( fabs( p.z()*scale - q.z() ) < epsilon );
  }

  cerr << "Test commutativity of the scale matrices:\n";
  for ( int k=0; k<n; k++ ) {
    Point p( urand(), urand(), urand() );
    p -= 0.5;
    p *= 20.0;
    Transform s[3];
    s[0].scaleX( 4.0*urand() + 0.01 );
    s[1].scaleY( 4.0*urand() + 0.01 );
    s[2].scaleZ( 4.0*urand() + 0.01 );
    Point q0 = (s[0]*s[1]*s[2])*p;
    for ( int k=0; k<3; k++ ) {
      for ( int j=1; j<3; j++ ) {
	int jj = (k+j)%3; 
	for ( int i=0; i<3; i++ ) {
	  if ( i != k && i != (k+j)%3 ) {
	    Transform t1 = s[k];
	    t1 *= s[jj];
	    t1 *= s[i];
	    Point q1 = t1 * p;
	    Transform t2 = s[i];
	    t2 *= s[jj];
	    t2 *= s[k];
	    Point q2 = t2 * p;
	    Transform t3 = s[k]*s[jj]*s[i];
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

  cerr << "Test Transform::rotateX()\n";
  Point p = Point::Ones;
  for ( int k = 0; k<=4; k++ ) {
    Transform a;
    Point q = a.rotateX( 2.0*M_PI/4.0*k ) * p;
    assert( q.x() == 1.0 );
    assert( fabs( q.y() - (2*(((k+3)/2)%2)-1) ) < epsilon );
    assert( fabs( q.z() - (2*(((k+2)/2)%2)-1) ) < epsilon );
  }
  for ( int k=0; k<n; k++ ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    Transform a;
    a.rotateX( angle );
    assert( fabs( fabs( a.det() ) - 1.0 ) < epsilon );
    Transform b;
    b = b.rotateX( -angle ) * a;
    check_identity( b );
    Transform c = a * a.inverse();
    check_identity( c );
    Transform d = a * a.transpose();
    check_identity( d );
    Transform e;
    a *= e.rotateX( -angle );
    check_identity( a );

    Point p( 0.0, urand(), urand() );
    Transform f;
    Point q = f.rotateX( angle ) * p;
    assert( fabs( q.x() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
    Transform g;
    q = g.rotateX( angle ) * Point::UnitY;
    double anglee = atan2( q.z(), q.y() );
    assert( fabs( angle - anglee ) < epsilon );
  }

  cerr << "Test Transform::rotateY()\n";
  p = Point::Ones;
  for ( int k = 0; k<=4; k++ ) {
    Transform a;
    Point q = a.rotateY( 2.0*M_PI/4.0*k ) * p;
    assert( q.y() == 1.0 );
    assert( fabs( q.x() - (2*(((k+3)/2)%2)-1) ) < epsilon );
    assert( fabs( q.z() - (2*(((k+2)/2)%2)-1) ) < epsilon );
  }
  for ( int k=0; k<n; k++ ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    Transform a;
    a.rotateY( angle );
    assert( fabs( fabs( a.det() ) - 1.0 ) < epsilon );
    Transform b;
    b = b.rotateY( -angle ) * a;
    check_identity( b );
    Transform c = a * a.inverse();
    check_identity( c );
    Transform d = a * a.transpose();
    check_identity( d );
    Transform e;
    a *= e.rotateY( -angle );
    check_identity( a );

    Point p( urand(), 0.0, urand() );
    Transform f;
    Point q = f.rotateY( angle ) * p;
    assert( fabs( q.y() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
    Transform g;
    q = g.rotateY( angle ) * Point::UnitX;
    double anglee = atan2( q.z(), q.x() );
    assert( fabs( angle - anglee ) < epsilon );
  }

  cerr << "Test Transform::rotateZ()\n";
  p = Point::Ones;
  for ( int k = 0; k<=4; k++ ) {
    Transform a;
    Point q = a.rotateZ( 2.0*M_PI/4.0*k ) * p;
    assert( q.z() == 1.0 );
    assert( fabs( q.y() - (2*(((k+2)/2)%2)-1) ) < epsilon );
    assert( fabs( q.x() - (2*(((k+3)/2)%2)-1) ) < epsilon );
  }
  for ( int k=0; k<n; k++ ) {
    double angle = (2.0*urand()-1.0)*M_PI;
    Transform a;
    a.rotateZ( angle );
    assert( fabs( fabs( a.det() ) - 1.0 ) < epsilon );
    Transform b;
    b = b.rotateZ( -angle ) * a;
    check_identity( b );
    Transform c = a * a.inverse();
    check_identity( c );
    Transform d = a * a.transpose();
    check_identity( d );
    Transform e;
    a *= e.rotateZ( -angle );
    check_identity( a );

    Point p( urand(), urand(), 0.0 );
    Transform f;
    Point q = f.rotateZ( angle ) * p;
    assert( fabs( q.z() ) < epsilon );
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
    Transform g;
    q = g.rotateZ( angle ) * Point::UnitX;
    double anglee = atan2( q.y(), q.x() );
    assert( fabs( angle - anglee ) < epsilon );
  }

  cerr << "Test Transform::rotate(axis, angle)\n";
  for ( int k=0; k<n; k++ ) {
    double angle = 2.0*urand()*M_PI;
    Point axis( urand(), urand(), urand() );
    Transform a;
    a.rotate( axis, angle );
    assert( fabs( fabs( a.det() ) - 1.0 ) < epsilon );
    Transform b;
    b = b.rotate( axis, -angle ) * a;
    check_identity( b );
    Transform b1;
    b1 = b1.rotate( -axis, angle ) * a;
    check_identity( b );
    Transform c = a * a.inverse();
    check_identity( c );
    Transform d = a * a.transpose();
    check_identity( d );
    Transform e;
    a *= e.rotate( axis, -angle );
    check_identity( a );

    Point p( urand(), urand(), urand() );
    Transform f;
    Point q = f.rotate( axis, angle ) * p;
    assert( fabs( p.magnitude() - q.magnitude() ) < epsilon );
    Point pa = axis + p;
    Point pb = axis.cross( pa );
    Point pc = f * pb;
    double anglee = acos( pb.dot( pc )/pb.magnitude()/pc.magnitude() );
    if ( angle > M_PI )
      angle = 2*M_PI - angle;
    assert( fabs( angle - anglee ) < epsilon );
  }

  cerr << "Test two sequential transformations\n";
  for ( int k=0; k<n; k++ ) {
    p = Point( urand(), urand(), urand() );
    Transform t1 = random_transformation();
    Transform t2 = random_transformation();
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
    Transform t1 = random_transformation();
    Transform t2 = random_transformation();
    Transform t3 = random_transformation();
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

  cerr << "Test transformation of planes and their normals\n";
  for ( int k=0; k<n; k++ ) {
    Point p1 = Point( urand(), urand(), urand() );
    Point p2 = Point( urand(), urand(), urand() );
    Point n = p1.cross( p2 );
    assert( ::fabs( n.dot( p1 ) ) < epsilon );
    assert( ::fabs( n.dot( p2 ) ) < epsilon );
    Transform t1 = random_transformation();
    t1.clearTransProj();
    Transform t2 = t1.inverse().transpose();
    Point tp1 = t1 * p1;
    Point tp2 = t1 * p2;
    Point tn = t2 * n;
    assert( ::fabs( tn.dot( tp1 ) ) < epsilon );
    assert( ::fabs( tn.dot( tp2 ) ) < epsilon );
  }

  /*
  p = Point::UnitX;
  cerr << p;

  Point q = Transform::rotateZ( 0.25*M_PI ) * p;
  cerr << q;

  q = Transform::rotateY( 0.25*M_PI ) * p;
  cerr << q;
  cerr << '\n';

  q = (  Transform::rotateZ( 0.25*M_PI ) * Transform::rotateY( 0.25*M_PI ) ) * p;  // scaleX by two then rotate
  cerr << q;
  q = p;  // scale first, then rotate
  q *= Transform::rotateY( 0.25*M_PI );
  q *= Transform::rotateZ( 0.25*M_PI );
  cerr << q;

  q = (  Transform::rotateY( 0.25*M_PI ) * Transform::rotateZ( 0.25*M_PI ) ) * p; // rotate, then scale global X
  cerr << q;
  cerr << '\n';

  return 0;

  q = ( Transform::scaleY( 4.0 ) *  Transform::rotateZ( 0.25*M_PI ) * Transform::scaleX( 2.0 ) ) * p;  // scaleX by two then rotate then scaleY by four
  cerr << q;
  q = p;  // scale x first, then rotate, then scale Y
  q *= Transform::scaleX( 2.0 );
  q *= Transform::rotateZ( 0.25*M_PI );
  q *= Transform::scaleY( 4.0 );
  cerr << q;

  q = (  Transform::scaleX( 2.0 ) * Transform::rotateZ( 0.25*M_PI ) * Transform::scaleY( 4.0 ) ) * p; // scale Y, rotate, then scale global X
  cerr << q;
  */
}
