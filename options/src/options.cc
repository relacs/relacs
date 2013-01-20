/*
  options.cc
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

#include <iomanip>
#include <relacs/options.h>

namespace relacs {


Parameter Options::Dummy = Parameter();
Options Options::SecDummy = Options();


Options::Options( void )
  : ParentSection( 0 ),
    Name( "" ),
    Type( "" ),
    Include( "" ),
    Flag( 0 ),
    Style( 0 ),
    Opt(),
    Secs(),
    OwnSecs(),
    AddOpts( this ),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
}


Options::Options( const Options &o )
  : ParentSection( o.ParentSection ),
    Name( o.Name ),
    Type( o.Type ),
    Include( o.Include ),
    Flag( o.Flag ),
    Style( o.Style ),
    AddOpts( this ),
    Warning( "" ),
    Notified( false ),
    CallNotify( o.CallNotify )
{
  assign( o );
}


Options::Options( const Options &o, int flags )
  : ParentSection( 0 ),
    Name( "" ),
    Type( "" ),
    Include( "" ),
    Flag( 0 ),
    Style( 0 ),
    Opt(),
    Secs(),
    OwnSecs(),
    AddOpts( this ),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
  assign( o, flags );
}


Options::Options( const string &name, const string &type, int flags, int style )
  : ParentSection( 0 ),
    Name( name ),
    Type( type ),
    Include( "" ),
    Flag( flags ),
    Style( style ),
    Opt(),
    Secs(),
    OwnSecs(),
    AddOpts( this ),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
}


Options::Options( const Str &opttxt, const string &assignment,
		  const string &separator )
  : ParentSection( 0 ),
    Name( "" ),
    Type( "" ),
    Include( "" ),
    Flag( 0 ),
    Style( 0 ),
    Opt(),
    Secs(),
    OwnSecs(),
    AddOpts( this ),
    Warning( "" ),
    Notified( false ),
    CallNotify( true )
{
  load( opttxt, assignment, separator );
}


Options::Options( const StrQueue &sq, const string &assignment )
  : ParentSection( 0 ),
    Name( "" ),
    Type( "" ),
    Include( "" ),
    Flag( 0 ),
    Style( 0 ),
    Opt(),
    Secs(),
    OwnSecs(),
    AddOpts( this ),
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
    Type( "" ),
    Include( "" ),
    Flag( 0 ),
    Style( 0 ),
    Opt(),
    Secs(),
    OwnSecs(),
    AddOpts( this ),
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

  clear();
  Name = o.Name;
  Type = o.Type;
  Include = o.Include;
  Flag = o.Flag;
  Style = o.Style;
  ParentSection = 0;
  Opt = o.Opt;
  for ( iterator pp = begin(); pp != end(); ++pp )
    pp->setParentSection( this );
  for ( const_section_iterator sp = o.sectionsBegin();
	sp != o.sectionsEnd();
	++sp ) {
    Options *o = new Options( **sp );
    o->setParentSection( this );
    Secs.push_back( o );
    OwnSecs.push_back( true );
  }
  AddOpts = this;
  Notified = false;
  CallNotify = o.CallNotify;

  return *this;
}


Options &Options::append( const Options &o )
{
  Warning = "";
  if ( this == &o )
    return *this;

  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp ) {
    Opt.push_back( *pp );
    Opt.back().setParentSection( AddOpts );
  }
  for ( const_section_iterator sp = o.sectionsBegin();
	sp != o.sectionsEnd();
	++sp ) {
    Options *o = new Options( **sp );
    o->setParentSection( this );
    Secs.push_back( o );
    OwnSecs.push_back( true );
  }

  return *this;
}


Options &Options::add( const Options &o )
{
  Warning = "";
  if ( this == &o || AddOpts == &o )
    return *this;

  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp ) {
    AddOpts->Opt.push_back( *pp );
    AddOpts->Opt.back().setParentSection( AddOpts );
  }
  for ( const_section_iterator sp = o.sectionsBegin();
	sp != o.sectionsEnd();
	++sp ) {
    Options *o = new Options( **sp );
    o->setParentSection( AddOpts );
    AddOpts->Secs.push_back( o );
    AddOpts->OwnSecs.push_back( true );
  }

  return *AddOpts;
}


Options &Options::insert( const Options &o, const string &atname )
{
  Warning = "";

  if ( atname.empty() ) {
    // insert at beginning of list:
    Opt.insert( Opt.begin(), o.begin(), o.end() );
  }
  else {
    // insert option at element specified by atname:

    // search element:
    iterator pp = find( atname );
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
  for ( iterator pp = begin(); pp != end(); ++pp )
    pp->setParentSection( this );
  return *this;
}


Options &Options::assign( const Options &o, int flags )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  clear();
  Name = o.Name;
  Type = o.Type;
  Include = o.Include;
  Flag = o.Flag;
  Style = o.Style;
  ParentSection = 0;
  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp ) {
    if ( pp->flags( flags ) ) {
      Opt.push_back( *pp );
      Opt.back().setParentSection( this );
    }
  }
  for ( const_section_iterator sp = o.sectionsBegin();
	sp != o.sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( flags ) && (*sp)->size( flags ) > 0 ) {
      Options *o = new Options( **sp, flags );
      o->setParentSection( this );
      Secs.push_back( o );
      OwnSecs.push_back( true );
    }
  }
  AddOpts = this;
  Notified = false;
  CallNotify = o.CallNotify;

  return *this;
}


Options &Options::copy( Options &o, int flags )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  o.clear();
  o.Name = Name;
  o.Type = Type;
  o.Include = Include;
  o.Flag = Flag;
  o.Style = Style;
  o.ParentSection = ParentSection;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( flags ) ) {
      o.Opt.push_back( *pp );
      o.Opt.back().setParentSection( &o );
    }
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( flags ) && (*sp)->size( flags ) > 0 ) {
      Options *oo = new Options( **sp, flags );
      oo->setParentSection( &o );
      o.Secs.push_back( oo );
      o.OwnSecs.push_back( true );
    }
  }
  o.AddOpts = &o;
  Notified = false;
  o.CallNotify = CallNotify;

  return *this;
}


Options &Options::append( const Options &o, int flags )
{
  Warning = "";
  if ( this == &o ) 
    return *this;

  // add Parameter to current section:
  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp ) {
    if ( pp->flags( flags ) ) {
      Opt.push_back( *pp );
      Opt.back().setParentSection( this );
    }
  }
  // add Sections to current section:
  for ( const_section_iterator sp = o.sectionsBegin();
	sp != o.sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( flags ) && (*sp)->size( flags ) > 0 ) {
      // add empty section:
      Options *o = new Options( *(*sp), flags );
      o->setParentSection( this );
      Secs.push_back( o );
      OwnSecs.push_back( true );
    }
  }

  return *this;
}


Options &Options::add( const Options &o, int flags )
{
  Warning = "";
  if ( this == &o || AddOpts == &o ) 
    return *this;

  // add Parameter to current section:
  for ( const_iterator pp = o.begin(); pp != o.end(); ++pp ) {
    if ( pp->flags( flags ) ) {
      AddOpts->Opt.push_back( *pp );
      AddOpts->Opt.back().setParentSection( AddOpts );
    }
  }
  // add Sections to current section:
  for ( const_section_iterator sp = o.sectionsBegin();
	sp != o.sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( flags ) && (*sp)->size( flags ) > 0 ) {
      Options *o = new Options( *(*sp), flags );
      o->setParentSection( AddOpts );
      AddOpts->Secs.push_back( o );
      AddOpts->OwnSecs.push_back( true );
    }
  }

  return *AddOpts;
}


Options &Options::insert( const Options &o, int flags, const string &atname )
{
  Warning = "";

  if ( atname.empty() ) {
    // insert at beginning of list:
    for ( deque< Parameter >::const_reverse_iterator op = o.Opt.rbegin();
	  op < o.Opt.rend();
	  ++op ) {
      if ( op->flags( flags ) ) {
	Opt.push_front( *op );
	Opt.front().setParentSection( this );
      }
    }
    return *this;
  }
  else {
    // insert option at element specified by atname:

    // search element:
    iterator pp = find( atname );
    if ( pp != end() ) {
      for ( deque< Parameter >::const_reverse_iterator op = o.Opt.rbegin();
	    op < o.Opt.rend();
	    ++op ) {
	if ( (*op).flags( flags ) ) {
	  pp = Opt.insert( pp, *op );
	  pp->setParentSection( this );
	}
      }
    }
    else {
      // not found:
      for ( const_iterator op = o.begin(); op != o.end(); ++op ) {
	if ( (*op).flags( flags ) ) {
	  Opt.push_back( *op );
	  Opt.back().setParentSection( this );
	}
      }
    }
  }
  return *this;
}


Parameter *Options::assign( const string &name, const string &value )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->assign( value );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::assign( " << name << " ) -> " << Warning << '\n';
  }
#endif

  // notify the change:
  callNotifies();

  return pp != end() ? &(*pp) : 0;
}


bool operator==( const Options &o1, const Options &o2 )
{
  if ( o1.size() != o2.size() )
    return false;
  for ( Options::const_iterator p1 = o1.begin(), p2 = o2.begin();
	p1 != o1.end() && p2 != o2.end();
	++p1, ++p2 ) {
    if ( (*p1).name() != (*p2).name() )
      return false;
    if ( (*p1).text() != (*p2).text() )
      return false;
  }
  for ( Options::const_section_iterator sp1 = o1.sectionsBegin(),
	  sp2 = o2.sectionsBegin();
	sp1 != o1.sectionsEnd() && sp2 != o2.sectionsEnd();
	++sp1, ++sp2 ) {
    if ( (*sp1)->name() != (*sp2)->name() )
      return false;
    if ( ! (*sp1)->type().empty() && ! (*sp2)->type().empty() &&
	 (*sp1)->type() != (*sp2)->type() )
      return false;
    bool r = ( **sp1 == **sp2 );
    if ( ! r )
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
  int s1 = o1.size();
  int s2 = o2.size();
  if ( s1 < s2 )
    return true;
  if ( s1 > s2 )
    return false;
  for ( Options::const_iterator p1 = o1.begin(), p2 = o2.begin();
	p1 != o1.end() && p2 != o2.end();
	++p1, ++p2 ) {
    if ( (*p1).name() < (*p2).name() )
      return true;
    else if ( (*p1).name() > (*p2).name() )
      return false;
    if ( (*p1).text() < (*p2).text() )
      return true;
    else if ( (*p1).text() > (*p2).text() )
      return false;
  }
  for ( Options::const_section_iterator sp1 = o1.sectionsBegin(),
	  sp2 = o2.sectionsBegin();
	sp1 != o1.sectionsEnd() && sp2 != o2.sectionsEnd();
	++sp1, ++sp2 ) {
    if ( (*sp1)->name() < (*sp2)->name() )
      return true;
    if ( (*sp1)->name() > (*sp2)->name() )
      return false;
    if ( ! (*sp1)->type().empty() && ! (*sp2)->type().empty() ) {
      if ( (*sp1)->type() < (*sp2)->type() )
	return true;
      if ( (*sp1)->type() > (*sp2)->type() )
	return false;
    }
    bool r = ( **sp1 < **sp2 );
    if ( r )
      return true;
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


void Options::resetParents( void )
{
  for ( section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    (*sp)->setParentSection( this );
    (*sp)->resetParents();
  }
}


Options *Options::rootSection( void )
{
  if ( parentSection() == 0 )
    return this;

  Options *ps = parentSection();
  while ( ps->parentSection() != 0 )
    ps = ps->parentSection();
  return ps;
}


const Options *Options::rootSection( void ) const
{
  if ( parentSection() == 0 )
    return this;

  const Options *ps = parentSection();
  while ( ps->parentSection() != 0 )
    ps = ps->parentSection();
  return ps;
}


string Options::name( void ) const
{
  return Name;
}


void Options::setName( const string &name )
{
  Name = name;
}


void Options::setName( const string &name, const string &type )
{
  setName( name );
  Type = type;
}


string Options::type( void ) const
{
  return Type;
}


void Options::setType( const string &type )
{
  Type = type;
}


string Options::include( void ) const
{
  return Include;
}


void Options::setInclude( const string &include )
{
  Include = include;
}


void Options::setInclude( const string &url, const string &name )
{
  if ( name.empty() )
    Include = url;
  else
    Include = url + '#' + name;
}


int Options::flag( void ) const
{
  return Flag;
}


bool Options::flag( int selectflag ) const
{
  return ( selectflag == 0 || selectflag == NonDefault || 
	   flag() == 0 || ( ( flag() & abs(selectflag) ) ) );
}


Options & Options::setFlag( int flag )
{
  Flag = flag;
  return *this;
}


Options &Options::addFlag( int flag )
{
  Flag |= flag;
  return *this;
}


Options &Options::delFlag( int flag )
{
  Flag &= ~flag;
  return *this;
}


Options &Options::clearFlag( void )
{
  Flag = 0;
  return *this;
}


int Options::style( void ) const
{
  return Style;
}


Options &Options::setStyle( int style )
{
  Style = style;
  return *this;
}


Options &Options::addStyle( int style )
{
  Style |= style;
  return *this;
}


Options &Options::delStyle( int style )
{
  Style &= ~style;
  return *this;
}


Options &Options::clearStyle( void )
{
  Style = 0;
  return *this;
}


bool Options::checkType( int mask ) const
{
  return ( mask == 0 ||
	   ( mask > 0 && ( mask & Parameter::Section ) > 0 ) ||
	   ( mask < 0 && ( -mask & Parameter::Section ) == 0 ) );
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


const Parameter &Options::operator[]( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::operator[]( const string &name )
{
  iterator pp = find( name );
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


const Options &Options::section( int i ) const
{
  Warning = "";

  if ( i >= 0 && i < (int)Secs.size() )
    return *Secs[i];

  Warning = "section with index " + Str( i ) + " does not exist!";
#ifndef NDEBUG
  cerr << "!warning in Options::section(" << i << ") -> " << Warning << '\n';
#endif
  SecDummy = Options();
  return SecDummy;
}


Options &Options::section( int i )
{
  Warning = "";

  if ( i >= 0 && i < (int)Secs.size() )
    return *Secs[i];

  Warning = "section with index " + Str( i ) + " does not exist!";
#ifndef NDEBUG
  cerr << "!warning in Options::section(" << i << ") -> " << Warning << '\n';
#endif
  SecDummy = Options();
  return SecDummy;
}


const Options &Options::section( const string &name ) const
{
  Warning = "";

  const_section_iterator sp = findSection( name );
  if ( sp != sectionsEnd() )
    return *(*sp);

  Warning = "section with name '" + name + "' does not exist!";
#ifndef NDEBUG
  cerr << "!warning in Options::section(" << name << ") -> " << Warning << '\n';
#endif
  SecDummy = Options();
  return SecDummy;
}


Options &Options::section( const string &name )
{
  Warning = "";

  section_iterator sp = findSection( name );
  if ( sp != sectionsEnd() )
    return *(*sp);

  Warning = "section with name '" + name + "' does not exist!";
#ifndef NDEBUG
  cerr << "!warning in Options::section(" << name << ") -> " << Warning << '\n';
#endif
  SecDummy = Options();
  return SecDummy;
}


Options::const_iterator Options::find( const string &pattern, int level ) const
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    const_iterator pbegin = begin();
    const_section_iterator sbegin = sectionsBegin();
    bool findagain = false;
    do {
      findagain = false;
      size_t pi = patterns.find( '>' );
      if ( k > 0 || pi == string::npos || pi == patterns.size()-1 ) {
	// search in key-value pairs:
	if ( pi == patterns.size()-1 )
	  patterns.resize( patterns.size()-1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( patterns, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( patterns, "" );
	// search all alternatives:
	for ( int s=0; s<sq.size(); s++ ) {
	  // search element:
	  for ( const_iterator pp = pbegin; pp != end(); ++pp ) {
	    if ( *pp == sq[s] )
	      return pp;
	  }
	  // search in subsections:
	  for ( const_section_iterator sp = sbegin;
		sp != sectionsEnd();
		++sp ) {
	    const_iterator pp = (*sp)->find( sq[s], k );
	    if ( pp != (*sp)->end() )
	      return pp;
	  }
	}
      }
      else {
	// search in sections:
	string search = patterns.substr( 0, pi );
	string subsearch = patterns.substr( pi+1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( search, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( search, "" );
	// search:
	for ( int s=0; s<sq.size() && ! findagain; s++ ) {
	  // section name:
	  if ( *this == sq[s] ) {
	    patterns = subsearch;
	    pbegin = begin();
	    findagain = true;
	    break;
	  }
	  // search parameter:
	  for ( const_iterator pp = pbegin; pp != end(); ++pp ) {
	    if ( *pp == sq[s] ) {
	      patterns = subsearch;
	      pbegin = pp+1;
	      findagain = true;
	      break;
	    }
	  }
	  // search section:
	  for ( const_section_iterator sp = sbegin;
		sp != sectionsEnd() && ! findagain;
		++sp ) {
	    if ( **sp == sq[s] ) {
	      const_iterator pp = (*sp)->find( subsearch, k );
	      if ( pp != (*sp)->end() )
		return pp;
	    }
	    else {
	      const_iterator pp = (*sp)->find( sq[s] + '>' + subsearch, k );
	      if ( pp != (*sp)->end() )
		return pp;
	    }
	  }
	}
      }
    } while ( findagain );
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
}


Options::iterator Options::find( const string &pattern, int level )
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    iterator pbegin = begin();
    section_iterator sbegin = sectionsBegin();
    bool findagain = false;
    do {
      findagain = false;
      size_t pi = patterns.find( '>' );
      if ( k > 0 || pi == string::npos || pi == patterns.size()-1 ) {
	// search in key-value pairs:
	if ( pi == patterns.size()-1 )
	  patterns.resize( patterns.size()-1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( patterns, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( patterns, "" );
	// search all alternatives:
	for ( int s=0; s<sq.size(); s++ ) {
	  // search element:
	  for ( iterator pp = pbegin; pp != end(); ++pp ) {
	    if ( *pp == sq[s] )
	      return pp;
	  }
	  // search in subsections:
	  for ( section_iterator sp = sbegin;
		sp != sectionsEnd();
		++sp ) {
	    iterator pp = (*sp)->find( sq[s], k );
	    if ( pp != (*sp)->end() )
	      return pp;
	  }
	}
      }
      else {
	// search in sections:
	string search = patterns.substr( 0, pi );
	string subsearch = patterns.substr( pi+1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( search, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( search, "" );
	// search:
	for ( int s=0; s<sq.size() && ! findagain; s++ ) {
	  // search parameter:
	  for ( iterator pp = pbegin; pp != end(); ++pp ) {
	    if ( *pp == sq[s] ) {
	      patterns = subsearch;
	      pbegin = pp+1;
	      findagain = true;
	      break;
	    }
	  }
	  // search section:
	  for ( section_iterator sp = sbegin;
		sp != sectionsEnd() && ! findagain;
		++sp ) {
	    if ( **sp == sq[s] ) {
	      iterator pp = (*sp)->find( subsearch, k );
	      if ( pp != (*sp)->end() )
		return pp;
	    }
	    else {
	      iterator pp = (*sp)->find( sq[s] + '>' + subsearch, k );
	      if ( pp != (*sp)->end() )
		return pp;
	    }
	  }
	}
      }
    } while ( findagain );
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
}


Options::const_iterator Options::rfind( const string &pattern, int level ) const
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    const_iterator pend = end();
    const_section_iterator send = sectionsEnd();
    bool findagain = false;
    do {
      findagain = false;
      size_t pi = patterns.find( '>' );
      if ( k > 0 || pi == string::npos || pi == patterns.size()-1 ) {
	// search in key-value pairs:
	if ( pi == patterns.size()-1 )
	  patterns.resize( patterns.size()-1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( patterns, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( patterns, "" );
	// search all alternatives:
	for ( int s=0; s<sq.size(); s++ ) {
	  // search in subsections:
	  const_section_iterator sp = send;
	  if ( sp != sectionsBegin() ) {
	    do {
	      --sp;
	      const_iterator pp = (*sp)->rfind( sq[s], k );
	      if ( pp != (*sp)->end() )
		return pp;
	    } while ( sp != sectionsBegin() );
	  }
	  // search element:
	  const_iterator pp = pend;
	  if ( pp != begin() ) {
	    do {
	      --pp;
	      if ( *pp == sq[s] )
		return pp;
	    } while ( pp != begin() );
	  }
	}
      }
      else {
	// search in sections:
	string search = patterns.substr( 0, pi );
	string subsearch = patterns.substr( pi+1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( search, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( search, "" );
	// search:
	for ( int s=0; s<sq.size() && ! findagain; s++ ) {
	  // search section and subsections:
	  const_section_iterator sp = send;
	  if ( sp != sectionsBegin() ) {
	    do {
	      --sp;
	      const_iterator pp = (*sp)->rfind( sq[s] + '>' + subsearch, k );
	      if ( pp != (*sp)->end() )
		return pp;
	      if ( **sp == sq[s] ) {
		const_iterator pp = (*sp)->rfind( subsearch, k );
		if ( pp != (*sp)->end() )
		  return pp;
	      }
	    } while ( sp != sectionsBegin() && ! findagain );
	  }
	  // search parameter:
	  const_iterator pp = pend;
	  if ( pp != begin() && ! findagain ) {
	    do {
	      --pp;
	      if ( *pp == sq[s] ) {
		patterns = subsearch;
		pend = pp;
		send = sectionsBegin();
		findagain = true;
		break;
	      }
	    } while ( pp != begin() );
	  }
	}
      }
    } while ( findagain );
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
}


Options::iterator Options::rfind( const string &pattern, int level )
{
  Warning = "";

  if ( empty() )
    return end();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return end();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    iterator pend = end();
    section_iterator send = sectionsEnd();
    bool findagain = false;
    do {
      findagain = false;
      size_t pi = patterns.find( '>' );
      if ( k > 0 || pi == string::npos || pi == patterns.size()-1 ) {
	// search in key-value pairs:
	if ( pi == patterns.size()-1 )
	  patterns.resize( patterns.size()-1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( patterns, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( patterns, "" );
	// search all alternatives:
	for ( int s=0; s<sq.size(); s++ ) {
	  // search in subsections:
	  section_iterator sp = send;
	  if ( sp != sectionsBegin() ) {
	    do {
	      --sp;
	      iterator pp = (*sp)->rfind( sq[s], k );
	      if ( pp != (*sp)->end() )
		return pp;
	    } while ( sp != sectionsBegin() );
	  }
	  // search element:
	  iterator pp = pend;
	  if ( pp != begin() ) {
	    do {
	      --pp;
	      if ( *pp == sq[s] )
		return pp;
	    } while ( pp != begin() );
	  }
	}
      }
      else {
	// search in sections:
	string search = patterns.substr( 0, pi );
	string subsearch = patterns.substr( pi+1 );
	StrQueue sq;
	if ( k < 2 ) {
	  sq.assign( search, "|" );
	  for ( int j=0; j<sq.size(); ) {
	    if ( sq[j].empty() )
	      sq.erase( j );
	    else
	      j++;
	  }
	}
	else
	  sq.assign( search, "" );
	// search:
	for ( int s=0; s<sq.size() && ! findagain; s++ ) {
	  // search section and subsections:
	  section_iterator sp = send;
	  if ( sp != sectionsBegin() ) {
	    do {
	      --sp;
	      iterator pp = (*sp)->rfind( sq[s] + '>' + subsearch, k );
	      if ( pp != (*sp)->end() )
		return pp;
	      if ( **sp == sq[s] ) {
		iterator pp = (*sp)->rfind( subsearch, k );
		if ( pp != (*sp)->end() )
		  return pp;
	      }
	    } while ( sp != sectionsBegin() && ! findagain );
	  }
	  // search parameter:
	  iterator pp = pend;
	  if ( pp != begin() && ! findagain ) {
	    do {
	      --pp;
	      if ( *pp == sq[s] ) {
		patterns = subsearch;
		pend = pp;
		send = sectionsBegin();
		findagain = true;
		break;
	      }
	    } while ( pp != begin() );
	  }
	}
      }
    } while ( findagain );
  }
  // nothing found:
  Warning = "requested option '" + pattern + "' not found!";
  return end();
}


Options::const_section_iterator Options::findSection( const string &pattern,
						      int level ) const
{
  Warning = "";

  if ( sectionsSize() <= 0 )
    return sectionsEnd();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return sectionsEnd();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    size_t pi = patterns.find( '>' );
    // search in sections:
    if ( k > 0 )
      pi = string::npos;
    string search = patterns.substr( 0, pi );
    string subsearch = "";
    if ( pi != string::npos )
      subsearch = patterns.substr( pi+1 );
    StrQueue sq;
    if ( k < 2 ) {
      sq.assign( search, "|" );
      for ( int j=0; j<sq.size(); ) {
	if ( sq[j].empty() )
	  sq.erase( j );
	else
	  j++;
      }
    }
    else
      sq.assign( search, "" );
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      for ( const_section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	if ( **sp == sq[s] ) {
	  if ( subsearch.empty() )
	    return sp;
	  else {
	    const_section_iterator pp = (*sp)->findSection( subsearch, k );
	    if ( pp != (*sp)->sectionsEnd() )
	      return pp;
	  }
	}
      }
      for ( const_section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	const_section_iterator pp = (*sp)->findSection( sq[s] + '>' + subsearch, k );
	if ( pp != (*sp)->sectionsEnd() )
	  return pp;
      }
    }
  }
  // nothing found:
  Warning = "requested section '" + pattern + "' not found!";
  return sectionsEnd();

}


Options::section_iterator Options::findSection( const string &pattern,
						int level )
{
  Warning = "";

  if ( sectionsSize() <= 0 )
    return sectionsEnd();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return sectionsEnd();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    size_t pi = patterns.find( '>' );
    // search in sections:
    if ( k > 0 )
      pi = string::npos;
    string search = patterns.substr( 0, pi );
    string subsearch = "";
    if ( pi != string::npos )
      subsearch = patterns.substr( pi+1 );
    StrQueue sq;
    if ( k < 2 ) {
      sq.assign( search, "|" );
      for ( int j=0; j<sq.size(); ) {
	if ( sq[j].empty() )
	  sq.erase( j );
	else
	  j++;
      }
    }
    else
      sq.assign( search, "" );
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      for ( section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	if ( **sp == sq[s] ) {
	  if ( subsearch.empty() )
	    return sp;
	  else {
	    section_iterator pp = (*sp)->findSection( subsearch, k );
	    if ( pp != (*sp)->sectionsEnd() )
	      return pp;
	  }
	}
      }
      for ( section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	section_iterator pp = (*sp)->findSection( sq[s] + '>' + subsearch, k );
	if ( pp != (*sp)->sectionsEnd() )
	  return pp;
      }
    }
  }
  // nothing found:
  Warning = "requested section '" + pattern + "' not found!";
  return sectionsEnd();

}


Options::const_section_iterator Options::rfindSection( const string &pattern,
						       int level ) const
{
  Warning = "";

  if ( sectionsSize() <= 0 )
    return sectionsEnd();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return sectionsEnd();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    size_t pi = patterns.find( '>' );
    // search in sections:
    if ( k > 0 )
      pi = string::npos;
    string search = patterns.substr( 0, pi );
    string subsearch = "";
    if ( pi != string::npos )
      subsearch = patterns.substr( pi+1 );
    StrQueue sq;
    if ( k < 2 ) {
      sq.assign( search, "|" );
      for ( int j=0; j<sq.size(); ) {
	if ( sq[j].empty() )
	  sq.erase( j );
	else
	  j++;
      }
    }
    else
      sq.assign( search, "" );
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      const_section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
	  if ( **sp == sq[s] ) {
	    if ( subsearch.empty() )
	      return sp;
	    else {
	      const_section_iterator pp = (*sp)->rfindSection( subsearch, k );
	      if ( pp != (*sp)->sectionsEnd() )
		return pp;
	    }
	  }
	} while ( sp != sectionsBegin() );
      }
      sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
	  const_section_iterator pp = (*sp)->rfindSection( sq[s] + '>' + subsearch, k );
	  if ( pp != (*sp)->sectionsEnd() )
	    return pp;
	} while ( sp != sectionsBegin() );
      }
    }
  }
  // nothing found:
  Warning = "requested section '" + pattern + "' not found!";
  return sectionsEnd();
}


Options::section_iterator Options::rfindSection( const string &pattern,
						 int level )
{
  Warning = "";

  if ( sectionsSize() <= 0 )
    return sectionsEnd();

  if ( pattern.empty() ) {
    Warning = "empty search string!";
    return sectionsEnd();
  }

  int fromlevel = level < 0 ? 0 : level;
  int uptolevel = level < 0 ? 3 : level+1;

  for ( int k=fromlevel; k<uptolevel; k++ ) {
    string patterns = pattern;
    size_t pi = patterns.find( '>' );
    // search in sections:
    if ( k > 0 )
      pi = string::npos;
    string search = patterns.substr( 0, pi );
    string subsearch = "";
    if ( pi != string::npos )
      subsearch = patterns.substr( pi+1 );
    StrQueue sq;
    if ( k < 2 ) {
      sq.assign( search, "|" );
      for ( int j=0; j<sq.size(); ) {
	if ( sq[j].empty() )
	  sq.erase( j );
	else
	  j++;
      }
    }
    else
      sq.assign( search, "" );
    // search:
    for ( int s=0; s<sq.size(); s++ ) {
      // search section:
      section_iterator sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
	  if ( **sp == sq[s] ) {
	    if ( subsearch.empty() )
	      return sp;
	    else {
	      section_iterator pp = (*sp)->rfindSection( subsearch, k );
	      if ( pp != (*sp)->sectionsEnd() )
		return pp;
	    }
	  }
	} while ( sp != sectionsBegin() );
      }
      sp = sectionsEnd();
      if ( sp != sectionsBegin() ) {
	do {
	  --sp;
	  section_iterator pp = (*sp)->rfindSection( sq[s] + '>' + subsearch, k );
	  if ( pp != (*sp)->sectionsEnd() )
	    return pp;
	} while ( sp != sectionsBegin() );
      }
    }
  }
  // nothing found:
  Warning = "requested section '" + pattern + "' not found!";
  return sectionsEnd();
}


Str Options::request( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Str s = pp->request();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::request( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setRequest( const string &name, const string &request )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setRequest( request );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setRequest( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter::ValueType Options::valueType( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Parameter::ValueType t = pp->valueType();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::valueType( " << name << " ) -> " << Warning << '\n';
#endif
    return t;
  }
  else
    return Parameter::NoType;
}


Parameter &Options::setValueType( const string &name, Parameter::ValueType type )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setValueType( type );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setValueType( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


int Options::flags( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    int m = pp->flags();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::flags( " << name << " ) -> " << Warning << '\n';
#endif
    return m;
  }
  else
    return 0;
}


bool Options::flags( const string &name, int flag ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    bool r = pp->flags( flag );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::flags( " << name << " ) -> " << Warning << '\n';
#endif
    return r;
  }
  else
    return false;
}


Parameter &Options::setFlags( const string &name, int flags )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setFlags( flags );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setFlags( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::addFlags( const string &name, int flags )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->addFlags( flags );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::addFlags( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::delFlags( const string &name, int flags )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->delFlags( flags );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::delFlags( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::clearFlags( const string &name )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->clearFlags();
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::clearFlags( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


bool Options::changed( const string &name )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    return pp->changed();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::changed( " << name << " ) -> " << Warning << '\n';
  }
#endif
  return false;
}


int Options::style( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    int m = pp->style();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::style( " << name << " ) -> " << Warning << '\n';
#endif
    return m;
  }
  else
    return 0;
}


Parameter &Options::setStyle( const string &name, int style )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setStyle( style );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setStyle( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::addStyle( const string &name, int style )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->addStyle( style );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::addStyle( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::delStyle( const string &name, int style )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->delStyle( style );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::delStyle( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


int Options::size( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    int s = pp->size();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::size( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return 0;
}


Str Options::format( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Str s = pp->format();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::format( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setFormat( const string &name, int width, int prec, char fmt )
{
  iterator pp = find( name );
  // set values:
  if ( pp != end() ) {
    pp->setFormat( width, prec, fmt );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setFormat( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setFormat( const string &name, const string &format )
{
  iterator pp = find( name );
  // set values:
  if ( pp != end() ) {
    pp->setFormat( format );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setFormat( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


int Options::formatWidth( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    int w = pp->formatWidth();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::formatWidth( " << name << " ) -> " << Warning << '\n';
#endif
    return w;
  }
  else
    return 0;
}


Parameter &Options::add( const Parameter &np )
{
  Warning = "";
  AddOpts->Opt.push_back( np );
  AddOpts->Opt.back().setParentSection( AddOpts );
  return AddOpts->Opt.back();
}


Parameter &Options::insert( const Parameter &np, const string &atname )
{
  Warning = "";

  if ( atname.empty() ) {
    // insert at beginning of currently active list:
    AddOpts->Opt.push_front( np );
    AddOpts->Opt.front().setParentSection( AddOpts );
    return AddOpts->Opt.front();
  }
  else {
    // insert option at element specified by atname:
    iterator pp = find( atname );
    if ( pp != end() ) {
      Options *po = pp->parentSection();
      Parameter &p = *(po->Opt.insert( pp, np ));
      p.setParentSection( po );
      return p;
    }
    else {
      // not found:
      AddOpts->Opt.push_back( np );
      AddOpts->Opt.back().setParentSection( AddOpts );
      return AddOpts->Opt.back();
    }
  }
}


Parameter &Options::addText( const string &name, const string &request,
			     const string &dflt, int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addText( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertText( const string &name, const string &atname,
				const string &request, const string &dflt,
				int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atname );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertText( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Str Options::text( const string &name, int index, const string &dflt,
		   const string &format, const string &unit ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Str s = pp->text( index, format, unit );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::text( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return dflt;
}


Str Options::allText( const string &name, const string &dflt, const string &format,
		      const string &unit, const string &separator ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Str s = pp->allText( format, unit, separator );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::allText( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return dflt;
}


Parameter &Options::setText( const string &name, const string &strg )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setText( strg );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setText( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::pushText( const string &name, const string &strg )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->addText( strg );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::pushText( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setText( const string &name, const Parameter &p )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setText( p );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setText( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Str Options::defaultText( const string &name,
			     const string &format, const string &unit ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Str s = pp->defaultText( format, unit );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultText( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setDefaultText( const string &name, const string &dflt )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDefaultText( dflt );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultText( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::addSelection( const string &name,
				  const string &request,
				  const string &selection,
				  int flags, int style )
{
  return addText( name, request, selection,
		  flags, style | Parameter::SelectText );
}


Parameter &Options::insertSelection( const string &name,
				     const string &atname,
				     const string &request,
				     const string &selection,
				     int flags, int style )
{
  return insertText( name, atname, request, selection,
		     flags, style | Parameter::SelectText );
}


Parameter &Options::selectText( const string &name, const string &strg,
				int add )
{
  iterator pp = find( name );
  if ( pp != end() ) {
    pp->selectText( strg, add );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::selectText( " << name << " ) -> " << Warning << '\n';
#endif

    // notify the change:
    callNotifies();

    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


int Options::index( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    int inx = pp->index();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::index( " << name << " ) -> " << Warning << '\n';
#endif
    return inx;
  }
  else
    return -1;
}


int Options::index( const string &name, const string &strg ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    int inx = pp->index( strg );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::index( " << name << " ) -> " << Warning << '\n';
#endif
    return inx;
  }
  else
    return -1;
}


Parameter &Options::addNumber( const string &name, const string &request,
			       double dflt, double minimum, double maximum,
			       double step,
			       const string &unit, const string &outputunit,
			       const string &format, int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, -1.0, minimum, maximum,
		step, unit, outputunit, format, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addNumber( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertNumber( const string &name, const string &atname,
				  const string &request, double dflt,
				  double minimum, double maximum, double step,
				  const string &unit, const string &outputunit,
				  const string &format, int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, -1.0, minimum, maximum,
		step, unit, outputunit, format, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atname );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertNumber( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


double Options::number( const string &name, double dflt,
			const string &unit, int index ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    double v = pp->number( unit, index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::number( " << name << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return dflt;
}


double Options::error( const string &name, const string &unit, int index ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    double v = pp->error( unit, index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::error( " << name << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


Parameter &Options::setNumber( const string &name, double number, 
			       double error, const string &unit )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setNumber( number, error, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setNumber( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::pushNumber( const string &name, double number, 
				double error, const string &unit )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->addNumber( number, error, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::pushNumber( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setNumber( const string &name, const Parameter &p )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setNumber( p );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setNumber( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


double Options::defaultNumber( const string &name, const string &unit ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    double v = pp->defaultNumber( unit );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultNumber( " << name << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


Parameter &Options::setDefaultNumber( const string &name, double dflt, 
				      const string &unit )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDefaultNumber( dflt, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultNumber( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


double Options::minimum( const string &name, const string &unit ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    double v = pp->minimum( unit );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::minimum( " << name << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


double Options::maximum( const string &name, const string &unit ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    double v = pp->maximum( unit );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::maximum( " << name << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


double Options::step( const string &name, const string &unit ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    double v = pp->step( unit );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::step( " << name << " ) -> " << Warning << '\n';
#endif
    return v;
  }
  else
    return 0.0;
}


Parameter &Options::setStep( const string &name, double step, const string &unit )
{
  iterator pp = find( name );
  // set values:
  if ( pp != end() ) {
    pp->setStep( step, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setStep( " << name << " ) -> " << Warning << '\n';
  }
#endif

  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setMinMax( const string &name, double minimum, 
			       double maximum, double step, const string &unit )
{
  iterator pp = find( name );
  // set values:
  if ( pp != end() ) {
    pp->setMinMax( minimum, maximum, step, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setMinMax( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Str Options::unit( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Str s = pp->unit();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::unit( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Str Options::outUnit( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    Str s = pp->outUnit();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::outUnit( " << name << " ) -> " << Warning << '\n';
#endif
    return s;
  }
  else
    return "";
}


Parameter &Options::setUnit( const string &name, const string &internunit, 
			     const string &outunit )
{
  iterator pp = find( name );
  // set values:
  if ( pp != end() ) {
    pp->setUnit( internunit, outunit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setUnit( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setOutUnit( const string &name,
				const string &outputunit )
{
  iterator pp = find( name );
  // set values:
  if ( pp != end() ) {
    pp->setOutUnit( outputunit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setOutUnit( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::changeUnit( const string &name, const string &internunit )
{
  iterator pp = find( name );
  // set values:
  if ( pp != end() ) {
    pp->changeUnit( internunit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::changeUnit( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::addInteger( const string &name, const string &request,
				long dflt, long minimum, long maximum, long step,
				const string &unit, const string &outputunit,
				int width, int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, -1L, minimum, maximum,
		step, unit, outputunit, width, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addInteger( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertInteger( const string &name, const string &atname,
				   const string &request, long dflt,
				   long minimum, long maximum, long step,
				   const string &unit, const string &outputunit,
				   int width, int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, -1L, minimum, maximum,
		step, unit, outputunit, width, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atname );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertInteger( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


long Options::integer( const string &name,
		       const string &unit, long dflt, int index ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    long i = pp->integer( unit, index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::integer( " << name << " ) -> " << Warning << '\n';
#endif
    return i;
  }
  else
    return dflt;
}


Parameter &Options::setInteger( const string &name, long number, long error,
				const string &unit )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setInteger( number, error, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setInteger( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::pushInteger( const string &name, long number, long error,
				 const string &unit )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->addInteger( number, error, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::pushInteger( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setInteger( const string &name, const Parameter &p )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setInteger( p );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setInteger( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


long Options::defaultInteger( const string &name, const string &unit ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    long i = pp->defaultInteger( unit );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultInteger( " << name << " ) -> " << Warning << '\n';
#endif
    return i;
  }
  else
    return 0;
}


Parameter &Options::setDefaultInteger( const string &name, long dflt,
				       const string &unit )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDefaultInteger( dflt, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultInteger( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setMinMax( const string &name, long minimum,
			       long maximum, long step, const string &unit )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setMinMax( minimum, maximum, step, unit );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setMinMax( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::addBoolean( const string &name, const string &request,
				bool dflt, int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addBoolean( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertBoolean( const string &name, const string &atname,
				   const string &request, bool dflt, int flags, int style )
{
  // new parameter:
  Parameter np( name, request, dflt, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atname );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertBoolean( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


bool Options::boolean( const string &name, bool dflt, int index ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    bool b = pp->boolean( index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::boolean( " << name << " ) -> " << Warning << '\n';
#endif
    return b;
  }
  else
    return dflt;
}


Parameter &Options::setBoolean( const string &name, bool b )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setBoolean( b );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setBoolean( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setBoolean( const string &name, const Parameter &p )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setBoolean( p );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setBoolean( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


bool Options::defaultBoolean( const string &name ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    bool b = pp->defaultBoolean();
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultBoolean( " << name << " ) -> " << Warning << '\n';
#endif
    return b;
  }
  else
    return false;
}


Parameter &Options::setDefaultBoolean( const string &name, bool dflt )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDefaultBoolean( dflt );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultBoolean( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::addDate( const string &name, const string &request,
			     int year, int month, int day,
			     int flags, int style )
{
  // new parameter:
  Parameter np( name, request, Parameter::Date,
		year, month, day, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addDate( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertDate( const string &name, const string &atname,
				const string &request,
				int year, int month, int day,
				int flags, int style )
{
  // new parameter:
  Parameter np( name, request, Parameter::Date,
		year, month, day, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atname );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertDate( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


const Parameter &Options::date( const string &name, int index,
				int &year, int &month, int &day ) const
{
  year = 0;
  month = 0;
  day = 0;
  const_iterator pp = find( name );
  if ( pp != end() ) {
    pp->date( year, month, day, index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::date( " << name << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDate( const string &name,
			     int year, int month, int day )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDate( year, month, day );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDate( const string &name, const string &date )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDate( date );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDate( const string &name, const struct tm &date )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDate( date );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDate( const string &name, const time_t &time )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDate( time );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setCurrentDate( const string &name )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setCurrentDate();
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setCurrentDate( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDate( const string &name, const Parameter &p )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDate( p );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDate( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


const Parameter &Options::defaultDate( const string &name, int index,
				       int &year, int &month, int &day ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    pp->defaultDate( year, month, day, index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultDate( " << name << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDefaultDate( const string &name,
				    int year, int month, int day )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDefaultDate( year, month, day );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultDate( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::addTime( const string &name, const string &request,
			     int hour, int minutes, int seconds,
			     int flags, int style )
{
  // new parameter:
  Parameter np( name, request, Parameter::Time,
		hour, minutes, seconds, flags, style, this );
  // add option:
  Parameter &pp = add( np );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::addTime( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


Parameter &Options::insertTime( const string &name, const string &atname,
				const string &request,
				int hour, int minutes, int seconds,
				int flags, int style )
{
  // new parameter:
  Parameter np( name, request, Parameter::Time,
		hour, minutes, seconds, flags, style, this );
  // insert option:
  Parameter &pp = insert( np, atname );
  // error?
  Warning += np.warning();
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::insertTime( " << name << " ) -> " << Warning << '\n';
#endif
  return pp;
}


const Parameter &Options::time( const string &name, int index,
				int &hour, int &minutes, int &seconds ) const
{
  hour = 0;
  minutes = 0;
  seconds = 0;
  const_iterator pp = find( name );
  if ( pp != end() ) {
    pp->time( hour, minutes, seconds, index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::time( " << name << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setTime( const string &name,
			     int hour, int minutes, int seconds )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setTime( hour, minutes, seconds );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setTime( const string &name, const string &time )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setTime( time );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setTime( const string &name, const struct tm &time )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setTime( time );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setTime( const string &name, const time_t &time )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setTime( time );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setCurrentTime( const string &name )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setCurrentTime();
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setCurrentTime( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setTime( const string &name, const Parameter &p )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setTime( p );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setTime( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() ) {
    // notify the change:
    callNotifies();
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


const Parameter &Options::defaultTime( const string &name, int index,
				       int &hour, int &minutes, int &seconds ) const
{
  const_iterator pp = find( name );
  if ( pp != end() ) {
    pp->defaultTime( hour, minutes, seconds, index );
    Warning += pp->warning();
#ifndef NDEBUG
    if ( ! Warning.empty() )
      cerr << "!warning in Options::defaultTime( " << name << " ) -> " << Warning << '\n';
#endif
    return *pp;
  }
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Parameter &Options::setDefaultTime( const string &name,
				    int hour, int minutes, int seconds )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDefaultTime( hour, minutes, seconds );
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setDefaultTime( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Options &Options::newSection( int level, const string &name, const string &type,
			      int flags, int style )
{
  Warning = "";
  Options *so = this;
  for ( int l=0; l<level; l++ ) {
    if ( so->Secs.empty() ) {
      Warning += "Cannot add a subsection without having the appropriate parent section";
      return *this;
    }
    so = so->Secs.back();
  }
  Options *o = new Options( name, type, flags, style );
  o->setParentSection( so );
  so->Secs.push_back( o );
  so->OwnSecs.push_back( true );
  AddOpts = o;
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::newSection() -> " << Warning << '\n';
#endif
  return *o;
}


Options &Options::newSection( const string &name, const string &type,
			      int flags, int style )
{
  return newSection( 0, name, type, flags, style );
}


Options &Options::newSubSection( const string &name, const string &type,
				 int flags, int style )
{
  return newSection( 1, name, type, flags, style );
}


Options &Options::newSubSubSection( const string &name, const string &type,
				    int flags, int style )
{
  return newSection( 2, name, type, flags, style );
}


Options &Options::addSection( const string &name, const string &type,
			      int flags, int style )
{
  Warning = "";
  Options *o = &AddOpts->newSection( 0, name, type, flags, style );
  AddOpts->clearSections();
  AddOpts = o;
  return *o;
}


Options &Options::insertSection( const string &name, const string &atpattern,
				 const string &type, int flag, int style )
{
  Warning = "";
  Options *o = new Options( name, type, flag, style );

  // insert at front:
  if ( atpattern.empty() ) {
    AddOpts->Secs.push_front( o );
    AddOpts->OwnSecs.push_front( true );
    o->setParentSection( AddOpts );
  }
  else {
    // insert at atpattern:
    section_iterator sp = findSection( atpattern );
    if ( sp != sectionsEnd() ) {
      Options *ps = (*sp)->parentSection();
      if ( ps != 0 ) {
	ps->Secs.insert( sp, o );
	ps->OwnSecs.insert( ps->OwnSecs.begin() + ( sp - ps->Secs.begin() ), true );
	o->setParentSection( ps );
      }
    }
    else {
      // not found, add to sections:
      AddOpts->Secs.push_back( o );
      AddOpts->OwnSecs.push_back( true );
      o->setParentSection( AddOpts );
    }
  }

  AddOpts = o;
  return *AddOpts;
}


Options &Options::newSection( int level, const Options &opt, int selectmask,
			      const string &name, const string &type,
			      int flags, int style )
{
  Warning = "";
  Options *so = this;
  for ( int l=0; l<level; l++ ) {
    if ( so->Secs.empty() ) {
      Warning += "Cannot add a subsection without having the appropriate parent section";
      return *this;
    }
    so = so->Secs.back();
  }
  Options *o = new Options( opt, selectmask );
  if ( ! name.empty() )
    o->setName( name );
  if ( ! type.empty() )
    o->setType( type );
  o->addFlag( flags );
  o->addStyle( style );
  o->setParentSection( so );
  so->Secs.push_back( o );
  so->OwnSecs.push_back( true );
  AddOpts = o;
#ifndef NDEBUG
  if ( !Warning.empty() )
    cerr << "!warning in Options::newSection() -> " << Warning << '\n';
#endif
  return *o;
}


Options &Options::newSection( const Options &opt, int selectmask,
			      const string &name, const string &type,
			      int flags, int style )
{
  return newSection( 0, opt, selectmask, name, type, flags, style );
}


Options &Options::newSubSection( const Options &opt, int selectmask,
				 const string &name, const string &type,
				 int flags, int style )
{
  return newSection( 1, opt, selectmask, name, type, flags, style );
}


Options &Options::newSubSubSection( const Options &opt, int selectmask,
				    const string &name, const string &type,
				    int flags, int style )
{
  return newSection( 2, opt, selectmask, name, type, flags, style );
}


Options &Options::addSection( const Options &opt, int selectmask,
			      const string &name, const string &type,
			      int flags, int style )
{
  Warning = "";
  Options *o = &AddOpts->newSection( 0, opt, selectmask,
				     name, type, flags, style );
  AddOpts->clearSections();
  AddOpts = o;
  return *o;
}


Options &Options::insertSection( const Options &opt, int selectmask,
				 const string &name, const string &atpattern,
				 const string &type, int flag, int style )
{
  Warning = "";
  Options *o = new Options( opt, selectmask );
  if ( ! name.empty() )
    o->setName( name );
  if ( ! type.empty() )
    o->setType( type );
  o->addFlag( flag );
  o->addStyle( style );

  // insert at front:
  if ( atpattern.empty() ) {
    AddOpts->Secs.push_front( o );
    AddOpts->OwnSecs.push_front( true );
    o->setParentSection( AddOpts );
  }
  else {
    // insert at atpattern:
    section_iterator sp = findSection( atpattern );
    if ( sp != sectionsEnd() ) {
      Options *ps = (*sp)->parentSection();
      if ( ps != 0 ) {
	ps->Secs.insert( sp, o );
	ps->OwnSecs.insert( ps->OwnSecs.begin() + ( sp - ps->Secs.begin() ), true );
	o->setParentSection( ps );
      }
    }
    else {
      // not found, add to sections:
      AddOpts->Secs.push_back( o );
      AddOpts->OwnSecs.push_back( true );
      o->setParentSection( AddOpts );
    }
  }

  AddOpts = o;
  return *AddOpts;
}


Options &Options::newSection( Options *opt, bool newparent )
{
  Warning = "";
  Secs.push_back( opt );
  OwnSecs.push_back( false );
  if ( Secs.back()->parentSection() == 0 || newparent )
    Secs.back()->setParentSection( this );
  return *this;
}


Options &Options::addSection( Options *opt, bool newparent )
{
  Warning = "";
  AddOpts->Secs.push_back( opt );
  AddOpts->OwnSecs.push_back( false );
  if ( opt->parentSection() == 0 || newparent )
    opt->setParentSection( AddOpts );
  return *AddOpts;
}


Options &Options::insertSection( Options *opt, const string &atpattern,
				 bool newparent )
{
  Warning = "";
  // insert at front:
  if ( atpattern.empty() ) {
    AddOpts->Secs.push_front( opt );
    AddOpts->OwnSecs.push_front( false );
    if ( opt->parentSection() == 0 || newparent )
      opt->setParentSection( AddOpts );
  }
  else {
    // insert at atpattern:
    section_iterator sp = findSection( atpattern );
    if ( sp != sectionsEnd() ) {
      Options *ps = (*sp)->parentSection();
      if ( ps != 0 ) {
	ps->Secs.insert( sp, opt );
	ps->OwnSecs.insert( ps->OwnSecs.begin() + ( sp - ps->Secs.begin() ), false );
	if ( opt->parentSection() == 0 || newparent )
	  opt->setParentSection( ps );
      }
    }
    else {
      // not found, add to sections:
      AddOpts->Secs.push_back( opt );
      AddOpts->OwnSecs.push_back( false );
      if ( opt->parentSection() == 0 || newparent )
	opt->setParentSection( AddOpts );
    }
  }
  return *AddOpts;
}


Options &Options::newSections( Options *opt, bool newparent )
{
  Warning = "";
  for ( unsigned int k=0; k<opt->Secs.size(); k++ ) {
    Secs.push_back( opt->Secs[k] );
    OwnSecs.push_back( false );
    if ( Secs.back()->parentSection() == 0 || newparent )
      Secs.back()->setParentSection( this );
  }
  return *this;
}


void Options::endSection( void )
{
  Options *newaddopts = AddOpts->parentSection();
  if ( newaddopts != 0 )
    AddOpts = newaddopts;
}


void Options::clearSections( void )
{
  AddOpts = this;
}


void Options::setSection( Options &opt )
{
  AddOpts = &opt;
}


int Options::up( void )
{
  if ( parentSection() == 0 )
    return -1;

  int r = 0;
  if ( parentSection()->Opt.size() > 0 )
    r |= 2;
  if ( parentSection()->Secs.size() > 1 )
    r |= 4;

  parentSection()->Name = Name;
  parentSection()->Type = Type;
  parentSection()->Include = Include;
  parentSection()->Flag = Flag;
  parentSection()->Style = Style;
  parentSection()->Opt = Opt;
  for ( iterator pp = parentSection()->begin();
	pp != parentSection()->end();
	++pp )
    pp->setParentSection( parentSection() );
  parentSection()->Secs.clear();
  for ( unsigned int k=0; k<Secs.size(); k++ ) {
    Secs[k]->setParentSection( parentSection() );
    parentSection()->Secs.push_back( Secs[k] );
    parentSection()->OwnSecs.push_back( OwnSecs[k] );
  }
  parentSection()->AddOpts = parentSection();
  return r;
}


int Options::down( void )
{
  if ( name().empty() && type().empty() )
    return -1;

  Options *o = new Options( *this );
  o->setParentSection( this );
  Secs.clear();
  Secs.push_back( o );
  OwnSecs.clear();
  OwnSecs.push_back( true );
  Opt.clear();
  setName( "" );
  setType( "" );
  setInclude( "" );
  setStyle( 0 );

  return 0;
}


Parameter &Options::setDefault( const string &name )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setDefault();
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    cerr << "!warning in Options::setDefault( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Options &Options::setDefaults( int flags )
{
  Warning = "";
  // set defaults:
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( flags ) ) {
      pp->setDefault();
    }
  }
  for ( section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( flags ) )
      (*sp)->setDefaults( flags );
  }
  return *this;
}


Parameter &Options::setToDefault( const string &name )
{
  iterator pp = find( name );
  // set value:
  if ( pp != end() ) {
    pp->setToDefault();
    Warning += pp->warning();
  }
#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::setToDefault( " << name << " ) -> " << Warning << '\n';
  }
#endif
  if ( pp != end() )
    return *pp;
  else {
    Dummy = Parameter();
    return Dummy;
  }
}


Options &Options::setToDefaults( int flags )
{
  Warning = "";
  // set defaults:
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( flags ) ) {
      pp->setToDefault();
    }
  }
  for ( section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( flags ) )
      (*sp)->setToDefaults( flags );
  }
  return *this;
}


Options &Options::erase( Options::iterator p )
{
  if ( p != end() ) {
    Options *po = p->parentSection();
    po->Opt.erase( p );
  }
  return *this;
}


Options &Options::erase( Options::section_iterator s )
{
  if ( s != sectionsEnd() ) {
    Options *po = (*s)->parentSection();
    if ( po == 0 ) {
      cerr << "ERROR in OPTIONS::erase( sectio_iterator ): no parentSection()!\n";
      return *this;
    }
    int inx = s - po->Secs.begin();
    if ( po->OwnSecs[ inx ] )
      delete *s;
    else {
      if ( (*s)->parentSection() == po )
	(*s)->setParentSection( 0 );
    }
    po->Secs.erase( s );
    po->OwnSecs.erase( po->OwnSecs.begin() + inx );
  }
  return *this;
}


Options &Options::erase( Options *s )
{
  int inx = 0;
  for ( section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp, ++inx ) {
    if ( *sp == s ) {
      if ( OwnSecs[ inx ] )
	delete *sp;
      else {
	if ( s->parentSection() == this )
	  s->setParentSection( 0 );
      }
      Secs.erase( sp );
      OwnSecs.erase( OwnSecs.begin() + inx );
      break;
    }
  }
  return *this;
}


Options &Options::erase( const string &pattern )
{
  Warning = "";

  bool erased = false;
  iterator pp = end();
  while ( (pp = find( pattern )) != end() ) {
    Options *po = pp->parentSection();
    po->Opt.erase( pp );
    erased = true;
  }

  section_iterator sp = sectionsEnd();
  while ( (sp = findSection( pattern )) != sectionsEnd() ) {
    Options *po = (*sp)->parentSection();
    if ( po == 0 ) {
      cerr << "ERROR in Options::erase( pattern ) : no parentSection()!\n";
      continue;
    }
    int inx = sp - po->Secs.begin();
    if ( po->OwnSecs[ inx ] )
      delete *sp;
    else {
      if ( (*sp)->parentSection() == po )
	(*sp)->setParentSection( 0 );
    }
    po->Secs.erase( sp );
    po->OwnSecs.erase( po->OwnSecs.begin() + inx );
    erased = true;
  }

  // not found:
  if ( ! erased )
    Warning = "cannot erase '" + pattern + "': not found!";

  return *this;
}


Options &Options::erase( int selectflag )
{
  Warning = "";
  // search element:
  for ( iterator pp = begin(); pp != end(); ) {
    if ( pp->flags( selectflag ) )
      pp = Opt.erase( pp );
    else
      ++pp;
  }

  for ( section_iterator sp = sectionsBegin(); sp != sectionsEnd(); ) {
    if ( (*sp)->flag() != 0 && (*sp)->flag( selectflag ) ) {
      int inx = sp - Secs.begin();
      if ( OwnSecs[ inx ] )
	delete *sp;
      else {
	if ( (*sp)->parentSection() == this )
	  (*sp)->setParentSection( 0 );
      }
      sp = Secs.erase( sp );
      OwnSecs.erase( OwnSecs.begin() + inx );
    }
    else {
      (*sp)->erase( selectflag );
      if ( (*sp)->size() == 0 ) {
	int inx = sp - Secs.begin();
	if ( OwnSecs[ inx ] )
	  delete *sp;
	else {
	  if ( (*sp)->parentSection() == this )
	    (*sp)->setParentSection( 0 );
	}
	sp = Secs.erase( sp );
	OwnSecs.erase( OwnSecs.begin() + inx );
      }
      else
	++sp;
    }
  }

  return *this;
}


Options &Options::pop( void )
{
  Warning = "";
  if ( ! AddOpts->Opt.empty() )
    AddOpts->Opt.pop_back();

  return *this;
}


Options &Options::popSection( void )
{
  Warning = "";
  if ( ! AddOpts->Secs.empty() ) {
    if ( AddOpts->OwnSecs.back() )
      delete AddOpts->Secs.back();
    else {
      if ( AddOpts->Secs.back()->parentSection() == AddOpts )
	AddOpts->Secs.back()->setParentSection( 0 );
    }
    AddOpts->Secs.pop_back();
    AddOpts->OwnSecs.pop_back();
  }

  return *this;
}


Options &Options::clear( void )
{
  Warning = "";
  Name = "";
  Type = "";
  Include = "";
  Flag = 0;
  Style = 0;
  Opt.clear();
  int inx = 0;
  for ( section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp, ++inx ) {
    if ( OwnSecs[inx] )
      delete *sp;
    else {
      if ( (*sp)->parentSection() == this )
	(*sp)->setParentSection( 0 );
    }
  }
  Secs.clear();
  OwnSecs.clear();
  AddOpts = this;
  return *this;
}


int Options::size( void ) const
{
  Warning = "";
  int n = Opt.size();
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    n += (*sp)->size();
  }
  return n;
}


int Options::size( int flags ) const
{
  Warning = "";
  int n=0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( flags ) )
      n++;
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp )
    n += (*sp)->size( flags );
  return n;
}


bool Options::empty( void ) const
{
  Warning = "";
  return ( size() <= 0 );
}


int Options::parameterSize( void ) const
{
  return Opt.size();
}


int Options::parameterSize( int flags ) const
{
  int n=0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( flags ) )
      n++;
  }
  return n;
}


int Options::sectionsSize( void ) const
{
  return Secs.size();
}


int Options::sectionsSize( int flags ) const
{
  int n=0;
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->size( flags ) > 0 )
      n++;
  }
  return n;
}


bool Options::exist( const string &pattern ) const
{
  const_iterator pp = find( pattern );
  Warning = "";
  return ( pp != end() );
}


bool Options::existSection( const string &pattern ) const
{
  Warning = "";
  if ( *this == pattern )
    return true;

  const_section_iterator sp = findSection( pattern );
  Warning = "";
  return ( sp != sectionsEnd() );
}


Options &Options::setFlags( int flags, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectflag ) )
      pp->setFlags( flags );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectflag ) ) {
      if ( (*sp)->flag() != 0 )
	(*sp)->setFlag( flags );
      (*sp)->setFlags( flags, selectflag );
    }
  }
  return *this;
}


Options &Options::addFlags( int flags, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectflag ) )
      pp->addFlags( flags );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectflag ) ) {
      if ( (*sp)->flag() != 0 )
	(*sp)->addFlag( flags );
      (*sp)->addFlags( flags, selectflag );
    }
  }
  return *this;
}


Options &Options::delFlags( int flags, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectflag ) )
      pp->delFlags( flags );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectflag ) ) {
      if ( (*sp)->flag() != 0 )
	(*sp)->delFlag( flags );
      (*sp)->delFlags( flags, selectflag );
    }
  }
  return *this;
}


Options &Options::setValueTypeFlags( int flags, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->valueType( typemask ) )
      pp->setFlags( flags );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->checkType( typemask ) )
      (*sp)->setFlag( flags );
    (*sp)->setValueTypeFlags( flags, typemask );
  }
  return *this;
}


Options &Options::addValueTypeFlags( int flags, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->valueType( typemask ) )
      pp->addFlags( flags );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->checkType( typemask ) )
      (*sp)->addFlag( flags );
    (*sp)->addValueTypeFlags( flags, typemask );
  }
  return *this;
}


Options &Options::delValueTypeFlags( int flags, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->valueType( typemask ) )
      pp->delFlags( flags );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->checkType( typemask ) )
      (*sp)->delFlag( flags );
    (*sp)->delValueTypeFlags( flags, typemask );
  }
  return *this;
}


Options &Options::setStyles( int style, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectflag ) )
      pp->setStyle( style );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectflag ) ) {
      if ( (*sp)->flag() != 0 )
	(*sp)->setStyle( style );
      (*sp)->setStyles( style, selectflag );
    }
  }
  return *this;
}


Options &Options::addStyles( int style, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectflag ) )
      pp->addStyle( style );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectflag ) ) {
      if ( (*sp)->flag() != 0 )
	(*sp)->addStyle( style );
      (*sp)->addStyles( style, selectflag );
    }
  }
  return *this;
}


Options &Options::delStyles( int style, int selectflag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectflag ) )
      pp->delStyle( style );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectflag ) ) {
      if ( (*sp)->flag() != 0 )
	(*sp)->delStyle( style );
      (*sp)->delStyles( style, selectflag );
    }
  }
  return *this;
}


int Options::styleSize( int style ) const
{
  Warning = "";
  int n=0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( ( pp->style() & style ) == style )
      n++;
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    n += (*sp)->styleSize( style );
  }
  return n;
}


Options &Options::setValueTypeStyles( int style, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->valueType( typemask ) )
      pp->setStyle( style );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->checkType( typemask ) )
      (*sp)->setStyle( style );
    (*sp)->setValueTypeStyles( style, typemask );
  }
  return *this;
}


Options &Options::addValueTypeStyles( int style, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->valueType( typemask ) )
      pp->addStyle( style );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->checkType( typemask ) )
      (*sp)->addStyle( style );
    (*sp)->addValueTypeStyles( style, typemask );
  }
  return *this;
}


Options &Options::delValueTypeStyles( int style, int typemask )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->valueType( typemask ) )
      pp->delStyle( style );
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->checkType( typemask ) )
      (*sp)->delStyle( style );
    (*sp)->delValueTypeStyles( style, typemask );
  }
  return *this;
}


int Options::nameWidth( int selectmask, bool detailed ) const
{
  Warning = "";

  // search largest name:
  unsigned int width = 0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectmask ) ) {
      unsigned int w = pp->name().size();
      if ( pp->name().find_first_of( ",{}[]:=" ) != string::npos )
	w += 2;
      if ( detailed && pp->name() != pp->request() ) {
	w += 3 + pp->request().size();
	if ( pp->request().find_first_of( ",{}[]:=" ) != string::npos )
	  w += 2;
      }
      if ( w > width )
	width = w;
    }
  }

  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->size( selectmask ) > 0 &&
	 ( ! flag( selectmask ) ||
	   ( name().empty() && type().empty() ) ) ) {
      unsigned int w = (*sp)->nameWidth( selectmask, detailed );
      if ( w > width )
	width = w;
    }
  }
  return width;
}


ostream &Options::save( ostream &str, const string &start,
			int selectmask, int flags, int width ) const
{
  Warning = "";

  string starts = start;

  // write options to file:
  string ns = name();
  string ts = type();
  if ( ( flags & SwitchNameType ) > 0 ) {
    ns = type();
    ts = name();
  }
  bool printname = ( ( ! ns.empty() ) && ( ( flags & NoName ) == 0 ) );
  bool printtype = ( ( ! ts.empty() ) && ( ( flags & NoType ) == 0 ) );
  bool printsection = ( flag( selectmask ) && ( printname || printtype ) );
  if ( printsection ) {
    if ( printname ) {
      if ( (flags & PrintStyle) && (style() & TabSection) > 0 )
	ns = '-' + ns + '-';
      if ( ns.find_first_of( ",{}[]:=" ) != string::npos )
	str << starts << '"' << ns << '"';
      else
	str << starts << ns;
    }
    if ( printtype ) {
      if ( ts.find_first_of( ",{}[]:=" ) != string::npos )
	str << " (\"" << ts << "\")";
      else
	str << " (" << ts << ")";
    }
    str << ":\n";
    starts += "    ";
    width = nameWidth( selectmask, (flags & PrintRequest) );
  }
  if ( width < 0 )
    width = nameWidth( selectmask, (flags & PrintRequest) );
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectmask ) ) {
      str << starts;
      pp->save( str, width, flags ) << '\n';
    }
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->size( selectmask ) > 0 )
      (*sp)->save( str, starts, selectmask, flags, width );
  }

  return str;
}


ostream &Options::save( ostream &str, const string &textformat,
			const string &numberformat, const string &boolformat,
			const string &dateformat, const string &timeformat,
			const string &sectionformat, int selectmask,
			const string &start ) const
{
  Warning = "";

  string starts = start;

  // write options to file:
  if ( ! name().empty() ) {
    Str f( sectionformat );
    f.format( name(), 'i' );
    f.format( name(), 's' );
    f.format( type(), 'T' );
    str << starts << f << '\n';
    starts += "    ";
  }
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectmask ) ) {
      str << starts;
      pp->save( str, textformat, numberformat, boolformat,
		dateformat, timeformat );
      str << '\n';
    }
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectmask ) )
      (*sp)->save( str, textformat, numberformat, boolformat,
		   dateformat, timeformat, sectionformat, selectmask, starts );
  }

  return str;
}


string Options::save( int selectmask, int flags ) const
{
  Warning = "";

  // write options to string:
  string str;

  string ns = name();
  string ts = type();
  if ( ( flags & SwitchNameType ) > 0 ) {
    ns = type();
    ts = name();
  }
  bool printname = ( ( ! ns.empty() ) && ( ( flags & NoName ) == 0 ) );
  bool printtype = ( ( ! ts.empty() ) && ( ( flags & NoType ) == 0 ) );
  bool printsection = ( flag( selectmask ) && ( printname || printtype ) );
  if ( printsection ) {
    if ( printname ) {
      if ( (flags & PrintStyle) && (style() & TabSection) > 0 )
	ns = '-' + ns + '-';
      if ( ns.find_first_of( ",{}[]:=" ) != string::npos )
	str += '"' + ns + '"';
      else
	str += ns;
    }
    if ( printtype ) {
      if ( ts.find_first_of( ",{}[]:=" ) != string::npos )
	str += " (\"" + ts + "\")";
      else
	str += " (" + ts + ")";
    }
    str += ": { ";
  }
  else if ( flags & Embrace )
    str += "{ ";

  int n=0;
  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectmask ) ) {
      if ( n > 0 )
	str += ", ";
      str += pp->save( flags );
      n++;
    }
  }
  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->size( selectmask ) > 0 ) {
      if ( n > 0 )
	str += ", ";
      str += (*sp)->save( selectmask, flags );
      ++n;
    }
  }
  if ( printsection || (flags & Embrace) )
    str += " }";

  return str;
}


ostream &operator<< ( ostream &str, const Options &o )
{
  o.save( str );

  return str;
}


ostream &Options::saveXML( ostream &str, int selectmask, int flags, int level,
			   int indent ) const
{
  string indstr1( level*indent, ' ' );
  string indstr2( (level+1)*indent, ' ' );

  string ns = name();
  string ts = type();
  if ( ( flags & SwitchNameType ) > 0 ) {
    ns = type();
    ts = name();
  }
  bool printname = ( ( ! ns.empty() ) && ( ( flags & NoName ) == 0 ) );
  bool printtype = ( ( ! ts.empty() ) && ( ( flags & NoType ) == 0 ) );
  bool printsection = ( flag( selectmask ) && ( printname || printtype ) );
  if ( printsection ) {
    str << indstr1 << "<section>\n";
    if ( printname )
      str << indstr2 << "<name>" << ns << "</name>\n";
    if ( printtype )
      str << indstr2 << "<type>" << ts << "</type>\n";
    if ( ! include().empty() && ( flags & NoInclude ) == 0 )
      str << indstr2 << "<include>" << include() << "</include>\n";
    level++;
  }

  for ( const_iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->flags( selectmask ) ) {
      pp->saveXML( str, level, indent, flags );
    }
  }

  for ( const_section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->flag( selectmask ) )
      (*sp)->saveXML( str, selectmask, flags, level, indent );
  }

  if ( printsection )
    str << indstr1 << "</section>\n";

  return str;
}


Options &Options::read( const string &opttxt, int flag,
			const string &assignment, const string &separator,
			int *indent )
{
  Warning = "";
  bool cn = CallNotify;
  CallNotify = false;

  int myindent = -1;
  if ( indent == 0 )
    indent = &myindent;

  Options *retopt = this;
  Str s = opttxt;
  int newindent = s.findFirstNot( " {" );
  if ( newindent >= 0 && newindent < *indent ) {
    // end of section, keep searching the parent:
    *indent = newindent;
    Options *ps = parentSection();
    if ( ps != 0 ) {
      retopt = &ps->read( opttxt, flag, assignment, separator, indent );
      // notify the change:
      CallNotify = cn;
      callNotifies();
      return *retopt;
    }
  }
  if ( newindent >= 0 )
    *indent = newindent;

  // remove white space and curly braces:
  s.strip();
  if ( ! s.empty() && s[0] == '{' ) {
    s.preventFirst( '{' );
    s.preventLast( '}' );
    s.strip();
  }
  if ( s.empty() ) {
    // notify the change:
    CallNotify = cn;
    callNotifies();
    return *retopt;
  }

  // split up parameter list:
  int index = 0;
  int next = 0;
  do {
    // XXX keep care of leftover closing curly bracket here!
    // extract name:
    next = s.findSeparator( index, assignment, "\"" );
    Str name = s.mid( index, next-1 );
    name.strip( Str::WhiteSpace + "-\"" );
    if ( ! name.empty() && name[name.size()-1] == ')' ) {
      name.erase( name.find( '(' ) );  // erase request string/section type specifier
      name.strip( Str::WhiteSpace + "\"-" );
    }
    // go to value:
    if ( next >= 0 )
      index = s.findFirstNot( Str::WhiteSpace, next+1 );
    else
      index = -1;
    if ( index >= 0 ) {
      // end of value:
      next = s.findSeparator( index, separator, "{[\"" );
      if ( s[index] == '{' ) {
	// subsection:
	index = s.findFirstNot( Str::WhiteSpace, index+1 );
	int r = s.findLastNot( Str::WhiteSpace, next<0?s.size()-1:next-1 );
	bool closing = false;
	if ( r >= 0 && s[r] == '}' ) {
	  r = s.findLastNot( Str::WhiteSpace, r-1 );
	  closing = true;
	}
	if ( r > index ) {
	  // read section:
	  string secstr = s.mid( index, r );
	  if ( name == Options::name() ) {
	    string error = Warning;
	    read( secstr, flag, assignment, separator );
	    Warning = error + Warning;
	  }
	  else {
	    string error = Warning;
	    section_iterator sp = findSection( name );
	    Warning = error + Warning;
	    if ( sp != sectionsEnd() ) {
	      error = Warning;
	      (*sp)->read( secstr, flag, assignment, separator );
	      Warning = error + Warning;
	      if ( ! closing )
		retopt = *sp;
	    }
	  }
	}
	else {
	  // empty sections:
	  break;
	}
      }
      else {
	// value:
	next = s.findSeparator( index, separator, "[\"" );
	int r = s.findLastNot( Str::WhiteSpace, next<0?s.size()-1:next-1 );
	string value = s.mid( index, r );
	// set value for Parameter:
	string error = Warning;
	Parameter *pp = assign( name, value );
	// set flags:
	if ( pp != 0 && flag != 0 )
	  pp->addFlags( flag );
	Warning += error;
      }
      index = next<0 ? -1 : next+1;
    }
    else {
      // empty last value, i.e. a section:
      if ( name == Options::name() )
	break;
      // this is a new section:
      string error = Warning;
      Options *ps = this;
      bool found = false;
      do {
	section_iterator spp = ps->findSection( name );
	if ( spp != ps->sectionsEnd() ) {
	  retopt = *spp;
	  found = true;
	  break;
	}
	ps = ps->parentSection();
      } while ( ps != 0 );
      Warning = error;
      if ( ! found )
	Warning += "requested section '" + name + "' not found!";
      break;
    }
  } while ( index >= 0 );

#ifndef NDEBUG
  if ( ! Warning.empty() )
    cerr << "!warning in Options::read() -> " << Warning << '\n';
#endif

  // notify the change:
  CallNotify = cn;
  callNotifies();

  return *retopt;
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
  int indent = -1;
  Options *copt = this;

  // read first line:
  if ( line != 0 && !line->empty() ) {
    // read line:
    string error = Warning;
    copt = &copt->read( *line, flag, assignment, "", &indent );
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
    copt = &copt->read( s, flag, assignment, ",;", &indent );
    Warning = error + Warning;
  }

  // store last read line:
  if ( line != 0 )
    *line = s;

  // notify the change:
  CallNotify = cn;
  callNotifies();

  return str;
}


Options &Options::read( const StrQueue &sq, int flag,
			const string &assignment )
{
  bool cn = CallNotify;
  CallNotify = false;

  Warning = "";
  int indent = -1;
  Options *copt = this;
  for ( StrQueue::const_iterator i = sq.begin(); i != sq.end(); ++i ) {
    // read line:
    string error = Warning;
    copt = &copt->read( *i, flag, assignment, ",;", &indent );
    Warning = error + Warning;
  }

  // notify the change:
  CallNotify = cn;
  callNotifies();

  return *this;
}


bool Options::read( const Parameter &p, int flag )
{
  for ( iterator pp = begin(); pp != end(); ++pp ) {
    if ( pp->read( p ) ) {
      pp->addFlags( flag );
      // notify the change:
      callNotifies();
      return true;
    }
  }
  for ( section_iterator sp = sectionsBegin();
	sp != sectionsEnd();
	++sp ) {
    if ( (*sp)->read( p, flag ) ) {
      // notify the change:
      callNotifies();
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
	if ( pp->read( *op ) ) {
	  pp->addFlags( flag );
	  changed = true;
	  break;
	}
      }
    }
  }
  for ( const_section_iterator op = o.sectionsBegin();
	op != o.sectionsEnd();
	++op ) {
    if ( (*op)->flag( flags ) ) {
      for ( section_iterator sp = sectionsBegin();
	    sp != sectionsEnd();
	    ++sp ) {
	if ( (*op)->name() == (*sp)->name() ) {
	  (*sp)->read( **op, flags, flag );
	  (*sp)->addFlag( flag );
	  changed = true;
	  break;
	}
      }
    }
  }

  // notify the change:
  if ( changed )
    callNotifies();

  return *this;
}


bool Options::readAppend( const Parameter &p )
{
  if ( ! p.name().empty() ) {
    if ( read( p ) )
      return true;
  }

  // not found:
  add( p );

  // notify the change:
  callNotifies();

  return false;
}


Options &Options::readAppend( const Options &o, int flags )
{
  for ( const_iterator op = o.begin(); op != o.end(); ++op ) {
    if ( (*op).flags( flags ) ) {
      bool app = true;
      if ( ! (*op).name().empty() ) {
	for ( iterator pp = begin(); pp != end(); ++pp ) {
	  if ( pp->read( *op ) ) {
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
  callNotifies();

  return *this;
}


Options &Options::load( const Str &opttxt,
			const string &assignment, const string &separator,
			int *indent, int *indentspacing, int *level )
{
  Warning = "";

  Options *retopt = this;
  Str s = opttxt;
  int newindent = s.findFirstNot( " {" );

  // remove white space and curly braces:
  s.strip();
  if ( ! s.empty() && s[0] == '{' ) {
    s.preventFirst( '{' );
    s.preventLast( '}' );
    s.strip();
  }
  if ( s.empty() )
    return *retopt;

  int myindent = -1;
  if ( indent == 0 )
    indent = &myindent;
  int myindentspacing = 0;
  if ( indentspacing == 0 )
    indentspacing = &myindentspacing;
  int mylevel = -1;
  if ( level == 0 )
    level = &mylevel;
  (*level)++;

  bool cn = CallNotify;
  CallNotify = false;

  // split up parameter list:
  int index = 0;
  int next = 0;
  do {
    // XXX keep care of leftover closing curly bracket here!
    // extract name:
    next = s.findSeparator( index, assignment, "\"" );
    Str name = s.mid( index, next-1 );
    name.strip();
    // go to value:
    if ( next >= 0 )
      index = s.findFirstNot( Str::WhiteSpace, next+1 );
    else
      index = -1;
    if ( index >= 0 ) {
      // end of value:
      next = s.findSeparator( index, separator, "{[\"" );
      if ( s[index] == '{' ) {
	// subsection:
	index = s.findFirstNot( Str::WhiteSpace, index+1 );
	int r = s.findLastNot( Str::WhiteSpace, next<0?s.size()-1:next-1 );
	bool closing = false;
	if ( r >= 0 && s[r] == '}' ) {
	  r = s.findLastNot( Str::WhiteSpace, r-1 );
	  closing = true;
	}
	if ( r > index ) {
	  // load section:
	  string secstr = s.mid( index, r );
	  Str type = "";
	  int ri = name.find( '(' );
	  if ( name.size() > 2 && name[name.size()-1] == ')' && ri > 0 ) {
	    type = name.mid( ri+1, name.size()-2 );
	    type.strip( Str::WhiteSpace + '"' );
	    name.erase( ri );
	  }
	  name.strip( Str::WhiteSpace + '"' );
	  int style = 0;
	  if ( name.size() > 2 &&
	       name[0] == '-' &&
	       name[name.size()-1] == '-' ) {
	    style = TabSection;
	    name.strip( Str::WhiteSpace + '-' );
	  }
	  if ( Name.empty() && Opt.empty() && Secs.empty() ) {
	    setName( name );
	    setType( type );
	    setStyle( style );
	    load( secstr, assignment, separator, indent, indentspacing, level );
	  }
	  else {
	    if ( *level == 0 && Secs.empty() &&
		 parentSection() == 0 && ! Name.empty() ) {
	      // downgrade current options:
	      CallNotify = cn;
	      down();
	      CallNotify = false;
	    }
	    Options *o = new Options( name, type, 0, style );
	    o->setParentSection( this );
	    Secs.push_back( o );
	    OwnSecs.push_back( true );
	    AddOpts = o;
	    string error = Warning;
	    o->load( secstr, assignment, separator, indent, indentspacing, level );
	    Warning = error + Warning;
	    if ( ! closing )
	      retopt = o;
	  }
	}
	else {
	  // empty sections:
	  break;
	}
      }
      else {
	// value:
	next = s.findSeparator( index, separator, "[\"" );
	int r = s.findLastNot( Str::WhiteSpace, next<0?s.size()-1:next-1 );
	string value = s.mid( index, r );
	// load new parameter:
	Parameter np( name, value );
	np.setParentSection( this );
	Warning += np.warning();
	Opt.push_back( np );
      }
      index = next<0 ? -1 : next+1;
    }
    else {
      // empty last value, i.e. a section:
      int changeindent = *indentspacing > 0 ? *indentspacing : 1;
      bool firstsection = false;
      if ( newindent >= 0 ) {
	if ( *indent >= 0 ) {
	  changeindent = newindent - *indent;
	  if ( *indentspacing == 0 )
	    *indentspacing = changeindent;
	}
	else
	  firstsection = true;
	*indent = newindent;
      }
      // extract name and type:
      Str type = "";
      int ri = name.find( '(' );
      if ( name.size() > 2 && name[name.size()-1] == ')' && ri > 0 ) {
	type = name.mid( ri+1, name.size()-2 );
	type.strip( Str::WhiteSpace + '"' );
	name.erase( ri );
      }
      name.strip( Str::WhiteSpace + '"' );
      int style = 0;
      if ( name.size() > 2 &&
	   name[0] == '-' &&
	   name[name.size()-1] == '-' ) {
	style = TabSection;
	name.strip( Str::WhiteSpace + '-' );
      }
      // this is a new section:
      if ( firstsection && Name.empty() && Opt.empty() && Secs.empty() ) {
	// this is the name of the current options:
	setName( name );
	setType( type );
	setStyle( style );
      }
      else {
	if ( changeindent == 0 && Secs.empty() &&
	     parentSection() == 0 && ! Name.empty() ) {
	  // downgrade current options:
	  CallNotify = cn;
	  down();
	  CallNotify = false;
	  changeindent = 1;
	}
	if ( changeindent > 0 ) {
	  Options *o = new Options( name, type, 0, style );
	  o->setParentSection( this );
	  Secs.push_back( o );
	  OwnSecs.push_back( true );
	  AddOpts = o;
	  retopt = o;
	}
	else {
	  int is = 4;
	  if ( *indentspacing > 0 )
	    is = *indentspacing;
	  Options *pps = this;
	  Options *ps = parentSection();
	  for ( int k=0; k < -changeindent/is && ps != 0; k++ ) {
	    pps = ps;
	    ps = ps->parentSection();
	  }
	  if ( ps == 0 )
	    ps = pps;
	  Options *o = new Options( name, type, 0, style );
	  o->setParentSection( ps );
	  ps->Secs.push_back( o );
	  ps->OwnSecs.push_back( true );
	  AddOpts = o;
	  retopt = o;
	}
      }
      break;
    }
  } while ( index >= 0 );

#ifndef NDEBUG
  if ( ! Warning.empty() )
    cerr << "!warning in Options::read() -> " << Warning << '\n';
#endif

  // notify the change:
  CallNotify = cn;
  if ( *level == 0 )
    callNotifies();

  (*level)--;

  return *retopt;
}


istream &Options::load( istream &str, const string &assignment,
			const string &comment,
			const string &stop, string *line )
{
  Warning = "";
  Str s = "";
  int indent = -1;
  int indentspacing = 0;
  int level = -1;
  Options *copt = this;
  bool stopempty = ( stop == StrQueue::StopEmpty );

  bool cn = CallNotify;
  CallNotify = false;

  // read first line:
  if ( line != 0 && !line->empty() ) {
    s = line;
    // erase comments:
    s.stripComment( comment );
    // load line:
    copt = &copt->load( s, assignment, ",;", &indent, &indentspacing, &level );
    Warning += copt->warning();
  }

  // get line:
  while ( getline( str, s ) ) {
    // stop line reached:
    if ( ( !stop.empty() && s.find( stop ) == 0 ) ||
	 ( stopempty && s.findFirstNot( Str::WhiteSpace ) < 0 ) )
      break;

    // erase comments:
    s.stripComment( comment );

    // load line:
    copt = &copt->load( s, assignment, ",;", &indent, &indentspacing, &level );
    Warning += copt->warning();
  }

  // store last read line:
  if ( line != 0 )
    *line = s;

  CallNotify = cn;
  callNotifies();

  return str;
}


Options &Options::load( const StrQueue &sq, const string &assignment )
{
  Warning = "";
  int indent = -1;
  int indentspacing = 0;
  int level = -1;
  Options *copt = this;

  bool cn = CallNotify;
  CallNotify = false;

  for ( StrQueue::const_iterator i = sq.begin(); i != sq.end(); ++i ) {
    // load string:
    copt = &copt->load( *i, assignment, ",;", &indent, &indentspacing, &level );
    Warning += copt->warning();
  }

#ifndef NDEBUG
  if ( ! Warning.empty() ) {
    // error?
    cerr << "!warning in Options::load( StrQueue ) -> " << Warning << '\n';
  }
#endif

  CallNotify = cn;
  callNotifies();

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


void Options::callNotifies( void )
{
  bool tn = Notified;
  bool rn = rootSection()->Notified;
  Notified = true;
  rootSection()->Notified = true;
  if ( CallNotify && ! tn )
    notify();
  if ( rootSection() != this &&
       CallNotify && rootSection()->CallNotify && ! rn )
    rootSection()->notify();
  rootSection()->Notified = rn;
  Notified = tn;
}


}; /* namespace relacs */

