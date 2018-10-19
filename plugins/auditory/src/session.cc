/*
  auditory/session.cc
  Session for recordings from auditory neurons

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
#include <QLabel>
#include <relacs/auditory/session.h>
using namespace relacs;

namespace auditory {


Session::Session( void )
  : Control( "Session", "auditory", "Jan Benda", "1.5", "Feb 2, 2010" )
{
  QHBoxLayout *hb = new QHBoxLayout;
  hb->setSpacing( 0 );
  setLayout( hb );

  hb->addWidget( &P );

  QVBoxLayout *vb = new QVBoxLayout;
  vb->setContentsMargins( 0, 0, 0, 0 );
  hb->addLayout( vb );

  ASW = new OptWidget;
  ASW->setVerticalSpacing( 2 );
  ASW->setMargins( 0 );
  vb->addWidget( ASW );

  SW = new OptWidget;
  SW->setVerticalSpacing( 2 );
  SW->setMargins( 0 );
  vb->addWidget( SW );

  SessionButton = new QPushButton;
  SessionButton->setText( "Cell Found" );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  vb->addWidget( SessionButton );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );

  P.lock();
  P.setMinimumWidth( 210 );
  P.resize( 2, 1, true, Plot::Pointer );
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
}


Session::~Session( void )
{
}


void Session::preConfig( void )
{
  // get trace indices:
  ephys::Traces::initialize( this, traces(), events() );
  acoustic::Traces::initialize( this, traces(), events() );
}


void Session::config( void )
{
  // additional meta data properties:
  lockMetaData();
  metaData().unsetNotify();

  if ( ! metaData().existSection( "Cell" ) )
    metaData().newSection( "Cell" );
  Options &mo = metaData().section( "Cell" );
  Options::iterator p = mo.find( "best rate" );
  double rate =  100.0;
  if ( p != mo.end() ) {
    rate = p->number();
    mo.erase( p );
  }
  Options::section_iterator sp = mo.findSection( "Cell properties" );
  if ( sp != mo.sectionsEnd() )
    mo.erase( sp );
  mo.newSection( "Cell properties", MetaDataSave+MetaDataDisplay );

  mo.addSelection( "best side", "Best side", "left|right", MetaDataReadOnly+MetaDataDisplay ).setUnit( "speaker" );
  mo.addNumber( "best frequency", "Best frequency", -1000.0, -1000.0, 100000.0, 500.0, "Hz", "kHz", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "best threshold", "Best threshold", -1.0, -1.0, 200.0, 1.0, "dB SPL", "dB SPL", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "best slope", "Best slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  mo.addNumber( "best intensity", "Best intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "best rate", "Best rate", rate, "Hz", "%.1f", MetaData::presetDialogFlag() + MetaData::configFlag() + MetaDataSave );

  mo.addNumber( "best saturation", "Best saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "best maxrate", "Best maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  mo.addNumber( "left frequency", "Left frequency", -1000.0, "Hz", "%.1f", MetaDataReset ).setUnit( "Hz", "kHz" );
  mo.addNumber( "left threshold", "Left threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "left slope", "Left slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  mo.addNumber( "left intensity", "Left intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "left saturation", "Left saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "left maxrate", "Left maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  mo.addNumber( "right frequency", "Right frequency", -1000.0, "Hz", "%.1f", MetaDataReset ).setUnit( "Hz", "kHz" );
  mo.addNumber( "right threshold", "Right threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "right slope", "Right slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  mo.addNumber( "right intensity", "Right intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "right saturation", "Right saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "right maxrate", "Right maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  mo.addNumber( "left noise threshold", "Left threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "left noise slope", "Left slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  mo.addNumber( "left noise intensity", "Left intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "left noise saturation", "Left saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "left noise maxrate", "Left maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  mo.addNumber( "right noise threshold", "Left threshold", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "right noise slope", "Left slope", -1.0, "Hz/dB", "%.1f", MetaDataReset );
  mo.addNumber( "right noise intensity", "Left intensity", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "right noise saturation", "Left saturation", -1.0, "dB SPL", "%.1f", MetaDataReset );
  mo.addNumber( "right noise maxrate", "Left maximum rate", -1.0, "Hz", "%.1f", MetaDataReset );

  mo.addNumber( "silent rate", "Silent rate", -1.0, "Hz", "%.1f", MetaDataReset );

  mo.addStyles( OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, MetaDataDisplay );

  metaData().delSaveFlags( MetaData::dialogFlag() + MetaData::presetDialogFlag() );
  metaData().addSaveFlags( MetaData::configFlag() + MetaData::standardFlag() + MetaDataSave );
  
  if ( simulation() ) {
    mo.setNumber( "left frequency", 6000.0 );
    mo.setNumber( "left threshold", 45.0 );
    mo.setNumber( "left slope", 25.0 );
    mo.setNumber( "left intensity", 49.0 );
    mo.setNumber( "left saturation", 58.0 );
    mo.setNumber( "left maxrate", 325.0 );
    mo.selectText( "best side", "left" );
    mo.setNumber( "best frequency", 6000.0 );
    mo.setNumber( "best threshold", 45.0 );
    mo.setNumber( "best slope", 25.0 );
    mo.setNumber( "best intensity", 49.0 );
    mo.setNumber( "best rate", rate );
    mo.setNumber( "best saturation", 58.0 );
    mo.setNumber( "best maxrate", 325.0 );
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
    Options &mo = metaData().section( "Recording" );
    mo.unsetNotify();
    mo.erase( "temp-1" );
    mo.addNumber( "temp-1", "Temperature", 0.0, "°C", "%.1f", MetaDataReadOnly+MetaDataDisplay, 
		  OptWidget::ValueBold + OptWidget::ValueRed + OptWidget::ValueBackBlack );
    mo.setNotify();
    unlockMetaData();
    lockStimulusData();
    stimulusData().addNumber( "temp-1", 0.0, "°C", "%.1f" );
    unlockStimulusData();
  }

  ASW->assign( *metaData().findSection( "Cell>Cell properties" ),
	       MetaDataDisplay, MetaDataReadOnly, true, 
	       OptWidget::BreakLinesStyle + OptWidget::ExtraSpaceStyle,
	       metaDataMutex() );
  ASW->setVerticalSpacing( 2 );

  lockStimulusData();
  if ( ! stimulusData().exist( "Drugs" ) )
       stimulusData().addText( "Drugs", "Applied drugs", "" ).setFormat( "%-20s" ).setFlags( 16 );
  unlockStimulusData();
  SW->assign( &stimulusData(), 16, 0, true, 
	      OptWidget::BreakLinesStyle + OptWidget::ExtraSpaceStyle,
	      stimulusDataMutex() );
  if ( SpikeTraces <= 0 || SpikeEvents[0] < 0 || SpikeTrace[0] < 0 ) {
    P.hide();
    ASW->hide();
  }
}


void Session::clearDevices( void )
{
  Temp = 0;
}


void Session::sessionStarted( void )
{
  SessionButton->setText( "Cell lost" );

  lockMetaData();
  double bf[2];
  bf[0] = metaData().number( "Cell>left frequency" );
  bf[1] = metaData().number( "Cell>right frequency" );

  // reset values of metaData() options:
  metaData().setDefaults( MetaDataReset );
  metaData().delFlags( MetaDataSave + Parameter::changedFlag(), MetaDataReset );
  unlockMetaData();

  lock();
  for ( int k=0; k<2; k++ ) {
    // copy last threshold curve:
    if ( ThreshCurve[k].empty() )
      OldThreshCurve[k].clear();
    else
      OldThreshCurve[k] = ThreshCurve[k].back();
    ThreshCurve[k].clear();
    // copy last f-I curve:
    if( FICurve[k].empty() )
      OldFICurve[k].clear();
    else
      OldFICurve[k] = fICurve( k, bf[k], false );
    FICurve[k].clear();
    FICurveCarrier[k].clear();
    OnFICurve[k].clear();
    OnFICurveCarrier[k].clear();
    SSFICurve[k].clear();
    SSFICurveCarrier[k].clear();
  }
  unlock();

  plot();
}


void Session::sessionStopped( bool saved )
{
  lock();
  SessionButton->setText( "Cell found" );
  unlock();
}


void Session::main( void )
{
  if ( Temp == 0 )
    return;

  while ( ! interrupt() ) {
    double temp = Temp->temperature();
    lockMetaData();
    metaData().setNumber( "Recording>temp-1", temp );
    unlockMetaData();
    lockStimulusData();
    stimulusData().setNumber( "temp-1", temp );
    unlockStimulusData();
    sleep( 1.0 );
  }
}


void Session::notifyMetaData( void )
{
  if ( ! metaData().existSection( "Cell properties" ) )
    return;
  Options &mo = metaData().section( "Cell properties" );

  string ss = mo.text( "best side" );

  if ( mo.changed( "best frequency" ) )
    mo.setNumber( ss + " frequency", mo.number( "best frequency" ), mo.error( "best frequency" ) );
  if ( mo.changed( "best threshold" ) )
    mo.setNumber( ss + " threshold", mo.number( "best threshold" ), mo.error( "best threshold" ) );
  if ( mo.changed( "best slope" ) )
    mo.setNumber( ss + " slope", mo.number( "best slope" ), mo.error( "best slope" )  );
  if ( mo.changed( "best intensity" ) )
    mo.setNumber( ss + " intensity", mo.number( "best intensity" ), mo.error( "best intensity" )  );
  if ( mo.changed( "best saturation" ) )
    mo.setNumber( ss + " saturation", mo.number( "best saturation" ), mo.error( "best saturation" )  );
  if ( mo.changed( "best maxrate" ) )
    mo.setNumber( ss + " maxrate", mo.number( "best maxrate" ), mo.error( "best maxrate" )  );

  if ( mo.changed( "best side" ) ) {
    mo.setNumber( "best frequency", mo.number( ss + " frequency" ), mo.error( ss + " frequency" ) );
    mo.setNumber( "best threshold", mo.number( ss + " threshold" ), mo.error( ss + " threshold" ) );
    mo.setNumber( "best slope", mo.number( ss + " slope" ), mo.error( ss + " slope" )  );
    mo.setNumber( "best intensity", mo.number( ss + " intensity" ), mo.error( ss + " intensity" )  );
    mo.setNumber( "best saturation", mo.number( ss + " saturation" ), mo.error( ss + " saturation" )  );
    mo.setNumber( "best maxrate", mo.number( ss + " maxrate" ), mo.error( ss + " maxrate" )  );
  }
  metaData().section( "Cell" ).addFlags( MetaDataSave, Parameter::changedFlag() );
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 )
    ASW->updateValues( MetaDataDisplay );
}


void Session::notifyStimulusData( void )
{
  SW->updateValues( OptWidget::changedFlag(), false );
}


MapD Session::threshCurve( int side ) const
{
  // default values:
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  // get threshold curve:
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


MapD Session::fICurve( int side, double carrierfreq, bool lockit ) const
{
  // default values:
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  if ( ::fabs( carrierfreq ) < 1.0e-8 ) {
    lockMetaData();
    carrierfreq = metaData().number( "Cell>" + side > 0 ? "right frequency" : "left frequency" );
    if ( carrierfreq < 0.0 )
      carrierfreq = metaData().number( "Cell>best frequency" );
    unlockMetaData();
  }
  // search f-I curve:
  MapD fi( 0 );
  if ( lockit )
    lock();
  for ( int k=FICurve[side].size()-1; k>=0; k-- ) {
    if ( ::fabs( FICurveCarrier[side][k] - carrierfreq ) < 1.0 ) {
      fi = FICurve[side][k];
      break;
    }
  }
  if ( lockit )
    unlock();
  return fi;
}


MapD Session::fICurve( int index, int side, double &carrierfreq ) const
{
  // default values:
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  // get f-I curve:
  MapD fi( 0 );
  carrierfreq = 0.0;
  if ( index < (int)FICurve[side].size() ) {
    lock();
    fi = FICurve[side][FICurve[side].size()-1-index];
    carrierfreq = FICurveCarrier[side][FICurveCarrier[side].size()-1-index];
    unlock();
  }
  return fi;
}


void Session::addFICurve( const MapD &ficurve, int side, double carrierfreq )
{
  lock();
  FICurve[side].push_back( ficurve );
  FICurveCarrier[side].push_back( carrierfreq );
  unlock();
  plot();
}


MapD Session::onFICurve( int side, double carrierfreq ) const
{
  // default values:
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  if ( ::fabs( carrierfreq ) < 1.0e-8 ) {
    lockMetaData();
    carrierfreq = metaData().number( "Cell>" + side > 0 ? "right frequency" : "left frequency" );
    if ( carrierfreq < 0.0 )
      carrierfreq = metaData().number( "Cell>best frequency" );
    unlockMetaData();
  }
  // search f-I curve:
  MapD ofi( 0 );
  lock();
  for ( int k=OnFICurve[side].size()-1; k>=0; k-- ) {
    if ( ::fabs( OnFICurveCarrier[side][k] - carrierfreq ) < 1.0 ) {
      ofi = OnFICurve[side][k];
      break;
    }
  }
  unlock();
  return ofi;
}


MapD Session::onFICurve( int index, int side, double &carrierfreq ) const
{
  // default values:
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  // get f-I curve:
  MapD onfi( 0 );
  carrierfreq = 0.0;
  if ( index < (int)OnFICurve[side].size() ) {
    lock();
    onfi = OnFICurve[side][OnFICurve[side].size()-1-index];
    carrierfreq = OnFICurveCarrier[side][OnFICurveCarrier[side].size()-1-index];
    unlock();
  }
  return onfi;
}


void Session::addOnFICurve( const MapD &onficurve, int side, double carrierfreq )
{
  lock();
  OnFICurve[side].push_back( onficurve );
  OnFICurveCarrier[side].push_back( carrierfreq );
  unlock();
  plot();
}


MapD Session::ssFICurve( int side, double carrierfreq ) const
{
  // default values:
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  if ( ::fabs( carrierfreq ) < 1.0e-8 ) {
    lockMetaData();
    carrierfreq = metaData().number( "Cell>" + side > 0 ? "right frequency" : "left frequency" );
    if ( carrierfreq < 0.0 )
      carrierfreq = metaData().number( "Cell>best frequency" );
    unlockMetaData();
  }
  // search f-I curve:
  MapD sfi( 0 );
  lock();
  for ( int k=SSFICurve[side].size()-1; k>=0; k-- ) {
    if ( ::fabs( SSFICurveCarrier[side][k] - carrierfreq ) < 1.0 ) {
      sfi = FICurve[side][k];
      break;
    }
  }
  unlock();
  return sfi;
}


MapD Session::ssFICurve( int index, int side, double &carrierfreq ) const
{
  // default values:
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  // get f-I curve:
  MapD ssfi( 0 );
  carrierfreq = 0.0;
  if ( index < (int)SSFICurve[side].size() ) {
    lock();
    ssfi = SSFICurve[side][SSFICurve[side].size()-1-index];
    carrierfreq = SSFICurveCarrier[side][SSFICurveCarrier[side].size()-1-index];
    unlock();
  }
  return ssfi;
}


void Session::addSSFICurve( const MapD &ssficurve, int side, double carrierfreq )
{
  lock();
  SSFICurve[side].push_back( ssficurve );
  SSFICurveCarrier[side].push_back( carrierfreq );
  unlock();
  plot();
}


void Session::updateBestSide( void )
{
  lockMetaData();
  Options &mo = metaData().section( "Cell>Cell properties" );
  int bestnoise = -1;
  double left = mo.number( "left noise threshold" );
  double right = mo.number( "right noise threshold" );
  if ( left > 0.0 && right > 0.0 )
    bestnoise = left < right ? 0 : 1;
  else
    bestnoise = left > 0.0 ? 0 : ( right > 0.0 ? 1 : -1 );

  int bestsine = -1;
  left = mo.number( "left threshold" );
  right = mo.number( "right threshold" );
  if ( left > 0.0 && right > 0.0 )
    bestsine = left < right ? 0 : 1;
  else
    bestsine = left > 0.0 ? 0 : ( right > 0.0 ? 1 : -1 );

  if ( bestsine >= 0 ) {
    mo.selectText( "best side", bestsine == 0 ? "left" : "right" );
  }
  else if ( bestnoise >= 0 )
    mo.selectText( "best side", bestnoise == 0 ? "left" : "right" );
  unlockMetaData();
}


void Session::plot( void )
{
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 )
    return;

  lock();
  lockMetaData();
  P.lock();

  Options &mo = metaData().section( "Cell>Cell properties" );

  // threshold curves:
  P[0].clear();
  double bf = mo.number( "best frequency" );
  if ( bf > 0.0 )
    P[0].plotVLine( 0.001*bf, Plot::White, 2 );
  if ( ! OldThreshCurve[0].empty() )
    P[0].plot( OldThreshCurve[0], 0.001, Plot::Gray, 2, Plot::LongDash );
  if ( ! OldThreshCurve[1].empty() )
    P[0].plot( OldThreshCurve[1], 0.001, Plot::Gray, 2, Plot::Solid );
  for ( int k=0; k < int(ThreshCurve[0].size()) - 1; k++ )
    P[0].plot( ThreshCurve[0][k], 0.001, Plot::DarkOrange, 2, Plot::LongDash );
  for ( int k=0; k < int(ThreshCurve[1].size()) - 1; k++ )
    P[0].plot( ThreshCurve[1][k], 0.001, Plot::DarkOrange, 2, Plot::Solid );
  if ( ! ThreshCurve[0].empty() )
    P[0].plot( ThreshCurve[0].back(), 0.001, Plot::Yellow, 3, Plot::LongDash );
  if ( ! ThreshCurve[1].empty() )
    P[0].plot( ThreshCurve[1].back(), 0.001, Plot::Yellow, 3, Plot::Solid );

  // f-I curves:
  P[1].clear();
  double bt = mo.number( "best threshold" );
  if ( bt > 0.0 )
    P[1].plotVLine( bt, Plot::White, 2 );
  double bs = mo.number( "best saturation" );
  if ( bs > 0.0 )
    P[1].plotVLine( bs, Plot::White, 2 );
  if ( ! OldFICurve[0].empty() )
    P[1].plot( OldFICurve[0], 1.0, Plot::White, 2, Plot::LongDash );
  if ( ! OldFICurve[1].empty() )
    P[1].plot( OldFICurve[1], 1.0, Plot::White, 2, Plot::Solid );
  double lbf = mo.number( "left frequency" );
  double rbf = mo.number( "right frequency" );
  int ssl = -1;
  int ssr = -1;
  int onl = -1;
  int onr = -1;
  int fil = -1;
  int fir = -1;
  for ( int k=0; k < int(SSFICurve[0].size()); k++ ) {
    if ( ::fabs( SSFICurveCarrier[0][k] - lbf ) < 1.0 ) {
      ssl = k;
      P[1].plot( SSFICurve[0][k], 1.0, Plot::OrangeRed, 2, Plot::LongDash );
    }
  }
  for ( int k=0; k < int(SSFICurve[1].size()); k++ ) {
    if ( ::fabs( SSFICurveCarrier[1][k] - rbf ) < 1.0 ) {
      ssr = k;
      P[1].plot( SSFICurve[1][k], 1.0, Plot::OrangeRed, 2, Plot::Solid );
    }
  }
  for ( int k=0; k < int(OnFICurve[0].size()); k++ ) {
    if ( ::fabs( OnFICurveCarrier[0][k] - lbf ) < 1.0 ) {
      onl = k;
      P[1].plot( OnFICurve[0][k], 1.0, Plot::DarkGreen, 2, Plot::LongDash );
    }
  }
  for ( int k=0; k < int(OnFICurve[1].size()); k++ ) {
    if ( ::fabs( OnFICurveCarrier[1][k] - rbf ) < 1.0 ) {
      onr = k;
      P[1].plot( OnFICurve[1][k], 1.0, Plot::DarkGreen, 2, Plot::Solid );
    }
  }
  for ( int k=0; k < int(FICurve[0].size()); k++ ) {
    if ( ::fabs( FICurveCarrier[0][k] - lbf ) < 1.0 ) {
      fil = k;
      P[1].plot( FICurve[0][k], 1.0, Plot::DarkOrange, 2, Plot::LongDash );
    }
  }
  for ( int k=0; k < int(FICurve[1].size()); k++ ) {
    if ( ::fabs( FICurveCarrier[1][k] - rbf ) < 1.0 ) {
      fir = k;
      P[1].plot( FICurve[1][k], 1.0, Plot::DarkOrange, 2, Plot::Solid );
    }
  }
  if ( ssl >= 0 )
    P[1].plot( SSFICurve[0][ssl], 1.0, Plot::Red, 3, Plot::LongDash );
  if ( ssr >= 0 )
    P[1].plot( SSFICurve[1][ssr], 1.0, Plot::Red, 3, Plot::Solid );
  if ( onl >= 0 )
    P[1].plot( OnFICurve[0][onl], 1.0, Plot::Green, 3, Plot::LongDash );
  if ( onr >= 0 )
    P[1].plot( OnFICurve[1][onr], 1.0, Plot::Green, 3, Plot::Solid );
  if ( fil >= 0 )
    P[1].plot( FICurve[0][fil], 1.0, Plot::Orange, 3, Plot::LongDash );
  if ( fir >= 0 )
    P[1].plot( FICurve[1][fir], 1.0, Plot::Orange, 3, Plot::Solid );

  P.unlock(); 
  unlockMetaData();
  unlock();

  P.draw();
}


addControl( Session, auditory );

}; /* namespace auditory */

#include "moc_session.cc"
