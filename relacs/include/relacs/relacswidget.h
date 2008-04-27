/*
  relacswidget.h
  Coordinates RePros, session, input, output, plotting, and saving. 

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

#ifndef _RELACSWIDGET_H_
#define _RELACSWIDGET_H_

#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qapplication.h>
#include <vector>
#include <relacs/configclass.h>
#include <relacs/configureclasses.h>
#include "settings.h"
#include "relacsdevices.h"
#include "metadata.h"
#include "plottrace.h"
#include <relacs/acquire.h>
#include "simulator.h"
#include "savefiles.h"
#include "filterdetectors.h"
#include "session.h"
#include "control.h"
#include "relacsplugin.h"
#include "repros.h"
#include "macros.h"

using namespace std;


/*! 
\class RELACSWidget
\brief Coordinates RePros, session, input, output, plotting, and saving. 
\author Jan Benda
\author Christian Machens
\version 1.8

\bug what about wroteData?
\todo setupHardware() addInput: make reference type configurable
\todo Set ID gain factors before configuring Session

The data are acquired from the DAQ boards and filtered, 
events are detected, data are saved,
plotted, and analysed from an extra thread, function run().

readRePro() periodically calls the read()
function of the current RePro. It is controlled by sleep()
which is called from the RePro.

A RePro is stoppped with the stopRePro() function and a new RePro is
started with startRePro().
A RePro terminates itself by calling doneRePro().

When a session is started, startSession() is called.
If the session is stopped, first preStopSession() is called.
Then the dialog for the meta-data is launched.
If the session is really to be stopped, stopSession() is called.

*/


class RELACSWidget : public QMainWindow, public QThread, public ConfigClass
{
  Q_OBJECT

public:

  RELACSWidget( const string &configbase, int mode,
		QWidget *parent=0, const char *name=0 );
  ~RELACSWidget( void );

    /*! Start data aquisition and first RePro. */
  void init( void );

    /*! Locks the GUI thread. 
        Use it whenever you call a function from within run()
	that directly or indirectly draws on the screen.
        \sa unlockGUI() */
  void lockGUI( void );
    /*! Unlocks the GUI thread.. 
        \sa lockGUI() */
  void unlockGUI( void );

    /*! Write current time and \a message to stderr and into a log file. */
  void printlog( const string &message ) const;

    /*! Updates the InData buffers and calls the filter and event detectors */
  void updateData( void );
    /*! Writes the data to files and plots it. */
  void processData( void );
    /*! Locks the data mutex of the data thread for reading. */
  void readLockData( void ) { DataMutexCount++; DataMutex.lock(); };
    /*! Locks the data mutex of the data thread for writing. */
  void writeLockData( void ) { DataMutexCount++; DataMutex.lock(); };
    /*! Unlocks the data mutex of the data thread. */
  void unlockData( void ) { DataMutexCount--; DataMutex.unlock(); };
    /*! Returns how often the DataMutex is locked. */
  int dataMutexCount( void ) { return DataMutexCount; };
    /*! Wakes up all waitconditions. */
  void wakeAll( void );

    /*! Output of signal \a signal.
        Passing infos to SaveFiles. */  
  int write( OutData &signal );
  int write( OutList &signal );

    /*! Data recorded during runtime of the current RePro don't have to be saved. */
  void noSaving( void );
    /*! */
  void plotToggle( bool on, bool fixed, double length, double offs );

    /*! Inform all plugins about changes in the stimulus data. */
  void notifyStimulusData( void );
    /*! Inform all plugins about changes in the meta data. */
  void notifyMetaData( void );

    /*! Tells RELACSWidget that a new Session is started. Opens the files of
        SaveFiles and starts the initial macro if \a startmacro is true. */
  void startSession( bool startmacro=true );
    /*! Tells RELACSWidget that the current session might be stopped.
        Starts the fallback macro. */
  void preStopSession( void );
    /*! Tells RELACSWidget that the current session is not stopped. */
  void resumeSession( void );
    /*! Tells RELACSWidget that the current session is stopped. */
  void stopSession( bool saved );

    /*! The name of a started macro. */
  void startedMacro( const string &ident, const string &param );

    /*! The current working mode.
        Can be AcquisitionMode, SimulationMode, AnalysisMode, or IdleMode. 
        \sa acquisition(), simulation(), analysis(), idle() */
  int mode( void ) const;
    /*! Return a string describing the current working mode.
        \sa mode(), acquisition(), simulation(), analysis(), idle() */
  string modeStr( void ) const;
    /*! Acquiring real data from a data acquisition board. 
        \sa mode(), aquisition() */
  const static int AcquisitionMode = 1;
    /*! Simulate data using a Model. 
        \sa mode(), simulation() */
  const static int SimulationMode = 2;
    /*! Reanalyse previously recorded or simulated data. 
        \sa mode(), analysis() */
  const static int AnalysisMode = 4;
    /*! Do nothing, i.e. wait for a selection from the user.
        \sa mode(), idle() */
  const static int IdleMode = 0;
    /*! True if the current working mode is to
        acquiring real data from a data acquisition board. 
	\sa mode(), simulation(), analysis(), idle() */
  bool acquisition( void ) const;
    /*! True if the current working mode is to
        simulate data using a Model. 
	\sa mode(), aquisition(), analysis(), idle() */
  bool simulation( void ) const;
    /*! True if the current working mode is to
        reanalyse previously recorded or simulated data. 
	\sa mode(), acquisition(), simulation(), idle() */
  bool analysis( void ) const;
    /*! True if the current working mode is to
        nothing, i.e. to wait for a selection from the user. 
	\sa mode(), acquisition(), simulation(), analysis() */
  bool idle( void ) const;
    /*! Set the mode to \a mode. */
  void setMode( int mode );

    /*! Activates the new gain settings for analog input
        set by the adjustGain() functions. */
  void activateGains( void );


public slots:

    /*! Starts the research program \a repro.*/
  void startRePro( RePro *repro, int macroaction, bool saving );
    /*! Stop the current RePro. */
  void stopRePro( void );
    /*! Updates indices for input data and events
        and call the readRePro functions of the Session and Control plugins.  */
  void updateRePro( void );

    /*! Starts the thread responsible for updating data.  */
  void startDataThread( void );
    /*! Contains the UpateDataThread loop, continuously updates and 
        processes data. */    
  void run( void );

    /*! Save settings to configuration files. */
  void saveConfig( void );

    /*! Stop all running threads. */
  void stopThreads( void );
    /*! Stop all activity and switch into idle mode. */
  void stopActivity( void );
    /*! Clear the state of stopped activity. */
  void clearActivity( void );
    /*! Start acquisition mode. */
  void startFirstAcquisition( void );
    /*! Clear activity and start acquisition mode. */
  void startAcquisition( void );
    /*! Start simulation mode. */
  void startFirstSimulation( void );
    /*! Clear activity and start simulation mode. */
  void startSimulation( void );
    /*! Start idle mode. */
  void startIdle( void );

    /*! Stops all RELACSWidget activities and exits. */
  void quit( void );

    /*! Toggles full screen mode. */
  void fullScreen( void );

    /*! Displays highly relevant information about the culprits that
        wrote this piece of software.*/
  void about( void );

    /*! Displays RELACS help. */
  void help( void );


signals:

    /*! Data signal for Plot and SaveFiles. */
  void data( const InList &data, const EventList &events );
    /*! Data signal for RePro. */
  void dataRePro( const InList &data, const EventList &events );
    /*! After a signal is written to the daq-board for output
        the function write( OData &OD ) emits this signal.
        It can be used to check the success of the output operation.
        For that purpose read out the error flags of \a OD. */
  //  void wroteData( const OData &OD );


protected:

  virtual void closeEvent( QCloseEvent* );
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual void customEvent( QCustomEvent *qce );


protected slots:

  void helpClosed( int r );
  void simLoadMessage( void );


private:

  friend class Settings;
  friend class MetaData;
  friend class RELACSPlugin;
  friend class Session;
  friend class Model;
  friend class RePro;
  friend class RePros;
  friend class PlotTrace;
  friend class Macros;
  friend class Control;
  friend class FilterDetectors;
  friend class SaveFiles;

  void checkPlugin( void *plugin, const string &ident );

  int setupHardware( int n );
  void closeHardware( void );
  void clearHardware( void );

  void setupInTraces( void );
  void setupOutTraces( void );

  int Mode;
  static const string ModeStr[5];

  // Internal classes
  QWidget *MainWidget;

  ConfigureClasses CFG;
  Plugins *PG;

  Settings SS;
  MetaData MTDT;
  SetupData Setup;

  AllDevices *ADV;
  Devices *DV;
  AIDevices *AID;
  AODevices *AOD;
  AttDevices *ATD;
  AttInterfaces *ATI;

  Acquire *AQ;
  Acquire *AQD;
  Simulator *SIM;
  Model *MD;
  PlotTrace *PT;
  SaveFiles *FW;
  RePros *RP;
  Macros *MC;
  FilterDetectors *FD;
  Session *SN;
  vector<Control*> CN;

  InList IL;
  EventList ED;
  vector<PlotTrace::TraceStyle> TraceStyles;
  vector<PlotTrace::EventStyle> EventStyles;

  // Research Program = RePros
  RePro *CurrentRePro;      // always the current program
  bool ReProRunning;

  ofstream *LogFile;
  ofstream *InfoFile;
  string InfoFileMacro;
  QColor OrgBackground;
  bool IsFullScreen;

  QTimer SimLoad;
  QLabel *SimLabel;

    /*! Take care of the qApp->lock() */
  int GUILock;

    /*! Controls the data reading thread. */
  bool RunData;
  QMutex RunDataMutex;
  QMutex DataMutex;
  int DataMutexCount;

  // Synchronization of Session and Control threads:
  QWaitCondition DataSleepWait;
  QWaitCondition ReProSleepWait;
  QWaitCondition ReProAfterWait;
  QWaitCondition SessionStartWait;
  QWaitCondition SessionStopWait;
  QWaitCondition SessionPrestopWait;

  QTime ReProTime;

  QAction *AcquisitionAction;
  QAction *SimulationAction;
  QAction *IdleAction;

  QAction *FullscreenAction;

  QPopupMenu *DeviceMenu;

  bool Help;

  class KeyTimeOut *KeyTime;

};


class KeyTimeOut : public QObject
{
  Q_OBJECT

public:

  KeyTimeOut( QWidget *tlw );
  virtual ~KeyTimeOut( void );


protected:

  virtual bool eventFilter( QObject *o, QEvent *e );
  virtual void timerEvent( QTimerEvent *e );


private:

  QWidget *TLW;

};


#endif

