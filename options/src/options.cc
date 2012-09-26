/*
  options.cc
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

#include <iomanip>
#include <relacs/options.h>

namespace relacs {


Parameter Options::Dummy = Parameter();


Options::Options( void )
  : ParentSection( 0 ),
    Name( "" ),
    Opt(),
    Secs(),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
}


Options::Options( const Options &o )
  : ParentSection( 0 ),
    Name( "" ),
    Opt(),
    Secs(),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
  assign( o );
}


Options::Options( const Options &o, int flags )
  : ParentSection( 0 ),
    Name( "" ),
    Opt(),
    Secs(),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
  assign( o, flags );
}


Options::Options( const Str &opttxt, const string &assignment,
		  const string &separator )
  : ParentSection( 0 ),
    Name( "" ),
    Opt(),
    Secs(),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
  load( opttxt, assignment, separator );
}


Options::Options( const StrQueue &sq, const string &assignment )
  : ParentSection( 0 ),
    Name( "" ),
    Opt(),
    Secs(),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
  load( sq, assignment );
}


Options::Options( istream &str, const string &assignment,
		  const string &comment, 
		  const string &stop, string *line )
  : ParentSection( 0 ),
    Name( "" ),
    Opt(),
    Secs(),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
  load( str, assignment, comment, stop, line );
}


Options::~Options( void )
{
  clear();
}


Options &Options::operator=( const Options &o )
{
  return assign( o );
}


Options &Options::assign( const Options &o )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  ParentSection = o.ParentSection;
  Opt = o.Opt;
  Secs = o.Secs;
  Notified = false;
  CallNotify = o.CallNotify;

  return *this;
}


Options &Options::append( const Options &o )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp )
    Opt.push_back( *pp );

  return *this;
}


Options &Options::insert( const Options &o, const string &atident )
{
  Warning = "";

  if ( atident.empty() ) {
    // insert at beginning of list:
    Opt.insert( Opt.begin(), o.begin(), o.end() );
  }
  else {
    // insert option at element specified by atident:

    // search element:
    iterator pp = find( atident );
    if ( pp != end() ) {
      Opt.insert( pp, o.begin(), o.end() );
    }
    else {
      // not found:
      for ( const_iterator op = o.begin(); op != o.end(); ++op ) {
	Opt.push_back( *op );
      }
    }
  }
  return *this;
}


Options &Options::assign( const Options &o, int flags )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  Opt.clear();
  ParentSection = o.ParentSection;
  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp ) {
    if ( (*pp).flags( flags ) ) {
      Opt.push_back( *pp );
    }
  }

  return *this;
}


Options &Options::copy( Options &o, int flags )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  o.Opt.clear();
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( flags ) ) {
      o.Opt.push_back( *pp );
    }
  }

  return *this;
}


Options &Options::append( const Options &o, int flags )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp ) {
    if ( (*pp).flags( flags ) )
      Opt.push_back( *pp );
  }

  return *this;
}


Options &Options::insert( const Options &o, int flags, const string &atident )
{
  Warning = "";

  if ( atident.empty() ) {
    // insert at beginning of list:
    for ( deque< Parameter >::const_reverse_iterator op = o.Opt.rbegin(); op < o.Opt.rend(); ++op ) {
      if ( (*op).flags( flags ) )
	Opt.push_front( *op );
    }
    return *this;
  }
  else {
    // insert option at element specified by atident:

    // search element:
    iterator pp = find( atident );
    if ( pp != end() ) {
      for ( deque< Parameter >::const_reverse_iterator op = o.Opt.rbegin(); op < o.Opt.rend(); ++op ) {
	if ( (*op).flags( flags ) )
	  pp = Opt.insert( pp, *op );
      }
    }
    else {
      // not found:
      for ( const_iterator op = o.begin(); op != o.end(); ++op ) {
	if ( (*op).flags( flags ) )
	  Opt.push_back( *op );
      }
    }
  }
  return *this;
}


Parameter *Options::assign( const string &ident, const string &value )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).assign( value );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::assign( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return pp != end() ? &(*pp) : 0;
}


bool operator==( const Options &o1, const Options &o2 )
{
  if ( o1.size() != o2.size() )
    return false;
  for ( Options::const_iterator p1 = o1.begin(), p2 = o2.begin();
	p1 != o1.end() && p2 != o2.end();
	++p1, ++p2 ) {
    if ( (*p1).ident() != (*p2).ident() )
      return false;
    if ( (*p1).text() != (*p2).text() )
      return false;
  }
  return true; // all parameter are equal
}


bool operator==( const Options &o, const string &name )
{
  // XXX implement comparison with special characters ^*xxx* ...
  return ( o.name() == name );
}


bool operator<( const Options &o1, const Options &o2 )
{
  if ( o1.size() < o2.size() )
    return true;
  if ( o1.size() > o2.size() )
    return false;
  for ( Options::const_iterator p1 = o1.begin(), p2 = o2.begin();
	p1 != o1.end() && p2 != o2.end();
	++p1, ++p2 ) {
    if ( (*p1).ident() < (*p2).ident() )
      return true;
    else if ( (*p1).ident() > (*p2).ident() )
      return false;
    if ( (*p1).text() < (*p2).text() )
      return true;
    else if ( (*p1).text() > (*p2).text() )
      return false;
  }
  return false; // all parameter are equal
}


Options *Options::parentSection( void )
{
  return ParentSection;
}


const Options *Options::parentSection( void ) const
{
  return ParentSection;
}


void Options::setParentSection( Options *parentsection )
{
  ParentSection = parentsection;
}


string Options::name( void ) const
{
  return Name;
}


void Options::setName( const string &name )
{
  Name = name;
}


const Parameter &Options::operator[]( int i ) const
{
  Warning = "";

  if ( i >= 0 && i < (int)Opt.size() )
    return Opt[i];

  Warning = "index " + Str( i ) + " does not exist!";
#ifndef NDEBUG
  cerr << "!warning in Options::operator[" << i << "] const -> " << Warning << '\n';
#endif
  Dummy = Parameter();
  return Dummy;
}


Parameter &Options::operator[]( int i )
{
  Warning = "";

  if ( i >= 0 && i < (int)Opt.size() )
    return Opt[i];

  Warning = "index " + Str( i ) + " does not exist!";
#ifndef NDEBUG
  cerr << "!warning in Options::operator[" << i << "] -> " << Warning << '\n';
#endif
  Dummy = Parameter();
  return Dummy;
}


const Parameter &Options::operator[]( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::operator[]( const string &ident )
{
  iterator pp = find( ident );
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Options::const_iterator Options::find( const string &pattern ) const
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }

  /*
  // NEW IMPLEMENTATION FOR SECTIONS:
  size_t pi = pattern.find( '<' );
  if ( pi == npos ) {
    // search in key-value pairs:
    StrQueue sq;
    sq.assign( pattern, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search element:
      for ( const_iterator pp = begin(); pp != end(); ++pp ) {
	if ( *pp == sq[s] )
	  return pp;
      }
      // search in subsections:
      for ( const_section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	const_iterator pp = sp->find( sq[s] );
	if ( pp != sp->end() )
	  return pp;
      }
    }
  }
  else {
    // search in sections:
    string search = pattern.substr( 0, pi );
    string subsearch = pattern.substr( pi+1 );
    StrQueue sq;
    sq.assign( search, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      for ( const_section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	if ( *sp == sq[s] )
	  return sp->find( subsearch );
      }
      // search in subsection:
      for ( const_section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	const_iterator pp = sp->find( sq[s] + '>' + subsearch );
	if ( pp != sp->end() )
	  return pp;
      }
    }
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
  */

  const_iterator fp = end();
  bool found = false;
  for ( int k=0; k<3 && ! found; k++ ) {

    const_iterator pp = begin();
    fp = end();

    // split pattern:
    StrQueue pq;
    if ( k < 1 ) {
      pq.assign( pattern, ">" );
      for ( int j=0; j<pq.size(); j++ ) {
	while ( j<pq.size() && pq[j].empty() )
	  pq.erase( j );
      }
      if ( pq.empty() )
	continue;
    }
    else {
      pq.assign( pattern, "" );
    }

    for ( int p = 0; p<pq.size(); p++ ) {

      // split subpattern:
      StrQueue sq;
      if ( k < 2 ) {
	sq.assign( pq[p], "|" );
	for ( int j=0; j<sq.size(); j++ ) {
	  while ( j<sq.size() && sq[j].empty() )
	    sq.erase( j );
	}
	if ( sq.empty() )
	  continue;
      }
      else
	sq.assign( pq[p], "" );

      // search:
      found = false;
      for ( int s=0; s<sq.size(); s++ ) {
	// search element:
	for ( const_iterator sp = pp; sp != end(); ++sp ) {
	  if ( *sp == sq[s] ) {
	    found = true;
	    fp = sp;
	    pp = sp+1;
	    break;
	  }
	}
	if ( found ) 
	  break;
      }

    }
  }

  // nothing found:
  if ( ! found )
    Warning = "requested option '" + pattern + "' not found!";
  return fp;
}


Options::iterator Options::find( const string &pattern )
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }


  /*
  // NEW IMPLEMENTATION FOR SECTIONS:
  size_t pi = pattern.find( '<' );
  if ( pi == npos ) {
    // search in key-value pairs:
    StrQueue sq;
    sq.assign( pattern, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search element:
      for ( iterator pp = begin(); pp != end(); ++pp ) {
	if ( *pp == sq[s] )
	  return pp;
      }
      // search in subsections:
      for ( section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	iterator pp = sp->find( sq[s] );
	if ( pp != sp->end() )
	  return pp;
      }
    }
  }
  else {
    // search in sections:
    string search = pattern.substr( 0, pi );
    string subsearch = pattern.substr( pi+1 );
    StrQueue sq;
    sq.assign( search, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      for ( section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	if ( *sp == sq[s] )
	  return sp->find( subsearch );
      }
      // search in subsection:
      for ( section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	iterator pp = sp->find( sq[s] + '>' + subsearch );
	if ( pp != sp->end() )
	  return pp;
      }
    }
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
  */


  iterator fp = end();
  bool found = false;
  for ( int k=0; k<3 && ! found; k++ ) {

    iterator pp = begin();
    fp = end();

    // split pattern:
    StrQueue pq;
    if ( k < 1 ) {
      pq.assign( pattern, ">" );
      for ( int j=0; j<pq.size(); j++ ) {
	while ( j<pq.size() && pq[j].empty() )
	  pq.erase( j );
      }
      if ( pq.empty() )
	continue;
    }
    else {
      pq.assign( pattern, "" );
    }

    for ( int p = 0; p<pq.size(); p++ ) {

      // split subpattern:
      StrQueue sq;
      if ( k < 2 ) {
	sq.assign( pq[p], "|" );
	for ( int j=0; j<sq.size(); j++ ) {
	  while ( j<sq.size() && sq[j].empty() )
	    sq.erase( j );
	}
	if ( sq.empty() )
	  continue;
      }
      else
	sq.assign( pq[p], "" );

      // search:
      found = false;
      for ( int s=0; s<sq.size(); s++ ) {
	// search element:
	for ( iterator sp = pp; sp != end(); ++sp ) {
	  if ( *sp == sq[s] ) {
	    found = true;
	    fp = sp;
	    pp = sp+1;
	    break;
	  }
	}
	if ( found ) 
	  break;
      }

    }
  }

  // nothing found:
  if ( ! found )
    Warning = "requested option '" + pattern + "' not found!";
  return fp;
}


Options::const_iterator Options::rfind( const string &pattern ) const
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }

  /*
  // NEW IMPLEMENTATION FOR SECTIONS:
  size_t pi = pattern.find( '<' );
  if ( pi == npos ) {
    // search in key-value pairs:
    StrQueue sq;
    sq.assign( pattern, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search element:
      const_iterator pp = end();
      if ( pp != begin() ) {
	do {
	  --pp;
	  if ( *pp == sq[s] )
  	    return pp;
	} while ( pp != begin() );
      }
      // search in subsections:
      const_section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
        do {
	  --sp;
	  const_iterator pp = sp->rfind( sq[s] );
	  if ( pp != sp->end() )
	    return pp;
	} while ( sp != sectionsBegin() );
      }
    }
  }
  else {
    // search in sections:
    string search = pattern.substr( 0, pi );
    string subsearch = pattern.substr( pi+1 );
    StrQueue sq;
    sq.assign( search, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      const_section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
	  if ( *sp == sq[s] ) {
  	    const_iterator pp = sp->rfind( subsearch );
	    if ( pp != sp->end() )
	    return pp;
          }
	} while ( sp != sectionsBegin() );
      }
      // search in subsection:
      const_section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
  	  const_iterator pp = sp->rfind( sq[s] + '>' + subsearch );
	  if ( pp != sp->end() )
	    return pp;
	} while ( sp != sectionsBegin() );
      }
    }
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
  */

  const_iterator fp = end();
  bool found = false;
  for ( int k=0; k<3 && ! found; k++ ) {

    const_iterator pp = begin();
    fp = end();

    // split pattern:
    StrQueue pq;
    if ( k < 1 ) {
      pq.assign( pattern, ">" );
      for ( int j=0; j<pq.size(); j++ ) {
	while ( j<pq.size() && pq[j].empty() )
	  pq.erase( j );
      }
      if ( pq.empty() )
	continue;
    }
    else {
      pq.assign( pattern, "" );
    }

    for ( int p = 0; p<pq.size(); p++ ) {

      // split subpattern:
      StrQueue sq;
      if ( k < 2 ) {
	sq.assign( pq[p], "|" );
	for ( int j=0; j<sq.size(); j++ ) {
	  while ( j<sq.size() && sq[j].empty() )
	    sq.erase( j );
	}
	if ( sq.empty() )
	  continue;
      }
      else
	sq.assign( pq[p], "" );

      // search:
      found = false;
      for ( int s=0; s<sq.size(); s++ ) {
	// search element:
	const_iterator sp = end();
	if ( sp == pp )
	  break;
	do {
	  --sp;
	  if ( *sp == sq[s] ) {
	    found = true;
	    fp = sp;
	    pp = sp+1;
	    break;
	  }
	} while ( sp != pp );
	if ( found ) 
	  break;
      }

    }
  }

  // nothing found:
  if ( ! found )
    Warning = "requested option '" + pattern + "' not found!";
  return fp;
}


Options::iterator Options::rfind( const string &pattern )
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }

  /*
  // NEW IMPLEMENTATION FOR SECTIONS:
  size_t pi = pattern.find( '<' );
  if ( pi == npos ) {
    // search in key-value pairs:
    StrQueue sq;
    sq.assign( pattern, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search element:
      iterator pp = end();
      if ( pp != begin() ) {
	do {
	  --pp;
	  if ( *pp == sq[s] )
  	    return pp;
	} while ( pp != begin() );
      }
      // search in subsections:
      section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
        do {
	  --sp;
	  iterator pp = sp->rfind( sq[s] );
	  if ( pp != sp->end() )
	    return pp;
	} while ( sp != sectionsBegin() );
      }
    }
  }
  else {
    // search in sections:
    string search = pattern.substr( 0, pi );
    string subsearch = pattern.substr( pi+1 );
    StrQueue sq;
    sq.assign( search, "|" );
    for ( int j=0; j<sq.size(); ) {
      if ( sq[j].empty() )
	sq.erase( j );
      else
	j++;
    }
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
	  if ( *sp == sq[s] ) {
  	    iterator pp = sp->rfind( subsearch );
	    if ( pp != sp->end() )
	    return pp;
          }
	} while ( sp != sectionsBegin() );
      }
      // search in subsection:
      section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
  	  iterator pp = sp->rfind( sq[s] + '>' + subsearch );
	  if ( pp != sp->end() )
	    return pp;
	} while ( sp != sectionsBegin() );
      }
    }
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
  */

  iterator fp = end();
  bool found = false;
  for ( int k=0; k<3 && ! found; k++ ) {

    iterator pp = begin();
    fp = end();

    // split pattern:
    StrQueue pq;
    if ( k < 1 ) {
      pq.assign( pattern, ">" );
      for ( int j=0; j<pq.size(); j++ ) {
	while ( j<pq.size() && pq[j].empty() )
	  pq.erase( j );
      }
      if ( pq.empty() )
	continue;
    }
    else {
      pq.assign( pattern, "" );
    }

    for ( int p = 0; p<pq.size(); p++ ) {

      // split subpattern:
      StrQueue sq;
      if ( k < 2 ) {
	sq.assign( pq[p], "|" );
	for ( int j=0; j<sq.size(); j++ ) {
	  while ( j<sq.size() && sq[j].empty() )
	    sq.erase( j );
	}
	if ( sq.empty() )
	  continue;
      }
      else
	sq.assign( pq[p], "" );

      // search:
      found = false;
      for ( int s=0; s<sq.size(); s++ ) {
	// search element:
	iterator sp = end();
	if ( sp == pp )
	  break;
	do {
	  --sp;
	  if ( *sp == sq[s] ) {
	    found = true;
	    fp = sp;
	    pp = sp+1;
	    break;
	  }
	} while ( sp != pp );
	if ( found ) 
	  break;
      }

    }
  }

  // nothing found:
  if ( ! found )
    Warning = "requested option '" + pattern + "' not found!";
  return fp;
}


Str Options::request( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Str s = (*pp).request();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::request( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setRequest( const string &ident, const string &request )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setRequest( request );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setRequest( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter::Type Options::type( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Parameter::Type t = (*pp).type();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::type( " << ident << " ) -> " << Warning << '\n';
#endif
    return t;
  }
  else
    return Parameter::NoType;
}


Parameter &Options::setType( const string &ident, Parameter::Type type )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setType( type );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setType( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


int Options::flags( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    int m = (*pp).flags();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::flags( " << ident << " ) -> " << Warning << '\n';
#endif
    return m;
  }
  else
    return 0;
}


Parameter &Options::setFlags( const string &ident, int flags )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setFlags( flags );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setFlags( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addFlags( const string &ident, int flags )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).addFlags( flags );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::addFlags( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::delFlags( const string &ident, int flags )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).delFlags( flags );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::delFlags( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::clearFlags( const string &ident )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).clearFlags();
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::clearFlags( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


bool Options::changed( const string &ident )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    return (*pp).changed();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::changed( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return false;
}


int Options::style( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    int m = (*pp).style();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::style( " << ident << " ) -> " << Warning << '\n';
#endif
    return m;
  }
  else
    return 0;
}


Parameter &Options::setStyle( const string &ident, int style )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setStyle( style );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setStyle( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addStyle( const string &ident, int style )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).addStyle( style );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::addStyle( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::delStyle( const string &ident, int style )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).delStyle( style );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::delStyle( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


int Options::size( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    int s = (*pp).size();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::size( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return 0;
}


Str Options::format( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Str s = (*pp).format();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::format( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setFormat( const string &ident, int width, int prec, char fmt )
{
  iterator pp = find( ident );
  // set values:
  if ( pp != end() ) {
    (*pp).setFormat( width, prec, fmt );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setFormat( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::setFormat( const string &ident, const string &format )
{
  iterator pp = find( ident );
  // set values:
  if ( pp != end() ) {
    (*pp).setFormat( format );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setFormat( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


int Options::formatWidth( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    int w = (*pp).formatWidth();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::formatWidth( " << ident << " ) -> " << Warning << '\n';
#endif
    return w;
  }
  else
    return 0;
}


Parameter &Options::add( const Parameter &np )
{
  Warning = "";
  Opt.push_back( np );
  Opt.back().setParentSection( this );
  return Opt.back();
}


Parameter &Options::insert( const Parameter &np, const string &atident )
{
  Warning = "";

  if ( atident.empty() ) {
    // insert at beginning of list:
    Opt.push_front( np );
    Opt.front().setParentSection( this );
    return Opt.front();
  }
  else {
    // insert option at element specified by atident:

    // search element:
    iterator pp = find( atident );
    if ( pp != end() ) {
      Parameter &p = *Opt.insert( pp, np );
      p.setParentSection( this );
      return p;
    }
    else {
      // not found:
      Opt.push_back( np );
      Opt.back().setParentSection( this );
      return Opt.back();
    }
  }
}


Parameter &Options::addText( const string &ident, const string &request,  
			     const string &dflt, int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addText( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertText( const string &ident, const string &atident, 
				const string &request, const string &dflt,
				int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertText( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Str Options::text( const string &ident, int index, const string &dflt,
		   const string &format, const string &unit ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Str s = (*pp).text( index, format, unit );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::text( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return dflt;
}


Parameter &Options::setText( const string &ident, const string &strg )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setText( strg );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setText( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::pushText( const string &ident, const string &strg )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).addText( strg );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::pushText( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setText( const string &ident, const Parameter &p )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setText( p );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setText( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Str Options::defaultText( const string &ident,
			     const string &format, const string &unit ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Str s = (*pp).defaultText( format, unit );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultText( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setDefaultText( const string &ident, const string &dflt )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDefaultText( dflt );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultText( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addSelection( const string &ident,
				  const string &request,  
				  const string &selection,
				  int flags, int style )
{
  return addText( ident, request, selection,
		  flags, style | Parameter::SelectText );
}


Parameter &Options::insertSelection( const string &ident,
				     const string &atident, 
				     const string &request,
				     const string &selection,
				     int flags, int style )
{
  return insertText( ident, atident, request, selection,
		     flags, style | Parameter::SelectText );
}


Parameter &Options::selectText( const string &ident, const string &strg,
				int add )
{
  iterator pp = find( ident );
  if ( pp != end() ) {
    (*pp).selectText( strg, add );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::selectText( " << ident << " ) -> " << Warning << '\n';
#endif

    // notify the change:
    if ( CallNotify && ! Notified ) {
      Notified = true;
      notify();
      Notified = false;
    }

    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


int Options::index( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    int inx = (*pp).index();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::index( " << ident << " ) -> " << Warning << '\n';
#endif
    return inx;
  }
  else
    return -1;
}


int Options::index( const string &ident, const string &strg ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    int inx = (*pp).index( strg );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::index( " << ident << " ) -> " << Warning << '\n';
#endif
    return inx;
  }
  else
    return -1;
}


Parameter &Options::addNumber( const string &ident, const string &request,  
			       double dflt, double minimum, double maximum,
			       double step,
			       const string &unit, const string &outputunit, 
			       const string &format, int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, -1.0, minimum, maximum, 
		step, unit, outputunit, format, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addNumber( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertNumber( const string &ident, const string &atident, 
				  const string &request, double dflt, 
				  double minimum, double maximum, double step,
				  const string &unit, const string &outputunit, 
				  const string &format, int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, -1.0, minimum, maximum, 
		step, unit, outputunit, format, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertNumber( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


double Options::number( const string &ident, double dflt,
			const string &unit, int index ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    double v = (*pp).number( unit, index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::number( " << ident << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return dflt;
}


double Options::error( const string &ident, const string &unit, int index ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    double v = (*pp).error( unit, index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::error( " << ident << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


Parameter &Options::setNumber( const string &ident, double number, 
			       double error, const string &unit )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setNumber( number, error, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setNumber( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::pushNumber( const string &ident, double number, 
				double error, const string &unit )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).addNumber( number, error, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::pushNumber( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setNumber( const string &ident, const Parameter &p )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setNumber( p );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setNumber( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


double Options::defaultNumber( const string &ident, const string &unit ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    double v = (*pp).defaultNumber( unit );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultNumber( " << ident << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


Parameter &Options::setDefaultNumber( const string &ident, double dflt, 
				      const string &unit )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDefaultNumber( dflt, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultNumber( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


double Options::minimum( const string &ident, const string &unit ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    double v = (*pp).minimum( unit );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::minimum( " << ident << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


double Options::maximum( const string &ident, const string &unit ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    double v = (*pp).maximum( unit );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::maximum( " << ident << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


double Options::step( const string &ident, const string &unit ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    double v = (*pp).step( unit );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::step( " << ident << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


Parameter &Options::setStep( const string &ident, double step, const string &unit )
{
  iterator pp = find( ident );
  // set values:
  if ( pp != end() ) {
    (*pp).setStep( step, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setStep( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::setMinMax( const string &ident, double minimum, 
			       double maximum, double step, const string &unit )
{
  iterator pp = find( ident );
  // set values:
  if ( pp != end() ) {
    (*pp).setMinMax( minimum, maximum, step, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setMinMax( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Str Options::unit( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Str s = (*pp).unit();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::unit( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Str Options::outUnit( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Str s = (*pp).outUnit();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::outUnit( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setUnit( const string &ident, const string &internunit, 
			     const string &outunit )
{
  iterator pp = find( ident );
  // set values:
  if ( pp != end() ) {
    (*pp).setUnit( internunit, outunit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setUnit( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::setOutUnit( const string &ident, 
				const string &outputunit )
{
  iterator pp = find( ident );
  // set values:
  if ( pp != end() ) {
    (*pp).setOutUnit( outputunit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setOutUnit( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::changeUnit( const string &ident, const string &internunit )
{
  iterator pp = find( ident );
  // set values:
  if ( pp != end() ) {
    (*pp).changeUnit( internunit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::changeUnit( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addInteger( const string &ident, const string &request,  
				long dflt, long minimum, long maximum, long step,
				const string &unit, const string &outputunit, 
				int width, int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, -1L, minimum, maximum, 
		step, unit, outputunit, width, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addInteger( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertInteger( const string &ident, const string &atident, 
				   const string &request, long dflt, 
				   long minimum, long maximum, long step,
				   const string &unit, const string &outputunit, 
				   int width, int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, -1L, minimum, maximum, 
		step, unit, outputunit, width, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertInteger( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


long Options::integer( const string &ident,
		       const string &unit, long dflt, int index ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    long i = (*pp).integer( unit, index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::integer( " << ident << " ) -> " << Warning << '\n';
#endif
    return i;
  }
  else
    return dflt;
}


Parameter &Options::setInteger( const string &ident, long number, long error,
				const string &unit )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setInteger( number, error, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setInteger( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::pushInteger( const string &ident, long number, long error,
				 const string &unit )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).addInteger( number, error, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::pushInteger( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setInteger( const string &ident, const Parameter &p )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setInteger( p );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setInteger( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


long Options::defaultInteger( const string &ident, const string &unit ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    long i = (*pp).defaultInteger( unit );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultInteger( " << ident << " ) -> " << Warning << '\n';
#endif
    return i;
  }
  else
    return 0;
}


Parameter &Options::setDefaultInteger( const string &ident, long dflt, 
				 const string &unit )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDefaultInteger( dflt, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultInteger( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::setMinMax( const string &ident, long minimum, 
			       long maximum, long step, const string &unit )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setMinMax( minimum, maximum, step, unit );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setMinMax( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addBoolean( const string &ident, const string &request,  
				bool dflt, int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addBoolean( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertBoolean( const string &ident, const string &atident, 
				   const string &request, bool dflt, int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, dflt, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertBoolean( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


bool Options::boolean( const string &ident, bool dflt, int index ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    bool b = (*pp).boolean( index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::boolean( " << ident << " ) -> " << Warning << '\n';
#endif
    return b;
  }
  else
    return dflt;
}


Parameter &Options::setBoolean( const string &ident, bool b )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setBoolean( b );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setBoolean( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setBoolean( const string &ident, const Parameter &p )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setBoolean( p );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setBoolean( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


bool Options::defaultBoolean( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    bool b = (*pp).defaultBoolean();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultBoolean( " << ident << " ) -> " << Warning << '\n';
#endif
    return b;
  }
  else
    return false;
}


Parameter &Options::setDefaultBoolean( const string &ident, bool dflt )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDefaultBoolean( dflt );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultBoolean( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addDate( const string &ident, const string &request,  
			     int year, int month, int day,
			     int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, Parameter::Date,
		year, month, day, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addDate( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertDate( const string &ident, const string &atident, 
				const string &request,
				int year, int month, int day,
				int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, Parameter::Date,
		year, month, day, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertDate( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


const Parameter &Options::date( const string &ident, int index,
				int &year, int &month, int &day ) const
{
  year = 0;
  month = 0;
  day = 0;
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    (*pp).date( year, month, day, index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::date( " << ident << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDate( const string &ident,
			     int year, int month, int day )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDate( year, month, day );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setDate( const string &ident, const string &date )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDate( date );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setDate( const string &ident, const struct tm &date )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDate( date );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setDate( const string &ident, const time_t &time )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDate( time );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setCurrentDate( const string &ident )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setCurrentDate();
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setCurrentDate( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setDate( const string &ident, const Parameter &p )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDate( p );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


const Parameter &Options::defaultDate( const string &ident, int index,
				       int &year, int &month, int &day ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    (*pp).defaultDate( year, month, day, index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultDate( " << ident << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDefaultDate( const string &ident,
				    int year, int month, int day )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDefaultDate( year, month, day );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultDate( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addTime( const string &ident, const string &request,  
			     int hour, int minutes, int seconds,
			     int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, Parameter::Time,
		hour, minutes, seconds, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addTime( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertTime( const string &ident, const string &atident, 
				const string &request,
				int hour, int minutes, int seconds,
				int flags, int style )
{
  // new parameter:
  Parameter np( ident, request, Parameter::Time,
		hour, minutes, seconds, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertTime( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


const Parameter &Options::time( const string &ident, int index,
				int &hour, int &minutes, int &seconds ) const
{
  hour = 0;
  minutes = 0;
  seconds = 0;
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    (*pp).time( hour, minutes, seconds, index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::time( " << ident << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setTime( const string &ident,
			     int hour, int minutes, int seconds )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setTime( hour, minutes, seconds );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setTime( const string &ident, const string &time )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setTime( time );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setTime( const string &ident, const struct tm &time )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setTime( time );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setTime( const string &ident, const time_t &time )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setTime( time );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setCurrentTime( const string &ident )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setCurrentTime();
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setCurrentTime( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::setTime( const string &ident, const Parameter &p )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setTime( p );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


const Parameter &Options::defaultTime( const string &ident, int index,
				       int &hour, int &minutes, int &seconds ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    (*pp).defaultTime( hour, minutes, seconds, index );
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultTime( " << ident << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDefaultTime( const string &ident,
				    int hour, int minutes, int seconds )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDefaultTime( hour, minutes, seconds );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultTime( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Parameter &Options::addLabel( const string &ident, int flags, int style )
{
  // new parameter:
  Parameter np( ident, false, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addLabel( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertLabel( const string &ident, const string &atident,
				 int flags, int style )
{
  // new parameter:
  Parameter np( ident, false, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertLabel( " << ident << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Str Options::label( const string &ident ) const
{
  const_iterator pp = find( ident );
  if ( pp != end() ) {
    Str s = (*pp).label();
    Warning += (*pp).warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::label( " << ident << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setLabel( const string &ident, const string &label )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setLabel( label );
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setLabel( " << ident << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *pp;
}


Parameter &Options::addSeparator( int flags, int style )
{
  // new parameter:
  Parameter np( "", true, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addSeparator() -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertSeparator( const string &atident, int flags, int style )
{
  // new parameter:
  Parameter np( "", true, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atident );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertSeparator() -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::setDefault( const string &ident )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setDefault();
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefault( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Options &Options::setDefaults( int flags )
{
  Warning = "";
  // set defaults:
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( flags ) ) {
      (*pp).setDefault();
    }
  }
  return *this;
}


Parameter &Options::setToDefault( const string &ident )
{
  iterator pp = find( ident );
  // set value:
  if ( pp != end() ) {
    (*pp).setToDefault();
    Warning += (*pp).warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setToDefault( " << ident << " ) -> " << Warning << '\n';
  }
#endif
  return *pp;
}


Options &Options::setToDefaults( int flags )
{
  Warning = "";
  // set defaults:
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( flags ) ) {
      (*pp).setToDefault();
    }
  }
  return *this;
}


Options &Options::takeFirst( const string &ident )
{
  Warning = "";

  if ( ident.empty() ) {
    for ( iterator ip = begin(); ip != end(); ++ip )
      if ( !ip->isBlank() ) {
	for ( iterator pp = ip+1; pp != end(); )
	  if ( *pp == *ip ) {
	    // delete option:
	    pp = Opt.erase( pp );
	  }
	  else
	    ++pp;
      }
  }
  else {
    iterator pp = find( ident );
    if ( pp != end() ) {
      for ( ++pp; pp != end(); )
	if ( *pp == ident ) {
	  // delete option:
	  pp = Opt.erase( pp );
	}
	else
	  ++pp;
    }
  }
  return *this;
}


Options &Options::takeLast( const string &ident )
{
  Warning = "";

  if ( size() < 2 )
    return *this;

  if ( ident.empty() ) {
    for ( iterator ip = end()-1; ip != begin(); --ip ) {
      if ( !ip->isBlank() ) {
	iterator pp = ip;
	do {
	  --pp;
	  if ( *pp == *ip ) {
	    // delete option:
	    pp = Opt.erase( pp );
	  }
	} while ( pp != begin() );
      }
    };
  }
  else {
    iterator pp = rfind( ident );
    if ( pp != end() && pp != begin() ) {
      do {
	--pp;
	if ( *pp == ident ) {
	  // delete option:
	  pp = Opt.erase( pp );
	}
      } while ( pp != begin() );
    }
  }
  return *this;
}


Options &Options::combineFirst( const string &ident )
{
  Warning = "";

  if ( ident.empty() ) {
    for ( iterator ip = begin(); ip != end(); ++ip )
      if ( !ip->isBlank() ) {
	for ( iterator pp = ip+1; pp != end(); )
	  if ( *pp == *ip ) {
	    (*ip).addText( (*pp).text() );
	    // delete option:
	    pp = Opt.erase( pp );
	  }
	  else
	    ++pp;
      }
  }
  else {
    iterator ip = find( ident );
    if ( ip != end() ) {
      for ( iterator pp=ip+1; pp != end(); )
	if ( *pp == ident ) {
	  (*ip).addText( (*pp).text() );
	  // delete option:
	  pp = Opt.erase( pp );
	}
	else
	  ++pp;
    }
  }
  return *this;
}


Options &Options::combineLast( const string &ident )
{
  Warning = "";

  if ( size() < 2 )
    return *this;

  if ( ident.empty() ) {
    for ( iterator ip = end()-1; ip != begin(); --ip ) {
      if ( !ip->isBlank() ) {
	iterator pp = ip;
	do {
	  --pp;
	  if ( *pp == *ip ) {
	    (*ip).addText( (*pp).text() );
	    // delete option:
	    pp = Opt.erase( pp );
	  }
	} while ( pp != begin() );
      }
    };
  }
  else {
    iterator ip = rfind( ident );
    if ( ip != end() && ip != begin() ) {
      iterator pp = ip;
      do {
	--pp;
	if ( *pp == ident ) {
	  (*ip).addText( (*pp).text() );
	  // delete option:
	  pp = Opt.erase( pp );
	}
      } while ( pp != begin() );
    }
  }
  return *this;
}


Options &Options::erase( Options::iterator p )
{
  if ( p != end() )
    Opt.erase( p );
  return *this;
}


Options &Options::erase( const string &ident )
{
  Warning = "";

  bool erased = false;
  iterator pp = end();
  while ( (pp = find( ident )) != end() ) {
    Opt.erase( pp );
    erased = true;
  }

  // option not found:
  if ( ! erased )
    Warning = "requested option '" + ident + "' not found!";

  return *this;
}


Options &Options::erase( int selectflag )
{
  Warning = "";
  // search element:
  for ( iterator pp = begin(); pp != end(); ) {
    if ( (*pp).flags( selectflag ) )
      pp = Opt.erase( pp );
    else
      ++pp;
  }

  return *this;
}


Options &Options::pop( void )
{
  Warning = "";
  if ( ! Opt.empty() )
    Opt.pop_back();

  return *this;
}


Options &Options::clear( void )
{
  Warning = "";
  Opt.clear();
  return *this;
}


Options &Options::strip( void )
{
  // delete all Label and Separator:
  for ( iterator pp = begin(); pp != end(); ) {
    if ( pp->isBlank() ) {
      // delete option:
      pp = Opt.erase( pp );
    }
    else
      ++pp;
  }
  return *this;
}


int Options::size( void ) const
{
  Warning = "";
  return Opt.size();
}


int Options::size( int flags ) const
{
  Warning = "";
  int n=0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( flags ) ) {
      n++;
    }
  }
  return n;
}


bool Options::empty( void ) const
{
  Warning = "";
  return Opt.empty();
}


bool Options::exist( const string &ident ) const
{
  Warning = "";
  const_iterator pp = find( ident );
  return ( pp != end() );
}


Options &Options::setFlags( int flags, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( selectflag ) ) {
      (*pp).setFlags( flags );
    }
  }
  return *this;
}


Options &Options::addFlags( int flags, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( selectflag ) ) {
      (*pp).addFlags( flags );
    }
  }
  return *this;
}


Options &Options::delFlags( int flags, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( selectflag ) ) {
      (*pp).delFlags( flags );
    }
  }
  return *this;
}


Options &Options::setTypeFlags( int flags, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).types( typemask ) ) {
      (*pp).setFlags( flags );
    }
  }
  return *this;
}


Options &Options::addTypeFlags( int flags, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).types( typemask ) ) {
      (*pp).addFlags( flags );
    }
  }
  return *this;
}


Options &Options::delTypeFlags( int flags, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).types( typemask ) ) {
      (*pp).delFlags( flags );
    }
  }
  return *this;
}


Options &Options::setStyle( int style, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp )
    if ( (*pp).flags( selectflag ) ) {
      (*pp).setStyle( style );
    }
  return *this;
}


Options &Options::addStyle( int style, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp )
    if ( (*pp).flags( selectflag ) ) {
      (*pp).addStyle( style );
    }
  return *this;
}


Options &Options::delStyle( int style, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp )
    if ( (*pp).flags( selectflag ) ) {
      (*pp).delStyle( style );
    }
  return *this;
}


int Options::styleSize( int style ) const
{
  Warning = "";
  int n=0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( ( (*pp).style() & style ) == style ) {
      n++;
    }
  }
  return n;
}


Options &Options::setTypeStyle( int style, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).types( typemask ) ) {
      (*pp).setStyle( style );
    }
  }
  return *this;
}


Options &Options::addTypeStyle( int style, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).types( typemask ) ) {
      (*pp).addStyle( style );
    }
  }
  return *this;
}


Options &Options::delTypeStyle( int style, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).types( typemask ) ) {
      (*pp).delStyle( style );
    }
  }
  return *this;
}


int Options::identWidth( int selectmask ) const
{
  Warning = "";

  // search largest identifier:
  unsigned int width = 0;
  unsigned int pwidth = 0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).isLabel() && ( (*pp).style() & Parameter::SavePatternLabel ) )
      pwidth = (*pp).ident().size() + 1;
    if ( (*pp).flags( selectmask ) ) {
      unsigned int w = (*pp).ident().size();
      if ( pwidth > 0 && ! (*pp).isBlank() )
	w += pwidth;
      if ( !(*pp).isBlank() && w > width )
	width = w;
    }
  }

  return width;
}


ostream &Options::save( ostream &str, const string &start, int width,
			int selectmask, bool detailed, bool firstonly ) const
{
  Warning = "";

  if ( width < 0 )
    width = identWidth( selectmask );

  // write options to file:
  string pattern = "";
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).isLabel() && ( (*pp).style() & Parameter::SavePatternLabel ) )
      pattern = (*pp).ident() + '>';
    if ( (*pp).flags( selectmask ) ) {
      str << start;
      (*pp).save( str, width, detailed, firstonly, pattern ) << '\n';
    }
  }

  return str;
}


ostream &Options::save( ostream &str, const string &textformat,
			const string &numberformat, const string &boolformat,
			const string &dateformat, const string &timeformat,
			const string &labelformat,
			const string &separatorformat, int selectmask ) const
{
  Warning = "";

  // write options to file:
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( selectmask ) ) {
      (*pp).save( str, textformat, numberformat, boolformat,
		  dateformat, timeformat, labelformat, separatorformat );
    }
  }

  return str;
}


string Options::save( string separator, 
		      int selectmask, bool firstonly ) const
{
  Warning = "";

  // write options to string:
  string str;
  int n=0;
  string pattern = "";
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).isLabel() && ( (*pp).style() & Parameter::SavePatternLabel ) )
      pattern = (*pp).ident() + '>';
    if ( (*pp).flags( selectmask ) ) {
      if ( n > 0 )
	str += separator;
      if ( ! pattern.empty() && ! (*pp).isBlank() )
	str += pattern;
      str += (*pp).save( false, firstonly );
      n++;
    }
  }

  return str;
}


ostream &operator<< ( ostream &str, const Options &o )
{
  o.save( str );

  return str;
}


ostream &Options::saveXML( ostream &str, int selectmask, int level,
			   int indent ) const
{
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).flags( selectmask ) ) {
      (*pp).saveXML( str, level, indent );
    }
  }
  return str;
}


Options &Options::read( const string &opttxt, int flag,
			const string &assignment, const string &separator,
			string *pattern )
{
  Warning = "";
  bool cn = CallNotify;
  CallNotify = false;

  // split up opttxt:
  StrQueue sq( Str( opttxt ).stripped().preventLast( separator ), separator );

  for ( StrQueue::iterator sp=sq.begin(); sp != sq.end(); ++sp ) {

    // get identifier:
    string ident = (*sp).ident( 0, assignment, Str::WhiteSpace + '-' );
    if ( ! ident.empty() ) {
      string value = (*sp).value( 0, assignment );
      // set value:
      string error = Warning;
      Parameter *pp = assign( pattern == 0 ? ident : *pattern + ident, value );
      // set style:
      if ( pp != 0 && flag != 0 ) {
	pp->addFlags( flag );
      }
      if ( pattern != 0 && pp != 0 &&
	   (*pp).isLabel() && ( (*pp).style() & Parameter::ReadPatternLabel ) )
	*pattern = (*pp).ident() + '>';
      Warning = error + Warning;
    }

  }

#ifndef NDEBUG
  if ( ! Warning.empty() )
    cerr << "!warning in Options::read() -> " << Warning << '\n';
#endif

  // notify the change:
  CallNotify = cn;
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *this;
}


istream &operator>> ( istream &str, Options &o )
{
  Str s;
 
  // get line:
  getline( str, s );
  o.read( s );

  return str;
}


istream &Options::read( istream &str, int flag, const string &assignment, 
			const string &comment, const string &stop, Str *line )
{
  bool cn = CallNotify;
  CallNotify = false;

  Warning = "";
  Str s;
  bool stopempty = ( stop == StrQueue::StopEmpty );
  string pattern = "";

  // read first line:
  if ( line != 0 && !line->empty() ) {
    // read line:
    string error = Warning;
    read( *line, flag, assignment, "", &pattern );
    Warning = error + Warning;
  }
 
  // get line:
  while ( getline( str, s ) ) {
    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
	 ( stopempty && s.findFirstNot( Str::WhiteSpace ) < 0 ) )
      break;

    // erase comments:
    s.stripComment( comment );

    // read line:
    string error = Warning;
    read( s, flag, assignment, "", &pattern );
    Warning = error + Warning;
  }

  // store last read line:
  if ( line != 0 )
    *line = s;

  // notify the change:
  CallNotify = cn;
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return str;
}


Options &Options::read( const StrQueue &sq, int flag,
			const string &assignment )
{
  bool cn = CallNotify;
  CallNotify = false;

  string pattern = "";
  for ( StrQueue::const_iterator i = sq.begin(); i != sq.end(); ++i ) {
    // read line:
    string error = Warning;
    read( *i, flag, assignment, ";", &pattern );
    Warning = error + Warning;
  }

  // notify the change:
  CallNotify = cn;
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *this;
}


bool Options::read( const Parameter &p, int flag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( (*pp).read( p ) ) {
      (*pp).addFlags( flag );
      // notify the change:
      if ( CallNotify && ! Notified ) {
	Notified = true;
	notify();
	Notified = false;
      }
      return true;
    }
  }
  return false;
}


Options &Options::read( const Options &o, int flags, int flag )
{
  bool changed = false;

  for ( const_iterator op = o.begin(); op != o.end(); ++op ) {
    if ( (*op).flags( flags ) ) {
      for ( iterator pp = begin(); pp != end(); ++pp ) {
	if ( (*pp).read( *op ) ) {
	  (*pp).addFlags( flag );
	  changed = true;
	  break;
	}
      }
    }
  }

  // notify the change:
  if ( changed && CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *this;
}


bool Options::readAppend( const Parameter &p, bool appendseparator )
{
  if ( ! ( p.isSeparator() && p.ident().empty() && appendseparator ) ) {
    for ( iterator pp = begin(); pp != end(); ++pp ) {
      if ( (*pp).read( p ) ) {
	// notify the change:
	if ( CallNotify && ! Notified ) {
	  Notified = true;
	  notify();
	  Notified = false;
	}
	return true;
      }
    }
  }

  Opt.push_back( p );

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return false;
}


Options &Options::readAppend( const Options &o, int flags, bool appendseparator )
{
  for ( const_iterator op = o.begin(); op != o.end(); ++op ) {
    if ( (*op).flags( flags ) ) {
      bool app = true;
      if ( ! ( (*op).isSeparator() && (*op).ident().empty() && appendseparator ) ) {
	for ( iterator pp = begin(); pp != end(); ++pp ) {
	  if ( (*pp).read( *op ) ) {
	    app = false;
	    break;
	  }
	}
      }
      if ( app )
	Opt.push_back( *op );
    }
  }

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *this;
}


Options &Options::readAppend( const StrQueue &sq, bool appendseparator,
			      const string &assignment )
{
  Warning = "";
  for ( StrQueue::const_iterator i = sq.begin(); i != sq.end(); ++i ) {
    // create option:
    Parameter np;
    np.load( *i, assignment );
    Warning += np.warning();

    bool app = true;
    if ( ! ( np.isSeparator() && np.ident().empty() && appendseparator ) ) {
      for ( iterator pp = begin(); pp != end(); ++pp ) {
	if ( (*pp).read( np ) ) {
	  app = false;
	  break;
	}
      }
    }

    if ( app ) {
      Opt.push_back( np );
    }

  }

#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::readAppend( StrQueue ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  if ( CallNotify && ! Notified ) {
    Notified = true;
    notify();
    Notified = false;
  }

  return *this;
}


Options &Options::load( const Str &opttxt, const string &assignment,
			const string &separator )
{
  Warning = "";

  string s = opttxt.stripped().preventLast( separator );
  if ( s.empty() )
    return *this;

  // split up opttxt:
  StrQueue sq( s, separator );

  for ( StrQueue::const_iterator sp=sq.begin(); sp != sq.end(); ++sp ) {
    Parameter np( *sp, assignment );
    Warning += np.warning();
    Opt.push_back( np );
  }

#ifndef NDEBUG
  if ( ! Warning.empty() )
    cerr << "!warning in Options::load() -> " << Warning << '\n';
#endif

  return *this;
}


istream &Options::load( istream &str, const string &assignment,
			const string &comment, 
			const string &stop, string *line )
{
  Warning = "";
  Str s = "";
  Parameter np;
  bool stopempty = ( stop == StrQueue::StopEmpty );

  // read first line:
  if ( line != 0 && !line->empty() ) { 
    s = line;
    // erase comments:
    s.stripComment( comment );
    // create option:
    np.load( s, assignment );
    Warning += np.warning();
    Opt.push_back( np );
  }
 
  // get line:
  while ( getline( str, s ) ) {
    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
	 ( stopempty && s.findFirstNot( Str::WhiteSpace ) < 0 ) )
      break;

    // erase comments:
    s.stripComment();

    // create option:
    np.load( s, assignment );
    Warning += np.warning();
    Opt.push_back( np );
  }
  
  // store last read line:
  if ( line != 0 )
    *line = s;

  return str;
}


Options &Options::load( const StrQueue &sq, const string &assignment )
{
  Warning = "";
  for ( StrQueue::const_iterator i = sq.begin(); i != sq.end(); ++i ) {
    // create option:
    Parameter np;
    np.load( *i, assignment );
    Warning += np.warning();
    Opt.push_back( np );
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::load( StrQueue ) -> " << Warning << '\n';
  }
#endif
  return *this;
}


void Options::setNotify( bool notify )
{
  CallNotify = notify;
}


void Options::unsetNotify( void )
{
  CallNotify = false;
}


}; /* namespace relacs */

