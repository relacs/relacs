/*
  cyclicsampledata.h
  A template defining an one-dimensional cyclic array of eveny sampled data.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_CYCLICSAMPLEDATA_H_
#define _RELACS_CYCLICSAMPLEDATA_H_ 1

#include <relacs/cyclicarray.h>
#include <relacs/linearrange.h>
#include <relacs/stats.h>

namespace relacs {


/*!
\class CyclicSampleData
\brief A template defining an one-dimensional cyclic array of periodically sampled data.
\author Jan Benda

This class is very similar to SampleData, in that it is a
random access container of objects of type \a T
that were sampled with stepsize().
The size() of CyclicSampleData, however, can exceed its capacity().
Data elements below size()-capacity() are therefore not accessible.
*/

template < typename T = double >
  class CyclicSampleData : public CyclicArray<T>
{

  public:

    /*! Creates an empty CyclicSampleData with offset() at zero and unit stepsize(). */
  CyclicSampleData( void );
    /*! Creates an empty CyclicSampleData with capacity() \a n data elements,
        zero offset(), and stepsize() set to \a stepsize. */
  CyclicSampleData( int n, double stepsize=1.0 );
    /*! Creates an empty CyclicSampleData with capacity() \a n data elements,
        offset() set to \a offset, and stepsize() set to \a stepsize. */
  CyclicSampleData( int n, double offset, double stepsize );
    /*! Creates an empty CyclicSampleData with its capacity() set to
        \a duration / \a stepsize data elements,
        zero offset(), and stepsize() set to \a stepsize. */
  CyclicSampleData( double duration, double stepsize=1.0 );
    /*! Creates an empty CyclicSampleData with its capacity() set to
        \a duration / \a stepsize data elements,
        offset() set to \a offset, and stepsize() set to \a stepsize. */
  CyclicSampleData( double duration, double offset, double stepsize );
    /*! Creates a CyclicSampleData with the same size, content, and range as \a csd
        that shares the buffer with the one of \a csd. */
  CyclicSampleData( CyclicSampleData<T> *csd );
    /*! Copy constructor.
        Creates a CyclicSampleData with the same size, content, and range as \a csd. */
  CyclicSampleData( const CyclicSampleData<T> &csd );
    /*! The destructor. */
  ~CyclicSampleData( void );

    /*! Assigns \a a to *this by copying the content. */
  const CyclicSampleData<T> &operator=( const CyclicSampleData<T> &a );
    /*! Assigns \a a to *this by copying the content. */
  const CyclicSampleData<T> &assign( const CyclicSampleData<T> &a );
    /*! Assigns \a a to *this by only copying a pointer to the data. */
  const CyclicSampleData<T> &assign( const CyclicSampleData<T> *a );

    /*! The index of the first accessible data element.
        \sa minPos(), accessibleSize() */
  virtual int minIndex( void ) const;
    /*! The position of the first accessible data element.
        \sa minIndex(), accessibleSize() */
  virtual double minPos( void ) const;

    /*! Resize the CyclicSampleData to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
	The capacity is not changed!
        If, however, the capacity() of the CyclicSampleData is zero,
        then memory for \a n data elements is allocated
        and initialized with \a val. */
  virtual void resize( int n, const T &val=0 );
    /*! Resize the CyclicSampleData to \a duration / stepsize() data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
	The capacity is not changed!
        If, however, the capacity() of the CyclicSampleData is zero,
        then memory for \a n data elements is allocated
        and initialized with \a val. */
  virtual void resize( double duration, const T &val=0 );

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
    /*! If \a duration / stepsize() is less than or equal to capacity(), 
        this call has no effect. 
	Otherwise, it is a request for allocation 
	of additional memory. 
	If the request is successful, 
	then capacity() is greater than or equal to \a duration / stepsize(); 
	otherwise, capacity() is unchanged. 
	In either case, size() is unchanged and the content
	of the array is preserved. */
  virtual void reserve( double duration );

     /*! The offset of the range. */
  double offset( void ) const;
     /*! Set the offset of the range to \a offset. */
  void setOffset( double offset );
     /*! The stepsize of the range. */
  double stepsize( void ) const;
     /*! Set the stepsize of the range to \a stepsize. */
  void setStepsize( double stepsize );
     /*! Set the offset and the stepsize of the range to \a offset and
         \a stepsize, respectively. */
  void setRange( const T &offset, const T &stepsize );
     /*! The length of the range, i.e. abs( stepsize() * size() ) */
  double length( void ) const;
    /*! Returns the first range element, i.e. the offset. \sa offset() */
  double rangeFront( void ) const;
    /*! Returns the last range element. */
  double rangeBack( void ) const;

     /*! Add \a val to the offset of the range,
         i.e. shift the range by \a val. */
  void shift( double val );
    /*! Multiply the offset and the stepsize of the range with \a scale, 
        i.e. rescale the range by \a scale. */
  void scale( double scale );

    /*! Returns the range element at index \a i. */
  double pos( int i ) const;
    /*! Returns the interval covered by \a indices indices. */
  double interval( int indices ) const;

    /*! The index of the range corresponding to \a pos. */
  int index( double pos ) const;
    /*! The number of indices corresponding to an interval \a iv. */
  int indices( double iv ) const;
    /*! True if \a pos is within the range. */
  bool contains( double p ) const;

    /*! Returns a reference to the data array. */
  const CyclicArray<T> &array( void ) const;
    /*! Returns a reference to the data array. */
  CyclicArray<T> &array( void );

    /*! Returns a copy of the range. */
  LinearRange range( void );

    /*! The type of an element of the range. */
  typedef double range_type;
    /*! Const iterator used to iterate through the range. */
  typedef LinearRangeIterator const_range_iterator;

    /*! Return the minimum value of the data between index \a from inclusively
        and index \a upto exclusively. */
  T min( int from, int upto ) const;
    /*! Return the minimum value of the data during \a duration seconds
        starting at time \a time seconds. */
  T min( double from, double upto ) const;
    /*! Return the minimum value of the data since time \a time seconds. */
  T min( double from ) const;
    /*! Return the maximum value of the data between index \a from inclusively
        and index \a upto exclusively. */
  T max( int from, int upto ) const;
    /*! Return the maximum value of the data during \a duration seconds
        starting at time \a time seconds. */
  T max( double from, double upto ) const;
    /*! Return the maximum value of the data since time \a time seconds. */
  T max( double from ) const;
    /*! Return the minimum and maximum value, \a min and \a max, of
        the data between index \a from inclusively and index \a upto
        exclusively. */
  void minMax( T &min, T &max, int from, int upto ) const;
    /*! Return the minimum and maximum value, \a min and \a max, of
        the data during \a duration seconds starting at time \a time
        seconds. */
  void minMax( T &min, T &max, double from, double upto ) const;
    /*! Return the minimum and maximum value, \a min and \a max, of
        the data since time \a time seconds. */
  void minMax( T &min, T &max, double from ) const;
    /*! Return the minimum absolute value of the data between index \a from inclusively
        and index \a upto exclusively. */
  T minAbs( int from, int upto ) const;
    /*! Return the minimum absolute value of the data during \a duration seconds
        starting at time \a time seconds. */
  T minAbs( double from, double upto ) const;
    /*! Return the minimum absolute value of the data since time \a time seconds. */
  T minAbs( double from ) const;
    /*! Return the maximum absolute value of the data between index \a from inclusively
        and index \a upto exclusively. */
  T maxAbs( int from, int upto ) const;
    /*! Return the maximum absolute value of the data during \a duration seconds
        starting at time \a time seconds. */
  T maxAbs( double from, double upto ) const;
    /*! Return the maximum absolute value of the data since time \a time seconds. */
  T maxAbs( double from ) const;

    /*! Return the mean value of the data between index \a from inclusively
        and index \a upto exclusively. */
  typename numerical_traits<T>::mean_type
  mean( int from, int upto ) const;
    /*! Return the mean value of the data during times
        \a from and \a upto. */
  typename numerical_traits<T>::mean_type
  mean( double from, double upto ) const;
    /*! Returns in \a d the mean values of the data calculated during
        \a width long time windows starting at the times
        \a time + \a d.pos(i) (moving average).
        If \a width equals zero it is set to the stepsize defined by \a d. */
  template < typename R >
  void mean( double time, SampleData<R> &d, double width=0.0 ) const;

    /*! Return the variance of the data between index \a from inclusively
        and index \a upto exclusively. */
  typename numerical_traits<T>::variance_type
  variance( int from, int upto ) const;
    /*! Return the variance of the data during times
        \a from and \a upto. */
  typename numerical_traits<T>::variance_type
  variance( double from, double upto ) const;
    /*! Returns in \a d the variance calculated during
        \a width long time windows starting at the times
        \a time + \a d.pos(i).
        If \a width equals zero it is set to the stepsize defined by \a d. */
  template < typename R >
  void variance( double time, SampleData<R> &d, double width=0.0 ) const;

    /*! Return the standard deviation of the data between index \a from inclusively
        and index \a upto exclusively. */
  typename numerical_traits<T>::variance_type
  stdev( int from, int upto ) const;
    /*! Return the standard deviation of the data during times
        \a from and \a upto. */
  typename numerical_traits<T>::variance_type
  stdev( double from, double upto ) const;
    /*! Returns in \a d the standard deviation calculated during
        \a width long time windows starting at the times
        \a time + \a d.pos(i).
        If \a width equals zero it is set to the stepsize defined by \a d. */
  template < typename R >
  void stdev( double time, SampleData<R> &d, double width=0.0 ) const;

    /*! Return the root-mean-square of the data between index \a from inclusively
        and index \a upto exclusively. */
  typename numerical_traits<T>::variance_type
  rms( int from, int upto ) const;
    /*! Return the root-mean-square of the data during times
        \a from and \a upto. */
  typename numerical_traits<T>::variance_type
  rms( double from, double upto ) const;
    /*! Returns in \a d the root-mean-square calculated during
        \a width long time windows starting at the times
        \a time + \a d.pos(i).
        If \a width equals zero it is set to the stepsize defined by \a d. */
  template < typename R >
  void rms( double time, SampleData<R> &d, double width=0.0 ) const;

    /*! Compute histogram \a h of all data elements between index \a
        from inclusively and index \a upto exclusively. */
  template< typename S >
  void hist( SampleData< S > &h, int from, int upto ) const;
    /*! Compute histogram \a h of all data elements during times
        \a from and \a upto. */
  template< typename S >
  void hist( SampleData< S > &h, double from, double upto ) const;
    /*! Compute histogram \a h of all data elements currently stored
        in the array. */
  template< typename S >
  void hist( SampleData< S > &h ) const;

  template < typename S > 
  friend ostream &operator<<( ostream &str, const CyclicSampleData<S> &a );

protected:

  double Offset;
  double Stepsize;

};


typedef CyclicSampleData< double > CyclicSampleDataD;
typedef CyclicSampleData< float > CyclicSampleDataF;
typedef CyclicSampleData< int > CyclicSampleDataI;


template < typename T >
CyclicSampleData<T>::CyclicSampleData( void )
  : CyclicArray<T>(),
    Offset( 0 ),
    Stepsize( 1 )
{
}


template < typename T >
CyclicSampleData<T>::CyclicSampleData( int n, double stepsize )
  : CyclicArray<T>( n ),
    Offset( 0 ),
    Stepsize( stepsize )
{
}


template < typename T >
CyclicSampleData<T>::CyclicSampleData( int n, double offset, double stepsize )
  : CyclicArray<T>( n ),
    Offset( offset ),
    Stepsize( stepsize )
{
}


template < typename T >
CyclicSampleData<T>::CyclicSampleData( double duration, double stepsize )
  : CyclicArray<T>( (int)::ceil( duration/stepsize ) ),
    Offset( 0 ),
    Stepsize( stepsize )
{
}


template < typename T >
CyclicSampleData<T>::CyclicSampleData( double duration, double offset, double stepsize )
  : CyclicArray<T>( (int)::ceil( duration/stepsize ) ),
    Offset( offset ),
    Stepsize( stepsize )
{
}


template < typename T >
CyclicSampleData<T>::CyclicSampleData( CyclicSampleData<T> *csd )
  : CyclicArray<T>( csd ),
    Offset( csd->Offset ),
    Stepsize( csd->Stepsize )
{
}


template < typename T >
CyclicSampleData<T>::CyclicSampleData( const CyclicSampleData<T> &csd )
  : CyclicArray<T>( csd ),
    Offset( csd.Offset ),
    Stepsize( csd.Stepsize )
{
}


template < typename T >
CyclicSampleData<T>::~CyclicSampleData( void )
{
}


template < typename T >
const CyclicSampleData<T> &CyclicSampleData<T>::operator=( const CyclicSampleData<T> &a )
{
  return assign( a );
}


template < typename T >
const CyclicSampleData<T> &CyclicSampleData<T>::assign( const CyclicSampleData<T> &a )
{
  if ( &a == this )
    return *this;

  CyclicArray<T>::assign( a );
  Offset = a.Offset;
  Stepsize = a.Stepsize;

  return *this;
}


template < typename T >
const CyclicSampleData<T> &CyclicSampleData<T>::assign( const CyclicSampleData<T> *a )
{
  if ( a == this )
    return *this;

  CyclicArray<T>::assign( a );
  Offset = a->Offset;
  Stepsize = a->Stepsize;

  return *this;
}


template < typename T >
int CyclicSampleData<T>::minIndex( void ) const
{
  return CyclicArray<T>::minIndex();
}


template < typename T >
double CyclicSampleData<T>::minPos( void ) const
{
  return pos( minIndex() );
}


template < typename T >
void CyclicSampleData<T>::resize( int n, const T &val )
{
  CyclicArray<T>::resize( n, val );
}


template < typename T >
void CyclicSampleData<T>::resize( double duration, const T &val )
{
  CyclicArray<T>::resize( (int)::ceil( duration/stepsize() ), val );
}



template < typename T >
void CyclicSampleData<T>::reserve( int n )
{
  CyclicArray<T>::reserve( n );
}


template < typename T >
void CyclicSampleData<T>::reserve( double duration )
{
  CyclicArray<T>::reserve( (int)::ceil( duration/stepsize() ) );
}


template < typename T >
double CyclicSampleData<T>::offset( void ) const
{
  return Offset;
}


template < typename T >
void CyclicSampleData<T>::setOffset( double offset )
{
  Offset = offset;
}


template < typename T >
double CyclicSampleData<T>::stepsize( void ) const
{
  return Stepsize;
}


template < typename T >
void CyclicSampleData<T>::setStepsize( double stepsize )
{
  Stepsize = stepsize;
}


template < typename T >
void CyclicSampleData<T>::setRange( const T &offset, const T &stepsize )
{
  Offset = offset;
  Stepsize = stepsize;
}


template < typename T >
double CyclicSampleData<T>::length( void ) const
{
  return ::fabs( stepsize() * this->size() );
}


template < typename T >
double CyclicSampleData<T>::rangeFront( void ) const
{
  return Offset;
}


template < typename T >
double CyclicSampleData<T>::rangeBack( void ) const
{
  return pos( this->size() );
}


template < typename T >
void CyclicSampleData<T>::shift( double val )
{
  Offset += val;
}


template < typename T >
void CyclicSampleData<T>::scale( double scale )
{
  Offset *= scale;
  Stepsize *= scale;
}


template < typename T >
double CyclicSampleData<T>::pos( int i ) const
{
  return Offset + i * Stepsize;
}


template < typename T >
double CyclicSampleData<T>::interval( int indices ) const
{
  return indices * Stepsize;
}


template < typename T >
int CyclicSampleData<T>::index( double pos ) const
{
  return int( ::floor( (pos - offset())/stepsize() + 1.0e-6 ) );
}


template < typename T >
int CyclicSampleData<T>::indices( double iv ) const
{
  return int( ::floor( iv/stepsize() + 1.0e-6 ) );
}


template < typename T >
bool CyclicSampleData<T>::contains( double p ) const
{
  return ( p >= offset() && p <= pos( this->size() ) );
}


template < typename T >
const CyclicArray<T> &CyclicSampleData<T>::array( void ) const
{
  return *this;
}


template < typename T >
CyclicArray<T> &CyclicSampleData<T>::array( void )
{
  return *this;
}


template < typename T >
LinearRange CyclicSampleData<T>::range( void )
{
  return LinearRange( this->size(), offset(), stepsize() );
}


template < typename T >
T CyclicSampleData<T>::min( int from, int upto ) const
{
  return CyclicArray<T>::min( from, upto );
}


template < typename T >
T CyclicSampleData<T>::min( double from, double upto ) const
{
  return min( index( from ), index( upto ) );
}


template < typename T >
T CyclicSampleData<T>::min( double from ) const
{
  return min( index( from ), this->size() );
}


template < typename T >
T CyclicSampleData<T>::max( int from, int upto ) const
{
  return CyclicArray<T>::max( from, upto );
}


template < typename T >
T CyclicSampleData<T>::max( double from, double upto ) const
{
  return max( index( from ), index( upto ) );
}


template < typename T >
T CyclicSampleData<T>::max( double from ) const
{
  return max( index( from ), this->size() );
}


template < typename T >
void CyclicSampleData<T>::minMax( T &min, T &max, int from, int upto ) const
{
  return CyclicArray<T>::minMax( min, max, from, upto );
}


template < typename T >
void CyclicSampleData<T>::minMax( T &min, T &max, double from, double upto ) const
{
  return minMax( min, max, index( from ), index( upto ) );
}


template < typename T >
void CyclicSampleData<T>::minMax( T &min, T &max, double from ) const
{
  return minMax( min, max, index( from ), this->size() );
}


template < typename T >
T CyclicSampleData<T>::minAbs( int from, int upto ) const
{
  return CyclicArray<T>::minAbs( from, upto );
}


template < typename T >
T CyclicSampleData<T>::minAbs( double from, double upto ) const
{
  return minAbs( index( from ), index( upto ) );
}


template < typename T >
T CyclicSampleData<T>::minAbs( double from ) const
{
  return minAbs( index( from ), this->size() );
}


template < typename T >
T CyclicSampleData<T>::maxAbs( int from, int upto ) const
{
  return CyclicArray<T>::maxAbs( from, upto );
}


template < typename T >
T CyclicSampleData<T>::maxAbs( double from, double upto ) const
{
  return maxAbs( index( from ), index( upto ) );
}


template < typename T >
T CyclicSampleData<T>::maxAbs( double from ) const
{
  return maxAbs( index( from ), this->size() );
}

template < typename T >
typename numerical_traits<T>::mean_type
CyclicSampleData<T>::mean( int from, int upto ) const
{
  return CyclicArray<T>::mean( from, upto );
}


template < typename T >
typename numerical_traits<T>::mean_type
CyclicSampleData<T>::mean( double from, double upto ) const
{
  return mean( index( from ), index( upto ) );
}


template < typename T > template < typename R >
void CyclicSampleData<T>::mean( double time, SampleData<R> &d, double width ) const
{
  if ( width <= 0.0 )
    width = d.stepsize();
  int wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<d.size(); i++ ) {
    int from = index( time + d.pos( i ) );
    int upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > this->size() )
      upto = this->size();

    // mean:
    R mean = 0.0;
    int n = 0;
    for ( int k=from; k<upto; k++ )
      mean += ( this->operator[]( k ) - mean ) / (++n);

    d[i] = mean;
  }
}


template < typename T >
typename numerical_traits<T>::variance_type
CyclicSampleData<T>::variance( int from, int upto ) const
{
  return CyclicArray<T>::variance( from, upto );
}


template < typename T >
typename numerical_traits<T>::variance_type
CyclicSampleData<T>::variance( double from, double upto ) const
{
  return variance( index( from ), index( upto ) );
}


template < typename T > template < typename R >
void CyclicSampleData<T>::variance( double time, SampleData<R> &d, double width ) const
{
  if ( width <= 0.0 )
    width = d.stepsize();
  int wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<d.size(); i++ ) {
    int from = index( time + d.pos( i ) );
    int upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > this->size() )
      upto = this->size();

    // mean:
    R mean = 0.0;
    int n = 0;
    for ( int k=from; k<upto; k++ )
      mean += ( this->operator[]( k ) - mean ) / (++n);

    // mean squared diffference from mean:
    R var = 0.0;
    n = 0;
    for ( int k=from; k<upto; k++ ) {
      // subtract mean:
      R d = this->operator[]( k ) - mean;
      // average over squares:
      var += ( d*d - var ) / (++n);
    }

    // variance:
    d[i] = var;
  }
}


template < typename T >
typename numerical_traits<T>::variance_type
CyclicSampleData<T>::stdev( int from, int upto ) const
{
  return CyclicArray<T>::stdev( from, upto );
}


template < typename T >
typename numerical_traits<T>::variance_type
CyclicSampleData<T>::stdev( double from, double upto ) const
{
  return stdev( index( from ), index( upto ) );
}


template < typename T > template < typename R >
void CyclicSampleData<T>::stdev( double time, SampleData<R> &d, double width ) const
{
  if ( width <= 0.0 )
    width = d.stepsize();
  int wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<d.size(); i++ ) {
    int from = index( time + d.pos( i ) );
    int upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > this->size() )
      upto = this->size();

    // mean:
    R mean = 0.0;
    int n = 0;
    for ( int k=from; k<upto; k++ )
      mean += ( this->operator[]( k ) - mean ) / (++n);

    // mean squared diffference from mean:
    R var = 0.0;
    n = 0;
    for ( int k=from; k<upto; k++ ) {
      // subtract mean:
      R d = this->operator[]( k ) - mean;
      // average over squares:
      var += ( d*d - var ) / (++n);
    }

    // square root:
    d[i] = sqrt( var );
  }
}


template < typename T >
typename numerical_traits<T>::variance_type
CyclicSampleData<T>::rms( int from, int upto ) const
{
  return CyclicArray<T>::rms( from, upto );
}


template < typename T >
typename numerical_traits<T>::variance_type
CyclicSampleData<T>::rms( double from, double upto ) const
{
  return rms( index( from ), index( upto ) );
}


template < typename T > template < typename R >
void CyclicSampleData<T>::rms( double time, SampleData<R> &d, double width ) const
{
  if ( width <= 0.0 )
    width = d.stepsize();
  int wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<d.size(); i++ ) {
    int from = index( time + d.pos( i ) );
    int upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > this->size() )
      upto = this->size();

    // mean squared diffference from mean:
    R var = 0.0;
    int n = 0;
    for ( int k=from; k<upto; k++ ) {
      R d = this->operator[]( k );
      // average over squares:
      var += ( d*d - var ) / (++n);
    }

    // square root:
    d[i] = sqrt( var );
  }
}


template < typename T > template< typename S >
void CyclicSampleData<T>::hist( SampleData< S > &h, int from, int upto ) const
{
  CyclicArray<T>::hist( h, from, upto );
}


template < typename T > template< typename S >
void CyclicSampleData<T>::hist( SampleData< S > &h, double from, double upto ) const
{
  hist( h, index( from ), index( upto ) );
}


template < typename T > template< typename S >
void CyclicSampleData<T>::hist( SampleData< S > &h ) const
{
  CyclicArray<T>::hist( h );
}


template < typename T >
ostream &operator<<( ostream &str, const CyclicSampleData<T> &a )
{
  str << CyclicArray<T>( a );
  str << "Offset: " << a.Offset << '\n';
  str << "Stepsize: " << a.Stepsize << '\n';
  return str;
}


}; /* namespace relacs */

#endif /* ! _RELACS_CYCLICSAMPLEDATA_H_ */
