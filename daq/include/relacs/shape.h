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

#include <deque>
#include <relacs/point.h>


namespace relacs {


/*!
\class Shape
\brief A shape in 3D space that has an inside.
\author Alexander Ott, Jan Benda
 */

class Shape 
{

public:

  enum ShapeType {
    Unknown = 0,
    Zone = 1,
    Sphere = 2,
    Cuboid = 3,
  };

    /*! Constructs a shape of a specific \a type and with an optional \a name. */
  Shape( ShapeType type, const string &name="" );

  virtual ~Shape( void );

    /*! The type of shape. */
  ShapeType type( void ) const { return Type; };
    /*! Set the type of the shape. */
  void setType( ShapeType type ) { Type = type; };

    /*! Sets the name of the shape. */
  void setName( const string &name ) { Name = name; };
    /*! The shape's name. */
  string name( void ) const { return Name; };

    /*! Returns a pointer to a copy of a shape. */
  virtual Shape *copy( void ) const = 0;

    /*! Minimum corner of bounding box. */
  virtual Point boundingBoxMin( void ) const = 0;
    /*! Maximum corner of bounding box. */
  virtual Point boundingBoxMax( void ) const = 0;

    /*! Return \c true if point \a p is inside the shape.
        This implementation checks whether the point is inside the bounding box. */
  virtual bool inside( const Point &p ) const;
    /*! Return \c true if point \a p is below the shape.
        This implementation checks whether the point is inside or below the bounding box. */
  virtual bool below( const Point &p ) const;


private:

    /*! The type of the shape. */
  ShapeType Type;
    /*! The name of the shape. */
  string Name;

};


/*!
\class Zone
\brief A shape made up of a collection of basic shapes.
\author Fabian Sinz, Jan Benda
 */

class Zone : public Shape 
{

 public:

    /*! Constructor. */
  Zone( void );
    /*! Copy constructor. */
  Zone( const Zone &z );
    /*! Construct zone with name \a name from a single shape \a s. */
  Zone( const Shape &s, const string &name="" );
    /*! Construct zone with name \a name from a list of shapes \a s.
        The shapes are copied into the zone. */
  Zone( const deque<Shape*> &s, const string &name="" );

    /*! Destructor. */
  ~Zone( void );

    /*! Returns a pointer to a copy of the zone. */
  virtual Shape *copy( void ) const;

    /*! Add shape \a s to the zone. */
  void add( const Shape &s );
    /*! Subtract shape \a s from the zone. */
  void subtract( const Shape &s );

    /*! Add shape \a s to the zone. */
  void operator+=( const Shape &s );
    /*! Subtract shape \a s from the zone. */
  void operator-=( const Shape &s );

    /*! Return the zone with shape \a s added to it. */
  Zone operator+( const Shape &s ) const;
    /*! Return the zone with shape \a s subtracted from it. */
  Zone operator-( const Shape &s ) const;

    /*! The number of shapes contained by the zone. */
  int size( void ) const { return (int)Shapes.size(); };
    /*! Return \c true if no shapes are contained by the zone. */
  bool empty( void ) const { return Shapes.empty(); };
    /*! The i-th shape. */
  const Shape *operator[]( int i ) const { return Shapes[i]; }
    /*! The i-th shape. */
  Shape *operator[]( int i ) { return Shapes[i]; }

    /*! Minimum corner of bounding boxes of all additive shapes. */
  virtual Point boundingBoxMin( void ) const;
    /*! Maximum corner of bounding boxes of all additive shapes. */
  virtual Point boundingBoxMax( void ) const;

    /*! Return \c true if point \a p is inside the zone. */
  virtual bool inside( const Point &p ) const;
    /*! Return \c true if point \a p is below all additive shapes of the zone. */
  virtual bool below( const Point &p ) const;


private:

  deque<Shape*> Shapes;
  deque<bool> Add;

};


/*!
\class Sphere
\brief A sphere.
\author Jan Benda
 */

class Sphere : public Shape 
{

 public:

    /*! Constructor. */
  Sphere( void );
    /*! Copy constructor. */
  Sphere( const Sphere &s );
    /*! Construct a sphere with name \a name from \a center and \a radius. */
  Sphere( const Point &center, double radius, const string &name="" );

    /*! Returns a pointer to a copy of the sphere. */
  virtual Shape *copy( void ) const;

    /*! Center of the sphere. */
  Point center( void ) const { return Center; }
    /*! Set the center of the sphere to \a center. */
  void setCenter( const Point &center ) { Center = center; }

    /*! The radius of the sphere. */
  double radius( void ) const { return Radius; }
    /*! Set the radius of the sphere to \a radius. */
  void setRadius( double radius ) { Radius = radius; }

    /*! Minimum corner of bounding box. */
  virtual Point boundingBoxMin( void ) const;
    /*! Maximum corner of bounding box. */
  virtual Point boundingBoxMax( void ) const;

    /*! Return \c true if point \a p is inside the sphere. */
  virtual bool inside( const Point &p ) const;
    /*! Return \c true if point \a p is below the sphere. */
  virtual bool below( const Point &p ) const;


private:

  Point Center;
  double Radius;

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
    /*! Construct a cuboid with name \a name from minimum corner \a a and size defined by 
        \a lenght, \a width, and \a height. */
  Cuboid( const Point &a, double length, double width, double height, const string &name="" );
    /*! Construct a cuboid with name \a name from minimum corner \a start and  
        maximum corner \a end. */
  Cuboid( const Point &start, const Point &end, const string &name="" );

    /*! Returns a pointer to a copy of the cuboid. */
  virtual Shape *copy( void ) const;

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
