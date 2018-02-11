/*
  matrix.h
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

#ifndef _RELACS_MATRIX_H
#define _RELACS_MATRIX_H 1

#include <iostream>
#include <relacs/point.h>
using namespace std;

namespace relacs {


/*!
\class Matrix
\brief A 4-D matrix used for affine transformations of 3D points.
\author Jan Benda, Fabian Sinz
 */

class Matrix {

public:

    /*! Identity matrix. */
  static const Matrix Identity;
    /*! Matrix with all elements zero. */
  static const Matrix Zeros;
    /*! Matrix with all elements ones. */
  static const Matrix Ones;

    /*! Projection matrix that projects onto the x-y plane. */
  static const Matrix ProjectXY;
    /*! Projection matrix that projects onto the x-z plane. */
  static const Matrix ProjectXZ;
    /*! Projection matrix that projects onto the y-z plane. */
  static const Matrix ProjectYZ;

    /*! Constructor. */
  Matrix( void );
    /*! Copy constructor. */
  Matrix( const Matrix &p );
    /*! A matrix with elements copied from \a m. All other elements
        are set to 0 and a44 is set ot 1. */
  Matrix( const double m[3][3] );
    /*! A matrix defined by its elements \a a_i,j. All other elements
        are set to 0 and a44 is set ot 1. */
  Matrix( double a11, double a12, double a13,
	  double a21, double a22, double a23,
	  double a31, double a32, double a33 );

    /*! The element (i, j) of the matrix. */
  double operator()( int i, int j ) const { return Elems[i][j]; }
    /*! The element (i, j) of the matrix. */
  double &operator()( int i, int j ) { return Elems[i][j]; }

    /*! The i-th row of the matrix. */
  const double*  operator[]( int i ) const { return Elems[i]; }
    /*! The i-th row of the matrix. */
  double* operator[]( int i ) { return Elems[i]; }

    /*! Assign elements of matrix \a m to this matrix. */
  Matrix &assign( const Matrix &m );
    /*! Assign elements of matrix \a m to this matrix. */
  Matrix &operator=( const Matrix &m );

    /*! Return the negative of all elements. */
  Matrix operator-( void ) const;

    /*! Add \a a to all elements of this matrix. */
  Matrix operator+( double a ) const;
    /*! Subtract \a a from all elements of this matrix. */
  Matrix operator-( double a ) const;
    /*! Multiply all elements of this matrix with \a a. */
  Matrix operator*( double a ) const;
    /*! Divide all elements of this matrix by \a a. */
  Matrix operator/( double a ) const;

    /*! Add \a a to all elements of this matrix. */
  Matrix &operator+=( double a );
    /*! Subtract \a a from all elements of this matrix. */
  Matrix &operator-=( double a );
    /*! Multiply all elements of this matrix with \a a. */
  Matrix &operator*=( double a );
    /*! Divide all elements of this matrix by \a a. */
  Matrix &operator/=( double a );

    /*! Matrix multiplication of this matrix with point \a p. */
  Point operator*( const Point &p ) const;
    /*! Matrix multiplication of this matrix with matrix \a m. */
  Matrix operator*( const Matrix &m ) const;
    /*! Apply transformation matrix \a m to this matrix.
        That is compute matrix \a m multiplied with this matrix.
        \note \a m is multiplied from the left, not from the right! */
  Matrix &operator*=( const Matrix &m );

    /*! Return the determinant of the matrix. */
  double det( void ) const;
    /*! Return the inverse matrix. */
  Matrix inverse( void ) const;
    /*! Return the transposed matrix. */
  Matrix transpose( void ) const;

    /*! Return the transformation matrix that translates along the x-axis by \a x. */
  static Matrix translateX( double x );
    /*! Return the transformation matrix that translates along the y-axis by \a y. */
  static Matrix translateY( double y );
    /*! Return the transformation matrix that translates along the z-axis by \a z. */
  static Matrix translateZ( double z );
    /*! Return the transformation matrix that translates along the x-, y-, and
        z-axis by \a x, \a y, \a z, respectively. */
  static Matrix translate( double x, double y, double z );
    /*! Return the transformation matrix that translates the x-, y-, and
        z-axis by \a trans. */
  static Matrix translate( const Point &trans );

    /*! Return the transformation matrix that scales the x-axis by \a xscale. */
  static Matrix scaleX( double xscale );
    /*! Return the transformation matrix that scales the y-axis by \a yscale. */
  static Matrix scaleY( double yscale );
    /*! Return the transformation matrix that scales the z-axis by \a zscale. */
  static Matrix scaleZ( double zscale );
    /*! Return the transformation matrix that scales the x-, y-, and
        z-axis by \a xscale, \a yscale, \a zscale, respectively. */
  static Matrix scale( double xscale, double yscale, double zscale );
    /*! Return the transformation matrix that scales the x-, y-, and
        z-axis by \a scale. */
  static Matrix scale( const Point &scale );
    /*! Return the transformation matrix that scales the x-, y-, and
        z-axis uniformly by \a scale. */
  static Matrix scale( double scale );

    /*! Return the transformation matrix that rotates counterclockwise
        around the world z-axis by \a angle radians. */
  static Matrix rotateZ( double angle );
    /*! Return the transformation matrix that rotates counterclockwise
        around the world y-axis by \a angle radians. */
  static Matrix rotateY( double angle );
    /*! Return the transformation matrix that rotates counterclockwise
        around the world y-axis by \a angle radians. */
  static Matrix rotateX( double angle );
    /*! Return the transformation matrix that rotates counterclockwise
        around the world z-axis by \a anglez, then around the world y-axis by \a
        angley, and then around the world x-axis by \a world x. All
        angles in radians. */
  static Matrix rotate( double anglez, double angley, double anglex );

    /*! Write the elements of the matrix to stream \a str. */
  friend ostream &operator<<( ostream &str, const Matrix &m );


protected:

    /*! Comput determinant of minor of 3D matrix for row 0 and column \a j. */
  double detMinor( double m[3][3], int j ) const;
    /*! Return the determinant of the minor matrix for (i, j). */
  double detMinor( int i, int j ) const;

    /*! The the matrix elements. */
  double Elems[4][4];

};


}; /* namespace relacs */

#endif /* ! _RELACS_MATRIX_H_ */
