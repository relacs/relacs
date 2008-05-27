/*
  configureclasses.cc
  Coordinates configureable classes Config..Class.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
    Configs()
{
  ConfigClass::setConfigClassList( &Configs );
  ConfigClass::setConfigureClasses( this );
}


ConfigureClasses::ConfigureClasses( int groups )
  : ConfigFile(),
    Configs()
{
  ConfigFile.resize( groups );
  ConfigClass::setConfigClassList( &Configs );
  ConfigClass::setConfigureClasses( this );
}


ConfigureClasses::ConfigureClasses( const string &file )
  : ConfigFile(),
    Configs()
{
  ConfigFile.resize( 1 );
  setConfigFile( file );
  ConfigClass::setConfigClassList( &Configs );
  ConfigClass::setConfigureClasses( this );
}


ConfigureClasses::~ConfigureClasses( void )
{
  Configs.clear();
  ConfigClass::setConfigClassList( 0 );
  ConfigClass::setConfigureClasses( 0 );
}


int ConfigureClasses::groups( void ) const
{
  return ConfigFile.size();
}


void ConfigureClasses::addGroup( const string &file )
{
  ConfigFile.push_back( vector< string >() );
  addConfigFile( file, ConfigFile.size()-1 );  
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
       level < 0 || level >= (int)ConfigFile[group].size() )
    return;
  else if ( ! file.empty() )
    ConfigFile[group][level] = file;
}


void ConfigureClasses::addConfigFile( const string &file, int group )
{
  if ( group < 0 || group >= (int)ConfigFile.size() ||
       file.empty() )
    return;
  else
    ConfigFile[group].push_back( file );
}


void ConfigureClasses::read( int group, int level )
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
    for ( ConfigClassList::iterator cp = Configs.begin(); cp != Configs.end(); ++cp ) {
      if ( (*cp)->configGroup() == group &&
	   (*cp)->configIdent() == ident ) {
	(*cp)->readConfig( sq );
      }
    }
  }

  sf.close();
}


void ConfigureClasses::read( int group )
{
  for ( unsigned int l = 0; l < ConfigFile[group].size(); l++ ) {
    read( group, l );
  }
}


void ConfigureClasses::read( void )
{
  for ( unsigned int g = 0; g < ConfigFile.size(); g++ ) {
    read( g );
  }
}


void ConfigureClasses::read( int group, int level, ConfigClass &config )
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


void ConfigureClasses::read( int group, ConfigClass &config )
{
  for ( unsigned int l = 0; l < ConfigFile[group].size(); l++ ) {
    read( group, l, config );
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
       << " save configuration in " << file << endl;
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


ostream &operator<<( ostream &str, const ConfigureClasses &c )
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


string ConfigureClasses::currentTime( void )
{
  char s[200];
  time_t ct = time( NULL );
  strftime( s, 200, "%H:%M:%S", localtime( &ct ) );
  return s;
}


}; /* namespace relacs */

