/*
  tablekey.cc
  Handling a table header

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

#include <cmath>
#include <relacs/strqueue.h>
#include <relacs/tabledata.h>
#include <relacs/tablekey.h>

namespace relacs {


TableKey::TableKey( void )
  : Opt(),
    Columns(),
    Width(),
    PrevCol( -1 ),
    Comment( "#" ),
    KeyStart( "# " ),
    DataStart( "  " ),
    Separator( "  " ),
    Missing( "-" )
{
}


TableKey::TableKey( const TableKey &key )
  : Opt( key.Opt ),
    Columns( key.Columns ),
    Width( key.Width ),
    PrevCol( key.PrevCol ),
    Comment( key.Comment ),
    KeyStart( key.KeyStart ),
    DataStart( key.DataStart ),
    Separator( key.Separator ),
    Missing( key.Missing )
{
}


TableKey::TableKey( const Options &o )
  : Opt( o ),
    Columns(),
    Width(),
    PrevCol( -1 ),
    Comment( "#" ),
    KeyStart( "# " ),
    DataStart( "  " ),
    Separator( "  " ),
    Missing( "-" )
{
  init();
}


TableKey::~TableKey( void )
{
  clear();
}


Parameter &TableKey::addNumber( const string &name, const string &unit, 
				const string &format, int flags, double value )
{
  Parameter &p = Opt.addNumber( name, name, value, -MAXDOUBLE, MAXDOUBLE, 1.0,
				unit, unit, format, flags );
  init();
  return p;
}


Parameter &TableKey::addNumber( const string &name, const string &unit, 
				const string &format, double value, int flags )
{
  Parameter &p = Opt.addNumber( name, name, value, -MAXDOUBLE, MAXDOUBLE, 1.0,
				unit, unit, format, flags );
  init();
  return p;
}


Parameter &TableKey::insertNumber( const string &name, const string &atname, 
				   const string &unit, const string &format,
				   int flags )
{
  Parameter &p = Opt.insertNumber( name, atname, name, 0.0,
				   -MAXDOUBLE, MAXDOUBLE, 1.0,
				   unit, unit, format, flags );
  init();
  return p;
}


Parameter &TableKey::setNumber( const string &name, double number,
				const string &unit )
{
  return operator[]( name ).setNumber( number, unit );
}


Parameter &TableKey::setInteger( const string &name, long number,
				 const string &unit )
{
  return operator[]( name ).setInteger( number, unit );
}


Parameter &TableKey::addText( const string &name, const string &format,
			      int flags, const string &value )
{
  Parameter &p = Opt.addText( name, name, value, flags );
  p.setFormat( format );
  p.setUnit( "-" );
  init();
  return p;
}


Parameter &TableKey::addText( const string &name, const string &format,
			      const string &value, int flags )
{
  Parameter &p = Opt.addText( name, name, value, flags );
  p.setFormat( format );
  p.setUnit( "-" );
  init();
  return p;
}


Parameter &TableKey::addText( const string &name, int width, int flags,
			      const string &value )
{
  Parameter &p = Opt.addText( name, name, value, flags );
  p.setFormat( width );
  p.setUnit( "-" );
  init();
  return p;
}


Parameter &TableKey::addText( const string &name, int width, const string &value,
			      int flags )
{
  Parameter &p = Opt.addText( name, name, value, flags );
  p.setFormat( width );
  p.setUnit( "-" );
  init();
  return p;
}


Parameter &TableKey::insertText( const string &name, const string &atname, 
				 const string &format, int flags )
{
  Parameter &p = Opt.insertText( name, atname, name, "", flags );
  p.setFormat( format );
  p.setUnit( "-" );
  init();
  return p;
}


Parameter &TableKey::setText( const string &name, const string &text )
{
  return operator[]( name ).setText( text );
}


Parameter &TableKey::addLabel( const string &name, int flags )
{
  Parameter &p = Opt.addLabel( name, flags );
  init();
  return p;
}


Parameter &TableKey::insertLabel( const string &name, const string &atname,
				  int flags )
{
  Parameter &p = Opt.insertLabel( name, atname, flags );
  init();
  return p;
}


void TableKey::add( const Options &opts, int selectflag )
{
  Opt.append( opts, selectflag );
  init();
}


void TableKey::insert( const Options &opts, const string &atname )
{
  Opt.insert( opts, atname );
  init();
}


void TableKey::insert( const Options &opts, int selectflag, const string &atname )
{
  Opt.insert( opts, selectflag, atname );
  init();
}


void TableKey::erase( int column )
{
  if ( column >= 0 && column < (int)Columns.size() && ! Columns[column].empty() ) {
    vector< int > inx;
    inx.push_back( Columns[column][0] - Opt.begin() );
    for ( int l=1; l<(int)Columns[column].size(); l++ ) {
      if ( ( column+1 >= (int)Columns.size() || Columns[column][l] != Columns[column+1][l] ) &&
	   ( column <= 0 || Columns[column][l] != Columns[column-1][l] ))
	inx.push_back( Columns[column][l] - Opt.begin() );
    }
    for ( unsigned int k=0; k<inx.size(); k++ )
      Opt.erase( Opt.begin() + inx[k] );
    init();
  }
}


void TableKey::erase( const string &pattern )
{
  erase( column( pattern ) );
}


int TableKey::column( const string &pattern ) const
{
  // split pattern:
  StrQueue pq( pattern, ">" );
  while ( pq[0].empty() ) {
    pq.erase( 0 );
    if ( pq.empty() )
      return -1;
  }
  int p = 0;
  StrQueue sq( pq[p], "|" );
  int s = 0;

  // level index:
  int l = pq.size() - 1;
  if ( l >= level() )
    l = level() - 1;

  for ( unsigned int c=0; ; ) {
    if ( c >= Columns.size() ) {
      c = 0;
      s++;
      if ( s >= sq.size() ) {
	s = 0;
	l--;
	if ( l < 0 )
	  break;
      }
    }
    if ( c > 0 && Columns[c][l] == Columns[c-1][l] )
      c++;
    else if ( (*Columns[c][l]) == sq[s] ) {
      do {
	if ( l > 0 )
	  l--;
	p++;
	if ( p >= pq.size() )
	  return c;
      } while ( pq[p].empty() );
      sq.assign( pq[p], "|" );
      s = 0;
    }
    else
      c++;
  }

  return -1;
}


Str TableKey::name( int column ) const
{
  if ( column >= 0 && column < (int)Columns.size() && ! Columns[column].empty() )
    return (*Columns[column][0]).name();
  else
    return "";
}


Str TableKey::name( const string &pattern ) const
{
  return name( column( pattern ) );
}


Parameter &TableKey::setName( int column, const string &name )
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).setName( name );
  else
    return Dummy;
}


Parameter &TableKey::setName( const string &pattern, const string &name )
{
  return setName( column( pattern ), name );
}


Str TableKey::unit( int column ) const
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).unit();
  else
    return "";
}


Str TableKey::unit( const string &pattern ) const
{
  return unit( column( pattern ) );
}


Parameter &TableKey::setUnit( int column, const string &unit )
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).setUnit( unit );
  else
    return Dummy;
}


Parameter &TableKey::setUnit( const string &pattern, const string &unit )
{
  return setUnit( column( pattern ), unit );
}


Str TableKey::format( int column ) const
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).format();
  else
    return "";
}


Str TableKey::format( const string &pattern ) const
{
  return format( column( pattern ) );
}


int TableKey::formatWidth( int column ) const
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).formatWidth();
  else
    return 0;
}


int TableKey::formatWidth( const string &pattern ) const
{
  return formatWidth( column( pattern ) );
}


Parameter &TableKey::setFormat( int column, const string &format )
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).setFormat( format );
  else
    return Dummy;
}


Parameter &TableKey::setFormat( const string &name, const string &format )
{
  return setFormat( column( name ), format );
}


bool TableKey::isNumber( int column ) const
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).isAnyNumber();
  else
    return false;
}


bool TableKey::isNumber( const string &pattern ) const
{
  return isNumber( column( pattern ) );
}


bool TableKey::isText( int column ) const
{
  if ( column >= 0 && column < (int)Columns.size() &&
       ! Columns[column].empty() )
    return (*Columns[column][0]).isText();
  else
    return false;
}


bool TableKey::isText( const string &pattern ) const
{
  return isText( column( pattern ) );
}


Str TableKey::group( int column, int level ) const
{
  if ( column >= 0 && column < (int)Columns.size() &&
       (int)Columns[column].size() > level )
    return (*Columns[column][level]).name();
  else
    return "";
}


Str TableKey::group( const string &pattern, int level ) const
{
  return group( column( pattern ), level );
}


Parameter &TableKey::setGroup( int column, const string &name, int level )
{
  if ( column >= 0 && column < (int)Columns.size() &&
       (int)Columns[column].size() > level )
    return (*Columns[column][level]).setName( name );
  else
    return Dummy;
}


Parameter &TableKey::setGroup( const string &pattern, const string &name,
			       int level )
{
  return setGroup( column( pattern ), name, level );
}


Options TableKey::groupOptions( int column, int level ) const
{
  Options opts;
  if ( column >= 0 && column < (int)Columns.size() &&
       (int)Columns[column].size() > level ) {
    Options::const_iterator bp = Columns[column][level];
    while ( column < (int)Columns.size() && 
	    Columns[column][level] == bp )
      column++;
    Options::const_iterator ep = Opt.end();
    if ( column < (int)Columns.size() )
      ep = Columns[column][level];
    for ( Options::const_iterator pp = bp; pp != ep; ++pp )
      opts.add( *pp );
  }
  return opts;
}


Options TableKey::groupOptions( const string &pattern, int level ) const
{
  return groupOptions( column( pattern ), level );
}


const Parameter &TableKey::operator[]( int i ) const
{
  if ( i >= 0 && i < (int)Columns.size() )
    return *Columns[i][0];
  else
    return Dummy;
}


Parameter &TableKey::operator[]( int i )
{
  if ( i >= 0 && i < (int)Columns.size() )
    return *Columns[i][0];
  else
    return Dummy;
}


const Parameter &TableKey::operator[]( const string &pattern ) const
{
  return operator[]( column( pattern ) );
}


Parameter &TableKey::operator[]( const string &pattern )
{
  return operator[]( column( pattern ) );
}


int TableKey::columns( void ) const
{ 
  return Columns.size();
}


int TableKey::level( void ) const
{
  if ( Columns.size() > 0 )
    return Columns[0].size();
  else
    return 0;
}


bool TableKey::empty( void ) const
{
  return Columns.empty();
}


void TableKey::clear( void )
{
  Opt.clear();
  Columns.clear();
  Width.clear();
}


ostream &TableKey::saveKey( ostream &str, bool key, bool num,
			    bool units, int flags ) const
{
  if ( Columns[0].size() < 1 )
    return str;

  // key marker:
  if ( key )
    str << Str( KeyStart ).strip() << "Key" << '\n';

  // groups:
  for ( unsigned int l=Columns[0].size()-1; l>0; l-- ) {
    str << KeyStart;
    int w = Width[0];
    int n = 0;
    for ( unsigned int c=1; c<Columns.size(); c++ ) {
      if ( Columns[c][l] != Columns[c-1][l] ) {
	if ( flags == 0 || ( (*Columns[c-1][l]).flags() & flags ) ) {
	  if ( n > 0 )
	    str << Separator;
	  str << Str( (*Columns[c-1][l]).name(), -w );
	  w = Width[c];
	  n++;
	}
      }
      else if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) )
	w += Separator.size() + Width[c];
    }
    if ( flags == 0 || ( (*Columns.back()[l]).flags() & flags ) ) {
      if ( n > 0 )
	str << Separator;
      str << (*Columns.back()[l]).name();
      n++;
    }
    str << '\n';
  }

  // name:
  int n = 0;
  str << KeyStart;
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
      if ( n > 0 )
	str << Separator;
      str << Str( (*Columns[c][0]).name(), -Width[ c ] );
      n++;
    }
  }
  str << '\n';  

  // unit:
  bool unit = false;
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( ! (*Columns[c][0]).unit().empty() ) {
      unit = true;
      break;
    }
  }
  if ( units && unit ) {
    n = 0;
    str << KeyStart;
    for ( unsigned int c=0; c<Columns.size(); c++ ) {
      if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
	if ( n > 0 )
	  str << Separator;
	string us = (*Columns[c][0]).unit();
	if ( us.empty() )
	  us = "-";
	str << Str( us, -Width[ c ] );
	n++;
      }
    }
    str << '\n';  
  }

  // number:
  if ( num ) {
    n = 0;
    str << KeyStart;
    for ( unsigned int c=0; c<Columns.size(); c++ ) {
      if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
	if ( n > 0 )
	  str << Separator;
	str << Str( c+1, Width[ c ] );
	n++;
      }
    }
    str << '\n';
  }

  return str;  
}


ostream &TableKey::saveKeyLaTeX( ostream &str, bool num, bool units,
				 int flags ) const
{
  if ( Columns[0].size() < 1 )
    return str;

  // begin tabular:
  str << "\\begin{tabular}{";
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) )
      str << 'r';
  }
  str << "}\n";
  str << "  \\hline\n";

  // groups:
  for ( unsigned int l=Columns[0].size()-1; l>0; l-- ) {
    str << "  ";
    int w = 1;
    int n = 0;
    for ( unsigned int c=1; c<Columns.size(); c++ ) {
      if ( Columns[c][l] != Columns[c-1][l] ) {
	if ( flags == 0 || ( (*Columns[c-1][l]).flags() & flags ) ) {
	  if ( n > 0 )
	    str << " & ";
	  str << "\\multicolumn{" << w << "}{l}{" << (*Columns[c-1][l]).name().latex() << "}";
	  w = 1;
	  n++;
	}
      }
      else if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) )
	w ++;
    }
    if ( flags == 0 || ( (*Columns.back()[l]).flags() & flags ) ) {
      if ( n > 0 )
	str << " & ";
      str << "\\multicolumn{" << w << "}{l}{" << (*Columns.back()[l]).name().latex() << "}";
      n++;
    }
    str << " \\\\\n";
  }

  // name:
  int n = 0;
  str << "  ";
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
      if ( n > 0 )
	str << " & ";
      str << "\\multicolumn{1}{l}{" << (*Columns[c][0]).name().latex() << "}";
      n++;
    }
  }
  str << "\\\\\n";  

  // unit:
  bool unit = false;
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( ! (*Columns[c][0]).unit().empty() ) {
      unit = true;
      break;
    }
  }
  if ( units && unit ) {
    n = 0;
    str << "  ";
    for ( unsigned int c=0; c<Columns.size(); c++ ) {
      if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
	if ( n > 0 )
	  str << " & ";
	str << "\\multicolumn{1}{l}{" << (*Columns[c][0]).unit().latexUnit() << "}";
	n++;
      }
    }
    str << "\\\\\n";  
  }

  // number:
  if ( num ) {
    n = 0;
    str << "  ";
    for ( unsigned int c=0; c<Columns.size(); c++ ) {
      if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
	if ( n > 0 )
	  str << " & ";
	str << c+1;
	n++;
      }
    }
    str << "\\\\\n";
  }

  // end key:
  str << "  \\hline\n";

  return str;  
}


ostream &TableKey::saveKeyHTML( ostream &str, bool num, bool units,
				int flags ) const
{
  if ( Columns[0].size() < 1 )
    return str;

  // begin table:
  str << "      <table class=\"data\">\n";
  str << "        <thead class=\"datakey\">\n";

  // groups:
  for ( unsigned int l=Columns[0].size()-1; l>0; l-- ) {
    str << "          <tr class=\"group" << l << "\">\n";
    int w = 1;
    for ( unsigned int c=1; c<Columns.size(); c++ ) {
      if ( Columns[c][l] != Columns[c-1][l] ) {
	if ( flags == 0 || ( (*Columns[c-1][l]).flags() & flags ) ) {
	  str << "            <th colspan=\"" << w << "\" align=\"left\">"
	      << (*Columns[c-1][l]).name().html() << "</th>\n";
	  w = 1;
	}
      }
      else if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) )
	w ++;
    }
    if ( flags == 0 || ( (*Columns.back()[l]).flags() & flags ) ) {
      str << "            <th colspan=\"" << w << "\" align=\"left\">"
	  << (*Columns.back()[l]).name().html() << "</th>\n";
    }
    str << "          </tr>\n";
  }

  // name:
  str << "          <tr class=\"datanames\">\n";
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
      str << "            <th align=\"left\">"
	  << (*Columns[c][0]).name().html() << "</th>\n";
    }
  }
  str << "          </tr>\n";

  // unit:
  bool unit = false;
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( ! (*Columns[c][0]).unit().empty() ) {
      unit = true;
      break;
    }
  }
  if ( units && unit ) {
    str << "          <tr class=\"dataunits\">\n";
    for ( unsigned int c=0; c<Columns.size(); c++ ) {
      if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
	str << "            <th align=\"left\">"
	    << (*Columns[c][0]).unit().htmlUnit() << "</th>\n";
      }
    }
    str << "          </tr>\n";
  }

  // number:
  if ( num ) {
    str << "          <tr class=\"datanums\">\n";
    for ( unsigned int c=0; c<Columns.size(); c++ ) {
      if ( flags == 0 || ( (*Columns[c][0]).flags() & flags ) ) {
	str << "            <th align=\"right\">"
	    << c+1 << "</th>\n";
      }
    }
    str << "          </tr>\n";
  }

  // end key:
  str << "        </thead>\n";

  return str;  
}


ostream &operator<< ( ostream &str, const TableKey &tk )
{
  tk.saveKey( str );
  return str;
}


TableKey &TableKey::loadKey( const StrQueue &sq )
{
  clear();

  StrQueue::const_iterator sp = sq.begin();

  string key = "Key";
  string comment = Str( KeyStart ).strip();

  // skip empty lines and key nameifier:
  while ( sp != sq.end() ) {
    int p = (*sp).first();
    int c = (*sp).find( comment );
    // line introduced by comment:
    if ( p >= 0 && p == c ) {
      // line behind comment not empty:
      if ( ! (*sp).substr( c + comment.size() ).stripComment( comment ).empty() ) {
	if ( (*sp).substr( p + comment.size(), key.size() ) == key )
	  ++sp;
	break;
      }
    }
    ++sp;
  }

  // memorize first line:
  StrQueue::const_iterator fp = sp;

  // last line:
  StrQueue::const_iterator lp = sp+1;
  while ( lp != sq.end() ) {
    int p = (*lp).first();
    int c = (*lp).find( comment );
    // line introduced by comment:
    if ( p >= 0 && p == c ) {
      // line behind comment empty:
      if ( (*lp).substr( c + comment.size() ).stripComment( comment ).empty() ) {
	break;
      }
    }
    else
      break;
    ++lp;
  }

  // get positions of labels in each line:
  vector< vector < int > > pos;
  pos.reserve( 10 );
  while ( sp != lp ) {
    pos.push_back( vector< int >( 0 ) );
    pos.back().reserve( 100 );
    int p = (*sp).first() + comment.size();
    for ( ; ; ) {
      int c = (*sp).nextWord( p, Str::DoubleWhiteSpace, comment );
      if ( c >= 0 )
	pos.back().push_back( c );
      else {
	pos.back().push_back( (*sp).size() );
	/*
	// this makes problems, if a unit is missing!
	// and anyways, we do not expect weired things in the key.
	if ( pos.size() > 1 && pos[pos.size()-1].size() < pos[pos.size()-2].size() ) {
	  pos.pop_back();
	  lp = sp;
	  --sp;
	}
	*/
	break;
      }
    }
    ++sp;
  }

  // number-line:
  vector< int > numpos;
  bool num = true;
  sp = lp-1;

  for ( unsigned int k=0; k<pos.back().size()-1; k++ ) {
    if ( (*sp).number( 0.0, pos.back()[k] ) != double( k+1 ) ) {
      num = false;
      break;
    }
  }
  if ( num ) {
    numpos = pos.back();
    pos.pop_back();
  }

  // number of column lines:
  int cn = 1;
  for ( int k = pos.size()-2; k >= 0; k--, cn++ ) {
    // check whether column positions are nameical:
    unsigned int n = pos[k].size();
    if ( n > pos.back().size() )
      n = pos.back().size();
    int differ = abs( (int)pos.back().size() - (int)pos[k].size() );
    for ( unsigned int j=0; j<n-1; j++ ) {
      if ( pos[k][j] != pos.back()[j] ) {
	differ++;
      }
    }
    int maxdiffer = n / 5;
    if ( maxdiffer < 1 )
      maxdiffer = 1;
    if ( differ > maxdiffer ) // allow some columns without unit
      break;
  }
  // nameify table lines:
  int level = pos.size() - cn;
  bool units = ( cn > 1 );

  // read in table header:
  vector< int > inx( level, 0 );
  for ( unsigned int k=0; k<pos[level].size()-1; k++ ) {
    for ( int j=0; j<level; j++ ) {
      if ( inx[j] < int(pos[j].size())-1 && pos[j][inx[j]] == pos[level][k] ) {
	int index = pos[j][inx[j]];
	string name = (*(fp+j)).wordAt( index, Str::DoubleWhiteSpace, comment );
	addLabel( name );
	inx[j]++;
      }
    }
    int index = pos[level][k];
    string name = (*(fp+level)).wordAt( index, Str::DoubleWhiteSpace, comment );
    index = units ? pos[level+1][k] : 0;
    string unit = units ? (*(fp+level+1)).wordAt( index, Str::DoubleWhiteSpace, comment ) : "";
    int width = pos[level][k+1] - pos[level][k] - 2;  // this '2' is separator dependent!
    if ( num ) {
      int w = numpos[k] + 1 + (int)floor(log10(double(k+1))) - pos[level][k];
      if ( w > width )
	width = w;
    }
    addNumber( name, unit, "%" + Str( width ) + "g" );
  }

  return *this;
}


ostream &TableKey::saveData( ostream &str )
{
  str << DataStart;
  for ( unsigned int c=0; c<Columns.size(); c++ ) {
    if ( c > 0 )
      str << Separator;
    Str s = (*Columns[c][0]).text();
    if ( s.empty() )
      s = Missing;
    if ( s.size() >= Width[c] )
      str << s;
    else {
      if ( (*Columns[c][0]).isText() )
	str << Str( s, -Width[c] );
      else
	str << Str( s, Width[c] );
    }
  }
  str << '\n';
  return str;
}


ostream &TableKey::saveData( ostream &str, int from, int to )
{
  for ( int c=from; c<=to && c<(int)Columns.size(); c++ ) {
    if ( c > 0 )
      str << Separator;
    else 
      str << DataStart;
    Str s = (*Columns[c][0]).text();
    if ( s.empty() )
      s = Missing;
    if ( s.size() >= Width[c] )
      str << s;
    else {
      if ( (*Columns[c][0]).isText() )
	str << Str( s, -Width[c] );
      else
	str << Str( s, Width[c] );
    }
  }
  return str;
}


ostream &TableKey::saveMetaData( ostream &str, const string &start ) const
{
  // XXX a separate algorithmen with level dependent indent would be better!
  // and different format: name unit
  // maybe this should be an additional function!
  return Opt.save( str, start );
}


ostream &TableKey::save( ostream &str, double v, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 || c >= (int)Columns.size() )
    return str;

  PrevCol = c;

  if ( c > 0 )
    str << Separator;
  else
    str << DataStart;
  Str s( v, format( c ) );
  if ( s.size() >= Width[c] )
    str << s;
  else
    str << Str( s, Width[c] );
  return str;  
}


ostream &TableKey::save( ostream &str, const TableData &table,
			 int r, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 )
    return str;

  for ( int k=0; k<table.columns(); k++ ) {
    if ( c >= (int)Columns.size() )
      return str;
    if ( c > 0 )
      str << Separator;
    else
      str << DataStart;
    Str s( r < (int)table.rows() ? table( k, r ) : 0.0, format( c ) );
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
    PrevCol = c;
    c++;
  }
  return str;  
}


ostream &TableKey::save( ostream &str, const TableData &table,
			 int r, int cbegin, int cend, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 )
    return str;

  if ( cbegin < 0 )
    cbegin = 0;
  if ( cend >= table.columns() || cend < 0 )
    cend = table.columns();

  for ( int k=cbegin; k<cend; k++ ) {
    if ( c >= (int)Columns.size() )
      return str;
    if ( c > 0 )
      str << Separator;
    else
      str << DataStart;
    Str s( r < (int)table.rows() ? table( k, r ) : 0.0, format( c ) );
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
    PrevCol = c;
    c++;
  }
  return str;  
}


ostream &TableKey::save( ostream &str, const TableData &table ) const
{
  for ( int r=0; r<table.rows(); r++ ) {
    for ( int c=0; c<table.columns() && c<columns(); c++ ) {
      if ( c > 0 )
	str << Separator;
      else
	str << DataStart;
      Str s( table( c, r ), format( c ) );
      if ( s.size() >= Width[c] )
	str << s;
      else
	str << Str( s, Width[c] );
    }
    str << '\n';
  }
  PrevCol = -1;
  return str;  
}


ostream &TableKey::save( ostream &str, const string &text, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 || c >= (int)Columns.size() )
    return str;

  PrevCol = c;

  if ( c > 0 )
    str << Separator;
  else
    str << DataStart;
  if ( (int)text.size() > Width[c] && ! text.empty() ) {
    str << text;
  }
  else {
    Str s;
    if ( text.empty() )
      s = Str( Missing, -formatWidth( c ) );
    else {
      if ( isText( c ) )
	s = Str( text, format( c ) );
      else
	s = Str( text, -formatWidth( c ) );
    }
    if ( s.size() >= Width[c] )
      str << s;
    else
      str << Str( s, Width[c] );
  }
  return str;  
}


ostream &TableKey::save( ostream &str, const Parameter &param, int c ) const
{
  if ( c < 0 ) 
    c = PrevCol + 1;

  if ( c < 0 || c >= (int)Columns.size() )
    return str;

  PrevCol = c;

  if ( c > 0 )
    str << Separator;
  else
    str << DataStart;
  Str s = "";
  if ( param.isText() && param.text().empty() )
    s = Str( Missing, -formatWidth( c ) );
  else
    s = param.text( format( c ) );
  if ( s.size() >= Width[c] )
    str << s;
  else
    str << Str( s, Width[c] );
  return str;  
}


void TableKey::setSaveColumn( int col )
{
  if ( col < -1 )
    col = -1;
  PrevCol = col;
}


void TableKey::init( void )
{
  // group levels:
  vector< Options::iterator > groups( 0 );
  Options::iterator pp = Opt.begin();
  for ( ; pp != Opt.end(); ++pp ) {
    groups.insert( groups.begin(), pp );
    if ( ! (*pp).isLabel() )
      break;
  }

  if ( groups.size() == 0 )
    cerr << "! fatal error in TableKey::init() -> groups.size() == 0 !\n";

  // columns:
  Columns.clear();
  unsigned int l = 0;
  for ( ; pp != Opt.end(); ++pp )
    if ( (*pp).isLabel() ) {
      l++;
      if ( l >= groups.size() )
	cerr << "! fatal error in TableKey::init() -> l >= groups.size() !\n";
      groups.erase( groups.begin() + l );
      groups.insert( groups.begin() + 1, pp );
    }
    else {
      groups.front() = pp;
      Columns.push_back( groups );
      l = 0;
    }

  // width of each column:
  Width.resize( Columns.size() );
  for ( unsigned int c=0; c<Width.size(); c++ ) {
    int fw = (*Columns[c][0]).formatWidth();
    int iw = (*Columns[c][0]).name().size();
    int uw = (*Columns[c][0]).unit().size();
    int w = fw > iw ? fw : iw;
    Width[c] = uw > w ? uw : w;
  }

}


string TableKey::comment( void ) const
{
  return Comment;
}


void TableKey::setComment( const string &comment )
{
  Comment = comment; KeyStart = comment + " ";
}


string TableKey::keyStart( void ) const
{
  return KeyStart;
}


void TableKey::setKeyStart( const string &start )
{
  KeyStart = start;
}


string TableKey::dataStart( void ) const
{
  return DataStart;
}


void TableKey::setDataStart( const string &start )
{
  DataStart = start;
}


string TableKey::separator( void ) const
{
  return Separator;
}


void TableKey::setSeparator( const string &separator )
{
  Separator = separator;
}


string TableKey::missing( void ) const
{
  return Missing;
}


void TableKey::setMissing( const string &missing )
{
  Missing = missing;
}


}; /* namespace relacs */

