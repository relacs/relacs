/*
  controltabs.cc
  Container organizing Control plugins.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#include <relacs/parameter.h>
#include <relacs/messagebox.h>
#include <relacs/plugins.h>
#include <relacs/control.h>
#include <relacs/relacswidget.h>
#include <relacs/controltabs.h>

namespace relacs {


ControlTabs::ControlTabs( RELACSWidget *rw, QWidget *parent )
  : PluginTabs( Qt::Key_C, rw, parent ),
    HandlingEvent( false )
{
}


ControlTabs::~ControlTabs( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    Plugins::destroy( (*cp)->uniqueName(), RELACSPlugin::ControlId );
    delete *cp;
    *cp = 0;
  }
  CN.clear();
}


void ControlTabs::createControls( void )
{
  CN.clear();
  Parameter &cp = RW->SS[ "controlplugin" ];
  for ( int k=0; k<cp.size(); k++ ) {
    string cm = cp.text( k, "" );
    if ( ! cm.empty() ) {
      Control *cn = (Control *)Plugins::create( cm, RELACSPlugin::ControlId );
      if ( cn == 0 ) {
	RW->printlog( "! warning: Contol plugin " + cm + " not found!" );
	MessageBox::warning( "RELACS Warning !", 
			     "Contol plugin <b>" + cm + "</b> not found!",
			     this );
      }
      else {
	if ( cn->widget() != 0 )
	  addTab( cn->widget(), cn->name().c_str() );
	cn->setRELACSWidget( RW );
	CN.push_back( cn );
      }
    }
  }

  // check for duplicate Control names:
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    for ( deque< Control* >::iterator np=cp+1; np != CN.end(); ++np ) {
      if ( (*cp)->name() == (*np)->name() ) {
	(*np)->setLongUniqueName();
	setTabText( indexOf( (*np)->widget() ), (*np)->uniqueName().c_str() );
      }
    }
  }
}


void ControlTabs::addMenu( QMenu *menu, bool doxydoc )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->addActions( menu, doxydoc );
}


void ControlTabs::initDevices( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    (*cp)->setSettings();
    (*cp)->initDevices();
  }
}


void ControlTabs::clearDevices( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->clearDevices();
}


void ControlTabs::assignTracesEvents( const InList &il, const EventList &el )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    (*cp)->assignTracesEvents( il, el );
  }
}


void ControlTabs::assignTracesEvents( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    (*cp)->assignTracesEvents();
  }
}


void ControlTabs::requestStop( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->requestStop();
}


void ControlTabs::start( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->start();
}


void ControlTabs::wait( double time )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->wait( time );
}


void ControlTabs::modeChanged( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->modeChanged();
}


void ControlTabs::notifyStimulusData( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->notifyStimulusData();
}


void ControlTabs::notifyMetaData( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->notifyMetaData();
}


void ControlTabs::sessionStarted( void )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->sessionStarted();
}


void ControlTabs::sessionStopped( bool saved )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp )
    (*cp)->sessionStopped( saved );
}


Control *ControlTabs::control( int index )
{
  if ( index >= 0 && index < (int)CN.size() )
    return CN[index];
  return 0;
}


Control *ControlTabs::control( const string &name )
{
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    if ( (*cp)->uniqueName() == name )
      return *cp;
  }
  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    if ( (*cp)->name() == name )
      return *cp;
  }
  return 0;
}


void ControlTabs::keyPressEvent( QKeyEvent *event )
{
  // unused events are propagated back to parent widgets,
  // therefore we have to prevent this additional call:
  if ( HandlingEvent ) {
    event->ignore();
    return;
  }
  HandlingEvent = true;

  PluginTabs::keyPressEvent( event );

  if ( ! event->isAccepted() && currentWidget() != 0  )
    QCoreApplication::sendEvent( currentWidget(), event );

  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    if ( (*cp)->globalKeyEvents() &&
	 (*cp)->widget() != 0 &&
	 (*cp)->widget() != currentWidget() )
      QCoreApplication::sendEvent( (*cp)->widget(), event );
  }

  HandlingEvent = false;
}


void ControlTabs::keyReleaseEvent( QKeyEvent *event )
{
  // unused events are propagated back to parent widgets,
  // therefore we have to prevent this additional call:
  if ( HandlingEvent ) {
    event->ignore();
    return;
  }
  HandlingEvent = true;

  if ( ! event->isAccepted() && currentWidget() != 0 )
    QCoreApplication::sendEvent( currentWidget(), event );

  for ( deque< Control* >::iterator cp=CN.begin(); cp != CN.end(); ++cp ) {
    if ( (*cp)->globalKeyEvents() &&
	 (*cp)->widget() != 0 &&
	 (*cp)->widget() != currentWidget() )
      QCoreApplication::sendEvent( (*cp)->widget(), event );
  }

  HandlingEvent = false;
}


}; /* namespace relacs */

#include "moc_controltabs.cc"

