/*
  parameter.h
  A Parameter has a name, value and unit.

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

#ifndef _RELACS_PARAMETER_H_
#define _RELACS_PARAMETER_H_ 1


#include <limits.h>
#include <values.h>
#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>
#include <relacs/str.h>
#include <relacs/strqueue.h>
using namespace std;

namespace relacs {


/*! 
\class Parameter
\author Jan Benda
\version 1.0
\brief A Parameter has a name, value and unit.


A single parameter has a value, which can be of different types
(numbers, booleans, dates, times, text, etc., see setType() ).
The parameter is uniquely identified by its ident-string.
Number parameters have in addition a unit and a format string,
which is used to generate pretty formated output.
This value can be changed by a dialog (as in Options)
or set to a default value.
For that purpose, the parameter has a request string, 
which is used in such dialogs.
*/

class Parameter 
{
public:
  
    /*! Type of the parameter's value. */
  enum Type {
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
      /*! The parameter is a label. */
    Label=64,
      /*! The parameter is a separator. */
    Separator=128,
      /*! The parameter is a label or a separator. */
    Blank=64+128
  };

  static const int ChangedFlag = 16384;
  static const int NonDefault = -32768;

    /*! Use this label to distinguish search patterns
        while saving parameter. */
  static const long SavePatternLabel = 0x01000000;
    /*! Use this label to distinguish search patterns
        while reading parameter. */
  static const long ReadPatternLabel = 0x02000000;
    /*! If this bit is set in the Parameter's style
        you can only select from text options with multiple values,
        but not add new values. */
  static const long SelectText = 0x01000000;
    /*! Mark a Label as a tab. */
  static const long TabLabel = 0x04000000;

    /*! Construct a single Parameter. 
        Use setType() to define the type of the parameter,
	setRequest() to define the request string, which is used
	for dialogs, setIdent() to give the parameter a unique identifier.
        With the set*() functions, the values of the parameter
        can be initialized. */
  Parameter( void );
    /*! Copy constructor. */
  Parameter( const Parameter &p );
    /*! Construct and initialize a single Parameter of type Text.
        Its value an its default value are set to \a strg. */
  Parameter( const string &ident, const string &request,
	     const string &strg, int flags=0, int style=0 );
    /*! Construct and initialize a single Parameter of type Text.
        Its value an its default value are set to \a strg. */
  Parameter( const string &ident, const string &request,
	     const char *strg, int flags=0, int style=0 );
    /*! Construct and initialize a single Parameter of type Number. 
        Its value and its default value are set to \a number,
	its standard deviation to \a error.
        The allowed range for its value is defined by
	\a minimum and \a maximum. 
        The unit of the number is \a interunit. 
	For output and dialogs \a outputunit is used as unit. */
  Parameter( const string &ident, const string &request,  
	     double number, double error,
	     double minimum=-MAXDOUBLE, double maximum=MAXDOUBLE, double step=1.0,
	     const string &internunit="", const string &outputunit="", 
	     const string &format="", int flags=0, int style=0 );
    /*! Construct and initialize a single Parameter of type Number. 
        Its values are set to \a numbers,
        its default value is set to \a numbers[0],
	its standard deviation to \a error.
        The allowed range for its value is defined by
	\a minimum and \a maximum. 
        The unit of the number is \a interunit. 
	For output and dialogs \a outputunit is used as unit. */
  Parameter( const string &ident, const string &request,  
	     const vector<double> &numbers,
	     const vector<double> &errors, 
	     double minimum=-MAXDOUBLE, double maximum=MAXDOUBLE, double step=1.0,
	     const string &internunit="", const string &outputunit="", 
	     const string &format="", int flags=0, int style=0 );
    /*! Construct and initialize a single Parameter of type Integer.
        Its value an its default value are set to \a number,
	its standard deviation to \a error.
        The allowed range for its value is defined by
        \a minimum and \a maximum. 
        The unit of the number is \a interunit. 
	For output and dialogs \a outputunit is used as unit. */
  Parameter( const string &ident, const string &request, 
	     long number, long error=-1, 
	     long minimum=LONG_MIN, long maximum=LONG_MAX, long step=1,
	     const string &internunit="", const string &outputunit="", 
	     int width=0, int flags=0, int style=0 );
    /*! Construct and initialize a single Parameter of type Boolean.
        Its value and its default value are set to \a dflt. */
  Parameter( const string &ident, const string &request,
	     bool dflt, int flags=0, int style=0 );
    /*! Construct and initialize a single Parameter of \a type Date or Time.
        Its value and its default value are set to 
	\a yearhour, \a monthminutes and \a dayseconds. */
  Parameter( const string &ident, const string &request, Type type,
	     int yearhour, int monthminutes, int dayseconds,
	     int flags=0, int style=0 );
    /*! Construct and initialize a single Parameter of type Label or
        Separator. If \a ident is empty, a Separator is constructed.
	otherwise a label with label \a ident is constructed.
	If \a sep is \c true, a Label gets the TabLabel-bit in its style set. */
  Parameter( const string &ident, bool sep, int flags=0, int style=0 );
    /*! Load option from string \a s using load(). */
  Parameter( const string &s, const string &assignment=":=" );
    /*! Load option by reading a single line from \a str and using load(). */
  Parameter( istream &str, const string &assignment=":=" );
    /*! Deconstruct Parameter. */
  ~Parameter( void );

    /*! Clear the parameter.
        Set the identity string to \a ident, the request string to \a request,
	and the type to \a type.
	The value of the parameter and the default value is cleared.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &clear( const string &ident="", const string &request="",
		    Type type=NoType );

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
        i.e. they have the same identifier. */
  friend bool operator==( const Parameter &p1, const Parameter &p2 );
    /*! Returns true if the Parameter \a p has an identifier 
        equal to \a ident. */
  friend bool operator==( const Parameter &p, const string &ident );

    /*! Returns \c true if the value differs from the default. */
  bool nonDefault( void ) const;

    /*! Returns the warning messages of the last called 
        Parameter member-function. */
  Str warning( void ) const { return Warning; };

    /*! Returns the identity string. */
  Str ident( void ) const { return Ident; };
    /*! Set identity string to \a ident. */
  Parameter &setIdent( const string &ident );
  
    /*! Returns the request string. */
  Str request( void ) const { return Request; };
    /*! Set request string to \a request. */
  Parameter &setRequest( const string &request );

    /*! The type of the parameter. */
  Type type( void ) const { return PType; };
    /*! True if one of the bits specified by \a mask corresponds to the
        parameter's type, or \a mask equals zero,
	or \a mask is negative and the parameter's type
	is not contained in abs( mask ). */
  bool types( int mask ) const;
    /*! Set type of parameter to \a pt. */
  Parameter &setType( Type pt );

    /*! The flags of the parameter. */
  int flags( void ) const { return Flags; };
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
  static int changedFlag( void ) { return ChangedFlag; };
    /*! Return \c true if the changedFlag() is set,
        i.e. whose value has been changed. */
  bool changed( void ) const;

    /*! The style of the parameter. */
  int style( void ) const { return Style; };
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
        %i is the identifier string, and
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
    /*! Set value of text parameter to \a strg.
        The text is splitted into several strings at each '|'.
        Each of the strings is also converted into numbers.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setText( const string &strg );
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
        if \a unit is an empty string. */
  double number( const string &unit="", int index=0 ) const;
    /*! Returns the \a index-th number value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string. */
  inline double number( int index, const string &unit="" ) const
    { return number( unit, index ); };
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
        if \a unit is an empty string. */
  long integer( const string &unit="", int index=0 ) const;
    /*! Returns the \a index-th integer value in 
        the unit \a unit or in the internal standard unit, 
        if \a unit is an empty string. */
  inline long integer( int index, const string &unit="" ) const
    { return integer( unit, index ); };
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
    /*! Add \a number and \a error to the integer values.
        The warning message is set if \a number is invalid.
        The ChangedFlag is not set, since the value is not changing. */
  Parameter &addInteger( long number, long error, const string &unit="" );
    /*! Add \a number to the integer values.
        The warning message is set if \a number is invalid.
        The ChangedFlag is not set, since the value is not changing. */
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
    /* Set the step size to \a step.
       If \a step is negative, the step size is set to \a maximum - \minimum
       devided by \a step.
       If \a step equals zero, the step size is set to 1/50 of
       \a maximum - \a minimum. */
  Parameter &setStep( double step );
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
  Parameter &setUnit( const string &internunit="", 
		      const string &outputunit="" );
    /*! Set the unit used for output and dialogs to \a outputunit. */
  Parameter &setOutUnit( const string &outputunit );
  
    /*! True if parameter is of type boolean. */
  bool isBoolean( void ) const;
    /*! Returns the \a index-th boolean value. */
  bool boolean( int index=0 ) const;
    /*! Set value of boolean parameter to \a b.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setBoolean( bool b );
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
    /*! \return the \a index-th hour. \sa minutes(), seconds(), text() */ 
  int hour( int index=0 ) const;
    /*! \return the \a index-th minutes. \sa hour(), seconds(), text() */ 
  int minutes( int index=0 ) const;
    /*! \return the \a index-th seconds. \sa hour(), minutes(), text() */ 
  int seconds( int index=0 ) const;
    /*! Returns the \a index-th time.
        \param[out] hour If Parameter is not a Time or \a index is invalid, 0 is returned.
        \param[out] minutes If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[out] seconds If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[in] index determines which one of the size() stored
	time values is returned in \a hour, \a minutes, and \a seconds.
	\sa hour(), minutes(), seconds(), setTime() */
  void time( int &hour, int &minutes, int &seconds, int index=0 ) const;
    /*! Set time of time parameter to \a hour, \a minutes, and \a seconds.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setTime( int hour, int minutes, int seconds );
    /*! Add time \a hour, \a minutes, and \a seconds to time parameter.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &addTime( int hour, int minutes, int seconds );
    /*! Set time of time parameter to \a time.
        See Str::time() for valid date strings.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setTime( const string &time );
    /*! Set time of time parameter to \a time.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setTime( const struct tm &time );
    /*! Set time of time parameter to \a time.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setTime( const time_t &time );
    /*! Set time of time parameter to the current time.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  Parameter &setCurrentTime( void );
    /*! \return the \a index-th default hour. 
        \sa defaultMinutes(), defaultSeconds(), defaultText() */ 
  int defaultHour( int index=0 ) const;
    /*! \return the \a index-th default minutes. 
        \sa defaultHour(), defaultSeconds(), defaultText() */ 
  int defaultMinutes( int index=0 ) const;
    /*! \return the \a index-th default seconds. 
        \sa defaultHour(), defaultMinutes(), defaultText() */ 
  int defaultSeconds( int index=0 ) const;
    /*! Returns the \a index-th default time.
        \param[out] hour If Parameter is not a Time or \a index is invalid, 0 is returned.
        \param[out] minutes If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[out] seconds If Parameter is not a Time or \a index is invalid, 0 is returned. 
        \param[in] index determines which one of the size() stored
	default time values is returned in \a hour, \a minutes, and \a seconds.
	\sa defaultHour(), defaultMinutes(), defaultSeconds(), setDefaultTime() */
  void defaultTime( int &hour, int &minutes, int &seconds, int index=0 ) const;
    /*! Set default time of time parameter to \a hour,
        \a minutes, and \a seconds. */
  Parameter &setDefaultTime( int hour, int minutes, int seconds );
    /*! Add time \a hour, \a minutes, and \a seconds to default of time parameter to. */
  Parameter &addDefaultTime( int hour, int minutes, int seconds );
    /*! Set default time of time parameter to \a time.
        See Str::time() for valid date strings. */
  Parameter &setDefaultTime( const string &time );

    /*! True if parameter is of type label (parameter without value). */
  bool isLabel( void ) const;
    /*! True if parameter is of type separator (parameter without identity and value). */
  bool isSeparator( void ) const;
    /*! True if parameter is of type label or separator. */
  bool isBlank( void ) const;
    /*! Returns the label, i.e. the identifier. */
  inline string label( void ) const { return ident(); };
    /*! Set the label (i.e. identifier string) to \ a label. */
  inline Parameter &setLabel( const string &label ) { return setIdent( label ); };

    /*! True if parameter is of no type. */
  bool isNotype( void ) const;
    /*! True if parameter is of no type and has no indentifier string. */
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
        \sa clearActivation(), activationName(), activationValue(),
	activationValues(), activationNumber(), activationComparison(), activation() */
  Parameter &setActivation( const string &name, const string &value, bool activate=true );
    /*! Disconnect this Parameter from another Parameter 
        that can activate or inactivate it.
        \sa setActivation(), activationName(), activationValue(),
	activationValues(), activation() */
  Parameter &clearActivation( void );
    /*! The name of the Parameter that might activate or inactivate
        this Parameter.
        \sa setActivation(), clearActivation(),
	activationValue(), activationValues(), activation() */
  string activationName( void ) const;
    /*! The first value of the Parameter on which this Parameter is activated
        or inactivated.
        \sa setActivation(), clearActivation(), activationValues(), activationNumber(),
        activationName(), activation() */
  string activationValue( void ) const;
    /*! All values of the Parameter on which this Parameter is activated
        or inactivated separated by '|'.
        \sa setActivation(), clearActivation(), activationValue(), activationNumber(),
        activationName(), activation() */
  string activationValues( void ) const;
    /*! The numerical value of the Parameter on which this Parameter is activated
        or inactivated according to activationComparison().
        \sa setActivation(), clearActivation(), activationComparison(),
	activationValue(), activationName(), activation() */
  double activationNumber( void ) const;
    /*! The type of numerical comparison used for activating
        or inactivating this Parameter.
        \sa setActivation(), clearActivation(), activationNumber(),
	activationValue(), activationName(), activation() */
  int activationComparison( void ) const;
    /*! If \c true, this Parameter is activated, otherwise inactivated,
        whenever the Parameter with identifyer \a activationName()
	takes on the value activationValue().
	\sa setActivation(), clearActivation(), activationNumber(),
        activationName(), activationValue(), activationValues() */
  bool activation( void ) const;
    /*! Return \c true if \a value matches one of the activation values
        activationValues().
        \sa setActivation(), activationValue(), activationValues() */
  bool testActivation( const string &value );
    /*! Return \c true if \a value matches the activation number and
        type of comparison within the given toelrance \a tol.
        \sa setActivation(), activationNumber(), activationComparison() */
  bool testActivation( double value, double tol=1e-8 );

    /*! Return string in the format "ident: value".
        If \a detailed equals \c true the request string is written, too. */
  string save( bool detailed=false, bool firstonly=false ) const;
    /*! Write parameter to stream \a str in the format "PatternIdent: Value".
        If \a detailed equals \c true the request string is written, too. */
  ostream &save( ostream &str, int width=0, bool detailed=false,
		 bool firstonly=false, const string &pattern="" ) const;
    /*! Write parameter to stream \a str according to the formats
        \a textformat, \a numberformat, \a boolformat, \a dateformat, 
	\a timeformat, \a labelformat, and \a separatorformat. */
  ostream &save( ostream &str, const string &textformat,
		 const string &numberformat, const string &boolformat,
		 const string &dateformat, const string &timeformat,
		 const string &labelformat, const string &separatorformat ) const;
    /*! Write parameter to stream \a str using save() */
  friend ostream &operator<< ( ostream &str, const Parameter &p );

    /*! Write parameter in XML format to output stream.
        \param[in] str the output stream
        \param[in] level the level of indentation
        \param[in] indent the indentation depth, 
                   i.e. number of white space characters per level
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int level=0, int indent=2 ) const;

    /*! Load parameter from string \a s.
        The warning message is set if \a s is invalid.
        The changedFlag() is set. */
  Parameter &load( Str s, const string &assignment=":=" );
    /*! Load parameter from a line get from stream \a str using load(). */
  friend istream &operator>> ( istream &str, Parameter &p );

    /*! Read parameter value from string \a s if it has the same identifier.
        Uses assign() to assign the value of the parameter.
        Returns true if a value was assigned.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  bool read( const Str &s, const string &assignment=":=" );
    /*! Read parameter value from string \a value
        if \a ident equals the parameters identifier.
        Uses assign() to assign the value of the parameter.
        Returns true if a value was assigned.
        If the value of the parameter is changing 
	then the changedFlag() is set. */
  bool read( const string &ident, const string &value );
    /*! Read parameter value from \a p if it has the same identifier.
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

    /*! Identifier string of the parameter. */
  Str Ident;
    /*! Request string for the Options::dialog(). */
  Str Request;
    /*! Specifies the type of the parameter. */
  Type PType;
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
    /*! Default hour */
  vector< int > DefaultHour;
    /*! Default minutes */
  vector< int > DefaultMinutes;
    /*! Default seconds */
  vector< int > DefaultSeconds;
    /*! Internal unit of the parameter. */
  Str InternUnit;
    /*! Unit used for output. */
  Str OutUnit;

    /*! The name of another Parameter. */
  string ActivationName;
    /*! The values the ActivationName Parameter needs to have
        in order to activate/inactivate this Parameter
        (in a dialog). */
  StrQueue ActivationValues;
    /*! The number against which value of the ActivationName Parameter
        is compared in order to activate/inactivate this Parameter
        (in a dialog). */
  double ActivationNumber;
    /*! The type of numerical comparison used for activating/inactivating this Parameter
        (in a dialog). 0=none, 1: =, 2: >, 3: >=, 4: <, 5: <=, 6: '<>' */
  int ActivationComparison;
    /*! Activate (\c true) or inactivate (\c false) this Parameter
        if the other Parameter specified by ActivationName
        has the value ActivationValue. */
  bool Activation;

    /*! Some warning messages. */
  mutable Str Warning;
};


}; /* namespace relacs */

#endif /* ! _RELACS_PARAMETER_H_ */
