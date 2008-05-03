/*
  savefiles.cc
  Write data to files

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

#include <cstdio>
#include <qdatetime.h>
#include <qpainter.h>
#include <qtooltip.h>
#include <relacs/relacswidget.h>
#include <relacs/savefiles.h>

namespace relacs {


SaveFiles::SaveFiles( RELACSWidget *rw, int height,
		      QWidget *parent, const char *name )
  : QHBox( parent, name ),
    Options(),
    RW( rw ),
    StimulusDataLock( true )
{
  setPath( defaultPath() );
  Path = "";  // XXX ????

  OpenPath = true;
  OpenTrigger = true;
  OpenTrace = true;
  OpenEvents = true;

  FilesOpen = false;
  Writing = false;

  TF = 0;
  VF = 0;
  EF.clear();

  clearRemoveFiles();

  TraceToWrite = 0;
  SessionTime = 0.0;
  TraceIndex = 0;
  TraceOffs = 0;
  SignalOffs = 0;

  EventsToWrite.clear();
  EventFileNames.clear();
  EventOffs.clear();
  EventLines.clear();
  SignalEvents.clear();

  StimulusToWrite.clear();
  StimulusData = false;
  TriggerKey.clear();

  ToggleOn = false;
  ToggleData = false;

  ReProName = "";
  ReProAuthor = "";
  ReProVersion = "";
  ReProDate = "";
  ReProSettings.clear();
  ReProData = false;
  
  FileNumber = 0;
  FileName = "";
  FileTime = ::time( 0 );

  setFixedHeight( height );

  FileLabel = new QLabel( "no files open", this );
  FileLabel->setTextFormat( PlainText );
  FileLabel->setIndent( 2 );
  FileLabel->setAlignment( AlignLeft | AlignVCenter );
  QToolTip::add( FileLabel, "The directory where files are currently stored" );

  SaveLabel = new SpikeTrace( 0.8, 8, 3, this );
  SaveLabel->setFixedWidth( SaveLabel->minimumSizeHint().width() );
  QToolTip::add( SaveLabel, "An animation indicating that raw data are stored on disk" );
}


SaveFiles::~SaveFiles()
{
  closeFiles();

  clearRemoveFiles();

  TraceToWrite = 0;

  EventsToWrite.clear();
  EventFileNames.clear();
  EventOffs.clear();

  StimulusToWrite.clear();
}


bool SaveFiles::openState( void ) const
{
  return ( OpenTrace || OpenTrigger || 
	   OpenEvents || OpenPath );
}


bool SaveFiles::writing( void ) const
{
  return Writing;
}


bool SaveFiles::saving( void ) const
{
  return FilesOpen;
}


string SaveFiles::path( void ) const
{
  return Path;
}


void SaveFiles::setPath( const string &path )
{
  Path = path;
  if ( Path.size() > 0 && Path[Path.size()-1] != '/' )
    Path += '/';
  setenv( "RELACSDATAPATH", Path.c_str(), 1 );
}


string SaveFiles::addPath( const string &file ) const
{
  return path() + file;
}


string SaveFiles::defaultPath( void ) const
{
  RW->SS.lock();
  string dp = RW->SS.text( "defaultpath" );
  RW->SS.unlock();
  if ( dp.size() > 0 && dp[dp.size()-1] != '/' )
    dp += '/';
  return dp;
}


string SaveFiles::addDefaultPath( const string &file ) const
{
  return defaultPath() + file;
}


void SaveFiles::notify( void )
{
  RW->notifyStimulusData();
}


void SaveFiles::lock( void ) const
{
  StimulusDataLock.lock();
}


void SaveFiles::unlock( void ) const
{
  StimulusDataLock.unlock();
}


QMutex *SaveFiles::mutex( void )
{
  return &StimulusDataLock;
}


void SaveFiles::polish( void )
{
  QWidget::polish();

  NormalFont = FileLabel->font();
  HighlightFont = QFont( fontInfo().family(), fontInfo().pointSize()*4/3, QFont::Bold );

  RW->SS.lock();
  Str fn = RW->SS.text( "pathformat" );
  RW->SS.unlock();
  fn.format( localtime( &FileTime ) );
  fn.format( 99, 'n', 'd' );
  fn.format( "aa", 'a' );
  fn.format( "AA", 'A' );
  FileLabel->setFixedWidth( QFontMetrics( HighlightFont ).boundingRect( fn.c_str() ).width() + 8 );

  NormalPalette = FileLabel->palette();
  HighlightPalette = FileLabel->palette();
  HighlightPalette.setColor( QPalette::Normal, QColorGroup::Foreground, red );
  HighlightPalette.setColor( QPalette::Inactive, QColorGroup::Foreground, red );
}


void SaveFiles::write( bool on )
{
  //  cerr << "write toggle: " << on << '\n';
  ToggleData = true;
  ToggleOn = on;
}


void SaveFiles::writeToggle( void )
{
  //  cerr << "writeToggle(): " << ToggleData << ", on=" << ToggleOn << '\n';

  if ( ToggleData ) {
    Writing = ToggleOn;
    SaveLabel->setPause( !writing() );
    ToggleData = false;
  }

}


void SaveFiles::write( const InList &data, const EventList &events )
{
  //  cerr << "SaveFiles::write( InList &data, EventList &events )\n";

  // update write status:
  writeToggle();

  // indicate the new RePro:
  writeRePro();

  if ( !writing() )
    return;

  // write trace data:
  TraceToWrite = &data;
  /*
  //      cerr << "writeTrace\n";
  if ( VF != 0 && Writing ) {
    TraceOffs += TraceToWrite->saveBinary( VF, -1, SaveFilesMode, TraceIndex );
    TraceIndex = TraceToWrite->currentIndex( -1 );
    SignalOffs = TraceOffs - TraceIndex + TraceToWrite->signalIndex( -1 ); // XXX this works only if all traces are written!
  }
  */

  // write event data:
  for ( int k=0; k<(int)EventsToWrite.size() && k<events.size(); k++ )
    EventsToWrite[k] = &events[k];
  //      cerr << "writeEvents\n";
  if ( ! EF.empty() && Writing ) {
    for ( unsigned int k=0; k<EventsToWrite.size() && k<EF.size(); k++ ) {
      if ( EF[k] != 0 ) {

	while ( EventOffs[k] < EventsToWrite[k]->size() ) {
	  double et = (*EventsToWrite[k])[EventOffs[k]];
	  if ( EventsToWrite[0]->size() > 0 &&
	       et >= EventsToWrite[0]->back() &&
	       ( EventOffs[k] == 0 || 
		 (*EventsToWrite[k])[EventOffs[k]-1] < EventsToWrite[0]->back() ) ) {
	    SignalEvents[k] = EventLines[k];
	    *EF[k] << '\n';
	  }
	  *EF[k] << Str( et - SessionTime, 0, 5, 'f' ) << '\n';
	  EventLines[k]++;
	  EventOffs[k]++;
	}

      }
    }
  }

}


void SaveFiles::write( const OutData &signal )
{
  //  cerr << "SaveFiles::write( OutData &signal )\n";

  // write last trial data here!
  // we know that trace and events were written just before RePro::read
  // was called (from which the write( signal ) originates. 
  // Therefore all indices are known for writing the data of the last signal.
  writeStimulus();

  if ( signal.error() != 0 )
    return;

  if ( StimulusData )
    RW->printlog( "! warning: SaveFiles::write( OutData & ) -> already stimulus data there" );

  StimulusData = true;
  StimulusToWrite.add( &signal );
}


void SaveFiles::write( const OutList &signal )
{
  //  cerr << "SaveFiles::write( OutList &signal )\n";

  // write last trial data here!
  // we know that trace and events were written just before RePro::read
  // was called (from which the write( signal ) originates. 
  // Therefore all indices are known for writing the data of the last signal.
  writeStimulus();

  if ( signal.empty() || signal[0].failed() )
    return;

  if ( StimulusData )
    RW->printlog( "! warning: SaveFiles::write( OutList& ) -> already stimulus data there" );

  StimulusData = true;
  for ( int k=0; k<signal.size(); k++ )
    StimulusToWrite.add( &signal[k] );
}


void SaveFiles::writeStimulus( void )
{
  if ( StimulusData ) {
    
    //      cerr << "writeStimulus \n";
    
    // trigger file:
    if ( TF != 0 && Writing ) {
      TriggerKey.save( *TF, SignalOffs, 0 );
      for ( int k=0; k<TraceToWrite->size(); k++ )
	if ( (*TraceToWrite)[k].mode() & SaveFilesMode ) {
	  TriggerKey.save( *TF, (*TraceToWrite)[k].gain() * (*TraceToWrite)[k].scale() );
	  TriggerKey.save( *TF, (*TraceToWrite)[k].offset() );
	}
      for ( unsigned int k=0; k<EF.size(); k++ )
	if ( EF[k] != 0 ) {
	  TriggerKey.save( *TF, SignalEvents[k] );
	  TriggerKey.save( *TF, EventsToWrite[k]->meanRate() );  // XXX adaptive Zeit!
	  TriggerKey.save( *TF, EventsToWrite[k]->meanSize() );
	}
      lock();
      if ( !Options::empty() ) {
	for( int k=0; k<Options::size(); k++ )
	  TriggerKey.save( *TF, (*this)[k].number() );
      }
      unlock();
      // XXX this is not really multi board!
      TriggerKey.save( *TF, (*TraceToWrite)[0].signalTime() - SessionTime );
      // StimulusToWrite:
      TriggerKey.save( *TF, 1000.0*StimulusToWrite[0].delay() );
      for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
	for ( int j=0; j<StimulusToWrite.size(); j++ ) {
	  if ( StimulusToWrite[j] == RW->AQ->outTrace( k ) ) {
	    TriggerKey.save( *TF, 0.001*StimulusToWrite[j].sampleRate() );
	    TriggerKey.save( *TF, 1000.0*StimulusToWrite[j].length() );
	    TriggerKey.save( *TF, StimulusToWrite[j].intensity() );
	    TriggerKey.save( *TF, StimulusToWrite[j].carrierFreq() );
	    TriggerKey.save( *TF, StimulusToWrite[j].ident() );
	  }
	  else {
	    TriggerKey.save( *TF, "" );
	    TriggerKey.save( *TF, "" );
	    TriggerKey.save( *TF, "" );
	    TriggerKey.save( *TF, "" );
	    TriggerKey.save( *TF, "" );
	  }
	}
      }
      *TF << endl;
    }
    
    StimulusData = false;
    StimulusToWrite.clear();
  }
}


void SaveFiles::write( const RePro &RP )
{
  //  cerr << "SaveFiles::write( const RePro &RP ) \n";

  if ( ReProData )
    RW->printlog( "! warning: SaveFiles::write( RePro & ) -> already RePro data there." );
  ReProData = true;
  ReProName =  RP.name();
  ReProAuthor =  RP.author();
  ReProVersion =  RP.version();
  ReProDate =  RP.date();
  ReProSettings = RP;

  // write last stimulus here!
  // it is the probably unfinished one of the previous RePro.
  writeStimulus();
}


void SaveFiles::writeRePro( void )
{
  //  cerr << "write RePro\n";

  if ( ReProData ) {
    
    // trigger file:
    if ( TF != 0 && Writing ) {
      *TF << '\n';
      *TF << "# repro: " << ReProName  << '\n';
      *TF << "# author: " << ReProAuthor << '\n';
      *TF << "# version: " << ReProVersion << '\n';
      *TF << "# date: " << ReProDate << '\n';
      if ( ! ReProSettings.empty() ) {
	ReProSettings.setFlags( 0 );
	ReProSettings.setTypeFlags( 1, -Parameter::Blank );
	ReProSettings.save( *TF, "# ", -1, 1, false, true );
      }
      ReProSettings.clear();

      // write TriggerKey:
      *TF << '\n';
      TriggerKey.saveKey( *TF, true, true );
    }

    ReProData = false;
  }
}


void SaveFiles::addRemoveFile( const string &filename )
{
  RemoveFiles.push_back( filename );
}


void SaveFiles::clearRemoveFiles( void )
{
  RemoveFiles.clear();
}


bool SaveFiles::tryFile( const string &filename )
{
  string fs = path() + filename;
  ifstream f( fs.c_str() );
  return f.good();
}


ofstream *SaveFiles::openFile( const string &filename, int type )
{
  string fs = path() + filename;
  addRemoveFile( fs );
  ofstream *f = new ofstream( fs.c_str(), ofstream::openmode( type ) );
  if ( ! f->good() ) {
    f = 0;
    RW->printlog( "SaveFiles::openFile: can't open <" + fs + ">" );
  }
  return f;
}


void SaveFiles::createTraceFile( const InList &data )
{
  // init trace variables:
  TraceToWrite = &data;
  SessionTime = (*TraceToWrite)[0].currentTime();
  TraceIndex = (*TraceToWrite)[0].currentIndex();
  TraceOffs = 0;
  SignalOffs = 0;

  /*
  // create file for trace-data:
  string filename = "traces." + TraceToWrite->binaryExtension( -1 );
  VF = openFile( filename, ios::out | ios::binary );
  */
  VF = 0;
}


void SaveFiles::createEventFiles( const EventList &events )
{
  EF.clear();
  EF.reserve( events.size() );
  EventsToWrite.resize( events.size() );
  EventFileNames.resize( events.size() );
  EventOffs.resize( events.size() );
  EventLines.resize( events.size() );
  SignalEvents.resize( events.size() );

  // a file for each event:
  for ( int k=0; k<events.size(); k++ ) {

    // init event variables:
    EventsToWrite[k] = &events[k];
    EventOffs[k] = events[k].size();
    EventLines[k] = 0;
    SignalEvents[k] = 0;

    // create file:
    if ( ( events[k].mode() & SaveFilesMode ) &&
	 !(events[k].mode() & StimulusEventMode) ) {
      Str fn = events[k].ident();
      EventFileNames[k] = fn.lower() + "-events.dat";
      ofstream *ef = openFile( EventFileNames[k], ios::out );
      EF.push_back( ef );
      if ( ef->good() ) {
	// write key:
	*ef << "events: " << events[k].ident() << '\n';
	*ef << '\n';
	*ef << "#Key\n";
	*ef << "# t\n";
	*ef << "# sec\n";
      }
      else
	EventFileNames[k] = "";
    }
    else {
      EF.push_back( 0 );
      EventFileNames[k] = "";
    }
  }
}


void SaveFiles::createTriggerFile( const InList &data, const EventList &events )
{
  // init stimulus variables:
  StimulusData = false;

  // create file for trigger to trace:
  TF = openFile( "trigger.dat", ios::out );

  if ( (*TF) ) {
    // write header:
    *TF << "# sample interval: " << Str( 1000.0*data[0].sampleInterval(), 0, 2, 'f' ) << "ms\n";
    //    *TF << "#      trace file: " << "traces." << data.binaryExtension( -1 ) << '\n';
    for ( unsigned int k=0; k<EventFileNames.size(); k++ ) {
      if ( ! EventFileNames[k].empty() )
	*TF << "#      event file: " << EventFileNames[k] << '\n';
    }
    *TF << "# signals:\n";
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      TraceSpec trace( RW->AQ->outTrace( k ) );
      *TF << "#   identifier" + Str( k+1 ) + ": " << trace.traceName() << '\n';
      *TF << "#       device" + Str( k+1 ) + ": " << trace.device() << '\n';
      *TF << "#      channel" + Str( k+1 ) + ": " << trace.channel() << '\n';
      *TF << "# signal delay" + Str( k+1 ) + ": " << 1000.0*trace.signalDelay() << "ms\n";
      *TF << "# maximum rate" + Str( k+1 ) + ": " << 0.001*trace.maxSampleRate() << "kHz\n";
    }
    *TF << '\n';

    // create key:
    TriggerKey.clear();
    TriggerKey.addLabel( "traces" );
    TriggerKey.addLabel( "index" );
    TriggerKey.addNumber( "index", "word", "%10.0f" );
    for ( int k=0; k<data.size(); k++ )
      if ( data[k].mode() & SaveFilesMode ) {
	TriggerKey.addLabel( data[k].ident() );
	TriggerKey.addNumber( "factor", "mV", "%9.4g" );
	TriggerKey.addNumber( "offset", "mV", "%8.3g" );
      }
    TriggerKey.addLabel( "events" );
    for ( unsigned int k=0; k<EF.size(); k++ )
      if ( EF[k] != 0 ) {
	TriggerKey.addLabel( events[k].ident() );
	TriggerKey.addNumber( "index", "line", "%10.0f" );
	TriggerKey.addNumber( "freq", "Hz", "%6.1f" );
	TriggerKey.addNumber( "size", "mV", "%6.1f" );
      }
    lock();
    if ( !Options::empty() ) {
      TriggerKey.addLabel( "data" );
      TriggerKey.addLabel( "data" );
      const Options &data = *this;
      for( int k=0; k<data.size(); k++ )
	TriggerKey.addNumber( data[k].ident(), data[k].outUnit(), data[k].format() );
    }
    unlock();
    TriggerKey.addLabel( "stimulus" );
    TriggerKey.addLabel( "timing" );
    TriggerKey.addNumber( "time", "s", "%11.5f" );
    TriggerKey.addNumber( "delay", "ms", "%5.1f" );
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      TriggerKey.addLabel( RW->AQ->outTraceName( k ) );
      TriggerKey.addNumber( "rate", "kHz", "%8.3f" );
      TriggerKey.addNumber( "duration", "ms", "%8.0f" );
      TriggerKey.addNumber( "intensity", "dB", "%9.3f" );
      TriggerKey.addNumber( "frequency", "Hz", "%9.0f" );
      TriggerKey.addText( "signal", -30 );
    }
  }
}


void SaveFiles::openFiles( const InList &data, const EventList &events )
{
  // nothing to be done, if files are already open:
  if ( ( VF != 0 || !OpenTrace ) && 
       ( TF != 0 || !OpenTrigger ) && 
       ( !EF.empty() || !OpenEvents ) &&
       ( path() != defaultPath() || !OpenPath ) )
    return;

  // close all open files:
  closeFiles();
  clearRemoveFiles();

  // reset variables:
  ToggleData = false;
  ToggleOn = true;
  Writing = false;

  ReProData = false;
  ReProName = "";
  ReProAuthor = "";
  ReProVersion = "";
  ReProDate = "";
  ReProSettings.clear();

  setPath( defaultPath() );
  
  // no files need to be opened?
  if ( !OpenPath && !OpenTrace && !OpenTrigger && !OpenEvents )
    return;

  // get current time:
  time_t currenttime = RW->SN->startSessionTime();
  // time changed?
  if ( difftime( currenttime, FileTime ) != 0.0  )
    FileNumber = 0;
  FileTime = currenttime;

  // generate unused name for new files/directory:
  FileNumber++;
  int az = ('z'-'a'+1);
  for ( ; FileNumber <= az*az; FileNumber++ ) {

    // create file name:
    RW->SS.lock();
    FileName = RW->SS.text( "pathformat" );
    RW->SS.unlock();
    FileName.format( localtime( &FileTime ) );
    FileName.format( FileNumber, 'n', 'd' );
    int n = FileNumber-1;
    Str s = char( 'a' + n%az );
    n /= az;
    while ( n > 0 ) {
      s.insert( 0u, 1u, char( 'a' + n%az ) );
      n /= az;
    }
    FileName.format( s, 'a' );
    s.upper();
    FileName.format( s, 'A' );

    if ( OpenPath ) {
      // try to create new directory:
      char s[200];
      sprintf( s, "%s %s", "mkdir", FileName.c_str() );
      // success?
      if ( system( s ) == 0 ) {
	// set path:
	setPath( FileName );
	break;
      }
    }
    else {
      // try to open files:
      int ok = 0;
      if ( OpenTrigger ) {
	if ( tryFile( "trigger.dat" ) )
	  ok |= 1;
	
      }
      if ( OpenEvents ) {
	if ( tryFile( "events.dat" ) )
	  ok |= 2;
      }
      // files do not exist?
      if ( ok == 0 )
	break;
    }
  }
  // running out of names?
  if ( FileNumber > az*az ) {
    RW->printlog( "! panic: SaveFiles::openFiles -> can't create data file!" );
    return;
  }

  // open files:
  if ( OpenTrace )
    createTraceFile( data );
  if ( OpenEvents )
    createEventFiles( events );
  if ( OpenTrigger )
    createTriggerFile( data, events );
  FilesOpen = true;

  // message:
  RW->printlog( "save in " + FileName );

  // update widget:
  FileLabel->setFont( HighlightFont );
  FileLabel->setPalette( HighlightPalette );
  FileLabel->setText( FileName.c_str() );
  SaveLabel->setSpike( true );
}


void SaveFiles::closeFiles( void )
{
  ToggleData = true;
  ToggleOn = false;
  writeStimulus();

  if ( TF != 0 )
    delete TF;
  if ( VF != 0 )
    delete VF;
  for ( unsigned int k=0; k<EF.size(); k++ )
    if ( EF[k] != 0 )
      delete EF[k];
  TF = 0;
  VF = 0;
  EF.clear();

  FilesOpen = false;

  SaveLabel->setSpike( false );
}


void SaveFiles::deleteFiles( void )
{
  closeFiles();

  if ( path() != defaultPath() && 
       path() != "" ) {
    // remove the whole directory:
    string s = "rm -f -r " + path();
    system( s.c_str() );
  }
  else {
    // remove files:
    for ( vector<string>::iterator h = RemoveFiles.begin();
	  h != RemoveFiles.end();
	  ++h )
      remove( h->c_str() );
  }
  clearRemoveFiles();

  // message:
  RW->printlog( "discarded " + FileName );
  FileLabel->setFont( NormalFont );
  FileLabel->setPalette( NormalPalette );
  FileLabel->setText( "deleted" );

  setPath( defaultPath() );
  FileNumber--;
}


void SaveFiles::completeFiles( void )
{
  // no files need to be deleted:
  clearRemoveFiles();

  // no files are open:
  if ( VF == 0 && TF == 0 )
    return;

  //close all files:
  closeFiles();

  // message:
  FileLabel->setPalette( NormalPalette );
  RW->printlog( "saved as " + FileName );

  setPath( defaultPath() );
}



}; /* namespace relacs */

#include "moc_savefiles.cc"

