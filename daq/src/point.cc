/*
  point.cc
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

#include <math.h>
#include <relacs/point.h>
#include <relacs/str.h>


namespace relacs {


Point::Point( void )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] = 0;
}


Point::Point( const Point &p )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] = p.Coords[k];
}


Point::Point( double x, double y, double z )
{
  Coords[0] = x;
  Coords[1] = y;
  Coords[2] = z;
}


Point::Point( const string &position ) : Point( )
{
  if ( position.size() > 0 ) {
    Str s = position;
    int next;
    int idx = 0;
    for (int i = 0; i < Dim; i++) {
      Coords[i] = s.number(0.0, idx, &next);
      idx = next + 1;
      if (next > position.size()-1)
	break;
    }
  }
}


void Point::set( double x, double y, double z )
{
  Coords[0] = x;
  Coords[1] = y;
  Coords[2] = z;
}


Point Point::operator+( const Point &p ) const
{
  Point q( *this );
  for ( int k=0; k<Dim; k++ )
    q[k] += p[k];
  return q;
}


Point Point::operator-( const Point &p ) const
{
  Point q( *this );
  for ( int k=0; k<Dim; k++ )
    q[k] -= p[k];
  return q;
}


Point &Point::operator+=( const Point &p )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] += p[k];
  return *this;
}


Point &Point::operator-=( const Point &p )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] -= p[k];
  return *this;
}


Point Point::operator+( double a ) const
{
  Point p( *this );
  for ( int k=0; k<Dim; k++ )
    p[k] += a;
  return p;
}


Point Point::operator-( double a ) const
{
  Point p( *this );
  for ( int k=0; k<Dim; k++ )
    p[k] -= a;
  return p;
}


Point Point::operator*( double a ) const
{
  Point p( *this );
  for ( int k=0; k<Dim; k++ )
    p[k] *= a;
  return p;
}


Point Point::operator/( double a ) const
{
  Point p( *this );
  for ( int k=0; k<Dim; k++ )
    p[k] /= a;
  return p;
}


Point &Point::operator+=( double a )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] += a;
  return *this;
}


Point &Point::operator-=( double a )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] -= a;
  return *this;
}


Point &Point::operator*=( double a )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] *= a;
  return *this;
}


Point &Point::operator/=( double a )
{
  for ( int k=0; k<Dim; k++ )
    Coords[k] /= a;
  return *this;
}


double Point::operator*( const Point &p ) const
{
  double d;
  for ( int k=0; k<Dim; k++ )
    d += Coords[k]*p[k];
  return d;
}


bool Point::operator==( const Point &p ) const
{
  for ( int k=0; k<Dim; k++ ) {
    if ( Coords[k] != p[k] )
      return false;
  }
  return true;
}


bool Point::operator<( const Point &p ) const
{
  for ( int k=0; k<Dim; k++ ) {
    if ( Coords[k] >= p[k] )
      return false;
  }
  return true;
}


bool Point::operator<=( const Point &p ) const
{
  for ( int k=0; k<Dim; k++ ) {
    if ( Coords[k] > p[k] )
      return false;
  }
  return true;
}


bool Point::operator>( const Point &p ) const
{
  for ( int k=0; k<Dim; k++ ) {
    if ( Coords[k] <= p[k] )
      return false;
  }
  return true;
}


bool Point::operator>=( const Point &p ) const
{
  for ( int k=0; k<Dim; k++ ) {
    if ( Coords[k] < p[k] )
      return false;
  }
  return true;
}


double Point::distance( const Point &p ) const
{
  double d = 0.0;
  for ( int k=0; k<Dim; k++ )
    d += ::pow( Coords[k] - p[k], 2.0 );
  return ::sqrt( d );
}


Point Point::center( const Point &p ) const
{
  Point middle;
  for ( int k=0; k<Dim; k++ )
    middle[k] = 0.5*(Coords[k] + p[k]);
  return middle;
}


Point abs( Point p )
{
  for ( int k=0; k<p.Dim; k++ )
    p[k] = ::abs( p[k] );
  return p;
}


string Point::toString( void ) const {
  Str s = "(";
  for ( int i = 0; i < Dim; i++ ) {
    s += Str(Coords[i], 0, 3, 'f') + (i < Dim-1 ? "," : "");
  }
  s += ")";
  return s;
}


ostream &operator<<( ostream &str, const Point &p ) 
{
  str << "point x=" << p[0] << ", ";
  str << "y=" << p[1] << ", ";
  str << "z=" << p[2] << "\n";
  return str;
}


}; /* namespace relacs */
