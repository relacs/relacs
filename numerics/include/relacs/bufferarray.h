/*
  bufferarray.h
  A template defining a buffered, one-dimensional cyclic array of data.

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

#ifndef _RELACS_BUFFERARRAY_H_
#define _RELACS_BUFFERARRAY_H_ 1

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <relacs/cyclicarray.h>
using namespace std;

namespace relacs {


/*!
\class BufferArray
\brief A template defining a buffered, one-dimensional cyclic array of data.
\author Jan Benda

This is a CyclicArray with the additional feature that some of the array's
capacity is reserved for writing new data. This way new data can be written 
into the array without the need to block reading processes.
*/

template < typename T = double >
class BufferArray : public CyclicArray< T >
{

public:

    /*! Creates an empty BufferArray. */
  BufferArray( void );
    /*! Creates an empty array with capacity \a n data elements
        of wich \a m < \a n are reserved for writing.
	\a m can be at maximum n/2. */
  BufferArray( int n, int m );
    /*! Copy constructor.
        Creates an BufferArray with the same size and content as \a ca. */
  BufferArray( const BufferArray< T > &ba );
    /*! The destructor. */
  virtual ~BufferArray( void );

    /*! Assigns \a a to *this. */
  const BufferArray<T> &operator=( const BufferArray<T> &a );
    /*! Assigns \a a to *this. */
  const BufferArray<T> &assign( const BufferArray<T> &a );

    /*! The number of data elements that are actually stored in the array
        and are available and accessable for a reading process.
        Less or equal than capacity() and size()!
        \sa minIndex(), readSize(), empty() */
  int accessibleSize( void ) const;
    /*! The index of the first accessible data element of a reading process.
        \sa accessibleSize() */
  int minIndex( void ) const;
    /*! Resize the array to \a n data elements
        such that the size() of the array equals \a n.
        Data values are preserved and new data values
	are initialized with \a val.
	The capacity is not changed.
        If, however, the capacity() of the array is zero,
        then memory for \a n data elements is allocated
        and initialized with \a val. 
        The writing index is set to \a n. */
  virtual void resize( int n, const T &val=0 );
    /*! Resize the array to zero length.
        The capacity() remains unchanged. */
  virtual void clear( void );

    /*! If \a n is less than or equal to capacity(), 
        this call has no effect. 
	Otherwise, it is a request for allocation 
	of additional memory. 
	If the request is successful, 
	then capacity() is greater than or equal to \a n; 
	otherwise, capacity() is unchanged. 
	In either case, size() is unchanged and the content
	of the array inclusively newly written data is preserved. */
  virtual void reserve( int n );
    /*! \return the size of the part of the buffer reserved for writing new data. */
  int writeBufferCapacity( void ) const { return NWrite; };
    /*! Set the capacity of the part of the bufer to be used for
        writing new data to \a m. If \a m is greater than half of
        capacity(), then the size of the writing buffer is set to half
        of the capacity(). */
  void setWriteBufferCapacity( int m );

    /*! Add \a val as a new element to the array.
        It will only readable after the read index has been set to
        the current write index using submit(). */
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
    /*! Tell BufferArray that \a n data elements have been added to
        pushBuffer(). The new data elements will only readable after
        the read index has been set to the current write index using
        submit().  \sa maxPush(), pushBuffer() */
  void push( int n );
    /*! Make the written data readable. This is the only action that
        needs to be locked() between writing and reading threads.
        \sa push() */
  void submit( void );

  template < typename TT > 
  friend ostream &operator<<( ostream &str, const BufferArray<TT> &ba );


protected:

    /*! The number of elements reserved for the writing process. */
  int NWrite;
    /*! The number of cycles the writing process ("write index")
        filled the buffer.  \a RCycles and \a R indicate the top index
        until a reading process is allowed to read data. */
  int WCycles;
    /*! The index into the buffer where to append new data. */
  int W;
  
};


typedef BufferArray< double > BufferArrayD;
typedef BufferArray< float > BufferArrayF;
typedef BufferArray< int > BufferArrayI;


template < typename T >
BufferArray< T >::BufferArray( void )
  : CyclicArray<T>(),
    NWrite( 0 ),
    WCycles( 0 ),
    W( 0 )
{
  this->NBuffer = 0;
}


template < typename T >
BufferArray< T >::BufferArray( int n, int m )
  : CyclicArray<T>( n ),
    NWrite( m ),
    WCycles( 0 ),
    W( 0 )
{
  setWriteBufferCapacity( m );
}


template < typename T >
BufferArray< T >::BufferArray( const BufferArray< T > &ba )
  : CyclicArray<T>( ba ),
    NWrite( ba.NWrite ),
    WCycles( ba.WCycles ),
    W( ba.W )
{
}


template < typename T >
BufferArray< T >::~BufferArray( void )
{
}


template < typename T >
const BufferArray<T> &BufferArray<T>::operator=( const BufferArray<T> &a )
{
  return assign( a );
}


template < typename T >
const BufferArray<T> &BufferArray<T>::assign( const BufferArray<T> &a )
{
  if ( &a == this )
    return *this;

  CyclicArray< T >::assign( a );
  if ( a.capacity() > 0 ) {
    NWrite = a.NWrite;
    WCycles = a.WCycles;
    W = a.W;
  }
  else {
    NWrite = 0;
    WCycles = 0;
    W = 0;
  }

  return *this;
}


template < typename T >
int BufferArray< T >::accessibleSize( void ) const
{
  int n = this->RCycles == 0 ? this->R : this->NBuffer;
  return n < this->NBuffer - this->NWrite ? n : this->NBuffer - NWrite;
}


template < typename T >
int BufferArray< T >::minIndex( void ) const
{
  int n = (this->RCycles-1) * this->NBuffer + this->R + this->NWrite;
  return n >= 0 ? n : 0;
}


template < typename T >
void BufferArray< T >::resize( int n, const T &val )
{
  CyclicArray< T >::resize( n, val );
  WCycles = this->RCycles;
  W = this->R;
}


template < typename T >
void BufferArray< T >::clear( void )
{
  CyclicArray< T >::clear();
  WCycles = 0;
  W = 0;
}


template < typename T >
void BufferArray< T >::reserve( int n )
{
  if ( n > this->NBuffer ) {
    T *newbuf = new T[ n ];
    if ( this->Buffer != 0 && this->NBuffer > 0 ) {
      int oln = this->LCycles*this->NBuffer + this->L;
      this->LCycles = (oln-1) / n;
      this->L = 1 + (oln-1) % n;
      int orn = this->size();
      this->RCycles = (orn-1) / n;
      this->R = 1 + (orn-1) % n;
      int owi = W;
      int own = WCycles * NWrite + W;
      WCycles = (own-1) / n;
      W = 1 + (own-1) % n;
      int j = owi;
      int k = W;
      for ( int i=0; i < this->NBuffer; i++ ) {
	if ( j == 0 )
	  j = this->NBuffer;
	if ( k == 0 )
	  k = n;
	j--;
	k--;
	newbuf[k] = this->Buffer[j];
      }
      delete [] this->Buffer;
    }
    this->Buffer = newbuf;
    this->NBuffer = n;
  }
}


template < typename T >
void BufferArray< T >::setWriteBufferCapacity( int m )
{
  NWrite = m;
  if ( NWrite > CyclicArray<T>::capacity()/2 )
    NWrite =  CyclicArray<T>::capacity()/2;
}


template < typename T >
void BufferArray< T >::push( const T &val )
{
  assert( this->Buffer != 0 && this->NBuffer > 0 );
  if ( this->NBuffer <= 0 ) {
    reserve( 100 );
    setWriteBufferCapacity( 25 );
  }

  if ( W >= this->NBuffer ) {
    W = 0;
    WCycles++;
  }

  this->Val = this->Buffer[ W ];
  this->Buffer[ W ] = val;

  W++;
}


template < typename T >
T BufferArray< T >::pop( void )
{
  if ( this->NBuffer <= 0 || W <= 0 )
    return 0;

  W--;

  T v = this->Buffer[ W ];
  this->Buffer[ W ] = this->Val;

  if ( W == 0 && WCycles > 0 ) {
    W = this->NBuffer;
    WCycles--;
  }

  return v;
}


template < typename T >
int BufferArray< T >::maxPush( void ) const
{
  int n = W < this->NBuffer ? this->NBuffer - W : this->NBuffer;
  return n < this->NWrite ? n : this->NWrite;
}


template < typename T >
T *BufferArray< T >::pushBuffer( void )
{
  return W < this->NBuffer ? this->Buffer + W : this->Buffer;
}


template < typename T >
void BufferArray< T >::push( int n )
{
  if ( W >= this->NBuffer ) {
    W = 0;
    WCycles++;
  }

  W += n;

#ifndef NDEBUG
  if ( !( W >= 0 && W <= this->NBuffer ) )
    cerr << "BufferArray::push( int n ): W=" << W << " <0 or > NBuffer=" << this->NBuffer << endl; 
#endif
  assert( ( W >= 0 && W <= this->NBuffer ) );
}


template < typename T >
void BufferArray< T >::submit( void )
{
  this->RCycles = WCycles;
  this->R = W;
}


template < typename T >
ostream &operator<<( ostream &str, const BufferArray< T > &ba )
{
  str << CyclicArrayF( ba );
  str << "NWrite: " << ba.NWrite << '\n';
  str << "WCycles: " << ba.WCycles << '\n';
  str << "W: " << ba.W << '\n';
  return str;
}


}; /* namespace relacs */

#endif /* ! _RELACS_BUFFERARRAY_H_ */

