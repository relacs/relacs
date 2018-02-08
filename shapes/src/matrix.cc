/*
  matrix.cc
  A matrix in 3D space.

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
#include <relacs/str.h>


namespace relacs {


Matrix::Matrix( void )
{
  // identity matrix:
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ ) {
      if ( i == j )
	Elems[i][j] = 1.0;
      else
	Elems[i][j] = 0.0;
    }
  }
}


Matrix::Matrix( const Matrix &m )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
}


Matrix::Matrix( const double m[3][3] )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] = m[i][j];
  }
}


Matrix &Matrix::assign( const Matrix &m )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
  return *this;
}


Matrix &Matrix::operator=( const Matrix &m )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] = m.Elems[i][j];
  }
  return *this;
}


Matrix Matrix::operator-( void ) const
{
  Matrix q( *this );
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      q.Elems[i][j] = Elems[i][j];
  }
  return q;
}


Matrix Matrix::operator+( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      p.Elems[i][j] += a;
  }
  return p;
}


Matrix Matrix::operator-( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      p.Elems[i][j] -= a;
  }
  return p;
}


Matrix Matrix::operator*( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      p.Elems[i][j] *= a;
  }
  return p;
}


Matrix Matrix::operator/( double a ) const
{
  Matrix p( *this );
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      p.Elems[i][j] /= a;
  }
  return p;
}


Matrix &Matrix::operator+=( double a )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] += a;
  }
  return *this;
}


Matrix &Matrix::operator-=( double a )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] -= a;
  }
  return *this;
}


Matrix &Matrix::operator*=( double a )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] *= a;
  }
  return *this;
}


Matrix &Matrix::operator/=( double a )
{
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      Elems[i][j] /= a;
  }
  return *this;
}


Point Matrix::operator*( const Point &p ) const
{
  Point c;
  for ( int i=0; i<3; i++ ) {
    c[i] = 0.0;
    for ( int j=0; j<3; j++ )
      c[i] += Elems[i][j]*p[j];
  }
  return c;
}


Matrix Matrix::operator*( const Matrix &m ) const
{
  Matrix c;
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ ) {
      c.Elems[i][j] = 0.0;
      for ( int k=0; k<3; k++ )
	c.Elems[i][j] += Elems[i][k]*m.Elems[k][j];
    }
  }
  return c;
}


Matrix &Matrix::operator*=( const Matrix &m )
{
  Matrix a( *this );
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ ) {
      Elems[i][j] = 0.0;
      for ( int k=0; k<3; k++ )
	Elems[i][j] += m.Elems[i][k]*a.Elems[k][j];
    }
  }
  return *this;
}


double Matrix::detMinor( int i, int j ) const
{
  // rows and cols of minors:
  int ii[2] = { i+1, i+2 };
  int jj[2] = { j+1, j+2 };
  for ( int k=0; k<2; k++ ) {
    if ( ii[k] >= 3 )
      ii[k] -= 3;
    if ( jj[k] >= 3 )
      jj[k] -= 3;
  }
  // order them:
  if ( ii[0] > ii[1] ) {
    int s = ii[0];
    ii[0] = ii[1];
    ii[1] = s;
  }
  if ( jj[0] > jj[1] ) {
    int s = jj[0];
    jj[0] = jj[1];
    jj[1] = s;
  }
  // determinant of 2-D minor:
  return Elems[ii[0]][jj[0]]*Elems[ii[1]][jj[1]] - Elems[ii[0]][jj[1]]*Elems[ii[1]][jj[0]];
}


double Matrix::det( void ) const
{
  double d = Elems[0][0]*detMinor(0, 0);
  d -= Elems[0][1]*detMinor(0, 1);
  d += Elems[0][2]*detMinor(0, 2);
  return d;
}


Matrix Matrix::inverse( void ) const
{
  Matrix m;
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ ) {
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
  for ( int i=0; i<3; i++ ) {
    for ( int j=0; j<3; j++ )
      m.Elems[i][j] = Elems[j][i];
  }  
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
  for ( int i=0; i<3; i++ ) {
    if ( i > 0 )
      str << "       ( ";
    for ( int j=0; j<3; j++ ) {
      str << setw( 10 ) << m.Elems[i][j];
      if ( j < 2 )
	str << " ";
    }
    str << " )\n";
  }
  return str;
}


}; /* namespace relacs */
