/*
  patchclamp/session.cc
  Session for current-clamp recordings

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

  QHBoxLayout *h = new QHBoxLayout;
  h->setContentsMargins( 2, 0, 2, 0 );
  h->setSpacing( 8 );
  l->addLayout( h );

  SW1 = new OptWidget;
  SW1->setVerticalSpacing( 1 );
  SW1->setMargins( 0 );
  h->addWidget( SW1 );

  SW2 = new OptWidget;
  SW2->setVerticalSpacing( 1 );
  SW2->setMargins( 0 );
  h->addWidget( SW2 );

  DW = new OptWidget;
  DW->setVerticalSpacing( 1 );
  DW->setMargins( 4 );
  l->addWidget( DW );

  SessionButton = new QPushButton( "Cell found" );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );
  l->addWidget( SessionButton );
}


Session::~Session( void )
{
}


void Session::preConfig( void )
{
  // get trace indices:
  ephys::Traces::initialize( this, traces(), events() );
}


void Session::config( void )
{
  // additional meta data properties:
  lockMetaData();
  metaData().unsetNotify();

  if ( ! metaData().existSection( "Cell" ) )
    metaData().newSection( "Cell" );
  Options &mo = metaData().section( "Cell" );
  mo.newSection( "Cell properties", MetaDataSave+MetaDataDisplay );

  mo.addNumber( "vrest", "Resting potential V_rest", -1.0, -10.0, 10.0, 1.0, "V", "mV", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "rm", "Resistance R_m", -1.0, -1.0, 1000000.0, 1.0, "MOhm", "MOhm", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "rmss", "Steady-state resistance", -1.0, -1.0, 1000000.0, 1.0, "MOhm", "MOhm", "%.1f", MetaDataReset );
  mo.addNumber( "cm", "Capacitance C_m", -1.0, -1.0, 1000000.0, 1.0, "pF", "pF", "%.0f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "taum", "Time-constant tau_m", -1.0, -1.0, 100.0, 0.001, "s", "ms", "%.0f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "em", "Reversal potential of leak current", 0.0, -500.0, 500.0, .01, "V", "mV", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "ithreshon", "Threshold for onset spikes", 0.0, -1000.0, 1000.0, 1.0, "nA", "nA", "%.3f", MetaDataReset );
  mo.addNumber( "ithreshss", "Threshold for periodic firing", 0.0, -1000.0, 1000.0, 1.0, "nA", "nA", "%.3f", MetaDataReset );

  //wanted cell parameters for dynamic clamp
  mo.newSection( "Wanted cell properties", MetaDataSave+MetaDataDisplay );
  mo.addNumber( "rnew", "wanted membrane resistance Rnew", 0.0, -1.0, 1000000.0, 1.0, "MOhm", "MOhm", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "enew", "wanted reversal potential of leak current Enew", 0.0, -500.0, 500.0, .01, "V", "mV", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "cnew", "wanted capacitance Cnew", 0.0, -1.0, 1000000.0, 1.0, "pF", "pF", "%.0f", MetaDataDisplay+MetaDataReset );

  //dynamic clamp input parameters
  mo.newSection( "Dynamic Clamp input parameters", MetaDataSave+MetaDataDisplay );
  mo.addNumber( "gleak", "Leak current conductance", 0.0, -1000000.0, 1000000.0, 1.0, "nS", "nS", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "Eleak", "Current to adjust resting potential", 0.0, -500.0, 500.0, .01, "pA", "pA", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "Cleak", "Capacitive current", 0.0, -100000.0, 1000000.0, 1.0, "pF", "pF", "%.0f", MetaDataDisplay+MetaDataReset );


  mo.addStyles( OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, MetaDataDisplay );
  metaData().delSaveFlags( MetaData::dialogFlag() + MetaData::presetDialogFlag() );
  metaData().addSaveFlags( MetaData::configFlag() + MetaData::standardFlag() + MetaDataSave );

  metaData().setNotify();
  unlockMetaData();
}


void Session::initDevices( void )
{
  CW->assign( *metaData().section( "Cell" ).findSection( "Cell properties" ),
	      MetaDataDisplay, MetaDataReadOnly, true,
	      0, metaDataMutex() );

  lockStimulusData();
  if ( stimulusData().size( stimulusDataTraceFlag() ) >= 4 ) {
    int n = 0;
    for ( int k=0; k<stimulusData().size(); k++ ) {
      if ( ( stimulusData()[k].flags() & stimulusDataTraceFlag() ) > 0 ) {
	if ( n <= stimulusData().size( stimulusDataTraceFlag() )/2 )
	  stimulusData()[k].addFlags( 32 );
	else
	  stimulusData()[k].addFlags( 64 );
	n++;
      }
    }
    unlockStimulusData();
    SW1->assign( &stimulusData(), 32, stimulusDataTraceFlag(), true, 0, stimulusDataMutex() );
    SW2->assign( &stimulusData(), 64, stimulusDataTraceFlag(), true, 0, stimulusDataMutex() );
    lockStimulusData();
  }
  else {
    unlockStimulusData();
    SW1->assign( &stimulusData(), stimulusDataTraceFlag(),
		 stimulusDataTraceFlag(), true, 0, stimulusDataMutex() );
    SW2->hide();
    lockStimulusData();
  }

  if ( ! stimulusData().exist( "Drugs" ) )
    stimulusData().addText( "Drugs", "Applied drugs", "" ).setFormat( "%-20s" ).setFlags( 16 );
  unlockStimulusData();

  DW->assign( &stimulusData(), 16, 0, true, 0, stimulusDataMutex() );
}


void Session::sessionStarted( void )
{
  SessionButton->setText( "Cell lost" );

  // reset values of metaData() options:
  lockMetaData();
  metaData().section( "Cell" ).setDefaults( MetaDataReset );
  metaData().section( "Cell" ).delFlags( MetaDataSave + Parameter::changedFlag(), MetaDataReset );
  unlockMetaData();
}


void Session::sessionStopped( bool saved )
{
  lock();
  SessionButton->setText( "Cell found" );
  unlock();
}


void Session::notifyMetaData( void )
{
  metaData().section( "Cell" ).addFlags( MetaDataSave, Parameter::changedFlag() );
  CW->updateValues( OptWidget::changedFlag(), false );
}


void Session::notifyStimulusData( void )
{
  SW1->updateValues( OptWidget::changedFlag(), false );
  SW2->updateValues( OptWidget::changedFlag(), false );
  DW->updateValues( OptWidget::changedFlag(), false );
}


addControl( Session, patchclamp );

}; /* namespace patchclamp */

#include "moc_session.cc"
