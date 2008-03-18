/*
  configure.h
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

#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_

#include <string>
#include <vector>
#include <relacs/configclass.h>
using namespace std;


/*!
\class Configure
\author Jan Benda
\version 1.0
\brief Coordinates configureable classes derived from ConfigClass.
*/

class Configure
{

public:

  Configure( void );
  Configure( int groups );
  Configure( const string &file );
  ~Configure( void );

  int groups( void ) const;
  void addGroup( const string &file="" );

  string configFile( int group=0, int level=0 ) const;
  void setConfigFile( const string &file, int group=0, int level=0 );
  void addConfigFile( const string &file, int group=0 );

  void read( int group, int level );
  void read( int group );
  void read( void );

  void read( int group, int level, ConfigClass &config );
  void read( int group, ConfigClass &config );

  void configure( int group, const string &ident );
  void configure( int group );
  void configure( void );

  void save( int group, const string &file );
  void save( int group, int level );
  void save( int group );
  void save( void );

  friend ostream &operator<<( ostream &str, const Configure &c );


private:

  string currentTime( void );
  vector < vector < string > > ConfigFile;
  ConfigList Configs;

};


#endif
