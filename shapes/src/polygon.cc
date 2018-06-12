/*
  polygon.cc
  A polygon in 3D space.

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

#include <cmath>
#include <iomanip>
#include <relacs/transform.h>
#include <relacs/shape.h>
#include <relacs/polygon.h>


namespace relacs {


Polygon::Polygon( void )
  : Normal( Point::None ),
    Dist( 0.0 )
{
  Points.clear();
}


Polygon::Polygon( const Polygon &p )
  : Points( p.Points ),
    Normal( p.Normal ),
    Dist( p.Dist )
{
}


Point Polygon::normal( void ) const
{ 
  return Normal;
}


void Polygon::setNormal( const Point &normal )
{
  Normal = normal;
  Dist = Normal.dot( Points.front() );
}


void Polygon::setNormal( void )
{
  if ( Normal.isNone() && Points.size() > 2 ) {
    Point n = (Points[1] - Points[0]).cross( Points.back() - Points.front() );
    Normal = n.normalized();
    Dist = Normal.dot( Points.front() );
  }
}


Polygon &Polygon::assign( const Polygon &p )
{
  Points = p.Points;
  Normal = p.Normal;
  Dist = p.Dist;
  return *this;
}


Polygon &Polygon::operator=( const Polygon &p )
{
  Points = p.Points;
  Normal = p.Normal;
  Dist = p.Dist;
  return *this;
}


void Polygon::flipNormal( void )
{
  Normal *= -1.0;
  Dist *= -1.0;
}


void Polygon::apply( const Transform &trafo, const Transform &invtransptrafo )
{
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi )
    *pi *= trafo;
  Normal *= invtransptrafo;
  Normal /= Normal.magnitude();
  Dist = Normal.dot( Points.front() );
}


Point Polygon::center( void ) const
{
  if ( Points.empty() )
    return Point::None;

  Point c = Point::Origin;
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi )
    c += *pi;
  return c/Points.size();
}


void Polygon::project( const Transform &trafo, vector<double> &x, vector<double> &y ) const
{
  x.clear();
  y.clear();
  x.reserve( Points.size() );
  y.reserve( Points.size() );
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    Point p = trafo * (*pi);
    p.homDivide();
    x.push_back( p.x() );
    y.push_back( p.y() );
  }
}


bool Polygon::inside( const Shape &shape ) const
{
  if ( Points.empty() )
    return false;

  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( ! shape.inside( *pi ) )
      return false;
  }
  return true;
}


bool Polygon::insideShape( const Shape &shape ) const
{
  if ( Points.empty() )
    return false;

  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( ! shape.insideShape( *pi ) )
      return false;
  }
  return true;
}


bool Polygon::outside( const Shape &shape ) const
{
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( shape.inside( *pi ) )
      return false;
  }
  return true;
}


bool Polygon::outsideShape( const Shape &shape ) const
{
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    if ( shape.insideShape( *pi ) )
      return false;
  }
  return true;
}


Polygon Polygon::intersect( Polygon &polygon )
{
  //  cerr << "Polygon::intersect\n";
  // one of the polygons is not a plane:
  if ( Normal.isNone() || polygon.Normal.isNone() ) {
    //    cerr << "  not a plane\n";
    return Polygon();
  }

  Point cp = Normal.cross( polygon.Normal );
  // polygons are parallel planes:
  if ( cp.magnitude() < 1.0e-8 ) {
    //    cerr << "  parallel planes\n";
    // XXX need to check whether they are coplanar!?
    return Polygon();
  }

  //  cerr << "polygon 1:\n" << *this;
  //  cerr << "polygon 2:\n" << polygon;
  // compute intersection line of polygon planes:
  double n12 = Normal.dot( polygon.Normal );
  double dn = 1.0 - n12*n12;
  double h1 = Dist;
  double h2 = polygon.Dist;
  double c1 = (h1-h2*n12)/dn;
  double c2 = (h2-h1*n12)/dn;
  Point p = c1*Normal + c2*polygon.Normal;
  cerr << p << " " << cp/cp.magnitude() << '\n';
  // intersection line = p + lambda*cp;
  Polygon line;
  line.push(p-5.0*cp/cp.magnitude());
  line.push(p+5.0*cp/cp.magnitude());
  //  cerr << "  intersection line\n";
  return line;
  /*
  // find intersections of polygon 1 with plane of polygon 2:
  Polygon line1;
  for ( auto pi = Points.begin(); pi != Points.end(); ++pi ) {
    auto ppi = pi + 1;
    if ( ppi == Points.end() )
      ppi = Points.begin();
    Point dp = *ppi - *pi;  // direction vector of polygon edge
    double ln = dp.dot( polygon.Normal );
    // edge parallel to plane -> no intersection point:
    if ( ::fabs( ln ) < 1e-8 )
      continue;
    double d = (polygon.Dist - (*pi).dot( polygon.Normal ))/ln;
    // intersection is not within edge:
    if ( d < -1e-8 || d > 1.0+1e-8 )
      continue;
    line1.push( *pi + d*dp );
  }
  if ( line1.size() > 2 )
    cerr << "line1: " << line1.size() << '\n';

  // find intersections of polygon 2 with plane of polygon 1:
  Polygon line2;
  for ( auto pi = polygon.Points.begin(); pi != polygon.Points.end(); ++pi ) {
    auto ppi = pi + 1;
    if ( ppi == polygon.Points.end() )
      ppi = polygon.Points.begin();
    Point dp = *ppi - *pi;  // direction vector of polygon edge
    double ln = dp.dot( Normal );
    // edge parallel to plane -> no intersection point:
    if ( ::fabs( ln ) < 1e-8 )
      continue;
    double d = (Dist - (*pi).dot( Normal ))/ln;
    // intersection is not within edge:
    if ( d < -1e-8 || d > 1.0+1e-8 )
      continue;
    line2.push( *pi + d*dp );
  }
  if ( line2.size() > 2 )
    cerr << "line2: " << line2.size() << '\n';

  if ( line1.size() < 2 || line2.size() < 2 )
    return Polygon();

  Point cp = Normal.cross( polygon.Normal );
  double n12 = Normal.dot( polygon.Normal );
  double dn = 1.0 - n12*n12;
  double h1 = Dist;
  double h2 = polygon.Dist;
  double c1 = (h1-h2*n12)/dn;
  double c2 = (h2-h1*n12)/dn;
  Point p = c1*Normal + c2*polygon.Normal;
  // intersection line = p + lambda*cp;
  Polygon line;
  line.push(p-5.0*cp/cp.magnitude());
  line.push(p+5.0*cp/cp.magnitude());
  return line;
  */
}


ostream &operator<<( ostream &str, const Polygon &p ) 
{
  int k=0;
  for ( auto pi = p.Points.begin(); pi != p.Points.end(); ++pi )
    str << k++ << " " << *pi << '\n';
  if ( ! p.Normal.isNone() ) {
    str << "N " << p.Normal << '\n';
    str << "h " << p.Dist << '\n';
  }
  return str;
}


}; /* namespace relacs */
