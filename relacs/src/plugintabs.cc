/*
  plugintabs.cc
  Container organizing relacs plugins.

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

#include <relacs/relacswidget.h>
#include <relacs/plugintabs.h>
#include <QApplication>
#include <QKeyEvent>
#include <QLayout>

namespace relacs {


PluginTabs::PluginTabs( int key, RELACSWidget *rw, QWidget *parent )
  : QTabWidget( parent ),
    RW( rw ),
    HotKey( key )
{
  qApp->installEventFilter( this );
}


PluginTabs::~PluginTabs( void )
{
  qApp->removeEventFilter( this );
}


QWidget *PluginTabs::firstEnabledChildWidget( QLayout *l )
{
  if ( l == 0 || l->count() == 0 )
    return 0;

  for ( int k=0; k<l->count(); k++ ) {
    QWidget *w = l->itemAt( k )->widget();
    if ( w ) {
      if ( w->layout() )
	w = firstEnabledChildWidget( w->layout() );
    }
    else if ( l->itemAt( k )->layout() )
      w = firstEnabledChildWidget( l->itemAt( k )->layout() );
    if ( w && w->isEnabled() && w->focusPolicy() != Qt::NoFocus )
      return w;
  }

  return 0;
}


bool PluginTabs::eventFilter( QObject *obj, QEvent *event )
{
  if ( event->type() == QEvent::KeyPress ) {
    QKeyEvent *ke = dynamic_cast<QKeyEvent *>( event );
    if ( ke != 0 ) {
      if ( ( ke->modifiers() & Qt::ALT ) &&
	   ( ke->key() == Qt::Key_Left || ke->key() == Qt::Key_Right )  ) {
	// find out whether keyboard focus is within PluginTabs:
	QWidget *w = qApp->focusWidget();
	while ( w ) {
	  if ( w == this ) {
	    // yes! Let's change pages:
	    int c = currentIndex();
	    if ( ke->key() == Qt::Key_Left ) {
	      c--;
	      if ( c >= 0 )
		setCurrentIndex( c );
	    }
	    else {
	      c++;
	      if ( c < count() )
		setCurrentIndex( c );
	    }
	    return true;
	  }
	  w = w->parentWidget();
	}
	return false;
      }
    }
  }
  return QTabWidget::eventFilter( obj, event );
}


void PluginTabs::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() == HotKey ) {
    QWidget *w = currentWidget();
    if ( w != 0 ) {
      // find the child-widget that should receive focus:
      if ( w->focusWidget() )
	w->focusWidget()->setFocus( Qt::TabFocusReason );
      else {
	if ( w->layout() )
	  w = firstEnabledChildWidget( w->layout() );
	if ( w && w->isEnabled() && w->focusPolicy() != Qt::NoFocus )
	  w->setFocus( Qt::TabFocusReason );
      }
    }
    event->accept();
  }
  else
    event->ignore();
}


}; /* namespace relacs */

#include "moc_plugintabs.cc"

