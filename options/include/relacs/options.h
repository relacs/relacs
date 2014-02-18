/*
  options.h
  A hierarchical name-value list for configuration files and dialogs.

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

#ifndef _RELACS_OPTIONS_H_
#define _RELACS_OPTIONS_H_ 1


#include <string>
#include <deque>
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/parameter.h>
using namespace std;

namespace relacs {


/*!
\class Options
\author Jan Benda
\version 2.0
\brief A hierarchical name-value list for configuration files and dialogs.

An Options class contains a list of name-value pairs that are
identified by a string \a name and have a default value \a dflt.  For
the dialog the string \a request is used to request the value of the
name-value pair.  Name-value pairs whose value-type is a Number have
in addition a minimum and maximum value, a step size, a unit, and a
format string. See class Parameter for more details on the name-value pairs.

Use addNumber() and addText() to add option variables,
which can be either numbers or strings, respectively.
With insertNumber() and insertStr() new options can be
inserted at the beginning of the options list.

Use setNumber() and setText() to change the default,
minimum and maximum value, unit and format of an already existing option.
With erase() options can selectively be erased.

The values of the options can be read out with number() or text().
In addition the default values are accesible with defaultNumber()
and defaultText().
Minimum and maximum values can be read with minimum() and maximum().
The corresponding unit and formatting string can be read with unit()
and format().

In addition to the name-value pairs, an Options class contains further
Options, i.e. a list of sections of name-value pairs.

The total number of defined options is returned by size().
To check wether an option with a certain name exist use exist().

The read(), readAppend(), load() and save() functions read the values
of options, load new options, and save options from and to strings or files.
*/


class Options
{

public:

    /*! Use this flag to select Parameter whose value differs
        from their default value. */
  static const int NonDefault = Parameter::NonDefault;
    /*! Section is a new tab. */
  static const long TabSection = 0x04000000;

    /*! Flags for the customizing saving of Options. */
  enum SaveFlags {
      /*! Do not save the name of a section. */
    NoName = 1,
      /*! Do not save the type of a section. */
    NoType = 2,
      /*! Do not save the include file of a section. */
    NoInclude = 4,
      /*! Switch name and type, i.e.  the sections name is saved as its
          type, and its type is saved as its name. */
    SwitchNameType = 8,
      /*! Make sure that the saved string is embraced with curly braces
          to get a valid YAML string. */
    Embrace = 16,
      /*! Print the request string. */
    PrintRequest = 32,
      /*! If the Parameter::ListAlways style bit is not set only print
	  out the first value. */
    FirstOnly = 64,
      /*! Specify the type of the value. */
    PrintType = 128,
      /*! Indicate the style of a section or Parameter name. */
    PrintStyle = 256,
      /*! Do not write the finale closing </section>. */
    DontCloseSection = 512
  };

    /*! Constructs an empty options list. */
  Options( void );
    /*! Copy constructor. */
  Options( const Options &o );
    /*! Copy Options \a o that have flags() & \a flags greater than zero
        to this. If \a flags equals zero, all options are copied. */
  Options( const Options &o, int flags );
    /*! Constructs an empty Options with name \a name, type string \a type,
        flag \a flags, and style flag \a style. */
  Options( const string &name, const string &type, int flags, int style );
    /*! Create options from string \a opttxt.
        See load( const Str &opttxt, const string &separator ) for details. */
  Options( const Str &opttxt, const string &assignment=":=",
	   const string &separator=",;" );
    /*! Create options from strings \a sq.
        See load( const StrQueue &sq ) for details. */
  Options( const StrQueue &sq, const string &assignment=":=" );
     /*! Create options from input stream \a str.
         See load( istream &str, const string &comment, const string &stop, string *line ) for details. */
  Options( istream &str, const string &assignment=":=",
	   const string &comment="#",
	   const string &stop="", string *line=0 );
    /*! Deconstructs an options list. */
  virtual ~Options( void );

    /*! Copy Options \a o to this. */
  Options &operator=( const Options &o );
    /*! Copy Options \a o to this. */
  Options &assign( const Options &o );
    /*! Append Options \a o to this.
        That is, all Parameter of \a o are added to this Options' Parameter
	and all sections of \a o are added to the list of this' sections.
	\note If you want to add \a o as a complete section use
	addSection(), newSection(), or insertSection().
        \sa add(), assign() */
  Options &append( const Options &o );
    /*! Append Options \a o to the currently active section of Options.
        That is, all Parameter and Sections of \a o are added to the Parameter
	and Sections of the currently active section of Options.
	\note If you want to add \a o as a complete section use
	addSection(), newSection(), or insertSection().
        \sa append(), assign(), endSection(), clearSections() */
  Options &add( const Options &o );
    /*! Insert all name-value pairs of Options \a o at the beginning
        of the options list (\a atname == "") or at the position of
        the option with name \a atname. If the option with name \a
        atname does not exist, the options are appended to the end of
        the list. Sections of \a o are not considered for insertion. */
  Options &insert( const Options &o, const string &atname="" );

    /*! Copy Options \a o that have flags() & \a flags greater than zero
        to this. If \a flags equals zero, all options are copied.
	If \a flags is negative, only options whose values differ
	from the default value and have abs(\a flags) set in their flags()
	are copied.
        If \a flags equals NonDefault, all options whose values differ
	from their default value are copied. */
  Options &assign( const Options &o, int flags );
    /*! Copy all options \a o that have flags() & \a flags greater than zero.
        If \a flags equals zero, all options are copied.
	If \a flags is negative, only options whose values differ
	from the default value and have abs(\a flags) set in their flags()
	are copied.
        If \a flags equals NonDefault, all options whose values differ
	from their default value are copied. */
  Options &copy( Options &o, int flags );
    /*! Append all parameter and sections from \a o
        that have flags() & \a flags greater
        than zero to this.
        If \a flags equals zero, all Parameter and sections are appended.
	If \a flags is negative, only Parameter and sections whose values differ
	from the default value and have abs(\a flags) set in their flags()
	are copied.
        If \a flags equals NonDefault, all Parameter whose values differ
	from their default value are copied. */
  Options &append( const Options &o, int flags );
    /*! Append all parameter and sections from \a o
        that have flags() & \a flags greater
        than zero to the currently active section.
        If \a flags equals zero, all Parameter and sections are appended.
	If \a flags is negative, only Parameter and sections whose values differ
	from the default value and have abs(\a flags) set in their flags()
	are copied.
        If \a flags equals NonDefault, all Parameter whose values differ
	from their default value are copied. */
  Options &add( const Options &o, int flags );
    /*! Insert name-value pairs of \a o that have flags() & \a flags
        greater than zero at the beginning of the options list (\a
        atname == "") or at the position of the option with name \a
        atname. If the option with name \a atname does not exist, the
        options are appended to the end of the list.  Sections of \a o
        are not considered for insertion. */
  Options &insert( const Options &o, int flags, const string &atname="" );

    /*! Set the value of an existing option
        with name \a name to \a value.
	Set warning message, if the \a value is invalid.
        Used by read(). */
  Parameter *assign( const string &name, const string &value );

    /*! Returns true if the two Options \a o1 and \a o2 are equal,
        i.e. they have the same number of Parameter with identical name
	and value (as returned by Parameter::text())
	and the same sections with identical name and type. */
  friend bool operator==( const Options &o1, const Options &o2 );
    /*! Returns true if the name() of the Options \a o equals \a name. */
  friend bool operator==( const Options &o, const string &name );
    /*! Returns true if the Option \a o1 is smaller than \a o2,
        i.e. \a o2 has less elements than \a o1,
	an name of \a o2 is smaller than the corresponding one in \a o1,
	or a value of \a o2 is smaller than the corresponding one in \a o1.
        This function is provided just to define some ordering of Options,
	as is needed for example for an stl::map. */
  friend bool operator<( const Options &o1, const Options &o2 );

    /*! Returns a pointer to the Options where this Options belongs to
        as a section.
        If this Options does not belong to an Options, NULL is returned.
        \sa rootSection() */
  Options *parentSection( void );
    /*! Returns a const pointer to the Options where this Options belongs to
        as a section.
        If this Options does not belong to an Options, NULL is returned.
        \sa rootSection() */
  const Options *parentSection( void ) const;
    /*! Set the parent Options of this Options to \a parent.
        \sa parentSection() */
  void setParentSection( Options *parentsection );
    /*! Reset the parentSection() of all sections and subsections
        for this tree of Options.
        \sa parentSection() */
  void resetParents( void );

    /*! Returns a pointer to the top most Options in the hierarchy.
        If \a this Options does not have a parentSection() \a this is returned.
        \sa parentSection() */
  Options *rootSection( void );
    /*! Returns a pointer to the top most Options in the hierarchy.
        If \a this Options does not have a parentSection() \a this is returned.
        \sa parentSection() */
  const Options *rootSection( void ) const;

    /*! The name of this section of options. */
  string name( void ) const;
    /*! Set the name of this section of options to \a name. */
  virtual void setName( const string &name );
    /*! Set the name of this section of options to \a name
        and its type to \a type.
        The default implementation uses setName() for setting the name. */
  virtual void setName( const string &name, const string &type );

    /*! The type specifyier of this section of options. */
  string type( void ) const;
    /*! Set the type specifier of this section of options to \a type. */
  void setType( const string &type );

    /*! The include path. \sa setInclude()  */
  string include( void ) const;
    /*! Tell this section of options that it inherits some of it
        contents from a section in a different file specified by \a include.
	\sa include() */
  void setInclude( const string &include );
    /*! Tell this section of options that it inherits some of it
        contents from a section with name \a name saved in a different
        file at \a url. This information is written out by the
        saveXML() function only. \sa include() */
  void setInclude( const string &url, const string &name );

    /*! The flag for this secion of options. */
  int flag( void ) const;
    /*! True if one of the bits specified by \a abs(selectflag) is set
        in the Option's flag(), or \a selectflag equals zero or NonDefault. */
  bool flag( int selectflag ) const;
    /*! Set the flag of this section of options to \a flag. */
  Options &setFlag( int flag );
    /*! Add the bits specified by \a flag to the flag
        of this section of options. */
  Options &addFlag( int flag );
    /*! Clear the bits specified by \a flag in the flag
        of this section of options. */
  Options &delFlag( int flag );
    /*! Set all bits of the flag of this section of options,
        i.e. set it to 0. */
  Options &clearFlag( void );

    /*! The style flag for name() and type() of this secion of options. */
  int style( void ) const;
    /*! Set the style flag for name() and type()
        of this section of options to \a style. */
  Options &setStyle( int style );
    /*! Add the bits specified by \a style in the style flag
        for name() and type() of this section of options. */
  Options &addStyle( int style );
    /*! Clear the bits specified by \a style in the style flag
        for name() and type() of this section of options. */
  Options &delStyle( int style );
    /*! Clear all bits of the style of this section of options,
        i.e. set it to 0. */
  Options &clearStyle( void );

    /*! \return \c true if one of the bits specified by \a mask corresponds
        to the Parameter::Section bit, or \a mask equals zero,
	or \a mask is negative and the Parameter::Section bit
	is not contained in abs( mask ). */
  bool checkType( int mask ) const;

    /*! Returns the warning messages of the last called
        Option member-function. */
  Str warning( void ) const { return Warning; };

  typedef deque< Parameter >::iterator iterator;
  inline iterator begin( void ) { return Opt.begin(); };
  inline iterator end( void ) { return Opt.end(); };
  typedef deque< Parameter >::const_iterator const_iterator;
  inline const_iterator begin( void ) const { return Opt.begin(); };
  inline const_iterator end( void ) const { return Opt.end(); };

  typedef deque< Options* >::iterator section_iterator;
  inline section_iterator sectionsBegin( void ) { return Secs.begin(); };
  inline section_iterator sectionsEnd( void ) { return Secs.end(); };
  typedef deque< Options* >::const_iterator const_section_iterator;
  inline const_section_iterator sectionsBegin( void ) const { return Secs.begin(); };
  inline const_section_iterator sectionsEnd( void ) const { return Secs.end(); };

    /*! Search for the first option that matches \a pattern.
        \a pattern can be a list of search terms separated by '>',
	for example 'aaa>bbb', to search parameter 'bbb' in section 'aaa'.
        Alternative search terms can be separated by '|'.
	For example, if \a pattern is "date|time", then
	"date" is searched first and if this is not found,
	"time" is searched.
        Returns end() if no match for \a pattern is found. */
  const_iterator find( const string &pattern, int level=-1 ) const;
    /*! Search for the first option that matches \a pattern.
        \a pattern can be a list of search terms separated by '>',
	for example 'aaa>bbb', to search parameter 'bbb' in section 'aaa'.
        Alternative search terms can be separated by '|'.
	For example, if \a pattern is "date|time", then
	"date" is searched first and if this is not found,
	"time" is searched.
        Returns end() if no match for \a pattern is found. */
  iterator find( const string &pattern, int level=-1 );
    /*! Search for the last option that matches \a pattern.
        Returns end() if \a name is not found.
        See find() for details about valid patterns \a pattern. */
  const_iterator rfind( const string &pattern, int level=-1 ) const;
    /*! Search for the last option that matches \a pattern.
        Returns end() if \a name is not found.
        See find() for details about valid patterns \a pattern. */
  iterator rfind( const string &pattern, int level=-1 );

    /*! Search for the first section of Options that matches \a pattern.
        \a pattern can be a list of search terms separated by '>',
	for example 'aaa>bbb', to search subsection 'bbb' in section 'aaa'.
        Alternative search terms can be separated by '|'.
	For example, if \a pattern is "date|time", then
	"date" is searched first and if this is not found,
	"time" is searched.
        Returns sectionsEnd() if no match for \a pattern is found.
        \note the name() of this Options is not found. */
  const_section_iterator findSection( const string &pattern, int level=-1 ) const;
    /*! Search for the first section of Options that matches \a pattern.
        \a pattern can be a list of search terms separated by '>',
	for example 'aaa>bbb', to search subsection 'bbb' in section 'aaa'.
        Alternative search terms can be separated by '|'.
	For example, if \a pattern is "date|time", then
	"date" is searched first and if this is not found,
	"time" is searched.
        Returns sectionsEnd() if no match for \a pattern is found.
        \note the name() of this Options is not found. */
  section_iterator findSection( const string &pattern, int level=-1 );
    /*! Search for the last section of Options that matches \a pattern.
        \a pattern can be a list of search terms separated by '>',
	for example 'aaa>bbb', to search subsection 'bbb' in section 'aaa'.
        Alternative search terms can be separated by '|'.
	For example, if \a pattern is "date|time", then
	"date" is searched first and if this is not found,
	"time" is searched.
        Returns sectionsEnd() if no match for \a pattern is found.
        \note the name() of this Options is not found. */
  const_section_iterator rfindSection( const string &pattern, int level=-1 ) const;
    /*! Search for the last section of Options that matches \a pattern.
        \a pattern can be a list of search terms separated by '>',
	for example 'aaa>bbb', to search subsection 'bbb' in section 'aaa'.
        Alternative search terms can be separated by '|'.
	For example, if \a pattern is "date|time", then
	"date" is searched first and if this is not found,
	"time" is searched.
        Returns sectionsEnd() if no match for \a pattern is found.
        \note the name() of this Options is not found. */
  section_iterator rfindSection( const string &pattern, int level=-1 );

    /*! Get \a i-th name-value pair. */
  const Parameter &operator[]( int i ) const;
    /*! Get \a i-th name-value pair. */
  Parameter &operator[]( int i );
    /*! Get the name-value pair with name \a name. */
  const Parameter &operator[]( const string &name ) const;
    /*! Get the name-value pair with name \a name. */
  Parameter &operator[]( const string &name );

    /*! Get \a i-th section. */
  const Options &section( int i ) const;
    /*! Get \a i-th section. */
  Options &section( int i );
    /*! Get section with name \a name. */
  const Options &section( const string &name ) const;
    /*! Get section with name \a name. */
  Options &section( const string &name );

      /*! Returns the request string of the option with name \a name. */
  Str request( const string &name ) const;
    /*! Set request string of the option
        with name \a name to \a request. */
  Parameter &setRequest( const string &name, const string &request );

    /*! The type of the option with name \a name. */
  Parameter::ValueType valueType( const string &name ) const;
    /*! Set type of the option with name \a name to \a type. */
  Parameter &setValueType( const string &name, Parameter::ValueType type );

    /*! The flags of the option with name \a name. */
  int flags( const string &name ) const;
    /*! \return \c true if the flags of the option with name \a name
        have \a flag set. \sa Parameter::flags( int ) */
  bool flags( const string &name, int flag ) const;
    /*! Set the flags of the option with name \a name to \a flags. */
  Parameter &setFlags( const string &name, int flags );
    /*! Add the bits specified by \a flags to the flags of the option
        with name \a name. */
  Parameter &addFlags( const string &name, int flags );
    /*! Clear the bits specified by \a flags of the flags of the option
        with name \a name. */
  Parameter &delFlags( const string &name, int flags );
    /*! Clear all bits of the flags of the option
        with name \a name. */
  Parameter &clearFlags( const string &name );
    /*! Return \a true if the changedFlag() of the option
        with name \a name is set,
        i.e. whose value has been changed.. */
  bool changed( const string &name );

    /*! The style of the option with name \a name. */
  int style( const string &name ) const;
    /*! Set the style of the option with name \a name to \a style. */
  Parameter &setStyle( const string &name, int style );
    /*! Add the bits specified by \a style to the style of the option
        with name \a name. */
  Parameter &addStyle( const string &name, int style );
    /*! Clear the bits specified by \a style of the style of the option
        with name \a name. */
  Parameter &delStyle( const string &name, int style );

    /*! Return the format string of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, an empty string is returned. */
  Str format( const string &name ) const;
    /*! Set format string of an existing number option
        with name \a name to have a width of \a width,
	precision of \a prec and type \a fmt. */
  Parameter &setFormat( const string &name, int width=0, int prec=-1,
			char fmt='g' );
    /*! Set the format of an existing number option
        with name \a name to \a format. */
  Parameter &setFormat( const string &name, const string &format );
    /*! Returns the width specified by the format string of the option
        specified by \a name. */
  int formatWidth( const string &name ) const;

    /*! If several values correspond to the option
        with name \a name
        size() returns the number of values. */
  int size( const string &name ) const;

    /*! Add parameter \a np to options.
        \sa insert(), endSection(), clearSections() */
  Parameter &add( const Parameter &np );
    /*! Insert a new Parameter \a np at the position of the Parameter
        specified by \a atname.  list (\a atname == "") or. If \a
        atname is empty or no Parameter is found for \a atname, the
        Parameter is inserted at the beginning or appended to the end
        of the currently active section, respectively.
	\sa add(), endSection(), clearSections() */
  Parameter &insert( const Parameter &np, const string &atname="" );

    /*! Add a text option at the end of the options list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt. */
  Parameter &addText( const string &name, const string &request,
		      const string &dflt, int flags=0, int style=0 );
    /*! Add a text option at the end of the options list.
        Its name string and its request string for the options dialog
	are set to \a name.
        The option has a default value \a dflt. */
  inline Parameter &addText( const string &name, const string &dflt="",
			     int flags=0, int style=0 )
    { return addText( name, name, dflt, flags, style ); };
    /*! Insert a new text option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt. */
  Parameter &insertText( const string &name, const string &atname,
			 const string &request, const string &dflt,
			 int flags=0, int style=0 );
    /*! Insert a new text option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and its request string for the options dialog
	are set to \a name.
        The option has a default value \a dflt. */
  inline Parameter &insertText( const string &name, const string &atname="",
				const string &dflt="", int flags=0, int style=0 )
    { return insertText( name, atname, name, dflt, flags, style ); };
    /*! Return the \a index-th string of the text option
        with name \a name.
        If there is no option with name \a name an empty string is returned. */
  Str text( const string &name, int index, const string &dflt="",
	    const string &format="", const string &unit="" ) const;
    /*! Return the first string of the text option
        with name \a name.
        If there is no option with name \a name an empty string is returned. */
  inline Str text( const string &name, const string &dflt="",
		   const string &format="", const string &unit="" ) const
    { return text( name, 0, dflt, format, unit ); };
    /*! Return all values of the parameter with name \a name,
        formatted according to \a format
        using \a unit, and concatenated using \a separator.
        If there is no option with name \a name an empty string is returned.
        See Parameter::text() for details. */
  Str allText( const string &name, const string &dflt="", const string &format="",
	       const string &unit="", const string &separator=", " ) const;
    /*! Set the value of an existing text option
        with name \a name to \a strg. */
  Parameter &setText( const string &name, const string &strg );
    /*! Set value of an existing text option
        with name \a name to the one of \a p,
        if both are of valueType() Text.
	If the value of the parameter is changing
	then the changedFlag() is set. */
  Parameter &setText( const string &name, const Parameter &p );
    /*! Add \a strg to the value of an existing text option
        with name \a name. */
  Parameter &pushText( const string &name, const string &strg );
    /*! Return the default string of the text option
        with name \a name.
        If there is no option with name \a name, or the option is
	not a text, an empty string is returned. */
  Str defaultText( const string &name,
		   const string &format="", const string &unit="" ) const;
    /*! Set the default string of an existing text option
        with name \a name to \a dflt. */
  Parameter &setDefaultText( const string &name, const string &dflt );

    /*! Add a text-selection option at the end of the options list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a list \a selection of strings (separated by ", ")
        that can be selected. */
  Parameter &addSelection( const string &name, const string &request,
			   const string &selection, int flags=0, int style=0 );
    /*! Add a text option at the end of the options list.
        Its name string and its request string for the options dialog
	are set to \a name.
	The option has a list \a selection of strings (separated by ", ")
        that can be selected. */
  inline Parameter &addSelection( const string &name, const string &selection="",
			     int flags=0, int style=0 )
    { return addSelection( name, name, selection, flags, style ); };
    /*! Insert a new text option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
	The option has a list \a selection of strings (separated by ", ")
        that can be selected. */
  Parameter &insertSelection( const string &name, const string &atname,
			      const string &request, const string &selection,
			      int flags=0, int style=0 );
    /*! Insert a new text option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and its request string for the options dialog
	are set to \a name.
	The option has a list \a selection of strings (separated by ", ")
        that can be selected. */
  inline Parameter &insertSelection( const string &name, const string &atname="",
				     const string &selection="", int flags=0, int style=0 )
    { return insertSelection( name, atname, name, selection, flags, style ); };
    /*! If the text parameter has several values,
        then the value that is matched by \a strg is
        inserted as the first value.
        If \a strg does not match any of the text values
        \a strg is added to the text values and is marked as selected
	if \a add > 0, or if \a add == 0 and the SelectText - Bit
	in the Parameter's style is not set. */
  Parameter &selectText( const string &name, const string &strg,
			 int add=0 );
    /*! If the text parameter has several values,
        then the \a index-th value is selected by inserting it as the first value. */
  Parameter &selectText( const string &name, int index );
    /*! Returns the index of the selected value, i.e.
        the index minus one of the matching value with the first value. */
  int index( const string &name ) const;
    /*! Returns the index of the value that matches \a strg. */
  int index( const string &name, const string &strg ) const;

    /*! Add a new number option at the end of the options list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  Parameter &addNumber( const string &name, const string &request,
			double dflt, double minimum,
			double maximum=MAXDOUBLE, double step=1.0,
			const string &unit="", const string &outputunit="",
			const string &format="", int flags=0, int style=0 );
    /*! Add a new number option at the end of the options list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt,
        a unit \a unit and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  inline Parameter &addNumber( const string &name, const string &request,
			       double dflt, const string &unit="",
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( name, request, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ); };
    /*! Add a new number option at the end of the options list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt,
        a unit \a unit and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  inline Parameter &addNumber( const string &name, double dflt,
			       const string &unit="",
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( name, name, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ); };
    /*! Add a new number option at the end of the options list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt and
        a unit \a unit. */
  inline Parameter &addNumber( const string &name, double dflt,
			       const string &unit, int flags, int style=0 )
    { return addNumber( name, name, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, "", flags, style ); };
    /*! Add a new number option at the end of the options list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt,
	an error \a error, a unit \a unit and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  inline Parameter &addNumber( const string &name, double dflt,
			       double error, const string &unit="",
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( name, name, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ).setNumber( dflt, error ); };
    /*! Add a new number option at the end of the options list.
        Its name string and request string is set to \a name.
	The option has a unit \a unit and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  inline Parameter &addNumber( const string &name, const string &unit="",
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( name, name, 0.0, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ); };
    /*! Insert a new number option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  Parameter &insertNumber( const string &name, const string &atname,
			   const string &request, double dflt,
			   double minimum, double maximum=MAXDOUBLE,
			   double step=1.0,
			   const string &unit="", const string &outputunit="",
			   const string &format="", int flags=0, int style=0 );
    /*! Insert a new number option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt,
	a unit \a unit and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  inline Parameter &insertNumber( const string &name, const string &atname,
				  const string &request, double dflt, const string &unit="",
				  const string &format="", int flags=0, int style=0 )
    { return insertNumber( name, atname, request, dflt,
			   -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit,
			   format, flags, style ); };
    /*! Insert a new number option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt,
	a unit \a unit and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  inline Parameter &insertNumber( const string &name, const string &atname,
				  double dflt, const string &unit="",
				  const string &format="", int flags=0, int style=0 )
    { return insertNumber( name, atname, name, dflt,
			   -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit,
			   format, flags, style ); };
    /*! Insert a new number option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and request string is set to \a name.
	The option has a unit \a unit and a format string \a format
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed,
	since the number is a double). */
  inline Parameter &insertNumber( const string &name, const string &atname,
				  const string &unit="",
				  const string &format="", int flags=0, int style=0 )
    { return insertNumber( name, atname, name, 0.0,
			   -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit,
			   format, flags, style ); };
    /*! Return the \a index-th number of the option with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  double number( const string &name, double dflt=0.0, const string &unit="", int index=0 ) const;
  double number( const string &name, const string &unit, double dflt=0.0, int index=0 ) const
    { return number( name, dflt, unit, index ); };
    /*! Return the \a index-th number of the option with name \a name.
        If there is no option with name \a name, or the option is
        neither a number nor an integer, zero is returned. */
  double number( const string &name, int index, double dflt=0.0, const string &unit="" ) const
    { return number( name, dflt, unit, index ); };
  double number( const string &name, int index, const string &unit, double dflt=0.0 ) const
    { return number( name, dflt, unit, index ); };
    /*! Return the \a index-th standard deviation of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  double error( const string &name, const string &unit="", int index=0 ) const;
    /*! Return the \a index-th standard deviation of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  double error( const string &name, int index, const string &unit="" ) const
    { return error( name, unit, index ); };
    /*! Set the value of an existing number option
        with name \a name to \a number. */
  Parameter &setNumber( const string &name, double number, double error,
			const string &unit="" );
    /*! Set the value of an existing number option
        with name \a name to \a number. */
  inline Parameter &setNumber( const string &name, double number,
			       const string &unit="" )
    { return setNumber( name, number, -1.0, unit ); };
    /*! Add \a number to the value of an existing number option
        with name \a name. */
  Parameter &pushNumber( const string &name, double number, double error=-1.0,
			 const string &unit="" );
    /*! Add \a number to the value of an existing number option
        with name \a name. */
  inline Parameter &pushNumber( const string &name, double number,
				const string &unit )
    { return pushNumber( name, number, -1.0, unit ); };
    /*! Set value of an existing number option
        with name \a name to the one of \a p,
        if both are of valueType() Number.
	If the value of the parameter is changing
	then the changedFlag() is set. */
  Parameter &setNumber( const string &name, const Parameter &p );
    /*! Return the default number of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  double defaultNumber( const string &name, const string &unit="" ) const;
    /*! Set the default number of an existing number option
        with name \a name to \a dflt. */
  Parameter &setDefaultNumber( const string &name, double dflt,
			       const string &unit="" );
    /*! Return the minimum number of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  double minimum( const string &name, const string &unit="" ) const;
    /*! Return the maximum number of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  double maximum( const string &name, const string &unit="" ) const;
    /*! Return the step size of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, unity is returned. */
  double step( const string &name, const string &unit="" ) const;
    /* Set the step size of an existing number option
       with name \a name to \a step of unit \a unit.
       If \a step is negative, the step size is set to \a maximum - \minimum
       devided by \a step.
       If \a step equals zero, the step size is set to 1/50 of
       \a maximum - \a minimum.
       If \a unit is empty, \a step is assumed to be given in the internal unit. */
  Parameter &setStep( const string &name, double step, const string &unit="" );
    /*! Set the minimum and maximum of an existing number option
        with name \a name to \a minimum and \a maximum, respectively. */
  Parameter &setMinMax( const string &name, double minimum=-MAXDOUBLE,
			double maximum=MAXDOUBLE, double step=1.0,
			const string &unit="" );

    /*! Returns the largest power of ten smaller or equal to \a v. */
  inline static double floorLog10( double v )
    { return Parameter::floorLog10( v ); };
    /*! Returns the next smaller multiple of floorLog10( \a v ).
        This is flooring to the leading digit.
        For example: 23.7 -> 20, 1.45 -> 1.
        \a scale determines to which digit the number should be floored.
        For example with \a scale = 0.1 you get 23.7 -> 23, 1.45 -> 1.4 */
  static double floor10( double v, double scale=1.0 )
    { return Parameter::floor10( v, scale ); };
    /*! Returns the next larger multiple of floorLog10( \a v ).
        This is ceiling to the leading digit.
        For example: 23.7 -> 30, 1.45 -> 2.
        \a scale determines to which digit the number should be ceiled.
        For example with \a scale = 0.1 you get 23.7 -> 24, 1.45 -> 1.5 */
  inline static double ceil10( double v, double scale=1.0 )
    { return Parameter::ceil10( v, scale ); };

    /*! Return the internal unit of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, an empty string is returned. */
  Str unit( const string &name ) const;
    /*! Return the unit used for output and dialogs
        of the option with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, an empty string is returned. */
  Str outUnit( const string &name ) const;
    /*! Set the internal unit and the output unit of an existing number option
        with name \a name to \a internunit and \a outunit, respectively.
        If \a outunit is an empty string it is set to \a internunit. */
  Parameter &setUnit( const string &name, const string &internunit,
		      const string &outunit="" );
    /*! Set the output unit of an existing number option
        with name \a name to \a outputunit. */
  Parameter &setOutUnit( const string &name, const string &outputunit );
    /*! Set the internal unit of an existing number option
        with name \a name to \a inputunit.
	The values of the parameter are converted accordingly. */
  Parameter &changeUnit( const string &name, const string &internunit );

    /*! Add a new integer option at the end of the options list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a width of output format \a width. */
  Parameter &addInteger( const string &name, const string &request,
			 long dflt, long minimum=LONG_MIN,
			 long maximum=LONG_MAX, long step=1,
			 const string &unit="", const string &outputunit="",
			 int width=0, int flags=0, int style=0 );
    /*! Add a new integer option at the end of the options list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt, a unit \a unit,
	and a width of output format \a width. */
  inline Parameter &addInteger( const string &name, long dflt,
				const string &unit="", int width=0, int flags=0, int style=0 )
    { return addInteger( name, name, dflt, LONG_MIN, LONG_MAX, 1,
			 unit, unit, width, flags, style ); };
    /*! Add a new integer option at the end of the options list.
        Its name string and request string is set to \a name.
        The option has a unit \a unit,
	and a width of output format \a width. */
  inline Parameter &addInteger( const string &name, const string &unit="" )
    { return addInteger( name, name, 0, LONG_MIN, LONG_MAX, 1,
			 unit, unit, -1, 0 ); };
    /*! Insert a new integer option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a width of output format \a width. */
  Parameter &insertInteger( const string &name, const string &atname,
			    const string &request, long dflt,
			    long minimum=LONG_MIN, long maximum=LONG_MAX,
			    long step=1,
			    const string &unit="", const string &outputunit="",
			    int width=0, int flags=0, int style=0 );
    /*! Insert a new integer option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt, a unit \a unit,
	and a width of output format \a width. */
  Parameter &insertInteger( const string &name, const string &atname,
			    long dflt, const string &unit="",
			    int width=0, int flags=0, int style=0 )
    { return insertInteger( name, atname, name, dflt, LONG_MIN, LONG_MAX,
			    1, unit, unit, width, flags, style ); };
    /*! Insert a new integer option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and request string is set to \a name.
        The option has a unit \a unit,
	and a width of output format \a width. */
  Parameter &insertInteger( const string &name, const string &atname,
			    const string &unit="",
			    int width=0, int flags=0, int style=0 )
    { return insertInteger( name, atname, name, 0, LONG_MIN, LONG_MAX,
			    1, unit, unit, width, flags, style ); };
    /*! Return the \a index-th integer number of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  long integer( const string &name, const string &unit="", long dflt=0, int index=0 ) const;
    /*! Return the \a index-th integer number of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  long integer( const string &name, int index, long dflt=0, const string &unit="" ) const
    { return integer( name, unit, dflt, index ); }
  long integer( const string &name, int index, const string &unit, long dflt=0 ) const
    { return integer( name, unit, dflt, index ); }
   /*! Set the value of an existing integer option
        with name \a name to \a number. */
  Parameter &setInteger( const string &name, long number, long error,
			 const string &unit="" );
   /*! Set the value of an existing integer option
        with name \a name to \a number. */
  inline Parameter &setInteger( const string &name, long number, const string &unit="" )
    { return setInteger( name, number, -1, unit ); };
   /*! Add \a number to the value of an existing integer option
        with name \a name. */
  Parameter &pushInteger( const string &name, long number, long error,
			  const string &unit="" );
   /*! Add \a number to the value of an existing integer option
        with name \a name. */
  inline Parameter &pushInteger( const string &name, long number, const string &unit="" )
    { return pushInteger( name, number, -1, unit ); };
    /*! Set value of an existing integer option
        with name \a name to the one of \a p,
        if both are of valueType() Integer.
	If the value of the parameter is changing
	then the changedFlag() is set. */
  Parameter &setInteger( const string &name, const Parameter &p );
    /*! Return the default number of the integer option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a number nor an integer, zero is returned. */
  long defaultInteger( const string &name, const string &unit="" ) const;
    /*! Set the default number of an existing integer option
        with name \a name to \a dflt. */
  Parameter &setDefaultInteger( const string &name, long dflt,
				const string &unit="" );
    /*! Set the minimum and maximum of an existing integer number option
        with name \a name to \a minimum and \a maximum, respectively. */
  Parameter &setMinMax( const string &name, long minimum=LONG_MIN,
			long maximum=LONG_MAX, long step=1, const string &unit="" );
    /*! Set the minimum and maximum of an existing integer number option
        with name \a name to \a minimum and \a maximum, respectively. */
  Parameter &setMinMax( const string &name, int minimum,
			int maximum, int step=1, const string &unit="" )
    { return setMinMax( name, (long)minimum, (long)maximum, (long)step, unit ); };

    /*! Add a boolean option at the end of the options list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt. */
  Parameter &addBoolean( const string &name, const string &request,
			 bool dflt, int flags=0, int style=0 );
    /*! Add a boolean option at the end of the options list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt. */
  inline Parameter &addBoolean( const char *name, const char *request, bool dflt, int flags=0, int style=0 )
    { return addBoolean( string( name ), string( request ), dflt, flags, style ); };
  inline Parameter &addBoolean( const string &name, bool dflt, int flags=0, int style=0 )
    { return addBoolean( name, name, dflt, flags, style ); };
  inline Parameter &addBoolean( const char *name, bool dflt, int flags=0, int style=0 )
    { return addBoolean( string( name ), dflt, flags, style ); };
    /*! Insert a new boolean option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its name string is set to \a name.
        The option has a default value \a dflt. */
  Parameter &insertBoolean( const string &name, const string &atname="",
			    const string &request="", bool dflt=false,
			    int flags=0, int style=0 );
    /*! Insert a new boolean option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and request string is set to \a name.
        The option has a default value \a dflt. */
  /*
  Parameter &insertBoolean( const string &name, const string &atname,
			    bool dflt, int flags=0, int style=0 )
    { return insertBoolean( name, atname, name, dflt, flags, style ); };
  */
    /*! Insert a new boolean option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        Its name string and request string is set to \a name. */
  Parameter &insertBoolean( const string &name, const string &atname,
			    int flags=0, int style=0 )
    { return insertBoolean( name, atname, name, false, flags, style ); };
    /*! Return the boolean of the option with name \a name.
        If there is no option with name \a name, or the option is
	neither a boolean, nor a number, nor an integer,
	false is returned. */
  bool boolean( const string &name, bool dflt=false, int index=0 ) const;
  bool boolean( const string &name, int index, bool dflt=false ) const
    { return boolean( name, dflt, index ); };
    /*! Set the value of an existing boolean option
        with name \a name to \a string. */
  Parameter &setBoolean( const string &name, bool b );
    /*! Set value of an existing boolean option
        with name \a name to the one of \a p,
        if both are of valueType() Boolean.
	If the value of the parameter is changing
	then the changedFlag() is set. */
  Parameter &setBoolean( const string &name, const Parameter &p );
    /*! Return the default boolean of the option
        with name \a name.
        If there is no option with name \a name, or the option is
	neither a boolean, nor a number nor an integer,
	false is returned. */
  bool defaultBoolean( const string &name ) const;
    /*! Set the default value of an existing boolean option
        with name \a name to \a dflt. */
  Parameter &setDefaultBoolean( const string &name, bool dflt );

    /*! Add a date option at the end of the options list.
        \param[in] name the name string of the new option
        \param[in] request the request string of the new option
	\param[in] year the default value for the year
	\param[in] month the default value for the month
	\param[in] day the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertDate(), setDate(), date() */
  Parameter &addDate( const string &name, const string &request,
		      int year=0, int month=0, int day=0,
		      int flags=0, int style=0 );
    /*! Add a date option at the end of the options list.
        \param[in] name the name and request string of the new option
	\param[in] year the default value for the year
	\param[in] month the default value for the month
	\param[in] day the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertDate(), setDate(), date() */
  inline Parameter &addDate( const string &name,
			     int year, int month, int day,
			     int flags=0, int style=0 )
    { return addDate( name, name, year, month, day, flags, style ); };
    /*! Add a date option at the end of the options list
        with year, month and day set to zero.
        \param[in] name the name and request string of the new option
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertDate(), setDate(), date() */
  inline Parameter &addDate( const string &name, int flags=0, int style=0 )
    { return addDate( name, name, 0, 0, 0, flags, style ); };
    /*! Insert a new date option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        \param[in] name the name string of the new option
        \param[in] atname the name string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
        \param[in] request the request string of the new option
	\param[in] year the default value for the year
	\param[in] month the default value for the month
	\param[in] day the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addDate(), setDate(), date() */
  Parameter &insertDate( const string &name, const string &atname="",
			 const string &request="",
			 int year=0, int month=0, int day=0,
			 int flags=0, int style=0 );
    /*! Insert a new date option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        \param[in] name the name and request string of the new option
        \param[in] atname the name string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] year the default value for the year
	\param[in] month the default value for the month
	\param[in] day the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addDate(), setDate(), date() */
  Parameter &insertDate( const string &name, const string &atname,
			 int year=0, int month=0, int day=0,
			 int flags=0, int style=0 )
    { return insertDate( name, atname, name, year, month, day, flags, style ); };
    /*! Insert a new date option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        The year, month and day are set to zero.
        \param[in] name the name and request string of the new option
        \param[in] atname the name string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addDate(), setDate(), date() */
  Parameter &insertDate( const string &name, const string &atname,
			 int flags=0, int style=0 )
    { return insertDate( name, atname, name, 0, 0, 0, flags, style ); };
    /*! Get the date from a date option.
	\param[in] name the name string of the option
	\param[in] index the index of the date
	\param[out] year the year of the specified date
	\param[out] month the month of the specified date
	\param[out] day the day of the specified date
	\return the reference of the option
        \sa defaultDate(), setDate(), addDate(), insertDate() */
  const Parameter &date( const string &name, int index,
			 int &year, int &month, int &day ) const;
    /*! Set the value of an existing date option.
	\param[in] name the name string of the option
	\param[in] year the new value for the year
	\param[in] month the new value for the month
	\param[in] day the new value for the day
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &name, int year, int month, int day );
    /*! Set the value of an existing date option.
	\param[in] name the name string of the option
	\param[in] date the new value for the date
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &name, const string &date );
    /*! Set the value of an existing date option.
	\param[in] name the name string of the option
	\param[in] date the new value for the date
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &name, const struct tm &date );
    /*! Set the value of an existing date option.
	\param[in] name the name string of the option
	\param[in] time the new value for the date
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &name, const time_t &time );
    /*! Set the value of an existing date option to the current date.
	\param[in] name the name string of the option
	\return a reference to the option.
        \sa setDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setCurrentDate( const string &name );
    /*! Set value of an existing date option
        with name \a name to the one of \a p,
        if both are of valueType() Date.
	If the value of the parameter is changing
	then the changedFlag() is set. */
  Parameter &setDate( const string &name, const Parameter &p );
    /*! Get the default date from a date option.
	\param[in] name the name string of the option
	\param[in] index the index of the default date
	\param[out] year the year of the specified default date
	\param[out] month the month of the specified default date
	\param[out] day the day of the specified default date
	\return the reference of the option
        \sa date(), setDate(), addDate(), insertDate() */
  const Parameter &defaultDate( const string &name, int index,
				int &year, int &month, int &day ) const;
    /*! Set the default value of an existing date option.
	\param[in] name the name string of the option
	\param[in] year the new value for the default year
	\param[in] month the new value for the default month
	\param[in] day the new value for the default day
	\return a reference to the option.
        \sa setDate(), addDate(), insertDate(), defaultDate() */
  Parameter &setDefaultDate( const string &name,
			     int year, int month, int day );

    /*! Add a time option at the end of the options list.
        \param[in] name the name string of the new option
        \param[in] request the request string of the new option
	\param[in] hour the default value for the hour
	\param[in] minutes the default value for the minutes
	\param[in] seconds the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertTime(), setTime(), time() */
  Parameter &addTime( const string &name, const string &request,
		      int hour=0, int minutes=0, int seconds=0,
		      int flags=0, int style=0 );
    /*! Add a time option at the end of the options list.
        \param[in] name the name and request string of the new option
	\param[in] hour the default value for the hour
	\param[in] minutes the default value for the minutes
	\param[in] seconds the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertTime(), setTime(), time() */
  inline Parameter &addTime( const string &name,
			     int hour, int minutes, int seconds,
			     int flags=0, int style=0 )
    { return addTime( name, name, hour, minutes, seconds, flags, style ); };
    /*! Add a time option at the end of the options list
        with hour, minutes and seconds set to zero.
        \param[in] name the name and request string of the new option
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertTime(), setTime(), time() */
  inline Parameter &addTime( const string &name, int flags=0, int style=0 )
    { return addTime( name, name, 0, 0, 0, flags, style ); };
    /*! Insert a new time option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        \param[in] name the name string of the new option
        \param[in] atname the name string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
        \param[in] request the request string of the new option
	\param[in] hour the default value for the hour
	\param[in] minutes the default value for the minutes
	\param[in] seconds the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addTime(), setTime(), time() */
  Parameter &insertTime( const string &name, const string &atname="",
			 const string &request="",
			 int hour=0, int minutes=0, int seconds=0,
			 int flags=0, int style=0 );
    /*! Insert a new time option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
        \param[in] name the name and request string of the new option
        \param[in] atname the name string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] hour the default value for the hour
	\param[in] minutes the default value for the minutes
	\param[in] seconds the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addTime(), setTime(), time() */
  Parameter &insertTime( const string &name, const string &atname,
			 int hour=0, int minutes=0, int seconds=0,
			 int flags=0, int style=0 )
    { return insertTime( name, atname, name, hour, minutes, seconds, flags, style ); };
    /*! Insert a new time option at the beginning of the options list
        (\a atname == "") or at the position of the option with
        name \a atname. If the option with name \a atname
        does not exist, the option is appended to the end of the list.
	The hour, minutes and seconds are set to zero.
        \param[in] name the name and request string of the new option
        \param[in] atname the name string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addTime(), setTime(), time() */
  Parameter &insertTime( const string &name, const string &atname,
			 int flags=0, int style=0 )
    { return insertTime( name, atname, name, 0, 0, 0, flags, style ); };
    /*! Get the time from a time option.
	\param[in] name the name string of the option
	\param[in] index the index of the time
	\param[out] hour the hour of the specified time
	\param[out] minutes the minutes of the specified time
	\param[out] seconds the seconds of the specified time
	\return the reference of the option
        \sa defaultTime(), setTime(), addTime(), insertTime() */
  const Parameter &time( const string &name, int index,
			 int &hour, int &minutes, int &seconds ) const;
    /*! Set the value of an existing time option.
	\param[in] name the name string of the option
	\param[in] hour the new value for the hour
	\param[in] minutes the new value for the minutes
	\param[in] seconds the new value for the seconds
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &name,
		      int hour, int minutes, int seconds );
    /*! Set the value of an existing time option.
	\param[in] name the name string of the option
	\param[in] time the new value for the time
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &name, const string &time );
    /*! Set the value of an existing time option.
	\param[in] name the name string of the option
	\param[in] time the new value for the time
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &name, const struct tm &time );
    /*! Set the value of an existing time option.
	\param[in] name the name string of the option
	\param[in] time the new value for the time
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &name, const time_t &time );
    /*! Set the value of an existing time option to the current time.
	\param[in] name the name string of the option
	\return a reference to the option.
        \sa setTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setCurrentTime( const string &name );
    /*! Set value of an existing time option
        with name \a name to the one of \a p,
        if both are of valueType() Time.
	If the value of the parameter is changing
	then the changedFlag() is set. */
  Parameter &setTime( const string &name, const Parameter &p );
    /*! Get the default time from a time option.
	\param[in] name the name string of the option
	\param[in] index the index of the default time
	\param[out] hour the hour of the specified default time
	\param[out] minutes the minutes of the specified default time
	\param[out] seconds the seconds of the specified default time
	\return the reference of the option
        \sa time(), setTime(), addTime(), insertTime() */
  const Parameter &defaultTime( const string &name, int index,
				int &hour, int &minutes, int &seconds ) const;
    /*! Set the default value of an existing time option.
	\param[in] name the name string of the option
	\param[in] hour the new value for the default hour
	\param[in] minutes the new value for the default minutes
	\param[in] seconds the new value for the default seconds
	\return a reference to the option.
        \sa setTime(), addTime(), insertTime(), defaultTime() */
  Parameter &setDefaultTime( const string &name,
			     int hour, int minutes, int seconds );

    /*! Add a new subsection of level \a level.
        The new section is named \a name, has the optional
	type specifier \a type, some \a flag for selecting this section,
	and is formatted according to the \a style flag.
	\a level = 0 is the upper level, i.e. a new section is added.
	Higher \a levels add sections lower in the hierachy,
	i.e. \a level = 1 adds a subsection, \a \a level = 2 a subsubsection, etc.
        Subsequent calls to addText(), addNumber(), etc. add new Parameter
	to the added section.
        \sa newSubSection(), newSubSubSection(), addSection(), insertSection(),
	endSection(), clearSections() */
  Options &newSection( int level, const string &name, const string &type="",
		       int flag=0, int style=0 );
  Options &newSection( int level, const string &name, int flag, int style=0 )
    { return newSection( level, name, "", flag, style ); };
    /*! Add a new section of Options to the end of the sections list.
        The new section is named \a name, has the optional
	type specifier \a type, some \a flag for selecting this section,
	and is formatted according to the \a style flag.
        Subsequent calls to addText(), addNumber(), etc. add new Parameter
	to the added section.
        \sa newSubSection(), newSubSubSection(), addSection(), insertSection(),
	endSection(), clearSections() */
  Options &newSection( const string &name, const string &type="",
		       int flag=0, int style=0 );
  Options &newSection( const string &name, int flag, int style=0 )
    { return newSection( name, "", flag, style ); };
    /*! Add a new subsection of Options to the last section.
        The new subsection is named \a name, has the optional
	type specifier \a type, some \a flag for selecting this section,
	and is formatted according to the \a style flag.
        Subsequent calls to addText(), addNumber(), etc. add new Parameter
	to the added subsection.
        \note You can only add a subsection after having added a section!
        \sa newSection(), newSubSubSection(), addSection(), insertSection(),
	endSection(), clearSections() */
  Options &newSubSection( const string &name, const string &type="",
			  int flag=0, int style=0 );
  Options &newSubSection( const string &name, int flag, int style=0 )
    { return newSubSection( name, "", flag, style ); };
    /*! Add a new subsubsection of Options to the last subsection
        of the last section.
        The new subsubsection is named \a name, has the optional
	type specifier \a type, some \a flag for selecting this section,
	and is formatted according to the \a style flag.
        Subsequent calls to addText(), addNumber(), etc. add new Parameter
	to the added subsubsection.
        \note You can only add a subsubsection after having added a subsection!
        \sa newSection(), newSubSection(), addSection(), insertSection(),
	endSection(), clearSections() */
  Options &newSubSubSection( const string &name, const string &type="",
			     int flag=0, int style=0 );
  Options &newSubSubSection( const string &name, int flag, int style=0 )
    { return newSubSubSection( name, "", flag, style ); };
    /*! Add a new section of Options to the end of the currently active
        Option's sections list.
        The new section is named \a name, has the optional
	type specifier \a type, some \a flag for selecting this section,
	and is formatted according to the \a style flag.
        Subsequent calls to addText(), addNumber(), etc. add new Parameter
	to the added section.
        \sa newSection(), newSubSection(), newSubSubSection(), insertSection(),
	endSection(), clearSections() */
  Options &addSection( const string &name, const string &type="",
		       int flag=0, int style=0 );
  Options &addSection( const string &name, int flag, int style=0 )
    { return addSection( name, "", flag, style ); };
    /*! Insert a new section of Options before the section
        specified by \a atpattern.
	If \a atpattern is not found or if \atpattern is empty,
	the new section is added to the beginning or the end
	of the currently active Options' section list, respectively.
        The new section is named \a name, has the optional
	type specifier \a type, some \a flag for selecting this section,
	and is formatted according to the \a style flag.
        Subsequent calls to addText(), addNumber(), etc. add new Parameter
	to the inserted section.
        \sa newSection(), newSubSection(), newSubSubSection(), addSection(),
	endSection(), clearSections(), findSection()  */
  Options &insertSection( const string &name, const string &atpattern,
			  const string &type="", int flag=0, int style=0 );
  Options &insertSection( const string &name, const string &atpattern,
			  int flag, int style=0 )
    { return insertSection( name, atpattern, "", flag, style ); };

    /*! Add \a opt as a new subsection of level \a level. Only
        name-value pairs and sections as specified by \a selectmask
        are taken from \a opt. If \a name is not an empty string, the
        name of the new section is set to \a name.  If \a type is not
        an empty string, the type of the new section is set to \a
        type.  \a flag and \a style are added to the new sections \a
        flag and \a style flag, respectively.  \a level = 0 is the
        upper level, i.e. a new section is added.  Higher \a levels
        add sections lower in the hierachy, i.e. \a level = 1 adds a
        subsection, \a \a level = 2 a subsubsection, etc.  Subsequent
        calls to addText(), addNumber(), etc. add new Parameter to the
        added section.  \sa newSubSection(), newSubSubSection(),
        addSection(), insertSection(), endSection(),
        clearSections() */
  Options &newSection( int level, const Options &opt, int selectmask,
		       const string &name="", const string &type="",
		       int flag=0, int style=0 );
  Options &newSection( int level, const Options &opt,
		       const string &name="", const string &type="",
		       int flag=0, int style=0 )
    { return newSection( level, opt, 0, name, type, flag, style ); };
  Options &newSection( int level, const Options &opt, const string &name,
		       int flag, int style=0 )
    { return newSection( level, opt, 0, name, "", flag, style ); };
    /*! Add \a opt as a new section to the end of this Options section
        list. Only name-value pairs and sections as specified by \a
        selectmask are taken from \a opt.  If \a name is not an empty
        string, the name of the new section is set to \a name.  If \a
        type is not an empty string, the type of the new section is
        set to \a type.  \a flag and \a style are added to the new
        sections \a flag and \a style flag, respectively.  Subsequent
        calls to addText(), addNumber(), etc. add new Parameter to the
        added section.  \sa newSubSection(), newSubSubSection(),
        addSection(), insertSection(), endSection(),
        clearSections() */
  Options &newSection( const Options &opt, int selectmask,
		       const string &name="", const string &type="",
		       int flag=0, int style=0 );
  Options &newSection( const Options &opt,
		       const string &name="", const string &type="",
		       int flag=0, int style=0 )
    { return newSection( opt, 0, name, type, flag, style ); };
  Options &newSection( const Options &opt, const string &name, int flag, int style=0 )
    { return newSection( opt, 0, name, "", flag, style ); };
    /*! Add \a opt as a new subsection to the last section. Only
        name-value pairs and sections as specified by \a selectmask
        are taken from \a opt.  If \a name is not an empty string, the
        name of the new section is set to \a name.  If \a type is not
        an empty string, the type of the new section is set to \a
        type.  \a flag and \a style are added to the new sections \a
        flag and \a style flag, respectively.  Subsequent calls to
        addText(), addNumber(), etc. add new Parameter to the added
        section.  \note You can only add a subsection after having
        added a section!  \sa newSection(), newSubSubSection(),
        addSection(), insertSection(), endSection(),
        clearSections() */
  Options &newSubSection( const Options &opt, int selectmask,
			  const string &name="", const string &type="",
			  int flag=0, int style=0 );
  Options &newSubSection( const Options &opt,
			  const string &name="", const string &type="",
			  int flag=0, int style=0 )
    { return newSubSection( opt, 0, name, type, flag, style ); };
  Options &newSubSection( const Options &opt, const string &name, int flag, int style=0 )
    { return newSubSection( opt, 0, name, "", flag, style ); };
    /*! Add \a opt as a new subsubsection to the last subsection of
        the last section. Only name-value pairs and sections as
        specified by \a selectmask are taken from \a opt.  If \a name
        is not an empty string, the name of the new section is set to
        \a name.  If \a type is not an empty string, the type of the
        new section is set to \a type.  \a flag and \a style are added
        to the new sections \a flag and \a style flag, respectively.
        Subsequent calls to addText(), addNumber(), etc. add new
        Parameter to the added section.  \note You can only add a
        subsubsection after having added a subsection!  \sa
        newSection(), newSubSection(), addSection(), insertSection(),
        endSection(), clearSections() */
  Options &newSubSubSection( const Options &opt, int selectmask,
			     const string &name="", const string &type="",
			     int flag=0, int style=0 );
  Options &newSubSubSection( const Options &opt,
			     const string &name="", const string &type="",
			     int flag=0, int style=0 )
    { return newSubSubSection( opt, 0, name, type, flag, style ); };
  Options &newSubSubSection( const Options &opt, const string &name, int flag, int style=0 )
    { return newSubSubSection( opt, 0, name, "", flag, style ); };
    /*! Add \a opt as a new section to the end of the currently active
        Option's sections list. Only name-value pairs and sections as
        specified by \a selectmask are taken from \a opt.  The new
        section is named \a name, has the optional type specifier \a
        type, some \a flag for selecting this section, and is
        formatted according to the \a style flag.  Subsequent calls to
        addText(), addNumber(), etc. add new Parameter to the added
        section.
        \sa newSection(), newSubSection(), newSubSubSection(), insertSection(),
	endSection(), clearSections() */
  Options &addSection( const Options &opt, int selectmask,
		       const string &name="", const string &type="",
		       int flag=0, int style=0 );
  Options &addSection( const Options &opt,
		       const string &name="", const string &type="",
		       int flag=0, int style=0 )
    { return addSection( opt, 0, name, type, flag, style ); };
  Options &addSection( const Options &opt, const string &name, int flag, int style=0 )
    { return addSection( opt, 0, name, "", flag, style ); };
    /*! Insert \a opt as a new section of Options before the section
        specified by \a atpattern. Only name-value pairs and sections
        as specified by \a selectmask are taken from \a opt.  If \a
        atpattern is not found or if \atpattern is empty, the new
        section is added to the beginning or the end of the currently
        active Options' section list, respectively.  If \a name is not
        an empty string, the name of the new section is set to \a
        name.  If \a type is not an empty string, the type of the new
        section is set to \a type.  \a flag and \a style are added to
        the new sections \a flag and \a style flag, respectively.
        Subsequent calls to addText(), addNumber(), etc. add new
        Parameter to the inserted section.  \sa newSection(),
        newSubSection(), newSubSubSection(), addSection(),
        endSection(), clearSections(), findSection()  */
  Options &insertSection( const Options &opt, int selectmask,
			  const string &name, const string &atpattern,
			  const string &type="", int flag=0, int style=0 );
  Options &insertSection( const Options &opt,
			  const string &name, const string &atpattern,
			  const string &type="", int flag=0, int style=0 )
    { return insertSection( opt, 0, name, atpattern, type, flag, style ); };
  Options &insertSection( const Options &opt, const string &name, const string &atpattern,
			  int flag, int style=0 )
    { return insertSection( opt, 0, name, atpattern, "", flag, style ); };

    /*! Add \a opt as a section. Only a pointer of \a opt is stored,
	the content of \a opt is not copied.
	If \a opt does not have a parentSection() or \a newparent is \c true
	then its parent section	is set to \a this.
        Subsequent calls to addText(), addNumber(), etc. still add new
	Parameter to the currently active section. */
  Options &newSection( Options *opt, bool newparent=false );
    /*! Add \a opt as a section to the end of the currently active
        Option's sections list. Only a pointer of \a opt is stored,
	the content of \a opt is not copied.
	If \a opt does not have a parentSection() or \a newparent is \c true
	then its parent section	is set to currently active section.
        Subsequent calls to addText(), addNumber(), etc. still add new
	Parameter to the currently active section. */
  Options &addSection( Options *opt, bool newparent=false );
    /*! Insert \a opt as a section of Options before the section
        specified by \a atpattern.  If \a atpattern is not found or if
        \atpattern is empty, the new section is added to the beginning
        or the end of the currently active Options' section list,
        respectively. Only a pointer of \a opt is stored,
	the content of \a opt is not copied.
	If \a opt does not have a parentSection() or \a newparent is \c true
	then its parent section	is set to its parent in the new tree.
        Subsequent calls to addText(), addNumber(), etc. still add new
	Parameter to the currently active section. */
  Options &insertSection( Options *opt, const string &atpattern,
			  bool newparent=false );

    /*! Add all sections of \a opt as a section. Only a pointer of the
	sections is stored, their content is not copied.  If \a
	newparent is \c true then the sections' parent section is set
	to \a this.  Subsequent calls to addText(), addNumber(),
	etc. still add new Parameter to the currently active
	section. */
  Options &newSections( Options *opt, bool newparent=false );

    /*! End the currently active section such that subsequent calls
        to addText(), addNumber(), etc. add new Parameter
	to the parent section.
        \sa clearSections(), setSection(), lastSection(), newSection(), newSubSection(), newSubSubSection(),
	insertSection() */
  void endSection( void );
    /*! Reset the currently active section such that subsequent calls
        to addText(), addNumber(), etc. add new Parameter
	to this Options.
        \sa endSection(), setSection(), lastSection(), newSection(), newSubSection(), newSubSubSection(),
	insertSection() */
  void clearSections( void );
    /*! Make \a opt the currently active section of \a this such that
        subsequent calls to addText(), addNumber(), etc. add new
        Parameter to \a opt.
        \sa endSection(), setSection(), lastSection(), newSection(), newSubSection(), newSubSubSection(),
	insertSection() */
  void setSection( Options &opt );
    /*! Make the last top-level section of \a this the currently active section such that
        addText(), addNumber(), etc. add new Parameter to the last top-level section.
	If \a this does not have sections, \a this is made the currently active section.
        \sa endSection(), clearSection(), setSection() */
  void lastSection( void );

    /*! Move this Options with its name-value pairs and sections
        one level up in the hierachy to its parentSection().
	\return
	- 0 successfully moved the Options up
        - -1 failed because this Options does not have a valid parentSection()
        - +2 moved Options up but deleted existing name-value pairs of the parentSection()
        - +4 moved Options up but deleted further sections of the parentSection() */
  int up( void );
    /*! Make this Options with its name-value pairs and sections
        a section of this, i.e. move it down the hierarchy.
	\return
	- 0 successfully moved the Options down
        - -1 failed because this Options does not have a name or type */
  int down( void );

    /*! Set value of option with name \a name
        to its default. */
  Parameter &setDefault( const string &name );
    /*! Set values of all options to their default. */
  Options &setDefaults( int flags=0 );

    /*! Set default of option with name \a name
        to its value. */
  Parameter &setToDefault( const string &name );
    /*! Set defaults of all options to their values. */
  Options &setToDefaults( int flags=0 );

    /*! Remove the Parameter where \a p points to.
        \a p can also be contained in a subsection of Options. */
  Options &erase( iterator p );
    /*! Remove the Options where \a s points to.
        \a s can also be contained in a subsection of Options. */
  Options &erase( section_iterator p );
    /*! Remove the Options where \a s points to.
        \a s can also be contained in a subsection of Options. */
  Options &erase( Options *s );
    /*! Remove all Parameter and Options specified by \a pattern.
        \sa find(), findSection() */
  Options &erase( const string &pattern );
    /*! Remove all Parameter and Options whose flag matches \a selectflag. */
  Options &erase( int selectflag );
    /*! Remove last Parameter from the currently active section.
        \sa endSection(), clearSections() */
  Options &pop( void );
    /*! Remove last Options from the currently active section.
        \sa endSection(), clearSections() */
  Options &popSection( void );
    /*! Remove all Parameter and sections of Options.  If \a revert is
        set to \c true, also revert parentSection() of non-owned
        sections to zero. */
  Options &clear( bool revert=true );

    /*! Total number of name-value pairs in this Options and all its sections.
        \sa parameterSize(), sectionSize() */
  int size( void ) const;
    /*! Total number of name-value pairs in this Options and all its sections
        that have \a selectflag set in their flags().
        If \a selectflag equals zero, all options are counted.
	If \a selectflag is negative, only options whose values differ
	from the default value and have abs(\a selectflag) set in their flags
	are counted.
        If \a selectflag equals NonDefault, all options whose values differ
	from their default value are counted.
        The flag() of the sections is not used by this function. */
  int size( int selectflag ) const;
    /*! True if there are no name-value pairs in this Options and all
        its sections. */
  bool empty( void ) const;
    /*! \return the number of name-value pairs of this Options
        without the ones in its sections.
	\sa sectionsSize(), size() */
  int parameterSize( void ) const;
    /*! \return the number of name-value pairs of this Options
        that have \a flag set in their flags() without the ones in its sections.
	\sa sectionsSize(), size() */
  int parameterSize( int flags ) const;
    /*! \return the number of sections of this Options without subsections.
	\sa parameterSize(), size() */
  int sectionsSize( void ) const;
    /*! \return the number of sections of this Options
        that contain at least on name-value pair with \a flag set in its flags().
	\sa parameterSize(), size() */
  int sectionsSize( int flags ) const;
    /*! True if a name-value pair with name \a name exist somewhere in
        the hierarchy of Options.
        \sa find(), existSection() */
  bool exist( const string &pattern ) const;
    /*! True if a section (including the name() of this Options) of
        name-value pairs with name \a name exist somewhere in the
        hierarchy of Options.
	\sa findSection(), exist() */
  bool existSection( const string &pattern ) const;

    /*! Set flags of all options that are selected by \a selectflag to \a flags.
        If \a selectflag equals zero, all options are considered.
	If \a selectflag is negative, only options whose values differ
	from the default value and have abs(\a selectflag) set in their flags
	are considered.
        If \a selectflag equals NonDefault, all options whose values differ
	from their default value are considered. */
  Options &setFlags( int flags, int selectflag=0 );
    /*! Add the bits specified by \a flags to the flags of all options
        that are selected by \a selectflag.
        If \a selectflag equals zero, all options are considered.
	If \a selectflag is negative, only options whose values differ
	from the default value and have abs(\a selectflag) set in their flags
	are considered.
        If \a selectflag equals NonDefault, all options whose values differ
	from their default value are considered. */
  Options &addFlags( int flags, int selectflag=0 );
    /*! Clear the bits specified by \a flags of the flags of all options
        that are selected by \a selectflag.
        If \a selectflag equals zero, all options are considered.
	If \a selectflag is negative, only options whose values differ
	from the default value and have abs(\a selectflag) set in their flags
	are considered.
        If \a selectflag equals NonDefault, all options whose values differ
	from their default value are considered. */
  Options &delFlags( int flags, int selectflag=0 );

    /*! Set flags of all options whose type matches \a typemask
        (see Parameter::valueTypes() ) to \a flags. */
  Options &setValueTypeFlags( int flags, int typemask );
    /*! Add \a flags to the flags of all options
        whose type matches \a typemask (see Parameter::valueTypes() ). */
  Options &addValueTypeFlags( int flags, int typemask );
    /*! Clear the bits specified by \a flags of the flags of all options
        whose type matches \a typemask (see Parameter::vallueTypes() ). */
  Options &delValueTypeFlags( int flags, int typemask );

    /*! Set style of all Parameter and sections that match \a selectflag
        to \a style. */
  Options &setStyles( int style, int selectflag=0 );
    /*! Add the bits specified by \a style to the style of
        all Parameter and sections that match \a selectflag. */
  Options &addStyles( int style, int selectflag=0 );
    /*! Clear the bits specified by \a style of the style of
        all Parameter and sections that match \a selectflag. */
  Options &delStyles( int style, int selectflag=0 );
    /*! Total number of Parameter in all asections
        that have the style \a style set in their style(). */
  int styleSize( int style ) const;

    /*! Set style of all Parameter and sections whose type matches \a typemask
        (see Parameter::valueTypes() ) to \a style. */
  Options &setValueTypeStyles( int style, int typemask );
    /*! Add the bits specified by \a style to the style of all Parameter
        and sections whose type matches \a typemask
	(see Parameter::valueTypes() ). */
  Options &addValueTypeStyles( int style, int typemask );
    /*! Clear the bits specified by \a style of the style of all Parameter
        and sections whose type matches \a typemask
	(see Parameter::valueTypes() ). */
  Options &delValueTypeStyles( int style, int typemask );

    /*! Length of largest name.
        If \a detailed is \c true, then include length of request string as well. */
  int nameWidth( int selectmask=0, bool detailed=false ) const;

    /*! Write names and their values to a string
        separated by ", ".
	Saves only options that have \a selectmask set in their flags().
        If \a selectmask equals zero, all options are saved.
	If \a selectmask is negative, only options whose values differ
	from the default value and have abs(\a selectmask) set in their flags
	are saved.
        If \a selectmask equals NonDefault, all options whose values differ
	from their default value are saved.
        Use for \a flags values from SaveFlags to modify the way
	how the Options are written to the string. */
  string save( int selectmask=0, int flags=0 ) const;
    /*! Write names and their values to stream \a str
        as a valid YAML document.
        Start each line with \a start.
	Saves only options that have \a selectmask set in their flags().
        If \a selectmask equals zero, all options are saved.
	If \a selectmask is negative, only options whose values differ
	from the default value and have abs(\a selectmask) set in their flags
	are saved.
        If \a selectmask equals NonDefault, all options whose values differ
	from their default value are saved.
        Use for \a flags values from SaveFlags to modify the way
	how the Options are written to the string. */
  ostream &save( ostream &str, const string &start="",
		 int selectmask=0, int flags=0,
		 int width=-1 ) const;
    /*! Write each name-value pair as a separate line to stream \a str
        and use \a textformat, \a numberformat, \a boolformat, \a
        dateformat, \a timeformat, and \a sectionformat for formatting
        text, number, boolean, date, and time parameter, or sections, respectively.
        Each line is started with \a start. */
  ostream &save( ostream &str, const string &textformat,
		 const string &numberformat, const string &boolformat="%i=%b",
		 const string &dateformat="%i=%04Y-%02m-%02d", const string &timeformat="%i=%02H:%02M:%02S",
		 const string &sectionformat="%i", int selectmask=0,
		 const string &start="" ) const;
    /*! Write names and their values to stream \a str */
  friend ostream &operator<< ( ostream &str, const Options &o );

    /*! Write options in XML format to output stream.
        \param[in] str the output stream
        \param[in] selectmask selects options that have \a selectmask set in their flags().
                   See Parameter::flags(int) for details.
        \param[in] flags some details on how the data should be saved.
        \param[in] level the level of indentation
	\param[in] indent the indentation depth,
                   i.e. number of white space characters per level
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int selectmask=0, int flags=0, int level=0,
		    int indent=4 ) const;

    /*! Read options from string \a opttxt of the form
        "parameter1: x, parameter2: y, parameter3: z, ...".
        If the parameters 'parameter1', 'parameter2', ... match the
        name of an option, its value is set to x, y, z, ... respectively,
	and \a flag is added to its flags, if \a flag is not 0.
        \return the Options for which to continue to read. */
  Options &read( const string &opttxt, int flag,
		 const string &assignment=":=", const string &separator=",;",
		 int *indent=0 );
  Options &read( const string &opttxt, const string &assignment=":=",
		 const string &separator=",;" )
    { return read( opttxt, 0, assignment, separator ); };
    /*! Read a single line from stream \a str and set options. */
  friend istream &operator>> ( istream &str, Options &o );
    /*! Read from stream \a str and set the values of existing
        options, until end of file
        or a line beginning with \a stop is reached.
	If \a stop equals the \a StopEmpty string,
	reading is stopped at an empty line.
	Before reading the option comments are removed from each line using
	Str::stripComment( \a comment ).
	If \a line does not equal zero
	then the last read line is returned in \a line.
	The warning message is set. */
  istream &read( istream &str, int flag, const string &assignment=":=",
		 const string &comment="#",
		 const string &stop="", Str *line=0 );
  istream &read( istream &str, const string &assignment=":=",
		 const string &comment="#",
		 const string &stop="", Str *line=0 )
    { return read( str, 0, assignment, comment, stop, line ); };
    /*! Read options from the list of strings \a sq.
	Each line is assumed to be a single option.
	The warning message is set. */
  Options &read( const StrQueue &sq, int flag=0,
		 const string &assignment=":=" );

    /*! Read options from the parameter \a p.
        Returns \c true if \a p was read and
        ad \a flag to the flags of the Options that read it. */
  bool read( const Parameter &p, int flag=0 );
    /*! Read options from the options \a o.
        A subset of the options can be selected by \a flags.
        Options that read a value get \a flag added to their flags. */
  Options &read( const Options &o, int flags=0, int flag=0 );

    /*! Read options from the parameter \a p.
        If no option with the name equal to \a p's name
        exist \a p is appended to the active section
	of Options.
        Returns \c true if \a p was read, \c false if it was appended.
        \sa read(), endSection(), clearSections() */
  bool readAppend( const Parameter &p );
    /*! Read options from the options \a o.
        All options \a o that do not exist are appended.. */
  Options &readAppend( const Options &o, int flags=0 );

    /*! Load options from string \a opttxt of the form
        "parameter1=x; parameter2=y; parameter3=z; ...".
        New options are created with the names and request strings
	set to parameter1, parameter2, parameter3, ...
	and their values set to x, y, z, ..., respectively. */
  Options &load( const Str &opttxt,
		 const string &assignment=":=", const string &separator=",;",
		 int *indent=0, int *indentspacing=0, int *level=0 );
    /*! Read from stream \a str and create new options, until end of file
        or a line beginning with \a stop is reached.
	If \a stop equals the \a StopEmpty string,
	reading is stopped at an empty line.
	Each line is assumed to be a single option, which is loaded with
	Parameter::load().
	Before loading the option comments are removed from each line using
	Str::stripComment( \a comment ).
	If \a line does not equal zero
	then the last read line is returned in \a line.
	The warning message is set. */
  istream &load( istream &str, const string &assignment=":=",
		 const string &comment="#",
		 const string &stop="", string *line=0 );
    /*! Create new options from the list of strings \a sq.
	Each line is assumed to be a single option, which is loaded with
	Parameter::load().
	The warning message is set. */
  Options &load( const StrQueue &sq, const string &assignment=":=" );

    /*! This function can be reimplemented to react to a change of the value
        of some parameters.
        This function is called after each of the setNumber(), setText(), etc
        functions. */
  virtual void notify( void ) {};
    /*! Calls the rootSections() and this Options notify() functions
        if appropriate.
        \sa notif(), setNotify() */
  void callNotifies( void );
    /*! Enables the call of the notify() function if \a notify equals \c true. */
  void setNotify( bool notify=true );
    /*! Disables the call of the notify() function. */
  void unsetNotify( void );
    /*! Returns true if calling the notify() function is enabled. */
  bool notifying( void ) const { return CallNotify; };


private:


    /*! A pointer to the Options this Options belongs to. */
  Options *ParentSection;

    /*! Name of this section of options. */
  string Name;
    /*! Type of this section of options. */
  string Type;
    /*! An optional include path for this section of options. */
  string Include;
    /*! Flag for this section. */
  int Flag;
    /*! Style flag for the section Name and Type. */
  int Style;
    /*! The options. */
  deque< Parameter > Opt;
    /*! Sections of options. */
  deque< Options* > Secs;
    /*! True if the corresponding section in \a Secs is owned by this Options,
        i.e. whether it should delete it. */
  deque< bool > OwnSecs;
    /*! Pointer to the Options where addText(), addNumber(), etc. should be added. */
  Options *AddOpts;
    /*! A warning message. */
  mutable Str Warning;
    /*! Avoid recursive call of notify(). */
  bool Notified;
    /*! Enables calling the notify() function. */
  bool CallNotify;

    /*! Dummy Parameter for index operator. */
  static Parameter Dummy;
    /*! Dummy section for index function. */
  static Options SecDummy;

};


}; /* namespace relacs */

#endif /* ! _RELACS_OPTIONS_H_ */
