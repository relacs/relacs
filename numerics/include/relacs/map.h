/*
  map.h
  A template handling an x- and an y-data array.

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

#ifndef _RELACS_MAP_H_
#define _RELACS_MAP_H_ 1

#include <cstdlib>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <relacs/containerops.h>
#include <relacs/array.h>
#include <relacs/sampledata.h>
#include <relacs/stats.h>
using namespace std;

namespace relacs {


/*! 
\class Map
\author Jan Benda
\version 0.3
\brief A template handling an x- and an y-data array.


A Map is an Array (the y-data array) which owns
a second array (the x-data array).
*/ 


template < typename T = double > 
class Map : public Array < T >
{

 public:

    /*! Creates an empty map. */
  Map( void );
    /*! Creates a map with \a n data elements 
        without initializing them. */
  Map( int n );
    /*! Creates a map with \a n data elements and 
        initialzes the x-data with \a xval and the y-data with \a yval. */
  Map( int n, const T &xval, const T &yval );
    /*! Creates a map with \a n data elements and
        initialzes them with the values given in \a x and \a y. */
  Map( const T *x, const T *y, int n );
    /*! Creates a map with the same size and content
        as the vectors \a x and \a y. */
  Map( const vector< T > &x, const vector< T > &y );
    /*! Creates a map with the same size and content
        as the Array \a x and \a y. */
  Map( const Array< T > &x, const Array< T > &y );
    /*! Copy constructor. */
  Map( const Map< T > &a );
    /*! The destructor. */
  virtual ~Map( void );

    /*! Set the size() and the capacity() of
        the map to the size of \a y
        and set the content of the y-data array to \a y.
        \note Some values of the x-data array might be undefined! */
  template < typename R >
  const Map<T> &operator=( const R &y );
    /*! Set the size() and the capacity() of
        the map to the size of \a y
        and set the content of the y-data array to \a y.
        \note Some values of the x-data array might be undefined! */
  const Map<T> &operator=( const Array<T> &y );
    /*! Set the size(), capacity(), and content 
        of the map to \a a. */
  const Map<T> &operator=( const Map<T> &a );

    /*! Set the size() and the capacity() of the map to \a n
        and set the content of the y-data array to \a y. 
        If necessary remaining elements of the x-data array
        are set to \a xval. */
  const Map<T> &assign( const T *y, int n, const T &xval=0 );
    /*! Set the size() and the capacity() of the map to \a n
        and its content to \a x and \a y. */
  const Map<T> &assign( const T *x, const T *y, int n );
    /*! Set the size() and the capacity() of the map to the size of \a y
        and set the content of the y-data array to \a y. 
        If necessary remaining elements of the x-data array
        are set to \a xval. */
  const Map<T> &assign( const vector<T> &y, const T &xval=0 );
    /*! Set the size(), capacity(), and content of the map to \a x and \a y. */
  const Map<T> &assign( const vector<T> &x, const vector<T> &y );
    /*! Set the size() and the capacity() of the map to the size of \a y
        and set the content of the y-data array to \a y. 
        If necessary remaining elements of the x-data array
        are set to \a xval. */
  const Map<T> &assign( const Array<T> &y, const T &xval=0 );
    /*! Set the size(), capacity(), and content of the map to \a x and \a y. */
  const Map<T> &assign( const Array<T> &x, const Array<T> &y );
    /*! Set the size(), capacity(), and content of the map to \a a. */
  const Map<T> &assign( const Map<T> &a );

    /*! Copy the content of the y-data array to \a y. 
        If necessary remaining elements of \a y
        are set to \a yval. */
  const Map<T> &copy( T *y, int n, const T &yval=0 );
    /*! Copy the content of the x-data array to \a x and
        of the y-data array to \a y. 
        If necessary remaining elements of \a x
        are set to \a xval and remaining elements of \a y
        are set to \a yval. */
  const Map<T> &copy( T *x, T *y, int n, 
		      const T &xval=0, const T &yval=0 );
    /*! Copy the content of the y-data array to \a y. */
  const Map<T> &copy( vector<T> &y );
    /*! Copy the content of the x-data array to \a x 
        and the content of the y-data array to \a y. */
  const Map<T> &copy( vector<T> &x, vector<T> &y );
    /*! Copy the content of the y-data array to \a y. */
  const Map<T> &copy( Array<T> &y );
    /*! Copy the content of the x-data array to \a x 
        and the content of the y-data array to \a y. */
  const Map<T> &copy( Array<T> &x, Array<T> &y );
    /*! Copy the content of the map to \a a. */
  const Map<T> &copy( Map<T> &a );

    /*! Append \a x and \a y of size \a n to the map's
        x-data array and y-data array, respectively. */
  const Map<T> &append( const T *x, const T *y, int n );
    /*! Append \a x and \a y to the map's
        x-data array and y-data array, respectively. */
  const Map<T> &append( const vector<T> &x, const vector<T> &y );
    /*! Append \a x and \a y to the map's
        x-data array and y-data array, respectively. */
  const Map<T> &append( const Array<T> &x, const Array<T> &y );
    /*! Append \a a to the map. */
  const Map<T> &append( const Map<T> &a );

    /*! The size of the map, 
        i.e. the number of data elements. */
  int size( void ) const { return Array<T>::size(); };
    /*! True if the map does not contain any data elements. */
  bool empty( void ) const { return Array<T>::empty(); };
    /*! Resize the map to \a n data elements
        such that both the size() and the capacity of
	the map equals \a n.
        Data values are preserved. */
  void resize( int n, const T &val=0 );
    /*! Resize the map to \a n data elements
        such that both the size() and the capacity of
	the map equals \a n.
        Data values are preserved. */
  void resize( int n, const T &xval, const T &yval );
    /*! Resize the map to zero length and free all memory. */
  void clear( void );

    /*! The capacity of the map, i.e. the number of data
        pairs for which memory has been allocated. 
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
	of the array is preserved. */
  void reserve( int n );
    /*! In contrast to the reserve() function, this function
        frees or allocates memory, such that capacity()
	equals exactly \a n.
	If the size() of the array is greater than \a n
	it is set to \a n as well. */
  void free( int n=0 );

    /*! Returns a reference to the x-data array. */
  const Array< T > &x( void ) const { return XData; };
    /*! Returns a const reference to the x-data array. */
  Array< T > &x( void ) { return XData; };
    /*! Returns a const reference to the y-data array. */
  const Array< T > &y( void ) const { return *this; };
    /*! Returns a reference to the y-data array. */
  Array< T > &y( void ) { return *this; };

    /*! Returns a reference to the element of the x-data array at index \a i.
        No range checking is performed. */
  const T &x( int i ) const { return XData[ i ]; };
    /*! Returns a reference to the element of the x-data array at index \a i.
        No range checking is performed. */
  T &x( int i ) { return XData[ i ]; };
    /*! Returns a reference to the element of the y-data array at index \a i.
        No range checking is performed. */
  const T &y( int i ) const { return Array<T>::operator[]( i ); };
    /*! Returns a reference to the element of the y-data array at index \a i.
        No range checking is performed. */
  T &y( int i ) { return Array<T>::operator[]( i ); };
    /*! Returns a reference to the element of the y-data array at index \a i.
        No range checking is performed. */
  const T &operator[]( int i ) const { return Array<T>::operator[]( i ); };
    /*! Returns a reference to the element of the y-data array at index \a i.
        No range checking is performed. */
  T &operator[]( int i ) { return Array<T>::operator[]( i ); };
    /*! Returns a reference to the element at index \a j
        of the x-data array if \a i equals zero
        or the y-data array if \a i equals one.
        No range checking is performed. */
  const T &operator()( int i, int j ) const { return (*A[i])[j]; };
    /*! Returns a reference to the element at index \a j
        of the x-data array if \a i equals zero
        or the y-data array if \a i equals one.
        No range checking is performed. */
  T &operator()( int i, int j ) { return (*A[i])[j]; };

    /*! Add the data pair \a xval, \a yval to the map. */
  void push( const T &xval, const T &yval );
    /*! Add the data pairs from the containers \a x and \a y to the map. */
  template < typename R >
  void push( const R &xval, const R &yval );
    /*! Remove the last element of the map. */
  void pop( void );

    /*! The type of object, T, stored in the x- and y-data arrays. */
  typedef T value_type;
    /*! Pointer to the type of object, T, stored in the x- and y-data arrays. */
  typedef T* pointer;
    /*! Reference to the type of object, T, stored in the x- and y-data arrays. */
  typedef T& reference;
    /*! Const reference to the type of object, T, stored in the x- and y-data arrays. */
  typedef const T& const_reference;
    /*! The type used for sizes and indices. */
  typedef int size_type;
    /*! Iterator used to iterate through the y-data array. */
  typedef T* iterator;
    /*! Const iterator used to iterate through the y-data array. */
  typedef const T* const_iterator;

    /*! Returns an iterator pointing to the first element of the y-data array. */
  iterator begin( void ) { return Array<T>::begin(); };
    /*! Returns an const_iterator pointing to the first element of the y-data array. */
  const_iterator begin( void ) const { return Array<T>::begin(); };
    /*! Returns an iterator pointing behind the last element of the y-data array. */
  iterator end( void ) { return Array<T>::end(); };
    /*! Returns an const_iterator pointing behind the last element of the y-data array. */
  const_iterator end( void ) const { return Array<T>::end(); };

    /*! Insert the data pair \a xval, \a yval at position \a i. */
  Map<T> &insert( int i, const T &xval, const T &yval );
    /*! Insert the data pair \a xval, \a yval after the first element
        whose x-value is smaller than \a xval. 
        Returns the index of the inserted data element. */
  int insert( const T &xval, const T &yval );
    /*! Remove data pair at position \a i from the Map. */
  Map<T> &erase( int i );

    /*! True if size and content of map \a a and \a b are equal. */
  template < typename TT > 
    friend bool operator==( const Map<TT> &a, const Map<TT> &b );
    /*! True if the value of each data element and size of map \a a 
        is smaller than \a b. */
  template < typename TT > 
    friend bool operator<( const Map<TT> &a, const Map<TT> &b );

    /*! Set the value of each data element to \a val. */
 CONTAINEROPS1SCALARDEC( Map<T>, operator= );
    /*! Add each value of \a x to the corresponding data element. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Map<T>, operator+= );
    /*! Subtract each value of \a x from the corresponding data element. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Map<T>, operator-= );
    /*! Multiply each data element by the corresponding value of \a x. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Map<T>, operator*= );
    /*! Divide each data element by the corresponding value of \a x. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Map<T>, operator/= );
    /*! Return the remainder of the division of 
        each data element by the corresponding value of \a x. 
        \a x can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
 CONTAINEROPS1DEC( Map<T>, operator%= );

    /*! Return the sum of \a x and \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Map<TT>, operator+ );
    /*! Return the difference between \a x and \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Map<TT>, operator- );
    /*! Return the product of \a x and \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Map<TT>, operator* );
    /*! Return \a x divided by \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Map<TT>, operator/ );
    /*! Return the remainder of \a x divided by \a y computed for each element. 
        \a x and \a y can be either a container or 
	a scalar type like \c float, \c double, \c int, etc. */
  CONTAINEROPS2DEC( class TT, Map<TT>, operator% );

    /*! Negates each element of the y-data array. */
  Map<T> operator-( void );

    /*! Set the value of each y-data element to the value of the corresponding
        x-data element. */
  Map<T> &identity( void );

    /*! The minimum value of the elements of the x-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	In \a y the corresponding value of the y-data array is returned. */
  double minX( double &y, int first=0, int last=-1 ) const;
    /*! The minimum value of the elements of the y-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	In \a x the corresponding value of the x-data array is returned. */
  double minY( double &x, int first=0, int last=-1 ) const;
    /*! The index of the element with the minimum value 
        of the elements of the x-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
        The value of the minimum element is returned in \a min
        and the corresponding value of the y-data array is returned in \a y. */
  int minXIndex( double &min, double &y, int first=0, int last=-1 ) const;
    /*! The index of the element with the minimum value 
        of the elements of the y-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
        The value of the minimum element is returned in \a min
        and the corresponding value of the x-data array is returned in \a x. */
  int minYIndex( double &min, double &x, int first=0, int last=-1 ) const;

    /*! The maximum value of the elements of the x-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	In \a y the corresponding value of the y-data array is returned. */
  double maxX( double &y, int first=0, int last=-1 ) const;
    /*! The maximum value of the elements of the y-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	In \a x the corresponding value of the x-data array is returned. */
  double maxY( double &x, int first=0, int last=-1 ) const;
    /*! The index of the element with the maximum value 
        of the elements of the x-data array between indices
	\a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	The value of the maximum element is returned in \a max
        and the corresponding value of the y-data array is returned in \a y. */
  int maxXIndex( double &max, double &x, int first=0, int last=-1 ) const;
    /*! The index of the element with the maximum value 
        of the elements of the y-data array between indices
	\a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	The value of the maximum element is returned in \a max
        and the corresponding value of the x-data array is returned in \a x. */
  int maxYIndex( double &max, double &x, int first=0, int last=-1 ) const;

    /*! The minimum value \a min and the maximum value \a max
        of the elements of the x-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	In \a miny and \a maxy the corresponding values of the y-data array 
	are returned. */
  void minMaxX( double &min, double &miny, double &max, double &maxy,
		int first=0, int last=-1 ) const;
    /*! The minimum value \a min and the maximum value \a max
        of the elements of the y-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	In \a minx and \a maxx the corresponding values of the x-data array
	are returned. */
  void minMaxY( double &min, double &minx, double &max, double &maxx,
		int first=0, int last=-1 ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum value \a min and maximum value \a max
	of the elements of the x-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
        The corresponding values of the y-data array are returned 
	in \a ymin and \a ymax. */
  void minMaxXIndex( double &min, int &minindex, double &miny,
		     double &max, int &maxindex, double &maxy,
		     int first=0, int last=-1 ) const;
    /*! The indices \a minindex and \a maxindex of the elements
        with the minimum value \a min and the maximum value \a max 
	of the elements of the y-data array between indices
        \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered.
	The corresponding values of the x-data array are returned in \a minx and \a maxx. */
  void minMaxYIndex( double &min, int &minindex, double &minx,
		     double &max, int &maxindex, double &maxx,
		     int first=0, int last=-1 ) const;

    /*! The covariance cov(x,y) = \<(x-\<x\>)(y-\<y\>)\> 
        of the elements of the x-data and y-data array between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
 double cov( int first=0, int last=-1 ) const;
    /*! The linear correlation coefficient 
        r = cov(x,y)/sqrt(var(x)var(y)) (Pearson's r)
	of the elements of the x-data and y-data array between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. */
 double corrCoef( int first=0, int last=-1 ) const;

    /*! Fit line y = m*x to the x-data and y-data array between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
	Returns in \a m the slope, in \a mu the slope's uncertainty,
	and in \a chisq the chi squared. */
 void propFit( int first, int last,
	       double &m, double &mu, double &chisq ) const;
    /*! Fit line y = m*x to the complete x-data and y-data array. 
	Returns in \a m the slope, in \a mu the slope's uncertainty,
	and in \a chisq the chi squared. */
 void propFit( double &m, double &mu, double &chisq ) const { propFit( 0, -1, m, mu, chisq ); };
    /*! Fit line y = m*x to the x-data and y-data array between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
        Called with no arguments all data elements are considered. 
	Returns the slope of the line. */
 double propFit( int first, int last ) const { double m, mu, ch; propFit( first, last, m, mu, ch ); return m; };

    /*! Fit line y = b+m*x to the x-data and y-data array between
        indices \a first (inclusively) and \a last (exclusively). 
        If \a last is negative it is set behind the last data element. 
 	Returns in \a bu and \a mu the uncertainty
	of the offset \a b und slope \a m,
	and in \a chisq the chi squared. */
 void lineFit( int first, int last, double &b, double &bu, 
	       double &m, double &mu, double &chisq ) const;
    /*! Fit line y = b+m*x to the complete x-data and y-data array. 
	Returns in \a bu and \a mu the uncertainty
	of the offset \a b und slope \a m,
	and in \a chisq the chi squared. */
 void lineFit( double &b, double &bu, double &m, double &mu, 
	       double &chisq ) const { lineFit( 0, -1, b, bu, m, mu, chisq ); };
    /*! Fit line y = b+m*x to the complete x-data and y-data array.
	Returns the offset \a b und slope \a m of the line. */
 void lineFit( double &b, double &m ) const { double bu, mu, ch; lineFit( 0, -1, b, bu, m, mu, ch ); };

    /*! Returns in \a meantrace the average over \a traces
        at each position \a x() of \a meantrace.
        The \a traces are linearly interpolated. */
  template < typename TT > 
    friend void average( Map<TT> &meantrace,
			 const vector< Map<TT> > &traces );
    /*! Returns in \a meantrace and \a stdev the average 
        and standard deviation over \a traces
        at each position \a x() of \a meantrace.
        The \a traces are linearly interpolated. */
  template < typename TT > 
    friend void average( Map<TT> &meantrace, Map<TT> &stdev,
			 const vector< Map<TT> > &traces );
    /*! Returns in \a meantrace the average over \a traces
        at each position \a pos() of \a meantrace.
        The \a traces are linearly interpolated. */
  template < typename TT > 
    friend void average( SampleData<TT> &meantrace,
			 const vector< Map<TT> > &traces );
    /*! Returns in \a meantrace and \a stdev the average 
        and standard deviation over \a traces
        at each position \a pos() of \a meantrace.
        The \a traces are linearly interpolated. */
  template < typename TT > 
    friend void average( SampleData<TT> &meantrace, SampleData<TT> &stdev,
			 const vector< Map<TT> > &traces );

    /*! Write content of the Map into stream \a str.
        Each element is written in a line.
        First, the string \a start is written,
        then the value of the x-data array,
        then the string \a separator followed
        by the value of the y-data array element,
        and finally a newline.
        The array elements are formatted in a field of width \a width characters
        and \a precision decimals. */
  ostream &save( ostream &str, int width=8, int prec=3,
		 const string &start="", const string &separator=" " ) const;
  Map<T> &save( const string &file, int width=8, int prec=3 ) const;
  template < typename TT > 
  friend ostream &operator<<( ostream &str, const Map<TT> &a );
    /*! Read from stream \a str until end of file
        or a line beginning with \a stop is reached.
        If \a stop equals "EMPTY",
        reading is stopped at an empty line.
        If \a line does not equal zero
        then the last read line is returned in \a line. */
  istream &load( istream &str, const string &stop="", string *line=0 );
  Map<T> &load( const string &file, const string &stop="" );
  template < typename TT >
  friend istream &operator>>( istream &str, Map<TT> &a );


 private:

  Array < T > XData;
  Array < T > *A[2];

};


typedef Map< double > MapD;
typedef Map< float > MapF;


template < typename T > 
Map<T>::Map( void )
  : Array< T >(),
    XData()
{
  A[0] = &XData;
  A[1] = this;
}


template < typename T > 
Map<T>::Map( int n )
  : Array< T >( n ),
    XData( n )
{
  A[0] = &XData;
  A[1] = this;
}


template < typename T > 
Map<T>::Map( int n, const T &xval, const T &yval )
  : Array< T >( n, yval ),
  XData( n, xval )
{
  A[0] = &XData;
  A[1] = this;
}


template < typename T > 
Map<T>::Map( const T *x, const T *y, int n )
  : Array< T >( y, n ),
  XData( x, n )
{
  A[0] = &XData;
  A[1] = this;
}


template < typename T > 
Map<T>::Map( const vector< T > &x, const vector< T > &y )
  : Array< T >( y ),
    XData( x )
{
  A[0] = &XData;
  A[1] = this;
}


template < typename T > 
Map<T>::Map( const Array< T > &x, const Array< T > &y )
  : Array< T >( y ),
    XData( x )
{
  A[0] = &XData;
  A[1] = this;
}


template < typename T > 
Map<T>::Map( const Map< T > &a )
  : Array< T >( a.y() ),
    XData( a.x() )
{
  A[0] = &XData;
  A[1] = this;
}


template < typename T > 
Map<T>::~Map<T>( void )
{
}


template < typename T > template < typename R > 
const Map<T> &Map<T>::operator=( const R &y )
{
  return assign( y );
}


template < typename T > 
const Map<T> &Map<T>::operator=( const Array<T> &y )
{
  return assign( y );
}


template < typename T > 
const Map<T> &Map<T>::operator=( const Map<T> &a )
{
  return assign( a );
}


template < typename T > 
const Map<T> &Map<T>::assign( const T *y, int n, const T &xval )
{
  XData.resize( n, xval );
  Array< T >::assign( y, n );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::assign( const T *x, const T *y, int n )
{
  XData.assign( x, n );
  Array< T >::assign( y, n );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::assign( const vector<T> &y, const T &xval )
{
  XData.resize( y.size(), xval );
  Array< T >::assign( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::assign( const vector<T> &x, const vector<T> &y )
{
  XData.assign( x );
  Array< T >::assign( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::assign( const Array<T> &y, const T &xval )
{
  XData.resize( y.size(), xval );
  Array< T >::assign( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::assign( const Array<T> &x, const Array<T> &y )
{
  XData.assign( x );
  Array< T >::assign( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::assign( const Map<T> &a )
{
  if ( &a == this )
    return *this;

  XData.assign( a.x() );
  Array< T >::assign( a.y() );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::copy( T *y, int n, const T &yval )
{
  Array<T>::copy( y, n, yval );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::copy( T *x, T *y, int n, 
			    const T &xval, const T &yval )
{
  XData.copy( x, n, xval );
  Array<T>::copy( y, n, yval );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::copy( vector<T> &y )
{
  Array<T>::copy( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::copy( vector<T> &x, vector<T> &y )
{
  XData.copy( x );
  Array<T>::copy( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::copy( Array<T> &y )
{
  Array<T>::copy( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::copy( Array<T> &x, Array<T> &y )
{
  XData.copy( x );
  Array<T>::copy( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::copy( Map<T> &a )
{
  if ( &a == this )
    return *this;

  a.assign( *this );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::append( const T *x, const T *y, int n )
{
  XData.append( x, n );
  Array<T>::append( y, n );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::append( const vector<T> &x, const vector<T> &y )
{
  XData.append( x );
  Array<T>::append( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::append( const Array<T> &x, const Array<T> &y )
{
  XData.append( x );
  Array<T>::append( y );

  return *this;
}


template < typename T > 
const Map<T> &Map<T>::append( const Map<T> &a )
{
  XData.append( a.x() );
  Array<T>::append( a.y() );

  return *this;
}


template < typename T > 
void Map<T>::resize( int n, const T &val )
{
  XData.resize( n, val );
  Array<T>::resize( n, val );
}


template < typename T > 
void Map<T>::resize( int n, const T &xval, const T &yval )
{
  XData.resize( n, xval );
  Array<T>::resize( n, yval );
}


template < typename T > 
void Map<T>::clear( void )
{
  XData.clear();
  Array<T>::clear();
}


template < typename T > 
void Map<T>::reserve( int n )
{
  XData.reserve( n );
  Array<T>::reserve( n );
}


template < typename T > 
void Map<T>::free( int n )
{
  XData.free( n );
  Array<T>::free( n );
}


template < typename T > 
void Map<T>::push( const T &xval, const T &yval )
{
  XData.push( xval );
  Array<T>::push( yval );
}


template < typename T > template < typename R > 
void Map<T>::push( const R &x, const R &y )
{
  XData.push( x );
  Array<T>::push( y );
}


template < typename T > 
void Map<T>::pop( void )
{
  XData.pop();
  Array<T>::pop();
}


template < typename T > 
Map<T> &Map<T>::insert( int i, const T &xval, const T &yval )
{
  if ( i>=0 && i<=size() ) {
    resize( size()+1 );
    for ( int k=size()-1; k>i; k-- ) {
      x(k) = x(k-1);
      y(k) = y(k-1);
    }
    x(i) = xval;
    y(i) = yval;
  }

  return *this;
}


template < typename T > 
int Map<T>::insert( const T &xval, const T &yval )
{
  // find position:
  int i = 0;
  for ( i=0; i<size() && x(i) <= xval; i++ );

  // insert:
  resize( size()+1 );
  for ( int k=size()-1; k>i; k-- ) {
    x(k) = x(k-1);
    y(k) = y(k-1);
  }
  x(i) = xval;
  y(i) = yval;

  return i;
}


template < typename T > 
Map<T> &Map<T>::erase( int i )
{
  if ( i>=0 && i<size() ) {
    resize( size()-1 );
    for ( ; i<size(); i++ ) {
      x(i) = x(i+1);
      y(i) = y(i+1);
    }
  }

  return *this;
}


template < typename TT > 
bool operator==( const Map<TT> &a, const Map<TT> &b )
{
  return ( a.XData == b.XData && Array<TT>::operator==( a, b ) );
}


template < typename TT >
bool operator<( const Map<TT> &a, const Map<TT> &b )
{
  return ( a.XData < b.XData && Array<TT>::operator<( a, b ) );
}


CONTAINEROPS1SCALARDEF( template < typename T >, Map<T>, operator=, = )
CONTAINEROPS1DEF( template < typename T >, Map<T>, operator+=, += )
CONTAINEROPS1DEF( template < typename T >, Map<T>, operator-=, -= )
CONTAINEROPS1DEF( template < typename T >, Map<T>, operator*=, *= )
CONTAINEROPS1DEF( template < typename T >, Map<T>, operator/=, /= )
CONTAINEROPS1DEF( template < typename T >, Map<T>, operator%=, %= )

CONTAINEROPS2DEF( typename TT, Map<TT>, operator+, + )
CONTAINEROPS2DEF( typename TT, Map<TT>, operator-, - )
CONTAINEROPS2DEF( typename TT, Map<TT>, operator*, * )
CONTAINEROPS2DEF( typename TT, Map<TT>, operator/, / )
CONTAINEROPS2DEF( typename TT, Map<TT>, operator%, % )


template < typename T >
Map<T> Map<T>::operator-( void )
{
  Map<T> z( *this );
  iterator iter1 = z.y().begin();
  iterator end1 = z.y().end();
  while ( iter1 != end1 ) {
    (*iter1) = -(*iter1);
    ++iter1;
  };
  return z;
}


template < typename T > 
Map<T> &Map<T>::identity( void )
{
  for ( int k=0; k<size(); k++ )
    y(k) = x(k);
  return *this;
}


template < typename T > 
double Map<T>::minX( double &yy, int first, int last ) const
{
  int index = -1;
  double min = XData.min( index, first, last );
  yy = y( index );
  return min;
}


template < typename T > 
double Map<T>::minY( double &xx, int first, int last ) const
{
  int index = -1;
  double min = Array<T>::min( index, first, last );
  xx = x( index );
  return min;
}


template < typename T > 
double Map<T>::maxX( double &yy, int first, int last ) const
{
  int index = -1;
  double max = XData.max( index, first, last );
  yy = y( index );
  return max;
}


template < typename T > 
double Map<T>::maxY( double &xx, int first, int last ) const
{
  int index = -1;
  double max = Array<T>::max( index, first, last );
  xx = x( index );
  return max;
}


template < typename T > 
int Map<T>::minXIndex( double &min, double &yy, int first, int last ) const
{
  int index = -1;
  min = XData.min( index, first, last );
  yy = y( index );
  return index;
}


template < typename T > 
int Map<T>::minYIndex( double &min, double &xx, int first, int last ) const
{
  int index = -1;
  min = Array<T>::min( index, first, last );
  xx = x( index );
  return index;
}


template < typename T > 
int Map<T>::maxXIndex( double &max, double &yy, int first, int last ) const
{
  int index = -1;
  max = XData.max( index, first, last );
  yy = y( index );
  return index;
}


template < typename T > 
int Map<T>::maxYIndex( double &max, double &xx, int first, int last ) const
{
  int index = -1;
  max = Array<T>::max( index, first, last );
  xx = x( index );
  return index;
}


template < typename T > 
void Map<T>::minMaxX( double &min, double &miny, double &max, double &maxy,
		      int first, int last ) const
{
  int minindex = -1;
  int maxindex = -1;
  XData.minMaxIndex( min, minindex, max, maxindex, first, last );
  miny = y( minindex );
  maxy = y( maxindex );
}


template < typename T > 
void Map<T>::minMaxY( double &min, double &minx, double &max, double &maxx,
		      int first, int last ) const
{
  int minindex = -1;
  int maxindex = -1;
  Array<T>::minMaxIndex( min, minindex, max, maxindex, first, last );
  minx = x( minindex );
  maxx = x( maxindex );
}
		    

template < typename T > 
void Map<T>::minMaxXIndex( double &min, int &minindex, double &miny,
			   double &max, int &maxindex, double &maxy,
			   int first, int last ) const
{
  XData.minMaxIndex( min, minindex, max, maxindex, first, last );
  miny = y( minindex );
  maxy = y( maxindex );
}


template < typename T > 
void Map<T>::minMaxYIndex( double &min, int &minindex, double &minx,
			   double &max, int &maxindex, double &maxx,
			   int first, int last ) const
{
  Array<T>::minMaxIndex( min, minindex, max, maxindex, first, last );
  minx = x( minindex );
  maxx = x( maxindex );
}


template < typename T >
double Map<T>::cov( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return ::relacs::cov( XData.begin()+first, XData.end(), 
			  begin()+first, end() );
  else
    return ::relacs::cov( XData.begin()+first, XData.begin()+last, 
			  begin()+first, begin()+last );
}


template < typename T >
double Map<T>::corrCoef( int first, int last ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    return ::relacs::corrCoef( XData.begin()+first, XData.end(), 
			       begin()+first, end() );
  else
    return ::relacs::corrCoef( XData.begin()+first, XData.begin()+last, 
			       begin()+first, begin()+last );
}


template < typename T > 
void Map<T>::propFit( int first, int last,
		      double &m, double &mu, double &chisq ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    ::relacs::propFit( XData.begin()+first, XData.end(), 
		       begin()+first, end(),
		       m, mu, chisq );
  else
    ::relacs::propFit( XData.begin()+first, XData.begin()+last, 
		       begin()+first, begin()+last,
		       m, mu, chisq );
}


template < typename T > 
void Map<T>::lineFit( int first, int last, double &b, double &bu,
		      double &m, double &mu, double &chisq ) const
{
  if ( first < 0 )
    first = 0;
  if ( last < 0 )
    ::relacs::lineFit( XData.begin()+first, XData.end(), 
		       begin()+first, end(),
		       b, bu, m, mu, chisq );
  else
    ::relacs::lineFit( XData.begin()+first, XData.begin()+last, 
		       begin()+first, begin()+last,
		       b, bu, m, mu, chisq );
}


template < typename TT > 
void average( Map<TT> &meantrace,
	      const vector< Map<TT> > &traces )
{
  meantrace = 0.0;
  Array< int > inx( traces.size(), 0 );
  for ( int k=0; k<meantrace.size(); k++ ) {
    for ( unsigned int j = 0; j<traces.size(); j++ ) {
      double time = meantrace.x( k );
      for ( ; inx[j] < traces[j].size() && traces[j].x(inx[j]) < time; ++inx[j] );
      double tr = 0.0;
      if ( inx[j] >= traces[j].size() )
	tr = traces[j].y().back();
      else if ( inx[j] == 0 )
	tr = traces[j].y().front();
      else {
	double slope = ( traces[j].y(inx[j]) - traces[j].y(inx[j]-1) ) / ( traces[j].x(inx[j]) - traces[j].x(inx[j]-1) );
	tr = slope*( time - traces[j].x(inx[j]) ) + traces[j].y(inx[j]);
      }
      meantrace[k] += ( tr - meantrace[k] )/(j+1);
    }
  }
}


template < typename TT > 
void average( Map<TT> &meantrace, Map<TT> &stdev,
	      const vector< Map<TT> > &traces )
{
  meantrace = 0.0;
  stdev = meantrace;
  Array< int > inx( traces.size(), 0 );
  Array< TT > tr( traces.size() );
  for ( int k=0; k<meantrace.size(); k++ ) {
    for ( unsigned int j = 0; j<traces.size(); j++ ) {
      double time = meantrace.x( k );
      for ( ; inx[j] < traces[j].size() && traces[j].x(inx[j]) < time; ++inx[j] );
      if ( inx[j] >= traces[j].size() )
	tr[j] = traces[j].y().back();
      else if ( inx[j] == 0 )
	tr[j] = traces[j].y().front();
      else {
	double slope = ( traces[j].y(inx[j]) - traces[j].y(inx[j]-1) ) / ( traces[j].x(inx[j]) - traces[j].x(inx[j]-1) );
	tr[j] = slope*( time - traces[j].x(inx[j]) ) + traces[j].y(inx[j]);
      }
    }
    meantrace[k] = tr.mean( stdev[k] );
  }
}


template < typename TT > 
void average( SampleData<TT> &meantrace,
	      const vector< Map<TT> > &traces )
{
  meantrace = 0.0;
  Array< int > inx( traces.size(), 0 );
  for ( int k=0; k<meantrace.size(); k++ ) {
    for ( unsigned int j = 0; j<traces.size(); j++ ) {
      double time = meantrace.pos( k );
      for ( ; inx[j] < traces[j].size() && traces[j].x(inx[j]) < time; ++inx[j] );
      double tr = 0.0;
      if ( inx[j] >= traces[j].size() )
	tr = traces[j].y().back();
      else if ( inx[j] == 0 )
	tr = traces[j].y().front();
      else {
	double slope = ( traces[j].y(inx[j]) - traces[j].y(inx[j]-1) ) / ( traces[j].x(inx[j]) - traces[j].x(inx[j]-1) );
	tr = slope*( time - traces[j].x(inx[j]) ) + traces[j].y(inx[j]);
      }
      meantrace[k] += ( tr - meantrace[k] )/(j+1);
    }
  }
}


template < typename TT > 
void average( SampleData<TT> &meantrace, SampleData<TT> &stdev,
	      const vector< Map<TT> > &traces )
{
  meantrace = 0.0;
  stdev = meantrace;
  Array< int > inx( traces.size(), 0 );
  Array< TT > tr( traces.size() );
  for ( int k=0; k<meantrace.size(); k++ ) {
    for ( unsigned int j = 0; j<traces.size(); j++ ) {
      double time = meantrace.pos( k );
      for ( ; inx[j] < traces[j].size() && traces[j].x(inx[j]) < time; ++inx[j] );
      if ( inx[j] >= traces[j].size() )
	tr[j] = traces[j].y().back();
      else if ( inx[j] == 0 )
	tr[j] = traces[j].y().front();
      else {
	double slope = ( traces[j].y(inx[j]) - traces[j].y(inx[j]-1) ) / ( traces[j].x(inx[j]) - traces[j].x(inx[j]-1) );
	tr[j] = slope*( time - traces[j].x(inx[j]) ) + traces[j].y(inx[j]);
      }
    }
    meantrace[k] = tr.mean( stdev[k] );
  }
}


template < typename T > 
ostream &Map<T>::save( ostream &str, int width, int prec, 
		       const string &start, const string &separator ) const
{
  if ( ! str.good() )
    return str;

  str.setf( ios::fixed, ios::floatfield );
  for ( int k=0; k<size(); k++ ) {
    str << start 
	<< setw( width ) << setprecision( prec ) << x(k)
        << separator
	<< setw( width ) << setprecision( prec ) << y(k) << '\n';
  }
  return str;
}


template < typename T > 
Map<T> &Map<T>::save( const string &file, int width, int prec ) const
{
  ofstream str( file.c_str() );
  save( str, width, prec );
  return *this;
}


template < typename T > 
ostream &operator<<( ostream &str, const Map<T> &a )
{
  return a.save( str );
}


template < typename T > 
istream &Map<T>::load( istream &str, const string &stop, string *line )
{
  clear();

  bool stopempty = ( stop == "EMPTY" );

  // stream not opened:
  if ( !str )
    return str;

  // current file position:
  long pos = str.tellg();

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
    char *fp = s.c_str();
    char *ep;
    double x = strtod( fp, &ep );
    if ( ep > fp ) {
      fp = ep;
      double y = strtod( fp, &ep );
      if ( ep > fp )
	n++;
    }
  }

  // allocate memory:
  reserve( n );

  // rewind:
  str.seekg( pos );

  // read first line:
  if ( line != 0 && !line->empty() ) {
    // load string:
    char *fp = s.c_str();
    char *ep;
    double x = strtod( fp, &ep );
    if ( ep > fp ) {
      fp = ep;
      double y = strtod( fp, &ep );
      if ( ep > fp )
	push( x, y );
    }
  }

  // read lines:
  while ( getline( str, s ) ) {

    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
         ( stopempty && int(s.find_first_not_of( " \t\n\r\v\f" )) < 0 ) )
      break;

    // load string:
    char *fp = s.c_str();
    char *ep;
    double x = strtod( fp, &ep );
    if ( ep > fp ) {
      fp = ep;
      double y = strtod( fp, &ep );
      if ( ep > fp )
	push( x, y );
    }
  }

  // store last read line:
  if ( line != 0 )
    *line = s;

  return str;
}


template < typename T > 
Map<T> &Map<T>::load( const string &file, const string &stop )
{
  ifstream str( file.c_str() );
  load( str, stop, 0 );
  return *this;
}


template < typename T > 
istream &operator>>( istream &str, Map<T> &a )
{
  return a.load( str );
}


}; /* namespace relacs */

#endif /* ! _RELACS_MAP_H_ */
