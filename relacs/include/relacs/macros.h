/*
  macros.h
  A single Macro

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_MACROS_H_
#define _RELACS_MACROS_H_ 1

#include <iostream>
#include <string>
#include <vector>
#include <QObject>
#include <QWidget>
#include <QMenu> 
#include <QAction> 
#include <QLayout>
#include <QPushButton>
#include <QPixmap>
#include <QMouseEvent>
#include <relacs/str.h>
#include <relacs/configclass.h>
using namespace std;

namespace relacs {


class RELACSWidget;
class RePro;
class RePros;
class Macros;
class MacroCommand;
class MacroButton;

/*!
  \class Macro
  \brief A single Macro
*/

class Macro : public QObject
{
  Q_OBJECT

  friend class Macros;
  friend class MacroCommand;

public:

    /*! This Macro is the startup Macro. */
  static const int StartUp = 0x01;
    /*! This Macro is the shutdown Macro. */
  static const int ShutDown = 0x02;
    /*! This Macro is the fallback Macro. */
  static const int FallBack = 0x04;
    /*! This Macro explicitly requested to be the fallback Macro. */
  static const int ExplicitFallBack = 0x08;
    /*! This Macro is the startsession Macro. */
  static const int StartSession = 0x10;
    /*! This Macro is the stopsession Macro. */
  static const int StopSession = 0x20;
    /*! This is no Macro. */
  static const int NoMacro = 0x40;

  Macro( void );
  Macro( Str name, Macros *mc );
  Macro( const Macro &macro );

    /*! The name of the macro. */
  string name( void ) const;

    /*! True if this Macro requests to be the startup Macro. */
  bool startUp( void ) const { return ( Action & StartUp ); };
    /*! True if this Macro requests to be the shutdown Macro. */
  bool shutDown( void ) const { return ( Action & ShutDown ); };
    /*! True if this Macro requests to be the fallback Macro. */
  bool fallBack( void ) const { return ( Action & FallBack ); };
    /*! True if this Macro explicitly requests to be the fallback Macro. */
  bool explicitFallBack( void ) const { return ( Action & ExplicitFallBack ); };
    /*! True if this Macro requests to be the startsession Macro. */
  bool startSession( void ) const { return ( Action & StartSession ); };
    /*! True if this Macro requests to be the stopsession Macro. */
  bool stopSession( void ) const { return ( Action & StopSession ); };

    /*! Set the startup flag. */
  void setStartUp( void ) { Action |= StartUp; };
    /*! Set the shutdown flag. */
  void setShutDown( void ) { Action |= ShutDown; };
    /*! Set the fallback flag. */
  void setFallBack( void ) { Action |= FallBack; };
    /*! Set the explicit-fallback flag. */
  void setExplicitFallBack( void ) { Action |= ExplicitFallBack; };
    /*! Set the start-session flag. */
  void setStartSession( void ) { Action |= StartSession; };
    /*! Set the stop-session flag. */
  void setStopSession( void ) { Action |= StopSession; };

    /*! Delete the startup flag. */
  void noStartUp( void ) { Action &= ~StartUp; };
    /*! Delete the shutdown flag. */
  void noShutDown( void ) { Action &= ~ShutDown; };
    /*! Delete the fallback flag. */
  void noFallBack( void ) { Action &= ~FallBack; };
    /*! Delete the explicit-fallback flag. */
  void noExplicitFallBack( void ) { Action &= ~ExplicitFallBack; };
    /*! Delete the start-session flag. */
  void noStartSession( void ) { Action &= ~StartSession; };
    /*! Delete the stop-session flag. */
  void noStopSession( void ) { Action &= ~StopSession; };

    /*! Returns the macro variables. */
  Options &variables( void );
    /*! Returns the macro variables as a string. */
  string variablesStr( void ) const;
    /*! True if this Macro is not to be cleared. */
  bool keep( void ) const;

    /*! Add parameter from \a param to the macros variables. */
  void addParams( const Str &param );
    /*! Replaces macro variables in \a params by their value.
        The project options, "project" and "experiment" are deleted from
        the string and loaded into \a prjopt. */
  string expandParams( const Str &params, Options &prjopt ) const;

    /*! Add menu for the macro and its commands to \a menu
        using \a text as a label. */
  void addMenu( QMenu *menu, const string &text );
    /*! Forms a string for the menu consisting of the macro name
        and its variables. */
  string menuStr( void ) const;
    /*! Add button for the macro and its commands to \a menu. */
  void addButton( const string &keys );
    /*! The button of this macro. */
  MacroButton *button( void );
    /*! Clear menu and button. */
  void clear( void );
    /*! Update the macro for the reloaded repro \a repro. */
  void reloadRePro( RePro *repro );

  static void createIcons( int size );
  static void destroyIcons( void );
  void clearButton( void );
  void runButton( void );
  void stackButton( bool base );

  friend ostream &operator<< ( ostream &str, const Macro &macro );


public slots:

    /*! Opens a macro dialog. */
  void dialog( void );
  void acceptDialog( void );
  void dialogAction( int r );
  void dialogClosed( int r );
    /*! Runs this macro. */
  void run( void );

    /*! Stops the currently running repro and starts this macro. */
  void launch( void );
    /*! Open the popup menu right above the macro's button. */
  void popup( void );


private:

      /*! The name of the Macro. */
  string Name;
    /*! Macro variables */
  Options Variables;
    /*! Macro project/experiment identifiers */
  Options Project;

    /*! The key code launching this macro. */
  int KeyCode;
    /*! Defines whether this Macro requests to be the 
        startup, shutdown, fallback, startsession, or stopsession Macro.
        This is a bitfield which is evaluated with the 
	constants defined in Macros. 
	\sa Macros::StartUp, Macros::ShutDown, Macros::FallBack, 
	Macros::ExplicitFallBack,
	Macros::StartSession, Macros::StopSession */
  int Action;
    /*! True if this Macro is going to have a button. */
  bool Button;
    /*! True if this Macro is going to have an menu entry. */
  bool Menu;
    /*! True if this Macro is going to have a key. */
  bool Key;
    /*! True if this Macro is not cleared. */
  bool Keep;
    /*! True if this Macro should overwrite an exisitng macro. */
  bool Overwrite;
    /*! A pointer to the macro's button. */
  MacroButton *PushButton;
    /*! A pointer to the macro's menu action that might have a key shortcut
        for popping up the menu. */
  QAction *MenuAction;
    /*! A pointer to the action that executes the macro. */
  QAction *RunAction;
    /*! A pointer to the last action of the firt macro menu for
        positioning the popup menu. */
  QAction *BottomAction;
  int MacroNum;
    /*! Pointer to Macros. */
  Macros *MC;

    /*! The list of commands associated with the Macro. */
  vector< MacroCommand* > Commands;

  static QPixmap *BaseIcon;
  static QPixmap *StackIcon;
  static QPixmap *RunningIcon;
  static QPixmap *IdleIcon;
  static QPixmap *SessionIcon;

  const static string StartUpIdent;
  const static string ShutDownIdent;
  const static string FallBackIdent;
  const static string StartSessionIdent;
  const static string StopSessionIdent;
  const static string NoButtonIdent;
  const static string NoKeyIdent;
  const static string NoMenuIdent;
  const static string KeepIdent;
  const static string OverwriteIdent;

  bool DialogOpen;

};


/*! 
\class Macros
\brief Container handling Macros
\author Jan Benda
\version 1.3
\todo RePro setOptions() / checking options
\todo change Button layout in resize event
\todo load defaultrepro if it is requested in macros.cfg
\bug When switching to macros that contain more buttons, higher
  keyshortcuts (from the macros menu) are not recognized.

This class provides the macro buttons and the macros-menu.
*/

class Macros : public QWidget, public ConfigClass
{
  Q_OBJECT

  friend class RELACSWidget;
  friend class Macro;
  friend class MacroCommand;


private:

  struct MacroPos;


public:

    /*! Maximum number of characters for menu entry. */
  static const int MenuWidth = 40;

  Macros( RELACSWidget *rw, QWidget *parent=0 );
  ~Macros( void );

    /*! The number of macros. */
  int size( void ) const { return MCs.size(); };

    /*! Clear macros list.
        Keep macros marked with "keep" if \a keep is true (default). */
  void clear( bool keep=true );
    /*! Load Macro definitions from file \a macrofile. */
  void load( const string &macrofile="", bool main=false );
    /*! Check macros and initialize repros.
        Returns true, if no macro or no fallback RePro is specified. */
  bool check( void );
    /*! Returns true if check detected no macro or no fallback RePro. */
  bool fatal( void ) const { return Fatal; };
    /*! Displays warning messages from load and check if there are any. */
  void warning( void );
    /*! Create the button aray and the menu for the macros. */
  void create( void );
    /*! The menu starting and configuring Macros should be added to \a menu. */
  void setMenu( QMenu *menu );

    /*! Stops the currently running repro.
        Then executes commands of the current macro until the next repro.
	Finally starts that repro.
        \a saving toggles whether saving to files via SaveFiles
        is enabled.
        \a enable overrides disbaled macro commands. */
  void startNextRePro( bool saving, bool enable=false );

    /*! Emits stopRePro(), then 
        starts command number \a command of macro number \a macro
	and further commands until the next repro.
        Emits startRePro() and returns a pointer to the running repro.
        \a saving toggles whether saving to files via SaveFiles
        is enabled. */
  void startMacro( int macro, int command=0, bool saving=true, 
		   bool enable=false, vector<MacroPos> *newstack=0 );
    /*! Execute next macro command. */
  void executeMacro( int newmacro, const Str &params );
    /*! Starts the startup Macro. */
  void startUp( void ) { startMacro( StartUpIndex, 0, false ); };
    /*! Starts the shutdown Macro. */
  void shutDown( void ) { startMacro( ShutDownIndex, 0, false ); };
    /*! Starts the fallback Macro. */
  void fallBack( bool saving=true ) { startMacro( FallBackIndex, 0, saving ); };
    /*! Starts the startsession Macro. */
  void startSession( void ) { startMacro( StartSessionIndex ); };
    /*! Starts the stopsession Macro. */
  void stopSession( void ) { startMacro( StopSessionIndex, 0, false ); };

    /*! The index of the startup Macro. */
  int startUpIndex( void ) const { return StartUpIndex; };
    /*! The index of the shutdown Macro. */
  int shutDownIndex( void ) const { return ShutDownIndex; };
    /*! The index of the fallback Macro. */
  int fallBackIndex( void ) const { return FallBackIndex; };
    /*! The index of the startsession Macro. */
  int startSessionIndex( void ) const { return StartSessionIndex; };
    /*! The index of the stopsession Macro. */
  int stopSessionIndex( void ) const { return StopSessionIndex; };

   /*! True if the current Macro is the startup Macro. */
  bool isStartUp( void ) const { return ( CurrentMacro == StartUpIndex ); };
   /*! True if the current Macro is the shutdown Macro. */
  bool isShutDown( void ) const { return ( CurrentMacro == ShutDownIndex ); };
   /*! True if the current Macro is the fallback Macro. */
  bool isFallBack( void ) const { return ( CurrentMacro == FallBackIndex ); };
   /*! True if the current Macro is the startsession Macro. */
  bool isStartSession( void ) const { return ( CurrentMacro == StartSessionIndex ); };
   /*! True if the current Macro is the stopsession Macro. */
  bool isStopSession( void ) const { return ( CurrentMacro == StopSessionIndex ); };

    /*! The index of the macro with name \a macro. */
  int index( const string &macro ) const;
    /*! The name of the current Macro. */
  string macro( void ) const { return MCs[CurrentMacro]->name(); };
    /*! Returns the options of the current RePro. */
  string options( void ) const;
    /*! Returns the project variables of macro \a macro. */
  Options &project( int macro );

  virtual void saveConfig( ofstream &str );

  void setRePros( RePros *repros ) { RPs = repros; };
  
  friend ostream &operator<< ( ostream &str, const Macros &macros );


public slots:

    /*! Stops the currently running repro.
        Then executes commands of the current macro until the next repro.
	Finally starts that repro with \a svaing set to \c true. */
  void startNextRePro( void );
    /*! Load macros from \a file,
        check them and create buttons and menu. */
  void loadMacros( const string &file );
    /*! Open file dialog and load macros from selected file,
        check them and create buttons and menu. */
  void selectMacros( void );
    /*! Load macros from file associated with \a action,
        check them and create buttons and menu. */
  void switchMacro( QAction *action );
    /*! Reload the macros from the current macro file. */
  void reload( void );
    /*! Updates the macros if the repro \a name was reloaded. */
  void reloadRePro( const string &name );
    /*! Memorizes the currently running repro and macro. */
  void store( void );
    /*! Memorizes the currently running repro and macro and 
        starts the fallback macro after the current repro is finished.
        The currently running repro gets a request to stop ( RePro::setSoftStop() ). */
  void softBreak( void );
    /*! Memorizes the currently running repro and macro and 
        starts the fallback macro immediately. */
  void hardBreak( void );
    /*! Starts the previously memorized macro at the memorized repro. */
  void resume( void );
    /*! Starts the previously memorized
        macro at the repro following the memorized repro. */
  void resumeNext( void );
    /*! Inform macros that a repro is started that is not part of a macro. */
  void noMacro( RePro *repro );


private:

  void clearButton( void );
  void runButton( void );
  void stackButton( void );
  void stackButtons( void );
  void clearStackButtons( void );

  RELACSWidget *RW;
  RePros *RPs;

    /*! The list of all Macros. */
  typedef vector< Macro* > MacrosType;
  MacrosType MCs;

    /*! Index to the current running Macro. */
  int CurrentMacro;
    /*! Index to the current running RePro of the current Macro. */
  int CurrentCommand;

    /*! \struct MacroPos
        \brief Store a macro and a command index. 
    */
  struct MacroPos
  {
    MacroPos( void );
    MacroPos( int macro, int command, Options &var, Options &prj );
    MacroPos( const MacroPos &mp );
    void set( int macro, int command, Options &var, Options &prj );
    void clear( void );
    bool defined( void );
    int MacroID;
    int CommandID;
    Options MacroVariables;
    Options MacroProject;
  };
    /*! A stack of macro commands. */  
  vector< MacroPos > Stack;
    /*! Macro command for resume. */
  MacroPos ResumePos;
    /*! The stack for resume. */  
  vector< MacroPos > ResumeStack;
    /*! Memorize the ThisMacroOnly variable. */
  bool ResumeMacroOnly;

    /*! True if only a single Command of a Macro should be executed. */
  bool ThisCommandOnly;
    /*! True if only a single Macro should be executed. */
  bool ThisMacroOnly;

  Str Warnings;

  int StartUpIndex;
  int ShutDownIndex;
  int FallBackIndex;
  int StartSessionIndex;
  int StopSessionIndex;

  string MacroFile;

  QMenu *Menu;
  QMenu *SwitchMenu;
  vector< QAction* > SwitchActions;
  QAction *ResumeAction;
  QAction *ResumeNextAction;
  QGridLayout *ButtonLayout;

  bool Fatal;

};


/*! 
\class MacroCommand. 
\brief A single command of a macro.


This class is used in the private list of macros of the Macros class.
It describes the function (repro, message, shell, macro) of the command.
*/
class MacroCommand : public QObject
{
  Q_OBJECT

  friend class Macros;
  friend class Macro;

public:

  MacroCommand( void );
  MacroCommand( const string &name, const string &params, 
		bool enabled, int macro, bool filter, bool detector, bool switchm,
		bool startsession, bool shell, bool mes, double to,
		bool browse, Macros *mcs, Macro *mc );
  MacroCommand( RePro *repro, const string &params, Macros *mcs, Macro *mc );
  MacroCommand( const MacroCommand &com );

    /*! Adds the submenu for this command to \a menu. */
  void addMenu( QMenu *menu );
    /*! Execute the command. Returns \c true if a RePro was executed. */
  bool execute( bool saving );
    /*! Update the macro command for the reloaded repro \a repro. */
  void reloadRePro( RePro *repro );

  static void createIcons( int size );
  static void destroyIcons( void );

  friend ostream &operator<< ( ostream &str, const MacroCommand &command );


public slots:

    /*! Start macro at this command. */
  void start( void );
    /*! Run only this command. */
  void run( void );
    /*! Launch the dialog of the command. */
  void dialog( void );
    /*! View the RePro. */
  void view( void );
    /*! Reload the RePro plugin. */
  void reload( void );
    /*! Display the RePro's help text. */
  void help( void );
    /*! Enable/disable the command. */
  void enable( void );

  void acceptDialog( void );
  void dialogAction( int r );
  void dialogClosed( int r );


 private:

    /*! The name of the requested repro, macro, or shell script. */
  string Name;
    /*! Parameters for the RePro. */
  Str Params;
    /*! The RePro (if it is a repro.). */
  RePro *RP;
    /*! The current options for this RePro that differ from the default. */
  Options CO;
    /*! The project options for this RePro. */
  Options PO;
    /*! The additional dialog options from RePros. */
  Options *DO;
    /*! If the command refers to a macro (\a MacroIndex >= 0)
        this is the index of the macro. */
  int MacroIndex;
    /*! > 0 if the command refers to a filter.
       1: save, 2: autoConfigure */
  int FilterCom;
    /*! > 0 if the command refers to an event detector.
       1: save, 2: autoConfigure */
  int DetectorCom;
    /*! Time for auto-configuring a filter or detector. */
  double AutoConfigureTime;
    /*! True if the command requests to switch the macros file. */
  bool Switch;
    /*! True if the command starts a session. */
  bool StartSession;
    /*! True if the command refers to a shell script. */
  bool Shell;
    /*! True if the command refers to a message. */
  bool Message;
    /*! Timeout for a message in seconds. */
  double TimeOut;
    /*! True if the command refers to a browsing a file. */
  bool Browse;
    /*! True if this command is enabled. */
  bool Enabled;
    /*! The menu entry for enabling/disabling the command. */
  QAction *EnabledAction;
    /*! The index of the macro this command belongs to. */
  int MacroNum;
    /*! The index of this command withing its Macro. */
  int CommandNum;
    /*! Pointer to the parent Macro. */
  Macro *MC;
    /*! Pointer to Macros. */
  Macros *MCs;
    /*! True if dialog for this command is open. */
  bool DialogOpen;
    /*! Options for a Macro-variable dialog. */
  Options MacroVars;
    /*! Options for a Macro-project dialog. */
  Options MacroProject;
    /*! The shortcut key for the menu entry (e.g. "&2 "). */
  string MenuShortcut;
    /*! The menu offering various actions for the command. */
  QMenu *SubMenu;

  static QPixmap *EnabledIcon;
  static QPixmap *DisabledIcon;

};


/*! 
  \class MacroButton
  \author Christian Machens, Jan Benda
  \version 2.0
  \brief Adds a rightClock signal to a push button.
*/
  
class MacroButton : public QPushButton
{
  Q_OBJECT
    
public:
    
  MacroButton( const string &title, QWidget *parent = 0 );
    
signals:
    
  void rightClicked( void );
    
    
protected:
    
  void mouseReleaseEvent( QMouseEvent *qme );  
    
};


}; /* namespace relacs */

#endif /* ! _RELACS_MACROS_H_ */

