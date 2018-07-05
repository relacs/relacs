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
#include <relacs/polygon.h>
#include <relacs/transform.h>


namespace relacs {


class Zone;


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
    ZoneShape = 1,
    Sphere = 2,
    Cylinder = 3,
    Cuboid = 4,
  };

    /*! Constructs a shape of a specific \a type with name \a name.
        The resolution for approximating the shape is set to \a resolution. */
  Shape( ShapeType type, const string &name, int resolution );
    /*! Constructs a shape of a specific \a type, with name \a name,
        and transformed by \a trafo.
        The resolution for approximating the shape is set to \a resolution. */
  Shape( ShapeType type, const string &name, int resolution, const Transform &trafo );
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

    /*! The parent Zone. */
  const Zone *parent( void ) const { return Parent; };
    /*! Set the parent zone to \a parent. */
  void setParent( const Zone *parent ) { Parent = parent; };

    /*! Translate the shape by \a x along the world x-axis. */
  void translateX( double x );
    /*! Translate the shape by \a y along the world y-axis. */
  void translateY( double y );
    /*! Translate the shape by \a z along the world z-axis. */
  void translateZ( double z );
    /*! Translate the shape along the x-, y-, and z-axis by \a x, \a
        y, \a z, respectively. */
  void translate( double x, double y, double z );
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
    /*! Rotate the shape counterclockwise around the given axis \a axis
        by \a angle radians. */
  void rotate( const Point &axis, double angle );

    /*! Apply the transformation \a trafo to the shape. */
  void transform( const Transform &trafo );

    /*! The transformation matrix that transforms shape coordinates to
        world coordinates. */
  const Transform &trafo( void ) const { return Trafo; };
    /*! Set the transformation matrix of the shape to \a trafo. */
  void setTransform( const Transform &trafo );
    /*! Clear all transformations and reset the transformation matrix. */
  void clearTransform( void );
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

    /*! The number of polygons used to approximate the shape.
        If no resultion is set for this shape, check for the parant's resolution. */
  int resolution( void ) const;
    /*! Set the number of polygons used to approximate the shape to \a resolution. */
  void setResolution( int resolution ) { Resolution = resolution; };

    /*! Return the list of polygons that make up the shape. 
        \sa updatePolygons() */
  const deque<Polygon> &polygons( void ) const { return Polygons; };
    /*! Return the list of polygons that make up the shape. 
        \sa updatePolygons() */
  deque<Polygon> &polygons( void ) { return Polygons; };
    /*! Reset the polygons making up the shape to the ones in shape coordinates. */
  virtual void resetPolygons( void ) const = 0;
    /*! Update the polygons making up the shape in world coordinates. */
  virtual void updatePolygons( void ) const;

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


protected:

    /*! List of polygons that make up the shape. */
  mutable deque<Polygon> Polygons;


private:

    /*! The type of the shape. */
  ShapeType Type;
    /*! The name of the shape. */
  string Name;
    /*! Pointer to the parent zone. */
  const Zone *Parent;
    /*! The transformation matrix for transforming shape coordinates
        to world coordinates. */
  Transform Trafo;
    /*! The inverse transformation matrix for transforming world
        coordinates to shape coordinates. */
  Transform InvTrafo;
    /*! Number of polygons used to approximate the shape. */
  int Resolution;

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
    /*! Construct an empty zone with name \a name. */
  Zone( const string &name );
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

    /*! True if the \a i-th shape is added to the zone. */
  bool added( int i ) const;
    /*! True if the shape with name \a name is added to the zone. */
  bool added( const string &name ) const;
    /*! True if the \a i-th shape is subtracted from the zone. */
  bool subtracted( int i ) const;
    /*! True if the shape with name \a name is subtracted from the zone. */
  bool subtracted( const string &name ) const;

    /*! Remove all shapes from the zone. */
  void clear( void );

    /*! Reset the polygons making up the zone to the ones in shape coordinates. */
  virtual void resetPolygons( void ) const;
    /*! Update the polygons making up the shapes of the zone in world coordinates. */
  virtual void updatePolygons( void ) const;

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
    /*! Construct a default sphere with name \a name.
        The resolution for approximating the sphere is set to \a resolution. */
  Sphere( const string &name, int resolution=0 );
    /*! Construct a sphere with name \a name from \a center and \a radius.
        The resolution for approximating the sphere is set to \a resolution. */
  Sphere( const Point &center, double radius,
	  const string &name="sphere", int resolution=0 );

    /*! Returns a pointer to a copy of the sphere. */
  virtual Shape *copy( void ) const;

    /*! The radius of the sphere in world coordinates. */
  double radius( void ) const;

    /*! Reset the polygons making up the sphere to the ones in shape coordinates. */
  virtual void resetPolygons( void ) const;

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
    /*! Construct a default cylinder with name \a name.
        The resolution for approximating the cylinder is set to \a resolution. */
  Cylinder( const string &name, int resolution=0 );
    /*! Construct a cylinder with name \a name from \a anchor, \a radius, and \a length.
        The anchor point is the center of the left circle.
        The resolution for approximating the cylinder is set to \a resolution. */
  Cylinder( const Point &anchor, double radius, double length,
	    const string &name="cylinder", int resolution=0 );

    /*! Returns a pointer to a copy of the cylinder. */
  virtual Shape *copy( void ) const;

    /*! The radius of the cylinder in world coordinates. */
  double radius( void ) const;
    /*! The length of the cylinder in world coordinates. */
  double length( void ) const;

    /*! Reset the polygons making up the cylinder to the ones in shape coordinates. */
  virtual void resetPolygons( void ) const;

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
    /*! Construct a default cuboid with name \a name. */
  Cuboid( const string &name );
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

    /*! Reset the polygons making up the cuboid to the ones in shape coordinates. */
  virtual void resetPolygons( void ) const;

  using Shape::boundingBoxMin;
  using Shape::boundingBoxMax;

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

    /*! Compatibility with old robot plugins: */

    /*! Return length(), width(), height(). */
  Point corner( void ) const;
    /*! Set length, width and height of Cuboid under the assumption that
        it is not roated! */
  void setCorner( const Point &corner );
    /*!  Set length (extend along x-axis) of cuboid to \a length.
         Assumes that Cuboid is not rotated! */
  void setLength( double length );
    /*!  Set width (extend along y-axis) of cuboid to \a width.
         Assumes that Cuboid is not rotated! */
  void setWidth( double width );
    /*!  Set height (extend along z-axis) of cuboid to \a height.
         Assumes that Cuboid is not rotated! */
  void setHeight( double height );

};


}; /* namespace relacs */

#endif /* ! _RELACS_SHAPE_H_ */
