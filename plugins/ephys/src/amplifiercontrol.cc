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

#include <relacs/ephys/amplifiercontrol.h>
using namespace relacs;

namespace ephys {


AmplifierControl::AmplifierControl( void )
  : Control( "AmplifierControl", "ephys", "Jan Benda", "1.0", "Apr 16, 2010" )
{

  AmplBox = new QHBoxLayout;
  setLayout( AmplBox );

  BuzzerButton = 0;
  ResistanceButton = 0;
  ResistanceLabel = 0;

  Ampl = 0;
  RMeasure = false;

  MaxResistance = 100.0;
  ResistanceScale = 1.0;

  addNumber( "resistancescale", "Scaling factor for computing R from stdev of voltage trace", ResistanceScale, 0.0, 100000.0, 0.01 );
  addNumber( "maxresistance", "Maximum resistance to be expected for scaling voltage trace", MaxResistance, 0.0, 1000000.0, 10.0, "MOhm" );

  setGlobalKeyEvents();
}


void AmplifierControl::notify( void )
{
  ResistanceScale = number( "resistancescale" );
  MaxResistance = number( "maxresistance" );
}


void AmplifierControl::initDevices( void )
{
  Ampl = dynamic_cast< misc::AmplMode* >( device( "ampl-1" ) );
  if ( Ampl != 0 ) {
    lockMetaData();
    if ( ! metaData().existSection( "Electrode" ) )
      metaData().newSection( "Electrode", "Electrode" );
    Options &mo = metaData().section( "Electrode" );
    mo.unsetNotify();
    if ( ! mo.exist( "Resistance" ) )
      mo.addNumber( "Resistance", "Resistance", 0.0, "MOhm", "%.0f" );
    mo.setNotify();
    unlockMetaData();
    if ( ResistanceButton == 0 && BuzzerButton == 0 ) {
    
      AmplBox->addWidget( new QLabel );

      BuzzerButton = new QPushButton( "Buzz" );
      AmplBox->addWidget( BuzzerButton );
      connect( BuzzerButton, SIGNAL( clicked() ),
	       this, SLOT( buzz() ) );

      AmplBox->addWidget( new QLabel );

      ResistanceButton = new QPushButton( "R" );
      AmplBox->addWidget( ResistanceButton );
      connect( ResistanceButton, SIGNAL( pressed() ),
	       this, SLOT( startResistance() ) );
      connect( ResistanceButton, SIGNAL( released() ),
	       this, SLOT( stopResistance() ) );
    
      QLabel *label = new QLabel( "=" );
      label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
      AmplBox->addWidget( label );

      ResistanceLabel = new QLabel( "000" );
      ResistanceLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
      ResistanceLabel->setFrameStyle( QFrame::Panel | QFrame::Sunken );
      ResistanceLabel->setLineWidth( 2 );
      QFont nf( ResistanceLabel->font() );
      nf.setPointSizeF( 1.6 * nf.pointSizeF() );
      nf.setBold( true );
      ResistanceLabel->setFont( nf );
      QPalette qp( ResistanceLabel->palette() );
      qp.setColor( QPalette::Window, Qt::black );
      qp.setColor( QPalette::WindowText, Qt::green );
      ResistanceLabel->setPalette( qp );
      ResistanceLabel->setFixedHeight( ResistanceLabel->sizeHint().height() );
      ResistanceLabel->setFixedWidth( ResistanceLabel->sizeHint().width() );
      ResistanceLabel->setText( "0" );
      AmplBox->addWidget( ResistanceLabel );

      AmplBox->addWidget( new QLabel( "MOhm" ) );
    
      AmplBox->addWidget( new QLabel );
    }
    widget()->show();
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
  if ( Ampl != 0 && SpikeTrace[0] >= 0 && ! RMeasure ) {
    lock();
    DGain = trace( SpikeTrace[0] ).gainIndex();
    adjustGain( trace( SpikeTrace[0] ), MaxResistance / ResistanceScale );
    unlock();
    activateGains();
    Ampl->resistance();
    RMeasure = true;
  }
}


void AmplifierControl::measureResistance( void )
{
  if ( Ampl != 0 && SpikeTrace[0] >= 0 && RMeasure ) {
    lock();
    double r = trace( SpikeTrace[0] ).stdev( currentTime() - 0.05,
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
  if ( Ampl != 0 && SpikeTrace[0] >= 0 && RMeasure ) {
    Ampl->manual();
    lock();
    setGain( trace( SpikeTrace[0] ), DGain );
    unlock();
    activateGains();
    RMeasure = false;
  }
}


void AmplifierControl::buzz( void )
{
  if ( Ampl != 0 ) {
    Ampl->buzzer( );
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
