/*
  configureclasses.h
  Coordinates configureable classes ConfigClass.

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

#ifndef _RELACS_CONFIGURECLASSES_H_
#define _RELACS_CONFIGURECLASSES_H_ 1

#include <string>
#include <vector>
#include <relacs/configclass.h>
using namespace std;

namespace relacs {


/*!
\class ConfigureClasses
\brief Coordinates configureable classes ConfigClass.
\author Jan Benda
\version 1.0
*/

class ConfigureClasses
{

public:

    /*! Constructs an empty ConfigureClasses (no configuration files)
        and makes itself and the list of configureable classes
	known to the ConfigClass (ConfigClass::setConfigureClasses()
	and ConfigClass::setConfigClassList() ). */
  ConfigureClasses( void );
    /*! Constructs an empty ConfigureClasses (no configuration files)
        that is set up for \a groups of configuration files
        and makes itself and the list of configureable classes
	known to the ConfigClass (ConfigClass::setConfigureClasses()
	and ConfigClass::setConfigClassList() ).
        \param[in] groups the number of groups of configuration files
	to be used. */
  ConfigureClasses( int groups );
    /*! Constructs an ConfigureClasses with a single configuration file
        \a file (in one group)
        and makes itself and the list of configureable classes
	known to the ConfigClass (ConfigClass::setConfigureClasses()
	and ConfigClass::setConfigClassList() ).
        \param[in] file the name of the configuration file to be used. */
  ConfigureClasses( const string &file );
    /*! Destructs ConfigureClasses and removes itself and the list 
        of configureable classes from ConfigClass. */
  ~ConfigureClasses( void );

    /*! \return the number of different groups of configuration files.
        \sa addGroup() */
  int groups( void ) const;
    /*! Adds a new gropu of configuration files
        and optionally initializes the new group with
	the configuration file name \a file.
	\param[in] name of the configuration file for the added group.
	If left empty, then no filename is assigned to the group.
	\sa groups() */
  void addGroup( const string &file="" );

    /*! \return the name (full path) of the configuration file
        for group \a group and level \a level or an empty string
	if \a group or \a level are invalid.
        \param[in] group the group index of the requested configuration file.
        \param[in] level the level of the requested configuration file.
        \sa setConfigFile(), addConfigFile() */
  string configFile( int group=0, int level=0 ) const;
    /*! Set the name (full path) of the configuration file of group \a group
        for level \a level to \a file.
	If the group index or level do not exist or \a file is an empty string,
	this function has no effect.
	The group must have been created before
	via the constructor or addGroup().
	The level must have been created begore by addConfigFile().
        \param[in] file the name (full path) of the configuration file.
        \param[in] group the index of the group. 
        \param[in] level the level index. 
        \sa configFile(), addConfigFile() */
  void setConfigFile( const string &file, int group=0, int level=0 );
    /*! Add a new level to the group \a group and
        set the name of the corresponding configuration file to \a file.
	If the group index does not exist or \a file is an empty string,
	this function has no effect.
	The group must have been created before 
	via the constructor or addGroup().
        \param[in] file the name (full path) of the configuration file.
        \param[in] group the group index. 
        \sa configFile(), setConfigFile() */
  void addConfigFile( const string &file, int group=0 );

    /*! */
  void read( int group, int level );
    /*! */
  void read( int group );
    /*! */
  void read( void );

    /*! */
  void read( int group, int level, ConfigClass &config );
    /*! */
  void read( int group, ConfigClass &config );

    /*! */
  void configure( int group, const string &ident );
    /*! */
  void configure( int group );
    /*! */
  void configure( void );

    /*! */
  void save( int group, const string &file );
    /*! */
  void save( int group, int level );
    /*! */
  void save( int group );
    /*! */
  void save( void );

    /*! */
  friend ostream &operator<<( ostream &str, const ConfigureClasses &c );


private:

    /*! Returns a string with the current time. 
        Used for log meassages by read() and save() */
  string currentTime( void );
    /*! List of configuration file names for different
        groups and levels. */
  vector < vector < string > > ConfigFile;
    /*! List of pointers to ConfigClass instances 
        that want to be configured. */
  ConfigClassList Configs;

};


}; /* namespace relacs */

#endif /* ! _RELACS_CONFIGURECLASSES_H_ */
