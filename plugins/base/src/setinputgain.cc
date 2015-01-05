/*
  base/setinputgain.cc
  Set the gain of analog input traces.

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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <relacs/base/setinputgain.h>
using namespace relacs;

namespace base {


SetInputGain::SetInputGain( void )
  : RePro( "SetInputGain", "base", "Jan Benda", "1.0", "Feb 18, 2014" )
{
  Interactive = true;

  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addInteger( "gainindex", "Index of the gain to be set", 0, 0, 20 );
  addBoolean( "interactive", "Set values interactively", Interactive );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;

  // parameter:
  vb->addWidget( &SGW );

  // buttons:
  QHBoxLayout *bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  // Ok button:
  QPushButton *okbutton = new QPushButton( "&Ok" );
  bb->addWidget( okbutton );
  okbutton->setFixedHeight( okbutton->sizeHint().height() );
  connect( okbutton, SIGNAL( clicked() ),
	   this, SLOT( acceptGains() ) );
  grabKey( Qt::ALT+Qt::Key_O );
  grabKey( Qt::Key_Return );
  grabKey( Qt::Key_Enter );

  // Set button:
  QPushButton *setbutton = new QPushButton( "&Set" );
  bb->addWidget( setbutton );
  setbutton->setFixedHeight( setbutton->sizeHint().height() );
  connect( setbutton, SIGNAL( clicked() ),
	   this, SLOT( setGains() ) );
  grabKey( Qt::ALT+Qt::Key_S );

  // Max range button:
  QPushButton *maxbutton = new QPushButton( "&Max Ranges" );
  bb->addWidget( maxbutton );
  maxbutton->setFixedHeight( maxbutton->sizeHint().height() );
  connect( maxbutton, SIGNAL( clicked() ),
	   this, SLOT( setMaxRanges() ) );
  grabKey( Qt::ALT+Qt::Key_M );

  // Cancel button:
  QPushButton *cancelbutton = new QPushButton( "&Cancel" );
  bb->addWidget( cancelbutton );
  cancelbutton->setFixedHeight( cancelbutton->sizeHint().height() );
  connect( cancelbutton, SIGNAL( clicked() ),
	   this, SLOT( keepGains() ) );
  grabKey( Qt::ALT+Qt::Key_C );
  grabKey( Qt::Key_Escape );

  setLayout( vb );
}


void SetInputGain::preConfig( void )
{
  setText( "intrace", rawTraceNames() );
  setToDefault( "intrace" );

  // assemble input traces to InOpts:
  InOpts.clear();
  for ( int k=0; k<traces().size(); k++ ) {
    if ( trace( k ).source() == 0 ) {
      vector<double> ranges;
      maxVoltages( trace( k ), ranges );
      string rs = "";
      for ( unsigned int j=0; j<ranges.size(); j++ ) {
	if ( j > 0 )
	  rs += '|';
	rs += '-' + Str( ranges[j] ) + " --- " + Str( ranges[j] ) + ' ' + trace( k ).unit();
      }
      InOpts.addSelection( trace( k ).ident(), trace( k ).ident(), rs );
    }
  }

  // display values:
  SGW.assign( &InOpts, 0, 0, false, 0, mutex() );
}


void SetInputGain::notify( void )
{
  int intrace = traceIndex( text( "intrace" ) );
  if ( intrace >= 0 && intrace < traces().size() ) {
    vector<double> ranges;
    maxVoltages( trace( intrace ), ranges );
    setMinMax( "gainindex", 0, (int)ranges.size() );
  }
}


void SetInputGain::acceptGains( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = true;
    SGW.accept( false );
    wake();
  }
}


void SetInputGain::setGains( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = false;
    SGW.accept( false );
    wake();
  }
}


void SetInputGain::setMaxRanges( void )
{
  if ( Interactive ) {
    lock();
    for ( int k=0; k<InOpts.size(); k++ )
      InOpts[k].selectText( 0 );
    SGW.updateValues();
    unlock();
    SGW.accept( false );
  }
}


void SetInputGain::keepGains( void )
{
  if ( Interactive ) {
    Change = false;
    Quit = false;
    wake();
  }
}


int SetInputGain::main( void )
{
  // get options:
  int intrace = traceIndex( text( "intrace" ) );
  int gainindex = integer( "gainindex" );
  Interactive = boolean( "interactive" );

  noMessage();

  InOpts[intrace].selectText( gainindex );

  if ( Interactive ) {  
    keepFocus();
    postCustomEvent( 11 ); // SGW.setFocus()
    SGW.updateValues();
    Quit = true;
    do {
      InOpts.delFlags( Parameter::changedFlag() );
      // wait for input:
      Change = false;
      sleepWait();
      // set new values:
      if ( Change ) {
	string msg = "";
	int j = 0;
	for ( int k=0; k<InOpts.size(); k++ ) {
	  if ( InOpts[k].changed() ) {
	    setGain( trace( k ), InOpts[k].index() );
	    if ( j == 0 ) {
	      selectText( "intrace", trace( k ).ident() );
	      setInteger( "gainindex", InOpts[k].index() );
	      setToDefaults();
	    }
	    else
	      msg += ",  ";
	    msg += InOpts[k].name() + ": gain-index " + Str( InOpts[k].index() );
	    j++;
	  }
	}
	activateGains();
	message( msg );
	InOpts.setToDefaults();
      }
      else {
	InOpts.setDefaults();
	SGW.updateValues();
	postCustomEvent( 12 ); // clearFocus()
	return Aborted;
      }
    } while ( ! Quit );
    postCustomEvent( 12 ); // clearFocus()
    Interactive = false;
  }
  else {
    setGain( trace( intrace ), gainindex );
    activateGains();
    SGW.updateValues();
  }

  sleep( 0.01 );
  return Completed;
}


void SetInputGain::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Qt::Key_O && ( e->modifiers() & Qt::AltModifier ) ) {
    acceptGains();
    e->accept();
  }
  if ( e->key() == Qt::Key_S && ( e->modifiers() & Qt::AltModifier ) ) {
    setGains();
    e->accept();
  }
  if ( e->key() == Qt::Key_M && ( e->modifiers() & Qt::AltModifier ) ) {
    setMaxRanges();
    e->accept();
  }
  else if ( e->key() == Qt::Key_C && ( e->modifiers() & Qt::AltModifier ) ) {
    keepGains();
    e->accept();
  }
  else if ( ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) && e->modifiers() == Qt::NoModifier ) {
    acceptGains();
    e->accept();
  }
  else if ( e->key() == Qt::Key_Escape && e->modifiers() == Qt::NoModifier ) {
    keepGains();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void SetInputGain::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    if ( SGW.firstWidget() != 0 )
      SGW.firstWidget()->setFocus( Qt::TabFocusReason );
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


addRePro( SetInputGain, base );

}; /* namespace base */

#include "moc_setinputgain.cc"
