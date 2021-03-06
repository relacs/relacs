/*
  tablekey.h
  Handling a table header

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#ifndef _RELACS_TABLEKEY_H_
#define _RELACS_TABLEKEY_H_ 1


#include <deque>
#include <vector>
#include <relacs/array.h>
#include <relacs/options.h>
using namespace std;

namespace relacs {


class StrQueue;
class TableData;

/*!
\class TableKey
\author Jan Benda
\brief Handling a table header
*/

class TableKey
{

public:

    /*! Construct an empty TableKey. */
  TableKey( void );
    /*! Copy-constructor. */
  TableKey( const TableKey &key );
    /*! Construct an TableKey from Options \a o. */
  TableKey( const Options &o );
    /*! Destructor. */
  ~TableKey( void );

    /*! Add a new column for numbers with name name() and value \a value to the key. 
        The column has a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  Parameter &addNumber( const string &name, const string &unit="", 
			const string &format="", int flags=0, double value=0.0 );
    /*! Add a new column with name name() and value \a value to the key. 
        The column has a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  Parameter &addNumber( const string &name, const string &unit, 
			const string &format, double value, int flags=0 );
    /*! Insert a new column for numbers with name name() and value value()
        at the beginning of the key
        (\a atindent == "") or at the position of the column with
        name \a atname. If the column with name \a atname
        does not exist, the column is appended to the end of the key.
        The column has a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  Parameter &insertNumber( const string &name, const string &atname="", 
			   const string &unit="", const string &format="",
			   int flags=0 );
    /*! Set the value of an existing colum 
        with name \a name to \a number. */
  Parameter &setNumber( const string &name, double number,
			const string &unit="" );
    /*! Set the value of an existing column 
        with name \a name to \a number. */
  Parameter &setInteger( const string &name, long number,
			 const string &unit="" );

    /*! Add a new column for strings with name name() and value \a value to the key. 
        The column has a format string \a format 
	(ANSI C printf() syntax, only %%s)
	and its unit is set to '-'. */
  Parameter &addText( const string &name, const string &format="",
		      int flags=0, const string &value="" );
    /*! Add a new column for strings with name name() and value \a value to the key. 
        The column has a format string \a format 
	(ANSI C printf() syntax, only %%s)
	and its unit is set to '-'. */
  Parameter &addText( const string &name, const string &format,
		      const string &value, int flags=0 );
    /*! Add a new column for strings with name name() and value \a value to the key. 
	The unit is set to '-'.
        The column is formatted to a \c abs( \c width \c ) long string.
        If \a width is positive then the string is aligned to the right,
        if \a width is positive it is aligned to the left. */
  Parameter &addText( const string &name, int width, int flags=0,
		      const string &value="" );
    /*! Add a new column for strings with name name() and value \a value to the key. 
	The unit is set to '-'.
        The column is formatted to a \c abs( \c width \c ) long string.
        If \a width is positive then the string is aligned to the right,
        if \a width is positive it is aligned to the left. */
  Parameter &addText( const string &name, int width, const string &value,
		      int flags=0 );
    /*! Insert a new column for strings with name name() and value \a value
        at the beginning of the key
        (\a atindent == "") or at the position of the column with
        name \a atname. If the column with name \a atname
        does not exist, the column is appended to the end of the key.
        The column has a format string \a format 
	(ANSI C printf() syntax, only %%s)
	and its unit is set to '-'. */
  Parameter &insertText( const string &name, const string &atname="", 
			 const string &format="", int flags=0 );
    /*! Insert a new column for strings with name name() and value \a value
        at the beginning of the key
        (\a atindent == "") or at the position of the column with
        name \a atname. If the column with name \a atname
        does not exist, the column is appended to the end of the key.
	The unit is set to '-'.
        The column is formatted to a \c abs( \c width \c ) long string.
        If \a width is positive then the string is aligned to the right,
        if \a width is positive it is aligned to the left. */
  Parameter &insertText( const string &name, const string &atname, 
			 int width, int flags=0 );
    /*! Set the value of an existing tet column 
        with name \a name to \a text. */
  Parameter &setText( const string &name, const string &text );

    /*! Add a new section \a name with level \a level to the key.
        \a level = 0 is the top-level section.
        Subsequent calls to addNumber() adds columns to this section.
        \sa newSubSection(), newSubSubSection(), insertSection() */
  Options &newSection( int level, const string &name, int flags=0 );
    /*! Add a new top-level section \a name to the key.
        Subsequent calls to addNumber() adds columns to this section.
        \sa newSubSection(), newSubSubSection(), insertSection() */
  Options &newSection( const string &name, int flags=0 );
    /*! Add a new second-level section \a name to the key.
        Subsequent calls to addNumber() adds columns to this section.
        \sa newSection(), newSubSubSection(), insertSection() */
  Options &newSubSection( const string &name, int flags=0 );
    /*! Add a new third-level section \a name to the key.
        Subsequent calls to addNumber() adds columns to this section.
        \sa newSection(), newSubSection(), insertSection() */
  Options &newSubSubSection( const string &name, int flags=0 );
    /*! Insert a new section of columns before the section
        specified by \a atpattern.
	If \a atpattern is not found or if \a atname is empty,
	the new section is added to the beginning or the end
	of the currently active section, respectively.
        The new section is named \a name and has some \a flag for selecting this section.
        Subsequent calls to addNumber() adds columns to the inserted section.
        \sa newSection(), newSubSection(), newSubSubSection() */
  Options &insertSection( const string &name, const string &atname="",
			int flags=0 );

    /*! Add \a opt as a new subsection of level \a level. Only
        name-value pairs and sections as specified by \a selectmask
        are taken from \a opt. If \a name is not an empty string, the
        name of the new section is set to \a name.  \a flag is added
        to the new section's \a flag, respectively.  \a level = 0 is
        the upper level, i.e. a new section is added.  Higher \a
        levels add sections lower in the hierachy, i.e. \a level = 1
        adds a subsection, \a \a level = 2 a subsubsection, etc.
        Subsequent calls to addText(), addNumber(), etc. add new
        Parameter to the added section.  \sa newSubSection(),
        newSubSubSection() */
  Options &newSection( int level, const Options &opt, int selectmask=0,
		       const string &name="", int flag=0 );
    /*! Add \a opt as a new section to the end of this section
        list. Only name-value pairs and sections as specified by \a
        selectmask are taken from \a opt.  If \a name is not an empty
        string, the name of the new section is set to \a name.
	\a flag is added to the new section's \a flag.  Subsequent
        calls to addText(), addNumber(), etc. add new Parameter to the
        added section.  \sa newSubSection(), newSubSubSection() */
  Options &newSection( const Options &opt, int selectmask=0,
		       const string &name="", int flag=0 );
    /*! Add \a opt as a new subsection to the last section. Only
        name-value pairs and sections as specified by \a selectmask
        are taken from \a opt.  If \a name is not an empty string, the
        name of the new section is set to \a name. 
	\a flag is added to the new section's flag. Subsequent calls to
        addText(), addNumber(), etc. add new Parameter to the added
        section.  \note You can only add a subsection after having
        added a section!  \sa newSection(), newSubSubSection() */
  Options &newSubSection( const Options &opt, int selectmask=0,
			  const string &name="", int flag=0 );
    /*! Add \a opt as a new subsubsection to the last subsection of
        the last section. Only name-value pairs and sections as
        specified by \a selectmask are taken from \a opt.  If \a name
        is not an empty string, the name of the new section is set to
        \a name. \a flag are added to the new section's flag.
        Subsequent calls to addText(), addNumber(), etc. add new
        Parameter to the added section.  \note You can only add a
        subsubsection after having added a subsection!  \sa
        newSection(), newSubSection() */
  Options &newSubSubSection( const Options &opt, int selectmask=0,
			     const string &name="", int flag=0 );

    /*! Adds all Parameters and sections from \a opts
        to the key as specified by \a selectflag.
        \sa Options::append(const Options&, int) */
  void add( const Options &opts, int selectflag=0 );
    /*! Insert all Parameters and sections from \a opts
        at the beginning of the key
        (\a atname == "") or at the position of the column or section specifier with
        name \a atname. If the column or section with name \a atname
        does not exist, the options are appended to the end of the key.
       \sa Options::insert(const Options&, const string& ) */
  void insert( const Options &opts, const string &atname="" );
    /*! Insert all Parameter and sections from \a opts as
        specified by \a selectflag at the beginning of the key
        (\a atname == "") or at the position of the column or section specifier with
        name \a atname. If the column or section with name \a atname
        does not exist, the options are appended to the end of the key.
       \sa Options::insert(const Options&, int, const string& ) */
  void insert( const Options &opts, int selectflag, const string &atname="" );

    /*! Erase the column \a column from the key. */
  void erase( int column );
    /*! Erase the column whose name string matches \a pattern from the key. */
  void erase( const string &pattern );

    /*! Returns the column whose name string matches \a pattern.
        The first column is 0. */
  int column( const string &pattern ) const;
    /*! Returns the column whose name string matches \a pattern.
        The first column is 0. */
  inline int operator()( const string &pattern ) const
    { return column( pattern ); };

    /*! Returns the name string of the \a column-th column. */
  Str name( int column ) const;
    /*! Returns the name string of the column specified by \a pattern.
        \sa column() */
  Str name( const string &pattern ) const;
    /*! Set the name string of the \a column-th column to \a name. */
  Parameter &setName( int column, const string &name );
    /*! Set the name string of the column specified by \a pattern 
        to \a name. \sa column() */
  Parameter &setName( const string &pattern, const string &name );
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
    /*! Returns the section name string of level \a level
        for the \a column-th column. 
        \a level = 0 returns the name string of the column. */
  Str sectionName( int column, int level=1 ) const;
    /*! Returns the section name string of level \a level
        for the column specified by \a pattern. 
        \a level = 0 returns the name string of the column.
	\sa column() */
  Str sectionName( const string &pattern, int level=1 ) const;
    /*! Set the section name string of level \a level
        for the \a column-th column to \a section. 
        \a level = 0 sets the name string of the column. */
  void setSectionName( int column, const string &section, int level=1 );
    /*! Set the section name string of level \a level
        for the column specified by \a pattern to \a section. 
        \a level = 0 sets the name string of the column.
	\sa column() */
  void setSectionName( const string &pattern, const string &section, int level=1 );

    /*! Returns the section at \a column column and level \a level
        with all its subsections and column labels.  \a level = 0
        returns an empty Options. */
  const Options &subSection( int column, int level=1 ) const;
  Options &subSection( int column, int level=1 );
    /*! Returns the section at the column specified by \a pattern and
        level \a level with all its subsections and column labels.  \a
        level = 0 returns an empty Options. */
  const Options &subSection( const string &pattern, int level=1 ) const;
  Options &subSection( const string &pattern, int level=1 );

    /*! Get \a i-th column. */
  const Parameter &operator[]( int i ) const;
    /*! Get \a i-th column. */
  Parameter &operator[]( int i );
    /*! Get the column specified by \a pattern. \sa column() */
  const Parameter &operator[]( const string &pattern ) const;
    /*! Get the column specified by \a pattern. \sa column() */
  Parameter &operator[]( const string &pattern );

    /*! The number of columns the TableKey describes. */
  int columns( void ) const;
    /*! The number of levels of the TableKey.
        0: No columns are described by the TableKey,
	1: Each column has an name, but columns are not grouped in sections.
        2: Columns are grouped in sections.
        3: Groups of columns are grouped in sections ... */
  int level( void ) const;

    /*! Returns true if the TableKey does not describe any columns.
        However, it can already contain descriptions of sections. */     
  bool empty( void ) const;
    /*! Clears the TableKey. Erases all column and section descriptions. */
  void clear( void );

    /*! Write the table header to \a str.
        If \a key equals true, it is preceded by a line "Key".
	If \a num equals true, a line indicating the column numbers is added.
	If \a units equals true, a line indicating the units
	for the column entries is added.
        Each line is preceded by the \a KeyStart string. */
  ostream &saveKey( ostream &str, bool key=true, bool num=true,
		    bool units=true, int flags=0 ) const;
    /*! Write the table header as a LaTeX table header to \a str.
	If \a num equals true, a line indicating the column numbers is added.
	If \a units equals true, a line indicating the units
	for the column entries is added. */
  ostream &saveKeyLaTeX( ostream &str, bool num=true,
			 bool units=true, int flags=0 ) const;
    /*! Write the table header as a HTML table header to \a str.
	If \a num equals true, a line indicating the column numbers is added.
	If \a units equals true, a line indicating the units
	for the column entries is added. */
  ostream &saveKeyHTML( ostream &str, bool num=true, bool units=true,
			int flags=0 ) const;
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
	and is preceeded by its name.
	Each line is started with \a start.
        These are the data that were set with setNumber(). */
  ostream &saveMetaData( ostream &str, const string &start="" ) const;

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
  ostream &save( ostream &str, const TableData &table, int r, int c=-1 ) const;
    /*! Write the numbers from colum \a cbegin to column \a cend (exclusively)
        of the row \a r of \a table
        to the output stream \a str 
	using the formats starting at column \a c.
	If \a cend is negative, it is set behind the last column of the table.
	If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the numbers.
	The numbers are separeted by \a Separator.
	If an output column does not exist,
	it is not written to \a str. */
  ostream &save( ostream &str, const TableData &table, int r, int cbegin, int cend, int c=-1 ) const;
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
  ostream &save( ostream &str, const char *text, int c=-1 ) const;
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
    /*! Write the Parameter \a param to the output stream \a str 
        using the format for column \a c.
        If \a c is negative, the output column is set to the column
	following the one used in the most recent call
	of one of the save() functions.
	If the output column is the first column,
	\a DataStart, otherwise \a Separator is written right before the string.
	If the output column does not exist,
	nothing is written to \a str. */
  ostream &save( ostream &str, const Parameter &param, int c=-1 ) const;
    /*! Skip one column of automatically formatted output.
        Nothing is written, just the current column is incremented. */
  void saveSkip( void ) const { PrevCol++; };
    /*! Return the column number of the last output 
        via one of the save() functions. */
  int saveColumn( void ) const { return PrevCol; };
    /*! Set the column number of the last output to \a col. */
  void setSaveColumn( int col );
    /*! Reset the column number such that the next save() function
        writes into the first column. */
  void resetSaveColumn( void );

    /*! The string indicating a comment. */
  string comment( void ) const;
    /*! Set the string for indicating comments to \a comment
        and the start string to \a comment + " ". */
  void setComment( const string &comment );

    /*! The string introducing each line of the table header. */
  string keyStart( void ) const;
    /*! Set the string introducing each line of the table header to \a start. */
  void setKeyStart( const string &start );

    /*! The string introducing each line of data. */
  string dataStart( void ) const;
    /*! Set the string introducing each line of data to \a start. */
  void setDataStart( const string &start );

    /*! The string separating two columns. */
  string separator( void ) const;
    /*! Set the string separating two columns to \a separator. */
  void setSeparator( const string &separator );

    /*! The string indicating a missing data value. */
  string missing( void ) const;
    /*! Set the string indicating missing data values to \a missing. */
  void setMissing( const string &missing );


 private:

  void init( void );
  void addParams( Options *o, deque < Options::section_iterator > &sections, int &level );

  template < typename T >
  ostream &saveVector( ostream &str, const vector< T > &vec, int c=-1 ) const;

  Options Opt;

  deque< deque < Options::section_iterator > > Sections;
  deque < Options::iterator > Columns;
  vector < int > Width;
  mutable int PrevCol;

  mutable Parameter Dummy;
  mutable Options DummySection;

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


}; /* namespace relacs */

#endif /* ! _RELACS_TABLEKEY_H_ */
