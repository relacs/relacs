/*
  str.h
  Advanced string manipulation.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_STR_H_
#define _RELACS_STR_H_ 1

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

namespace relacs {

/*! 
\class Str
\author Jan Benda
\version 1.0
\brief Advanced string manipulation.

\bug check all find, find_first_of, etc for usage with empty search strings!!!
\todo Str( long, width, pad ), format( long ): missing type flag (u,d,x,...)
\todo generalize the double blank concept for spaces and separators to any character.
\todo Do we need each function with comments? Cant we just use stripComment before?
*/


class Str : public string
{
public:

   // diverse constructoren zur formatierung von Zahlen und strings, etc.

    /*! Constructs an empty string. */
  Str( void );
    /*! Constructs a copy of \a s. */
  Str( const Str &s );
    /*! Constructs a copy of \a s. */
  Str( const string &s );
    /*! Constructs a copy of \a s. */
  Str( const char *s );
    /*! Constructs a string containing the single character \a c. */
  Str( char c );

    /*! Constructs a string of size \a width containing a copy of \a s.
        If \a width is positive, \a s is right justified,
	otherwise \a s is left justified.
	\a pad is used as the character that is filled in.
        If \a s is longer than \a width, it is truncated.
        If \a width equals zero, the whole string \a s is copied to\a this. */
  Str( const string &s, int width, char pad=Pad );
    /*! Constructs a string of size \a width containing a copy of \a s.
        If \a width is positive, \a s is right justified,
	otherwise \a s is left justified.
	\a pad is used as the character that is filled in.
        If \a s is longer than \a width, it is truncated.
        If \a width equals zero, the whole string \a s is copied to\a this. */
  Str( const char *s, int width, char pad=Pad );
    /*! Constructs a string containing \a len copies of \a c. */
  Str( char c, int len );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( double val, int width=Width, int precision=Precision, 
       char format=DoubleFormat, char pad=Pad );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( signed long val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( unsigned long val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( signed int val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( unsigned int val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( signed short val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( unsigned short val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  Str( long long val, int width=Width, char pad=Pad );
    /*! Convert \a b into a string of size \a width. 
        Depending on \a format the resulting string is
        \c true or \c false (\a format equals \c t or \c f),
        \c yes or \c no (\a format equals \c y or \c n)
        \c 1 or \c 0 (otherwise).
        If \a width is positive, the resulting string is right justified,
        otherwise the resulting string is left justified. */
  Str( bool b, int width=Width, char format=BoolFormat, 
       char pad=Pad );

    /*! Constructs a string containing \a s according to the
        C-printf-style format string \a format. */
  Str( const string &s, const char *format );
    /*! Constructs a string containing \a s according to the
        C-printf-style format string \a format. */
  Str( const char *s, const char *format );
    /*! Constructs a string containing \a c according to the
        C-printf-style format string \a format. */
  Str( char c, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( double val, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( signed long val, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( unsigned long val, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( signed int val, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( unsigned int val, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( signed short val, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( unsigned short val, const char *format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( long long val, const char *format );

    /*! Constructs a string containing \a s according to the
        C-printf-style format string \a format. */
  Str( const string &s, const string &format );
    /*! Constructs a string containing \a s according to the
        C-printf-style format string \a format. */
  Str( const char *s, const string &format );
    /*! Constructs a string containing \a c according to the
        C-printf-style format string \a format. */
  Str( char c, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( double val, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( signed long val, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( unsigned long val, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( signed int val, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( unsigned int val, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( signed short val, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( unsigned short val, const string &format );
    /*! Converts \a val to this string according to the
        C-printf-style format string \a format. */
  Str( long long val, const string &format );

  // assign:
    /*! Assign \a s to \a this. */
  Str &operator=( const string &s );
    /*! Assign \a s to \a this. */
  Str &operator=( const char *s );
    /*! Assign \a c to \a this. */
  Str &operator=( char c );

    /*! Assigns a string of size \a width containing a copy of \a s.
        If \a width is positive, \a s is right justified,
	otherwise \a s is left justified.
	\a pad is used as the character that is filled in.
        If \a s is longer than \a width, it is truncated.
        If \a width equals zero, the whole string \a s is copied to\a this. */
  const Str &assign( const string &s, int width=Width, char pad=Pad );
    /*! Assigns a string of size \a width containing a copy of \a s.
        If \a width is positive, \a s is right justified,
	otherwise \a s is left justified.
	\a pad is used as the character that is filled in.
        If \a s is longer than \a width, it is truncated.
        If \a width equals zero, the whole string \a s is copied to\a this. */
  const Str &assign( const char *s, int width=Width, char pad=Pad );
    /*! Assigns a string containing \a len copies of \a c. */
  const Str &assign( char c, int len=CharRepeat );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( double val, int width=Width, int precision=Precision, 
		     char format=DoubleFormat, char pad=Pad );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( signed long val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( unsigned long val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( signed int val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( unsigned int val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( signed short val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( unsigned short val, int width=Width, char pad=Pad );
    /*! Convert \a val into a string of size \a width and assign it to \a this. 
        If \a width is positive, the number is right justified,
        otherwise the number is left justified. */
  const Str &assign( long long val, int width=Width, char pad=Pad );
    /*! Convert \a b into a string of size \a width and assign it to \a this. 
        Depending on \a format the resulting string is
        \c true or \c false (\a format equals \c t or \c f),
        \c yes or \c no (\a format equals \c y or \c n)
        \c 1 or \c 0 (otherwise).
        If \a width is positive, the resulting string is right justified,
        otherwise the resulting string is left justified. */
  const Str &assign( bool b, int width=Width, char format=BoolFormat, 
		     char pad=Pad );

    /*! Assigns a string containing \a s according to the
        C-printf-style format string \a format to \a this. */
  const Str &assign( const string &s, const char *format );
    /*! Assigns a string containing \a s according to the
        C-printf-style format string \a format to \a this. */
  const Str &assign( const char *s, const char *format );
    /*! Assigns a string containing \a c according to the
        C-printf-style format string \a format to \a this. */
  const Str &assign( char c, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( double val, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( signed long val, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( unsigned long val, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( signed int val, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( unsigned int val, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( signed short val, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( unsigned short val, const char *format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( long long val, const char *format );

    /*! Assigns a string containing \a s according to the
        C-printf-style format string \a format to \a this. */
  const Str &assign( const string &s, const string &format );
    /*! Assigns a string containing \a s according to the
        C-printf-style format string \a format to \a this. */
  const Str &assign( const char *s, const string &format );
    /*! Assigns a string containing \a c according to the
        C-printf-style format string \a format to \a this. */
  const Str &assign( char c, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( double val, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( signed long val, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( unsigned long val, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( signed int val, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( unsigned int val, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( signed short val, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( unsigned short val, const string &format );
    /*! Converts \a val to a string according to the C-printf-style format
        string \a format and assign it to \a this. */
  const Str &assign( long long val, const string &format );

  // append:
  inline Str &operator+=( const string &s ) { return static_cast<Str &>( string::operator+=( s ) ); };
  inline Str &operator+=( const char *s ) { return static_cast<Str &>( string::operator+=( s ) ); };
  inline Str &operator+=( char c ) { return static_cast<Str &>( string::operator+=( c ) ); };

  const Str &append( const string &s, int width=Width, char pad=Pad );
  const Str &append( const char *s, int width=Width, char pad=Pad );
  const Str &append( char c, int len=CharRepeat );
  const Str &append( double val, int width=Width, int precision=Precision, 
		     char format=DoubleFormat, char pad=Pad );
  const Str &append( signed long val, int width=Width, char pad=Pad );
  const Str &append( unsigned long val, int width=Width, char pad=Pad );
  const Str &append( signed int val, int width=Width, char pad=Pad );
  const Str &append( unsigned int val, int width=Width, char pad=Pad );
  const Str &append( signed short val, int width=Width, char pad=Pad );
  const Str &append( unsigned short val, int width=Width, char pad=Pad );
  const Str &append( long long val, int width=Width, char pad=Pad );
  const Str &append( bool b, int width=Width, char format=BoolFormat, 
		     char pad=Pad );

  const Str &append( const string &s, const char *format );
  const Str &append( const char *s, const char *format )
    { Construct( s, format, true ); return *this; };
  const Str &append( char c, const char *format )
    { Construct( c, format, true ); return *this; };
  const Str &append( double val, const char *format )
    { Construct( val, format, true ); return *this; };
  const Str &append( signed long val, const char *format )
    { Construct( (long)val, format, true ); return *this; };
  const Str &append( unsigned long val, const char *format )
    { Construct( (long)val, format, true ); return *this; };
  const Str &append( signed int val, const char *format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( unsigned int val, const char *format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( signed short val, const char *format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( unsigned short val, const char *format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( long long val, const char *format )
    { Construct( val, format, true ); return *this; };

  const Str &append( const string &s, const string &format );
  const Str &append( const char *s, const string &format )
    { Construct( s, format, true ); return *this; };
  const Str &append( char c, const string &format )
    { Construct( c, format, true ); return *this; };
  const Str &append( double val, const string &format )
    { Construct( val, format, true ); return *this; };
  const Str &append( signed long val, const string &format )
    { Construct( (long)val, format, true ); return *this; };
  const Str &append( unsigned long val, const string &format )
    { Construct( (long)val, format, true ); return *this; };
  const Str &append( signed int val, const string &format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( unsigned int val, const string &format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( signed short val, const string &format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( unsigned short val, const string &format )
    { Construct( long( val ), format, true ); return *this; };
  const Str &append( long long val, const string &format )
    { Construct( val, format, true ); return *this; };


  // add:
  inline friend Str operator+( const Str &s1, const Str &s2 )
    { return operator+( static_cast<const string &>(s1), static_cast<const string &>(s2) ); };
  inline friend Str operator+( const Str &s1, const string &s2 )
    { return operator+( static_cast<const string &>(s1), s2 ); };
  inline friend Str operator+( const string &s1, const Str &s2 ) { 
    return operator+( s1, static_cast<const string &>(s2) ); };
  inline friend Str operator+( const Str &s1, const char *s2 )
    { return operator+( static_cast<const string &>(s1), s2 ); };
  inline friend Str operator+( const char *s1, const Str &s2 )
    { return operator+( s1, static_cast<const string &>(s2) ); };
  inline friend Str operator+( const Str &s, char c )
    { return operator+( static_cast<const string &>(s), c ); };
  inline friend Str operator+( char c, const Str &s )
    { return operator+( c, static_cast<const string &>(s) ); };

    /*! The size of the string, i.e. the number of characters it contains. */
  inline int size( void ) const { return string::size(); };

  // ANSI-C like format specifiers:
    /*! Analyse the format string beginning at \a pos and returns the specified
        width \a width, precision \a precision, the format type \a type,
        and the padding character \a pad. 
        The index behind the last read character is returned. */
  int readFormat( int pos, int &width, int &precision, char &type, char &pad );
    /*! Returns the width specified by the ANSI-C like format string 
        where \a pos points to. */
  int formatWidth( int pos=0 ) const;
    /*! Returns the width of the string taking expanded ANSI-C like format
        strings into account. */
  int totalWidth( void ) const;

    /*! Replaces the first format specifier with type \a type 
        by the formatted string \a s.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( s, formatstring ) instead. */
  int format( const string &s, char type='s', int pos=0 );
    /*! Replaces the first format specifier with type \a type 
        by the formatted string \a s.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( s, formatstring ) instead. */
  int format( const char* s, char type='s', int pos=0 );
    /*! Replaces the first format specifier with type \a type 
        by the character \a c.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( c, formatstring ) instead. */
  int format( char c, char type='c', int pos=0 );
    /*! Replaces the first format specifier with type \a type 
        by the formatted number \a number using type \a ftype.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( number, formatstring ) instead. */
  int format( double number, char type, char ftype=' ', int pos=0 );
    /*! Replaces the first format specifier with type equal 
        to one of the charactert of \a type 
        by the formatted number \a number using type \a ftype.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( number, formatstring ) instead. */
  int format( double number, const string &type="fge", const string &ftype="", int pos=0 );
    /*! Replaces the first format specifier with type \a type 
        by the formatted number \a number using type \a ftype.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( number, formatstring ) instead. */
  int format( long number, char type='d', char ftype=' ', int pos=0 );
    /*! Replaces the first format specifier with type \a type 
        by the formatted number \a number using type \a ftype.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( number, formatstring ) instead. */
  inline int format( signed int number, char type='d', char ftype=' ', int pos=0 )
   { return format( long( number ), type, ftype, pos ); };
    /*! Replaces the first format specifier with type \a type 
        by the formatted number \a number using type \a ftype.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( number, formatstring ) instead. */
  inline int format( unsigned int number, char type='u', char ftype=' ', int pos=0 )
   { return format( long( number ), type, ftype, pos ); };
    /*! Replaces the first format specifier with type \a type 
        by the formatted number \a number using type \a ftype.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( number, formatstring ) instead. */
  inline int format( signed short number, char type='d', char ftype=' ', int pos=0 )
   { return format( long( number ), type, ftype, pos ); };
    /*! Replaces the first format specifier with type \a type 
        by the formatted number \a number using type \a ftype.
	Returns the position of the character following the replacement.
	If nothing was replaced, \a pos is returned.
        \warning format() does not return the formatted string!
        Use the constructor Str( number, formatstring ) instead. */
  inline int format( unsigned short number, char type='u', char ftype=' ', int pos=0 )
   { return format( long( number ), type, ftype, pos ); };

  /*! Format the string using the time given by \a time.
      The following format specifiers are recognized:
      %S: second (0..59)
      %M: minute (0..59)
      %H: hour (0..23)
      %d: day of month (1..31)
      %m: month (1..12)
      %y: last two digits of year (0..99)
      %Y: year (1970...)
      Width and padding character may also be specified, 
      e.g. %02d produces 01, 02, 03, ... 10, 11, ... 31. */
  Str &format( const struct tm *time );


  // read numbers:

    /*! Returns the value of the first number in the string.
        The number may be preceeded by white space and a single opening bracket.
        If there is no number or an error occured during conversion,
        \a dflt is returned and \a next is set to \a index.
        Search for a number is started at the \a index -th character.
        The index of the character following the number is retuned in \a next
        if \a next is not 0. */
  double number( double dflt=0.0, int index=0, int *next=0,
		 const string &space=Space ) const;
    /*! Returns the value of the first number in the string.
        The number may be preceeded by white space and a single opening bracket.
	In \a error the error value following the number is returned.
        \a error is only set at success.
        If there is no number or an error occured during conversion,
        \a dflt is returned and \a next is set to \a index.
        Search for a number is started at the \a index -th character.
        The index of the character following the number is retuned in \a next
        if \a next is not 0. */
  double number( double &error, double dflt=0.0, int index=0, int *next=0,
		 const string &space=Space ) const;
    /*! Returns the value of the first number in the string.
        The number may be preceeded by white space and a single opening bracket.
	In \a error the error value following the number is returned.
	In \a unit the unit string following the number (and error value) is returned.
        \a error and \a unit are only set at success.
        If there is no number or an error occured during conversion,
        \a dflt is returned and \a next is set to \a index.
        Search for a number is started at the \a index -th character.
        The index of the character following the number is retuned in \a next
        if \a next is not 0. */
  double number( double &error, string &unit,
		 double dflt=0.0, int index=0, int *next=0,
		 const string &space=Space ) const;
    /*! Returns the value of error following the first number in the string.
        The number and its error are separated by '+/-' or '+-'.
        The number may be preceeded by white space and a single opening bracket.
        If there is no error value or an error occured during conversion,
        \a dflt is returned and \a next is set to \a index.
        Search for a number is started at the \a index -th character.
        The index of the character following the error is retuned in \a next
        if \a next is not 0. */
  double error( double dflt=0.0, int index=0, int *next=0,
		const string &space=Space ) const;
    /*! Returns the unit following the first number in the string.
        The number and its possible error value 
        error are separated by '+/-' or '+-'.
        The number may NOT be preceeded by any non-numerical characters.
        If the string does not start directly with a number,
        it is treated as a unit.
        If there is no unit string,
        \a dflt is returned and \a next is set to \a index.
        Search for a number is started at the \a index -th character.
        The index of the character following the unit is retuned in \a next
        if \a next is not 0. */
  Str unit( const string &dflt="", int index=0, int *next=0,
	    const string &space=Space ) const;

    /*! Returns the stripped() string befor the first occurence of one
        of the characters specified in \a a. */
  Str ident( int index=0, const string &a=":=",
	     const string &space=Space ) const;
    /*! Returns the stripped() string right behind the first occurence of one
        of the characters specified in \a a. */
  Str value( int index=0, const string &a=":=",
	     const string &space=Space ) const;

    /*! Reads the \a year, \a month, and \a day of a date string.
        The string should be formatted as "YYYY-MM-DD", "MM/DD/YYYY"
	or "MMM DD, YYYY"
        \return 0 on success
        \return -1 the string is not a date
        \return -2 invalid year
        \return -4 invalid month
        \return -8 invalid day */
  int date( int &year, int &month, int &day ) const;

    /*! Reads the \a hour, \a minutes, and \a seconds of a time string.
        The string should be formatted as "HH:MM:SS".
        \return 0 on success
        \return -1 the string is not a time
        \return -2 invalid hour
        \return -4 invalid minutes
        \return -8 invalid seconds */
  int time( int &hour, int &minutes, int &seconds ) const;

  // find value of identifier:

    /*! Searches for the string \a search.
        Returns index to the found search string or -1 if it wasn't found.
        Additional control characters in the search string are
	supportet (they can be combined):
	^aaa: search is case sensitive,
	!aaa: aaa has to be the beginning of a word,
	!!aaa: aaa is the first text appearing in the string,
	aaa!: aaa has to be the end of a word,
	!aaa!: aaa has to be a word. */
  int ident( const string &search,
	     const string &space=Space ) const;
    /*! Searches for the string \a search.
        Returns an index to the text following the first ':' or '='
	after the found search string or -1 if \a search wasn't found.
        Additional control characters in the search string are
	supportet (they can be combined):
	^aaa: search is case sensitive,
	!aaa: aaa has to be the beginning of a word,
	!!aaa: aaa is the first text appearing in the string,
	aaa!: aaa has to be the end of a word,
	!aaa!: aaa has to be a word. */
  int value( const string &search,
	     const string &space=Space ) const;
    /*! Returns the text following the first ':' or '='
	after the found search string \a search
	or \a dflt if \a search wasn't found or there isn't any number.
	ident() is used for searching. */
  Str text( const string &search, const string &dflt,
	    const string &space=Space ) const;
    /*! Returns the value of the number following the first ':' or '='
	after the found search string \a search
	or \a dflt if \a search wasn't found or there isn't any number.
	ident() is used for searching. */
  double number( const string &search, double dflt=0.0,
		 const string &space=Space ) const;
    /*! Returns the error of the number following the first ':' or '='
	after the found search string \a search
	or \a dflt if \a search wasn't found or there isn't any error.
	ident() is used for searching. */
  double error( const string &search, double dflt=0.0,
		const string &space=Space ) const;
    /*! Returns the unit of the number following the first ':' or '='
	after the found search string \a search
	or \a dflt if \a search wasn't found or there isn't any number.
	ident() is used for searching. */
  Str unit( const string &search, const string &dflt="",
	    const string &space=Space ) const;

  // range:
    /*! Extracts a list of integer numbers from the content of the string.
        Individual numbers are separated by \a sep, ranges of numbers are
	indicated by \a r.
	Example: "1,3,6..8,10" results in 1,3,6,7,8,10 */
  void range( vector< int > &ri, const string &sep=",", const string &r=".." ) const;

  // find:
  int find( const string &s, int index=0, bool cs=true, 
	    int word=0, const string &space=Space ) const;
  int find( const char *s, int index=0, bool cs=true, 
	    int word=0, const string &space=Space ) const;
  int find( char c, int index=0, bool cs=true, 
	    int word=0, const string &space=Space ) const;

  int rfind( const string &s, int index=-2, bool cs=true, 
	     int word=0, const string &space=Space ) const;
  int rfind( const char *s, int index=-2, bool cs=true, 
	     int word=0, const string &space=Space ) const;
  int rfind( char c, int index=-2, bool cs=true, 
	     int word=0, const string &space=Space ) const;

  int findFirst( const string &s, int index=0 ) const;
  int findFirst( const char *s, int index=0 ) const;
  int findFirst( char c, int index=0 ) const;

  int findFirstNot( const string &s, int index=0 ) const;
  int findFirstNot( const char *s, int index=0 ) const;
  int findFirstNot( char c, int index=0 ) const;

  int findLast( const string &s, int index=-2 ) const;
  int findLast( const char *s, int index=-2 ) const;
  int findLast( char c, int index=-2 ) const;

  int findLastNot( const string &s, int index=-2 ) const;
  int findLastNot( const char *s, int index=-2 ) const;
  int findLastNot( char c, int index=-2 ) const;


  // contains:
  int contains( const string &s, int index=0, bool cs=true, 
		int word=0, const string &space=Space, 
		const string &comment=Comment ) const;
  int contains( const char *s, int index=0, bool cs=true, 
		int word=0, const string &space=Space, 
		const string &comment=Comment ) const;
  int contains( char ch, int index=0, bool cs=true, 
		int word=0, const string &space=Space, 
		const string &comment=Comment ) const;


  // brackets:

  static const string &bracket( void ) { return Bracket; };
  static void setBracket( const string &bracket ) { Bracket = bracket; };

    /*! Find corresponding closing bracket for the one at position \a index. */
  int findBracket( int index=0, const string &brackets=Bracket, 
		   const string &comment=Comment ) const;

  Str &stripBracket( const string &brackets=Bracket, 
			const string &comment=Comment );
  Str strippedBracket( const string &brackets=Bracket, 
			  const string &comment=Comment ) const;


  // manipulation:
  Str &truncate( int len ) 
    { if ( len < size() && len >= 0 ) string::resize( len ); return *this; };
  Str &resize( int len ) 
    { if ( len < size() && len >= 0 ) string::resize( len ); return *this; };
  Str &resize( int len, char ch ) 
    { if ( len >= 0 ) string::resize( len, ch ); return *this; };

  Str left( int len ) const 
    { if ( len >= 0 ) return string::substr( 0, len ); else return *this; };
  Str right( int len ) const 
    { if ( len >= 0 && len <= size() ) return string::substr( size()-len, len ); else return *this; };
  Str mid( int pos, int upto=-1 ) const 
    { if ( upto < 0 || upto >= size() ) upto = size()-1; if ( pos < 0 || pos >= size() || upto < pos ) return ""; return string::substr( pos, upto-pos+1 ); };
  Str substr( int pos, int len=-1 ) const 
    { if ( pos < 0 || pos >= size() ) return ""; if ( len < 0 || len > size()-pos ) len = size() - pos; return string::substr( pos, len ); };


    /*! Erase substring at \a pos with length \a n. */
  Str &erase( int pos, int n=-1 ) 
    { string::size_type nn = n < 0 ? npos : n; string::erase( pos, nn ); return *this; };
    /*! Remove all sequences in the string which equal \a s. 
        Returns the number of removed sequences. */
  int erase( const string &s );
    /*! Returns the string with all sequences which equal \a s removed. */
  Str erased( const string &s ) const;
    /*! Remove all sequences in the string which equal \a s.
        Finding the sequences is started at index \a index.
	It is case sensitive if \a cs equals true.
	If bit one in word is set \a s has to be preceeded by 
	at least one character given in \a space.
	If bit two in word is set \a s has to be succeeded by 
	at least one character given in \a space.
        Returns the number of removed sequences. */
  int erase( const string &s, int index, bool cs=true, 
	     int word=0, const string &space=Space );
    /*! Remove \a s from the string if it is found at the beginning. 
        Returns \c true if \a s was removed from the string. */
  bool eraseFirst( const string &s );
    /*! Returns the string with \a s removed
        it \a s was found at the beginning of the string. */
  Str erasedFirst( const string &s ) const;
    /*! Remove \a s from the string if \a s is found
        at the position of the first character not in \a space.
        Finding \a s is started at index \a index.
	It is case sensitive if \a cs equals true.
	If bit two in word is set \a s has to be succeeded by 
	at least one character given in \a space.
        Returns true if \a s was found and removed. */
  bool eraseFirst( const string &s, int index, bool cs=true, 
		   int word=0, const string &space=Space );

    /*! Replace substring at \a pos with length \a n by \a rs. */
  Str &replace( int pos, int n, const string &rs ) 
    { string::size_type nn = n < 0 ? npos : n; string::replace( pos, nn, rs ); return *this; };
    /*! Replace all sequences in the string which equal \a s by \a rs. 
        Returns the number of replaced sequences. */
  int replace( const string &s, const string &rs );
    /*! Replace all sequences in the string which equal \a s by \a rs.
        Finding the sequences is started at index \a index.
	It is case sensitive if \a cs equals true.
	If bit one in word is set \a s has to be preceeded by 
	at least one character given in \a space.
	If bit two in word is set \a s has to be succeeded by 
	at least one character given in \a space.
        Returns the number of replaced sequences. */
  int replace( const string &s, const string &rs, int index, bool cs=true, 
	       int word=0, const string &space=Space );
    /*! Returns the string with all sequences which equal \a s
        replaced by \a rs. */
  Str replaced( const string &s, const string &rs ) const;

    /*! Removes all markup sequences in the string. 
        That is everything inside sharp braces "<>".
	\warning This is not yet properly implemented.
	Currently only <b>, </b>, and <br> are removed. */
  int eraseMarkup( void );
    /*! Retruns the string with all markup sequences removed. 
        That is everything inside sharp braces "<>".
	\warning This is not yet properly implemented.
	Currently only <b>, </b>, and <br> are removed. */
  Str erasedMarkup( void ) const;

    /*! Remove the first character of the string
        if it equals \a c. */
  Str &preventFirst( char c );
    /*! Returns the string with its first character removed 
        if it equals \a c. */
  Str preventedFirst( char c ) const;
    /*! Remove the initial part of the string
        if it equals \a s. */
  Str &preventFirst( const char* s );
    /*! Returns the string with its initial part removed 
        if it equals \a s. */
  Str preventedFirst( const char* s ) const;
    /*! Remove the initial part of the string
        if it equals \a s. */
  Str &preventFirst( const string &s );
    /*! Returns the string with its initial part removed 
        if it equals \a s. */
  Str preventedFirst( const string &s ) const;

    /*! Remove the last character of the string
        if it equals \a c. */
  Str &preventLast( char c );
    /*! Returns the string with its last character removed 
        if it equals \a c. */
  Str preventedLast( char c ) const;
    /*! Remove the end of the string
        if it equals \a s. */
  Str &preventLast( const char* s );
    /*! Returns the string with its end removed 
        if it equals \a s. */
  Str preventedLast( const char* s ) const;
    /*! Remove the end of the string
        if it equals \a s. */
  Str &preventLast( const string &s );
    /*! Returns the string with its end removed 
        if it equals \a s. */
  Str preventedLast( const string &s ) const;

    /*! Insert \a c at the beginning of the string
        if the strings first character does not equal \a c. */
  Str &provideFirst( char c );
    /*! Returns the string with \a c inserted at the beginning
       if the strings first character does not equal \a c. */
  Str providedFirst( char c ) const;
    /*! Insert \a s at the beginning of the string
        if the strings first characters do not equal \a s. */
  Str &provideFirst( const char* s );
    /*! Returns the string with \a s inserted at the beginning
       if the strings first characters do not equal \a s. */
  Str providedFirst( const char* s ) const;
    /*! Insert \a s at the beginning of the string
        if the strings first characters do not equal \a s. */
  Str &provideFirst( const string &s );
    /*! Returns the string with \a s inserted at the beginning
       if the strings first characters do not equal \a s. */
  Str providedFirst( const string &s ) const;

    /*! Add \a c to the string
       if the strings last character does not equal \a c. */
  Str &provideLast( char c );
    /*! Returns the string with \a c added
       if the strings last character does not equal \a c. */
  Str providedLast( char c ) const;
    /*! Add \a s to the string
       if the strings last characters do not equal \a s. */
  Str &provideLast( const char* s );
    /*! Returns the string with \a s added
       if the strings last characters do not equal \a s. */
  Str providedLast( const char* s ) const;
    /*! Add \a s to the string
       if the strings last characters do not equal \a s. */
  Str &provideLast( const string &s );
    /*! Returns the string with \a s added
       if the strings last characters do not equal \a s. */
  Str providedLast( const string &s ) const;

  // upper/lower case:
  Str &upper( void );
  Str up( void ) const;

  Str &lower( void );
  Str low( void ) const;

  /// justify:
  Str &leftJustify( int width=Width, char pad=Pad );
  Str leftJustified( int width=Width, char pad=Pad ) const
    { return Str( *this, -::abs( width ), pad ); };

  Str &rightJustify( int width=Width, char pad=Pad );
  Str rightJustified( int width=Width, char pad=Pad ) const
    { return Str( *this, ::abs( width ), pad ); };

  Str &centerJustify( int width=Width, char pad=Pad );
  Str centerJustified( int width=Width, char pad=Pad ) const;

  Str &goldenJustify( int width=Width, char pad=Pad );
  Str goldenJustified( int width=Width, char pad=Pad ) const;


  // whitespace:

  static const string &space( void ) { return Space; };
  static void setSpace( const string &space ) { Space = space; };

  static string comment( void ) { return Comment; };
  static void setComment( const string &comment ) { Comment = comment; };

  bool empty( const string &space=Space, const string &comment=Comment ) const;

  Str &strip( const string &space=Space, const string &comment=Comment );
  Str &strip( char space, const string &comment=Comment );
  Str stripped( const string &space=Space, const string &comment=Comment ) const;
  Str stripped( char space, const string &comment=Comment ) const;

  Str &simplify( const string &space=Space, const string &comment=Comment );
  Str &simplify( char space, const string &comment=Comment );
  Str simplified( const string &space=Space, const string &comment=Comment ) const;
  Str simplified( char space, const string &comment=Comment ) const;

    /*! Removes comments from the string.
        If \a comment is set to 'XXX' everything behind 'XXX' inclusively
        is erased from the string.
        If \a comment is set to '-XXX' everything before 'XXX' inclusively
        is erased from the string. */
  Str &stripComment( const string &comment=Comment );
    /*! Returns the string with comments removed.
        If \a comment is set to 'XXX' everything behind 'XXX' inclusively
        is erased from the string.
        If \a comment is set to '-XXX' everything before 'XXX' inclusively
        is erased from the string. */
  Str strippedComment( const string &comment=Comment ) const;

  int first( const string &space=Space ) const
    { return findFirstNot( space ); };

    /*! Returns the index of the next word following position \a index. 
        In \a index the position of the first character
	following the word is returned.
        Words are separated by arbitrary many characters specified in \a space. */
  int nextWord( int &index, const string &space=Space, const string &comment=Comment ) const;
    /*! Returns the word following position \a index. 
        In \a index the position of the first character
	following the word is returned.
        Words are separated by arbitrary many characters specified in \a space. */
  Str wordAt( int &index, const string &space=Space, const string &comment=Comment ) const;
  Str word( int n, const string &space=Space, const string &comment=Comment ) const;
    /*! Returns the number of words contained in the string separated by \a space
        before a comment \a comment. */
  int words( const string &space=Space, const string &comment=Comment ) const;

  // einzelne Felder:

    /*! Returns the index of the next field following position \a index. 
        In \a index the position of the first character of the
	following field is returned.
        Fields are separated by single characters specified in \a space. */
  int nextField( int &index, const string &space=Space, const string &comment=Comment ) const;

  static const string &separator( void ) { return Separator; };
  static void setSeparator( const string &separator ) { Separator = separator; };

    /*! Find first separator following position \a index. */
  int findSeparator( int index=0, const string &separator=Separator,
		     const string &brackets=Bracket, 
		     const string &comment=Comment ) const;
  Str field( int n, const string &separator=Separator,
		const string &brackets=Bracket, 
		const string &comment=Comment ) const;


  // file name manipulation:

    /*! The character which is used to separate directories in a file name. 
        By default it is a slash '/'. */
  static char dirSep( void ) { return DirSep; };
    /*! Set the character which is used to separate directories 
        in a file name to \a dirsep. */
  static void setDirSep( char dirsep ) { DirSep = dirsep; };

    /*! The character which is used to separate the extension of a file name. 
        By default it is a period. */
  static char extSep( void ) { return ExtSep; };
    /*! Set the character which is used to separate the extension
        of a file name to \a extsep. */
  static void setExtSep( char extsep ) { ExtSep = extsep; };

    /*! The character which specifies the home directoy of a file name.
        By default it is a tilde. */
  static char homeChr( void ) { return HomeChr; };
    /*! Set the character which specifies the home directory
        of a file name to \a homechr. */
  static void setHomeChr( char homechr ) { HomeChr = homechr; };

    /*! The environment variable which specifies the home directoy.
        By default it is "HOME". */
  static string &homeEnv( void ) { return HomeEnv; };
    /*! Set the name of the environment variable which specifies 
        the home directory to \a homeenv. */
  static void setHomeEnv( const string &homeenv ) { HomeEnv = homeenv; };

    /*! The home directory with a slash(\a dirsep) at the end. 
        It is the path specified by the environment variable
        specified by \a HomeEnv.
        If this environment variable does not exist, \a dirsep 
        (a slash) is returned. */
  static Str homePath( char dirsep=DirSep );

    /*! The environment variable which specifies the current working directoy.
        By default it is "PWD". */
  static string &workingEnv( void ) { return WorkingEnv; };
    /*! Set the name of the environment variable which specifies 
        the current working directory to \a home. */
  static void setWorkingEnv( const string &workingenv ) { WorkingEnv = workingenv; };

    /*! The current working directory with a slash(\a dirsep) at the end. 
        It is the path specified by the environment variable
        specified by \a WorkingEnv.
        If this environment variable does not exist, an empty string
        is returned. */
  static Str workingPath( char dirsep=DirSep );
    /*! If the string contains an absolute path (beginning with \a dirsep)
        then convert the path to a path relative to the 
        current working directory.
        Only do this for at maximum \a maxlevel levels below the 
        current working directory. */
  Str &stripWorkingPath( int maxlevel=10, char dirsep=DirSep );

    /*! Returns the directory part of a file name.
        This is everything upto and including the last character \a dirsep
        (defaults to a slash) found in the string. 
        If no \a dirsep is found, an empty string is returned.
        For example: If the string is \c example/data.txt.gz, 
        \c example/ is returned. */
  Str dir( char dirsep=DirSep ) const;
    /*! Returns everything except the directory part of a file name.
        This is everything behind the last character \a dirsep
        (defaults to a slash) found in the string.
        If no \a dirsep is found, the whole string is returned.
        For example: If the string is \c example/data.txt.gz, 
        \c data.txt.gz is returned. */
  Str notdir( char dirsep=DirSep ) const;
    /*! Returns everything except the directory part and all extensions
        of a file name.
        This is everything behind the last character \a dirsep
        (defaults to a slash) found in the string
	and before the following first character \a extsep 
        (defaults to a period).
        For example: If the string is \c example/data.txt.gz, 
        \c data is returned. */
  Str name( char dirsep=DirSep, char extsep=ExtSep ) const;
    /*! Returns everything except the directory part and the last extension
        of a file name.
        This is everything behind the last character \a dirsep
        (defaults to a slash) found in the string
	and before the last character \a extsep 
        (defaults to a period).
        For example: If the string is \c example/data.txt.gz, 
        \c data.txt is returned. */
  Str longName( char dirsep=DirSep, char extsep=ExtSep ) const;
    /*! Returns all extensions of a file name
        including their period.
        For example: If the string is \c example/data.txt.gz, 
        \c .txt.gz is returned. */
  Str extension( char dirsep=DirSep, char extsep=ExtSep ) const;
    /*! Returns the last extension of a file name
        including its period.
        For example: If the string is \c example/data.txt.gz, 
        \c .gz is returned. */
  Str suffix( char dirsep=DirSep, char extsep=ExtSep ) const;
    /*! Returns everything exept of all extensions of a file name.
        For example: If the string is \c example/data.txt.gz, 
        \c example/data is returned. */
  Str basename( char dirsep=DirSep, char extsep=ExtSep ) const;

    /*! Remove the directory part of a file name.
        This is everything upto and including the last character \a dirsep
        (defaults to a slash) found in the string. 
        If no \a dirsep is found, nothing is done.
        For example: If the string is \c example/data.txt.gz, 
        \c data.txt.gz remains. */
  Str &stripDir( char dirsep=DirSep );
    /*! Removes everything except the directory part of a file name.
        This is everything behind the last character \a dirsep
        (defaults to a slash) found in the string.
        If no \a dirsep is found, the string is cleared.
        For example: If the string is \c example/data.txt.gz, 
        \c example/ remains. */
  Str &stripNotdir( char dirsep=DirSep );
    /*! Remove all extensions of a file name
        including their period.
        For example: If the string is \c example/data.txt.gz, 
        \c example/data remains. */
  Str &stripExtension( char dirsep=DirSep, char extsep=ExtSep );
    /*! Remove the last extension of a file name
        including its period.
        For example: If the string is \c example/data.txt.gz, 
        \c example/data.txt remains. */
  Str &stripSuffix( char dirsep=DirSep, char extsep=ExtSep );
    /*! Removes everything exept of all extensions of a file name.
        For example: If the string is \c example/data.txt.gz, 
        \c .txt.gz remains. */
  Str &stripBasename( char dirsep=DirSep, char extsep=ExtSep );

    /*! Remove the last character of the string
        if it equals \a dirsep (defaults to a slash). */
  Str &preventSlash( char dirsep=DirSep ) 
    { return preventLast( dirsep ); };
    /*! Returns the string with its last character removed 
        if it equals \a dirsep (defaults to a slash). */
  Str preventedSlash( char dirsep=DirSep ) const 
    { return preventedLast( dirsep ); };
    /*! Add \a dirsep to the string
       if its last character does not equal \a dirsep (defaults to a slash)
       and the string is not empty. */
  Str &provideSlash( char dirsep=DirSep );
    /*! Returns the string with \a dirsep added
       if its last character does not equal \a dirsep (defaults to a slash)
       and the string is not empty. */
  Str providedSlash( char dirsep=DirSep ) const;

    /*! Remove the last character of the string
        if it equals \a extsep (defaults to a period). */
  Str &preventPeriod( char extsep=ExtSep ) 
    { return preventLast( extsep ); };
    /*! Returns the string with its last character removed 
        if it equals \a extsep (defaults to a period). */
  Str preventedPeriod( char extsep=ExtSep ) const 
    { return preventedLast( extsep ); };
    /*! Add \a extsep to the string if its last character 
        does not equal \a extsep (defaults to a period). */
  Str &providePeriod( char extsep=ExtSep ) 
    { return provideLast( extsep ); };
    /*! Returns the string with \a extsep added if its last 
        character does not equal \a extsep (defaults to a period). */
  Str providedPeriod( char extsep=ExtSep ) const 
    { return providedLast( extsep ); };

    /*! Expands a leading \a homechr (defaults to a tilde)
        to the home directoy which is obtained from the 
	environment variable \a HOME. 
        The default for \a homechr can be changed by setHomeChr(),
        the name of the environment variable used to get
        the home directory can be changed by setHome(). */
  Str &expandHome( char dirsep=DirSep, char homechr=HomeChr );
    /*! Returns the string with a leading \a homechr (defaults to a tilde)
        expanded to the home directoy which is obtained from the 
	environment variable \a HOME. 
        The default for \a homechr can be changed by setHomeChr(),
        the name of the environment variable used to get
        the home directory can be changed by setHome(). */
  Str expandedHome( char dirsep=DirSep, char homechr=HomeChr ) const;

    /*! Remove all '.' directories from the string. */
  Str &removeWorking( char dirsep=DirSep );
    /*! Return the string with all '.' directories removed. */
  Str removedWorking( char dirsep=DirSep ) const;

    /*! Prepends the directory \a dir to the sring
        if it does not begin with a slash (\a dirsep).
	It is ensured that between the current content of the Str and 
	\a dir there is slash (\a dirsep).
	If \a dir is not specified, then
        the current working directory is obtained from the 
        environment variable specified by setWorkingEnv(),
        which defaults to PWD. */
  Str &addWorking( const string &dir="", char dirsep=DirSep );
    /*! Return the string with the directory \a dir prepended
        if it does not begin with a slash (\a dirsep).
	It is ensured that between the current content of the Str and 
	\a dir there is slash (\a dirsep).
	If \a dir is not specified, then
        the current working directory is obtained from the 
        environment variable specified by setWorkingEnv(),
        which defaults to PWD. */
  Str addedWorking( const string &dir="", char dirsep=DirSep ) const;

    /*! Expand all '..' directories in the string. */
  Str &expandParent( char dirsep=DirSep );
    /*! Return the string with all '..' directories expanded. */
  Str expandedParent( char dirsep=DirSep ) const;

    /*! Expands the string to a complete file name.
        All special characters (~, ./, and /..) are
	processed.
        First a leading tilde (\a homechr) is expanded to the current
        home directory (see expandHome() for details. 
	Then all references to the current directory ("./")
 	are removed from the string (see removeWorking() for details).
	If the string does not begin with a slash (\a dirsep)
	\a dir is prepended to the string.
	If \a dir is empty, the current working directory is used
	(see addWorking() for details).
	Finally all "/.." are expanded (see expandParent() for details. */
  Str &expandPath( const string &dir="", 
		      char dirsep=DirSep, char homechr=HomeChr );
    /*! Returns the string expanded to a complete file name.
        All special characters (~, ./, and /..) are
	processed.
        First a leading tilde (\a homechr) is expanded to the current
        home directory (see expandHome() for details. 
	Then all references to the current directory ("./")
 	are removed from the string (see removeWorking() for details).
	If the string does not begin with a slash (\a dirsep)
	\a dir is prepended to the string.
	If \a dir is empty, the current working directory is used
	(see addWorking() for details).
	Finally all "/.." are expanded (see expandParent() for details. */
  Str expandedPath( const string &dir="", 
		       char dirsep=DirSep, char homechr=HomeChr ) const;

    /*! Add the extension \a extension to the string.
        A period (\a extsep) is inserted if necessary. */
  Str &addExtension( const string &extension, char extsep=ExtSep );
    /*! Return the string with the extension \a extension added.
        A period (\a extsep) is inserted if necessary. */
  Str addedExtension( const string &extension, char extsep=ExtSep ) const;
    /*! Add the extension \a extension to the string
        provided there is no extension.
        A period (\a extsep) is inserted if necessary. */
  Str &provideExtension( const string &extension, 
			    char dirsep=DirSep, char extsep=ExtSep );
    /*! Return the string with the extension \a extension added
        provided there is no extension.
        A period (\a extsep) is inserted if necessary. */
  Str providedExtension( const string &extension, 
			    char dirsep=DirSep, char extsep=ExtSep ) const;


  // Environment variables:
    /*! Returns the content of the environment variable specified by \a env. 
        If the environment variable \a env does not exist,
        an empty string is returned. */
  static Str getEnvironment( const string &env );
    /*! Set the string to the content of the environment variable 
        specified by \a env.
        If the environment variable \a env does not exist,
        the string is emptied. */
  Str &environment( const string &env );
  
  // LaTeX strings:
  Str latex( void ) const;
  Str latexNum( void ) const;
  Str latexUnit( void ) const;

  // HTML strings:
  Str html( void ) const;
  Str htmlUnit( void ) const;

  // set default formats:
  static int width( void ) { return Width; };
  static void setWidth( int width ) { Width = width > 0 ? width : 0; };

  static int precision( void ) { return Precision; };
  static void setPrecision( int precision ) { Precision = precision > 0 ? precision : 0; };

  static int repeats( void ) { return CharRepeat; };
  static void setRepeats( int repeats ) { CharRepeat = repeats > 0 ? repeats : 0; };

  static char doubleFormat( void ) { return DoubleFormat; };
  static void setDoubleFormat( char format ) { DoubleFormat = format; };

  static char boolFormat( void ) { return BoolFormat; };
  static void setBoolFormat( char format ) { BoolFormat = format; };

  static char pad( void ) { return Pad; };
  static void setPad( char pad ) { Pad = pad; };

  static void setDefault( void );

  static const Str WhiteSpace;
  static const Str DoubleWhiteSpace;
  static const Str BlankSpace;
  static const Str DoubleBlankSpace;
  static const Str WordSpace;
  static const Str DoubleWordSpace;

  static const Str FirstNumber;
  static const Str Number;
  static const Str Digit;

  static const Str LeftBracket;
  static const Str RightBracket;


private:

  static int Width;
  static int Precision;
  static int CharRepeat;
  static char DoubleFormat;
  static char BoolFormat;
  static char Pad;
  static Str Comment;
  static Str Separator;
  static Str Space;
  static Str Bracket;
  static char DirSep;
  static char ExtSep;
  static char HomeChr;
  static string HomeEnv;
  static string WorkingEnv;

  void Construct( const string &s, int width, char pad, bool append=false );
  void Construct( const char *s, int width, char pad, bool append=false );
  void Construct( double val, int width, int precision, char format, 
		  char pad, bool append=false );
  void Construct( signed long val, int width, char pad, bool append=false );
  void Construct( unsigned long val, int width, char pad, bool append=false );
  void Construct( long long val, int width, char pad, bool append=false );
  void Construct( bool b, int width, char format, 
		  char pad, bool append=false );

  void Construct( const string &s, const char *format, bool append=false );
  void Construct( const char *s, const char *format, bool append=false );
  void Construct( char c, const char *format, bool append=false );
  void Construct( double val, const char *format, bool append=false );
  void Construct( long val, const char *format, bool append=false );
  void Construct( long long val, const char *format, bool append=false );

  void Construct( const string &s, const string &format, bool append=false );
  void Construct( const char *s, const string &format, bool append=false );
  void Construct( char c, const string &format, bool append=false );
  void Construct( double val, const string &format, bool append=false );
  void Construct( long val, const string &format, bool append=false );
  void Construct( long long val, const string &format, bool append=false );

  void ReadFormat( const char *format, int &width, int &findex );
};

}; /* namespace relacs */

#endif /* ! _RELACS_STR_H_ */
