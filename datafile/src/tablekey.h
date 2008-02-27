/*
  tablekey.h
  Handling a table header

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

#ifndef _TABLEKEY_H_
#define _TABLEKEY_H_


#include <vector>
#include "array.h"
#include "options.h"
using namespace std;

class StrQueue;
class TableData;

/*!
\class TableKey
\author Jan Benda
\version 1.0
\brief Handling a table header
*/

class TableKey
{

public:

  TableKey( void );
  ~TableKey( void );

    /*! Add a new number option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  Parameter &addNumber( const string &ident, const string &unit="", 
			const string &format="", int flags=0 );
    /*! Insert a new number option at the beginning of the options list
        (\a atindent == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident.
        The option has a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  Parameter &insertNumber( const string &ident, const string &atident="", 
			   const string &unit="", const string &format="",
			   int flags=0 );
    /*! Set the value of an existing number option 
        with identifier \a ident to \a number. */
  Parameter &setNumber( const string &ident, double number,
			const string &unit="" );
    /*! Set the value of an existing number option 
        with identifier \a ident to \a number. */
  Parameter &setInteger( const string &ident, long number,
			 const string &unit="" );

    /*! Add a new text option at the end of the options list. 
        Its identifier string and request string is set to \a ident,
	the unit to "-".
        The option has format string \a format (%s). */
  Parameter &addText( const string &ident, const string &format="",
		      int flags=0 );
    /*! Add a new text option at the end of the options list. 
        Its identifier string and request string is set to \a ident,
	the unit to "-".
        The option is formatted to a \t abs( \a width \t ) long string.
        If \a width is positive then the string is aligned to the right,
        if \a width is positive it is aligned to the left. */
  Parameter &addText( const string &ident, int width, int flags=0 );
    /*! Insert a new text option at the beginning of the options list
        (\a atindent == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident,
	the unit to "-".
        The option has a format string \a format (%s). */
  Parameter &insertText( const string &ident, const string &atident="", 
			 const string &format="", int flags=0 );
    /*! Insert a new text option at the beginning of the options list
        (\a atindent == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident,
	the unit to "-".
        The option is formatted to a \a width long string. */
  Parameter &insertText( const string &ident, const string &atident, 
			 int width, int flags=0 );
    /*! Set the value of an existing number option 
        with identifier \a ident to \a number. */
  Parameter &setText( const string &ident, const string &text );

    /*! Add a label \a ident at the end of the options list. */
  Parameter &addLabel( const string &ident, int flags=0 );
    /*! Insert a new label \a ident at the beginning of the options list
        (\a atindent == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list. */
  Parameter &insertLabel( const string &ident, const string &atident="",
			  int flags=0 );

    /*! Returns the column whose identifier string matches \a pattern.
        The first column is 0. */
  int column( const string &pattern ) const;
    /*! Returns the column whose identifier string matches \a pattern.
        The first column is 0. */
  inline int operator()( const string &pattern ) const
    { return column( pattern ); };

    /*! Returns the identifier string of the \a column-th column. */
  Str ident( int column ) const;
    /*! Returns the identifier string of the column specified by \a pattern.
        \sa column() */
  Str ident( const string &pattern ) const;
    /*! Set the identifier string of the \a column-th column to \a ident. */
  Parameter &setIdent( int column, const string &ident );
    /*! Set the identifier string of the column specified by \a pattern 
        to \a ident. \sa column() */
  Parameter &setIdent( const string &pattern, const string &ident );
    /*! Returns the unit of the \a column-th column. */
  Str unit( int column ) const;
    /*! Returns the unit of the column specified by \a pattern.
        \sa column() */
  Str unit( const string &pattern ) const;
    /*! Set the unit of the \a column-th column to \a unit. */
  Parameter &setUnit( int column, const string &unit );
    /*! Set the unit of the column specified by \a pattern to \a unit.
        \sa column() */
  Parameter &setUnit( const string &pattern, const string &unit );
    /*! Returns the format string of the \a column-th column. */
  Str format( int column ) const;
    /*! Returns the format string of the column specified by \a pattern.
        \sa column() */
  Str format( const string &pattern ) const;
    /*! Returns the width defined in the format string of the \a column-th column. */
  int formatWidth( int column ) const;
    /*! Returns the width defined in the format string of the column specified by \a pattern.
        \sa column() */
  int formatWidth( const string &pattern ) const;
    /*! Set the format string of the \a column-th column to \a format. */
  Parameter &setFormat( int column, const string &format );
    /*! Set the format string of the column specified by \a pattern 
        to \a format. \sa column() */
  Parameter &setFormat( const string &pattern, const string &format );
    /*! Returns \c true if the column \a column contains numbers. */
  bool isNumber( int column ) const;
    /*! Returns \c true if the column specified by \a pattern contains numbers.
        \sa column() */
  bool isNumber( const string &pattern ) const;
    /*! Returns \c true if the column \a column contains strings. */
  bool isText( int column ) const;
    /*! Returns \c true if the column specified by \a pattern contains strings.
        \sa column() */
  bool isText( const string &pattern ) const;
    /*! Returns the group identifier string of level \a level
        for the \a column-th column. 
        \a level = 0 returns the identifier sting of the column. */
  Str group( int column, int level=1 ) const;
    /*! Returns the group identifier string of level \a level
        for the column specified by \a pattern. 
        \a level = 0 returns the identifier sting of the column.
	\sa column() */
  Str group( const string &pattern, int level=1 ) const;
    /*! Set the group identifier string of level \a level
        for the \a column-th column to \a group. 
        \a level = 0 sets the identifier sting of the column. */
  Parameter &setGroup( int column, const string &group, int level=1 );
    /*! Set the group identifier string of level \a level
        for the column specified by \a pattern to \a group. 
        \a level = 0 sets the identifier sting of the column.
	\sa column() */
  Parameter &setGroup( const string &pattern, const string &group, int level=1 );

    /*! Get \a i-th column. */
  const Parameter &operator[]( int i ) const;
    /*! Get \a i-th column. */
  Parameter &operator[]( int i );
    /*! Get the column specified by \a pattern. \sa column() */
  const Parameter &operator[]( const string &pattern ) const;
    /*! Get the column specified by \a pattern. \sa column() */
  Parameter &operator[]( const string &pattern );

    /*! The number of columns the TableKey describes. */
  int columns( void ) const { return Columns.size(); };
    /*! The number of levels of the TableKey.
        0: No columns are described by the TableKey,
	1: Each column has an identifier, but columns are not grouped.
        2: Columns are grouped.
        3: Groups of columns are grouped... */
  int level( void ) const;

    /*! Returns true if the TableKey does not describe any columns.
        However, it can already contain descriptions of groups. */     
  bool empty( void ) const { return Columns.empty(); };
    /*! Clears the TableKey. Erases all column and group descriptions. */
  void clear( void );

    /*! Write the table header to \a str.
        If \a key equals true, it is preceded by a line "Key".
	If \ num equals true, a line indicating the column numbers is added.
        Each line is preceded by the \a KeyStart string. */
  ostream &saveKey( ostream &str, bool key=true, bool num=true, int flags=0 ) const;
    /*! Write the table header to \a str using saveKey() with the default settings. */
  friend ostream &operator<< ( ostream &str, const TableKey &tk );
    /*! Load the table header from \a sq. */
  TableKey &loadKey( const StrQueue &sq );

    /*! Write the data stored in the TableKey
        as one line of the table to \a str.
        These are the data that were set with setNumber() or setText(). */
  ostream &saveData( ostream &str );
    /*! Write the data stored in the TableKey columns \a from to \a to
        as one line of the table to \a str (no trailing newline).
        These are the data that were set with setNumber() or setText(). */
  ostream &saveData( ostream &str, int from, int to );
    /*! Write the data stored in the TableKey to \a str.
        Each datum is written into a separate line
	and is preceeded by its identifier.
	Each line is started with \a start.
	The identifiers are written into a field of width \a width characters.
        These are the data that were set with setNumber(). */
  ostream &saveMetaData( ostream &str, const string &start="",
			 int width=-1 ) const;

    /*! Write the number \a v to the output stream \a str 
        using the format for column \a c.
        If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the number.
	If the output column does not exist,
	nothing is written to \a str. */
  ostream &save( ostream &str, double v, int c=-1 ) const;
    /*! Write the \a n numbers \a v to the output stream \a str 
        using the formats starting at column \a c.
        If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the numbers.
	The numbers are separeted by \a Separator.
	If an output column does not exist,
	it is not written to \a str. */
  template < typename T >
  ostream &save( ostream &str, const T *v, int n, int c=-1 ) const;
    /*! Write the numbers of the vector \a vec to the output stream \a str
        using the formats starting at column \a c.
        If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a Start, otherwise \a Separator is written right before the numbers.
	The numbers are separeted by \a Separator.
	If an output column does not exist,
	it is not written to \a str. */
  ostream &save( ostream &str, const vector< double > &vec, int c=-1 ) const
    { return saveVector( str, vec, c ); };
  ostream &save( ostream &str, const vector< float > &vec, int c=-1 ) const
    { return saveVector( str, vec, c ); };
  template < typename T >
  ostream &save( ostream &str, const Array< T > &vec, int c=-1 ) const;
    /*! Write the numbers of the row \a r of \a v
        to the output stream \a str 
	using the formats starting at column \a c.
	If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the numbers.
	The numbers are separeted by \a Separator.
	If an output column does not exist,
	it is not written to \a str. */
  template < typename T >
  ostream &save( ostream &str, const vector< vector< T > > &v, int r, int c=0 ) const;
    /*! Write the numbers of the row \a r of \a v
        to the output stream \a str 
	using the formats starting at column \a c.
	If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the numbers.
	The numbers are separeted by \a Separator.
	If an output column does not exist,
	it is not written to \a str. */
  template < typename T >
  ostream &save( ostream &str, const vector< Array< T > > &v, int r, int c=0 ) const;
    /*! Write the numbers of the row \a r of \a table
        to the output stream \a str 
	using the formats starting at column \a c.
	If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the numbers.
	The numbers are separeted by \a Separator.
	If an output column does not exist,
	it is not written to \a str. */
  ostream &save( ostream &str, const TableData &table, int r, int c=0 ) const;
    /*! Write the whole table \a table to the output stream \a str 
	using the formats of the TableKey.
	\a DataStart is written at the beginning of each line
        and \a Separator is written between the numbers.
	If an output column does not exist,
	it is not written to \a str. */
  ostream &save( ostream &str, const TableData &table ) const;
    /*! Write the string \a text to the output stream \a str 
        using the format for column \a c.
        If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the string.
	If the output column does not exist,
	nothing is written to \a str.
        If \a text is empty, the missing-string is written instead ( see setMissing() ). 
        The whole string is written, even if it is longer than the width of the column! */
  ostream &save( ostream &str, const string &text, int c=-1 ) const;
    /*! Skip one column of automatically formatted output.
        Nothing is written, just the current column is incremented. */
  void saveSkip( void ) const { PrevCol++; };
    /*! Return the column number of the last output 
        via one of the save() functions. */
  int saveColumn( void ) const { return PrevCol; };
    /*! Set the column number of the last output to \a col. */
  void setSaveColumn( int col );

    /*! The string indicating a comment. */
  string comment( void ) const { return Comment; };
    /*! Set the string for indicating comments to \a comment
        and the start string to \a comment + " ". */
  void setComment( const string &comment ) { Comment = comment; KeyStart = comment + " "; };

    /*! The string introducing each line of the table header. */
  string keyStart( void ) const { return KeyStart; };
    /*! Set the string introducing each line of the table header to \a start. */
  void setKeyStart( const string &start ) { KeyStart = start; };

    /*! The string introducing each line of data. */
  string dataStart( void ) const { return DataStart; };
    /*! Set the string introducing each line of data to \a start. */
  void setDataStart( const string &start ) { DataStart = start; };

    /*! The string separating two columns. */
  string separator( void ) const { return Separator; };
    /*! Set the string separating two columns to \a separator. */
  void setSeparator( const string &separator ) { Separator = separator; };

    /*! The string indicating a missing data value. */
  string missing( void ) const { return Missing; };
    /*! Set the string indicating missing data values to \a missing. */
  void setMissing( const string &missing ) { Missing = missing; };


 private:

  void init( void );
  template < typename T >
  ostream &saveVector( ostream &str, const vector< T > &vec, int c=-1 ) const;

  Options Opt;

  vector < vector < Options::iterator > > Columns;
  vector < int > Width;
  mutable int PrevCol;

  Parameter Dummy;

  string Comment;
  string KeyStart;
  string DataStart;
  string Separator;
  string Missing;

};


template < typename T >
ostream &TableKey::save( ostream &str, const T *v, int n, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 )
    return str;

  const T *vp = v;
  for ( int k=0; k < n; k++, ++vp ) {
    if ( c >= (int)Columns.size() )
      return str;
    if ( c > 0 )
      str << Separator;
    else
      str << DataStart;
    Str s( *vp, format( c ) );
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
    PrevCol = c;
    c++;
  }
  return str;  
}


template < typename T >
ostream &TableKey::saveVector( ostream &str, const vector< T > &vec, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 )
    return str;

  for ( typename vector< T >::const_iterator vp = vec.begin();
	vp != vec.end();
	++vp ) {
    if ( c >= (int)Columns.size() )
      return str;
    if ( c > 0 )
      str << Separator;
    else
      str << DataStart;
    Str s( *vp, format( c ) );
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
    PrevCol = c;
    c++;
  }
  return str;  
}


template < typename T >
ostream &TableKey::save( ostream &str, const Array< T > &vec, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 )
    return str;

  for ( typename Array< T >::const_iterator vp = vec.begin();
	vp != vec.end();
	++vp ) {
    if ( c >= (int)Columns.size() )
      return str;
    if ( c > 0 )
      str << Separator;
    else
      str << DataStart;
    Str s( *vp, format( c ) );
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
    PrevCol = c;
    c++;
  }
  return str;  
}


template < typename T >
ostream &TableKey::save( ostream &str, const vector< vector< T > > &v,
			 int r, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 )
    return str;

  for ( unsigned int k=0; k<v.size(); k++ ) {
    if ( c >= (int)Columns.size() )
      return str;
    if ( c > 0 )
      str << Separator;
    else
      str << DataStart;
    Str s( r < (int)v[k].size() ? v[k][r] : 0.0, format( c ) );
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
    PrevCol = c;
    c++;
  }
  return str;  
}


template < typename T >
ostream &TableKey::save( ostream &str, const vector< Array< T > > &v,
			 int r, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 )
    return str;

  for ( unsigned int k=0; k<v.size(); k++ ) {
    if ( c >= (int)Columns.size() )
      return str;
    if ( c > 0 )
      str << Separator;
    else
      str << DataStart;
    Str s( r < (int)v[k].size() ? v[k][r] : 0.0, format( c ) );
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
    PrevCol = c;
    c++;
  }
  return str;  
}


#endif
