/*
  optwidget.h
  A Widget for modifying Options.

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

#ifndef _RELACS_OPTWIDGET_H_
#define _RELACS_OPTWIDGET_H_ 1

#include <vector>
#include <QWidget>
#include <QEvent>
#include <QMutex>
#include <QLabel>
#include <QLayout>
#include <QEvent>
#include <relacs/options.h>
using namespace std;

namespace relacs {


/*! 
\class OptWidget
\author Jan Benda
\version 1.0
\brief A Widget for modifying Options.
\bug OptWidgetDate input widget is crashing. Error in QDateEdit?

This widget is an input form for Options that can be used
to interactively edit the values of Options.
If you want a dialog-window, use OptDialog, which is based on OptWidget.

The OptWidget supports simple text editing for text-options with a single value,
combo-boxes for text-options with multiple values, spin-boxes for
number and integer options, check boxes for boolean options, labels, 
and separators (a horizontal line).

Here is an example:
\image html optwidget.png
It was created with the following code:
\code
#include <options.h>
#include <optwidget.h>

...

Options opt;
opt.addLabel( "Timing" );
opt.addNumber( "duration", "Duration of Signal", 0.3527, 0.01, 1.0, 0.0001, "seconds", "ms" );
opt.addNumber( "pause", "Pause between Signals", 0.2, 0.0, 1.0, 0.01, "seconds", "ms", "%g", 1 );
opt.setNumber( "pause", 0.180 );
opt.addSeparator( "Flow Control" );
opt.addInteger( "repeats", "Repeats", 8, 0, 100 );
opt.addBoolean( "loop", "Loop", true );
opt.addSeparator();
opt.addText( "color", "Color", "red,green,blue");
opt.addText( "comment", "Comments", "no comment" );

...

OptWidget ow( this );
ow.assign( opt, 0, 1 );
ow.setSpacing( 4 );
ow.setMargin( 10 );

...
\endcode

First you have to create some Options by either defining them explicitely
as in the example with the addNumber(), addText(), ... functions,
or you load them from a file with the load( istream& ) function.
See the documentation for Options for more details.
Then you construct the OptWidget. Either the Options are passed directly
to OptWidget via the constructor,
or they are assigned later on with assign().
Both ways you can specify a select-mask to select a subset of the Options
and a readonly-mask to specify which Options are not editable
(in the example the "pause" options is not editable).

Normally the new values entered by the user are assigned to the Options
by calling the accept() function. If you want the values of the Options
to be continuosly updated, set the \a contupdate argument in the constructor
or the assign() function to \c true.
Each time a new value is entered in the widget
the corresponding option gets this value set.
You can react to the change by reimplementing the 
Options::notify() function.
The option whose value has changed will have the changedFlag() set.
This flag is automatically cleared after notify() was called.

reset() sets the values in the input form to their initial values.
resetDefault() sets them to the Options' default values.
If the values or properties of the Options are changed,
use the update functions updateValue(), updateValues(), updateSettings()
to update the input form of the OptWidget.

Options whose values were changed via the OptWidget get the changedFlag()
in their flags() set.

The spacing between two input lines can be adjusted with setSpacing().
The spacing between the widget margins and its content are set with
setMargin().


*/

class OptWidgetBase;

class OptWidget : public QWidget
{
  Q_OBJECT

public:

    /*! Use normal sized font for the label. */
  static const long LabelNormal = 0x00000000;
    /*! Use small sized font for the label. */
  static const long LabelSmall = 0x00000001;
    /*! Use large sized font for the label. */
  static const long LabelLarge = 0x00000002;
    /*! Use huge sized font for the label. */
  static const long LabelHuge = 0x00000003;
    /*! Use bold font for label. */
  static const long LabelBold = 0x00000004;
    /*! Use italic font for label. */
  static const long LabelItalic = 0x00000008;
    /*! Use black color for label. */
  static const long LabelBlack = 0x00000000;
    /*! Use red color for label.*/
  static const long LabelRed = 0x00000010;
    /*! Use green color for label.*/
  static const long LabelGreen = 0x00000020;
    /*! Use blue color for label.*/
  static const long LabelBlue = 0x00000030;
    /*! Use standard background for label. */
  static const long LabelBack = 0x00000000;
    /*! Use black background for label.*/
  static const long LabelBackBlack = 0x00000040;
    /*! Use white background for label.*/
  static const long LabelBackWhite = 0x00000080;

    /*! Use normal sized font for the value. */
  static const long ValueNormal = 0x00000000;
    /*! Use small sized font for the value. */
  static const long ValueSmall = 0x00001000;
    /*! Use large sized font for the value. */
  static const long ValueLarge = 0x00002000;
    /*! Use huge sized font for the value. */
  static const long ValueHuge = 0x00003000;
    /*! Use bold font for value. */
  static const long ValueBold = 0x00004000;
    /*! Use italic font for value. */
  static const long ValueItalic = 0x00008000;
    /*! Use black color for value. */
  static const long ValueBlack = 0x00000000;
    /*! Use red color for value.*/
  static const long ValueRed = 0x00010000;
    /*! Use green color for value.*/
  static const long ValueGreen = 0x00020000;
    /*! Use blue color for value.*/
  static const long ValueBlue = 0x00030000;
    /*! Use standard background for value. */
  static const long ValueBack = 0x00000000;
    /*! Use black background for value.*/
  static const long ValueBackBlack = 0x00040000;
    /*! Use white background for value.*/
  static const long ValueBackWhite = 0x00080000;

    /*! Use normal sized font for the label and the value. */
  static const long Normal = LabelNormal + ValueNormal;
    /*! Use small sized font for the label and the value. */
  static const long Small = LabelSmall + ValueSmall;
    /*! Use large sized font for the label and the value. */
  static const long Large = LabelLarge + ValueLarge;
    /*! Use huge sized font for the label and the value. */
  static const long Huge = LabelHuge + ValueHuge;
    /*! Use bold font for label and the value. */
  static const long Bold = LabelBold + ValueBold;
    /*! Use italic font for label and the value. */
  static const long Italic = LabelItalic + ValueItalic;
    /*! Use black color for label and the value. */
  static const long Black = LabelBlack + ValueBlack;
    /*! Use red color for label and the value.*/
  static const long Red = LabelRed + ValueRed;
    /*! Use green color for label and the value.*/
  static const long Green = LabelGreen + ValueGreen;
    /*! Use blue color for label and the value.*/
  static const long Blue = LabelBlue + ValueBlue;
    /*! Use standard background for label and the value. */
  static const long Back = LabelBack + ValueBack;
    /*! Use black background for label and the value.*/
  static const long BackBlack = LabelBackBlack + ValueBackBlack;
    /*! Use white background for label and the value.*/
  static const long BackWhite = LabelBackWhite + ValueBackWhite;

    /*! If set you can only select from text options with multiple values,
        but not add new values. */
  static const long SelectText = Parameter::SelectText;
    /*! Enables auto completion mode of editable combo boxes. */
  static const long ComboAutoCompletion = 0x02000000;
    /*! Browse for existing file. */
  static const long BrowseExisting = 0x04000000;
    /*! Browse for any file. */
  static const long BrowseAny = 0x08000000;
    /*! Browse for directories. */
  static const long BrowseDirectory = 0x10000000;
    /*! Browse for some file or directory. */
  static const long Browse = BrowseExisting + BrowseAny + BrowseDirectory;
    /*! Do not strip the current working directory from the path. */
  static const long BrowseAbsolute = 0x20000000;
    /*! Use LCD numbers to display a number. */
  static const long ValueLCD = 0x01000000;
    /*! Use this label to distinguish search patterns
        while saving parameter. */
  static const long SavePatternLabel = Parameter::SavePatternLabel;
    /*! Use this label to distinguish search patterns
        while reading parameter. */
  static const long ReadPatternLabel = Parameter::ReadPatternLabel;
    /*! Label is a new tab. */
  static const long TabLabel = 0x04000000;
    /*! Try to format label as a mathematical equation. */
  static const long MathLabel = 0x08000000;

    /*! Put request string and value in separate lines. */
  static const int BreakLinesStyle = 0x0001;
    /*! Insert extra space between two options. */
  static const int ExtraSpaceStyle = 0x0002;
    /*! Labels are introducing new tabs. */
  static const int TabLabelStyle = 0x0004;
    /*! Print the options name in front of the request string. */
  static const int NameFrontStyle = 0x0008;
    /*! Print the options name behind the request string. */
  static const int NameBehindStyle = 0x0010;
    /*! Use a different font for the option's name.
        Only used in combination with NameFrontStyle or NameBehindStyle. */
  static const int HighlightNameStyle = 0x0020;


    /*! Constructs an empty OptWidget.
        To add Options to the widget use assign(). */
  OptWidget( QWidget *parent=0, Qt::WindowFlags f=0 );
    /*! Constructs an OptWidget for the Options \a o. 
        All Options are displayed and are editable.
        \sa assign() */
  OptWidget( Options *o, QMutex *mutex=0, 
	     QWidget *parent=0, Qt::WindowFlags f=0 );
    /*! Constructs an OptWidget for the Options \a o. 
        Only Options with their flags() & \a selectmask > 0 are displayed.
	If \a selectmask ist less or equal to zero,
	all Options \a o are displayed.
        Options with their flags() & \a romask > 0 are not editable.
	If \a romask equals zero all Options \a o are editable.
	If \a romask is negative none of the Options \a o is editable.
	If \a contupdate equals \a true, inputs by the user are immediately
	applied to the Options \a o.
        Otherwise, accept() has to be called in order
	to apply the changes to the Options \a o. */
  OptWidget( Options *o, int selectmask, int romask, bool contupdate, int style,
	     QMutex *mutex=0, QWidget *parent=0, Qt::WindowFlags f=0 );
    /*! Destructs the OptWidget. */
  ~OptWidget( void );

    /*! Assigns Options \a o to the widget. 
        All Options are displayed and are editable. */
  OptWidget &operator=( Options *o );
    /*! Assigns Options \a o to the widget.
        Only Options with their flags() & \a selectmask > 0 are displayed.
	If \a selectmask ist less or equal to zero,
	all Options \a o are displayed.
        Options with their flags() & \a romask > 0 are not editable.
	If \a romask equals zero all Options \a o are editable.
	If \a romask() is negative none of the Options \a o is editable.
	If \a contupdate equals \a true, inputs by the user are immediately
	applied to the Options \a o.
        Otherwise, accept() has to be called in order
	to apply the changes to the Options \a o.
        The standard layout \a style = 0 puts each option into a separate line.
        If \a style = 1 then the option's identifier is in one line
        and the options value and unit is in the following line.
        \a mutex is a mutex that is used by OptWidget to lock
        access to the options \a o while they are accessed from OptWidget functions.
	Especially, during calls of the notify() function this mutex will be locked. */
  OptWidget &assign( Options *o, int selectmask=0, int romask=0,
		     bool contupdate=false, int style=0,
		     QMutex *mutex=0 );

    /*! The maximum number of lines in a single tab. */
  int lines( void ) const { return MaxLines; };
    /*! Set the spacing between the lines to \a pixel pixel. */
  void setSpacing( int pixel );
    /*! Set the spacing between the widget margins and its content
        to \a pixel pixel. */
  void setMargin( int pixel );

    /*! A pointer to the first editable widget in OptWidget.
        Is null, if there isn't any editable widget.
        Can be used to set the tab order. */
  QWidget *firstWidget( void ) const;
    /*! A pointer to the last editable widget in OptWidget.
        Is null, if there isn't any editable widget.
        Can be used to set the tab order. */
  QWidget *lastWidget( void ) const;

    /*! The mask that was used to select single options.
	\sa assign(), readOnlyMask() */
  int selectMask( void ) const { return SelectMask; };
    /*! The mask that was used to decide whether
        an option should be editable or not.
	\sa assign(), selectMask(). */
  int readOnlyMask( void ) const { return ReadOnlyMask; };
    /*! \c True if changes by the user are immediately applied to the
        Options values.
        \sa assign(). */
  bool continuousUpdate( void ) const { return ContinuousUpdate; };
    /*! The flag that is used to mark options whose value were changed
        by or-ing their flags with this flag.
        It is preset to a constant value (16384) and
	cannot be changed by the user. */
  static int changedFlag( void ) { return ChangedFlag; };

    /*! Returns a pointer to the Options on which this widgets works on. */
  Options *options( void ) const { return Opt; };

    /*! Provide a mutex that is used by OptWidget to lock
        access to the options while they are accessed. 
        Passing a '0' disables the mutex. */
  void setMutex( QMutex *mutex );


public slots:

    /*! Set values of the Options according to the changes
        entered by the user via the OptWidget.
	Each options whose value was changed by the user gets 
	the changedFlag() set.
	Then the Options::notify() function is called
	and the signal valuesChanged() is emitted.
	Finally the changedFlag() is cleared 
	if \a clearchanged is set to \c true.
	If a mutex was provided it is locked within this function
	and consequently during notify() and valuesChanged().
	This function can only be called from the GUI thread.
	\sa reset(), resetDefault(), setMutex() */
  void accept( bool clearchanged );
    /*! Same as accept( bool ) with \a clearchanged set to true. */
  void accept( void );
    /*! Reset all input fields to their initial values.
	If a mutex was provided it is locked within this function.
	This function can only be called from the GUI thread.
        \sa resetDefault(), accept(), setMutex() */
  void reset( void );
    /*! Reset all input fields to the Options default values.
	If a mutex was provided it is locked within this function.
	This function can only be called from the GUI thread.
        \sa reset(), accept(), setMutex() */
  void resetDefault( void );

    /*! Read out the value of the option with identifier string \a ident
        and update the input field accordingly.
	Can be called from any thread.
	If a mutex was provided it is locked while the options
	are accessed.
	In a multithreaded program this is usually after this function returns.
        \sa updateValues(), updateSettings(), setMutex() */
  void updateValue( const string &ident );
    /*! Read out the values of all options 
        and update their input fields accordingly.
	If a mutex was provided it is locked while the options
	are accessed.
	In a multithreaded program this is usually after this function returns.
        \sa updateValue(), updateSettings(), setMutex() */
  void updateValues( void );
    /*! Read out the values of all options that have \a flag set
        and update their input fields accordingly.
	If a mutex was provided it is locked while the options
	are accessed.
	In a multithreaded program this is usually after this function returns.
        \sa updateValue(), updateSettings(), setMutex() */
  void updateValues( int flag );
    /*! Read out the value, range, stepsize, and unit
        of the option with identifier string \a ident
        and update the input field accordingly.
	If a mutex was provided it is locked while the options
	are accessed.
	In a multithreaded program this is usually after this function returns.
        \sa updateValue(), updateValues(), setMutex() */
  void updateSettings( const string &ident );
    /*! Read out the value, range, stepsize, and unit
        of all options and update their input fields accordingly.
	If a mutex was provided it is locked while the options
	are accessed.
	In a multithreaded program this is usually after this function returns.
        \sa updateValue(), updateValues(), setMutex() */
  void updateSettings( void );
    /*! Read out the value, range, stepsize, and unit
        of all options that have \a flag set
	and update their input fields accordingly.
	If a mutex was provided it is locked while the options
	are accessed.
	In a multithreaded program this is usually after this function returns.
        \sa updateValue(), updateValues(), setMutex() */
  void updateSettings( int flag );


signals:

    /*! This signal is emitted by accept(). */
  void valuesChanged( void );


protected:

  virtual void customEvent( QEvent *e );


private:

  friend class OptWidgetBase;
  friend class OptWidgetText;
  friend class OptWidgetMultiText;
  friend class OptWidgetNumber;
  friend class OptWidgetBoolean;
  friend class OptWidgetDate;
  friend class OptWidgetTime;
  friend class OptWidgetLabel;
  friend class OptWidgetSeparator;

    /*! For internal use only. */
  void addWidget( OptWidgetBase *owb );
  static void setLabelFontStyle( QWidget *w, long style );
  static void setLabelColorStyle( QWidget *w, long style, bool palette=false, bool base=false, bool button=false );
  static void setLabelStyle( QWidget *w, long style, bool palette=false, bool base=false, bool button=false );
  static void setValueFontStyle( QWidget *w, long style );
  static void setValueColorStyle( QWidget *w, long style, bool palette=false, bool base=false, bool button=false );
  static void setValueStyle( QWidget *w, long style, bool palette=false, bool base=false, bool button=false );
  static QLabel* unitLabel( const Parameter &p, QWidget *parent );

  Options *Opt;

  QWidget *MainWidget;
  QWidget *FirstWidget;
  QWidget *LastWidget;

  QMutex *OMutex;

  vector< QGridLayout* > Layout;
  int MaxLines;

  vector< OptWidgetBase* > Widgets;

  bool DisableUpdate;

  int SelectMask;
  int ReadOnlyMask;
  bool ContinuousUpdate;
  static const int ChangedFlag = Parameter::ChangedFlag;
  static const int UpdateFlag = Parameter::ChangedFlag >> 1;

  class UpdateEvent : public QEvent
  {
  public:
    UpdateEvent( int type )
      : QEvent( QEvent::Type( QEvent::User+type ) ) {};
  };

  class UpdateIdentEvent : public QEvent
  {
  public:
    UpdateIdentEvent( int type, const string &ident )
      : QEvent( QEvent::Type( QEvent::User+type ) ), Ident( ident ) {};
    string ident( void ) const { return Ident; };
  private:
    string Ident;
  };


};

}; /* namespace relacs */

#endif /* ! _RELACS_OPTWIDGET_H_ */
