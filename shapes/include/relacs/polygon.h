/*
  polygon.h
  A planar polygon in 3D space.

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

#ifndef _RELACS_POLYGON_H
#define _RELACS_POLYGON_H 1

#include <deque>
#include <vector>
#include <iostream>
#include <relacs/point.h>
using namespace std;

namespace relacs {


class Transform;
class Shape;


/*!
\class Polygon
\brief A planar polygon in 3D space.
\author Jan Benda

A polygon contains a list of its corner points and a normal vector
pointing to the outside.
 */

class Polygon {

public:

    /*! Constructs an empty polygon. */
  Polygon( void );
    /*! Copy constructor. */
  Polygon( const Polygon &p );

    /*! The number of corner points. */
  int size( void ) const { return Points.size(); };
    /*! True if no corner points are assigned to the polygon yet. */
  bool empty( void ) const { return Points.empty(); };

    /*! The first corner point of the polygon. */
  Point front( void ) const { return Points.front(); };
    /*! The first corner point of the polygon. */
  Point &front( void ) { return Points.front(); };
    /*! The last corner point of the polygon. */
  Point back( void ) const { return Points.back(); };
    /*! The last corner point of the polygon. */
  Point &back( void ) { return Points.back(); };

    /*! The i-th corner point of the polygon. */
  Point operator[]( int i ) const { return Points[i]; };
    /*! The i-th corner point of the polygon. */
  Point &operator[]( int i ) { return Points[i]; };

    /*! Add Point \a p to the list of corner points. */
  void push( const Point &p ) { Points.push_back( p ); };
    /*! Delete all corner points from the polygon. */
  void clear( void ) { Points.clear(); };

    /*! Return the normal vector of the polygon, which can be Point::None. */
  Point normal( void ) const;
    /*! Set the normal vector of the polygon to \a normal.
        \a normal must be normalized. */
  void setNormal( const Point &normal );
    /*! Set the normal vector of the polygon to the cross product
        between the two vector p_1-p_0 and p_last - p_0.. */
  void setNormal( void );
    /*! Flip the direction of the normal vector of the polygon. */
  void flipNormal( void );

    /*! Assign the properties of polygon \a p to this polygon. */
  Polygon &assign( const Polygon &p );
    /*! Assign the properties of polygon \a p to this polygon. */
  Polygon &operator=( const Polygon &p );

  /*! Apply the transformation matrix \a trafo on all the corner
      points and \a invtransptrafo to the normal of the polygon. */
  void apply( const Transform &trafo, const Transform &invtransptrafo );

    /*! Return the center of gravity of the polygon points. */
  Point center( void ) const;
    /*! Apply the projection matrix onto each of the points of the polygon
        and return the resulting x- and y-coordinates. */
  void project( const Transform &trafo, vector<double> &x, vector<double> &y ) const;

    /*! True if all corner points of the polygon in world coordinates
        are inside the shape \a shape. */
  bool inside( const Shape &shape ) const;
    /*! True if all corner points of the polygon in shape coordinates
        are inside the shape \a shape. */
  bool insideShape( const Shape &shape ) const;
    /*! True if all corner points of the polygon in world coordinates
        are outside the shape \a shape. */
  bool outside( const Shape &shape ) const;
    /*! True if all corner points of the polygon in shape coordinates
        are outside the shape \a shape. */
  bool outsideShape( const Shape &shape ) const;

    /*! Intersect this polygon with \a polygon. If the polygons
        intersect they are cut back. */
  Polygon intersect( Polygon &polygon );

    /*! Write the coordinates of corner points of the polygon to stream \a str. */
  friend ostream &operator<<( ostream &str, const Polygon &p );


protected:

    /*! List of points that make up the polygon. */
  deque<Point> Points;
    /*! The normal vector pointing to the outside of the polygone plane. */
  Point Normal;
    /*! The dot product between the normal vector and the first point of the polygon. */
  double Dist;

};


}; /* namespace relacs */

#endif /* ! _RELACS_POLYGON_H_ */
