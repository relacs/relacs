/*
  control.h
  Base class for widgets that control hardware or analyze data.

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

#ifndef _RELACS_CONTROL_H_
#define _RELACS_CONTROL_H_ 1

#include <string>
#include <values.h>
#include <QDateTime>
#include <QMenu>
#include <QThread>
#include <QWaitCondition>
#include <relacs/relacsplugin.h>
using namespace std;

namespace relacs {


class ControlThread;


/*! 
\class Control
\brief Base class for widgets that control hardware or analyze data.
\author Jan Benda

A %Control widget can be used to control some hardware or analyze data
independently and in parallel to the RePros.
Use it to implement things that do not fit in the RePro,
Session, or EventDetector concepts.

A %Control widget has a name(),
the last version() was written by author() on date().
This information is set either by the constructor or by
setName(), setAuthor(), setVersion(), and setDate().

The Control is initialized by reimplementing preConfig(), config(),
and initDevices().

For continuosly doing something use the Control thread by
reimplementing main().
For synchronization with data, repros or sessions use waitOnData(),
waitOnReProSleep(), waitOnReProFinished(), waitOnSessionStart(),
waitOnSessionPrestop(), waitOnSessionStop().
Simply sleeping is possible with sleep(), timeStamp(), and sleepOn().
Within main() access to the Control is locked (via lock()),
during the waitOn*(), sleep(), and sleepOn() functions
access is unlocked (via unlock() ).
You need to return from the thread if interrupt() returns \c true.
If you want the thread to be stopped in a different way,
reimplement requestStop().
Use lockGUI() and unlockGUI() to encapsulate code that directly or
indirectly draws something on the screen.

A separate window displaying a warning message can be opened with
warning(), a window displaying some information with info().

You are able to set the input gain for the data acquisition with
setGain(), adjustGain(), adjust(), and activateGains().
Beware that a RePro may also set the gains.

addPath() prepends the current directory for data storage to a filename.

Access to other RELACS plugins is provided by devices(), device(),
manipulator(), attenuator(), detector(), detectorOpts(), session(),
sessionOpts(), sessionData(), sessionInfo(), sessionTime(),
sessionTimeStr(), sessionRunning(), control(), controlOpts(),
repro(), reproOpts(), currentRePro(), currentReProOpts().

You can define some Options (inherited via Config) that are stored
in the relacsplugins.cfg file and that can be edited with the Control
dialog(). You can select a subset of the Options for the dialog
by defining a dialogSelectMask() with setDialogSelectMask() 
and addDialogSelectMask().
Likewise, some Options can be marked as read-only with
dialogReadOnlyMask(), setDialogReadOnlyMask(), and addDialogReadOnlyMask().
The appearance of the dialog can be controlled with dialogStyle(),
setDialogStyle(), and addDialogStyle().
dialogOpen() returns \c true if the dialog is already displayed.
*/


class Control : public RELACSPlugin
{
  Q_OBJECT


public:

    /*! Construct a control widget with name \a name (has to be identical
        to the class name) that
	belongs to the set of plugins named \a pluginset.
	The implementation of a class derived from Control
	has the version \a version and was written
	by \a author on \a date. */
  Control( const string &name, 
	   const string &pluginset="",
	   const string &author="unknown",
	   const string &version="unknown",
	   const string &date=__DATE__ );
    /*! Deconstruct the Control. */
  virtual ~Control( void );

    /*! Set the name of this %Control to \a name
        and its identifier for the configuration file to
        "Control: \a name". */
  virtual void setName( const string &name );

    /*! This function is called AFTER the configuration parameters
        are read in and all the devices are initialized.
        Implement this function to check for devices. */
  virtual void initDevices( void );
    /*! This function is called BEFORE devices are destroyed, i.e. at
        a restart. Implement this function to unset any pointers to
        devices. */
  virtual void clearDevices( void );

    /*! Start execution of the %Control thread. 
        Reimplement main() to make the thread do something. */
  void start( void );
    /*! Kindly ask the Control thread to stop by setting interrupt()
        to \c true and interrupt a possible sleep(). */
  virtual void requestStop( void );
    /*! Wait for the thread to finish execution (\a time < 0, returns \c true) 
        or for \a time seconds to be elapsed (returns \c false). */
  bool wait( double time=-1.0 );

    /*! Add specific actions to the RELACS File-menu.
        This implementation adds a "Dialog" and a "Help" menu entry. */
  virtual void addActions( QMenu *menu, bool doxydoc );


protected:

    /*! Reimplement this function if you want to analyze input data traces
        and events. 
        This function is executed as a thread.
	Within this thread the Control is locked (via lock() ).
	Access to those data is unlocked during sleep(), sleepOn(),
	and the waitOn* functions.
        To synchronize with the data, repros, or session use waitOnData(),
        waitOnReProSleep(), waitOnReProFinished(), waitOnSessionStart(),
	waitOnSessionPrestop(), waitOnSessionStop().
	You need to return from main() if the waitOn*(), sleep(),
	or interrupt() functions return \c true.
        \sa interrupt(), sleep(), timeStamp(), sleepOn() */
  virtual void main( void );

    /*! Returns \c true if the thread should be stopped.
        Use this from within main()
        \code
         void main( void )
	 {
	 ...
	 if ( interrupt() )
	   return;
	 ...
	 }
        \endcode
        */
  bool interrupt( void ) const;

    /*! Wait until new data are available or \a time seconds have been elapsed.
        Returns \a true if the thread needs to be stopped.
	\code
         void main( void )
	 {
	 ...
         while ( ! waitOnData() ) {
	   analyzeData( data, events );
	 }
	 ...
	 }
	\endcode
	\sa main(), interrupt(), waitOnReProSleep(), waitOnReProFinished(),
        waitOnSessionStart(), waitOnSessionPrestop(), waitOnSessionStop(),
	sleep(), timeStamp(), sleepOn() */
  bool waitOnData( double time=MAXDOUBLE );
    /*! Wait until the current RePro goes to sleep or \a time seconds have been elapsed.
        Returns \a true if the thread needs to be stopped.
	\sa main(), interrupt(), waitOnData(), waitOnReProFinished(),
        waitOnSessionStart(), waitOnSessionPrestop(), waitOnSessionStop(),
	sleep(), timeStamp(), sleepOn(),
        currentRePro(), currentReProOpts() */
  bool waitOnReProSleep( double time=MAXDOUBLE );
    /*! Wait until the current RePro has finished or \a time seconds have been elapsed.
        Returns \a true if the thread needs to be stopped.
	\sa main(), interrupt(), waitOnData(), waitOnReProSleep(),
        waitOnSessionStart(), waitOnSessionPrestop(), waitOnSessionStop(),
	sleep(), timeStamp(), sleepOn(),
        currentRePro(), currentReProOpts() */
  bool waitOnReProFinished( double time=MAXDOUBLE );
    /*! Wait until a new session starts or \a time seconds have been elapsed.
        Returns \a true if the thread needs to be stopped.
	\sa main(), interrupt(), waitOnData(), waitOnReProSleep(),
        waitOnSessionPrestop(), waitOnSessionStop(),
	sleep(), timeStamp(), sleepOn() */
  bool waitOnSessionStart( double time=MAXDOUBLE );
    /*! Wait until a session might be stopped or \a time seconds have been elapsed.
        Returns \a true if the thread needs to be stopped.
	\sa main(), interrupt(), waitOnData(), waitOnReProSleep(),
        waitOnSessionStart(), waitOnSessionStop(),
	sleep(), timeStamp(), sleepOn() */
  bool waitOnSessionPrestop( double time=MAXDOUBLE );
    /*! Wait until a session is stopped or \a time seconds have been elapsed.
        Returns \a true if the thread needs to be stopped.
	\sa main(), interrupt(), waitOnData(), waitOnReProSleep(),
        waitOnSessionStart(), waitOnSessionPrestop(),
	sleep(), timeStamp(), sleepOn() */
  bool waitOnSessionStop( double time=MAXDOUBLE );

    /*! Sleep for \a t seconds.
        Returns \a true if the thread needs to be stopped.
        \sa main(), sleepOn(), timeStamp() */
  bool sleep( double t );
    /*! Memorize the current time. 
        This time is used by sleepOn() to calculate the remaining
	time to sleep. 
        \sa main(), sleepOn(), sleep() */
  void timeStamp( void );
    /*! Sleep for \a t seconds relative to the last call of timeStamp(). 
        Returns \a true if the thread needs to be stopped.
	\code
	  timeStamp();     // memorize current time
          doSomething();   // this takes a while
	  sleepOn( 0.5 );  // sleep for the remaining time to 500ms.
	\endcode
        \sa main(), sleep(), timeStamp() */
  bool sleepOn( double t );


private:

  friend class ControlThread;

  void run( void );

  ControlThread *Thread;
  bool Interrupt;
  mutable QMutex InterruptLock;
  QWaitCondition SleepWait;
  QTime SleepTime;

};


class ControlThread : public QThread
{

public:
  
  ControlThread( Control *c );
  virtual void run( void );
  void usleep( unsigned long usecs );


private:

  Control *C;

};


}; /* namespace relacs */

#endif /* ! _RELACS_CONTROL_H_ */

