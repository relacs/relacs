/*
  cyclicsampledata.h
  A template defining an one-dimensional cyclic array of eveny sampled data.

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

#ifndef _RELACS_CYCLICSAMPLEDATA_H_
#define _RELACS_CYCLICSAMPLEDATA_H_ 1

#include <relacs/cyclicarray.h>
#include <relacs/linearrange.h>
#include <relacs/stats.h>

namespace relacs {


/*!
\class CyclicSampleData
\author Jan Benda
\version 0.1
\brief A template defining an one-dimensional cyclic array of eveny sampled data.


This class is very similar to SampleData, in that it is a
random access container of objects of type \a T
that were sampled with stepsize().
The size() of CyclicSampleData, however, can exceed its capacity().
Data elements below size()-capacity() are therefore not accessible.
*/

template < class T = double >
  class CyclicSampleData : public CyclicArray< T >
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
    /*! Copy constructor.
        Creates an CyclicSampleData with the same size, content, and range as \a csd. */
  CyclicSampleData( const CyclicSampleData< T > &csd );
    /*! The destructor. */
  ~CyclicSampleData( void );

    /*! The index of the first accessible data element.
        \sa minPos(), accessibleSize() */
  int minIndex( void ) const { return CyclicArray< T >::minIndex(); };
    /*! The position of the first accessible data element.
        \sa minIndex(), accessibleSize() */
  double minPos( void ) const { return pos( minIndex() ); };

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
  double offset( void ) const { return Offset; };
     /*! Set the offset of the range to \a offset. */
  void setOffset( double offset ) { Offset = offset; };
     /*! The stepsize of the range. */
  double stepsize( void ) const { return Stepsize; };
     /*! Set the stepsize of the range to \a stepsize. */
  void setStepsize( double stepsize ) { Stepsize = stepsize; };
     /*! Set the offset and the stepsize of the range to \a offset and \a stepsize, respectively. */
  void setRange( const T &offset, const T &stepsize ) { Offset=offset; Stepsize=stepsize; };
     /*! The length of the range, i.e. abs( stepsize() * size() ) */
  double length( void ) const { return ::fabs( stepsize()*size() ); };
    /*! Returns the first range element, i.e. the offset. \sa offset() */
  double rangeFront( void ) const { return Offset; };
    /*! Returns the last range element. */
  double rangeBack( void ) const { return pos( size() ); };

     /*! Add \a val to the offset of the range,
         i.e. shift the range by \a val. */
  void shift( double val ) { Offset += val; };
    /*! Multiply the offset and the stepsize of the range with \a scale, 
        i.e. rescale the range by \a scale. */
  void scale( double scale ) { Offset *= scale; Stepsize *= scale; };

    /*! Returns the range element at index \a i. */
  double pos( int i ) const { return Offset + i * Stepsize; };
    /*! Returns the interval covered by \a indices indices. */
  double interval( int indices ) const { return indices * Stepsize; };

    /*! The index of the range corresponding to \a pos. */
  int index( double pos ) const { return int( ::floor( (pos - offset())/stepsize() ) ); };
    /*! The number of indices corresponding to an interval \a iv. */
  int indices( double iv ) const { return long( ::floor( iv/stepsize() ) ); };
    /*! True if \a pos is within the range. */
  bool contains( double pos ) const { return ( p >= offset() && p <= pos( size() ) ); };

    /*! Returns a reference to the data array. */
  const CyclicArray<T> &array( void ) const { return *this; };
    /*! Returns a reference to the data array. */
  CyclicArray<T> &array( void ) { return *this; };

    /*! Returns a copy of the range. */
  LinearRange range( void ) { return LinearRange( size(), offset(), stepsize() ); };

    /*! The type of an element of the range. */
  typedef double range_type;
    /*! Const iterator used to iterate through the range. */
  typedef LinearRangeIterator const_range_iterator;

    /*! Return the maximum value of the data between index \a from inclusively
        and index \a upto exclusively. */
  double max( long from, long upto ) const;
    /*! Return the maximum value of the data during \a duration seconds
        starting at time \a time seconds. */
  double max( double time, double duration ) const
    { return max( indices( time ), indices( time + duration ) ); };
    /*! Return the maximum value of the data since time \a time seconds. */
  double max( double time ) const
    { return max( indices( time ), size() ); };
    /*! Return the minimum value of the data between index \a from inclusively
        and index \a upto exclusively. */
  double min( long from, long upto ) const;
    /*! Return the minimum value of the data during \a duration seconds
        starting at time \a time seconds. */
  double min( double time, double duration ) const
    { return min( indices( time ), indices( time + duration ) ); };
    /*! Return the minimum value of the data since time \a time seconds. */
  double min( double time ) const
    { return min( indices( time ), size() ); };
    /*! Return the maximum absolute value of the data between index \a from inclusively
        and index \a upto exclusively. */
  double maxAbs( long from, long upto ) const;
    /*! Return the maximum absolute value of the data during \a duration seconds
        starting at time \a time seconds. */
  double maxAbs( double time, double duration ) const
    { return maxAbs( indices( time ), indices( time + duration ) ); };
    /*! Return the maximum absolute value of the data since time \a time seconds. */
  double maxAbs( double time ) const
    { return maxAbs( indices( time ), size() ); };
    /*! Return the minimum absolute value of the data between index \a from inclusively
        and index \a upto exclusively. */
  double minAbs( long from, long upto ) const;
    /*! Return the minimum absolute value of the data during \a duration seconds
        starting at time \a time seconds. */
  double minAbs( double time, double duration ) const
    { return minAbs( indices( time ), indices( time + duration ) ); };
    /*! Return the minimum absolute value of the data since time \a time seconds. */
  double minAbs( double time ) const
    { return minAbs( indices( time ), size() ); };

    /*! Return the mean value of the data between index \a from inclusively
        and index \a upto exclusively. */
  double mean( long from, long upto ) const;
    /*! Return the mean value of the data during \a duration seconds
        starting at time \a time seconds. */
  double mean( double time, double duration ) const
    { return mean( indices( time ), indices( time + duration ) ); };
    /*! Returns in \a md the mean values of the data calculated during
        \a width long time windows starting at the times
        \a time + \a md.pos(i) (moving average).
        If \a width equals zero it is set to the stepsize defined by \a rd. */
  void mean( double time, SampleDataD &md, double width=0.0 ) const;

    /*! Return the variance of the data between index \a from inclusively
        and index \a upto exclusively. */
  double variance( long from, long upto ) const;
    /*! Return the variance of the data during \a duration seconds
        starting at time \a time seconds. */
  double variance( double time, double duration ) const
    { return variance( indices( time ), indices( time + duration ) ); };
    /*! Returns in \a vd the variance calculated during
        \a width long time windows starting at the times
        \a time + \a rd.pos(i).
        If \a width equals zero it is set to the stepsize defined by \a rd. */
  void variance( double time, SampleDataD &vd, double width=0.0 ) const;

    /*! Return the standard deviation of the data between index \a from inclusively
        and index \a upto exclusively. */
  double stdev( long from, long upto ) const;
    /*! Return the standard deviation of the data during \a duration seconds
        starting at time \a time seconds. */
  double stdev( double time, double duration ) const
    { return stdev( indices( time ), indices( time + duration ) ); };
    /*! Returns in \a sd the standard deviation calculated during
        \a width long time windows starting at the times
        \a time + \a rd.pos(i).
        If \a width equals zero it is set to the stepsize defined by \a rd. */
  void stdev( double time, SampleDataD &rd, double width=0.0 ) const;

    /*! Return the root-mean-square of the data between index \a from inclusively
        and index \a upto exclusively. */
  double rms( long from, long upto ) const;
    /*! Return the root-mean-square of the data during \a duration seconds
        starting at time \a time seconds. */
  double rms( double time, double duration ) const
    { return rms( indices( time ), indices( time + duration ) ); };
    /*! Returns in \a rd the root-mean-square calculated during
        \a width long time windows starting at the times
        \a time + \a rd.pos(i).
        If \a width equals zero it is set to the stepsize defined by \a rd. */
  void rms( double time, SampleDataD &rd, double width=0.0 ) const;


  private:

  double Offset;
  double Stepsize;

};


typedef CyclicSampleData< double > CyclicSampleDataD;
typedef CyclicSampleData< float > CyclicSampleDataF;
typedef CyclicSampleData< int > CyclicSampleDataI;


template < class T >
CyclicSampleData< T >::CyclicSampleData( void )
  : CyclicArray< T >(),
    Offset( 0 ),
    Stepsize( 1 )
{
}


template < class T >
CyclicSampleData< T >::CyclicSampleData( int n, double stepsize )
  : CyclicArray< T >( n ),
    Offset( 0 ),
    Stepsize( stepsize )
{
}


template < class T >
CyclicSampleData< T >::CyclicSampleData( int n, double offset, double stepsize )
  : CyclicArray< T >( n ),
    Offset( offset ),
    Stepsize( stepsize ),
{
}


template < class T >
CyclicSampleData< T >::CyclicSampleData( double duration, double stepsize )
  : CyclicArray< T >( (int)::ceil( duration/stepsize ) ),
    Offset( 0 ),
    Stepsize( stepsize )
{
}


template < class T >
CyclicSampleData< T >::CyclicSampleData( double duration, double offset, double stepsize )
  : CyclicArray< T >( (int)::ceil( duration/stepsize ) ),
    Offset( offset ),
    Stepsize( stepsize )
{
}


template < class T >
CyclicSampleData< T >::CyclicSampleData( const CyclicSampleData< T > &csd )
  : CyclicArray< T >( csd ),
    Offset( csd.Offset ),
    Stepsize( csd.Stepsize ),
{
}


template < class T >
CyclicSampleData< T >::~CyclicSampleData( void )
{
}


template < class T >
void CyclicSampleData< T >::resize( int n, const T &val )
{
  CyclicArray< T >::resize( n, val );
}


template < class T >
void CyclicSampleData< T >::resize( double duration, const T &val )
{
  CyclicArray< T >::resize( (int)::ceil( duration/stepsize() ), val );
}



template < class T >
void CyclicSampleData< T >::reserve( int n )
{
  CyclicArray< T >::reserve( n );
}


template < class T >
void CyclicSampleData< T >::reserve( double duration )
{
  CyclicArray< T >::reserve( (int)::ceil( duration/stepsize() ) );
}


double CyclicSampleData< T >::max( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::max( begin()+from, begin()+upto );
}


double CyclicSampleData< T >::min( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::min( begin()+from, begin()+upto );
}


double CyclicSampleData< T >::maxAbs( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::maxAbs( begin()+from, begin()+upto );
}


double CyclicSampleData< T >::minAbs( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::minAbs( begin()+from, begin()+upto );
}


double CyclicSampleData< T >::mean( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::mean( begin()+from, begin()+upto );
}


double CyclicSampleData< T >::variance( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::variance( begin()+from, begin()+upto );
}


double CyclicSampleData< T >::stdev( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::stdev( begin()+from, begin()+upto );
}


double CyclicSampleData< T >::rms( long from, long upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  return ::relacs::rms( begin()+from, begin()+upto );
}


}; /* namespace relacs */

#endif /* ! _RELACS_CYCLICSAMPLEDATA_H_ */
