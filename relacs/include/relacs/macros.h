/*
  macros.h
  A single Macro

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

#ifndef _MACROS_H_
#define _MACROS_H_

#include <iostream>
#include <string>
#include <vector>
#include <qobject.h>
#include <qwidget.h>
#include <qpopupmenu.h> 
#include <qlayout.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#include <qaccel.h>
#include <relacs/str.h>
#include <relacs/config.h>
using namespace std;

class RELACSWidget;
class RePro;
class RePros;
class Macros;
class MacroCommand;

/*!
  \class Macro
  \brief A single Macro
*/

class Macro : public QObject
{
  Q_OBJECT

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

  Macro( void ) : Name( "" ), Action( 0 ), 
    Button( true ), Menu( true ), Key( true ),
    Keep( false ), Overwrite( false ),
    PushButton( 0 ), AccelId( -1 ), PMenu( 0 ), PMenuId( -1 ), MacroNum( -1 ),
    MC( 0 ), Commands(), DialogOpen( false ) {};
  Macro( Str name, Macros *mc );
  Macro( const Macro &macro ) 
    : Name( macro.Name ), 
    Variables( macro.Variables ),
    Action( macro.Action ), 
    Button( macro.Button ), Menu( macro.Menu ), Key( macro.Key ),
    Keep( false ), Overwrite( false ),
    PushButton( macro.PushButton ), AccelId( macro.AccelId ), 
    PMenu( macro.PMenu ), PMenuId( macro.PMenuId ), MacroNum( macro.MacroNum ),
    MC( macro.MC ), Commands( macro.Commands ),
    DialogOpen( macro.DialogOpen ) {};

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

    /*! Opens a macro dialog. */
  void dialog( Macros *macros );

    /*! Add parameter from \a param to the macros variables. */
  void addParams( const Str &param );
    /*! Replaces macro variables in \a params by their value. */
  string expandParams( const Str &params ) const;

      /*! The name of the Macro. */
  string Name;
    /*! Macro variables */
  Options Variables;

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
  QPushButton *PushButton;
  int AccelId;
    /*! The popup menu of the Macro . */
  QPopupMenu *PMenu;
  int PMenuId;
  int MacroNum;
    /*! Pointer to Macros. */
  Macros *MC;

    /*! The list of commands associated with the Macro. */
  vector< MacroCommand* > Commands;


public slots:

  void acceptDialog( void );
  void dialogAction( int r );
  void dialogClosed( int r );


private:

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

class Macros : public QWidget, public Config
{
  Q_OBJECT

  friend class RELACSWidget;
  friend class Macro;


private:

  struct MacroPos;


public:

    /*! Maximum number of characters for menu entry. */
  static const int MenuWidth = 40;

  Macros( RELACSWidget *rw, QWidget *parent=0, const char *name=0 );
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
    /*! Create the button aray for the macros. */
  void buttons( void );
    /*! The popup menu starting and configuring Macros. */
  QPopupMenu* menu( void );

    /*! Stops the currently running repro.
        Then executes commands of the current macro until the next repro.
	Finally starts that repro. */
  void startNextRePro( bool saving );

    /*! Emits stopRePro(), then 
        starts command number \a command of macro number \a macro
	and further commands until the next repro.
        Emits startRePro() and returns a pointer to the running repro. */
  void startMacro( int macro, int command=0, bool saving=true, 
		   vector<MacroPos> *newstack=0 );
    /*! Starts the startup Macro. */
  void startUp( bool saving=true ) { startMacro( StartUpIndex ); };
    /*! Starts the shutdown Macro. */
  void shutDown( bool saving=true ) { startMacro( ShutDownIndex ); };
    /*! Starts the fallback Macro. */
  void fallBack( bool saving=true ) { startMacro( FallBackIndex ); };
    /*! Starts the startsession Macro. */
  void startSession( bool saving=true ) { startMacro( StartSessionIndex ); };
    /*! Starts the stopsession Macro. */
  void stopSession( bool saving=true ) { startMacro( StopSessionIndex ); };

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
  const string &macro( void ) const { return MCs[CurrentMacro]->Name; };
    /*! Returns the options of the current RePro. */
  string options( void ) const;
    /*! Returns the macro variables of macro \a macro. */
  Options &variables( int macro );
    /*! Returns the macro variables of macro \a macro as a string. */
  string variablesStr( int macro );
    /*! Replaces variables of macro \a macro in \a params by their value. */
  string expandParams( int macro, const Str &params ) const;

  virtual void saveConfig( ofstream &str );

  void setRePros( RePros *repros ) { RP = repros; };
  
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
    /*! Load macros from file with id \a id,
        check them and create buttons and menu. */
  void switchMacro( int id );
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
    /*! Stops the currently running repro and starts the macro \a number. */
  void launch( int number );
    /*! Open the popup menu right above the macro's button. */
  void popup( int number );
    /*! Inform macros that a repro is started that is not part of a macro. */
  void noMacro( RePro *repro );

  
signals:

    /*! Stop the currently running repro. */
  void stopRePro( void );
    /*! Start \a repro. The macro from which the repro is started has
        \a macroaction - actions set. */
  void startRePro( RePro *repro, int macroaction, bool saving );


private:

  void clearButton( void );
  void runButton( void );
  void stackButton( void );
  void stackButtons( void );
  void clearStackButtons( void );

  RELACSWidget *RW;
  RePros *RP;

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
    MacroPos( void )
      : MacroID( -1 ), CommandID( -1 ), MacroVariables() {};
    MacroPos( int macro, int command, Options &var )
      : MacroID( macro ), CommandID( command ),
        MacroVariables( var ) {};
    MacroPos( const MacroPos &mp )
      : MacroID( mp.MacroID ), CommandID( mp.CommandID ), 
	MacroVariables( mp.MacroVariables ) {};
    void set( int macro, int command, Options &var ) 
      { MacroID = macro; CommandID = command; MacroVariables = var; };
    void clear( void ) 
      { MacroID = -1; CommandID = -1; MacroVariables.clear(); };
    bool defined( void ) { return ( MacroID >= 0 && CommandID >= 0 ); };
    int MacroID;
    int CommandID;
    Options MacroVariables;
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
    /*! Overide disabled commands. */
  bool Enable;

  Str Warnings;

  int StartUpIndex;
  int ShutDownIndex;
  int FallBackIndex;
  int StartSessionIndex;
  int StopSessionIndex;

  QPixmap BaseIcon;
  QPixmap StackIcon;
  QPixmap RunningIcon;
  QPixmap IdleIcon;
  QPixmap SessionIcon;
  QPixmap EnabledIcon;
  QPixmap DisabledIcon;

  string MacroFile;

  QPopupMenu *Menu;
  QPopupMenu *SwitchMenu;
  QGridLayout *ButtonLayout;
  QAccel *ButtonMenuKeys;

  bool Fatal;

private slots:

    /*! This function is called from the macros menu. 
        Depending on \id it starts a macro via launch( int ),
        starts a specific repro within a macro,
        opens the repros options dialog or help window,
        reloads the repro or views the repro. */
  void select( int id );


private:

  int dialog( RePro *repro, const string &options, Options &co );
  void createIcons( void );

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

public:

  MacroCommand( void );
  MacroCommand( const string &name, const string &params, 
		bool enabled, int macro, bool filter, bool detector, bool switchm,
		bool startsession, bool shell, bool mes, double to,
		bool browse, Macros *mc );
  MacroCommand( RePro *repro, const string &params, Macros *mc );
  MacroCommand( const MacroCommand &com );
  void dialog( int macro, int command, Options &dopt );

    /*! The name of the requested repro, macro, or shell script. */
  string Name;
    /*! Parameters for the RePro. */
  Str Params;
    /*! The RePro (if it is a repro.). */
  RePro *RP;
    /*! The current options for this RePro that differ from the default. */
  Options CO;
    /*! The additional dialog options from RePros. */
  Options *DO;
    /*! If the command refers to a macro (\a Macro >= 0)
        this is the index of the macro. */
  int Macro;
    /*! True if the command refers to a filter. */
  bool Filter;
    /*! True if the command refers to an event detector. */
  bool Detector;
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
    /*! The index of the macro this command belongs to. */
  int MacroNum;
    /*! The index of this command withing its Macro. */
  int CommandNum;
    /*! Pointer to Macros. */
  Macros *MC;
    /*! True if dialog for this command is open. */
  bool DialogOpen;
    /*! Options for a Macro-variable dialog. */
  Options MacroVars;
    /*! The shortcut key for the menu entry. */
  string MenuShortcut;
  string MenuText;
  QPopupMenu *Menu;
  int MenuId;
  QPopupMenu *SubMenu;

public slots:

  void acceptDialog( void );
  void dialogAction( int r );
  void dialogClosed( int r );

};


/*! 
  \class MacroButton
  \author Christian Machens, Jan Benda
  \version 1.1
  \brief Gives a push button a number.
  \note Cannot be private to Macros, since this doesn't work
  with the signals and other QObject components.
*/
  
class MacroButton : public QPushButton
{
  Q_OBJECT
    
public:
    
  MacroButton( int number, const string &title,
	       QWidget *parent = 0, const char *name = 0 );
    
signals:
    
  void leftClicked( int number );
  void rightClicked( int number );
    
    
protected:
    
  void mouseReleaseEvent( QMouseEvent *qme );  
    
    
private:
    
  int Number;
    
private slots:

  void wasLeftClicked( void );
  void wasRightClicked( void );
    
};


#endif
