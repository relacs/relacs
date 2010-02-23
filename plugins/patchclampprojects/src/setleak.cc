/*
  patchclampprojects/setleak.cc
  Set leak resistance and reversal potential of a dynamic clamp model.

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

#include <qhbox.h>
#include <relacs/patchclampprojects/setleak.h>
using namespace relacs;

namespace patchclampprojects {


SetLeak::SetLeak( void )
  : RePro( "SetLeak", "SetLeak", "patchclampprojects",
	   "Jan Benda", "1.0", "Mar 21, 2009" ),
    STW( (QWidget*)this )
{
  // add some options:
  addBoolean( "interactive", "Set values interactively", true ).setFlags( 1 );
  addSelection( "preset", "Set g and E to", "previous|zero|custom" ).setUnit( "values" ).setFlags( 1 );
  addNumber( "g", "New value of leak conductance", 0.0, -100000.0, 100000.0, 0.1, "nS" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "E", "New value of leak reversal-potential", 0.0, -10000.0, 10000.0, 1.0, "mV" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addBoolean( "reversaltorest", "Set leak reversal-potential to resting potential", true ).setActivation( "preset", "zero", false ).setFlags( 1 );
  setConfigSelectMask( 1 );
  setDialogSelectMask( 1 );

  // dialog:
  addLabel( "Passive membrane properties of the cell:" ).setFlags( 2 );
  addNumber( "Rm", "Resistance R_m", 0.0, 0.0, 1.0e8, 1.0, "MOhm", "MOhm", "%.1f" ).setFlags( 2+4 );
  addNumber( "Taum", "Time constant tau_m", 0.0, 0.0, 1.0e6, 0.001, "s", "ms", "%.1f" ).setFlags( 2+4 );
  addLabel( "Injected current I=g(E-V):" ).setFlags( 2 );
  addNumber( "gdc", "Additional leak conductance g", 0.0, -1.0e8, 1.0e8, 1.0, "nS" ).setFlags( 2 );
  addNumber( "Edc", "Reversal potential E", 0.0, -1000.0, 1000.0, 1.0, "mV" ).setFlags( 2 );
  addLabel( "Resulting membrane properties:" ).setFlags( 2 );
  addNumber( "Rdc", "New membrane resistance 1/R=1/R_m+g", 0.0, 0.0, 1.0e8, 1.0, "MOhm" ).setFlags( 2 );
  addNumber( "taudc", "New membrane time constant", 0.0, 0.0, 1.0e6, 0.001, "s", "ms" ).setFlags( 2 );
  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // display values:
  STW.assign( this, 2, 4, true, 0, mutex() );
  updateGeometry();
  STW.setSpacing( 2 );
  STW.setMargin( 4 );

  QHBox *bb = new QHBox( this );
  bb->setSpacing( 4 );

  // Ok button:
  OKButton = new QPushButton( "&Ok", bb, "OkButton" );
  connect( OKButton, SIGNAL( clicked() ),
	   this, SLOT( setValues() ) );
  grabKey( ALT+Key_O );
  grabKey( Key_Return );
  grabKey( Key_Enter );

  // Cancel button:
  CancelButton = new QPushButton( "&Cancel", bb, "CancelButton" );
  connect( CancelButton, SIGNAL( clicked() ),
	   this, SLOT( keepValues() ) );
  grabKey( ALT+Key_C );
  grabKey( Key_Escape );

  // Reset button:
  ResetButton = new QPushButton( "&Reset", bb, "ResetButton" );
  connect( ResetButton, SIGNAL( clicked() ),
	   this, SLOT( resetValues() ) );
  grabKey( ALT+Key_R );

  bb->setFixedHeight( OKButton->sizeHint().height() );
  bb->setSpacing( 4 );

  // tab order
  if ( STW.lastWidget() != 0 )
    setTabOrder( STW.lastWidget(), OKButton );
  setTabOrder( OKButton, CancelButton );
  setTabOrder( CancelButton, ResetButton );
}


void SetLeak::notify( void )
{
  double rm = number("Rm");
  if ( rm > 1.0e-6 ) {
    if ( changed( "gdc" ) ) {
      double rdc = 1.0/(0.001*number( "gdc" )+1.0/rm);
      double cm = metaData( "Cell" ).number( "cm" );
      setNumber( "Rdc", rdc );
      setNumber( "taudc", 1.0e-6*rdc*cm );
    }
    else if ( changed( "Rdc" ) ) {
      double rdc = number( "rdc" );
      double cm = metaData( "Cell" ).number( "cm" );
      setNumber( "gdc", 1000.0/rdc-1000.0/rm );
      setNumber( "taudc", 1.0e-6*rdc*cm );
    }
    else if ( changed( "taudc" ) ) {
      double taudc = number( "taudc" );
      double cm = metaData( "Cell" ).number( "cm" );
      double rdc = 1.0e6*taudc/cm;
      setNumber( "Rdc", rdc );
      setNumber( "gdc", 1000.0/rdc-1000.0/rm );
    }
  }
  delFlags( Parameter::changedFlag() );
  postCustomEvent( 13 );   // STW.updateValues();
}


void SetLeak::setValues( void )
{
  Change = true;
  Reset = false;
  STW.accept();
  wake();
}


void SetLeak::keepValues( void )
{
  Change = false;
  Reset = false;
  wake();
}


void SetLeak::resetValues( void )
{
  Change = true;
  Reset = true;
  wake();
}


int SetLeak::main( void )
{
  // get options:
  int preset = index( "preset" );
  bool interactive = boolean( "interactive" );
  double g = number( "g" );
  double E = number( "E" );
  bool reversaltorest = boolean( "reversaltorest" );

  if ( preset == 0 ) {
    // previous values:
    E = stimulusData().number( "E" );
    g = stimulusData().number( "g" );
  }
  else if ( preset == 1 ) {
    // zero:
    E = 0.0;
    g = 0.0;
  }
  if ( reversaltorest && preset != 1 )
    E = 1000.0*metaData( "Cell" ).number( "vrest" );

  noMessage();

  setNumber( "Rm", metaData( "Cell" ).number( "rm" ) );
  setNumber( "Taum", metaData( "Cell" ).number( "taum" ) );
  setNumber( "Edc", E );
  setNumber( "gdc", g );
  delFlags( Parameter::changedFlag() );
  addFlags( "gdc", Parameter::changedFlag() );
  notify();

  if ( interactive ) {
    postCustomEvent( 11 ); // STW.setFocus();
    // wait for input:
    Change = false;
    Reset = false;
    sleepWait();
    postCustomEvent( 12 ); // STW.clearFocus();
    // set new values:
    if ( Change ) {
      g = Reset ? 0.0 : number( "gdc" );
      E = Reset ? 0.0 : number( "Edc" );
    }
    else {
      setDefaults();
      STW.updateValues();
      return Aborted;
    }
  }

  // set the requested values:
  OutList signal;
  signal.resize( 2 );
  signal[0] = OutData( g );
  signal[0].setIdent( "g=" + Str( g ) + "nS" );
  signal[0].setTraceName( "g" );
  signal[1] = OutData( E );
  signal[1].setIdent( "E=" + Str( E ) + "mV" );
  signal[1].setTraceName( "E" );
  directWrite( signal );
  if ( signal.failed() ) {
    warning( "Failed to write new values: " + signal.errorText() );
    return Failed;
  }
  setNumber( "gdc", g );
  setNumber( "Edc", E );
  
  sleep( 0.01 );
  return Completed;
}


void SetLeak::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Key_O && ( e->state() & AltButton ) ) {
    OKButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Key_C && ( e->state() & AltButton ) ) {
    CancelButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Key_R && ( e->state() & AltButton ) ) {
    ResetButton->animateClick();
    e->accept();
  }
  else if ( ( e->key() == Key_Return || e->key() == Key_Enter ) && ( e->state() & KeyButtonMask ) == 0 ) {
    OKButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Key_Escape && ( e->state() & KeyButtonMask ) == 0 ) {
    CancelButton->animateClick();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void SetLeak::customEvent( QCustomEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    if ( STW.firstWidget() != 0 )
      STW.firstWidget()->setFocus();
  }
  else if ( qce->type() == QEvent::User+12 ) {
    clearFocus();
  }
  else if ( qce->type() == QEvent::User+13 ) {
    STW.updateValues();
  }
  else
    RePro::customEvent( qce );
}


addRePro( SetLeak );

}; /* namespace patchclampprojects */

#include "moc_setleak.cc"

