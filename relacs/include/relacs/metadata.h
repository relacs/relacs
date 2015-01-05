/*
  metadata.h
  Manages meta data for a recording session.

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

    /*! Clear all Options and create Recording section with standarad
        parameter. */
  void clear( void );

    /*! Add standard parameter to \a opt. */
  static void addRecordingOptions( Options *opt );


protected:

  MetaData *MD;

};


/*!
\class MetaData
\brief Manages meta data describing a recording session
\author Jan Benda

Meta data are loaded from both the \c relacs.cfg and the \c
relacsplugins.cfg file (after Control::initialize() and before
Control::initDevices() is called) via two instances of the
MetaDataGroup class.

Never add options to MetaData within a Control constructor,
since these get cleared right before the meta data are loaded from the
configuration files!

With the dialogFlag() and the presetDialogFlag() meta data can be
selected that are displayed in the dialog() or presetDialog(),
respectively.

The "Recording" section is always created by MetaData and contains a
few standard properties.
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

If you really want you can remove individual standard options in
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
        This function calls notifyMetaData() in all RELACSPlugins. */
  virtual void notify( void );

    /*! Update the values of the standarad meta data.
        Should be called before save(). */
  void update( void );
    /*! Add the meta data \a opts with title \a title as a section to the meta data,
        if both of them are not empty. \sa remove() */
  void add( const string &title="", const Options &opts = Options() );
    /*! Remove an added section from the metadata. \sa update() */
  void remove( void );

    /*! Saves the meta data of all sections into the info file of the session. */
  void save( void );
    /*! Write meta data that have saveFlags() set in their flags()
        in XML format to output stream.
        \param[in] str the output stream
        \param[in] level the level of indentation
        \param[in] name the name prefix for the name tag of the section.
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int level=0, const string &name="" );

    /*! Clear the entire meta data. */
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


protected:

  virtual void customEvent( QEvent *qe );
  void setSectionName( Options *opt, const string &name );


private:

  static const int DialogFlag = 128;
  static const int PresetDialogFlag = 256;
  static const int ConfigFlag = 512;
  static const int StandardFlag = 1024;
  int SaveFlags;
  Options *AddedSection;

  MetaDataGroup CoreData;
  MetaDataGroup PluginData;

  bool Dialog;

  mutable QMutex MetaDataLock;

  RELACSWidget *RW;

};


}; /* namespace relacs */

#endif /* ! _RELACS_METADATA_H_ */

