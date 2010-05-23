/*
  patchclamp/setdc.cc
  Interactively set the DC current.

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <relacs/patchclamp/setdc.h>
using namespace relacs;

namespace patchclamp {


SetDC::SetDC( void )
  : RePro( "SetDC", "patchclamp", "Jan Benda", "1.0", "Feb 09, 2010" ),
    IUnit( "nA" )
{
  // add some options:
  addSelection( "outcurrent", "Output trace", "Current-1" );
  addSelection( "dcamplitudesel", "Set DC amplitude", "to absolute value|to previous value|to a fraction of the threshold|relative to threshold" );
  addNumber( "dcamplitude", "DC amplitude", 0.0, -1000.0, 1000.0, 0.01 ).setActivation( "dcamplitudesel", "to absolute value" );
  addNumber( "dcamplitudefrac", "Fraction of threshold", 0.9, 0.0, 1.0, 0.01, "1", "%" ).setActivation( "dcamplitudesel", "to a fraction of the threshold" );
  addNumber( "dcamplitudedecr", "Decrement below threshold", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "dcamplitudesel", "relative to threshold" );
  addBoolean( "interactive", "Set dc amplitude interactively", false );
  addNumber( "dcamplitudestep", "Stepsize for entering dc", 0.001, 0.0, 1000.0, 0.001 );

  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );

  // edit:
  QHBoxLayout *eb = new QHBoxLayout;
  vb->addLayout( eb );
  eb->addWidget( new QLabel( "DC current" ) );
  EW = new QDoubleSpinBox;
  EW->setRange( -1000.0, 1000.0 );
  EW->setValue( 0.0 );
  EW->setDecimals( 3 );
  EW->setSingleStep( 0.001 );
  eb->addWidget( EW );
  UnitLabel = new QLabel( "nA" );
  eb->addWidget( UnitLabel );

  // buttons:
  QHBoxLayout *bb = new QHBoxLayout;
  vb->addLayout( bb );
  bb->setSpacing( 4 );

  // Ok button:
  OKButton = new QPushButton( "&Ok" );
  OKButton->setFixedHeight( OKButton->sizeHint().height() );
  bb->addWidget( OKButton );
  QWidget::connect( OKButton, SIGNAL( clicked() ),
		    (QWidget*)this, SLOT( setValue() ) );
  grabKey( Qt::ALT+Qt::Key_O );
  grabKey( Qt::Key_Return );
  grabKey( Qt::Key_Enter );
  
  // Cancel button:
  CancelButton = new QPushButton( "&Cancel" );
  CancelButton->setFixedHeight( OKButton->sizeHint().height() );
  bb->addWidget( CancelButton );
  QWidget::connect( CancelButton, SIGNAL( clicked() ),
		    (QWidget*)this, SLOT( keepValue() ) );
  grabKey( Qt::ALT+Qt::Key_C );
  grabKey( Qt::Key_Escape );

}


void SetDC::config( void )
{
  setText( "outcurrent", currentOutputNames() );
  setToDefault( "outcurrent" );
}


void SetDC::notify( void )
{
  int outcurrent = index( "outcurrent" );
  if ( outcurrent >= 0 && CurrentOutput[outcurrent] >= 0 ) {
    IUnit = outTrace( CurrentOutput[outcurrent] ).unit();
    setUnit( "dcamplitude", IUnit );
    setUnit( "dcamplitudedecr", IUnit );
    setUnit( "dcamplitudestep", IUnit );
    UnitLabel->setText( IUnit.c_str() );
  }
  postCustomEvent( 13 ); // setStep();
}


int SetDC::main( void )
{
  // get options:
  OutCurrent = outTraceIndex( text( "outcurrent", 0 ) );
  int dcamplitudesel = index( "dcamplitudesel" );
  double dcamplitude = number( "dcamplitude" );
  double dcamplitudefrac = number( "dcamplitudefrac" );
  double dcamplitudedecr = number( "dcamplitudedecr" );
  bool interactive = boolean( "interactive" );

  // don't print repro message:
  noMessage();

  // plot trace:
  plotToggle( true, false, 1.0, 0.0 );

  // init:
  double orgampl = stimulusData().number( outTraceName( OutCurrent ) );
  DCAmplitude = metaData( "Cell" ).number( "ithreshss" );
  if ( dcamplitudesel == 0 )
    DCAmplitude = dcamplitude;
  else if ( dcamplitudesel == 1 )
    DCAmplitude = orgampl;
  else if ( dcamplitudesel == 2 )
    DCAmplitude *= dcamplitudefrac;
  else
    DCAmplitude -= dcamplitudedecr;

  if ( interactive ) {
    OutData dcsignal( DCAmplitude );
    dcsignal.setTrace( OutCurrent );
    dcsignal.setIdent( "DC=" + Str( DCAmplitude ) + IUnit );
    directWrite( dcsignal );
    if ( dcsignal.overflow() ) {
      printlog( "Requested DC current I=" + Str( DCAmplitude ) + IUnit + "too high!" );
      DCAmplitude = dcsignal.maxValue();
    }
    else if ( dcsignal.underflow() ) {
      printlog( "Requested DC current I=" + Str( DCAmplitude ) + IUnit + "too small!" );
      DCAmplitude = dcsignal.minValue();
    }
    if ( dcsignal.failed() ) {
      dcsignal = DCAmplitude;
      dcsignal.setIdent( "DC=" + Str( DCAmplitude ) + IUnit );
      directWrite( dcsignal );
    }
    if ( dcsignal.failed() ) {
      warning( "Failed to write out DC current! " + dcsignal.errorText() );
      return Failed;
    }
    message( "DC=<b>" + Str( DCAmplitude ) + "</b> " + IUnit );
    EW->setMinimum( dcsignal.minValue() );
    EW->setMaximum( dcsignal.maxValue() );
    EW->setValue( DCAmplitude );
    sleep( 0.01 );
    postCustomEvent( 11 ); // setFocus();
    // wait for input:
    SetValue = false;
    Finished = false;
    do {
      sleepWait();
      if ( ! Finished || ! SetValue ) {
	if ( Finished )
	  DCAmplitude = orgampl;
	dcsignal = DCAmplitude;
	dcsignal.setIdent( "DC=" + Str( DCAmplitude ) + IUnit );
	directWrite( dcsignal );
	message( "DC=<b>" + Str( DCAmplitude ) + "</b> " + IUnit );
	sleep( 0.01 );
      }
    } while ( ! Finished && ! interrupt() );
    postCustomEvent( 12 ); // clearFocus();
    if ( interrupt() )
      return Aborted;
  }
  else
    SetValue = true;

  if ( SetValue ) {
    // DC signal:
    OutData dcsignal( DCAmplitude );
    dcsignal.setTrace( OutCurrent );
    dcsignal.setIdent( "DC=" + Str( DCAmplitude ) + IUnit );
    directWrite( dcsignal );
    if ( dcsignal.failed() ) {
      DCAmplitude = orgampl;
      dcsignal = DCAmplitude;
      dcsignal.setIdent( "DC=" + Str( DCAmplitude ) + IUnit );
      directWrite( dcsignal );
    }
    message( "DC=<b>" + Str( DCAmplitude ) + "</b> " + IUnit );
  }

  sleep( 0.01 );
  return Completed;
}


void SetDC::setValue( double value )
{
  DCAmplitude = value;
  wake();
}


void SetDC::setValue( void )
{
  lock();
  SetValue = true;
  DCAmplitude = EW->value();
  Finished = true;
  unlock();
  wake();
}


void SetDC::keepValue( void )
{
  lock();
  SetValue = false;
  Finished = true;
  unlock();
  wake();
}


void SetDC::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Qt::Key_O && ( e->modifiers() & Qt::AltModifier ) ) {
    OKButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_C && ( e->modifiers() & Qt::AltModifier ) ) {
    CancelButton->animateClick();
    e->accept();
  }
  else if ( ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) && ( e->modifiers() == Qt::NoModifier ) ) {
    OKButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_Escape && ( e->modifiers() == Qt::NoModifier ) ) {
    CancelButton->animateClick();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void SetDC::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    EW->setFocus();
    QWidget::connect( EW, SIGNAL( valueChanged( double ) ),
		      (QWidget*)this, SLOT( setValue( double ) ) );
  }
  else if ( qce->type() == QEvent::User+12 ) {
    widget()->window()->setFocus();
    QWidget::disconnect( EW, SIGNAL( valueChanged( double ) ),
			 (QWidget*)this, SLOT( setValue( double ) ) );
  }
  else if ( qce->type() == QEvent::User+13 ) {
    EW->setSingleStep( number( "dcamplitudestep" ) );
  }
  else
    RELACSPlugin::customEvent( qce );
}


addRePro( SetDC );

}; /* namespace patchclamp */

#include "moc_setdc.cc"
