/*
  ephys/motorcontrol.cc
  Control widgets for motorized manipulators.

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

#include <unistd.h>
#include <cmath>
#include <QApplication>
#include <QGroupBox>
#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <relacs/manipulator.h>
#include <relacs/ephys/motorcontrol.h>
using namespace relacs;

namespace ephys {


MotorControl::MotorControl( void )
  : Control( "MotorControl", "Control", "EPhys",
	     "Jan Benda", "1.0", "Jul 1 2004" )
{
  MM.clear();

  // parameter:
  MinRate = 20.0;
  MinSize = 20.0;

  // options:
  addNumber( "minrate", "Minimum Rate", MinRate, 0.0, 2000.0, 0.1, "Hz", "Hz", "%.1f" );
  addNumber( "minsize", "Minimum Size", MinSize, 0.0, 900.0, 0.1, "mV", "mV", "%.1f" );

  // variables:
  SpikeEvents.clear();

  // indicators:
  QPainter p;
  int is = fontInfo().pixelSize() * 2;
  QColor orange( 255, 165, 0 );
  GoodQuality = QPixmap( is, is );
  p.begin( &GoodQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( GoodQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::green );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::green ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5+1, 3*is/5+1 );
  p.drawLine( 6*is/10, 6*is/10, 4*is/5, 4*is/5 );
  p.end();

  OkQuality = QPixmap( is, is );
  p.begin( &OkQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( OkQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::yellow );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::yellow ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  PotentialQuality = QPixmap( is, is );
  p.begin( &PotentialQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( PotentialQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( orange );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( orange ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  BadQuality = QPixmap( is, is );
  p.begin( &BadQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( BadQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::red );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::red ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  QPolygon pa( 7 );
  QPixmap BadArrow( is, is );
  p.begin( &BadArrow );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( BadArrow.rect(), Qt::black );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::red );
  pa.setPoint( 0, is/4, 0 );
  pa.setPoint( 1, 3*is/4, 0 );
  pa.setPoint( 2, 3*is/4, 2*is/3 );
  pa.setPoint( 3, is, 2*is/3 );
  pa.setPoint( 4, is/2, is );
  pa.setPoint( 5, 0, 2*is/3 );
  pa.setPoint( 6, is/4, 2*is/3 );
  p.drawPolygon( pa );
  p.end();

  BadTrend = QPixmap( is, is );
  p.begin( &BadTrend );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( BadTrend.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::red );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::red ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  OkTrend = QPixmap( is, is );
  p.begin( &OkTrend );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( OkTrend.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::yellow );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::yellow ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  GoodTrend = QPixmap( is, is );
  p.begin( &GoodTrend );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( GoodTrend.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::green );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::green ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  GoodArrow = QPixmap( is, is );
  p.begin( &GoodArrow );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( GoodArrow.rect(), Qt::black );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::green );
  pa.setPoint( 0, is/4, is-1 );
  pa.setPoint( 1, 3*is/4, is-1 );
  pa.setPoint( 2, 3*is/4, is/3 );
  pa.setPoint( 3, is, is/3 );
  pa.setPoint( 4, is/2, 0 );
  pa.setPoint( 5, 0, is/3 );
  pa.setPoint( 6, is/4, is/3 );
  p.drawPolygon( pa );
  p.end();

  // quality indicator:
  Quality = 0;
  QualityPixs[0] = &BadQuality;
  QualityPixs[1] = &PotentialQuality;
  QualityPixs[2] = &OkQuality;
  QualityPixs[3] = &GoodQuality;

  // trend indicator:
  Trend = 2;
  TrendPixs[0] = &BadArrow;
  TrendPixs[1] = &BadTrend;
  TrendPixs[2] = &OkTrend;
  TrendPixs[3] = &GoodTrend;
  TrendPixs[4] = &GoodArrow;
}


MotorControl::~MotorControl( void )
{
}


void MotorControl::initialize( void )
{
  SpikeEvents.push_back( events().index( "Spikes-1" ) );
  SpikeEvents.push_back( events().index( "Spikes-2" ) );
}


void MotorControl::initDevices( void )
{
  for ( unsigned int k=0; k<SpikeEvents.size(); k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      // get manipulator:
      Str ns( k+1, 0 );
      Manipulator *m = dynamic_cast< Manipulator* >( device( "mimapu-" + ns ) );
      MM.push_back( new MiMaPu( m, k, "Spikes-"+ns, 
				detectorEventsOpts( SpikeEvents[k] ), this ) );
    }
  }

  QHBoxLayout *h = new QHBoxLayout;
  boxLayout()->addLayout( h );
  QPushButton *db = new QPushButton( "Dialog" );
  h->addWidget( db );
  QWidget::connect( db, SIGNAL( clicked() ), (QWidget*)this, SLOT( dialog() ) );
  QPushButton *hb = new QPushButton( "Help" );
  h->addWidget( hb );
  QWidget::connect( hb, SIGNAL( clicked() ), (QWidget*)this, SLOT( help() ) );
}


void MotorControl::notify( void )
{
  MinRate = number( "minrate" );
  MinSize = number( "minsize" );
}


void MotorControl::keyPressEvent( QKeyEvent *e )
{
  switch ( e->key() ) {

  case Qt::Key_Up:
    if ( MM.size() > 0 )
      MM[0]->keyUp( e->QInputEvent::modifiers() );
    break;

  case Qt::Key_Down:
    if ( MM.size() > 0 )
      MM[0]->keyDown( e->QInputEvent::modifiers() );
    break;

  case Qt::Key_Left:
    if ( MM.size() > 1 )
      MM[1]->keyUp( e->QInputEvent::modifiers() );
    break;

  case Qt::Key_Right:
    if ( MM.size() > 1 )
      MM[1]->keyDown( e->QInputEvent::modifiers() );
    break;

  case Qt::Key_G:
    if ( MM.size() > 0 )
      MM[0]->find();
    break;

  case Qt::Key_H:
    if ( MM.size() > 1 )
      MM[1]->find();
    break;

  default:
    e->ignore();

  }
}


int MotorControl::spikes( int trace )
{
  Str ns( trace+1, 0 );

  return 0;
  /*
XXXX instead of sessionOpts we should access the spikedtector directly!
  return ( sessionOpts().number( "firingrate"+ns ) > MinRate &&
	   detectorOpts( SpikeEvents[trace] ).number( "size" ) > MinSize );
  */
}


MiMaPu::MiMaPu( Manipulator *m,	int trace, const string &title,
		const Options &detect,MotorControl *parent )
  : QObject( (QWidget*)parent ), 
    ConfigClass( "MiMaPu", RELACSPlugin::Plugins, Save, 0 ), 
    MC( parent ), 
    Trace( trace ),
    M( m ),
    Interval( 0.7 ),
    Steps( 2 ),
    Back( 0 ),
    Dir( true ),
    Amplitude( 80 ),
    AmplAsymm( 0.8 ),
    OW( 0 ),
    QualityIndicator( 0 ),
    TrendIndicator( 0 ),
    Detect( detect ),
    Find( false ),
    StartFind( false ),
    FindButton( 0 )
{
  setConfigIdent( "MiMaPu-" + Str( trace+1, 0 ) );

  QGroupBox *g = new QGroupBox( title.c_str() );
  MC->boxLayout()->addWidget( g );
  QVBoxLayout *mbox = new QVBoxLayout;
  g->setLayout( mbox );

  // manipulator:
  if ( M != 0 ) {
    addNumber( "pos", "Pos", 0.0, -100000.0, 100000.0, 1.0, "um", "um", "%6.1f",
	       1+2, OptWidget::ValueBold + OptWidget::ValueRed + OptWidget::ValueBackBlack );
    addInteger( "steps", "Steps", Steps, 1, 10000, 1 ).setFlags( 1 );
    addInteger( "back", "Back", Back, 0, 10000, 1 ).setFlags( 1 );
    addInteger( "amplitude", "Amplitude", Amplitude, 1, 80, 5 ).setFlags( 1 );
    addNumber( "amplasymm", "Asymm", AmplAsymm, 0.0, 1.0, 0.01, "", "", "%4.2f", 0 );
    addNumber( "interval", "Interval", Interval, 0.0, 5.0, 0.05, "sec", "ms", "%.0f", 1 );
    M->setAmplZ( Amplitude, AmplAsymm*Amplitude );
    QHBoxLayout *hbox = new QHBoxLayout;
    mbox->addLayout( hbox );
    OW = new OptWidget( (Options*)this, 1, 2, true, 0, 0 );
    hbox->addWidget( OW );
    QVBoxLayout *vbox = new QVBoxLayout;
    hbox->addLayout( vbox );
    QPushButton *b;
    b = new QPushButton( "clear" );
    vbox->addWidget( b );
    connect( b, SIGNAL( clicked( void ) ), this, SLOT( clear( void ) ) );
    clear();
    b = new QPushButton( "home" );
    vbox->addWidget( b );
    connect( b, SIGNAL( clicked( void ) ), this, SLOT( home( void ) ) );
    FindButton = new QPushButton( "go" );
    vbox->addWidget( b );
    connect( FindButton, SIGNAL( clicked( void ) ), this, SLOT( find( void ) ) );
  }
  
  // spike detector:
  QHBoxLayout *hbox = new QHBoxLayout;
  mbox->addLayout( hbox );
  hbox->setSpacing( 6 );
  QualityIndicator = new QLabel;
  QualityIndicator->setPixmap( *MC->QualityPixs[0] );
  QualityIndicator->setFixedSize( QualityIndicator->sizeHint() );
  hbox->addWidget( QualityIndicator );
  TrendIndicator = new QLabel;
  TrendIndicator->setPixmap( *MC->TrendPixs[0] );
  TrendIndicator->setFixedSize( TrendIndicator->sizeHint() );
  hbox->addWidget( TrendIndicator );
  SpikeSize = new QLabel( "100.0" );
  SpikeSize->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
  SpikeSize->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  SpikeSize->setLineWidth( 2 );
  QFont nf( SpikeSize->font() );
  nf.setPointSizeF( 1.5 * nf.pointSizeF() );
  nf.setBold( true );
  SpikeSize->setFont( nf );
  QPalette qp( SpikeSize->palette() );
  qp.setColor( QPalette::Active, QPalette::Background, Qt::black );
  qp.setColor( QPalette::Active, QPalette::Foreground, Qt::green );
  SpikeSize->setPalette( qp );
  SpikeSize->setFixedSize( SpikeSize->sizeHint() );
  hbox->addWidget( SpikeSize );
  QLabel *ul = new QLabel( "mV" );
  ul->setFixedSize( ul->sizeHint() );
  hbox->addWidget( ul );
  hbox->addWidget( new QLabel( "" ) );
  SpikeRate = new QLabel( "100.0" );
  SpikeRate->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
  SpikeRate->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  SpikeRate->setLineWidth( 2 );
  SpikeRate->setFont( nf );
  SpikeRate->setFixedSize( SpikeRate->sizeHint() );
  hbox->addWidget( SpikeRate );
  ul = new QLabel( "Hz" );
  ul->setFixedSize( ul->sizeHint() );
  hbox->addWidget( ul );

  T = new QTimer( this );
  connect( T, SIGNAL( timeout() ), this, SLOT( update() ) );
  T->start( (int)rint( 1000.0*Interval ) );

  LastStep.start();

  readConfig();
}


void MiMaPu::keyUp( int state )
{
  if ( M == 0 )
    return;

  if ( state & Qt::ALT ) {
    int ampl = Amplitude + 5;
    if ( ampl > 80 ) 
      ampl = 80;
    setNumber( "amplitude", ampl );
  }
  else if ( ( state & Qt::SHIFT ) && ( state & Qt::CTRL ) ) {
    Back += 1;
    if ( Back > 2000 ) 
      Back = 2000;
    setNumber( "back", Back );
  }
  else if ( state & Qt::SHIFT ) {
    Steps += 1;
    if ( Steps > 2000 ) 
      Steps = 2000;
    setNumber( "steps", Steps );
  }
  else if ( state & Qt::CTRL ) {
    double interv = Interval + 0.05;
    if ( interv > 5.0 ) 
      interv = 5.0;
    setNumber( "interval", interv );
  }
  else if ( 0.001*LastStep.elapsed() > Interval ) {
    stop();
    M->stepZ( Steps );
    setNumber( "pos", -M->posZ() );
    LastStep.start();
  }
}


void MiMaPu::keyDown( int state )
{
  if ( M == 0 )
    return;

  if ( state & Qt::ALT ) {
    int ampl = Amplitude - 5;
    if ( ampl < 1 ) 
      ampl = 1;
    setNumber( "amplitude", ampl );
  }
  else if ( ( state & Qt::SHIFT ) && ( state & Qt::CTRL ) ) {
    Back -= 1;
    if ( Back < 0 ) 
      Back = 0;
    setNumber( "back", Back );
  }
  else if ( state & Qt::SHIFT ) {
    Steps -= 1;
    if ( Steps < 1 ) 
      Steps = 1;
    setNumber( "steps", Steps );
  }
  else if ( state & Qt::CTRL ) {
    double interv = Interval - 0.05;
    if ( interv < 0.05 ) 
      interv = 0.05;
    setNumber( "interval", interv );
  }
  else if ( 0.001*LastStep.elapsed() > Interval ) {
    stop();
    M->stepZ( -Steps );
    setNumber( "pos", -M->posZ() );
    LastStep.start();
  }
}


void MiMaPu::notify( void )
{
  double interval = number( "interval" );
  if ( interval != Interval ) {
    Interval = interval;
    if ( T != 0 )
      T->setInterval( (int)rint( 1000.0*Interval ) );
  }
  Steps = integer( "steps" );
  Back = integer( "back" );
  int ampl = integer( "amplitude" );
  double asymm = number( "amplasymm" );
  if ( ampl != Amplitude || asymm != AmplAsymm ) {
    if ( M->setAmplZ( ampl, asymm*ampl ) == 0 ) {
      Amplitude = ampl;
      AmplAsymm = asymm;
    }
    else {
      setInteger( "amplitude", Amplitude );
      setNumber( "amplasymm", AmplAsymm );
    }
  }
  if ( OW != 0 )
    OW->updateValues( OptWidget::changedFlag() );
}


void MiMaPu::update( void )
{
  if ( Find && M != 0 ) {
    bool spikes = MC->spikes( Trace );
    if ( spikes && ! StartFind ) {
      QApplication::beep();
      stop();
    }
    else {
      if ( ! spikes )
	StartFind = false;
      if ( Dir ) {
	M->stepZ( -Steps );
      }
      else {
	if ( Back != 0 ) {
	  M->stepZ( Back );
	}
      }
      if ( Back != 0 )
	Dir = ! Dir;
      setNumber( "pos", -M->posZ() );
    }
  }

  QualityIndicator->setPixmap( *MC->QualityPixs[ Detect.integer( "quality" ) ] );
  TrendIndicator->setPixmap( *MC->TrendPixs[ Detect.integer( "trend" ) ] );
  SpikeSize->setText( Detect.text( "size" ).c_str() );
  SpikeRate->setText( Detect.text( "rate" ).c_str() );
}


void MiMaPu::clear( void )
{
  if ( M == 0 )
    return;

  M->clearZ();
  setNumber( "pos", -M->posZ() + 1.0e-8 );
}


void MiMaPu::home( void )
{
  if ( M == 0 )
    return;

  stop();
  M->homeZ();
  setNumber( "pos", -M->posZ() + 1.0e-8 );
}


void MiMaPu::find( void )
{
  if ( M == 0 )
    return;

  Find = ! Find;
  StartFind = Find;
  Dir = true;
  FindButton->setText( Find ? "stop" : "go" );
}


void MiMaPu::stop( void )
{
  if ( Find )
    FindButton->setText( "go" );
  Find = false;
}


addControl( MotorControl );

}; /* namespace ephys */

#include "moc_motorcontrol.cc"
