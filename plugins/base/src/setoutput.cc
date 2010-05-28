/*
  base/setoutput.cc
  Set an output to a specified value

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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <relacs/base/setoutput.h>
using namespace relacs;

namespace base {


const int SetOutput::ChannelFlag = 1;
const int SetOutput::ParameterFlag = 2;


SetOutput::SetOutput( void )
  : RePro( "SetOutput", "Base", "Jan Benda", "1.0", "Mar 21, 2009" )
{
  // add some options:
  addSelection( "outtrace", "Output trace", "V-1" );
  addNumber( "value", "Value to be writen to output trace", 0.0, -100000.0, 100000.0, 0.1 );
  addBoolean( "interactive", "Set values interactively", false );

  // Ok button:
  QPushButton *OKButton = new QPushButton( "&Ok" );
  OKButton->setFixedHeight( OKButton->sizeHint().height() );
  QWidget::connect( OKButton, SIGNAL( clicked() ),
		    (QWidget*)this, SLOT( setValues() ) );
  grabKey( Qt::ALT+Qt::Key_O );
  grabKey( Qt::Key_Return );
  grabKey( Qt::Key_Enter );

  // Cancel button:
  QPushButton *CancelButton = new QPushButton( "&Cancel" );
  CancelButton->setFixedHeight( OKButton->sizeHint().height() );
  QWidget::connect( CancelButton, SIGNAL( clicked() ),
		    (QWidget*)this, SLOT( keepValues() ) );
  grabKey( Qt::ALT+Qt::Key_C );
  grabKey( Qt::Key_Escape );

  // layout:
  QHBoxLayout *hb = new QHBoxLayout;
  hb->setSpacing( 4 );
  hb->addWidget( OKButton );
  hb->addWidget( CancelButton );

  QVBoxLayout *l = new QVBoxLayout;
  setLayout( l );
  l->addWidget( &STW );
  l->addLayout( hb );
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
		       -1.0e10, 1.0e10, 0.0001, outTrace( k ).unit() ).setFlags( flag );
  }

  // display values:
  STW.assign( &OutOpts, ParameterFlag, 0, false, 0, mutex() );
  widget()->updateGeometry();
  if ( STW.lastWidget() != 0 )
    widget()->setTabOrder( STW.lastWidget(), OKButton );
  //  setTabOrder( OKButton, CancelButton );
}


void SetOutput::notify( void )
{
  int outtrace = index( "outtrace" );
  setUnit( "value", outTrace( outtrace ).unit() );
}


void SetOutput::setValues( void )
{
  Change = true;
  STW.accept( false );
  wake();
}


void SetOutput::keepValues( void )
{
  Change = false;
  wake();
}


int SetOutput::main( void )
{
  // get options:
  int outtrace = index( "outtrace" );
  double value = number( "value" );
  bool interactive = boolean( "interactive" );

  noMessage();

  if ( interactive ) {
    OutOpts.delFlags( Parameter::changedFlag() );
    postCustomEvent( 11 ); // STW.setFocus();
    // wait for input:
    Change = false;
    sleepWait();
    postCustomEvent( 12 ); // clearFocus();
    // set new values:
    if ( Change ) {
      OutList sigs;
      for ( int k=0; k<OutOpts.size(); k++ ) {
	if ( OutOpts[k].changed() ) {
	  double value = OutOpts[k].number();
	  OutData sig( value );
	  sig.setTraceName( OutOpts[k].ident() );
	  sig.setIdent( OutOpts[k].ident() + "=" + Str( value ) + OutOpts[k].unit() );
	  sigs.push( sig );
	}
      }
      if ( sigs.size() > 0 ) {
	directWrite( sigs );
	if ( sigs.failed() ) {
	  warning( sigs.errorText() );
	  return Failed;
	}
      }
    }
    else {
      OutOpts.setDefaults();
      STW.updateValues();
    }
  }
  else {
    // set the single requested value:
    OutData signal( value );
    signal.setIdent( "value=" + Str( value ) );
    signal.setTrace( outtrace );
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
    OKButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_C && ( e->modifiers() & Qt::AltModifier ) ) {
    CancelButton->animateClick();
    e->accept();
  }
  else if ( ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) && e->modifiers() == Qt::NoModifier ) {
    OKButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_Escape && e->modifiers() == Qt::NoModifier ) {
    CancelButton->animateClick();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void SetOutput::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    if ( STW.firstWidget() != 0 )
      STW.firstWidget()->setFocus();
  }
  else if ( qce->type() == QEvent::User+12 ) {
    widget()->window()->setFocus();
  }
  else
    RELACSPlugin::customEvent( qce );
}


addRePro( SetOutput );

}; /* namespace base */

#include "moc_setoutput.cc"

