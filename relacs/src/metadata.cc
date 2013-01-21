/*
  metadata.cc
  Manages meta data for a recording session.

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

#include <cstdlib>
#include <cmath>
#include <ctime>
#include <QDateTime>
#include <relacs/str.h>
#include <relacs/optdialog.h>
#include <relacs/relacsplugin.h>
#include <relacs/relacswidget.h>
#include <relacs/savefiles.h>
#include <relacs/session.h>
#include <relacs/metadata.h>

namespace relacs {


MetaDataGroup::MetaDataGroup( int group, MetaData *md )
  : ConfigClass( "Metadata", group, Save ),
    MD( md )
{
  setConfigSelectMask( MD->configFlag() );
}


MetaDataGroup::~MetaDataGroup( void )
{
}


void MetaDataGroup::readConfig( StrQueue &sq )
{
  MD->lock();
  Options::clear();
  load( sq );
  addFlags( MD->configFlag() | MD->dialogFlag() );
  if ( configGroup() == RELACSPlugin::Plugins ) {
    section_iterator sp = findSection( "Recording" );
    if ( sp != sectionsEnd() )
      addRecordingOptions( *sp );
    else {
      Options &opt = newSection( "", "Recording" );
      addRecordingOptions( &opt );
    }
  }
  MD->add( this );
  MD->unlock();
}


void MetaDataGroup::saveConfig( ofstream &str )
{
  MD->lock();
  save( str, "  ", MD->configFlag(),
	Options::NoType | Options::PrintRequest | Options::PrintStyle );
  MD->unlock();
}


int MetaDataGroup::configSize( void ) const
{
  MD->lock();
  int n = size( MD->configFlag() );
  MD->unlock();
  return n;
}


void MetaDataGroup::clear( void )
{
  Options::clear();
  if ( configGroup() == RELACSPlugin::Plugins ) {
    Options &opt = newSection( "", "Recording" );
    addRecordingOptions( &opt );
  }
}


void MetaDataGroup::addRecordingOptions( Options *opt )
{
  opt->addText( "Name", "", MetaData::standardFlag() );
  opt->addText( "Folder", "", MetaData::standardFlag() );
  opt->addDate( "Date", MetaData::standardFlag() );
  opt->addTime( "Time", MetaData::standardFlag() );
  opt->addNumber( "Recording duration", 0.0, "min" ).setFlags( MetaData::standardFlag() );
  opt->addText( "Mode", "unknown", MetaData::standardFlag() );
  opt->addText( "Software", "RELACS", MetaData::standardFlag() );
  opt->addText( "Software version", RELACSVERSION, MetaData::standardFlag() );
}


MetaData::MetaData( RELACSWidget *rw )
  : SaveFlags( 0 ),
    CoreData( RELACSPlugin::Core, this ),
    PluginData( RELACSPlugin::Plugins, this ),
    Dialog( false ),
    MetaDataLock( QMutex::Recursive ),
    RW( rw )
{
  clear();
}


MetaData::~MetaData( void )
{
}


void MetaData::notify( void )
{
  RW->notifyMetaData();
}


void MetaData::save( const string &title, const Options &opts )
{
  lock();

  // update file, date, time:
  section_iterator sp = findSection( "Recording" );
  if ( sp != sectionsEnd() ) {
    (*sp)->setText( "Name", Str( RW->SF->path() ).preventedSlash().name() );
    (*sp)->setText( "Folder", Str( RW->SF->path() ).preventedSlash().expandedPath() );
    (*sp)->setDate( "Date", RW->SN->startSessionTime() );
    (*sp)->setTime( "Time", RW->SN->startSessionTime() );
    (*sp)->setNumber( "Recording duration", RW->SN->sessionTime()/60.0 );
    (*sp)->setUnit( "Recording duration", "min", "min" );
    (*sp)->setFormat( "Recording duration", "%.2f" );
    (*sp)->setText( "Mode", RW->modeStr() );
  }

  // write infofile:
  RW->SS.lock();
  ofstream of( RW->SF->addPath( RW->SS.text( "infofile" ) ).c_str() );
  RW->SS.unlock();

  // add model options:
  Options *mo = 0;
  if ( ! title.empty() && ! opts.empty() )
    mo = &newSection( opts, title ).addFlags( saveFlags() );

  // save meta data:
  Options::save( of, "# ", saveFlags(), Options::FirstOnly );

  if ( mo != 0 )
    erase( mo );

  unlock();

}


ostream &MetaData::saveXML( ostream &str, int level,
			    const string &names )
{
  lock();
  setSectionName( this, names );
  Options::saveXML( str, 0, Options::SwitchNameType | Options::FirstOnly, level );
  unlock();
  return str;
}


void MetaData::setSectionName( Options *opt, const string &names )
{
  if ( ! opt->name().empty() )
    opt->setType( opt->name() + '-' + names );
  for ( section_iterator sp = opt->sectionsBegin(); sp != opt->sectionsEnd(); ++sp )
    setSectionName( *sp, names );
}


void MetaData::clear( void )
{
  lock();
  // Clear only PluginData!
  //  CoreData.clear();
  PluginData.clear();
  Options::clear();
  unlock();
}


void MetaData::add( ConfigClass *opt )
{
  if ( opt->configGroup() == RELACSPlugin::Core )
    Options::clear();
  else {
    if ( ! PluginData.name().empty() || PluginData.sectionsSize() == 0 )
      newSection( &PluginData, true );
    else
      newSections( &PluginData, true );
    if ( ! CoreData.name().empty() || CoreData.sectionsSize() == 0 )
      newSection( &CoreData, true );
    else
      newSections( &CoreData, true );
  }
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


int MetaData::standardFlag( void )
{
  return StandardFlag;
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
  if ( ! MetaDataLock.tryLock( 5 ) )
    return -1000;
  setToDefaults();
  unlock();

  // create and exec dialog:
  Dialog = true;
  OptDialog *od = new OptDialog( this );
  string tabhotkeys = "sdotc";
  od->addOptions( *this, dialogFlag(), 0,
		  OptWidget::BoldSectionsStyle, &MetaDataLock, &tabhotkeys );
  od->setCaption( "Stop Session Dialog" );
  od->setRejectCode( -1 );
  od->addButton( "&Save", OptDialog::Accept, 1 );
  od->addButton( "&Discard", OptDialog::NoAction, 0 );
  od->addButton( "Rese&t", OptDialog::Defaults );
  od->addButton( "&Cancel" );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( dialogClosed( int ) ) );
  return od->exec();
}


void MetaData::dialogClosed( int r )
{
  Dialog = false;
}


void MetaData::presetDialog( void )
{
  if ( Dialog )
    return;

  // we do not want to block the event queue:
  if ( ! MetaDataLock.tryLock( 5 ) ) {
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+11 ) ) );
    return;
  }
  setToDefaults();
  unlock();

  // create and exec dialog:
  Dialog = true;
  OptDialog *od = new OptDialog( this );
  string tabhotkeys = "sdotc";
  od->addOptions( *this, dialogFlag()+presetDialogFlag(), 0,
		  OptWidget::BoldSectionsStyle, &MetaDataLock, &tabhotkeys );
  od->setCaption( "Stop Session Dialog" );
  od->setRejectCode( -1 );
  od->addButton( "&Ok", OptDialog::Accept, 1 );
  od->addButton( "Rese&t", OptDialog::Defaults );
  od->addButton( "&Cancel" );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( dialogClosed( int ) ) );
  od->exec();
}


void MetaData::customEvent( QEvent *qe )
{
  if ( qe->type() == QEvent::User+11 ) {
    presetDialog();
  }
  else
    QObject::customEvent( qe );
}


void MetaData::addActions( QMenu *menu )
{
  menu->addAction( "Session &Info...",
		   this, SLOT( presetDialog() ), Qt::ALT + Qt::Key_I );
}


}; /* namespace relacs */

#include "moc_metadata.cc"

