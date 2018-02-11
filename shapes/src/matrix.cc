/*
  matrix.cc
  A 4-D matrix used for affine transformations of 3D points.

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
#include <relacs/matrix.h>


namespace relacs {


const Matrix Matrix::Identity = Matrix( 1.0, 0.0, 0.0,
					0.0, 1.0, 0.0,
					0.0, 0.0, 1.0 );

const Matrix Matrix::Zeros = Matrix( 0.0, 0.0, 0.0,
				     0.0, 0.0, 0.0,
				     0.0, 0.0, 0.0 );

const Matrix Matrix::Ones = Matrix( 1.0, 1.0, 1.0,
				    1.0, 1.0, 1.0,
				    1.0, 1.0, 1.0 );

const Matrix Matrix::ProjectXY = Matrix( 1.0, 0.0, 0.0,
					 0.0, 1.0, 0.0,
					 0.0, 0.0, 0.0 );
  
const Matrix Matrix::ProjectXZ = Matrix( 1.0, 0.0, 0.0,
					 0.0, 0.0, 1.0,
					 0.0, 0.0, 0.0 );
  
const Matrix Matrix::ProjectYZ = Matrix( 0.0, 0.0, 1.0,
					 0.0, 1.0, 0.0,
					 0.0, 0.0, 0.0 );

Matrix::Matrix( void )
{
  // identity matrix:
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      if ( i == j )
	Elems[i][j] = 1.0;
      else
	Elems[i][j] = 0.0;
    }
  }
}


Matrix::Matrix( const Matrix &m )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
}


Matrix::Matrix( const double m[3][3] )
  : Matrix()
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] = m[i][j];
  }
}

Matrix::Matrix( double a11, double a12, double a13,
		double a21, double a22, double a23,
		double a31, double a32, double a33 )
  : Matrix()
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


Matrix &Matrix::assign( const Matrix &m )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
  return *this;
}


Matrix &Matrix::operator=( const Matrix &m )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
  return *this;
}


Matrix Matrix::operator-( void ) const
{
  Matrix q( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      q.Elems[i][j] = Elems[i][j];
  }
  return q;
}


Matrix Matrix::operator+( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] += a;
  }
  return p;
}


Matrix Matrix::operator-( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] -= a;
  }
  return p;
}


Matrix Matrix::operator*( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] *= a;
  }
  return p;
}


Matrix Matrix::operator/( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      p.Elems[i][j] /= a;
  }
  return p;
}


Matrix &Matrix::operator+=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] += a;
  }
  return *this;
}


Matrix &Matrix::operator-=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] -= a;
  }
  return *this;
}


Matrix &Matrix::operator*=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] *= a;
  }
  return *this;
}


Matrix &Matrix::operator/=( double a )
{
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      Elems[i][j] /= a;
  }
  return *this;
}


Point Matrix::operator*( const Point &p ) const
{
  Point c;
  for ( int i=0; i<4; i++ ) {
    c[i] = 0.0;
    for ( int j=0; j<4; j++ )
      c[i] += Elems[i][j]*p[j];
  }
  return c;
}


Matrix Matrix::operator*( const Matrix &m ) const
{
  Matrix c;
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      c.Elems[i][j] = 0.0;
      for ( int k=0; k<4; k++ )
	c.Elems[i][j] += Elems[i][k]*m.Elems[k][j];
    }
  }
  return c;
}


Matrix &Matrix::operator*=( const Matrix &m )
{
  Matrix a( *this );
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      Elems[i][j] = 0.0;
      for ( int k=0; k<4; k++ )
	Elems[i][j] += m.Elems[i][k]*a.Elems[k][j];
    }
  }
  return *this;
}


double Matrix::detMinor( double m[3][3], int j ) const
{
  // determinant of minor of 3D matrix for row 0 and col j:
  int j0 = 0;
  if ( j == 0 )
    j0 = 1;
  int j1 = 2;
  if ( j == 2 )
    j1 = 1;
  return m[1][j0]*m[2][j1] - m[2][j0]*m[1][j1];
}


double Matrix::detMinor( int i, int j ) const
{
  // minor matrix for row i, col j:
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
  // determinant of minor matrix:
  return m[0][0]*detMinor(m, 0)
    - m[0][1]*detMinor(m, 1)
    + m[0][2]*detMinor(m, 2);
}


double Matrix::det( void ) const
{
  double d = Elems[0][0]*detMinor( 0, 0 );
  d -= Elems[0][1]*detMinor( 0, 1 );
  d += Elems[0][2]*detMinor( 0, 2 );
  d -= Elems[0][3]*detMinor( 0, 3 );
  return d;
}


Matrix Matrix::inverse( void ) const
{
  Matrix m;
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ ) {
      m.Elems[i][j] = detMinor( j, i );
      if ( (i+j)%2 == 1 )
	m.Elems[i][j] *= -1.0;
    }
  }  
  return m / det();
}


Matrix Matrix::transpose( void ) const
{
  Matrix m;
  for ( int i=0; i<4; i++ ) {
    for ( int j=0; j<4; j++ )
      m.Elems[i][j] = Elems[j][i];
  }  
  return m;
}


Matrix Matrix::translateX( double x )
{
  Matrix m;
  m.Elems[0][3] = x;
  return m;
}


Matrix Matrix::translateY( double y )
{
  Matrix m;
  m.Elems[1][3] = y;
  return m;
}


Matrix Matrix::translateZ( double z )
{
  Matrix m;
  m.Elems[2][3] = z;
  return m;
}


Matrix Matrix::translate( double x, double y, double z )
{
  Matrix m;
  m.Elems[0][3] = x;
  m.Elems[1][3] = y;
  m.Elems[2][3] = z;
  return m;
}


Matrix Matrix::translate( const Point &trans )
{
  Matrix m;
  m.Elems[0][3] = trans.x();
  m.Elems[1][3] = trans.y();
  m.Elems[2][3] = trans.z();
  return m;
}


Matrix Matrix::scaleX( double xscale )
{
  Matrix m;
  m.Elems[0][0] *= xscale;
  return m;
}


Matrix Matrix::scaleY( double yscale )
{
  Matrix m;
  m.Elems[1][1] *= yscale;
  return m;
}


Matrix Matrix::scaleZ( double zscale )
{
  Matrix m;
  m.Elems[2][2] *= zscale;
  return m;
}


Matrix Matrix::scale( double xscale, double yscale, double zscale )
{
  Matrix m;
  m.Elems[0][0] *= xscale;
  m.Elems[1][1] *= yscale;
  m.Elems[2][2] *= zscale;
  return m;
}


Matrix Matrix::scale( const Point &scale )
{
  Matrix m;
  for ( int k=0; k<3; k++ )
    m.Elems[k][k] *= scale[k];
  return m;
}


Matrix Matrix::scale( double scale )
{
  Matrix m;
  for ( int k=0; k<3; k++ )
    m.Elems[k][k] *= scale;
  return m;
}


Matrix Matrix::rotateZ( double angle )
{
  double sy = sin( angle );
  double cy = cos( angle );
  Matrix m;
  m.Elems[0][0] = cy;
  m.Elems[0][1] = -sy;
  m.Elems[1][0] = sy;
  m.Elems[1][1] = cy;
  return m;
}


Matrix Matrix::rotateY( double angle )
{
  double sp = sin( angle );
  double cp = cos( angle );
  Matrix m;
  m.Elems[0][0] = cp;
  m.Elems[0][2] = -sp;
  m.Elems[2][0] = sp;
  m.Elems[2][2] = cp;
  return m;
}


Matrix Matrix::rotateX( double angle )
{
  double sr = sin( angle );
  double cr = cos( angle );
  Matrix m;
  m.Elems[1][1] = cr;
  m.Elems[1][2] = -sr;
  m.Elems[2][1] = sr;
  m.Elems[2][2] = cr;
  return m;
}


Matrix Matrix::rotate( double anglez, double angley, double anglex )
{
  Matrix m = rotateZ( anglez );
  m *= rotateY( angley );
  m *= rotateZ( anglez );
  return m;
}


ostream &operator<<( ostream &str, const Matrix &m ) 
{
  str << "matrix ( ";
  for ( int i=0; i<4; i++ ) {
    if ( i > 0 )
      str << "       ( ";
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
