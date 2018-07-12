/*
  base/setdigitaloutput.cc
  Set the level of digital output lines.

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

#include <relacs/digitalio.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <relacs/base/setdigitaloutput.h>
using namespace relacs;

namespace base {


SetDigitalOutput::SetDigitalOutput( void )
  : RePro( "SetDigitalOutput", "base", "Jan Benda", "1.0", "Jul 12, 2018" )
{
  Interactive = false;

  // add some options:
  addText( "device", "Name of the digital I/O device", "dio-1" );
  addInteger( "line", "Digital output line", 0, 0, 8 ).setFlags( 8 ).setActivation( "interactive", "false" );
  addInteger( "value", "Level to be writen to output line", 0, 0, 1 ).setFlags( 8 ).setActivation( "interactive", "false" );
  addBoolean( "interactive", "Set values interactively", false );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;

  // parameter:
  STW.assign( this, 8, 0, false, 0, mutex() );
  vb->addWidget( &STW );

  // buttons:
  QHBoxLayout *bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  // Ok button:
  QPushButton *okbutton = new QPushButton( "&Ok" );
  bb->addWidget( okbutton );
  okbutton->setFixedHeight( okbutton->sizeHint().height() );
  connect( okbutton, SIGNAL( clicked() ),
	   this, SLOT( acceptValues() ) );
  grabKey( Qt::ALT+Qt::Key_O );
  grabKey( Qt::Key_Return );
  grabKey( Qt::Key_Enter );

  // Set button:
  QPushButton *setbutton = new QPushButton( "&Set" );
  bb->addWidget( setbutton );
  setbutton->setFixedHeight( setbutton->sizeHint().height() );
  connect( setbutton, SIGNAL( clicked() ),
	   this, SLOT( setValues() ) );
  grabKey( Qt::ALT+Qt::Key_S );

  // Cancel button:
  QPushButton *cancelbutton = new QPushButton( "&Cancel" );
  bb->addWidget( cancelbutton );
  cancelbutton->setFixedHeight( cancelbutton->sizeHint().height() );
  connect( cancelbutton, SIGNAL( clicked() ),
	   this, SLOT( keepValues() ) );
  grabKey( Qt::ALT+Qt::Key_C );
  grabKey( Qt::Key_Escape );

  setLayout( vb );
}


void SetDigitalOutput::acceptValues( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = true;
    STW.accept( false );
    wake();
  }
}


void SetDigitalOutput::setValues( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = false;
    STW.accept( false );
    wake();
  }
}


void SetDigitalOutput::keepValues( void )
{
  if ( Interactive ) {
    lock();
    Change = false;
    Quit = true;
    unlock();
    wake();
  }
}


int SetDigitalOutput::main( void )
{
  // get options:
  string device = text( "device" );
  Interactive = boolean( "interactive" );

  noMessage();

  DigitalIO *dio = digitalIO( device );
  if ( dio == 0 ) {
    warning( "Digital I/O device <b>" + device + "</b> not found!" );
    return Failed;
  }

  if ( Interactive ) {
    keepFocus();
    postCustomEvent( 11 ); // STW.setFocus()
    Quit = true;
    do {
      // wait for input:
      Change = false;
      sleepWait();
      // set new values:
      if ( Change ) {
	int line = integer( "line" );
	int value = integer( "value" );
	message( "On <b>" + device + "</b> set level of line <b>" + Str( line ) + "</b> to <b>" + (value ? "high" : "low" ) + "</b>" );
	int r = dio->configureLine( line, true );
	if ( r == 0 )
	  r = dio->write( line, value );
	if ( r != 0 ) {
	  warning( "Failed to set level on DIO line <b>" + Str( line ) + "</b>!" );
	  postCustomEvent( 12 ); // clearFocus()
	  return Failed;
	}
      }
      else {
	STW.updateValues();
	postCustomEvent( 12 ); // clearFocus()
	return Aborted;
      }
    } while ( ! Quit );
    postCustomEvent( 12 ); // clearFocus()
    Interactive = false;
  }
  else {
    int line = integer( "line" );
    int value = integer( "value" );
    message( "On <b>" + device + "</b> set level of line <b>" + Str( line ) + "</b> to <b>" + (value ? "high" : "low" ) + "</b>" );
    int r = dio->configureLine( line, true );
    if ( r == 0 )
      r = dio->write( line, value );
    if ( r != 0 ) {
      warning( "Failed to set level on DIO line <b>" + Str( line ) + "</b>!" );
      postCustomEvent( 12 ); // clearFocus()
      return Failed;
    }
    STW.updateValues();
  }

  sleep( 0.01 );
  return Completed;
}


void SetDigitalOutput::keyPressEvent( QKeyEvent *e )
{
  e->ignore();
  if ( e->key() == Qt::Key_O && ( e->modifiers() & Qt::AltModifier ) ) {
    acceptValues();
    e->accept();
  }
  if ( e->key() == Qt::Key_S && ( e->modifiers() & Qt::AltModifier ) ) {
    setValues();
    e->accept();
  }
  if ( e->key() == Qt::Key_C && ( e->modifiers() & Qt::AltModifier ) ) {
    keepValues();
    e->accept();
  }
  else if ( ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) && e->modifiers() == Qt::NoModifier ) {
    acceptValues();
    e->accept();
  }
  else if ( e->key() == Qt::Key_Escape && e->modifiers() == Qt::NoModifier ) {
    keepValues();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void SetDigitalOutput::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    if ( STW.firstWidget() != 0 )
      STW.firstWidget()->setFocus( Qt::TabFocusReason );
    break;
  }
  case 12: {
    removeFocus();
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( SetDigitalOutput, base );

}; /* namespace base */

#include "moc_setdigitaloutput.cc"
