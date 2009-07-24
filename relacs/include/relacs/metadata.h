/*
  metadata.h
  Manages meta data for a recording session.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <qpopupmenu.h>
#include <qmutex.h>
#include <relacs/configclass.h>

using namespace std;

namespace relacs {


class MetaData;
class RELACSWidget;


/*! 
\class MetaDataSection
\brief A section of MetaData
\author Jan Benda
\version 1.0
*/

class MetaDataSection : public ConfigClass
{

public:
  
  /*! Construct a section of meta data.
      \param[in] name the section title
      \param[in] group the group of the configuration file
                 where the configuration of this section is stored.
		 See ConfigClass for details.
      \param[in] tab does this section request an own tab in the dialog?
      \param[in] md pointer to the MetaData that created this section.
      \param[in] rw pointer to the main RELACSWidget.
  */
  MetaDataSection( const string &name, int group, bool tab,
		   MetaData *md, RELACSWidget *rw );
  virtual ~MetaDataSection( void );

    /*! Calls clear(), then loads the options from the config file
        and set their flags to MetaData::dialogFlag() and
	MetaData::configFlag(). */
  virtual void readConfig( StrQueue &sq );
    /*! Save the options that have the MetaData::configFlag() set
        to the configuration file. */
  virtual void saveConfig( ofstream &str );
    /*! React to changes of the meta data.
        This function calls notifyMetaData() in all RELACSPlugins. */
  virtual void notify( void );
    /*! Clear the options. */
  virtual void clear( void );
    /*! Save the name of the section and the options 
        that have MetaData::saveFlags() set in their flags()
	into info file \a str. */
  virtual void save( ofstream &str );
    /*! Write meta data that have MetaData::saveFlags() set in their flags()
        in XML format to output stream.
        \param[in] str the output stream
        \param[in] level the level of indentation
        \param[in] indent the indentation depth, 
                   i.e. number of white space characters per level
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int level=0, int indent=2 ) const;

    /*! \return \c true if this section shold get its own tab in the dialog. */
  bool ownTab( void ) const;
  /*! Determines whether this section should get its own tab
      in the dialog (\a tab = \c true). */
  void setOwnTab( bool tab );


protected:

  MetaData *MD;
  RELACSWidget *RW;


private:

  bool Tab;

};


/*! 
\class MetaDataRecordingSection
\brief The general Recording section of MetaData
\author Jan Benda
\version 1.0

A couple of predifined properties are defined set in the Recording section:
- File: the base path for all the files saved by the recording session
- Date: the date of the recording session
- Time: the time when the recording session ended
- Recording duration: the duration of the recording session
- Mode: "experiment" or "simulation"
- Software: "RELACS"
- Software version: the RELACS version number
All these standard options have the standardFlag() set.

The values of the standard options are set appropriately in save()
right before they are saved to the info file of the session.

You can remove individual standard options in 
Control::config() by doing something like
\code
  metaData( "Recording" ).erase( "File" );
\endcode
*/

class MetaDataRecordingSection : public MetaDataSection
{

public:
  
    /*! Construct a "recording" section of meta data
        and adds its standradFlag() to the MetaData::saveFlags().
        \param[in] tab does this section request an own tab in the dialog?
        \param[in] md pointer to the MetaData that created this section.
        \param[in] rw pointer to the main RELACSWidget.
    */
  MetaDataRecordingSection( bool tab, MetaData *md, RELACSWidget *rw );
  virtual ~MetaDataRecordingSection( void );

    /*! The number of options to be saved in the configuration file. */
  virtual int configSize( void ) const;

    /*! Clear the options and preset them with standard options. */
  virtual void clear( void );
    /*! Set the values of the standard options and 
        save the name of the section and the options 
        that have MetaData::saveFlags() set in their flags()
	into info file \a str. */
  virtual void save( ofstream &str );

    /*! The flag that is used to mark the standard options. */
  static int standardFlag( void );


private:

  static const int StandardFlag = 1024;

};


/*! 
\class MetaData
\brief Manages sections of meta data describing a recording session
\author Jan Benda
\version 1.0

Usually, meta data sections are defined in the \c relacs.cfg file and are
typically loaded from the \c relacsplugins.cfg file
(after Control::initialize() and before Control::initDevices() is called).

Never add options to a MetaDataSection within a Control constructor,
since these get cleared right before the meta data are loaded from the configuration file!

With the dialogFlag() and the presetDialogFlag() meta data
can be selected that are displayed in the dialog() or presetDialog(), respectively.

The "Recording" section (MetaDataRecordingSection)
is always used and contains a few standard properties.
*/

class MetaData : public QObject, public ConfigClass
{
  Q_OBJECT

public:
  
  MetaData( RELACSWidget *rw );
  ~MetaData( void );

    /*! Add a new section to the meta data.
        \param[in] name the section title
        \param[in] tab does this section request an own tab in the dialog? */
  void add( const string &name, bool tab=false );

    /*! Load the meta data Options from the configuration file
        and immediately create the requested MetaDataSections. */
  virtual void readConfig( StrQueue &sq );

    /*! React to changes in the meta data sections.
        This function calls notifyMetaData() in all RELACSPlugins.
        \param[in] section the name of the MetaDataSection in which the change occured. */
  void notifyMetaData( const string &section );

    /*! Saves the meta data into the info file of the session. */
  void save( void );
    /*! Write meta data that have saveFlags() set in their flags()
        in XML format to output stream.
        \param[in] str the output stream
        \param[in] level the level of indentation
        \param[in] indent the indentation depth, 
                   i.e. number of white space characters per level
        \return the output stream \a str */
  ostream &saveXML( ostream &str, int level=0, int indent=2 ) const;
    /*! Clear the meta data. */
  void clear( void );

    /*! \return \c true if a meta data section with name \a section exist. */
  bool exist( const string &section ) const;
    /*! Return the MetaData options from section \a section. */
  Options &section( const string &section );
    /*! Return the MetaData options from section \a section. */
  const Options &section( const string &section ) const;

    /*! Lock the meta data. */
  void lock( void ) const;
    /*! Unlock the meta data. */
  void unlock( void ) const;
    /*! The mutex of the meta data. */
  QMutex *mutex( void );

    /*! Add actions to the RELACS menu. */
  void addActions( QPopupMenu *menu );

    /*! The flag that is used to mark meta data options for the dialog. */
  static int dialogFlag( void );
    /*! The flag that is used to mark meta data options for the preset dialog. */
  static int presetDialogFlag( void );
    /*! The flag that is used by MetaDataSection::readConfig()
        to mark meta data options loaded from the config file. */
  static int configFlag( void );

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
        -1 to continue the session. */
  int dialog( void );
    /*! Launches a dialog from the menu.
        It is used to preset values of the dialog(). */
  void presetDialog( void );


protected slots:

    /*! Informs MetaData that the dialog window is closed. */
  void dialogClosed( int r );
    /*! Apply changes made in the dialog to the MetaDataSections. */
  void dialogChanged( void );
    /*! Informs MetaData that the preset dialog window is closed. */
  void presetDialogClosed( int r );
    /*! Apply changes made in the preset dialog to the MetaDataSections. */
  void presetDialogChanged( void );


private:

  static const int DialogFlag = 128;
  static const int PresetDialogFlag = 256;
  static const int ConfigFlag = 512;
  static const int LabelFlag = 4096;
  int SaveFlags;

  vector< MetaDataSection* > MetaDataSections;

  mutable Options DummyOpts;

  bool Dialog;
  bool PresetDialog;

  Options DialogOpts;
  Options PresetDialogOpts;
  
  mutable QMutex MetaDataLock;

  RELACSWidget *RW;

};


}; /* namespace relacs */

#endif /* ! _RELACS_METADATA_H_ */

