/*
  linearrange.h
  A linear range of discretized numbers.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_LINEARRANGE_H_
#define _RELACS_LINEARRANGE_H_ 1

#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

namespace relacs {


/*! 
\class LinearRange
\author Jan Benda
\version 0.1
\brief A linear range of discretized numbers.

*/ 


class LinearRangeIterator;


class LinearRange 
{

  public:

    /*! Creates an empty range. */
  LinearRange( void );
    /*! Creates a range with \a n data elements
        starting at 0 and incremented by 1. */
  LinearRange( int n );
    /*! Creates a range with \a n data elements
        starting at 0 and incremented by 1. */
  LinearRange( long n );
    /*! Creates a range with \a n data elements
        starting at 0 and incremented by \a stepsize. */
  LinearRange( int n, double stepsize );
    /*! Creates a range with \a n data elements
        starting at 0 and incremented by \a stepsize. */
  LinearRange( long n, double stepsize );
    /*! Creates a range with \a n data elements
        starting at \a offset and incremented by \a stepsize. */
  LinearRange( int n, double offset, double stepsize );
    /*! Creates a range with \a n data elements
        starting at \a offset and incremented by \a stepsize. */
  LinearRange( long n, double offset, double stepsize );
    /*! Creates a range starting at 0, ending at \a r,
        with increments \a stepsize. 
        If \a stepsize equals zero it is set to \a r. */
  LinearRange( double r, double stepsize );
    /*! Creates a range starting at \a l, ending at \a r,
        with increments \a stepsize. 
        If \a stepsize equals zero it is set to \a r - \a l. */
  LinearRange( double l, double r, double stepsize );
    /*! Copy constructor.
        Creates a range with the same offset, stepsize and size as range \a r. */
  LinearRange( const LinearRange &r );
    /*! The destructor. */
  virtual ~LinearRange( void );

    /*! Set the offset, stepsize and size to the values of range \a r. */
  const LinearRange &operator=( const LinearRange &r ) { return assign( r ); };

    /*! Set the offset, stepsize and size to the values of range \a r. */
  const LinearRange &assign( const LinearRange &r );
    /*! Set the offset, stepsize and size to the values of range \a r
        from index \a first (inclusively) to \a last (exclusively). */
  const LinearRange &assign( const LinearRange &r, long first, long last=-1 );
    /*! Creates a range with \a n data elements, stepsize \a stepsize,
        and offset 0. */
  const LinearRange &assign( int n, double stepsize );
    /*! Creates a range with \a n data elements, stepsize \a stepsize,
        and offset 0. */
  const LinearRange &assign( long n, double stepsize );
    /*! Creates a range with \a n data elements
        starting at \a offset and incremented by \a stepsize. */
  const LinearRange &assign( int n, double offset, double stepsize );
  const LinearRange &assign( long n, double offset, double stepsize );
    /*! Creates a range starting at 0, ending at \a r,
        with increments \a stepsize. 
        If \a stepsize equals zero it is set to \a r. */
  const LinearRange &assign( double r, double stepsize );
    /*! Creates a range starting at \a l, ending at \a r,
        with increments \a stepsize. 
        If \a stepsize equals zero it is set to \a r - \a l. */
  const LinearRange &assign( double l, double r, double stepsize );

    /*! Make \a r a copy of the range. */
  const LinearRange &copy( LinearRange &r ) const;
    /*! Make \a r a copy of the range
        from index \a first (inclusively) to \a last (exclusively). */
  const LinearRange &copy( LinearRange &r, long first, long last=-1 ) const;

    /*! Append \a n elements to the range. */
  const LinearRange &append( long n );

    /*! The size of the range, 
        i.e. the number of data elements. */
  long size( void ) const { return NSize; };
    /*! True if the range does not contain any data elements. */
  bool empty( void ) const { return ( NSize <= 0 ); };
    /*! Resize the range to \a n elements 
        without changing the offset() and the stepsize(). */
  void resize( long n ) { NSize = n >= 0 ? n : 0; };
    /*! Resize the range to zero length. */
  void clear( void ) { NSize = 0; };

     /*! The offset of the range. */
  double offset( void ) const { return Offset; };
     /*! Set the offset of the range to \a offset. */
  void setOffset( double offset ) { Offset = offset; };
     /*! The stepsize of the range. */
  double stepsize( void ) const { return Stepsize; };
     /*! Set the stepsize of the range to \a stepsize.
         This also changes length() and rangeBack(). */
  void setStepsize( double stepsize ) { Stepsize = stepsize; };
     /*! Multiply the stepsize of the range by \a scale
         and adjust the size of the range appropriately.
         This does not change offset(), length(), and rangeBack(). */
  void scaleStepsize( double scale );
     /*! Set the offset and the stepsize of the range to \a offset and \a stepsize, respectively. */
  void setRange( double offset, double stepsize ) { Offset=offset; Stepsize=stepsize; };
     /*! The length of the range, i.e. abs( stepsize() * size() ) */
  double length( void ) const { return ::fabs( stepsize()*size() ); };
     /*! Set the size of the range such that it has the length \a l. */
  void setLength( double l ) { resize( (int)::ceil( ::fabs( l/stepsize() ) ) ); };
    /*! Returns the first range element. Same as offset(). */
  double front( void ) const;
    /*! Set the offset of the range to \a front without changing 
        the last range element. */
  void setFront( double front );
    /*! Returns the last range element. */
  double back( void ) const;
    /*! Resize the range such that the last range element equals \a back. */
  void setBack( double back );

    /*! Returns the range element at index \a i. */
  double operator[]( long i ) const { return pos( i ); };
    /*! Returns the range element at index \a i. */
  double at( long i ) const { return pos( i ); };
    /*! Returns the range element at index \a i. */
  double pos( long i ) const { return Offset + i * Stepsize; };
    /*! Returns the interval covered by \a indices indices. */
  double interval( int indices ) const { return indices * Stepsize; };

    /*! The index of the range corresponding to \a pos. */
  long index( double pos ) const { return long( ::floor( (pos - offset())/stepsize() ) ); };
    /*! The number of indices corresponding to an interval \a iv. */
  long indices( double iv ) const { return long( ::floor( iv/stepsize() ) ); };
    /*! True if \a pos is within the range. */
  bool contains( double pos ) const;

    /*! Add an element to the range. */
  void push( void ) { NSize++; };
    /*! Remove an element from the range. */
  void pop( void );

    /*! The type of an element of the range (double). */
  typedef double value_type;
    /*! Pointer to the type of an element of the range (double). */
  typedef double* pointer;
    /*! Reference to the type of an element of the range (double). */
  typedef double& reference;
    /*! Const reference to the type of an element of the range (double). */
  typedef const double& const_reference;
    /*! The type used for sizes and indices. */
  typedef long size_type;
    /*! Const iterator used to iterate through a range. */
  typedef LinearRangeIterator const_iterator;

    /*! Returns an const_iterator pointing to the first element of the range. */
  const_iterator begin( void ) const;
    /*! Returns an const_iterator pointing behind the last element of the range. */
  const_iterator end( void ) const;

    /*! True if range \a a and \a b are equal. */
  friend bool operator==( const LinearRange &r, const LinearRange &b );
    /*! True if range \a a is smaller than \a b, i.e. either \a is smaller in size than \a b
        or the offset of \a is smaller or the stepsize of \a is smaller. */
  friend bool operator<( const LinearRange &r, const LinearRange &b );

    /*! Set the stepsize to zero and the offset to \a val. */
  const LinearRange &operator=( double val );
    /*! Add \a val to the offset, i.e. shift the range by \a val. */
  const LinearRange &operator+=( double val );
    /*! Subtract \a val from the offset, i.e. shift the range by \a -val. */
  const LinearRange &operator-=( double val );
    /*! Multiply the offset and the stepsize with \a val, 
        i.e. rescale the range by \a val. */
  const LinearRange &operator*=( double val );
    /*! Divide the offset and the stepsize by \a val, 
        i.e. rescale the range by \a 1/val. */
  const LinearRange &operator/=( double val );

    /*! Add \a val to the offset of the range \a r,
        i.e. shift the range by \a val. */
  friend LinearRange operator+( const LinearRange &r, double val );
    /*! Add \a val to the offset of the range \a r,
        i.e. shift the range by \a val. */
  friend LinearRange operator+( double val, const LinearRange &r );
    /*! Subtract \a val from the offset of the range \a r,
        i.e. shift the range by \a -val. */
  friend LinearRange operator-( const LinearRange &r, double val );
    /*! Subtract the offset of the range \a r from \a val
        and multiply the stepsize by -1. */
  friend LinearRange operator-( double val, const LinearRange &r );
    /*! Multiply the offset and the stepsize of the range \a r with \a val, 
        i.e. rescale the range by \a val. */
  friend LinearRange operator*( const LinearRange &r, double val );
    /*! Multiply the offset and the stepsize of the range \a r  with \a val, 
        i.e. rescale the range by \a val. */
  friend LinearRange operator*( double val, const LinearRange &r );
    /*! Divide the offset and the stepsize of the range \a r by \a val, 
        i.e. rescale the range by \a 1/val. */
  friend LinearRange operator/( const LinearRange &r, double val );

    /*! The minimum value of the range between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set to size(). */
  double min( long first=0, long last=-1 ) const;
    /*! The index of the element with the minimum value of the range between indices
        \a first (inclusively) and \a last (exclusively), i.e.
	either \a first or \a last are returned.
        If \a last is negative it is set to size(). */
  long minIndex( long first=0, long last=-1 ) const;
    /*! The index of the element with the minimum value 
        of the range between indices
        \a first (inclusively) and \a last (exclusively), i.e.
	either \a first or \a last are returned. 
        If \a last is negative it is set to size().
        The value of the minimum element is returned in \a min. */
  long minIndex( double &min, long first=0, long last=-1 ) const;

    /*! The maximum value of the range between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set to size(). */
  double max( long first=0, long last=-1 ) const;
    /*! The index of the element with the maximum value of the range between indices
        \a first (inclusively) and \a last (exclusively), i.e.
	either \a first or \a last are returned. 
        If \a last is negative it is set to size(). */
  long maxIndex( long first=0, long last=-1 ) const;
    /*! The index of the element with the maximum value 
        of the range between indices
	\a first (inclusively) and \a last (exclusively), i.e.
	either \a first or \a last are returned. 
        If \a last is negative it is set to size().
	The value of the maximum element is returned in \a max. */
  long maxIndex( double &max, long first=0, long last=-1 ) const;

    /*! The minimum value \a min and maximum value \a max
        of the range between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set to size(). */
  void minMax( double &min, double &max, long first=0, long last=-1 ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum and the maximum value of the range between indices
        \a first (inclusively) and \a last (exclusively),
	i.e. \a first or \a last.
        If \a last is negative it is set to size(). */
  void minMaxIndex( long &minindex, long &maxindex, long first=0, long last=-1 ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum value \a min and the maximum value \a max
        of the range between indices
	\a first (inclusively) and \a last (exclusively),
	i.e. \a first or \a last. 
        If \a last is negative it is set to size(). */
  void minMaxIndex( double &min, long &minindex, double &max, long &maxindex,
		    long first=0, long last=-1 ) const;

  friend ostream &operator<<( ostream &str, const LinearRange &r );


 private:

  double Offset;
  double Stepsize;
  long NSize;

};


/*! 
  \class LinearRangeIterator
  \author Jan Benda
  \version 1.0
  \brief Iterator for a LinearRange
*/
  
class LinearRangeIterator
{
    
public:
    
    /*! Constructs an empty invalid iterator for a Range. */
  LinearRangeIterator( void ) 
    : Index( 0 ), Offset( 0.0 ), Stepsize( -1.0 ) {};
    /*! Constructs a valid iterator for a range with offset \a offset
        and stepsize \a stepsize pointing to element \a index. */
    LinearRangeIterator( double offset, double stepsize, long index ) 
    : Index( index ), Offset( offset ), Stepsize( stepsize ) {};
    /*! Constructs a valid iterator for a Range \a r
        pointing to element \a index. */
  LinearRangeIterator( const LinearRange &r, long index ) 
    : Index( index ), Offset( r.offset() ), Stepsize( r.stepsize() ) {};
    /*! Copy constructor. */
  LinearRangeIterator( const LinearRangeIterator &p )
    : Index( p.Index ), Offset( p.Offset ), Stepsize( p.Stepsize ) {};
    /*! Destructor. */
  ~LinearRangeIterator( void ) {};
    
    /*! Assigns \a p to this. */
  LinearRangeIterator &operator=( const LinearRangeIterator &p );
    
    /*! Returns true if both iterators point to the same element 
        of the same instance of a Range. */
  inline bool operator==( const LinearRangeIterator &p ) const
    { return ( Offset == p.Offset && Stepsize == p.Stepsize && Index == p.Index ); };
    /*! Returns true if the iterators do not point to the same element 
        of the same instance of a Range. */
  inline bool operator!=( const LinearRangeIterator &p ) const
    { return ( Offset != p.Offset || Stepsize != p.Stepsize || Index != p.Index ); };
    /*! Returns true if \a this points to an element preceeding the 
        element where \a points to. */
  inline bool operator<( const LinearRangeIterator &p ) const
    { return ( Offset == p.Offset && Stepsize == p.Stepsize && Index < p.Index ); };
    /*! Returns true if \a this points to an element succeeding the 
        element where \a p points to. */
  inline bool operator>( const LinearRangeIterator &p ) const
    { return ( Offset == p.Offset && Stepsize == p.Stepsize && Index > p.Index ); };
    /*! Returns true if \a this points to an element preceeding 
        or equaling the element where this points to. */
  inline bool operator<=( const LinearRangeIterator &p ) const
    { return ( Offset == p.Offset && Stepsize == p.Stepsize && Index <= p.Index ); };
    /*! Returns true if \a this points to an element succeeding
        or preceeding the element where this points to. */
  inline bool operator>=( const LinearRangeIterator &p ) const
    { return ( Offset == p.Offset && Stepsize == p.Stepsize && Index >= p.Index ); };

    /*! Increments the iterator to the next element. */
  inline const LinearRangeIterator &operator++( void )
    { Index++; return *this; };
    /*! Decrements the iterator to the previous element. */
  inline const LinearRangeIterator &operator--( void )
    { Index--; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const LinearRangeIterator &operator+=( unsigned int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const LinearRangeIterator &operator+=( signed int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const LinearRangeIterator &operator+=( unsigned long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const LinearRangeIterator &operator+=( signed long incr )
    { Index += incr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const LinearRangeIterator &operator-=( unsigned int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const LinearRangeIterator &operator-=( signed int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const LinearRangeIterator &operator-=( unsigned long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const LinearRangeIterator &operator-=( signed long decr )
    { Index -= decr; return *this; };
    /*! Adds \a incr to the iterator. */
  inline LinearRangeIterator operator+( unsigned int incr ) const
    { LinearRangeIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline LinearRangeIterator operator+( signed int incr ) const
    { LinearRangeIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline LinearRangeIterator operator+( unsigned long incr ) const
    { LinearRangeIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline LinearRangeIterator operator+( signed long incr ) const
    { LinearRangeIterator p( *this ); p.Index += incr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline LinearRangeIterator operator-( unsigned int decr ) const
    { LinearRangeIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline LinearRangeIterator operator-( signed int decr ) const
    { LinearRangeIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline LinearRangeIterator operator-( unsigned long decr ) const
    { LinearRangeIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline LinearRangeIterator operator-( signed long decr ) const
    { LinearRangeIterator p( *this ); p.Index -= decr; return p; };
    /*! Returns the number of elements between the two iterators. */
  inline int operator-( const LinearRangeIterator &p ) const
    { if ( Offset == p.Offset && Stepsize == p.Stepsize ) return Index - p.Index; return 0; };

    /*! Returns the value of the range where the iterator points to. */
  inline double operator*( void ) const
    { return Offset + Stepsize*Index; };
    /*! Returns the value of the range where the iterator + n points to. */
  inline double operator[]( long n ) const
    { return Offset + Stepsize*(Index+n); };
    
    
protected:

  int Index;    
  double Offset;
  double Stepsize;
    
};


}; /* namespace relacs */

#endif /* ! _RELACS_LINEARRANGE_H_ */
