/*
  ephys/setleak.cc
  Set leak resistance and reversal potential of a dynamic clamp model.

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <relacs/ephys/setleak.h>
using namespace relacs;

namespace ephys {


SetLeak::SetLeak( void )
  : RePro( "SetLeak", "ephys", "Jan Benda", "1.2", "Jan 1, 2021" )
{
  HaveC = true;

  // add some options:
  addBoolean( "interactive", "Set values interactively", true ).setFlags( 1 );
  addSelection( "preset", "Set g, E, and C to", "previous|zero|custom" ).setUnit( "values" ).setFlags( 1 );
  addNumber( "g", "New value of leak conductance", 0.0, -100000.0, 100000.0, 0.1, "nS" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "E", "New value of leak reversal potential", 0.0, -10000.0, 10000.0, 1.0, "mV" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "C", "New value of membrane capacitance", 0.0, -1.0e9, 1.0e9, 10.0, "pF", "pF", "%.0f" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addBoolean( "reversaltorest", "Set leak reversal-potential to resting potential", true ).setActivation( "preset", "zero", false ).setFlags( 1 );
  addSelection( "settau", "Setting membrane time constant changes", "g|C" ).setActivation( "interactive", "true" ).setFlags( 1 );
  addSelection( "involtage", "Input voltage trace for measuring resting potential", "V-1" ).setFlags( 1 );
  addNumber( "duration", "Duration of resting potential measurement", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" ).setFlags( 1 );
  setConfigSelectMask( 1 );
  setDialogSelectMask( 1 );

  // dialog:
  newSection( "Passive membrane properties of the cell:" ).setFlags( 2 );
  addNumber( "Rm", "Resistance R_m", 0.0, 0.0, 1.0e8, 1.0, "MOhm", "MOhm", "%.1f" ).setFlags( 8+32 );
  addNumber( "Cm", "Capacitance C_m", 0.0, 0.0, 1.0e9, 1.0, "pF", "pF", "%.0f" ).setFlags( 8+16+32 );
  addNumber( "Taum", "Time constant tau_m", 0.0, 0.0, 1.0e6, 0.001, "s", "ms", "%.1f" ).setFlags( 8+32 );
  addNumber( "Em", "Leak reversal potential", 0.0, -500.0, 500.0, 0.01, "mV", "mV", "%.1f" ).setFlags( 8+32 );
  newSection( "Injected current I = g (E-V) - C dV/dt:" ).setFlags( 2 );
  addNumber( "gleak", "Additional leak conductance g", 0.0, -1.0e8, 1.0e8, 1.0, "nS" ).setFlags( 2 );
  addNumber( "Eleak", "Added leak current", 0.0, -1000.0, 1000.0, 0.001, "nA", "nA", "%.3f" ).setFlags( 2 );
  addNumber( "Cleak", "Added membrane capacitance C", 0.0, -1.0e9, 1.0e9, 10.0, "pF", "pF", "%.0f" ).setFlags( 4 );
  newSection( "Resulting membrane properties:" ).setFlags( 8 );
  addNumber( "Rnew", "New membrane resistance 1/R=1/R_m+g", 0.0, 0.0, 1.0e8, 1.0, "MOhm" ).setFlags( 8 );
  addNumber( "Enew", "New Leak reversal potential E", 0.0, -1000.0, 1000.0, 1.0, "mV", "mV", "%.1f" ).setFlags( 8 );
  addNumber( "Cnew", "New membrane capacitance Cm + C", 0.0, -1.0e9, 1.0e9, 10.0, "pF", "pF", "%.0f" ).setFlags( 16 );
  addNumber( "taunew", "New membrane time constant", 0.0, 0.0, 1.0e6, 0.001, "s", "ms", "%.1f" ).setFlags( 8 );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );

  // display values:
  STW.assign( (Options*)this, 2, 32, true, 0, mutex() );
  STW.setVerticalSpacing( 2 );
  STW.setMargins( 4 );
  vb->addWidget( &STW );

  QHBoxLayout *bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  // Ok button:
  OKButton = new QPushButton( "&Ok" );
  bb->addWidget( OKButton );
  connect( OKButton, SIGNAL( clicked() ),
	   this, SLOT( setValues() ) );
  grabKey( Qt::ALT+Qt::Key_O );
  grabKey( Qt::Key_Return );
  grabKey( Qt::Key_Enter );

  // Cancel button:
  CancelButton = new QPushButton( "&Cancel" );
  bb->addWidget( CancelButton );
  connect( CancelButton, SIGNAL( clicked() ),
	   this, SLOT( keepValues() ) );
  grabKey( Qt::ALT+Qt::Key_C );
  grabKey( Qt::Key_Escape );

  // Zero button:
  ZeroButton = new QPushButton( "&Zero" );
  bb->addWidget( ZeroButton );
  connect( ZeroButton, SIGNAL( clicked() ),
	   this, SLOT( zeroValues() ) );
  grabKey( Qt::ALT+Qt::Key_Z );

  // E to VRest button:
  VRestButton = new QPushButton( "&E to VRest" );
  bb->addWidget( VRestButton );
  connect( VRestButton, SIGNAL( clicked() ),
	   this, SLOT( measureVRest() ) );
  grabKey( Qt::ALT+Qt::Key_E );

  OKButton->setFixedHeight( OKButton->sizeHint().height() );
  CancelButton->setFixedHeight( OKButton->sizeHint().height() );
  ZeroButton->setFixedHeight( OKButton->sizeHint().height() );
  VRestButton->setFixedHeight( OKButton->sizeHint().height() );
  bb->setSpacing( 4 );
}


void SetLeak::preConfig( void )
{
  setText( "involtage", traceNames() );
  setToDefault( "involtage" );
  lockStimulusData();
  HaveC = stimulusData().exist( "C" );
  unlockStimulusData();
}


void SetLeak::notify( void )
{
  double rm = number( "Rm", 0.0, "MOhm" );
  double gm = 1000.0 / rm;
  double cm = number( "Cm", 0.0, "pF" );
  double em = number( "Em", 0.0, "mV" );

  cerr << "\nbefore adjustments in setleak:\n";
  cerr << "Rm=" << number( "Rm" ) <<        ", changed=" << changed("Rm") << "\n";
  cerr << "Cm=" << number( "Cm" ) <<        ", changed=" << changed("Cm") << "\n";
  cerr << "Taum=" << number( "Taum" ) <<    ", changed=" << changed("Taum") << "\n";
  cerr << "Em=" << number( "Em" ) <<        ", changed=" << changed("Em") << "\n";
  cerr << "gleak=" << number( "gleak" ) <<  ", changed=" << changed("gleak") << "\n";
  cerr << "Eleak=" << number( "Eleak" ) <<  ", changed=" << changed("Eleak") << "\n";
  cerr << "Cleak=" << number( "Cleak" ) <<  ", changed=" << changed("Cleak") << "\n";
  cerr << "Rnew=" << number( "Rnew" ) <<    ", changed=" << changed("Rnew") << "\n";
  cerr << "Enew=" << number( "Enew" ) <<    ", changed=" << changed("Enew") << "\n";
  cerr << "Cnew=" << number( "Cnew" ) <<    ", changed=" << changed("Cnew") << "\n";
  cerr << "taunew=" << number( "taunew" ) << ", changed=" << changed("taunew") <<"\n\n";


  if ( rm > 1.0e-6 && cm > 1.0e-6 ) {
    bool update = true;
    bool sn = unsetNotify();

    // changes in Cell or wanted parameters -> change input
    if ( changed( "Rm" ) or changed( "Em" ) or changed( "Cm" ) or changed( "Rnew" ) or changed( "Enew" ) or changed( "Cnew" ) ) {
      double Rnew = number( "Rnew", 0.0, "MOhm" );
      double Enew = number( "Enew", 0.0, "mV" );
      double Cnew = number( "Cnew", 0.0, "pF" );
      double gnew = 1000.0 / Rnew;
      double gleak = number( "gleak", 0.0, "nS" );

      setNumber( "gleak", gnew - gm, 0.0, "nS" );
      setNumber( "Eleak", (gm + gleak) * Enew - gm * em, 0.0, "pA" );
      setNumber( "Cleak", Cnew - cm, 0.0, "pF" );
      setNumber( "taunew", 1.0e-3 * number( "Rnew", 0.0, "MOhm" ) * number( "Cnew", 0.0, "pF" ), 0.0, "ms" );
    }

    // changes in taunew are special
    else if ( changed( "taunew" ) ) {
      double taunew = number( "taunew", 0.0, "ms" );
      double Rnew = number( "Rnew", 0.0, "MOhm" );
      double Enew = number( "Enew", 0.0, "mV" );
      double gnew = 1000.0 / Rnew;

      if ( SetC ) {
        setNumber( "Cnew", taunew * gnew, 0.0, "pF");
        setNumber( "Cleak", number( "Cnew", 0.0, "pF") - cm, 0.0, "pF" );
      }
      else {
        Rnew = taunew / cm * 1000.0;
        double gleak = 1000.0/Rnew - gm;
        setNumber( "Rnew", Rnew, 0.0, "MOhm" );
        setNumber( "gleak", gleak, 0.0, "nS" );
        setNumber( "Eleak", (gleak + gm) * Enew - gm * em, 0.0, "pA");
      }
    }

    // changes in input -> adapt new cell parameters
    else if ( changed( "gleak" ) or changed( "Eleak" ) or changed( "Cleak" ) ) {
      double gleak = number( "gleak", 0.0, "nS" );
      double Eleak = number( "Eleak", 0.0, "pA" );
      double Cleak = number( "Cleak", 0.0, "pF" );
      double gnew = gleak + gm;

      setNumber( "Rnew", 1000.0/gnew, 0.0, "MOhm" );
      setNumber( "Enew", (gm*em + Eleak) / (gleak + gm), 0.0, "mV" );
      setNumber( "Cnew", Cleak + cm, 0.0, "pF");
      setNumber( "taunew", 1.0e-3 * number( "Rnew", 0.0, "MOhm" ) * number( "Cnew", 0.0, "pF" ), 0.0, "ms" );
    }

    else if ( ! changed( "Eleak" ) )
      update = false;
    setNotify( sn );
    if ( update ) {
      delFlags( Parameter::changedFlag() );
      STW.updateValues();
    }
  }

  cerr << "\nafter adjustments:\n";
  cerr << "Rm="   << number( "Rm" ) << "\n";
  cerr << "Em="   << number( "Em" ) << "\n";
  cerr << "Cm="   << number( "Cm" ) << "\n";
  cerr << "Taum=" << number( "Taum" ) << "\n";
  cerr << "gleak=" << number( "gleak" ) << "\n";
  cerr << "Eleak=" << number( "Eleak" ) << "\n";
  cerr << "Cleak=" << number( "Cleak" ) << "\n";
  cerr << "Rnew=" << number( "Rnew" ) << "\n";
  cerr << "Enew=" << number( "Enew" ) << "\n";
  cerr << "Cnew=" << number( "Cnew" ) << "\n";
  cerr << "taunew=" << number( "taunew" ) << "\n\n";

//  lockMetaData();
//  metaData().setNumber( "Cell>gleak", number( "gleak", 0.0, "nS" ), 0.0, "nS" );
//  metaData().setNumber( "Cell>Eleak", number( "Eleak", 0.0, "pA" ), 0.0, "pA" );
//  metaData().setNumber( "Cell>Cleak", number( "Cleak", 0.0, "pF" ), 0.0, "pF" );
//  metaData().setNumber( "Cell>rnew", number( "Rnew", 0.0, "MOhm" ), 0.0, "MOhm" );
//  metaData().setNumber( "Cell>enew", number( "Enew", 0.0, "pA" ), 0.0, "pA" );
//  metaData().setNumber( "Cell>cnew", number( "Cnew", 0.0, "pF" ), 0.0, "pF" );
//  unlockMetaData();
}


void SetLeak::setValues( void )
{
  Change = true;
  Zero = false;
  STW.accept();
  wake();
}


void SetLeak::keepValues( void )
{
  Change = false;
  Zero = false;
  wake();
}


void SetLeak::zeroValues( void )
{
  Change = true;
  Zero = true;
  wake();
}


void SetLeak::measureVRest( void )
{
  int involtage = traceIndex( settings().text( "involtage", -1 ) );
  if ( involtage < 0 ) 
    return;
  lock();
  double duration = settings().number( "duration" );
  const InData &data = trace( involtage );
  double vrest = data.mean( currentTime()-duration, currentTime() );
  setNumber( "Eleak", vrest );
  STW.updateValue( "Eleak" );  
  unlock();
}


int SetLeak::main( void )
{
  // get options:
  int preset = index( "preset" );
  bool interactive = boolean( "interactive" );
  if ( ! interactive )
    preset = 2;
  double g = number( "g" );
  double E = number( "E" );
  double C = number( "C" );
  bool reversaltorest = boolean( "reversaltorest" );
  SetC = ( index( "settau" ) > 0 );

  noMessage();

  if ( preset == 0 ) {
    // previous values:
    lockStimulusData();
    E = stimulusData().number( "E", 0.0, "mV" );
    g = stimulusData().number( "g", 0.0, "nS" );
    C = stimulusData().number( "C", 0.0, "pF" );
    unlockStimulusData();
  }
  else if ( preset == 1 ) {
    // zero:
    E = 0.0;
    g = 0.0;
    C = 0.0;
  }
  if ( reversaltorest && preset != 1 ) {
    int involtage = traceIndex( text( "involtage", 0 ) );
    if ( involtage >= 0 ) {
      double duration = number( "duration" );
      const InData &data = trace( involtage );
      E = data.mean( currentTime()-duration, currentTime() );
    }  
  }

  unsetNotify();
  lockMetaData();
  setNumber( "Rm", metaData().number( "Cell>rm", 0.0, "MOhm" ), 0.0, "MOhm" );
  setNumber( "Cm", metaData().number( "Cell>cm", 0.0, "pF" ), 0.0, "pF" );
  setNumber( "Taum", metaData().number( "Cell>taum", 0.0, "ms" ), 0.0, "ms" );
  setNumber( "Em", metaData().number( "Cell>em", 0.0, "mV" ), 0.0, "mV" );
  setNumber( "Rnew", metaData().number( "Cell>rnew", 0.0, "MOhm" ), 0.0, "MOhm" );
  setNumber( "Enew", metaData().number( "Cell>enew", 0.0, "mV" ), 0.0, "mV" );
  setNumber( "Cnew", metaData().number( "Cell>cnew", 0.0, "pF" ), 0.0, "pF" );

  cerr << "\n";
  cerr << "Rm: "  <<   number( "Rm"   , 0.0, "MOhm") << "\n";
  cerr << "Cm: " <<    number( "Cm"   , 0.0, "pF") << "\n";
  cerr << "Taum: " <<  number( "Taum" , 0.0, "ms") << "\n";
  cerr << "Em: " <<    number( "Em"   , 0.0, "mV") << "\n";
  cerr << "Rnew: " <<  number( "Rnew" , 0.0, "MOhm") << "\n";
  cerr << "Enew: " <<  number( "Enew" , 0.0, "mV") << "\n";
  cerr << "Cnew: " <<  number( "Cnew" , 0.0, "pF") << "\n";


  unlockMetaData();

  if ( number( "Rnew", 0.0, "MOhm" ) < 1e-6 ) {
    setNumber("gleak", g, 0.0, "nS");
    setNumber("Eleak", E, 0.0, "pA");
    setNumber("Cleak", C, 0.0, "pF");
    setNumber("Rnew", number( "Rm" , 0.0, "MOhm"), 0.0, "MOhm");
    setNumber("Enew", number( "Em" , 0.0, "mV"), 0.0, "mV");
    setNumber("Cnew", number( "Cm" , 0.0, "pF"), 0.0, "pF");
    delFlags( Parameter::changedFlag() );
    addFlags( "gleak", Parameter::changedFlag() );
  }
  else
    delFlags( Parameter::changedFlag() );
    addFlags( "Rnew", Parameter::changedFlag() );

  notify();  // calls already STW.updateValues()
//  if ( HaveC ) {
//    addFlags( "Cleak", Parameter::changedFlag() );
//    notify();
//  }
  setNotify();
//  notify();

  if ( interactive ) {
    postCustomEvent( 13 ); // STW.assign();
    keepFocus();
    postCustomEvent( 11 ); // STW.setFocus();
    // wait for input:
    Change = false;
    Zero = false;
    sleepWait();
    postCustomEvent( 12 ); // clearFocus();
    // set new values:

    if ( Change ) {
      g = Zero ? 0.0 : number( "gleak" );
      E = Zero ? 0.0 : number( "Eleak" );
      C = Zero ? 0.0 : number( "Cleak" );
    }
    else {
      setDefaults();   // calls STW.updateValues() via notify
      return Aborted;
    }
  }

  // set the requested values:
  string msg = "set <b>g=" + Str( g ) + "nS</b>, <b>E=" + Str( E ) + "mV</b>";
  if ( HaveC )
    msg += ", <b>C=" + Str( C ) + "pF</b>";
  message( msg );
  OutList signal;
  signal.resize( 2 + HaveC );
  signal[0].setTraceName( "g" );
  signal[0].constWave( g );
  signal[0].setIdent( "g=" + Str( g ) + "nS" );
  signal[1].setTraceName( "E" );
  signal[1].constWave( E );
  signal[1].setIdent( "E=" + Str( E ) + "mV" );
  if ( HaveC ) {
    signal[2].setTraceName( "C" );
    signal[2].constWave( C );
    signal[2].setIdent( "C=" + Str( C ) + "pF" );
  }
  directWrite( signal );
  if ( signal.failed() ) {
    warning( "Failed to write new values: " + signal.errorText() );
    return Failed;
  }

//  unsetNotify();
  setNumber( "Cleak", C );
  setNumber( "gleak", g );
  setNumber( "Eleak", E );

  lockMetaData();
  metaData().setNumber( "Cell>gleak", number( "gleak", 0.0, "nS" ), 0.0, "nS" );
  metaData().setNumber( "Cell>Eleak", number( "Eleak", 0.0, "pA" ), 0.0, "pA" );
  metaData().setNumber( "Cell>Cleak", number( "Cleak", 0.0, "pF" ), 0.0, "pF" );
  metaData().setNumber( "Cell>rnew", number( "Rnew", 0.0, "MOhm" ), 0.0, "MOhm" );
  metaData().setNumber( "Cell>enew", number( "Enew", 0.0, "pA" ), 0.0, "pA" );
  metaData().setNumber( "Cell>cnew", number( "Cnew", 0.0, "pF" ), 0.0, "pF" );
  unlockMetaData();

  unsetNotify();
  setToDefaults();
//  notify();  // calls already STW.updateValues()
  setNotify();


  sleep( 0.01 );
  return Completed;
}


void SetLeak::keyPressEvent( QKeyEvent *e )
{
  e->ignore();
  if ( e->key() == Qt::Key_O && ( e->modifiers() & Qt::AltModifier ) ) {
    OKButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_C && ( e->modifiers() & Qt::AltModifier ) ) {
    CancelButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_Z && ( e->modifiers() & Qt::AltModifier ) ) {
    ZeroButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_E && ( e->modifiers() & Qt::AltModifier ) ) {
    VRestButton->animateClick();
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


void SetLeak::customEvent( QEvent *qce )
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
  case 13: {
    lock();
    double rm = number( "Rm", 0.0, "MOhm" );
    double cm = number( "Cm", 0.0, "pF" );
    unlock();
    bool havemembrane = ( rm > 1.0e-6 && cm > 1.0e-6 );
    STW.assign( (Options*)this, 2+4*HaveC + (8+16*HaveC)*havemembrane, 32, true, 0, mutex() );
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( SetLeak, ephys );

}; /* namespace ephys */

#include "moc_setleak.cc"
