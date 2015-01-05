/*
  configclass.h
  Base class for each class that has some parameters to be configured.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#ifndef _RELACS_CONFIGCLASS_H_
#define _RELACS_CONFIGCLASS_H_ 1

#include <string>
#include <fstream>
#include <vector>
#include <relacs/strqueue.h>
#include <relacs/options.h>
using namespace std;

namespace relacs {

/*!
\class ConfigClass
\brief Base class for each class that has some parameters to be configured.
\author Jan Benda
\version 1.1

Each ConfigClass instance can read and save configuration settings from/to
its section of a configuration file.
The sections are introduced by a line starting with '*' immediately followed
by the configIdent().
A group of configuration files can be selected by setConfigGroup()
(see ConfigureClasses for more details).

The configMode() determines whether 
the settings of this instance should be saved in a configuration file,
get a configuration dialog, etc. See ConfigClass::Action for possible values.

The configSelectMask() selects a subset of items to be saved
in a configuration file.

readConfig( StrQueue& ) reads the configuration settings from 
its section of a configuration file.
saveConfig() stores the configuration settings in a configuration file
and configSize() returns the number of items to be stored.
The default implementation of these functions use the corresponding
functions Options::read(), Options::save(), and Options::size()
from the Options class.
You may reimplement them according to your needs.

The config() function can be reimplemented for some post-configuration
after all configuration files have been read in.
The preConfig() function is for some pre-configuration before the configuration
files are read in.
*/

class ConfigureClasses;

class ConfigClass : public Options
{

public:

    /*! Possible flags for the configuration mode. */
  enum Action {
      /*! The ConfigClass instance should get its configuration settings
          saved to a configuration file. */
    Save=1,
      /*! The ConfigClass instance should get a configuration dialog. */
    Dialog=2,
      /*! The ConfigClass instance provides a help text. */
    Help=4,
      /*! The ConfigClass instance should get a menu entry. */
    Menu=8
  };

    /*! Constructs a ConfigClass with identifier \a ident
        that is a member of the configuration group with
        index \a group. It has the configuration mode \a mode
        and \a selectmask is used to select specific items
        to be saved into the configuration file.
	The new ConfigClass instance is then added to the
	static list of ConfigClass instances managed by ConfigureClasses.
        \param[in] ident the identifier string of the ConfigClass instance.
	It is used to specify the right section in a configuration file.
	\param[in] group the index of the configuration group
	where the ConfigClass instance should belong to.
        \param[in] mode an integer flag that determines whether
	the settings of this instance should be saved in a configuration file,
	get a configuration dialog, etc. 
	See ConfigClass::Action or possible values.
        \param[in] selectmask selects a subset of items to be saved
	in a configuration file.
        \sa addConfig() */
  ConfigClass( const string &ident, int group=0, int mode=0xffff,
	       int selectmask=0 );
    /*! Copy constructor. Never should be called. */
  ConfigClass( const ConfigClass &C );
    /*! Deconstructs a ConfigClass and removes it from
        the list of ConfigClass instances. */
  virtual ~ConfigClass( void );

    /*! \return the identifier string that is used to specify the
        right section for this ConfigClass instance in a configuration file. 
        \sa setConfigIdent() */
  const string &configIdent( void ) const;
    /*! Set the identifier string that is used to specify the
        right section for this ConfigClass instance in a configuration file
	to \a ident. 
	\param[in] ident the identifier string of this ConfigClass instance.
        \sa configIdent() */
  void setConfigIdent( const string &ident );

    /*! \return the index of the configuration group 
        this instance of ConfigClass belongs to.
        \sa setConfigGroup() */
  int configGroup( void ) const;
    /*! Set the index of the configuration group 
        this instance of ConfigClass belongs to to \a group.
	\param[in] group the index of the configuration group.
        \sa configGroup() */
  void setConfigGroup( int group );

    /*! \return the mode flags of this ConfigClass instance that determines whether
	its settings should be saved in a configuration file,
	get a configuration dialog, etc. 
	See ConfigClass::Action or possible values.
        \sa setConfigMode(), addConfigMode(), delConfigMode() */
  int configMode( void ) const;
    /*! Set the mode flags of this instance of ConfigClass to \a mode.
	\param[in] mode the mode flags that determine whether
	the settings of this instance should be saved in a configuration file,
	get a configuration dialog, etc. 
	See ConfigClass::Action or possible values.
        \sa configMode(), addConfigMode(), delConfigMode() */
  void setConfigMode( int mode );
    /*! Sets the bits specified by \a mode in the mode flags.
	\param[in] mode the mode flags that determine whether
	the settings of this instance should be saved in a configuration file,
	get a configuration dialog, etc. 
	See ConfigClass::Action or possible values.
        \sa configMode(), setConfigMode(), delConfigMode() */
  void addConfigMode( int mode );
    /*! Unsets the bits specified by \a mode from the mode flags.
	\param[in] mode the mode flags that determine whether
	the settings of this instance should be saved in a configuration file,
	get a configuration dialog, etc. See ConfigClass::Action or possible values.
        \sa setConfigMode(), addConfigMode(), configMode() */
  void delConfigMode( int mode );

    /*! \return the mask that is used to select specific items
        to be saved into the configuration file.
        \sa setConfigSelectMask() */
  int configSelectMask( void ) const;
    /*! Set the mask that is used to select specific items
        to be saved into the configuration file to \a mask.
	\param[in] mask the mask for selecting configuration items.
        \sa configSelectMask() */
  void setConfigSelectMask( int mask );

    /*! Adds this ConfigClass instance to the list of ConfigClass instances.
        This function needs to be called only if at the time of construction
        this list was not yet created, i.e. no instance of ConfigureClasses
        existed. */
  void addConfig( void );
    /*! Read in all configuration files and pass the right sections
        to readConfig( StrQueue& ) of this instance only.
        \sa readConfig( StrQueue& ) */
  void readConfig( void );

    /*! Read out the configuration settings from \a sq.
        The default implementation simply calls Options::read( sq, 0, ':' )
	to set the Options directly as specified by \a sq.
	This function is called from the ConfigureClasses::read() functions.
        \param[in] sq the section from a configuration file
        for this ConfigClass instance.
        The first line specifying the ConfigClass instance is already removed
        as well as any leading and trailing empty lines.
        \sa saveConfig(), readConfig( void ) */
  virtual void readConfig( StrQueue &sq );
    /*! Save the configuration settings to \a str.
        The default implementation simply calls 
	Options::save( str, "  ", -1, configSelectMask(), CFG->Detailed, CFG->FirstOnly )
	to directly save the selected Options to \a str.
	This function is called from the ConfigureClasses::save() functions
	only if configMode() has the Save flag set.
        \param[in] str the output stream for saving the configuration
	settings. The first line specifying the ConfigClass instance
	is already written.
        \sa readConfig( StrQueue& ) */
  virtual void saveConfig( ofstream &str );
    /*! \return the number of items as selected by configSelectMask()
        to be saved to a configuration file.
	saveConfig() is only called from the ConfigureClasses::save()
	functions if configSize() returns a number greater than zero.
        The default implementation returns Options::size( configSelectMask() ). */
  virtual int configSize( void ) const;
    /*! This function can be reimplemented to do some post-configuration.
        It is called by ConfigureClasses::configure() after all configuration
        files have been read in via readConfig( StrQueue& ). */
  virtual void config( void );
    /*! This function can be reimplemented to do some pre-configuration.
        It is called by ConfigureClasses::preConfigure() before the configuration
        files are read in via readConfig( StrQueue& ). */
  virtual void preConfig( void );

    /*! This function is called by the constructors of ConfigureClasses
        to make the ConfigureClasses instance \a cfg known to 
        each ConfigClass. */
  static void setConfigureClasses( ConfigureClasses *cfg );


private:

    /*! The string identifying sections in the configuration files
        for this ConfigClass instance. */
  string ConfigIdent;
    /*! The index to the configuration group this ConfigClas instance
        belongs to. */
  int ConfigGroup;
    /*! The configuration mode of this ConfigClass instance. */
  int ConfigMode;
    /*! The mask used for selecting configuration items. */
  int ConfigSelect;
    /*! A pointer to the ConfigureClasses instance. */
  static ConfigureClasses *CFG;

};


}; /* namespace relacs */

#endif /* ! _RELACS_CONFIGCLASS_H_ */
