/*
  auditory/session.cc
  Session for recordings from auditory neurons

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

#include <qvbox.h>
#include <qlabel.h>
#include <relacs/auditory/session.h>
using namespace relacs;

namespace auditory {


Session::Session( void )
  : Control( "Session", "Info", "Auditory",
	     "Jan Benda", "1.3", "Jan 22, 2008" ),
    P( 2, 1, true, Plot::Pointer, this, "sessionplot" )

{
  //  setColumnLayout( 2, Horizontal );
  boxLayout()->setDirection( QBoxLayout::LeftToRight );

  QVBox *bb = new QVBox( this );
  bb->setSpacing( 0 );

  ASW = new OptWidget( bb );
  ASW->setSpacing( 4 );
  ASW->setMargin( 4 );

  AmplBox = new QHBox( bb );
  ResistanceButton = 0;
  BuzzerButton = 0;

  SessionButton = new QPushButton( "Cell Found", bb, "SessionButton" );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );

  P.lock();
  P.setDataMutex( mutex() );

  P[0].setXRange( 0.0, 40.0 );
  P[0].setXLabel( "Frequency [kHz]" );
  P[0].setYRange( 20.0, 100.0 );
  P[0].setYLabel( "Threshold [dB]" );
  P[0].setYLabelPos( 2.2, Plot::FirstMargin, -0.2, Plot::SecondMargin, 
		     Plot::Right, -90.0 );
  P[0].setLMarg( 7.0 );
  P[0].setRMarg( 2.0 );
  P[0].setTMarg( 1.5 );
  P[0].setBMarg( 4.5 );

  P[1].setXRange( 20.0, 100.0 );
  P[1].setXLabel( "Intensity [dB SPL]" );
  P[1].setYRange( 0.0, Plot::AutoScale );
  P[1].setYLabel( "Firing rate [Hz]" );
  P[1].setYLabelPos( 2.2, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setLMarg( 7.0 );
  P[1].setRMarg( 2.0 );
  P[1].setTMarg( 1.0 );
  P[1].setBMarg( 5.0 );

  P.unlock();

  Temp = 0;
  Ampl = 0;
  RMeasure = false;

  MaxResistance = 100.0;
  ResistanceScale = 1.0;

}


Session::~Session( void )
{
}


void Session::initialize( void )
{
  // get trace indices:
  ephys::Traces::initialize( traces(), events() );
  acoustic::Traces::initialize( this, traces(), events() );

  // additional header data:
  // XXX important: the following must be called AFTER metaData() is cleared
  // and BEFORE the configuration file is read in!!!!!
  lockMetaData();
  metaData().unsetNotify();
  metaData().addLabel( "Cell properties", MetaDataSave );

  metaData().addSelection( "best side", "Best side", "left|right", MetaDataReadOnly+MetaDataDisplay ).setUnit( "speaker" );
  metaData().addNumber( "best frequency", "Best frequency", -1000.0, -1000.0, 100000.0, 500.0, "Hz", "kHz", "%.1f", MetaDataDisplay+MetaDataReset );
  metaData().addNumber( "best threshold", "Best threshold", -1.0, -1.0, 200.0, 1.0, "dB SPL", "dB SPL", "%.1f", MetaDataDisplay+MetaDataReset );
  metaData().addNumber( "best slope", "Best slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  metaData().addNumber( "best intensity", "Best intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "best rate", "Best rate", 100.0, "Hz", "%.1f", metaData().presetDialogFlag() + MetaDataSave );
  metaData().addNumber( "best saturation", "Best saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "best maxrate", "Best maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  metaData().addNumber( "left frequency", "Left frequency", -1000.0, "Hz", "%.1f", MetaDataReset ).setUnit( "Hz", "kHz" );
  metaData().addNumber( "left threshold", "Left threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "left slope", "Left slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  metaData().addNumber( "left intensity", "Left intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "left saturation", "Left saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "left maxrate", "Left maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  metaData().addNumber( "right frequency", "Right frequency", -1000.0, "Hz", "%.1f", MetaDataReset ).setUnit( "Hz", "kHz" );
  metaData().addNumber( "right threshold", "Right threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "right slope", "Right slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  metaData().addNumber( "right intensity", "Right intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "right saturation", "Right saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "right maxrate", "Right maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  metaData().addNumber( "left noise threshold", "Left threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "left noise slope", "Left slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  metaData().addNumber( "left noise intensity", "Left intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "left noise saturation", "Left saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "left noise maxrate", "Left maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  metaData().addNumber( "right noise threshold", "Left threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "right noise slope", "Left slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  metaData().addNumber( "right noise intensity", "Left intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "right noise saturation", "Left saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  metaData().addNumber( "right noise maxrate", "Left maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  metaData().addNumber( "silent rate", "Silent rate", -1.0, "Hz", "%.1f", MetaDataReset );

  metaData().addLabel( "plugins" );

  metaData().addLabel( "metadata", MetaDataSave );

  metaData().addStyle( OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, MetaDataDisplay );

  metaData().setSaveFlag( metaData().standardFlag() + metaData().configFlag()
			  + metaData().setupFlag() + MetaDataSave );

  if ( simulation() ) {
    metaData().selectText( "best side", "left" );
    metaData().setNumber( "best frequency", 6000.0 );
    metaData().setNumber( "best threshold", 45.0 );
    metaData().setNumber( "best slope", 25.0 );
    metaData().setNumber( "best intensity", 49.0 );
    metaData().setNumber( "best rate", 100.0 );
    metaData().setNumber( "best saturation", 58.0 );
    metaData().setNumber( "best maxrate", 325.0 );
  }
  metaData().setNotify();
  unlockMetaData();
}


void Session::initDevices( void )
{
  Temp = dynamic_cast< Temperature* >( device( "temp-1" ) );
  // or get just the first one:
  //  Temp = dynamic_cast< Temperature* >( device( Temperature::Type, 0 ) );
  if ( Temp != 0 ) {
    lockMetaData();
    metaData().unsetNotify();
    metaData().erase( "temp-1" );
    metaData().insertNumber( "temp-1", "metadata", "Temperature", 0.0, "°C", "%.1f", MetaDataReadOnly+MetaDataDisplay, 
			 OptWidget::ValueBold + OptWidget::ValueRed + OptWidget::ValueBackBlack );
    metaData().setNotify();
    unlockMetaData();
    lockStimulusData();
    stimulusData().addNumber( "temp-1", 0.0, "°C", "%.1f" );
    unlockStimulusData();
  }

  Ampl = dynamic_cast< misc::AmplMode* >( device( "ampl-1" ) );
  if ( Ampl != 0 ) {
    lockMetaData();
    metaData().unsetNotify();
    metaData().insertNumber( "resistance", "metadata", "Resistance", 0.0, "MOhm", "%.0f", MetaDataReadOnly+MetaDataDisplay );
    metaData().setNotify();
    unlockMetaData();
    AmplBox->show();
    if ( ResistanceButton == 0 && BuzzerButton == 0 ) {
      ResistanceButton = new QPushButton( "R", AmplBox );
      connect( ResistanceButton, SIGNAL( pressed() ),
	       this, SLOT( startResistance() ) );
      connect( ResistanceButton, SIGNAL( released() ),
	       this, SLOT( stopResistance() ) );
    
      new QLabel( AmplBox );
      
      BuzzerButton = new QPushButton( "Buzz", AmplBox );
      connect( BuzzerButton, SIGNAL( clicked() ),
	       this, SLOT( buzz() ) );
    }
  }
  else {
    AmplBox->hide();
  }

  ASW->assign( &metaData(), MetaDataDisplay, MetaDataReadOnly, true, 
	       OptWidget::BreakLinesStyle + OptWidget::ExtraSpaceStyle,
	       metaDataMutex() );
  ASW->setSpacing( 2 );
  updateGeometry();
}


void Session::startSession( void )
{
  SessionButton->setText( "Cell Lost" );

  // reset values of metaData() options:
  lockMetaData();
  metaData().setDefaults( MetaDataReset );
  metaData().delFlags( MetaDataSave + Parameter::changedFlag(), MetaDataReset );
  unlockMetaData();

  lock();
  for ( int k=0; k<2; k++ ) {
    if ( ThreshCurve[k].empty() )
      OldThreshCurve[k].clear();
    else
      OldThreshCurve[k] = ThreshCurve[k].back();
    ThreshCurve[k].clear();
    if( FICurve[k].empty() ) {
      if( OnFICurve[k].empty() )
	OldFICurve[k].clear();
      else
	OldFICurve[k] = OnFICurve[k].back();
    }
    else
      OldFICurve[k] = FICurve[k].back();
    FICurve[k].clear();
    OnFICurve[k].clear();
    SSFICurve[k].clear();
  }
  unlock();

  plot();
}


void Session::stopSession( bool saved )
{
  lock();
  SessionButton->setText( "Cell Found" );
  unlock();
}


void Session::main( void )
{
  if ( Temp == 0 )
    return;

  while ( ! interrupt() ) {
    double temp = Temp->temperature();
    lockMetaData();
    metaData().setNumber( "temp-1", temp );
    unlockMetaData();
    lockStimulusData();
    stimulusData().setNumber( "temp-1", temp );
    unlockStimulusData();
    sleep( 1.0 );
  }
}


void Session::startResistance( void )
{
  if ( Ampl != 0 && SpikeTrace[0] >= 0 && ! RMeasure ) {
    readLockData();
    DGain = trace( SpikeTrace[0] ).gainIndex();
    adjustGain( trace( SpikeTrace[0] ), MaxResistance / ResistanceScale );
    unlockData();
    activateGains();
    Ampl->resistance();
    RMeasure = true;
  }
}


void Session::measureResistance( void )
{
  if ( Ampl != 0 && SpikeTrace[0] >= 0 && RMeasure ) {
    readLockData();
    double r = trace( SpikeTrace[0] ).stdev( trace( SpikeTrace[0] ).currentTime() - 0.05,
					     trace( SpikeTrace[0] ).currentTime() );
    unlockData();
    r *= ResistanceScale;
    lockMetaData();
    metaData().setNumber( "resistance", r );
    unlockMetaData();
  }
}


void Session::stopResistance( void )
{
  if ( Ampl != 0 && SpikeTrace[0] >= 0 && RMeasure ) {
    Ampl->manual();
    readLockData();
    setGain( trace( SpikeTrace[0] ), DGain );
    unlockData();
    activateGains();
    RMeasure = false;
  }
}


void Session::buzz( void )
{
  if ( Ampl != 0 ) {
    Ampl->buzzer( );
  }
}


void Session::keyPressEvent( QKeyEvent *e )
{
  Control::keyPressEvent( e );

  switch ( e->key() ) {

  case Key_O:
    if ( Ampl != 0 && ResistanceButton != 0 ) {
      ResistanceButton->setDown( true );
      startResistance();
    }
    else
      e->ignore();
    break;

  case Key_Z:
    if ( Ampl != 0 && BuzzerButton != 0 ) {
      BuzzerButton->animateClick();
    }
    else
      e->ignore();
    break;

  default:
    e->ignore();

  }
}


void Session::keyReleaseEvent( QKeyEvent *e )
{
  switch ( e->key() ) {

  case Key_O: 
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
    e->ignore();

  }
}


void Session::notifyMetaData( void )
{
  metaData().addFlags( MetaDataSave, Parameter::changedFlag() );
  ASW->updateValues( OptWidget::changedFlag() );
}


MapD Session::threshCurve( int side ) const
{
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "best side" );
    unlockMetaData();
  }
  MapD tc( 0 );
  lock();
  if ( ! ThreshCurve[side].empty() )
    tc = ThreshCurve[side].back();
  unlock();
  return tc;
}


void Session::addThreshCurve( const MapD &thresh, int side )
{
  lock();
  ThreshCurve[side].push_back( thresh );
  unlock();
  plot();
}


MapD Session::fICurve( int side ) const
{
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "best side" );
    unlockMetaData();
  }
  MapD fi( 0 );
  lock();
  if ( ! FICurve[side].empty() )
    fi = FICurve[side].back();
  unlock();
  return fi;
}


void Session::addFICurve( const MapD &ficurve, int side )
{
  lock();
  FICurve[side].push_back( ficurve );
  unlock();
  plot();
}


MapD Session::onFICurve( int side ) const
{
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "best side" );
    unlockMetaData();
  }
  MapD ofi( 0 );
  lock();
  if ( ! OnFICurve[side].empty() )
    ofi = OnFICurve[side].back();
  unlock();
  return ofi;
}


void Session::addOnFICurve( const MapD &onficurve, int side )
{
  lock();
  OnFICurve[side].push_back( onficurve );
  unlock();
  plot();
}


MapD Session::ssFICurve( int side ) const
{
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "best side" );
    unlockMetaData();
  }
  MapD sfi( 0 );
  lock();
  if ( ! SSFICurve[side].empty() )
    sfi = SSFICurve[side].back();
  unlock();
  return sfi;
}


void Session::addSSFICurve( const MapD &ssficurve, int side )
{
  lock();
  SSFICurve[side].push_back( ssficurve );
  unlock();
  plot();
}


void Session::updateBestSide( void )
{
  lockMetaData();
  int bestnoise = -1;
  double left = metaData().number( "left noise threshold" );
  double right = metaData().number( "right noise threshold" );
  if ( left > 0.0 && right > 0.0 )
    bestnoise = left < right ? 0 : 1;
  else
    bestnoise = left > 0.0 ? 0 : ( right > 0.0 ? 1 : -1 );

  int bestsine = -1;
  left = metaData().number( "left threshold" );
  right = metaData().number( "right threshold" );
  if ( left > 0.0 && right > 0.0 )
    bestsine = left < right ? 0 : 1;
  else
    bestsine = left > 0.0 ? 0 : ( right > 0.0 ? 1 : -1 );

  if ( bestsine >= 0 ) {
    metaData().selectText( "best side", bestsine == 0 ? "left" : "right" );
  }
  else if ( bestnoise >= 0 )
    metaData().selectText( "best side", bestnoise == 0 ? "left" : "right" );
  unlockMetaData();
}


void Session::plot( void )
{
  lock();
  lockMetaData();
  P.lock();

  P[0].clear();
  double bf = metaData().number( "best frequency" );
  if ( bf > 0.0 )
    P[0].plotVLine( 0.001*bf, Plot::White, 2 );
  P[0].plot( OldThreshCurve[0], 0.001, Plot::Gray, 2, Plot::LongDash );
  P[0].plot( OldThreshCurve[1], 0.001, Plot::Gray, 2, Plot::Solid );
  for ( int k=0; k < int(ThreshCurve[0].size()) - 1; k++ )
    P[0].plot( ThreshCurve[0][k], 0.001, Plot::DarkOrange, 2, Plot::LongDash );
  for ( int k=0; k < int(ThreshCurve[1].size()) - 1; k++ )
    P[0].plot( ThreshCurve[1][k], 0.001, Plot::DarkOrange, 2, Plot::Solid );
  if ( ! ThreshCurve[0].empty() )
    P[0].plot( ThreshCurve[0].back(), 0.001, Plot::Yellow, 3, Plot::LongDash );
  if ( ! ThreshCurve[1].empty() )
    P[0].plot( ThreshCurve[1].back(), 0.001, Plot::Yellow, 3, Plot::Solid );

  P[1].clear();
  double bt = metaData().number( "best threshold" );
  if ( bt > 0.0 )
    P[1].plotVLine( bt, Plot::White, 2 );
  double bs = metaData().number( "best saturation" );
  if ( bs > 0.0 )
    P[1].plotVLine( bs, Plot::White, 2 );
  P[1].plot( OldFICurve[0], 1.0, Plot::White, 2, Plot::LongDash );
  P[1].plot( OldFICurve[1], 1.0, Plot::White, 2, Plot::Solid );
  for ( int k=0; k < int(SSFICurve[0].size()) - 1; k++ )
    P[1].plot( SSFICurve[0][k], 1.0, Plot::OrangeRed, 2, Plot::LongDash );
  for ( int k=0; k < int(SSFICurve[1].size()) - 1; k++ )
    P[1].plot( SSFICurve[1][k], 1.0, Plot::OrangeRed, 2, Plot::Solid );
  for ( int k=0; k < int(OnFICurve[0].size()) - 1; k++ )
    P[1].plot( OnFICurve[0][k], 1.0, Plot::DarkGreen, 2, Plot::LongDash );
  for ( int k=0; k < int(OnFICurve[1].size()) - 1; k++ )
    P[1].plot( OnFICurve[1][k], 1.0, Plot::DarkGreen, 2, Plot::Solid );
  for ( int k=0; k < int(FICurve[0].size()) - 1; k++ )
    P[1].plot( FICurve[0][k], 1.0, Plot::DarkOrange, 2, Plot::LongDash );
  for ( int k=0; k < int(FICurve[1].size()) - 1; k++ )
    P[1].plot( FICurve[1][k], 1.0, Plot::DarkOrange, 2, Plot::Solid );
  if ( ! SSFICurve[0].empty() )
    P[1].plot( SSFICurve[0].back(), 1.0, Plot::Red, 3, Plot::LongDash );
  if ( ! SSFICurve[1].empty() )
    P[1].plot( SSFICurve[1].back(), 1.0, Plot::Red, 3, Plot::Solid );
  if ( ! OnFICurve[0].empty() )
    P[1].plot( OnFICurve[0].back(), 1.0, Plot::Green, 3, Plot::LongDash );
  if ( ! OnFICurve[1].empty() )
    P[1].plot( OnFICurve[1].back(), 1.0, Plot::Green, 3, Plot::Solid );
  if ( ! FICurve[0].empty() )
    P[1].plot( FICurve[0].back(), 1.0, Plot::Orange, 3, Plot::LongDash );
  if ( ! FICurve[1].empty() )
    P[1].plot( FICurve[1].back(), 1.0, Plot::Orange, 3, Plot::Solid );

  P.unlock(); 
  unlockMetaData();
  unlock();

  P.draw();
}


addControl( Session );

}; /* namespace auditory */

#include "moc_session.cc"
