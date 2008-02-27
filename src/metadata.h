/*
  metadata.h
  Manages meta data for a recording session.

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

#ifndef _METADATA_H_
#define _METADATA_H_

#include <iostream>
#include <fstream>
#include <qobject.h>
#include <qpopupmenu.h>
#include <qmutex.h>
#include "config.h"

using namespace std;


class RELACSWidget;


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
Never add info() options within the %Control constructor!
With the infoDialogFlag() and the presetDialogFlag() Options
can be selected that are displayed in the dialog() or presetDialog().
*/

class MetaData : public QObject, public Config
{
  Q_OBJECT

public:
  
  MetaData( RELACSWidget *rw );
  ~MetaData( void );

    /*! Load the meta data Options from the configuration file. */
  virtual void readConfig( StrQueue &sq );
    /*! Save the meta data Options from the configuration file. */
  virtual void saveConfig( ofstream &str );
    /*! React to settings of the session info options.
        This function calls notifyMetaData() in all RELACSPlugins. */
  virtual void notify( void );

    /*! Saves the meta data into the info file of the session. */
  void save( void );
    /*! Clear the info options. */
  void clear( void );

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
    /*! The flag that is used to mark info standard options. */
  static int standardFlag( void );
    /*! The flag that is used to mark the meta data about
        the experimental setup. */
  static int setupFlag( void );
    /*! The flag that is used to select meta-data Options to be saved 
        in the info file of the session (default (=0) selects all). */
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
    /*! Informs MetaData that the preset dialog window is closed. */
  void presetDialogClosed( int r );


private:

  int Flags;

  static const int DialogFlag = 128;
  static const int PresetDialogFlag = 256;
  static const int ConfigFlag = 512;
  static const int StandardFlag = 1024;
  static const int SetupFlag = 2048;
  int SaveFlag;

  bool Dialog;
  bool PresetDialog;
  
  mutable QMutex MetaDataLock;

  RELACSWidget *RW;

};



/*! 
\class SetupData
\brief Includes data about the experimental setup into the configure mechanism.
\author Jan Benda
\version 1.0

The SetupData options are loaded from the relacs.cfg file and get
the setupFlag() set.
They are saved from their copy in MetaData.
*/

class SetupData : public Config
{

public:
  
  SetupData( Options *md );
  virtual ~SetupData( void );

    /*! Load the SetupData options from relacs.cfg and set setupFlag(). */
  virtual void readConfig( StrQueue &sq );
    /*! Save the SetupData options from Session::info(). */
  virtual void saveConfig( ofstream &str );
    /*! The size of the SetupData options within Session::info(). */
  virtual int configSize( void ) const;

    /*! The flag that is used to mark the meta data about
        the experimental setup. */
  static int setupFlag( void );


private:

  static const int SetupFlag = 2048;
  Options *MD;


};


#endif
