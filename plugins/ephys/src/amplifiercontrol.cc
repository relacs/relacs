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
  : Control( "AmplifierControl", "ephys", "Jan Benda", "2.4", "Sep 11, 2015" )
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
  SyncSpinBox = 0;

  Ampl = 0;
  DIO = 0;
  RMeasure = false;
  SyncPulseEnabled = false;
  SyncPulseDuration = 0.00001;
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
  addNumber( "syncpulse", "Duration of SEC current injection", SyncPulseDuration, 0.0, 0.1, 0.0000001, "s", "us" );

  setGlobalKeyEvents();
}


void AmplifierControl::notify( void )
{
  SyncPulseDuration = number( "syncpulse" );
  if ( SyncSpinBox != 0 )
    SyncSpinBox->setValue( 1.0e6*SyncPulseDuration );
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
    if ( boolean( "showbridge" ) )
      BridgeButton->show();
    else
      BridgeButton->hide();
  }
  if ( CCButton != 0 ) {
    if ( boolean( "showcc" ) )
      CCButton->show();
    else
      CCButton->hide();
  }
  if ( DCButton != 0 ) {
    if ( boolean( "showdc" ) )
      DCButton->show();
    else
      DCButton->hide();
  }
  if ( VCButton != 0 ) {
    if ( boolean( "showvc" ) )
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
  if ( Ampl != 0 ) {
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
      if ( ! boolean( "showbridge" ) )
	BridgeButton->hide();
      vbox->addWidget( CCButton );
      if ( ! boolean( "showcc" ) )
        CCButton->hide();
      vbox->addWidget( DCButton );
      if ( ! boolean( "showdc" ) )
        DCButton->hide();
      vbox->addWidget( VCButton );
      if ( ! boolean( "showvc" ) )
        VCButton->hide();
      vbox->addWidget( ManualButton );
      if ( ! boolean( "showmanual" ) )
        ManualButton->hide();
      ModeBox->setLayout( vbox );
      AmplBox->addWidget( ModeBox );
      AmplBox->addWidget( new QLabel );
    }
    // add amplifier synchronization:
    DIO = digitalIO( "dio-1" );
    if ( DIO != 0 ) {
      if ( DIO->clearSyncPulse() == Device::InvalidDevice ) {
	DIO = 0;
      }
    }
    if ( DIO != 0 ) {
      lockStimulusData();
      double spd = 0.0;
      if ( SyncPulseEnabled )
	spd = 1.0e6*SyncPulseDuration;
      stimulusData().addNumber( "SyncPulse", "Synchronization pulse", spd, "us" );
      unlockStimulusData();
    }
    else
      DCButton->hide();
    if ( DIO != 0 && SyncSpinBox == 0 && vbox != 0 ) {
      vbox->addWidget( new QLabel );
      QHBoxLayout *hbox = new QHBoxLayout;
      vbox->addLayout( hbox );
      QLabel *label = new QLabel( "Pulse duration" );
      hbox->addWidget( label );
      SyncSpinBox = new DoubleSpinBox;
      SyncSpinBox->setRange( 0.1, 1000.0 );
      SyncSpinBox->setSingleStep( 0.1 );
      SyncSpinBox->setPrecision( 1 );
      SyncSpinBox->setKeyboardTracking( false );
      SyncSpinBox->setValue( 1.0e6 * SyncPulseDuration );
      connect( SyncSpinBox, SIGNAL( valueChanged( double ) ), this, SLOT( setSyncPulse( double ) ) );
      hbox->addWidget( SyncSpinBox );
      label = new QLabel( "microseconds" );
      hbox->addWidget( label );
    }
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
  else {
    widget()->hide();
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
  if ( Ampl != 0 && RMeasure ) {
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
  if ( Ampl != 0 && activate ) {
    Ampl->setBridgeMode();
    BridgeButton->setChecked( true );
    activateSyncPulse( false );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "Bridge" );
    unlockStimulusData();
  }
}


void AmplifierControl::activateCurrentClampMode( bool activate )
{
  if ( Ampl != 0 && activate ) {
    Ampl->setCurrentClampMode();
    CCButton->setChecked( true );
    activateSyncPulse( false );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "CC" );
    unlockStimulusData();
  }
}


void AmplifierControl::activateDynamicClampMode( bool activate )
{
  if ( Ampl != 0 && activate ) {
    Ampl->setDynamicClampMode();
    DCButton->setChecked( true );
    activateSyncPulse( activate );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "DC" );
    unlockStimulusData();
  }
}


void AmplifierControl::activateVoltageClampMode( bool activate )
{
  if ( Ampl != 0 && activate ) {
    Ampl->setVoltageClampMode();
    VCButton->setChecked( true );
    activateSyncPulse( false );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "VC" );
    unlockStimulusData();
  }
}


void AmplifierControl::manualSelection( bool activate )
{
  if ( Ampl != 0 && activate ) {
    Ampl->setManualSelection();
    ManualButton->setChecked( true );
    activateSyncPulse( false );
    lockStimulusData();
    stimulusData().setText( "AmplifierMode", "Manual" );
    unlockStimulusData();
  }
}


void AmplifierControl::activateSyncPulse( bool activate )
{
  if ( DIO != 0 ) {
    if ( activate ) {
      if ( DIO->setSyncPulse( SyncPulseDuration ) == 0 ) {
	lockStimulusData();
	stimulusData().setNumber( "SyncPulse", 1.0e6*SyncPulseDuration );
	unlockStimulusData();
	unsetNotify();
	setNumber( "syncpulse", SyncPulseDuration );
	setDefault( "syncpulse" );
	setNotify();
      }
      else
	activateCurrentClampMode( true );
    }
    else {
      DIO->clearSyncPulse();
      lockStimulusData();
      stimulusData().setNumber( "SyncPulse", 0.0 );
      unlockStimulusData();
    }
    SyncPulseEnabled = activate;
  }
}


void AmplifierControl::setSyncPulse( double durationus )
{
  double syncpulseduration = 1.0e-6 * durationus;
  SyncPulseDuration = syncpulseduration;
  unsetNotify();
  setNumber( "syncpulse", SyncPulseDuration );
  setToDefault( "syncpulse" );
  setNotify();
  if ( DIO != 0 && SyncPulseEnabled ) {
    if ( DIO->setSyncPulse( syncpulseduration ) == 0 ) {
      activateDynamicClampMode( true );
      lockStimulusData();
      stimulusData().setNumber( "SyncPulse", 1.0e6*SyncPulseDuration );
      unlockStimulusData();
    }
  }
}


void AmplifierControl::keyPressEvent( QKeyEvent *e )
{
  switch ( e->key() ) {

  case Qt::Key_O:
    if ( Ampl != 0 && ResistanceButton != 0 ) {
      ResistanceButton->setDown( true );
      startResistance();
    }
    else
      e->ignore();
    break;

  case Qt::Key_Period:
  case Qt::Key_Z:
    if ( Ampl != 0 && BuzzerButton != 0 ) {
      BuzzerButton->animateClick();
    }
    else
      e->ignore();
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
