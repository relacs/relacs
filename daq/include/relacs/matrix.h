/*
  matrix.h
  A 3-D matrix.

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
\brief A 3-D matrix.
\author Jan Benda, Fabian Sinz
 */

class Matrix {

public:

    /*! Constructor. */
  Matrix( void );
    /*! Copy constructor. */
  Matrix( const Matrix &p );
    /*! A matrix with elements copied from \a m. */
  Matrix( const double m[3][3] );

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
    /*! Matrix multiplication of this matrix with matrix \a m. */
  Matrix &operator*=( const Matrix &m );

    /*! Return the determinant of the minor matrix for (i, j). */
  double detMinor( int i, int j ) const;
    /*! Return the determinant of the matrix. */
  double det( void ) const;
    /*! Return the inverse matrix. */
  Matrix inverse( void ) const;

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

    /*! Return the transformation matrix that rotates around the
        z-axis by \a yaw radians. */
  static Matrix rotateYaw( double yaw );
    /*! Return the transformation matrix that rotates around the
        y-axis by \a pitch radians. */
  static Matrix rotatePitch( double pitch );
    /*! Return the transformation matrix that rotates around the x-axis by \a roll radians. */
  static Matrix rotateRoll( double roll );
    /*! Return the transformation matrix that rotates around the
        z-axis by \a yaw, then around the new y-axis by \a pitch, and
        then around the resulting x-axis by \a roll. All angles in
        radians. */
  static Matrix rotate( double yaw, double pitch, double roll );

    /*! Write the elements of the matrix to stream \a str. */
  friend ostream &operator<<( ostream &str, const Matrix &m );


protected:

  double Elems[3][3];

};


}; /* namespace relacs */

#endif /* ! _RELACS_MATRIX_H_ */
