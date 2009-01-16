/*
  cyclicarray.h
  A template defining an one-dimensional cyclic array of data.

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

#ifndef _RELACS_CYCLICARRAY_H_
#define _RELACS_CYCLICARRAY_H_ 1

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <relacs/sampledata.h>
using namespace std;

namespace relacs {


/*!
\class CyclicArray
\brief A template defining an one-dimensional cyclic array of data.
\author Jan Benda
\version 0.1

This class is very similar to the vector class from 
the standard template library, in that it is a
random access container of objects of type \a T.
The size() of CyclicArray, however, can exceed its capacity().
Data elements below size()-capacity() are therefore not accessible.
*/

template < class T = double >
class CyclicArray
{

  public:

    /*! Creates an empty CyclicArray. */
  CyclicArray( void );
    /*! Creates an empty array with capacity \a n data elements. */
  CyclicArray( int n );
    /*! Copy constructor.
        Creates an CyclicArray with the same size and content as \a ca. */
  CyclicArray( const CyclicArray< T > &ca );
    /*! The destructor. */
  virtual ~CyclicArray( void );

    /*! Assigns \a a to *this. */
  const CyclicArray<T> &operator=( const CyclicArray<T> &a );
    /*! Assigns \a a to *this. */
  const CyclicArray<T> &assign( const CyclicArray<T> &a );

    /*! The size of the array, 
        i.e. the total number of added data elements.
        Can be larger than capacity()!
        \sa accessibleSize(), readSize(), empty() */
  int size( void ) const;
    /*! The number of data elements that are actually stored in the array
        and therefore are accessible.
        Less or equal than capacity() and size()!
        \sa minIndex(), readSize(), empty() */
  int accessibleSize( void ) const;
    /*! The index of the first accessible data element.
        \sa accessibleSize() */
  int minIndex( void ) const;
    /*! True if the array does not contain any data elements,
        i.e. size() equals zero.
        \sa size(), accessibleSize(), readSize() */
  bool empty( void ) const;
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
	The capacity is not changed!
        If, however, the capacity() of the array is zero,
        then memory for \a n data elements is allocated
        and initialized with \a val. */
  virtual void resize( int n, const T &val=0 );
    /*! Resize the array to zero length.
        The capacity() remains unchanged. */
  virtual void clear( void );

    /*! The capacity of the array, i.e. the number of data
        elements for which memory has been allocated. */
  int capacity( void ) const { return NBuffer; };
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
        If the array is empty or the first element is 
	not accesible
	a reference to a variable set to zero is returned. */
  const T &front( void ) const;
    /*! Returns a reference to the first data element.
        If the array is empty or the first element is 
	not accesible
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
    /*! Remove the last element of the array
        and return its value. */
  inline T pop( void );
    /*! Maximum number of data elements allowed to be added to the buffer 
        at once. 
        \sa pushBuffer(), push() */
  int maxPush( void ) const;
    /*! Pointer into the buffer where to add data.
        \sa maxPush(), push() */
  T *pushBuffer( void );
    /*! Tell CyclicArray that \a n data elements have been added to
        pushBuffer().
        \sa maxPush() */
  void push( int n );

    /*! The number of data elements available to be read from the array. 
        \sa read(), readIndex(), size(), accessibleSize() */
  int readSize( void ) const;
    /*! The index of the data element to be read next from the array. 
        \sa read(), readSize() */
  int readIndex( void ) const;
    /*! Return value of the first to be read data element
        and increment read index.
        \sa readSize(), readIndex() */
  inline T read( void );

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

    /*! Return the minimum value of the array between index \a from inclusively
        and index \a upto exclusively. */
  T min( int from, int upto ) const;
    /*! Return the maximum value of the array between index \a from inclusively
        and index \a upto exclusively. */
  T max( int from, int upto ) const;
    /*! Return the minimum absolute value of the array between index \a from inclusively
        and index \a upto exclusively. */
  T minAbs( int from, int upto ) const;
    /*! Return the maximum absolute value of the array between index \a from inclusively
        and index \a upto exclusively. */
  T maxAbs( int from, int upto ) const;

    /*! Return the mean value of the array between index \a from inclusively
        and index \a upto exclusively. */
  T mean( int from, int upto ) const;
    /*! Return the variance of the array between index \a from inclusively
        and index \a upto exclusively. */
  T variance( int from, int upto ) const;
    /*! Return the standard deviation of the array between index \a from inclusively
        and index \a upto exclusively. */
  T stdev( int from, int upto ) const;
    /*! Return the root-mean-square of the array between index \a from inclusively
        and index \a upto exclusively. */
  T rms( int from, int upto ) const;

    /*! Compute histogram \a h of all data elements currently
        stored in the array. */
  template< typename S >
  void hist( SampleData< S > &h ) const;

    /*! Save binary data to stream \a os starting at index \a index upto size().
        \return the number of saved data elements. */
  int saveBinary( ostream &os, int index ) const;

  template < typename TT > 
  friend ostream &operator<<( ostream &str, const CyclicArray<TT> &ca );


  private:

  T *Buffer;
  int NBuffer;
  int RCycles;
  int R;
  int LCycles;
  int L;
  T Val;  // for pop()
  mutable T Dummy;
  
};


typedef CyclicArray< double > CyclicArrayD;
typedef CyclicArray< float > CyclicArrayF;
typedef CyclicArray< int > CyclicArrayI;


template < class T >
CyclicArray< T >::CyclicArray( void )
  : Buffer( 0 ),
    NBuffer( 0 ),
    RCycles( 0 ),
    R( 0 ),
    LCycles( 0 ),
    L( 0 ),
    Val( 0 ),
    Dummy( 0 )
{
}


template < class T >
CyclicArray< T >::CyclicArray( int n )
  : Buffer( 0 ),
    NBuffer( 0 ),
    RCycles( 0 ),
    R( 0 ),
    LCycles( 0 ),
    L( 0 ),
    Val( 0 ),
    Dummy( 0 )
{
  if ( n > 0 ) {
    Buffer = new T[ n ];
    NBuffer = n;
  }
}


template < class T >
CyclicArray< T >::CyclicArray( const CyclicArray< T > &ca )
  : Buffer( 0 ),
    NBuffer( 0 ),
    RCycles( ca.RCycles ),
    R( ca.R ),
    LCycles( ca.LCycles ),
    L( ca.L ),
    Val( ca.Val ),
    Dummy( ca.Dummy )
{
  if ( ca.capacity() > 0 ) {
    Buffer = new T[ ca.capacity() ];
    NBuffer = ca.capacity();
    memcpy( Buffer, ca.Buffer, NBuffer * sizeof( T ) );
  }
}


template < class T >
CyclicArray< T >::~CyclicArray( void )
{
  if ( Buffer != 0 )
    delete [] Buffer;
}


template < class T >
const CyclicArray<T> &CyclicArray<T>::operator=( const CyclicArray<T> &a )
{
  return assign( a );
}


template < class T >
const CyclicArray<T> &CyclicArray<T>::assign( const CyclicArray<T> &a )
{
  if ( &a == this )
    return *this;

  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  NBuffer = 0;
  Val = 0;

  if ( a.capacity() > 0 ) {
    Buffer = new T[ a.capacity() ];
    NBuffer = a.capacity();
    R = a.size();
    memcpy( Buffer, a.Buffer, NBuffer * sizeof( T ) );
    RCycles = a.RCycles;
    R = a.R;
    LCycles = a.LCycles;
    L = a.L;
  }
  else {
    RCycles = 0;
    R = 0;
    LCycles = 0;
    L = 0;
  }

  return *this;
}


template < class T >
int CyclicArray< T >::size( void ) const
{
  return RCycles * NBuffer + R;
}


template < class T >
int CyclicArray< T >::accessibleSize( void ) const
{
  return RCycles == 0 ? R : NBuffer;
}


template < class T >
int CyclicArray< T >::minIndex( void ) const
{
  return RCycles == 0 ? 0 : (RCycles-1) * NBuffer + R;
}


template < class T >
bool CyclicArray< T >::empty( void ) const
{
  return RCycles == 0 && R == 0;
}


template < class T >
void CyclicArray< T >::resize( int n, const T &val )
{
  if ( n <= 0 ) {
    clear();
    return;
  }

  if ( NBuffer <= 0 ) {
    reserve( n );
    for ( int k=0; k<NBuffer; k++ )
      Buffer[k] = val;
    RCycles = 0;
    R = n;
    LCycles = 0;
    L = 0;
    return;
  }

  if ( n < size() ) {
    RCycles = (n-1) / NBuffer;
    R = 1 + (n-1) % NBuffer;
    if ( RCycles * NBuffer + R < LCycles * NBuffer + L ) {
      RCycles = LCycles;
      R = L;
    }
  }
  else if ( n > size() ) {
    if ( n - size() >= NBuffer ) {
      for ( int k=0; k<NBuffer; k++ )
	Buffer[k] = val;
      RCycles = (n-1) / NBuffer;
      R = 1 + (n-1) % NBuffer;
    }
    else {
      int orc = RCycles;
      int ori = R;
      RCycles = (n-1) / NBuffer;
      R = 1 + (n-1) % NBuffer;
      if ( RCycles > orc ) {
	for ( int k=ori; k<NBuffer; k++ )
	  Buffer[k] = val;
      }
      for ( int k=0; k<R; k++ )
	Buffer[k] = val;
    }
    if ( (LCycles+1)*NBuffer + L < RCycles*NBuffer + R ) {
      LCycles = RCycles-1;
      L = R;
    }
  }
}


template < class T >
void CyclicArray< T >::clear( void )
{
  RCycles = 0;
  R = 0;
  LCycles = 0;
  L = 0;
}


template < class T >
void CyclicArray< T >::reserve( int n )
{
  if ( n > NBuffer ) {
    T *newbuf = new T[ n ];
    if ( Buffer != 0 && NBuffer > 0 ) {
      int ori = R;
      int on = size();
      RCycles = (on-1) / n;
      R = 1 + (on-1) % n;
      int j = ori;
      int k = R;
      for ( int i=0; i < NBuffer; i++ ) {
	if ( j == 0 )
	  j = NBuffer;
	if ( k == 0 )
	  k = n;
	j--;
	k--;
	newbuf[k] = Buffer[j];
      }
      delete [] Buffer;
      int oln = LCycles*NBuffer + L;
      LCycles = (oln-1) / n;
      L = 1 + (oln-1) % n;
    }
    Buffer = newbuf;
    NBuffer = n;
  }
}


template < class T >
const T &CyclicArray< T >::operator[]( int i ) const
{
  assert( ( i >= minIndex() && i < size() ) );
  return Buffer[ i % NBuffer ];
}


template < class T >
T &CyclicArray< T >::operator[]( int i )
{
  assert( ( i >= minIndex() && i < size() ) );
  return Buffer[ i % NBuffer ];
}


template < typename T > 
const T &CyclicArray<T>::at( int i ) const
{
  if ( Buffer != 0 &&
       i >= minIndex() && i < size() ) {
    return Buffer[ i % NBuffer ];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
T &CyclicArray<T>::at( int i )
{
  if ( Buffer != 0 &&
       i >= minIndex() && i < size() ) {
    return Buffer[ i % NBuffer ];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
const T &CyclicArray<T>::front( void ) const
{
  if ( Buffer != 0 && size() > 0 && minIndex() == 0 ) {
    return Buffer[0];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
T &CyclicArray<T>::front( void )
{
  if ( Buffer != 0 && size() > 0 && minIndex() == 0 ) {
    return Buffer[0];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
const T &CyclicArray<T>::back( void ) const
{
  if ( Buffer != 0 && size() > 0 ) {
    return Buffer[R-1];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < typename T > 
T &CyclicArray<T>::back( void )
{
  if ( Buffer != 0 && size() > 0 ) {
    return Buffer[R-1];
  }
  else {
    Dummy = 0;
    return Dummy;
  }
}


template < class T >
void CyclicArray< T >::push( const T &val )
{
  assert( Buffer != 0 && NBuffer > 0 );
  if ( NBuffer <= 0 )
    reserve( 100 );

  if ( R >= NBuffer ) {
    R = 0;
    RCycles++;
  }

  Val = Buffer[ R ];
  Buffer[ R ] = val;

  R++;
}


template < class T >
T CyclicArray< T >::pop( void )
{
  if ( NBuffer <= 0 || R <= 0 )
    return 0;

  R--;

  T v = Buffer[ R ];
  Buffer[ R ] = Val;

  if ( R == 0 && RCycles > 0 ) {
    R = NBuffer;
    RCycles--;
  }

  return v;
}


template < class T >
int CyclicArray< T >::maxPush( void ) const
{
  return R < NBuffer ? NBuffer - R : NBuffer;
}


template < class T >
T *CyclicArray< T >::pushBuffer( void )
{
  return R < NBuffer ? Buffer + R : Buffer;
}


template < class T >
void CyclicArray< T >::push( int n )
{
  if ( R >= NBuffer ) {
    R = 0;
    RCycles++;
  }

  R += n;

#ifndef NDEBUG
  if ( !( R >= 0 && R <= NBuffer ) )
    cerr << "CyclicArray::push( int n ): R=" << R << " <0 or > NBuffer=" << NBuffer << endl; 
#endif
  assert( ( R >= 0 && R <= NBuffer ) );
}


template < class T >
int CyclicArray< T >::readSize( void ) const
{ 
  int n = (RCycles - LCycles)*NBuffer + R - L;
#ifndef NDEBUG
  if ( n > NBuffer )
    cerr << "overflow in CyclicArray!\n";
#endif
  assert ( n <= NBuffer );
  return n;
}


template < class T >
int CyclicArray< T >::readIndex( void ) const
{
  return LCycles*NBuffer + L;
}


template < class T >
T CyclicArray< T >::read( void )
{
  if ( NBuffer <= 0 )
    return 0.0;

  int l = L++;
  if ( L >= NBuffer ) {
    L = 0;
    LCycles++;
  }

  return Buffer[ l ];
}


template < class T >
T CyclicArray< T >::min( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  T m = operator[]( from );
  for ( int k=from+1; k<upto; k++ )
    if ( operator[]( k ) < m )
      m = operator[]( k );

  return m;
}


template < class T >
T CyclicArray< T >::max( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  T m = operator[]( from );
  for ( int k=from+1; k<upto; k++ )
    if ( operator[]( k ) > m )
      m = operator[]( k );

  return m;
}


template < class T >
T CyclicArray< T >::maxAbs( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  T m = ::fabs( operator[]( from ) );
  for ( int k=from+1; k<upto; k++ )
    if ( ::fabs( operator[]( k ) ) > m )
      m = ::fabs( operator[]( k ) );

  return m;
}


template < class T >
T CyclicArray< T >::minAbs( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  T m = ::fabs( operator[]( from ) );
  for ( int k=from+1; k<upto; k++ )
    if ( ::fabs( operator[]( k ) ) < m )
      m = ::fabs( operator[]( k ) );

  return m;
}


template < class T >
T CyclicArray< T >::mean( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  // mean:
  T mean = 0.0;
  int n = 0;
  for ( int k=from; k<upto; k++ )
    mean += ( operator[]( k ) - mean ) / (++n);

  return mean;
}


template < class T >
T CyclicArray< T >::variance( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  // mean:
  T mean = 0.0;
  int n = 0;
  for ( int k=from; k<upto; k++ )
    mean += ( operator[]( k ) - mean ) / (++n);

  // mean squared diffference from mean:
  T var = 0.0;
  n = 0;
  for ( int k=from; k<upto; k++ ) {
    // subtract mean:
    T d = operator[]( k ) - mean;
    // average over squares:
    var += ( d*d - var ) / (++n);
  }

  // variance:
  return var;
}


template < class T >
T CyclicArray< T >::stdev( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  // mean:
  T mean = 0.0;
  int n = 0;
  for ( int k=from; k<upto; k++ )
    mean += ( operator[]( k ) - mean ) / (++n);

  // mean squared diffference from mean:
  T var = 0.0;
  n = 0;
  for ( int k=from; k<upto; k++ ) {
    // subtract mean:
    T d = operator[]( k ) - mean;
    // average over squares:
    var += ( d*d - var ) / (++n);
  }

  // square root:
  return sqrt( var );
}


template < class T >
T CyclicArray< T >::rms( int from, int upto ) const
{
  if ( from < minIndex() )
    from = minIndex();
  if ( upto > size() )
    upto = size();

  if ( from >= upto )
    return 0.0;

  // mean squared values:
  T var = 0.0;
  int n = 0;
  for ( int k=from; k<upto; k++ ) {
    T d = operator[]( k );
    // average over squares:
    var += ( d*d - var ) / (++n);
  }

  // square root:
  return sqrt( var );
}


template < class T > template< typename S >
void CyclicArray< T >::hist( SampleData< S > &h ) const
{
  h = 0.0;

  int n = NBuffer;
  if ( RCycles == 0 )
    n = R;

  double l = h.rangeFront();
  double s = h.stepsize();

  for ( int k=0; k<n; k++ ){
    int b = (int)rint( ( Buffer[k] - l ) / s );
    if ( b >= 0  && b < h.size() )
      h[b] += 1;
  }
}


template < class T >
int CyclicArray< T >::saveBinary( ostream &os, int index ) const
{
  // stream not open:
  if ( !os )
    return -1;

  assert( ( index >= minIndex() && index < size() ) );

  int buffinx = RCycles * NBuffer;
  int li = index - buffinx;
  int ri = R;

  // write buffer:
  if ( li >= 0 )
    os.write( (char *)(Buffer+li), sizeof( T )*( ri - li ) );
  else {
    os.write( (char *)(Buffer + (li+NBuffer)), sizeof( T )*( -li ) );
    os.write( (char *)Buffer, sizeof( T )*ri );
  }
  os.flush();
  return ri - li;
}


template < class T >
ostream &operator<<( ostream &str, const CyclicArray< T > &ca )
{
  str << "Buffer: " << ca.Buffer << '\n';
  str << "NBuffer: " << ca.NBuffer << '\n';
  str << "RCycles: " << ca.RCycles << '\n';
  str << "R: " << ca.R << '\n';
  str << "LCycles: " << ca.LCycles << '\n';
  str << "L: " << ca.L << '\n';
  str << "Val: " << ca.Val << '\n';
  return str;
}


}; /* namespace relacs */

#endif /* ! _RELACS_CYCLICARRAY_H_ */

