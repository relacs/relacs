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
#include <relacs/transform.h>


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

    /*! Constructs a shape of a specific \a type with name \a name. */
  Shape( ShapeType type, const string &name="" );
    /*! Constructs a shape of a specific \a type, with name \a name,
        and transformed by \a trafo. */
  Shape( ShapeType type, const string &name, const Transform &trafo );
    /*! Copy constructor. */
  Shape( const Shape &s );

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

    /*! Translate the shape by \a x along the world x-axis. */
  void translateX( double x );
    /*! Translate the shape by \a y along the world y-axis. */
  void translateY( double y );
    /*! Translate the shape by \a z along the world z-axis. */
  void translateZ( double z );
    /*! Translate the shape by \a p in the world coordinate system. */
  void translate( const Point &p );

    /*! Scale the shape along the x-axis by the factor \a xscale. */
  void scaleX( double xscale );
    /*! Scale the shape along the y-axis by the factor \a yscale. */
  void scaleY( double yscale );
    /*! Scale the shape along the z-axis by the factor \a zscale. */
  void scaleZ( double zscale );
    /*! Scale the shape along the x-, y-, and z-axis 
        by the factor \a xscale, \a yscale, \a zscale, respectively. */
  void scale( double xscale, double yscale, double zscale );
    /*! Scale the shape along the x-, y-, and z-axis by \a scale. */
  void scale( const Point &scale );
    /*! Uniformly scale the shape along the x-, y-, and z-axis 
        by the factor \a scale. */
  void scale( double scale );

    /*! Rotate the shape counterclockwise around the world x-axis by \a angle
        radians. */
  void rotateX( double angle );
    /*! Rotate the shape counterclockwise around the world y-axis by \a
        angle radians. */
  void rotateY( double angle );
    /*! Rotate the shape counterclockwise around the world z-axis by \a angle
        radians. */
  void rotateZ( double angle );
    /*! Rotate the shape counterclockwise around the world x-axis by \a anglex,
        then around the world y-axis by \a angley, and then around the
        world z-axis by \a anglez. All angles in radians. */
  void rotate( double anglex, double angley, double anglez );

    /*! The transformation matrix that transforms shape coordinates to
        world coordinates. */
  const Transform &trafo( void ) const { return Trafo; };
    /*! The inverse transformation matrix that transforms world coordinates to
        shape coordinates. */
  const Transform &invTrafo( void ) const { return InvTrafo; };

    /*! Transform coordinates of point \a p from shape coordinates to
        world coordinates by applying the transformation matrix. */
  Point transform( const Point &p ) const;
    /*! Transform coordinates of point \a p from world coordinates to
        shape coordinates by applying the inverse transformation
        matrix. */
  Point inverseTransform( const Point &p ) const;

    /*! Minimum corner of bounding box. */
  Point boundingBoxMin( void ) const;
    /*! Maximum corner of bounding box. */
  Point boundingBoxMax( void ) const;
    /*! Minimum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMin( const Transform &trafo ) const = 0;
    /*! Maximum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMax( const Transform &trafo ) const = 0;

    /*! Return \c true if point \a p in world coordinates is inside the shape.
        The point is transformed into shape coordinates and then tested with insideShape(). */
  bool inside( const Point &p ) const;
    /*! Return \c true if point \a p in shape coordinates is inside the shape. */
  virtual bool insideShape( const Point &p ) const = 0;
    /*! Return \c true if point \a p is below the shape.
        This implementation checks whether \a p is below the bounding box. 
        XXX Replace this function by something more general....*/
  virtual bool below( const Point &p ) const ;

    /*! Return in \a ip1 and \a ip2 the intersection points of the
        shape with the line connecting \a pos1 with \a pos2. \a ip1
        and \a ip2 can be Point::None if there is no intersection or
        if \a pos1 and \a pos2 are the same. If \a pos1 and/or \a pos2
        are already inside the shape, then \a ip1 and/or \a ip2 are
        the intersection points on the line connecting \a pos1 and \a
        pos2 but not between the two points. All points in world
        coordinates. */
  void intersectionPoints( const Point &pos1, const Point &pos2,
			   Point &ip1, Point &ip2 ) const;
    /*! Return in \a ip1 and \a ip2 the intersection points of the
        shape with the line connecting \a pos1 with \a pos2. \a ip1
        and \a ip2 can be Point::None if there is no intersection or
        if \a pos1 and \a pos2 are the same. If \a pos1 and/or \a pos2
        are already inside the shape, then \a ip1 and/or \a ip2 are
        the intersection points on the line connecting \a pos1 and \a
        pos2 but not between the two points. All points in shape
        coordinates. */
  virtual void intersectionPointsShape( const Point &pos1, const Point &pos2,
					Point &ip1, Point &ip2 ) const = 0;

    /*! Print some information about the shape into the stream \a str. */
  virtual ostream &print( ostream &str ) const = 0;

    /*! Print some information about the shape \a s into the stream \a str. */
  friend ostream &operator<<( ostream &str, const Shape &s );


private:

    /*! The type of the shape. */
  ShapeType Type;
    /*! The name of the shape. */
  string Name;
    /*! The transformation matrix for transforming shape coordinates
        to world coordinates. */
  Transform Trafo;
    /*! The inverse transformation matrix for transforming world
        coordinates to shape coordinates. */
  Transform InvTrafo;

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

    /*! Approximation of the minimum corner of bounding boxes of all
        additive shapes for the transformation from shape to world
        coordinates specified by \a trafo. */
  virtual Point boundingBoxMin( const Transform &trafo ) const;
    /*! Approximation of the maximum corner of bounding boxes of all
        additive shapes for the transformation from shape to world
        coordinates specified by \a trafo. */
  virtual Point boundingBoxMax( const Transform &trafo ) const;

    /*! Return \c true if point \a p in shape coordinates is inside the zone. */
  virtual bool insideShape( const Point &p ) const;
    /*! Return \c true if point \a p is below all additive shapes of the zone. */
  virtual bool below( const Point &p ) const;

    /*! Return in \a ip1 and \a ip2 the intersection points of the
        zone with the line connecting \a pos1 with \a pos2. \a ip1 and
        \a ip2 can be Point::None if there is no intersection or if \a
        pos1 and \a pos2 are the same. If \a pos1 and/or \a pos2 are
        already inside the shape, then \a ip1 and/or \a ip2 are the
        intersection points on the line connecting \a pos1 and \a pos2
        but not between the two points. All points in shape
        coordinates. */
  virtual void intersectionPointsShape( const Point &pos1, const Point &pos2,
					Point &ip1, Point &ip2 ) const;

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

    /*! The radius of the sphere in world coordinates. */
  double radius( void ) const;

    /*! Minimum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMin( const Transform &trafo ) const;
    /*! Maximum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMax( const Transform &trafo ) const;

    /*! Return \c true if point \a p in shape coordinates is inside the sphere. */
  virtual bool insideShape( const Point &p ) const;

    /*! Return in \a ip1 and \a ip2 the intersection points of the
        sphere with the line connecting \a pos1 with \a pos2. \a ip1
        and \a ip2 can be Point::None if there is no intersection or
        if \a pos1 and \a pos2 are the same. If \a pos1 and/or \a pos2
        are already inside the shape, then \a ip1 and/or \a ip2 are
        the intersection points on the line connecting \a pos1 and \a
        pos2 but not between the two points. All points in shape
        coordinates. */
  virtual void intersectionPointsShape( const Point &pos1, const Point &pos2,
					Point &ip1, Point &ip2 ) const;

    /*! Print some information about the sphere into the stream \a str. */
  virtual ostream &print( ostream &str ) const;

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
    /*! Construct a cylinder with name \a name from \a anchor, \a radius, and \a length.
        The anchor point is the center of the left circle. */
  Cylinder( const Point &anchor, double radius, double length, const string &name="cylinder" );

    /*! Returns a pointer to a copy of the cylinder. */
  virtual Shape *copy( void ) const;

    /*! The radius of the cylinder in world coordinates. */
  double radius( void ) const;
    /*! The length of the cylinder in world coordinates. */
  double length( void ) const;

    /*! Minimum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMin( const Transform &trafo ) const;
    /*! Maximum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMax( const Transform &trafo ) const;

    /*! Return \c true if point \a p in shape coordinates is inside the cylinder. */
  virtual bool insideShape( const Point &p ) const;

    /*! Return in \a ip1 and \a ip2 the intersection points of the
        cylinder with the line connecting \a pos1 with \a pos2. \a ip1
        and \a ip2 can be Point::None if there is no intersection or
        if \a pos1 and \a pos2 are the same. If \a pos1 and/or \a pos2
        are already inside the shape, then \a ip1 and/or \a ip2 are
        the intersection points on the line connecting \a pos1 and \a
        pos2 but not between the two points.. All points in shape
        coordinates. */
  virtual void intersectionPointsShape( const Point &pos1, const Point &pos2,
					Point &ip1, Point &ip2 ) const;

    /*! Print some information about the cylinder into the stream \a str. */
  virtual ostream &print( ostream &str ) const;

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

    /*! Return in \a pts the corners of the cuboid transformed
        according to \a trafo. */
  void corners( deque< Point > &pts,
		const Transform &trafo ) const;
    /*! Return in \a pts the corners of the cuboid in world coordinates. */
  void corners( deque< Point > &pts ) const;

    /*! The length of the cuboid in world coordinates. */
  double length( void ) const;
    /*! The width of the cuboid in world coordinates. */
  double width( void ) const;
    /*! The height of the cuboid in world coordinates. */
  double height( void ) const;

    /*! Minimum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMin( const Transform &trafo ) const;
    /*! Maximum corner of bounding box for the transformation from
        shape to world coordinates specified by \a trafo. */
  virtual Point boundingBoxMax( const Transform &trafo ) const;

    /*! Return \c true if point \a p in shape coordinates is inside the cuboid. */
  virtual bool insideShape( const Point &p ) const;

    /*! Return in \a ip1 and \a ip2 the intersection points of the
        cuboid with the line connecting \a pos1 with \a pos2. \a ip1
        and \a ip2 can be Point::None if there is no intersection or
        if \a pos1 and \a pos2 are the same. If \a pos1 and/or \a pos2
        are already inside the shape, then \a ip1 and/or \a ip2 are
        the intersection points on the line connecting \a pos1 and \a
        pos2 but not between the two points. All points in shape
        coordinates. */
  virtual void intersectionPointsShape( const Point &pos1, const Point &pos2,
					Point &ip1, Point &ip2 ) const;

    /*! Print some information about the cuboid into the stream \a str. */
  virtual ostream &print( ostream &str ) const;

};


}; /* namespace relacs */

#endif /* ! _RELACS_SHAPE_H_ */
