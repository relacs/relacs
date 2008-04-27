/*
  configdialog.h
  ConfigClass widget with dialogs.

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

#ifndef _CONFIGDIALOG_H_
#define _CONFIGDIALOG_H_

#include <string>
#include <vector>
#include <qwidget.h>
#include <qmutex.h>
#include <qlayout.h>
#include <relacs/configclass.h>

using namespace std;


/*! 
\class ConfigDialog
\brief ConfigClass widget with dialogs.
\author Jan Benda
\version 1.0

ConfigDialog adds a dialog for editing the configuration Options
of a ConfigClass.
You can select a subset of the Options for the dialog
by defining a dialogSelectMask() with setDialogSelectMask() 
and addDialogSelectMask().
Likewise, some Options can be marked as read-only with
dialogReadOnlyMask(), setDialogReadOnlyMask(), and addDialogReadOnlyMask().
The appearance of the dialog can be controlled with dialogStyle(),
setDialogStyle(), and addDialogStyle().
dialogOpen() returns \c true if the dialog is already opened.
The dialog is opened with dialog().

ConfigDialog also provides help() for displaying some html-based help text.
Help texts are assumed to be contained in html files with name
helpFileName().
These files are searched in directories as specified in the list of
default search pathes that can be accessed with helpPathes(), helpPath()
and manipulated with clearHelpPathes(), setHelpPath(), addHelpPath().

A %ConfigDialog widget has a name() and a title(),
the last version() was written by author() on date().
This information is set either by the constructor or by
setName(), setTitle(), setAuthor(), setVersion(), and setDate().
*/


class ConfigDialog : public QWidget, public ConfigClass
{
  Q_OBJECT


public:

    /*! Construct a %ConfigDialog.
        The identifier \a configident is used for identifying this class
	in the configuration file of group \a configgroup.
	The class has a unique \a name and a widget \a title.
	The implementation of a class derived from %ConfigDialog
	has a \a version and was written by \a author on \a date.
        \sa setConfigIdent(), setConfigGroup(),
	setName(), setTitle(), setAuthor(), setDate(), setVersion() */
  ConfigDialog( const string &configident="", int configgroup=0,
		const string &name="", 
		const string &title="",
		const string &author="",
		const string &version="",
		const string &date=__DATE__ );
    /*! Destruct the ConfigDialog. */
  virtual ~ConfigDialog( void );

    /*! Return the default BoxLayout that is managing the widget's geometry. 
        It is empty, so therefore ou might delete it and attach 
	a different layout to the widget. */
  QBoxLayout *boxLayout( void );

    /*! The name of the class. */
  string name( void ) const;
    /*! Set the name of the class to \a name. */
  virtual void setName( const string &name );
    /*! The title of the class as it appears above the widget. */
  string title( void ) const;
    /*! Set the title of the class to \a title. */
  virtual void setTitle( const string &title );
    /*! The author of the class. */ 
  string author( void ) const;
    /*! Set the author of the class to \a author. */
  virtual void setAuthor( const string &author );
    /*! The version string of the class. */
  string version( void ) const;
    /*! Set the version string of the class to \a version. */
  virtual void setVersion( const string &version );
    /*! The date of the last revision of the class 
        (default is set to the compile time). */
  string date( void ) const;
    /*! Set the date of the last revision of the class to \a date. */
  virtual void setDate( const string &date );

    /*! Returns the \a inx-th default path used for searching help files. */
  string helpPath( int inx=0 ) const;
    /*! Returns the number of default pathes used for searching help files. */
  int helpPathes( void ) const;
    /*! Clear the list of default pathes, where to look for help files. */
  void clearHelpPathes( void );
    /*! Use \a path as the single default path,
        where to loock for help files. */
  void setHelpPath( const string &path );
    /*! Add \a path to the list of default pathes,
        where to loock for help files. */
  void addHelpPath( const string &path );

    /*! \return the name of the file to be displayed in the help dialog.
        The default implementation returns name() + \c ".html". */
  virtual string helpFileName( void ) const;

    /*! True if the dialog is open. */
  bool dialogOpen( void ) const;
    /*! True if the help window is open. */
  bool helpOpen( void ) const;

    /*! Lock the mutex of the ConfigDialog.
        \sa unlock(), mutex() */
  void lock() const;
    /*! Unlock the mutex of the ConfigDialog.
        \sa lock(), mutex() */
  void unlock() const;
    /*! Returns a pointer to the mutex of the ConfigDialog.
        \sa lock(), unlock() */
  QMutex *mutex( void );


public slots:

    /*! Launches a dialog that allows to edit the values of the Options.
        You may reimplement dialog() with your own dialog.
        \sa dialogOpen(), setDialogOpen(), dClosed(),
        dialogAccepted(), dialogAction(), dialogClosed() */
  virtual void dialog( void );
    /*! Opens a new window that displays some help information.
        You may reimplement help() with your own help browser.
        \sa helpOpen(), setHelpOpen(), hClosed() */
  virtual void help( void );


signals:

    /*! Values entered via the dialog are copied to the Options. */
  void dialogAccepted( void );
    /*! A button with return code \a r was clicked in the dialog. */
  void dialogAction( int r );
    /*! The dialog was closed and returned \a r. */
  void dialogClosed( int r );


protected:

    /*! Returns the caption for the dialog. */
  string dialogCaption( void ) const;
    /*! Set the caption for the dialog to \a caption. */
  void setDialogCaption( const string &caption );
    /*! Returns true if a header should be displayed in the dialog. */
  bool dialogHeader( void ) const;
    /*! Display a header with author, version and date info in the dialog
        if \a d is set to true. */
  void setDialogHeader( bool d );
    /*! Returns true if a help button should be displayed in the dialog header. */
  bool dialogHelp( void ) const;
    /*! Add a help-button to the dialog header if \a d is set to true. */
  void setDialogHelp( bool d );
    /*! The mask that is used to select single options for the standard dialog. */
  int dialogSelectMask( void ) const;
    /*! Set the mask that is used to select single options 
        for the standard dialog to \a mask.
        Default is 0, i.e. all options appear in the dialog. */
  void setDialogSelectMask( int mask );
    /*! Add \a mask to the mask that is used to select single options 
        for the standard dialog. */
  void addDialogSelectMask( int mask );
    /*! The mask that is used to select single options as read-only
        for the standard dialog. */
  int dialogReadOnlyMask( void ) const;
    /*! Set the mask that is used to select single options 
        as read-only for the standard dialog to \a mask.
        Default is 0, i.e. all options are read-writeable. */
  void setDialogReadOnlyMask( int mask );
    /*! Add \a mask to the mask that is used to select single options 
        as read-only for the standard dialog. */
  void addDialogReadOnlyMask( int mask );
    /*! The style used for the standard dialog. */
  int dialogStyle( void ) const;
    /*! Set the style used for the standard dialog to \a style.
        The implementation of dialog() uses OptWidget.
        See there for a list of possible styles. */
  void setDialogStyle( int style );
    /*! Add \a style to the style used for the standard dialog. */
  void addDialogStyle( int style );
    /*! Set the status of the dialog to \a open. 
        Use this if you reimplement dialog()
	to inform ConfigDialog about the status of the dialog window. */
  void setDialogOpen( bool open=true );

    /*! Returns the caption for the help window. */
  string helpCaption( void ) const;
    /*! Set the caption for the help window to \a caption. */
  void setHelpCaption( const string &caption );
    /*! Set the status of the help window to \a open. 
        Use this if you reimplement help()
	to inform ConfigDialog about the status of the help window. */
  void setHelpOpen( bool open=true );


protected slots:

    /*! Informs the class that the dialog window is closed. */
  virtual void dClosed( int r );
    /*! Informs the class that the help window is closed. */
  virtual void hClosed( int r );


private:

  QBoxLayout *BoxLayout;

  string Name;
  string Title;
  string Author;
  string Version;
  string Date;

  int DialogSelectMask;
  int DialogROMask;
  int DialogStyle;
  string DialogCaption;
  bool Dialog;
  bool UseHeader;
  bool UseHelp;
  string HelpCaption;
  bool Help;
  vector<string> HelpPathes;

  mutable QMutex CDMutex;

};

#endif
