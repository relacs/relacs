/*
  parameter.h
  A Parameter has a name, value and unit.

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

#ifndef _RELACS_PARAMETER_H_
#define _RELACS_PARAMETER_H_ 1


#include <limits.h>
#include <values.h>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
#include <deque>
#include <relacs/str.h>
#include <relacs/strqueue.h>
using namespace std;

namespace relacs {


class Options;


/*! 
\class Parameter
\author Jan Benda
\brief A Parameter has a name, value and unit.

Parameter basically are name-value pairs.  The parameter is uniquely
identified by its name().  The value can be of different types (number(),
boolean(), date(), time(), text(), etc., see setValueType() ).  Number
parameters have in addition a unit() and a format() string, which is used
to generate pretty formatted output.  The value can be changed by one
of the setText(), setNumber(), etc. functions or set to a default
value.
*/

class Parameter 
{
public:
  
    /*! Type of the parameter's value. */
  enum ValueType {
      /*! No type specified yet. */
    NoType=0,
      /*! The parameter's value is a string. */
    Text=1,
      /*! The parameter's value is a floating point number. */
    Number=2,
      /*! The parameter's value is an integer. */
    Integer=4,
      /*! The parameter's value is a boolean. */
    Boolean=8,
      /*! The parameter's value is a floating point number,
	  an integer, or a boolean. */
    AnyNumber=14, 
      /*! The parameter's value is a date that contains year, month, and day. */
    Date=16,
      /*! The parameter's value is a time that contains hour, minutes, and seconds. */
    Time=32,
      /*! A section.
	  \note A Parameter cannot be a section.
          This type is for the \a typemask of the Options::setValueTypeFlags()
          functions. */
    Section=64,
  };

    /*! This flag is set whenever the value of the Parameter is changed. */
  static const int ChangedFlag = 16384;
    /*! Use this flag to select Parameter whose value differs
        from their default value. */
  static const int NonDefault = -32768;

    /*! If this bit is set in the Parameter's style
        you can only select from text options with multiple values,
        but not add new values. */
  static const long SelectText = 0x01000000;
    /*! If this bit is set in the Parameter's style
        all values of the parameter get saved
        even if only the first value is requested to be saved. */
  static const int ListAlways = 0x02000000;

    /*! Construct a single Parameter. 
        Use setValueType() to define the type of the parameter,
	setRequest() to define the request string, which is used
	for dialogs, setName() to give the parameter a unique name.
        With the set*() functions, the values of the parameter
        can be initialized. */
  Parameter( void );
    /*! Copy constructor. */
  Parameter( const Parameter &p );
    /*! Construct and initialize a single Parameter of type Text.
        Its value an its default value are set to \a strg. */
  Parameter( const string &name, const string &request,
	     const string &strg, int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of type Text.
        Its value an its default value are set to \a strg. */
  Parameter( const string &name, const string &request,
	     const char *strg, int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of type Number. 
        Its value and its default value are set to \a number,
	its standard deviation to \a error.
        The allowed range for its value is defined by
	\a minimum and \a maximum. 
        The unit of the number is \a interunit. 
	For output and dialogs \a outputunit is used as unit. */
  Parameter( const string &name, const string &request,  
	     double number, double error,
	     double minimum=-MAXDOUBLE, double maximum=MAXDOUBLE, double step=1.0,
	     const string &internunit="", const string &outputunit="", 
	     const string &format="", int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of type Number. 
        Its value and its default value are set to \a number,
	its standard deviation to \a 0.0.
        The unit of the number is \a unit. */
  Parameter( const string &name, const string &request,  
	     double number, const string &unit="", 
	     const string &format="", int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of type Number. 
        Its values are set to \a numbers,
        its default value is set to \a numbers[0],
	its standard deviation to \a error.
        The allowed range for its value is defined by
	\a minimum and \a maximum. 
        The unit of the number is \a interunit. 
	For output and dialogs \a outputunit is used as unit. */
  Parameter( const string &name, const string &request,  
	     const vector<double> &numbers,
	     const vector<double> &errors, 
	     double minimum=-MAXDOUBLE, double maximum=MAXDOUBLE, double step=1.0,
	     const string &internunit="", const string &outputunit="", 
	     const string &format="", int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of type Integer.
        Its value an its default value are set to \a number,
	its standard deviation to \a error.
        The allowed range for its value is defined by
        \a minimum and \a maximum. 
        The unit of the number is \a interunit. 
	For output and dialogs \a outputunit is used as unit. */
  Parameter( const string &name, const string &request, 
	     long number, long error=-1, 
	     long minimum=LONG_MIN, long maximum=LONG_MAX, long step=1,
	     const string &internunit="", const string &outputunit="", 
	     int width=0, int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of type Boolean.
        Its value and its default value are set to \a dflt. */
  Parameter( const string &name, const string &request,
	     bool dflt, int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of \a type Date or Time.
        Its value and its default value are set to 
	\a yearhour, \a monthminutes and \a dayseconds. */
  Parameter( const string &name, const string &request, ValueType type,
	     int yearhour, int monthminutes, int dayseconds,
	     int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Construct and initialize a single Parameter of type Time.
        Its value and its default value are set to 
	\a hour, \a minutes, \a seconds, and \a milliseconds. */
  Parameter( const string &name, const string &request,
	     int hour, int minutes, int seconds, int milliseconds,
	     int flags=0, int style=0,
	     Options *parentsection=0 );
    /*! Create parameter with name \a name and value \a value.
        The value type is set appropriately. \sa asign() */
  Parameter( const string &name, const string &value );
    /*! Deconstruct Parameter. */
  ~Parameter( void );

    /*! Clear the parameter.
        Set the identity string to \a name, the request string to \a request,
	and the type to \a type.
	The value of the parameter and the default value is cleared.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &clear( const string &name="", const string &request="",
		    ValueType type=NoType );

    /*! Copy content of parameter \a p to this.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &operator=( const Parameter &p );
    /*! Copy content of parameter \a p to this.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &assign( const Parameter &p );

    /*! Set the value of a parameter depending on its type to
        the content of the string \a value.
	If the type of the parameter is Number, Integer, or Boolean,
	\a value is first splitted into a number and a unit.
	If the parameter has no type, it is set to Number
	if \a value contains a floating point number or a unit,
	Integer if \a value is an integer number,
        Boolean if \a value equals either "true" or "false",
        otherwise it is set to Text.
        The warning message is set if \a value is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &operator=( const string &value );
    /*! Set the value of a parameter depending on its type to
        the content of the string \a value.
	If the type of the parameter is Number, Integer, or Boolean,
	\a value is first splitted into a number and a unit.
	If the parameter has no type, it is set to Number
	if \a value contains a floating point number or a unit,
	Integer if \a value is an integer number,
        Boolean if \a value equals either "true" or "false",
        otherwise it is set to Text.
        The warning message is set if \a value is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &assign( const string &value );

    /*! Returns true if the two Parameters \a p1 and \a p2 are equal,
        i.e. they have the same name. */
  friend bool operator==( const Parameter &p1, const Parameter &p2 );
    /*! Returns true if the Parameter \a p has an name 
        equal to \a name. */
  friend bool operator==( const Parameter &p, const string &name );

    /*! Returns a pointer to the Options where this Parameter belongs to.
        If this Parameter does not belong to an Options, NULL is returned. */
  Options *parentSection( void );
    /*! Returns a const pointer to the Options where this Parameter belongs to.
        If this Parameter does not belong to an Options, NULL is returned. */
  const Options *parentSection( void ) const;
    /*! Set the parent Options of this Parameter to \a parent. */
  void setParentSection( Options *parentsection );

    /*! Returns \c true if the value differs from the default. */
  bool nonDefault( void ) const;

    /*! Returns the warning messages of the last called 
        Parameter member-function. */
  Str warning( void ) const;

    /*! Returns the identity string. */
  Str name( void ) const;
    /*! Set identity string to \a name. */
  Parameter &setName( const string &name );

    /*! Returns the request string. */
  Str request( void ) const;
    /*! Set request string to \a request. */
  Parameter &setRequest( const string &request );

    /*! The type of the parameter. */
  ValueType valueType( void ) const;
    /*! \return \c true if one of the bits specified by \a mask corresponds
        to the parameter's type, or \a mask equals zero,
	or \a mask is negative and the parameter's type
	is not contained in abs( mask ). */
  bool valueType( int mask ) const;
    /*! Set type of parameter to \a typex. */
  Parameter &setValueType( ValueType type );

    /*! The flags of the parameter. */
  int flags( void ) const;
    /*! True if one of the bits specified by \a selectflag is set
        in the parameter's flags, or \a selectflag equals zero,
	or \a selectflag is negative and the parameter's value differs
	from the default value and abs(\a selectflag)
	is set in the parameter's flags,
	or \a selectflag equals NonDefault and the parameter's values differs
	from the default value. */
  bool flags( int selectflag ) const;
    /*! Set flags of parameter to \a flags. */
  Parameter &setFlags( int flags );
    /*! Add the bits specified by \a flags to the parameter's flags. */
  Parameter &addFlags( int flags );
    /*! Clear the bits specified by \a flags of the parameter's flags. */
  Parameter &delFlags( int flags );
    /*! Clear all bits of the parameter's flags. */
  Parameter &clearFlags( void );
    /*! The flag that is used to mark options whose value has been changed.
        It is preset to a constant value (16384) and
	cannot be changed by the user. */
  static int changedFlag( void );
    /*! Return \c true if the changedFlag() is set,
        i.e. whose value has been changed. */
  bool changed( void ) const;

    /*! The style of the parameter. */
  int style( void ) const;
    /*! Set style of parameter to \a style. */
  Parameter &setStyle( int style );
    /*! Add the bits specified by \a style to the parameter's style. */
  Parameter &addStyle( int style );
    /*! Clear the bits specified by \a style of the parameter's style. */
  Parameter &delStyle( int style );

    /*! Returns the format string. */
  Str format( void ) const;
    /*! Set format string for numbers to have a width of \a width and 
        precision of \a prec.
	The width \a width can be negative to indicate left aligned output.
	The type is set to \a fmt which can be either 'f',
	'g' or 'e', according to the ANSI C printf double formats
	or 's' for text.
	The default is 'g' for numbers and 's' for strings.
        The warning message is set if \a w, \a prec or \a fmt are invalid.
        Does nothing if the parameter is a date or time. */
  Parameter &setFormat( int width, int prec=-1, char fmt='-' );
    /*! Set format string to \a format
        (ANSI C printf() syntax, only 'g', 'f', 'e' formats are allowed, 
        since the number is a double).
	For example, "%5.1f" is an valid format string.
        The warning message is set if \a format is invalid.
        See text() for an detailed description of formats. */
  Parameter &setFormat( const string &format="" );
    /*! Returns the width of the expanded format string. */
  int formatWidth( void ) const;

    /*! If several values correspond to this parameter
        size() returns the number of values. */
  int size( void ) const;

    /*! True if parameter is of type text. */
  bool isText( void ) const;
    /*! Returns the \a index-th value of a parameter.
        It is formatted as specified by the \a format string.
        %s is the text string.
	If the text string is a file path (e.g. example/data.tar.gz), then
	%p is the path of the file (e.g. example/),
        %d is the full file name (e.g. data.tar.gz),
        %n is the basename of the file name (e.g. data), and
	%x is the extension (e.g. tar.gz).
	If the parameter is a number (float, integer, boolean), then
        %f, %g, %e is the number value,
        %F, %G, %E is the number error, and
        %b is a boolean value expressed as true or false.
	If the parameter is a time, then
        %S: second (0..59),
        %M: minute (0..59),
        %H: hour (0..23).
	If the parameter is a date, then
	%d: day of month (1..31),
	%m: month (1..12),
	%y: last two digits of year (0..99),
	%Y: year (1970...).
        %u is the unit string,
        %i is the name (identifier) string, and
        %r is the request string.
        %T is the type of the parameter as a human readable string.
 	If \a format is empty, the format set by setFormat() is used.
        If the parameter is a number, then its value is returned
        in the unit specified by \a unit.
        If \a unit is empty, the outUnit() is used. */ 
  Str text( int index, const string &format="", const string &unit="" ) const;
    /*! Returns the first value of a parameter.
        It is formatted as specified by the \a format string.
	See text() for more details.
        If the parameter is a number, then its value is returned
        in the unit specified by \a unit.
        If \a unit is empty, then outUnit() is used. */ 
  Str text( const string &format="", const string &unit="" ) const;
    /*! Return in \a s all values of the parameter as a list of strings
        formatted according to \a format using \a unit.
        See text() for details. */
  void texts( vector<string> &s, const string &format="", const string &unit="" ) const;
    /*! Return all values of the parameter formatted according to \a format
        using \a unit, and concatenated using \a separator.
        See text() for details. */
  Str allText( const string &format="", const string &unit="", const string &separator=", " ) const;
    /*! Set value of text parameter to \a strg.
        The text is splitted into several strings at each '|'.
        Each of the strings is also converted into numbers.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setText( const string &strg );
    /*! Set value of text parameter to the one of \a p,
        if \a this and \a p are both of valueType() Text.
	If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setText( const Parameter &p );
    /*! Add \a strg to values of text parameter.
        \a strg is also converted into a number.
        If \a clear is true then the current value of the parameter is
        erased prior to adding \a strg.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &addText( const string &strg, bool clear=false );
    /*! Returns the default text value.
        It is formatted as specified by the \a format string.
	See text() for details. */ 
  Str defaultText( int index, const string &format="", const string &unit="" ) const;
  Str defaultText( const string &format="", const string &unit="" ) const;
    /*! Set default value of text parameter to \a strg.
        The text is splitted into several strings at each '|'.
        Each of the strings is also converted into numbers. */
  Parameter &setDefaultText( const string &strg );
    /*! Add \a strg to default values of text parameter.
        \a strg is also converted into a number. */
  Parameter &addDefaultText( const string &strg );
    /*! If the text parameter has several values,
        then the value that is matched by \a strg is
        inserted as the first value.
        If \a strg does not match any of the text values
        \a strg is added to the text values and is marked as selected
	if \a add > 0, or if \a add == 0 and the SelectText - Bit
	in the Parameter's style is not set.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &selectText( const string &strg, int add=0 );
    /*! If the text parameter has several values,
        then the \a index-th value is selected by inserting it as the first value. */
  Parameter &selectText( int index );
    /*! Returns the index of the selected value, i.e. 
        the index minus one of the matching value with the first value. */
  int index( void ) const;
    /*! Returns the index of the value that matches \a strg. */
  int index( const string &strg ) const;

    /*! True if the parameter is of type Number, Integer, or Boolean.
        \sa isNumber(), isInteger(), isBoolean(). */
  bool isAnyNumber( void ) const;
  
    /*! True if parameter is of type Number,
        i.e. its value is a floating-point number. */
  bool isNumber( void ) const;
  
    /*! Returns the \a index-th number value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
        If the Parameter is not a number, \a dflt is returned. */
  double number( const string &unit="", int index=0, double dflt=0.0 ) const;
    /*! Returns the \a index-th number value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
        If the Parameter is not a number, \a dflt is returned. */
  inline double number( int index, const string &unit="", double dflt=0.0 ) const
  { return number( unit, index, dflt ); };
    /*! Returns the \a index-th number value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
        If the Parameter is not a number, \a dflt is returned. */
  inline double number( int index, double dflt, const string &unit="" ) const
  { return number( unit, index, dflt ); };
    /*! Return in \a n all number-values of the parameter in the unit \a unit. */
  void numbers( vector<double> &n, const string &unit="" ) const;
    /*! Returns the standard deviation value of the \a index-th number in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
	If no error value is set -1 is returned. */
  double error( const string &unit="", int index=0 ) const;
    /*! Returns the standard deviation value of the \a index-th number in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
	If no error value is set -1 is returned. */
  inline double error( int index, const string &unit="" ) const
    { return error( unit, index ); };
    /*! Return in \a n all error-values of the parameter in the unit \a unit. */
  void errors( vector<double> &n, const string &unit="" ) const;
    /*! Set number value to \a number and its
        standard deviation to \a error (only if it is non negative). 
        The text value is set according to the specified format.
        The warning message is set if \a number is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setNumber( double number, double error, const string &unit="" );
    /*! Set number value to \a number. 
        The text value is set according to the specified format.
        The warning message is set if \a number is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  inline Parameter &setNumber( double number, const string &unit="" )
    { return setNumber( number, -1.0, unit ); };
    /*! Set values to \a numbers and their
        standard deviations to \a errors. 
        The text values are set according to the specified format. 
        The warning message is set if \a numbers is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setNumbers( const vector<double> &numbers,
			 const vector<double> &errors, 
			 const string &unit="" );
    /*! Set values to \a numbers and their
        standard deviation to \a error. 
        The text values are set according to the specified format. 
        The warning message is set if \a numbers is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  inline Parameter &setNumbers( const vector<double> &numbers,  double error,
				const string &unit="" )
    { return setNumbers( numbers, vector<double>( numbers.size(), error ), unit ); };
    /*! Set values to \a numbers and their
        standard deviation to -1.0. 
        The text values are set according to the specified format. 
        The warning message is set if \a numbers is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  inline Parameter &setNumbers( const vector<double> &numbers,
				const string &unit="" )
    { return setNumbers( numbers, -1.0, unit ); };
    /*! Set number value, error, and unit to the ones of \a p,
        if \a this and \a p are both of valueType() Number.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setNumber( const Parameter &p );
    /*! Add \a number to the number values and their
        standard deviation to \a error (only if it is non negative).
        A text value is added according to the specified format.
        The warning message is set if \a number is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set.
        \a clear clears all values before adding the number
        (for internal use) */
  Parameter &addNumber( double number, double error, const string &unit="",
			bool clear=false );
    /*! Add \a number to the number values.
        A text value is added according to the specified format.
        The warning message is set if \a number is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  inline Parameter &addNumber( double number, const string &unit="" )
    { return addNumber( number, -1.0, unit ); };
    /*! Interpret \a s as a number and add it to the number values.
        The warning message is set if \a s is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &addNumber( const Str &s, const string &unit="" );
  
    /*! True if parameter is of type Integer,
        i.e. its value is a integer number. */
  bool isInteger( void ) const;
    /*! Returns the \a index-th integer value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
        If the Parameter is not a number, \a dflt is returned. */
  long integer( const string &unit="", int index=0, long dflt=0 ) const;
    /*! Returns the \a index-th integer value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
        If the Parameter is not a number, \a dflt is returned. */
  inline long integer( int index, const string &unit="", long dflt=0 ) const
  { return integer( unit, index, dflt ); };
    /*! Returns the \a index-th integer value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string.
        If the Parameter is not a number, \a dflt is returned. */
  inline long integer( int index, long dflt, const string &unit="" ) const
  { return integer( unit, index, dflt ); };
    /*! Return in \a n all integer-values of the parameter in the unit \a unit. */
  void integers( vector<long> &n, const string &unit="" ) const;
    /*! Return in \a n all integer-values of the parameter in the unit \a unit. */
  void integers( vector<int> &n, const string &unit="" ) const;
    /*! Set integer value to \a number and its
        standard deviation to \a error. 
        The warning message is set if \a number is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setInteger( long number, long error, const string &unit="" );
    /*! Set integer value to \a number. 
        The warning message is set if \a number is invalid.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  inline Parameter &setInteger( long number, const string &unit="" )
    { return setInteger( number, -1, unit ); };
    /*! Set integer value, error, and unit to the ones of \a p,
        if \a this and \a p are both of valueType() Integer.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setInteger( const Parameter &p );
    /*! Add \a number and \a error to the integer values.
        The warning message is set if \a number is invalid. */
  Parameter &addInteger( long number, long error, const string &unit="" );
    /*! Add \a number to the integer values.
        The warning message is set if \a number is invalid. */
  inline Parameter &addInteger( long number, const string &unit="" )
    { return addInteger( number, -1, unit ); };
  
    /*! Returns the default number value in
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string. */
  double defaultNumber( const string &unit="", int index=0 ) const;
  inline double defaultNumber( int index, const string &unit="" ) const
    { return defaultNumber( unit, index ); };
    /*! Set default number value to \a number. 
        The default text value is set according to the specified format.
        The warning message is set if \a number is invalid. */
  Parameter &setDefaultNumber( double number, const string &unit="" );
    /*! Add \a number to the default number values.
        A text value is added according to the specified format.
        The warning message is set if \a number is invalid. */
  Parameter &addDefaultNumber( double number, const string &unit="" );
    /*! Interpret \a s as a number and add it to the default number values.
        The warning message is set if \a s is invalid. */
  Parameter &addDefaultNumber( const Str &s, const string &unit="" );

    /*! Returns the default ineger value in
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string. */
  long defaultInteger( const string &unit="", int index=0 ) const;
  inline long defaultInteger( int index,  const string &unit="" ) const
    { return defaultInteger( unit, index ); };
    /*! Set default integer value to \a dflt.
        The default text value is also set.
        The warning message is set if \a dflt is invalid. */
  Parameter &setDefaultInteger( long dflt, const string &unit="" );
    /*! Add \a number to the default integer values.
        A text value is added according to the specified format.
        The warning message is set if \a number is invalid. */
  Parameter &addDefaultInteger( double number, const string &unit="" );
  
    /*! Returns the minimum value allowed for number 
        or integer parameter values in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string. */
  double minimum( const string &unit="" ) const;
    /*! Returns the maximum value allowed for number 
        or integer parameter values in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string. */
  double maximum( const string &unit="" ) const;
    /*! Returns the step size of number 
        or integer parameters in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string. */
  double step( const string &unit="" ) const;
    /* Set the step size to \a step of unit \a unit.
       If \a step is negative, the step size is set to \a maximum - \minimum
       devided by \a step.
       If \a step equals zero, the step size is set to 1/50 of
       \a maximum - \a minimum.
       If \a unit is empty, \a step is assumed to be given in the internal unit. */
  Parameter &setStep( double step, const string &unit="" );
    /* Set the step size to \a step.
       If \a step is negative, the step size is set to \a maximum - \minimum
       devided by \a step.
       If \a step equals zero, the step size is set to 1/50 of
       \a maximum - \a minimum. */
  Parameter &setStep( long step );
    /*! Set minimum and maximum values of a number parameter
        to \a minimum and \a maximum, respectively.
        The warning message is set if \a minimum or \a maximum is invalid. */
  Parameter &setMinMax( double minimum=-MAXDOUBLE, double maximum=MAXDOUBLE,
			double step=1.0, const string &unit="" );
    /*! Set minimum and maximum values of an integer parameter 
        to \a minimum and \a maximum, respectively.
        The warning message is set if \a minimum or \a maximum is invalid. */
  Parameter &setMinMax( long minimum=LONG_MIN, long maximum=LONG_MAX, long step=1,
			const string &unit="" );
    /*! Set minimum and maximum values of an integer parameter 
        to \a minimum and \a maximum, respectively.
        The warning message is set if \a minimum or \a maximum is invalid. */
  Parameter &setMinMax( int minimum, int maximum, int step=1,
			const string &unit="" )
    { return setMinMax( (long)minimum, long(maximum), (long)step, unit ); };
    /*! Returns the largest power of ten smaller or equal to \a v. */
  static double floorLog10( double v );
    /*! Returns the next smaller multiple of floorLog10( \a v ).
        This is flooring to the leading digit.
        For example: 23.7 -> 20, 1.45 -> 1.
        \a scale determines to which digit the number should be floored.
        For example with \a scale = 0.1 you get 23.7 -> 23, 1.45 -> 1.4 */
  static double floor10( double v, double scale=1.0 );
    /*! Returns the next larger multiple of floorLog10( \a v ).
        This is ceiling to the leading digit.
        For example: 23.7 -> 30, 1.45 -> 2.
        \a scale determines to which digit the number should be ceiled.
        For example with \a scale = 0.1 you get 23.7 -> 24, 1.45 -> 1.5 */
  static double ceil10( double v, double scale=1.0 );

    /*! Returns the internal unit of the numbers. */
  Str unit( void ) const;
    /*! Returns the unit to be used for output. */
  Str outUnit( void ) const;
    /*! Set unit of numbers to \a internunit.
        The unit used for output and dialogs is set to \a outputunit.
	If \a outputunit is empty it is set to \a internunit. */
  Parameter &setUnit( const string &internunit, 
		      const string &outputunit="" );
    /*! Set the unit used for output and dialogs to \a outputunit. */
  Parameter &setOutUnit( const string &outputunit );
    /*! Set the unit of numbers to \a internunit
        and convert the value of the parameter accordingly. */
  Parameter &changeUnit( string internunit );
  
    /*! True if parameter is of type boolean. */
  bool isBoolean( void ) const;
    /*! Returns the \a index-th boolean value. */
  bool boolean( int index=0 ) const;
    /*! Return in \a n all boolean-values of the parameter. */
  void booleans( vector<bool> &n ) const;
    /*! Set value of boolean parameter to \a b.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setBoolean( bool b );
    /*! Set boolean value to the one of \a p,
        if \a this and \a p are both of valueType() Boolean.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setBoolean( const Parameter &p );
    /*! Returns the \a index-th default boolean value. */
  bool defaultBoolean( int index=0 ) const;
    /*! Set default boolean value to \a dflt. */
  Parameter &setDefaultBoolean( bool dflt );

    /*! True if parameter is of type date. */
  bool isDate( void ) const;
    /*! \return the \a index-th year. \sa month(), day(), text() */ 
  int year( int index=0 ) const;
    /*! \return the \a index-th month. \sa year(), day(), text() */ 
  int month( int index=0 ) const;
    /*! \return the \a index-th day of the month.
        \sa year(), month(), text() */ 
  int day( int index=0 ) const;
    /*! Returns the \a index-th date.
        \param[out] year If Parameter is not a Date or \a index is invalid, 0 is returned. 
        \param[out] month If Parameter is not a Date or \a index is invalid, 0 is returned. 
        \param[out] day If Parameter is not a Date or \a index is invalid, 0 is returned. 
        \param[in] index determines which one of the size() stored
	date values is returned in \a year, \a month, and \a day.
	\sa year(), month(), day(), setDate() */
  void date( int &year, int &month, int &day, int index=0 ) const;
    /*! Set date of date parameter to \a year, \a month, and \a day.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setDate( int year, int month, int day );
    /*! Add date \a year, \a month, and \a day to date parameter.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &addDate( int year, int month, int day );
    /*! Set date of date parameter to \a date.
        See Str::date() for valid date strings.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setDate( const string &date );
    /*! Set date of date parameter to \a date.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setDate( const struct tm &date );
    /*! Set date of date parameter to the date of \a time.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setDate( const time_t &time );
    /*! Set date of date parameter to the current date.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setCurrentDate( void );
    /*! Set date value to the one of \a p,
        if \a this and \a p are both of valueType() Date.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setDate( const Parameter &p );
    /*! \return the \a index-th default year. 
        \sa defaultMonth(), defaultDay(), defaultText() */ 
  int defaultYear( int index=0 ) const;
    /*! \return the \a index-th default month. 
        \sa defaultYear(), defaultDay(), defaultText() */ 
  int defaultMonth( int index=0 ) const;
    /*! \return the \a index-th default day of the month. 
        \sa defaultYear(), defaultMonth(), defaultText() */ 
  int defaultDay( int index=0 ) const;
    /*! Returns the \a index-th default date.
        \param[out] year If Parameter is not a Date or \a index is invalid, 0 is returned. 
        \param[out] month If Parameter is not a Date or \a index is invalid, 0 is returned. 
        \param[out] day If Parameter is not a Date or \a index is invalid, 0 is returned. 
        \param[in] index determines which one of the size() stored
	default date values is returned in \a year, \a month, and \a day.
	\sa defaultYear(), defaultMonth(), defaultDay(), setDefaultDate() */
  void defaultDate( int &year, int &month, int &day, int index=0 ) const;
    /*! Set default date of date parameter to \a year, \a month, and \a day. */
  Parameter &setDefaultDate( int year, int month, int day );
    /*! Add \a year, \a month, and \a day to default date of date parameter. */
  Parameter &addDefaultDate( int year, int month, int day );
    /*! Set default date of date parameter to \a date.
        See Str::date() for valid date strings. */
  Parameter &setDefaultDate( const string &date );

    /*! True if parameter is of type time. */
  bool isTime( void ) const;
    /*! \return the \a index-th hour. \sa minutes(), seconds(), milliSeconds(), text() */ 
  int hour( int index=0 ) const;
    /*! \return the \a index-th minutes. \sa hour(), seconds(), milliSeconds(), text() */ 
  int minutes( int index=0 ) const;
    /*! \return the \a index-th seconds. \sa hour(), minutes(), milliSeconds(), text() */ 
  int seconds( int index=0 ) const;
    /*! \return the \a index-th milliseconds. \sa hour(), minutes(), seconds(), text() */ 
  int milliSeconds( int index=0 ) const;
    /*! Returns the \a index-th time.
        \param[out] hour If Parameter is not a Time or \a index is invalid, 0 is returned.
        \param[out] minutes If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[out] seconds If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[out] milliseconds If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[in] index determines which one of the size() stored
	time values is returned in \a hour, \a minutes, and \a seconds.
	\sa hour(), minutes(), seconds(), milliSeconds(), setTime() */
  void time( int &hour, int &minutes, int &seconds, int &milliseconds, int index=0 ) const;
    /*! Set time of time parameter to \a hour, \a minutes, \a seconds, and \a milliseconds.
        If the value of the parameter is changing then the changedFlag() is set. */
  Parameter &setTime( int hour, int minutes, int seconds, int milliseconds=0 );
    /*! Add time \a hour, \a minutes, \a seconds, and \a milliseconds to time parameter.
        If the value of the parameter is changing then the changedFlag() is set. */
  Parameter &addTime( int hour, int minutes, int seconds, int milliseconds=0 );
    /*! Set time of time parameter to \a time.
        See Str::time() for valid date strings.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setTime( const string &time );
    /*! Set time of time parameter to \a time.
        If the value of the parameter is changing then the changedFlag() is set.
	\note the milliseconds are set to 0. */
  Parameter &setTime( const struct tm &time );
    /*! Set time of time parameter to \a time.
        If the value of the parameter is changing then the changedFlag() is set.
	\note the milliseconds are set to 0. */
  Parameter &setTime( const time_t &time );
    /*! Set time of time parameter to the current time.
        If the value of the parameter is changing then the changedFlag() is set.
	\note the milliseconds are set to 0. */
  Parameter &setCurrentTime( void );
    /*! Set time value to the one of \a p,
        if \a this and \a p are both of valueType() Time.
        If the value of the parameter is changing then the changedFlag() is set. */
  Parameter &setTime( const Parameter &p );
    /*! \return the \a index-th default hour. 
        \sa defaultMinutes(), defaultSeconds(), defaultMilliSeconds(), defaultText() */ 
  int defaultHour( int index=0 ) const;
    /*! \return the \a index-th default minutes. 
        \sa defaultHour(), defaultSeconds(), defaultMilliSeconds(), defaultText() */ 
  int defaultMinutes( int index=0 ) const;
    /*! \return the \a index-th default seconds. 
        \sa defaultHour(), defaultMinutes(), defaultMilliSeconds(), defaultText() */ 
  int defaultSeconds( int index=0 ) const;
    /*! \return the \a index-th default milliseconds. 
        \sa defaultHour(), defaultMinutes(), defaultSeconds(), defaultText() */ 
  int defaultMilliSeconds( int index=0 ) const;
    /*! Returns the \a index-th default time.
        \param[out] hour If Parameter is not a Time or \a index is invalid, 0 is returned.
        \param[out] minutes If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[out] seconds If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[out] milliseconds If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[in] index determines which one of the size() stored
	default time values is returned in \a hour, \a minutes, \a seconds, and \a milliseconds.
	\sa defaultHour(), defaultMinutes(), defaultSeconds(), defaultMilliSeconds(), setDefaultTime() */
  void defaultTime( int &hour, int &minutes, int &seconds, int &milliseconds, int index=0 ) const;
    /*! Set default time of time parameter to \a hour,
        \a minutes, \a milliseconds, and \a seconds. */
  Parameter &setDefaultTime( int hour, int minutes, int seconds, int milliseconds=0 );
    /*! Add time \a hour, \a minutes, \a seconds, and \a milliseconds to default of time parameter to. */
  Parameter &addDefaultTime( int hour, int minutes, int seconds, int milliseconds=0 );
    /*! Set default time of time parameter to \a time.
        See Str::time() for valid date strings. */
  Parameter &setDefaultTime( const string &time );

    /*! True if parameter is of no type. */
  bool isNotype( void ) const;
    /*! True if parameter is of no type and has no name. */
  bool empty( void ) const;
  
    /*! Set value of parameter to its default. */
  Parameter &setDefault( void );
    /*! Set default of parameter to its value. */
  Parameter &setToDefault( void );

    /*! Connect this Parameter to another Parameter with identifyer \a name.
        Such that in a dialog (like OptWidget) setting the value of 
	the parameter \a name to \a value will activate (\a activate = \c true)
	or inactivate (\a activate = \c false) this Parameter.
	Several values can be supplied by seperating them by '|'.
	A numerical value can be preceeded by '=', '>', '>=', '<', '<=', '<>'
        to determine the way of a numerical comparison.
	Numerical values can be supplied with a unit.
	If no unit is given, then the internally used unit is assumed.
        \sa addActivation(), clearActivation(), activations(),
	activationName(), activationValue(), activationValues(),
	activationNumber(), activationComparison(), activation() */
  Parameter &setActivation( const string &name, const string &value, bool activate=true );
    /*! Connect this Parameter to an additional Parameter with identifyer \a name.
        Such that in a dialog (like OptWidget) setting the value of 
	the parameter \a name to \a value will activate (\a activate = \c true)
	or inactivate (\a activate = \c false) this Parameter.
	Several values can be supplied by seperating them by '|'.
	A numerical value can be preceeded by '=', '>', '>=', '<', '<=', '<>'
        to determine the way of a numerical comparison.
	Numerical values can be supplied with a unit.
	If no unit is given, then the internally used unit is assumed.
        \sa setActivation(), clearActivation(), activations(),
	activationName(), activationValue(), activationValues(),
	activationNumber(), activationComparison(), activation() */
  Parameter &addActivation( const string &name, const string &value, bool activate=true );
    /*! Disconnect this Parameter from other Parameters 
        that can activate or inactivate it.
        \sa setActivation(), addActivation(), activations(),
	activationName(), activationValue(), activationValues(),
	activation() */
  Parameter &clearActivation( void );
    /*! The number of defined activation patterns.
        \sa setActivation(), addActivation(), clearActivation(),
	activationName(), activationValue(), activationValues(), activation() */
  int activations() const;
    /*! The name of the \a index-th Parameter that might activate or inactivate
        this Parameter.
        \sa setActivation(), addActivation(), clearActivation(),
	activations(), activationValue(), activationValues(), activation() */
  string activationName( int index=0 ) const;
    /*! The first value of the \a index-th Parameter on which this Parameter is activated
        or inactivated.
        \sa setActivation(), addActivation(), clearActivation(),
        activations(), activationValues(), activationNumber(),
        activationName(), activation() */
  string activationValue( int index=0 ) const;
    /*! All values of the \a index-th Parameter on which this Parameter is activated
        or inactivated separated by '|'.
        \sa setActivation(), addActivation(), clearActivation(),
        activations(), activationValue(), activationNumber(),
        activationName(), activation() */
  string activationValues( int index=0 ) const;
    /*! The numerical value of the \a index-th Parameter on which this Parameter is activated
        or inactivated according to activationComparison( index ).
        \sa setActivation(), addActivation(), clearActivation(),
	activations(), activationComparison(), activationValue(),
	activationName(), activation() */
  double activationNumber( int index=0 ) const;
    /*! The type of numerical comparison used for activating
        or inactivating this Parameter by the \a index-th Parameter.
        \sa setActivation(), addActivation(), clearActivation(),
	activations(), activationNumber(), activationValue(),
	activationName(), activation() */
  int activationComparison( int index=0 ) const;
    /*! If \c true, this Parameter is activated, otherwise inactivated,
        whenever the Parameter with identifier activationName( index )
	takes on the value activationValue( index).
	\sa setActivation(), addActivation(), clearActivation(),
        activations(), activationNumber(), activationName(),
        activationValue(), activationValues() */
  bool activation( int index=0 ) const;
    /*! Set the status of comparisopn \a index to \c true if \a value
        matches one of the activation values activationValues( index
        ).  \return \a true if all the test have their status set to \a true.
	\sa setActivation(), addActivation(), activations(),
        activationValue(), activationValues() */
  bool testActivation( int index, const string &value );
    /*! Set the status of comparisopn \a index to \c true true if \a
        value matches the activation number and type of comparison
        within the given tolerance \a tol.
	\return \a true if all the test have their status set to \a true.
        \sa setActivation(), addActivation(), activations(),
        activationNumber(), activationComparison() */
  bool testActivation( int index, double value, double tol=1e-8 );

    /*! Return string in the format "name: value".  Use for \a flags
        values from Options::SaveFlags to modify the way how the
        Parameter is written to the string. */
  string save( int flags=0 ) const;
    /*! Write parameter to stream \a str in the format "name: value".
        Use for \a flags values from Options::SaveFlags to modify the
        way how the Parameter is written to the string. */
  ostream &save( ostream &str, int width=0, int flags=0 ) const;
    /*! Write parameter to stream \a str according to the formats
        \a textformat, \a numberformat, \a boolformat, \a dateformat,
	and \a timeformat.
	\note No newline is added to the stream. */
  ostream &save( ostream &str, const string &textformat,
		 const string &numberformat, const string &boolformat,
		 const string &dateformat, const string &timeformat ) const;
    /*! Write parameter to stream \a str using save() */
  friend ostream &operator<< ( ostream &str, const Parameter &p );

    /*! Write parameter in XML format to output stream.
        \param[in] str the output stream
        \param[in] level the level of indentation
        \param[in] indent the indentation depth,
                   i.e. number of white space characters per level
        \param[in] flags modify the way how the Parameter is written to the stream.
	           See Options::SaveFlags for possible values
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int level=0, int indent=4,
		    int flags=0 ) const;

    /*! Read parameter value from string \a s if it has the same name.
        Uses assign() to assign the value of the parameter.
        Returns true if a value was assigned.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  bool read( const Str &s, const string &assignment=":=" );
    /*! Read parameter value from \a p if it has the same name.
        Numbers and Errors are converted from \a p's unit to the
	internal unit of the parameter.
        Returns true if a value was assigned.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  bool read( const Parameter &p );

    /*! Returns the value of \a val given in \a oldunit in the unit \a newunit.
        \a oldunit and \a newunit both are strings
        containing an optional number and an unit.
	if \a oldUnit or \a newUnit does not contain an unit,
	it is assumed that the units are both the same. */
  static double changeUnit( double val, const Str &oldunit, 
			    const Str &newunit );

  
private:

    /*! Add quotation marks around the string if necessary.
        If \a always is \t true, then always quote the string.
        If \a escape is \t true, then escape the quotes with backslash. */
  static string quoteString( string s, bool always=false, bool escape=false );

    /*! A pointer to the Options this parameter belongs to. */
  Options *ParentSection;

    /*! Name string of the parameter. */
  Str Name;
    /*! Request string for a dialog. */
  Str Request;
    /*! Specifies the type of the parameter's value. */
  ValueType VType;
    /*! Set some freely defineable flags. */
  int Flags;
    /*! Some flags defining the style how the parameter is displayed. */
  long Style;
    /*! Format string. */
  Str Format;
    /*! Text value. */
  StrQueue String;
    /*! Default text value. */
  StrQueue DefaultString;
    /*! Number value of the parameter. */
  vector< double > Value;
    /*! Default value of the number. */
  vector< double > DefaultValue;
    /*! Error (standard deviation) for the numbers. */
  vector< double > Error;
    /*! Minimum value of the number. */
  double Minimum;
    /*! Maximum value of the number. */
  double Maximum;
    /*! Step size for number parameter. */
  double Step;
    /*! Year */
  vector< int > Year;
    /*! Month */
  vector< int > Month;
    /*! Day */
  vector< int > Day;
    /*! Default year */
  vector< int > DefaultYear;
    /*! Default month */
  vector< int > DefaultMonth;
    /*! Default day */
  vector< int > DefaultDay;
    /*! Hour */
  vector< int > Hour;
    /*! Minutes */
  vector< int > Minutes;
    /*! Seconds */
  vector< int > Seconds;
    /*! Milliseconds */
  vector< int > MilliSeconds;
    /*! Default hour */
  vector< int > DefaultHour;
    /*! Default minutes */
  vector< int > DefaultMinutes;
    /*! Default seconds */
  vector< int > DefaultSeconds;
    /*! Default milliseconds */
  vector< int > DefaultMilliSeconds;
    /*! Internal unit of the parameter. */
  Str InternUnit;
    /*! Unit used for output. */
  Str OutUnit;

    /*! The names of another Parameter. */
  deque< string > ActivationName;
    /*! The values the ActivationName Parameters need to have
        in order to activate/inactivate this Parameter
        (in a dialog). */
  deque< StrQueue > ActivationValues;
    /*! The numbers against which value of the ActivationName Parameters
        are compared in order to activate/inactivate this Parameter
        (in a dialog). */
  deque< double > ActivationNumber;
    /*! The types of numerical comparison used for activating/inactivating this Parameter
        (in a dialog). 0=none, 1: =, 2: >, 3: >=, 4: <, 5: <=, 6: '<>' */
  deque< int > ActivationComparison;
    /*! Activate (\c true) or inactivate (\c false) this Parameter
        if the other Parameters specified by ActivationName
        have the value ActivationValue. */
  deque< bool > ActivationType;
    /*! Status of each activation test. */
  deque< bool > ActivationStatus;

    /*! Some warning messages. */
  mutable Str Warning;
};


}; /* namespace relacs */

#endif /* ! _RELACS_PARAMETER_H_ */
