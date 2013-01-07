/*
  metadata.h
  Manages meta data for a recording session.

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

#ifndef _RELACS_METADATA_H_
#define _RELACS_METADATA_H_ 1

#include <iostream>
#include <fstream>
#include <vector>
#include <QMenu>
#include <QMutex>
#include <relacs/configclass.h>

using namespace std;

namespace relacs {


class MetaData;
class RELACSWidget;


/*! 
\class MetaDataGroup
\brief Loads meta data from a configuration file
\author Jan Benda
*/

class MetaDataGroup : public ConfigClass
{

public:
  
  /*! Construct MetaDataGroup for loading setup specific meta data.
      \param[in] group identifies the group of configuration files from which
      the "Metadata" section is loaded.
      \param[in] md pointer to the MetaData that manages all meta data.
  */
  MetaDataGroup( int group, MetaData *md );
  virtual ~MetaDataGroup( void );

    /*! Loads the options from the config file and set their flags to
        MetaData::configFlag() and MetaData::dialogFlag(). */
  virtual void readConfig( StrQueue &sq );
    /*! Save the options that have the MetaData::configFlag() set to
        the configuration file. */
  virtual void saveConfig( ofstream &str );
    /*! \return the number of items as selected by MetaData::configFlag()
        to be saved to a configuration file. */
  virtual int configSize( void ) const;
    /*! React to changes of the meta data.
        This function calls notifyMetaData() in all RELACSPlugins. */
  virtual void notify( void );

    /*! Clear all Options and create Recording section with standarad parameter. */
  void clear( void );

    /*! Add standard parameter to \a opt. */
  static void addRecordingOptions( Options *opt );


protected:

  MetaData *MD;

};


/*! 
\class MetaData
\brief Managesd meta data describing a recording session
\author Jan Benda

Meta data are defined loaded from both the \c relacs.cfg and the \c
relacsplugins.cfg file (after Control::initialize() and before
Control::initDevices() is called).

Never add options to a MetaData within a Control constructor,
since these get cleared right before the meta data are loaded from the
configuration files!

With the dialogFlag() and the presetDialogFlag() meta data can be
selected that are displayed in the dialog() or presetDialog(),
respectively.

The "Recording" section is always used and contains a few standard
properties (see MetaDataPlugins). 

Also ensures the existance of a Recording section.
A couple of predifined properties are defined in the Recording section:
- File: the base path for all the files saved by the recording session
- Date: the date of the recording session
- Time: the time when the recording session ended
- Recording duration: the duration of the recording session
- Mode: "experiment" or "simulation"
- Software: "RELACS"
- Software version: the RELACS version number
All these standard options have the MetaData::standardFlag() set.

The values of the standard options are set appropriately in save()
right before they are saved to the info file of the session.

You can remove individual standard options in 
Control::config() by doing something like
\code
  metaData( "Recording" ).erase( "File" );
\endcode
*/

class MetaData : public QObject, public Options
{
  Q_OBJECT

public:
  
  MetaData( RELACSWidget *rw );
  ~MetaData( void );

    /*! React to changes in the meta data sections.
        This function calls notifyMetaData() in all RELACSPlugins.
        \param[in] section the name of the MetaData section in which the change occured. */
  void notifyMetaData( const string &section );

    /*! Saves the meta data of all sections into the info file of the session.
        Additionally appends the meta data \a opts with title \a title
        to the info file, if both of them are not empty. */
  void save( const string &title="", const Options &opts = Options() );
    /*! Write meta data that have saveFlags() set in their flags()
        in XML format to output stream.
        \param[in] str the output stream
        \param[in] level the level of indentation
        \param[in] indent the indentation depth, 
                   i.e. number of white space characters per level
        \param[in] name the name prefix for the name tag of the section.
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int level=0, int indent=2, const string &name="" );
    /*! Clear the meta data. */
  void clear( void );

    /*! Notify MetaData about loaded meta data \a opt. */
  void add( ConfigClass *opt );

    /*! Lock the meta data. */
  void lock( void ) const;
    /*! Unlock the meta data. */
  void unlock( void ) const;
    /*! The mutex of the meta data. */
  QMutex *mutex( void );

    /*! Add actions to the RELACS menu. */
  void addActions( QMenu *menu );

    /*! The flag that is used to mark meta data options for the dialog. */
  static int dialogFlag( void );
    /*! The flag that is used to mark meta data options for the preset dialog. */
  static int presetDialogFlag( void );
    /*! The flag that is used by MetaDataGroup::readConfig()
        to mark meta data options loaded from the config file. */
  static int configFlag( void );
    /*! The flag that is used to mark the standard options. */
  static int standardFlag( void );

    /*! The flags that are used to select meta-data Options to be saved 
        in the info file of the session 
	(default (=0) selects all). */
  int saveFlags( void ) const;
    /*! Set the flags that are used to select meta-data Options to be saved 
        in the info file of the session to \a flags.
	Setting \a flags to 0 selects all (default). */
  void setSaveFlags( int flags );
    /*! Add \a flags to the flags that are used to select meta-data Options to be saved 
        in the info file of the session. */
  void addSaveFlags( int flags );
    /*! Delete the bits set in \a flags from the flags that are used to select 
        meta-data Options to be saved in the info file of the session. */
  void delSaveFlags( int flags );


public slots:

    /*! Launches a dialog at stopTheSession(). 
        It should return 1 to stop and safe the data, 
	0 to stop and discard the data, and
        -1 to continue the session.
        If -1000 is returned,
        then the dialog() functions should be called again at a later time,
	because the MetaData are currently locked. */
  int dialog( void );
    /*! Launches a dialog from the menu.
        It is used to preset values of the dialog(). */
  void presetDialog( void );


protected slots:

    /*! Informs MetaData that the dialog window is closed. */
  void dialogClosed( int r );
    /*! Apply changes made in the dialog to the MetaDataSections. */
  void dialogChanged( void );


protected:

  virtual void customEvent( QEvent *qe );


private:

  static const int DialogFlag = 128;
  static const int PresetDialogFlag = 256;
  static const int ConfigFlag = 512;
  static const int StandardFlag = 1024;
  int SaveFlags;

  MetaDataGroup CoreData;
  MetaDataGroup PluginData;

  bool Dialog;
  
  mutable QMutex MetaDataLock;

  RELACSWidget *RW;

};


}; /* namespace relacs */

#endif /* ! _RELACS_METADATA_H_ */

