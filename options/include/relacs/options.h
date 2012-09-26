/*
  options.h
  The Options class contains options

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
\version 1.0
\brief The Options class contains options
which can be modified with a dialog.
\bug return values of type Parameter& are not defined if identifier is not found!
\bug takeFirst, takeLast, combineFirst, combineLast should use find instead of ==


Each option is a key-value pair identified by a string \a ident and
has a default value \a dflt.  For the dialog the string \a request is
used to request the option.  Number options in addition have a minimum
and maximum value, a step size, a unit and a format string.

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

The total number of defined options is returned by size().
To check wether an option with a certain identifier exist use exist().

The read(), readAppend(), load() and save() functions read the values
of options, load new options, and save options from and to strings or files.
*/


class Options
{

public:

  static const int NonDefault = Parameter::NonDefault;

    /*! Constructs an empty options list. */
  Options( void );
    /*! Copy constructor. */
  Options( const Options &o );
    /*! Copy Options \a o that have flags() & \a flags greater than zero
        to this. If \a flags equals zero, all options are copied. */
  Options( const Options &o, int flags );
  /*! Create options from string \a opttxt.
      See load( const Str &opttxt, const string &separator ) for details. */
  Options( const Str &opttxt, const string &assignment=":=",
	   const string &separator=";" );
  /*! Create options from strings \a sq.
      See load( const StrQueue &sq ) for details. */
  Options( const StrQueue &sq, const string &assignment=":=" );
  /*! Create options from input stream \a str.
      See load( istream &str, const string &comment, const string &stop, string *line ) for details. */
  Options( istream &str, const string &assignment=":=",
	   const string &comment="", 
	   const string &stop="", string *line=0 );
    /*! Deconstructs an options list. */
  virtual ~Options( void );

    /*! Copy Options \a o. */
  Options &operator=( const Options &o );
    /*! Copy Options \a o to this. */
  Options &assign( const Options &o );
    /*! Append Options \a o to this. */
  Options &append( const Options &o );
    /*! Insert Options \a o at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the options are appended to the end of the list. */
  Options &insert( const Options &o, const string &atident="" );

    /*! Copy Options \a o that have flags() & \a flags greater than zero
        to this. If \a flags equals zero, all options are copied.
	If \a flags is negative, only options whose values differ
	from the default value and have abs(\a flags) set in their flags()
	are copied.
        If \a flags equals NonDefault, all options whose values differ
	from their default value are copied. */
  Options &assign( const Options &o, int flags );
    /*! Copy all options to \a that have flags() & \a flags greater than zero.
        If \a flags equals zero, all options are copied.
	If \a flags is negative, only options whose values differ
	from the default value and have abs(\a flags) set in their flags()
	are copied.
        If \a flags equals NonDefault, all options whose values differ
	from their default value are copied. */
  Options &copy( Options &o, int flags );
    /*! Append all options to \a that have flags() & \a flags greater than zero.
        If \a flags equals zero, all options are appended.
	If \a flags is negative, only options whose values differ
	from the default value and have abs(\a flags) set in their flags()
	are copied.
        If \a flags equals NonDefault, all options whose values differ
	from their default value are copied. */
  Options &append( const Options &o, int flags );
    /*! Insert Options \a o that have flags() & \a flags greater than zero
        at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the options are appended to the end of the list. */
  Options &insert( const Options &o, int flags, const string &atident="" );

    /*! Set the value of an existing option 
        with identifier \a ident to \a value.
	Set warning message, if the \a value is invalid.
        Used by read(). */
  Parameter *assign( const string &ident, const string &value );

    /*! Returns true if the two Options \a o1 and \a o2 are equal,
        i.e. they have the same number of Parameter with identical identifier
	and value (as returned by Parameter::text()). */
  friend bool operator==( const Options &o1, const Options &o2 );
    /*! Returns true if the name() of the Options \a o equals \a name. */
  friend bool operator==( const Options &o, const string &name );
    /*! Returns true if the Option \a o1 is smaller than \a o2,
        i.e. \a o2 has less elements than \a o1,
	an identifier of \a o2 is smaller than the corresponding one in \a o1,
	or a value of \a o2 is smaller than the corresponding one in \a o1.
        This function is provided just to define some ordering of Options,
	as is needed for example for an stl::map. */
  friend bool operator<( const Options &o1, const Options &o2 );

    /*! Returns a pointer to the Options where this Options belongs to
        as a section.
        If this Options does not belong to an Options, NULL is returned. */
  Options *parentSection( void );
    /*! Returns a const pointer to the Options where this Options belongs to
        as a section.
        If this Options does not belong to an Options, NULL is returned. */
  const Options *parentSection( void ) const;
    /*! Set the parent Options of this Options to \a parent. */
  void setParentSection( Options *parentsection );

    /*! The name of this section of options. */
  string name( void ) const;
    /*! Set the name of this section of options to \a name. */
  void setName( const string &name );

    /*! Returns the warning messages of the last called 
        Option member-function. */
  Str warning( void ) const { return Warning; };

  typedef deque< Parameter >::iterator iterator;
  inline iterator begin( void ) { return Opt.begin(); };
  inline iterator end( void ) { return Opt.end(); };
  typedef deque< Parameter >::const_iterator const_iterator;
  inline const_iterator begin( void ) const { return Opt.begin(); };
  inline const_iterator end( void ) const { return Opt.end(); };

  typedef deque< Options >::iterator section_iterator;
  inline section_iterator sectionsBegin( void ) { return Secs.begin(); };
  inline section_iterator sectionsEnd( void ) { return Secs.end(); };
  typedef deque< Options >::const_iterator const_section_iterator;
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
  const_iterator find( const string &pattern ) const;
    /*! Search for the first option that matches \a pattern.
        \a pattern can be a list of search terms separated by '>',
	for example 'aaa>bbb', to search parameter 'bbb' in section 'aaa'.
        Alternative search terms can be separated by '|'.
	For example, if \a pattern is "date|time", then
	"date" is searched first and if this is not found,
	"time" is searched.
        Returns end() if no match for \a pattern is found. */
  iterator find( const string &pattern );
    /*! Search for the last option that matches \a pattern.
        Returns end() if \a ident is not found. 
        See find() for details about valid patterns \a pattern. */
  const_iterator rfind( const string &ident ) const;
    /*! Search for the last option that matches \a pattern.
        Returns end() if \a ident is not found. 
        See find() for details about valid patterns \a pattern. */
  iterator rfind( const string &ident );

    /*! Get \a i-th options. */
  const Parameter &operator[]( int i ) const;
    /*! Get \a i-th options. */
  Parameter &operator[]( int i );
    /*! Get the option with identifier \a ident. */
  const Parameter &operator[]( const string &ident ) const;
    /*! Get the option with identifier \a ident. */
  Parameter &operator[]( const string &ident );

      /*! Returns the request string of the option 
          with identifier equal to \a ident. */
  Str request( const string &ident ) const;
    /*! Set request string of the option 
        with identifier equal to \a ident to \a request. */
  Parameter &setRequest( const string &ident, const string &request );

    /*! The type of the option 
        with identifier equal to \a ident. */
  Parameter::Type type( const string &ident ) const;
    /*! Set type of the option 
        with identifier equal to \a ident to \a pt. */
  Parameter &setType( const string &ident, Parameter::Type pt );

    /*! The flags of the option 
        with identifier equal to \a ident. */
  int flags( const string &ident ) const;
    /*! Set the flags of the option 
        with identifier equal to \a ident to \a flags. */
  Parameter &setFlags( const string &ident, int flags );
    /*! Add the bits specified by \a flags to the flags of the option 
        with identifier equal to \a ident. */
  Parameter &addFlags( const string &ident, int flags );
    /*! Clear the bits specified by \a flags of the flags of the option 
        with identifier equal to \a ident. */
  Parameter &delFlags( const string &ident, int flags );
    /*! Clear all bits of the flags of the option 
        with identifier equal to \a ident. */
  Parameter &clearFlags( const string &ident );
    /*! Return \a true if the changedFlag() of the option 
        with identifier equal to \a ident is set,
        i.e. whose value has been changed.. */
  bool changed( const string &ident );

    /*! The style of the option 
        with identifier equal to \a ident. */
  int style( const string &ident ) const;
    /*! Set the style of the option 
        with identifier equal to \a ident to \a style. */
  Parameter &setStyle( const string &ident, int style );
    /*! Add the bits specified by \a style to the style of the option 
        with identifier equal to \a ident. */
  Parameter &addStyle( const string &ident, int style );
    /*! Clear the bits specified by \a style of the style of the option 
        with identifier equal to \a ident. */
  Parameter &delStyle( const string &ident, int style );

    /*! Return the format string of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, an empty string is returned. */
  Str format( const string &ident ) const;
    /*! Set format string of an existing number option 
        with identifier \a ident to have a width of \a width, 
	precision of \a prec and type \a fmt. */
  Parameter &setFormat( const string &ident, int width=0, int prec=-1,
			char fmt='g' );
    /*! Set the format of an existing number option 
        with identifier \a ident to \a format. */
  Parameter &setFormat( const string &ident, const string &format );
    /*! Returns the width specified by the format string of the option
        specified by \a ident. */
  int formatWidth( const string &ident ) const;

    /*! If several values correspond to the option
        with identifier equal to \a ident
        size() returns the number of values. */
  int size( const string &ident ) const;

    /*! Add parameter \a np to options. */
  Parameter &add( const Parameter &np );
    /*! Insert a new parameter \a np at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list. */
  Parameter &insert( const Parameter &np, const string &atident="" );

    /*! Add a text option at the end of the options list. 
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt. */
  Parameter &addText( const string &ident, const string &request,  
		      const string &dflt, int flags=0, int style=0 );
    /*! Add a text option at the end of the options list. 
        Its identifier string and its request string for the options dialog 
	are set to \a ident.
        The option has a default value \a dflt. */
  inline Parameter &addText( const string &ident, const string &dflt="", 
			     int flags=0, int style=0 )
    { return addText( ident, ident, dflt, flags, style ); };
    /*! Insert a new text option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt. */
  Parameter &insertText( const string &ident, const string &atident, 
			 const string &request, const string &dflt,
			 int flags=0, int style=0 );
    /*! Insert a new text option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and its request string for the options dialog 
	are set to \a ident.
        The option has a default value \a dflt. */
  inline Parameter &insertText( const string &ident, const string &atident="", 
				const string &dflt="", int flags=0, int style=0 )
    { return insertText( ident, atident, ident, dflt, flags, style ); };
    /*! Return the \a index-th string of the text option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	not a text, an empty string is returned. */
  Str text( const string &ident, int index, const string &dflt="",
	    const string &format="", const string &unit="" ) const;
    /*! Return the first string of the text option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	not a text, an empty string is returned. */
  inline Str text( const string &ident, const string &dflt="",
		   const string &format="", const string &unit="" ) const
    { return text( ident, 0, dflt, format, unit ); };
    /*! Set the value of an existing text option 
        with identifier \a ident to \a strg. */
  Parameter &setText( const string &ident, const string &strg );
    /*! Set value of an existing text option 
        with identifier \a ident to the one of \a p,
        if both are of type() Text.
	If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setText( const string &ident, const Parameter &p );
    /*! Add \a strg to the value of an existing text option 
        with identifier \a ident. */
  Parameter &pushText( const string &ident, const string &strg );
    /*! Return the default string of the text option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	not a text, an empty string is returned. */
  Str defaultText( const string &ident,
		   const string &format="", const string &unit="" ) const;
    /*! Set the default string of an existing text option 
        with identifier \a ident to \a dflt. */
  Parameter &setDefaultText( const string &ident, const string &dflt );

    /*! Add a text-selection option at the end of the options list. 
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a list \a selection of strings (separated by '|')
        that can be selected. */
  Parameter &addSelection( const string &ident, const string &request,  
			   const string &selection, int flags=0, int style=0 );
    /*! Add a text option at the end of the options list. 
        Its identifier string and its request string for the options dialog 
	are set to \a ident.
	The option has a list \a selection of strings (separated by '|')
        that can be selected. */
  inline Parameter &addSelection( const string &ident, const string &selection="", 
			     int flags=0, int style=0 )
    { return addSelection( ident, ident, selection, flags, style ); };
    /*! Insert a new text option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
	The option has a list \a selection of strings (separated by '|')
        that can be selected. */
  Parameter &insertSelection( const string &ident, const string &atident, 
			      const string &request, const string &selection,
			      int flags=0, int style=0 );
    /*! Insert a new text option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and its request string for the options dialog 
	are set to \a ident.
	The option has a list \a selection of strings (separated by '|')
        that can be selected. */
  inline Parameter &insertSelection( const string &ident, const string &atident="", 
				     const string &selection="", int flags=0, int style=0 )
    { return insertSelection( ident, atident, ident, selection, flags, style ); };
    /*! If the text parameter has several values,
        then the value that is matched by \a strg is
        inserted as the first value.
        If \a strg does not match any of the text values
        \a strg is added to the text values and is marked as selected
	if \a add > 0, or if \a add == 0 and the SelectText - Bit
	in the Parameter's style is not set. */
  Parameter &selectText( const string &ident, const string &strg,
			 int add=0 );
    /*! Returns the index of the selected value, i.e. 
        the index minus one of the matching value with the first value. */
  int index( const string &ident ) const;
    /*! Returns the index of the value that matches \a strg. */
  int index( const string &ident, const string &strg ) const;

    /*! Add a new number option at the end of the options list. 
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  Parameter &addNumber( const string &ident, const string &request,  
			double dflt, double minimum,
			double maximum=MAXDOUBLE, double step=1.0,
			const string &unit="", const string &outputunit="", 
			const string &format="", int flags=0, int style=0 );
    /*! Add a new number option at the end of the options list. 
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt,
        a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  inline Parameter &addNumber( const string &ident, const string &request,
			       double dflt, const string &unit="", 
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( ident, request, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ); };
    /*! Add a new number option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt,
        a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  inline Parameter &addNumber( const string &ident, double dflt,
                               const string &unit="", 
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( ident, ident, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ); };
    /*! Add a new number option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt and
        a unit \a unit. */
  inline Parameter &addNumber( const string &ident, double dflt,
                               const string &unit, int flags, int style=0 )
    { return addNumber( ident, ident, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, "", flags, style ); };
    /*! Add a new number option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt,
	an error \a error, a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  inline Parameter &addNumber( const string &ident, double dflt,
                               double error, const string &unit="", 
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( ident, ident, dflt, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ).setNumber( dflt, error ); };
    /*! Add a new number option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  inline Parameter &addNumber( const string &ident, const string &unit="", 
			       const string &format="", int flags=0, int style=0 )
    { return addNumber( ident, ident, 0.0, -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit, format, flags, style ); };
    /*! Insert a new number option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  Parameter &insertNumber( const string &ident, const string &atident, 
			   const string &request, double dflt, 
			   double minimum, double maximum=MAXDOUBLE,
			   double step=1.0,
			   const string &unit="", const string &outputunit="", 
			   const string &format="", int flags=0, int style=0 );
    /*! Insert a new number option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt,
        a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  inline Parameter &insertNumber( const string &ident, const string &atident,
				  const string &request, double dflt, const string &unit="", 
				  const string &format="", int flags=0, int style=0 )
    { return insertNumber( ident, atident, request, dflt,
			   -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit,
			   format, flags, style ); };
    /*! Insert a new number option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt,
        a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  inline Parameter &insertNumber( const string &ident, const string &atident,
				  double dflt, const string &unit="", 
				  const string &format="", int flags=0, int style=0 )
    { return insertNumber( ident, atident, ident, dflt,
			   -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit,
			   format, flags, style ); };
    /*! Insert a new number option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident.
        The option has a unit \a unit and a format string \a format 
	(ANSI C printf() syntax, only %%g, %%f, %%e are allowed, 
	since the number is a double). */
  inline Parameter &insertNumber( const string &ident, const string &atident,
				  const string &unit="", 
				  const string &format="", int flags=0, int style=0 )
    { return insertNumber( ident, atident, ident, 0.0,
			   -MAXDOUBLE, MAXDOUBLE, 1.0, unit, unit,
			   format, flags, style ); };
    /*! Return the \a index-th number of the option with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is 
	neither a number nor an integer, zero is returned. */
  double number( const string &ident, double dflt=0.0, const string &unit="", int index=0 ) const;
  double number( const string &ident, const string &unit, double dflt=0.0, int index=0 ) const
    { return number( ident, dflt, unit, index ); };
    /*! Return the \a index-th number of the option with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is 
	neither a number nor an integer, zero is returned. */
  double number( const string &ident, int index, double dflt=0.0, const string &unit="" ) const
    { return number( ident, dflt, unit, index ); };
  double number( const string &ident, int index, const string &unit, double dflt=0.0 ) const
    { return number( ident, dflt, unit, index ); };
    /*! Return the \a index-th standard deviation of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  double error( const string &ident, const string &unit="", int index=0 ) const;
    /*! Return the \a index-th standard deviation of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  double error( const string &ident, int index, const string &unit="" ) const
    { return error( ident, unit, index ); };
    /*! Set the value of an existing number option 
        with identifier \a ident to \a number. */
  Parameter &setNumber( const string &ident, double number, double error, 
			const string &unit="" );
    /*! Set the value of an existing number option 
        with identifier \a ident to \a number. */
  inline Parameter &setNumber( const string &ident, double number,
			       const string &unit="" )
    { return setNumber( ident, number, -1.0, unit ); };
    /*! Add \a number to the value of an existing number option 
        with identifier \a ident. */
  Parameter &pushNumber( const string &ident, double number, double error=-1.0, 
			 const string &unit="" );
    /*! Add \a number to the value of an existing number option 
        with identifier \a ident. */
  inline Parameter &pushNumber( const string &ident, double number,
				const string &unit )
    { return pushNumber( ident, number, -1.0, unit ); };
    /*! Set value of an existing number option 
        with identifier \a ident to the one of \a p,
        if both are of type() Number.
	If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setNumber( const string &ident, const Parameter &p );
    /*! Return the default number of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  double defaultNumber( const string &ident, const string &unit="" ) const;
    /*! Set the default number of an existing number option 
        with identifier \a ident to \a dflt. */
  Parameter &setDefaultNumber( const string &ident, double dflt, 
			       const string &unit="" );
    /*! Return the minimum number of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  double minimum( const string &ident, const string &unit="" ) const;
    /*! Return the maximum number of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  double maximum( const string &ident, const string &unit="" ) const;
    /*! Return the step size of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, unity is returned. */
  double step( const string &ident, const string &unit="" ) const;
    /* Set the step size of an existing number option 
       with identifier \a ident to \a step of unit \a unit.
       If \a step is negative, the step size is set to \a maximum - \minimum
       devided by \a step.
       If \a step equals zero, the step size is set to 1/50 of
       \a maximum - \a minimum.
       If \a unit is empty, \a step is assumed to be given in the internal unit. */
  Parameter &setStep( const string &ident, double step, const string &unit="" );
    /*! Set the minimum and maximum of an existing number option 
        with identifier \a ident to \a minimum and \a maximum, respectively. */
  Parameter &setMinMax( const string &ident, double minimum=-MAXDOUBLE, 
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
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, an empty string is returned. */
  Str unit( const string &ident ) const;
    /*! Return the unit used for output and dialogs
        of the option with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, an empty string is returned. */
  Str outUnit( const string &ident ) const;
    /*! Set the internal unit and the output unit of an existing number option 
        with identifier \a ident to \a internunit and \a outunit, respectively.
        If \a outunit is an empty string it is set to \a internunit. */
  Parameter &setUnit( const string &ident, const string &internunit, 
		      const string &outunit="" );
    /*! Set the output unit of an existing number option 
        with identifier \a ident to \a outputunit. */
  Parameter &setOutUnit( const string &ident, const string &outputunit );
    /*! Set the internal unit of an existing number option 
        with identifier \a ident to \a inputunit.
	The values of the parameter are converted accordingly. */
  Parameter &changeUnit( const string &ident, const string &internunit );

    /*! Add a new integer option at the end of the options list. 
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a width of output format \a width. */
  Parameter &addInteger( const string &ident, const string &request,  
			 long dflt, long minimum=LONG_MIN,
			 long maximum=LONG_MAX, long step=1,
			 const string &unit="", const string &outputunit="", 
			 int width=0, int flags=0, int style=0 );
    /*! Add a new integer option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt, a unit \a unit,
	and a width of output format \a width. */
  inline Parameter &addInteger( const string &ident, long dflt,
				const string &unit="", int width=0, int flags=0, int style=0 )
    { return addInteger( ident, ident, dflt, LONG_MIN, LONG_MAX, 1,
			 unit, unit, width, flags, style ); };
    /*! Add a new integer option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a unit \a unit,
	and a width of output format \a width. */
  inline Parameter &addInteger( const string &ident, const string &unit="" )
    { return addInteger( ident, ident, 0, LONG_MIN, LONG_MAX, 1,
			 unit, unit, -1, 0 ); };
    /*! Insert a new integer option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt, minimum value \a minimum
        and maximum value \a maximum, as well as a unit \a unit
	and a width of output format \a width. */
  Parameter &insertInteger( const string &ident, const string &atident, 
			    const string &request, long dflt, 
			    long minimum=LONG_MIN, long maximum=LONG_MAX,
			    long step=1,
			    const string &unit="", const string &outputunit="", 
			    int width=0, int flags=0, int style=0 );
    /*! Insert a new integer option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt, a unit \a unit,
	and a width of output format \a width. */
  Parameter &insertInteger( const string &ident, const string &atident, 
			    long dflt, const string &unit="", 
			    int width=0, int flags=0, int style=0 )
    { return insertInteger( ident, atident, ident, dflt, LONG_MIN, LONG_MAX,
			    1, unit, unit, width, flags, style ); };
    /*! Insert a new integer option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident.
        The option has a unit \a unit,
	and a width of output format \a width. */
  Parameter &insertInteger( const string &ident, const string &atident, 
			    const string &unit="", 
			    int width=0, int flags=0, int style=0 )
    { return insertInteger( ident, atident, ident, 0, LONG_MIN, LONG_MAX,
			    1, unit, unit, width, flags, style ); };
    /*! Return the \a index-th integer number of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  long integer( const string &ident, const string &unit="", long dflt=0, int index=0 ) const;
    /*! Return the \a index-th integer number of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  long integer( const string &ident, int index, long dflt=0, const string &unit="" ) const
    { return integer( ident, unit, dflt, index ); }
  long integer( const string &ident, int index, const string &unit, long dflt=0 ) const
    { return integer( ident, unit, dflt, index ); }
   /*! Set the value of an existing integer option 
        with identifier \a ident to \a number. */
  Parameter &setInteger( const string &ident, long number, long error, 
			 const string &unit="" );
   /*! Set the value of an existing integer option 
        with identifier \a ident to \a number. */
  inline Parameter &setInteger( const string &ident, long number, const string &unit="" )
    { return setInteger( ident, number, -1, unit ); };
   /*! Add \a number to the value of an existing integer option 
        with identifier \a ident. */
  Parameter &pushInteger( const string &ident, long number, long error, 
			  const string &unit="" );
   /*! Add \a number to the value of an existing integer option 
        with identifier \a ident. */
  inline Parameter &pushInteger( const string &ident, long number, const string &unit="" )
    { return pushInteger( ident, number, -1, unit ); };
    /*! Set value of an existing integer option 
        with identifier \a ident to the one of \a p,
        if both are of type() Integer.
	If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setInteger( const string &ident, const Parameter &p );
    /*! Return the default number of the integer option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a number nor an integer, zero is returned. */
  long defaultInteger( const string &ident, const string &unit="" ) const;
    /*! Set the default number of an existing integer option 
        with identifier \a ident to \a dflt. */
  Parameter &setDefaultInteger( const string &ident, long dflt, 
				const string &unit="" );
    /*! Set the minimum and maximum of an existing integer number option 
        with identifier \a ident to \a minimum and \a maximum, respectively. */
  Parameter &setMinMax( const string &ident, long minimum=LONG_MIN, 
			long maximum=LONG_MAX, long step=1, const string &unit="" );

    /*! Add a boolean option at the end of the options list. 
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt. */
  Parameter &addBoolean( const string &ident, const string &request,  
			 bool dflt, int flags=0, int style=0 );
    /*! Add a boolean option at the end of the options list. 
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt. */
  /*
  inline Parameter &addBoolean( const string &ident, bool dflt, int flags=0, int style=0 )
    { return addBoolean( ident, ident, dflt, flags, style ); };
  */
    /*! Add a boolean option at the end of the options list. 
        Its identifier string and request string is set to \a ident. */
  inline Parameter &addBoolean( const string &ident, int flags=0, int style=0 )
    { return addBoolean( ident, ident, false, flags, style ); };
    /*! Insert a new boolean option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its request string for the options dialog is set to \a request,
        its identifier string is set to \a ident.
        The option has a default value \a dflt. */
  Parameter &insertBoolean( const string &ident, const string &atident="", 
			    const string &request="", bool dflt=false,
			    int flags=0, int style=0 );
    /*! Insert a new boolean option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident.
        The option has a default value \a dflt. */
  /*
  Parameter &insertBoolean( const string &ident, const string &atident, 
			    bool dflt, int flags=0, int style=0 )
    { return insertBoolean( ident, atident, ident, dflt, flags, style ); };
  */
    /*! Insert a new boolean option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        Its identifier string and request string is set to \a ident. */
  Parameter &insertBoolean( const string &ident, const string &atident, 
			    int flags=0, int style=0 )
    { return insertBoolean( ident, atident, ident, false, flags, style ); };
    /*! Return the boolean of the option with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a boolean, nor a number, nor an integer, 
	false is returned. */
  bool boolean( const string &ident, bool dflt=false, int index=0 ) const;
  bool boolean( const string &ident, int index, bool dflt=false ) const
    { return boolean( ident, dflt, index ); };
    /*! Set the value of an existing boolean option 
        with identifier \a ident to \a string. */
  Parameter &setBoolean( const string &ident, bool b );
    /*! Set value of an existing boolean option 
        with identifier \a ident to the one of \a p,
        if both are of type() Boolean.
	If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setBoolean( const string &ident, const Parameter &p );
    /*! Return the default boolean of the option 
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident, or the option is
	neither a boolean, nor a number nor an integer, 
	false is returned. */
  bool defaultBoolean( const string &ident ) const;
    /*! Set the default value of an existing boolean option 
        with identifier \a ident to \a dflt. */
  Parameter &setDefaultBoolean( const string &ident, bool dflt );

    /*! Add a date option at the end of the options list. 
        \param[in] ident the identifier string of the new option
        \param[in] request the request string of the new option
	\param[in] year the default value for the year
	\param[in] month the default value for the month
	\param[in] day the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertDate(), setDate(), date() */
  Parameter &addDate( const string &ident, const string &request,  
		      int year=0, int month=0, int day=0,
		      int flags=0, int style=0 );
    /*! Add a date option at the end of the options list. 
        \param[in] ident the identifier and request string of the new option
	\param[in] year the default value for the year
	\param[in] month the default value for the month
	\param[in] day the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertDate(), setDate(), date() */
  inline Parameter &addDate( const string &ident,
			     int year, int month, int day,
			     int flags=0, int style=0 )
    { return addDate( ident, ident, year, month, day, flags, style ); };
    /*! Add a date option at the end of the options list
        with year, month and day set to zero. 
        \param[in] ident the identifier and request string of the new option
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertDate(), setDate(), date() */
  inline Parameter &addDate( const string &ident, int flags=0, int style=0 )
    { return addDate( ident, ident, 0, 0, 0, flags, style ); };
    /*! Insert a new date option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        \param[in] ident the identifier string of the new option
        \param[in] atident the identifier string of the existing option
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
  Parameter &insertDate( const string &ident, const string &atident="", 
			 const string &request="",
			 int year=0, int month=0, int day=0,
			 int flags=0, int style=0 );
    /*! Insert a new date option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        \param[in] ident the identifier and request string of the new option
        \param[in] atident the identifier string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] year the default value for the year
	\param[in] month the default value for the month
	\param[in] day the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addDate(), setDate(), date() */
  Parameter &insertDate( const string &ident, const string &atident, 
			 int year=0, int month=0, int day=0,
			 int flags=0, int style=0 )
    { return insertDate( ident, atident, ident, year, month, day, flags, style ); };
    /*! Insert a new date option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        The year, month and day are set to zero. 
        \param[in] ident the identifier and request string of the new option
        \param[in] atident the identifier string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addDate(), setDate(), date() */
  Parameter &insertDate( const string &ident, const string &atident, 
			 int flags=0, int style=0 )
    { return insertDate( ident, atident, ident, 0, 0, 0, flags, style ); };
    /*! Get the date from a date option.
	\param[in] ident the identifier string of the option
	\param[in] index the index of the date
	\param[out] year the year of the specified date
	\param[out] month the month of the specified date
	\param[out] day the day of the specified date
	\return the reference of the option
        \sa defaultDate(), setDate(), addDate(), insertDate() */
  const Parameter &date( const string &ident, int index,
			 int &year, int &month, int &day ) const;
    /*! Set the value of an existing date option.
	\param[in] ident the identifier string of the option
	\param[in] year the new value for the year
	\param[in] month the new value for the month
	\param[in] day the new value for the day
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &ident, int year, int month, int day );
    /*! Set the value of an existing date option.
	\param[in] ident the identifier string of the option
	\param[in] date the new value for the date
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &ident, const string &date );
    /*! Set the value of an existing date option.
	\param[in] ident the identifier string of the option
	\param[in] date the new value for the date
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &ident, const struct tm &date );
    /*! Set the value of an existing date option.
	\param[in] ident the identifier string of the option
	\param[in] time the new value for the date
	\return a reference to the option.
        \sa setCurrentDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setDate( const string &ident, const time_t &time );
    /*! Set the value of an existing date option to the current date.
	\param[in] ident the identifier string of the option
	\return a reference to the option.
        \sa setDate(), setDefaultDate(), addDate(), insertDate(), date() */
  Parameter &setCurrentDate( const string &ident );
    /*! Set value of an existing date option 
        with identifier \a ident to the one of \a p,
        if both are of type() Date.
	If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setDate( const string &ident, const Parameter &p );
    /*! Get the default date from a date option.
	\param[in] ident the identifier string of the option
	\param[in] index the index of the default date
	\param[out] year the year of the specified default date
	\param[out] month the month of the specified default date
	\param[out] day the day of the specified default date
	\return the reference of the option
        \sa date(), setDate(), addDate(), insertDate() */
  const Parameter &defaultDate( const string &ident, int index,
				int &year, int &month, int &day ) const;
    /*! Set the default value of an existing date option.
	\param[in] ident the identifier string of the option
	\param[in] year the new value for the default year
	\param[in] month the new value for the default month
	\param[in] day the new value for the default day
	\return a reference to the option.
        \sa setDate(), addDate(), insertDate(), defaultDate() */
  Parameter &setDefaultDate( const string &ident,
			     int year, int month, int day );

    /*! Add a time option at the end of the options list. 
        \param[in] ident the identifier string of the new option
        \param[in] request the request string of the new option
	\param[in] hour the default value for the hour
	\param[in] minutes the default value for the minutes
	\param[in] seconds the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertTime(), setTime(), time() */
  Parameter &addTime( const string &ident, const string &request,  
		      int hour=0, int minutes=0, int seconds=0,
		      int flags=0, int style=0 );
    /*! Add a time option at the end of the options list. 
        \param[in] ident the identifier and request string of the new option
	\param[in] hour the default value for the hour
	\param[in] minutes the default value for the minutes
	\param[in] seconds the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertTime(), setTime(), time() */
  inline Parameter &addTime( const string &ident,
			     int hour, int minutes, int seconds,
			     int flags=0, int style=0 )
    { return addTime( ident, ident, hour, minutes, seconds, flags, style ); };
    /*! Add a time option at the end of the options list
        with hour, minutes and seconds set to zero. 
        \param[in] ident the identifier and request string of the new option
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa insertTime(), setTime(), time() */
  inline Parameter &addTime( const string &ident, int flags=0, int style=0 )
    { return addTime( ident, ident, 0, 0, 0, flags, style ); };
    /*! Insert a new time option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        \param[in] ident the identifier string of the new option
        \param[in] atident the identifier string of the existing option
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
  Parameter &insertTime( const string &ident, const string &atident="", 
			 const string &request="",
			 int hour=0, int minutes=0, int seconds=0,
			 int flags=0, int style=0 );
    /*! Insert a new time option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        \param[in] ident the identifier and request string of the new option
        \param[in] atident the identifier string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] hour the default value for the hour
	\param[in] minutes the default value for the minutes
	\param[in] seconds the default value for the dat
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addTime(), setTime(), time() */
  Parameter &insertTime( const string &ident, const string &atident, 
			 int hour=0, int minutes=0, int seconds=0,
			 int flags=0, int style=0 )
    { return insertTime( ident, atident, ident, hour, minutes, seconds, flags, style ); };
    /*! Insert a new time option at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list.
        The hour, minutes and seconds are set to zero. 
        \param[in] ident the identifier and request string of the new option
        \param[in] atident the identifier string of the existing option
	where the new options should be inserted. If left empty,
	the new option is inserted at the beginning of the option list.
	\param[in] flags some flags
	\param[in] style defines the style in a GUI.
	\return a reference to the new option
	\sa addTime(), setTime(), time() */
  Parameter &insertTime( const string &ident, const string &atident, 
			 int flags=0, int style=0 )
    { return insertTime( ident, atident, ident, 0, 0, 0, flags, style ); };
    /*! Get the time from a time option.
	\param[in] ident the identifier string of the option
	\param[in] index the index of the time
	\param[out] hour the hour of the specified time
	\param[out] minutes the minutes of the specified time
	\param[out] seconds the seconds of the specified time
	\return the reference of the option
        \sa defaultTime(), setTime(), addTime(), insertTime() */
  const Parameter &time( const string &ident, int index,
			 int &hour, int &minutes, int &seconds ) const;
    /*! Set the value of an existing time option.
	\param[in] ident the identifier string of the option
	\param[in] hour the new value for the hour
	\param[in] minutes the new value for the minutes
	\param[in] seconds the new value for the seconds
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &ident,
		      int hour, int minutes, int seconds );
    /*! Set the value of an existing time option.
	\param[in] ident the identifier string of the option
	\param[in] time the new value for the time
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &ident, const string &time );
    /*! Set the value of an existing time option.
	\param[in] ident the identifier string of the option
	\param[in] time the new value for the time
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &ident, const struct tm &time );
    /*! Set the value of an existing time option.
	\param[in] ident the identifier string of the option
	\param[in] time the new value for the time
	\return a reference to the option.
        \sa setCurrentTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setTime( const string &ident, const time_t &time );
    /*! Set the value of an existing time option to the current time.
	\param[in] ident the identifier string of the option
	\return a reference to the option.
        \sa setTime(), setDefaultTime(), addTime(), insertTime(), time() */
  Parameter &setCurrentTime( const string &ident );
    /*! Set value of an existing time option 
        with identifier \a ident to the one of \a p,
        if both are of type() Time.
	If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setTime( const string &ident, const Parameter &p );
    /*! Get the default time from a time option.
	\param[in] ident the identifier string of the option
	\param[in] index the index of the default time
	\param[out] hour the hour of the specified default time
	\param[out] minutes the minutes of the specified default time
	\param[out] seconds the seconds of the specified default time
	\return the reference of the option
        \sa time(), setTime(), addTime(), insertTime() */
  const Parameter &defaultTime( const string &ident, int index,
				int &hour, int &minutes, int &seconds ) const;
    /*! Set the default value of an existing time option.
	\param[in] ident the identifier string of the option
	\param[in] hour the new value for the default hour
	\param[in] minutes the new value for the default minutes
	\param[in] seconds the new value for the default seconds
	\return a reference to the option.
        \sa setTime(), addTime(), insertTime(), defaultTime() */
  Parameter &setDefaultTime( const string &ident,
			     int hour, int minutes, int seconds );

    /*! Add a label \a ident at the end of the options list. */
  Parameter &addLabel( const string &ident, int flags=0, int style=0 );
    /*! Insert a new label \a ident at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list. */
  Parameter &insertLabel( const string &ident, const string &atident="",
			  int flags=0, int style=0 );
    /*! Return the label of a label or separator
        with identifier equal to \a ident. 
        If there is no option with identifier \a ident
	an empty string is returned. */
  Str label( const string &ident ) const;
    /*! Set the lable of an existing label or separator
        with identifier \a ident to \a label. */
  Parameter &setLabel( const string &ident, const string &label );

    /*! Add a separator at the end of the options list. */
  Parameter &addSeparator( int flags=0, int style=0 ); 
    /*! Insert a new separator at the beginning of the options list
        (\a atident == "") or at the position of the option with
        identity \a atident. If the option with identity \a atident
        does not exist, the option is appended to the end of the list. */
  Parameter &insertSeparator( const string &atident="", int flags=0, int style=0 );

    /*! Set value of option with identifier equal to \a ident
        to its default. */
  Parameter &setDefault( const string &ident );
    /*! Set values of all options to their default. */
  Options &setDefaults( int flags=0 );

    /*! Set default of option with identifier equal to \a ident
        to its value. */
  Parameter &setToDefault( const string &ident );
    /*! Set defaults of all options to their values. */
  Options &setToDefaults( int flags=0 );

    /*! Remove all except of the first Parameter of Parameters with
        identical identifier. 
        If an identifier \a ident is specified,
        only options with this identifier are processed. */
  Options &takeFirst( const string &ident="" );
    /*! Remove all except of the last Parameter of Parameters with
        identical identifier. 
        If an identifier \a ident is specified,
        only options with this identifier are processed. */
  Options &takeLast( const string &ident="" );
    /*! Combine values of Text-Parameters with identical identifier
        by adding the text values to the first one and
	deleting the following.
        If an identifier \a ident is specified,
        only options with this identifier are processed. */
  Options &combineFirst( const string &ident="" );
    /*! Combine values of Text-Parameters with identical identifier
        by adding the text values to the last one and
	deleting the following.
	The combined values are separated by \a separator.
        If an identifier \a ident is specified,
        only options with this identifier are processed. */
  Options &combineLast( const string &ident="" );

    /*! Remove the option where a p points to. */
  Options &erase( iterator p );
    /*! Remove all options whose identifier match \a ident from options list. */
  Options &erase( const string &ident );
    /*! Remove all options whose flag matches \a selectflag from options list. */
  Options &erase( int selectflag );
    /*! Remove last option. */
  Options &pop( void );
    /*! Remove all options. */
  Options &clear( void );
    /*! Remove all options without value, i.e. Labels, Separators, GroupBoxes. */
  Options &strip( void );

    /*! Total number of options. */
  int size( void ) const;
    /*! Total number of options that have \a selectflag set in their flags().
        If \a selectflag equals zero, all options are counted.
	If \a selectflag is negative, only options whose values differ
	from the default value and have abs(\a selectflag) set in their flags
	are counted.
        If \a selectflag equals NonDefault, all options whose values differ
	from their default value are counted. */
  int size( int selectflag ) const;
    /*! True if there are no options. */
  bool empty( void ) const;
    /*! True if option with identifier \a ident exist. */
  bool exist( const string &ident ) const;

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
        (see Parameter::types() ) to \a flags. */
  Options &setTypeFlags( int flags, int typemask );
    /*! Add \a flags to the flags of all options
        whose type matches \a typemask (see Parameter::types() ). */
  Options &addTypeFlags( int flags, int typemask );
    /*! Clear the bits specified by \a flags of the flags of all options
        whose type matches \a typemask (see Parameter::types() ). */
  Options &delTypeFlags( int flags, int typemask );

    /*! Set style of all options to \a style. */
  Options &setStyle( int style, int selectflag=0 );
    /*! Add the bits specified by \a style to the style of all options. */
  Options &addStyle( int style, int selectflag=0 );
    /*! Clear the bits specified by \a style of the style of all options. */
  Options &delStyle( int style, int selectflag=0 );
    /*! Total number of options that have the style \a style set in their style(). */
  int styleSize( int style ) const;

    /*! Set style of all options whose type matches \a typemask 
        (see Parameter::types() ) to \a style. */
  Options &setTypeStyle( int style, int typemask );
    /*! Add the bits specified by \a style to the style of all options
        whose type matches \a typemask (see Parameter::types() ). */
  Options &addTypeStyle( int style, int typemask );
    /*! Clear the bits specified by \a style of the style of all options
        whose type matches \a typemask (see Parameter::types() ). */
  Options &delTypeStyle( int style, int typemask );

    /*! Length of largest identifier. */
  int identWidth( int selectmask=0 ) const;

    /*! Write identifiers and their values to a string
        separated by \a separator.
	Saves only options that have \a selectmask set in their flags().
        If \a selectmask equals zero, all options are saved.
	If \a selectmask is negative, only options whose values differ
	from the default value and have abs(\a selectmask) set in their flags
	are saved.
        If \a selectmask equals NonDefault, all options whose values differ
	from their default value are saved. */
  string save( string separator="; ", 
	       int selectmask=0, bool firstonly=false ) const;
    /*! Write identifiers and their values to stream \a str.
        Start each line with \a start.
	Saves only options that have \a selectmask set in their flags().
        If \a selectmask equals zero, all options are saved.
	If \a selectmask is negative, only options whose values differ
	from the default value and have abs(\a selectmask) set in their flags
	are saved.
        If \a selectmask equals NonDefault, all options whose values differ
	from their default value are saved. */
  ostream &save( ostream &str, const string &start="",
		 int width=-1, int selectmask=0, bool detailed=false,
		 bool firstonly=false ) const;
    /*! Write options to stream \a str und use \a textformat,
        \a numberformat, \a boolformat, \a dateformat, 
	\a timeformat, \a labelformat, and \a separatorformat
	for formatting text, number, boolean, label, and separator parameter,
	respectively. */
  ostream &save( ostream &str, const string &textformat,
		 const string &numberformat, const string &boolformat="%i=%b\n",
		 const string &dateformat="%04Y-%02m-%02d", const string &timeformat="%02H:%02M:%02S",
		 const string &labelformat="%i\n", const string &separatorformat="",
		 int selectmask=0 ) const;
    /*! Write identifiers and their values to stream \a str */
  friend ostream &operator<< ( ostream &str, const Options &o );

    /*! Write options in XML format to output stream.
        \param[in] str the output stream
        \param[in] selectmask selects options that have \a selectmask set in their flags().
                   See Parameter::flags(int) for details.
        \param[in] level the level of indentation
        \param[in] indent the indentation depth, 
                   i.e. number of white space characters per level
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int selectmask=0, int level=0,
		    int indent=2 ) const;

    /*! Read options from string \a opttxt of the form 
        "parameter1=x; parameter2=y; parameter3=z; ...".
        If the parameters 'parameter1', 'parameter2', ... match the
        identifier of an option, its value is set to x, y, z, ... respectively,
	and \a flag is added to its flags. */
  Options &read( const string &opttxt, int flag,
		 const string &assignment=":=", const string &separator=";",
		 string *pattern=0 );
  Options &read( const string &opttxt, const string &assignment=":=",
		 const string &separator=";", string *pattern=0 )
    { return read( opttxt, 0, assignment, separator, pattern ); };
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
		 const string &comment="", 
		 const string &stop="", Str *line=0 );
  istream &read( istream &str, const string &assignment=":=",
		 const string &comment="", 
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
        If no option with the identifier equal to \a p's identifier
        exist \a p is appended to the options.
	If \a appendseparator is true,
	separators with empty identifier are appended.
        Returns \c true if \a p was read, \c false if it was appended. */
  bool readAppend( const Parameter &p, bool appendseparator=true );
    /*! Read options from the options \a o.
        All options \a o that do not exist are appended.
	If \a appendseparator is true,
	separators with empty identifier are appended. */
  Options &readAppend( const Options &o, int flags=0, bool appendseparator=true );
    /*! Read options from the list of strings \a sq.
        All options from \a sq that do not exist are appended.
	If \a appendseparator is true,
	separators with empty identifier are appended. */
  Options &readAppend( const StrQueue &sq, bool appendseparator=true,
		       const string &assignment=":=" );


    /*! Load options from string \a opttxt of the form 
        "parameter1=x; parameter2=y; parameter3=z; ...".
        New options are created with the identifiers and request strings
	set to parameter1, parameter2, parameter3, ... 
	and their values set to x, y, z, ..., respectively. */
  Options &load( const Str &opttxt, const string &assignment=":=",
		 const string &separator=";" );
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
		 const string &comment="", 
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
    /*! The options. */
  deque< Parameter > Opt;
    /*! Sections of options. */
  deque< Options > Secs;
    /*! A warning message. */
  mutable Str Warning;
    /*! Avoid recursive call of notify(). */
  bool Notified;
    /*! Enables calling the notify() function. */
  bool CallNotify;

    /*! Dummy Parameter for index operator. */
  static Parameter Dummy;

};


}; /* namespace relacs */

#endif /* ! _RELACS_OPTIONS_H_ */
