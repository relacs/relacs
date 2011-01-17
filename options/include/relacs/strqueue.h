/*
  strqueue.h
  A list of strings.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_STRQUEUE_H_
#define _RELACS_STRQUEUE_H_ 1

#include <deque>
#include <relacs/str.h>
using namespace std;

namespace relacs {

/*!
\class StrQueue
\author Jan Benda
\version 1.0
\brief A list of strings.
*/


typedef deque< Str > StrDeque;

class StrQueue : public StrDeque
{

 public:

    /*! Constructs an empty list of strings. */
  StrQueue( void ) : StrDeque() {};
    /*! Copy constructor. */
  StrQueue( const StrQueue &sq ) : StrDeque( sq ) {};
    /*! Assign the fields of \a s separated by \a separator to
        the StrQueue. */
  StrQueue( const Str &s, const string &separator );
    /*! Deconstructs a StrQueue. */
  ~StrQueue( void ) {};

    /*! Assigns \a sq to the list. */
  inline StrQueue &operator=( const StrQueue &sq )
    { return (StrQueue&)StrDeque::operator=( sq ); };

    /*! Create a list with the single element \a s. */
  inline StrQueue &operator=( const string &s ) { return assign( s ); };
    /*! Create a list with the single element \a s. */
  StrQueue &assign( const string &s );

    /*! Assign the fields of \a s separated by \a separator to
        the StrQueue. */
  StrQueue &assign( const Str &s, const string &separator );

    /*! Append the fields of \a s separated by \a separator to
        the StrQueue. */
  StrQueue &append( const Str &s, const string &separator );

    /*! Write the elements of the StrQueue into \a s separated by \a separator. */
  StrQueue &copy( string &s, const string &separator );

    /*! The number of strings in the list. */
  inline int size( void ) const { return StrDeque::size(); };
    /*! True if there aren't any strings contained in the list. */
  inline bool empty( void ) const { return StrDeque::empty(); };

    /*! Inserts or erases elements \a s at the end
        such that the size becomes \a n. */
  inline void resize( int n, const Str &s = Str() )
    { StrDeque::resize( n, s ); };
    /*! Erases all of the strings in the list. */
  inline void clear( void ) { StrDeque::clear(); };

    /*! Returns the string at index \a index. */
  inline Str &operator[]( int index ) { return StrDeque::operator[]( index ); };
    /*! Returns the string at index \a index. */
  inline const Str &operator[]( int index ) const { return StrDeque::operator[]( index ); };

    /*! Return the index of the string that matches \a strg. 
        If there is no matching \a strg -1 is returned. 
        Searching is started at the \a index-th element. */
  int find( const string &strg , int index=0 ) const;

    /*! Compare two StrQueue.
        Returns \c true if they contain the same number of
        strings and all of the strings are equal. */
  friend bool operator==( const StrQueue &sq1, const StrQueue &sq2 );

    /*! Add string \a s to the end of the list. */
  inline void add( const string &s ) { push_back( s ); };
    /*! Add StrQueue \a sq to the list. */
  void add( const StrQueue &sq );

    /*! Insert string \a s at the beginning of the list. */
  inline void insert( const string &s ) { push_front( s ); };
    /*! Insert StrQueue \a sq at the beginning of the list. */
  void insert( const StrQueue &sq );

    /*! Erases the string at position \a pos. */
  iterator erase( iterator pos );
    /*! Erases the string at index \a index. */
  void erase( int index );
    /*! Erases all strings within the range \a first to \a last exclusively. */
  iterator erase( iterator first, iterator last );
    /*! Erases all strings within the range \a from to \a to exclusively. */
  iterator erase( int from, int to );

    /*! Reverses the order of strings in the list. */
  void reverse( void );
    /*! Sort the strings in the list. */
  void sort( void );

    /*! Remove leading and trailing empty lines. */
  StrQueue &strip( void );

    /*! Removes comments from each string.
        If \a comment is set to 'XXX' everything behind 'XXX' inclusively
        is erased from each string.
        If \a comment is set to '-XXX' everything before 'XXX' inclusively
        is erased from each string. */
  StrQueue &stripComments( const string &comment=Str::comment() );
    /*! Returns the StrQueue with comments removed.
        If \a comment is set to 'XXX' everything behind 'XXX' inclusively
        is erased from each string.
        If \a comment is set to '-XXX' everything before 'XXX' inclusively
        is erased from each string. */
  StrQueue strippedComments( const string &comment=Str::comment() ) const;

    /* Returns a string with all the elements of te list 
       separated by \a separator. */
  string save( const string &separator=" " ) const;

    /*! Read from stream \a str until end of file
        or a line beginning with \a stop is reached. 
	If \a stop equals the \a StopEmpty string,
	reading is stopped at an empty line.
        Each line is appended to the list.
	If \a line does not equal zero
	then the last read line is returned in \a line. */
  istream &load( istream &str, const string &stop="", string *line=0 );
    /* Write the whole StrQueue into stream \a str. 
       Start each line with string \a start. */
  ostream &save( ostream &str, const string &start="" ) const;

    /*! Write the whole StrQueue into stream \a str. */
  friend ostream &operator<< ( ostream &str, const StrQueue &sq );

    /*! Stop-string for load() to stop reading at an empty line. */
  static string StopEmpty;

};


}; /* namespace relacs */

#endif /* ! _RELACS_STRQUEUE_H_ */
