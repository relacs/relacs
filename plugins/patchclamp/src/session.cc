/*
  patchclamp/session.cc
  Session for current-clamp recordings

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

#include <qvbox.h>
#include <qlabel.h>
#include <relacs/patchclamp/session.h>
using namespace relacs;

namespace patchclamp {


Session::Session( void )
  : Control( "Session", "Info", "Patch-clamp",
	     "Jan Benda", "1.0", "Oct 13, 2008" )

{
  boxLayout()->setDirection( QBoxLayout::TopToBottom );

  PSW = new OptWidget( (QWidget*)this );
  PSW->setSpacing( 4 );
  PSW->setMargin( 4 );

  SessionButton = new QPushButton( "Cell found", this, "SessionButton" );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );
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

  mo.addNumber( "vrest", "Resting potential", -1.0, -10.0, 10.0, 1.0, "V", "mV", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "membraner", "Membrane resistance", -1.0, -1.0, 1000000.0, 1.0, "MOhm", "MOhm", "%.1f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "membranetau", "Membrane time constant", -1.0, -1.0, 100.0, 0.001, "s", "ms", "%.0f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "membranec", "Membrane capacitance", -1.0, -1.0, 1000000.0, 1.0, "pF", "pF", "%.0f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "ithresh", "Current threshold", 0.0, -1000.0, 1000.0, 1.0, "nA", "nA", "%.3f", MetaDataDisplay+MetaDataReset );
  mo.addNumber( "dc", "DC current", 0.0, -1000.0, 1000.0, 1.0, "nA", "nA", "%.3f", MetaDataDisplay+MetaDataReset );

  mo.addStyle( OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, MetaDataDisplay );
  metaData().delSaveFlags( MetaData::dialogFlag() + MetaData::presetDialogFlag() );
  metaData().addSaveFlags( MetaData::configFlag() + MetaDataRecordingSection::standardFlag() + MetaDataSave );

  mo.setNotify();
  unlockMetaData();
}


void Session::initDevices( void )
{
  PSW->assign( &metaData( "Cell" ), MetaDataDisplay, MetaDataReadOnly, true, 
	       0, metaDataMutex() );
  PSW->setSpacing( 2 );
  updateGeometry();
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
    PSW->updateValues( OptWidget::changedFlag() );
  }
}


addControl( Session );

}; /* namespace patchclamp */

#include "moc_session.cc"
