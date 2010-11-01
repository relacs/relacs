/*
  patchclamp/session.cc
  Session for current-clamp recordings

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
#include <QLabel>
#include <relacs/patchclamp/session.h>
using namespace relacs;

namespace patchclamp {


Session::Session( void )
  : Control( "Session", "patchclamp", "Jan Benda", "1.0", "Oct 13, 2008" )

{
  QVBoxLayout *l = new QVBoxLayout;
  setLayout( l );

  CW = new OptWidget;
  CW->setVerticalSpacing( 1 );
  CW->setMargins( 4 );
  l->addWidget( CW );
  SW = new OptWidget;
  SW->setVerticalSpacing( 1 );
  SW->setMargins( 4 );
  l->addWidget( SW );

  SessionButton = new QPushButton( "Cell found" );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );
  l->addWidget( SessionButton );
}


Session::~Session( void )
{
}


void Session::initialize( void )
{
  // get trace indices:
  ephys::Traces::initialize( this, traces(), events() );
}


void Session::config( void )
{
  // additional meta data properties:
  if ( ! metaData().exist( "Cell" ) )
    metaData().add( "Cell", "Cell" );

  lockMetaData();
  Options &mo = metaData( "Cell" );
  mo.unsetNotify();
  mo.addLabel( "Cell properties", MetaDataSave );

  mo.addNumber( "vrest", "Resting potential V_rest", -1.0, -10.0, 10.0, 1.0, "V", "mV", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "rm", "Resistance R_m", -1.0, -1.0, 1000000.0, 1.0, "MOhm", "MOhm", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "rmss", "Steady-state resistance", -1.0, -1.0, 1000000.0, 1.0, "MOhm", "MOhm", "%.1f", MetaDataReset );
  mo.addNumber( "taum", "Time-constant tau_m", -1.0, -1.0, 100.0, 0.001, "s", "ms", "%.0f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "cm", "Capacitance C_m", -1.0, -1.0, 1000000.0, 1.0, "pF", "pF", "%.0f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "ithreshon", "Threshold for onset spikes", 0.0, -1000.0, 1000.0, 1.0, "nA", "nA", "%.3f", MetaDataReset );
  mo.addNumber( "ithreshss", "Threshold for periodic firing", 0.0, -1000.0, 1000.0, 1.0, "nA", "nA", "%.3f", MetaDataDisplay+MetaDataReset );

  mo.addStyle( OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, MetaDataDisplay );
  metaData().delSaveFlags( MetaData::dialogFlag() + MetaData::presetDialogFlag() );
  metaData().addSaveFlags( MetaData::configFlag() + MetaDataRecordingSection::standardFlag() + MetaDataSave );

  mo.setNotify();
  unlockMetaData();
}


void Session::initDevices( void )
{
  CW->assign( &metaData( "Cell" ), MetaDataDisplay, MetaDataReadOnly, true, 
	      0, metaDataMutex() );

  for ( int k=0; k<CurrentOutputs; k++ )
    stimulusData().setRequest( outTraceName( CurrentOutput[k] ), "DC current " + Str( k+1 ) );
  stimulusData().addText( "Drugs", "Applied drugs", "" ).setFormat( "%-20s" ).setFlags( 16 );
  SW->assign( &stimulusData(), 16+stimulusDataTraceFlag(),
	      stimulusDataTraceFlag(), true, 0, stimulusDataMutex() );
}


void Session::sessionStarted( void )
{
  SessionButton->setText( "Cell lost" );

  // reset values of metaData() options:
  lockMetaData();
  metaData( "Cell" ).setDefaults( MetaDataReset );
  metaData( "Cell" ).delFlags( MetaDataSave + Parameter::changedFlag(), MetaDataReset );
  unlockMetaData();
}


void Session::sessionStopped( bool saved )
{
  lock();
  SessionButton->setText( "Cell found" );
  unlock();
}


void Session::notifyMetaData( const string &section )
{
  if ( section == "Cell" ) {
    metaData( "Cell" ).addFlags( MetaDataSave, Parameter::changedFlag() );
    CW->updateValues( OptWidget::changedFlag() );
  }
}


void Session::notifyStimulusData( void )
{
  SW->updateValues( OptWidget::changedFlag() );
}


addControl( Session );

}; /* namespace patchclamp */

#include "moc_session.cc"
