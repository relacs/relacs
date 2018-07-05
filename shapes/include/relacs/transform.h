/*
  transform.h
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

#ifndef _RELACS_TRANSFORM_H
#define _RELACS_TRANSFORM_H 1

#include <iostream>
#include <relacs/point.h>
using namespace std;

namespace relacs {


/*!
\class Transform
\brief A 4-D matrix used for affine and perspective transformations of 3D points.
\author Jan Benda, Fabian Sinz
 */

class Transform {

public:

    /*! Identity transformation. */
  static const Transform Identity;
    /*! Transformation matrix with all elements zero. */
  static const Transform Zeros;
    /*! Transformation with all elements ones. */
  static const Transform Ones;

    /*! Constructor. */
  Transform( void );
    /*! Copy constructor. */
  Transform( const Transform &p );
    /*! A transformation matrix with elements copied from \a m. All
        other elements are set to 0 and a44 is set ot 1. */
  Transform( const double m[3][3] );
    /*! A transformation matrix defined by its elements \a a_i,j. All
        other elements are set to 0 and a44 is set ot 1. */
  Transform( double a11, double a12, double a13,
	     double a21, double a22, double a23,
	     double a31, double a32, double a33 );

    /*! The element (i, j) of the transformation matrix. */
  double operator()( int i, int j ) const { return Elems[i][j]; }
    /*! The element (i, j) of the transformation matrix. */
  double &operator()( int i, int j ) { return Elems[i][j]; }

    /*! The i-th row of the transformation matrix. */
  const double*  operator[]( int i ) const { return Elems[i]; }
    /*! The i-th row of the transformation matrix. */
  double* operator[]( int i ) { return Elems[i]; }

    /*! Assign elements of transformation matrix \a m to this
        transformation matrix. */
  Transform &assign( const Transform &m );
    /*! Assign elements of transformation matrix \a m to this
        transformation matrix. */
  Transform &operator=( const Transform &m );

    /*! Return the negative of all elements of the transformation matrix. */
  Transform operator-( void ) const;

    /*! Add \a a to all elements of this transformation matrix. */
  Transform operator+( double a ) const;
    /*! Subtract \a a from all elements of this transformation matrix. */
  Transform operator-( double a ) const;
    /*! Multiply all elements of this transformation matrix with \a a. */
  Transform operator*( double a ) const;
    /*! Divide all elements of this transformation matrix by \a a. */
  Transform operator/( double a ) const;

    /*! Add \a a to all elements of this transformation matrix. */
  Transform &operator+=( double a );
    /*! Subtract \a a from all elements of this transformation matrix. */
  Transform &operator-=( double a );
    /*! Multiply all elements of this transformation matrix with \a a. */
  Transform &operator*=( double a );
    /*! Divide all elements of this transformation matrix by \a a. */
  Transform &operator/=( double a );

    /*! Apply this transformation matrix on point \a p. */
  Point operator*( const Point &p ) const;
    /*! Matrix multiplication of this transformwith with transformation matrix \a m. */
  Transform operator*( const Transform &m ) const;
    /*! Apply transformation matrix \a m to this transformation
        matrix.  That is compute transformation matrix \a m multiplied
        with this transformation matrix.
        \note \a m is multiplied from the left, not from the right! */
  Transform &operator*=( const Transform &m );

    /*! Return the determinant of the transformation matrix. */
  double det( void ) const;
    /*! Return the inverse transformation matrix. */
  Transform inverse( void ) const;
    /*! Return the transposed transformation matrix. */
  Transform transpose( void ) const;

    /*! Reset the transformation matrix to the identity transformation . */
  void clear( void );
    /*! Set the translation and projection to zero. */
  void clearTransProj( void );

    /*! Apply the transformation that translates along the x-axis by \a x. */
  Transform &translateX( double x );
    /*! Apply the transformation that translates along the y-axis by \a y. */
  Transform &translateY( double y );
    /*! Apply the transformation that translates along the z-axis by \a z. */
  Transform &translateZ( double z );
    /*! Apply the transformation that translates along the x-, y-, and
        z-axis by \a x, \a y, \a z, respectively. */
  Transform &translate( double x, double y, double z );
    /*! Apply the transformation that translates the x-, y-, and
        z-axis by \a trans. */
  Transform &translate( const Point &trans );

    /*! Apply the transformation that scales the x-axis by \a xscale. */
  Transform &scaleX( double xscale );
    /*! Apply the transformation that scales the y-axis by \a yscale. */
  Transform &scaleY( double yscale );
    /*! Apply the transformation that scales the z-axis by \a zscale. */
  Transform &scaleZ( double zscale );
    /*! Apply the transformation that scales the x-, y-, and
        z-axis by \a xscale, \a yscale, \a zscale, respectively. */
  Transform &scale( double xscale, double yscale, double zscale );
    /*! Apply the transformation that scales the x-, y-, and
        z-axis by \a scale. */
  Transform &scale( const Point &scale );
    /*! Apply the transformation that scales the x-, y-, and
        z-axis uniformly by \a scale. */
  Transform &scale( double scale );

    /*! Apply the transformation that rotates counterclockwise
        around the world y-axis by \a angle radians. */
  Transform &rotateX( double angle );
    /*! Apply the transformation that rotates counterclockwise
        around the world y-axis by \a angle radians. */
  Transform &rotateY( double angle );
    /*! Apply the transformation that rotates counterclockwise
        around the world z-axis by \a angle radians. */
  Transform &rotateZ( double angle );
    /*! Apply the transformation that rotates counterclockwise
        around the world x-axis by \a anglex, then around the world y-axis by \a
        angley, and then around the world z-axis by \a world z. All
        angles in radians. */
  Transform &rotate( double anglex, double angley, double anglez );
    /*! Apply the transformation that rotates counterclockwise
        around the given axis \a axis by \a angle radians.
	\a axis does not need to be normalized. */
  Transform &rotate( const Point &axis, double angle );
    /*! Apply the transformation that rotates vector \a from into
        vector \a to. */
  Transform &rotate( const Point &from, const Point &to );

    /*! Apply the transformation that adds perspective scaling for a
        view point at a distance \a distance along the x-axis,
        i.e. set a41 to 1/distance. */
  Transform &perspectiveX( double distance );
    /*! Apply the transformation that adds perspective scaling for a
        view point at a distance \a distance along the y-axis,
        i.e. set a42 to 1/distance. */
  Transform &perspectiveY( double distance );
    /*! Apply the transformation that adds perspective scaling for a
        view point at a distance \a distance along the z-axis,
        i.e. set a43 to 1/distance. */
  Transform &perspectiveZ( double distance );

    /*! Write the elements of the transformation matrix to stream \a str. */
  friend ostream &operator<<( ostream &str, const Transform &m );


protected:

    /*! Comput determinant of minor of 3D matrix for (0, j ). */
  double detMinor( double m[3][3], int j ) const;
    /*! Return the determinant of the minor matrix for (i, j). */
  double detMinor( int i, int j ) const;

    /*! The matrix elements of the transformation. */
  double Elems[4][4];

};


}; /* namespace relacs */

#endif /* ! _RELACS_TRANSFORM_H_ */
