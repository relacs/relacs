/*
  sampledata.h
  A template defining an one-dimensional Array of data with an associated Range.

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

#ifndef _RELACS_SAMPLEDATA_H_
#define _RELACS_SAMPLEDATA_H_ 1

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <relacs/array.h>
#include <relacs/linearrange.h>
#include <relacs/containerfuncs.h>
#include <relacs/spectrum.h>
#include <relacs/eventdata.h>
#include <relacs/detector.h>

using namespace std;

namespace relacs {


/*! 
\class SampleData
\author Jan Benda
\version 0.5
\brief A template defining an one-dimensional Array of data with an associated Range.
\todo colored noise
\todo interface to class Kernel
\todo Handle mismatch of stepsize() in various functions.


SampleData is an Array of data elements of type \a T
and owns a LinearRange.
Each element of the LinearRange has an corresponding element in the Array.
SampleData is thus suited to store and handle evenly sampled data values.

For initializing a SampleData with a function, several interfaces are implemented.
For example, creating a SampleData with a range from 0.0 to 1.0, sampled
every 0.01 and initialized with a sine wave of frequency \a f,
the following alternatives are all possible and lead to the same result:
\code
  SampleDataD x();
  x = sin( LinearRange( 0.0, 1.0, 0.01 ), f );
  x = sin( 0.0, 1.0, 0.01, f );
  x = sin( 100, 0.0, 0.01 ), f );

  SampleDataD y( 0.0, 1.0, 0.01 );
  x = sin( y.range(), f );

  SampleDataD z();
  z.sin( LinearRange( 0.0, 1.0, 0.01 ), f );
  z.sin( 0.0, 1.0, 0.01, f );
  z.sin( 100, 0.0, 0.01, f );
\endcode
The following functions are supported in this way: 
sin(), cos(), gauss(),
alpha(), line(), rectangle(), sawUp(), sawDown(), and triangle().

The more general interface for computations with mathematical functions
is defined in \c basefuncs.h and operates directly on the array.
The range is neither affected nor used.
\code
  #include <relacs/basefuncs.h>

  SampleDataD y( 0.0, 1.0, 0.01 );
  y.identity();
  SampleDataD x = sin( y, f ) - 0.3*cos( 2.0*pi*f*y ) + exp( -0.5*y );
\endcode
The following functions are available:
\a sin, \a cos, \a tan, \a asin, \a acos, \a atan, 
\a sinh, \a cosh, \a tanh, \a asinh, \a acosh, \a atanh,
\a exp, \a log, \a log10, \a erf, \a erfc, 
\a sqrt, \a cbrt, \a hypot, \a square, \a cube, \a pow,
\a ceil, \a floor, \a abs
The following operators are available: \c +, \c -, \c *, \c /, \c % .
*/

template < typename T > class Map;

template < typename T = double > 
class SampleData : public Array< T >
{

  public:

    /*! Creates an empty SampleData. */
  SampleData( void );
    /*! Creates a SampleData with \a n data elements and 
        initialzes them with \a val. */
  SampleData( int n, const T &val=0 );
    /*! Creates a SampleData with \a n data elements that were sampled with
        \a stepsize starting at \a offset and 
        initialzes them with \a val. */
  SampleData( long n, const T &val=0 );
    /*! Creates a SampleData with \a n data elements that were sampled with
        \a stepsize starting at \a offset and 
        initialzes them with \a val. */
  SampleData( int n, double offset, double stepsize, const T &val=0 );
    /*! Creates a SampleData with \a n data elements that were sampled with
        \a stepsize starting at \a offset and 
        initialzes them with \a val. */
  SampleData( long n, double offset, double stepsize, const T &val=0 );
    /*! Creates a SampleData of data elements that were sampled with
        \a stepsize starting at \a l and ending at \a r, and 
        initialzes them with \a val. */
  SampleData( double l,  double r, double stepsize, const T &val=0 );
    /*! Creates a SampleData with the range \a range and 
        each data element initialized with \a val. */
  SampleData( const LinearRange &range, const T &val=0 );
    /*! Creates a SampleData with \a n data elements that were sampled with
        \a stepsize starting at \a offset and
        initialzes them with the values given in \a a. */
  template < typename R >
  SampleData( const R *a, int n, double offset=0, double stepsize=1 );
    /*! Creates a SampleData with \a n data elements that were sampled 
        according to \a range and
        initialzes them with the values given in \a a. */
  template < typename R >
  SampleData( const R *a, int n, const LinearRange &range );
    /*! Creates a SampleData with the same size and content
        as the vector \a a that was sampled with
        \a stepsize starting at \a offset. */
  template < typename R >
  SampleData( const vector< R > &a, double offset=0, double stepsize=1 );
    /*! Creates a SampleData with the same size and content
        as the vector \a a that was sampled according to \a range. */
  template < typename R >
  SampleData( const vector< R > &a, const LinearRange &range );
    /*! Creates a SampleData with the same size and content
        as the array \a a that was sampled with
        \a stepsize starting at \a offset. */
  template < typename R >
  SampleData( const Array< R > &a, double offset=0, double stepsize=1 );
    /*! Creates a SampleData with the same size and content
        as the array \a a that was sampled according to \a range. */
  template < typename R >
  SampleData( const Array< R > &a, const LinearRange &range );
    /*! Creates a SampleData with the same size, range, and content
        as \a sa. */
  template < typename R >
  SampleData( const SampleData< R > &sa );
    /*! Creates a SampleData from \a sa resampled according to \a range. */
  template < typename R >
  SampleData( const SampleData< R > &sa, const LinearRange &range );
    /*! Copy constructor.
        Creates a SampleData with the same size, range, and content
        as \a sa. */
  SampleData( const SampleData< T > &sa );
    /*! The destructor. */
  virtual ~SampleData( void );

    /*! Set the size(), capacity() and content of the array to \a a
        without affecting the offset() and stepsize().
        \sa assign() */
  template < typename R >
  const SampleData< T > &operator=( const R &a );
    /*! Set the size(), capacity(), range() and content of the array to \a a.
        \sa assign() */
  template < typename R >
  const SampleData< T > &operator=( const SampleData< R > &a );
    /*! Set the size(), capacity(), range() and content of the array to \a a.
        \sa assign() */
  const SampleData< T > &operator=( const SampleData< T > &a );

    /*! Set the size() and capacity() of the array to \a n and
        its content to \a a without affecting the offset() and stepsize(). */
  template < typename R >
  const SampleData< T > &assign( const R *a, int n );
    /*! Set the size() and capacity() of the array to \a n and
        its content to \a a. The range is set to start at \a offset
	with a stepsize \a stepsize. */
  template < typename R >
  const SampleData< T > &assign( const R *a, int n, double offset, double stepsize=1 );
    /*! Set the size(), capacity(), and content of the array to \a a
        without affecting the offset() and stepsize(). */
  template < typename R >
  const SampleData< T > &assign( const R &a );
    /*! Set the size(), capacity(), and content of the array to \a a.
        The range is set to start at \a offset
	with a stepsize \a stepsize. */
  template < typename R >
  const SampleData< T > &assign( const R &a, double offset, double stepsize=1 );
    /*! Set the size(), capacity(), range(), and content of the array to \a sa. */
  template < typename R >
  const SampleData< T > &assign( const SampleData< R > &sa );
    /*! Set the size(), capacity(), range(), and content of the array to \a sa. */
  const SampleData< T > &assign( const SampleData< T > &sa );

    /*! Assign the array \a sa resampled with stepsize \a stepsize 
        and linearly interpolated. */
  template < typename R >
  const SampleData< T > &interpolate( const SampleData< R > &sa, double stepsize )
        { return assign( sa, sa.offset(), stepsize ); };
    /*! Assign the array \a sa resampled with \a stepsize 
        and linearly interpolated starting from \a offset. */
  template < typename R >
  const SampleData< T > &interpolate( const SampleData< R > &sa, 
				      double offset, double stepsize );
    /*! Assign the array \a sa resampled over the \a range 
        and linearly interpolated. */
  template < typename R >
  const SampleData< T > &interpolate( const SampleData< R > &sa, 
				      const LinearRange &range );

    /*! Resize the array to \a n data elements sampled 
        with stepsize \a step and initialize the data elements
        with Gaussian distributed white noise
        with zero mean and unit standard deviation,
	lower and upper cutoff frequency \a cl and \a cu.
        Use random number generator \a r.
        \sa ouNoise(), Array::zeros(), Array::ones(), Array::rand(), Array::randNorm() */
  template < typename R >
  SampleData< T > &whiteNoise( int n, double step,
			       double cl, double cu, R &r=rnd );
    /*! Resize the array to hold \a l / \a step data elements sampled 
        with stepsize \a step and initialize the data elements
        with Gaussian distributed white noise
        with zero mean and unit standard deviation,
	lower and upper cutoff frequency \a cl and \a cu.
        Use random number generator \a r.
        \sa ouNoise(), Array::zeros(), Array::ones(), Array::rand(), Array::randNorm() */
  template < typename R >
  SampleData< T > &whiteNoise( double l, double step,
			       double cl, double cu, R &r=rnd );
    /*! Resize the array to \a n data elements sampled 
        with stepsize \a step and initialize the data elements
        with Ornstein-Uhlenbeck noise
        with zero mean and unit standard deviation
	and time constant \a tau.
        Use random number generator \a r.
        Algorithmus from L. Bartosch (2001), International Journal of Modern Physics C, 12, 851-855.
        \sa whiteNoise(), Array::zeros(), Array::ones(), Array::rand(), Array::randNorm() */
  template < typename R >
  SampleData< T > &ouNoise( int n, double step, double tau, R &r=rnd );
    /*! Resize the array to hold \a l / \a step data elements sampled 
        with stepsize \a step and initialize the data elements
        with Ornstein-Uhlenbeck noise
        with zero mean and unit standard deviation
	and time constant \a tau.
        Use random number generator \a r.
        Algorithmus from L. Bartosch (2001), International Journal of Modern Physics C, 12, 851-855.
        \sa whiteNoise(), Array::zeros(), Array::ones(), Array::rand(), Array::randNorm() */
  template < typename R >
  SampleData< T > &ouNoise( double l, double step, double tau, R &r=rnd );

    /*! Copy the content of the array to \a a. 
        If necessary remaining elements of \a a
        are set to \a val. */
  template < typename R >
  const SampleData< T > &copy( R *a, int n, const T &val=0 ) const;
    /*! Copy the content of the array to \a a. */
  template < typename R >
  const SampleData< T > &copy( R &a ) const;
    /*! Copy the content and range of the array to \a sa. 
        The size() and capacity() of \a are set
        to the size() of the array. */
  template < typename R >
  const SampleData< T > &copy( SampleData< R > &sa ) const;
    /*! Copy the content of the array from position \a x1 to \a x2 to \a sa. */
  template < typename R >
  const SampleData< T > &copy( double x1, double  x2, SampleData< R > &sa ) const;
    /*! Copy the content of the array from position \a x1 to \a x2 to \a m. */
  template < typename R >
  const SampleData< T > &copy( double x1, double  x2, Map< R > &m ) const;
    /*! Copy the content of the array from position \a x1 to \a x2 to \a a. */
  template < typename R >
  const SampleData< T > &copy( double x1, double  x2, Array< R > &a ) const;
    /*! Copy the content of the array from position \a x1 to \a x2 to \a v. */
  template < typename R >
  const SampleData< T > &copy( double x1, double  x2, vector< R > &v ) const;

    /*! Append \a \a n - times to the array. */
  const SampleData< T > &append( T a, int n=1 );
    /*! Append \a a of size \a n to the array. */
  template < typename R >
  const SampleData< T > &append( const R *a, int n );
    /*! Append \a a to the array. */
  template < typename R >
  const SampleData< T > &append( const R &a );
    /*! Append \a sa to the array.
        If the stepsize of \a sa differs,
        then \a sa is resampled with linear interpolation. */
  template < typename R >
  const SampleData< T > &append( const SampleData< R > &sa );

    /*! The size of the array, 
        i.e. the number of data elements. */
  int size( void ) const { return Array<T>::size(); };
    /*! True if the array does not contain any data elements. */
  bool empty( void ) const { return Array<T>::empty(); };
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The offset() and the stepsize() are preserved.
        \return the new size() (might be smaller than \a n). */
  virtual int resize( int n, const T &val=0 );
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The offset() and the stepsize() are preserved.
        \return the new size() (might be smaller than \a n). */
  int resize( long n, const T &val=0 );
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The stepsize is set to \a stepsize while the offset is preserved.
        \return the new size() (might be smaller than \a n). */
  int resize( int n, double stepsize, const T &val );
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The stepsize is set to \a stepsize while the offset is preserved.
        \return the new size() (might be smaller than \a n). */
  int resize( long n, double stepsize, const T &val );
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The stepsize is set to \a stepsize and the offset to \a offset.
        \return the new size() (might be smaller than \a n). */
  int resize( int n, double offset, double stepsize, const T &val );
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The stepsize is set to \a stepsize and the offset to \a offset.
        \return the new size() (might be smaller than \a n). */
  int resize( long n, double offset, double stepsize, const T &val );
    /*! Resize the array to the length() \a r.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The stepsize is set to \a stepsize and the offset set to 0.
        \return the new size() (might be smaller than \a n). */
  int resize( double r, double stepsize, const T &val );
    /*! Resize the array to the length() \a r - \a l.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The stepsize is set to \a stepsize and the offset to \a l.
        \return the new size() (might be smaller than \a n). */
  int resize( double l,  double r, double stepsize, const T &val );
    /*! Resize the array to the size of \a range.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve().
        The stepsize and the offset are set to 
	the stepsize and the offset of \a range, respectively.
        \return the new size() (might be smaller than \a n). */
  int resize( const LinearRange &range, const T &val=0 );

    /*! Resize the array to zero length.
        The capacity() remains unchanged. */
  virtual void clear( void );

    /*! The capacity of the array, i.e. the number of data
        elements for which memory has been allocated. 
        capacity() is always greater than or equal to size(). */
  int capacity( void ) const { return Array<T>::capacity(); };
    /*! If \a n is less than or equal to capacity(), 
        this call has no effect. 
	Otherwise, it is a request for allocation 
	of additional memory. 
	If the request is successful, 
	then capacity() is greater than or equal to \a n; 
	otherwise, capacity() is unchanged. 
	In either case, size() is unchanged and the content
	of the array is preserved.
        \return the new capacity. */
  virtual int reserve( int n );
    /*! In contrast to the reserve() function, this function
        frees or allocates memory, such that capacity()
	equals exactly \a n.
	If the size() of the array is greater than \a n
	it is set to \a n as well. */
  virtual void free( int n=0 );

     /*! The offset of the range. */
  double offset( void ) const { return Samples.offset(); };
     /*! Set the offset of the range to \a offset. */
  void setOffset( double offset ) { Samples.setOffset( offset ); };
     /*! The stepsize of the range. */
  double stepsize( void ) const { return Samples.stepsize(); };
     /*! Set the stepsize of the range to \a stepsize.
         This also changes length() and rangeBack(). */
  void setStepsize( double stepsize ) { Samples.setStepsize( stepsize ); };
     /*! Multiply the stepsize of the range by \a scale
         and adjust the size of the range appropriately.
         This does not change offset(), length(), and rangeBack(). */
  void scaleStepsize( double scale ) { Samples.scaleStepsize( scale ); };
     /*! Set the offset and the stepsize of the range to \a offset and \a stepsize, respectively. */
  void setRange( double offset, double stepsize ) { Samples.setRange( offset, stepsize ); };
     /*! The length of the range, i.e. abs( stepsize() * size() ) */
  double length( void ) const { Samples.resize( Array<T>::size() ); return Samples.length(); };
     /*! Set the size of the range such that it has the length \a l.
         The array is resized accordingly. \sa resize() */
  void setLength( double l );
    /*! Returns the first range element, i.e. the offset. \sa offset() */
  double rangeFront( void ) const { return Samples.front(); };
    /*! Returns the last range element. */
  double rangeBack( void ) const { Samples.resize( Array<T>::size() ); return Samples.back(); };
    /*! Resize the range such that it last value equals \a val. */
  void setRangeBack( double val ) { resize( index( val ) ); };

     /*! Add \a val to the offset of the range,
         i.e. shift the range by \a val. */
  void shift( double val ) { Samples += val; };
    /*! Multiply the offset and the stepsize of the range with \a scale, 
        i.e. rescale the range by \a scale. */
  void scale( double scale ) { Samples *= scale; };

    /*! Returns the range element at index \a i. */
  double pos( int i ) const { return Samples.pos( i ); };
    /*! Returns the interval covered by \a indices indices. */
  double interval( int indices ) const { return Samples.interval( indices ); };

    /*! The index of the range corresponding to \a pos. */
  int index( double pos ) const { return Samples.index( pos ); };
    /*! The number of indices corresponding to an interval \a iv. */
  int indices( double iv ) const { return Samples.indices( iv ); };
    /*! True if \a pos is within the range. */
  bool contains( double pos ) const { Samples.resize( Array<T>::size() ); return Samples.contains( pos ); };

    /*! Returns a reference to the data element at index \a i.
        No range checking is performed. */
  const T &operator[]( int i ) const { return Array<T>::operator[]( i ); };
    /*! Returns a reference to the data element at index \a i.
        No range checking is performed. */
  T &operator[]( int i ) { return Array<T>::operator[]( i ); };
    /*! Returns a reference to the data element at index \a i.
        If \a i is an invalid index
	a reference to a variable set to zero is returned. */
  const T &at( int i ) const { return Array<T>::at( i ); };
    /*! Returns a reference to the data element at index \a i.
        If \a i is an invalid index
	a reference to a variable set to zero is returned. */
  T &at( int i ) { return Array<T>::at( i ); };

    /*! Returns a reference to the data element left to position \a x.
        No range checking is performed. */
  const T &operator[]( double x ) const { return Array<T>::operator[]( index( x ) ); };
    /*! Returns a reference to the data element left to position \a x.
        No range checking is performed. */
  T &operator[]( double x ) { return Array<T>::operator[]( index( x ) ); };
    /*! Returns an linearly interpolated value
        of the data array at position \a x. */
  T operator()( double x ) const { return interpolate( x ); };

    /*! Returns a reference to the first data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  const T &front( void ) const { return Array<T>::front(); };
    /*! Returns a reference to the first data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  T &front( void ) { return Array<T>::front(); };
    /*! Returns a reference to the last data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  const T &back( void ) const { return Array<T>::back(); };
    /*! Returns a reference to the last data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  T &back( void ) { return Array<T>::back(); };

    /*! Add \a val as a new element to the array.
        \return the number of added elements (0 or 1). */
  inline int push( const T &val );
    /*! Remove the last element of the array
        and return its value. */
  T pop( void );

    /*! Returns a pointer to the data buffer. */
  const T *data( void ) const { return Array<T>::data(); };
    /*! Returns a pointer to the data buffer. */
  T *data( void ) { return Array<T>::data(); };

    /*! Returns a reference to the data array. */
  const Array<T> &array( void ) const { return *this; };
    /*! Returns a reference to the data array. */
  Array<T> &array( void ) { return *this; };

    /*! Returns a reference to the range. */
  const LinearRange &range( void ) const { return Samples; };
    /*! Returns a reference to the range. */
  LinearRange &range( void ) { return Samples; };

    /*! The type of object, T, stored in the arry. */
  typedef T value_type;
    /*! Pointer to the type of object, T, stored in the array. */
  typedef T* pointer;
    /*! Reference to the type of object, T, stored in the array. */
  typedef T& reference;
    /*! Const reference to the type of object, T, stored in the array. */
  typedef const T& const_reference;
    /*! The type used for sizes and indices. */
  typedef int size_type;
    /*! Iterator used to iterate through the data array. */
  typedef T* iterator;
    /*! Const iterator used to iterate through the data array. */
  typedef const T* const_iterator;
    /*! The type of an element of the range. */
  typedef double range_type;
    /*! Const iterator used to iterate through the range. */
  typedef LinearRange::const_iterator const_range_iterator;

    /*! Returns an iterator pointing to the first element of the array. */
  iterator begin( void ) { return Array<T>::begin(); };
    /*! Returns an const_iterator pointing to the first element of the array. */
  const_iterator begin( void ) const { return Array<T>::begin(); };
    /*! Returns an iterator pointing behind the last element of the array. */
  iterator end( void ) { return Array<T>::end(); };
    /*! Returns an const_iterator pointing behind the last element of the array. */
  const_iterator end( void ) const { return Array<T>::end(); };

    /*! Returns an const_iterator pointing to the first element of the array. */
  const_range_iterator rangeBegin( void ) const { Samples.resize( Array<T>::size() ); return Samples.begin(); };
    /*! Returns an const_iterator pointing behind the last element of the array. */
  const_range_iterator rangeEnd( void ) const { Samples.resize( Array<T>::size() ); return Samples.end(); };

    /*! Insert the data element \a yval at position \a i. */
  SampleData< T > &insert( int i, const T &yval );
    /*! Insert the data element \a yval at position \a i. */
  iterator insert( iterator i, const T &yval );
    /*! Remove data element at position \a i from the Array. */
  SampleData< T > &erase( int i );
    /*! Remove data element at position \a i from the Array. */
  iterator erase( iterator i );

    /*! True if size, content, and range of \a a and \a b are equal. */
  template < typename TT > friend bool operator==( const SampleData<TT> &a, const SampleData<TT> &b );
    /*! True if the value of each data element of array \a a 
        is smaller than \a b and if the range of \a 
	is smaller than the range of \a b. */
  template < typename TT > friend bool operator<( const SampleData<TT> &a, const SampleData<TT> &b );

  /* Generates declarations for unary operators of class SampleData
     that take scalars as argument.
     \a COP is the operator name (like operator+= ). */
#define SAMPLEDARRAYOPS1SCALARDEC( COP )	\
  const SampleData< T > &COP( float x );	\
  const SampleData< T > &COP( double x );	\
  const SampleData< T > &COP( long double x );	\
  const SampleData< T > &COP( signed char x );	\
  const SampleData< T > &COP( unsigned char x );\
  const SampleData< T > &COP( signed int x );	\
  const SampleData< T > &COP( unsigned int x );	\
  const SampleData< T > &COP( signed long x );	\
  const SampleData< T > &COP( unsigned long x );

    /*! Set the value of each data element to \a val. */
  SAMPLEDARRAYOPS1SCALARDEC( operator= );
    /*! Add each value of the container \a x
        to the corresponding data element. */
  template < class R >
  const SampleData< T > &operator+=( const R &x );
    /*! Add each value of the container \a x
        to the corresponding data element.
        Also copies the range (offset(), size(), and stepsize())
	of \a x to \a this. */
  template < class R >
  const SampleData< T > &operator+=( const SampleData<R> &x );
    /*! Add \a x to each of the data elements. 
        \a x is a scalar type like \c float, \c double, \c int, etc. */
  SAMPLEDARRAYOPS1SCALARDEC( operator+= );
    /*! Subtract each value of the container \a x 
        from the corresponding data element. */
  template < class R >
  const SampleData< T > &operator-=( const R &x );
    /*! Subtract each value of the container \a x
        from the corresponding data element.
        Also copies the range (offset(), size(), and stepsize())
	of \a x to \a this. */
  template < class R >
  const SampleData< T > &operator-=( const SampleData<R> &x );
    /*! Subtract \a x from each of the data elements. 
        \a x is a scalar type like \c float, \c double, \c int, etc. */
  SAMPLEDARRAYOPS1SCALARDEC( operator-= );
    /*! Multiply each value of the container \a x 
        with the corresponding data element. */
  template < class R >
  const SampleData< T > &operator*=( const R &x );
    /*! Multiply each value of the container \a x
        with the corresponding data element.
        Also copies the range (offset(), size(), and stepsize())
	of \a x to \a this. */
  template < class R >
  const SampleData< T > &operator*=( const SampleData<R> &x );
    /*! Multiply \a x with each of the data elements. 
        \a x is a scalar type like \c float, \c double, \c int, etc. */
  SAMPLEDARRAYOPS1SCALARDEC( operator*= );
    /*! Divide each data element by the 
        corresponding element of the container \a x. */
  template < class R >
  const SampleData< T > &operator/=( const R &x );
    /*! Divide each data element by the 
        corresponding element of the container \a x.
        Also copies the range (offset(), size(), and stepsize())
	of \a x to \a this. */
  template < class R >
  const SampleData< T > &operator/=( const SampleData<R> &x );
    /*! Divide each data element by \a x. 
        \a x is a scalar type like \c float, \c double, \c int, etc. */
  SAMPLEDARRAYOPS1SCALARDEC( operator/= );
    /*! Return the remainder of the division of each data element by the 
        corresponding element of the container \a x. */
  template < class R >
  const SampleData< T > &operator%=( const R &x );
    /*! Return the remainder of the division of each data element by the 
        corresponding element of the container \a x.
        Also copies the range (offset(), size(), and stepsize())
	of \a x to \a this. */
  template < class R >
  const SampleData< T > &operator%=( const SampleData<R> &x );
    /*! Return the remainder of each data element divided by \a x. 
        \a x is a scalar type like \c float, \c double, \c int, etc. */
  SAMPLEDARRAYOPS1SCALARDEC( operator%= );

#undef SAMPLEDARRAYOPS1SCALARDEC

/* Generates declarations for binary class friend operators
   that take the class and a scalar as argument.
   \a COP is the operator name (like operator+ ). */
#define SAMPLEDARRAYOPS2SCALARDEC( COP )	\
  template < typename TT > friend SampleData<TT> COP( float x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, float y ); \
  template < typename TT > friend SampleData<TT> COP( double x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, double y ); \
  template < typename TT > friend SampleData<TT> COP( long double x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, long double y ); \
  template < typename TT > friend SampleData<TT> COP( signed char x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, signed char y ); \
  template < typename TT > friend SampleData<TT> COP( unsigned char x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, unsigned char y ); \
  template < typename TT > friend SampleData<TT> COP( signed int x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, signed int y ); \
  template < typename TT > friend SampleData<TT> COP( unsigned int x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, unsigned int y ); \
  template < typename TT > friend SampleData<TT> COP( signed long x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, signed long y ); \
  template < typename TT > friend SampleData<TT> COP( unsigned long x, const SampleData<TT> &y ); \
  template < typename TT > friend SampleData<TT> COP( const SampleData<TT> &x, unsigned long y );

    /*! Return the sum of the containers \a x and \a y computed for each element. */
  template < typename TT, typename RR >
  friend SampleData<TT> operator+( const SampleData<TT> &x,  const RR &y );
    /*! Return the sum of \a x and \a y computed for each element. 
        One of the parameters is a scalar type
	like \c float, \c double, \c int, etc.,
        the other parameter is a SampleData. */
  SAMPLEDARRAYOPS2SCALARDEC( operator+ );
    /*! Return the difference of the containers \a x and \a y computed for each element. */
  template < typename TT, typename RR >
  friend SampleData<TT> operator-( const SampleData<TT> &x,  const RR &y );
    /*! Return the difference of \a x and \a y computed for each element. 
        One of the parameters is a scalar type
	like \c float, \c double, \c int, etc.,
        the other parameter is a SampleData. */
  SAMPLEDARRAYOPS2SCALARDEC( operator- );
    /*! Return the product of the containers \a x and \a y computed for each element. */
  template < typename TT, typename RR >
  friend SampleData<TT> operator*( const SampleData<TT> &x,  const RR &y );
    /*! Return the product of \a x and \a y computed for each element. 
        One of the parameters is a scalar type
	like \c float, \c double, \c int, etc.,
        the other parameter is a SampleData. */
  SAMPLEDARRAYOPS2SCALARDEC( operator* );
    /*! Return container \a x divided by container \a y computed for each element. */
  template < typename TT, typename RR >
  friend SampleData<TT> operator/( const SampleData<TT> &x,  const RR &y );
    /*! Return \a x divided by \a y computed for each element. 
        One of the parameters is a scalar type
	like \c float, \c double, \c int, etc.,
        the other parameter is a SampleData. */
  SAMPLEDARRAYOPS2SCALARDEC( operator/ );
    /*! Return the remainder of container \a x divided by container \a y computed for each element. */
  template < typename TT, typename RR >
  friend SampleData<TT> operator%( const SampleData<TT> &x,  const RR &y );
    /*! Return the remainder of \a x divided by \a y computed for each element. 
        One of the parameters is a scalar type
	like \c float, \c double, \c int, etc.,
        the other parameter is a SampleData. */
  SAMPLEDARRAYOPS2SCALARDEC( operator% );

#undef SAMPLEDARRAYOPS2SCALARDEC

    /*! Negates each element of the data array. */
  SampleData< T > operator-( void );

    /*! Set the value of each data element to the value of the corresponding
        element of the range. */
  SampleData< T > &identity( void );

    /*! Returns sin(2*pi*f*x)
        computed for each element \a x of the range \a r. */
  friend SampleData<> sin( const LinearRange &r, double f );
  friend SampleData<> sin( int n, double offset, double stepsize, double f );
  friend SampleData<> sin( double l, double r, double stepsize, double f );
    /*! Initializes the range  with \a r 
        and the array with sin(2*pi*f*x)
        computed for each element \a x of the range \a r. */
  SampleData< T > &sin( const LinearRange &r, double f );
  SampleData< T > &sin( int n, double offset, double stepsize, double f )
    { return sin( LinearRange( n, offset, stepsize ), f ); };
  SampleData< T > &sin( double l,  double r, double stepsize, double f )
    { return sin( LinearRange( l, r, stepsize ), f ); };

    /*! Returns cos(2*pi*f*x)
        computed for each element \a x of the range \a r. */
  friend SampleData<> cos( const LinearRange &r, double f );
  friend SampleData<> cos( int n, double offset, double stepsize, double f );
  friend SampleData<> cos( double l, double r, double stepsize, double f );
    /*! Initializes the range  with \a r 
        and the array with cos(2*pi*f*x)
        computed for each element \a x of the range \a r. */
  SampleData< T > &cos( const LinearRange &r, double f );
  SampleData< T > &cos( int n, double offset, double stepsize, double f )
    { return cos( LinearRange( n, offset, stepsize ), f ); };
  SampleData< T > &cos( double l,  double r, double stepsize, double f )
    { return cos( LinearRange( l, r, stepsize ), f ); };

    /*! Returns the standard normal distribution exp( -0.5*x^2 )/sqrt(2*pi) 
        for each element \a x of the range \a r. */
  friend SampleData<> gauss( const LinearRange &r );
  friend SampleData<> gauss( int n, double offset, double stepsize );
  friend SampleData<> gauss( double l, double r, double stepsize );
    /*! Initializes the range  with \a r 
        and the array with the standard normal distribution exp( -0.5*x^2 )/sqrt(2*pi) 
        computed for each element \a x of the range \a r. */
  SampleData< T > &gauss( const LinearRange &r );
  SampleData< T > &gauss( int n, double offset, double stepsize )
    { return gauss( LinearRange( n, offset, stepsize ) ); };
  SampleData< T > &gauss( double l,  double r, double stepsize )
    { return gauss( LinearRange( l, r, stepsize ) ); };

    /*! Returns the normal distribution
        exp( -0.5*(x-m)^2/s^2 )/sqrt(2*pi)/s 
	with standard deviation \a s and mean \a m
	for each element \a x of the range \a r. */
  friend SampleData<> gauss( const LinearRange &r, double s, double m );
  friend SampleData<> gauss( int n, double offset, double stepsize, double s, double m );
  friend SampleData<> gauss( double l, double r, double stepsize, double s, double m );
    /*! Initializes the range  with \a r 
        and the array with the normal distribution
        exp( -0.5*(x-m)^2/s^2 )/sqrt(2*pi)/s 
	with standard deviation \a s and mean \a m
        computed for each element \a x of the range \a r. */
  SampleData< T > &gauss( const LinearRange &r, double s, double m=0.0 );
  SampleData< T > &gauss( int n, double offset, double stepsize, double s, double m=0.0 )
    { return gauss( LinearRange( n, offset, stepsize ), s, m ); };
  SampleData< T > &gauss( double l,  double r, double stepsize, double s, double m=0.0 )
    { return gauss( LinearRange( l, r, stepsize ), s, m ); };

    /*! Returns the alpha function y*exp(-y) with
        y = (x-offs)/tau for each element \a x of the range \a r. */
  friend SampleData<> alpha( const LinearRange &r, double tau, double offs );
  friend SampleData<> alpha( int n, double offset, double stepsize, double tau, double offs );
  friend SampleData<> alpha( double l, double r, double stepsize, double tau, double offs );
    /*! Initializes the range  with \a r 
        and the array with the alpha function
	y*exp(-y) with y = (x-offs)/tau
        computed for each element \a x of the range \a r. */
  SampleData< T > &alpha( const LinearRange &r, double tau, double offs=0.0 );
  SampleData< T > &alpha( int n, double offset, double stepsize, double tau, double offs=0.0 )
    { return alpha( LinearRange( n, offset, stepsize ), tau, offs ); };
  SampleData< T > &alpha( double l,  double r, double stepsize, double tau, double offs=0.0 )
    { return alpha( LinearRange( l, r, stepsize ), tau, offs ); };

    /*! Returns a straight line with abscissa \a abscissa and 
        slope \a slope
        computed for each element \a x of the range \a r. */
  friend SampleData<> line( const LinearRange &r, double abscissa, double slope );
  friend SampleData<> line( int n, double offset, double stepsize, double abscissa, double slope );
  friend SampleData<> line( double l, double r, double stepsize, double abscissa, double slope );
    /*! Initializes the range  with \a r 
        and the array with a straight line
	with abscissa \a abscissa and slope \a slope
        computed for each element \a x of the range \a r. */
  SampleData< T > &line( const LinearRange &r, double abscissa, double slope );
  SampleData< T > &line( int n, double offset, double stepsize, double abscissa, double slope )
    { return line( LinearRange( n, offset, stepsize ), abscissa, slope ); };
  SampleData< T > &line( double l,  double r, double stepsize, double abscissa, double slope )
    { return line( LinearRange( l, r, stepsize ), abscissa, slope ); };

    /*! Returns a rectangular pulse pattern with period \a period, 
        duration of the rectangle \a width, and maximum value 1.0
        computed for each element \a x of the range \a r.
	The up- and downstrokes have a width of \a ramp. */
  friend SampleData<> rectangle( const LinearRange &r,
				 double period, double width, double ramp );
  friend SampleData<> rectangle( int n, double offset, double stepsize,
				 double period, double width, double ramp );
  friend SampleData<> rectangle( double l, double r, double stepsize,
				 double period, double width, double ramp );
    /*! Initializes the range  with \a r 
        and the array with a rectangular pulse pattern with period \a period, 
        duration of the rectangle \a width, and maximum value 1.0
        computed for each element \a x of the range \a r.
	The up- and downstrokes have a width of \a ramp. */
  SampleData< T > &rectangle( const LinearRange &r,
			      double period, double width, double ramp=0.0 );
  SampleData< T > &rectangle( int n, double offset, double stepsize,
			      double period, double width, double ramp=0.0 )
    { return rectangle( LinearRange( n, offset, stepsize ), period, width, ramp ); };
  SampleData< T > &rectangle( double l,  double r, double stepsize,
			      double period, double width, double ramp=0.0 )
    { return rectangle( LinearRange( l, r, stepsize ), period, width, ramp ); };

    /*! Returns a sawtooth with period \a period and maximum value 1.0
        computed for each element \a x of the range \a r.
	The downstroke has a width of \a ramp. */
  friend SampleData<> sawUp( const LinearRange &r, double period, double ramp );
  friend SampleData<> sawUp( int n, double offset, double stepsize, double period, double ramp );
  friend SampleData<> sawUp( double l, double r, double stepsize, double period, double ramp );
    /*! Initializes the range  with \a r 
        and the array with a sawtooth with period \a period and maximum value 1.0
        computed for each element \a x of the range \a r.
	The downstroke has a width of \a ramp. */
  SampleData< T > &sawUp( const LinearRange &r, double period, double ramp=0.0 );
  SampleData< T > &sawUp( int n, double offset, double stepsize, double period, double ramp=0.0 )
    { return sawUp( LinearRange( n, offset, stepsize ), period, ramp ); };
  SampleData< T > &sawUp( double l,  double r, double stepsize, double period, double ramp=0.0 )
    { return sawUp( LinearRange( l, r, stepsize ), period, ramp ); };

    /*! Returns a sawtooth with period \a period and maximum value 1.0
        computed for each element \a x of the range \a r.
	The upstroke has a width of \a ramp. */
  friend SampleData<> sawDown( const LinearRange &r, double period, double ramp );
  friend SampleData<> sawDown( int n, double offset, double stepsize, double period, double ramp );
  friend SampleData<> sawDown( double l, double r, double stepsize, double period, double ramp );
    /*! Initializes the range  with \a r 
        and the array with a sawtooth with period \a period and maximum value 1.0
        computed for each element \a x of the range \a r.
	The upstroke has a width of \a ramp. */
  SampleData< T > &sawDown( const LinearRange &r, double period, double ramp=0.0 );
  SampleData< T > &sawDown( int n, double offset, double stepsize, double period, double ramp=0.0 )
    { return sawDown( LinearRange( n, offset, stepsize ), period, ramp ); };
  SampleData< T > &sawDown( double l,  double r, double stepsize, double period, double ramp=0.0 )
    { return sawDown( LinearRange( l, r, stepsize ), period, ramp ); };

    /*! Returns a triangular sawtooth with period \a period and maximum value 1.0
        computed for each element \a x of the range \a r. */
  friend SampleData<> triangle( const LinearRange &r, double period );
  friend SampleData<> triangle( int n, double offset, double stepsize, double period );
  friend SampleData<> triangle( double l, double r, double stepsize, double period );
    /*! Initializes the range  with \a r 
        and the array with a triangular sawtooth with period \a period and maximum value 1.0
        computed for each element \a x of the range \a r. */
  SampleData< T > &triangle( const LinearRange &r, double period );
  SampleData< T > &triangle( int n, double offset, double stepsize, double period )
    { return triangle( LinearRange( n, offset, stepsize ), period ); };
  SampleData< T > &triangle( double l,  double r, double stepsize, double period )
    { return triangle( LinearRange( l, r, stepsize ), period ); };

    /*! Return a linearly interpolated value for position \a x.
        If \a x is outside the range the value of the first 
        or last data element is returned. */
  T interpolate( double x ) const;

    /*! Returns the integral of the SampleData, 
        i.e. the sum of the data elements times stepsize(). */
  T integral( void ) const;

    /*! Multiply the first \a indices( x ) y-data elements with a ramp
        linearly increasing from zero to one. */
  SampleData< T > &rampUp( double x );
    /*! Multiply the last \a indices( x ) y-data elements with a ramp
        linearly decreasing from one to zero. */
  SampleData< T > &rampDown( double x );
    /*! Multiply the first and the last \a indices( x ) y-data elements 
        with a ramp.
        \sa rampUp(), rampDown() */
  SampleData< T > &ramp( double x );

    /*! Add the values of the range \a first, \a last
        to the histogram. */
  template < typename ForwardIter >
  SampleData< T > &addHist( ForwardIter first, ForwardIter last );
    /*! Add the values of the container \a x to the histogram. */
  template < typename R >
  SampleData< T > &addHist( const R &x );
    /*! Compute histogram for the values of the range \a first, \a last. */
  template < typename ForwardIter >
  SampleData< T > &hist( ForwardIter first, ForwardIter last );
    /*! Compute histogram for the container \a x. */
  template < typename R >
  SampleData< T > &hist( const R &x );

     /*! Return the convolution of \a x with the container \a y.
         \a y can be shifted by \a offs indices.
         If possible, y.size() should be smaller than x.size(). */
  template < typename TT, typename RR >
  friend SampleData<TT> convolve( const SampleData<TT> &x, const RR &y, int offs=0 );
  template < typename TT >
  friend SampleData<TT> convolve( const SampleData<TT> &x, const SampleData<TT> &y );

    /*! Filters the array with the filter \a g.
        First, the array is fourier transformed.
        Then the amplitudes of the fourier components are multiplied with \a g.
        Finally, the array is transformed back into the time domain.
	If \a rescale is \c true, the amplitude of the resulting signal is 
	rescaled such that its rms value is the same as the original one. */
  template < typename TT >
  SampleData< T > &freqFilter( const SampleData<TT> &g, bool rescale=true );

    /*! The minimum value of the data elements between position
        \a first (inclusively) and \a last (exclusively). */
  T min( double first, double last ) const;
    /*! The index of the element with the minimum value of the data elements between position
        \a first (inclusively) and \a last (exclusively). */
  int minIndex( double first, double last ) const;
    /*! The index of the element with the minimum value 
        of the data elements between position
        \a first (inclusively) and \a last (exclusively).
        The value of the minimum element is returned in \a min. */
  int minIndex( T &min, double first, double last ) const;

    /*! The maximum value of the data elements between position
        \a first (inclusively) and \a last (exclusively). */
  T max( double first, double last ) const;
    /*! The index of the element with the maximum value of the data elements between position
        \a first (inclusively) and \a last (exclusively). */
  int maxIndex( double first, double last ) const;
    /*! The index of the element with the maximum value 
        of the data elements between position
	\a first (inclusively) and \a last (exclusively).
	The value of the maximum element is returned in \a max. */
  int maxIndex( T &max, double first, double last ) const;

    /*! The minimum value \a min and maximum value \a max
        of the data elements between position
        \a first (inclusively) and \a last (exclusively). */
  void minMax( T &min, T &max, double first, double last ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum and the maximum value of the data elements between position
        \a first (inclusively) and \a last (exclusively). */
  void minMaxIndex( int &minindex, int &maxindex, double first, double last ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum value \a min and the maximum value \a max
        of the data elements between position
	\a first (inclusively) and \a last (exclusively). */
  void minMaxIndex( T &min, int &minindex, T &max, int &maxindex,
		    double first, double last ) const;

    /*! The mean \<x\> of the data elements between position 
        \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::mean_type
  mean( double first, double last ) const;
    /*! The mean \<x\> of the data elements between position 
        \a first (inclusively) and \a last (exclusively).
        In \a stdev the standard deviation is returned. */
  typename numerical_traits< T >::mean_type
  mean( typename numerical_traits< T >::variance_type &stdev,
	double first, double last ) const;
    /*! The unbiased variance var(x)=\<(x-\<x\>)^2\> of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  variance( double first, double last ) const;
    /*! The unbiased variance var(x)=\<(x-mean)^2\> of the data elements between
        position \a first (inclusively) and \a last (exclusively) for known \a mean. */
  typename numerical_traits< T >::variance_type
  varianceKnown( typename numerical_traits< T >::mean_type mean,
		 double first, double last ) const;
    /*! The variance var(x)=\<(x-mean)^2\> of the data elements between
        position \a first (inclusively) and \a last (exclusively) for fixed \a mean. */
  typename numerical_traits< T >::variance_type
  varianceFixed( typename numerical_traits< T >::mean_type fixedmean,
		 double first, double last ) const;
    /*! The unbiased standard deviation sqrt(var(x)) 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  stdev( double first, double last ) const;
    /*! The unbiased standard deviation sqrt(var(x)) 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively) for known \a mean. */
  typename numerical_traits< T >::variance_type
  stdevKnown( typename numerical_traits< T >::mean_type mean,
	      double first, double last ) const;
    /*! The standard deviation sqrt(var(x)) 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively) for fixed \a mean. */
  typename numerical_traits< T >::variance_type
  stdevFixed( typename numerical_traits< T >::mean_type fixedmean,
	      double first, double last ) const;
    /*! The unbiased standard error mean sqrt(var(x)/N) 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  sem( double first, double last ) const;
    /*! The unbiased standard error mean sqrt(var(x)/N) 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively) for known \a mean. */
  typename numerical_traits< T >::variance_type
  semKnown( typename numerical_traits< T >::mean_type mean,
	    double first, double last ) const;
    /*! The standard deviation sqrt(var(x)/N) 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively) for fixed \a mean. */
  typename numerical_traits< T >::variance_type
  semFixed( typename numerical_traits< T >::mean_type fixedmean,
	    double first, double last ) const;
    /*! The absolute deviation <|x-mu|> 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  absdev( double first, double last ) const;
    /*! The absolute deviation <|x-mu|> 
        of the data elements between
        position \a first (inclusively) and \a last (exclusively) for known \a mean. */
  typename numerical_traits< T >::variance_type
  absdevKnown( typename numerical_traits< T >::mean_type mean,
	       double first, double last ) const;
    /*! The root-mean-square
        of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type rms( double first, double last ) const;
    /*! The skewness of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  skewness( double first, double last ) const;
    /*! The kurtosis of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  kurtosis( double first, double last ) const;
    /*! The sum of all elements of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  T sum( double first, double last ) const;
    /*! The sum of the square of all elements of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  squaredSum( double first, double last ) const;
    /*! The power \<x^2\> of all elements of the data elements between
        position \a first (inclusively) and \a last (exclusively). */
  typename numerical_traits< T >::variance_type
  power( double first, double last ) const;

    /*! Compute power \a p of the half-complex sequence in \a hc.
        Sets the stepsize() of  \a p to the one of \a hc.
	Half the number hc.size() of data elements in \a hc
	can be assigned a power in \a p, excess elements are set to zero.
	The spectrum is normalized such that its sum equals the
	mean squared amplitudes of the signal.
	\a TT and \a SS are real numbers.
        \sa hcMagnitude(), hcPhase(), transfer() */
  template < typename TT, typename SS >
  friend void hcPower( const SampleData<TT> &hc, SampleData<SS> &p );
    /*! Compute magnitude \a m of the half-complex sequence in \a hc.
        Sets the stepsize() of  \a m to the one of \a hc.
	Half the number hc.size() of data elements in \a hc
	can be assigned a magnitude in \a m, excess elements are set to zero.
	\a TT and \a SS are real numbers.
        \sa hcPower(), hcPhase(), transfer() */
  template < typename TT, typename SS >
  friend void hcMagnitude( const SampleData<TT> &hc, SampleData<SS> &m );
    /*! Compute phase \a p (argument, from -pi to pi) 
        of the half-complex sequence in \a hc.
        Sets the stepsize() of  \a p to the one of \a hc.
	Half the number hc.size() of data elements in \a hc
	can be assigned a phase in \a p, excess elements are set to zero.
	\a TT and \a SS are real numbers.
        \sa hcPower(), hcMagnitude(), transfer() */
  template < typename TT, typename SS >
  friend void hcPhase( const SampleData<TT> &hc, SampleData<SS> &p );
    /*! Compute real parts of the half-complex sequence in \a hc.
        Sets the stepsize() of  \a r to the one of \a hc.
	Half the number hc.size() of data elements in \a hc
	can be assigned a real part in \a p, excess elements are set to zero.
	\a TT and \a SS are real numbers.
        \sa hcPower(), hcMagnitude(), hcPhase(), hcImaginary(), rFFT(), transfer() */
  template < typename TT, typename SS >
  friend void hcReal( const SampleData<TT> &hc, SampleData<SS> &r );
    /*! Compute imaginary parts of the half-complex sequence in \a hc.
        Sets the stepsize() of  \a i to the one of \a hc.
	Half the number hc.size() of data elements in \a hc
	can be assigned a imaginary part in \a p, excess elements are set to zero.
	\a TT and \a SS are real numbers.
        \sa hcPower(), hcMagnitude(), hcPhase(), hcReal(), rFFT(), transfer() */
  template < typename TT, typename SS >
  friend void hcImaginary( const SampleData<TT> &hc, SampleData<SS> &i );

  /*! Compute an in-place radix-2 FFT on \a x containing real numbers.
      The size \a N of \a x has to be a power of two, otherwise -1 is returned.
      The output is a half-complex sequence, which is stored in-place. 
      The arrangement of the half-complex terms uses the following
      scheme: for k < N/2 the real part of the k-th term is stored in
      location k, and the corresponding imaginary part is stored in
      location N-k. Terms with k > N/2 (the negative frequencies)
      can be reconstructed using the symmetry z_k = z^*_{N-k}.
      The terms for k=0 and k=N/2 are both
      purely real, and count as a special case. Their real parts are
      stored in locations 0 and N/2 respectively, while their
      imaginary parts which are zero are not stored.
      The stepsize() is set to 1/(N stepsize()), such that
      the first half of the output range contains the positive frequencies
      at i*stepsize(), i=0..N/2.
      Use hcPower() and hcPhase() to compute power and phase of the spectrum.
      Algorithm adapted from the GNU Scientific Library http://www.gnu.org/software/gsl .
      \sa hcPower(), hcMagnitude(), hcPhase(), rPSD() */
  template < typename TT >
  friend int rFFT( SampleData<TT> &x );

    /*! Power spectrum \a p of \a x.
        The spectrum is normalized such that its sum equals the
        mean squared amplitudes of the signal \a x.
        A fourier window of size \a n ( a power of two no less than 2*p.size() ) is used.
	p.stepsize() is set to its appropriate value 0.5/x.stepsize()/n.
	\a TT and \a SS are real numbers. */
  template < typename TT, typename SS >
  friend int rPSD( const SampleData<TT> &x, SampleData<SS> &p,
		   bool overlap, double (*window)( int j, int n ) );
    /*! Compute transfer function \a h (half-complex sequence) 
        between \a x and \a y.
	\a x and \a y must have the same stepsize() and size().
	\a x and \a y are divided into chunks of \a N data points,
	where \a N = h.size(). h.size() must be a power of two.
	The stepsize() of \a h is set to 1.0/x.stepsize()/h.size().
	The gain and phase of the transfer function can be obtained
	using hcMagnitude() and hcPhase().
	\a TT, \a SS, and \a RR are real numbers. */
  template < typename TT, typename SS, typename RR >
  friend int transfer( const SampleData<TT> &x, const SampleData<SS> &y,
		       SampleData<RR> &h,
		       bool overlap, double (*window)( int j, int n ) );
    /*! Compute gain \a g (magnitude of the transfer function)
        between \a x and \a y.
	\a x and \a y must have the same stepsize() and size().
	\a x and \a y are divided into chunks of two times \a N data points,
	where \a N is the minimum power of two not less than g.size().
	The stepsize() of \a g is set to 0.5/x.stepsize()/N.
	\a TT, \a SS, and \a RR are real numbers. */
  template < typename TT, typename SS, typename RR >
  friend int gain( const SampleData<TT> &x, const SampleData<SS> &y,
		   SampleData<RR> &g,
		   bool overlap, double (*window)( int j, int n ) );
    /*! Compute coherence \a c of \a x and \a y.
        \a x and \a y must have the same stepsize() and size().
	\a x and \a y are divided into chunks of two times \a N data points,
	where \a N is the minimum power of two not less than c.size().
	The stepsize() of \a c is set to 0.5/x.stepsize()/N.
	\a TT, \a SS, and \a RR are real numbers. */
  template < typename TT, typename SS, typename RR >
  friend int coherence( const SampleData<TT> &x, const SampleData<SS> &y,
			SampleData<RR> &c,
			bool overlap, double (*window)( int j, int n ) );
    /*! Returns a lower bound of transmitted information based on the coherence
        \f$ \gamma^2 \f$ in \a c computed by
        \f[ I_{\mathrm{LB}} = -\int_{f_0}^{f_1} \log_2(1-\gamma^2) \, df \f] */
  template < typename RR >
  friend double coherenceInfo( const SampleData< RR > &c, 
			       double f0, double f1 );
    /*! Compute cross spectrum \a c of \a x and \a y.
	\a TT, \a SS, and \a RR are real numbers. */
  template < typename TT, typename SS, typename RR >
  friend int rCSD( const SampleData<TT> &x, const SampleData<SS> &y,
		   SampleData<RR> &c,
		   bool overlap, double (*window)( int j, int n ) );
    /*! Compute gain \a g, coherence \a c and powerspectrum \a ys
        between \a x and \a y.
	\a TT, \a SS, and \a RR are real numbers. */
  template < typename TT, typename SS, typename RR >
  friend int spectra( const SampleData<TT> &x, const SampleData<SS> &y,
		      SampleData<RR> &g, SampleData<RR> &c,
		      SampleData<RR> &ys,
		      bool overlap, double (*window)( int j, int n ) );
    /*! Compute gain \a g, coherence \a c, auto- (\a xs and \a ys)
        and cross spectra (\a cs) between \a x and \a y.
	\a TT, \a SS, and \a RR are real numbers. */
  template < typename TT, typename SS, typename RR >
  friend int spectra( const SampleData<TT> &x, const SampleData<SS> &y,
		      SampleData<RR> &g, SampleData<RR> &c,
		      SampleData<RR> &cs, 
		      SampleData<RR> &xs, SampleData<RR> &ys,
		      bool overlap, double (*window)( int j, int n ) );

    /*! Returns in \a meantrace the average over \a traces
        at each position \a pos() of \a meantrace.
        The \a traces are linearly interpolated. */
  template < typename TT > 
    friend void average( SampleData<TT> &meantrace,
			 const vector< SampleData<TT> > &traces );
    /*! Returns in \a meantrace and \a stdev the average 
        and standard deviation over \a traces
        at each position \a pos() of \a meantrace.
        The \a traces are linearly interpolated. */
  template < typename TT > 
    friend void average( SampleData<TT> &meantrace, SampleData<TT> &stdev,
			 const vector< SampleData<TT> > &traces );

    /*! Return in \a peaks and \a troughs 
        detected peaks and troughs in \a x, respectively.
        Uses the algorithm from B. Todd and D. Andrews 
        ("The identification of peaks in physiological signals.",
	Computers and Biomedical Research, 32, 322-335, 1999).*/
  template < typename TT, class Check >
  friend void peaksTroughs( const SampleData<TT> &x,
			    EventData &peaks, EventData &troughs,
			    double &threshold, Check &check );
    /*! Return in \a events detected peaks in \a x.
        Uses the algorithm from B. Todd and D. Andrews 
        ("The identification of peaks in physiological signals.",
	Computers and Biomedical Research, 32, 322-335, 1999).*/
  template < typename TT, class Check >
  friend void peaks( const SampleData<TT> &x, EventData &events,
		     double &threshold, Check &check );
    /*! Return in \a events detected troughs in \a x.
        Uses the algorithm from B. Todd and D. Andrews 
        ("The identification of peaks in physiological signals.",
	Computers and Biomedical Research, 32, 322-335, 1999).*/
  template < typename TT, class Check >
  void troughs( const SampleData<TT> &x, EventData &events,
		double &threshold, Check &check );
    /*! Return in \a events detected events in \a x
        that cross \a threshold with a positive slope. */
  template < typename TT, class Check >
  void rising( const SampleData<TT> &x, EventData &events,
	       double &threshold, Check &check );
    /*! Return in \a events detected events in \a x
        that cross \a threshold with a negative slope. */
  template < typename TT, class Check >
  void falling( const SampleData<TT> &x, EventData &events,
	      double &threshold, Check &check );

    /*! Write content of the SampleData into stream \a str.
        Each element is written in a line.
        First, the string \a start is written,
        then the value of the range element ( pos( k ) ),
        then the string \a separator followed
        by the value of the Array element,
        and finally a newline.
        The array element is formatted in a field of width \a width characters
        and \a precision decimals. 
        The range element is automatically formatted. */
  ostream &save( ostream &str, int width=8, int prec=3,
		 const string &start="", const string &separator=" " ) const;
  void save( const string &file, int width=8, int prec=3,
	     const string &start="", const string &separator=" " ) const;
  template < typename TT > 
  friend ostream &operator<<( ostream &str, const SampleData<TT> &a );
    /*! Read from stream \a str until end of file
        or a line beginning with \a stop is reached.
        If \a stop equals "EMPTY",
        reading is stopped at an empty line.
        If \a line does not equal zero
        then the last read line is returned in \a line. */
  istream &load( istream &str, const string &stop="", string *line=0 );
  SampleData< T > &load( const string &file, const string &comment="#",
			   const string &stop="" );
  template < typename TT >
  friend istream &operator>>( istream &str, SampleData<TT> &a );


private:
  
  mutable LinearRange Samples;
  
};


typedef SampleData< double > SampleDataD;
typedef SampleData< float > SampleDataF;

  /*! Returns sin(2*pi*f*x)
      computed for each element \a x of the range \a r. */
SampleData<> sin( const LinearRange &r, double f );
SampleData<> sin( int n, double offset, double stepsize, double f );
SampleData<> sin( double l, double r, double stepsize, double f );

  /*! Returns cos(2*pi*f*x)
      computed for each element \a x of the range \a r. */
SampleData<> cos( const LinearRange &r, double f );
SampleData<> cos( int n, double offset, double stepsize, double f );
SampleData<> cos( double l, double r, double stepsize, double f );

  /*! Returns the standard normal distribution exp( -0.5*x^2 )/sqrt(2*pi) 
      for each element \a x of the range \a r. */
SampleData<> gauss( const LinearRange &r );
SampleData<> gauss( int n, double offset, double stepsize );
SampleData<> gauss( double l, double r, double stepsize );

  /*! Returns the normal distribution
      exp( -0.5*(x-m)^2/s^2 )/sqrt(2*pi)/s 
      with standard deviation \a s and mean \a m
      for each element \a x of the range \a r. */
SampleData<> gauss( const LinearRange &r, double s, double m );
SampleData<> gauss( int n, double offset, double stepsize, double s, double m );
SampleData<> gauss( double l, double r, double stepsize, double s, double m );

  /*! Returns the alpha function y*exp(-y) with
      y = (x-offs)/tau for each element \a x of the range \a r. */
SampleData<> alpha( const LinearRange &r, double tau, double offs );
SampleData<> alpha( int n, double offset, double stepsize, double tau, double offs );
SampleData<> alpha( double l, double r, double stepsize, double tau, double offs );

  /*! Returns a straight line with abscissa \a abscissa and 
      slope \a slope
      computed for each element \a x of the range \a r. */
SampleData<> line( const LinearRange &r, double abscissa, double slope );
SampleData<> line( int n, double offset, double stepsize, double abscissa, double slope );
SampleData<> line( double l, double r, double stepsize, double abscissa, double slope );

  /*! Returns a rectangular pulse pattern with period \a period, 
      duration of the rectangle \a width, and maximum value 1.0
      computed for each element \a x of the range \a r.
      The up- and downstrokes have a width of \a ramp. */
SampleData<> rectangle( const LinearRange &r,
			double period, double width, double ramp );
SampleData<> rectangle( int n, double offset, double stepsize,
			double period, double width, double ramp );
SampleData<> rectangle( double l, double r, double stepsize,
			double period, double width, double ramp );

  /*! Returns a sawtooth with period \a period and maximum value 1.0
      computed for each element \a x of the range \a r.
      The downstroke has a width of \a ramp. */
SampleData<> sawUp( const LinearRange &r, double period, double ramp );
SampleData<> sawUp( int n, double offset, double stepsize, double period, double ramp );
SampleData<> sawUp( double l, double r, double stepsize, double period, double ramp );

  /*! Returns a sawtooth with period \a period and maximum value 1.0
      computed for each element \a x of the range \a r.
      The upstroke has a width of \a ramp. */
SampleData<> sawDown( const LinearRange &r, double period, double ramp );
SampleData<> sawDown( int n, double offset, double stepsize, double period, double ramp );
SampleData<> sawDown( double l, double r, double stepsize, double period, double ramp );

  /*! Returns a triangular sawtooth with period \a period and maximum value 1.0
      computed for each element \a x of the range \a r. */
SampleData<> triangle( const LinearRange &r, double period );
SampleData<> triangle( int n, double offset, double stepsize, double period );
SampleData<> triangle( double l, double r, double stepsize, double period );


  /*! Compute power \a p of the half-complex sequence in \a hc.
      Sets the stepsize() of  \a p to the one of \a hc.
      Half the number hc.size() of data elements in \a hc
      can be assigned a power in \a p, excess elements are set to zero.
      The spectrum is normalized such that its sum equals the
      mean squared amplitudes of the signal.
      \a TT and \a SS are real numbers.
      \sa hcMagnitude(), hcPhase(), transfer() */
template < typename TT, typename SS >
void hcPower( const SampleData<TT> &hc, SampleData<SS> &p );
  /*! Compute magnitude \a m of the half-complex sequence in \a hc.
      Sets the stepsize() of  \a m to the one of \a hc.
      Half the number hc.size() of data elements in \a hc
      can be assigned a magnitude in \a m, excess elements are set to zero.
      \a TT and \a SS are real numbers.
      \sa hcPower(), hcPhase(), transfer() */
template < typename TT, typename SS >
void hcMagnitude( const SampleData<TT> &hc, SampleData<SS> &m );
  /*! Compute phase \a p (argument, from -pi to pi) 
      of the half-complex sequence in \a hc.
      Sets the stepsize() of  \a p to the one of \a hc.
      Half the number hc.size() of data elements in \a hc
      can be assigned a phase in \a p, excess elements are set to zero.
      \a TT and \a SS are real numbers.
      \sa hcPower(), hcMagnitude(), transfer() */
template < typename TT, typename SS >
void hcPhase( const SampleData<TT> &hc, SampleData<SS> &p );
  /*! Compute real parts of the half-complex sequence in \a hc.
    Sets the stepsize() of  \a r to the one of \a hc.
    Half the number hc.size() of data elements in \a hc
    can be assigned a real part in \a p, excess elements are set to zero.
    \a TT and \a SS are real numbers.
    \sa hcPower(), hcMagnitude(), hcPhase(), hcImaginary(), rFFT(), transfer() */
template < typename TT, typename SS >
void hcReal( const SampleData<TT> &hc, SampleData<SS> &r );
  /*! Compute imaginary parts of the half-complex sequence in \a hc.
      Sets the stepsize() of  \a i to the one of \a hc.
      Half the number hc.size() of data elements in \a hc
      can be assigned a imaginary part in \a p, excess elements are set to zero.
      \a TT and \a SS are real numbers.
      \sa hcPower(), hcMagnitude(), hcPhase(), hcReal(), rFFT(), transfer() */
template < typename TT, typename SS >
void hcImaginary( const SampleData<TT> &hc, SampleData<SS> &i );

  /*! Compute an in-place radix-2 FFT on \a x containing real numbers.
      The size \a N of \a x has to be a power of two, otherwise -1 is returned.
      The output is a half-complex sequence, which is stored in-place. 
      The arrangement of the half-complex terms uses the following
      scheme: for k < N/2 the real part of the k-th term is stored in
      location k, and the corresponding imaginary part is stored in
      location N-k. Terms with k > N/2 (the negative frequencies)
      can be reconstructed using the symmetry z_k = z^*_{N-k}.
      The terms for k=0 and k=N/2 are both
      purely real, and count as a special case. Their real parts are
      stored in locations 0 and N/2 respectively, while their
      imaginary parts which are zero are not stored.
      The first half of the output range contains the positive frequencies
      at i/(N stepsize()), i=0..N/2.
      Use hcPower() and hcPhase() to compute power and phase of the spectrum.
      Algorithm adapted from the GNU Scientific Library http://www.gnu.org/software/gsl .
      \sa hcPower(), hcMagnitude(), hcPhase(), rPSD() */
template < typename TT >
int rFFT( SampleData<TT> &x );
  /*! Power spectrum \a p of \a x.
      The spectrum is normalized such that its sum equals the
      mean squared amplitudes of the signal \a x.
      A fourier window of size \a n ( a power of two no less than 2*p.size() ) is used.
      p.stepsize() is set to its appropriate value 0.5/x.stepsize()/n.
      \a TT and \a SS are real numbers. */
template < typename TT, typename SS >
  int rPSD( const SampleData<TT> &x, SampleData<SS> &p,
	    bool overlap=true, 
	    double (*window)( int j, int n )=bartlett );
  /*! Compute transfer function \a h (half-complex sequence) 
      between \a x and \a y.
      \a x and \a y must have the same stepsize() and size().
      \a x and \a y are divided into chunks of \a N data points,
      where \a N = h.size(). h.size() must be a power of two.
      The stepsize() of \a h is set to 1.0/x.stepsize()/h.size().
      The gain and phase of the transfer function can be obtained
      using hcMagnitude() and hcPhase().
      \a TT, \a SS, and \a RR are real numbers. */
template < typename TT, typename SS, typename RR >
  int transfer( const SampleData<TT> &x, const SampleData<SS> &y,
		SampleData<RR> &h,
		bool overlap=true, 
		double (*window)( int j, int n )=bartlett );
  /*! Compute gain \a g (magnitude of the transfer function)
      between \a x and \a y.
      \a x and \a y must have the same stepsize() and size().
      \a x and \a y are divided into chunks of two times \a N data points,
      where \a N is the minimum power of two not less than g.size().
      The stepsize() of \a g is set to 0.5/x.stepsize()/N.
      \a TT, \a SS, and \a RR are real numbers. */
template < typename TT, typename SS, typename RR >
  int gain( const SampleData<TT> &x, const SampleData<SS> &y,
	    SampleData<RR> &g, bool overlap=true, 
	    double (*window)( int j, int n )=bartlett );
  /*! Compute coherence \a c of \a x and \a y.
      \a x and \a y must have the same stepsize() and size().
      \a x and \a y are divided into chunks of two times \a N data points,
      where \a N is the minimum power of two not less than c.size().
      The stepsize() of \a c is set to 0.5/x.stepsize()/N.
      \a TT, \a SS, and \a RR are real numbers. */
template < typename TT, typename SS, typename RR >
  int coherence( const SampleData<TT> &x, const SampleData<SS> &y,
		 SampleData<RR> &c,
		 bool overlap=true, 
		 double (*window)( int j, int n )=bartlett );
  /*! Returns a lower bound of transmitted information based on the coherence
      \f$ \gamma^2 \f$ in \a c computed by
      \f[ I_{\mathrm{LB}} = -\int_{f_0}^{f_1} \log_2(1-\gamma^2) \, df \f] */
template < typename RR >
  double coherenceInfo( const SampleData< RR > &c, 
			double f0=0.0, double f1=-1.0 );
  /*! Compute cross spectrum \a c of \a x and \a y.
      \a TT, \a SS, and \a RR are real numbers. */
template < typename TT, typename SS, typename RR >
  int rCSD( const SampleData<TT> &x, const SampleData<SS> &y,
	    SampleData<RR> &c,
	    bool overlap=true, 
	    double (*window)( int j, int n )=bartlett );
  /*! Compute gain \a g, coherence \a c and powerspectrum \a ys
      between \a x and \a y.
      a TT, \a SS, and \a RR are real numbers. */
template < typename TT, typename SS, typename RR >
  int spectra( const SampleData<TT> &x, const SampleData<SS> &y,
	       SampleData<RR> &g, SampleData<RR> &c, SampleData<RR> &ys,
	       bool overlap=true, 
	       double (*window)( int j, int n )=bartlett );
  /*! Compute gain \a g, coherence \a c, auto- (\a xs and \a ys)
      and cross spectra (\a cs) between \a x and \a y.
      \a TT, \a SS, and \a RR are real numbers. */
template < typename TT, typename SS, typename RR >
  int spectra( const SampleData<TT> &x, const SampleData<SS> &y,
	       SampleData<RR> &g, SampleData<RR> &c, 
	       SampleData<RR> &cs, 
	       SampleData<RR> &xs, SampleData<RR> &ys,
	       bool overlap=true, 
	       double (*window)( int j, int n )=bartlett );


template < typename T > 
SampleData< T >::SampleData( void )
  : Array<T>::Array(),
    Samples()
{
}


template < typename T > 
SampleData< T >::SampleData( int n, const T &val )
  : Array<T>::Array( n, val ),
    Samples( n, 0, 1 )
{
}


template < typename T > 
SampleData< T >::SampleData( long n, const T &val )
  : Array<T>::Array( n, val ),
    Samples( n, 0, 1 )
{
}


template < typename T > 
SampleData< T >::SampleData( int n, double offset, double stepsize, 
			     const T &val )
  : Array<T>::Array( n, val ),
    Samples( n, offset, stepsize )
{
}


template < typename T > 
SampleData< T >::SampleData( long n, double offset, double stepsize, 
			     const T &val )
  : Array<T>::Array( n, val ),
    Samples( n, offset, stepsize )
{
}


template < typename T > 
SampleData< T >::SampleData( double l, double r, double stepsize, 
			     const T &val )
  : Array<T>::Array(),
    Samples( l, r, stepsize )
{
  Array<T>::resize( Samples.size(), val );
}


template < typename T > 
SampleData< T >::SampleData( const LinearRange &range, const T &val )
  : Array<T>::Array(),
    Samples( range )
{
  Array<T>::resize( Samples.size(), val );
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const R *a, int n, double offset, 
			     double stepsize )
  : Array<T>::Array( a, n ),
    Samples( n, offset, stepsize )
{
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const R *a, int n, const LinearRange &range )
  : Array<T>::Array( a, n ),
    Samples( range )
{
  Samples.resize( n );
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const vector< R > &a,
			     double offset, double stepsize )
       : Array<T>::Array( a ),
         Samples( (int)a.size(), offset, stepsize )
{
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const vector< R > &a, const LinearRange &range )
       : Array<T>::Array( a ),
         Samples( range )
{
  Samples.resize( a.size() );
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const Array< R > &a,
			     double offset, double stepsize )
  : Array<T>::Array( a ),
    Samples( a.size(), offset, stepsize )
{
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const Array< R > &a, const LinearRange &range )
  : Array<T>::Array( a ),
    Samples( range )
{
  Samples.resize( a.size() );
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const SampleData< R > &sa )
  : Array<T>::Array( sa ),
    Samples( sa.range() )
{
}


template < typename T > template < typename R >
SampleData< T >::SampleData( const SampleData< R > &sa,
			     const LinearRange &range )
{
  assign( sa, range );
}


template < typename T >
SampleData< T >::SampleData( const SampleData< T > &sa )
  : Array<T>::Array( sa ),
    Samples( sa.range() )
{
}


template < typename T > 
SampleData< T >::~SampleData( void )
{
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::operator=( const R &a )
{
  assign( a );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::operator=( const SampleData< R > &a )
{
  return assign( a );
}


template < typename T >
const SampleData< T > &SampleData< T >::operator=( const SampleData< T > &a )
{
  return assign( a );
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::assign( const R *a, int n )
{
  Array<T>::assign( a, n );
  Samples.resize( n );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::assign( const R *a, int n, 
						double offset,
						double stepsize )
{
  Array<T>::assign( a, n );
  Samples.assign( n, offset, stepsize );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::assign( const R &a )
{
  Array<T>::assign( a );
  Samples.resize( (int)a.size() );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::assign( const R &a, double offset,
						double stepsize )
{
  Array<T>::assign( a );
  Samples.assign( (int)a.size(), offset, stepsize );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::assign( const SampleData< R > &sa )
{
  Array<T>::assign( sa.array() );
  Samples.assign( sa.range() );
  return *this;
}


template < typename T >
const SampleData< T > &SampleData< T >::assign( const SampleData< T > &sa )
{
  Array<T>::assign( sa.array() );
  Samples.assign( sa.range() );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::interpolate( const SampleData< R > &sa, 
						     double offset, 
						     double stepsize )
{
  int n = (int)::ceil( sa.length() / stepsize );
  resize( n );
  setRange( offset, stepsize );
  for ( int k=0; k<size(); k++ )
    operator[]( k ) = static_cast< T >( sa.interpolate( pos( k ) ) );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::interpolate( const SampleData< R > &sa,
						     const LinearRange &range )
{
  resize( range );
  for ( int k=0; k<size(); k++ )
    operator[]( k ) = static_cast< T >( sa.interpolate( pos( k ) ) );
  return *this;
}


template < typename T > template < typename R >
SampleData< T > &SampleData< T >::whiteNoise( int n, double step,
					      double cflow, double cfup, 
					      R &r )
{
  clear();
  setRange( 0.0, step );
  resize( n, 0 );

  if ( cflow > cfup )
    return *this;

  // invalid cutoff frequency (Nyquist!):
  //  if ( cfup > 0.5/stepsize() )
  //    return *this;

  // take next power of 2 to n:
  int nn = 1 << (int)::ceil( ::log(size())/::log(2.0) );

  // frequency range:
  int inx0 = (int)::rint( stepsize()*nn*cflow );
  int inx1 = (int)::rint( stepsize()*nn*cfup );
  double sigma = 0.5 / ::sqrt( inx1 - inx0 );

  // invalid frequency range:
  if ( inx1 < inx0 )
    return *this;

  // workspace:
  Array<T> whitef( nn, 0 );

  // generating noise in fourier space:
  if ( inx0 <= 0 ) {
    whitef[0] = r.gaussian();
    inx0++;
  }
  int ninx1 = inx1 < nn/2 ? inx1 : nn/2-1;
  for ( int i=inx0; i <= ninx1; i++ ) {
    whitef[i] = r.gaussian();
    whitef[nn-i] = r.gaussian();
  }
  if ( inx1 >= nn/2 )
    whitef[nn/2] = r.gaussian();

  // fourier inversion and renormalization:
  hcFFT( whitef );
  whitef *= sigma;

  // copy result into buffer:
  for ( int i=0; i<size() && i<nn; i++ )
    operator[]( i ) =  whitef[i];

  return *this;
}


template < typename T > template < typename R >
SampleData< T > &SampleData< T >::whiteNoise( double l, double stepsize,
					      double cflow, double cfup, R &r )
{
  clear();
  setRange( 0.0, stepsize );
  return whiteNoise( index( l ) + 1, 
		     stepsize, cflow, cfup, r );
}


template < typename T > template < typename R >
SampleData< T > &SampleData< T >::ouNoise( int n, double step,
					   double tau, R &r )
{
  clear();
  setRange( 0.0, step );
  randNorm( n, r );

  // invalid tau:
  if ( tau < stepsize() )
    return *this;

  double rho = ::exp( -stepsize()/tau );
  double sqrho = ::sqrt( 1.0 - rho*rho );

  // euler integration:
  iterator iter1 = begin();
  iterator end1 = end();
  T x = *iter1;
  
  ++iter1;
  while ( iter1 != end1 ) {
    x = rho * x + sqrho * (*iter1);
    *iter1 = x;
    ++iter1;
  }
  
  return *this;
}


template < typename T > template < typename R >
SampleData< T > &SampleData< T >::ouNoise( double l, double stepsize,
					   double tau, R &r )
{
  clear();
  setRange( 0.0, stepsize );
  return ouNoise( index( l ) + 1, stepsize, tau, r );
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::copy( R *a, int n, const T &val ) const
{
  Array<T>::copy( a, n, val );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::copy( R &a ) const
{
  Array<T>::copy( a );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::copy( SampleData< R > &sa ) const
{
  sa.assign( *this );
  return *this;
}


template < typename T > template < typename R >
  const SampleData< T > &SampleData< T >::copy( double x1, double x2,
						SampleData< R > &sa ) const
{
  sa.clear();
  int k1 = index( x1 );
  if ( k1 < 0 )
    k1 = 0;
  x1 = pos( k1 );
  int k2 = index( x2 );
  if ( k2 >= size() )
    k2 = size()-1;
  int n = k2 - k1 + 1;
  if ( n <= 0 )
    return *this;
  sa.reserve( n );
  sa.setRange( x1, stepsize() );
  for ( int k=0; k<n; k++ )
    sa.push( static_cast< R >( operator[]( k1+k ) ) );
  return *this;
}


template < typename T > template < typename R >
  const SampleData< T > &SampleData< T >::copy( double x1, double x2,
						Map< R > &m ) const
{
  m.clear();
  int k1 = index( x1 );
  if ( k1 < 0 )
    k1 = 0;
  int k2 = index( x2 );
  if ( k2 >= size() )
    k2 = size()-1;
  int n = k2 - k1 + 1;
  if ( n <= 0 )
    return *this;
  m.reserve( n );
  for ( int k=0; k<n; k++ )
    m.push( pos( k1+k ), static_cast< R >( operator[]( k1+k ) ) );
  return *this;
}


template < typename T > template < typename R >
  const SampleData< T > &SampleData< T >::copy( double x1, double x2,
						Array< R > &a ) const
{
  a.clear();
  int k1 = index( x1 );
  if ( k1 < 0 )
    k1 = 0;
  int k2 = index( x2 );
  if ( k2 >= size() )
    k2 = size()-1;
  int n = k2 - k1 + 1;
  if ( n <= 0 )
    return *this;
  a.reserve( n );
  for ( int k=0; k<n; k++ )
    a.push( static_cast< R >( operator[]( k1+k ) ) );
  return *this;
}


template < typename T > template < typename R >
  const SampleData< T > &SampleData< T >::copy( double x1, double x2,
						vector< R > &v ) const
{
  v.clear();
  int k1 = index( x1 );
  if ( k1 < 0 )
    k1 = 0;
  int k2 = index( x2 );
  if ( k2 >= size() )
    k2 = size()-1;
  int n = k2 - k1 + 1;
  if ( n <= 0 )
    return *this;
  v.reserve( n );
  for ( int k=0; k<n; k++ )
    v.push( static_cast< R >( operator[]( k1+k ) ) );
  return *this;
}


template < typename T >
const SampleData< T > &SampleData< T >::append( T a, int n )
{
  Array<T>::append( a, n );
  Samples.append( n );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::append( const R *a, int n )
{
  Array<T>::append( a, n );
  Samples.append( n );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::append( const R &a )
{
  Array<T>::append( a );
  Samples.append( a.size() );
  return *this;
}


template < typename T > template < typename R >
const SampleData< T > &SampleData< T >::append( const SampleData< R > &sa )
{
  if ( stepsize() == sa.stepsize() ) {
    Array<T>::append( sa );
    Samples.append( sa.size() );
  }
  else {
    int n = int( sa.length() / stepsize() );
    reserve( size() + n );
    n = capacity() - size();
    for ( int k=0; k<n; k++ )
      push( static_cast< T >( sa.interpolate( sa.offset() + k*stepsize() ) ) );
  }
  return *this;
}


template < typename T > 
int SampleData< T >::resize( int n, const T &val )
{
  n =  Array<T>::resize( n, val );
  Samples.resize( n );
  return n;
}


template < typename T > 
int SampleData< T >::resize( long n, const T &val )
{
  n = Array<T>::resize( n, val );
  Samples.resize( n );
  return n;
}


template < typename T > 
int SampleData< T >::resize( int n, double stepsize, const T &val )
{
  n = Array<T>::resize( n, val );
  Samples.assign( n, offset(), stepsize );
  return n;
}


template < typename T > 
int SampleData< T >::resize( long n, double stepsize, const T &val )
{
  n = Array<T>::resize( n, val );
  Samples.assign( n, offset(), stepsize );
  return n;
}


template < typename T > 
int SampleData< T >::resize( int n, double offset, double stepsize,
			     const T &val )
{
  n = Array<T>::resize( n, val );
  Samples.assign( n, offset, stepsize );
  return n;
}


template < typename T > 
int SampleData< T >::resize( long n, double offset, double stepsize,
			      const T &val )
{
  n = Array<T>::resize( n, val );
  Samples.assign( n, offset, stepsize );
  return n;
}


template < typename T > 
int SampleData< T >::resize( double r, double stepsize, const T &val )
{
  Samples.assign( r, stepsize );
  return Array<T>::resize( Samples.size(), val );
}


template < typename T > 
int SampleData< T >::resize( double l,  double r, double stepsize, const T &val )
{
  Samples.assign( l, r, stepsize );
  return Array<T>::resize( Samples.size(), val );
}


template < typename T > 
int SampleData< T >::resize( const LinearRange &range, const T &val )
{
  Samples.assign( range );
  return Array<T>::resize( Samples.size(), val );
}


template < typename T > 
void SampleData< T >::clear( void )
{
  Array<T>::clear();
  Samples.clear();
}


template < typename T > 
int SampleData< T >::reserve( int n )
{
  return Array<T>::reserve( n );
}


template < typename T > 
void SampleData< T >::free( int n )
{
  Array<T>::free( n );
  Samples.resize( Array<T>::size() );
}

template < typename T > 
void SampleData< T >::setLength( double l )
{ 
  Samples.resize( ceil( fabs( l/stepsize() ) ) ); 
  Array<T>::resize( Samples.size() );
}


template < typename T > 
int SampleData< T >::push( const T &val )
{
  return Array<T>::push( val );
  // XXX this does not resize Samples! but you want to be efficient!
}


template < typename T > 
T SampleData< T >::pop( void )
{
  Samples.pop();
  return Array<T>::pop();
}


template < typename T > 
SampleData< T > &SampleData< T >::insert( int i, const T &yval )
{
  if ( i>=0 && i<=size() ) {
    Samples.resize( size() + 1 );
    Array<T>::insert( i, yval );
  }

  return *this;
}


template < typename T > 
typename SampleData< T >::iterator SampleData< T >::insert( iterator i, 
							    const T &yval )
{
  int n = i - begin();
  insert( n, yval );
  return begin() + n;
}


template < typename T > 
SampleData< T > &SampleData< T >::erase( int i )
{
  if ( i>=0 && i<size() ) {
    Samples.resize( size() - 1 );
    Array<T>::erase( i );
  }

  return *this;
}


template < typename T > 
typename SampleData< T >::iterator SampleData< T >::erase( iterator i )
{
  int n = i - begin();
  erase( n );
  return begin() + n;
}


template < typename TT > 
bool operator==( const SampleData<TT> &a, const SampleData<TT> &b )
{
  a.range().resize( a.array().size() );
  b.range().resize( b.array().size() );
  return ( a.range() == b.range() && a.array() == b.array() );
}


template < typename  TT> 
bool operator<( const SampleData<TT> &a, const SampleData<TT> &b )
{
  a.range().resize( a.array().size() );
  b.range().resize( b.array().size() );
  return ( a.range() < b.range() && a.array() < b.array() );
}


/* Used by macro SAMPLEDARRAYOPS1SCALARDEF to generate
   definitions for unary operators of class SampleData 
   that take a scalar as argument. 
   \a COPNAME is the operator name (like operator+= ),
   \a COP is the operator (like += ), and
   \a SCALAR is the type of the scalar argument. */
#define SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, SCALAR ) \
  template < typename T >							\
  const SampleData< T > &SampleData< T >::COPNAME( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) COP static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary operators of class SampleData. 
   that take scalars as argument.
   \a COPNAME is the operator name (like operator+= ), and
   \a COP is the operator name (like += ). */
#define SAMPLEDARRAYOPS1SCALARDEF( COPNAME, COP ) \
  SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, float ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, double ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, long double ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, signed char ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, unsigned char ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, signed int ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, unsigned int ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, signed long ) \
    SAMPLEDARRAYOPS1SINGLESCALARDEF( COPNAME, COP, unsigned long ) \


/* Generates definitions for unary operators of class SampleData. 
   \a COPNAME is the operator name (like operator+= ), and
   \a COP is the operator name (like += ). */
#define SAMPLEDARRAYOPS1DEF( COPNAME, COP )                             \
  template < typename T > template < class COT >			\
  const SampleData< T > &SampleData< T >::COPNAME ( const COT &x )	\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    typename COT::const_iterator iter2 = x.begin();			\
    typename COT::const_iterator end2 = x.end();			\
    while ( iter1 != end1 && iter2 != end2 ) {				\
      (*iter1) COP static_cast< value_type >(*iter2);			\
      ++iter1;								\
      ++iter2;								\
    };									\
    return *this;							\
  }									\
  template < typename T > template < class COT >			\
  const SampleData< T > &SampleData< T >::COPNAME ( const SampleData<COT> &x )	\
  {									\
    resize( x.range(), 0 );						\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    typename SampleData<COT>::const_iterator iter2 = x.begin();			\
    typename SampleData<COT>::const_iterator end2 = x.end();			\
    while ( iter1 != end1 && iter2 != end2 ) {				\
      (*iter1) COP static_cast< value_type >(*iter2);			\
      ++iter1;								\
      ++iter2;								\
    };									\
    return *this;							\
  }									\
  SAMPLEDARRAYOPS1SCALARDEF( COPNAME, COP ) \


  SAMPLEDARRAYOPS1SCALARDEF( operator=, = )
  SAMPLEDARRAYOPS1DEF( operator+=, += )
  SAMPLEDARRAYOPS1DEF( operator-=, -= )
  SAMPLEDARRAYOPS1DEF( operator*=, *= )
  SAMPLEDARRAYOPS1DEF( operator/=, /= )
  SAMPLEDARRAYOPS1DEF( operator%=, %= )


#undef SAMPLEDARRAYOPS1SINGLESCALARDEF
#undef SAMPLEDARRAYOPS1SCALARDEF
#undef SAMPLEDARRAYOPS1DEF


/* Used by macro SAMPLEDARRAYOPS2SCALARDEF to generate
   definitions for binary class friend operators 
   that take the class and a scalar as argument. 
   \a COPNAME is the operator name (like operator+ ),
   \a COP is the operator (like + ), and
   \a SCALAR is the type of the scalar argument. */
#define SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, SCALAR ) \
  template < typename TT >							\
  SampleData<TT> COPNAME( SCALAR x, const SampleData<TT> &y )		\
  {									\
    SampleData<TT> z( y );						\
    typename SampleData<TT>::iterator iter1 = z.begin();			\
    typename SampleData<TT>::iterator end1 = z.end();			\
    while ( iter1 != end1 ) {						\
      (*iter1) = static_cast< typename SampleData<TT>::value_type >( x COP (*iter1) ); \
      ++iter1;								\
    };									\
    return z;								\
  }									\
									\
  template < typename TT >							\
  SampleData<TT> COPNAME( const SampleData<TT> &x, SCALAR y )		\
  {									\
    SampleData<TT> z( x );						\
    typename SampleData<TT>::iterator iter1 = z.begin();			\
    typename SampleData<TT>::iterator end1 = z.end();			\
    while ( iter1 != end1 ) {						\
      (*iter1) = static_cast< typename SampleData<TT>::value_type >( (*iter1) COP y ); \
      ++iter1;								\
    };									\
    return z;								\
  }									\


/* Generates definitions for binary class member operators. 
   that take scalars as argument.
   \a COPNAME is the operator name (like operator+ ), and
   \a COP is the operator name (like + ). */
#define SAMPLEDARRAYOPS2SCALARDEF( COPNAME, COP ) \
  SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, float ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, double ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, long double ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, signed char ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, unsigned char ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, signed int ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, unsigned int ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, signed long ) \
    SAMPLEDARRAYOPS2SINGLESCALARDEF( COPNAME, COP, unsigned long ) \


/* Generates declarations for binary class friend operators
   that take other containers as the second argument or a scalar as either argument.
   \a COPNAME is the operator name (like operator+ ), and
   \a COP is the operator name (like += ). */
#define SAMPLEDARRAYOPS2DEF( COPNAME, COP ) \
  template < typename TT, typename COT >				\
    SampleData<TT> COPNAME( const SampleData<TT> &x, const COT &y )	\
  {									\
    SampleData<TT> z( x );						\
    typename SampleData<TT>::iterator iter1 = z.begin();			\
    typename SampleData<TT>::iterator end1 = z.end();			\
    typename COT::const_iterator iter2 = y.begin();			\
    typename COT::const_iterator end2 = y.end();			\
    while ( iter1 != end1 && iter2 != end2 ) {				\
      (*iter1) = (*iter1) COP (*iter2);					\
      ++iter1;								\
      ++iter2;								\
    };									\
    return z;								\
  }									\
									\
  SAMPLEDARRAYOPS2SCALARDEF( COPNAME, COP ) \


  SAMPLEDARRAYOPS2DEF( operator+, + )
  SAMPLEDARRAYOPS2DEF( operator-, - )
  SAMPLEDARRAYOPS2DEF( operator*, * )
  SAMPLEDARRAYOPS2DEF( operator/, / )
  SAMPLEDARRAYOPS2DEF( operator%, % )


#undef SAMPLEDARRAYOPS2SINGLESCALARDEF
#undef SAMPLEDARRAYOPS2SCALARDEF
#undef SAMPLEDARRAYOPS2DEF


template < typename T > 
T SampleData< T >::integral( void ) const
{
  if ( empty() )
    return 0.0;

  const_iterator first = begin();
  const_iterator last = end();
  double sum = *first;
  while ( ++first != last ) {
    sum += *first;
  }

  return sum*stepsize();
}


template < typename T >
SampleData< T > &SampleData< T >::rampUp( double x )
{
  int maxi = indices( x );
  for ( int k=0; k<maxi; k++ ) {
    Array<T>::operator[]( k ) *= static_cast<T>( double(k)/double(maxi) );
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::rampDown( double x )
{
  int maxi = indices( x );
  for ( int k=size()-1, i=0; k>size()-1-maxi; k--, i++ ) {
    Array<T>::operator[]( k ) *= static_cast<T>( double(i)/double(maxi) );
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::ramp( double x )
{
  rampUp( x );
  rampDown( x );
  return *this;
}


template < typename T >
SampleData< T > SampleData< T >::operator-( void )
{
  SampleData< T > z( *this );
  iterator iter1 = z.array().begin();
  iterator end1 = z.array().end();
  while ( iter1 != end1 ) {
    (*iter1) = -(*iter1);
    ++iter1;
  };
  return z;
}


template < typename T > 
SampleData< T > &SampleData< T >::identity( void )
{
  for ( int k=0; k<size(); k++ )
    Array<T>::operator[]( k ) = Samples.pos( k );
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::sin( const LinearRange &r, double f )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = ::sin( 6.28318530717959*f*(*iter2) );
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::cos( const LinearRange &r, double f )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = ::cos( 6.28318530717959*f*(*iter2) );
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::gauss( const LinearRange &r )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = ::exp( -0.5*(*iter2)*(*iter2) ) * 0.398942280401433;
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::gauss( const LinearRange &r, double s, double m )
{
  Samples = r;
  resize( r.size() );
  T n = 0.398942280401433 / s;
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    T xx = ((*iter2) - m)/s;
    *iter1 = n * ::exp( -0.5*xx*xx);
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::alpha( const LinearRange &r, double tau, double offs )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    T xx = ((*iter2) - offs)/tau;
    *iter1 = xx > 0.0 ? xx * ::exp( -xx ) : 0.0;
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::line( const LinearRange &r, double abscissa, double slope )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = abscissa + slope * (*iter2);
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::rectangle( const LinearRange &r,
					     double period, double width, double ramp )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  if ( ramp <= 0.0 ) {
    while ( iter1 != end1 ) {
      *iter1 = ::fmod( (*iter2), period ) < width ? 1.0 : 0.0;
      ++iter1;
      ++iter2;
    }
  }
  else {
    if ( ramp > width )
      ramp = width;
    if ( ramp > period - width )
      ramp = period - width;
    while ( iter1 != end1 ) {
      double p = ::fmod( (*iter2), period );
      *iter1 = p < width ? ( p < ramp ? p / ramp : 1.0 ) : ( p < width + ramp ? ( width + ramp - p ) / ramp : 0.0 );
      ++iter1;
      ++iter2;
    }
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::sawUp( const LinearRange &r, double period, double ramp )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  if ( ramp <= 0.0 ) {
    while ( iter1 != end1 ) {
      *iter1 = ::fmod( (*iter2), period ) / period;
      ++iter1;
      ++iter2;
    }
  }
  else {
    if ( ramp > 0.5*period )
      ramp = 0.5*period;
    while ( iter1 != end1 ) {
      double p = ::fmod( (*iter2), period );
      *iter1 = p < period-ramp ? p / ( period - ramp ) : ( period - p ) / ramp;
      ++iter1;
      ++iter2;
    }
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::sawDown( const LinearRange &r, double period, double ramp )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  if ( ramp <= 0.0 ) {
    while ( iter1 != end1 ) {
      *iter1 = 1.0 - ::fmod( (*iter2), period ) / period;
      ++iter1;
      ++iter2;
    }
  }
  else {
    if ( ramp > 0.5*period )
      ramp = 0.5*period;
    while ( iter1 != end1 ) {
      double p = ::fmod( (*iter2), period );
      *iter1 = p < ramp ? p/ramp : ( period - p ) / ( period - ramp );
      ++iter1;
      ++iter2;
    }
  }
  return *this;
}


template < typename T >
SampleData< T > &SampleData< T >::triangle( const LinearRange &r, double period )
{
  Samples = r;
  resize( r.size() );
  typedef typename LinearRange::const_iterator ForwardIter2;
  iterator iter1 = begin();
  iterator end1 = end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    T p = ::fmod( (*iter2), period );
    *iter1 = p < 0.5*period ? 2.0 * p / period : 2.0 - 2.0 * p / period;
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T > 
T SampleData< T >::interpolate( double x ) const
{
  if ( empty() )
    return 0;

  int i = index( x );

  if ( i < 0 )
    return front();
  else if ( i+1 >= size() )
    return back();
  else {
    T slope = ( Array<T>::operator[]( i+1 ) - Array<T>::operator[]( i ) ) / stepsize();
    return Array<T>::operator[]( i ) + slope * ( x - pos( i ) );
  }
}


template < typename T > template < typename ForwardIter >
SampleData< T > &SampleData< T >::addHist( ForwardIter first, ForwardIter last )
{
  while ( first != last ) {
    int bin = index( *first );
    if ( bin >= 0 && bin < size() )
      Array<T>::operator[]( bin ) += 1.0;
    ++first;
  }
  return *this;
}


template < typename T > template < typename R >
SampleData< T > &SampleData< T >::addHist( const R &x )
{
  return addHist( x.begin(), x.end() );
}


template < typename T > template < typename ForwardIter >
SampleData< T > &SampleData< T >::hist( ForwardIter first, ForwardIter last )
{
  *this = 0.0;
  return addHist( first, last );
}


template < typename T > template < typename R >
SampleData< T > &SampleData< T >::hist( const R &x )
{
  return hist( x.begin(), x.end() );
}


template < typename TT, typename RR >
SampleData< TT > convolve( const SampleData< TT > &x, const RR &y, int offs )
{
  SampleData< TT > c = SampleData< TT >( convolve( x.array(), y, offs ), x.range() );
  return c * x.stepsize();
}


template < typename TT >
SampleData< TT > convolve( const SampleData< TT > &x, const SampleData< TT > &y )
{
  int offs = y.index( 0.0 );
  SampleData< TT > c = SampleData< TT >( convolve( x.array(), y, offs ), x.range() );
  return c * x.stepsize();
}


template < typename T > template < typename TT >
SampleData< T > &SampleData< T >::freqFilter( const SampleData< TT > &g,
					      bool rescale )
{
  // original size and power:
  int n = size();
  double orms = 1.0;
  if ( rescale )
    orms = ::relacs::rms( array() );

  // take next power of 2 to n:
  int nn = 1 << (int)::ceil( ::log(size())/::log(2.0) );

  // expand size:
  resize( nn, 0 );

  // fourier transformation:
  rFFT( array() );

  // apply filter:
  operator[]( 0 ) *= g[0];
  for ( int i=1; i < nn/2; i++ ) {
    T gain = g( double( i ) / length() );
    operator[]( i ) *= gain;
    operator[]( nn-i ) *= gain;
  }
  operator[]( nn/2 ) *= g( double( nn/2 ) / length() );

  // fourier inversion and renormalization:
  hcFFT( array() );
  array() *= 1.0/nn;

  // shrink to original size:
  resize( n );

  // rescale rms amplitude:
  if ( rescale ) {
    double frms = ::relacs::rms( array() );
    array() *= orms/frms;
  }

  return *this;
}





template < typename T >
T SampleData< T >::min( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0.0;
  else
    return ::relacs::min( begin()+fi, begin()+li );
}


template < typename T >
int SampleData< T >::minIndex( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return fi + ::relacs::minIndex( begin()+fi, begin()+li );
}


template < typename T >
int SampleData< T >::minIndex( T &min, double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi ) {
    min = 0.0;
    return 0;
  }
  else {
    int index = -1;
    min = ::relacs::min( index, begin()+fi, begin()+li );
    return fi + index;
  }
}


template < typename T >
T SampleData< T >::max( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0.0;
  else
    return ::relacs::max( begin()+fi, begin()+li );
}


template < typename T >
int SampleData< T >::maxIndex( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return fi + ::relacs::maxIndex( begin()+fi, begin()+li );
}


template < typename T >
int SampleData< T >::maxIndex( T &max, double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi ) {
    max = 0.0;
    return 0;
  }
  else {
    int index = -1;
    max = ::relacs::max( index, begin()+fi, begin()+li );
    return fi + index;
  }
}


template < typename T >
void SampleData< T >::minMax( T &min, T &max, double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi ) {
    min = 0.0;
    max = 0.0;
  }
  else
    ::relacs::minMax( min, max, begin()+fi, begin()+li );
}


template < typename T >
void SampleData< T >::minMaxIndex( int &minindex, int &maxindex, double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi ) {
    minindex = 0;
    maxindex = 0;
  }
  else {
    ::relacs::minMaxIndex( minindex, maxindex, begin()+fi, begin()+li );
    minindex += fi;
    maxindex += fi;
  }
}


template < typename T >
void SampleData< T >::minMaxIndex( T &min, int &minindex, T &max, int &maxindex,
				   double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi ) {
    min = 0.0;
    max = 0.0;
    minindex = 0;
    maxindex = 0;
  }
  else {
    ::relacs::minMax( min, minindex, max, maxindex, begin()+fi, begin()+li );
    minindex += fi;
    maxindex += fi;
  }
}


template < typename T >
typename numerical_traits< T >::mean_type
  SampleData< T >::mean( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::mean( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::mean_type
  SampleData< T >::mean( typename numerical_traits< T >::variance_type &stdev,
			 double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi ) {
    stdev = 0;
    return 0;
  }
  else
    return ::relacs::meanStdev( stdev, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::variance( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::variance( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::varianceKnown( typename numerical_traits< T >::mean_type mean,
				  double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::varianceKnown( mean, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::varianceFixed( typename numerical_traits< T >::mean_type fixedmean,
				  double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::varianceFixed( fixedmean, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::stdev( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::stdev( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::stdevKnown( typename numerical_traits< T >::mean_type mean,
			       double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::stdevKnown( mean, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::stdevFixed( typename numerical_traits< T >::mean_type fixedmean,
			       double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::stdevFixed( fixedmean, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::sem( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::sem( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::semKnown( typename numerical_traits< T >::mean_type mean,
			     double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::sem( mean, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::semFixed( typename numerical_traits< T >::mean_type fixedmean,
			     double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::semFixed( fixedmean, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::absdev( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::absdev( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::absdevKnown( typename numerical_traits< T >::mean_type mean,
				double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::absdevKnown( mean, begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::rms( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::rms( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::skewness( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::skewness( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::kurtosis( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::kurtosis( begin()+fi, begin()+li );
}


template < typename T >
T SampleData< T >::sum( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::sum( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::squaredSum( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::squaredSum( begin()+fi, begin()+li );
}


template < typename T >
typename numerical_traits< T >::variance_type
  SampleData< T >::power( double first, double last ) const
{
  int fi = index( first );
  if ( fi < 0 )
    fi = 0;
  int li = index( last );
  if ( li > size() )
    li = size();
  if ( li <= fi )
    return 0;
  else
    return ::relacs::power( begin()+fi, begin()+li );
}


template < typename TT, typename SS >
void hcPower( const SampleData<TT> &hc, SampleData<SS> &p )
{
  p.setRange( 0.0, hc.stepsize() );
  hcPower( hc.array(), p.array() );
}


template < typename TT, typename SS >
void hcMagnitude( const SampleData<TT> &hc, SampleData<SS> &m )
{
  m.setRange( 0.0, hc.stepsize() );
  hcMagnitude( hc.array(), m.array() );
}


template < typename TT, typename SS >
void hcPhase( const SampleData<TT> &hc, SampleData<SS> &p )
{
  p.setRange( 0.0, hc.stepsize() );
  hcPhase( hc.array(), p.array() );
}


template < typename TT, typename SS >
void hcReal( const SampleData<TT> &hc, SampleData<SS> &r )
{
  r.setRange( 0.0, hc.stepsize() );
  hcReal( hc.array(), r.array() );
}


template < typename TT, typename SS >
void hcImaginary( const SampleData<TT> &hc, SampleData<SS> &i )
{
  i.setRange( 0.0, hc.stepsize() );
  hcReal( hc.array(), i.array() );
}


template < typename TT >
int rFFT( SampleData< TT > &x )
{
  x.setRange( 0.0, 1.0/x.stepsize()/x.size() );
  return rFFT( x.array() );
}


template < typename TT, typename SS >
int rPSD( const SampleData< TT > &x, SampleData< SS > &p,
	  bool overlap, double (*window)( int j, int n ) )
{
  int n = 1;
  for ( n = 1; n < p.size(); n <<= 1 );
  p.setRange( 0.0, 0.5/x.stepsize()/n );

  return rPSD( x.array(), p.array(), overlap, window );
}


template < typename TT, typename SS, typename RR >
int transfer( const SampleData<TT> &x, const SampleData<SS> &y,
	      SampleData<RR> &h,
	      bool overlap, double (*window)( int j, int n ) )
{
  h.setRange( 0.0, 1.0/x.stepsize()/h.size() );
  return transfer( x.array(), y.array(), h.array(), overlap, window );
}


template < typename TT, typename SS, typename RR >
int gain( const SampleData<TT> &x, const SampleData<SS> &y,
	  SampleData<RR> &g,
	  bool overlap, double (*window)( int j, int n ) )
{
  int n = 1;
  for ( n = 1; n < g.size(); n <<= 1 );
  g.setRange( 0.0, 0.5/x.stepsize()/n );
  return gain( x.array(), y.array(), g.array(), overlap, window );
}


template < typename TT, typename SS, typename RR >
int coherence( const SampleData<TT> &x, const SampleData<SS> &y,
	       SampleData<RR> &c,
	       bool overlap, double (*window)( int j, int n ) )
{
  int n = 1;
  for ( n = 1; n < c.size(); n <<= 1 );
  c.setRange( 0.0, 0.5/x.stepsize()/n );
  return coherence( x.array(), y.array(), c.array(), overlap, window );
}


template < typename RR >
double coherenceInfo( const SampleData< RR > &c, 
		      double f0, double f1 )
{
  int i0 = c.index( f0 );
  if ( i0 < 0 )
    i0 = 0;
  int i1 = f1 >= 0.0 ? c.index( f1 ) : c.size();
  if ( i1 > c.size() )
    i1 = c.size();
  if ( i1 <= i0 )
    return 0.0;
  else
    return coherenceInfo( c.begin()+i0, c.begin()+i1, c.stepsize() );
}


template < typename TT, typename SS, typename RR >
int rCSD( const SampleData<TT> &x, const SampleData<SS> &y,
	  SampleData<RR> &c,
	  bool overlap, double (*window)( int j, int n ) )
{
  c.setRange( 0.0, 1.0/x.stepsize()/c.size() );
  return rCSD( x.array(), y.array(), c.array(), overlap, window );
}


template < typename TT, typename SS, typename RR >
int spectra( const SampleData<TT> &x, const SampleData<SS> &y,
	     SampleData<RR> &g, SampleData<RR> &c, SampleData<RR> &ys,
	     bool overlap, double (*window)( int j, int n ) )
{
  int n = 1;
  for ( n = 1; n < c.size(); n <<= 1 );
  g.setRange( 0.0, 0.5/x.stepsize()/n );
  c.setRange( 0.0, 0.5/x.stepsize()/n );
  ys.setRange( 0.0, 0.5/x.stepsize()/n );
  return spectra( x.array(), y.array(), g.array(), c.array(), ys.array(),
		  overlap, window );
}


template < typename TT, typename SS, typename RR >
int spectra( const SampleData<TT> &x, const SampleData<SS> &y,
	     SampleData<RR> &g, SampleData<RR> &c, SampleData<RR> &cs, 
	     SampleData<RR> &xs, SampleData<RR> &ys,
	     bool overlap, double (*window)( int j, int n ) )
{
  int n = 1;
  for ( n = 1; n < c.size(); n <<= 1 );
  g.setRange( 0.0, 0.5/x.stepsize()/n );
  c.setRange( 0.0, 0.5/x.stepsize()/n );
  cs.setRange( 0.0, 1.0/x.stepsize()/n );
  xs.setRange( 0.0, 0.5/x.stepsize()/n );
  ys.setRange( 0.0, 0.5/x.stepsize()/n );
  return spectra( x.array(), y.array(), g.array(), c.array(), cs.array(),
		  xs.array(), ys.array(), overlap, window );
}


template < typename TT > 
void average( SampleData<TT> &meantrace,
	      const vector< SampleData<TT> > &traces )
{
  meantrace = 0.0;

  bool equal = true;
  for ( unsigned int j = 0; j<traces.size(); j++ ) {
    if ( ! ( meantrace.range() == traces[j].range() ) ) {
      equal = false;
      break;
    }
  }

  if ( equal ) {
    vector< TT > buf( traces.size() );
    for ( int k=0; k<meantrace.size(); k++ ) {
      for ( unsigned int j=0; j<traces.size(); j++ )
	buf[j] = traces[j][k];
      meantrace[k] = mean( buf );
    }
  }
  else {
    Array< int > inx( traces.size(), 0 );
    for ( int k=0; k<meantrace.size(); k++ ) {
      for ( unsigned int j = 0; j<traces.size(); j++ ) {
	double time = meantrace.pos( k );
	for ( ; inx[j] < traces[j].size() && traces[j].pos(inx[j]) < time; ++inx[j] );
	double tr = 0.0;
	if ( inx[j] >= traces[j].size() )
	  tr = traces[j].back();
	else if ( inx[j] == 0 )
	  tr = traces[j].front();
	else {
	  double slope = ( traces[j][inx[j]] - traces[j][inx[j]-1] ) / ( traces[j].pos(inx[j]) - traces[j].pos(inx[j]-1) );
	  tr = slope*( time - traces[j].pos(inx[j]) ) + traces[j][inx[j]];
	}
	meantrace[k] += ( tr - meantrace[k] )/(j+1);
      }
    }
  }
}


template < typename TT > 
void average( SampleData<TT> &meantrace, SampleData<TT> &stdev,
	      const vector< SampleData<TT> > &traces )
{
  meantrace = 0.0;
  stdev = meantrace;

  bool equal = true;
  for ( unsigned int j = 0; j<traces.size(); j++ ) {
    if ( ! ( meantrace.range() == traces[j].range() ) ) {
      equal = false;
      break;
    }
  }

  if ( equal ) {
    vector< TT > buf( traces.size() );
    for ( int k=0; k<meantrace.size(); k++ ) {
      for ( unsigned int j=0; j<traces.size(); j++ )
	buf[j] = traces[j][k];
      meantrace[k] = meanStdev( stdev[k], buf );
    }
  }
  else {
    Array< int > inx( traces.size(), 0 );
    Array< TT > tr( traces.size() );
    for ( int k=0; k<meantrace.size(); k++ ) {
      for ( unsigned int j = 0; j<traces.size(); j++ ) {
	double time = meantrace.pos( k );
	for ( ; inx[j] < traces[j].size() && traces[j].pos(inx[j]) < time; ++inx[j] );
	if ( inx[j] >= traces[j].size() )
	  tr[j] = traces[j].back();
	else if ( inx[j] == 0 )
	  tr[j] = traces[j].front();
	else {
	  double slope = ( traces[j][inx[j]] - traces[j][inx[j]-1] ) / ( traces[j].pos(inx[j]) - traces[j].pos(inx[j]-1) );
	  tr[j] = slope*( time - traces[j].pos(inx[j]) ) + traces[j][inx[j]];
	}
      }
      meantrace[k] = tr.mean( stdev[k] );
    }
  }
}


template < typename TT, class Check >
void peaksTroughs( const SampleData<TT> &x,
		   EventData &peaks, EventData &troughs,
		   double &threshold, Check &check )
{
  EventList peaktroughs( &peaks );
  peaktroughs.add( &troughs );
  Detector< typename SampleData<TT>::const_iterator, 
    typename SampleData<TT>::const_range_iterator > D;
  D.init( x.begin(), x.end(), x.range().begin() );
  D.peakTrough( x.begin(), x.end(), peaktroughs,
		threshold, threshold, threshold, check );
}


template < typename TT, class Check >
void peaks( const SampleData<TT> &x,
	    EventData &events,
	    double &threshold, Check &check )
{
  Detector< typename SampleData<TT>::const_iterator, 
    typename SampleData<TT>::const_range_iterator > D;
  D.init( x.begin(), x.end(), x.range().begin() );
  D.peak( x.begin(), x.end(), events,
	  threshold, threshold, threshold, check );
}


template < typename TT, class Check >
void troughs( const SampleData<TT> &x,
	      EventData &events,
	      double &threshold, Check &check )
{
  Detector< typename SampleData<TT>::const_iterator, 
    typename SampleData<TT>::const_range_iterator > D;
  D.init( x.begin(), x.end(), x.range().begin() );
  D.trough( x.begin(), x.end(), events,
	    threshold, threshold, threshold, check );
}


template < typename TT, class Check >
void rising( const SampleData<TT> &x,
	     EventData &events,
	     double &threshold, Check &check )
{
  Detector< typename SampleData<TT>::const_iterator, 
    typename SampleData<TT>::const_range_iterator > D;
  D.init( x.begin(), x.end(), x.range().begin() );
  D.rising( x.begin(), x.end(), events,
	    threshold, threshold, threshold, check );
}


template < typename TT, class Check >
void falling( const SampleData<TT> &x,
	      EventData &events,
	      double &threshold, Check &check )
{
  Detector< typename SampleData<TT>::const_iterator, 
    typename SampleData<TT>::const_range_iterator > D;
  D.init( x.begin(), x.end(), x.range().begin() );
  D.falling( x.begin(), x.end(), events,
	     threshold, threshold, threshold, check );
}


template < typename T > 
ostream &SampleData< T >::save( ostream &str, 
				int width, int prec,
				const string &start, 
				const string &separator ) const
{
  if ( ! str.good() )
    return str;

  // time format:
  int timeprec=0;
  int timewidth=0;
  numberFormat( stepsize(), rangeBack(), timewidth, timeprec );

  // output:
  str.setf( ios::fixed, ios::floatfield );
  for ( int k=0; k<size(); k++ ) {
    str << start 
	<< setw( timewidth ) << setprecision( timeprec ) << pos( k ) 
	<< separator 
	<< setw( width ) << setprecision( prec ) << (*this)[k] 
	<< '\n';
  }
  return str;
}


template < typename T > 
void SampleData< T >::save( const string &file, 
			    int width, int prec,
			    const string &start, 
			    const string &separator ) const
{
  ofstream str( file.c_str() );
  save( str, width, prec, start, separator );
}


template < typename T > 
ostream &operator<<( ostream &str, const SampleData< T > &a )
{
  return a.save( str );
}


template < typename T > 
istream &SampleData< T >::load( istream &str, const string &stop,
				string *line )
{
  clear();

  bool stopempty = ( stop == "EMPTY" );

  // stream not opened:
  if ( !str )
    return str;

  // current file position:
  streampos pos = str.tellg();

  string s;

  // count lines:
  long n = 0;
  double x = 0.0;
  double x0 = 0.0;
  double xp = 0.0;
  double y = 0.0;

  // read first line:
  if ( line != 0 && !line->empty() ) {
    // load string:
    const char *fp = line->c_str();
    char *ep;
    x = strtod( fp, &ep );
    if ( ep > fp ) {
      fp = ep;
      y = strtod( fp, &ep );
      if ( ep > fp ) {
	x0 = x;
	n++;
	xp = x;
      }
    }
  }

  while ( getline( str, s ) ) {

    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
         ( stopempty && int(s.find_first_not_of( " \t\n\r\v\f" )) < 0 ) )
      break;

    // load string:
    const char *fp = s.c_str();
    char *ep;
    x = strtod( fp, &ep );
    if ( ep > fp ) {
      fp = ep;
      y = strtod( fp, &ep );
      if ( ep > fp ) {
	if ( n == 0 )
	  x0 = x;
	n++;
	xp = x;
      }
    }
  }

  // allocate memory:
  setOffset( x0 );
  setStepsize( (xp - x0) / double(n-1) );
  reserve( n );

  // rewind:
  str.clear();
  str.seekg( pos );

  // read first line:
  if ( line != 0 && !line->empty() ) {
    // load string:
    const char *fp = line->c_str();
    char *ep;
    x = strtod( fp, &ep );
    if ( ep > fp ) {
      fp = ep;
      y = strtod( fp, &ep );
      if ( ep > fp )
	push( y );
    }
  }

  // read lines:
  while ( getline( str, s ) ) {

    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
         ( stopempty && int(s.find_first_not_of( " \t\n\r\v\f" )) < 0 ) )
      break;

    // load string:
    const char *fp = s.c_str();
    char *ep;
    x = strtod( fp, &ep );
    if ( ep > fp ) {
      fp = ep;
      y = strtod( fp, &ep );
      if ( ep > fp && size() < capacity() )
	push( y );
    }

  }

  Samples.resize( size() ); // push does not set range size.

  // store last read line:
  if ( line != 0 )
    *line = s;

  return str;
}


template < typename T > 
SampleData< T > &SampleData< T >::load( const string &file, const string &comment, 
					const string &stop )
{
  ifstream str( file.c_str() );
  string s;
  while ( getline( str, s ) && 
	  ( s.empty() || s.find( comment ) == 0 ) );
  load( str, stop, &s );
  return *this;
}


template < typename T > 
istream &operator>>( istream &str, SampleData< T > &a )
{
  return a.load( str );
}


}; /* namespace relacs */

#endif /* ! _RELACS_SAMPLEDATA_H_ */
