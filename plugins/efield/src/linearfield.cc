/*
  efield/linearfield.cc
  Measure the electric field manually with a single electrode in one direction

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <qlabel.h>
#include <relacs/efield/linearfield.h>
using namespace relacs;

namespace efield {


LinearField::LinearField( void )
  : RePro( "LinearField", "LinearField", "Efield",
	   "Jan Benda", "1.0", "Apr 23, 2009" ),
    B( (QWidget*)this ),
    O( &B ),
    P( (QWidget*)this )
{
  // add some options:
  //  addNumber( "value", "Value to be writen to output trace", 0.0, -100000.0, 100000.0, 0.1 );

  // layout:
  boxLayout()->setDirection( QBoxLayout::LeftToRight );

  // user interaction:
  GUIOpts.addNumber( "conductivity", "Water conductivity", 0.0, 0.0, 100000.0, 1.0, "uS/cm" );
  GUIOpts.addNumber( "distance", "Distance", 0.0, -100000.0, 100000.0, 1.0, "cm" );
  O.assign( &GUIOpts, 0, 0, false, OptWidget::BreakLinesStyle + OptWidget::ExtraSpaceStyle );
  O.setSpacing( 2 );

  // measure button:
  MeasureButton = new QPushButton( "&Measure", &B, "MeasureButton" );
  connect( MeasureButton, SIGNAL( clicked() ),
	   this, SLOT( measure() ) );
  grabKey( ALT+Key_M );
  grabKey( Key_Return );
  grabKey( Key_Enter );

  // finish button:
  FinishButton = new QPushButton( "&Finish", &B, "FinishButton" );
  connect( FinishButton, SIGNAL( clicked() ),
	   this, SLOT( finish() ) );
  grabKey( ALT+Key_F );
  grabKey( Key_Escape );

  updateGeometry();

  // setting the right tab order (seems not really to be needed!)
  if ( O.lastWidget() != 0 )
    setTabOrder( O.lastWidget(), MeasureButton );
  setTabOrder( MeasureButton, FinishButton );
}


void LinearField::measure( void )
{
  Measure = true;
  O.accept( false );
  wake();
}


void LinearField::finish( void )
{
  Measure = false;
  wake();
}


int LinearField::main( void )
{
  // get options:

  noMessage();
  keepFocus();

  // plot:
  P.setXLabel( "Distance [cm]" );
  P.setYLabel( "RMS Amplitude [V]" );

  postCustomEvent( 1 ); // O.setFocus();
  do {
    // wait for input:
    Measure = false;
    unlockAll();
    sleepWait();
    lockAll();
    if ( Measure ) {
      double d = GUIOpts.number( "distance" );
      message( "measure at " + Str( d ) + "cm" );
      // analyse:
      /*
      OutList sigs;
      for ( int k=0; k<OutOpts.size(); k++ ) {
	if ( OutOpts[k].changed() ) {
	  double value = OutOpts[k].number();
	  OutData sig( value );
	  sig.setTraceName( OutOpts[k].ident() );
	  sig.setIdent( "value=" + Str( value ) );
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
      */
    }
  } while ( Measure && ! interrupt() );
  postCustomEvent( 2 ); // O.clearFocus();
  return Completed;
}


void LinearField::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Key_M && ( e->state() & AltButton ) ) {
    MeasureButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Key_F && ( e->state() & AltButton ) ) {
    FinishButton->animateClick();
    e->accept();
  }
  else if ( ( e->key() == Key_Return || e->key() == Key_Enter ) && ( e->state() & KeyButtonMask ) == 0 ) {
    MeasureButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Key_Escape && ( e->state() & KeyButtonMask ) == 0 ) {
    FinishButton->animateClick();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void LinearField::customEvent( QCustomEvent *qce )
{
  if ( qce->type() == QEvent::User+1 ) {
    if ( O.firstWidget() != 0 )
      O.firstWidget()->setFocus();
  }
  else if ( qce->type() == QEvent::User+2 ) {
    clearFocus();
  }
}


addRePro( LinearField );

}; /* namespace efield */

#include "moc_linearfield.cc"

