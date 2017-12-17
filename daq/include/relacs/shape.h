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

    /*! Minimum corner of bounding box. */
  virtual Point boundingBoxMin( void ) const = 0;
    /*! Maximum corner of bounding box. */
  virtual Point boundingBoxMax( void ) const = 0;

    /*! Return \a\c true if point \a p is inside the shape.
        This implementation checks whether the point is inside the bounding box. */
  virtual bool inside( const Point & p ) const;
    /*! Return \a\c true if point \a p is below the shape.
      This implementation checks whether the point is inside or below the bounding box. */
  virtual bool below( const Point & p ) const;

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
    /*! Copy constructor. */
  Cuboid( const Cuboid &c );
    /*! Construct cuboid from minimum corner \a a and size defined by 
        \a lenght, \a width, and \a height. */
  Cuboid( const Point &a, double length, double width, double height );
    /*! Construct cuboid from minimum corner \a start and  
        maximum corner \a end. */
  Cuboid( const Point &start, const Point &end );

    /*! The minimum corner of the cuboid. */
  const Point &corner( void ) { return Corner; }
    /*! Set the minimum corner of the cuboid to \a corner. */
  void setCorner( const Point &corner ) { Corner = corner; }

    /*! The size of the cuboid in x-direction. */
  double length( void ) const { return Size[0]; }
    /*! The width of the cuboid in y-direction. */
  double width( void ) const { return Size[1]; }
    /*! The height of the cuboid in z-direction. */
  double height( void ) const { return Size[2]; }

    /*! Set the size of the cuboid in x-direction to \a lenght. */
  void setLength( double length ) { Size[0] = length; }
    /*! Set the size of the cuboid in y-direction to \a width. */
  void setWidth( double width ) { Size[1] = width; }
    /*! Set the size of the cuboid in z-direction to \a height. */
  void setHeight(double height) { Size[2] = height; }

    /*! The size of the cuboid. */
  const Point &size( void ) { return Size; }
    /*! Set the size of the cuboid to \a size. */
  void setSize( const Point &size ) { Size = size; }

    /*! Minimum corner of bounding box. */
  virtual Point boundingBoxMin( void ) const;
    /*! Maximum corner of bounding box. */
  virtual Point boundingBoxMax( void ) const;


private:

  Point Corner;
  Point Size;

};


}; /* namespace relacs */

#endif /* ! _RELACS_SHAPE_H_ */
