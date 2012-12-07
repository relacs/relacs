/*
  base/setoutput.cc
  Set an output to a specified value

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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <relacs/base/setoutput.h>
using namespace relacs;

namespace base {


const int SetOutput::ChannelFlag = 1;
const int SetOutput::ParameterFlag = 2;


SetOutput::SetOutput( void )
  : RePro( "SetOutput", "base", "Jan Benda", "1.0", "Mar 21, 2009" )
{
  Interactive = false;

  // add some options:
  addSelection( "outtrace", "Output trace", "V-1" );
  addNumber( "value", "Value to be writen to output trace", 0.0, -100000.0, 100000.0, 0.1 );
  addBoolean( "interactive", "Set values interactively", false );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;

  // parameter:
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
	   this, SLOT( setValues() ) );
  grabKey( Qt::ALT+Qt::Key_O );
  grabKey( Qt::Key_Return );
  grabKey( Qt::Key_Enter );

  // Cancel button:
  QPushButton *cancelbutton = new QPushButton( "&Cancel" );
  bb->addWidget( cancelbutton );
  cancelbutton->setFixedHeight( okbutton->sizeHint().height() );
  connect( cancelbutton, SIGNAL( clicked() ),
	   this, SLOT( keepValues() ) );
  grabKey( Qt::ALT+Qt::Key_C );
  grabKey( Qt::Key_Escape );

  setLayout( vb );
}


void SetOutput::config( void )
{
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );

  // assemble output traces to OutOpts:
  OutOpts.clear();
  for ( int k=0; k<outTracesSize(); k++ ) {
    int flag = outTrace( k ).channel() < 1000 ? ChannelFlag : ParameterFlag;
    OutOpts.addNumber( outTraceName( k ), outTraceName( k ), 0.0,
		       -1.0e6, 1.0e6, 0.001, outTrace( k ).unit() ).setFlags( flag );
  }

  // display values:
  STW.assign( &OutOpts, ParameterFlag, 0, false, 0, mutex() );
}


void SetOutput::notify( void )
{
  int outtrace = index( "outtrace" );
  if ( outtrace >= 0 && outtrace < outTracesSize() )
    setUnit( "value", outTrace( outtrace ).unit() );
}


void SetOutput::setValues( void )
{
  if ( Interactive ) {
    Change = true;
    STW.accept( false );
    wake();
  }
}


void SetOutput::keepValues( void )
{
  if ( Interactive ) {
    Change = false;
    wake();
  }
}


int SetOutput::main( void )
{
  // get options:
  int outtrace = index( "outtrace" );
  double value = number( "value" );
  Interactive = boolean( "interactive" );

  noMessage();

  if ( Interactive ) {
    keepFocus();
    OutOpts.delFlags( Parameter::changedFlag() );
    postCustomEvent( 11 ); // STW.setFocus()
    // wait for input:
    Change = false;
    sleepWait();
    postCustomEvent( 12 ); // clearFocus()
    Interactive = false;
    // set new values:
    if ( Change ) {
      OutList sigs;
      for ( int k=0; k<OutOpts.size(); k++ ) {
	if ( OutOpts[k].changed() ) {
	  double value = OutOpts[k].number();
	  OutData sig( value );
	  sig.setTraceName( OutOpts[k].name() );
	  sig.setIdent( OutOpts[k].name() + "=" + Str( value ) + OutOpts[k].unit() );
	  sig.description().newSection( "", "stimulus/value" );
	  sig.description().addNumber( OutOpts[k].name(), value, OutOpts[k].unit() );
	  sigs.push( sig );
	}
      }
      if ( sigs.size() > 0 ) {
	string msg = "";
	for ( int k=0; k<sigs.size(); k++ ) {
	  if ( k > 0 )
	    msg += ",  ";
	  msg += sigs[k].ident();
	}
	message( msg );
	directWrite( sigs );
	if ( sigs.failed() ) {
	  warning( "Failed to write new values: " + sigs.errorText() );
	  return Failed;
	}
      }
      OutOpts.setToDefaults();
    }
    else {
      OutOpts.setDefaults();
      STW.updateValues();
      return Aborted;
    }
  }
  else {
    // set the single requested value:
    OutData signal( value );
    signal.setIdent( "value=" + Str( value ) );
    signal.setTrace( outtrace );
    signal.description().newSection( "", "stimulus/value" );
    signal.description().addNumber( OutOpts[outtrace].name(),
				    value, OutOpts[outtrace].name() );
    directWrite( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }
    OutOpts[outtrace].setNumber( value );
    STW.updateValues();
  }

  sleep( 0.01 );
  return Completed;
}


const Options &SetOutput::outTraces( void ) const
{
  return OutOpts;
}


void SetOutput::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Qt::Key_O && ( e->modifiers() & Qt::AltModifier ) ) {
    setValues();
    e->accept();
  }
  else if ( e->key() == Qt::Key_C && ( e->modifiers() & Qt::AltModifier ) ) {
    keepValues();
    e->accept();
  }
  else if ( ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) && e->modifiers() == Qt::NoModifier ) {
    setValues();
    e->accept();
  }
  else if ( e->key() == Qt::Key_Escape && e->modifiers() == Qt::NoModifier ) {
    keepValues();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void SetOutput::customEvent( QEvent *qce )
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


addRePro( SetOutput, base );

}; /* namespace base */

#include "moc_setoutput.cc"

