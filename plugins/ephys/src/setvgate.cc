/*
  ephys/setvgate.cc
  Set parameter of voltage gated channel for dynamic clamp.

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
#include <relacs/ephys/setvgate.h>
using namespace relacs;

namespace ephys {


SetVGate::SetVGate( void )
  : RePro( "SetVGate", "ephys", "Jan Benda", "1.2", "Jan 4, 2021" )
{
  HaveMembrane = true;
  HaveVGate = true;
  HaveTauV = true;

  // add some options:
  addBoolean( "interactive", "Set values interactively", true ).setFlags( 1 );
  addSelection( "preset", "Set g and E to", "previous|zero|custom" ).setUnit( "values" ).setFlags( 1 );
  addNumber( "g", "Maximum conductance", 0.0, -100000.0, 100000.0, 0.1, "nS" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "E", "Reversal-potential", 0.0, -10000.0, 10000.0, 1.0, "mV" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "vmid", "Position of activation curve", 0.0, -1000.0, 1000.0, 1.0, "mV" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "width", "Width of activation curve", 0.0, -1000.0, 1000.0, 1.0, "mV" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "tau", "Activation time constant", 10.0, 0.0, 100000.0, 1.0, "ms" ).setActivation( "preset", "custom" ).setFlags( 1 );
  addNumber( "delta", "Asymmetry of energy barrier", 0.5, 0.0, 1.0, 0.05 ).setActivation( "preset", "custom" ).setFlags( 1 );
  addBoolean( "reversaltorest", "Set reversal-potential to resting potential", false ).setActivation( "preset", "zero", false ).setFlags( 1 );
  addSelection( "involtage", "Input voltage trace for measuring resting potential", "V-1" ).setFlags( 1 );
  addNumber( "duration", "Duration of resting potential measurement", 0.1, 0.001, 1000.0, 0.001, "sec", "ms" ).setFlags( 1 );
  setConfigSelectMask( 1 );
  setDialogSelectMask( 1 );

  // dialog:
  newSection( "Passive membrane properties of the cell:" ).setFlags( 16 );
  addNumber( "gm", "Conductance g_m", 0.0, 0.0, 1.0e8, 1.0, "nS", "nS", "%.1f" ).setFlags( 16+128 );
  addNumber( "Taum", "Time constant tau_m", 0.0, 0.0, 1.0e6, 0.001, "s", "ms", "%.1f" ).setFlags( 16+128 );
  newSection( "Injected current I=g m (E-V):" ).setFlags( 2 );
  addNumber( "gvgate", "Conductance g", 0.0, -1.0e8, 1.0e8, 1.0, "nS" ).setFlags( 4 );
  addNumber( "Evgate", "Reversal potential E", 0.0, -1000.0, 1000.0, 1.0, "mV", "mV", "%.1f" ).setFlags( 4 );
  addNumber( "vgatevmid", "Position of activation curve Vmid", 0.0, -200.0, 200.0, 1.0, "mV" ).setFlags( 4 );
  addNumber( "vgatewidth", "Width of activation curve 1/k", 10.0, -1000.0, 1000.0, 1.0, "mV" ).setFlags( 4 );
  addNumber( "vgatetau", "Time constant tau", 10.0, 0.0, 1000000.0, 1.0, "ms" ).setFlags( 4 );
  addNumber( "vgatedelta", "Asymmetry of time constant delta", 0.5, 0.0, 1.0, 0.05 ).setFlags( 32 );
  addText( "notext1", "Sorry, the dynamic clamp model does", "" ).setFlags( 8+128 );
  addText( "notext2", "not include a voltage gated current!", "" ).setFlags( 8+128 );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );
  QHBoxLayout *hb = new QHBoxLayout;
  vb->addLayout( hb );

  // display values:
  STW.assign( (Options*)this, 2, 128, true, 0, mutex() );
  STW.setVerticalSpacing( 2 );
  STW.setMargins( 4 );
  hb->addWidget( &STW );

  // plot activation curve:
  P.lock();
  P.resize( 2, 1, true );
  P[0].setXRange( -120.0, 40.0 );
  P[0].setYLabel( "Activation [%]" );
  P[0].setYRange( 0.0, 100.0 );
  P[1].setXRange( -120.0, 40.0 );
  P[1].setXLabel( "Membrane potential [mV]" );
  P[1].setYRange( 0.0, 10.0 );
  P[1].setYLabel( "Time constant [ms]" );
  P.unlock();
  hb->addWidget( &P );

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


void SetVGate::preConfig( void )
{
  setText( "involtage", traceNames() );
  setToDefault( "involtage" );
}


void SetVGate::notify( void )
{
  double vmid = number( "vgatevmid" );
  double width = number( "vgatewidth" );
  if ( ::fabs( width ) < 1e-6 )
    width = width < 0.0 ? -1e-6 : 1e-6;
  double slope = 1.0/width;
  SampleDataD activation( -120.0, 40.0, 0.1 );
  for ( int k=0; k<activation.size(); k++ )
    activation[k] = 100.0/(1.0+::exp(-slope*(activation.pos(k)-vmid)));
  double taumax = number( "vgatetau" );
  SampleDataD timeconstant( -120.0, 40.0, 0.1 );
  if ( HaveTauV ) {
    double delta = number( "vgatedelta" );
    for ( int k=0; k<timeconstant.size(); k++ ) {
      timeconstant[k] = taumax * ::pow(1.0-delta, delta-1.0) / (::exp(delta*slope*(timeconstant.pos(k)-vmid)) + ::exp(-(1.0-delta)*slope*(timeconstant.pos(k)-vmid))) / ::pow(delta, delta);
    }
  }
  else {
    for ( int k=0; k<timeconstant.size(); k++ )
      timeconstant[k] = taumax;
  }
  P.lock();
  P[0].clear();
  P[0].plot( activation, 1.0, Plot::Green, 3, Plot::Solid );
  P[1].clear();
  P[1].plot( timeconstant, 1.0, Plot::Green, 3, Plot::Solid );
  P[1].setYRange( 0.0, 1.1*taumax );
  P.draw();
  P.unlock();
}


void SetVGate::setValues( void )
{
  Change = true;
  Zero = false;
  STW.accept();
  wake();
}


void SetVGate::keepValues( void )
{
  Change = false;
  Zero = false;
  wake();
}


void SetVGate::zeroValues( void )
{
  Change = true;
  Zero = true;
  wake();
}


void SetVGate::measureVRest( void )
{
  int involtage = traceIndex( settings().text( "involtage", 0 ) );
  if ( involtage < 0 ) 
    return;
  lock();
  double duration = settings().number( "duration" );
  const InData &data = trace( involtage );
  double vrest = data.mean( currentTime()-duration, currentTime() );
  setNumber( "Evgate", vrest );
  STW.updateValue( "Evgate" );  
  unlock();
}


int SetVGate::main( void )
{
  // get options:
  int preset = index( "preset" );
  bool interactive = boolean( "interactive" );
  if ( ! interactive )
    preset = 2;
  double g = number( "g" );
  double E = number( "E" );
  double vmid = number( "vmid" );
  double width = number( "width" );
  if ( ::fabs( width ) < 1e-6 )
    width = width < 0.0 ? -1e-6 : 1e-6;
  double slope = 1.0/width;
  double tau = number( "tau" );
  double delta = number( "delta" );
  bool reversaltorest = boolean( "reversaltorest" );

  noMessage();

  lockMetaData();
  double rm = metaData().number( "Cell>rm", 0.0, "MOhm" );
  double cm = metaData().number( "Cell>cm", 0.0, "pF" );
  unlockMetaData();
  HaveMembrane = ( rm > 0.0 );
  lockStimulusData();
  HaveVGate = stimulusData().exist( "vgatevmid" );
  HaveTauV = stimulusData().exist( "vgatetaumax" );
  unlockStimulusData();
  if ( HaveTauV )
    setRequest( "vgatetau", "Maximum time constant tau" );
  else
    setRequest( "vgatetau", "Time constant tau" );

  if ( preset == 0 ) {
    // previous values:
    lockStimulusData();
    E = stimulusData().number( "Evgate", 0.0, "mV" );
    g = stimulusData().number( "gvgate", 0.0, "nS" );
    vmid = stimulusData().number( "vgatevmid", 0.0, "mV" );
    slope = stimulusData().number( "vgateslope", 0.0, "1/mV" );
    if ( ::fabs( slope ) < 1e-2 )
      slope = slope < 0.0 ? -1e-2 : 1e-2;
    width = 1.0/slope;
    if ( HaveTauV )
      tau = stimulusData().number( "vgatetaumax", 0.0, "ms" );
    else
      tau = stimulusData().number( "vgatetau", 0.0, "ms" );
    delta = stimulusData().number( "vgatedelta", 0.5 );
    unlockStimulusData();
  }
  else if ( preset == 1 ) {
    // zero:
    E = 0.0;
    g = 0.0;
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
  double gleak = stimulusData().number( "g", 0.0, "nS" );
  double cleak = stimulusData().number( "C", 0.0, "pF" );
  double rnew = 1.0/(0.001*gleak + 1.0/rm);
  double taunew = 1.0e-6*rnew*(cm+cleak);
  setNumber( "gm", 1000.0/rnew );
  setNumber( "Taum", taunew );
  setNumber( "Evgate", E );
  setNumber( "gvgate", g );
  setNumber( "vgatevmid", vmid );
  setNumber( "vgatewidth", width );
  setNumber( "vgatetau", tau );
  setNumber( "vgatetaumax", tau );
  setNumber( "vgatedelta", delta );
  delFlags( Parameter::changedFlag() );
  addFlags( "gvgate", Parameter::changedFlag() );
  notify();
  postCustomEvent( 13 ); // STW.updateValues();
  setNotify();

  if ( interactive ) {
    postCustomEvent( 14 ); // STW.assign();
    keepFocus();
    postCustomEvent( 11 ); // STW.setFocus();
    // wait for input:
    Change = false;
    Zero = false;
    sleepWait();
    postCustomEvent( 12 ); // clearFocus();
    // set new values:
    if ( Change ) {
      g = Zero ? 0.0 : number( "gvgate" );
      E = Zero ? 0.0 : number( "Evgate" );
      vmid = number( "vgatevmid" );
      width = number( "vgatewidth" );
      if ( ::fabs( width ) < 1e-6 )
	width = width < 0.0 ? -1e-6 : 1e-6;
      slope = 1.0/width;
      tau = number( "vgatetau" );
      delta = number( "vgatedelta" );
    }
    else {
      setDefaults();   // calls STW.updateValues() via notify
      return Aborted;
    }
  }

  // set the requested values:
  string msg = "set <b>g=" + Str( g ) + "nS</b>, <b>E=" + Str( E ) + "mV</b>, <b>Vmid=" + Str( vmid ) + "mV</b>, <b>slope=" + Str( slope ) + "/mV</b>, <b> tau=" + Str( tau ) + "ms</b>";
  if ( HaveTauV )
    msg += ", <b> delta=" + Str( delta ) + "</b>";
  message( msg );
  OutList signal;
  signal.resize( HaveTauV?6:5 );
  signal[0].setTraceName( "gvgate" );
  signal[0].constWave( g );
  signal[0].setIdent( "gvgate=" + Str( g ) + "nS" );
  signal[1].setTraceName( "Evgate" );
  signal[1].constWave( E );
  signal[1].setIdent( "Evgate=" + Str( E ) + "mV" );
  signal[2].setTraceName( "vgatevmid" );
  signal[2].constWave( vmid );
  signal[2].setIdent( "vgatevmid=" + Str( vmid ) + "mV" );
  signal[3].setTraceName( "vgateslope" );
  signal[3].constWave( slope );
  signal[3].setIdent( "vgateslope=" + Str( slope ) + "/mV" );
  signal[4].setTraceName( HaveTauV?"vgatetaumax":"vgatetau" );
  signal[4].constWave( tau );
  signal[4].setIdent( "vgatetau=" + Str( tau ) + "ms" );
  if ( HaveTauV ) {
    signal[5].setTraceName( "vgatedelta" );
    signal[5].constWave( delta );
    signal[5].setIdent( "vgatedelta=" + Str( delta ) );
  }
  directWrite( signal );
  if ( signal.failed() ) {
    warning( "Failed to write new values: " + signal.errorText() );
    return Failed;
  }
  unsetNotify();
  setNumber( "gvgate", g );
  setNumber( "Evgate", E );
  setNumber( "vgatevmid", vmid );
  setNumber( "vgatewidth", width );
  setNumber( "vgatetau", tau );
  setNumber( "vgatedelta", delta );
  setToDefaults();
  notify();  // calls already STW.updateValues()
  setNotify();
  
  sleep( 0.01 );
  return Completed;
}


void SetVGate::keyPressEvent( QKeyEvent *e )
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


void SetVGate::customEvent( QEvent *qce )
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
    STW.updateValues();
    break;
  }
  case 14: {
    int flags = 2;
    flags += HaveMembrane?16:0;
    flags += HaveVGate?4:8;
    if ( HaveVGate && HaveTauV )
      flags += 32;
    STW.assign( (Options*)this, flags, 128, true, 0, mutex() );
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( SetVGate, ephys );

}; /* namespace ephys */

#include "moc_setvgate.cc"
