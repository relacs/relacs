/*
  configureclasses.h
  Coordinates configureable classes ConfigClass.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
\brief Coordinates storage of configuration settings of ConfigClass instances.
\author Jan Benda
\version 1.1

ConfigureClasses stores a pointer to each instance of a ConfigClass.
The configuration of each ConfigClass instance can be read in from
configuration files by the read() functions and saved by the save()
functions. The configure() functions call the config() function
of the ConfigClass instances for some post-configuration.

ConfigClass instances are organized in configurations groups 
via their ConfigClass::configGroup() flag.
The configuration groups are indexed starting with 0.
Each configuration group gets its own set of configuration files.
Configuration groups can be added with addGroup(), cleared with clearGroups(),
and the number of defined groups is returned by groups().

To each configuration group several configuration files of increasing
level can be added by addConfigFile(), set by setConfigFile(),
and cleared by clearConfigFiles().
The read() functions read in the configuration files with
increasing level, such that configuration files of higher levels may overwrite
the settings specified by the configuration files of lower levels.

For example, if your program should first read in a system wide configuration
file in \c /etc/myprog.cfg, then a personal configuration file in
\c /home/me/.myprog.cfg, and finally a configuration file \c myprog.cfg
in the current working directory, then you would specify three levels
of configuration files in the following order:
\code
ConfigureClasses c;
c.addConfigFile( "/etc/myprog.cfg" );
c.addConfigFile( "/home/me/.myprog.cfg" );
c.addConfigFile( "myprog.cfg" );
\endcode

The name of a configuration file of a given configuration group and level
is returned by configFile().
*/

class ConfigureClasses
{

  friend class ConfigClass;

public:

    /*! Constructs a ConfigureClasses with a single but empty
        (no configuration files) group of configuration files
        and makes itself and the list of configureable classes
	known to the ConfigClass (ConfigClass::setConfigureClasses()
	and ConfigClass::setConfigClassList() ).
	You need to add configuration files to the group by calling addConfigFile().
        You can add further configuration groups by calling addGroup(). */
  ConfigureClasses( void );
    /*! Constructs an empty ConfigureClasses (no configuration files)
        that is set up for \a groups of configuration files
        and makes itself and the list of configureable classes
	known to the ConfigClass (ConfigClass::setConfigureClasses()
	and ConfigClass::setConfigClassList() ).
	You then can specify configuration file names
	by calling addConfigFile( const string&, int) for each of the groups
	at least once.
        \param[in] groups the number of groups of configuration files
	to be used. */
  ConfigureClasses( int groups );
    /*! Constructs an ConfigureClasses with a single configuration file
        \a file (in one configuration group)
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
    /*! Adds a new group of configuration files
        and optionally initializes the new configuration group with
	the configuration file names given in \a files.
	\param[in] files names (full pathes) of the configuration files
	for the added group separated by '|'.
	If left empty, then no filename is assigned to the group.
	\sa groups(), addConfigFile() */
  void addGroup( const string &files="" );
    /*! Clear all configuration groups and all
        levels of configuration files they contain.
	\sa clearConfigFiles(), addGroup() */
  void clearGroups( void );

    /*! \return the name (full path) of the configuration file
        for configuration group \a group and level \a level or an empty string
	if \a group or \a level are invalid.
        \param[in] group the configuration group index of the requested configuration file.
        \param[in] level the level of the requested configuration file.
        \sa setConfigFile(), setConfigFiles(),
	addConfigFile(), addConfigFiles(), clearConfigFiles() */
  string configFile( int group=0, int level=0 ) const;
    /*! Set the name (full path) of the configuration file of group \a group
        for level \a level to \a file.
	Configuration files of level 0 are read in first.
	Thus, configuration files of higher levels may overwrite
	the settings specified by the configuration files of lower levels.
	If the configuration group index or level do not exist or 
	\a file is an empty string, this function has no effect.
	The configuration group must have been created before
	via the constructor or addGroup().
	The level must have been created before by addConfigFile().
        \param[in] file the name (full path) of the configuration file.
        \param[in] group the index of the configuration group. 
        \param[in] level the level index. 
        \sa configFile(), setConfigFiles(),
	addConfigFile(), addConfigFiles(), clearConfigFiles() */
  void setConfigFile( const string &file, int group=0, int level=0 );
    /*! Set the names (full pathes) of the configuration files 
        of group \a group according to \a files.
	If the configuration group index does not exist
	this function has no effect.
	The configuration group must have been created before
	via the constructor or addGroup().
        \param[in] files the names (full pathes) of the configuration files
	separated by '|'.
        \param[in] group the index of the configuration group. 
        \sa configFile(), setConfigFile(),
	addConfigFile(), addConfigFiles(), clearConfigFiles() */
  void setConfigFiles( const string &files, int group=0 );
    /*! Add a new level to the configuration group \a group and
        set the name of the corresponding configuration file to \a file.
	This configuration file is read in after the previously
	added configuration files of the same group
	and might therefore overwrite their settings.
	If the configuration group index does not exist or
	\a file is an empty string, this function has no effect.
	The configuration group must have been created before 
	via the constructor or addGroup().
        \param[in] file the name (full path) of the configuration file.
        \param[in] group the configuration group index. 
        \sa addGroup(), configFile(), setConfigFile(), setConfigFiles(),
	addConfigFiles(), clearConfigFiles() */
  void addConfigFile( const string &file, int group=0 );
    /*! Add the names (full pathes) of the configuration files 
        of group \a group according to \a files.
	If the configuration group index does not exist
	this function has no effect.
	The configuration group must have been created before
	via the constructor or addGroup().
        \param[in] files the names (full pathes) of the configuration files
	separated by '|'.
        \param[in] group the index of the configuration group. 
        \sa addGroup(), configFile(), setConfigFile(), setConfigFiles(),
	addConfigFile(), clearConfigFiles() */
  void addConfigFiles( const string &files, int group=0 );
    /*! Clear all levels of configuration files of
        the configuration group with index \a group.
        \param[in] group the configuration group index. 
	\sa clearGroups(), configFile(), setConfigFile(), setConfigFiles(),
	addConfigFile(), addConfigFiles() */
  void clearConfigFiles( int group );
    /*! Clear all levels of configuration files of
        all configuration groups.
	The groups are not erased, this is done by clearGroups().
	\sa configFile(), setConfigFile(), setConfigFiles(),
	addConfigFile(), addConfigFiles() */
  void clearConfigFiles( void );

    /*! Read in the configuration of the configuration group with
        index \a group from file \a file and pass each section to
	the corresponding ConfigClass instances via ConfigClass::readConfig().
        \param[in] group the configuration group index.
        \param[in] file the file from which the configuration is read in.
        \sa read( int, int), save(), configure() */
  void read( int group, const string &file );
    /*! Read in the configuration file of the configuration group with
        index \a group and the level \a level and pass each section to
	the corresponding ConfigClass instances via ConfigClass::readConfig().
        \param[in] group the configuration group index of the requested configuration file.
        \param[in] level the level of the requested configuration file.
        \sa read( int, ConfigClass&), save(), configure() */
  void read( int group, int level );
    /*! Read in the configuration files of all levels
        of the configuration group with index \a group and pass each section 
	to the corresponding ConfigClass instances
	via ConfigClass::readConfig().
	Configuration files of level 0 are read in first.
	Thus, configuration files of higher levels may overwrite
	the settings specified by the configuration files of lower levels.
        \param[in] group the configuration group index of the requested configuration files.
        \sa read( ConfigClass&), save(), configure() */
  void read( int group );
    /*! Read in the configuration files of all configuration groups and all
        levels and pass each section to the corresponding ConfigClass
	instances via ConfigClass::readConfig().
	Configuration files of level 0 are read in first.
	Thus, configuration files of higher levels may overwrite
	the settings specified by the configuration files of lower levels.
        \sa save(), configure() */
  void read( void );

    /*! Read in the configuration file of the configuration group as
        specified by config.configGroup() and the level \a level.
	Pass the section for \a config to \a config
	via ConfigClass::readConfig().
        \param[in] level the level of the requested configuration file.
	\param[in] config the ConfigClass instance for which the 
	configuration settings should be read in.
        \sa read( int, int ), save(), configure() */
  void read( int level, ConfigClass &config );
    /*! Read in the configuration files of the configuration group as
        specified by config.configGroup() and all levels.
	Pass the sections for \a config to \a config
	via ConfigClass::readConfig().
	Configuration files of level 0 are read in first.
	Thus, configuration files of higher levels may overwrite
	the settings specified by the configuration files of lower levels.
	\param[in] config the ConfigClass instance for which the 
	configuration settings should be read in.
        \sa read( int ), save(), configure() */
  void read( ConfigClass &config );

    /*! Configure, i.e. call ConfigClass::config() of the 
        ConfigClass instance from the configuration group with index \a group
        and identifier (ConfigClass::configIdent()) matching \a ident.
	\note This function should be called only after
	all configuration files have been read in.
        \param[in] group the configuration group index.
        \param[in] ident the configuration identifier specifying
	a ConfigClass instance.
	\sa read(), save() */
  void configure( int group, const string &ident );
    /*! Configure, i.e. call ConfigClass::config() of all the 
        ConfigClass instances from the configuration group with index \a group. 
	\note This function should be called only after
	all configuration files have been read in.
        \param[in] group the configuration group index.
	\sa read(), save() */
  void configure( int group );
    /*! Configure, i.e. call ConfigClass::config() of all 
        ConfigClass instances.
	\note This function should be called only after
	all configuration files have been read in.
	\sa read(), save() */
  void configure( void );

    /*! Save the configuration of all ConfigClass instances of the 
        configuration group with index \a group to the file \a file
	by calling ConfigClass::saveConfig().
	Only the settings of those ConfigClass instances are saved
	that have the ConfigClass::Save flag 
	in their ConfigClass::configMode() set.
        \param[in] group the configuration group index.
        \param[in] file the name (full path) of the file.
        \sa setSaveStyle(), read(), configure() */
  void save( int group, const string &file );
    /*! Save the configuration of all ConfigClass instances of the 
        configuration group with index \a group to the
	configuration file from level \a level
	by calling ConfigClass::saveConfig().
	Only the settings of those ConfigClass instances are saved
	that have the ConfigClass::Save flag 
	in their ConfigClass::configMode() set.
        \param[in] group the configuration group index.
        \param[in] level the level of the requested configuration file.
        \sa setSaveStyle(), read(), configure() */
  void save( int group, int level );
    /*! Save the configuration of all ConfigClass instances of the 
        configuration group with index \a group to the 
	respective top-level configuration file
	by calling ConfigClass::saveConfig().
	Only the settings of those ConfigClass instances are saved
	that have the ConfigClass::Save flag 
	in their ConfigClass::configMode() set.
        \param[in] group the configuration group index.
        \sa setSaveStyle(), read(), configure() */
  void save( int group );
    /*! Save the configuration of all ConfigClass instances to the 
	respective top-level configuration file of each configuration group
	by calling ConfigClass::saveConfig().
	Only the settings of those ConfigClass instances are saved
	that have the ConfigClass::Save flag 
	in their ConfigClass::configMode() set.
        \sa setSaveStyle(), read(), configure() */
  void save( void );

    /*! Set the way how the configuration is saved for the default implementation
        of COnfigClass::saveConfig().
        \param[in] detailed if \c true the identifier together with the request string are saved
        \param[in] firstonly if \c true only the first value of each option is saved
        \sa save() */
  void setSaveStyle( bool detailed, bool firstonly );

    /*! Write the names of all configuration files to \a str.
        The file names of each configuration group are written
        in a single line, levels are separated by ','.
        \param[in] str the output stream.
        \param[in] c the ConfigureClasses instance. */
  friend ostream &operator<<( ostream &str, const ConfigureClasses &c );


protected:

    /*! Adds a class that wants to be configured to the list of classes.
        \param[in] cfg a pointer to the configureable class. */
  void addConfigClass( ConfigClass *cfg );
    /*! Removes a class from the list of classes.
        \param[in] cfg a pointer to the configureable class. */
  void eraseConfigClass( ConfigClass *cfg );


private:

    /*! Returns a string with the current time. 
        Used for log meassages by read() and save() */
  string currentTime( void );
    /*! List of configuration file names for different
        groups and levels. */
  vector < vector < string > > ConfigFile;
    /*! List of pointers to ConfigClass instances 
        that want to be configured. */
  typedef vector<ConfigClass*> ConfigClassList;
  ConfigClassList Configs;

    /*! Save identifier string together with request string to configuration file. */
  bool Detailed;
    /*! Save only the first value of each configuration option. */
  bool FirstOnly;

};


}; /* namespace relacs */

#endif /* ! _RELACS_CONFIGURECLASSES_H_ */
