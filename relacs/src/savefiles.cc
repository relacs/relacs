/*
  savefiles.cc
  Write data to files

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

  clearRemoveFiles();

  ToggleOn = false;
  ToggleData = false;

  ReProInfo.clear();
  ReProInfo.addText( "project" );
  ReProInfo.addText( "experiment" );
  ReProInfo.addText( "repro" );
  ReProInfo.addText( "author" );
  ReProInfo.addText( "version" );
  ReProInfo.addText( "date" );
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


void SaveFiles::write( const InList &traces )
{
  //  cerr << "SaveFiles::write( InList &traces )\n";

  // update write status:
  writeToggle();

  // indicate the new RePro:
  writeRePro();

  if ( !saving() || !writing() )
    return;

  // write trace data:
  if ( (int)TraceFiles.size() != traces.size() )
    cerr << "! error in SaveFiles::write( InList ) -> TraceFiles.size() != traces.size() !\n";
  for ( int k=0; k<(int)TraceFiles.size() && k<traces.size(); k++ ) {
    TraceFiles[k].Trace = &traces[k];
    if ( TraceFiles[k].Stream != 0 ) {
      TraceFiles[k].Offset += traces[k].saveBinary( *TraceFiles[k].Stream,
						    TraceFiles[k].Index );
      TraceFiles[k].Index = traces[k].currentIndex();
      TraceFiles[k].SignalOffset = TraceFiles[k].Offset - TraceFiles[k].Index
	+ traces[k].signalIndex();
    }
  }

}


void SaveFiles::write( const EventList &events )
{
  //  cerr << "SaveFiles::write( EventList &events )\n";

  // update write status:
  writeToggle();

  // indicate the new RePro:
  writeRePro();

  if ( !saving() || !writing() )
    return;

  // write event data:
  for ( int k=0; k<(int)EventFiles.size() && k<events.size(); k++ ) {
    EventFiles[k].Events = &events[k];
    if ( EventFiles[k].Stream != 0 ) {

      while ( EventFiles[k].Offset < EventFiles[k].Events->size() ) {
	double et = (*EventFiles[k].Events)[EventFiles[k].Offset];
	if ( EventFiles[0].Events->size() > 0 &&
	     et >= EventFiles[0].Events->back() &&
	     ( EventFiles[k].Offset == 0 || 
	       (*EventFiles[k].Events)[EventFiles[k].Offset-1] < EventFiles[0].Events->back() ) ) {
	  EventFiles[k].SignalEvent = EventFiles[k].Lines;
	  *EventFiles[k].Stream << '\n';
	}
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
	EventFiles[k].Offset++;
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

  if ( StimulusData ) {
    RW->printlog( "! warning: SaveFiles::write( OutData & ) -> already stimulus data there" );
    Stimuli.clear();
  }

  StimulusData = true;
  Stimuli.push_back( Stimulus( signal ) );
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

  if ( StimulusData ) {
    RW->printlog( "! warning: SaveFiles::write( OutList& ) -> already stimulus data there" );
    Stimuli.clear();
  }

  StimulusData = true;
  for ( int k=0; k<signal.size(); k++ )
    Stimuli.push_back( signal[k] );
}


void SaveFiles::writeStimulus( void )
{
  if ( StimulusData ) {
    
    //      cerr << "writeStimulus \n";
    
    // stimulus indices file:
    if ( SF != 0 && saving() && writing() ) {
      StimulusKey.setSaveColumn( -1 );
      for ( unsigned int k=0; k<TraceFiles.size(); k++ )
	if ( TraceFiles[k].Stream != 0 ) {
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
      *XF << "    <section name=\"stimulus\">\n";
      lock();
      if ( !Options::empty() ) {
	int col = StimulusKey.column( "data>" + (*this)[0].ident() );
	*XF << "      <section name=\"data\">\n";
	for( int k=0; k<Options::size(); k++ )
	  StimulusKey[col++].setNumber( (*this)[k].number() ).saveXML( *XF, 5 );
	*XF << "      </section>\n";
      }
      unlock();
      int col = StimulusKey.column( "stimulus>timing>time" );
      StimulusKey[col++].setNumber( TraceFiles[0].Trace->signalTime() - SessionTime ).saveXML( *XF, 3 );
      // Stimulus:
      StimulusKey[col++].setNumber( 1000.0*Stimuli[0].Delay ).saveXML( *XF, 3 );
      for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
	for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
	  const Attenuate *att = RW->AQ->outTraceAttenuate( k );
	  if ( Stimuli[j].Device == RW->AQ->outTrace( k ).device() &&
	       Stimuli[j].Channel == RW->AQ->outTrace( k ).channel() ) {
	    Parameter p( "identifier", "identifier", RW->AQ->outTraceName( k ) );
	    p.saveXML( *XF, 3 );
	    StimulusKey[col++].setNumber( 0.001*Stimuli[j].SampleRate ).saveXML( *XF, 3 );
	    StimulusKey[col++].setNumber( 1000.0*Stimuli[j].Length );
	    if ( att != 0 ) {
	      StimulusKey[col++].setNumber( Stimuli[j].Intensity ).saveXML( *XF, 3 );
	      if ( ! att->frequencyName().empty() )
		StimulusKey[col++].setNumber( Stimuli[j].CarrierFreq ).saveXML( *XF, 3 );
	    }
	    StimulusKey[col++].setText( Stimuli[j].Ident ).saveXML( *XF, 3 );
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
}


void SaveFiles::write( const RePro &rp )
{
  //  cerr << "SaveFiles::write( const RePro &rp ) \n";

  if ( ReProData )
    RW->printlog( "! warning: SaveFiles::write( RePro & ) -> already RePro data there." );
  ReProData = true;
  ReProInfo.setText( "project", rp.projectOptions().text( "project" ) );
  ReProInfo.setText( "experiment", rp.projectOptions().text( "experiment" ) );
  ReProInfo.setText( "repro", rp.name() );
  ReProInfo.setText( "author", rp.author() );
  ReProInfo.setText( "version", rp.version() );
  ReProInfo.setText( "date", rp.date() );
  ReProSettings = rp;

  // write last stimulus here!
  // it is the probably unfinished one of the previous RePro.
  writeStimulus();
}


void SaveFiles::writeRePro( void )
{
  //  cerr << "write RePro\n";

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

      // write StimulusKey:
      *SF << '\n';
      StimulusKey.saveKey( *SF );
    }
    
    // xml metadata file:
    if ( XF != 0 && saving() && writing() ) {
      if ( DatasetOpen ) {
	for ( unsigned int k=0; k<ReProFiles.size(); k++ ) {
	  Parameter p( "file", "file", ReProFiles[k] );
	  p.saveXML( *XF, 2 );
	}
	ReProFiles.clear();
	*XF << "  </section>\n";
      }
      *XF << "  <section name=\"dataset\">\n";
      Parameter p( "name", "name", ReProInfo.text( "experiment" ) + "-" +
		   ReProInfo.text( "repro" ) + "-" + Str( path() ).preventedSlash().name() );
      p.saveXML( *XF, 2 );
      ReProInfo.saveXML( *XF, 0, 2 );
      if ( ! ReProSettings.empty() ) {
	*XF << "    <section name=\"settings\">\n";
	ReProSettings.saveXML( *XF, 1, 3 );
	*XF << "    </section>\n";
      }
      DatasetOpen = true;
    }

    ReProData = false;
    ReProSettings.clear();
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
    TraceFiles[k].SignalOffset = 0;

    // create file:
    if ( traces[k].mode() & SaveTrace ) {
      Str fn = traces[k].ident();
      TraceFiles[k].FileName = "trace-" + Str( k+1, format ) + ".f1";
      TraceFiles[k].Stream = openFile( TraceFiles[k].FileName, ios::out | ios::binary );
      if ( ! TraceFiles[k].Stream->good() ) {
	TraceFiles[k].FileName = "";
	TraceFiles[k].Stream->close();
	TraceFiles[k].Stream = 0;
      }
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
	// write header:
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
	// write key:
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

  // create file for stimuli:
  SF = openFile( "stimulus-indices.dat", ios::out );

  if ( (*SF) ) {
    // write header:
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
	StimulusKey.addNumber( "index", "word", "%10.0f" );
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
    *XF << "<ephysmetadata\n";
    *XF << "  xmlns:dc=\"http://purl.org/metadata/dublin_core#\"\n";
    *XF << "  xmlns:md=\"http://www.g-node.org/md-syntax-ns#\">\n";

    *XF << "  <section name=\"hardware\">\n";
    for ( int k=0; k<RW->ADV->size(); k++ ) {
      const Device &dev = (*RW->ADV)[k];
      Options opts;
      opts.load( dev.info() );
      int dt = opts.integer( "type" );
      if ( dt == 4 ) // Attenuate
	continue;
      opts.erase( "type" );
      string dts = Device::deviceTypeStr( dt );
      if ( dts.empty() )
	dts = "Unknown Device";
      *XF << "    <section name=\"" << dts << "\">\n";
      opts.saveXML( *XF, 0, 3 ); 
      *XF << "    </section>\n";
    }
    *XF << "  </section>\n";
  }
}


void SaveFiles::openFiles( const InList &traces, const EventList &events )
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
  Writing = false;

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
      string fs = path() + "stimulus-indices.dat";
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
  writeStimulus();

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
	p.saveXML( *XF, 2 );
	}
      ReProFiles.clear();
      *XF << "  </section>\n";
      DatasetOpen = false;
    }
    RW->MTDT.saveXML( *XF, 1 );
    *XF << "</ephysmetadata>\n";
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

