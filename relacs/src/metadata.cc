/*
  metadata.cc
  Manages meta data for a recording session.

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

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <qdatetime.h>
#include <relacs/str.h>
#include <relacs/optdialog.h>
#include <relacs/relacsplugin.h>
#include <relacs/relacswidget.h>
#include <relacs/metadata.h>

namespace relacs {


MetaDataSection::MetaDataSection( const string &name, int group, bool tab,
				  MetaData *md, RELACSWidget *rw )
  : ConfigClass( name, group, Save ),
    MD( md ),
    RW( rw ),
    Tab( tab )
{
}


MetaDataSection::~MetaDataSection( void )
{
}


void MetaDataSection::readConfig( StrQueue &sq )
{
  MD->lock();
  clear();
  int n = Options::size();
  Options::load( sq );
  for ( int k=n; k<Options::size(); k++ )
    Options::operator[]( k ).addFlags( MD->dialogFlag() | MD->configFlag() );
  MD->unlock();
}


void MetaDataSection::saveConfig( ofstream &str )
{
  MD->lock();
  Options::save( str, "  ", -1, MD->configFlag(), true, false );
  MD->unlock();
}


void MetaDataSection::notify( void )
{
  MD->notifyMetaData( configIdent() );
}


void MetaDataSection::clear( void )
{
  Options::clear();
}


void MetaDataSection::save( ofstream &str )
{
  str << "# " << configIdent() << '\n';
  Options::save( str, "# ", -1, MD->saveFlags(), false, true );
}


ostream &MetaDataSection::saveXML( ostream &str, int level, int indent ) const
{
  string indstr1( level*indent, ' ' );
  str << indstr1 << "<section name=\"" << configIdent() << "\">\n";
  Options::saveXML( str, MD->saveFlags(), level+1, indent );
  str << indstr1 << "</section>\n";
  return str;
}


bool MetaDataSection::ownTab( void ) const
{
  return Tab;
}

  
void MetaDataSection::setOwnTab( bool tab )
{
  Tab = tab;
}


MetaDataRecordingSection::MetaDataRecordingSection( bool tab,
						    MetaData *md,
						    RELACSWidget *rw )
  : MetaDataSection( "Recording", RELACSPlugin::Plugins, tab, md, rw )
{
  MD->addSaveFlags( standardFlag() );
  clear();
}


MetaDataRecordingSection::~MetaDataRecordingSection( void )
{
}


int MetaDataRecordingSection::configSize( void ) const
{
  return ( Options::size() - Options::size( standardFlag() ) );
}


void MetaDataRecordingSection::clear( void )
{
  Options::clear();
  addText( "File", "", standardFlag() );
  addText( "Date", "", standardFlag() );
  addText( "Time", "", standardFlag() );
  addNumber( "Recording duration", "", standardFlag() );
  addText( "Mode", RW->modeStr(), standardFlag() );
  addText( "Software", "RELACS", standardFlag() );
  addText( "Software version", RELACSVERSION, standardFlag() );
}


void MetaDataRecordingSection::save( ofstream &str )
{
  // set the <date> and <time> strings:
  RW->SS.lock();
  Str date = RW->SS.text( "dateformat" );
  time_t tt = RW->SN->startSessionTime();
  date.format( localtime( &tt ) );
  Str time = RW->SS.text( "timeformat" );
  time.format( localtime( &tt ) );
  RW->SS.unlock();

  // update file, date, time
  setText( "File", Str( RW->SF->path() ).preventSlash() );
  setText( "Date", date );
  setText( "Time", time );
  setNumber( "Recording duration", RW->SN->sessionTime()/60.0 );
  setUnit( "Recording duration", "min", "min" );
  setFormat( "Recording duration", "%.2f" );
  setText( "Mode", RW->modeStr() );

  str << "# " << configIdent() << '\n';
  Options::save( str, "# ", -1, MD->saveFlags(), false, true );
}


int MetaDataRecordingSection::standardFlag( void )
{
  return StandardFlag;
}


MetaData::MetaData( RELACSWidget *rw )
  : ConfigClass( "MetaData", RELACSPlugin::Core, Save ),
    SaveFlags( 0 ),
    Dialog( false ),
    PresetDialog( false ),
    MetaDataLock( true ),
    RW( rw )
{
}


MetaData::~MetaData( void )
{
}


void MetaData::add( const string &name, bool tab )
{
  MetaDataSections.push_back( new MetaDataSection( name, RELACSPlugin::Plugins, tab,
						   this, RW ) );
}


void MetaData::readConfig( StrQueue &sq )
{
  Options::clear();
  Options::load( sq );

  // read out options and create meta data sections:
  // (this cannot go into config(), since we also want to add things
  // to the same configuration file)
  lock();
  MetaDataSections.clear();
  MetaDataSections.reserve( 10 );
  MetaDataSections.push_back( new MetaDataRecordingSection( false, this, RW ) );
  int max = 10;
  for ( int k=0; k<max; k++ ) {
    string num = Str( k );
    string name = text( "section" + num );
    if ( ! name.empty() ) {
      int group = ( text( "config" + num ) == "core" ? RELACSPlugin::Core : RELACSPlugin::Plugins );
      bool tab = boolean( "tab" + num );
      if ( name == "Recording" ) {
	MetaDataSections[0]->setConfigGroup( group );
	MetaDataSections[0]->setOwnTab( tab );
      }
      else
	MetaDataSections.push_back( new MetaDataSection( name, group, tab, this, RW ) );
      max = k+10;
    }
  }
  unlock();
}


void MetaData::notifyMetaData( const string &section )
{
  RW->notifyMetaData( section );
}


void MetaData::save( void )
{
  lock();

  // write infofile:
  RW->SS.lock();
  ofstream of( RW->SF->addPath( RW->SS.text( "infofile" ) ).c_str() );
  RW->SS.unlock();
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ )
    MetaDataSections[k]->save( of );

  unlock();
}


ostream &MetaData::saveXML( ostream &str, int level, int indent ) const
{
  lock();

  // write XML:
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ )
    MetaDataSections[k]->saveXML( str, level, indent );

  unlock();
  return str;
}


void MetaData::clear( void )
{
  lock();
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( MetaDataSections[k]->configGroup() != RELACSPlugin::Core )
      MetaDataSections[k]->clear();
  }
  unlock();
}


bool MetaData::exist( const string &section ) const
{
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( MetaDataSections[k]->configIdent() == section )
      return true;
  }
  return false;
}


Options &MetaData::section( const string &section )
{
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( MetaDataSections[k]->configIdent() == section )
      return *MetaDataSections[k];
  }
  DummyOpts.clear();
  return DummyOpts;
}


const Options &MetaData::section( const string &section ) const
{
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( MetaDataSections[k]->configIdent() == section )
      return *MetaDataSections[k];
  }
  DummyOpts.clear();
  return DummyOpts;
}


void MetaData::lock( void ) const
{
  MetaDataLock.lock();
}


void MetaData::unlock( void ) const
{
  MetaDataLock.unlock();
}


QMutex *MetaData::mutex( void )
{
  return &MetaDataLock;
}


int MetaData::dialogFlag( void )
{
  return DialogFlag;
}


int MetaData::presetDialogFlag( void )
{
  return PresetDialogFlag;
}


int MetaData::configFlag( void )
{
  return ConfigFlag;
}


int MetaData::saveFlags( void ) const
{
  return SaveFlags;
}


void MetaData::setSaveFlags( int flags )
{
  SaveFlags = flags;
}


void MetaData::addSaveFlags( int flags )
{
  SaveFlags |= flags;
}


void MetaData::delSaveFlags( int flags )
{
  SaveFlags &= ~flags;
}


int MetaData::dialog( void )
{
  if ( Dialog )
    return 0;

  // setup options:
  DialogOpts.clear();
  bool dflttab = false;
  lock();
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( ! MetaDataSections[k]->ownTab() && ! MetaDataSections[k]->empty() ) {
      if ( ! dflttab ) {
	DialogOpts.addLabel( "Meta Data", LabelFlag, OptWidget::TabLabel );
	dflttab = true;
      }
      DialogOpts.addLabel( MetaDataSections[k]->configIdent(), LabelFlag, OptWidget::Bold );
      DialogOpts.append( *MetaDataSections[k], dialogFlag() );
    }
  }
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( MetaDataSections[k]->ownTab() && ! MetaDataSections[k]->empty() ) {
      DialogOpts.addLabel( MetaDataSections[k]->configIdent(), LabelFlag, OptWidget::TabLabel );
      DialogOpts.append( *MetaDataSections[k], dialogFlag() );
    }
  }
  DialogOpts.setToDefaults();
  unlock();

  // create and exec dialog:
  Dialog = true;
  OptDialog *od = new OptDialog( this );
  od->addOptions( DialogOpts );
  od->setSpacing( 10 );
  od->setMargin( 10 );
  od->setCaption( "Stop Session Dialog" );
  od->setRejectCode( -1 );
  od->addButton( "&Save", OptDialog::Accept, 1 );
  od->addButton( "&Discard", OptDialog::NoAction, 0 );
  od->addButton( "&Reset", OptDialog::Defaults );
  od->addButton( "&Cancel" );
  connect( od, SIGNAL( valuesChanged() ),
	   this, SLOT( dialogChanged() ) );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( dialogClosed( int ) ) );
  return od->exec();
}


void MetaData::dialogClosed( int r )
{
  DialogOpts.clear();
  Dialog = false;
}


void MetaData::dialogChanged( void )
{
  lock();
  bool changed = false;
  int secinx = -1;
  for ( int k=0; k<DialogOpts.size(); k++ ) {
    if ( DialogOpts[k].flags() & LabelFlag ) {
      // find section:
      for ( unsigned int j=0; j<MetaDataSections.size(); j++ ) {
	if ( DialogOpts[k].label() == MetaDataSections[j]->configIdent() ) {
	  if ( secinx >= 0 && changed )
	    notifyMetaData( MetaDataSections[secinx]->configIdent() );
	  secinx = j;
	  changed = false;
	  break;
	}
      }
    }
    else if ( secinx >= 0 && DialogOpts[k].flags() & Parameter::changedFlag() ) {
      // set parameter:
      MetaDataSections[ secinx ]->read( DialogOpts[k], Parameter::changedFlag() );
      changed = true;
    }
  }
  if ( secinx >= 0 && changed )
    notifyMetaData( MetaDataSections[secinx]->configIdent() );
  unlock();
}


void MetaData::presetDialog( void )
{
  if ( PresetDialog )
    return;

  // setup options:
  PresetDialogOpts.clear();
  bool dflttab = false;
  lock();
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( ! MetaDataSections[k]->ownTab() && ! MetaDataSections[k]->empty() ) {
      if ( ! dflttab ) {
	PresetDialogOpts.addLabel( "Meta Data", LabelFlag, OptWidget::TabLabel );
	dflttab = true;
      }
      PresetDialogOpts.addLabel( MetaDataSections[k]->configIdent(), LabelFlag, OptWidget::Bold );
      PresetDialogOpts.append( *MetaDataSections[k], dialogFlag() | presetDialogFlag() );
    }
  }
  for ( unsigned int k=0; k<MetaDataSections.size(); k++ ) {
    if ( MetaDataSections[k]->ownTab() && ! MetaDataSections[k]->empty() ) {
      PresetDialogOpts.addLabel( MetaDataSections[k]->configIdent(), LabelFlag, OptWidget::TabLabel );
      PresetDialogOpts.append( *MetaDataSections[k], dialogFlag() | presetDialogFlag() );
    }
  }
  PresetDialogOpts.setToDefaults();
  unlock();

  // create and exec dialog:
  PresetDialog = true;

  OptDialog *od = new OptDialog( this );
  od->addOptions( PresetDialogOpts );
  od->setSpacing( 10 );
  od->setMargin( 10 );
  od->setCaption( "Stop Session Dialog" );
  od->setRejectCode( -1 );
  od->addButton( "&Ok", OptDialog::Accept, 1 );
  od->addButton( "&Reset", OptDialog::Defaults );
  od->addButton( "&Cancel" );
  connect( od, SIGNAL( valuesChanged() ),
	   this, SLOT( presetDialogChanged() ) );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( presetDialogClosed( int ) ) );
  od->exec();
}


void MetaData::presetDialogClosed( int r )
{
  PresetDialogOpts.clear();
  PresetDialog = false;
}


void MetaData::presetDialogChanged( void )
{
  lock();
  bool changed = false;
  int secinx = -1;
  for ( int k=0; k<PresetDialogOpts.size(); k++ ) {
    if ( PresetDialogOpts[k].flags() & LabelFlag ) {
      // find section:
      for ( unsigned int j=0; j<MetaDataSections.size(); j++ ) {
	if ( PresetDialogOpts[k].label() == MetaDataSections[j]->configIdent() ) {
	  if ( secinx >= 0 && changed )
	    notifyMetaData( MetaDataSections[secinx]->configIdent() );
	  secinx = j;
	  changed = false;
	  break;
	}
      }
    }
    else if ( secinx >= 0 && PresetDialogOpts[k].flags() & Parameter::changedFlag() ) {
      // set parameter:
      MetaDataSections[ secinx ]->read( PresetDialogOpts[k], Parameter::changedFlag() );
      changed = true;
    }
  }
  if ( secinx >= 0 && changed )
    notifyMetaData( MetaDataSections[secinx]->configIdent() );
  unlock();
}


void MetaData::addActions( QPopupMenu *menu )
{
  QAction *action = new QAction( RW, "MetaDataAction" );
  action->setMenuText( "Session &Info..." );
  action->setAccel( ALT+Key_I );
  connect( action, SIGNAL( activated() ),
	   this, SLOT( presetDialog() ) );
  action->addTo( menu );
}


}; /* namespace relacs */

#include "moc_metadata.cc"

