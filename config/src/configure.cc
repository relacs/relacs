/*
  configure.cc
  Coordinates configureable classes.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/configure.h>
using namespace std;


Configure::Configure( void )
  : ConfigFile(),
    Configs()
{
  ConfigClass::setConfigList( &Configs );
  ConfigClass::setConfigure( this );
}


Configure::Configure( int groups )
  : ConfigFile(),
    Configs()
{
  ConfigFile.resize( groups );
  ConfigClass::setConfigList( &Configs );
  ConfigClass::setConfigure( this );
}


Configure::Configure( const string &file )
  : ConfigFile(),
    Configs()
{
  ConfigFile.resize( 1 );
  setConfigFile( file );
  ConfigClass::setConfigList( &Configs );
  ConfigClass::setConfigure( this );
}


Configure::~Configure( void )
{
  Configs.clear();
  ConfigClass::setConfigList( 0 );
  ConfigClass::setConfigure( this );
}


int Configure::groups( void ) const
{
  return ConfigFile.size();
}


void Configure::addGroup( const string &file )
{
  ConfigFile.push_back( vector< string >() );
  addConfigFile( file, ConfigFile.size()-1 );  
}


string Configure::configFile( int group, int level ) const
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return "";
  else
    return ConfigFile[group][level];
}


void Configure::setConfigFile( const string &file, int group, int level )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return;
  else if ( ! file.empty() )
    ConfigFile[group][level] = file;
}


void Configure::addConfigFile( const string &file, int group )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       file.empty() )
    return;
  else
    ConfigFile[group].push_back( file );
}


void Configure::read( int group, int level )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return;

  ifstream sf( ConfigFile[group][level].c_str() );

  if ( ! sf.good() ) {
    cerr << currentTime()
	 << " failed to open configuration file " << ConfigFile[group][level] << endl;
    return;
  }

  Str line = "";
  while ( ( line.empty() || line[0] != '*' ) &&
	  getline( sf, line ).good() );

  if ( ! sf.good() ) {
    cerr << currentTime()
	 << " cannot read configuration from " << ConfigFile[group][level] << endl;
    return;
  }

  cerr << currentTime()
       << " read configuration from " << ConfigFile[group][level] << endl;

  while ( sf.good() ) {
    string ident = line.strip().substr( 1 );
    StrQueue sq;
    line = "";
    sq.load( sf, "*", &line );
    sq.strip();
    for ( ConfigList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
      if ( (*cp)->configGroup() == group &&
	   (*cp)->configIdent() == ident ) {
	(*cp)->readConfig( sq );
      }
    }
  }

  sf.close();
}


void Configure::read( int group )
{
  for ( unsigned int l = 0; l < ConfigFile[group].size(); l++ ) {
    read( group, l );
  }
}


void Configure::read( void )
{
  for ( unsigned int g = 0; g < ConfigFile.size(); g++ ) {
    read( g );
  }
}


void Configure::read( int group, int level, ConfigClass &config )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return;

  ifstream sf( ConfigFile[group][level].c_str() );
  Str line;

  while ( ( line.empty() || line[0] != '*' ) &&
	  getline( sf, line ).good() );

  while ( sf.good() ) {
    string is = line.strip().substr( 1 );
    StrQueue sq;
    line = "";
    sq.load( sf, "*", &line );
    if ( config.configGroup() == group &&
	 config.configIdent() == is ) {
      sq.strip();
      config.readConfig( sq );
    }
  }

  sf.close();
}


void Configure::read( int group, ConfigClass &config )
{
  for ( unsigned int l = 0; l < ConfigFile[group].size(); l++ ) {
    read( group, l, config );
  }
}


void Configure::configure( int group, const string &ident )
{
  for ( ConfigList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    if ( (*cp)->configGroup() == group &&
	 (*cp)->configIdent() == ident ) {
      (*cp)->config();
    }
  }
}


void Configure::configure( int group )
{
  for ( ConfigList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    if ( (*cp)->configGroup() == group ) {
      (*cp)->config();
    }
  }
}


void Configure::configure( void )
{
  for ( ConfigList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
    (*cp)->config();
  }
}


void Configure::save( int group, const string &file )
{
  cerr << currentTime()
       << " save configuration in " << file << endl;
  ofstream df( file.c_str() );
  for ( ConfigList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
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


void Configure::save( int group, int level )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       level < 0 || level >= (int)ConfigFile[group].size() )
    return;

  save( group, ConfigFile[group][level]);
}


void Configure::save( int group )
{
  save( group, ConfigFile[group].size()-1 );
}


void Configure::save( void )
{
  for ( unsigned int g = 0; g < ConfigFile.size(); g++ ) {
    save( g );
  }
}


ostream &operator<<( ostream &str, const Configure &c )
{
  for ( unsigned int g = 0; g < c.ConfigFile.size(); g++ ) {
    for ( unsigned int l = 0; l < c.ConfigFile[g].size(); l++ ) {
      if ( l > 0 )
	str << ", ";
      str << c.ConfigFile[g][l];
    }
    str << '\n';
  }  
  return str;
}


string Configure::currentTime( void )
{
  char s[200];
  time_t ct = time( NULL );
  strftime( s, 200, "%H:%M:%S", localtime( &ct ) );
  return s;
}
