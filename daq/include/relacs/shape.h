/*
  shape.h
  Shapes in 3D space.

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

#ifndef _RELACS_SHAPE_H_
#define _RELACS_SHAPE_H_ 1

#include <relacs/point.h>


namespace relacs {

/*!
\class Shape
\brief A shape in 3D space.
\author Alexander Ott, Jan Benda
 */

class Shape 
{

public:

    /*! Constructor. */
  Shape( void );

  // functions for forbidden areas:
  virtual bool point_inside( const Point &p ) const = 0;
  virtual bool point_below( const Point &p ) const = 0;
  virtual bool point_safe( const Point &p ) const = 0;

  // bounding box ??  return two points on corners diagonal from each other
  // -> you can create the box from those two points.
  virtual bool extreme_x( double &biggest, double &lowest ) const = 0;
  virtual bool extreme_y( double &biggest, double &lowest ) const = 0;
  virtual bool extreme_z( double &biggest, double &lowest ) const = 0;

};


/*!
\class Cuboid
\brief A cuboid.
\author Alexander Ott, Jan Benda
 */

class Cuboid : public Shape 
{

 public:

    /*! Constructor. */
  Cuboid( void );
  Cuboid( const Cuboid &c );
  Cuboid( const Point &a, double length, double width, double height );
  Cuboid( const Point &start, const Point &end );
  Cuboid( const Point &start, const Point &depth, const Point &length,
	  const Point &width );

  double length( void ) const { return Length; }
  double width( void ) const { return Width; }
  double height( void ) const { return Height; }

  void setLength( double length ) { Length = length; }
  void setWidth( double width ) { Width = width; }
  void setHeight(double height) { Height = height; }

  const Point &startPoint( void ) { return StartPoint; }
  void setStartPoint( const Point &startpoint ) { StartPoint = startpoint; }

  // functions for forbidden areas:
  bool point_inside( const Point & p ) const;
  bool point_below( const Point & p ) const;
  bool point_safe( const Point & p ) const;

  // bounding box:
  bool extreme_x( double &biggest, double &lowest ) const;
  bool extreme_y( double &biggest, double &lowest ) const;
  bool extreme_z( double &biggest, double &lowest ) const;


private:

  Point StartPoint;
  double Length;
  double Width;
  double Height;

};


}; /* namespace relacs */

#endif /* ! _RELACS_SHAPE_H_ */
