/*
  messagebox.h
  Displays a message in a separate window.

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

#ifndef _RELACS_MESSAGEBOX_H_
#define _RELACS_MESSAGEBOX_H_ 1

#include <string>
#include <qdialog.h>
using namespace std;

namespace relacs {


/*! 
\class MessageBox
\author Jan Benda
\version 1.2
\brief Displays a message in a separate window.

A MessageBox is a separate window which displays a
message text, an icon specifying the type of the message (see MessageType),
and a button labeled 'Ok'. Pressing the button closes the message window.
The message window can be blocking or non-blocking.
If it is blocking, executing the widget waits until the user
pressed the 'Ok' button before it returns.
In non-blocking mode it returns immediately.

The static functions information(), warning(), and error() 
create and execute a message window.
They also allow to specify a time after which the message window
is closed automatically.
*/


class MessageBox : public QDialog
{
  Q_OBJECT

public:

    /*! The type of message to be displayed. */
  enum MessageType {
      /*! The message is just an information. */
    Information,
      /*! The message is a warning, reporting some sort of non-fatal error. */
    Warning,
      /*! The message reports a fatal error.
          Usually the program is terminated after closing the message box. */
    Error
  };

    /*! Creates a new window with \a caption in the title bar displaying
        the message \a message and an icon specifying the type of the
	message \a type. The window can be blocking or non-blocking according
	to \a blocking.
        \sa information(), warning(), error() */
  MessageBox( const string &caption, const string &message,
	      MessageType type=Warning, bool blocking=false,
	      QWidget *parent=0, const char *name=0 );
    /*! Destroys a message box. */
  ~MessageBox( void );

    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type information.
	Calling this function can be blocking or non-blocking according
	to \a blocking.
        In blocking mode the function returns
	after the user pressed the 'Ok' button.
	A time \a timeout seconds can be specified
	after which the window is closed automatically.
        This feature works only with a non-blocking message.
	\sa warning(), error() */
  static MessageBox *information( const string &caption,
				  const string &message,
				  bool blocking=false, double timeout=0.0, 
				  QWidget *parent=0, const char *name=0 );
    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type information.
	After time \a timeout seconds the window is closed automatically.
	This function is non-blocking, i.e. it returns immediately.
	\sa warning(), error() */
  static inline MessageBox *information( const string &caption,
					 const string &message,
					 double timeout,
					 QWidget *parent=0,
					 const char *name=0 )
    { return information( caption, message, false, timeout, parent, name ); };
    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type information.
	This function is blocking,
	i.e. it returns after the user pressed the 'Ok' button.
	\sa warning(), error() */
  static inline MessageBox *information( const string &caption,
					 const string &message,
					 QWidget *parent=0,
					 const char *name=0 )
    { return information( caption, message, true, 0.0, parent, name ); };

    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type warning, which is an error which
	does not prevent the program from continuing.
	Calling this function can be blocking or non-blocking according
	to \a blocking.
        In blocking mode the function returns
	after the user pressed the 'Ok' button.
	A time \a timeout seconds can be specified
	after which the window is closed automatically.
        This feature works only with a non-blocking message.
        \sa information(), error() */
  static MessageBox *warning( const string &caption, const string &message,
			      bool blocking=false, double timeout=0.0, 
			      QWidget *parent=0, const char *name=0 );
    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type warning, which is an error which
	does not prevent the program from continuing.
	After time \a timeout seconds the window is closed automatically.
	This function is non-blocking, i.e. it returns immediately.
        \sa information(), error() */
  static inline MessageBox *warning( const string &caption,
				     const string &message,
				     double timeout,
				     QWidget *parent=0, const char *name=0 )
    { return warning( caption, message, false, timeout, parent, name ); };
    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type warning, which is an error which
	does not prevent the program from continuing.
	This function is blocking,
	i.e. it returns after the user pressed the 'Ok' button.
        \sa information(), error() */
  static inline MessageBox *warning( const string &caption,
				     const string &message,
				     QWidget *parent=0, const char *name=0 )
    { return warning( caption, message, true, 0.0, parent, name ); };

    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type error.
	Usually the program or action has to be terminated after
	displaying this message.
	Calling this function can be blocking or non-blocking according
	to \a blocking.
        In blocking mode the function returns
	after the user pressed the 'Ok' button.
	A time \a timeout seconds can be specified
	after which the window is closed automatically.
        This feature works only with a non-blocking message.
        \sa information(), warning() */
  static MessageBox *error( const string &caption, const string &message,
			    bool blocking=false, double timeout=0.0, 
			    QWidget *parent=0, const char *name=0 );
    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type error.
	Usually the program or action has to be terminated after
	displaying this message.
	After time \a timeout seconds the window is closed automatically.
	This function is non-blocking, i.e. it returns immediately.
        \sa information(), warning() */
  static inline MessageBox *error( const string &caption,
				   const string &message,
				   double timeout,
				   QWidget *parent=0, const char *name=0 )
    { return error( caption, message, false, timeout, parent, name ); };
    /*! Creates and executes a message window with \a caption
        in the title bar displaying the message \a message.
	The message is of type error.
	Usually the program or action has to be terminated after
	displaying this message.
	This function is blocking,
	i.e. it returns after the user pressed the 'Ok' button.
        \sa information(), warning() */
  static inline MessageBox *error( const string &caption,
				   const string &message,
				   QWidget *parent=0, const char *name=0 )
    { return error( caption, message, true, 0.0, parent, name ); };


private slots:

  void closeMessage( void );

};


}; /* namespace relacs */

#endif /* ! _RELACS_MESSAGEBOX_H_ */
