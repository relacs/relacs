/*
  ephys/amplifiercontrol.cc
  Controls an amplifier: buzzer and resistance measurement.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
  : Control( "AmplifierControl", "ephys", "Jan Benda", "2.0", "Apr 3, 2014" )
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
  VCButton = 0;
  ManualButton = 0;

  Ampl = 0;
  RMeasure = false;

  MaxResistance = 100.0;
  ResistanceScale = 1.0;
  BuzzPulse = 0.5;

  addSelection( "initmode", "Initial mode of the amplifier", "Bridge|Current-clamp|Voltage-clamp|Manual selection" );
  addNumber( "resistancescale", "Scaling factor for computing R from stdev of voltage trace", ResistanceScale, 0.0, 100000.0, 0.01 );
  addNumber( "maxresistance", "Maximum resistance to be expected for scaling voltage trace", MaxResistance, 0.0, 1000000.0, 10.0, "MOhm" );
  addNumber( "buzzpulse", "Duration of buzz pulse", BuzzPulse, 0.0, 100.0, 0.1, "s", "ms" );
  addBoolean( "adjust", "Adjust input gain for resistance measurement", false );

  setGlobalKeyEvents();
}


void AmplifierControl::notify( void )
{
  // initial mode:
  int initmode = index( "initmode" );
  switch ( initmode ) {
  case 1 : activateCurrentClampMode();
    break;
  case 2 : activateVoltageClampMode();
    break;
  case 3 : manualSelection();
    break;
  default :
    activateBridgeMode();
  }
  ResistanceScale = number( "resistancescale" );
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
      metaData().newSection( "Electrode", "Electrode" );
    Options &mo = metaData().section( "Electrode" );
    mo.unsetNotify();
    if ( ! mo.exist( "Resistance" ) )
      mo.addNumber( "Resistance", "Resistance", 0.0, "MOhm", "%.0f" );
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
    // add mode selection widgets:
    if ( ModeBox == 0 ) {
      ModeBox = new QGroupBox( "Amplifier mode" );
      BridgeButton = new QRadioButton( "&Bridge" );
      BridgeButton->setChecked( true );
      connect( BridgeButton, SIGNAL( clicked( bool ) ), this, SLOT( activateBridgeMode( bool ) ) );
      CCButton = new QRadioButton( "&Current-clamp" );
      connect( CCButton, SIGNAL( clicked( bool ) ), this, SLOT( activateCurrentClampMode( bool ) ) );
      VCButton = new QRadioButton( "&Voltage-clamp" );
      connect( VCButton, SIGNAL( clicked( bool ) ), this, SLOT( activateVoltageClampMode( bool ) ) );
      ManualButton = new QRadioButton( "&Manual selection" );
      connect( ManualButton, SIGNAL( clicked( bool ) ), this, SLOT( manualSelection( bool ) ) );
      QVBoxLayout *vbox = new QVBoxLayout;
      vbox->addWidget( BridgeButton );
      vbox->addWidget( CCButton );
      vbox->addWidget( VCButton );
      vbox->addWidget( ManualButton );
      ModeBox->setLayout( vbox );
      AmplBox->addWidget( ModeBox );
      AmplBox->addWidget( new QLabel );
    }
    widget()->show();
    // initial mode:
    int initmode = index( "initmode" );
    switch ( initmode ) {
    case 1 : activateCurrentClampMode();
      break;
    case 2 : activateVoltageClampMode();
      break;
    case 3 : manualSelection();
      break;
    default :
      activateBridgeMode();
    }
  }
  else {
    widget()->hide();
  }
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
    if ( Adjust && SpikeTrace[0] >= 0 ) {
      lock();
      updateData();
      DGain = trace( SpikeTrace[0] ).gainIndex();
      adjustGain( trace( SpikeTrace[0] ), MaxResistance / ResistanceScale );
      unlock();
      activateGains();
    }
    Ampl->startResistance();
    RMeasure = true;
  }
}


void AmplifierControl::measureResistance( void )
{
  if ( Ampl != 0 && RMeasure ) {
    int intrace = SpikeTrace[0];
    if ( intrace < 0 )
      intrace = 0;
    lock();
    updateData();
    double r = trace( intrace ).stdev( currentTime() - 0.05,
				       currentTime() );
    unlock();
    r *= ResistanceScale;
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
    RMeasure = false;
  }
}


void AmplifierControl::startBuzz( void )
{
  if ( Ampl != 0 ) {
    Ampl->startBuzz( );
    QTimer::singleShot( int( 1000.0*BuzzPulse ), this, SLOT( stopBuzz() ) );
  }
}


void AmplifierControl::stopBuzz( void )
{
  if ( Ampl != 0 )
    Ampl->stopBuzz( );
}


void AmplifierControl::activateBridgeMode( bool activate )
{
  if ( Ampl != 0 ) {
    Ampl->setBridgeMode();
    BridgeButton->setChecked( true );
  }
}


void AmplifierControl::activateCurrentClampMode( bool activate )
{
  if ( Ampl != 0 ) {
    Ampl->setCurrentClampMode();
    CCButton->setChecked( true );
  }
}


void AmplifierControl::activateVoltageClampMode( bool activate )
{
  if ( Ampl != 0 ) {
    Ampl->setVoltageClampMode();
    VCButton->setChecked( true );
  }
}


void AmplifierControl::manualSelection( bool activate )
{
  if ( Ampl != 0 ) {
    Ampl->setManualSelection();
    ManualButton->setChecked( true );
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
