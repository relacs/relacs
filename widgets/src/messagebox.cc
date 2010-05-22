/*
  messagebox.cc
  Displays a message in a separate window.

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

#include <qlayout.h>
#include <qhbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qmessagebox.h>
#include <relacs/messagebox.h>

namespace relacs {


MessageBox::MessageBox( const string &caption, const string &message,
			MessageType type, bool blocking,
			QWidget *parent, const char *name )
  : QDialog( parent, name, blocking )
{
  setCaption( caption.c_str() );

  QVBoxLayout *l = new QVBoxLayout( this, 4, 10 );
  l->setAutoAdd( true );

  QLabel *label;
  QHBox *space;

  space = new QHBox( this );

  QHBox *upper = new QHBox( this );
  upper->setSpacing( 7 );
  label = new QLabel( upper );
  label = new QLabel( upper );
#if QT_VERSION >= 300
  if ( type == Information )
    label->setPixmap( QMessageBox::standardIcon( QMessageBox::Information ) );
  else if ( type == Warning )
    label->setPixmap( QMessageBox::standardIcon( QMessageBox::Warning ) );
  else
    label->setPixmap( QMessageBox::standardIcon( QMessageBox::Critical ) );
#else
  if ( type == Information )
    label->setPixmap( QMessageBox::standardIcon( QMessageBox::Information, style().guiStyle() ) );
  else if ( type == Warning )
    label->setPixmap( QMessageBox::standardIcon( QMessageBox::Warning, style().guiStyle() ) );
  else
    label->setPixmap( QMessageBox::standardIcon( QMessageBox::Critical, style().guiStyle() ) );
#endif
  label = new QLabel( upper );
  label = new QLabel( "", upper );
  label->setTextFormat( RichText );
  label->setText( message.c_str() );
  label = new QLabel( upper );

  QHBox *lower = new QHBox( this );
  label = new QLabel( lower );
  QPushButton *button = new QPushButton( "OK", lower );
  button->setFocus();
  connect( button, SIGNAL( clicked( void ) ), this, SLOT( accept( void ) ) );
  label = new QLabel( lower );

  space = new QHBox( this );

  setActiveWindow();
}


MessageBox::~MessageBox( void )
{
}


MessageBox *MessageBox::information( const string &caption, 
				     const string &message,
				     bool blocking, double timeout, 
				     QWidget *parent, const char *name )
{
  MessageBox *w = new MessageBox( caption, message, Information, blocking,
				  parent, name );
  if ( blocking )
    w->exec();
  else {
    w->show();
    if ( timeout > 0.0 )
      QTimer::singleShot( int( 1000.0 * timeout ), w, SLOT( closeMessage() ) );
  }
  return w;
}


MessageBox *MessageBox::warning( const string &caption, 
				 const string &message,
				 bool blocking, double timeout, 
				 QWidget *parent, const char *name )
{
  MessageBox *w = new MessageBox( caption, message, Warning, blocking,
				  parent, name );
  if ( blocking )
    w->exec();
  else {
    w->show();
    if ( timeout > 0.0 )
      QTimer::singleShot( int( 1000.0 * timeout ), w, SLOT( closeMessage() ) );
  }
  return w;
}


MessageBox *MessageBox::error( const string &caption, 
			       const string &message,
			       bool blocking, double timeout, 
			       QWidget *parent, const char *name )
{
  MessageBox *w = new MessageBox( caption, message, Error, blocking,
				  parent, name );
  if ( blocking )
    w->exec();
  else {
    w->show();
    if ( timeout > 0.0 )
      QTimer::singleShot( int( 1000.0 * timeout ), w, SLOT( closeMessage() ) );
  }
  return w;
}


void MessageBox::closeMessage( void )
{
  delete this;
}


}; /* namespace relacs */

#include "moc_messagebox.cc"

