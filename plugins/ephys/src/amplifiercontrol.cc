/*
  ephys/amplifiercontrol.cc
  Controls an amplifier: buzzer and resistance measurement.

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

#include <QTimer>
#include <relacs/ephys/amplifiercontrol.h>
using namespace relacs;

namespace ephys {


AmplifierControl::AmplifierControl( void )
  : Control( "AmplifierControl", "ephys", "Jan Benda", "3.0", "Oct 5, 2015" )
{
  AmplBox = new QVBoxLayout;
  setLayout( AmplBox );

  BuzzBox = 0;
  BuzzerButton = 0;
  ResistanceButton = 0;
  ResistanceLabel = 0;
  ModeBox = 0;
  BridgeButton = 0;
  CCButton = 0;
  DCButton = 0;
  VCButton = 0;
  ManualButton = 0;
  DCPulseBox = 0;
  SyncPulseSpinBox = 0;
  SyncModeSpinBox = 0;

  Ampl = 0;
  RMeasure = false;
  SyncPulseEnabled = false;
  SyncPulseDuration = 0.00001;
  SyncMode = 0;
  MaxResistance = 100.0;
  ResistanceCurrent = 1.0;
  BuzzPulse = 0.5;
  DoBuzz = false;

  addSelection( "initmode", "Initial mode of the amplifier", "Bridge|Current-clamp|Dynamic-clamp|Voltage-clamp|Manual selection" );
  addNumber( "resistancecurrent", "The average current of the amplifier used for measuring electrode resistance", ResistanceCurrent, 0.0, 100000.0, 0.01, "nA" );
  addBoolean( "adjust", "Adjust input gain for resistance measurement", false );
  addNumber( "maxresistance", "Maximum resistance to be expected for scaling voltage trace", MaxResistance, 0.0, 1000000.0, 10.0, "MOhm" ).setActivation( "adjust", "true" );
  addNumber( "buzzpulse", "Duration of buzz pulse", BuzzPulse, 0.0, 100.0, 0.1, "s", "ms" );
  addBoolean( "showbridge", "Make bridge mode for amplifier selectable", true );
  addBoolean( "showcc", "Make current clamp mode for amplifier selectable", false );
  addBoolean( "showdc", "Make dynamic clamp mode for amplifier selectable", false );
  addBoolean( "showvc", "Make voltage clamp mode for amplifier selectable", false );
  addBoolean( "showmanual", "Make manual mode for amplifier selectable", false );
  addNumber( "syncpulse", "Duration of SEC current injection", SyncPulseDuration, 0.0, 0.1, 0.000001, "s", "us" );
  addInteger( "syncmode", "Interval is average over", SyncMode, 0, 1000 ).setUnit( "samples" );

  setGlobalKeyEvents();
}


void AmplifierControl::notify( void )
{
  SyncPulseDuration = number( "syncpulse" );
  if ( SyncPulseSpinBox != 0 )
    SyncPulseSpinBox->setValue( 1.0e6*SyncPulseDuration );
  SyncMode = integer( "syncmode" );
  if ( SyncModeSpinBox != 0 )
    SyncModeSpinBox->setValue( SyncMode );
  // initial mode:
  if ( changed( "initmode" ) ) {
    int initmode = index( "initmode" );
    switch ( initmode ) {
    case 1 : activateCurrentClampMode();
      break;
    case 2 : activateDynamicClampMode();
      break;
    case 3 : activateVoltageClampMode();
      break;
    case 4 : manualSelection();
      break;
    default :
      activateBridgeMode();
    }
  }
  // show mode selections:
  if ( BridgeButton != 0 ) {
    if ( boolean( "showbridge" ) &&
	 ( ( Ampl !=0 && Ampl->supportsBridgeMode() ) || simulation() ) )
      BridgeButton->show();
    else
      BridgeButton->hide();
  }
  if ( CCButton != 0 ) {
    if ( boolean( "showcc" ) &&
	 ( ( Ampl !=0 && Ampl->supportsCurrentClampMode() ) || simulation() ) )
      CCButton->show();
    else
      CCButton->hide();
  }
  if ( DCButton != 0 ) {
    if ( boolean( "showdc" ) &&
	 ( ( Ampl !=0 && Ampl->supportsDynamicClampMode() ) || simulation() ) )
      DCButton->show();
    else
      DCButton->hide();
  }
  if ( DCPulseBox != 0 ) {
    if ( boolean( "showdc" ) &&
	 ( ( Ampl !=0 && Ampl->supportsDynamicClampMode() ) || simulation() ) )
      DCPulseBox->show();
    else
      DCPulseBox->hide();
  }
  if ( VCButton != 0 ) {
    if ( boolean( "showvc" ) &&
	 ( ( Ampl !=0 && Ampl->supportsVoltageClampMode() ) || simulation() ) )
      VCButton->show();
    else
      VCButton->hide();
  }
  if ( ManualButton != 0 ) {
    if ( boolean( "showmanual" ) )
      ManualButton->show();
    else
      ManualButton->hide();
  }
  ResistanceCurrent = number( "resistancecurrent" );
  MaxResistance = number( "maxresistance" );
  BuzzPulse = number( "buzzpulse" );
  Adjust = number( "adjust" );
}


void AmplifierControl::initDevices( void )
{
  Ampl = dynamic_cast< misc::AmplMode* >( device( "ampl-1" ) );
  if ( Ampl == 0 && ! simulation() ) {
    widget()->hide();
    return;
  }

  // add meta data:
  lockMetaData();
  if ( ! metaData().existSection( "Electrode" ) )
    metaData().newSection( "Electrode", "Electrode", metaData().saveFlags() );
  Options &mo = metaData().section( "Electrode" );
  mo.unsetNotify();
  if ( ! mo.exist( "Resistance" ) )
    mo.addNumber( "Resistance", "Resistance", 0.0, "MOhm", "%.0f" ).addFlags( metaData().saveFlags() );
  mo.setNotify();
  unlockMetaData();
  // add buzzer and resistance widgets:
  if ( BuzzBox == 0 ) {
    AmplBox->addWidget( new QLabel );
    BuzzBox = new QHBoxLayout;
    AmplBox->addLayout( BuzzBox );
    
    BuzzBox->addWidget( new QLabel );

    BuzzerButton = new QPushButton( "Bu&zz" );
    BuzzBox->addWidget( BuzzerButton );
    connect( BuzzerButton, SIGNAL( clicked() ),
	     this, SLOT( startBuzz() ) );

    BuzzBox->addWidget( new QLabel );

    ResistanceButton = new QPushButton( "R" );
    BuzzBox->addWidget( ResistanceButton );
    connect( ResistanceButton, SIGNAL( pressed() ),
	     this, SLOT( startResistance() ) );
    connect( ResistanceButton, SIGNAL( released() ),
	     this, SLOT( stopResistance() ) );
    
    QLabel *label = new QLabel( "=" );
    label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    BuzzBox->addWidget( label );

    ResistanceLabel = new QLabel( "000" );
    ResistanceLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
    ResistanceLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    ResistanceLabel->setLineWidth( 2 );
    QFont nf( ResistanceLabel->font() );
    nf.setPointSizeF( 1.6 * nf.pointSizeF() );
    nf.setBold( true );
    ResistanceLabel->setFont( nf );
    ResistanceLabel->setAutoFillBackground( true );
    QPalette qp( ResistanceLabel->palette() );
    qp.setColor( QPalette::Window, Qt::black );
    qp.setColor( QPalette::WindowText, Qt::green );
    ResistanceLabel->setPalette( qp );
    ResistanceLabel->setFixedHeight( ResistanceLabel->sizeHint().height() );
    ResistanceLabel->setFixedWidth( ResistanceLabel->sizeHint().width() );
    ResistanceLabel->setText( "0" );
    BuzzBox->addWidget( ResistanceLabel );

    BuzzBox->addWidget( new QLabel( "M<u>O</u>hm" ) );
    
    BuzzBox->addWidget( new QLabel );
    AmplBox->addWidget( new QLabel );
  }
  // add stimulus data:
  lockStimulusData();
  stimulusData().addText( "AmplifierMode", "Amplifier mode", "Bridge" );
  unlockStimulusData();
  // add mode selection widgets:
  QVBoxLayout *vbox = 0;
  if ( ModeBox == 0 ) {
    ModeBox = new QGroupBox( "Amplifier mode" );
    BridgeButton = new QRadioButton( "&Bridge" );
    BridgeButton->setChecked( true );
    connect( BridgeButton, SIGNAL( clicked( bool ) ), this, SLOT( activateBridgeMode( bool ) ) );
    CCButton = new QRadioButton( "&Current-clamp" );
    connect( CCButton, SIGNAL( clicked( bool ) ), this, SLOT( activateCurrentClampMode( bool ) ) );
    DCButton = new QRadioButton( "D&ynamic-clamp" );
    connect( DCButton, SIGNAL( clicked( bool ) ), this, SLOT( activateDynamicClampMode( bool ) ) );
    VCButton = new QRadioButton( "&Voltage-clamp" );
    connect( VCButton, SIGNAL( clicked( bool ) ), this, SLOT( activateVoltageClampMode( bool ) ) );
    ManualButton = new QRadioButton( "M&anual selection" );
    connect( ManualButton, SIGNAL( clicked( bool ) ), this, SLOT( manualSelection( bool ) ) );
    vbox = new QVBoxLayout;
    vbox->addWidget( BridgeButton );
    vbox->addWidget( CCButton );
    vbox->addWidget( DCButton );
    vbox->addWidget( VCButton );
    vbox->addWidget( ManualButton );
    ModeBox->setLayout( vbox );
    AmplBox->addWidget( ModeBox );
    AmplBox->addWidget( new QLabel );
  }
  if ( ! boolean( "showbridge" ) || ( ! simulation() && ! Ampl->supportsBridgeMode() ) )
    BridgeButton->hide();
  else
    BridgeButton->show();
  if ( ! boolean( "showcc" ) || ( ! simulation() && ! Ampl->supportsCurrentClampMode() ) )
    CCButton->hide();
  else
    CCButton->show();
  if ( ! boolean( "showdc" ) || ( ! simulation() && ! Ampl->supportsDynamicClampMode() ) )
    DCButton->hide();
  else
    DCButton->show();
  if ( ! boolean( "showvc" ) || ( ! simulation() && ! Ampl->supportsVoltageClampMode() ) )
    VCButton->hide();
  else
    VCButton->show();
  if ( ! boolean( "showmanual" ) )
    ManualButton->hide();
  else
    ManualButton->show();
  // add amplifier synchronization for dynamic clamp:
  if ( simulation() || ( Ampl !=0 && Ampl->supportsDynamicClampMode() ) ) {
    lockStimulusData();
    double spd = 0.0;
    if ( SyncPulseEnabled )
      spd = 1.0e6*SyncPulseDuration;
    stimulusData().addNumber( "SyncPulse", "Synchronization pulse", spd, "us" );
    stimulusData().addInteger( "SyncMode", "Synchronization average", SyncMode ).setUnit( "samples" );
    unlockStimulusData();
    if ( DCPulseBox == 0 && SyncPulseSpinBox == 0 && vbox != 0 ) {
      vbox->addWidget( new QLabel );
      DCPulseBox = new QWidget;
      vbox->addWidget( DCPulseBox );
      QGridLayout *gbox = new QGridLayout;
      gbox->setContentsMargins( 0, 0, 0, 0 );
      DCPulseBox->setLayout( gbox );
      QLabel *label = new QLabel( "DC pulse duration" );
      gbox->addWidget( label, 0, 0 );
      SyncPulseSpinBox = new DoubleSpinBox;
      SyncPulseSpinBox->setRange( 1.0, 1000.0 );
      SyncPulseSpinBox->setSingleStep( 1.0 );
      SyncPulseSpinBox->setPrecision( 2 );
      SyncPulseSpinBox->setKeyboardTracking( false );
      SyncPulseSpinBox->setValue( 1.0e6 * SyncPulseDuration );
      connect( SyncPulseSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( setSyncPulse( double ) ) );
      gbox->addWidget( SyncPulseSpinBox, 0, 1 );
      label = new QLabel( "microseconds" );
      gbox->addWidget( label, 0, 2 );

      label = new QLabel( "DC average over" );
      gbox->addWidget( label, 1, 0 );
      SyncModeSpinBox = new QSpinBox;
      SyncModeSpinBox->setRange( 0, 1000 );
      SyncModeSpinBox->setSingleStep( 1 );
      SyncModeSpinBox->setKeyboardTracking( false );
      SyncModeSpinBox->setValue( SyncMode );
      connect( SyncModeSpinBox, SIGNAL( valueChanged( int ) ), this, SLOT( setSyncMode( int ) ) );
      gbox->addWidget( SyncModeSpinBox, 1, 1 );
      label = new QLabel( "samples" );
      gbox->addWidget( label, 1, 2 );

    }
    if ( ! boolean( "showdc" ) )
      DCPulseBox->hide();
    else
      DCPulseBox->show();
  }
  else if ( DCPulseBox != 0 )
    DCPulseBox->hide();
  widget()->show();
  // initial mode:
  int initmode = index( "initmode" );
  switch ( initmode ) {
  case 1 : activateCurrentClampMode();
    break;
  case 2 : activateDynamicClampMode();
    break;
  case 3 : activateVoltageClampMode();
    break;
  case 4 : manualSelection();
    break;
  default :
    activateBridgeMode();
  }
}


void AmplifierControl::modeChanged( void )
{
  if ( idle() )
    manualSelection();
}


class AmplifierEvent : public QEvent
{

public:

  AmplifierEvent( const string &ls )
    : QEvent( Type( User+11 ) ),
      LabelStr( ls )
  {
  }

  string LabelStr;
};


void AmplifierControl::startResistance( void )
{
  if ( Ampl != 0 && ! RMeasure ) {
    RMeasure = true;
    if ( Adjust && SpikeTrace[0] >= 0 ) {
      lock();
      getData();
      DGain = trace( SpikeTrace[0] ).gainIndex();
      adjustGain( trace( SpikeTrace[0] ), MaxResistance / ResistanceCurrent );
      unlock();
      activateGains();
    }
    muteAudioMonitor();
    QTimer::singleShot( 50, this, SLOT( doResistance() ) );
  }
}


void AmplifierControl::doResistance( void )
{
  if ( Ampl != 0 && RMeasure )
    Ampl->startResistance();
}


void AmplifierControl::measureResistance( void )
{
  if ( Ampl != 0 && RMeasure ) {
    int intrace = SpikeTrace[0];
    if ( intrace < 0 )
      intrace = 0;
    lock();
    getData();
    const InData &data = trace( intrace );
    double mv = data.mean( currentTime()-0.5, currentTime() );
    unlock();
    double r = mv/ResistanceCurrent;
    QCoreApplication::postEvent( this, new AmplifierEvent( Str( r, "%.0f" ) ) );
    lockMetaData();
    metaData().setNumber( "Electrode>Resistance", r );
    unlockMetaData();
  }
}


void AmplifierControl::stopResistance( void )
{
  if ( simulation() ) {
    lockMetaData();
    metaData().setNumber( "Electrode>Resistance", 80.0 );
    unlockMetaData();
  }
  else if ( Ampl != 0 && RMeasure ) {
    measureResistance();
    Ampl->stopResistance();
    if ( Adjust && SpikeTrace[0] >= 0 ) {
      lock();
      setGain( trace( SpikeTrace[0] ), DGain );
      unlock();
      activateGains();
    }
    QTimer::singleShot( 300, this, SLOT( unmuteAudioMonitor() ) );

    RMeasure = false;
  }
}


void AmplifierControl::startBuzz( void )
{
  if ( Ampl != 0 && ! DoBuzz ) {
    DoBuzz = true;
    muteAudioMonitor();
    QTimer::singleShot( 100, this, SLOT( doBuzz() ) );
  }
}


void AmplifierControl::doBuzz( void )
{
  if ( Ampl != 0 && DoBuzz ) {
    Ampl->startBuzz( );
    QTimer::singleShot( int( 1000.0*BuzzPulse ), this, SLOT( stopBuzz() ) );
  }
}


void AmplifierControl::stopBuzz( void )
{
  if ( Ampl != 0 && DoBuzz ) {
    Ampl->stopBuzz();
    QTimer::singleShot( 300, this, SLOT( unmuteAudioMonitor() ) );
    DoBuzz = false;
  }
}


void AmplifierControl::activateBridgeMode( bool activate )
{
  if ( BridgeButton != 0 && activate ) {
    if ( Ampl != 0 )
      Ampl->setBridgeMode();
    BridgeButton->setChecked( true );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "Bridge" );
    clearSyncPulse();
    unlockStimulusData();
  }
}


void AmplifierControl::activateCurrentClampMode( bool activate )
{
  if ( CCButton != 0 && activate ) {
    if ( Ampl != 0 )
      Ampl->setCurrentClampMode();
    CCButton->setChecked( true );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "CC" );
    clearSyncPulse();
    unlockStimulusData();
  }
}


void AmplifierControl::activateDynamicClampMode( bool activate )
{
  if ( DCButton != 0 &&
       ( simulation() || ( Ampl != 0 && Ampl->supportsDynamicClampMode() ) ) && activate ) {
    if ( simulation() || Ampl->setDynamicClampMode( SyncPulseDuration, SyncMode ) == 0 ) {
      lockStimulusData();
      stimulusData().setText( "AmplifierMode", "DC" );
      stimulusData().setNumber( "SyncPulse", 1.0e6*SyncPulseDuration );
      stimulusData().setInteger( "SyncMode", SyncMode );
      unlockStimulusData();
      unsetNotify();
      setNumber( "syncpulse", SyncPulseDuration );
      setToDefault( "syncpulse" );
      setInteger( "syncmode", SyncMode );
      setToDefault( "syncmode" );
      setNotify();
      SyncPulseEnabled = true;
      DCButton->setChecked( true );
    }
    else
      activateCurrentClampMode( true );
  }
}


void AmplifierControl::activateVoltageClampMode( bool activate )
{
  if ( VCButton != 0 && activate ) {
    if ( Ampl != 0 )
      Ampl->setVoltageClampMode();
    VCButton->setChecked( true );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "VC" );
    clearSyncPulse();
    unlockStimulusData();
  }
}


void AmplifierControl::manualSelection( bool activate )
{
  if ( ManualButton != 0 && activate ) {
    if ( Ampl != 0 )
      Ampl->setManualSelection();
    ManualButton->setChecked( true );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "Manual" );
    clearSyncPulse();
    unlockStimulusData();
  }
}


void AmplifierControl::clearSyncPulse( void )
{
  if ( simulation() || ( Ampl != 0 && Ampl->supportsDynamicClampMode() ) ) {
    stimulusData().setNumber( "SyncPulse", 0.0 );
    stimulusData().setInteger( "SyncMode", -1 );
  }
  SyncPulseEnabled = false;
}


void AmplifierControl::setSyncPulse( double durationus )
{
  SyncPulseDuration = 1.0e-6 * durationus;
  unsetNotify();
  setNumber( "syncpulse", SyncPulseDuration );
  setToDefault( "syncpulse" );
  setNotify();
  if ( ( simulation() || ( Ampl != 0 && Ampl->supportsDynamicClampMode() ) ) && SyncPulseEnabled ) {
    if ( simulation() || Ampl->setDynamicClampMode( SyncPulseDuration, SyncMode ) == 0 ) {
      lockStimulusData();
      stimulusData().setNumber( "SyncPulse", 1.0e6*SyncPulseDuration );
      stimulusData().setInteger( "SyncMode", SyncMode );
      unlockStimulusData();
    }
  }
}


void AmplifierControl::setSyncMode( int mode )
{
  SyncMode = mode;
  unsetNotify();
  setInteger( "syncmode", SyncMode );
  setToDefault( "syncmode" );
  setNotify();
  if ( ( simulation() || ( Ampl != 0 && Ampl->supportsDynamicClampMode() ) ) && SyncPulseEnabled ) {
    if ( simulation() || Ampl->setDynamicClampMode( SyncPulseDuration, SyncMode ) == 0 ) {
      lockStimulusData();
      stimulusData().setNumber( "SyncPulse", 1.0e6*SyncPulseDuration );
      stimulusData().setInteger( "SyncMode", SyncMode );
      unlockStimulusData();
    }
  }
}


void AmplifierControl::keyPressEvent( QKeyEvent *e )
{
  e->ignore();
  switch ( e->key() ) {

  case Qt::Key_O:
    if ( Ampl != 0 && ResistanceButton != 0 ) {
      ResistanceButton->setDown( true );
      startResistance();
      e->accept();
    }
    break;

  case Qt::Key_Period:
  case Qt::Key_Z:
    if ( Ampl != 0 && BuzzerButton != 0 ) {
      BuzzerButton->animateClick();
      e->accept();
    }
    break;

  default:
    Control::keyPressEvent( e );

  }
}


void AmplifierControl::keyReleaseEvent( QKeyEvent *e )
{
  switch ( e->key() ) {

  case Qt::Key_O: 
    if ( Ampl != 0 && ResistanceButton != 0 ) {
      measureResistance();
      if ( ! e->isAutoRepeat() ) {
	ResistanceButton->setDown( false );
	stopResistance();
      }
    }
    else
      e->ignore();
    break;

  default:
    Control::keyReleaseEvent( e );

  }
}


void AmplifierControl::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    AmplifierEvent *ae = dynamic_cast<AmplifierEvent*>( qce );
    ResistanceLabel->setText( ae->LabelStr.c_str() );
    break;
  }
  default:
    RELACSPlugin::customEvent( qce );
  }
}


addControl( AmplifierControl, ephys );

}; /* namespace ephys */

#include "moc_amplifiercontrol.cc"
