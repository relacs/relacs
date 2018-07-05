/*
  transform.cc
  A 4-D matrix used for affine and perspective transformations of 3D points.

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
#include <iomanip>
#include <relacs/transform.h>


namespace relacs {


const Transform Transform::Identity = Transform( 1.0, 0.0, 0.0,
						 0.0, 1.0, 0.0,
						 0.0, 0.0, 1.0 );

const Transform Transform::Zeros = Transform( 0.0, 0.0, 0.0,
					      0.0, 0.0, 0.0,
					      0.0, 0.0, 0.0 );

const Transform Transform::Ones = Transform( 1.0, 1.0, 1.0,
					     1.0, 1.0, 1.0,
					     1.0, 1.0, 1.0 );


Transform::Transform( void )
{
  clear();
}


Transform::Transform( const Transform &m )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
}


Transform::Transform( const double m[3][3] )
  : Transform()
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] = m[i][j];
  }
}

Transform::Transform( double a11, double a12, double a13,
		      double a21, double a22, double a23,
		      double a31, double a32, double a33 )
  : Transform()
{
  Elems[0][0] = a11;
  Elems[0][1] = a12;
  Elems[0][2] = a13;
  Elems[1][0] = a21;
  Elems[1][1] = a22;
  Elems[1][2] = a23;
  Elems[2][0] = a31;
  Elems[2][1] = a32;
  Elems[2][2] = a33;
}


Transform &Transform::assign( const Transform &m )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
  return *this;
}


Transform &Transform::operator=( const Transform &m )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
  return *this;
}


Transform Transform::operator-( void ) const
{
  Transform q( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      q.Elems[i][j] = Elems[i][j];
  }
  return q;
}


Transform Transform::operator+( double a ) const
{
  Transform p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] += a;
  }
  return p;
}


Transform Transform::operator-( double a ) const
{
  Transform p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] -= a;
  }
  return p;
}


Transform Transform::operator*( double a ) const
{
  Transform p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] *= a;
  }
  return p;
}


Transform Transform::operator/( double a ) const
{
  Transform p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] /= a;
  }
  return p;
}


Transform &Transform::operator+=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] += a;
  }
  return *this;
}


Transform &Transform::operator-=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] -= a;
  }
  return *this;
}


Transform &Transform::operator*=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] *= a;
  }
  return *this;
}


Transform &Transform::operator/=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] /= a;
  }
  return *this;
}


Point Transform::operator*( const Point &p ) const
{
  Point c;
  for ( int i=0; i<4; i++ ) {
    c[i] = 0.0;
    for ( int j=0; j<4; j++ )
      c[i] += Elems[i][j]*p[j];
  }
  return c;
}


Transform Transform::operator*( const Transform &m ) const
{
  Transform c;
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      c.Elems[i][j] = 0.0;
      for ( int k=0; k<4; k++ )
	c.Elems[i][j] += Elems[i][k]*m.Elems[k][j];
    }
  }
  return c;
}


Transform &Transform::operator*=( const Transform &m )
{
  Transform a( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      Elems[i][j] = 0.0;
      for ( int k=0; k<4; k++ )
	Elems[i][j] += m.Elems[i][k]*a.Elems[k][j];
    }
  }
  return *this;
}


double Transform::detMinor( double m[3][3], int j ) const
{
  // determinant of minor of 3D transform for row 0 and col j:
  int j0 = 0;
  if ( j == 0 )
    j0 = 1;
  int j1 = 2;
  if ( j == 2 )
    j1 = 1;
  return m[1][j0]*m[2][j1] - m[2][j0]*m[1][j1];
}


double Transform::detMinor( int i, int j ) const
{
  // minor transform for row i, col j:
  double m[3][3];
  int iidx = 0;
  for ( int ki=0; ki<4; ki++ ) {
    if ( ki != i ) {
      int jidx = 0;
      for ( int kj=0; kj<4; kj++ ) {
	if ( kj != j )
	  m[iidx][jidx++] = Elems[ki][kj];
      }
      iidx++;
    }
  }
  // determinant of minor transform:
  return m[0][0]*detMinor(m, 0)
    - m[0][1]*detMinor(m, 1)
    + m[0][2]*detMinor(m, 2);
}


double Transform::det( void ) const
{
  double d = Elems[0][0]*detMinor( 0, 0 );
  d -= Elems[0][1]*detMinor( 0, 1 );
  d += Elems[0][2]*detMinor( 0, 2 );
  d -= Elems[0][3]*detMinor( 0, 3 );
  return d;
}


Transform Transform::inverse( void ) const
{
  Transform m;
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      m.Elems[i][j] = detMinor( j, i );
      if ( (i+j)%2 == 1 )
	m.Elems[i][j] *= -1.0;
    }
  }
  return m / det();
}


Transform Transform::transpose( void ) const
{
  Transform m;
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      m.Elems[i][j] = Elems[j][i];
  }
  return m;
}


void Transform::clear( void )
{
  // identity transform:
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      if ( i == j )
	Elems[i][j] = 1.0;
      else
	Elems[i][j] = 0.0;
    }
  }
}


void Transform::clearTransProj( void )
{
  for ( int i=0; i<3; i++ ) {
    Elems[i][3] = 0.0;
    Elems[3][i] = 0.0;
  }
  Elems[3][3] = 1.0;
}


Transform &Transform::translateX( double x )
{
  Transform m;
  m.Elems[0][3] = x;
  return operator*=( m );
}


Transform &Transform::translateY( double y )
{
  Transform m;
  m.Elems[1][3] = y;
  return operator*=( m );
}


Transform &Transform::translateZ( double z )
{
  Transform m;
  m.Elems[2][3] = z;
  return operator*=( m );
}


Transform &Transform::translate( double x, double y, double z )
{
  Transform m;
  m.Elems[0][3] = x;
  m.Elems[1][3] = y;
  m.Elems[2][3] = z;
  return operator*=( m );
}


Transform &Transform::translate( const Point &trans )
{
  Transform m;
  m.Elems[0][3] = trans.x();
  m.Elems[1][3] = trans.y();
  m.Elems[2][3] = trans.z();
  return operator*=( m );
}


Transform &Transform::scaleX( double xscale )
{
  Transform m;
  m.Elems[0][0] = xscale;
  return operator*=( m );
}


Transform &Transform::scaleY( double yscale )
{
  Transform m;
  m.Elems[1][1] = yscale;
  return operator*=( m );
}


Transform &Transform::scaleZ( double zscale )
{
  Transform m;
  m.Elems[2][2] = zscale;
  return operator*=( m );
}


Transform &Transform::scale( double xscale, double yscale, double zscale )
{
  Transform m;
  m.Elems[0][0] = xscale;
  m.Elems[1][1] = yscale;
  m.Elems[2][2] = zscale;
  return operator*=( m );
}


Transform &Transform::scale( const Point &scale )
{
  Transform m;
  for ( int k=0; k<3; k++ )
    m.Elems[k][k] = scale[k];
  return operator*=( m );
}


Transform &Transform::scale( double scale )
{
  Transform m;
  for ( int k=0; k<3; k++ )
    m.Elems[k][k] = scale;
  return operator*=( m );
}


Transform &Transform::rotateX( double angle )
{
  double sr = sin( angle );
  double cr = cos( angle );
  Transform m;
  m.Elems[1][1] = cr;
  m.Elems[1][2] = -sr;
  m.Elems[2][1] = sr;
  m.Elems[2][2] = cr;
  return operator*=( m );
}


Transform &Transform::rotateY( double angle )
{
  double sp = sin( angle );
  double cp = cos( angle );
  Transform m;
  m.Elems[0][0] = cp;
  m.Elems[0][2] = -sp;
  m.Elems[2][0] = sp;
  m.Elems[2][2] = cp;
  return operator*=( m );
}


Transform &Transform::rotateZ( double angle )
{
  double sy = sin( angle );
  double cy = cos( angle );
  Transform m;
  m.Elems[0][0] = cy;
  m.Elems[0][1] = -sy;
  m.Elems[1][0] = sy;
  m.Elems[1][1] = cy;
  return operator*=( m );
}


Transform &Transform::rotate( double anglex, double angley, double anglez )
{
  Transform m = rotateX( anglex );
  m *= rotateY( angley );
  m *= rotateZ( anglez );
  return operator*=( m );
}


Transform &Transform::rotate( const Point &axis, double angle )
{
  double m = axis.magnitude();
  double x = axis.x()/m;
  double y = axis.y()/m;
  double z = axis.z()/m;
  double s = sin( angle );
  double c = cos( angle );
  double c1 = 1.0 - c;
  Transform t( c+x*x*c1, x*y*c1-z*s, x*z*c1+y*s,
	       x*y*c1+z*s, c+y*y*c1, y*z*c1-x*s,
	       x*z*c1-y*s, y*z*c1+x*s, c+z*z*c1 );
  return operator*=( t );
}


Transform &Transform::rotate( const Point &from, const Point &to )
{
  double angle = ::acos( from.dot( to ) / from.magnitude() / to.magnitude() );
  if ( ::fabs( angle ) < 1e-8 )
    return *this;
  else {
    Point axis = from.cross( to );
    return rotate( axis, angle );
  }
}


Transform &Transform::perspectiveX( double distance )
{
  Transform m;
  m.Elems[3][0] = 1.0/distance;
  return operator*=( m );
}


Transform &Transform::perspectiveY( double distance )
{
  Transform m;
  m.Elems[3][1] = 1.0/distance;
  return operator*=( m );
}


Transform &Transform::perspectiveZ( double distance )
{
  Transform m;
  m.Elems[3][2] = 1.0/distance;
  return operator*=( m );
}


ostream &operator<<( ostream &str, const Transform &m ) 
{
  str << "transform ( ";
  for ( int i=0; i<4; i++ ) {
    if ( i > 0 )
      str << "          ( ";
    for ( int j=0; j<4; j++ ) {
      str << setw( 10 ) << m.Elems[i][j];
      if ( j < 3 )
	str << " ";
    }
    str << " )\n";
  }
  return str;
}


}; /* namespace relacs */
