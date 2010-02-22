/*
  session.h
  Controls a recording session.

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

#ifndef _RELACS_SESSION_H_
#define _RELACS_SESSION_H_ 1

#include <ctime>
#include <string>
#include <qhbox.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <qtimer.h>
#include <qlabel.h>
#include <qpopupmenu.h>
#include <qaction.h>
#include <relacs/options.h>

namespace relacs {


class Session;


/*! 
\class Session
\brief Controls a recording session
\author Jan Benda
\version 2.0

A recording session is started by pressing the \c Enter or \a Return key, 
from the RELACS menu, or clicking a button,
that a Control implementation might provide.

During a session, running() returns \a true, sessions() returns the 
number of so far executed sessions, startSessionTime() returns
the absolute time the session was started, sessionTime() 
and sessionTimeStr() return the time the current session is running.
totalTime() returns the time RELACS is running.

Session sets the following environment variables
that can be used from within a shell command:

- \c RELACSSESSIONTIME : The elapsed time of the current session formatted as a string.
- \c RELACSSESSIONSECONDS : The elapsed time of the current session in seconds.
- \c RELACSSESSIONSAVED : Equals "1" if the session gets saved.

*/

class RELACSWidget;

class Session : public QHBox, public Options
{
  Q_OBJECT

public:

    /*! Construct a session. */
  Session( RELACSWidget *rw, int height,
	   QWidget *parent, const char *name );
    /*! Deconstruct the Session. */
  ~Session( void );

    /*! Returns \c true if a session is currently runnig,
        returns \c false otherwise.*/
  bool running( void ) const;
    /*! The number of so far executed sessions including the running one. */
  int sessions( void ) const;
    /*! The time the session was started. */
  time_t startSessionTime( void ) const;
    /*! The elapsed time of the current session in seconds. */
  double sessionTime( void ) const;
    /*! A string displaying the elapsed time of the current session
        as specified by \a SessionTimeFormat. */
  string sessionTimeStr( void ) const;
    /*! The elapsed time of the total program in seconds. */
  double totalTime( void ) const;

    /*! The number of so far executed RePros of the running session. */
  int reproCount( void ) const;
    /*! Increment the RePro counter. */
  void incrReProCount( void );

    /*! Add actions to the menu. */
  virtual void addActions( QPopupMenu *menu );

    /*! Returns \c true if the data currently recorded should
        be saved on disc, returns false otherwise. */
  bool saveData( void ) const;
    /*! Set the flag indicating whether data of this session should be saved
        to \a save. */
  void setSaveData( bool save );

    /*! True if a dialog is started on stopping a session. */
  bool saveDialog( void ) const;
    /*! Set whether a dialog should be started on stopping a session
        (\a dialog = true) or not (dialog=false).
        CAUTION: If set to false, all data of this session get lost! */
  void setSaveDialog( bool dialog=true );

    /*! Called whenever the mode is changed. */
  virtual void modeChanged( void );


public slots:

    /*! Start a new session, if it is not already running.
        If \a startmacro is \c true the startsession - macro is called.
	\note This function must not be called from a non GUI thread! */
  void startTheSession( bool startmacro );
    /*! Start a new session, if it is not already running,
        and calls the startsession - macro.
	\note This function must not be called from a non GUI thread! */
  void startTheSession( void );
    /*! Stop a running session.
	\note This function must not be called from a non GUI thread! */
  void stopTheSession( void );
    /*! Toggles the status of the session:
        stops a running session or start a new session
	if no session is running.
	\note This function must not be called from a non GUI thread! */
  void toggleSession( void );

    /*! Doing some late initialization, i.e. setting fonts and palette. */
  void polish( void );


private:

    /*! True if data of this session should be saved. */
  bool SaveData;
    /*! If true (default), the user will be
        asked on stopTheSession() whether the data recorded should be saved.
	IF SET TO FALSE, ALL DATA IS LOST ON STOPPING A SESSION.
	Hence, this variable should be unset only when testing RELACS!!!. */
  bool SaveDialog;

    /*! True if a session is runnning. */
  bool Running;

    /*! Counts the sessions. */
  int SessionCounter;

    /*! Counts the repros within a session. */
  int ReProCounter;

    /*! The time the session was started. */
  time_t StartSessionTime;

    /*! Measures the elapsed time of the current session. */
  mutable QTime SessionTimer;
    /*! Stores the elapsed time of the current session. */
  mutable double SessionSeconds;
    /*! Measures the totally elapsed time since starting the program. */
  mutable QTime TotalTimer;

  QTimer *MessageTimer;

  QLabel *TimeLabel;

  QAction *StartSessionAction;

  RELACSWidget *RW;


private slots:

  void timeMessage( void );

};


}; /* namespace relacs */

#endif /* ! _RELACS_SESSION_H_ */

