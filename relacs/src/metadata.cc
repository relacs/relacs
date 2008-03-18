/*
  ../include/relacs/metadata.cc
  Manages meta data for a recording session.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <qtimer.h>
#include <qdatetime.h>
#include <relacs/optdialog.h>
#include <relacs/relacswidget.h>
#include <relacs/metadata.h>


MetaData::MetaData( RELACSWidget *rw )
  : ConfigClass( "MetaData", RELACSPlugin::Plugins, 
		 Save, DialogFlag + PresetDialogFlag ),
    SaveFlag( 0 ),
    Dialog( false ),
    PresetDialog( false ),
    MetaDataLock( true ),
    RW( rw )
{
}


MetaData::~MetaData( void )
{
}


void MetaData::readConfig( StrQueue &sq )
{
  lock();
  int n = Options::size();
  Options::readAppend( sq );
  for ( int k=n; k<Options::size(); k++ ) {
    Options::operator[]( k ).addFlags( dialogFlag() | configFlag() );
  }
  if ( RW->Setup.size() > 0 ) {
    Options::append( RW->Setup );
  }
  unlock();
}


void MetaData::saveConfig( ofstream &str )
{
  lock();
  Options::save( str, "  ", 0, dialogFlag() + presetDialogFlag(),
		 true, false );
  unlock();
}


void MetaData::notify( void )
{
  RW->notifyMetaData();
}


void MetaData::save( void )
{
  // set the <date> and <time> strings:
  RW->SS.lock();
  Str date = RW->SS.text( "dateformat" );
  time_t tt = RW->SN->startSessionTime();
  date.format( localtime( &tt ) );
  Str time = RW->SS.text( "timeformat" );
  time.format( localtime( &tt ) );
  RW->SS.unlock();

  lock();

  // update file, date, time
  setText( "File", Str( RW->FW->path() ).preventSlash() );
  setText( "Date", date );
  setText( "Time", time );
  setNumber( "Recording duration", RW->SN->sessionTime()/60.0 );
  setUnit( "Recording duration", "min", "min" );
  setFormat( "Recording duration", "%.2f" );
  setText( "Mode", RW->modeStr() );
  setText( "Software version", RELACSVERSION );

  // write infofile:
  RW->SS.lock();
  ofstream of( RW->FW->addPath( RW->SS.text( "infofile" ) ).c_str() );
  RW->SS.unlock();
  Options::save( of, "# ", -1, saveFlag(), false, true );

  unlock();
}


void MetaData::clear( void )
{
  lock();
  Options::clear();
  addLabel( "Experiment", standardFlag() + dialogFlag(), Parameter::TabLabel );
  addText( "File", "", standardFlag() );
  addText( "Date", "", standardFlag() );
  addText( "Time", "", standardFlag() );
  addNumber( "Recording duration", "", standardFlag() );
  addText( "Mode", RW->modeStr(), standardFlag() );
  addText( "Software", "RELACS", standardFlag() );
  addText( "Software version", RELACSVERSION, standardFlag() );
  unlock();
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


int MetaData::setupFlag( void )
{
  return SetupFlag;
}


int MetaData::saveFlag( void ) const
{
  return SaveFlag;
}


void MetaData::setSaveFlag( int flag )
{
  SaveFlag = flag;
}


int MetaData::dialog( void )
{
  if ( Dialog )
    return 0;

  // create and exec dialog:
  Dialog = true;

  OptDialog *od = new OptDialog( this );
  od->addOptions( *this, dialogFlag() | setupFlag(), 0, 0, &MetaDataLock );
  od->setSpacing( 10 );
  od->setMargin( 10 );
  od->setCaption( "Stop Session Dialog" );
  od->setRejectCode( -1 );
  od->addButton( "&Save", OptDialog::Accept, 1 );
  od->addButton( "&Discard", OptDialog::NoAction, 0 );
  od->addButton( "&Reset", OptDialog::Defaults );
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
  if ( PresetDialog )
    return;

  // create and exec dialog:
  PresetDialog = true;

  OptDialog *od = new OptDialog( this );
  od->addOptions( *this, dialogFlag() | presetDialogFlag() | setupFlag(),
		  0, 0, &MetaDataLock );
  od->setSpacing( 10 );
  od->setMargin( 10 );
  od->setCaption( "Stop Session Dialog" );
  od->setRejectCode( -1 );
  od->addButton( "&Ok", OptDialog::Accept, 1 );
  od->addButton( "&Reset", OptDialog::Defaults );
  od->addButton( "&Cancel" );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( presetDialogClosed( int ) ) );
  od->exec();
}


void MetaData::presetDialogClosed( int r )
{
  PresetDialog = false;
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


SetupData::SetupData( Options *md )
  : ConfigClass( "Setup", RELACSPlugin::Core, 
		 Save, SetupFlag ),
    MD( md )
{
}


SetupData::~SetupData( void )
{
}


void SetupData::readConfig( StrQueue &sq )
{
  clear();
  addLabel( "Set&up", 0, Parameter::TabLabel );
  addText( "Name", "Name", "Setup1" );
  readAppend( sq );
  addFlags( SetupFlag );
}


void SetupData::saveConfig( ofstream &str )
{
  if ( MD != 0 )
    MD->save( str, "  ", -1, SetupFlag, true, false );
}


int SetupData::configSize( void ) const
{
  return MD != 0 ? MD->size( SetupFlag ) : 0;
}


int SetupData::setupFlag( void )
{
  return SetupFlag;
}

