/*
  point.h
  A point in 3D space.

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

#ifndef _RELACS_POINT_H
#define _RELACS_POINT_H 1

#include <iostream>
using namespace std;

namespace relacs {


/*!
\class Point
\brief A point in 3D space.
\author Alexander Ott, Jan Benda
\todo Add operators for addition and subtraction, etc.
 */

class Point {

public:

    /*! Constructor. */
  Point( void );
  Point( const Point &p );
  Point( double x, double y, double z);

  double x( void ) const { return Coords[0]; }
  double &x( void ) { return Coords[0]; }
  double y( void ) const { return Coords[1]; }
  double &y( void ) { return Coords[1]; }
  double z( void ) const { return Coords[2]; }
  double &z( void ) { return Coords[2]; }

  void set( double x, double y, double z);

  double operator[]( int i ) const { return Coords[i]; }
  double &operator[]( int i ) { return Coords[i]; }

  double distance( const Point &p ) const;

  Point abs_diff( const Point &p ) const;

  Point center( const Point &p ) const;

  friend ostream &operator<<( ostream &str, const Point &p );


protected:

  static const int Dim = 3;
  double Coords[Dim];

};


}; /* namespace relacs */

#endif /* ! _RELACS_POINT_H_ */
