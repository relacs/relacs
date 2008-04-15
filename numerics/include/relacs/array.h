/*
  array.h
  A template defining an one-dimensional array of data.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _ARRAY_H_
#define _ARRAY_H_


#include <cstdlib>
#include <cstring>
#include <cmath>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#ifdef HAVE_LIBGSL
#include <gsl/gsl_vector.h>
#endif
#include "containerops.h"
#include "stats.h"
#include "random.h"

using namespace std;


/*! 
\class Array
\author Jan Benda
\version 0.3
\brief A template defining an one-dimensional array of data.
\todo type independent reading of data elements in load()
\todo math functions with two arguments: one scalar/container as first argument for member function, z.B. pow/exp
\todo smooth functions (average over k neighboring points, or with kernel)
\todo implement logical operators with scalars and other vectors: >, <, >=, <=, == such that they return a vector of booleans!
\todo implement operator && and || that operate on zero and non-zero entries of arrays.
\todo implement "find": take an Array and return an array of indices pointing to elements != 0.0
\todo implement interface to index vectors


This class is very similar to the vector class from 
the standard template library, in that it is a
random access container of objects of type \a T.
In addition, however, it is specialized for
numerical objects, like double or float, in two ways.
First, the data are stored in a single classical *T array
that can be accessed by the data() function.
This allows to apply ordinary C functions on the array,
and thus makes this container useable for 
many numerical algorithm libraries.
Second, Array supports some basic computations on its
data elements.
All the +,-,*,/,+=,-=,*=,/= operators are implemented,
as well as some basic functions like min(), max(),
mean(), stdev(), etc.

In addition to resize() and reserve() there is a
free() function that allows to free allocated memory.

For convenience an array of double is defined as ArrayD,
an array of float as ArrayF, and an array of int as ArrayI.
*/ 


template < typename T = double > 
class Array 
{

  public:

    /*! Creates an empty array. */
  Array( void );
    /*! Creates an array with \a n data elements 
        without initializing them. */
  Array( int n );
    /*! Creates an array with \a n data elements and 
        initialzes them with \a val. */
  Array( int n, const T &val );
    /*! Creates an array with \a n data elements and
        initialzes them with the values given in \a a. */
  template < typename S >
  Array( const S *a, int n );
    /*! Creates an array with \a n data elements and
        initialzes them with the values given in \a a. */
  Array( const T *a, int n );
    /*! Creates an array with a copy of the range [\a first,\a last)
        of the vector \a a. */
  template < typename S >
  Array( const vector< S > &a, int first=0, int last=-1 );
    /*! Creates an array with a copy of the range [\a first,\a last)
        of the array \a a. */
  template < typename S >
  Array( const Array< S > &a, int first=0, int last=-1 );
    /*! Copy constructor.
        Creates an array with the same size and content
        as the array \a a. */
  Array( const Array< T > &a );
    /*! The destructor. */
  virtual ~Array( void );

    /*! Set the size(), capacity(), and content of the array to \a a. */
  template < typename S >
  const Array<T> &operator=( const S &a );
    /*! Set the size(), capacity(), and content of the array to \a a. */
  const Array<T> &operator=( const Array<T> &a );

    /*! Set the size() and capacity() of the array to \a n
        and its content to \a a. */
  template < typename S >
  const Array<T> &assign( const S *a, int n );
    /*! Set the size() and capacity() of the array to \a n
        and its content to \a a. */
  const Array<T> &assign( const T *a, int n );
    /*! Set the size(), capacity(), and content of the array to \a a. */
  template < typename S >
  const Array<T> &assign( const S &a );
    /*! Set the size(), capacity(), and content of the array to \a a. */
  const Array<T> &assign( const Array<T> &a );

    /*! Initialize the array with \a n zeros.
        \sa resize() */
  Array<T> &zeros( int n );
    /*! Initialize the array with \a n ones.
        \sa resize() */
  Array<T> &ones( int n );
    /*! Initialize the array with \a n uniformly distributed random numbers 
        between zero and one, using the random number generator \a r. */
  template < typename R >
  Array<T> &rand( int n, R &r=numerics::rnd );
    /*! Initialize the array with \a n normally distributed random numbers 
        with zero mean and unit standard deviation, 
	using the random number generator \a r. */
  template < typename R >
  Array<T> &randNorm( int n, R &r=numerics::rnd );

    /*! Copy the content of the array to \a a. 
        If necessary remaining elements of \a a
        are set to \a val. */
  template < typename S >
  const Array<T> &copy( S *a, int n, const S &val=0 ) const;
    /*! Copy the content of the array to \a a. 
        If necessary remaining elements of \a a
        are set to \a val. */
  const Array<T> &copy( T *a, int n, const T &val=0 ) const;
    /*! Copy the content of the array to \a a. */
  template < typename S >
  const Array<T> &copy( S &a ) const;
    /*! Copy the content of the array to \a a. 
        The size() and capacity() of \a are set
        to the size() of the array. */
  template < typename S >
  const Array<T> &copy( Array<S> &a ) const;

    /*! Append \a \a n - times to the array. */
  const Array< T > &append( T a, int n=1 );
    /*! Append \a a of size \a n to the array. */
  template < typename S >
  const Array<T> &append( const S *a, int n );
    /*! Append \a a of size \a n to the array. */
  const Array<T> &append( const T *a, int n );
    /*! Append \a a to the array. */
  template < typename S >
  const Array<T> &append( const S &a );
    /*! Append \a a to the array. */
  const Array<T> &append( const Array<T> &a );

    /*! The size of the array, 
        i.e. the number of data elements. */
  inline int size( void ) const;
    /*! True if the array does not contain any data elements. */
  inline bool empty( void ) const;
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
        If the capacity() is smaller than \a n
        new memory is allocated with reserve(). */
  virtual void resize( int n, const T &val=0 );
    /*! Resize the array to zero length.
        The capacity() remains unchanged. */
  virtual void clear( void );

    /*! The capacity of the array, i.e. the number of data
        elements for which memory has been allocated. 
        capacity() is always greater than or equal to size(). */
  int capacity( void ) const;
    /*! If \a n is less than or equal to capacity(), 
        this call has no effect. 
	Otherwise, it is a request for allocation 
	of additional memory. 
	If the request is successful, 
	then capacity() is greater than or equal to \a n; 
	otherwise, capacity() is unchanged. 
	In either case, size() is unchanged and the content
	of the array is preserved. */
  virtual void reserve( int n );
    /*! In contrast to the reserve() function, this function
        frees or allocates memory, such that capacity()
	equals exactly \a n.
	If the size() of the array is greater than \a n
	it is set to \a n as well. */
  virtual void free( int n=0 );

    /*! Returns a const reference to the data element at index \a i.
        No range checking is performed. */
  inline const T &operator[]( int i ) const;
    /*! Returns a reference to the data element at index \a i.
        No range checking is performed. */
  inline T &operator[]( int i );
    /*! Returns a const reference to the data element at index \a i.
        If \a i is an invalid index
	a reference to a variable set to zero is returned. */
  const T &at( int i ) const;
    /*! Returns a reference to the data element at index \a i.
        If \a i is an invalid index
	a reference to a variable set to zero is returned. */
  T &at( int i );

    /*! Returns a const reference to the first data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  const T &front( void ) const;
    /*! Returns a reference to the first data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  T &front( void );
    /*! Returns a const reference to the last data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  const T &back( void ) const;
    /*! Returns a reference to the last data element.
        If the array is empty
	a reference to a variable set to zero is returned. */
  T &back( void );

    /*! Add \a val as a new element to the array. */
  inline void push( const T &val );
    /*! Add the content of the container \a x as a new elements to the array. */
  template < typename R >
  void push( const R &x );
    /*! Remove the last element of the array
        and return its value. */
  T pop( void );

    /*! Returns a pointer to the data buffer. */
  inline const T *data( void ) const;
    /*! Returns a pointer to the data buffer. */
  inline T *data( void );

#ifdef HAVE_LIBGSL
    /*! Initializes the gsl_vector \a a such that it points
        to the data buffer of \a b. Ownership remains with \a b. */
  friend void gslVector( gsl_vector &a, const Array<double> &b );
    /*! Initializes the gsl_vector_float \a a such that it points
        to the data buffer of \a b. Ownership remains with \a b. */
  friend void gslVector( gsl_vector_float &a, const Array<float> &b );
    /*! Initializes the gsl_vector_int \a a such that it points
        to the data buffer of \a b. Ownership remains with \a b. */
  friend void gslVector( gsl_vector_int &a, const Array<int> &b );
#endif

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
    /*! Iterator used to iterate through an array. */
  typedef T* iterator;
    /*! Const iterator used to iterate through an array. */
  typedef const T* const_iterator;

    /*! Returns an iterator pointing to the first element of the array. */
  inline iterator begin( void );
    /*! Returns an const_iterator pointing to the first element of the array. */
  inline const_iterator begin( void ) const;
    /*! Returns an iterator pointing behind the last element of the array. */
  inline iterator end( void );
    /*! Returns an const_iterator pointing behind the last element of the array. */
  inline const_iterator end( void ) const;

    /*! Insert element \a v at position \a i. */
  Array<T> &insert( int i, const T &v );
    /*! Insert element \a v at position \a i. */
  iterator insert( iterator i, const T &v );
    /*! Remove element at position \a i from the Array. */
  Array<T> &erase( int i );
    /*! Remove element at position \a i from the Array. */
  iterator erase( iterator i );

    /*! Return in \a indices a list of indices such
        that operator[]( indices[ i ] ) results in
        a sorted list of the arrray elements. */
  Array<T> &sortedIndex( Array< int > &indices, int first=0, int last=-1 );

    /*! True if size and content of array \a a and \a b are equal. */
  template < typename TT >
  friend bool operator==( const Array<TT> &a, const Array<TT> &b );
    /*! True if the value of each data element of array \a a 
        is smaller than \a b and if the size of \a 
	is smaller than the size of \a b. */
  template < typename TT >
  friend bool operator<( const Array<TT> &a, const Array<TT> &b );

    /*! Set the value of each data element to \a val. */
 CONTAINEROPS1SCALARDEC( Array<T>, operator= );
    /*! Add each value of \a x to the corresponding data element. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Array<T>, operator+= );
    /*! Subtract each value of \a x from the corresponding data element. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Array<T>, operator-= );
    /*! Multiply each data element by the corresponding value of \a x. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Array<T>, operator*= );
    /*! Divide each data element by the corresponding value of \a x. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Array<T>, operator/= );
    /*! Return the remainder of the division of each data element 
        by the corresponding value of \a x. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Array<T>, operator%= );

    /*! Return the sum of \a x and \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Array<TT>, operator+ );
    /*! Return the difference between \a x and \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Array<TT>, operator- );
    /*! Return the product of \a x and \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Array<TT>, operator* );
    /*! Return \a x divided by \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Array<TT>, operator/ );
    /*! Return the remainder of \a x divided by \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Array<TT>, operator% );

    /*! Negates each element of the array. */
  Array<T> operator-( void );

    /*! Computes sin(x) of each data element of the array. */
  Array<T> &sin( void );
    /*! Computes cos(x) of each data element of the array. */
  Array<T> &cos( void );
    /*! Computes tan(x) of each data element of the array. */
  Array<T> &tan( void );

    /*! Computes asin(x) of each data element of the array. */
  Array<T> &asin( void );
    /*! Computes acos(x) of each data element of the array. */
  Array<T> &acos( void );
    /*! Computes atan(x) of each data element of the array. */
  Array<T> &atan( void );

    /*! Computes sinh(x) of each data element of the array. */
  Array<T> &sinh( void );
    /*! Computes cosh(x) of each data element of the array. */
  Array<T> &cosh( void );
    /*! Computes tanh(x) of each data element of the array. */
  Array<T> &tanh( void );

    /*! Computes asinh(x) of each data element of the array. */
  Array<T> &asinh( void );
    /*! Computes acosh(x) of each data element of the array. */
  Array<T> &acosh( void );
    /*! Computes atanh(x) of each data element of the array. */
  Array<T> &atanh( void );

    /*! Computes exp(x) of each data element of the array. */
  Array<T> &exp( void );
    /*! Computes log(x) of each data element of the array. */
  Array<T> &log( void );
    /*! Computes log10(x) of each data element of the array. */
  Array<T> &log10( void );

    /*! Computes erf(x) of each data element of the array. */
  Array<T> &erf( void );
    /*! Computes erfc(x) of each data element of the array. */
  Array<T> &erfc( void );

    /*! Computes sqrt(x) of each data element of the array. */
  Array<T> &sqrt( void );
    /*! Computes cbrt(x) of each data element of the array. */
  Array<T> &cbrt( void );

    /*! Squares each data element of the array. */
  Array<T> &square( void );
    /*! Computes cube(x) of each data element of the array. */
  Array<T> &cube( void );

    /*! Raise each data element of the array to the power \a x.
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINERFUNC1DEC( Array<T>, pow );
    /*! For each data element of the array raise \a base to the power of the data element. */
  Array<T> &exp( double base );

    /*! Computes ceil(x) of each data element of the array. */
  Array<T> &ceil( void );
    /*! Computes floor(x) of each data element of the array. */
  Array<T> &floor( void );

    /*! Computes the absolute value of each data element of the array. */
  Array<T> &abs( void );

    /*! Transforms the data elements into decibel relative to \a level
        according to  10.0 * log10( x / level ).
        By default (\a level=0.0),
        \a level is set to the maximum of the data array. */
  Array<T> &decibel( double level=0.0 );
    /*! Transforms the data elements from a decibel scala
        relative to \a level back to a linear scala
	according to pow( 10.0, 0.1 * x ) * level. */
  Array<T> &linear( double level );

    /*! The minimum value of the data elements between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double min( int first=0, int last=-1 ) const;
    /*! The index of the element with the minimum value of the data elements between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  int minIndex( int first=0, int last=-1 ) const;
    /*! The index of the element with the minimum value 
        of the data elements between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
        The value of the minimum element is returned in \a min. */
  int minIndex( double &min, int first=0, int last=-1 ) const;

    /*! The maximum value of the data elements between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double max( int first=0, int last=-1 ) const;
    /*! The index of the element with the maximum value of the data elements between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  int maxIndex( int first=0, int last=-1 ) const;
    /*! The index of the element with the maximum value 
        of the data elements between indices
	\a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	The value of the maximum element is returned in \a max. */
  int maxIndex( double &max, int first=0, int last=-1 ) const;

    /*! The minimum value \a min and maximum value \a max
        of the data elements between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  void minMax( double &min, double &max, int first=0, int last=-1 ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum and the maximum value of the data elements between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  void minMaxIndex( int &minindex, int &maxindex, int first=0, int last=-1 ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum value \a min and the maximum value \a max
        of the data elements between indices
	\a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  void minMaxIndex( double &min, int &minindex, double &max, int &maxindex,
		    int first=0, int last=-1 ) const;

    /*! The mean \<x\> of the data elements between indices 
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double mean( int first=0, int last=-1 ) const;
    /*! The mean \<x\> of the data elements between indices 
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
        In \a stdev the standard deviation is returned. */
  double mean( double &stdev, int first=0, int last=-1 ) const;
    /*! The unbiased variance var(x)=\<(x-\<x\>)^2\> of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double variance( int first=0, int last=-1 ) const;
    /*! The unbiased variance var(x)=\<(x-mean)^2\> of the data elements between
        indices \a first (inclusively) and \a last (exclusively) for known \a mean. 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double variance( double mean, int first=0, int last=-1 ) const;
    /*! The variance var(x)=\<(x-mean)^2\> of the data elements between
        indices \a first (inclusively) and \a last (exclusively) for fixed \a mean. 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double varianceFixed( double fixedmean, int first=0, int last=-1 ) const;
    /*! The unbiased standard deviation sqrt(var(x)) 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double stdev( int first=0, int last=-1 ) const;
    /*! The unbiased standard deviation sqrt(var(x)) 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively) for known \a mean. 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double stdev( double mean, int first=0, int last=-1 ) const;
    /*! The standard deviation sqrt(var(x)) 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively) for fixed \a mean. 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double stdevFixed( double fixedmean, int first=0, int last=-1 ) const;
    /*! The unbiased standard error mean sqrt(var(x)/N) 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double sem( int first=0, int last=-1 ) const;
    /*! The unbiased standard error mean sqrt(var(x)/N) 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively) for known \a mean. 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double sem( double mean, int first=0, int last=-1 ) const;
    /*! The standard deviation sqrt(var(x)/N) 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively) for fixed \a mean. 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double semFixed( double fixedmean, int first=0, int last=-1 ) const;
    /*! The absolute deviation <|x-mu|> 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double absdev( int first=0, int last=-1 ) const;
    /*! The absolute deviation <|x-mu|> 
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively) for known \a mean. 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double absdev( double mean, int first=0, int last=-1 ) const;
    /*! The root-mean-square
        of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double rms( int first=0, int last=-1 ) const;
    /*! The skewness of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double skewness( int first=0, int last=-1 ) const;
    /*! The kurtosis of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double kurtosis( int first=0, int last=-1 ) const;
    /*! The sum of all elements of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double sum( int first=0, int last=-1 ) const;
    /*! The sum of the square of all elements of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double squaredSum( int first=0, int last=-1 ) const;
    /*! The power \<x^2\> of all elements of the data elements between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
  double power( int first=0, int last=-1 ) const;

     /*! Return the convolution of \a x with the container \a y.
         \a y can be shifted by \a offs indices.
         If possible, y.size() should be smaller than x.size(). */
  template < typename TT, typename SS >
  friend Array<TT> convolve( const Array<TT> &x, const SS &y, int offs=0 );

    /*! Replace each element of the sorted array between
        indices \a first (inclusively) and \a last (exclusively)
        by its rank, including midranking of ties,
        and returns the sum of f^3-f, 
	where f is the number of elements in each tie. */
  double rank( int first=0, int last=-1 );

    /*! Write content of the Array into stream \a str.
        Each element is written in a line.
        The array element is formatted in a field of width \a width characters
        and \a precision decimals. */
  ostream &save( ostream &str, int width=8, int prec=3 ) const;
  void save( const string &file, int width=8, int prec=3 ) const;
  template < typename TT > 
  friend ostream &operator<<( ostream &str, const Array<TT> &a );
    /*! Read from stream \a str until end of file
        or a line beginning with \a stop is reached.
        If \a stop equals "EMPTY",
        reading is stopped at an empty line.
        If \a line does not equal zero
        then the last read line is returned in \a line. */
  istream &load( istream &str, const string &stop="", string *line=0 );
  Array<T> &load( const string &file, const string &stop="" );
  template < typename TT >
  friend istream &operator>>( istream &str, Array<TT> &a );


 private:

  T *Buffer;
  int NBuffer;
  int NSize;
  mutable T Dummy;

};


typedef Array< double > ArrayD;
typedef Array< float > ArrayF;
typedef Array< int > ArrayI;


template < typename T > 
Array<T>::Array( void )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
}


template < typename T > 
Array<T>::Array( int n )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
  if ( n > 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
  }
}


template < typename T > 
Array<T>::Array( int n, const T &val )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
  if ( n > 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
    for ( int k=0; k<n; k++ )
      Buffer[k] = val;
  }
}


template < typename T > template < typename S >
Array<T>::Array( const S *a, int n )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
  if ( n > 0 && a != 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
    for ( int k=0; k<n; k++ )
      Buffer[k] = static_cast< T >( a[k] );
  }
}


template < typename T >
Array<T>::Array( const T *a, int n )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
  if ( n > 0 && a != 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
    memcpy( Buffer, a, NSize * sizeof( T ) );
  }
}


template < typename T > template < typename S >
Array<T>::Array( const vector< S > &a, int first, int last )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 || last > a.size() )
    last = a.size();
  int n = last - first;
  if ( n > 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
    for ( unsigned k=0; k < n; k++ )
      Buffer[k] = a[first+k];
  }
}


template < typename T > template < typename S >
Array<T>::Array( const Array< S > &a, int first, int last )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 || last > a.size() )
    last = a.size();
  int n = last - first;
  if ( n > 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
    for ( int k=0; k < n; k++ )
      Buffer[k] = a[first+k];
  }
}


template < typename T > 
Array<T>::Array( const Array< T > &a )
  : Buffer( 0 ),
    NBuffer( 0 ),
    NSize( 0 ),
    Dummy( 0 )
{
  if ( a.size() > 0 ) {
    Buffer = new T[ a.size() ];
    NBuffer = a.size();
    NSize = a.size();
    memcpy( Buffer, a.Buffer, NSize * sizeof( T ) );
  }
}


template < typename T > 
Array<T>::~Array<T>( void )
{
  if ( Buffer != 0 )
    delete [] Buffer;
}


template < typename T > template < typename S >
const Array<T> &Array<T>::operator=( const S &a )
{
  return assign( a );
}


template < typename T >
const Array<T> &Array<T>::operator=( const Array<T> &a )
{
  return assign( a );
}


template < typename T > template < typename S >
const Array<T> &Array<T>::assign( const S *a, int n )
{
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  NBuffer = 0;
  NSize = 0;

  if ( n > 0 && a != 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
    for ( int k=0; k<n; k++ )
      Buffer[k] = static_cast< T >( a[k] );
  }

  return *this;
}


template < typename T > 
const Array<T> &Array<T>::assign( const T *a, int n )
{
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  NBuffer = 0;
  NSize = 0;

  if ( n > 0 && a != 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
    NSize = n;
    memcpy( Buffer, a, NSize * sizeof( T ) );
  }

  return *this;
}


template < typename T >  template < typename S >
const Array<T> &Array<T>::assign( const S &a )
{
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  NBuffer = 0;
  NSize = 0;

  if ( a.size() > 0 ) {
    Buffer = new T[ a.size() ];
    NBuffer = a.size();
    NSize = a.size();
    iterator iter1 = begin();
    iterator end1 = end();
    typename S::const_iterator iter2 = a.begin();
    typename S::const_iterator end2 = a.end();
    while ( iter1 != end1 && iter2 != end2 ) {
      (*iter1) = (*iter2);
      ++iter1;
      ++iter2;
    }
  }

  return *this;
}


template < typename T >
const Array<T> &Array<T>::assign( const Array<T> &a )
{
  if ( &a == this )
    return *this;

  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  NBuffer = 0;
  NSize = 0;

  if ( a.size() > 0 ) {
    Buffer = new T[ a.size() ];
    NBuffer = a.size();
    NSize = a.size();
    memcpy( Buffer, a.data(), NSize * sizeof( T ) );
  }

  return *this;
}


template < typename T >
Array<T> &Array<T>::zeros( int n )
{
  resize( n );
  for ( iterator p = begin(); p != end(); ++p )
    *p = 0;
  return *this;
}


template < typename T >
Array<T> &Array<T>::ones( int n )
{
  resize( n );
  for ( iterator p = begin(); p != end(); ++p )
    *p = 1;
  return *this;
}


template < typename T > template < typename R >
Array<T> &Array<T>::rand( int n, R &r )
{
  resize( n );
  for ( iterator p = begin(); p != end(); ++p )
    *p = r.uniform();
  return *this;
}


template < typename T > template < typename R >
Array<T> &Array<T>::randNorm( int n, R &r )
{
  resize( n );
  for ( iterator p = begin(); p != end(); ++p )
    *p = r.gaussian();
  return *this;
}


template < typename T > template < typename S >
const Array<T> &Array<T>::copy( S *a, int n, const S &val ) const
{
  int k = 0;
  for ( ; k<n && a<NSize; k++ )
    a[k] = Buffer[k];
  while ( k<n )
    a[k++] = val;

  return *this;
}


template < typename T > 
const Array<T> &Array<T>::copy( T *a, int n, const T &val ) const
{
  int minn = NSize < n ? NSize : n;
  memcpy( a, Buffer, minn * sizeof( T ) );
  for ( int k=minn; k<n; k++ )
    a[k] = val;

  return *this;
}


template < typename T > template < typename S > 
const Array<T> &Array<T>::copy( S &a ) const
{
  a.resize( size() );
  const_iterator iter1 = begin();
  const_iterator end1 = end();
  typename S::iterator iter2 = a.begin();
  typename S::iterator end2 = a.end();
  while ( iter1 != end1 && iter2 != end2 ) {
    *iter2 = static_cast< T >( *iter1 );
    ++iter1;
    ++iter2;
  }
  return *this;
}


template < typename T > template < typename S > 
const Array<T> &Array<T>::copy( Array<S> &a ) const
{
  a.assign( *this );
  return *this;
}


template < typename T >
const Array<T> &Array<T>::append( T a, int n )
{
  if ( n > 0 ) {
    reserve( NSize + n );
    for ( int k=0; k<n; k++ )
      Buffer[NSize++] = a;
  }

  return *this;
}


template < typename T > template < typename S > 
const Array<T> &Array<T>::append( const S *a, int n )
{
  if ( n > 0 ) {
    reserve( NSize + n );
    for ( int k=0; k<n; k++ )
      Buffer[NSize++] = static_cast< T >( a[k] );
  }

  return *this;
}


template < typename T > 
const Array<T> &Array<T>::append( const T *a, int n )
{
  if ( n > 0 ) {
    reserve( NSize + n );
    memcpy( Buffer + NSize, a, n * sizeof( T ) );
    NSize += n;
  }

  return *this;
}


template < typename T > template < typename S > 
const Array<T> &Array<T>::append( const S &a )
{
  if ( a.size() > 0 ) {
    reserve( NSize + a.size() );
    typename S::const_iterator iter1 = a.begin();
    typename S::const_iterator end1 = a.end();
    while ( iter1 != end1 ) {
      Buffer[NSize++] = static_cast< T >( *iter1 );
      ++iter1;
    }
  }

  return *this;
}


template < typename T > 
const Array<T> &Array<T>::append( const Array<T> &a )
{
  if ( a.size() > 0 ) {
    reserve( NSize + a.size() );
    memcpy( Buffer + NSize, a.Buffer, a.size() * sizeof( T ) );
    NSize += a.size();
  }

  return *this;
}


template < typename T > 
int Array<T>::size( void ) const
{
  return NSize;
}


template < typename T > 
bool Array<T>::empty( void ) const
{
  return ( NSize <= 0 );
}


template < typename T > 
void Array<T>::resize( int n, const T &val )
{
  reserve( n );
  if ( Buffer != 0 ) {
    for ( int k=NSize; k<n; k++ )
      Buffer[k] = val;
  }
  NSize = n;
}


template < typename T > 
void Array<T>::clear( void )
{
  resize( 0 );
}


template < typename T > 
int Array<T>::capacity( void ) const
{
  return NBuffer;
}


template < typename T > 
void Array<T>::reserve( int n )
{
  if ( n > NBuffer ) {
    T *newbuf = new T[ n ];
    int k=0;
    if ( Buffer != 0 ) {
      for ( k=0; k<NBuffer; k++ )
	newbuf[k] = Buffer[k];
      delete [] Buffer;
    }
    Buffer = newbuf;
    NBuffer = n;
  }
}


template < typename T > 
void Array<T>::free( int n )
{
  if ( n <= 0 ) {
    if ( Buffer != 0 )
      delete [] Buffer;
    Buffer = 0;
    NBuffer = 0;
    NSize = 0;
  }
  else if ( n < NBuffer ) {
    T *newbuf = new T[ n ];
    if ( Buffer != 0 ) {
      for ( int k=0; k<n; k++ )
	newbuf[k] = Buffer[k];
      delete [] Buffer;
    }
    Buffer = newbuf;
    NBuffer = n;
    if ( NSize > n )
      NSize = n;
  }
  else if ( n > NBuffer ) {
    T *newbuf = new T[ n ];
    if ( Buffer != 0 ) {
      for ( int k=0; k<NBuffer; k++ )
	newbuf[k] = Buffer[k];
      delete [] Buffer;
    }
    Buffer = newbuf;
    NBuffer = n;
  }
}


template < typename T > 
const T &Array<T>::operator[]( int i ) const
{
  return Buffer[ i ];
}


template < typename T > 
T &Array<T>::operator[]( int i )
{
  return Buffer[ i ];
}


template < typename T > 
const T &Array<T>::at( int i ) const
{
  if ( Buffer != 0 &&
       i >= 0 && i < NSize ) {
    return Buffer[i];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
T &Array<T>::at( int i )
{
  if ( Buffer != 0 &&
       i >= 0 && i < NSize ) {
    return Buffer[i];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
const T &Array<T>::front( void ) const
{
  if ( Buffer != 0 && NSize > 0 ) {
    return Buffer[0];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
T &Array<T>::front( void )
{
  if ( Buffer != 0 && NSize > 0 ) {
    return Buffer[0];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
const T &Array<T>::back( void ) const
{
  if ( Buffer != 0 && NSize > 0 ) {
    return Buffer[NSize-1];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
T &Array<T>::back( void )
{
  if ( Buffer != 0 && NSize > 0 ) {
    return Buffer[NSize-1];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
void Array<T>::push( const T &val )
{
  resize( NSize+1, val );
}


template < typename T > template < typename R > 
void Array<T>::push( const R &x )
{
  int n = x.end() - x.begin();
  reserve( NSize + n );
  for ( typename R::const_iterator iter = x.begin(); iter != x.end(); ++iter )
    Buffer[NSize++] = *iter;
}


template < typename T > 
T Array<T>::pop( void )
{
  if ( NSize > 0 ) {
    NSize--;
    return Buffer[NSize];
  }
  else
    return 0.0;
}

template < typename T >
const T *Array<T>::data( void ) const
{
  return Buffer;
}


template < typename T >
T *Array<T>::data( void )
{
  return Buffer;
}


template < typename T >
typename Array<T>::iterator Array<T>::begin( void )
{
  return Buffer;
}


template < typename T >
typename Array<T>::const_iterator Array<T>::begin( void ) const
{
  return Buffer;
}


template < typename T >
typename Array<T>::iterator Array<T>::end( void )
{
  return Buffer+NSize;
}


template < typename T >
typename Array<T>::const_iterator Array<T>::end( void ) const
{
  return Buffer+NSize;
}


template < typename T > 
Array<T> &Array<T>::insert( int i, const T &v )
{
  if ( i>=0 && i<=size() ) {
    resize( size()+1 );
    for ( int k=size()-1; k>i; k-- )
      Buffer[k] = Buffer[k-1];
    Buffer[i] = v;
  }

  return *this;
}


template < typename T > 
typename Array<T>::iterator Array<T>::insert( iterator i, const T &v )
{
  int n = i - begin();
  insert( n, v );
  return begin() + n;
}


template < typename T > 
Array<T> &Array<T>::sortedIndex( Array< int > &indices, int first, int last )
{
  const int m = 7;

  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    last = size();
  int ndata = last - first;

  int nstack = 2 * (int)::ceil( ::log( (double)ndata ) / ::log( 2.0 ) );
  if ( nstack < 10 )
    nstack = 10;
  ArrayI stack( nstack );
  int jstack = 0;

  indices.resize( ndata );
  for ( int j=0; j<indices.size(); j++ )
    indices[j] = j;

  int ir = ndata - 1;
  int l = first;
  int i = 0;
  int j = 0;
  for (;;) {
    if ( ir-l < m ) {
      for ( j=l+1; j<=ir; j++ ) {
	int indxt = indices[j];
	int a = indxt;
	for ( i=j-1; i>=l; i-- ) {
	  if ( operator[]( indices[i] ) <= operator[]( a ) )
	    break;
	  indices[i+1] = indices[i];
	}
	indices[i+1] = indxt;
      }
      if ( jstack == 0 )
	break;
      ir = stack[jstack--];
      l = stack[jstack--];
    }
    else {
      int k = (l+ir+2) >> 1;
      swap( indices[k-1], indices[l+1] );
      if ( operator[]( indices[l+1] ) >  operator[]( indices[ir] ) )
	swap( indices[l+1], indices[ir] );
      if ( operator[]( indices[l] ) >  operator[]( indices[ir] ) )
	swap( indices[l], indices[ir] );
      if ( operator[]( indices[l+1] ) >  operator[]( indices[l] ) )
	swap( indices[l+1], indices[l] );
      i = l+1;
      j = ir;
      int indxt = indices[l];
      int a = indxt;
      for (;;) {
	do
	  i++;
	while ( operator[]( indices[i] ) < operator[]( a ) );
	do
	  j--;
	while ( operator[]( indices[j] ) > operator[]( a ) );
	if ( j < i )
	  break;
	swap( indices[i], indices[j] );;
      }
      indices[l] = indices[j];
      indices[j] = indxt;
      jstack += 2;
      if ( jstack > nstack ) {
	// nstack too small
	indices.clear();
	return *this;
      }
      if ( ir-i+1 >= j-l ) {
	stack[jstack] = ir;
	stack[jstack-1] = i;
	ir = j-1;
      }
      else {
	stack[jstack] = j-1;
	stack[jstack-1] = l;
	l = i;
      }
    }
  }
  return *this;
}


template < typename T > 
Array<T> &Array<T>::erase( int i )
{
  if ( i>=0 && i<size() ) {
    resize( size() - 1 );
    for ( ; i<size(); i++ )
      Buffer[i] = Buffer[i+1];
  }

  return *this;
}


template < typename T > 
typename Array<T>::iterator Array<T>::erase( iterator i )
{
  if ( i != end() && size() > 0 ) {
    resize( size() - 1 );
    const_iterator n = i+1;
    for ( const_iterator p = i; p != end(); ++p, ++n )
      *p = *n;
  }
  return i;
}


template < typename TT > 
bool operator==( const Array<TT> &a, const Array<TT> &b )
{
  if ( a.size() != b.size() )
    return false;

  for ( int k=0; k<a.size(); k++ )
    if ( a[ k ] != b[ k ] )
      return false;

  return true;
}


template < typename TT >
bool operator<( const Array<TT> &a, const Array<TT> &b )
{
  for ( int i=0; i<a.size() && i<b.size(); i++ )
    if ( a[ i ] >= b[ i ] )
      return false;

  return ( a.size() <= b.size() );
}


CONTAINEROPS1SCALARDEF( template < typename T >, Array< T >, operator=, = )
CONTAINEROPS1DEF( template < typename T >, Array< T >, operator+=, += )
CONTAINEROPS1DEF( template < typename T >, Array< T >, operator-=, -= )
CONTAINEROPS1DEF( template < typename T >, Array< T >, operator*=, *= )
CONTAINEROPS1DEF( template < typename T >, Array< T >, operator/=, /= )
CONTAINEROPS1DEF( template < typename T >, Array< T >, operator%=, %= )

CONTAINEROPS2DEF( typename TT, Array<TT>, operator+, + )
CONTAINEROPS2DEF( typename TT, Array<TT>, operator-, - )
CONTAINEROPS2DEF( typename TT, Array<TT>, operator*, * )
CONTAINEROPS2DEF( typename TT, Array<TT>, operator/, / )
CONTAINEROPS2DEF( typename TT, Array<TT>, operator%, % )

CONTAINEROPS2DEF( typename TT, vector<TT>, operator+, + )
CONTAINEROPS2DEF( typename TT, vector<TT>, operator-, - )
CONTAINEROPS2DEF( typename TT, vector<TT>, operator*, * )
CONTAINEROPS2DEF( typename TT, vector<TT>, operator/, / )
CONTAINEROPS2DEF( typename TT, vector<TT>, operator%, % )

template < typename T >
Array<T> Array<T>::operator-( void )
{
  Array<T> z( *this );
  iterator iter1 = z.begin();
  iterator end1 = z.end();
  while ( iter1 != end1 ) {
    (*iter1) = -(*iter1);
    ++iter1;
  };
  return z;
}

CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, sin, ::sin )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, cos, ::cos )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, tan, ::tan )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, asin, ::asin )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, acos, ::acos )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, atan, ::atan )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, sinh, ::sinh )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, cosh, ::cosh )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, tanh, ::tanh )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, asinh, ::asinh )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, acosh, ::acosh )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, atanh, ::atanh )

CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, exp, ::exp )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, log, ::log )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, log10, ::log10 )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, erf, ::erf )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, erfc, ::erfc )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, sqrt, ::sqrt )
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, cbrt, ::cbrt )
#define NUMSQUARE( x ) (x)*(x)
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, square, NUMSQUARE )
#undef NUMSQUARE
#define NUMCUBE( x ) (x)*(x)*(x)
CONTAINERFUNC0DEF( template < typename TT >, Array<TT>, cube, NUMCUBE )
#undef NUMCUBE

CONTAINERFUNC1DEF( template < class TT >, Array<TT>, pow, ::pow )


template < class TT >
Array<TT> &Array<TT>::exp( double base )
{
  iterator iter1 = begin();
  iterator end1 = end();
  while ( iter1 != end1 ) {
    (*iter1) = ::exp( base, *iter1 );
    ++iter1;
  };
  return *this;
}


CONTAINERFUNC0DEF( template < class TT >, Array<TT>, ceil, ::ceil )
CONTAINERFUNC0DEF( template < class TT >, Array<TT>, floor, ::floor )
CONTAINERFUNC0DEF( template < class TT >, Array<TT>, abs, ::fabs )


template < typename T >
Array<T> &Array<T>::decibel( double level )
{
  if ( level == 0.0 )
    level = max();

  for ( int k=0; k<NSize; k++ )
    Buffer[k] = 10.0 * ::log10( Buffer[k] / level );

  return *this;
}


template < typename T >
Array<T> &Array<T>::linear( double level )
{
  for ( int k=0; k<NSize; k++ )
    Buffer[k] = ::pow( 10.0, 0.1 * Buffer[k] ) * level;

  return *this;
}


template < typename T >
double Array<T>::min( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::min( begin()+first, end() );
  else
    return numerics::min( begin()+first, begin()+last );
}


template < typename T >
int Array<T>::minIndex( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::minIndex( begin()+first, end() );
  else
    return numerics::minIndex( begin()+first, begin()+last );
}


template < typename T >
int Array<T>::minIndex( double &min, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  int index = -1;
  if ( last < 0 )
    min = numerics::min( index, begin()+first, end() );
  else
    min = numerics::min( index, begin()+first, begin()+last );
  return index;
}


template < typename T >
double Array<T>::max( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::max( begin()+first, end() );
  else
    return numerics::max( begin()+first, begin()+last );
}


template < typename T >
int Array<T>::maxIndex( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::maxIndex( begin()+first, end() );
  else
    return numerics::maxIndex( begin()+first, begin()+last );
}


template < typename T >
int Array<T>::maxIndex( double &max, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  int index = -1;
  if ( last < 0 )
    max = numerics::max( index, begin()+first, end() );
  else
    max = numerics::max( index, begin()+first, begin()+last );
  return index;
}


template < typename T >
void Array<T>::minMax( double &min, double &max, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    numerics::minMax( min, max, begin()+first, end() );
  else
    numerics::minMax( min, max, begin()+first, begin()+last );
}


template < typename T >
void Array<T>::minMaxIndex( int &minindex, int &maxindex, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    numerics::minMaxIndex( minindex, maxindex, begin()+first, end() );
  else
    numerics::minMaxIndex( minindex, maxindex, begin()+first, begin()+last );
}


template < typename T >
void Array<T>::minMaxIndex( double &min, int &minindex, double &max, int &maxindex,
			    int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    numerics::minMax( min, minindex, max, maxindex, begin()+first, end() );
  else
    numerics::minMax( min, minindex, max, maxindex, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::mean( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::mean( begin()+first, end() );
  else
    return numerics::mean( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::mean( double &stdev, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::meanStdev( stdev, begin()+first, end() );
  else
    return numerics::meanStdev( stdev, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::variance( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::variance( begin()+first, end() );
  else
    return numerics::variance( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::variance( double mean, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::variance( mean, begin()+first, end() );
  else
    return numerics::variance( mean, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::varianceFixed( double fixedmean, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::varianceFixed( fixedmean, begin()+first, end() );
  else
    return numerics::varianceFixed( fixedmean, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::stdev( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::stdev( begin()+first, end() );
  else
    return numerics::stdev( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::stdev( double mean, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::stdev( mean, begin()+first, end() );
  else
    return numerics::stdev( mean, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::stdevFixed( double fixedmean, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::stdevFixed( fixedmean, begin()+first, end() );
  else
    return numerics::stdevFixed( fixedmean, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::sem( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::sem( begin()+first, end() );
  else
    return numerics::sem( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::sem( double mean, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::sem( mean, begin()+first, end() );
  else
    return numerics::sem( mean, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::semFixed( double fixedmean, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::semFixed( fixedmean, begin()+first, end() );
  else
    return numerics::semFixed( fixedmean, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::absdev( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::absdev( begin()+first, end() );
  else
    return numerics::absdev( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::absdev( double mean, int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::absdev( mean, begin()+first, end() );
  else
    return numerics::absdev( mean, begin()+first, begin()+last );
}


template < typename T >
double Array<T>::rms( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::rms( begin()+first, end() );
  else
    return numerics::rms( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::skewness( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::skewness( begin()+first, end() );
  else
    return numerics::skewness( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::kurtosis( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::kurtosis( begin()+first, end() );
  else
    return numerics::kurtosis( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::sum( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::sum( begin()+first, end() );
  else
    return numerics::sum( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::squaredSum( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::squaredSum( begin()+first, end() );
  else
    return numerics::squaredSum( begin()+first, begin()+last );
}


template < typename T >
double Array<T>::power( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::power( begin()+first, end() );
  else
    return numerics::power( begin()+first, begin()+last );
}


template < typename T, typename S >
Array<T> convolve( const Array<T> &x, const S &y, int offs )
{
  Array<T> z(x);
  typename Array<T>::iterator iter3 = z.begin();
  typename Array<T>::iterator begin3 = z.begin();
  typename Array<T>::iterator end3 = z.end();
  while ( iter3 != end3 ) {
    (*iter3) = 0.0;
    typename S::const_iterator iter2 = y.begin();
    typename S::const_iterator end2 = y.end();
    typename Array<T>::iterator iter1 = x.begin() + (iter2-begin3) + offs;
    typename Array<T>::iterator begin1 = x.begin();
    typename Array<T>::iterator end1 = x.end();
    int n = iter1 - end1 + 1;
    if ( n > 0 ) {
      iter2 += n;
      iter1 -= n;
    }
    while ( iter2 != end2 && iter1 >= begin1 ) {
      (*iter3) += (*iter1) * (*iter2);
      --iter1;
      ++iter2;
    }
    ++iter3;
  };
  
  return z;
}


template < typename T > 
double Array<T>::rank( int first, int last )
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return numerics::rank( begin()+first, end() );
  else
    return numerics::rank( begin()+first, begin()+last );
}


template < typename T > 
ostream &Array<T>::save( ostream &str, int width, int prec ) const
{
  if ( ! str.good() )
    return str;

  str.setf( ios::fixed, ios::floatfield );
  for ( int k=0; k<size(); k++ ) {
    str << setw( width ) << setprecision( prec ) << (*this)[k] << '\n';
  }
  return str;
}


template < typename T > 
void Array<T>::save( const string &file, int width, int prec ) const
{
  ofstream str( file.c_str() );
  save( str, width, prec );
}


template < typename T > 
ostream &operator<<( ostream &str, const Array<T> &a )
{
  return a.save( str );
}


template < typename T > 
istream &Array<T>::load( istream &str, const string &stop, string *line )
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
  if ( line != 0 && ! line->empty() )
    n++;
  while ( getline( str, s ) ) {

    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
         ( stopempty && int(s.find_first_not_of( " \t\n\r\v\f" )) < 0 ) )
      break;

    // load string:
    char *ep;
    double v = strtod( s.c_str(), &ep );
    if ( ep > s.c_str() )
      n++;
  }

  // allocate memory:
  reserve( n );

  // rewind:
  str.clear();
  str.seekg( pos );

  // read first line:
  if ( line != 0 && !line->empty() ) {
    // load string:
    char *ep;
    double v = strtod( line->c_str(), &ep );
    if ( ep > line->c_str() )
      push( v );
  }

  // read lines:
  while ( getline( str, s ) ) {

    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
         ( stopempty && int(s.find_first_not_of( " \t\n\r\v\f" )) < 0 ) )
      break;

    // load string:
    char *ep;
    double v = strtod( s.c_str(), &ep );
    if ( ep > s.c_str() )
      push( v );
  }

  // store last read line:
  if ( line != 0 )
    *line = s;

  return str;
}


template < typename T > 
Array<T> &Array<T>::load( const string &file, const string &stop )
{
  ifstream str( file.c_str() );
  load( str, stop, 0 );
  return *this;
}


template < typename T > 
istream &operator>>( istream &str, Array<T> &a )
{
  return a.load( str );
}


#endif
