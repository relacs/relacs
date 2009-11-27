/*
  repros.h
  Container handling RePros

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

#ifndef _RELACS_REPROS_H_
#define _RELACS_REPROS_H_ 1

#include <QObject>
#include <QTabWidget>
#include <QMenu> 
#include <QLabel>
#include <vector>
#include <relacs/options.h>
#include <relacs/relacsplugin.h>

namespace relacs {


class RePro;
class ReProData;
class RELACSWidget;

/*! 
\class RePros
\author Jan Benda
\version 1.0
\brief Container handling RePros
*/


class RePros : public QTabWidget
{
  Q_OBJECT

public:

    /*! Initializes RePros and create all available repros. */ 
  RePros( RELACSWidget *rw=0, QWidget *parent=0 );
  ~RePros( void );

    /*! Displays the message \a msg of a RePro. */
  void message( const string &msg );
    /*! The display for RePro messages. */
  QLabel *display( QWidget *parent=0 );
    /*! Add the menu from which each RePro can be started and
        configured to \a menu. */
  void addMenu( QMenu *menu );

    /*! The index of the RePro with class name \a name. */
  int index( const string &name ) const;
    /*! The index of the RePro \a repro. */
  int index( const RePro *repro ) const;
    /*! Makes the repro specified by \a index visible. */
  void raise( int index );
    /*! Makes the repro \a repro visible. */
  void raise( RePro *repro );
    /*! Reloads the plugin of the repro \a repro. */
  void reload( RePro *repro );
    /*! Display the help text of the repro \a repro. */
  void help( RePro *repro );
    /*! The number of repros. */
  int size( void ) const { return RPs.size(); };
    /*! The RePro with index \a index. */
  RePro *repro( int index ) const;
    /*! The RePro with class name \a name. */
  RePro *repro( const string &name ) const;
    /*! The index of the RePro with name \a name. */
  int nameIndex( const string &name ) const;
    /*! The RePro with name \a name. */
  RePro *nameRepro( const string &name ) const;
    /*! \return The index of the currently active RePro. */
  int currentRePro( void ) const;

    /*! Makes \a repro the current RePro and raises its widget. */
  void activateRePro( RePro *repro, int macroaction=0 );
  void setMacro( const string &name, const string &param );
    /*! Return the name of the currently running macro.
        If the %RePro was called from the %RePro menu
        then "RePro" is returned. */
  string macroName( void ) const;
    /*! Return the parameters of the currently running macro.. */
  string macroParam( void ) const;

    /*! Calles modeChanged() of each RePro
        whenever the mode is changed. */
  void modeChanged( void );
    /*! Inform each RePro that some stimulus data have been changed. */
  void notifyStimulusData( void );
    /*! Inform each RePro that some meta data have been changed
        in MetaDataSection \a section. */
  void notifyMetaData( const string &section );
    /*! Inform each RePro that a new session is started. */
  void sessionStarted( void );
    /*! Inform each RePro that the session is stopped. */
  void sessionStopped( bool saved );

    /*! The additional options needed for RePro Dialogs. */
  Options &dialogOptions( void );

  friend ostream &operator<< ( ostream &str, const RePros &repros );


public slots:

    /*! Launches the options dialog of the current RePro. */
  void dialog( void );
    /*! Raise the widget of the previous RePro. */
  void raise( void );
    /*! Displays help for the current RePro. */
  void help( void );

signals:

    /*! Stop the currently running repro. */
  void stopRePro( void );
    /*! Start \a repro. \a macroaction is set to Macros::NoMacro. */
  void startRePro( RePro *repro, int macroaction, bool saving );
    /*! This is emitted right before startRePro() in order to indicate
        that the repro was not started from a macro. */
  void noMacro( RePro *repro );
    /*! The repro \a name was reloaded. */
  void reloadRePro( const string &name );


private slots:

  void customEvent( QEvent *qce );


private:
  
  typedef vector<ReProData*> ReProsList;
  ReProsList RPs;

  int CurrentRePro;
  int ActionRePro;

  string MacroName;
  string MacroParam;

  RePro *CurrentView;
  RePro *PreviousView;

  QLabel *Message;
  string MessageStr;

  Options DialogOpt;

  RELACSWidget *RW;

};


/*! 
\class ReProData
\author Jan Benda
\version 1.0
\brief Additional Data for a RePro used by RePros.
*/

class ReProData : public QObject
{
  Q_OBJECT

public:

  ReProData( const string &name, RePro *repro, Options &dopt,
	     RePros *rps, RELACSWidget *rw );

    /*! Adds the submenu for this RePro with inex \a inx to \a menu. */
  void addMenu( QMenu *menu, int inx );

    /*! \return the name of the RePro. */
  string name( void ) const;
    /*! \return a pointer to the RePro. */
  RePro *repro( void );

public slots:

  void start( void );
  void dialog( void );
  void acceptDialog( void );
  void dialogAction( int r );
  void dialogClosed( int r );
  void raise( void );
  void reload( void );
  void help( void );

signals:

    /*! Stop the currently running repro. */
  void stopRePro( void );
    /*! Start \a repro. */
  void startRePro( RePro *repro, int macroaction, bool saving );
    /*! This is emitted right before startRePro() in order to indicate
        that the repro was not started from a macro. */
  void noMacro( RePro *repro );
    /*! The repro \a name was reloaded. */
  void reloadRePro( const string &name );

 private:

  string Name;
  RePro *RP;
  Options CO;
  Options *DO;

  RePros *RPs;
  RELACSWidget *RW;

};


}; /* namespace relacs */

#endif /* ! _RELACS_REPROS_H_ */

