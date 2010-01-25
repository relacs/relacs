/*
  savefiles.cc
  Save data to files

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
  Path = "";
  PathTemplate = "%04Y-%02m-%02d-%a2a";
  DefaultPath = "";

  PathNumber = 0;
  PathTime = ::time( 0 );

  FilesOpen = false;
  Writing = false;

  SF = 0;
  XF = 0;
  TraceFiles.clear();
  EventFiles.clear();

  Stimuli.clear();
  StimulusData = false;
  StimulusKey.clear();
  SignalTime = -1.0;
  PrevSignalTime = -1.0;

  clearRemoveFiles();

  ToggleOn = false;
  ToggleData = false;

  ReProInfo.clear();
  ReProInfo.addText( "project" );
  ReProInfo.addText( "experiment" );
  ReProInfo.addText( "repro" );
  ReProInfo.addText( "author" );
  ReProInfo.addText( "version" );
  ReProInfo.addDate( "date" );
  ReProSettings.clear();
  ReProFiles.clear();
  ReProData = false;
  DatasetOpen = false;

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

  EventFiles.clear();

  Stimuli.clear();
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
  setenv( "RELACSDATAPATH", Path.c_str(), 1 );
}


string SaveFiles::addPath( const string &file ) const
{
  return path() + file;
}


void SaveFiles::storeFile( const string &file ) const
{
  for ( vector<string>::const_iterator sp = ReProFiles.begin(); sp != ReProFiles.end(); ++sp ) {
    if ( *sp == file )
      return;
  }
  ReProFiles.push_back( file );
}


string SaveFiles::pathTemplate( void ) const
{
  return PathTemplate;
}


void SaveFiles::setPathTemplate( const string &path )
{
  if ( path.empty() )
    return;

  PathTemplate = path;

  Str fn = PathTemplate;
  fn.format( localtime( &PathTime ) );
  fn.format( 99, 'n', 'd' );
  fn.format( "aa", 'a' );
  fn.format( "AA", 'A' );
  FileLabel->setFixedWidth( QFontMetrics( HighlightFont ).boundingRect( fn.c_str() ).width() + 8 );
}


string SaveFiles::defaultPath( void ) const
{
  return DefaultPath;
}


void SaveFiles::setDefaultPath( const string &defaultpath )
{
  if ( defaultpath.empty() )
    return;

  if ( Path == DefaultPath )
    setPath( defaultpath );
  DefaultPath = defaultpath;
  setenv( "RELACSDEFAULTPATH", DefaultPath.c_str(), 1 );
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

  Str fn = PathTemplate;
  fn.format( localtime( &PathTime ) );
  fn.format( 99, 'n', 'd' );
  fn.format( "aa", 'a' );
  fn.format( "AA", 'A' );
  FileLabel->setFixedWidth( QFontMetrics( HighlightFont ).boundingRect( fn.c_str() ).width() + 8 );

  NormalPalette = FileLabel->palette();
  HighlightPalette = FileLabel->palette();
  HighlightPalette.setColor( QPalette::Normal, QColorGroup::Foreground, red );
  HighlightPalette.setColor( QPalette::Inactive, QColorGroup::Foreground, red );
}


void SaveFiles::save( bool on, const InList &traces, const EventList &events  )
{
  //  cerr << "save toggle: " << on << '\n';
  if ( ! FilesOpen )
    return;

  // switch on writing?
  if ( on && ! Writing ) {
    // update offsets:
    for ( unsigned int k=0; k<TraceFiles.size(); k++ )
      TraceFiles[k].Index = traces[k].currentIndex();
    for ( unsigned int k=0; k<EventFiles.size(); k++ )
      EventFiles[k].Offset = events[k].size();
  }

  ToggleData = true;
  ToggleOn = on;
}


bool SaveFiles::saveToggle( const InList &traces, EventList &events )
{
  //  cerr << "saveToggle(): " << ToggleData << ", on=" << ToggleOn << '\n';

  if ( ToggleData ) {
    if ( RW->CurrentRePro == 0 ||
	 RW->CurrentRePro->reproTime() > 0.01 ||
	 StimulusData ) {
      if ( ToggleOn && ! Writing ) {
	// add recording event:
	for ( int k=0; k<events.size(); k++ ) {
	  if ( ( events[k].mode() & RecordingEventMode ) > 0 ) {
	    events[k].push( traces[0].pos( TraceFiles[0].Index ) );
	    break;
	  }
	}
	// update SessionTime to deal with the non written data:
	SessionTime += traces[0].interval( TraceFiles[0].Index - TraceFiles[0].LastIndex );
	TraceFiles[0].LastIndex = TraceFiles[0].Index;
      }
      Writing = ToggleOn;
      SaveLabel->setPause( !writing() );
      ToggleData = false;
    }
    else
      return true;
  }

  return false;
}


void SaveFiles::save( const InList &traces, EventList &events )
{
  // update save status:
  if ( saveToggle( traces, events ) )
    return;

  // indicate the new RePro:
  saveRePro();

  if ( events[0].size() > 0 ) {
    double st = events[0].back();
    if ( st > PrevSignalTime )
      SignalTime = st;
  }

  if ( saving() ) {
    save( traces );
    save( events );
  }

  saveStimulus();
}


void SaveFiles::save( const InList &traces )
{
  //  cerr << "SaveFiles::save( InList &traces )\n";

  // save trace data:
  if ( (int)TraceFiles.size() != traces.size() )
    cerr << "! error in SaveFiles::save( InList ) -> TraceFiles.size() != traces.size() !\n";
  for ( int k=0; k<(int)TraceFiles.size() && k<traces.size(); k++ ) {
    TraceFiles[k].Trace = &traces[k];
    if ( TraceFiles[k].Stream != 0 ) {
      if ( writing() ) {
	TraceFiles[k].Offset += traces[k].saveBinary( *TraceFiles[k].Stream,
						      TraceFiles[k].Index );
	TraceFiles[k].LastIndex = traces[k].currentIndex();
      }
      TraceFiles[k].Index = traces[k].currentIndex();
      if ( traces[k].signalIndex() >= 0 )
	TraceFiles[k].SignalOffset = TraceFiles[k].Offset - TraceFiles[k].Index
	  + traces[k].signalIndex();
    }
  }

}


void SaveFiles::save( const EventList &events )
{
  //  cerr << "SaveFiles::save( EventList &events )\n";

  // wait for availability of signal start time:
  if ( StimulusData && SignalTime < 0.0 )
    return;

  // save event data:
  double st = EventFiles[0].Events->size() > 0 ? EventFiles[0].Events->back() : EventFiles[0].Events->rangeBack();
  for ( int k=0; k<(int)EventFiles.size() && k<events.size(); k++ ) {
    EventFiles[k].Events = &events[k];

    if ( EventFiles[k].Stream != 0 ) {
      while ( EventFiles[k].Offset < EventFiles[k].Events->size() ) {
	double et = (*EventFiles[k].Events)[EventFiles[k].Offset];
	if ( et < st )
	  EventFiles[k].SignalEvent = EventFiles[k].Lines;
	else if ( EventFiles[k].Offset == 0 || 
		  (*EventFiles[k].Events)[EventFiles[k].Offset-1] < st ) {
	  EventFiles[k].SignalEvent = EventFiles[k].Lines;
	  if ( writing() )
	    *EventFiles[k].Stream << '\n';
	}
	if ( writing() ) {
	  EventFiles[k].Key.save( *EventFiles[k].Stream, et - SessionTime, 0 );
	  if ( EventFiles[k].SaveSize )
	    EventFiles[k].Key.save( *EventFiles[k].Stream,
				    EventFiles[k].Events->sizeScale() *
				    EventFiles[k].Events->eventSize( EventFiles[k].Offset ) );
	  if ( EventFiles[k].SaveWidth )
	    EventFiles[k].Key.save( *EventFiles[k].Stream,
				    EventFiles[k].Events->widthScale() *
				    EventFiles[k].Events->eventWidth( EventFiles[k].Offset ) );
	  *EventFiles[k].Stream << '\n';
	  EventFiles[k].Lines++;
	}
	EventFiles[k].Offset++;
      }
    }

  }

}


void SaveFiles::save( const OutData &signal )
{
  //  cerr << "SaveFiles::save( OutData &signal )\n";

  if ( signal.failed() )
    return;

  if ( StimulusData ) {
    RW->printlog( "! warning: SaveFiles::save( OutData & ) -> already stimulus data there" );
    Stimuli.clear();
  }

  // store stimulus:
  StimulusData = true;
  Stimuli.push_back( Stimulus( signal ) );

  // reset stimulus offset:
  PrevSignalTime = SignalTime;
  SignalTime = -1.0;
}


void SaveFiles::save( const OutList &signal )
{
  //  cerr << "SaveFiles::save( OutList &signal )\n";

  if ( signal.empty() || signal.failed() )
    return;

  if ( StimulusData ) {
    RW->printlog( "! warning: SaveFiles::save( OutList& ) -> already stimulus data there" );
    Stimuli.clear();
  }

  // store stimulus:
  StimulusData = true;
  for ( int k=0; k<signal.size(); k++ )
    Stimuli.push_back( signal[k] );

  // reset stimulus offset:
  PrevSignalTime = SignalTime;
  SignalTime = -1.0;
}


void SaveFiles::saveStimulus( void )
{
  //      cerr << "saveStimulus \n";
  
  if ( ! StimulusData )
    return;

  // no stimulus yet:
  if ( SignalTime < 0.0 )
    return;
    
  // stimulus indices file:
  if ( SF != 0 && saving() && writing() ) {
    StimulusKey.setSaveColumn( -1 );
    for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
      if ( TraceFiles[k].Stream != 0 )
	StimulusKey.save( *SF, TraceFiles[k].SignalOffset );
    }
    for ( unsigned int k=0; k<EventFiles.size(); k++ ) {
      if ( EventFiles[k].Stream != 0 ) {
	StimulusKey.save( *SF, EventFiles[k].SignalEvent );
	if ( EventFiles[k].SaveMeanRate )
	  StimulusKey.save( *SF, EventFiles[k].Events->meanRate() );  // XXX adaptive Zeit!
	if ( EventFiles[k].SaveMeanSize )
	  StimulusKey.save( *SF, EventFiles[k].Events->sizeScale() *
			    EventFiles[k].Events->meanSize() );
	if ( EventFiles[k].SaveMeanWidth )
	  StimulusKey.save( *SF, EventFiles[k].Events->widthScale() *
			    EventFiles[k].Events->meanWidth() );
	if ( EventFiles[k].SaveMeanQuality )
	  StimulusKey.save( *SF, 100.0*EventFiles[k].Events->meanQuality() );
      }
    }

    lock();
    if ( !Options::empty() ) {
      for( int k=0; k<Options::size(); k++ )
	StimulusKey.save( *SF, (*this)[k].number() );
    }
    unlock();
    StimulusKey.save( *SF, TraceFiles[0].Trace->signalTime() - SessionTime );
    // stimulus:
    StimulusKey.save( *SF, 1000.0*Stimuli[0].Delay );
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
	const Attenuate *att = RW->AQ->outTraceAttenuate( k );
	if ( Stimuli[j].Device == RW->AQ->outTrace( k ).device() &&
	     Stimuli[j].Channel == RW->AQ->outTrace( k ).channel() ) {
	  StimulusKey.save( *SF, 0.001*Stimuli[j].SampleRate );
	  StimulusKey.save( *SF, 1000.0*Stimuli[j].Length );
	  if ( att != 0 ) {
	    StimulusKey.save( *SF, Stimuli[j].Intensity );
	    if ( ! att->frequencyName().empty() )
	      StimulusKey.save( *SF, Stimuli[j].CarrierFreq );
	  }
	  StimulusKey.save( *SF, Stimuli[j].Ident );
	}
	else {
	  StimulusKey.save( *SF, "" );
	  StimulusKey.save( *SF, "" );
	  if ( att != 0 ) {
	    StimulusKey.save( *SF, "" );
	    if ( ! att->frequencyName().empty() )
	      StimulusKey.save( *SF, "" );
	  }
	  StimulusKey.save( *SF, "" );
	}
      }
    }
    *SF << endl;
  }
    
  // xml metadata file:
  if ( XF != 0 && saving() && writing() ) {
    *XF << "    <section name=\"Stimulus\">\n";
    lock();
    if ( !Options::empty() ) {
      int col = StimulusKey.column( "data>" + (*this)[0].ident() );
      *XF << "      <section name=\"Data\">\n";
      for( int k=0; k<Options::size(); k++ )
	StimulusKey[col++].setNumber( (*this)[k].number() ).saveXML( *XF, 5, "Stimulus.Data." );
      *XF << "      </section>\n";
    }
    unlock();
    int col = StimulusKey.column( "stimulus>timing>time" );
    StimulusKey[col++].setNumber( TraceFiles[0].Trace->signalTime() - SessionTime ).saveXML( *XF, 3, "Stimulus." );
    // Stimulus:
    StimulusKey[col++].setNumber( 1000.0*Stimuli[0].Delay ).saveXML( *XF, 3, "Stimulus." );
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
	const Attenuate *att = RW->AQ->outTraceAttenuate( k );
	if ( Stimuli[j].Device == RW->AQ->outTrace( k ).device() &&
	     Stimuli[j].Channel == RW->AQ->outTrace( k ).channel() ) {
	  Parameter p( "identifier", "identifier", RW->AQ->outTraceName( k ) );
	  p.saveXML( *XF, 3, "Stimulus." );
	  StimulusKey[col++].setNumber( 0.001*Stimuli[j].SampleRate ).saveXML( *XF, 3, "Stimulus." );
	  StimulusKey[col++].setNumber( 1000.0*Stimuli[j].Length );
	  if ( att != 0 ) {
	    StimulusKey[col++].setNumber( Stimuli[j].Intensity ).saveXML( *XF, 3, "Stimulus." );
	    if ( ! att->frequencyName().empty() )
	      StimulusKey[col++].setNumber( Stimuli[j].CarrierFreq ).saveXML( *XF, 3, "Stimulus." );
	  }
	  StimulusKey[col++].setText( Stimuli[j].Ident ).saveXML( *XF, 3, "Stimulus." );
	}
	else {
	  col += 3;
	  if ( att != 0 ) {
	    col++;
	    if ( ! att->frequencyName().empty() )
	      col++;
	  }
	}
      }
    }
    *XF << "    </section>\n";
  }
    
  StimulusData = false;
  Stimuli.clear();
}


void SaveFiles::save( const RePro &rp )
{
  //  cerr << "SaveFiles::save( const RePro &rp ) \n";

  if ( ReProData )
    RW->printlog( "! warning: SaveFiles::save( RePro & ) -> already RePro data there." );
  ReProData = true;
  ReProInfo.setText( "project", rp.projectOptions().text( "project" ) );
  ReProInfo.setText( "experiment", rp.projectOptions().text( "experiment" ) );
  ReProInfo.setText( "repro", rp.name() );
  ReProInfo.setText( "author", rp.author() );
  ReProInfo.setText( "version", rp.version() );
  ReProInfo.setText( "date", rp.date() );
  ReProSettings = rp;
}


void SaveFiles::saveRePro( void )
{
  //  cerr << "save RePro\n";

  if ( ReProData ) {

    ReProSettings.setFlags( 0 );
    ReProSettings.setTypeFlags( 1, -Parameter::Blank );
    
    // stimulus indices file:
    if ( SF != 0 && saving() && writing() ) {
      *SF << '\n';
      ReProInfo.save( *SF, "# ", -1, 0, false, true );
      if ( ! ReProSettings.empty() ) {
	ReProSettings.save( *SF, "# ", -1, 1, false, true );
      }

      // save StimulusKey:
      *SF << '\n';
      StimulusKey.saveKey( *SF );
    }
    
    // xml metadata file:
    if ( XF != 0 && saving() && writing() ) {
      if ( DatasetOpen ) {
	for ( unsigned int k=0; k<ReProFiles.size(); k++ ) {
	  Parameter p( "file", "file", ReProFiles[k] );
	  p.saveXML( *XF, 2, "Dataset." );
	}
	ReProFiles.clear();
	*XF << "  </section>\n";
      }
      *XF << "  <section name=\"Dataset\">\n";
      Parameter p( "name", "name", ReProInfo.text( "experiment" ) + "-" +
		   ReProInfo.text( "repro" ) + "-" + Str( path() ).preventedSlash().name() );
      p.saveXML( *XF, 2, "Dataset." );
      ReProInfo.saveXML( *XF, 0, 2, "Dataset." );
      if ( ! ReProSettings.empty() ) {
	*XF << "    <section name=\"Settings\">\n";
	ReProSettings.saveXML( *XF, 1, 3, "" );
	*XF << "    </section>\n";
      }
      DatasetOpen = true;
    }

    ReProData = false;
    ReProSettings.clear();
  }
}


bool SaveFiles::signalPending( void ) const
{
  return StimulusData;
}


void SaveFiles::clearSignal( void )
{
  StimulusData = false;
  Stimuli.clear();
}


void SaveFiles::addRemoveFile( const string &filename )
{
  RemoveFiles.push_back( filename );
}


void SaveFiles::clearRemoveFiles( void )
{
  RemoveFiles.clear();
}


void SaveFiles::removeFiles( void )
{
  for ( vector<string>::iterator h = RemoveFiles.begin();
	h != RemoveFiles.end();
	++h ) {
    remove( h->c_str() );
  }
  clearRemoveFiles();
}


ofstream *SaveFiles::openFile( const string &filename, int type )
{
  Options &opt = RW->MTDT.section( "Recording" );
  opt.insertText( "File", "Date", filename, MetaDataRecordingSection::standardFlag() );
  string fs = path() + filename;
  addRemoveFile( fs );
  ofstream *f = new ofstream( fs.c_str(), ofstream::openmode( type ) );
  if ( ! f->good() ) {
    f = 0;
    RW->printlog( "SaveFiles::openFile: can't open file '" + fs + "'" );
  }
  return f;
}


void SaveFiles::createTraceFiles( const InList &traces )
{
  SessionTime = traces[0].currentTime();

  string format = "%d";
  if ( traces.size() > 9 )
    format = "%02d";
  else if ( traces.size() > 99 )
    format = "%03d";

  TraceFiles.resize( traces.size() );

  for ( int k=0; k<traces.size(); k++ ) {

    // init trace variables:
    TraceFiles[k].Trace = &traces[k];
    TraceFiles[k].Index = traces[k].currentIndex();
    TraceFiles[k].Offset = 0;
    TraceFiles[k].SignalOffset = -1;

    // create file:
    if ( traces[k].mode() & SaveTrace ) {
      Str fn = traces[k].ident();
      TraceFiles[k].FileName = "trace-" + Str( k+1, format ) + ".raw";
      // TraceFiles[k].FileName = "trace-" + Str( k+1, format ) + ".au";
      TraceFiles[k].Stream = openFile( TraceFiles[k].FileName, ios::out | ios::binary );
      if ( ! TraceFiles[k].Stream->good() ) {
	TraceFiles[k].FileName = "";
	TraceFiles[k].Stream->close();
	TraceFiles[k].Stream = 0;
      }
      /*
      else {
	// write .au header:
	union {
	  char buffer[4];
	  unsigned long data;
	} d;
	d.data = 0x2e736e64;
	for ( int j=3; j>=0; j-- )
	  TraceFiles[k].Stream->write( &d.buffer[j], 1 );
	// data offset:
	d.data = 24;
	for ( int j=3; j>=0; j-- )
	  TraceFiles[k].Stream->write( &d.buffer[j], 1 );
	// data size:
	d.data = 0xffffffff;  // unknown
	for ( int j=3; j>=0; j-- )
	  TraceFiles[k].Stream->write( &d.buffer[j], 1 );
	// encoding:
	d.data = 6;  // float
	for ( int j=3; j>=0; j-- )
	  TraceFiles[k].Stream->write( &d.buffer[j], 1 );
	// sampling rate:
	d.data = (long)::rint( traces[k].sampleRate() );
	for ( int j=3; j>=0; j-- )
	  TraceFiles[k].Stream->write( &d.buffer[j], 1 );
	// channels:
	d.data = 1;
	for ( int j=3; j>=0; j-- )
	  TraceFiles[k].Stream->write( &d.buffer[j], 1 );
      }
      */
    }
    else {
      TraceFiles[k].FileName = "";
      TraceFiles[k].Stream = 0;
    }
  }

}


void SaveFiles::createEventFiles( const EventList &events )
{
  EventFiles.resize( events.size() );

  for ( int k=0; k<events.size(); k++ ) {

    // init event variables:
    EventFiles[k].Events = &events[k];
    EventFiles[k].Offset = events[k].size();
    EventFiles[k].Lines = 0;
    EventFiles[k].SignalEvent = 0;

    // create file:
    if ( events[k].mode() & SaveTrace ) {
      Str fn = events[k].ident();
      EventFiles[k].FileName = fn.lower() + "-events.dat";
      EventFiles[k].Stream = openFile( EventFiles[k].FileName, ios::out );
      if ( EventFiles[k].Stream->good() ) {
	// save header:
	*EventFiles[k].Stream << "# events: " << events[k].ident() << '\n';
	*EventFiles[k].Stream << '\n';
	// init key:
	EventFiles[k].Key.clear();
	EventFiles[k].Key.addNumber( "t", "sec", "%0.5f" );
	EventFiles[k].SaveSize = ( events[k].sizeBuffer() && (events[k].mode() & SaveSize) );
	if ( EventFiles[k].SaveSize )
	  EventFiles[k].Key.addNumber( events[k].sizeName(), events[k].sizeUnit(),
				       events[k].sizeFormat() );
	EventFiles[k].SaveWidth = ( events[k].widthBuffer() && (events[k].mode() & SaveWidth) );
	if ( EventFiles[k].SaveWidth )
	  EventFiles[k].Key.addNumber( events[k].widthName(), events[k].widthUnit(),
				       events[k].widthFormat() );
	// save key:
	EventFiles[k].Key.saveKey( *EventFiles[k].Stream );
      }
      else {
	EventFiles[k].FileName = "";
	EventFiles[k].Stream->close();
	EventFiles[k].Stream = 0;
      }
    }
    else {
      EventFiles[k].FileName = "";
      EventFiles[k].Stream = 0;
    }
  }
}


void SaveFiles::createStimulusFile( const InList &traces,
				    const EventList &events )
{
  // init stimulus variables:
  StimulusData = false;
  Stimuli.clear();
  SignalTime = -1.0;
  PrevSignalTime = -1.0;

  // create file for stimuli:
  SF = openFile( "stimuli.dat", ios::out );

  if ( (*SF) ) {
    // save header:
    *SF << "# analog input traces:\n";
    for ( int k=0; k<traces.size(); k++ ) {
      if ( ! TraceFiles[k].FileName.empty() ) {
	*SF << "#      identifier" + Str( k+1 ) + ": " << traces[k].ident() << '\n';
	*SF << "#       data file" + Str( k+1 ) + ": " << TraceFiles[k].FileName << '\n';
	*SF << "# sample interval" + Str( k+1 ) + ": " << Str( 1000.0*traces[k].sampleInterval(), 0, 2, 'f' ) << "ms\n";
	*SF << "#            unit" + Str( k+1 ) + ": " << traces[k].unit() << '\n';
      }
    }
    *SF << "# event lists:\n";
    for ( unsigned int k=0; k<EventFiles.size(); k++ ) {
      if ( ! EventFiles[k].FileName.empty() )
	*SF << "#      event file" + Str( k+1 ) + ": " << EventFiles[k].FileName << '\n';
    }
    *SF << "# analog output traces:\n";
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      TraceSpec trace( RW->AQ->outTrace( k ) );
      *SF << "#   identifier" + Str( k+1 ) + ": " << trace.traceName() << '\n';
      *SF << "#       device" + Str( k+1 ) + ": " << trace.device() << '\n';
      *SF << "#      channel" + Str( k+1 ) + ": " << trace.channel() << '\n';
      *SF << "# signal delay" + Str( k+1 ) + ": " << 1000.0*trace.signalDelay() << "ms\n";
      *SF << "# maximum rate" + Str( k+1 ) + ": " << 0.001*trace.maxSampleRate() << "kHz\n";
    }
    *SF << '\n';

    // create key:
    StimulusKey.clear();
    StimulusKey.addLabel( "traces" );
    for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
      if ( TraceFiles[k].Stream != 0 ) {
	StimulusKey.addLabel( traces[k].ident() );
	StimulusKey.addNumber( "index", "float", "%10.0f" );
      }
    }
    StimulusKey.addLabel( "events" );
    for ( unsigned int k=0; k<EventFiles.size(); k++ )
      if ( EventFiles[k].Stream != 0 ) {
	StimulusKey.addLabel( events[k].ident() );
	StimulusKey.addNumber( "index", "line", "%10.0f" );
	EventFiles[k].SaveMeanRate = ( events[k].mode() & SaveMeanRate );
	if ( EventFiles[k].SaveMeanRate )
	  StimulusKey.addNumber( "freq", "Hz", "%6.1f" );
	EventFiles[k].SaveMeanSize = ( events[k].mode() & SaveMeanSize );
	if ( EventFiles[k].SaveMeanSize )
	  StimulusKey.addNumber( events[k].sizeName(), events[k].sizeUnit(), events[k].sizeFormat() );
	EventFiles[k].SaveMeanWidth = ( events[k].mode() & SaveMeanWidth );
	if ( EventFiles[k].SaveMeanWidth )
	  StimulusKey.addNumber( events[k].widthName(), events[k].widthUnit(), events[k].widthFormat() );
	EventFiles[k].SaveMeanQuality = ( events[k].mode() & SaveMeanQuality );
	if ( EventFiles[k].SaveMeanQuality )
	  StimulusKey.addNumber( "quality", "%", "%3.0f" );
      }
    lock();
    if ( !Options::empty() ) {
      StimulusKey.addLabel( "data" );
      StimulusKey.addLabel( "data" );
      const Options &data = *this;
      for( int k=0; k<data.size(); k++ )
	StimulusKey.addNumber( data[k].ident(), data[k].outUnit(), data[k].format() );
    }
    unlock();
    StimulusKey.addLabel( "stimulus" );
    StimulusKey.addLabel( "timing" );
    StimulusKey.addNumber( "time", "s", "%11.5f" );
    StimulusKey.addNumber( "delay", "ms", "%5.1f" );
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      StimulusKey.addLabel( RW->AQ->outTraceName( k ) );
      StimulusKey.addNumber( "rate", "kHz", "%8.3f" );
      StimulusKey.addNumber( "duration", "ms", "%8.0f" );
      const Attenuate *att = RW->AQ->outTraceAttenuate( k );
      if ( att != 0 ) {
	StimulusKey.addNumber( att->intensityName(), att->intensityUnit(),
			       att->intensityFormat() );
	if ( ! att->frequencyName().empty() )
	  StimulusKey.addNumber( att->frequencyName(), att->frequencyUnit(),
				 att->frequencyFormat() );
      }
      StimulusKey.addText( "signal", -30 );
    }
  }
}


void SaveFiles::createXMLFile( const InList &traces,
			       const EventList &events )
{
  // create xml file for all data:
  XF = openFile( "metadata.xml", ios::out );

  if ( (*XF) ) {
    *XF << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    *XF << "<odML>\n";

    *XF << "  <section name=\"HardwareSettings\">\n";
    for ( int k=0; k<RW->ADV->size(); k++ ) {
      const Device &dev = (*RW->ADV)[k];
      int dt = dev.deviceType();
      if ( dt == Device::AttenuateType )
	continue;
      string dts = dev.deviceTypeStr();
      if ( dt == Device::AnalogInputType )
	dts = "DataAcquisition";
      else if ( dt == Device::AnalogOutputType )
	dts = "DataAcquisition";
      else if ( dt == Device::DigitalIOType )
	dts = "DigitialIO";
      else if ( dt == Device::AttenuateType )
	dts = "Attenuation";
      Options opts( dev.info() );
      opts.erase( "type" );
      *XF << "    <section name=\"" << dts << "\">\n";
      opts.saveXML( *XF, 0, 3, dts+"." ); 
      *XF << "    </section>\n";
    }
    *XF << "  </section>\n";
  }
}


void SaveFiles::openFiles( const InList &traces, EventList &events )
{
  // nothing to be done, if files are already open:
  if ( FilesOpen )
    return;

  // close all open files:
  closeFiles();
  clearRemoveFiles();

  // reset variables:
  ToggleData = false;
  ToggleOn = true;
  Writing = true;

  ReProData = false;
  ReProSettings.clear();
  ReProFiles.clear();
  DatasetOpen = false;

  setPath( defaultPath() );

  // get current time:
  time_t currenttime = RW->SN->startSessionTime();
  // time changed?
  if ( difftime( currenttime, PathTime ) != 0.0  )
    PathNumber = 0;
  PathTime = currenttime;

  // generate unused name for new files/directory:
  Str pathname = "";
  PathNumber++;
  int az = ('z'-'a'+1);
  for ( ; PathNumber <= az*az; PathNumber++ ) {

    // create file name:
    pathname = PathTemplate;
    pathname.format( localtime( &PathTime ) );
    pathname.format( PathNumber, 'n', 'd' );
    int n = PathNumber-1;
    Str s = char( 'a' + n%az );
    n /= az;
    while ( n > 0 ) {
      s.insert( 0u, 1u, char( 'a' + n%az ) );
      n /= az;
    }
    pathname.format( s, 'a' );
    s.upper();
    pathname.format( s, 'A' );

    if ( pathname[pathname.size()-1] == '/' ) {
      // try to create new directory:
      char s[200];
      sprintf( s, "%s %s", "mkdir", pathname.c_str() );
      // success?
      if ( system( s ) == 0 )
	break;
    }
    else {
      // try to open files:
      string fs = path() + "stimuli.dat";
      ifstream f( fs.c_str() );
      // files do not exist?
      if ( ! f.good() )
	break;
    }
  }
  // running out of names?
  if ( PathNumber > az*az ) {
    RW->printlog( "! panic: SaveFiles::openFiles -> can't create data file!" );
    return;
  }
  // valid base name found:
  setPath( pathname );

  // open files:
  createTraceFiles( traces );
  createEventFiles( events );
  createStimulusFile( traces, events );
  createXMLFile( traces, events );
  FilesOpen = true;

  // add recording event:
  for ( int k=0; k<events.size(); k++ ) {
    if ( (events[k].mode() & RecordingEventMode) > 0 ) {
      events[k].push( traces[0].currentTime() );
      break;
    }
  }

  // message:
  RW->printlog( "save in " + path() );

  // update widget:
  FileLabel->setFont( HighlightFont );
  FileLabel->setPalette( HighlightPalette );
  FileLabel->setText( path().c_str() );
  SaveLabel->setSpike( true );
}


void SaveFiles::closeFiles( void )
{
  ToggleData = true;
  ToggleOn = false;

  for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
    if ( TraceFiles[k].Stream != 0 ) {
      TraceFiles[k].Stream->close();
      delete TraceFiles[k].Stream;
    }
  }
  TraceFiles.clear();

  for ( unsigned int k=0; k<EventFiles.size(); k++ ) {
    if ( EventFiles[k].Stream != 0 ) {
      EventFiles[k].Stream->close();
      delete EventFiles[k].Stream;
    }
  }
  EventFiles.clear();

  if ( SF != 0 )
    delete SF;
  SF = 0;
  if ( XF != 0 ) {
    if ( DatasetOpen ) {
      for ( unsigned int k=0; k<ReProFiles.size(); k++ ) {
	Parameter p( "file", "file", ReProFiles[k] );
	p.saveXML( *XF, 2, "Dataset." );
	}
      ReProFiles.clear();
      *XF << "  </section>\n";
      DatasetOpen = false;
    }
    RW->MTDT.saveXML( *XF, 1 );
    *XF << "</odML>\n";
    delete XF;
  }
  XF = 0;

  if ( FilesOpen ) {
    RW->MTDT.section( "Recording" ).erase( "File" );
    FilesOpen = false;
  }

  SaveLabel->setSpike( false );
}


void SaveFiles::deleteFiles( void )
{
  closeFiles();

  // remove all files:
  removeFiles();

  if ( path() != defaultPath() && 
       path() != "" &&
       path()[path().size()-1] == '/' ) {
    // remove the whole directory:
    string s = "rm -f -r " + path();
    system( s.c_str() );
  }

  // message:
  RW->printlog( "discarded " + path() );
  FileLabel->setPalette( NormalPalette );
  FileLabel->setFont( NormalFont );
  FileLabel->setText( "deleted" );

  // back to default path:
  setPath( defaultPath() );
  PathNumber--;
}


void SaveFiles::completeFiles( void )
{
  closeFiles();

  // no files need to be deleted:
  clearRemoveFiles();

  // message:
  RW->printlog( "saved as " + path() );
  FileLabel->setPalette( NormalPalette );

  // back to default path:
  setPath( defaultPath() );
}


SaveFiles::Stimulus::Stimulus( void )
  : Device( 0 ),
    Channel( 0 ),
    Delay( 0.0 ),
    SampleRate( 0.0 ),
    Length( 0.0 ),
    Intensity( 0.0 ),
    CarrierFreq( 0.0 ),
    Ident( "" )
{
}



SaveFiles::Stimulus::Stimulus( const Stimulus &signal )
  : Device( signal.Device ),
    Channel( signal.Channel ),
    Delay( signal.Delay ),
    SampleRate( signal.SampleRate ),
    Length( signal.Length ),
    Intensity( signal.Intensity ),
    CarrierFreq( signal.CarrierFreq ),
    Ident( signal.Ident )
{
}


SaveFiles::Stimulus::Stimulus( const OutData &signal )
{
  Device = signal.device();
  Channel = signal.channel();
  Delay = signal.delay();
  SampleRate = signal.sampleRate();
  Length = signal.length();
  Intensity = signal.intensity();
  CarrierFreq = signal.carrierFreq();
  Ident = signal.ident();
}



}; /* namespace relacs */

#include "moc_savefiles.cc"

