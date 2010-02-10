/*
  patchclamp/setdc.cc
  Interactively set the DC current.

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

#include <qvbox.h>
#include <qhbox.h>
#include <relacs/patchclamp/setdc.h>
using namespace relacs;

namespace patchclamp {


SetDC::SetDC( void )
  : RePro( "SetDC", "SetDC", "patchclamp",
	   "Jan Benda", "1.0", "Feb 09, 2010" ),
    IUnit( "nA" )
{
  // add some options:
  addSelection( "outcurrent", "Output trace", "Current-1" );
  addSelection( "dcamplitudesel", "Set DC amplitude", "to absolute value|to a fraction of the threshold|relative to threshold" );
  addNumber( "dcamplitude", "DC amplitude", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "dcamplitudesel", "to absolute value" );
  addNumber( "dcamplitudefrac", "Fraction of threshold", 0.9, 0.0, 1.0, 0.01, "1", "%" ).setActivation( "dcamplitudesel", "to a fraction of the threshold" );
  addNumber( "dcamplitudedecr", "Decrement below threshold", 0.1, 0.0, 1000.0, 0.01 ).setActivation( "dcamplitudesel", "relative to threshold" );
  addBoolean( "interactive", "Set dc amplitude interactively", false );
  addNumber( "dcamplitudestep", "Stepsize for entering dc", 0.001, 0.0, 1000.0, 0.001 );

  QVBox *vb = new QVBox( this );

  // edit:
  QHBox *eb = new QHBox( vb );
  new QLabel( "DC current", eb );
  EW = new DoubleSpinBox( 0.0, -1000.0, 1000.0, 0.1, 0.001, "%0.3f", eb );
  UnitLabel = new QLabel( "nA", eb );

  // buttons:
  QHBox *bb = new QHBox( vb );
  bb->setSpacing( 4 );

  // Ok button:
  OKButton = new QPushButton( "&Ok", bb, "OkButton" );
  connect( OKButton, SIGNAL( clicked() ),
	   this, SLOT( setValue() ) );
  
  // Cancel button:
  CancelButton = new QPushButton( "&Cancel", bb, "CancelButton" );
  connect( CancelButton, SIGNAL( clicked() ),
	   this, SLOT( keepValue() ) );

  bb->setFixedHeight( OKButton->sizeHint().height() );
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
  postCustomEvent( 3 ); // setStep();
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
  double orgampl = metaData( "Cell" ).number( "dc" );
  DCAmplitude = metaData( "Cell" ).number( "ithresh" );
  if ( dcamplitudesel == 0 )
    DCAmplitude = dcamplitude;
  else if ( dcamplitudesel == 1 )
    DCAmplitude *= dcamplitudefrac;
  else
    DCAmplitude -= dcamplitudedecr;

  if ( interactive ) {
    EW->setValue( DCAmplitude );
    OutData dcsignal( DCAmplitude );
    dcsignal.setTrace( OutCurrent );
    dcsignal.setIdent( "DC=" + Str( DCAmplitude ) + IUnit );
    directWrite( dcsignal );
    message( "DC=<b>" + Str( DCAmplitude ) + "</b> " + IUnit );
    sleep( 0.01 );
    postCustomEvent( 1 ); // setFocus();
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
    postCustomEvent( 2 ); // clearFocus();
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
    metaData( "Cell" ).setNumber( "dc", DCAmplitude );
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


void SetDC::customEvent( QCustomEvent *qce )
{
  if ( qce->type() == QEvent::User+1 ) {
    EW->setFocus();
    connect( EW, SIGNAL( valueChanged( double ) ),
	     this, SLOT( setValue( double ) ) );
  }
  else if ( qce->type() == QEvent::User+2 ) {
    EW->clearFocus();
    disconnect( EW, SIGNAL( valueChanged( double ) ),
		this, SLOT( setValue( double ) ) );
  }
  else if ( qce->type() == QEvent::User+3 ) {
    EW->setStep( number( "dcamplitudestep" ), 0.001 );
  }
}


addRePro( SetDC );

}; /* namespace patchclamp */

#include "moc_setdc.cc"
