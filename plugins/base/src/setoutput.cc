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
  : RePro( "SetOutput", "base", "Jan Benda", "1.2", "May 13, 2013" )
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

  // Zero button:
  QPushButton *zerobutton = new QPushButton( "&Zero" );
  bb->addWidget( zerobutton );
  zerobutton->setFixedHeight( zerobutton->sizeHint().height() );
  connect( zerobutton, SIGNAL( clicked() ),
	   this, SLOT( setZeros() ) );
  grabKey( Qt::ALT+Qt::Key_Z );

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


void SetOutput::preConfig( void )
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


void SetOutput::acceptValues( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = true;
    STW.accept( false );
    wake();
  }
}


void SetOutput::setValues( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = false;
    STW.accept( false );
    wake();
  }
}


void SetOutput::setZeros( void )
{
  if ( Interactive ) {
    Change = true;
    Quit = false;
    for ( int k=0; k<OutOpts.size(); k++ )
      OutOpts[k].setNumber( 0.0 );
    STW.updateValues();
    STW.accept( false );
    wake();
  }
}


void SetOutput::keepValues( void )
{
  if ( Interactive ) {
    Change = false;
    Quit = true;
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
    postCustomEvent( 11 ); // STW.setFocus()
    Quit = true;
    do {
      OutOpts.delFlags( Parameter::changedFlag() );
      // wait for input:
      Change = false;
      sleepWait();
      // set new values:
      if ( Change ) {
	OutList sigs;
	for ( int k=0; k<OutOpts.size(); k++ ) {
	  if ( OutOpts[k].changed() ) {
	    double value = OutOpts[k].number();
	    OutData sig;
	    sig.setTraceName( OutOpts[k].name() );
	    sig.constWave( value );
	    sigs.push( sig );
	  }
	}
	if ( sigs.size() > 0 ) {
	  string msg = "";
	  for ( int k=0; k<sigs.size(); k++ ) {
	    if ( k > 0 )
	      msg += ",  ";
	    msg += sigs[k].traceName() + '=' + Str( sigs[k][0] ) + sigs[k].unit();
	  }
	  message( msg );
	  directWrite( sigs );
	  if ( sigs.failed() ) {
	    warning( "Failed to write new values: " + sigs.errorText() );
	    postCustomEvent( 12 ); // clearFocus()
	    return Failed;
	  }
	}
	OutOpts.setToDefaults();
      }
      else {
	OutOpts.setDefaults();
	STW.updateValues();
	postCustomEvent( 12 ); // clearFocus()
	return Aborted;
      }
    } while ( ! Quit );
    postCustomEvent( 12 ); // clearFocus()
    Interactive = false;
  }
  else {
    // set the single requested value:
    OutData signal;
    signal.setTrace( outtrace );
    signal.constWave( value );
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
    acceptValues();
    e->accept();
  }
  if ( e->key() == Qt::Key_S && ( e->modifiers() & Qt::AltModifier ) ) {
    setValues();
    e->accept();
  }
  if ( e->key() == Qt::Key_Z && ( e->modifiers() & Qt::AltModifier ) ) {
    setZeros();
    e->accept();
  }
  else if ( e->key() == Qt::Key_C && ( e->modifiers() & Qt::AltModifier ) ) {
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

