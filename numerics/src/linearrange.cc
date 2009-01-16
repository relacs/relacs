/*
  linearrange.cc
  A linear range of discretized numbers.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/linearrange.h>

namespace relacs {


LinearRange::LinearRange( void )
  : Offset( 0 ),
    Stepsize( 1 ),
    NSize( 0 )
{
}


LinearRange::LinearRange( int n )
  : Offset( 0 ),
    Stepsize( 1 ),
    NSize( n )
{
}


LinearRange::LinearRange( long n )
  : Offset( 0 ),
    Stepsize( 1 ),
    NSize( n )
{
}


LinearRange::LinearRange( int n, double stepsize )
  : Offset( 0 ),
    Stepsize( stepsize ),
    NSize( n )
{
}


LinearRange::LinearRange( long n, double stepsize )
  : Offset( 0 ),
    Stepsize( stepsize ),
    NSize( n )
{
}


LinearRange::LinearRange( int n, double offset, double stepsize )
  : Offset( offset ),
    Stepsize( stepsize ),
    NSize( n )
{
}


LinearRange::LinearRange( long n, double offset, double stepsize )
  : Offset( offset ),
    Stepsize( stepsize ),
    NSize( n )
{
}


LinearRange::LinearRange( double r, double stepsize )
  : Offset( 0 ),
    Stepsize( stepsize ),
    NSize( 0 )
{
  if ( Stepsize == 0 )
    Stepsize = r;

  if ( r == 0 ) {
    if ( Stepsize == 0 )
      Stepsize = 1;
  }
  else {
    //  NSize = (long)::floor( 1.5 + r / Stepsize );
    NSize = index( r ) + 1;
  }
}


LinearRange::LinearRange( double l, double r, double stepsize )
  : Offset( l ),
    Stepsize( stepsize ),
    NSize( 0 )
{
  if ( Stepsize == 0 )
    Stepsize = r - l;

  if ( r == l ) {
    if ( Stepsize == 0 )
      Stepsize = 1;
  }
  else {
    //  NSize = (long)::floor( 1.5 + ( r - l ) / Stepsize );
    NSize = index( r ) + 1;
  }
}


LinearRange::LinearRange( const LinearRange &r )
  : Offset( r.offset() ),
    Stepsize( r.stepsize() ),
    NSize( r.size() )
{
}


LinearRange::~LinearRange( void )
{
}


const LinearRange &LinearRange::assign( const LinearRange &r )
{
  if ( &r == this )
    return *this;

  Offset = r.offset();
  Stepsize = r.stepsize();
  NSize = r.size();

  return *this;
}


const LinearRange &LinearRange::assign( const LinearRange &r, 
					long first, long last )
{
  if ( last < 0 )
    last = r.size();
  Offset = r.pos( first );
  Stepsize = r.stepsize();
  NSize = last - first;

  return *this;
}


const LinearRange &LinearRange::assign( int n, double stepsize )
{
  Offset = 0;
  Stepsize = stepsize;
  NSize = n;
  return *this;
}


const LinearRange &LinearRange::assign( long n, double stepsize )
{
  Offset = 0;
  Stepsize = stepsize;
  NSize = n;
  return *this;
}


const LinearRange &LinearRange::assign( int n, double offset, double stepsize )
{
  Offset = offset;
  Stepsize = stepsize;
  NSize = n;
  return *this;
}


const LinearRange &LinearRange::assign( long n, double offset, double stepsize )
{
  Offset = offset;
  Stepsize = stepsize;
  NSize = n;
  return *this;
}


const LinearRange &LinearRange::assign( double r, double stepsize )
{
  Offset = 0;
  Stepsize = stepsize == 0 ? r : stepsize;
  //  NSize = (long)::floor( 1.5 + r / Stepsize );
  NSize = index( r ) + 1;
  return *this;
}


const LinearRange &LinearRange::assign( double l, double r, double stepsize )
{
  Offset = l;
  Stepsize = stepsize == 0 ? r - l : stepsize;
  //  NSize = (long)::floor( 1.5 + ( r - l ) / Stepsize );
  NSize = index( r ) + 1;
  return *this;
}


const LinearRange &LinearRange::copy( LinearRange &r ) const
{
  r.assign( *this );
  return *this;
}


const LinearRange &LinearRange::copy( LinearRange &r, long first, long last ) const
{
  r.assign( *this, first, last );
  return *this;
}


const LinearRange &LinearRange::append( long n )
{
  NSize += n;

  return *this;
}


void LinearRange::scaleStepsize( double scale )
{
  double b = back();
  Stepsize *= scale;
  setBack( b );
}


double LinearRange::front( void ) const
{
  return offset();
}


void LinearRange::setFront( double front )
{
  if ( front > back() )
    return;

  long n = indices( front - Offset );
  Offset = pos( n );
  resize( size() - n );
}


double LinearRange::back( void ) const
{
  return pos( NSize );
}


void LinearRange::setBack( double b )
{
  if ( b < front() )
    return;

  long n = index( b );
  resize( n );
}


bool LinearRange::contains( double p ) const
{
  if ( stepsize() >= 0.0 )
    return ( p >= offset() && p <= pos( size() ) );
  else
    return ( p <= offset() && p >= pos( size() ) );
}


void LinearRange::pop( void )
{
  if ( NSize > 0 )
    NSize--;
}


LinearRange::const_iterator LinearRange::begin( void ) const 
{
  return LinearRangeIterator( *this, 0 );
}


LinearRange::const_iterator LinearRange::end( void ) const
{
  return LinearRangeIterator( *this, size() );
}


bool operator==( const LinearRange &a, const LinearRange &b )
{
  return ( a.size() == b.size() &&
	   a.offset() == b.offset() &&
	   a.stepsize() == b.stepsize() );
}


bool operator<( const LinearRange &a, const LinearRange &b )
{
  if ( a.size() < b.size() )
    return true;

  if ( a.offset() < b.offset() )
    return true;

  if ( a.stepsize() < b.stepsize() )
    return true;

  return false;
}


const LinearRange &LinearRange::operator=( double val )
{
  Stepsize = 0.0;
  Offset = val;
  return *this;
}


const LinearRange &LinearRange::operator+=( double val )
{
  Offset += val;
  return *this;
}


const LinearRange &LinearRange::operator-=( double val )
{
  Offset -= val;
  return *this;
}


const LinearRange &LinearRange::operator*=( double val )
{
  Offset *= val;
  Stepsize *= val;
  return *this;
}


const LinearRange &LinearRange::operator/=( double val )
{
  Offset /= val;
  Stepsize /= val;
  return *this;
}


LinearRange operator+( const LinearRange &r, double val )
{
  LinearRange c( r );
  c.Offset += val;
  return c;
}


LinearRange operator+( double val, const LinearRange &r )
{
  LinearRange c( r );
  c.Offset += val;
  return c;
}


LinearRange operator-( const LinearRange &r, double val )
{
  LinearRange c( r );
  c.Offset -= val;
  return c;
}


LinearRange operator-( double val, const LinearRange &r )
{
  LinearRange c( r );
  c.Offset = val - c.Offset;
  c.Stepsize *= -1;
  return c;
}


LinearRange operator*( const LinearRange &r, double val )
{
  LinearRange c( r );
  c.Offset *= val;
  c.Stepsize *= val;
  return c;
}


LinearRange operator*( double val, const LinearRange &r )
{
  LinearRange c( r );
  c.Offset *= val;
  c.Stepsize *= val;
  return c;
}


LinearRange operator/( const LinearRange &r, double val )
{
  LinearRange c( r );
  c.Offset /= val;
  c.Stepsize /= val;
  return c;
}


double LinearRange::min( long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  return f < l ? f : l;
}


long LinearRange::minIndex( long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  return f < l ? first : last;
}


long LinearRange::minIndex( double &min, long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  if ( f < l ) {
    min = f;
    return first;
  }
  else { 
    min = l;
    return last;
  }
}


double LinearRange::max( long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  return f > l ? f : l;
}


long LinearRange::maxIndex( long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  return f > l ? first : last;
}


long LinearRange::maxIndex( double &max, long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  if ( f > l ) {
    max = f;
    return first;
  }
  else { 
    max = l;
    return last;
  }
}


void LinearRange::minMax( double &min, double &max, long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  if ( f > l ) {
    max = f;
    min = l;
  }
  else { 
    max = l;
    min = f;
  }
}


void LinearRange::minMaxIndex( long &minindex, long &maxindex, long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  if ( f > l ) {
    maxindex = first;
    minindex = last;
  }
  else { 
    maxindex = last;
    minindex = first;
  }
}


void LinearRange::minMaxIndex( double &min, long &minindex, double &max, long &maxindex,
			    long first, long last ) const
{
  if ( last < 0 )
    last = size();
  double f = pos( first );
  double l = pos( last );
  if ( f > l ) {
    max = f;
    min = l;
    maxindex = first;
    minindex = last;
  }
  else { 
    max = l;
    min = f;
    maxindex = last;
    minindex = first;
  }
}


ostream &operator<<( ostream &str, const LinearRange &r )
{
  for ( long k=0; k<r.size(); k++ )
    str << r.pos( k ) << '\n';
  return str;
}


LinearRangeIterator &LinearRangeIterator::operator=( const LinearRangeIterator &p )
{
  if ( &p == this )
    return *this;

  Offset = p.Offset;
  Stepsize = p.Stepsize;
  Index = p.Index;

  return *this;
}


}; /* namespace relacs */

