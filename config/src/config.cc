/*
  config.cc
  Base class for each class that has some parameters to be configured.

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

#include <relacs/configure.h>
#include <relacs/config.h>


ConfigList *Config::Configs = 0;
Configure *Config::CFG = 0;


Config::Config( const string &ident, int group, int mode, int selectmask )
  : Options(),
    ConfigIdent( ident ),
    ConfigGroup( group ),
    ConfigMode( mode ),
    ConfigSelect( selectmask )
{
  addConfig();
}


Config::Config( const Config &C )
  : Options( C ),
    ConfigIdent( C.ConfigIdent ),
    ConfigGroup( C.ConfigGroup ),
    ConfigMode( C.ConfigMode ),
    ConfigSelect( C.ConfigSelect )
{
  cerr << "! warning: copied class Config. What shall we do?" << endl;
}


Config::~Config( void )
{
  if ( Configs != 0 ) {
    for ( ConfigList::iterator cp = Configs->begin();
	  cp != Configs->end();
	  ++cp ) {
      if ( (*cp)->configIdent() == configIdent() ) {
	Configs->erase( cp );
	break;
      }
    }
  }
}


const string &Config::configIdent( void ) const
{
  return ConfigIdent;
}


void Config::setConfigIdent( const string &ident )
{
  ConfigIdent = ident;
}


int Config::configGroup( void ) const
{
  return ConfigGroup;
}


void Config::setConfigGroup( int group )
{
  ConfigGroup = group;
}


int Config::configMode( void ) const
{
  return ConfigMode;
}


void Config::setConfigMode( int mode )
{
  ConfigMode = mode;
}


int Config::configSelectMask( void ) const
{
  return ConfigSelect;
}


void Config::setConfigSelectMask( int flag )
{
  ConfigSelect = flag;
}


void Config::addConfig( void )
{
  if ( Configs != 0 ) {
    Configs->push_back( this );
  }
}


void Config::readConfig( void )
{
  if ( CFG != 0 ) {
    CFG->read( configGroup(), *this );
  }
}


void Config::setConfigList( ConfigList *cl )
{
  Configs = cl;
}


void Config::setConfigure( Configure *cfg )
{
  CFG = cfg;
}


void Config::readConfig( StrQueue &sq )
{
  Options::read( sq, 0, ":" );
}


void Config::config( void )
{
}


int Config::configSize( void ) const
{
  return Options::size( ConfigSelect );
}


void Config::saveConfig( ofstream &str )
{
  Options::save( str, "  ", -1, ConfigSelect );
}

