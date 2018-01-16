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
\brief An abstract shape in 3D space that has an inside.
\author Jan Benda, Alexander Ott, Fabian Sinz
 */

class Shape 
{

public:

  enum ShapeType {
    Unknown = 0,
    Zone = 1,
    Sphere = 2,
    Cylinder = 3,
    Cuboid = 4,
  };

    /*! Constructs a shape of a specific \a type, with name \a name
        and anchor point \a anchor. */
  Shape( ShapeType type, const string &name="", const Point &anchor=Point( 0.0, 0.0, 0.0 ) );

  virtual ~Shape( void );

    /*! Returns a pointer to a copy of this shape. */
  virtual Shape *copy( void ) const = 0;

    /*! The type of shape. */
  ShapeType type( void ) const { return Type; };
    /*! Set the type of the shape to \a type. */
  void setType( ShapeType type ) { Type = type; };

    /*! The shape's name. */
  string name( void ) const { return Name; };
    /*! Set the name of the shape to \a name. */
  void setName( const string &name ) { Name = name; };

    /*! The anchor point of the shape. */
  const Point &anchor( void ) const { return Anchor; }
    /*! Set the anchor point of the shape to \a anchor. */
  void setAnchor( const Point &anchor ) { Anchor = anchor; }

    /*! The yaw angle of the shape in radians. */
  double yaw( void ) const { return Yaw; };
    /*! Set the yaw angle of the shape to \a yaw in radians and
        recompute transformation matrices. */
  void setYaw( double yaw );
    /*! The pitch angle of the shape in radians. */
  double pitch( void ) const { return Pitch; };
    /*! Set the pitch angle of the shape to \a pitch in radians and
        recompute transformation matrices. */
  void setPitch( double pitch );
    /*! The roll angle of the shape in radians. */
  double roll( void ) const { return Roll; };
    /*! Set the roll angle of the shape to \a roll in radians and
        recompute transformation matrices. */
  void setRoll( double roll );
    /*! Set the yaw, pitch, and roll angles of the shape to 
        \a yaw, \a pitch, \a roll in radians, respectively, and
        recompute transformation matrices. */
  void setAngles( double yaw, double pitch, double roll );

    /*! Transform coordinates of point \a p from shape coordinates to world coordinates
        by rotation with the yaw, pitch, and roll angles and shifting to the anchor point. */
  Point transform( const Point &p ) const;
    /*! Transform coordinates of point \a p from world coordinates to shape coordinates
        by shifting the anchor point to the origin and 
	by rotation with the roll, pitch, and yaw angles. */
  Point inverseTransform( const Point &p ) const;

    /*! Minimum corner of bounding box. */
  virtual Point boundingBoxMin( void ) const = 0;
    /*! Maximum corner of bounding box. */
  virtual Point boundingBoxMax( void ) const = 0;

    /*! Return \c true if point \a p is inside the shape. */
  virtual bool inside( const Point &p ) const = 0;
    /*! Return \c true if point \a p is below the shape. */
  virtual bool below( const Point &p ) const = 0;
    /*! Check whether the path connecting the two points \a pos1 and \a pos2
        intersects a shape.
	Pathes less than \a resolution long may intersect a shape without notice.
        This implementation recursively checks whether points on the path
        are inside a shape. */
  virtual bool intersect( const Point &pos1, const Point &pos2, double resolution ) const;

    /*! Print some information about the shape into the stream \a str. */
  virtual ostream &print( ostream &str ) const = 0;

    /*! Print some information about the shape \a s into the stream \a str. */
  friend ostream &operator<<( ostream &str, const Shape &s );


protected:

    /*! The point (0, 0, 0). */
  static const Point Origin;


private:

    /*! Recompute the transformation matrices from the three angles. */
  void computeTrafos( void );

    /*! The type of the shape. */
  ShapeType Type;
    /*! The name of the shape. */
  string Name;
    /*! The location of the shape. */
  Point Anchor;
    /*! Rotation of standard shape around z-axis in radians. */
  double Yaw;
    /*! Rotation of standard shape around y-axis in radians. */
  double Pitch;
    /*! Rotation of standard shape around x-axis in radians. */
  double Roll;
    /*! The transformation matrix for transfroming shape coordinates
        to world coordinates. */
  double Trafo[3][3];
    /*! The inverse transformation matrix for transfroming world
        coordinates to shape coordinates. */
  double InvTrafo[3][3];

};


/*!
\class Zone
\brief A shape made up of a collection of basic shapes.
\author Jan Benda, Fabian Sinz
 */

class Zone : public Shape 
{

 public:

    /*! Constructor. */
  Zone( void );
    /*! Copy constructor. */
  Zone( const Zone &z );
    /*! Construct zone with name \a name from a single shape \a s. */
  Zone( const Shape &s, const string &name="zone" );
    /*! Construct zone with name \a name from a list of shapes \a s.
        The shapes are copied into the zone. */
  Zone( const deque<Shape*> &s, const string &name="zone" );

    /*! Destructor. */
  ~Zone( void );

    /*! Returns a pointer to a copy of the zone. */
  virtual Shape *copy( void ) const;

    /*! Add shape \a s to the zone. */
  void add( const Shape &s );
    /*! Subtract shape \a s from the zone. */
  void subtract( const Shape &s );
    /*! Add (\a add \c = \c true) or subtract (\a add \c = \c false)
        shape \a s to the zone. */
  void push( const Shape &s, bool add=true );

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
    /*! Return the i-th shape. */
  const Shape *operator[]( int i ) const { return Shapes[i]; }
    /*! Return the i-th shape. */
  Shape *operator[]( int i ) { return Shapes[i]; }
    /*! Return the first shape with name \a name.
        Return 0 if no shape with that name is found. */
  const Shape *operator[]( const string &name ) const;
    /*! Return the first shape with name \a name.
        Return 0 if no shape with that name is found. */
  Shape *operator[]( const string &name );

    /*! Remove all shapes from the zone. */
  void clear( void );

    /*! Approximation of the minimum corner of bounding boxes of all additive shapes. */
  virtual Point boundingBoxMin( void ) const;
    /*! Approximation of the maximum corner of bounding boxes of all additive shapes. */
  virtual Point boundingBoxMax( void ) const;

    /*! Return \c true if point \a p is inside the zone. */
  virtual bool inside( const Point &p ) const;
    /*! Return \c true if point \a p is below all additive shapes of the zone. */
  virtual bool below( const Point &p ) const;

    /*! Print some information about the zone into the stream \a str. */
  virtual ostream &print( ostream &str ) const;


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
  Sphere( const Point &center, double radius, const string &name="sphere" );

    /*! Returns a pointer to a copy of the sphere. */
  virtual Shape *copy( void ) const;

    /*! Center of the sphere. This is the anchor point of the sphere. */
  const Point &center( void ) const { return anchor(); }
    /*! Set the center of the sphere (its anchor point) to \a center. */
  void setCenter( const Point &center ) { setAnchor( center ); }

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

    /*! Print some information about the sphere into the stream \a str. */
  virtual ostream &print( ostream &str ) const;


private:

  double Radius;

};


/*!
\class Cylinder
\brief A cylinder.
\author Jan Benda
 */

class Cylinder : public Shape 
{

 public:

    /*! Constructor. */
  Cylinder( void );
    /*! Copy constructor. */
  Cylinder( const Cylinder &c );
    /*! Construct a cylinder with name \a name from \a anchor, \a radius, and \a height.
        The anchor point is the center of the bottom circle. */
  Cylinder( const Point &anchor, double radius, double height, const string &name="cylinder" );

    /*! Returns a pointer to a copy of the cylinder. */
  virtual Shape *copy( void ) const;

    /*! The radius of the cylinder. */
  double radius( void ) const { return Radius; }
    /*! Set the radius of the cylinder to \a radius. */
  void setRadius( double radius ) { Radius = radius; }

    /*! The height of the cylinder. */
  double height( void ) const { return Height; }
    /*! Set the height of the cylinder to \a height. */
  void setHeight( double height ) { Height = height; }

    /*! Minimum corner of bounding box. */
  virtual Point boundingBoxMin( void ) const;
    /*! Maximum corner of bounding box. */
  virtual Point boundingBoxMax( void ) const;

    /*! Return \c true if point \a p is inside the cylinder. */
  virtual bool inside( const Point &p ) const;
    /*! Return \c true if point \a p is below the cylinder. */
  virtual bool below( const Point &p ) const;

    /*! Print some information about the cylinder into the stream \a str. */
  virtual ostream &print( ostream &str ) const;


private:

  double Radius;
  double Height;

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
    /*! Construct a cuboid with name \a name from minimum corner \a anchor and size defined by 
        \a lenght, \a width, and \a height. All angles are zero. */
  Cuboid( const Point &anchor, double length, double width, double height,
	  const string &name="cuboid" );
    /*! Construct a cuboid with name \a name from minimum corner \a anchor and  
        maximum corner \a end. All angles are zero. */
  Cuboid( const Point &anchor, const Point &end, const string &name="cuboid" );
    /*! Construct a cuboid with name \a name. The anchor point of the cuboid is at \a anchor.
        The point \a px defines the lenght, pitch, and yaw of the cuboid.
	The point \a py defines the width and the roll.
	The point \a py defines the height of the cuboid. */
  Cuboid( const Point &anchor, const Point &px, const Point &py, const Point &pz,
	  const string &name="cuboid" );

    /*! Returns a pointer to a copy of the cuboid. */
  virtual Shape *copy( void ) const;

    /*! The minimum corner of the cuboid. This is the anchor point of the cuboid. */
  const Point &corner( void ) const { return anchor(); }
    /*! Set the minimum corner (the anchor point) of the cuboid to \a corner. */
  void setCorner( const Point &corner ) { setAnchor( corner ); }

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

    /*! Return \c true if point \a p is inside the cuboid. */
  virtual bool inside( const Point &p ) const;
    /*! Return \c true if point \a p is below the cuboid. */
  virtual bool below( const Point &p ) const;

    /*! Minimum corner of bounding box. */
  virtual Point boundingBoxMin( void ) const;
    /*! Maximum corner of bounding box. */
  virtual Point boundingBoxMax( void ) const;

    /*! Print some information about the cuboid into the stream \a str. */
  virtual ostream &print( ostream &str ) const;


private:

  Point Size;

};


}; /* namespace relacs */

#endif /* ! _RELACS_SHAPE_H_ */
