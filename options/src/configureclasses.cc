/*
  configureclasses.cc
  Coordinates configureable classes Config..Class.

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

#include <ctime>
#include <fstream>
#include <relacs/strqueue.h>
#include <relacs/configureclasses.h>
using namespace std;

namespace relacs {


ConfigureClasses::ConfigureClasses( void )
  : ConfigFile(),
    Configs(),
    Flags( Options::NoType )
{
  ConfigFile.resize( 1 );
  ConfigClass::setConfigureClasses( this );
}


ConfigureClasses::ConfigureClasses( int groups )
  : ConfigFile(),
    Configs(),
    Flags( Options::NoType )
{
  ConfigFile.resize( groups );
  ConfigClass::setConfigureClasses( this );
}


ConfigureClasses::ConfigureClasses( const string &file )
  : ConfigFile(),
    Configs(),
    Flags( Options::NoType )
{
  ConfigFile.resize( 1 );
  addConfigFile( file );
  ConfigClass::setConfigureClasses( this );
}


ConfigureClasses::~ConfigureClasses( void )
{
  ConfigClass::setConfigureClasses( 0 );
  Configs.clear();
}


int ConfigureClasses::groups( void ) const
{
  return ConfigFile.size();
}


void ConfigureClasses::addGroup( const string &files )
{
  ConfigFile.push_back( vector< string >() );
  setConfigFiles( files, ConfigFile.size()-1 );  
}


void ConfigureClasses::clearGroups( void )
{
  clearConfigFiles();
  ConfigFile.clear();
}


string ConfigureClasses::configFile( int group, int level ) const
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return "";
  else
    return ConfigFile[group][level];
}


void ConfigureClasses::setConfigFile( const string &file, int group, int level )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() ||
       file.empty() )
    return;

  ConfigFile[group][level] = file;
}


void ConfigureClasses::setConfigFiles( const string &files, int group )
{
  if ( group < 0 || group >= (int)ConfigFile.size() )
    return;
  else {
    ConfigFile[group].clear();
    StrQueue sq( files, "|" );
    sq.strip();
    for ( int k=0; k<sq.size(); k++ )
      ConfigFile[group].push_back( sq[k] );
  }
}


void ConfigureClasses::addConfigFile( const string &file, int group )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       file.empty() )
    return;

  ConfigFile[group].push_back( file );
}


void ConfigureClasses::addConfigFiles( const string &files, int group )
{
  if ( group < 0 || group >= (int)ConfigFile.size() )
    return;
  else {
    StrQueue sq( files, "|" );
    sq.strip();
    for ( int k=0; k<sq.size(); k++ )
      ConfigFile[group].push_back( sq[k] );
  }
}


void ConfigureClasses::clearConfigFiles( int group )
{
  if ( group < 0 || group >= (int)ConfigFile.size() )
    return;
  else
    ConfigFile[group].clear();
}


void ConfigureClasses::clearConfigFiles( void )
{
  for ( unsigned int g = 0; g < ConfigFile.size(); g++ ) {
    clearConfigFiles( g );
  }
}


int ConfigureClasses::read( int group, const string &file )
{
  // open the requested configuration file:
  ifstream sf( file.c_str() );
  if ( ! sf.good() ) {
    cerr << currentTime()
	 << " failed to open configuration file " << file << '\n';
    return 0;
  }

  // skip any initial lines until the first line starting with '*':
  Str line = "";
  while ( ( line.empty() || line[0] != '*' ) &&
	  getline( sf, line ).good() );
  if ( ! sf.good() ) {
    cerr << currentTime()
	 << " cannot read configuration from " << file << '\n';
    return 0;
  }

  cerr << currentTime()
       << " read configuration from " << file << '\n';

  // read in configuration sections:
  while ( sf.good() ) {
    string ident = line.strip().substr( 1 );
    // read in section:
    StrQueue sq;
    line = "";
    sq.load( sf, "*", &line );
    sq.strip();
    // pass this section to the corresponding ConfigClass instance:
    for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
      if ( (*cp)->configGroup() == group &&
	   (*cp)->configIdent() == ident ) {
	(*cp)->readConfig( sq );
	break;
      }
    }
  }

  sf.close();

  return 1;
}


int ConfigureClasses::read( int group, int level )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return 0;

  return read( group, ConfigFile[group][level].c_str() );
}


int ConfigureClasses::read( int group )
{
  int r = 0;
  for ( unsigned int l = 0; l < ConfigFile[group].size(); l++ ) {
    int rr = read( group, l );
    r += rr;
  }
  return r;
}


int ConfigureClasses::read( void )
{
  int r = 0;
  for ( unsigned int g = 0; g < ConfigFile.size(); g++ ) {
    int rr = read( g );
    r += rr;
  }
  return r;
}


int ConfigureClasses::read( int level, ConfigClass &config )
{
  int group = config.configGroup();

  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return 0;

  // open the requested configuration file:
  ifstream sf( ConfigFile[group][level].c_str() );
  if ( ! sf.good() ) {
    cerr << currentTime()
	 << " failed to open configuration file " << ConfigFile[group][level] << '\n';
    return 0;
  }

  // skip any initial lines until the first line starting with '*':
  Str line = "";
  while ( ( line.empty() || line[0] != '*' ) &&
	  getline( sf, line ).good() );
  if ( ! sf.good() ) {
    cerr << currentTime()
	 << " cannot read configuration from " << ConfigFile[group][level] << '\n';
    return 0;
  }

  cerr << currentTime()
       << " read configuration from " << ConfigFile[group][level] << '\n';

  // read in configuration sections:
  while ( sf.good() ) {
    string ident = line.strip().substr( 1 );
    // read in section:
    StrQueue sq;
    line = "";
    sq.load( sf, "*", &line );
    // pass this section to config:
    if ( config.configIdent() == ident ) {
      sq.strip();
      config.readConfig( sq );
      break;
    }
  }

  sf.close();

  return 1;
}


int ConfigureClasses::read( ConfigClass &config )
{
  int r = 0;
  for ( unsigned int l = 0;
	l < ConfigFile[config.configGroup()].size();
	l++ ) {
    int rr = read( l, config );
    r += rr;
  }
  return r;
}


void ConfigureClasses::preConfigure( int group, const string &ident )
{
  for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    if ( (*cp)->configGroup() == group &&
	 (*cp)->configIdent() == ident ) {
      (*cp)->preConfig();
    }
  }
}


void ConfigureClasses::preConfigure( int group )
{
  for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    if ( (*cp)->configGroup() == group ) {
      (*cp)->preConfig();
    }
  }
}


void ConfigureClasses::preConfigure( void )
{
  for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    (*cp)->preConfig();
  }
}


void ConfigureClasses::configure( int group, const string &ident )
{
  for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    if ( (*cp)->configGroup() == group &&
	 (*cp)->configIdent() == ident ) {
      (*cp)->config();
    }
  }
}


void ConfigureClasses::configure( int group )
{
  for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    if ( (*cp)->configGroup() == group ) {
      (*cp)->config();
    }
  }
}


void ConfigureClasses::configure( void )
{
  for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    (*cp)->config();
  }
}


void ConfigureClasses::save( int group, const string &file )
{
  cerr << currentTime()
       << " save configuration in " << file << '\n';
  ofstream df( file.c_str() );
  for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    if ( (*cp)->configGroup() == group &&
	 ( (*cp)->configMode() & ConfigClass::Save ) &&
	 (*cp)->configSize() > 0 ) { 
      df << '*' << (*cp)->configIdent() << '\n';
      (*cp)->saveConfig( df );
      df << '\n';
    }
  }
  df.close();
}


void ConfigureClasses::save( int group, int level )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return;

  save( group, ConfigFile[group][level]);
}


void ConfigureClasses::save( int group )
{
  save( group, ConfigFile[group].size()-1 );
}


void ConfigureClasses::save( void )
{
  for ( unsigned int g = 0; g < ConfigFile.size(); g++ ) {
    save( g );
  }
}


void ConfigureClasses::addConfigClass( ConfigClass *cfg )
{
  Configs.push_back( cfg );
}


void ConfigureClasses::eraseConfigClass( ConfigClass *cfg )
{
  for ( ConfigClassList::iterator cp = Configs.begin();
	cp != Configs.end();
	++cp ) {
    if ( (*cp) == cfg ) {
      Configs.erase( cp );
      break;
    }
  }
}


void ConfigureClasses::setSaveStyle( int flags )
{
  Flags = flags;
}


ostream &operator<<( ostream &str, const ConfigureClasses &c )
{
  for ( unsigned int g = 0; g < c.ConfigFile.size(); g++ ) {
    str << "group " << g << ": \n";
    for ( unsigned int l = 0; l < c.ConfigFile[g].size(); l++ ) {
      str << "  '" << c.ConfigFile[g][l] << "'\n";
    }
  }  
  return str;
}


string ConfigureClasses::currentTime( void )
{
  char s[200];
  time_t ct = time( NULL );
  strftime( s, 200, "%H:%M:%S", localtime( &ct ) );
  return s;
}


}; /* namespace relacs */

