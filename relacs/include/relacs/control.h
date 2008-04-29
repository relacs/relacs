/*
  control.h
  Base class for widgets that control hardware or analyze data.

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

#ifndef _RELACS_CONTROL_H_
#define _RELACS_CONTROL_H_ 1

#include <qglobal.h>
#if QT_VERSION >= 0x030301
#define USEWAITCONDITION
#endif
#include <string>
#include <values.h>
#include <qdatetime.h>
#include <qpopupmenu.h>
#include <qthread.h>
#ifdef USEWAITCONDITION
#include <qwaitcondition.h>
#endif
#include "relacsplugin.h"
using namespace std;


/*! 
\class Control
\brief Base class for widgets that control hardware or analyze data.
\author Jan Benda
\version 2.0

A %Control widget can be used to control some hardware or analyze data
independently and in parallel to the RePros.
Use it to implement things that do not fit in the RePro,
Session, or EventDetector concepts.

A %Control widget has a name() and a title(),
the last version() was written by author() on date().
This information is set either by the constructor or by
setName(), setTitle(), setAuthor(), setVersion(), and setDate().

The Control is initialized by reimplementing initialize(), config(),
and initDevices().

For continuosly doing something use the Control thread by
reimplementing main().
For synchronization with data, repros or sessions use waitOnData(),
waitOnReProSleep(), waitOnReProFinished(), waitOnSessionStart(),
waitOnSessionPrestop(), waitOnSessionStop().
Simply sleeping is possible with sleep(), timeStamp(), and sleepOn().
Within main() access to the Control and common data is locked (via lockAllData()),
during the waitOn*(), sleep(), and sleepOn() functions
access is unlocked() (via unlockAllData()).
You need to return from the thread if interrupt() returns \c true.
If you want the thread to be stopped in a different way,
reimplement requestStop().
Use lockGUI() and unlockGUI() to encapsulate code that directly or
indirectly draws something on the screen.

Reimplement keyPressEvent() and/or keyReleaseEvent() for keybord-based control.

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


class Control : public RELACSPlugin, protected QThread
{
  Q_OBJECT


public:

    /*! Construct a control widget with name \a name (has to be identical
        to the class name) and widget title \a title that
	belongs to the set of plugins named \a pluginset.
	The implementation of a class derived from Control
	has the version \a version and was written
	by \a author on \a date. */
  Control( const string &name, 
	   const string &title="",
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

    /*! This function is called BEFORE
        the configuration parameters are read in.
        Implement this function to do some initialization. */
  virtual void initialize( void );
    /*! This function is called AFTER the configuration parameters
        are read in and all the devices are initialized..
        Implement this function to check for devices. */
  virtual void initDevices( void );

    /*! Start execution of the %Control thread. 
        Reimplement main() to make the thread do something. */
  void start( void );
    /*! Kindly ask the Control thread to stop. 
        Blocks until the thread has terminated. 
        This implementation sets interrupt() to \c true,
        wakes up all wait-condidions and sleeps,
        and finally waits for the thread to terminate. */
  virtual void requestStop( void );

    /*! Add specific actions to the RELACS File-menu.
        This implementation adds a "Dialog" and a "Help" menu entry. */
  virtual void addActions( QPopupMenu *menu );

    /*! You may reimplement this function to handle key-press events. */
  virtual void keyPressEvent( QKeyEvent *e );
    /*! You may reimplement this function to handle key-release events. */
  virtual void keyReleaseEvent( QKeyEvent *e );


protected:

    /*! Reimplement this function if you want to analyze input data traces
        and events. 
        This function is executed as a thread.
	Within this thread the Control, all input data, events, meta data, 
	and stimulus data are already locked (via lockAll()).
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

    /*! This is the thread. It calls main(). */
  virtual void run( void );

  bool Interrupt;
  mutable QMutex InterruptLock;
#ifdef USEWAITCONDITION
  QWaitCondition SleepWait;
#endif
  QTime SleepTime;

};


#endif /* ! _RELACS_CONTROL_H_ */
