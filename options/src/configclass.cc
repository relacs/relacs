/*
  configclass.cc
  Base class for each class that has some parameters to be configured.

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

#include <relacs/configureclasses.h>
#include <relacs/configclass.h>

namespace relacs {


ConfigureClasses *ConfigClass::CFG = 0;


ConfigClass::ConfigClass( const string &ident, int group,
			  int mode, int selectmask )
  : Options(),
    ConfigIdent( ident ),
    ConfigGroup( group ),
    ConfigMode( mode ),
    ConfigSelect( selectmask )
{
  addConfig();
}


ConfigClass::ConfigClass( const ConfigClass &C )
  : Options( C ),
    ConfigIdent( C.ConfigIdent ),
    ConfigGroup( C.ConfigGroup ),
    ConfigMode( C.ConfigMode ),
    ConfigSelect( C.ConfigSelect )
{
  cerr << "! warning: copied class ConfigClass. What shall we do?" << endl;
}


ConfigClass::~ConfigClass( void )
{
  if ( CFG != 0 )
    CFG->eraseConfigClass( this );
}


const string &ConfigClass::configIdent( void ) const
{
  return ConfigIdent;
}


void ConfigClass::setConfigIdent( const string &ident )
{
  ConfigIdent = ident;
}


int ConfigClass::configGroup( void ) const
{
  return ConfigGroup;
}


void ConfigClass::setConfigGroup( int group )
{
  ConfigGroup = group;
}


int ConfigClass::configMode( void ) const
{
  return ConfigMode;
}


void ConfigClass::setConfigMode( int mode )
{
  ConfigMode = mode;
}


void ConfigClass::addConfigMode( int mode )
{
  ConfigMode |= mode;
}


void ConfigClass::delConfigMode( int mode )
{
  ConfigMode &= ~mode;
}


int ConfigClass::configSelectMask( void ) const
{
  return ConfigSelect;
}


void ConfigClass::setConfigSelectMask( int flag )
{
  ConfigSelect = flag;
}


void ConfigClass::addConfig( void )
{
  if ( CFG != 0 )
    CFG->addConfigClass( this );
}


void ConfigClass::readConfig( void )
{
  if ( CFG != 0 ) {
    CFG->read( *this );
  }
}


void ConfigClass::setConfigureClasses( ConfigureClasses *cfg )
{
  CFG = cfg;
}


void ConfigClass::readConfig( StrQueue &sq )
{
  Options::read( sq, 0 );
}


void ConfigClass::config( void )
{
}


int ConfigClass::configSize( void ) const
{
  return Options::size( ConfigSelect );
}


void ConfigClass::saveConfig( ofstream &str )
{
  Options::save( str, "  ", ConfigSelect, CFG->Detailed, CFG->FirstOnly );
}


}; /* namespace relacs */

