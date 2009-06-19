
/*
  parameter.cc
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

#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cctype>
#include <iomanip>
#include <relacs/parameter.h>

namespace relacs {


Parameter::Parameter( void )
{
  clear();
}


Parameter::Parameter( const Parameter &p )
{
  assign( p );
}


Parameter::Parameter( const string &ident, const string &request,  
		      const string &strg, int flags, int style ) 
{
  clear( ident, request, Text );

  string e;
  setText( strg );
  e += Warning;
  setDefaultText( strg );
  e += Warning;
  InternUnit = "";
  OutUnit = "";
  setFlags( flags );
  setStyle( style );
  Warning = e;
}


Parameter::Parameter( const string &ident, const string &request, 
		      double number, double error,
		      double minimum, double maximum, double step,
		      const string &internunit, const string &outputunit, 
		      const string &format, int flags, int style )
{
  clear( ident, request, Number );

  string e;
  setUnit( internunit, outputunit );
  e += Warning;
  setMinMax( minimum, maximum, step );
  e += Warning;
  setNumber( number, error );
  e += Warning;
  setDefaultNumber( number );
  e += Warning;
  setFormat( format );
  e += Warning;
  setFlags( flags );
  setStyle( style );
  Warning = e;
}


Parameter::Parameter( const string &ident, const string &request, 
		      const vector<double> &numbers,
		      const vector<double> &errors, 
		      double minimum, double maximum, double step,
		      const string &internunit, const string &outputunit, 
		      const string &format, int flags, int style )
{
  clear( ident, request, Number );

  string e;
  setUnit( internunit, outputunit );
  e += Warning;
  setMinMax( minimum, maximum, step );
  e += Warning;
  setNumbers( numbers, errors );
  e += Warning;
  if ( numbers.size() > 0 ) {
    setDefaultNumber( numbers[0] );
    e += Warning;
  }
  setFormat( format );
  e += Warning;
  setFlags( flags );
  setStyle( style );
  Warning = e;
}


Parameter::Parameter( const string &ident, const string &request, 
		      long number, long error,
		      long minimum, long maximum, long step,
		      const string &internunit, const string &outputunit, 
		      int width, int flags, int style )
{
  clear( ident, request, Integer );

  string e;
  setUnit( internunit, outputunit );
  e += Warning;
  setMinMax( minimum, maximum, step );
  e += Warning;
  setInteger( number, error );
  e += Warning;
  setDefaultInteger( number );
  e += Warning;
  setFormat( width, 0, 'f' );
  e += Warning;
  setFlags( flags ); 
  setStyle( style );
  Warning = e;
}


Parameter::Parameter( const string &ident, const string &request,  
		      bool dflt, int flags, int style ) 
{
  clear( ident, request, Boolean );

  string e;
  setBoolean( dflt );
  e += Warning;
  setDefaultBoolean( dflt );
  e += Warning;
  setFlags( flags );
  setStyle( style );
  Warning = e;
}


Parameter::Parameter( const string &ident, const string &request, Type type,
		      int yearhour, int monthminutes, int dayseconds,
		      int flags, int style ) 
{
  string e;
  if ( type != Date && type != Time ) {
    e = "type is neither Date nor Time";
    type = Date;
  }

  clear( ident, request, type );

  if ( isDate() )
    setDate( yearhour, monthminutes, dayseconds );
  else
    setTime( yearhour, monthminutes, dayseconds );
  e += Warning;
  if ( isDate() )
    setDefaultDate( yearhour, monthminutes, dayseconds );
  else
    setDefaultTime( yearhour, monthminutes, dayseconds );
  e += Warning;
  setFlags( flags );
  setStyle( style );
  Warning = e;
}


Parameter::Parameter( const string &ident, bool sep, int flags, int style ) 
{
  Type pt = Label;
  if ( ident.empty() )
    pt = Separator;
  else if ( sep )
    style |= TabLabel;

  clear( ident, ident, pt );
  setFlags( flags );
  setStyle( style );
}


Parameter::Parameter( const string &s, const string &assignment )
{
  load( s, assignment );
}


Parameter::Parameter( istream &str, const string &assignment )
{
  string s;
  getline( str, s );
  load( s, assignment );
}


Parameter::~Parameter( void ) 
{
}


Parameter &Parameter::clear( const string &ident, const string &request, Type type )
{
  Ident = ident;
  Request = request.empty() ? ident : request;
  PType = type;
  Flags = 0;
  if ( ( ! String.empty() && ! String[0].empty() ) || 
       ( ! Value.empty() && Value[ 0 ] != 0.0 ) )
    Flags |= ChangedFlag;
  Style = 0;
  setFormat();
  String.resize( 1, "" );
  DefaultString.resize( 1, "" );
  Year = 0;
  Month = 0;
  Day = 0;
  DefaultYear = 0;
  DefaultMonth = 0;
  DefaultDay = 0;
  Hour = 0;
  Minutes = 0;
  Seconds = 0;
  DefaultHour = 0;
  DefaultMinutes = 0;
  DefaultSeconds = 0;
  Value.resize( 1 );
  Value[ 0 ] = 0.0;
  DefaultValue.resize( 1 );
  DefaultValue[ 0 ] = 0.0;
  Error.resize( 1 );
  Error[ 0 ] = -1.0;
  Minimum = -MAXDOUBLE;
  Maximum = MAXDOUBLE;
  Step = 1.0;
  InternUnit = "";
  OutUnit = "";
  ActivationName = "";
  ActivationValues.clear();
  ActivationNumber = 0.0;
  ActivationComparison = 0;
  Activation = true;
  Warning = "";
  return *this;
}


Parameter &Parameter::assign( const Parameter &p )
{
  if ( this == &p ) 
    return *this;

  Ident = p.Ident;
  Request = p.Request;
  PType = p.PType;
  Flags = p.Flags;
  if ( ( ! String.empty() && ! p.String.empty() && String[0] != p.String[0] ) || 
       ( ! Value.empty() && ! p.Value.empty() && Value[0] != p.Value[0] ) )
    Flags |= ChangedFlag;
  Style = p.Style;
  Format = p.Format;
  String = p.String;
  DefaultString = p.DefaultString;
  Year = p.Year;
  Month = p.Month;
  Day = p.Day;
  DefaultYear = p.DefaultYear;
  DefaultMonth = p.DefaultMonth;
  DefaultDay = p.DefaultDay;
  Hour = p.Hour;
  Minutes = p.Minutes;
  Seconds = p.Seconds;
  DefaultHour = p.DefaultHour;
  DefaultMinutes = p.DefaultMinutes;
  DefaultSeconds = p.DefaultSeconds;
  Value = p.Value;
  DefaultValue = p.DefaultValue;
  Error = p.Error;
  Minimum = p.Minimum;
  Maximum = p.Maximum;
  Step = p.Step;
  InternUnit = p.InternUnit;
  OutUnit = p.OutUnit;
  ActivationName = p.ActivationName;
  ActivationValues = p.ActivationValues;
  ActivationNumber = p.ActivationNumber;
  ActivationComparison = p.ActivationComparison;
  Activation = p.Activation;
  Warning = "";

  return *this;
}


Parameter &Parameter::assign( const string &value )
{
  Warning = "";
  if ( isText() && size() > 1 ) {
    selectText( value );
  }
  else if ( ! isBlank() ) {
    setText( value );
    if ( isNotype() ) {
      // XXX check also for date and time!
      bool num = true;
      for ( unsigned int k=0; k<Value.size(); k++ )
	if ( Value[k] == MAXDOUBLE ) {
	  num = false;
	  break;
	}
      if ( num ) {
	bool b = true;
	for ( int k=0; k<String.size(); k++ )
	  if ( String[k] != "false" && String[k] != "true" ) {
	    b = false;
	    break;
	  }
	if ( b ) 
	  setType( Boolean );
	else {
	  bool integer = true;
	  if ( ! InternUnit.empty() && InternUnit != "L" )
	    integer = false;
	  for ( unsigned int k=0; integer && k<Value.size(); k++ )
	    if ( Value[k] != floor( Value[k] ) )
	      integer = false;
	  setType( integer ? Integer : Number );
	}
      }
      setFormat();
    }
  }

  return *this;
}


bool operator==( const Parameter &p1, const Parameter &p2 )
{
  return ( p1.ident() == p2.ident() );
}


bool operator==( const Parameter &p, const string &ident )
{
  // XXX implement comparison with special characters ^*xxx* ...
  return ( p.ident() == ident );
}


bool Parameter::nonDefault( void ) const
{
  if ( isAnyNumber() )
    return ( DefaultValue != Value );
  else if ( isDate() )
    return ( DefaultYear != Year || DefaultMonth != Month || DefaultDay != Day );
  else if ( isTime() )
    return ( DefaultHour != Hour || DefaultMinutes != Minutes || DefaultSeconds != Seconds );
  else if ( isText() )
    return ( DefaultString != String );
  else
    return false;
}


Parameter &Parameter::setIdent( const string &ident )
{
  Ident = ident;
  return *this;
}


Parameter &Parameter::setRequest( const string &request )
{
  Request = request;
  return *this;
}


bool Parameter::types( int mask ) const
{
  return ( mask == 0 ||
	   ( mask > 0 && ( mask & type() ) > 0 ) ||
	   ( mask < 0 && ( -mask & type() ) == 0 ) );
}


Parameter &Parameter::setType( Type pt )
{
  PType = pt;
  return *this;
}


bool Parameter::flags( int selectflag ) const
{
  return ( selectflag == 0 ||
	   ( ( selectflag == NonDefault || 
	       ( flags() & abs( selectflag ) ) ) &&
	     ( selectflag > 0 || nonDefault() ) ) );
}

    
Parameter &Parameter::setFlags( int flags )
{
  Flags = flags;
  return *this;
}


Parameter &Parameter::addFlags( int flags )
{
  Flags |= flags;
  return *this;
}


Parameter &Parameter::delFlags( int flags )
{
  Flags &= ~flags;
  return *this;
}


Parameter &Parameter::clearFlags( void )
{
  Flags = 0;
  return *this;
}


bool Parameter::changed( void ) const
{
  return ( Flags & ChangedFlag );
}


Parameter &Parameter::setStyle( int style )
{
  Style = style;
  return *this;
}


Parameter &Parameter::addStyle( int style )
{
  Style |= style;
  return *this;
}


Parameter &Parameter::delStyle( int style )
{
  Style &= ~style;
  return *this;
}


Str Parameter::format( void ) const
{ 
  Warning = "";
  return Format; 
}


Parameter &Parameter::setFormat( const string &format )
{
  Warning = "";
  if ( format.empty() ) {
    if ( isNumber() )
      Format = "%g";
    else if ( isBoolean() || isInteger() )
      Format = "%.0f";
    else if ( isDate() )
      Format = "%04Y-%02m-%02d";
    else if ( isTime() )
      Format = "%02H:%02M:%02S";
    else
      Format = "%s";
  }
  else {
    // format checking: XXX still missing XXX ...
    Format = format;
  }
  return *this;
}


Parameter &Parameter::setFormat( int width, int prec, char fmt )
{
  Warning = "";

  if ( isDate() || isTime() )
    return *this;

  char format[100];

  // check format character:
  string f( "risfgeFGEub" );
  if ( f.find( fmt ) == string::npos ) {
    if ( isNumber() )
      fmt = 'g';
    else if ( isBoolean() || isInteger() )
      fmt = 'f';
    else
      fmt = 's';
    if ( fmt != '-' )
      Warning = "invalid format specifier";
  }
  
  // set zero precision for integer:
  if ( isInteger() )
    prec = 0;
  
  // create format string:
  if ( width == 0 ) {
    if ( prec < 0 )
      sprintf( format, "%%%c", fmt );
    else
      sprintf( format, "%%.%d%c", prec, fmt );
  }
  else {
    if ( prec < 0 )
      sprintf( format, "%%%d%c", width, fmt );
    else
      sprintf( format, "%%%d.%d%c", width, prec, fmt );
  }
  
  Format = format;

  return *this;
}


int Parameter::formatWidth( void ) const
{
  // XXX what about dates and times?
  return Format.formatWidth();
}


int Parameter::size( void ) const
{
  if ( isAnyNumber() )
    return Value.size();

  else if ( isText() )
    return String.size();

  else if ( isDate() || isTime() )
    return 1;

  else
    return 0;
}


bool Parameter::isText( void ) const
{
  return ( PType == Text );
}


Str Parameter::text( int index, const string &format, const string &unit ) const
{ 
  Warning = "";

  if ( index < 0 ) {
    Warning = "! warning in Parameter::text -> negative index for parameter '" +
      Ident + "' requested!";
    return "";
  }

  if ( isText() && index >= String.size() ) {
    Warning = "! warning in Parameter::text -> requested parameter '" +
      Ident + "' has only " + Str( String.size() ) + " values !";
    return "";
  }

  if ( isAnyNumber() && index >= (int)Value.size() ) {
    Warning = "! warning in Parameter::text -> requested parameter '" +
      Ident + "' has only " + Str( Value.size() ) + " values !";
    return "";
  }

  Str f( format );
  if ( f.empty() )
    f = Format;

  f.format( Ident, 'i' );
  f.format( Request, 'r' );

  string typestr = "notype";
  if ( isText() )
    typestr = "string";
  else if ( isNumber() )
    typestr = "number";
  else if ( isInteger() )
    typestr = "integer";
  else if ( isBoolean() )
    typestr = "boolean";
  else if ( isDate() )
    typestr = "date";
  else if ( isTime() )
    typestr = "time";
  else if ( isLabel() )
    typestr = "label";
  else if ( isSeparator() )
    typestr = "separator";
  f.format( typestr, 'T' );

  string u( unit );
  if ( u.empty() )
    u = OutUnit;

  if ( isAnyNumber() ) {
    double uv = changeUnit( 1.0, InternUnit, u );

    double v = index < (int)Value.size() ? Value[index] : 0.0;
    double e = index < (int)Error.size() ? Error[index] : 0.0;
    if ( e < 0 )
      e = 0.0;
    v *= uv;
    e *= uv;
    
    f.format( v, "fge" );
    f.format( e, "FGE" );
    
    string b( v != 0 ? "true" : "false" );
    f.format( b, 'b' );
  }
  else if ( isDate() ) {
    struct tm time;
    memset( &time, 0, sizeof( time ) );
    time.tm_year = Year - 1900;
    time.tm_mon = Month - 1;
    time.tm_mday = Day;
    f.format( &time );
  }
  else if ( isTime() ) {
    struct tm time;
    memset( &time, 0, sizeof( time ) );
    time.tm_hour = Hour;
    time.tm_min = Minutes;
    time.tm_sec = Seconds;
    f.format( &time );
  }

  if ( u == "1" )
    u == "";
  int up = f.format( u, 'u' );
  if ( up > 0 && u.find( '%' ) != string::npos ) {
    // unit string was replaced and contains a '%': no more formatting!
    return f;
  }

  Str s( "" );
  if ( index < String.size() )
    s = String[index];
  f.format( s.dir(), 'p' );
  f.format( s.notdir(), 'd' );
  f.format( s.name(), 'n' );
  f.format( s.extension(), 'x' );
  f.format( s, 's' );

  return f;
}


Parameter &Parameter::setText( const string &strg )
{
  return addText( strg, true );
}


Parameter &Parameter::addText( const string &strg, bool clear )
{
  Warning = "";

  // split strg:
  StrQueue sq( strg, "|" );
  if ( sq.empty() )
    sq.add( "" );

  // clear:
  if ( clear ) {
    // changed:
    if ( String.empty() || String[0] != sq[0] )
      Flags |= ChangedFlag;
    String.clear();
    Value.clear();
    Error.clear();
  }

  // add sq:
  String.add( sq );

  if ( isDate() )
    setDate( String[0], false );
  else if ( isTime() )
    setTime( String[0], false );
  else if ( ! isBlank() ) {
    // get numbers:
    for ( int k=0; k<sq.size(); k++ )
      addNumber( String[ String.size() - sq.size() + k ], "", false );
  }

  return *this;
}


Str Parameter::defaultText( int index, const string &format,
			    const string &unit ) const
{ 
  Warning = "";

  if ( index < 0 ) {
    Warning = "! warning in Parameter::defaultText -> negative index for parameter '" +
      Ident + "' requested!";
    return "";
  }

  if ( isText() && index >= DefaultString.size() ) {
    Warning = "! warning in Parameter::defaultText -> requested parameter '" +
      Ident + "' has only " + Str( DefaultString.size() ) + " values !";
    return "";
  }

  if ( isAnyNumber() && index >= (int)Value.size() ) {
    Warning = "! warning in Parameter::defaultText -> requested parameter '" +
      Ident + "' has only " + Str( Value.size() ) + " values !";
    return "";
  }

  Str f( format );
  if ( f.empty() )
    f = Format;

  f.format( Ident, 'i' );
  f.format( Request, 'r' );

  string typestr = "notype";
  if ( isText() )
    typestr = "string";
  else if ( isNumber() )
    typestr = "number";
  else if ( isInteger() )
    typestr = "integer";
  else if ( isBoolean() )
    typestr = "boolean";
  else if ( isDate() )
    typestr = "date";
  else if ( isTime() )
    typestr = "time";
  else if ( isLabel() )
    typestr = "label";
  else if ( isSeparator() )
    typestr = "separator";
  f.format( typestr, 't' );

  string u( unit );
  if ( u.empty() )
    u = OutUnit;

  if ( isDate() ) {
    struct tm time;
    memset( &time, 0, sizeof( time ) );
    time.tm_year = DefaultYear;
    time.tm_mon = DefaultMonth;
    time.tm_mday = DefaultDay;
    f.format( &time );
  }
  else if ( isTime() ) {
    struct tm time;
    memset( &time, 0, sizeof( time ) );
    time.tm_hour = DefaultHour;
    time.tm_min = DefaultMinutes;
    time.tm_sec = DefaultSeconds;
    f.format( &time );
  }
  else {
    double uv = changeUnit( 1.0, InternUnit, u );

    double v = DefaultValue[index];
    double e = 0.0;
    v *= uv;
    e *= uv;

    f.format( v, "fge" );
    f.format( e, "FGE" );

    string b( v != 0 ? "true" : "false" );
    f.format( b, 'b' );
  }

  if ( u == "1" )
    u == "";
  int up = f.format( u, 'u' );
  if ( up > 0 && u.find( '%' ) != string::npos ) {
    // unit string was replaced and contains a '%': no more formatting!
    return f;
  }

  Str s( "" );
  if ( index < DefaultString.size() )
    s = DefaultString[index];
  f.format( s.dir(), 'p' );
  f.format( s.notdir(), 'd' );
  f.format( s.name(), 'n' );
  f.format( s.extension(), 'x' );
  f.format( s, 's' );

  return f;
}


Parameter &Parameter::setDefaultText( const string &strg )
{
  Warning = "";

  DefaultString.clear();
  DefaultValue.clear();

  return addDefaultText( strg );
}


Parameter &Parameter::addDefaultText( const string &strg )
{
  Warning = "";

  // split strg:
  StrQueue sq( strg, "|" );
  if ( sq.empty() )
    sq.add( "" );

  // add sq:
  DefaultString.add( sq );

  if ( isDate() )
    setDefaultDate( DefaultString[0], false );
  else if ( isTime() )
    setDefaultTime( DefaultString[0], false );
  else {
    // get numbers:
    for ( int k=0; k<sq.size(); k++ )
      addDefaultNumber( DefaultString[ DefaultString.size() - sq.size() + k ], "", false );
  }

  return *this;
}


Parameter &Parameter::selectText( const string &strg, int add )
{
  if ( ! isText() ) {
    Warning = "! warning in Parameter::selectText -> parameter '" + 
      Ident + "' is not of type text!";
    return *this;
  }

  // split strg:
  StrQueue sq( strg, "|" );

  int inx = String.find( sq[0] );
  if ( inx < 0 ) {
    if ( add > 0 ||
	 ( add == 0 && (Style & SelectText) == 0 ) ) {
      // strg not found, add it:
      String.add( sq[0] );
      if ( String.size() > 1 )
	String.insert( sq[0] );
      Flags |= ChangedFlag;
    }
  }
  else {
    // strg found, make sure it is doubled at the front:
    if ( String.find( String[0], 1 ) > 0 ) {
      if ( inx > 0 ) {
	String[0] = String[inx];
	Flags |= ChangedFlag;
      }
    }
    else {
      String.insert( String[inx] );
      Flags |= ChangedFlag;
    }
  }

  // update numbers:
  Value.clear();
  Error.clear();
  for ( int k=0; k<String.size(); k++ )
    addNumber( String[k], "", false );
  
  return *this;
}


int Parameter::index( void ) const
{
  if ( ! isText() ) {
    Warning = "! warning in Parameter::selectText -> parameter '" + 
      Ident + "' is not of type text!";
    return 0;
  }
  int inx = String.find( String[0], 1 );
  return inx < 0 ? 0 : inx-1;
}


int Parameter::index( const string &strg ) const
{
  if ( ! isText() ) {
    Warning = "! warning in Parameter::selectText -> parameter '" + 
      Ident + "' is not of type text!";
    return 0;
  }

  int inx = String.find( strg );

  // not found:
  if ( inx < 0 )
    return -1;

  if ( inx == 0 ) {
    inx = String.find( String[0], 1 );
    return inx < 0 ? 0 : inx-1;
  }

  return inx;
}


const int NUnits = 50;
string UnitPref[NUnits] = { "Deka", "deka", "Hekto", "hekto", "kilo", "Kilo", 
			    "Mega", "mega", "Giga", "giga", "Tera", "tera", 
			    "Peta", "peta", "Exa", "exa", 
			    "Dezi", "dezi", "Zenti", "centi", "Micro", "micro", 
			    "Milli", "milli", "Nano", "nano", "Piko", "piko", 
			    "Femto", "femto", "Atto", "atto", 
			    "da", "h", "K", "k", "M", "G", "T", "P", "E", 
			    "d", "c", "mu", "u", "m", "n", "p", "f", "a" };
double UnitFac[NUnits] = {  1.0,  1.0,  2.0,  2.0,  3.0,  3.0,  
			    6.0,  6.0,  9.0,  9.0,  12.0, 12.0, 
			   15.0, 15.0, 18.0, 18.0,
			   -1.0, -1.0, -2.0, -2.0, -6.0, -6.0, 
			   -3.0, -3.0, -9.0, -9.0, -12.0, -12.0, 
			  -15.0, -15.0, -18.0, -18.0, 
			    1.0,  2.0,  3.0,  3.0,  6.0,  9.0, 12.0, 15.0, 18.0,
			   -1.0, -2.0, -6.0, -6.0, -3.0, -9.0, -12.0, -15.0, -18.0 };

double Parameter::changeUnit( double val, const Str &oldunit,
			      const Str &newunit )
{
  // disect oldUnit into value ov and unit ou:
  double ov = oldunit.number( 1.0 );
  string ou = oldunit.unit();

  // disect newUnit into value nv and unit nu:
  double nv = newunit.number( 1.0 );
  string nu = newunit.unit();

  // missing unit?
  if ( nu.empty() || ou.empty() ) {
    if ( newunit == "1" && oldunit == "%" )
      nv = 100.0;
    else if ( newunit == "%" && oldunit == "1" ) 
      nv = 0.01;
    return val*ov/nv;
  }

  // find out order of old unit:
  int k = 0;
  for ( k=0; k<NUnits; k++ )
    if ( ou.find( UnitPref[k] ) == 0 )
      break;
  double e1 = 0.0;
  if ( k < NUnits && UnitPref[k].length() < ou.length() )
    e1 = UnitFac[k];

  // find out order of new unit:
  for ( k=0; k<NUnits; k++ )
    if ( nu.find( UnitPref[k] ) == 0 )
      break;
  double e2 = 0.0;
  if ( k < NUnits && UnitPref[k].length() < nu.length() )
    e2 = UnitFac[k];

  return val * (ov/nv) * pow( 10.0, e1-e2 );
}


bool Parameter::isAnyNumber( void ) const
{
  return ( PType == Number || PType == Integer || PType == Boolean );
}


bool Parameter::isNumber( void ) const
{
  return ( PType == Number );
}


double Parameter::number( const string &unit, int index ) const
{ 
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::number -> parameter '" + 
      Ident + "' is not of type number!";
    return 0.0;
  }
  if ( index < 0 || index >= (int)Value.size() ) {
    Warning = "! warning in Parameter::number -> invalid index " +
      Str( index ) + " requested for parameter '" + Ident + "' !";
    return 0.0;
  }

  return changeUnit( Value[index], InternUnit, unit );
}


double Parameter::error( const string &unit, int index ) const
{ 
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::error -> parameter '" + 
      Ident + "' is not of type number!";
    return 0.0;
  }
  if ( index < 0 || index >= (int)Error.size() ) {
    Warning = "! warning in Parameter::error -> invalid index " +
      Str( index ) + " requested for parameter '" + Ident + "' !";
    return 0.0;
  }

  if ( Error[index] >= 0.0 ) {
    return changeUnit( Error[index], InternUnit, unit );
  }
  return Error[index];
}


Parameter &Parameter::setNumber( double number, double error, 
				 const string &unit )
{
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::setNumber -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return addNumber( number, error, unit, true, true );
}


Parameter &Parameter::setNumbers( const vector<double> &numbers,
				  const vector<double> &errors,
				  const string &unit )
{
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::setNumbers -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  if ( numbers.size() == 0 ) {
    Value.resize( 1 );
    Value[ 0 ] = 0.0;
    Error.resize( 1 );
    Error[ 0 ] = -1.0;
    String.resize( 1, "" );
    Warning = "no numbers";
  }
  else {
    string e;
    setNumber( numbers[0], errors[0], unit );
    e += Warning;
    for ( int k=1; k<(int)numbers.size(); k++ ) {
      addNumber( numbers[k], errors[k], unit );
      e += Warning;
    }
    Warning = e;
  }
  return *this;
}


Parameter &Parameter::addNumber( double number, double error, const string &unit, 
				 bool settext, bool clear )
{
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::addNumber -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }

  // unit:
  if ( InternUnit.empty() && OutUnit.empty() ) {
    if ( number != MAXDOUBLE && ! isBoolean() )
      setUnit( unit );
  }
  else if ( number != MAXDOUBLE ) {
    double u = changeUnit( 1.0, unit, InternUnit );
    number *= u;
    if ( error >= 0.0 )
      error *= u;
  }
  
  // check range:
  if ( number < Minimum - 1.0e-8 ) {
    Warning += "number=" + Str( number ) + " < Minimum=" + Str( Minimum ) + ", ";
    number = Minimum;
  }
  if ( number != MAXDOUBLE && number > Maximum + 1.0e-8 ) {
    Warning += "number=" + Str( number ) + " > Maximum=" + Str( Maximum ) +", ";
    number = Maximum;
  }
  if ( error > Maximum + 1.0e-8 ) {
    Warning += "error=" + Str( error ) + " > Maximum=" + Str( Maximum ) +", ";
    error = Maximum;
  }

  // clear:
  if ( clear ) {
    // change:
    if ( Value[0] != number )
      Flags |= ChangedFlag;
    Value.clear();
    Error.clear();
    String.clear();
  }
      
  // add number and error:
  Value.push_back( number );
  Error.push_back( error );

  // add text:
  if ( settext ) {
    String.add( Str( number ) );
    String.back() = text( String.size()-1 );
  }

  return *this;
}


Parameter &Parameter::addNumber( const Str &s, const string &unit,
				 bool settext )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::addNumber -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  double e = -1.0;
  string u=unit;
  double v = s.number( e, u, MAXDOUBLE );
  if ( u.empty() )
    u = OutUnit;
  if ( v == MAXDOUBLE ) {
    if ( s == "true" )
      v = 1.0;
    else if ( s == "false" )
      v = 0.0;
  }
  return addNumber( v, e, u, settext );
}


bool Parameter::isInteger( void ) const
{
  return ( PType == Integer );
}


long Parameter::integer( const string &unit, int index ) const
{ 
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::integer -> parameter '" + 
      Ident + "' is not of type number!";
    return 0;
  }
  return static_cast<long>( rint( number( unit, index ) ) );
}

  
Parameter &Parameter::setInteger( long number, long error, 
				  const string &unit )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::setInteger -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return setNumber( static_cast<double>( number ),
		    static_cast<double>( error ), unit );
}

  
Parameter &Parameter::addInteger( long number, long error, const string &unit )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::addInteger -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return addNumber( static_cast<double>( number ),
		    static_cast<double>( error ), unit );
}


double Parameter::defaultNumber( const string &unit, int index ) const
{ 
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::defaultNumber -> parameter '" + 
      Ident + "' is not of type number!";
    return 0.0;
  }
  if ( index < 0 || index >= (int)DefaultValue.size() ) {
    Warning = "! warning in Parameter::defaultNumber -> invalid index " +
      Str( index ) + " requested for parameter '" + Ident + "' !";
    return 0.0;
  }

  return changeUnit( DefaultValue[index], InternUnit, unit );
}


Parameter &Parameter::setDefaultNumber( double dflt, const string &unit )
{
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::setDefaultNumber -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  DefaultValue.clear();
  DefaultString.clear();
  return addDefaultNumber( dflt, unit );
}


Parameter &Parameter::addDefaultNumber( double number, const string &unit, 
					bool settext )
{
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::addDefaultNumber -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }

  // unit:
  if ( InternUnit.empty() && OutUnit.empty() ) {
    if ( number != MAXDOUBLE && ! isBoolean() )
      setUnit( unit );
  }
  else if ( number != MAXDOUBLE ) {
    double u = changeUnit( 1.0, unit, InternUnit );
    number *= u;
  }
  
  // check range:
  if ( number < Minimum - 1.0e-8 ) {
    Warning += "number=" + Str( number ) + " < Minimum=" + Str( Minimum ) + ", ";
    number = Minimum;
  }
  if ( number != MAXDOUBLE && number > Maximum + 1.0e-8 ) {
    Warning += "number=" + Str( number ) + " > Maximum=" + Str( Maximum ) +", ";
    number = Maximum;
  }
      
  // add number and error:
  DefaultValue.push_back( number );

  // add text:
  if ( settext ) {
    DefaultString.add( Str( number ) );
    DefaultString.back() = defaultText( DefaultString.size()-1 );
  }

  return *this;
}


Parameter &Parameter::addDefaultNumber( const Str &s, const string &unit,
					bool settext )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::addDefaultNumber -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  double e = -1.0;
  string u=unit;
  double v = s.number( e, u, MAXDOUBLE );
  if ( u.empty() )
    u = OutUnit;
  if ( v == MAXDOUBLE ) {
    if ( s == "true" )
      v = 1.0;
    else if ( s == "false" )
      v = 0.0;
  }
  return addDefaultNumber( v, u, settext );
}


long Parameter::defaultInteger( const string &unit, int index ) const
{ 
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::defaultInteger -> parameter '" + 
      Ident + "' is not of type number!";
    return 0.0;
  }
  return static_cast<long>( rint( defaultNumber( unit, index ) ) );
}

  
Parameter &Parameter::setDefaultInteger( long dflt, const string &unit )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::setDefaultInteger -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return setDefaultNumber( static_cast<double>( dflt ), unit );
}


Parameter &Parameter::addDefaultInteger( double number, const string &unit,
					 bool settext )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::addDefaultInteger -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return addDefaultNumber( static_cast<double>( number ), unit, settext );
}


double Parameter::minimum( const string &unit ) const
{ 
  if ( ! isAnyNumber() ) {
    Warning = "! warning in Parameter::minimum -> parameter '" + 
      Ident + "' is not of type number!";
    return 0.0;
  }
  Warning = "";
  return changeUnit( Minimum, InternUnit, unit );
}


double Parameter::maximum( const string &unit ) const
{ 
  if ( ! isAnyNumber() ) {
    Warning = "! warning in Parameter::maximum -> parameter '" + 
      Ident + "' is not of type number!";
    return 0.0;
  }
  Warning = "";
  return changeUnit( Maximum, InternUnit, unit );
}


double Parameter::step( const string &unit ) const
{ 
  if ( ! isAnyNumber() ) {
    Warning = "! warning in Parameter::step -> parameter '" + 
      Ident + "' is not of type number!";
    return 0.0;
  }
  Warning = "";
  return changeUnit( Step, InternUnit, unit );
}


Parameter &Parameter::setStep( double step )
{
  if ( ! isAnyNumber() ) {
    Warning = "! warning in Parameter::setStep -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  Warning = "";
  if ( step < 0.0 ) {
    if ( Minimum > -MAXDOUBLE &&
	 Maximum < MAXDOUBLE )
      Step = - ( Maximum - Minimum ) / step;
    else {
      Warning += "step=" + Str( step ) + " < 0, ";
      Step = 1.0;
    }
  }
  else if ( step == 0.0 ) {
    // auto set stepsize:
    if ( Minimum > -MAXDOUBLE &&
	 Maximum < MAXDOUBLE )
      Step = ( Maximum - Minimum ) / 50.0;
    else {
      Warning += "step=" + Str( step ) + " == 0, ";
      Step = 1.0;
    }
  }
  else
    Step = step;

  return *this;
}


Parameter &Parameter::setStep( long step )
{
  if ( ! isAnyNumber() ) {
    Warning = "! warning in Parameter::setStep -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return setStep( static_cast<double>( step ) );
}

  
Parameter &Parameter::setMinMax( double minimum, double maximum,
				 double step, const string &unit )
{
  if ( ! isAnyNumber() ) {
    Warning = "! warning in Parameter::setMinMax -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  Warning = "";

  double u = changeUnit( 1.0, unit, InternUnit );
  if ( minimum > -MAXDOUBLE )
    minimum *= u;
  if ( maximum < MAXDOUBLE )
    maximum *= u;
  if ( step != 0.0 )
    step *= u;

  // check range:
  if ( minimum > maximum ) {
    Warning += "minimum=" + Str( minimum ) + " > maximum=" + Str( maximum ) + ", ";
    double swap;
    swap = maximum;
    maximum = minimum;
    minimum = swap;
  }
      
  // set minimum and maximum:
  Minimum = minimum;
  Maximum = maximum;

  // set step:
  return setStep( step );
}

  
Parameter &Parameter::setMinMax( long minimum, long maximum, long step,
				 const string &unit )
{
  if ( ! isAnyNumber() ) {
    Warning = "! warning in Parameter::setMinMax -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  double min = minimum == LONG_MIN ? -MAXDOUBLE : static_cast<double>( minimum );
  double max = maximum == LONG_MAX ? MAXDOUBLE : static_cast<double>( maximum );
  double st = static_cast<double>( step );
  return setMinMax( min, max, st, unit );
}


double Parameter::floorLog10( double v )
{
  return pow( 10.0, floor( log10( v ) ) );
}


double Parameter::floor10( double v, double scale )
{
  double f = floorLog10( scale * v );
  //  v -= 0.1 * scale * v; ????
  return floor( v/f ) * f;
}


double Parameter::ceil10( double v, double scale )
{
  double f = floorLog10( scale * v );
  //  v -= 0.1 * scale * v; ????
  return ceil( v/f ) * f;
}


Str Parameter::unit( void ) const
{ 
  Warning = "";
  return InternUnit; 
}


Str Parameter::outUnit( void ) const
{ 
  Warning = "";
  return OutUnit; 
}


Parameter &Parameter::setUnit( const string &internunit, 
			       const string &outputunit )
{
  Warning = "";
  // check units ...
  if ( outputunit == "%" && internunit == "" )
    InternUnit = "1";
  else
    InternUnit = internunit;
  if ( outputunit.empty() )
    OutUnit =  InternUnit;
  else
    OutUnit = outputunit;
  return *this;
}


Parameter &Parameter::setOutUnit( const string &outputunit )
{
  Warning = "";
  if ( outputunit.empty() )
    OutUnit =  InternUnit;
  else
    OutUnit = outputunit;
  return *this;
}


bool Parameter::isBoolean( void ) const
{
  return ( PType == Boolean );
}


bool Parameter::boolean( int index ) const
{ 
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::boolean -> parameter '" + 
      Ident + "' is not of type number!";
    return false;
  }
  if ( index < 0 || index >= (int)Value.size() ) {
    Warning = "! warning in Parameter::boolean -> invalid index " +
      Str( index ) + " requested for parameter '" + Ident + "' !";
    return false;
  }

  return ( Value[index] != 0.0 ); 
}

  
Parameter &Parameter::setBoolean( bool b )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::setBoolean -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return setNumber( b ? 1.0 : 0.0 );
}


bool Parameter::defaultBoolean( void ) const
{ 
  Warning = "";
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::defaultBoolean -> parameter '" + 
      Ident + "' is not of type number!";
    return false;
  }
  return ( DefaultValue[0] != 0.0 ); 
}

  
Parameter &Parameter::setDefaultBoolean( bool dflt )
{
  if ( ! isAnyNumber() && ! isText() ) {
    Warning = "! warning in Parameter::setDefaultBoolean -> parameter '" + 
      Ident + "' is not of type number!";
    return *this;
  }
  return setDefaultNumber( dflt ? 1.0 : 0.0 );
}


bool Parameter::isDate( void ) const
{
  return ( PType == Date );
}


int Parameter::year( void ) const
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::year -> parameter '" + 
      Ident + "' is not of type date!";
    return -1;
  }
  return Year;
}


int Parameter::month( void ) const
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::month -> parameter '" + 
      Ident + "' is not of type date!";
    return 0;
  }
  return Month;
}


int Parameter::day( void ) const
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::day -> parameter '" + 
      Ident + "' is not of type date!";
    return 0;
  }
  return Day;
}


Parameter &Parameter::setDate( int year, int month, int day )
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::setDate -> parameter '" + 
      Ident + "' is not of type date!";
    return *this;
  }
  // changed:
  if ( Year != year || Month != month || Day != day )
    Flags |= ChangedFlag;
  // set:
  Year = year;
  Month = month;
  Day = day;
  return *this;
}


Parameter &Parameter::setDate( const string &date, bool settext )
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::setDate -> parameter '" + 
      Ident + "' is not of type date!";
    return *this;
  }

  // read date:
  // XXX need error checking -> better a function in Str?
  int year = ::strtol( date.substr( 0, 4 ).c_str(), 0, 10 );
  int month = ::strtol( date.substr( 5, 2 ).c_str(), 0, 10 );
  int day = ::strtol( date.substr( 8, 2 ).c_str(), 0, 10 );

  // changed:
  if ( Year != year || Month != month || Day != day )
    Flags |= ChangedFlag;

  // set:
  Year = year;
  Month = month;
  Day = day;

  // text:
  if ( settext ) {
    String.clear();
    String.add( text() );
  }

  return *this;
}


int Parameter::defaultYear( void ) const
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::defaultYear -> parameter '" + 
      Ident + "' is not of type date!";
    return 0;
  }
  return DefaultYear;
}


int Parameter::defaultMonth( void ) const
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::defaultMonth -> parameter '" + 
      Ident + "' is not of type date!";
    return 0;
  }
  return DefaultMonth;
}


int Parameter::defaultDay( void ) const
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::defaultDay -> parameter '" + 
      Ident + "' is not of type date!";
    return 0;
  }
  return DefaultDay;
}


Parameter &Parameter::setDefaultDate( int year, int month, int day )
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::setDefaultDate -> parameter '" + 
      Ident + "' is not of type date!";
    return *this;
  }
  // set:
  DefaultYear = year;
  DefaultMonth = month;
  DefaultDay = day;
  return *this;
}


Parameter &Parameter::setDefaultDate( const string &date, bool settext )
{
  if ( ! isDate() ) {
    Warning = "! warning in Parameter::setDefaultDate -> parameter '" + 
      Ident + "' is not of type date!";
    return *this;
  }

  // read date:
  // XXX need error checking -> better a function in Str?
  int year = ::strtol( date.substr( 0, 4 ).c_str(), 0, 10 );
  int month = ::strtol( date.substr( 5, 2 ).c_str(), 0, 10 );
  int day = ::strtol( date.substr( 8, 2 ).c_str(), 0, 10 );

  // set:
  DefaultYear = year;
  DefaultMonth = month;
  DefaultDay = day;


  // text:
  if ( settext ) {
    DefaultString.clear();
    DefaultString.add( defaultText() );
  }

  return *this;
}


bool Parameter::isTime( void ) const
{
  return ( PType == Time );
}


int Parameter::hour( void ) const
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::hour -> parameter '" + 
      Ident + "' is not of type time!";
    return 0;
  }
  return Hour;
}


int Parameter::minutes( void ) const
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::minutes -> parameter '" + 
      Ident + "' is not of type time!";
    return 0;
  }
  return Minutes;
}


int Parameter::seconds( void ) const
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::seconds -> parameter '" + 
      Ident + "' is not of type time!";
    return 0;
  }
  return Seconds;
}


Parameter &Parameter::setTime( int hour, int minutes, int seconds )
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::setTime -> parameter '" + 
      Ident + "' is not of type time!";
    return *this;
  }
  // changed:
  if ( Hour != hour || Minutes != minutes || Seconds != seconds )
    Flags |= ChangedFlag;
  // set:
  Hour = hour;
  Minutes = minutes;
  Seconds = seconds;
  return *this;
}


Parameter &Parameter::setTime( const string &time, bool settext )
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::setTime -> parameter '" + 
      Ident + "' is not of type time!";
    return *this;
  }

  // read time:
  // XXX need error checking -> better a function in Str?
  int hour = ::strtol( time.substr( 0, 2 ).c_str(), 0, 10 );
  int minutes = ::strtol( time.substr( 3, 2 ).c_str(), 0, 10 );
  int seconds = ::strtol( time.substr( 6, 2 ).c_str(), 0, 10 );

  // changed:
  if ( Hour != hour || Minutes != minutes || Seconds != seconds )
    Flags |= ChangedFlag;

  // set:
  Hour = hour;
  Minutes = minutes;
  Seconds = seconds;

  // text:
  if ( settext ) {
    String.clear();
    String.add( text() );
  }

  return *this;
}


int Parameter::defaultHour( void ) const
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::defaultHour -> parameter '" + 
      Ident + "' is not of type time!";
    return 0;
  }
  return DefaultHour;
}


int Parameter::defaultMinutes( void ) const
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::defaultMinutes -> parameter '" + 
      Ident + "' is not of type time!";
    return 0;
  }
  return DefaultMinutes;
}


int Parameter::defaultSeconds( void ) const
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::defaultSeconds -> parameter '" + 
      Ident + "' is not of type time!";
    return 0;
  }
  return DefaultSeconds;
}


Parameter &Parameter::setDefaultTime( int hour, int minutes, int seconds )
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::setDefaultTime -> parameter '" + 
      Ident + "' is not of type time!";
    return *this;
  }
  // set:
  DefaultHour = hour;
  DefaultMinutes = minutes;
  DefaultSeconds = seconds;
  return *this;
}


Parameter &Parameter::setDefaultTime( const string &time, bool settext )
{
  if ( ! isTime() ) {
    Warning = "! warning in Parameter::setDefaultTime -> parameter '" + 
      Ident + "' is not of type time!";
    return *this;
  }

  // read time:
  // XXX need error checking -> better a function in Str?
  int hour = ::strtol( time.substr( 0, 2 ).c_str(), 0, 10 );
  int minutes = ::strtol( time.substr( 3, 2 ).c_str(), 0, 10 );
  int seconds = ::strtol( time.substr( 6, 2 ).c_str(), 0, 10 );

  // set:
  DefaultHour = hour;
  DefaultMinutes = minutes;
  DefaultSeconds = seconds;

  // text:
  if ( settext ) {
    DefaultString.clear();
    DefaultString.add( text() );
  }

  return *this;
}


bool Parameter::isLabel( void ) const
{
  return ( PType == Label );
}


bool Parameter::isSeparator( void ) const
{
  return ( PType == Separator );
}


bool Parameter::isBlank( void ) const
{
  return ( PType == Label || PType == Separator );
}


bool Parameter::isNotype( void ) const
{
  return ( PType == NoType );
}


bool Parameter::empty( void ) const
{
  return ( PType == NoType || Ident.empty() );
}


Parameter &Parameter::setDefault( void ) 
{
  if ( isDate() ) {
    Year = DefaultYear;
    Month = DefaultMonth;
    Day = DefaultDay;
  }
  else if ( isTime() ) {
    Hour = DefaultHour;
    Minutes = DefaultMinutes;
    Seconds = DefaultSeconds;
  }
  else {
    Value = DefaultValue;
    Error.clear();
    Error.resize( Value.size(), -1 );
  }
  String = DefaultString;
  return *this;
}


Parameter &Parameter::setToDefault( void ) 
{
  if ( isDate() ) {
    DefaultYear = Year;
    DefaultMonth = Month;
    DefaultDay = Day;
  }
  else if ( isTime() ) {
    DefaultHour = Hour;
    DefaultMinutes = Minutes;
    DefaultSeconds = Seconds;
  }
  else
    DefaultValue = Value;
  DefaultString = String;
  return *this;
}


Parameter &Parameter::setActivation( const string &name, const string &value,
				     bool activate )
{
  ActivationName = name;
  ActivationValues.assign( value, "|" );
  ActivationComparison = 0;
  Activation = activate;
  if ( ActivationValues.size() > 0 && ActivationValues.front().size() > 0 &&
       ( ActivationValues.front()[0] == '=' ||
	 ActivationValues.front()[0] == '>' ||
	 ActivationValues.front()[0] == '<' ) ) {
    int inx=1;
    if ( ActivationValues.front()[0] == '=' )
      ActivationComparison |= 1;
    else if ( ActivationValues.front()[0] == '>' )
      ActivationComparison |= 2;
    else
      ActivationComparison |= 4;
    if ( ActivationValues.front().size() > 1 &&
	 ( ActivationValues.front()[1] == '=' ||
	   ActivationValues.front()[1] == '>' ) ) {
      if ( ActivationValues.front()[1] == '=' )
	ActivationComparison |= 1;
      else
	ActivationComparison |= 2;
      inx=2;
    }
    Str vs( value );
    ActivationNumber = vs.number( 0.0, inx );
  }
  return *this;
}


Parameter &Parameter::clearActivation( void )
{
  ActivationName = "";
  ActivationValues.clear();
  Activation = true;
  return *this;
}


string Parameter::activationName( void ) const
{
  return ActivationName;
}


string Parameter::activationValue( void ) const
{
  return ActivationValues.empty() ? "" : ActivationValues[0];
}


string Parameter::activationValues( void ) const
{
  string s = "";
  for ( int k=0; k<ActivationValues.size(); k++ ) {
    if ( k > 0 )
      s += '|';
    s += ActivationValues[k];
  }
  return s;
}


double Parameter::activationNumber( void ) const
{
  return ActivationNumber;
}


int Parameter::activationComparison( void ) const
{
  return ActivationComparison;
}


bool Parameter::activation( void ) const
{
  return Activation;
}


bool Parameter::testActivation( const string &value )
{
  for ( int k=0; k<ActivationValues.size(); k++ ) {
    if ( ActivationValues[k] == value )
      return true;
  }
  return false;
}


bool Parameter::testActivation( double value, double tol )
{
  bool b = true;

  switch ( ActivationComparison ) {

  case 1:
    b = ( ::fabs( ActivationNumber - value ) < tol );
    break;

  case 2:
    b = ( value > ActivationNumber );
    break;

  case 3:
    b = ( value >= ActivationNumber-tol );
    break;

  case 4:
    b = ( value < ActivationNumber );
    break;

  case 5:
    b = ( value <= ActivationNumber+tol );
    break;

  case 6:
    b = ( ::fabs( ActivationNumber - value ) >= tol );
    break;

  default:
    // string comparison:
    Str f( format() );
    f.format( value, "fge" );
    b = testActivation( f );
  }

  return b;
}


string Parameter::save( bool detailed, bool firstonly ) const
{
  string str;
  if ( isLabel() )
    str = label();
  else if ( ! isSeparator() ) {
    // identifier:
    str = ident();
    if ( detailed && ident() != request() )
      str += " (" + request() + "): ";
    else
      str += "=";

    // value:
    if ( isNumber() || isInteger() ) {
      if ( error( 0 ) >= 0.0 )
	str += text( 0, "(" + format() + "+-" + format().up() + ")" );
      else
	str += text( 0 );
      if ( ! firstonly ) {
	for ( int k=1; k<(int)Value.size(); k++ ) {
	  str += "|";
	  if ( error( k ) >= 0.0 )
	    str += text( k, "(" + format() + "+-" + format().up() + ")" );
	  else
	    str += text( k );
	}
      }
      if ( outUnit() != "1" )
	str += outUnit();
    }
    else if ( isBoolean() ) {
      str += ( boolean( 0 ) ? "true" : "false" );
      if ( ! firstonly ) {
	for ( int k=1; k<(int)Value.size(); k++ ) {
	  str += "|";
	  str += ( boolean( k ) ? "true" : "false" );
	}
      }
    }
    else if ( isDate() || isTime() ) {
      str += text( 0 );
    }
    else if ( isText() ) {
      str += text();
      if ( ! firstonly ) {
	for ( int k=1; k<(int)String.size(); k++ )
	  str += "|" + text( k );
      }
    }
  }

  return str;
}


ostream &Parameter::save( ostream &str, int width, bool detailed,
			  bool firstonly, const string &pattern ) const
{
  if ( isLabel() ) {
    if ( ( style() & TabLabel ) > 0 )
      str << setw( 0 ) << Str( '-', 5 ) << " " << Str( label() + " ", -64, '-' );
    else
      str << setw( 0 ) << label();
  }
  else if ( isSeparator() )
    str << setw( 0 ) << Str( '-', 70 );
  else {
    // identifier:
    str << Str( pattern+ident(), width );
    if ( detailed && ident() != request() )
      str << " (" << request() << "): ";
    else
      str << ": ";

    // value:
    if ( isNumber() || isInteger() ) {
      if ( error( 0 ) >= 0.0 )
	str << text( 0, "(" + format() + "+-" + format().up() + ")" );
      else
	str << text( 0 );
      if ( ! firstonly ) {
	for ( int k=1; k<(int)Value.size(); k++ ) {
	  str << "|";
	  if ( error( k ) >= 0.0 )
	    str << text( k, "(" + format() + "+-" + format().up() + ")" );
	  else
	    str << text( k );
	}
      }
      if ( outUnit() != "1" )
	str << outUnit();
    }
    else if ( isBoolean() ) {
      str << ( boolean( 0 ) ? "true" : "false" );
      if ( ! firstonly ) {
	for ( int k=1; k<(int)Value.size(); k++ ) {
	  str << "|" << ( boolean( k ) ? "true" : "false" );
	}
      }
    }
    else if ( isDate() || isTime() ) {
      str << text( 0 );
    }
    else if ( isText() ) {
      str << text();
      if ( ! firstonly ) {
	for ( int k=1; k<(int)String.size(); k++ )
	  str << "|" << text( k );
      }
    }
    else if ( isNotype() )
      str << "! no type !";
    else
      str << "! unknown type !";
  }

  return str;
}


ostream &Parameter::save( ostream &str, const string &textformat,
			  const string &numberformat, const string &boolformat,
			  const string &dateformat, const string &timeformat,
			  const string &labelformat,
			  const string &separatorformat ) const
{
  if ( isText() )
    str << text( textformat );
  else if ( isNumber() || isInteger() )
    str << text( numberformat );
  else if ( isBoolean() )
    str << text( boolformat );
  else if ( isDate() )
    str << text( dateformat );
  else if ( isTime() )
    str << text( timeformat );
  else if ( isLabel() )
    str << text( labelformat );
  else if ( isSeparator() )
    str << text( separatorformat );
  else if ( isNotype() )
    str << "! no type !";
  else
    str << "! unknown type !";

  return str;
}


ostream &operator<<( ostream &str, const Parameter &p )
{
  p.save( str );
  return str;
}


ostream &Parameter::saveXML( ostream &str, int level, int indent ) const
{
  string indstr1( level*indent, ' ' );
  string indstr2( indstr1 );
  indstr2 += string( indent, ' ' );

  if ( isLabel() )
    str << indstr1 << "<label>" << label() << "</label>\n";
  else {
    str << indstr1 << "<property>\n";
    str << indstr2 << "<name>" << ident() << "</name>\n";
    if ( isNumber() || isInteger() ) {
      string vtype = "float";
      if ( isInteger() )
	vtype = "integer";
      str << indstr2 << "<value type=\"" << vtype << "\">" << Str( number( 0 ), format() ).strip() << "</value>\n";
      if ( error( 0 ) >= 0.0 )
	str << indstr2 << "<errorvalue type=\"" << vtype << "\">" << Str( error( 0 ), format() ).strip() << "</errorvalue>\n";
      if ( ! outUnit().empty() && outUnit() != "1" )
	str << indstr2 << "<unit>" << unit() << "</unit>\n";
    }
    else if ( isBoolean() ) {
      str << indstr2 << "<value type=\"boolean\">" << ( boolean( 0 ) ? "true" : "false" ) << "</value>\n";
    }
    else if ( isDate() ) {
      str << indstr2 << "<value type=\"date\">" << text( 0, "%04Y-%02m-%02d" ) << "</value>\n";
    }
    else if ( isTime() ) {
      str << indstr2 << "<value type=\"time\">" << text( 0, "%02H:%02M:%02S" ) << "</value>\n";
    }
    else if ( isText() ) {
      str << indstr2 << "<value type=\"string\">" << text().strip() << "</value>\n";
    }
    str << indstr1 << "</property>\n";
  }

  return str;
}


Parameter &Parameter::load( Str s, const string &assignment )
{
  // clear parameter:
  clear();

  // first character of identifier:
  int n = s.findFirstNot( Str::WhiteSpace );
  if ( n >= 0 ) {
    // find colon:
    int m = s.findFirst( assignment, n );
    if ( m >= n ) {
      // last character of identifier:
      int l = n;
      if ( m > n ) {
	l = s.findLastNot( Str::WhiteSpace, m-1 );
	if ( l < n )
	  l = m;
	else
	  l++;
      }
      Str ident;
      Str request;
      if ( l <= n )
	Warning += "\"" + s.stripped() + "\": missing identifier! ";
      else {
	ident = s.substr( n, l-n );
	if ( ident.size() > 2 && ident[ident.size()-1] == ')' ) {
	  // request string:
	  n = ident.rfind( '(' );
	  if ( n >= 0 ) {
	    request = ident.mid( n+1, ident.size()-2 );
	    ident.erase( n-1 );
	  }
	}
      }

      bool num = true;
      // first character of value:
      n = s.findFirstNot( Str::WhiteSpace, m+1 );
      if ( n <= m )
	n = m+1;
      // last character of value:
      if ( n < s.size() && s[n] == '"' ) {
	num = false;
	n++;
	m = s.find( '"', n );
	if ( m<0 )
	  m = s.size();
      }
      else
	m = s.findLastNot( Str::WhiteSpace )+1;
      string val = m >= n ? s.substr( n, m-n ) : "";
      // create text parameter:
      *this = Parameter( ident, request, val );

      // check for date and time:
      if ( num && String.size() == 1 &&
	   String[0].size() == 10 &&
	   isdigit( String[0][0] ) &&
	   isdigit( String[0][1] ) &&
	   isdigit( String[0][2] ) &&
	   isdigit( String[0][3] ) &&
	   String[0][4] == '-' &&
	   isdigit( String[0][5] ) &&
	   isdigit( String[0][6] ) &&
	   String[0][7] == '-' &&
	   isdigit( String[0][8] ) &&
	   isdigit( String[0][9] ) ) {
	setType( Date );
	setDate( val );
	setUnit( "" );
      }
      else if ( num && String.size() == 1 &&
		String[0].size() == 8 &&
		isdigit( String[0][0] ) &&
		isdigit( String[0][1] ) &&
		String[0][2] == ':' &&
		isdigit( String[0][3] ) &&
		isdigit( String[0][4] ) &&
		String[0][5] == ':' &&
		isdigit( String[0][6] ) &&
		isdigit( String[0][7] ) ) {
	setType( Time );
	setTime( val );
	setUnit( "" );
      }
      else {
      
	// check for numbers:
	for ( int k=0; num && k<String.size(); k++ ) {
	  if ( String[k].number( MAXDOUBLE ) == MAXDOUBLE )
	    num = false;
	}
	if ( num ) {
	  setType( Number );
	  for ( int k=0; k<String.size(); k++ )
	    addNumber( String[ k ], "", false );
	  bool integer = true;
	  if ( ! InternUnit.empty() && InternUnit != "L" )
	    integer = false;
	  for ( unsigned int k=0; integer && k<Value.size(); k++ ) {
	    if ( Value[k] != floor( Value[k] ) )
	      integer = false;
	  }
	  setType( integer ? Integer : Number );
	}
	else {
	  // check for booleans:
	  bool b = true;
	  for ( int k=0; b && k<String.size(); k++ ) {
	    if ( String[k] != "false" && String[k] != "true" )
	      b = false;
	  }
	  if ( b ) { 
	    for ( int k=0; k<String.size(); k++ )
	      Value.push_back( String[k] == "true" ? 1.0 : 0.0 );
	    setType( Boolean );
	    setUnit( "" );
	  }
	  else
	    setType( Text );
	}
      }
      setFormat();
    }
    else {
      // no value: label or separator
      if ( s[n] == '-' ) {
	// separator
	int m = s.findFirstNot( " -", n );
	n = s.findLastNot( Str::WhiteSpace + "-" );
	string val = "";
	if ( n >= m && m >= 0 )
	  val = s.substr( m, n+1-m );
	*this = Parameter( val, true );
      }
      else {
	// label:
	int m = s.findLastNot( Str::WhiteSpace );
	string val = "";
	if ( m >= n )
	  val = s.substr( n, m+1-n );
	*this = Parameter( val, false );
      }
    }
  }
  else {
    // separator:
    *this = Parameter( "", true );
  }

  Flags |= ChangedFlag;

  return *this;
}


bool Parameter::read( const Str &s, const string &assignment )
{
  Warning = "";
  if ( *this == s.ident( 0, assignment ) ) {
    assign( s.value( 0, assignment ) );
    return true;
  }
  else
    return false;
}


bool Parameter::read( const string &ident, const string &value )
{
  Warning = "";
  if ( *this == ident ) {
    assign( value );
    return true;
  }
  else
    return false;
}


bool Parameter::read( const Parameter &p )
{
  Warning = "";
  if ( ident() == p.ident() ) {
    if ( ! String.empty() && ! p.String.empty() && String[0] != p.String[0] )
      Flags |= ChangedFlag;
    String = p.String;
    if ( isDate() ) {
      Year = p.Year;
      Month = p.Month;
      Day = p.Day;
    }
    else if ( isTime() ) {
      Hour = p.Hour;
      Minutes = p.Minutes;
      Seconds = p.Seconds;
    }
    else {
      Value.clear();
      for ( unsigned int k=0; k<p.Value.size(); k++ )
	Value.push_back( changeUnit( p.Value[k], p.InternUnit, InternUnit ) );
      Error.clear();
      for ( unsigned int k=0; k<p.Error.size(); k++ )
	Error.push_back( changeUnit( p.Error[k], p.InternUnit, InternUnit ) );
    }
    return true;
  }
  else
    return false;
}


istream &operator>>( istream &str, Parameter &p )
{
  string s;

  // get line:
  getline( str, s );
  p.load( s );

  return str;
}

}; /* namespace relacs */

