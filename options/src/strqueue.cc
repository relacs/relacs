/*
  strqueue.cc
  A list of strings.

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

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <relacs/strqueue.h>

namespace relacs {


string StrQueue::StopEmpty = "EMPTY";


StrQueue::StrQueue( const Str &s, const string &separator )
  : StrDeque()
{
  assign( s, separator );
}


StrQueue &StrQueue::assign( const Str &s, const string &separator )
{
  clear();

  return append( s, separator );
}


StrQueue &StrQueue::assign( const string &s )
{
  clear();

  add( s );

  return *this;
}


StrQueue &StrQueue::append( const Str &s, const string &separator )
{
  if ( separator.empty() ) {
    add( s );
    return *this;
  }

  int index = -1;
  int word = 0;
  do {
    word = s.nextField( index, separator );
    if ( word >= 0 ) {
      if ( word == index )
	add( "" );
      else
	add( s.mid( word, index-1 ) );
    }
  } while ( word >= 0 );

  return *this;
}


StrQueue &StrQueue::copy( string &s, const string &separator )
{
  s = "";
  int n=0;
  for ( iterator i = begin(); i != end(); ++i, ++n ) {
    if ( n > 0 )
      s += separator;
    s += *i;
  }
  return *this;
}


bool operator==( const StrQueue &sq1, const StrQueue &sq2 )
{
  if ( sq1.size() != sq2.size() )
    return false;

  for ( int k=0; k<sq1.size(); k++ )
    if ( sq1[k] != sq2[k] )
      return false;

  return true;
}


int StrQueue::find( const string &strg, int index ) const
{
  for ( int k=index; k<size(); k++ ) {
    if ( ( operator[]( k ).empty() && strg.empty() ) ||
	 operator[]( k ) == strg )
      return k;
  }

  return -1;
}


void StrQueue::add( const StrQueue &sq )
{
  for ( const_iterator i = sq.begin(); i != sq.end(); ++i ) {
    push_back( *i );
  }
}


void StrQueue::insert( const StrQueue &sq )
{
  for ( const_iterator i = sq.end()-1; i >= sq.begin(); --i ) {
    push_front( *i );
  }
}


StrQueue::iterator StrQueue::erase( iterator pos )
{
  if ( pos >= begin() && pos < end() )
    return StrDeque::erase( pos );
  else
    return pos;
}


void StrQueue::erase( int index )
{
  if ( index >= 0 && begin() + index < end() )
    StrDeque::erase( begin() + index );
}


StrQueue::iterator StrQueue::erase( iterator first, iterator last )
{
  if ( first < begin() )
    first = begin();
  if ( last > end() )
    last = end();
  return StrDeque::erase( first, last );
}


StrQueue::iterator StrQueue::erase( int from, int to )
{ 
  if ( from < 0 )
    from = 0;
  if ( to > size() )
    to = size();
  return StrDeque::erase( begin() + from, begin() + to );
}


void StrQueue::reverse( void )
{
  std::reverse( begin(), end() );
}


void StrQueue::sort( void )
{
  std::sort( begin(), end() );
}


StrQueue &StrQueue::strip( void )
{
  // leading lines:
  for ( iterator i = begin(); i != end(); ) {
    if ( (*i).empty() ) {
      i = erase( i );
    }
    else
      break;
  }
  // trailing lines:
  for ( iterator i = end()-1; i >= begin(); --i ) {
    if ( (*i).empty() ) {
      erase( i );
    }
    else
      break;
  }
  return *this;
}


StrQueue &StrQueue::stripComments( const string &comment )
{
  for ( iterator i = begin(); i != end(); ++i ) {
    (*i).stripComment( comment );
  }
  return *this;
}


StrQueue StrQueue::strippedComments( const string &comment ) const
{
  StrQueue sq = StrQueue();
  for ( const_iterator i = begin(); i != end(); ++i ) {
    sq.add( (*i).strippedComment( comment ) );
  }
  return sq;
}


string StrQueue::save( const string &separator ) const
{
  string s = "";
  int k=0;
  for ( StrDeque::const_iterator p=begin(); p != end(); ++p ) {
    if ( k > 0 )
      s += separator;
    s += *p;
    k++;
  }
  return s;
}


istream &StrQueue::load( istream &str, const string &stop, string *line )
{
  string s;
  bool stopempty = ( stop == StopEmpty );

  // read first line:
  if ( line != 0 && !line->empty() ) { 
    // load string:
    add( *line );
  }
 
  // get line:
  while ( getline( str, s ) ) {

    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
	 ( stopempty && int(s.find_first_not_of( " \t\n\r\v\f" )) < 0 ) )
      break;

    // load string:
    add( s );
  }
  
  // store last read line:
  if ( line != 0 )
    *line = s;

  return str;
}


ostream &StrQueue::save( ostream &str, const string &start ) const
{
  for ( StrDeque::const_iterator p=begin(); p != end(); ++p )
    str << start << *p << endl;
  return str;
}


ostream &operator<< ( ostream &str, const StrQueue &sq )
{
  sq.save( str );

  return str;
}


}; /* namespace relacs */

