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
#include <qpushbutton.h>
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
  O.assign( &GUIOpts, 0, 0, true, OptWidget::BreakLinesStyle + OptWidget::ExtraSpaceStyle );
  //	       metaDataMutex() );
  O.setSpacing( 2 );

  // measure button:
  QPushButton *measurebutton = new QPushButton( "&Measure", &B, "MeasureButton" );
  connect( measurebutton, SIGNAL( clicked() ),
	   this, SLOT( measure() ) );

  // finish button:
  QPushButton *finishbutton = new QPushButton( "&Finish", &B, "FinishButton" );
  connect( finishbutton, SIGNAL( clicked() ),
	   this, SLOT( finish() ) );

  updateGeometry();
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


void LinearField::customEvent( QCustomEvent *qce )
{
  if ( qce->type() == QEvent::User+1 ) {
    O.setFocus();
  }
  else if ( qce->type() == QEvent::User+2 ) {
    O.clearFocus();
  }
}


addRePro( LinearField );

}; /* namespace efield */

#include "moc_linearfield.cc"

