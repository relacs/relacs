/*
  relacswidget.h
  Coordinates RePros, session, input, output, plotting, and saving. 

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

#ifndef _RELACS_RELACSWIDGET_H_
#define _RELACS_RELACSWIDGET_H_ 1

#include <QMainWindow>
#include <QGridLayout>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include <QMutex>
#include <QReadWriteLock>
#include <QWaitCondition>
#include <QSplashScreen>
#include <QApplication>
#include <deque>
#include <relacs/strqueue.h>
#include <relacs/configclass.h>
#include <relacs/configureclasses.h>
#include <relacs/settings.h>
#include <relacs/metadata.h>
#include <relacs/datathreads.h>
#include <relacs/plottrace.h>
#include <relacs/relacsplugin.h>

using namespace std;

namespace relacs {


class UpdateThread;
class AllDevices;
class Devices;
class AIDevices;
class AODevices;
class DIODevices;
class TriggerDevices;
class AttDevices;
class AttInterfaces;
class Acquire;
class Simulator;
class Model;
class PlotTrace;
class SaveFiles;
class RePros;
class Macros;
class FilterDetectors;
class ControlTabs;
class Session;


/*!
\class RELACSWidget
\brief Coordinates RePros, session, input, output, plotting, and saving.
\author Jan Benda
\author Christian Machens
\version 1.8

\todo make sure startFirstAcquisition() and startFirstSimulation() are only called from the GUI thread
      (because of MessageBox calls).
\bug what about wroteData?
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


class RELACSWidget : public QMainWindow, public ConfigClass
{
  Q_OBJECT

public:

    /*! The different working modes. */
  enum ModeTypes {
      /*! Do nothing, i.e. wait for a selection from the user. */
    IdleMode = 0,
      /*! Acquiring real data from a data acquisition board. */
    AcquisitionMode = 1,
      /*! Simulate data using a Model. */
    SimulationMode = 2,
      /*! Browse previously recorded or simulated data. */
    BrowseMode = 3,
      /*! Reanalyse previously recorded or simulated data. */
    AnalysisMode = 4
  };

  RELACSWidget( const string &pluginrelative,
		const string &pluginhome,
		const string &pluginhelp,
		const string &coreconfigfiles,
		const string &pluginconfigfiles,
		const string &docpath,
		const string &cfgexamplespath,
		const string &iconpath,
		bool doxydoc,
		QSplashScreen *splash,
		ModeTypes mode,
		QWidget *parent=0 );
  ~RELACSWidget( void );

    /*! Start data aquisition and first RePro. */
  void init( void );

    /*! Write current time and \a message to stderr and into a log file. */
  void printlog( const string &message ) const;

    /*! Updates the InData buffers and calls the filter and event detectors.
        If \a mintracetime is greater than zero, updateData() blocks
        until data upto \a mintracetime are available.
        \return \c true if new data have been got, otherwise you should wait() 
        on the UpdateDataWait condition to make sure data are available for you. */
  bool updateData( double mintracetime=0.0 );
    /*! Write the data to files and plot them. */
  void processData( void );

    /*! Locks the mutex of the raw data traces and events. */
  void readLockBuffer( void ) { BufferMutex.lockForRead(); };
    /*! Locks the buffer mutex of the raw data traces and events. */
  void writeLockBuffer( void ) { BufferMutex.lockForWrite(); };
    /*! Unlocks the mutex of the raw data traces and events. */
  void unlockBuffer( void ) { BufferMutex.unlock(); };
    /*! Returns the mutex used for locking the raw data traces and events. */
  QReadWriteLock &bufferMutex( void ) { return BufferMutex; };

    /*! Locks the mutex of all the data traces and events. */
  void readLockData( void ) { DataMutex.lockForRead(); };
    /*! Locks the data mutex of all the data traces and events. */
  void writeLockData( void ) { DataMutex.lockForWrite(); };
    /*! Unlocks the mutex of all the data traces and events. */
  void unlockData( void ) { DataMutex.unlock(); };
    /*! Returns the mutex used for locking all the data traces and events. */
  QReadWriteLock &dataMutex( void ) { return DataMutex; };

    /*! Locks the mutex for analog input. */
  void lockAI( void ) { AIMutex.lock(); };
    /*! Unlocks the mutex for analog input. */
  void unlockAI( void ) { AIMutex.unlock(); };
    /*! Locks the mutex of output signals. */
  void lockSignals( void ) { SignalMutex.lock(); };
    /*! Unlocks the mutex of output signals. */
  void unlockSignals( void ) { SignalMutex.unlock(); };

    /*! Wakes up all waitconditions. */
  void wakeAll( void );

    /*! Output of signal \a signal.
        Passing infos to SaveFiles. */
  int write( OutData &signal, bool setsignaltime=true );
  int write( OutList &signal, bool setsignaltime=true );

    /*! Direct output of a single data value as specified by \a signal
        to the DAQ board.
        Passing infos to SaveFiles. */
  int directWrite( OutData &signal, bool setsignaltime=true );
    /*! Direct output of single data values as specified by \a signal
        to different channels of the DAQ board.
        Passing infos to SaveFiles. */
  int directWrite( OutList &signal, bool setsignaltime=true );

    /*! Stop all analog output activity. */
  int stopWrite( void );

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
    /*! Returns pointer to the session. */
  Session *session( void );

    /*! The name of a started macro. */
  void startedMacro( const string &ident, const string &param );

    /*! Returns pointer to the filters and detectors. */
  FilterDetectors *filterDetectors( void );

    /*! The current working mode.
        Can be AcquisitionMode, SimulationMode, BrowseMode, AnalysisMode, or IdleMode. 
        \sa acquisition(), simulation(), browsing(), analysis(), idle() */
  ModeTypes mode( void ) const;
    /*! Return a string describing the current working mode.
        \sa mode(), acquisition(), simulation(), brwosing(), analysis(), idle() */
  string modeStr( void ) const;
    /*! True if the current working mode is to
        acquiring real data from a data acquisition board. 
	\sa mode(), simulation(), analysis(), browsing(), idle() */
  bool acquisition( void ) const;
    /*! True if the current working mode is to simulate data using a Model. 
	\sa mode(), aquisition(), browsing(), analysis(), idle() */
  bool simulation( void ) const;
    /*! True if the current working mode is to
        browse previously recorded or simulated data. 
	\sa mode(), acquisition(), simulation(), analysis(), idle() */
  bool browsing( void ) const;
    /*! True if the current working mode is to
        reanalyse previously recorded or simulated data. 
	\sa mode(), acquisition(), simulation(), browsing(), idle() */
  bool analysis( void ) const;
    /*! True if the current working mode is to
        nothing, i.e. to wait for a selection from the user. 
	\sa mode(), acquisition(), simulation(), analysis() */
  bool idle( void ) const;
    /*! Set the mode to \a mode. */
  void setMode( ModeTypes mode );

    /*! Activates the new gain settings for analog input
        set by the adjustGain() functions.
        activateGains() assumes the data traces and events not to be locked
	and will write-lock the raw data. */
  void activateGains( void );


public slots:

    /*! Starts the research program \a repro.
        \a saving toggles whether saving to files via SaveFiles
        is enabled. */
  void startRePro( RePro *repro, int macroaction, bool saving );
    /*! Stop the current RePro. */
  void stopRePro( void );
    /*! Updates indices for input data and events
        and call the readRePro functions of the Session and Control plugins.  */
  void updateRePro( void );

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

    /*! Toggle showing filters and detectors in full window. */
  void showFilters( void );
    /*! Toggle showing traces in full window. */
  void showTraces( void );
    /*! Toggle showing controls in full window. */
  void showControls( void );
    /*! Toggle showing RePros in full window. */
  void showRePros( void );
    /*! Toggles full screen mode. */
  void fullScreen( void );
    /*! Toggles maximized screen mode. */
  void maximizeScreen( void );

    /*! Displays highly relevant information about the culprits that
        wrote this piece of software.*/
  void about( void );

    /*! Displays RELACS help. */
  void help( void );


    /*! After a signal is written to the daq-board for output
        the function write( OData &OD ) emits this signal.
        It can be used to check the success of the output operation.
        For that purpose read out the error flags of \a OD. */
  //  void wroteData( const OData &OD );


protected:

  virtual void closeEvent( QCloseEvent* );
  virtual void keyPressEvent( QKeyEvent* );
  virtual void keyReleaseEvent( QKeyEvent* );
  virtual void customEvent( QEvent *qce );


protected slots:

  void helpClosed( int r );
  void simLoadMessage( void );
  void channels( void );


private:

  friend class UpdateThread;
  friend class Settings;
  friend class MetaData;
  friend class MetaDataSection;
  friend class MetaDataRecordingSection;
  friend class ReadThread;
  friend class WriteThread;
  friend class RELACSPlugin;
  friend class Session;
  friend class Model;
  friend class Control;
  friend class ControlTabs;
  friend class RePro;
  friend class RePros;
  friend class PlotTrace;
  friend class Macros;
  friend class FilterDetectors;
  friend class SaveFiles;

  void checkPlugin( void *plugin, const string &ident );

  int openHardware( int n, int errorlevel );
  int setupHardware( int n );
  void closeHardware( void );
  void clearHardware( void );

  void setupInTraces( void );
  void setupOutTraces( void );

    /*! Contains the UpateDataThread loop, continuously updates and 
        processes data. */    
  void run( void );

  UpdateThread *Thread;

  ModeTypes Mode;
  static const string ModeStr[5];

  // Internal classes
  QWidget *MainWidget;
  QGridLayout *MainLayout;

  ConfigureClasses CFG;
  Str DocPath;
  StrQueue HelpPathes;
  Str IconPath;

  Settings SS;
  MetaData MTDT;

  AllDevices *ADV;
  Devices *DV;
  AIDevices *AID;
  AODevices *AOD;
  DIODevices *DIOD;
  TriggerDevices *TRIGD;
  AttDevices *ATD;
  AttInterfaces *ATI;

  Acquire *AQ;
  Acquire *AQD;
  Simulator *SIM;
  Model *MD;
  PlotTrace *PT;
  SaveFiles *SF;
  ControlTabs *CW;
  RePros *RP;
  Macros *MC;
  FilterDetectors *FD;
  Session *SN;

  int ShowFull;

  InList IL;
  EventList ED;
  deque<PlotTrace::TraceStyle> TraceStyles;
  deque<PlotTrace::EventStyle> EventStyles;

  double SignalTime;

  ReadThread ReadLoop;
  WriteThread WriteLoop;

  // Research Program = RePros
  RePro *CurrentRePro;      // always the current program
  bool ReProRunning;

  ofstream *LogFile;
  QColor OrgBackground;
  bool IsFullScreen;
  bool IsMaximized;

  QTimer SimLoad;
  QLabel *SimLabel;

    /*! Controls the data reading thread. */
  QReadWriteLock BufferMutex;
    /*! Controls updateData(). */
  QReadWriteLock DataMutex;
  QMutex AIMutex;
  QMutex SignalMutex;
  bool RunData;
  QMutex RunDataMutex;

    /*! wait for stimulus data to be written */
  QWaitCondition ThreadSleepWait;
  QWaitCondition ReadDataWait;

  // synchronization of Session and Control threads:
  QWaitCondition UpdateDataWait;
  QWaitCondition ProcessDataWait;
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
  QAction *MaximizedAction;

  QMenu *DeviceMenu;

  string AIErrorMsg;
  bool Help;

  bool HandlingEvent;
  class KeyTimeOut *KeyTime;

};


class UpdateThread : public QThread
{

public:
  
  UpdateThread( RELACSWidget *rw );
  virtual void run( void );
  void msleep( unsigned long msecs );


private:

  RELACSWidget *RW;

};


class KeyTimeOut : public QObject
{
  Q_OBJECT

public:

  KeyTimeOut( QWidget *tlw );
  virtual ~KeyTimeOut( void );
  void setNoFocusWidget( QWidget *w );
  void unsetNoFocusWidget( void );


protected:

  virtual bool eventFilter( QObject *o, QEvent *e );
  virtual void timerEvent( QTimerEvent *e );
  bool noFocusWidget( void ) const;


private:

  int TimerId;
  QWidget *TopLevelWidget;
  QWidget *NoFocusWidget;

};


}; /* namespace relacs */

#endif /* ! _RELACS_RELACSWIDGET_H_ */

