/*
  motorcontrol.cc
  Control widgets for motorized manipulators.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <qapplication.h>
#include <qgroupbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qpainter.h>
#include <relacs/manipulator.h>
#include <relacs/common/motorcontrol.h>
using namespace relacs;


MotorControl::MotorControl( void )
  : Control( "MotorControl", "Control", "Common",
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
  int is = fontInfo().pointSize() * 2;
  QPainter p;
  QColor orange( 255, 165, 0 );
  GoodCircle.resize( is, is );
  p.begin( &GoodCircle );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( GoodCircle.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( green );
  p.drawEllipse( 0, 0, is, is );
  p.end();

  OkCircle.resize( is, is );
  p.begin( &OkCircle );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( OkCircle.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( yellow );
  p.drawEllipse( 0, 0, is, is );
  p.end();

  PotentialCircle.resize( is, is );
  p.begin( &PotentialCircle );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( PotentialCircle.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( orange );
  p.drawEllipse( 0, 0, is, is );
  p.end();

  BadCircle.resize( is, is );
  p.begin( &BadCircle );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( BadCircle.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( red );
  p.drawEllipse( 0, 0, is, is );
  p.end();

  QPointArray pa( 7 );
  GoodArrow.resize( is, is );
  p.begin( &GoodArrow );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( GoodArrow.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( green );
  pa.setPoint( 0, is/4, is-1 );
  pa.setPoint( 1, 3*is/4, is-1 );
  pa.setPoint( 2, 3*is/4, is/3 );
  pa.setPoint( 3, is, is/3 );
  pa.setPoint( 4, is/2, 0 );
  pa.setPoint( 5, 0, is/3 );
  pa.setPoint( 6, is/4, is/3 );
  p.drawPolygon( pa );
  p.end();

  BadArrow.resize( is, is );
  p.begin( &BadArrow );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( BadArrow.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( red );
  pa.setPoint( 0, is/4, 0 );
  pa.setPoint( 1, 3*is/4, 0 );
  pa.setPoint( 2, 3*is/4, 2*is/3 );
  pa.setPoint( 3, is, 2*is/3 );
  pa.setPoint( 4, is/2, is );
  pa.setPoint( 5, 0, 2*is/3 );
  pa.setPoint( 6, is/4, 2*is/3 );
  p.drawPolygon( pa );
  p.end();

  // quality indicator:
  QualityPixs[0] = &BadCircle;
  QualityPixs[1] = &PotentialCircle;
  QualityPixs[2] = &OkCircle;
  QualityPixs[3] = &GoodCircle;

  // trend indicator:
  TrendPixs[0] = &BadCircle;
  TrendPixs[1] = &BadArrow;
  TrendPixs[2] = &OkCircle;
  TrendPixs[3] = &GoodCircle;
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

  QHBox *h = new QHBox( this );
  QPushButton *db = new QPushButton( "Dialog", h );
  connect( db, SIGNAL( clicked( void ) ), this, SLOT( dialog( void ) ) );
  QPushButton *hb = new QPushButton( "Help", h );
  connect( hb, SIGNAL( clicked( void ) ), this, SLOT( help( void ) ) );
}


void MotorControl::notify( void )
{
  MinRate = number( "minrate" );
  MinSize = number( "minsize" );
}


void MotorControl::keyPressEvent( QKeyEvent *e )
{
  switch ( e->key() ) {

  case Key_Up:
    if ( MM.size() > 0 )
      MM[0]->keyUp( e->state() );
    break;

  case Key_Down:
    if ( MM.size() > 0 )
      MM[0]->keyDown( e->state() );
    break;

  case Key_Left:
    if ( MM.size() > 1 )
      MM[1]->keyUp( e->state() );
    break;

  case Key_Right:
    if ( MM.size() > 1 )
      MM[1]->keyDown( e->state() );
    break;

  case Key_G:
    if ( MM.size() > 0 )
      MM[0]->find();
    break;

  case Key_H:
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
  : QObject( parent ), 
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
  QGroupBox *g = new QGroupBox( 1, Horizontal, title.c_str(), MC );
  setConfigIdent( "MiMaPu-" + Str( trace+1, 0 ) );

  // manipulator:
  if ( M != 0 ) {
    QHBox *h = new QHBox( g );
    addNumber( "pos", "Pos", 0.0, -100000.0, 100000.0, 1.0, "um", "um", "%6.1f",
	       1+2, OptWidget::ValueBold + OptWidget::ValueRed + OptWidget::ValueBackBlack );
    addInteger( "steps", "Steps", Steps, 1, 10000, 1 ).setFlags( 1 );
    addInteger( "back", "Back", Back, 0, 10000, 1 ).setFlags( 1 );
    addInteger( "amplitude", "Amplitude", Amplitude, 1, 80, 5 ).setFlags( 1 );
    addNumber( "amplasymm", "Asymm", AmplAsymm, 0.0, 1.0, 0.01, "", "", "%4.2f", 0 );
    addNumber( "interval", "Interval", Interval, 0.0, 5.0, 0.05, "sec", "ms", "%.0f", 1 );
    M->setAmplZ( Amplitude, AmplAsymm*Amplitude );
    OW = new OptWidget( (Options*)this, 1, 2, true, 0, 0, h );
    QVBox *v = new QVBox( h );
    QPushButton *b;
    b = new QPushButton( "clear", v );
    connect( b, SIGNAL( clicked( void ) ), this, SLOT( clear( void ) ) );
    clear();
    b = new QPushButton( "home", v );
    connect( b, SIGNAL( clicked( void ) ), this, SLOT( home( void ) ) );
    FindButton = new QPushButton( "go", v );
    connect( FindButton, SIGNAL( clicked( void ) ), this, SLOT( find( void ) ) );
  }
  
  // spike detector:
  QHBox *h = new QHBox( g );
  h->setSpacing( 6 );
  new QLabel( "", h );
  QualityIndicator = new QLabel( h );
  QualityIndicator->setPixmap( MC->BadCircle );
  QualityIndicator->setFixedSize( QualityIndicator->sizeHint() );
  TrendIndicator = new QLabel( h );
  TrendIndicator->setPixmap( MC->BadCircle );
  TrendIndicator->setFixedSize( TrendIndicator->sizeHint() );
  SpikeSize = new QLabel( "100.0", h );
  SpikeSize->setAlignment( AlignRight | AlignVCenter );
  SpikeSize->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  SpikeSize->setLineWidth( 2 );
  QFont nf( SpikeSize->font() );
  nf.setPointSizeFloat( 1.5 * nf.pointSizeFloat() );
  nf.setBold( true );
  SpikeSize->setFont( nf );
  QPalette qp( SpikeSize->palette() );
  qp.setColor( QPalette::Active, QColorGroup::Background, black );
  qp.setColor( QPalette::Active, QColorGroup::Foreground, green );
  SpikeSize->setPalette( qp );
  SpikeSize->setFixedSize( SpikeSize->sizeHint() );
  QLabel *ul = new QLabel( "mV", h );
  ul->setFixedSize( ul->sizeHint() );
  new QLabel( "", h );
  SpikeRate = new QLabel( "100.0", h );
  SpikeRate->setAlignment( AlignRight | AlignVCenter );
  SpikeRate->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  SpikeRate->setLineWidth( 2 );
  SpikeRate->setFont( nf );
  SpikeRate->setFixedSize( SpikeRate->sizeHint() );
  ul = new QLabel( "Hz", h );
  ul->setFixedSize( ul->sizeHint() );
  new QLabel( "", h );

  T = new QTimer( this );
  connect( T, SIGNAL( timeout() ), this, SLOT( update() ) );
  T->start( (int)rint( 1000.0*Interval ), false );

  LastStep.start();

  readConfig();
}


void MiMaPu::keyUp( int state )
{
  if ( M == 0 )
    return;

  if ( state & AltButton ) {
    int ampl = Amplitude + 5;
    if ( ampl > 80 ) 
      ampl = 80;
    setNumber( "amplitude", ampl );
  }
  else if ( ( state & ShiftButton ) && ( state & ControlButton ) ) {
    Back += 1;
    if ( Back > 2000 ) 
      Back = 2000;
    setNumber( "back", Back );
  }
  else if ( state & ShiftButton ) {
    Steps += 1;
    if ( Steps > 2000 ) 
      Steps = 2000;
    setNumber( "steps", Steps );
  }
  else if ( state & ControlButton ) {
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

  if ( state & AltButton ) {
    int ampl = Amplitude - 5;
    if ( ampl < 1 ) 
      ampl = 1;
    setNumber( "amplitude", ampl );
  }
  else if ( ( state & ShiftButton ) && ( state & ControlButton ) ) {
    Back -= 1;
    if ( Back < 0 ) 
      Back = 0;
    setNumber( "back", Back );
  }
  else if ( state & ShiftButton ) {
    Steps -= 1;
    if ( Steps < 1 ) 
      Steps = 1;
    setNumber( "steps", Steps );
  }
  else if ( state & ControlButton ) {
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
      T->changeInterval( (int)rint( 1000.0*Interval ) );
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

#include "moc_motorcontrol.cc"
