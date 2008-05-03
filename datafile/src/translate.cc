/*****************************************************************************
 *
 * translate.cc
 * 
 *
 * RELACS
 * RealTime ELectrophysiological data Acquisition, Control, and Stimulation
 * Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RELACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <relacs/translate.h>

namespace relacs {


Str translate( const Str &s, vector< Options > &opt, 
	       const string &ts, const string &dflt,
	       const TableKey *tkey )
{
  string ds = "";
  string::const_iterator sp = s.begin();
  while ( sp != s.end() ) {
    if ( *sp == '$' && sp+1 != s.end() && *(sp+1) == '(' ) {
      sp++;

      // corresponding closing bracket:
      int obx = sp - s.begin();
      int cbx = s.findBracket( obx, "(", "" );
      if ( cbx < 1 )
	cbx = s.size();

      // translate string in brackets:
      Str bs( s.substr( obx+1, cbx-obx-1 ) );
      Str bts = translate( bs, opt, ts, dflt );
      sp = s.begin() + cbx + 1;
      string::const_iterator bp = bts.begin();

      // options ident:
      int t = -1;
      if ( bp != s.end() && bp+1 != s.end() &&
	   ( *(bp+1) == ' ' || *(bp+1) == '%' || 
	     *(bp+1) == ':' || *(bp+1) == '+' ) ) {
	if ( ts.empty() ) {
	  if ( *bp == '0' )
	    t = 0;
	  else {
	    t = *bp - '1' + 1;
	    if ( t < 1 || t > 9 )
	      t = -1;
	  }
	}
	else {
	  Str::size_type c = ts.find( *bp );
	  if ( c != Str::npos )
	    t = c;
	  if ( t >= (int)opt.size() &&
	       ! ( tkey != 0 && t == int( ts.size() ) - 1 ) )
	    t = -1;
	}
	++bp;
      }

      // format and unit string:
      Str format = "";
      string unit = "";
      if ( bp != s.end() && *bp == '+' && 
	   tkey != 0 && t == int( ts.size() ) - 1  ) {
	// format key for TableKey:
	format.push_back( *bp++ );
	while ( bp != s.end() && ( *bp == '0' || ( *bp >= '1' && *bp <= '9' ) ) ) {
	  format.push_back( *bp++ );
	}
      }
      else if ( bp != s.end() && *bp == '%' ) {
	// percent:
	format.push_back( *bp++ );
	// padding:
	if ( bp != s.end() && 
	     ( *bp == '+' || *bp == '-' || 
	       ( *bp != '.' && *bp != ':' && ( *bp < '1' || *bp > '9' ) &&
		 ( bp+1 == s.end() || *(bp+1) == '.' || *(bp+1) == ':' ||
		   ( *(bp+1) >= '1' && *(bp+1) <= '9' ) ) ) ) ) {
	  format.push_back( *bp++ );
	}
	// width:
	while ( bp != s.end() && *bp >= '1' && *bp <= '9' ) {
	  format.push_back( *bp++ );
	}
	// precision:
	if ( bp != s.end() && *bp == '.' ) {
	  format.push_back( *bp++ );
	  while ( bp != s.end() && ( *bp == '0' || ( *bp >= '1' && *bp <= '9' ) ) ) {
	    format.push_back( *bp++ );
	  }
	}
	// format type:
	if ( bp != s.end() && *bp != ':' ) {
	  format.push_back( *bp++ );
	}

	// unit:
	while ( bp != s.end() && *bp != ' ' && *bp != ':' )
	  unit.push_back( *bp++ );
      }

      // default:
      string dfltstr = dflt;
      if ( *bp == ':' ) {
	dfltstr = "";
	bp++;
	if ( *bp == '"' ) {
	  bp++;
	  while ( bp != s.end() && *bp != '"' ) 
	    dfltstr.push_back( *bp++ );
	  bp++;
	}
	else {
	  while ( bp != s.end() && *bp != ' ' ) 
	    dfltstr.push_back( *bp++ );
	}
      }

      // white space:
      while ( bp != s.end() && *bp == ' ' ) 
	++bp;

      // identifier:
      string ident = bts.substr( bp - bts.begin() );

      // default format:
      if ( format.empty() )
	format = "%s";

      // search value:
      if ( t < 0 ) {
	// search in all options:
	unsigned int k=0;
	for ( k=0; k<opt.size(); k++ ) {
	  Str val = opt[k].text( ident, 0, "", format, unit );
	  if ( val != "" ) {
	    if ( val.find( "$(" ) >= 0 )
	      val = translate( val, opt, ts, dflt, tkey );
	    ds.append( val );
	    break;
	  }
	}
	if ( k >= opt.size() ) {
	  ds.append( dfltstr );
	}
      }
      else if ( tkey != 0 && t == int( ts.size() ) - 1 ) {
	// search in tablekey:
	int c = tkey->column( ident );
	if ( c >= 0 ) {
	  int o = int( format.number( 0.0 ) );
	  if ( format == "+" )
	    o = 1;
	  ds.append( Str( c+o ) );
	}
	else {
	  ds.append( dfltstr );
	}
      }
      else {
	// search in specified option:
	Str val = opt[t].text( ident, 0, dfltstr, format, unit );
	if ( val.find( "$(" ) >= 0 )
	  val = translate( val, opt, ts, dflt, tkey );
	ds.append( val );
      }

    }
    else {
      ds.push_back( *sp++ );
    }
  }

  return ds;
}


Str translate( const Str &s, Options &opt, const string &dflt )
{
  vector< Options > opts;
  opts.push_back( opt );
  return translate( s, opts, "", dflt, 0 );
}


void translate( const StrQueue &sq, StrQueue &dq, vector< Options > &opt, 
		const string &ts, const string &dflt,
		const TableKey *tkey )
{
  dq.clear();
  for ( StrQueue::const_iterator i = sq.begin(); i != sq.end(); ++i ) {
    dq.add( translate( *i, opt, ts, dflt, tkey ) );
  }
}


}; /* namespace relacs */

