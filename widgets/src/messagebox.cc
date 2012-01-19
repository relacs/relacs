/*
  messagebox.cc
  Displays a message in a separate window.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#include <QTimer>
#include <relacs/messagebox.h>

namespace relacs {


MessageBox::MessageBox( const string &caption, const string &message,
			MessageType type, bool blocking,
			QWidget *parent )
  : QMessageBox( parent )
{
  setWindowTitle( caption.c_str() );

  switch ( type ) {
  case Information:
    setIcon( QMessageBox::Information );
    break;
  case Warning:
    setIcon( QMessageBox::Warning );
    break;
  case Error:
    setIcon( QMessageBox::Critical );
    break;
  default:
    setIcon( QMessageBox::NoIcon );
    break;
  }

  setText( message.c_str() );
  setTextFormat( Qt::RichText );

  setStandardButtons( QMessageBox::Ok );
}


MessageBox::~MessageBox( void )
{
}



void MessageBox::exec( double timeout )
{
  if ( timeout > 0.0 )
    QTimer::singleShot( int( 1000.0 * timeout ), this, SLOT( closeMessage() ) );
  if ( isModal() )
    QMessageBox::exec();
  else
    QMessageBox::show();
}



MessageBox *MessageBox::information( const string &caption, 
				     const string &message,
				     bool blocking, double timeout, 
				     QWidget *parent )
{
  MessageBox *w = new MessageBox( caption, message, Information, blocking, parent );
  w->exec( timeout );
  return w;
}


MessageBox *MessageBox::warning( const string &caption, 
				 const string &message,
				 bool blocking, double timeout, 
				 QWidget *parent )
{
  MessageBox *w = new MessageBox( caption, message, Warning, blocking, parent );
  w->exec( timeout );
  return w;
}


MessageBox *MessageBox::error( const string &caption, 
			       const string &message,
			       bool blocking, double timeout, 
			       QWidget *parent )
{
  MessageBox *w = new MessageBox( caption, message, Error, blocking, parent );
  w->exec( timeout );
  return w;
}


void MessageBox::closeMessage( void )
{
  done( 0 );
}


}; /* namespace relacs */

#include "moc_messagebox.cc"

