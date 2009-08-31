/*
  optdialog.h
  A Dialog for editing Options.

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

#ifndef _RELACS_OPTDIALOG_H_
#define _RELACS_OPTDIALOG_H_ 1

#include <QDialog>
#include <QMutex>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <relacs/options.h>
#include "optwidget.h"

namespace relacs {


/*! 
\class OptDialog
\author Jan Benda
\version 1.0
\brief A Dialog for editing Options.
\bug Qt bug in construct(): setResizeMode() places dialog on weired places.

This dialog widget can be used to edit the values of Options.
It is based on OptWidget.

Here is an example:
\image html optdialog.png
This example was created with the following code:
\code
#include <options.h>
#include <optdialog.h>

...

Options opt1;
opt1.addLabel( "Timing" );
opt1.addNumber( "duration", "Duration of Signal", 0.3527, 0.01, 1.0, 0.0001, "seconds", "ms" );
opt1.addNumber( "pause", "Pause between Signals", 0.2, 0.0, 1.0, 0.01, "seconds", "ms", "%g", 3 );
opt1.setNumber( "pause", 0.180 );
opt1.addInteger( "repeats", "Repeats", 8, 0, 100 );
opt1.addSeparator( "Settings" );
opt1.addText( "color", "Color", "red,green,blue");
opt1.addText( "comment", "Comments", "no comment" );

Options opt2;
opt2.addSeparator();
opt2.addBoolean( "sinewave", "Use Sine Wave", false );
opt2.addBoolean( "loop", "Loop", true, 1 );

...

OptDialog d( this );
d.setCaption( "Example Dialog" );
d.addOptions( opt1, 0, 1 );
d.addOptions( opt2 );
d.setSpacing( 4 );
d.setMargin( 10 );
d.addButton( "&Ok", OptDialog::Accept, 1 );
d.addButton( "&Reset", OptDialog::Reset );
d.addButton( "&Close" );
d.exec();

...
\endcode

First you have to create some Options by either defining them explicitely
as in the example with the addNumber(), addText(), ... functions,
or you load them from a file with the load( istream& ) function.
See the documentation for Options for more details.
Then you construct the OptDialog. Either the Options are passed directly
to OptDialog via the constructor,
or they are assigned later on with addOptions().
With addOptions() you can specify a select-mask to select a subset of the Options
and a readonly-mask to specify which Options are not editable
(in the example the "pause" options is not editable).
Then you may define some buttons (see below)
and finally you call exec() to launch the dialog.

The dialog window can be either modal or modeless.
In modal mode all input to the main program from 
which the dialog was launched is blocked.
The modality can be specified only by a constructor.

The OptDialog comes with four default buttons "Ok", "Defaults", "Reset", "Close".
"Ok" copies the entered input to the corresponding Options, closes the dialog, and returns 1.
"Defaults" sets the values of the input form to the corresponding default values of
the Options.
"Reset"  sets the values of the input form to their initial values.
"Close" closes the dialog without accepting the input and returns 0.

If you don't like these buttons, use the addButton() function
to define your own.
For example, you would get the default buttons by the following commands:
\code
addButton( "&Ok", OptDialog::Accept, 1 );
addButton( "&Reset", OptDialog::Reset );
addButton( "&Defaults", OptDialog::Defaults );
addButton( "&Close" );
\endcode

The dialog can also be closed by hitting "ESC" or by just closing the window.
With setRejectCode() you can determine what value exec() returns in these
cases. Default is 0.

OptDialog provides three signals: valuesChanged() is emitted whenever
the Accept action is performed.
buttonClicked() enables you to 
implement more functionality to the dialog buttons.
dialogClosed() tells you that the dialog is closed.
The last two signals are especially important for modeless dialogs.

The caption of the dialog can be set by setCaption().
The spacing between two input lines can be adjusted with setSpacing().
The spacing between the widget margins and its content are set with
setMargin().
*/


class OptDialogButton;


class OptDialog : public QDialog
{
  Q_OBJECT

public:

    /*! Different actions for the dialog buttons. */
  enum Action { NoAction, Defaults, Reset, Accept };
    /*! \var Action NoAction
        No action is associated with the button.
	Use this for a "Cancel" or "Close" button. */
    /*! \var Action Defaults
        Pressing the button sets the values of the input fields to
	the default values of the corresponding Options. */
    /*! \var Action Reset
        Pressing the button resets the values of the input fields to
	their initial values. */
    /*! \var Action Accept
        Pressing the button copies the values of the input fields to
	the corresponding Options.
        This is usually the "Ok" button. */

    /*! Use this for the return value of a button if this button
        should not emit a buttonClicked() or dialogClosed() signal.
        Like for example a "Reset" or "Clear" button. */
  const static int NoReturn = -10000;
    /*! The return value for a button whose return code
        should equal the rejectCode().
        Like for example a "Close" or "Cancel" button. */
  const static int ReturnReject = -10001;

    /*! Construct an empty modal dialog.
        This dialog will block input to the program.
        \note You still have to call exec() in order to launch the dialog. */
  OptDialog( QWidget *parent=0 );
    /*! Construct an empty dialog with modality \a modal.
        If \a modal is \c true,
	this dialog will block input to the program.
        \note You still have to call exec() in order to launch the dialog. */
  OptDialog( bool modal, QWidget *parent=0 );
    /*! Construct a modal dialog for editing the Options \a o.
        The caption of the dialog window is set to \a title.
        This dialog will block input to the program.
        \note You still have to call exec() in order to launch the dialog. */
  OptDialog( Options &opt, const string &title,
	     QMutex *mutex=0, QWidget *parent=0 );
    /*! Construct a dialog for editing the Options \a o with modality \a modal.
        The caption of the dialog window is set to \a title.
	If \a modal is \c true,
	this dialog will block input to the program.
        \note You still have to call exec() in order to launch the dialog. */
  OptDialog( Options &opt, const string &title, bool modal,
	     QMutex *mutex=0, QWidget *parent=0 );
    /*! Destructs the OptDialog. */
  ~OptDialog( void );

    /*! Set the caption of the dialog window to \a dialog. */
  void setCaption( const string &title );

    /*! Assigns Options \a o to the dialog.
        Input forms for different Options can be placed on top of each other
	by multiple calls of addOptions().
        Only Options with their mode() & \a selectmask > 0 are displayed.
	If \a selectmask ist less or equal to zero,
	all Options \a o are displayed.
        Options with their mode() & \a romask > 0 are not editable.
	If \a romask equals zero all Options \a o are editable.
	If \a romask() is negative none of the Options \a o is editable.
        The standard layout \a style = 0 puts each option into a separate line.
        If \a style = 1 then the option's identifier is in one line
        and the options value and unit is in the following line.
	\sa addTabOptions(), addWidget(), addTabWidget(), addButton() */
  OptWidget *addOptions( Options &opt, int selectmask=0,
			 int romask=0, int style=0, QMutex *mutex=0 );
    /*! Assigns Options \a o as a page of a TabWidget with label \a label
        to the dialog.
        Only Options with their mode() & \a selectmask > 0 are displayed.
	If \a selectmask ist less or equal to zero,
	all Options \a o are displayed.
        Options with their mode() & \a romask > 0 are not editable.
	If \a romask equals zero all Options \a o are editable.
	If \a romask() is negative none of the Options \a o is editable.
        The standard layout \a style = 0 puts each option into a separate line.
        If \a style = 1 then the option's identifier is in one line
        and the options value and unit is in the following line.
	\sa addOptions(), addWidget(), addTabWidget(), addButton() */
  OptWidget *addTabOptions( const string &label, Options &opt,
			    int selectmask=0, int romask=0, int style=0,
			    QMutex *mutex=0 );

    /*! Add some widget to the dialog. 
        The widget is deleted when the dialog is closed.
        \sa addOptions(), addTabOptions(), addTabWidget(), addButton() */
  void addWidget( QWidget *widget );
    /*! Add some widget as a page of a TabWidget with label \a label
        to the dialog. 
        The widget is deleted when the dialog is closed.
        \sa addOptions(), addTabOptions(), addWidget(), addButton() */
  void addTabWidget( const string &label, QWidget *widget );

    /*! Set the spacing between the lines to \a pixel pixel.
        Call this function after you added all Options to the dialog
        with addOptions().
        \sa setMargin() */
  void setSpacing( int pixel );
    /*! Set the spacing between the widget margins and its content
        to \a pixel pixel.
        Call this function after you added all Options to the dialog
        with addOptions().
        \sa setSpacing() */
  void setMargin( int pixel );

    /*! The code exec() returns if the dialog is closed by entering "ESC"
        or by closing the window.
	The default reject code is 0.
        The reject code can be set to a different value by setRejectCode().
	rejectCode() returns the actual set reject code.
	\sa addButton() */
  int rejectCode( void ) const;
    /*! Set the code exec() returns if the dialog is closed by entering "ESC"
        or by closing the window to \a code.
        \sa addButton(), rejectCode() */
  void setRejectCode( int code );

    /*! Remove all buttons.
        You normally don't have to call this functions, since the default buttons
	are removed automatically by the first call of addButton().
	\sa addButton(), setRejectCode() */
  void clearButtons( void );
    /*! Add a button with label \a title to the OptDialog.
        Clicking the button will execute the action specified
        by the Action \a action (\a NoAction, \a Defaults, \a Reset, \a Accept).
	The dialog is closed if \a close is true
	and exec() is returning \a rcode.
        If default is \c true than this button will be activated
        when hitting "Return".
        If no default button is specified, the first button with
	the Accept action and a return value will be the default button.
	\sa clearButtons(), setRejectCode() */
  void addButton( const string &title, OptDialog::Action action,
		  int rcode, bool close=true, bool dflt=false );
    /*! Add a button with label \a title to the OptDialog.
        Clicking the button will execute the action specified
        by the Action \a action (\a NoAction, \a Defaults, \a Reset, \a Accept).
	The dialog is not closed and the buttom does not emit a buttonClicked() signal. 
	\sa clearButtons() */
  inline void addButton( const string &title, OptDialog::Action action )
    { addButton( title, action, NoReturn, false, false ); };
    /*! Add a button with label \a title to the OptDialog.
        Clicking the button will close the dialog without accepting the input.
	The dialog returns rejectCode() (default 0).
	This function is for creating a "Close" or "Cancel" button.
	\sa clearButtons(), setRejectCode() */
  inline void addButton( const string &title )
    { addButton( title, NoAction, ReturnReject, true, false ); };
 
    /*! Opens the dialog window.
        If the OptDialog is modal, exec() blocks until the dialog
        is finished and returns the return code of the button that
        was clicked, or rejectCode() if the window was closed. 
        If the dialog is modeless, exec() returns immediately
        and returns rejectCode().
	The default reject code is 0.
	Use the dialogClose() - signal to process the return value
	of a modeless dialog.
        \sa addOptions(), addButton(), setRejectCode() */
  int exec( void );

    /*! The flag that is used to mark options whose value were changed
        by or-ing their mode with this flag.
        It is preset to a constant value (32768) and
	cannot be changed by the user. */
  static int changedFlag( void ) { return OptWidget::changedFlag(); };

signals:

    /*! This signal is emitted when a button with the Accept Action
        is clicked, i.e. the values of the input fields are copied to
	the corresponding Options. */
  void valuesChanged( void ); 

    /*! This signal is emitted when a button with a return value is clicked.
        \a r is the return value of the button.
        The signal is usefull for modeless dialogs. */
  void buttonClicked( int r );

    /*! This signal is emitted when the dialog is closed.
        \a r is the return value of the dialog,
	which equals for modal dialogs the return value of exec().
        The signal is usefull for modeless dialogs. */
  void dialogClosed( int r );


protected slots:

    /*! Reimplemented for internal reasons. */
  void done( int r );
    /*! Reimplemented for internal reasons. */
  void reject( void );


private:

  void construct( void );
  void createButtons( void );

  QVBoxLayout *DialogBox;
  QHBoxLayout *ButtonBox;

  vector< OptWidget* > OWs;

  QTabWidget *Tabs;

  vector< OptDialogButton* > Buttons;
  bool DefaultButtons;

  int RejectCode;

};


}; /* namespace relacs */

#endif /* ! _RELACS_OPTDIALOG_H_ */
