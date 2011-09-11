/*
  str.cc
  Advanced string manipulation.

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

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctype.h>
#include <cerrno>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <relacs/str.h>

namespace relacs {


///// global settings: //////////////////////////////////////////////////////

int Str::Width = 0;
int Str::Precision = 5;
int Str::CharRepeat = 1;
char Str::DoubleFormat = 'g';
char Str::BoolFormat = '0';
char Str::Pad = ' ';

const Str Str::BlankSpace = " \t";
const Str Str::DoubleBlankSpace = "  \t";
const Str Str::WhiteSpace = " \t\n\r\v\f";
const Str Str::DoubleWhiteSpace = "  \t\n\r\v\f";
const Str Str::WordSpace = " \t\n\r\v\f`'\",.:;?!";
const Str Str::DoubleWordSpace = "  \t\n\r\v\f`'\",.:;?!";

Str Str::Comment = "";
Str Str::Separator = " \t";
Str Str::Space = Str::WhiteSpace;
Str Str::Bracket = "\"";

char Str::DirSep = '/';
char Str::ExtSep = '.';
char Str::HomeChr = '~';
string Str::HomeEnv = "HOME";
string Str::WorkingEnv = "PWD";

const Str Str::LeftBracket = "([{<`";
const Str Str::RightBracket = ")]}>'";

const Str Str::FirstNumber = "0123456789+-";
const Str Str::Number = "0123456789.+-eE";
const Str Str::Digit = "0123456789";


Str::Str( void )
  : string()
{
}


Str::Str( const Str &s )
  : string( s )
{
}


Str::Str( const string &s )
  : string( s )
{
}


Str::Str( const char *s )
  : string( s )
{
}


Str::Str( char c )
  : string( 1, c )
{
}


///// explicit formatted constructors: ///////////////////////////////////////

void Str::Construct( const string &s, int width, char pad, bool append )
{
  size_type slen = s.size();
  size_type len = abs( width );

  if ( len == 0 )
    append ? string::append( s ) : string::assign( s );
  else if ( len < slen ) {
    if ( width < 0 )
      append ? string::append( s.substr( 0, len ) ) : string::assign( s.substr( 0, len ) );
    else
      append ? string::append( s.substr( slen-len, len ) ) : string::assign( s.substr( slen-len, len ) );
  }
  else {    
    string ss( len, pad );
    if ( width < 0 )
      ss.replace( 0, slen, s );
    else
      ss.replace( len-slen, slen, s );
    append ? string::append( ss ) : string::assign( ss );
  }
}


void Str::Construct( const char *s, int width, char pad, bool append )
{
  size_type slen = strlen( s );
  size_type len = abs( width );

  if ( width == 0 )
    append ? string::append( s ) : string::assign( s );
  else if ( len < slen )
    {
      char ss[len+1];
      if ( width < 0 )
	strncpy( ss, s, len );
      else
	strncpy( ss, s+slen-len, len );
      ss[len] = '\0';
      append ? string::append( ss ) : string::assign( ss );
    }
  else
    {   
      string ss( len, pad );
      if ( width < 0 )
	ss.replace( 0, slen, s );
      else
	ss.replace( len-slen, slen, s );
      append ? string::append( ss ) : string::assign( ss );
    }
}


Str::Str( const string &s, int width, char pad )
{
  Construct( s, width, pad );
}


Str::Str( const char *s, int width, char pad )
{
  Construct( s, width, pad );
}


Str::Str( char c, int len )
  : string( len >= 0 ? len : 0, c )
{
}


Str::Str( double val, int width, int precision, char format, char pad )
{
  Construct( val, width, precision, format, pad );
}


Str::Str( signed long val, int width, char pad )
{
  Construct( val, width, pad );
}


Str::Str( unsigned long val, int width, char pad )
{
  Construct( val, width, pad );
}


Str::Str( signed int val, int width, char pad )
{
  Construct( long( val ), width, pad );
}


Str::Str( unsigned int val, int width, char pad )
{
  Construct( long( val ), width, pad );
}


Str::Str( signed short val, int width, char pad )
{
  Construct( long( val ), width, pad );
}


Str::Str( unsigned short val, int width, char pad )
{
  Construct( long( val ), width, pad );
}


Str::Str( long long val, int width, char pad )
{
  Construct( val, width, pad );
}


Str::Str( bool b, int width, char format, char pad )
{
  Construct( b, width, format, pad );
}


void Str::Construct( double val, int width, int precision, char format, 
		     char pad, bool append )
{
  ostringstream oss( *this, ostringstream::out |
		     ( append ? ostringstream::app : ostringstream::trunc ) );

  if ( format == 'f' || format == 'F' )
    oss.setf( ios::fixed, ios::floatfield );
  else if ( format == 'e' || format == 'E' )
    oss.setf( ios::scientific, ios::floatfield );
  else
    oss.unsetf( ios::floatfield );

  if ( format == 'F' || format == 'E' || format == 'G' )
    oss.setf( ios::uppercase );

  if ( width < 0 )
    oss.setf( ios::left, ios::adjustfield );
  else
    oss.unsetf( ios::adjustfield );

  if ( precision < 0 )
    precision = Precision;

  oss << setfill( pad ) << setw( abs( width ) ) << setprecision( precision )
      << val;
  string::assign( oss.str() );
}


void Str::Construct( signed long val, int width, char pad, bool append )
{
  ostringstream oss( *this, ostringstream::out | 
		     ( append ? ostringstream::app : ostringstream::trunc ) );

  if ( width < 0 )
    oss.setf( ios::left, ios::adjustfield );
  else
    oss.unsetf( ios::adjustfield );

  oss << setfill( pad ) << setw( abs( width ) ) << val;
  string::assign( oss.str() );
}


void Str::Construct( unsigned long val, int width, char pad, bool append )
{
  ostringstream oss( *this, ostringstream::out | 
		     ( append ? ostringstream::app : ostringstream::trunc ) );

  if ( width < 0 )
    oss.setf( ios::left, ios::adjustfield );
  else
    oss.unsetf( ios::adjustfield );

  oss << setfill( pad ) << setw( abs( width ) ) << val;
  string::assign( oss.str() );
}


void Str::Construct( long long val, int width, char pad, bool append )
{
  ostringstream oss( *this, ostringstream::out | 
		     ( append ? ostringstream::app : ostringstream::trunc ) );

  if ( width < 0 )
    oss.setf( ios::left, ios::adjustfield );
  else
    oss.unsetf( ios::adjustfield );

  oss << setfill( pad ) << setw( abs( width ) ) << val;
  string::assign( oss.str() );
}


void Str::Construct( bool b, int width, char format, 
			char pad, bool app )
{
  string ss;

  if ( format == 't' || format == 'f' )
    ss = b ? "true" : "false";
  else if ( format == 'y' || format == 'n' )
    ss = b ? "yes" : "no";
  else
    ss = b ? "1" : "0";

  if ( app )
    append( ss, width, pad );
  else
    assign( ss, width, pad );
}


///// char format constructors: //////////////////////////////////////////////

void Str::Construct( const string &s, const char *format, bool append )
{
  int width;
  int findex;
  ReadFormat( format, width, findex );

  if ( findex < 0 )
    append ? string::append( s ) : string::assign( s );
  else 
    {
      char ss[ strlen( format ) + width + s.size() + 10 ];
      if ( findex == (int)strlen( format ) )
	{
	  char fs[ strlen( format ) + 2 ];
	  strcpy( fs, format );
	  fs[findex] = 's';
	  fs[findex+1] = '\0';
	  sprintf( ss, fs, s.c_str() );
	}
      else if ( format[findex] != 's' )
	{
	  char fs[ strlen( format ) ];
	  strcpy( fs, format );
	  fs[findex] = 's';
	  sprintf( ss, fs, s.c_str() );
	}
      else
	sprintf( ss, format, s.c_str() );
      append ? string::append( ss ) : string::assign( ss );
    }
}


void Str::Construct( const char *s, const char *format, bool append )
{
  int width;
  int findex;
  ReadFormat( format, width, findex );

  if ( findex < 0 )
    append ? string::append( s ) : string::assign( s );
  else 
    {
      char ss[ strlen( format ) + width + strlen( s ) + 10 ];
      if ( findex == (int)strlen( format ) )
	{
	  char fs[ strlen( format ) + 2 ];
	  strcpy( fs, format );
	  fs[findex] = 's';
	  fs[findex+1] = '\0';
	  sprintf( ss, fs, s );
	}
      else if ( format[findex] != 's' )
	{
	  char fs[ strlen( format ) ];
	  strcpy( fs, format );
	  fs[findex] = 's';
	  sprintf( ss, fs, s );
	}
      else
	sprintf( ss, format, s );
      append ? string::append( ss ) : string::assign( ss );
    }
}


void Str::Construct( char c, const char *format, bool append )
{
  int width;
  int findex;
  ReadFormat( format, width, findex );

  if ( findex < 0 )
    append ? string::append( 1, c ) : string::assign( 1, c );
  else 
    {
      char ss[ strlen( format ) + width + 400 ];
      if ( findex == (int)strlen( format ) )
	{
	  char fs[ strlen( format ) + 2 ];
	  strcpy( fs, format );
	  fs[findex] = 'c';
	  fs[findex+1] = '\0';
	  sprintf( ss, fs, c );
	}
      else if ( format[findex] != 'c' )
	{
	  char fs[ strlen( format ) ];
	  strcpy( fs, format );
	  fs[findex] = 'c';
	  sprintf( ss, fs, c );
	}
      else
	sprintf( ss, format, c );
      append ? string::append( ss ) : string::assign( ss );
    }
}


void Str::Construct( double val, const char *format, bool append )
{
  int width;
  int findex;
  ReadFormat( format, width, findex );

  char ss[ strlen( format ) + width + 400 ];
  if ( findex < 0 )
    sprintf( ss, "%g", val );
  else if ( findex == (int)strlen( format ) )
    {
      char fs[ strlen( format ) + 2 ];
      strcpy( fs, format );
      fs[findex] = 'g';
      fs[findex+1] = '\0';
      sprintf( ss, fs, val );
    }
  else if ( strchr( "aAfFgGeE", format[findex] ) == 0 )
    {
      char fs[ strlen( format ) ];
      strcpy( fs, format );
      fs[findex] = 'g';
      sprintf( ss, fs, val );
    }
  else
    sprintf( ss, format, val );
  append ? string::append( ss ) : string::assign( ss );
}


void Str::Construct( long val, const char *format, bool append )
{
  int width;
  int findex;
  ReadFormat( format, width, findex );

  char ss[ strlen( format ) + width + 400 ];
  if ( findex < 0 )
    sprintf( ss, "%ld", val );
  else if ( findex == (int)strlen( format ) )
    {
      char fs[ strlen( format ) + 3 ];
      strcpy( fs, format );
      fs[findex] = 'l';
      fs[findex+1] = 'd';
      fs[findex+2] = '\0';
      sprintf( ss, fs, val );
    }
  else if ( format[findex] != 'l' || 
	    strchr( "diouxX", format[findex+1] ) == 0 )
    {
      char fs[ strlen( format ) + 3 ];
      strcpy( fs, format );
      if ( format[findex] != 'l' )
	{
	  for ( int k=strlen( fs ); k>= findex; k-- )
	    fs[k+1] = fs[k];
	  fs[findex] = 'l';
	  findex++;
	}
      if ( strchr( "diouxX", fs[findex] ) == 0 )
	fs[findex] = 'd';
      sprintf( ss, fs, val );
    }
  else
    sprintf( ss, format, val );
  append ? string::append( ss ) : string::assign( ss );
}


void Str::Construct( long long val, const char *format, bool append )
{
  int width;
  int findex;
  ReadFormat( format, width, findex );

  char ss[ strlen( format ) + width + 400 ];
  if ( findex < 0 )
    sprintf( ss, "%lld", val );
  else if ( findex == (int)strlen( format ) )
    {
      char fs[ strlen( format ) + 4 ];
      strcpy( fs, format );
      fs[findex] = 'l';
      fs[findex+1] = 'l';
      fs[findex+2] = 'd';
      fs[findex+3] = '\0';
      sprintf( ss, fs, val );
    }
  else if ( format[findex] != 'l' || format[findex+1] != 'l' || 
	    strchr( "diouxX", format[findex+2] ) == 0 )
    {
      char fs[ strlen( format ) + 4 ];
      strcpy( fs, format );
      if ( format[findex] != 'l' ) {
	for ( int k=strlen( fs ); k>= findex; k-- )
	  fs[k+1] = fs[k];
	fs[findex] = 'l';
	findex++;
      }
      if ( format[findex] != 'l' ) {
	for ( int k=strlen( fs ); k>= findex; k-- )
	  fs[k+1] = fs[k];
	fs[findex] = 'l';
	findex++;
      }
      if ( strchr( "diouxX", fs[findex] ) == 0 )
	fs[findex] = 'd';
      sprintf( ss, fs, val );
    }
  else
    sprintf( ss, format, val );
  append ? string::append( ss ) : string::assign( ss );
}


Str::Str( const string &s, const char *format )
{
  Construct( s, format );
}


Str::Str( const char *s, const char *format )
{
  Construct( s, format );
}


Str::Str( char c, const char *format )
{
  Construct( c, format );
}


Str::Str( double val, const char *format )
{
  Construct( val, format );
}


Str::Str( signed long val, const char *format ) 
{
  Construct( (long)val, format );
}


Str::Str( unsigned long val, const char *format ) 
{
  Construct( (long)val, format );
}


Str::Str( signed int val, const char *format ) 
{
  Construct( long( val ), format );
}


Str::Str( unsigned int val, const char *format ) 
{
  Construct( long( val ), format );
}


Str::Str( signed short val, const char *format ) 
{
  Construct( long( val ), format );
}


Str::Str( unsigned short val, const char *format ) 
{
  Construct( long( val ), format );
}


Str::Str( long long val, const char *format ) 
{
  Construct( val, format );
}


///// string format constructors: ////////////////////////////////////////////

void Str::Construct( const string &s, const string &format, bool append )
{
  int width;
  int findex;
  ReadFormat( format.c_str(), width, findex );

  if ( findex < 0 )
    append ? string::append( s ) : string::assign( s );
  else 
    {
      char ss[ format.size() + width + s.size() + 10 ];
      if ( findex == (int)format.size() )
	{
	  char fs[ format.size() + 2 ];
	  strcpy( fs, format.c_str() );
	  fs[findex] = 's';
	  fs[findex+1] = '\0';
	  sprintf( ss, fs, s.c_str() );
	}
      else if ( format[findex] != 's' )
	{
	  char fs[ format.size() ];
	  strcpy( fs, format.c_str() );
	  fs[findex] = 's';
	  sprintf( ss, fs, s.c_str() );
	}
      else
	sprintf( ss, format.c_str(), s.c_str() );
      append ? string::append( ss ) : string::assign( ss );
    }
}


void Str::Construct( const char *s, const string &format, bool append )
{
  int width;
  int findex;
  ReadFormat( format.c_str(), width, findex );

  if ( findex < 0 )
    append ? string::append( s ) : string::assign( s );
  else 
    {
      char ss[ format.size() + width + strlen( s ) + 10 ];
      if ( findex == (int)format.size() )
	{
	  char fs[ format.size() + 2 ];
	  strcpy( fs, format.c_str() );
	  fs[findex] = 's';
	  fs[findex+1] = '\0';
	  sprintf( ss, fs, s );
	}
      else if ( format[findex] != 's' )
	{
	  char fs[ format.size() ];
	  strcpy( fs, format.c_str() );
	  fs[findex] = 's';
	  sprintf( ss, fs, s );
	}
      else
	sprintf( ss, format.c_str(), s );
      append ? string::append( ss ) : string::assign( ss );
    }
}


void Str::Construct( char c, const string &format, bool append )
{
  int width;
  int findex;
  ReadFormat( format.c_str(), width, findex );

  if ( findex < 0 )
    append ? string::append( 1, c ) : string::assign( 1, c );
  else 
    {
      char ss[ format.size() + width + 400 ];
      if ( findex == (int)format.size() )
	{
	  char fs[ format.size() + 2 ];
	  strcpy( fs, format.c_str() );
	  fs[findex] = 'c';
	  fs[findex+1] = '\0';
	  sprintf( ss, fs, c );
	}
      else if ( format[findex] != 'c' )
	{
	  char fs[ format.size() ];
	  strcpy( fs, format.c_str() );
	  fs[findex] = 'c';
	  sprintf( ss, fs, c );
	}
      else
	sprintf( ss, format.c_str(), c );
      append ? string::append( ss ) : string::assign( ss );
    }
}


void Str::Construct( double val, const string &format, bool append )
{
  int width;
  int findex;
  ReadFormat( format.c_str(), width, findex );

  char ss[ format.size() + width + 400 ];
  if ( findex < 0 ) {
    sprintf( ss, "%g", val );
  }
  else if ( findex == (int)format.size() ) {
    char fs[ format.size() + 2 ];
    strcpy( fs, format.c_str() );
    fs[findex] = 'g';
    fs[findex+1] = '\0';
    sprintf( ss, fs, val );
  }
  else if ( strchr( "aAfFgGeE", format[findex] ) == 0 ) {
    char fs[ format.size() ];
    strcpy( fs, format.c_str() );
    fs[findex] = 'g';
    sprintf( ss, fs, val );
  }
  else {
    sprintf( ss, format.c_str(), val );
  }
  append ? string::append( ss ) : string::assign( ss );
}


void Str::Construct( long val, const string &format, bool append )
{
  int width;
  int findex;
  ReadFormat( format.c_str(), width, findex );

  char ss[ format.size() + width + 400 ];
  if ( findex < 0 )
    sprintf( ss, "%ld", val );
  else if ( findex == (int)format.size() )
    {
      char fs[ format.size() + 3 ];
      strcpy( fs, format.c_str() );
      fs[findex] = 'l';
      fs[findex+1] = 'd';
      fs[findex+2] = '\0';
      sprintf( ss, fs, val );
    }
  else if ( format[findex] != 'l' || 
	    strchr( "diouxX", format[findex] ) == 0 )
    {
      char fs[ format.size() + 3 ];
      strcpy( fs, format.c_str() );
      if ( format[findex] != 'l' )
	{
	  for ( int k=strlen( fs ); k>= findex; k-- )
	    fs[k+1] = fs[k];
	  fs[findex] = 'l';
	  findex++;
	}
      if ( strchr( "diouxX", fs[findex] ) == 0 )
	fs[findex] = 'd';
      sprintf( ss, fs, val );
    }
  else
    sprintf( ss, format.c_str(), val );
  append ? string::append( ss ) : string::assign( ss );
}


void Str::Construct( long long val, const string &format, bool append )
{
  int width;
  int findex;
  ReadFormat( format.c_str(), width, findex );

  char ss[ format.size() + width + 400 ];
  if ( findex < 0 )
    sprintf( ss, "%lld", val );
  else if ( findex == (int)format.size() )
    {
      char fs[ format.size() + 4 ];
      strcpy( fs, format.c_str() );
      fs[findex] = 'l';
      fs[findex+1] = 'l';
      fs[findex+2] = 'd';
      fs[findex+3] = '\0';
      sprintf( ss, fs, val );
    }
  else if ( format[findex] != 'l' || format[findex+1] != 'l' || 
	    strchr( "diouxX", format[findex+2] ) == 0 )
    {
      char fs[ format.size() + 4 ];
      strcpy( fs, format.c_str() );
      if ( format[findex] != 'l' )
	{
	  for ( int k=strlen( fs ); k>= findex; k-- )
	    fs[k+1] = fs[k];
	  fs[findex] = 'l';
	  findex++;
	}
      if ( format[findex] != 'l' )
	{
	  for ( int k=strlen( fs ); k>= findex; k-- )
	    fs[k+1] = fs[k];
	  fs[findex] = 'l';
	  findex++;
	}
      if ( strchr( "diouxX", fs[findex] ) == 0 )
	fs[findex] = 'd';
      sprintf( ss, fs, val );
    }
  else
    sprintf( ss, format.c_str(), val );
  append ? string::append( ss ) : string::assign( ss );
}


Str::Str( const string &s, const string &format )
{
  Construct( s, format );
}


Str::Str( const char *s, const string &format )
{
  Construct( s, format );
}


Str::Str( char c, const string &format )
{
  Construct( c, format );
}


Str::Str( double val, const string &format )
{
  Construct( val, format );
}


Str::Str( signed long val, const string &format )
{
  Construct( (long)val, format );
}


Str::Str( unsigned long val, const string &format )
{
  Construct( (long)val, format );
}


Str::Str( signed int val, const string &format )
{
  Construct( long( val ), format );
}


Str::Str( unsigned int val, const string &format )
{
  Construct( long( val ), format );
}


Str::Str( signed short val, const string &format )
{
  Construct( long( val ), format );
}


Str::Str( unsigned short val, const string &format )
{
  Construct( long( val ), format );
}


Str::Str( long long val, const string &format )
{
  Construct( val, format );
}


///// assignment /////////////////////////////////////////////////////////////

Str &Str::operator=( const string &s )
{
  return static_cast<Str &>( string::operator=( s ) );
}


Str &Str::operator=( const char *s )
{
  return static_cast<Str &>( string::operator=( s ) );
}


Str &Str::operator=( char c )
{
  return static_cast<Str &>( string::operator=( c ) );
}


const Str &Str::assign( const string &s, int width, char pad )
{
  if ( &s == this )
    return *this;

  Construct( s, width, pad );
  return *this;
}


const Str &Str::assign( const char *s, int width, char pad )
{
  Construct( s, width, pad );
  return *this;
}


const Str &Str::assign( char c, int len )
{
  string::assign( len, c );
  return *this;
}


const Str &Str::assign( double val, int width, int precision, 
			      char format, char pad )
{
  Construct( val, width, precision, format, pad );
  return *this;
}


const Str &Str::assign( signed long val, int width, char pad )
{
  Construct( val, width, pad );
  return *this;
}


const Str &Str::assign( unsigned long val, int width, char pad )
{
  Construct( val, width, pad );
  return *this;
}


const Str &Str::assign( signed int val, int width, char pad )
{
  Construct( long( val ), width, pad );
  return *this;
}


const Str &Str::assign( unsigned int val, int width, char pad )
{
  Construct( long( val ), width, pad );
  return *this;
}


const Str &Str::assign( signed short val, int width, char pad )
{
  Construct( long( val ), width, pad );
  return *this;
}


const Str &Str::assign( unsigned short val, int width, char pad )
{
  Construct( long( val ), width, pad );
  return *this;
}


const Str &Str::assign( long long val, int width, char pad )
{
  Construct( val, width, pad );
  return *this;
}


const Str &Str::assign( bool b, int width, char format, char pad )
{
  Construct( b, width, format, pad );
  return *this;
}


const Str &Str::assign( const string &s, const char *format )
{
  if ( &s == this )
    return *this;

  Construct( s, format );
  return *this;
}


const Str &Str::assign( const char *s, const char *format )
{
  Construct( s, format );
  return *this;
}


const Str &Str::assign( char c, const char *format )
{
  Construct( c, format );
  return *this;
}


const Str &Str::assign( double val, const char *format )
{
  Construct( val, format );
  return *this;
}


const Str &Str::assign( signed long val, const char *format )
{
  Construct( (long)val, format );
  return *this;
}


const Str &Str::assign( unsigned long val, const char *format )
{
  Construct( (long)val, format );
  return *this;
}


const Str &Str::assign( signed int val, const char *format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( unsigned int val, const char *format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( signed short val, const char *format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( unsigned short val, const char *format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( long long val, const char *format )
{
  Construct( val, format );
  return *this;
}


const Str &Str::assign( const string &s, const string &format )
{
  if ( &s == this )
    return *this;

  Construct( s, format );
  return *this;
}


const Str &Str::assign( const char *s, const string &format )
{
  Construct( s, format );
  return *this;
}


const Str &Str::assign( char c, const string &format )
{
  Construct( c, format );
  return *this;
}


const Str &Str::assign( double val, const string &format )
{
  Construct( val, format );
  return *this;
}


const Str &Str::assign( signed long val, const string &format )
{
  Construct( (long)val, format );
  return *this;
}


const Str &Str::assign( unsigned long val, const string &format )
{
  Construct( (long)val, format );
  return *this;
}


const Str &Str::assign( signed int val, const string &format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( unsigned int val, const string &format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( signed short val, const string &format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( unsigned short val, const string &format )
{
  Construct( long( val ), format );
  return *this;
}


const Str &Str::assign( long long val, const string &format )
{
  Construct( val, format );
  return *this;
}


///// append //////////////////////////////////////////////////////////

const Str &Str::append( const string &s, int width, char pad )
{
  if ( &s == this )
    return *this;

  Construct( s, width, pad, true );
  return *this;
}


const Str &Str::append( const char *s, int width, char pad )
{
  Construct( s, width, pad, true );
  return *this;
}


const Str &Str::append( char c, int len )
{
  string::append( len, c );
  return *this;
}


const Str &Str::append( double val, int width, int precision, 
			      char format, char pad )
{
  Construct( val, width, precision, format, pad, true );
  return *this;
}


const Str &Str::append( signed long val, int width, char pad )
{
  Construct( val, width, pad, true );
  return *this;
}


const Str &Str::append( unsigned long val, int width, char pad )
{
  Construct( val, width, pad, true );
  return *this;
}


const Str &Str::append( signed int val, int width, char pad )
{
  Construct( long( val ), width, pad, true );
  return *this;
}


const Str &Str::append( unsigned int val, int width, char pad )
{
  Construct( long( val ), width, pad, true );
  return *this;
}


const Str &Str::append( signed short val, int width, char pad )
{
  Construct( long( val ), width, pad, true );
  return *this;
}


const Str &Str::append( unsigned short val, int width, char pad )
{
  Construct( long( val ), width, pad, true );
  return *this;
}


const Str &Str::append( long long val, int width, char pad )
{
  Construct( val, width, pad, true );
  return *this;
}


const Str &Str::append( bool b, int width, char format, 
			      char pad )
{
  Construct( b, width, format, pad, true );
  return *this;
}


const Str &Str::append( const string &s, const char *format )
{
  if ( &s == this )
    return *this;

  Construct( s, format, true );
  return *this;
}


const Str &Str::append( const string &s, const string &format )
{
  if ( &s == this )
    return *this;

  Construct( s, format, true );
  return *this;
}


///// format ////////////////////////////////////////////////////////////////


int Str::readFormat( int pos, int &width, int &precision, 
			char &type, char &pad )
{
  bool left = false;
  bool readpad = false;
  bool readtype = false;
  width = 0;
  precision = -1;
  type = ' ';
  pad = ' ';

  // skip '%':
  if ( pos < int( size() ) && operator[]( pos ) == '%' )
    pos++;

  // get sign or pad:
  if ( pos < int( size() ) ) {
    if ( operator[]( pos ) == '-' ) {
      left = true;
      pos++;
    }
    else if ( operator[]( pos ) == '+' ) {
      left = false;
      pos++;
    }
    else if ( operator[]( pos ) != '.' && 
	      ( operator[]( pos ) < '1' || operator[]( pos ) > '9' ) ) {
      pad = operator[]( pos );
      readpad = true;
      pos++;
    }
  }

  // read width:
  for ( ; 
	pos < int( size() ) && 
	  ( operator[]( pos ) >= '0' && operator[]( pos ) <= '9' );
	pos++ ) {
    int num = operator[]( pos ) - '0';
    width = 10*width+num;
    readtype = true;
  }

  // no width, but pad:
  if ( readpad && !readtype ) {
    if ( pad != '0' ) {
      type = pad;
      pad = ' ';
      return pos;
    }
    else if ( pos < int( size() ) && operator[]( pos ) == '.' ) {
      pad = ' ';
      width = 0;
      readtype = true;
    }
    else
      readtype = true;
  }

  // read precision:
  if ( pos < int( size() ) && operator[]( pos ) == '.' ) {
    precision = 0;
    bool readprec = false;
    for ( pos++; 
	  pos < int( size() ) && ( operator[]( pos ) >= '0' && operator[]( pos ) <= '9' );
	  pos++ ) {
      int num = operator[]( pos ) - '0';
      precision = 10*precision+num;
      readtype = true;
      readprec = true;
    }
    if ( !readprec )
      precision = -1;
  }

  // format type:
  if ( pos < int( size() ) && readtype ) {
    type = operator[]( pos );
    pos++;
  }
  else {
    type = pad;
    pad = ' ';
  }

  if ( left )
    width = -width;
  
  return pos;
}


int Str::formatWidth( int pos ) const
{
  if ( pos >= size() )
    return 0;

  int fwidth = 0;

  const_iterator p = begin() + pos;

  if ( *p == '%' ) {
    // skip '%':
    ++p;
    
    // get sign or pad:
    if ( p != end() && 
	 ( *p == '-' || *p == '+' || 
	   ( *p != '.' && 
	     ( *p < '1' || *p > '9' ) ) ) ) {
      p++;
    }
    
    // read width of format:
    for ( ; p != end() && isdigit( *p ); p++ ) {
      int num = *p - '0';
      fwidth = 10*fwidth+num;
    }
  }

  return fwidth;
}


int Str::totalWidth( void ) const
{
  int width = 0;

  const_iterator p = begin();
  while ( p != end() ) {

    if ( *p == '%' ) {
      // skip '%':
      ++p;
    
      // get sign or pad:
      if ( p != end() && 
	   ( *p == '-' || *p == '+' || 
	     ( *p != '.' && 
	       ( *p < '1' || *p > '9' ) ) ) ) {
	++p;
      }
      
      // read width of format:
      int fwidth = 0;
      bool nowidth = true;
      for ( ; p != end() && isdigit( *p ); ++p ) {
	int num = *p - '0';
	fwidth = 10*fwidth+num;
	nowidth = false;
      }

      // skip precision:
      if ( p != end() && *p == '.' ) {
	for ( ++p; p != end() && isdigit( *p ); ++p );
      }
      
      // skip format character:
      if ( p != end() ) {
	if ( nowidth ) {
	  char format = *p;
	  if ( strchr( "SMHdmy", format ) != 0 )
	    fwidth = 2;
	  else if ( format == 'Y' )
	    fwidth = 4;
	}
	++p;
      }
      width += fwidth;

    }
    else {
      ++p;
      ++width;
    }
  }

  return width;
}


int Str::format( const string &s, char type, int pos )
{
  for ( int ps=pos; ps < size(); ps++ ) {
    if ( operator[]( ps ) == '%' ) {
      int width = 0;
      int precision = -1;
      char ft = ' ';
      char pad = ' ';
      int np = readFormat( ps, width, precision, ft, pad );
      int n = np - ps;
      if ( ft == type ) {
	Str ss = Str( s, width, pad );
	string::replace( ps, n, ss );
	return ps + ss.size();
      }
      else
	ps += n-1;
    }
  }

  return pos;
}


int Str::format( const char* s, char type, int pos )
{
  for ( int ps=pos; ps < size(); ps++ ) {
    if ( operator[]( ps ) == '%' ) {
      int width = 0;
      int precision = -1;
      char ft = ' ';
      char pad = ' ';
      int np = readFormat( ps, width, precision, ft, pad );
      int n = np - ps;
      if ( ft == type ) {
	Str ss = Str( s, width, pad );
	string::replace( ps, n, ss );
	return ps + ss.size();
      }
      else
	ps += n-1;
    }
  }

  return pos;
}


int Str::format( char c, char type, int pos )
{
  for ( int ps=pos; ps < size(); ps++ ) {
    if ( operator[]( ps ) == '%' ) {
      int width = 0;
      int precision = -1;
      char ft = ' ';
      char pad = ' ';
      int np = readFormat( ps, width, precision, ft, pad );
      int n = np - ps;
      if ( ft == type ) {
	Str ss = Str( c, width );
	string::replace( ps, n, ss );
	return ps + ss.size();
      }
      else
	ps += n-1;
    }
  }

  return pos;
}


int Str::format( double number, char type, char ftype, int pos )
{
  if ( ftype == ' ' )
    ftype = type;

  for ( int ps=pos; ps < size(); ps++ ) {
    if ( operator[]( ps ) == '%' ) {
      int width = 0;
      int precision = -1;
      char ft = ' ';
      char pad = ' ';
      int np = readFormat( ps, width, precision, ft, pad );
      int n = np - ps;
      if ( ft == type ) {
	Str s = Str( number, width, precision, ftype, pad );
	string::replace( ps, n, s );
	return ps + s.size();
      }
      else
	ps += n-1;
    }
  }

  return pos;
}


int Str::format( double number, const string &type, const string &ftype,
		 int pos )
{
  for ( int ps=pos; ps < size(); ps++ ) {
    if ( operator[]( ps ) == '%' ) {
      int width = 0;
      int precision = -1;
      char ft = ' ';
      char pad = ' ';
      int np = readFormat( ps, width, precision, ft, pad );
      int n = np - ps;
      size_t fi = type.find( ft );
      if ( fi != npos ) {
	ft = fi < ftype.size() ? ftype[fi] : type[fi];
	Str s = Str( number, width, precision, ft, pad );
	string::replace( ps, n, s );
	return ps + s.size();
      }
      else
	ps += n-1;
    }
  }

  return pos;
}


int Str::format( long number, char type, char ftype, int pos )
{
  if ( ftype == ' ' )
    ftype = type;

  for ( int ps=pos; ps < size(); ps++ ) {
    if ( operator[]( ps ) == '%' ) {
      int width = 0;
      int precision = -1;
      char ft = ' ';
      char pad = ' ';
      int np = readFormat( ps, width, precision, ft, pad );
      int n = np - ps;
      if ( ft == type ) {
	Str s = Str( number, width, pad );
	string::replace( ps, n, s );
	return ps + s.size();
      }
      else
	ps += n-1;
    }
  }

  return pos;
}


Str &Str::format( const struct tm *t )
{
  for ( int pos=0; pos < size(); pos++ )
    if ( operator[]( pos ) == '%' ) {
      int width = 0;
      int precision = -1;
      char ft = ' ';
      char pad = ' ';
      int np = readFormat( pos, width, precision, ft, pad );
      int n = np - pos;
      switch ( ft ) {
      case 'S' : {
	Str s = Str( t->tm_sec, width, pad );
	string::replace( pos, n, s );
	pos += s.size();
	break; }
      case 'M' : {
	Str s = Str( t->tm_min, width, pad );
	string::replace( pos, n, s );
	pos += s.size();
	break; }
      case 'H' : {
	Str s = Str( t->tm_hour, width, pad );
	string::replace( pos, n, s );
	pos += s.size();
	break; }
      case 'd' : {
	Str s = Str( t->tm_mday, width, pad );
	string::replace( pos, n, s );
	pos += s.size();
	break; }
      case 'm' : {
	Str s = Str( t->tm_mon+1, width, pad );
	string::replace( pos, n, s );
	pos += s.size();
	break; }
      case 'y' : {
	Str s = Str( t->tm_year%100, width, pad );
	string::replace( pos, n, s );
	pos += s.size();
	break; }
      case 'Y' : {
	Str s = Str( t->tm_year+1900, width, pad );
	string::replace( pos, n, s );
	pos += s.size();
	break; }
      default:
	pos++;
      }
      continue;
    }

  return *this;
}


///// read numbers //////////////////////////////////////////////////////////

double Str::number( double dflt, int index, int *next,
		    const string &space ) const
{
  // preset next:
  if ( next != 0 )
    *next = index;

  // double blank?
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // skip leading white space:
  int n = findFirstNot( space, index );

  // nothing:
  if ( n < index )
    return dflt;

  // bracket?
  int bi = LeftBracket.find( operator[]( n ) );
  if ( bi >= 0 ) {
    n++;
    // skip single ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return dflt;
    }
  }

  // no number?
  if ( n >= size() ||
       ( FirstNumber.find( operator[]( n ) ) < 0 &&
	 ( operator[]( n ) != '.' ||
	   n+1 >= size() ||
	   Digit.find( operator[]( n+1 ) ) < 0 ) ) )
    return dflt;

  // convert number:
  errno = 0;
  string nstr( mid( n, findFirst( space, n )-1 ) );
  const char *sp = nstr.c_str();
  char *ep;
  double v = strtod( sp, &ep );

  // failed:
  if ( errno == ERANGE || ep - sp <= 0 )
    return dflt;

  // set index:
  if ( next != 0 )
    *next = n + ( ep - sp );

  return v;
}


double Str::number( double &error, double dflt, int index, int *next,
		    const string &space ) const
{
  // preset next:
  if ( next != 0 )
    *next = index;

  // double blank?
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // skip leading white space:
  int n = findFirstNot( space, index );

  // nothing:
  if ( n < index )
    return dflt;

  // bracket?
  int bi = LeftBracket.find( operator[]( n ) );
  if ( bi >= 0 ) {
    n++;

    // skip single ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return dflt;
    }
  }

  // number?
  if ( n < size() &&
       ( FirstNumber.find( operator[]( n ) ) >= 0 ||
	 ( operator[]( n ) == '.' &&
	   n+1 < size() &&
	   Digit.find( operator[]( n+1 ) ) >= 0 ) ) ) {
    // convert number:
    errno = 0;
    string nstr( mid( n, findFirst( space, n )-1 ) );
    const char *sp = nstr.c_str();
    char *ep;
    double value = strtod( sp, &ep );
    n += ep - sp;

    // failed:
    if ( errno == ERANGE || ep -sp <= 0 )
      return dflt;

    // set index:
    if ( next != 0 )
      *next = n;

    // skip ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return value;
    }

    // skip error:
    if ( n < size() && operator[]( n ) == '+' ) {
      n++;

      // skip '/':
      if ( n < size() && operator[]( n ) == '/' )
	n++;
    
      // skip '-':
      if ( n >= size() || operator[]( n ) != '-' )
	return value;
      n++;
    
      // skip ' ':
      if ( n < size() && operator[]( n ) == ' ' ) {
	if ( db )
	  n++;
	else
	  return value;
      }
    
      // no positive number:
      if ( n >= size() ||
	   ! ( Digit.find( operator[]( n ) ) >= 0 || operator[]( n ) == '.' ) )
	return value;
    
      // convert error:
      errno = 0;
      nstr = mid( n, findFirst( space, n )-1 );
      sp = nstr.c_str();
      double ev = strtod( sp, &ep );
      n += ep - sp;

      // failed:
      if ( errno == ERANGE || ep - sp <= 0 )
	return value;
    
      error = ev;

      // set index:
      if ( next != 0 )
	*next = n;

    }
      
    return value;

  }

  return dflt;
}


double Str::number( double &error, string &unit, 
		    double dflt, int index, int *next,
		    const string &space ) const
{
  double value = dflt;

  // preset next:
  if ( next != 0 )
    *next = index;

  // double blank?
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // skip leading white space:
  int n = findFirstNot( space, index );

  // nothing:
  if ( n < index )
    return dflt;

  // bracket?
  int bi = LeftBracket.find( operator[]( n ) );
  if ( bi >= 0 ) {
    n++;

    // skip single ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return dflt;
    }
  }

  // number?
  if ( n < size() &&
       ( FirstNumber.find( operator[]( n ) ) >= 0 ||
	 ( operator[]( n ) == '.' &&
	   n+1 < size() &&
	   Digit.find( operator[]( n+1 ) ) >= 0 ) ) ) {
    // convert number:
    errno = 0;
    string nstr( mid( n, findFirst( space, n )-1 ) );
    const char *sp = nstr.c_str();
    char *ep;
    value = strtod( sp, &ep );
    n += ep - sp;

    // failed:
    if ( errno == ERANGE || ep -sp <= 0 )
      return dflt;

    // set index:
    if ( next != 0 )
      *next = n;

    // skip ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return value;
    }

    // skip error:
    if ( n < size() && operator[]( n ) == '+' ) {
      n++;

      // skip '/':
      if ( n < size() && operator[]( n ) == '/' )
	n++;
    
      // skip '-':
      if ( n >= size() || operator[]( n ) != '-' )
	return value;
      n++;
    
      // skip ' ':
      if ( n < size() && operator[]( n ) == ' ' ) {
	if ( db )
	  n++;
	else
	  return value;
      }
    
      // no positive number:
      if ( n >= size() ||
	   ! ( Digit.find( operator[]( n ) ) >= 0 || operator[]( n ) == '.' ) )
	return value;
    
      // convert error:
      errno = 0;
      nstr = mid( n, findFirst( space, n )-1 );
      sp = nstr.c_str();
      double ev = strtod( sp, &ep );
      n += ep - sp;

      // failed:
      if ( errno == ERANGE || ep - sp <= 0 )
	return value;

      error = ev;

      // set index:
      if ( next != 0 )
	*next = n;
    
      // skip ' ':
      if ( n < size() && operator[]( n ) == ' ' ) {
	if ( db )
	  n++;
	else
	  return value;
      }
    }
  }
  else if ( bi >= 0 )
    return value;

  // skip bracket:
  if ( bi >= 0 ) {
    if ( n < size() && operator[]( n ) == RightBracket[bi] )
      n++;
    else
      return value;
    // skip ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return value;
    }
  }

  // more space?
  if ( n >= size() || space.find( operator[]( n ) ) != npos )
    return value;

  // the following text is the unit:
  int c = 0;
  for ( c = 0;
        n+c < size() && 
          ( space.find( operator[]( n+c ) ) == npos ||
	    ( db && operator[]( n+c ) == ' ' &&
	      n+c+1 < size() &&  operator[]( n+c+1 ) != ' ' ) );
        c++ );

  // no unit:
  if ( c <= 0 || ( n < size() && ( operator[]( n ) == '.' || operator[]( n ) == ',' ) ) )
    return value;

  // set index:
  if ( next != 0 )
    *next = n + c;

  unit = substr( n, c );

  return value;
}


double Str::error( double dflt, int index, int *next,
		   const string &space ) const
{
  // preset next:
  if ( next != 0 )
    *next = index;

  // double blank?
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // find number:
  int n;
  number( 0.0, index, &n, space );

  // failed:
  if ( n <= index )
    return dflt;

  // skip ' ':
  if ( n < size() && operator[]( n ) == ' ' ) {
    if ( db )
      n++;
    else
      return dflt;
  }

  // skip '+':
  if ( n >= size() || operator[]( n ) != '+' )
    return dflt;
  n++;

  // skip '/':
  if ( n < size() && operator[]( n ) == '/' )
    n++;

  // skip '-':
  if ( n >= size() || operator[]( n ) != '-' )
    return dflt;
  n++;

  // skip ' ':
  if ( n < size() && operator[]( n ) == ' ' ) {
    if ( db )
      n++;
    else
      return dflt;
  }

  // no positive number:
  if ( n >= size() ||
       ! ( Digit.find( operator[]( n ) ) >= 0 || operator[]( n ) == '.' ) )
    return dflt;

  // convert:
  errno = 0;
  string nstr( mid( n, findFirst( space, n )-1 ) );
  const char *sp = nstr.c_str();
  char *ep;
  double v = strtod( sp, &ep );

  // failed:
  if ( errno == ERANGE || ep - sp <= 0 )
    return dflt;

  // set index:
  if ( next != 0 )
    *next = n + ( ep - sp );

  return v;
}


Str Str::unit( const string &dflt, int index, int *next,
	       const string &space ) const
{
  // preset next:
  if ( next != 0 )
    *next = index;

  // double blank?
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // skip leading white space:
  int n = findFirstNot( space, index );

  // nothing:
  if ( n < index )
    return dflt;

  // bracket?
  int bi = LeftBracket.find( operator[]( n ) );
  if ( bi >= 0 ) {
    n++;

    // skip single ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return dflt;
    }
  }

  // number?
  if ( n < size() &&
       ( FirstNumber.find( operator[]( n ) ) >= 0 ||
	 ( operator[]( n ) == '.' &&
	   n+1 < size() &&
	   Digit.find( operator[]( n+1 ) ) >= 0 ) ) ) {
    // convert number:
    errno = 0;
    string nstr( mid( n, findFirst( space, n )-1 ) );
    const char *sp = nstr.c_str();
    char *ep;
    strtod( sp, &ep );
    n += ep - sp;

    // failed:
    if ( errno == ERANGE || ep -sp <= 0 )
      return dflt;

    // skip ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return dflt;
    }

    // skip error:
    if ( n < size() && operator[]( n ) == '+' ) {
      n++;

      // skip '/':
      if ( n < size() && operator[]( n ) == '/' )
	n++;
    
      // skip '-':
      if ( n >= size() || operator[]( n ) != '-' )
	return dflt;
      n++;
    
      // skip ' ':
      if ( n < size() && operator[]( n ) == ' ' ) {
	if ( db )
	  n++;
	else
	  return dflt;
      }
    
      // no positive number:
      if ( n >= size() ||
	   ! ( Digit.find( operator[]( n ) ) >= 0 || operator[]( n ) == '.' ) )
	return dflt;
    
      // convert error:
      errno = 0;
      nstr = mid( n, findFirst( space, n )-1 );
      sp = nstr.c_str();
      strtod( sp, &ep );
      n += ep - sp;

      // failed:
      if ( errno == ERANGE || ep - sp <= 0 )
	return dflt;
    
      // skip ' ':
      if ( n < size() && operator[]( n ) == ' ' ) {
	if ( db )
	  n++;
	else
	  return dflt;
      }
    }
  }
  else if ( bi >= 0 )
    return dflt;

  // skip bracket:
  if ( bi >= 0 ) {
    if ( n < size() && operator[]( n ) == RightBracket[bi] )
      n++;
    else
      return dflt;
    // skip ' ':
    if ( n < size() && operator[]( n ) == ' ' ) {
      if ( db )
	n++;
      else
	return dflt;
    }
  }

  // more space?
  if ( n >= size() || space.find( operator[]( n ) ) != npos )
    return dflt;

  // the following text is the unit:
  int c;
  for ( c = 0;
        n+c < size() && 
          ( space.find( operator[]( n+c ) ) == npos ||
	    ( db && operator[]( n+c ) == ' ' &&
	      n+c+1 < size() &&  operator[]( n+c+1 ) != ' ' ) );
        c++ );

  // no unit:
  if ( c <= 0 )
    return dflt;

  // set index:
  if ( next != 0 )
    *next = n + c;

  return substr( n, c );
}


///// read date/time ////////////////////////////////////////////////////////

int Str::date( int &year, int &month, int &day ) const
{
  year = 0;
  month = 0;
  day = 0;

  const_iterator p = begin();

  // first number:
  char numstr[100];
  int n = 0;
  while ( p != end() && ( isdigit( *p ) || ( n==0 && *p == '-' ) ) ) {
    numstr[n++] = *p;
    ++p;
  }
  numstr[n] = '\0';
  bool longdate = false;
  bool yearfirst = true;
  if ( n == 0 ) {
    // May 15, 2008:
    longdate = true;
    yearfirst = false;
    int n = 0;
    while ( p != end() && isalpha( *p ) ) {
      numstr[n++] = ::tolower( *p );
      ++p;
    }
    numstr[n] = '\0';
    if ( n < 3 )
      return -1;
    numstr[3] = '\0';
    const char monthstr[12][4] = { "jan", "feb", "mar", "apr", "may", "jun",
				   "jul", "aug", "sep", "oct", "nov", "dec" };
    int m = 0;
    for ( ; m<12; m++ ) {
      if  ( strcmp( numstr, monthstr[m] ) == 0 )
	break;
    }
    if ( m < 12 )
      month = m+1;
    else
      return -1;
    if ( *p != ' ' )
      return -1;
    ++p;
  }
  else {
    // 2008-05-15 or 05/15/2008:
    int num = strtol( numstr, 0, 10 );
    if ( n <= 2 && num > 0 && *p == '/' )
      yearfirst = false;
    if ( *p != '/' && *p != '-' )
      return -1;
    ++p;
    if ( yearfirst )
      year = num;
    else
      month = num;
  }

  // second number:
  n = 0;
  while ( p != end() && isdigit( *p ) ) {
    numstr[n++] = *p;
    ++p;
  }
  numstr[n] = '\0';
  if ( n == 0 || n > 2 )
    return -1;
  int num = strtol( numstr, 0, 10 );
  if ( yearfirst )
    month = num;
  else
    day = num;
  if ( longdate ) {
    if ( *p != ',' )
      return -1;
    ++p;
    while ( *p == ' ' )
      ++p;
  }
  else {
    if ( *p != '/' && *p != '-' )
      return -1;
    ++p;
  }
  
  // third number:
  n = 0;
  while ( p != end() && isdigit( *p ) ) {
    numstr[n++] = *p;
    ++p;
  }
  numstr[n] = '\0';
  if ( n == 0 )
    return -1;
  num = strtol( numstr, 0, 10 );
  if ( yearfirst )
    day = num;
  else
    year = num;

  // check ranges:
  if ( month < 1 || month > 12 )
    return -4;
  if ( day < 1 || day > 31 )
    return -8;

  return 0;
}


int Str::time( int &hour, int &minutes, int &seconds ) const
{
  hour = 0;
  minutes = 0;
  seconds = 0;

  const_iterator p = begin();

  // first number:
  char numstr[100];
  int n = 0;
  while ( p != end() && isdigit( *p ) ) {
    numstr[n++] = *p;
    ++p;
  }
  numstr[n] = '\0';
  if ( n == 0 )
    return -1;
  hour = strtol( numstr, 0, 10 );
  if ( *p != ':' )
    return -1;
  ++p;

  // second number:
  n = 0;
  while ( p != end() && isdigit( *p ) ) {
    numstr[n++] = *p;
    ++p;
  }
  numstr[n] = '\0';
  if ( n == 0 )
    return -1;
  minutes = strtol( numstr, 0, 10 );
  if ( *p != ':' )
    return -1;
  ++p;
  
  // third number:
  n = 0;
  while ( p != end() && isdigit( *p ) ) {
    numstr[n++] = *p;
    ++p;
  }
  numstr[n] = '\0';
  if ( n == 0 )
    return -1;
  seconds = strtol( numstr, 0, 10 );

  // check ranges:
  if ( hour < 0 || hour > 24 )
    return -2;
  if ( minutes < 0 || minutes > 59 )
    return -4;
  if ( seconds < 0 || seconds > 59 )
    return -8;

  return 0;
}


///// find value of identifier //////////////////////////////////////////////

int Str::ident( const string &search,
		const string &space ) const
{
  int word = 0;
  bool line = false;
  int cs = false;

  unsigned int f;
  for ( f=0; 
	f<search.size() && ( search[ f ] == '!' || search[ f ] == '^' ); 
	f++ )
    if ( search[ f ] == '!' ) {
      if ( word & 1 )
	line = true;
      else
	word |= 1;
    }
    else if ( search[ f ] == '^' ) {
      cs = true;
    }

  int n = search.size();
  if ( n > 0 && search[ n-1 ] == '!' ) {
    word |= 2;
    n--;
  }
  n -= f;

  if ( n <= 0 )
    return -1;

  string s( search, f, n );
  int i = find( s, 0, cs, word, space );
  if ( line ) {
    int c = first();
    if ( c < 0 || i > c )
      return -1;
  }

  return i;
}


int Str::value( const string &search,
		const string &space ) const
{
  int i = ident( search, space );
  
  if ( i < 0 )
    return -1;

  // count control characters:
  unsigned int n;
  for ( n=0; 
	n<search.size() && ( search[ n ] == '!' || search[ n ] == '^' ); 
	n++ );
  if ( search.size() > 0 && search[ search.size()-1 ] == '!' )
    n++;

  i += search.size() - n;

  i = findFirst( ":=", i );

  if ( i < 0 )
    return -1;

  i = findFirstNot( space, i+1 );

  return i;
}


Str Str::text( const string &search, const string &dflt="",
	       const string &space ) const
{
  int i = value( search, space );

  if ( i < 0 )
    return dflt;

  // double blank?
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // find end of text:
  int e = findFirst( space, i );
  if ( e < 0 )
    e = size();
  else if ( db ) {
    while ( operator[]( e ) == ' ' &&
	    e+1 < size() &&
	    operator[]( e+1 ) != ' ' ) {
      e = findFirst( space, e+1 );
      if ( e < 0 ) {
	e = size();
	break;
      }
    }
  }

  if ( e-i <= 0 )
    return dflt;

  return substr( i, e-i );
}


double Str::number( const string &search, double dflt,
		    const string &space ) const
{
  int i = value( search, space );

  if ( i < 0 )
    return dflt;

  return number( dflt, i, 0, space );  
}


double Str::error( const string &search, double dflt,
		   const string &space ) const
{
  int i = value( search, space );

  if ( i < 0 )
    return dflt;

  return error( dflt, i, 0, space );  
}


Str Str::unit( const string &search, const string &dflt,
		  const string &space ) const
{
  int i = value( search, space );

  if ( i < 0 )
    return dflt;

  return unit( dflt, i, 0, space );  
}


Str Str::ident( int index, const string &a,
		const string &space ) const
{
  int ai = findFirst( a );
  if ( ai < 0 )
    ai = size();

  int f = findFirstNot( space );
  int l = findLastNot( space, ai-1 );

  if ( f < 0 || l < 0 || l < f )
    return "";

  return substr( f, l-f+1 );
}


Str Str::value( int index, const string &a,
		const string &space ) const
{
  int ai = findFirst( a );
  if ( ai < 0 )
    return "";

  int f = findFirstNot( space, ai+1 );
  int l = findLastNot( space );

  if ( f < 0 || l < 0 || l < f )
    return "";

  return substr( f, l-f+1 );
}


void Str::range( vector< int > &ri, const string &sep, const string &r ) const
{
  ri.clear();

  string space = sep + r;
  
  int next;
  int index = 0;
  do {
    double val1 = number( 0.0, index, &next, space );
    if ( next <= index )
      break;
    index = next;
    if ( substr( next, r.size() ) == r ) {
      index += r.size();
      double val2 = number( 0.0, index, &next, space );
      if ( next <= index )
	break;
      index = next;
      for ( double val=val1; val <= val2; val += 1.0 )
	ri.push_back( static_cast< int >( val ) );
    }
    else if ( substr( next, sep.size() ) == sep ) {
      index += sep.size();
      ri.push_back( static_cast< int >( val1 ) );
    }
    else {
      ri.push_back( static_cast< int >( val1 ) );
      break;
    }
  } while ( true );
}



///// find //////////////////////////////////////////////////////////////////

int Str::find( const string &s, int index, bool cs, 
	       int word, const string &space ) const
{ 
  if ( index < 0 || index >= size() || s.size() == 0 )
    return -1;

  const string *fp;
  const string *sp;

  Str *fs = 0;
  Str *ss = 0;

  if ( cs ) {
    fp = &s;
    sp = this;
  }
  else {
    fs = new Str( s, 0 );
    fs->lower();
    fp = fs;
    ss = new Str( *this, 0 );
    ss->lower();
    sp = ss;
  }

  // find string:
  size_type p = sp->find( *fp, index ); 

  // check for word:
  if ( (word & 3) > 0 && space.size() > 0 ) {
    int l = fp->size();
    for ( ; p != npos; ) {
      bool b = true;
      // is fp found at the beginning of a word?
      if ( (word & 1) > 0 ) {
	if ( p > 0 && 
	     space.find( sp->at( p-1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p > 1 && 
		  sp->at( p-1 ) == ' ' &&
		  sp->at( p-2 ) != ' ' )
	  b = false;
      }
      // is fp found at the end of a word?
      if ( (word & 2) > 0 ) {
	if ( p + l < sp->size() && 
	     space.find( sp->at( p + l ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p + l + 1 < sp->size() && 
		  sp->at( p + l ) == ' ' &&
		  sp->at( p + l + 1 ) != ' ' )
		b = false;
      }
      if ( b )
	break;
      // search for next fp:
      p = sp->find( *fp, p+1 ); 
    }
  }
  
  if ( fs != 0 )
    delete fs;
  if ( ss != 0 )
    delete ss;
  
  return p == npos ? -1 : (int)p; 
}


int Str::find( const char *s, int index, bool cs, 
		  int word, const string &space ) const
{
  if ( index < 0 || index >= size() || strlen( s ) == 0 )
    return -1;

  const char *fp;
  const string *sp;

  char *fs = 0;
  Str *ss = 0;

  if ( cs ) {
    fp = s;
    sp = this;
  }
  else {
    fs = new char[ strlen( s ) + 1 ];
    strcpy( fs, s );
    for ( char *p = fs; *p != '\0'; p++ )
      *p = tolower( *p );
    fp = fs;
    ss = new Str( *this, 0 );
    ss->lower();
    sp = ss;
  }
  
  // find string:
  size_type p = sp->find( fp, index ); 
  
  // check for word:
  if ( (word & 3) > 0 && space.size() > 0 ) {
    int l = strlen( fp );
    for ( ; p != npos; ) {
      bool b = true;
      // is fp found at the beginning of a word?
      if ( (word & 1) > 0 ) {
	if ( p > 0 && 
	     space.find( sp->at( p-1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p > 1 && 
		  sp->at( p-1 ) == ' ' &&
		  sp->at( p-2 ) != ' ' )
	  b = false;
      }
      // is fp found at the end of a word?
      if ( (word & 2) > 0 ) {
	if ( p + l < sp->size() && 
	     space.find( sp->at( p + l ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p + l + 1 < sp->size() && 
		  sp->at( p + l ) == ' ' &&
		  sp->at( p + l + 1 ) != ' ' )
	  b = false;
      }
      if ( b )
	break;
      // search for next fp:
      p = sp->find( fp, p+1 ); 
    }
  }
  
  if ( fs != 0 )
    delete [] fs;
  if ( ss != 0 )
    delete ss;
  
  return p == npos ? -1 : (int)p; 
}


int Str::find( char c, int index, bool cs, 
		  int word, const string &space ) const
{ 
  if ( index < 0 || index >= size() )
    return -1;

  char fc;
  const string *sp;

  Str *ss = 0;

  if ( cs ) {
    fc = c;
    sp = this;
  }
  else {
    fc = tolower( c );
    ss = new Str( *this, 0 );
    ss->lower();
    sp = ss;
  }
  
  // find character:
  size_type p = sp->find( fc, index ); 
  
  // check for word:
  if ( (word & 3) > 0 && space.size() > 0 ) {
    for ( ; p != npos; ) {
      bool b = true;
      // is fc found at the beginning of a word?
      if ( (word & 1) > 0 ) {
	if ( p > 0 && 
	     space.find( sp->at( p-1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p > 1 && 
		  sp->at( p-1 ) == ' ' &&
		  sp->at( p-2 ) != ' ' )
	  b = false;
      }
      // is fc found at the end of a word?
      if ( (word & 2) > 0 ) {
	if ( p + 1 < sp->size() && 
	     space.find( sp->at( p + 1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p + 2 < sp->size() && 
		  sp->at( p + 1 ) == ' ' &&
		  sp->at( p + 2 ) != ' ' )
	  b = false;
      }
      if ( b )
	break;
      // search for next fc:
      p = sp->find( fc, p+1 ); 
    }
  }
  
  if ( ss != 0 )
    delete ss;
  
  return p == npos ? -1 : (int)p; 
}


int Str::rfind( const string &s, int index, bool cs, 
		   int word, const string &space ) const
{ 
  if ( index == -1 || s.size() == 0 )
    return -1;

  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  const string *fp;
  const string *sp;

  Str *fs = 0;
  Str *ss= 0;

  if ( cs ) {
    fp = &s;
    sp = this;
  }
  else {
    fs = new Str( s, 0 );
    fs->lower();
    fp = fs;
    ss = new Str( *this, 0 );
    ss->lower();
    sp = ss;
  }

  // find string:
  size_type p = sp->rfind( *fp, index ); 

  // check for word:
  if ( (word & 3) > 0 && space.size() > 0 ) {
    int l = fp->size();
    for ( ; p != npos; ) {
      bool b = true;
      // is fp found at the beginning of a word?
      if ( (word & 1) > 0 ) {
	if ( p > 0 && 
	     space.find( sp->at( p-1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p > 1 && 
		  sp->at( p-1 ) == ' ' &&
		  sp->at( p-2 ) != ' ' )
	  b = false;
      }
      // is fp found at the end of a word?
      if ( (word & 2) > 0 ) {
	if ( p + l < sp->size() && 
	     space.find( sp->at( p + l ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p + l + 1 < sp->size() && 
		  sp->at( p + l ) == ' ' &&
		  sp->at( p + l + 1 ) != ' ' )
	  b = false;
      }
      if ( b )
	break;
      // search for next fp:
      if ( p <= 0 ) {
	p = npos;
	break;
      }
      p = sp->rfind( *fp, p-1 ); 
    }
  }
  
  if ( fs != 0 )
    delete fs;
  if ( ss != 0 )
    delete ss;
  
  return p == npos ? -1 : (int)p; 
}


int Str::rfind( const char *s, int index, bool cs, 
		   int word, const string &space ) const
{ 
  if ( index == -1 || strlen( s ) == 0 )
    return -1;

  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  const char *fp;
  const string *sp;

  char *fs = 0;
  Str *ss = 0;

  if ( cs ) {
    fp = s;
    sp = this;
  }
  else {
    fs = new char[ strlen( s ) + 1 ];
    strcpy( fs, s );
    for ( char *p = fs; *p != '\0'; p++ )
      *p = tolower( *p );
    fp = fs;
    ss = new Str( *this, 0 );
    ss->lower();
    sp = ss;
  }
  
  // find string:
  size_type p = sp->rfind( fp, index ); 
  
  // check for word:
  if ( (word & 3) > 0 && space.size() > 0 ) {
    int l = strlen( fp );
    for ( ; p != npos; ) {
      bool b = true;
      // is fp found at the beginning of a word?
      if ( (word & 1) > 0 ) {
	if ( p > 0 && 
	     space.find( sp->at( p-1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p > 1 && 
		  sp->at( p-1 ) == ' ' &&
		  sp->at( p-2 ) != ' ' )
	  b = false;
      }
      // is fp found at the end of a word?
      if ( (word & 2) > 0 ) {
	if ( p + l < sp->size() && 
	     space.find( sp->at( p + l ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p + l + 1 < sp->size() && 
		  sp->at( p + l ) == ' ' &&
		  sp->at( p + l + 1 ) != ' ' )
	  b = false;
      }
      if ( b )
	break;
      // search for next fp:
      if ( p <= 0 ) {
	p = npos;
	break;
      }
      p = sp->rfind( fp, p-1 ); 
    }
  }
  
  if ( fs !=  0 )
    delete [] fs;
  if ( ss != 0 )
    delete ss;
  
  return p == npos ? -1 : (int)p; 
}


int Str::rfind( char c, int index, bool cs, 
		   int word, const string &space ) const
{ 
  if ( index == -1 )
    return -1;

  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  char fc;
  const string *sp;

  Str *ss = 0;

  if ( cs ) {
    fc = c;
    sp = this;
  }
  else {
    fc = tolower( c );
    ss = new Str( *this, 0 );
    ss->lower();
    sp = ss;
  }
  
  // find character:
  size_type p = sp->rfind( fc, index ); 
  
  // check for word:
  if ( (word & 3) > 0 && space.size() > 0 ) {
    for ( ; p != npos; ) {
      bool b = true;
      // is fc found at the beginning of a word?
      if ( (word & 1) > 0 ) {
	if ( p > 0 && 
	     space.find( sp->at( p-1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p > 1 && 
		  sp->at( p-1 ) == ' ' &&
		  sp->at( p-2 ) != ' ' )
	  b = false;
      }
      // is fc found at the end of a word?
      if ( (word & 2) > 0 ) {
	if ( p + 1 < sp->size() && 
	     space.find( sp->at( p + 1 ) ) == npos )
	  b = false;
	else if ( space.find( "  " ) != npos &&
		  p + 2 < sp->size() && 
		  sp->at( p + 1 ) == ' ' &&
			sp->at( p + 2 ) != ' ' )
	  b = false;
      }
      if ( b )
	break;
      // search for next fc:
      if ( p <= 0 ) {
	p = npos;
	break;
      }
      p = sp->rfind( fc, p-1 ); 
    }
  }
  
  if ( ss != 0 )
    delete ss;
  
  return p == npos ? -1 : (int)p; 
}


int Str::findFirst( const string &s, int index ) const
{ 
  if ( s.size() == 0 )
    return -1;
  size_type p = string::find_first_of( s, index ); 
  return p == npos ? -1 : (int)p; 
}


int Str::findFirst( const char *s, int index ) const
{ 
  if ( s[0] == '\0' )
    return -1;
  size_type p = string::find_first_of( s, index ); 
  return p == npos ? -1 : (int)p; 
}


int Str::findFirst( char c, int index ) const
{ 
  size_type p = string::find_first_of( c, index ); 
  return p == npos ? -1 : (int)p; 
}


int Str::findFirstNot( const string &s, int index ) const
{ 
  if ( s.size() == 0 )
    return size() > index ? index : -1;
  size_type p = string::find_first_not_of( s, index ); 
  return p == npos ? -1 : (int)p; 
}


int Str::findFirstNot( const char *s, int index ) const
{ 
  if ( s[0] == '\0' )
    return size() > index ? index : -1;
  size_type p = string::find_first_not_of( s, index ); 
  return p == npos ? -1 : (int)p; 
}


int Str::findFirstNot( char c, int index ) const
{ 
  size_type p = string::find_first_not_of( c, index ); 
  return p == npos ? -1 : (int)p; 
}


int Str::findLast( const string &s, int index ) const
{
  if ( index == -1 || s.size() == 0 )
    return -1;
 
  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  size_type p = string::find_last_of( s, index ); 

  return p == npos ? -1 : (int)p; 
}


int Str::findLast( const char *s, int index ) const
{ 
  if ( index == -1 || s[0] == '\0' )
    return -1;
 
  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  size_type p = string::find_last_of( s, index ); 

  return p == npos ? -1 : (int)p; 
}


int Str::findLast( char c, int index ) const
{ 
  if ( index == -1 )
    return -1;
 
  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  size_type p = string::find_last_of( c, index ); 

  return p == npos ? -1 : (int)p; 
}


int Str::findLastNot( const string &s, int index ) const
{ 
  if ( index == -1 )
    return -1;
 
  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  if ( s.size() == 0 )
    return size() > index ? index : -1;

  size_type p = string::find_last_not_of( s, index ); 

  return p == npos ? -1 : (int)p; 
}


int Str::findLastNot( const char *s, int index ) const
{ 
  if ( index == -1 )
    return -1;
 
  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  if (  s[0] == '\0' )
    return size() > index ? index : -1;

  size_type p = string::find_last_not_of( s, index ); 

  return p == npos ? -1 : (int)p; 
}


int Str::findLastNot( char c, int index ) const
{ 
  if ( index == -1 )
    return -1;
 
  if ( index < 0 || index >= size() ) 
    index = size()-1; 

  size_type p = string::find_last_not_of( c, index ); 

  return p == npos ? -1 : (int)p; 
}


///// contains ////////////////////////////////////////////////////////////////

int Str::contains( const string &s, int index, bool cs, 
		   int word, const string &space, 
		   const string &comment ) const
{
  // validate index:
  if ( index < 0 )
    return 0;

  // position of comment:
  int c;
  if ( comment.size() == 0 )
    c = size();
  else {
    c = string::find( comment, index );
    if ( c < 0 )
      c = size();
  }

  int k;
  for ( k=0; ; k++ ) {
    index = find( s, index, cs, word, space );
    if ( index < 0 || index >= c )
      break;
    index++;
  }

  return k;
}


int Str::contains( const char *s, int index, bool cs, 
		   int word, const string &space, 
		   const string &comment ) const
{
  // validate index:
  if ( index < 0 )
    return 0;

  // position of comment:
  int c;
  if ( comment.size() == 0 )
    c = size();
  else {
    c = string::find( comment, index );
    if ( c < 0 )
      c = size();
  }

  int k;
  for ( k=0; ; k++ ) {
    index = find( s, index, cs, word, space );
    if ( index < 0 || index >= c )
      break;
    index++;
  }

  return k;
}


int Str::contains( char ch, int index, bool cs, 
		   int word, const string &space, 
		   const string &comment ) const
{
  // validate index:
  if ( index < 0 )
    return 0;

  // position of comment:
  int c;
  if ( comment.size() == 0 )
    c = size(); 
  else {
    c = find( comment, index, cs, word, space );
    if ( c < 0 )
      c = size();
  }

  int k;
  for ( k=0; ; k++ ) {
    index = find( ch, index, cs, word, space );
    if ( index < 0 || index >= c )
      break;
    index++;
  }

  return k;
}


///// brackets ////////////////////////////////////////////////////////////////

int Str::findBracket( int index, const string &brackets, 
		      const string &comment ) const
{
  // no valid index:
  if ( index < 0 || index + 1 >= size() )
    return -1;

  // not an open bracket:
  char open = at( index );
  if ( brackets.find( open ) == npos )
    return -1;

  // corresponding closing bracket:
  int k = LeftBracket.find( open );
  char close = k<0 ? open : RightBracket[k];

  // find brackets:
  for ( ; ; ) {
    index++;
    int c = find( close, index );
    int o = findFirst( brackets, index );
    int l = find( comment, index );
    
    // no closing bracket in the string:
    if ( c < 0 || ( l >= 0 && c >= l ) )
      return -1;
      
    // another opening bracket:
    if ( o >= 0 && o < c ) {
      index = findBracket( o, brackets );
      if ( index < 0 )
	return -1;
    }
    else
      return c;
  }
}


Str &Str::stripBracket( const string &brackets, 
			      const string &comment )
{
  if ( comment.size() > 0 ) {
    int l = find( comment );
    if ( l >= 0 )
      string::erase( l, size()-l );
  }

  int o = findFirst( brackets );
  if ( o < 0 ) 
    return *this;

  int c = findBracket( o, brackets, "" );

  string::erase( 0, o + 1 );
  if ( c > o )
    string::resize( c - o - 1 );

  return *this;
}


Str Str::strippedBracket( const string &brackets, 
				const string &comment ) const
{
  Str s( *this );
  return s.stripBracket( brackets, comment );
}


///// erase /////////////////////////////////////////////////////////////////

int Str::erase( const string &s )
{
  if ( s.size() == 0 )
    return 0;

  int n = 0;
  for ( size_type p = string::find( s ); 
	p != npos; 
	p = string::find( s, p ) ) {
    string::erase( p, s.size() );
    n++;
  }

  return n;
}


Str Str::erased( const string &s ) const
{
  if ( s.size() == 0 )
    return *this;

  string ss = *this;

  for ( size_type p = ss.find( s ); 
	p != npos; 
	p = ss.find( s, p ) ) {
    ss.erase( p, s.size() );
  }

  return ss;
}


int Str::erase( const string &s, int index, bool cs, 
		int word, const string &space )
{
  if ( s.empty() )
    return 0;

  int n = 0;
  for ( int p = find( s, index, cs, word, space ); 
	p >= 0; 
	p = find( s, p, cs, word, space ) ) {
    string::erase( p, s.size() );
    n++;
  }

  return n;
}


bool Str::eraseFirst( const string &s )
{
  if ( s.empty() )
    return false;

  size_type ns = string::find_first_not_of( s ); 
  if ( ns == npos )
    return false;

  if ( string::find( s ) == ns ) {
    string::erase( ns, s.size() );
    return true;
  }

  return false;
}


Str Str::erasedFirst( const string &s ) const
{
  if ( s.empty() )
    return *this;

  size_type ns = string::find_first_not_of( s ); 
  if ( ns == npos )
    return *this;

  if ( string::find( s ) == ns ) {
    string ss = *this;
    ss.erase( ns, s.size() );
    return ss;
  }

  return *this;
}


bool Str::eraseFirst( const string &s, int index, bool cs, 
		      int word, const string &space )
{
  if ( s.empty() )
    return false;

  int ns = findFirstNot( space, index );
  if ( ns < 0 )
    return false;

  if ( find( s, index, cs, word, space ) == ns ) {
    string::erase( ns, s.size() );
    return true;
  }

  return false;
}


int Str::replace( const string &s, const string &rs )
{
  if ( s.size() == 0 )
    return 0;

  int n = 0;
  for ( size_type p = string::find( s ); 
	p != npos; 
	p = string::find( s, p ) ) {
    string::replace( p, s.size(), rs );
    p += rs.size();
    n++;
  }

  return n;
}


int Str::replace( const string &s, const string &rs, int index, bool cs, 
		  int word, const string &space )
{
  if ( s.empty() )
    return 0;

  int n = 0;
  for ( int p = find( s, index, cs, word, space ); 
	p >= 0; 
	p = find( s, p, cs, word, space ) ) {
    string::replace( p, s.size(), rs );
    p += rs.size();
    n++;
  }

  return n;
}


Str Str::replaced( const string &s, const string &rs ) const
{
  if ( s.size() == 0 )
    return *this;

  string ss = *this;

  for ( size_type p = ss.find( s ); 
	p != npos; 
	p = ss.find( s, p ) ) {
    ss.replace( p, s.size(), rs );
    p += rs.size();
  }

  return ss;
}


int Str::eraseMarkup( void )
{
  int n = 0;
  n += erase( "<p>" );
  n += replace( "<br>", " " );
  n += erase( "<b>" );
  n += erase( "</b>" );
  n += erase( "<i>" );
  n += erase( "</i>" );
  n += erase( "<h1>" );
  n += erase( "</h1>" );
  n += erase( "<h2>" );
  n += erase( "</h2>" );
  n += erase( "<ul>" );
  n += erase( "</ul>" );
  n += replace( "<li>", " - " );
  n += erase( "</li>" );
  return n;
}


Str Str::erasedMarkup( void ) const
{
  Str s( *this );
  s.erase( "<p>" );
  s.replace( "<br>", " " );
  s.erase( "<b>" );
  s.erase( "</b>" );
  s.erase( "<i>" );
  s.erase( "</i>" );
  s.erase( "<h1>" );
  s.erase( "</h1>" );
  s.erase( "<h2>" );
  s.erase( "</h2>" );
  s.erase( "<ul>" );
  s.erase( "</ul>" );
  s.replace( "<li>", " - " );
  s.erase( "</li>" );
  return s;
}


///// provide/prevent ///////////////////////////////////////////////////////

Str &Str::preventFirst( char c )
{
  if ( size() > 0 && at( 0 ) == c )
    string::erase( 0, 1 );

  return *this;
}


Str Str::preventedFirst( char c ) const
{
  if ( size() > 0 && at( 0 ) == c )
    return string::substr( 1 );
  else
    return *this;
}


Str &Str::preventFirst( const char* s )
{
  size_type sl = strlen( s );
  if ( sl > 0 && string( *this, 0, sl ).compare( s ) == 0 )
    string::erase( 0, sl );

  return *this;
}


Str Str::preventedFirst( const char* s ) const
{
  int sl = (int)strlen( s );
  if ( sl > 0 && size() >= sl && string( *this, 0, sl ).compare( s ) == 0 )
    return string::substr( sl );
  else
    return *this;
}


Str &Str::preventFirst( const string &s )
{
  int sl = (int)s.size();
  if ( sl > 0 && size() >= sl && string( *this, 0, sl ).compare( s ) == 0 )
    string::erase( 0, sl );

  return *this;
}


Str Str::preventedFirst( const string &s ) const
{
  int sl = (int)s.size();
  if ( sl > 0 && size() >= sl && string( *this, 0, sl ).compare( s ) == 0 )
    return string::substr( sl );
  else
    return *this;
}


Str &Str::preventLast( char c )
{
  int l = size();
  if ( l > 0 && size() >= l && at( l-1 ) == c )
    string::resize( l-1 );

  return *this;
}


Str Str::preventedLast( char c ) const
{
  int l = size();
  if ( l > 0 && at( l-1 ) == c )
    return string::substr( 0, l-1 );
  else
    return *this;
}


Str &Str::preventLast( const char* s )
{
  size_type l = size();
  size_type sl = strlen( s );
  if ( l > 0 && sl > 0 && string( *this, l-sl, sl ).compare( s ) == 0 )
    string::resize( l-sl );

  return *this;
}


Str Str::preventedLast( const char* s ) const
{
  size_type l = size();
  size_type sl = strlen( s );
  if ( sl > 0 && l >= l && string( *this, l-sl, sl ).compare( s ) == 0 )
    return string::substr( 0, l-sl );
  else
    return *this;
}


Str &Str::preventLast( const string &s )
{
  size_type l = size();
  size_type sl = s.size();
  if ( sl > 0 && l >= sl && string( *this, l-sl, sl ).compare( s ) == 0 )
    string::resize( l-sl );

  return *this;
}


Str Str::preventedLast( const string &s ) const
{
  size_type l = size();
  size_type sl = s.size();
  if ( sl > 0 && l >= sl && string( *this, l-sl, sl ).compare( s ) == 0 )
    return string::substr( 0, l-sl );
  else
    return *this;
}


Str &Str::provideFirst( char c )
{
  if ( size()==0 || at( 0 ) != c )
    string::insert( (unsigned int)0, (unsigned int)1, c );

  return *this;
}


Str Str::providedFirst( char c ) const
{
  if ( size()>0 && at( 0 ) == c )
    return *this;
  else
    return c + string( *this );
}


Str &Str::provideFirst( const char* s )
{
  size_type l = size();
  size_type sl = strlen( s );
  if ( sl > 0 && ( l < sl || string( *this, 0, sl ).compare( s ) != 0 ) )
    string::insert( 0, s );

  return *this;
}


Str Str::providedFirst( const char* s ) const
{
  size_type l = size();
  size_type sl = strlen( s );
  if ( sl == 0 || ( l >= sl && string( *this, 0, sl ).compare( s ) == 0 ) )
    return *this;
  else
    return s + string( *this );
}


Str &Str::provideFirst( const string &s )
{
  size_type l = size();
  size_type sl = s.size();
  if ( sl > 0 && ( l < sl || string( *this, 0, sl ).compare( s ) != 0 ) )
    string::insert( 0, s );

  return *this;
}


Str Str::providedFirst( const string &s ) const
{
  size_type l = size();
  size_type sl = s.size();
  if ( sl == 0 || ( l >= sl && string( *this, 0, sl ).compare( s ) == 0 ) )
    return *this;
  else
    return s + string( *this );
}


Str &Str::provideLast( char c )
{
  size_type l = size();
  if ( l==0 || at( l-1 ) != c )
    string::append( 1, c );

  return *this;
}


Str Str::providedLast( char c ) const
{
  size_type l = size();
  if ( l>0 && at( l-1 ) == c )
    return *this;
  else
    return string( *this ) + c;
}


Str &Str::provideLast( const char* s )
{
  size_type l = size();
  size_type sl = strlen( s );
  if ( sl > 0 && ( l < sl || string( *this, l-sl, sl ).compare( s ) != 0 ) )
    string::append( s );

  return *this;
}


Str Str::providedLast( const char* s ) const
{
  size_type l = size();
  size_type sl = strlen( s );
  if ( sl == 0 || ( l >= sl && string( *this, l-sl, sl ).compare( s ) == 0 ) )
    return *this;
  else
    return string( *this ) + s;
}


Str &Str::provideLast( const string &s )
{
  size_type l = size();
  size_type sl = s.size();
  if ( sl > 0 && ( l < sl || string( *this, l-sl, sl ).compare( s ) != 0 ) )
    string::append( s );

  return *this;
}


Str Str::providedLast( const string &s ) const
{
  size_type l = size();
  size_type sl = s.size();
  if ( sl == 0 || ( l >= sl && string( *this, l-sl, sl ).compare( s ) == 0 ) )
    return *this;
  else
    return string( *this ) + s;
}


///// upper/lower case //////////////////////////////////////////////////////////

Str &Str::upper( void )
{
  for ( iterator p=begin(); p != end(); p++ )
    *p = toupper( *p );

  return *this;
}


Str Str::up( void ) const
{
  string us;

  for ( const_iterator p=begin(); p != end(); p++ )
    us.append( 1, char( toupper( *p ) ) );

  return us;
}


Str &Str::lower( void )
{
  for ( iterator p=begin(); p != end(); p++ )
    *p = tolower( *p );

  return *this;
}


Str Str::low( void ) const
{
  string ls;

  for ( const_iterator p=begin(); p != end(); p++ )
    ls.append( 1, char( tolower( *p ) ) );

  return ls;
}


///// manipulate ////////////////////////////////////////////////////////////////

Str &Str::leftJustify( int width, char pad )
{
  strip( pad );
  width = abs( width );
  if ( width <= size() )
    string::resize( width );
  else
    string::append( width - size(), pad );
  return *this;
}


Str &Str::rightJustify( int width, char pad )
{
  strip( pad );
  width = abs( width );
  if ( width <= size() )
    string::resize( width );
  else
    string::insert( 0u, width - size(), pad );
  return *this;
}


Str &Str::centerJustify( int width, char pad )
{
  strip( pad );
  width = abs( width );
  if ( width <= size() )
    string::resize( width );
  else
    {
      int len = width - size();
      string::insert( 0u, len/2, pad );
      string::append( len - len/2, pad );
    }
  return *this;
}


Str Str::centerJustified( int width, char pad ) const
{
  string s = stripped( pad );
  width = abs( width );
  if ( width <= (int)s.size() )
    s.resize( width );
  else
    {
      int len = width - s.size();
      s.insert( 0u, len/2, pad );
      s.append( len - len/2, pad );
    }
  return s;
}


Str &Str::goldenJustify( int width, char pad )
{
  strip( pad );
  width = abs( width );
  if ( width <= size() )
    string::resize( width );
  else
    {
      int len = width - size();
      string::insert( 0u, len/3, pad );
      string::append( len - len/3, pad );
    }
  return *this;
}


Str Str::goldenJustified( int width, char pad ) const
{
  string s = stripped( pad );
  width = abs( width );
  if ( width <= (int)s.size() )
    s.resize( width );
  else
    {
      int len = width - s.size();
      s.insert( 0u, len/3, pad );
      s.append( len - len/3, pad );
    }
  return s;
}


///// whitespace ////////////////////////////////////////////////////////////

bool Str::empty( const string &space, const string &comment ) const
{
  if ( string::empty() )
    return true;

  int i = findFirstNot( space );
  return ( i < 0 || find( comment, i ) == i );
}


Str &Str::strip( const string &space, const string &comment )
{
  if ( space.size() == 0 )
    return *this;

  size_type l = string::find_first_not_of( space );
  if ( l == npos ) {
    string::resize( 0 );
    return *this;
  }

  string::erase( 0, l );

  int c = size();
  if ( comment.size() > 0 ) {
    size_type cp = string::find( comment );
    if ( cp != npos ) {
      if ( cp == 0 ) {
	string::resize( 0 );
	return *this;
      }
      else
	c = cp;
    }
  }

  size_type r = string::find_last_not_of( space, c-1 );
  if ( r != npos )
    string::resize( r + 1 );

  return *this;
}


Str &Str::strip( char space, const string &comment )
{
  size_type l=string::find_first_not_of( space );
  if ( l == npos ) {
    string::resize( 0 );
    return *this;
  }

  string::erase( 0, l );

  int c = size();
  if ( comment.size() > 0 ) {
    size_type cp = string::find( comment );
    if ( cp != npos ) {
      if ( c == 0 ) {
	string::resize( 0 );
	return *this;
      }
      else
	c = cp;
    }
  }

  size_type r = string::find_last_not_of( space, c-1 );
  if ( r != npos )
    string::resize( r + 1 );

  return *this;
}


Str Str::stripped( const string &space, const string &comment ) const
{
  if ( space.size() == 0 )
    return *this;

  size_type c = size();
  if ( comment.size() > 0 ) {
    size_type cp = string::find( comment );
    if ( cp != npos )
      c = cp;
  }

  size_type l = string::find_first_not_of( space );
  if ( l == npos || l >= c )
    return "";

  size_type r = string::find_last_not_of( space, c-1 );

  if ( r != npos )
    return string::substr( l, r+1-l );
  else
    return "";
}


Str Str::stripped( char space, const string &comment ) const
{
  size_type c;
  if ( comment.size() == 0 )
    c = size();
  else {
    c = string::find( comment );
    if ( c == npos )
      c = size();
  }

  size_type l = string::find_first_not_of( space );
  if ( l == npos || l >= c )
    return "";

  size_type r = string::find_last_not_of( space, c-1 );

  if ( r != npos )
    return string::substr( l, r+1-l );
  else
    return "";
}


Str &Str::simplify( const string &space, const string &comment )
{
  // delete leading and trailing spaces:
  strip( space, comment );

  // simplify internal series of spaces:
  int i, k;
  bool b = false;
  for ( k=i=0; i<size(); i++ ) {
    if ( space.find( at( i ) ) != npos ) {
      if ( !b ) {
	b = true;
	at( k ) = space[0];
	k++;
      }
    }
    else {
      b = false;
      at( k ) = at( i );
      k++;
    }
  }

  string::resize( k );
  
  return *this;
}


Str &Str::simplify( char space, const string &comment )
{
  // delete leading and trailing spaces:
  strip( space, comment );

  // simplify internal series of spaces:
  int i, k;
  bool b = false;
  for ( k=i=0; i<size(); i++ )
    {
      if ( at( i ) == space )
	{
	  if ( !b )
	    {
	      b = true;
	      at( k ) = space;
	      k++;
	    }
	}
      else
	{
	  b = false;
	  at( k ) = at( i );
	  k++;
	}
    }

  string::resize( k );

  return *this;
}


Str Str::simplified( const string &space, const string &comment ) const
{
  // delete leading and trailing spaces:
  Str ss( stripped( space, comment ) );

  // simplify internal series of spaces:
  int i, k;
  bool b = false;
  for ( k=i=0; i<ss.size(); i++ ) {
    if ( space.find( ss[ i ] ) != ss.npos ) {
      if ( !b ) {
	b = true;
	ss[ k ] = space[0];
	k++;
      }
    }
    else {
      b = false;
      ss[ k ] = ss[ i ];
      k++;
    }
  }
  
  ss.resize( k );
  
  return ss;
}


Str Str::simplified( char space, const string &comment ) const
{
  // delete leading and trailing spaces:
  Str ss( stripped( space, comment ) );

  // simplify internal series of spaces:
  int i, k;
  bool b = false;
  for ( k=i=0; i<ss.size(); i++ )
    {
      if ( ss[ i ] == space )
	{
	  if ( !b )
	    {
	      b = true;
	      ss[ k ] = space;
	      k++;
	    }
	}
      else
	{
	  b = false;
	  ss[ k ] = ss[ i ];
	  k++;
	}
    }

  ss.resize( k );

  return ss;
}


Str &Str::stripComment( const string &comment )
{
  if ( comment.empty() )
    return *this;

  if ( comment[0] == '-' && comment.size() > 1 ) {
    size_type c = string::find( comment.substr( 1 ) );
    if ( c != npos )
      string::erase( 0, c + comment.size() - 1 );
  }
  else {
    size_type c = string::find( comment );
    if ( c != npos )
      string::resize( c );
  }

  return *this;
}


Str Str::strippedComment( const string &comment ) const
{
  if ( comment.size() == 0 )
    return *this;

  if ( comment[0] == '-' && comment.size() > 1 ) {
    size_type c = string::find( comment.substr( 1 ) );
    if ( c != npos )
      return string::substr( c + comment.size()-1 );
  }
  else {
    size_type c = string::find( comment );
    if ( c != npos )
      return substr( 0, c );
  }

  return *this;
}


int Str::nextWord( int &index, const string &space, const string &comment ) const
{
  // maximum length of string:
  int l = find( comment, index );
  if ( l < 0 )
    l = size();

  // index not valid:
  if ( index < 0 || index >= l ) {
    index = -1;
    return -1;
  }

  // double-blank mode:
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // skip leading space:
  index = findFirstNot( space, index );

  // no word found:
  if ( index < 0 || index >= l ) {
    index = -1;
    return -1;
  }

  // store begin of word:
  int i = index;

  // find next space:
  for ( ; ; ) {
    // position of space:
    index = findFirst( space, index );
    // check for double blank:
    if ( db ) {
      if ( index < 0 || index+1 >= l ||
	   ( at( index ) == ' ' && at( index+1 ) == ' ' ) )
	break;
      index++;
    }
    else
      break;
  }
  
  // no valid space found:
  if ( index < 0 || index >= l )
    index = l;

  // return position of word:
  return i;
}


Str Str::wordAt( int &index, const string &space, const string &comment ) const
{
  int w = nextWord( index, space, comment );
  if ( w >= 0 )
    return mid( w, index-1 );
  else
    return "";
}


Str Str::word( int n, const string &space, const string &comment ) const
{
  int i=0, j=0;
  for ( int k=0; k<=n && i>=0; k++)
    j = nextWord( i, space, comment );

  if ( j >= 0 )
    return substr( j, i-j );
  else
    return "";
}


int Str::words( const string &space, const string &comment ) const
{
  int n = -1;
  for ( int i=0; i>=0; n++ )
    nextWord( i, space, comment );

  return n;
}


///// felder ////////////////////////////////////////////////////////////////


int Str::nextField( int &index, const string &space, const string &comment ) const
{
  // maximum length of string:
  int l = find( comment, index );
  if ( l < 0 )
    l = size();

  // index not valid:
  if ( index >= l ) {
    index = -1;
    return -1;
  }

  // double-blank mode:
  bool db = ( space.size() > 0 && space.find( "  " ) != npos );

  // skip leading space:
  if ( index < 0 )
    index = 0;
  else if ( space.find( at( index ) ) != npos ) {
    index++;
    if ( db && index > 0 && at( index-1 ) == ' ' && at( index ) == ' ' )
      index++;
  }

  // store begin of word:
  int i = index;

  // find next space:
  for ( ; ; ) {
    // position of space:
    index = findFirst( space, index );
    // check for double blank:
    if ( db ) {
      if ( index < 0 || index+1 >= l ||
	   ( at( index ) == ' ' && at( index+1 ) == ' ' ) )
	break;
      index += 2;
    }
    else
      break;
  }
  
  // no valid space found:
  if ( index < 0 || index >= l )
    index = l;

  // return position of word:
  return i;
}


int Str::findSeparator( int index, const string &separator,
			const string &brackets, 
			const string &comment ) const
{
  // maximum length of string:
  size_type l;
  if ( comment.size() == 0 )
    l = size(); 
  else {
    l = string::find( comment, index );
    if ( l == npos )
      l = size();
  }

  // index not valid:
  if ( index < 0 || index >= int( l ) )
    return -1;

  // blank-space mode:
  bool bs = ( separator.find( ' ' ) != npos );

  // double-blank mode:
  bool db = ( separator.find( "  " ) );

  // set search index:
  int i = index;

  // skip blank-space:
  if ( bs ) {
    i = findFirstNot( ' ', i );
    if ( i < 0 )
      return -1;
  }

  // find next separator:
  for ( ; ; ) {
    // position of an opening bracket:
    int b = findFirst( brackets, i );
    for ( ; ; ) {
      // position of separator:
      i = findFirst( separator, i );
      // check for double blank:
      if ( db ) {
	if ( i < 0 || 
	     ( i+1 < int( l ) && at( i ) == ' ' && at( i+1 ) == ' ' ) )
	  break;
	i++;
      }
      else
	break;
    }
    // there is an opening bracket before the separator.
    // find corresponding closing bracket:
    if ( i < int( l ) && b >= 0 && b < i ) {
      b = findBracket( b, brackets, comment );
      if ( b >= 0 ) {
	i = b + 1;
	continue;
      }
    }
    break;
  }
  
  // no valid separator found:
  if ( i < 0 || i >= int( l ) )
    return -1;
  
  // return position of separator:
  return i;
}


Str Str::field( int n, const string &separator,
		const string &brackets, const string &comment ) const
{
  // no content:
  if ( size() <= 0 )
    return "";

  // begin of field:
  int i = 0;
  for ( int k=0; k<n; k++ ) {
    i = findSeparator( i, separator, brackets, comment );
    if ( i < 0 )
      return "";
    i++;
  }

  // adjust for double blank:
  if ( separator.find( "  " ) != npos && at( i ) == ' ' )
    i++;

  // end of field:
  int j = findSeparator( i, separator, brackets, comment );
  if ( j < 0 )
    {
      // maximum length of string:
      int l = find( comment );
      j = l < 0 ? size() : l;
    }

  // cut out field:
  Str s = substr( i, j-i );

  return s;
}


///// file names: ////////////////////////////////////////////////////////////

Str Str::dir( char dirsep ) const
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // no dirsep found:
  if ( p == npos )
    return "";

  // dirsep found:
  return string::substr( 0, p+1 );
}


Str Str::notdir( char dirsep ) const
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // no dirsep found:
  if ( p == npos )
    return *this;

  // dirsep found:
  return string::substr( p+1 );
}


Str Str::name( char dirsep, char extsep ) const
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find first following extsep:
  size_type e = string::find( extsep, p );

  // extsep not found:
  if ( e == npos )
    return string::substr( p );

  // extsep found:
  return string::substr( p, e-p );
}


Str Str::longName( char dirsep, char extsep ) const
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find first following extsep:
  size_type e = string::rfind( extsep );

  // extsep not found:
  if ( e == npos )
    return string::substr( p );

  // extsep found:
  return string::substr( p, e-p );
}


Str Str::extension( char dirsep, char extsep ) const
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find first following extsep:
  size_type e = string::find( extsep, p );

  // extsep not found:
  if ( e == npos )
    return "";

  // extsep found:
  return string::substr( e );
}


Str Str::suffix( char dirsep, char extsep ) const
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find the last extsep:
  size_type e = string::rfind( extsep );

  // extsep not found:
  if ( e == npos || e < p )
    return "";

  // extsep found:
  return string::substr( e );
}


Str Str::basename( char dirsep, char extsep ) const
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find first following extsep:
  size_type e = string::find( extsep, p );

  // extsep not found:
  if ( e == npos )
    return *this;

  // extsep found:
  return string::substr( 0, e );
}


Str &Str::stripDir( char dirsep )
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // dirsep found:
  if ( p != npos )
    string::erase( 0, p+1 );

  return *this;
}


Str &Str::stripNotdir( char dirsep )
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  string::resize( p );

  return *this;
}


Str &Str::stripExtension( char dirsep, char extsep )
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find first following extsep:
  size_type e = string::find( extsep, p );

  // extsep found:
  if ( e != npos )
    string::resize( e );

  return *this;
}


Str &Str::stripSuffix( char dirsep, char extsep )
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find the last extsep:
  size_type e = string::rfind( extsep );

  // extsep found:
  if ( e != npos && e >= p )
    string::resize( e );

  return *this;
}


Str &Str::stripBasename( char dirsep, char extsep )
{
  // find last dirsep:
  size_type p = string::rfind( dirsep );

  // set p to the first character of the name:
  if ( p == npos )
    p = 0;
  else
    p++;

  // find first following extsep:
  size_type e = string::find( extsep, p );

  // extsep found:
  if ( e != npos )
    string::erase( 0, e );
  else
    string::erase();

  return *this;
}


Str &Str::provideSlash( char dirsep )
{ 
  if ( empty() )
    return *this;
  else
    return provideLast( dirsep );
}


Str Str::providedSlash( char dirsep ) const 
{ 
  if ( empty() )
    return *this;
  else
    return providedLast( dirsep );
}


Str Str::homePath( char dirsep )
{
  // get home directory:
  Str homepath( getEnvironment( HomeEnv.c_str() ), 0 );
  if ( !homepath.empty() ) {
    homepath.provideSlash( dirsep );
    return homepath;
  }
  else
    return dirsep;
}


Str Str::workingPath( char dirsep )
{
  // get working directory:
  Str workingpath( getEnvironment( WorkingEnv.c_str() ), 0 );
  workingpath.provideSlash( dirsep );
  return workingpath;
}


Str &Str::stripWorkingPath( int maxlevel, char dirsep )
{
  if ( ! empty() && at( 0 ) == dirsep ) {
    Str wp = workingPath();
    for ( int k=0; k<maxlevel && ! wp.empty(); k++ ) {
      if ( find( wp ) == 0 ) {
	erase( 0, wp.size() );
	Str path = "";
	for ( int i=0; i<k; i++ )
	  path += "../";
	*this = path + *this;
	break;
      }
      int sp = wp.rfind( dirsep, wp.size() - 2 );
      wp.erase( sp+1 );
    }
  }
  return *this;
}


Str &Str::expandHome( char dirsep, char homechr )
{
  // the string starting with homechr:
  if ( size() > 0 && at( 0 ) == homechr )
    {
      // get home directory:
      Str homepath( getEnvironment( HomeEnv.c_str() ), 0 );
      if ( !homepath.empty() )
	{
	  homepath.preventSlash( dirsep );

	  // second character is dirsep:
	  if ( size() > 1 && at( 1 ) != dirsep )
	    {
	      // remove the last directory from homepath:
	      homepath.stripNotdir( dirsep );
	    }
	  // erase home character:
	  string::erase( 0, 1 );
	  // insert home path:
	  string::insert( 0, homepath );
	}
      else
	{
	  // erase home character:
	  string::erase( 0, 1 );
	}
    }

  return *this;
}


Str Str::expandedHome( char dirsep, char homechr ) const
{
  // the string starting with homechr:
  if ( size() > 0 && at( 0 ) == homechr )
    {
      // get home directory:
      Str homepath( getEnvironment( HomeEnv.c_str() ), 0 );
      if ( !homepath.empty() )
	{
	  homepath.preventSlash( dirsep );

	  // second character is dirsep:
	  if ( size() > 1 && at( 1 ) != dirsep )
	    {
	      // remove the last directory from homepath:
	      homepath.stripNotdir( dirsep );
	    }
	  return homepath + string::substr( 1 );
	}
      else
	{
	  // erase home character:
	  return string::substr( 1 );
	}
    }
  else
    return *this;
}


Str &Str::removeWorking( char dirsep )
{
  // nothing to do:
  if ( size() == 0 )
    return *this;

  // remove single '.':
  if ( size() == 1 && at( 0 ) == '.' ) {
    string::resize( 0 );
    return *this;
  }

  // loop through all '.':
  size_type p = string::find( '.' );
  while ( p != npos ) {
    if ( p+1 < string::size() && at( p+1 ) == dirsep &&
	 ( p == 0 || at( p-1 ) == dirsep ) )
      string::erase( p, 2 );
    else
      p++;
    p = string::find( '.', p );
  }

  return *this;
}


Str Str::removedWorking( char dirsep ) const
{
  // nothing to do:
  if ( size() == 0 )
    return *this;

  // remove single '.':
  if ( size() == 1 && at( 0 ) == '.' )
    return "";

  // loop through all '.':
  string path( *this );
  size_type p = path.find( '.' );
  while ( p != npos )
    {
      if ( p+1 < path.size() && path[ p+1 ] == dirsep &&
	   ( p == 0 || path[ p-1 ] == dirsep ) )
	path.erase( p, 2 );
      else
	p++;
      p = path.find( '.', p );
    }

  return path;
}


Str &Str::addWorking( const string &dir, char dirsep )
{
  if ( size() == 0 || at( 0 ) != dirsep ) {
    Str s;
    if ( dir.empty() )
      s = workingPath();
    else {
      s = dir;
      s.provideSlash( dirsep );
    }
    string::insert( 0, s );
  }

  return *this;
}


Str Str::addedWorking( const string &dir, char dirsep ) const
{
  if ( size() == 0 || at( 0 ) != dirsep ) {
    Str s;
    if ( dir.empty() )
      s = workingPath();
    else {
      s = dir;
      s.provideSlash( dirsep );
    }
    return s.append( *this, 0 );
  }
  
  return *this;
}


Str &Str::expandParent( char dirsep )
{
  // nothing to do:
  if ( size() == 0 )
    return *this;

  // parent dirs with nothing to do:
  if ( *this == ".." || *this == "../" ) {
    resize( 0 );
    return *this;
  }

  // prepare parent directory:
  string parent( 1, dirsep );
  parent += "..";
  size_type l = parent.size();
  if ( l == 0 )
    return *this;

  // loop through all "/..":
  size_type p = string::find( parent );
  while ( p != npos ) {
    if ( p > 0 && ( p+l >= string::size() || at( p+l ) == dirsep ) ) {
      size_type s = string::rfind( dirsep, p-1 );
      if ( s == npos ) {
	string::erase( 0, p+l+1 );
	p = 0;
      }
      else {
	string::erase( s, p-s+l );
	p = s;
      }
    }
    else
      p += l;
    p = string::find( parent, p );
  }
  
  return *this;
}


Str Str::expandedParent( char dirsep ) const
{
  // nothing to do:
  if ( size() == 0 )
    return *this;

  // parent dirs with nothing to do:
  if ( *this == ".." || *this == "../" )
    return "";

  // prepare parent directory:
  string parent( 1, dirsep );
  parent += "..";
  size_type l = parent.size();
  if ( parent.size() == 0 )
    return *this;

  // loop through all "/..":
  string path( *this );
  size_type p = path.find( parent );
  while ( p != npos )
    {
      if ( p > 0 && ( p+l >= path.size() || path[ p+l ] == dirsep ) )
	{
	  size_type s = path.rfind( dirsep, p-1 );
	  if ( s == npos )
	    {
	      path.erase( 0, p+l+1 );
	      p = 0;
	    }
	  else
	    {
	      path.erase( s, p-s+l );
	      p = s;
	    }
	}
      else
	p += l;
      p = path.find( parent, p );
    }

  return path;
}


Str &Str::expandPath( const string &dir, char dirsep, char homechr )
{
  expandHome( dirsep, homechr );
  removeWorking( dirsep );
  addWorking( dir, dirsep );
  expandParent( dirsep );
  return *this;
}


Str Str::expandedPath( const string &dir, 
			     char dirsep, char homechr ) const
{
  Str path( expandedHome( dirsep, homechr ) );
  path.removeWorking( dirsep );
  path.addWorking( dir, dirsep );
  path.expandParent( dirsep );
  return path;
}


Str &Str::addExtension( const string &extension, char extsep )
{
  providePeriod( extsep );
  if ( extension.size() > 0 && extension[0] == extsep )
    string::append( extension.substr( 1 ) );
  else  
    string::append( extension );
  return *this;
}


Str Str::addedExtension( const string &extension, char extsep ) const
{
  Str path( providedPeriod( extsep ) );
  if ( extension.size() > 0 && extension[0] == extsep )
    return path + extension.substr( 1 );
  else
    return path + extension;
}


Str &Str::provideExtension( const string &ext, 
				  char dirsep, char extsep )
{
  if ( extension( dirsep, extsep ).size() <= 1 )
    addExtension( ext, extsep );
  return *this;
}


Str Str::providedExtension( const string &ext, 
				  char dirsep, char extsep ) const
{
  if ( extension( dirsep, extsep ).size() <= 1 )
    return addedExtension( ext, extsep );
  else
    return *this;
}


///// Environment Variables: //////////////////////////////////////////////////

Str Str::getEnvironment( const string &env )
{
  char *es = getenv( env.c_str() );
  if ( es != 0 )
    return Str ( es, 0 );
  else
    return "";
}


Str &Str::environment( const string &env )
{
  char *es = getenv( env.c_str() );
  if ( es != 0 )
    string::assign( es );
  else
    string::resize( 0 );

  return *this;
}


///// LaTeX: /////////////////////////////////////////////////////////////////

Str Str::latex( void ) const
{
  string ls;

  for ( const_iterator sp = begin(); sp != end(); sp++ )
    if ( strchr( "#$%&{}_", *sp ) != 0 )
      {
        ls.append( 1, '\\' );
        ls.append( 1, *sp );
      }
    else if ( *sp == '<' )
      ls.append( "$<$" );
    else if ( *sp == '>' )
      ls.append( "$>$" );
    else if ( (unsigned char)(*sp) == 132 )
      ls.append( "\\\"a" );
    else if ( (unsigned char)(*sp) == 148 )
      ls.append( "\\\"o" );
    else if ( (unsigned char)(*sp) == 129 )
      ls.append( "\\\"u" );
    else if ( (unsigned char)(*sp) == 142 )
      ls.append( "\\\"A" );
    else if ( (unsigned char)(*sp) == 153 )
      ls.append( "\\\"O" );
    else if ( (unsigned char)(*sp) == 154 )
      ls.append( "\\\"U" );
    else if ( (unsigned char)(*sp) == 225 )
      ls.append( "\\ss " );
    else if ( *sp == 20 )
      ls.append( "\\P" );
    else if ( *sp == 21 )
      ls.append( "\\S" );
    else if ( *sp == '*' )
      ls.append( "$\\ast$" );
    else if ( *sp == '\\' )
      ls.append( "$\\backslash$" );
    else if ( *sp == '~' )
      ls.append( "$\\sim$" );
    else if ( *sp < 32 )
      ls.append( 1, '.' );
    else
      ls.append( 1, *sp );

  return ls;
}


Str Str::latexNum( void ) const
{
  string ls;

  ls.append( 1, '$' );
  for ( const_iterator sp = begin(); sp != end(); sp++ )
    if ( *sp == 'e' || *sp == 'E' )
      {
	ls.append( "\\times 10^{" );
	for ( sp++;
              sp != end() && ((*sp >= '0' && *sp <= '9') || *sp == '+' || *sp == '-');
              sp++ )
	  ls.append( 1, *sp );
        ls.append( 1, '}' );
	sp--;
      }
    else if ( *sp == '^' )
      {
	ls.append( "^{" );
	for ( sp++;
              sp != end() && ((*sp >= '0' && *sp <= '9') || *sp == '+' || *sp == '-');
              sp++ )
	  ls.append( 1, *sp );
        ls.append( 1, '}' );
	sp--;
      }
    else if ( *sp == '_' )
      {
	ls.append( "_{" );
	for ( sp++;
              sp != end() && ((*sp >= '0' && *sp <= '9') || *sp == '+' || *sp == '-');
              sp++ )
	  ls.append( 1, *sp );
        ls.append( 1, '}' );
	sp--;
      }
    else if ( strchr( "#$%&{}_", *sp ) != 0 )
      {
        ls.append( 1, '\\' );
        ls.append( 1, *sp );
      }
    else if ( (unsigned char)(*sp) == 176 )
      ls.append( "^\\circ" );
    else if ( *sp < 32 )
      ls.append( 1, '.' );
    else
      ls.append( 1, *sp );

  ls.append( 1, '$' );

  return ls;
}


Str Str::latexUnit( void ) const
{
  string ls;

  for ( const_iterator sp = begin(); sp != end(); sp++ ) {
    if ( *sp == 'm' && sp+1 != end() && *(sp+1) == 'u' &&
	 ( sp == begin() || ! isalpha( *(sp-1) ) ) ) {
      ls.append( "$\\mu$" );
      sp++;
    }
    else if ( *sp == 'u' && sp+1 != end() && isalpha( *(sp+1) ) &&
	      ( sp == begin() || ! isalpha( *(sp-1) ) ) ) {
      ls.append( "$\\mu$" );
    }
    else if ( *sp == 'O' &&
	      sp+1 != end() && *(sp+1) == 'h' &&
	      sp+2 != end() && *(sp+2) == 'm' ) {
      ls.append( "$\\Omega$" );
      sp += 2;
    }
    else if ( ( sp == begin() || ( *(sp-1) >= '0' && *(sp-1) <= '9' ) || *(sp-1) == '.' ) && 
	      ( *sp == 'e' || *sp == 'E' ) && sp+1 != end() && 
	      ( (*(sp+1) >= '0' && *(sp+1) <= '9') ||
		*(sp+1) == '+' || *(sp+1) == '-' ) ) {
      ls.append( "$\\times 10^{" );
      for ( sp++;
	    sp != end() && ((*sp >= '0' && *sp <= '9') || *sp == '+' || *sp == '-');
	    sp++ )
	ls.append( 1, *sp );
      ls.append( "}$" );
      sp--;
    }
    else if ( *sp == '^' && sp+1 != end() && 
	      ( (*(sp+1) >= '0' && *(sp+1) <= '9') ||
		*(sp+1) == '+' || *(sp+1) == '-' ) ) {
      ls.append( "$^{" );
      int k = 0;
      for ( sp++;
	    sp != end() && ( (*sp >= '0' && *sp <= '9') || *(sp+1) == '.' || 
			     ( k == 0 && ( *sp == '+' || *sp == '-' ) ) );
	    sp++, k++ )
	ls.append( 1, *sp );
      ls.append( "}$" );
      sp--;
    }
    else if ( *sp == '_' && sp+1 != end() && 
	      ( (*(sp+1) >= '0' && *(sp+1) <= '9') ||
		*(sp+1) == '+' || *(sp+1) == '-' ) ) {
      ls.append( "$_{" );
      int k = 0;
      for ( sp++;
	    sp != end() && ( (*sp >= '0' && *sp <= '9') || *(sp+1) == '.' || 
			     ( k == 0 && ( *sp == '+' || *sp == '-' ) ) );
	    sp++, k++ )
	ls.append( 1, *sp );
      ls.append( "}$" );
      sp--;
    }
    else if ( strchr( "#$%&{}_", *sp ) != 0 ) {
      ls.append( 1, '\\' );
      ls.append( 1, *sp );
    }
    else if ( *sp == '<' && sp+1 != end() && *(sp+1) == '=' ) {
      ls.append( "$\\le$" );
      sp++;
    }
    else if ( *sp == '>' && sp+1 != end() && *(sp+1) == '=' ) {
      ls.append( "$\\ge$" );
      sp++;
    }
    else if ( *sp == '<' )
      ls.append( "$<$" );
    else if ( *sp == '>' )
      ls.append( "$>$" );
    else if ( *sp == '-' )
      ls.append( "$-$" );
    else if ( *sp == '*' )
      ls.append( "$\\cdot$" );
    else if ( (unsigned char)(*sp) == 176 )
      ls.append( "^\\circ" );
    else if ( (unsigned char)(*sp) == 132 )
      ls.append( "\\\"a" );
    else if ( (unsigned char)(*sp) == 148 )
      ls.append( "\\\"o" );
    else if ( (unsigned char)(*sp) == 129 )
      ls.append( "\\\"u" );
    else if ( (unsigned char)(*sp) == 142 )
      ls.append( "\\\"A" );
    else if ( (unsigned char)(*sp) == 153 )
      ls.append( "\\\"O" );
    else if ( (unsigned char)(*sp) == 154 )
      ls.append( "\\\"U" );
    else if ( (unsigned char)(*sp) == 225 )
      ls.append( "\\ss " );
    else if ( *sp == 20 )
      ls.append( "\\P" );
    else if ( *sp == 21 )
      ls.append( "\\S" );
    else if ( *sp == '\\' )
      ls.append( "$\\backslash$" );
    else if ( *sp == '~' )
      ls.append( "$\\sim$" );
    else if ( *sp < 32 )
      ls.append( 1, '.' );
    else
      ls.append( 1, *sp );
  }

  return ls;
}


Str Str::html( void ) const
{
  string ls;

  for ( const_iterator sp = begin(); sp != end(); sp++ ) {
    if ( *sp == '"' )
      ls.append( "&quot;" );
    else if ( *sp == '&' )
      ls.append( "&amp;" );
    else if ( *sp == '<' )
      ls.append( "&lt;" );
    else if ( *sp == '>' )
      ls.append( "&gt;" );
    else if ( (unsigned char)(*sp) == 132 )
      ls.append( "&auml;" );
    else if ( (unsigned char)(*sp) == 148 )
      ls.append( "&ouml;" );
    else if ( (unsigned char)(*sp) == 129 )
      ls.append( "&uuml;" );
    else if ( (unsigned char)(*sp) == 142 )
      ls.append( "&Auml;" );
    else if ( (unsigned char)(*sp) == 153 )
      ls.append( "&Ouml;" );
    else if ( (unsigned char)(*sp) == 154 )
      ls.append( "&Uuml;" );
    else if ( (unsigned char)(*sp) == 225 )
      ls.append( "&szlig;" );
    else if ( *sp == 20 )
      ls.append( "&para;" );
    else if ( *sp == 21 )
      ls.append( "&sect;" );
    else if ( *sp < 32 )
      ls.append( 1, '.' );
    else
      ls.append( 1, *sp );
  }

  return ls;
}


Str Str::htmlUnit( void ) const
{
  string ls;

  for ( const_iterator sp = begin(); sp != end(); sp++ ) {
    if ( *sp == 'm' && sp+1 != end() && *(sp+1) == 'u' &&
	 ( sp == begin() || ! isalpha( *(sp-1) ) ) ) {
      ls.append( "&micro;" );
      sp++;
    }
    else if ( *sp == 'u' && sp+1 != end() && isalpha( *(sp+1) ) &&
	      ( sp == begin() || ! isalpha( *(sp-1) ) ) ) {
      ls.append( "&micro;" );
    }
    else if ( *sp == 'O' &&
	      sp+1 != end() && *(sp+1) == 'h' &&
	      sp+2 != end() && *(sp+2) == 'm' ) {
      ls.append( "&Omega;" );
      sp += 2;
    }
    else if ( ( sp == begin() || ( *(sp-1) >= '0' && *(sp-1) <= '9' ) || *(sp-1) == '.' ) && 
	      ( *sp == 'e' || *sp == 'E' ) && sp+1 != end() && 
	      ( (*(sp+1) >= '0' && *(sp+1) <= '9') ||
		*(sp+1) == '+' || *(sp+1) == '-' ) ) {
      ls.append( "&times;10<sup>" );
      for ( sp++;
	    sp != end() && ((*sp >= '0' && *sp <= '9') ||
			    *sp == '+' || *sp == '-');
	    sp++ )
	ls.append( 1, *sp );
      ls.append( "</sup>" );
      sp--;
    }
    else if ( *sp == '^' && sp+1 != end() && 
	      ( (*(sp+1) >= '0' && *(sp+1) <= '9') ||
		*(sp+1) == '+' || *(sp+1) == '-' ) ) {
      ls.append( "<sup>" );
      int k = 0;
      for ( sp++;
	    sp != end() && ( (*sp >= '0' && *sp <= '9') || *(sp+1) == '.' || 
			     ( k == 0 && ( *sp == '+' || *sp == '-' ) ) );
	    sp++, k++ )
	ls.append( 1, *sp );
      ls.append( "</sup>" );
      sp--;
    }
    else if ( *sp == '_' && sp+1 != end() && 
	      ( (*(sp+1) >= '0' && *(sp+1) <= '9') ||
		*(sp+1) == '+' || *(sp+1) == '-' ) ) {
      ls.append( "<sub>" );
      int k = 0;
      for ( sp++;
	    sp != end() && ( (*sp >= '0' && *sp <= '9') || *(sp+1) == '.' || 
			     ( k == 0 && ( *sp == '+' || *sp == '-' ) ) );
	    sp++, k++ )
	ls.append( 1, *sp );
      ls.append( "</sub>" );
      sp--;
    }
    else if ( *sp == '<' && sp+1 != end() && *(sp+1) == '=' ) {
      ls.append( "&le;" );
      sp++;
    }
    else if ( *sp == '>' && sp+1 != end() && *(sp+1) == '=' ) {
      ls.append( "&ge;" );
      sp++;
    }
    else if ( *sp == '<' )
      ls.append( "&lt;" );
    else if ( *sp == '>' )
      ls.append( "&gt;" );
    else if ( *sp == '-' )
      ls.append( "&minus;" );
    else if ( *sp == '*' )
      ls.append( "&middot;" );
    else if ( *sp == '"' )
      ls.append( "&quot;" );
    else if ( *sp == '&' )
      ls.append( "&amp;" );
    else if ( (unsigned char)(*sp) == 176 )
      ls.append( "&deg;" );
    else if ( (unsigned char)(*sp) == 132 )
      ls.append( "&auml;" );
    else if ( (unsigned char)(*sp) == 148 )
      ls.append( "&ouml;" );
    else if ( (unsigned char)(*sp) == 129 )
      ls.append( "&uuml;" );
    else if ( (unsigned char)(*sp) == 142 )
      ls.append( "&Auml;" );
    else if ( (unsigned char)(*sp) == 153 )
      ls.append( "&Ouml;" );
    else if ( (unsigned char)(*sp) == 154 )
      ls.append( "&Uuml;" );
    else if ( (unsigned char)(*sp) == 225 )
      ls.append( "&szlig;" );
    else if ( *sp == 20 )
      ls.append( "&para;" );
    else if ( *sp == 21 )
      ls.append( "&sect;" );
    else if ( *sp < 32 )
      ls.append( 1, '.' );
    else
      ls.append( 1, *sp );
  }
  
  return ls;
}


///// format ////////////////////////////////////////////////////////////////

void Str::setDefault( void )
{
  Width = 0;
  Precision = 5;
  CharRepeat = 1;
  DoubleFormat = 'g';
  BoolFormat = '0';
  Pad = ' ';
}


///// private ////////////////////////////////////////////////////////////////

void Str::ReadFormat( const char *format, int &width, int &findex )
{
  // find format specifier in format:
  const char *fp = format;
  for ( fp = strchr( fp, '%' ); fp != 0; fp = strchr( fp, '%' ) )
    if ( *(fp+1) == '%' )
      fp += 2;
    else
      break;
  
  // no format specifier found:
  if ( fp == 0 )
    {
      width = 0;
      findex = -1;
      return;
    }

  // analyse format specifier:
  ++fp;
  char *ep;
  width = abs( strtol( fp, &ep, 10 ) );
  fp = ep;
  // precision:
  if ( *fp == '.' )
    {
      strtol( fp+1, &ep, 10 );
      fp = ep;
    }
  // index to format character:
  findex = fp - format;
}


}; /* namespace relacs */

