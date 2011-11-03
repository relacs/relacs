/*
  macros.h
  Macros execute RePros

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
#include <deque>
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


class Macro;
class MacroCommand;
class MacroButton;
class RePro;
class RePros;
class RELACSWidget;


/*! 
\class Macros
\brief Container handling Macros
\author Jan Benda
\todo RePro setOptions() / checking options
\todo change Button layout in resize event
\todo load defaultrepro if it is requested in macros.cfg
\bug When switching to macros that contain more buttons, higher
  keyshortcuts (from the macros menu) are not recognized.
\brief Macros execute RePros.


This class provides the macro buttons and the macros-menu.
*/

class Macros : public QWidget, public ConfigClass
{
  Q_OBJECT


private:

  struct MacroPos;


public:

    /*! Maximum number of characters for menu entry. */
  static const int MenuWidth = 40;

    /*! Constructs the Macros. */
  Macros( RELACSWidget *rw, QWidget *parent=0 );
    /*! Destructs the Macros. */
  ~Macros( void );

    /*! The index of the macro with name \a macro. */
  int index( const string &macro ) const;
    /*! The name of the current Macro. */
  string macro( void ) const;
    /*! Returns the options of the current RePro. */
  string options( void ) const;
    /*! Returns the project variables of macro \a macro. */
  Options &project( int macro );
    /*! Return the value for the identifier \a ident from the 
        last entry in the stack where it is defined. */
  string projectTextFromStack( const string &ident ) const;

    /*! The number of macros. */
  int size( void ) const;

    /*! Clear macros list.
        Keep macros marked with "keep" if \a keep is true (default). */
  void clear( bool keep=true );
    /*! Load Macro definitions from file.
        If \a macrofile is empty or \a main is \c true and no \a mainfile property
	is defined, use the \a file property of the Macros.
        \param[in] macrofile the path of a file containing macro definitions
	\param[in] main if \c true ignore \a macrofile and use
	the main macrofile (the \a mainfile property of the Macros). */
  void load( const string &macrofile="", bool main=false );
    /*! Check macros and initialize repros.
        Returns true, if no macro or no fallback RePro is specified. */
  bool check( void );
    /*! Returns true if check detected no macro or no fallback RePro. */
  bool fatal( void ) const;
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

    /*! Initializes macro number \a macro for command number \a command 
        and calls startNextRePro().
	\param[in] macro the index of the macro
	\param[in] command the index of the macro's command to be executed
	\param[in] saving toggles whether saving to files via SaveFiles is enabled.
	\param[in] enable if \c true also runs disabled commands
	\param[in] newstack if not 0 set the current stack to \a newstack. */
  void startMacro( int macro, int command=0, bool saving=true, 
		   bool enable=false, deque<MacroPos> *newstack=0 );
    /*! Starts the startup Macro (only in case there is one). */
  void startUp( void );
    /*! Starts the shutdown Macro (only in case there is one). */
  void shutDown( void );
    /*! Starts the fallback Macro (only in case there is one). */
  void fallBack( bool saving=true );
    /*! Starts the startsession Macro (only in case there is one). */
  void startSession( void );
    /*! Starts the stopsession Macro (only in case there is one). */
  void stopSession( void );

    /*! Execute next macro command. Called by MacroCommand::execute(). */
  void executeMacro( int newmacro, const Str &params );
    /*! Set \a ThisMacroOnly \c true if \a macro is \c true, otherwise
        set \a ThisCommandOnly \c true. */
  void setThisOnly( bool macro );

    /*! Update the default macro file and save all options to the config file. */
  virtual void saveConfig( ofstream &str );

    /*! Set the pointer to the RePros to \a repros. */
  void setRePros( RePros *repros );

    /*! Pointer to RELACSWidget. */
  RELACSWidget *RW;
    /*! Pointer to all RePros. */
  RePros *RPs;
  
    /*! Write information of the Macros to \a str. */
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

    /*! The list of all Macros. */
  typedef deque< Macro* > MacrosType;
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
  deque< MacroPos > Stack;
    /*! Macro command for resume. */
  MacroPos ResumePos;
    /*! The stack for resume. */  
  deque< MacroPos > ResumeStack;
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
  deque< QAction* > SwitchActions;
  QAction *ResumeAction;
  QAction *ResumeNextAction;
  QGridLayout *ButtonLayout;

  bool Fatal;

};


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

    /*! Default constructor. */
  Macro( void );
    /*! Construct a new macro with name \a name. */
  Macro( Str name, Macros *mcs );
    /*! Copy constructor. */
  Macro( const Macro &macro );

    /*! The name of the macro. */
  string name( void ) const;
    /*! Returns the macro variables. */
  Options &variables( void );
    /*! Returns the macro variables as a string. */
  string variablesStr( void ) const;
    /*! Returns the macro's project options. */
  Options &project( void );

    /*! Add parameter from \a param to the macros variables. */
  void addParameter( const Str &param );
    /*! Replaces macro variables in \a params by their value.
        The project options, "project" and "experiment" are deleted from
        the string and loaded into \a prjopt. */
  string expandParameter( const Str &params, Options &prjopt ) const;

    /*! The type of action on which this macro is executed. */
  int action( void ) const;
  /*! Set the Macro's action flag to \a action. */
  void setAction( int action );
    /*! Delete the action flag \a action from the Macro's action. */
  void delAction( int action );

    /*! True if this Macro has a button. */
  bool button( void ) const;
    /*! The button of the macro. */
  MacroButton *pushButton( void ) const;
    /*! Add button for the macro and its commands to \a menu. */
  void addButton( const string &keys );

    /*! True if this Macro has a menu. */
  bool menu( void ) const;
    /*! Add menu for the macro and its commands to \a menu
        using \a text as a label. */
  void addMenu( QMenu *menu, const string &text );
    /*! Forms a string for the menu consisting of the macro name
        and its variables. */
  string menuStr( void ) const;

    /*! True if this Macro has a shortcut key. */
  bool key( void ) const;
    /*! Set a shortcut key for the Macro and return it as a string. */
  string setKey( int &index );

    /*! Clear menu and button. */
  void clear( void );

    /*! True if this Macro is not to be cleared. */
  bool keep( void ) const;
    /*! True if this Macro overwrites existing ones. */
  bool overwrite( void ) const;

    /*! The number of commands the macro owns. */
  int size( void ) const;
    /*! The \a index -th command of this Macro . */
  MacroCommand *command( int index );
    /*! Add the command \a mc to the list of commands. */
  void push( MacroCommand *mc );

    /*! Iterator used to iterate through the commands. */
  typedef deque<MacroCommand*>::iterator iterator;
    /*! Returns an iterator pointing to the first command pointer. */
  iterator begin( void );
    /*! Returns an iterator pointing behind the last command pointer. */
  iterator end( void );

    /*! Load commands from a file until a new macro definition is encountered.
        Returns the stripped line and in \a line the full line containing
	the macro definition or macro parameter.  \a linenum holds the
	current line number and is incremented whenever load() reads a
	line. Warnings are appended tp \a warnings . */
  string load( ifstream &macrostream, string &line, int &linenum,
	       string &warnings );
    /*! Check and expand commands and initialize repros.
        Warnings are appended tp \a warnings .*/
  void check( int pass, string &warnings );
    /*! Set the indices of macro commands. */
  void setMacroIndices( void );
    /*! Initialize the number of this macro and the command numbers. */
  void init( int macronum );
    /*! Update the macro for the reloaded repro \a repro. */
  void reloadRePro( RePro *repro );

    /*! Unset running or stack icons. */
  void clearButton( void );
    /*! Set the icon indicating a running macro. */
  void runButton( void );
    /*! Set the icon indicating the stack position. */
  void stackButton( bool base );

    /*! Generates some icons. \sa destroyIcons() */
  static void createIcons( int size );
    /*! Frees the icons. \sa createIcons() */
  static void destroyIcons( void );

    /*! Write information of the Macro to \a str. */
  friend ostream &operator<< ( ostream &str, const Macro &macro );


public slots:

    /*! Runs this macro. */
  void run( void );
    /*! Stops the currently running repro and starts this macro. */
  void launch( void );
    /*! Open the popup menu right above the macro's button. */
  void popup( void );

    /*! Opens a macro dialog. */
  void dialog( void );
    /*! Accept the edited input from the dialog. */
  void acceptDialog( void );
    /*! Runs a macro for r==2. */
  void dialogAction( int r );
    /*! Unsets the DialogOpen flag. */
  void dialogClosed( int r );


private:

    /*! The name of the Macro. */
  string Name;
    /*! Macro variables */
  Options Variables;
    /*! Macro project/experiment identifiers */
  Options Project;

    /*! Defines whether this Macro requests to be the 
        startup, shutdown, fallback, startsession, or stopsession Macro.
        This is a bitfield which is evaluated with the 
	constants StartUp, ShutDown, FallBack, ExplicitFallBack,
	StartSession, and StopSession . */
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

    /*! The key code launching this macro. */
  int KeyCode;
    /*! A pointer to the macro's button. */
  MacroButton *PushButton;
    /*! A pointer to the macro's menu action that might have a key shortcut
        for popping up the menu. */
  QAction *MenuAction;
    /*! A pointer to the action that executes the macro. */
  QAction *RunAction;
    /*! A pointer to the last action of the first macro menu for
        positioning the popup menu. */
  QAction *BottomAction;

    /*! The index of this Macro. */
  int MacroNum;
    /*! Pointer to Macros. */
  Macros *MCs;
    /*! A flag indicating whether the dialog for this macro is already opened. */
  bool DialogOpen;

    /*! The list of commands associated with the Macro. */
  deque< MacroCommand* > Commands;

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

    /*! The different types of commands. */
  enum CommandType {
      /*! This command does nothing. */
    UnknownCom,
      /*! The command executes a RePro. */
    ReProCom,
      /*! The command executes another macro. */
    MacroCom,
      /*! The command executes a function of a filter . */
    FilterCom,
      /*! The command executes a function of a detector. */
    DetectorCom,
      /*! The command switches the macro file. */
    SwitchCom,
      /*! The command starts a session. */
    StartSessionCom,
      /*! The command executes a shell command. */
    ShellCom,
      /*! The command opens a message box with some text. */
    MessageCom, 
      /*! The command opens a browser for looking at a text. */
    BrowseCom
  };

  MacroCommand( void );
    /*! Create a MacroCommand from \a line. */
  MacroCommand( const string &line, Macros *mcs, Macro *mc );
    /*! Create a MacroCommand for a RePro. */
  MacroCommand( RePro *repro, const string &params,
		Macros *mcs, Macro *mc );
    /*! Copy constructor. */
  MacroCommand( const MacroCommand &com );

    /*! \return the CommandType of the MacroCommand. */
  CommandType command( void ) const;
    /*! \return the name of the MacroCommand. */
  string name( void ) const;
    /*! Set the name of the MacroCommand to \a name. */
  void setName( const string &name );
    /*! \return the parameter for the MacroCommand. */
  string parameter( void ) const;
    /*! Set the parameter of the MacroCommand to \a parameter. */
  void setParameter( const string &parameter );
    /*! Add \a s to the command's parameter string.
        Add a semicolon beforehand if \a addsep is \c true. */
  void addParameter( const string &s, bool addsep );

    /*! \return Whether the MacroCommand is enabled. */
  bool enabled( void ) const;

    /*! The RePro of this MacroCommand. */
  RePro *repro( void );
    /*! Set the CommandType of the MacroCommand to ReProCom . */
  void setReProCommand( void );
    /*! Set the RePro of this MacroCommand to \a repro. */
  void setRePro( RePro *repro );

    /*! Set the index of the Macro of this MacroCommand to \a index. */
  void setMacroIndex( int index );
    /*! Initialize the number of the parent macro and the command number. */
  void init( int macronum, int commandnum );

    /*! The menu offering various actions for the command. */
  QMenu *menu( void );
    /*! Adds the submenu for this command to \a menu. */
  void addMenu( QMenu *menu );

    /*! Execute the command. Returns \c true if a RePro was executed. */
  bool execute( bool saving );
    /*! Update the macro command for the reloaded repro \a repro. */
  void reloadRePro( RePro *repro );

    /*! Generates some icons. \sa destroyIcons() */
  static void createIcons( int size );
    /*! Frees the icons. \sa createIcons() */
  static void destroyIcons( void );

    /*! Write information of the MacroCommand to \a str. */
  friend ostream &operator<< ( ostream &str, const MacroCommand &command );


public slots:

    /*! Start macro at this command. */
  void start( void );
    /*! Run only this command. */
  void run( void );
    /*! View the RePro. */
  void view( void );
    /*! Reload the RePro plugin. */
  void reload( void );
    /*! Display the RePro's help text. */
  void help( void );
    /*! Enable/disable the command. */
  void enable( void );

    /*! Launch the dialog of the command. */
  void dialog( void );
    /*! Accept the edited input from the dialog. */
  void acceptDialog( void );
    /*! Runs the macro for r==2 and set the options to default for r==3. */
  void dialogAction( int r );
    /*! Unsets the DialogOpen flag and disconnects signals. */
  void dialogClosed( int r );


 private:

    /*! The type of the MacroCommand. */
  CommandType Command;
    /*! The name of the requested repro, macro, or shell script. */
  Str Name;
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
    /*! If the command refers to a macro (\a Command == MacroCommand)
        this is the index of the macro. */
  int MacroIndex;
    /*! if the command refers to a filter:
       1: save, 2: autoConfigure */
  int FilterCommand;
    /*! if the command refers to an event detector:
       1: save, 2: autoConfigure */
  int DetectorCommand;
    /*! Time for auto-configuring a filter or detector. */
  double AutoConfigureTime;
    /*! Timeout for a message in seconds. */
  double TimeOut;
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
  \brief Adds a rightClicked signal to a push button.
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

