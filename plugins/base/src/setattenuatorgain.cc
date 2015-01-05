/*
  base/setattenuatorgain.cc
  Set the gain factor of an Attenuate interface

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

#include <QPushButton>
#include <relacs/base/linearattenuate.h>
#include <relacs/base/setattenuatorgain.h>
using namespace relacs;

namespace base {


SetAttenuatorGain::SetAttenuatorGain( void )
  : RePro( "SetAttenuatorGain", "base", "Jan Benda", "1.0", "Nov 26, 2014" )
{
  Interactive = true;

  // add some options:
  addSelection( "outrace", "Input trace", "V-1" );
  addNumber( "gain", "attenuator gain to be set", 1.0, -1.0e6, 1.0e6, 0.001 );
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


void SetAttenuatorGain::preConfig( void )
{
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );

  // assemble output traces to OutOpts:
  AttOpts.clear();
  for ( int k=0; k<outTracesSize(); k++ ) {
    string tname = outTraceName( k );
    base::LinearAttenuate *latt = 
      dynamic_cast<base::LinearAttenuate*>( attenuator( tname ) );
    if ( latt != 0 ) {
      string name = tname;
      string iname = latt->intensityName();
      if ( ! iname.empty() && name != iname )
	name += " - " + iname;
      string unit = latt->intensityUnit();
      if ( unit.empty() )
	unit = "1";
      unit += "/" + outTrace( k ).unit();
      AttOpts.addNumber( tname, name, latt->gain(), -1.0e6, 1.0e6, 0.001, unit );
    }
  }
    
  // display values:
  SGW.assign( &AttOpts, 0, 0, false, 0, mutex() );
}


void SetAttenuatorGain::acceptGains( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = true;
    SGW.accept( false );
    wake();
  }
}


void SetAttenuatorGain::setGains( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = false;
    SGW.accept( false );
    wake();
  }
}


void SetAttenuatorGain::keepGains( void )
{
  if ( Interactive ) {
    Change = false;
    Quit = false;
    wake();
  }
}


int SetAttenuatorGain::main( void )
{
  // get options:
  int outtrace = index( "outtrace" );
  double gain = number( "gain" );
  Interactive = boolean( "interactive" );

  noMessage();

  // set the current gain values:
  for ( int k=0; k<AttOpts.parameterSize(); k++ ) {
    base::LinearAttenuate *latt = 
      dynamic_cast<base::LinearAttenuate*>( attenuator( AttOpts[k].name() ) );
    if ( latt != 0 )
      AttOpts[k].setNumber( latt->gain() );
  }

  if ( Interactive ) {  
    keepFocus();
    postCustomEvent( 11 ); // SGW.setFocus()
    SGW.updateValues();
    Quit = true;
    do {
      AttOpts.delFlags( Parameter::changedFlag() );
      // wait for input:
      Change = false;
      sleepWait();
      // set new values:
      if ( Change ) {
	string msg = "Set attenuator gains of ";
	int j = 0;
	for ( int k=0; k<AttOpts.parameterSize(); k++ ) {
	  if ( AttOpts[k].changed() ) {
	    base::LinearAttenuate *latt = 
	      dynamic_cast<base::LinearAttenuate*>( attenuator( AttOpts[k].name() ) );
	    if ( latt != 0 ) {
	      latt->setGain( AttOpts[k].number() );
	      if ( j > 0 )
		msg += ",  ";
	      msg += AttOpts[k].name() + ": " + Str( latt->gain() );
	      j++;
	    }
	  }
	}
	message( msg );
	AttOpts.setToDefaults();
      }
      else {
	AttOpts.setDefaults();
	SGW.updateValues();
	postCustomEvent( 12 ); // clearFocus()
	return Aborted;
      }
    } while ( ! Quit );
    postCustomEvent( 12 ); // clearFocus()
    Interactive = false;
  }
  else {
    base::LinearAttenuate *latt = 
      dynamic_cast<base::LinearAttenuate*>( attenuator( outTraceName( outtrace ) ) );
    if ( latt != 0 ) {
      latt->setGain( gain );
      AttOpts.setNumber( outTraceName( outtrace ), latt->gain() );
      SGW.updateValues();
    }
  }

  sleep( 0.01 );
  return Completed;
}


void SetAttenuatorGain::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Qt::Key_O && ( e->modifiers() & Qt::AltModifier ) ) {
    acceptGains();
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


void SetAttenuatorGain::customEvent( QEvent *qce )
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


addRePro( SetAttenuatorGain, base );

}; /* namespace base */

#include "moc_setattenuatorgain.cc"
