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
        that have MetaData::saveFlag() set into info file \a str. */
  virtual void save( ofstream &str );

    /*! Returns \c true if this section shold get its own tab in the dialog. */
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
*/

class MetaDataRecordingSection : public MetaDataSection
{

public:
  
  /*! Construct a "recording" section of meta data.
      \param[in] tab does this section request an own tab in the dialog?
      \param[in] md pointer to the MetaData that created this section.
      \param[in] rw pointer to the main RELACSWidget.
  */
  MetaDataRecordingSection( bool tab, MetaData *md, RELACSWidget *rw );
  virtual ~MetaDataRecordingSection( void );

    /*! The number of options to be save in the configuration file. */
  virtual int configSize( void ) const;

    /*! Clear the options and preset them with standard options. */
  virtual void clear( void );
    /*! Set the values of the standard options and 
        save the options into info file \a str. */
  virtual void save( ofstream &str );

};


/*! 
\class MetaData
\brief Manages meta data for a recording session
\author Jan Benda
\version 1.0

MetaData contains the meta data for a recording session.
They are loaded from the "MetaData" section of the \c relacsplugins.cfg file
(after Control::initialize() and before Contrl::initDevices() is called).
The MetaData-Options contain a few standard parameter:
(the name of the directory where the data are saved ("File"),
date ("Date") and time ("Time") of the session start, 
the duration of the session ("Recording duration"), 
the operation mode ("Mode", idle, acquisition, simulation, or analysis),
the name of the software ("Software: RELACS"), and
the version of the software ("Software version").
You can remove individual standard options in 
Control::initialize() by doing something like
\code
  metaData().erase( "File" );
\endcode
All standard options have the standardFlag() set.
The values of the standard options are set appropriately in save()
right before they are saved to the info file of the session.
Never add options to MetaData within a Control constructor!
With the dialogFlag() and the presetDialogFlag() Options
can be selected that are displayed in the dialog() or presetDialog().
*/

class MetaData : public QObject, public ConfigClass
{
  Q_OBJECT

public:
  
  MetaData( RELACSWidget *rw );
  ~MetaData( void );

    /*! Load the meta data Options from the configuration file
        and immediately create the requested MetaDataSections. */
  virtual void readConfig( StrQueue &sq );

    /*! React to changes in the meta data sections.
        This function calls notifyMetaData() in all RELACSPlugins. */
  void notifyMetaData( void );

    /*! Saves the meta data into the info file of the session. */
  void save( void );
    /*! Clear the meta data. */
  void clear( void );

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

    /*! The flag that is used to mark options for the dialog. */
  static int dialogFlag( void );
    /*! The flag that is used to mark options for the preset dialog. */
  static int presetDialogFlag( void );
    /*! The flag that is used to mark options loaded from the config file. */
  static int configFlag( void );
    /*! The flag that is used to mark the standard options. */
  static int standardFlag( void );
    /*! The flag that is used to mark the meta data about
        the experimental setup. */
  static int setupFlag( void );
    /*! The flag that is used to select meta-data Options to be saved 
        in the info file of the session 
	(default (=0) selects all). */
  int saveFlag( void ) const;
    /*! Set the flag that is used to select meta-data Options to be saved 
        in the info file of the session to \a flag.
	Setting \a flag to 0 selects all (default). */
  void setSaveFlag( int flag );


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
  static const int StandardFlag = 1024;
  static const int LabelFlag = 2048;
  static const int SetupFlag = 4096;
  int SaveFlag;

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

