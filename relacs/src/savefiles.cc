/*
  savefiles.cc
  Save data to files

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

#include <cstdio>
#include <QDir>
#include <QDateTime>
#include <QPainter>
#include <QToolTip>
#include <QMutexLocker>
#include <relacs/acquire.h>
#include <relacs/attenuate.h>
#include <relacs/relacsdevices.h>
#include <relacs/relacswidget.h>
#include <relacs/session.h>
#include <relacs/savefiles.h>

namespace relacs {


class StimuliEvent : public QEvent
{

public:

  StimuliEvent( const deque< OutDataInfo > &stimuli )
    : QEvent( Type( User+1 ) )
  {
    if ( stimuli.size() == 1 )
      Description = stimuli[0].description();
    else {
      Description.setType( "stimulus" );
      for ( unsigned int k=0; k<stimuli.size(); k++ )
	Description.newSection( stimuli[k].description() );
    }
  }

  Options Description;
};


SaveFiles::SaveFiles( RELACSWidget *rw, int height,
		      QWidget *parent )
  : QWidget( parent ),
    Options( "Status", "", 0 , 0 ),
    RW( rw )
{
  Path = "";
  PrevPath = "";
  PathTemplate = "%04Y-%02m-%02d-%a2a";
  DefaultPath = "";

  PathNumber = 0;
  PathTime = ::time( 0 );

  FilesOpen = false;
  Saving = false;
  Hold = false;

  SF = 0;
  SDF = 0;
  XF = 0;
  XSF = 0;
  TraceFiles.clear();
  EventFiles.clear();

  Stimuli.clear();
  StimuliRePro = "";
  StimuliReProCount.clear();
  ReProStimuli.clear();
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
  ReProInfo.addInteger( "run" );
  ReProSettings.clear();
  ReProFiles.clear();
  ReProData = false;
  DatasetOpen = false;

  setFixedHeight( height );

  StatusInfoLayout = new QHBoxLayout;
  StatusInfoLayout->setContentsMargins( 0, 0, 0, 0 );
  StatusInfoLayout->setSpacing( 4 );
  setLayout( StatusInfoLayout );

  FileLabel = new QLabel( "no files open" );
  // XXX  ensurePolished(); produces SIGSEGV
  NormalFont = FileLabel->font();
  HighlightFont = QFont( fontInfo().family(), fontInfo().pointSize()*4/3, QFont::Bold );
  FileLabel->setTextFormat( Qt::PlainText );
  FileLabel->setIndent( 2 );
  FileLabel->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  FileLabel->setToolTip( "The directory where files are currently stored" );
  setPathTemplate( PathTemplate );
  StatusInfoLayout->addWidget( FileLabel );

  SaveLabel = new SpikeTrace( 0.8, 8, 3 );
  SaveLabel->setFixedWidth( SaveLabel->minimumSizeHint().width() );
  SaveLabel->setToolTip( "An animation indicating that raw data are stored on disk" );
  StatusInfoLayout->addWidget( SaveLabel );

  setMinimumWidth( FileLabel->width() + 4 + SaveLabel->width() );

  NormalPalette = FileLabel->palette();
  HighlightPalette = FileLabel->palette();
  HighlightPalette.setColor( QPalette::WindowText, Qt::red );

  // load current directory:
  QDir dir;
  DI.loadDirectory( dir.currentPath().toStdString() );
}


SaveFiles::~SaveFiles()
{
  closeFiles();

  clearRemoveFiles();

  EventFiles.clear();

  Stimuli.clear();
  StimuliReProCount.clear();
  ReProStimuli.clear();
}


bool SaveFiles::saving( void ) const
{
  return FilesOpen && Saving;
}


bool SaveFiles::filesOpen( void ) const
{
  return FilesOpen;
}


void SaveFiles::holdOn( void )
{
  Hold = true;
}


void SaveFiles::holdOff( void )
{
  Hold = false;
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
  for ( deque<string>::const_iterator sp = ReProFiles.begin(); sp != ReProFiles.end(); ++sp ) {
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


void SaveFiles::save( bool on  )
{
  // this function is called from RELACSWidget::setSaving() and
  // right before starting a RePro in RELACSWidget::startRepRo()

  //  cerr << "save toggle: " << on << '\n';
  QMutexLocker locker( &SaveMutex );
  if ( ! FilesOpen ) {
    Saving = false;
    return;
  }

  ToggleData = true;
  ToggleOn = on;
}


void SaveFiles::saveToggle( const InList &traces, EventList &events )
{
  // only called by save( InList, EventList ).

  //  cerr << "saveToggle(): " << ToggleData << ", on=" << ToggleOn << '\n';

  if ( ToggleData && ! Hold ) {

    if ( ToggleOn && ! Saving ) {
      cerr << "UPDATE TOGGLE\n";
      // update offsets:
      for ( unsigned int k=0; k<TraceFiles.size(); k++ )
	TraceFiles[k].Index = traces[k].size();
      for ( unsigned int k=0; k<EventFiles.size(); k++ )
	EventFiles[k].Index = events[k].size();

      // add recording event:
      for ( int k=0; k<events.size(); k++ ) {
	if ( ( events[k].mode() & RecordingEventMode ) > 0 ) {
	  events[k].push( traces[0].pos( TraceFiles[0].Index ) );
	  break;
	}
      }
    }

    Saving = ToggleOn;
    SaveLabel->setPause( ! Saving );
    ToggleData = false;

  }

}


void SaveFiles::save( const InList &traces, EventList &events )
{
  QMutexLocker locker( &SaveMutex );

  // this function is called from RELACSWidget::processData()
  // and from the beginning of the write( OutData ) functions
  // in case of a pending signal
  // and from RELACSWidget::stopRePro().

  // update save status:
  saveToggle( traces, events );

  // check for new signal:
  if ( events[0].size() > 0 ) {
    double st = events[0].back();
    if ( saving() && ::fabs( TraceFiles[0].Trace->signalTime() - st ) >= TraceFiles[0].Trace->stepsize() )
      cerr << "SignalTime PROBLEM trace: " << Str( TraceFiles[0].Trace->signalTime(), 0, 5, 'f' ) << " stimulus: " << Str( st, 0, 5, 'f' ) << "\n";
    if ( st > PrevSignalTime )
      SignalTime = st;
  }

  saveRePro();
  saveTraces();
  double offs = 0.0;
  if ( saving() )
    offs = traces[0].interval( TraceFiles[0].Index - TraceFiles[0].Written );
  saveEvents( offs );
  saveStimulus();
}


void SaveFiles::saveTraces( void )
{
  //  cerr << "SaveFiles::saveTraces()\n";

  if ( ! saving() )
    return;

  for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
    if ( TraceFiles[k].Stream != 0 ) {
      int n = TraceFiles[k].Trace->saveBinary( *TraceFiles[k].Stream,
					       TraceFiles[k].Index );
      if ( n > 0 ) {
	TraceFiles[k].Written += n;
	TraceFiles[k].Index += n;
      }
      // there is a new stimulus:
      if ( !Stimuli.empty() && SignalTime >= 0.0 &&
	   TraceFiles[k].Trace->signalIndex() >= 0 )
	TraceFiles[k].SignalOffset = TraceFiles[k].Trace->signalIndex() -
	  TraceFiles[k].Index + TraceFiles[k].Written;
    }
  }

}


void SaveFiles::saveEvents( double offs )
{
  //  cerr << "SaveFiles::save( EventList &events )\n";

  if ( ! saving() )
    return;

  // wait for availability of signal start time:
  if ( !Stimuli.empty() && SignalTime < 0.0 )
    return;

  // save event data:
  double st = EventFiles[0].Events->size() > 0 ? EventFiles[0].Events->back() : EventFiles[0].Events->rangeBack();
  for ( unsigned int k=0; k<EventFiles.size(); k++ ) {

    if ( EventFiles[k].Stream != 0 ) {
      while ( EventFiles[k].Index < EventFiles[k].Events->size() ) {
	double et = (*EventFiles[k].Events)[EventFiles[k].Index];
	if ( et < st )
	  EventFiles[k].SignalEvent = EventFiles[k].Written;
	else if ( EventFiles[k].Index == 0 ||
		  (*EventFiles[k].Events)[EventFiles[k].Index-1] < st ) {
	  EventFiles[k].SignalEvent = EventFiles[k].Written;
	  *EventFiles[k].Stream << '\n';
	}
	EventFiles[k].Key.save( *EventFiles[k].Stream, et - offs, 0 );
	if ( EventFiles[k].SaveSize )
	  EventFiles[k].Key.save( *EventFiles[k].Stream,
				  EventFiles[k].Events->sizeScale() *
				  EventFiles[k].Events->eventSize( EventFiles[k].Index ) );
	if ( EventFiles[k].SaveWidth )
	  EventFiles[k].Key.save( *EventFiles[k].Stream,
				  EventFiles[k].Events->widthScale() *
				  EventFiles[k].Events->eventWidth( EventFiles[k].Index ) );
	*EventFiles[k].Stream << '\n';
	EventFiles[k].Written++;
	EventFiles[k].Index++;
      }
    }

  }

}


void SaveFiles::save( const OutData &signal )
{
  // this function is called from RELACSWidget::write()
  // after a successfull call of Acquire::write()

  //  cerr << "SaveFiles::save( OutData &signal )\n";

  if ( signal.failed() )
    return;

  QMutexLocker locker( &SaveMutex );

  if ( !Stimuli.empty() ) {
    RW->printlog( "! warning: SaveFiles::save( OutData & ) -> already stimulus data there" );
    Stimuli.clear();
  }

  // StimulusDataLock is already locked from within the RePro!
  StimulusOptions = *this;

  // store stimulus:
  Stimuli.push_back( signal );

  // reset stimulus offset:
  PrevSignalTime = SignalTime;
  SignalTime = -1.0;
}


void SaveFiles::save( const OutList &signal )
{
  //  cerr << "SaveFiles::save( OutList &signal )\n";

  if ( signal.empty() || signal.failed() )
    return;

  QMutexLocker locker( &SaveMutex );

  if ( !Stimuli.empty() ) {
    RW->printlog( "! warning: SaveFiles::save( OutList& ) -> already stimulus data there" );
    Stimuli.clear();
  }

  // StimulusDataLock is already locked from within the RePro!
  StimulusOptions = *this;

  // store stimulus:
  for ( int k=0; k<signal.size(); k++ )
    Stimuli.push_back( signal[k] );

  // reset stimulus offset:
  PrevSignalTime = SignalTime;
  SignalTime = -1.0;
}


void SaveFiles::saveStimulus( void )
{
  //      cerr << "saveStimulus \n";

  if ( Stimuli.empty() )
    return;

  // no stimulus yet:
  if ( SignalTime < 0.0 )
    return;

  // add to data index:
  QCoreApplication::postEvent( this, new StimuliEvent( Stimuli ) );

  // extract intensity from stimulus description:
  // XXX there should be a flag indicating, which quantities to extract!
  deque< Options > stimuliref( Stimuli.size() );
  for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
    Options::iterator pi = Stimuli[j].description().find( "Intensity" );
    if ( pi != Stimuli[j].description().end() ) {
      stimuliref[j].add( *pi );
      Stimuli[j].description().erase( pi );
    }
    // XXX once OutData does not have idents any more, the following lines can be erased:
    if ( ! Stimuli[j].ident().empty() ) {
      stimuliref[j].addText( "Description", Stimuli[j].ident() );
    }
  }

  // generate names for all stimulus traces:
  // i.e reproname-type1-type2 ...
  deque< string > stimulinames( Stimuli.size() );
  for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
    string sn = StimuliRePro;  // (name of the RePro from which the stimulus was written)
    // add type of stimulus description:
    Str tn = Stimuli[j].description().type();
    if ( tn.empty() )
      tn = Stimuli[j].description().name();
    tn.eraseFirst( "stimulus" );
    tn.preventFirst( '/' );
    if ( ! tn.empty() )
      sn += '-' + tn;
    for ( Options::const_section_iterator si=Stimuli[j].description().sectionsBegin();
	  si != Stimuli[j].description().sectionsEnd();
	  ++si ) {
      tn = (*si)->type();
      if ( tn.empty() )
	tn = (*si)->name();
      tn.eraseFirst( "stimulus" );
      tn.preventFirst( '/' );
      if ( ! tn.empty() )
	sn += '-' + tn;
    }
    stimulinames[j] = sn;
  }

  // track stimulus traces:
  deque< bool > newstimuli( Stimuli.size(), false );
  for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
    // get all stimulus descriptions for the stimulusname reproname-type1-type2:
    map < Options, string > &rsd = ReProStimuli[ stimulinames[j] ];
    // retrieve the corresponding unique identifier:
    string &rsds = rsd[ Stimuli[j].description() ];
    if ( rsds.empty() ) {
      // this stimulus description is new:
      newstimuli[j] = true;
      if ( ! Stimuli[j].description().name().empty() )
	stimulinames[j] = Stimuli[j].description().name();
      // append the number of stimuli in this category to make the name unique:
      stimulinames[j] += '-' + Str( rsd.size() );
      rsds = stimulinames[j];
    }
    else {
      // this stimulus already exists, get its unique identifier:
      stimulinames[j] = rsds;
    }
    // set name, type, and some additional information for each stimulus trace:
    Stimuli[j].description().setName( stimulinames[j] );
    stimuliref[j].setName( stimulinames[j] );
    if ( Stimuli[j].description().type().empty() ) {
      Stimuli[j].description().setType( "stimulus" );
      stimuliref[j].setType( "stimulus" );
    }
    Stimuli[j].description().insertNumber( "SamplingRate", "", 0.001*Stimuli[j].sampleRate(), "kHz" );
    Stimuli[j].description().insertText( "Modality", "", RW->AQ->outTrace( Stimuli[j].trace() ).modality() );
    stimuliref[j].setInclude( "stimulus-metadata.xml", stimulinames[j] );
  }

  // track stimulus name:
  int &rc = StimuliReProCount[ StimuliRePro ];
  rc++;
  string stimulirepro = StimuliRePro + "-" + Str( rc );

  // stimulus indices file:
  if ( SF != 0 && saving() ) {

    // stimulus description:
    if ( SDF != 0 ) {
      for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
	if ( newstimuli[j] ) {
	  Stimuli[j].description().save( *SDF, "  " );
	  *SDF << '\n';
	}
      }
    }

    StimulusKey.resetSaveColumn();
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

    if ( !StimulusOptions.empty() ) {
      for( int k=0; k<StimulusOptions.size(); k++ ) {
	StimulusKey.save( *SF, StimulusOptions[k] );
      }
    }
    StimulusKey.save( *SF, TraceFiles[0].Trace->signalTime() - SessionTime );
    // stimulus:
    StimulusKey.save( *SF, 1000.0*Stimuli[0].delay() );
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      const Attenuate *att = RW->AQ->outTraceAttenuate( k );
      unsigned int j=0;
      for ( j=0; j<Stimuli.size(); j++ ) {
	if ( Stimuli[j].device() == RW->AQ->outTrace( k ).device() &&
	     Stimuli[j].channel() == RW->AQ->outTrace( k ).channel() ) {
	  Stimuli[j].setTrace( k ); // needed for XML
	  StimulusKey.save( *SF, 0.001*Stimuli[j].sampleRate() );
	  StimulusKey.save( *SF, 1000.0*Stimuli[j].length() );
	  if ( att != 0 ) {
	    StimulusKey.save( *SF, Stimuli[j].intensity() );
	    if ( ! att->frequencyName().empty() )
	      StimulusKey.save( *SF, Stimuli[j].carrierFreq() );
	  }
	  StimulusKey.save( *SF, stimulinames[j] );
	  break;
	}
      }
      if ( j >= Stimuli.size() ) {
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
    *SF << endl;
  }

  // xml metadata file:
  if ( XF != 0 && saving() ) {

    // stimulus description:
    if ( XSF != 0 ) {
      for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
	if ( newstimuli[j] )
	  Stimuli[j].description().saveXML( *XSF, 0, Options::FirstOnly, 0 );
      }
    }

    // stimulus reference:
    Options sopt( stimulirepro, "stimulus", 0, 0 );
    sopt.append( StimulusOptions );
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
	const Attenuate *att = RW->AQ->outTraceAttenuate( k );
	if ( Stimuli[j].device() == RW->AQ->outTrace( k ).device() &&
	     Stimuli[j].channel() == RW->AQ->outTrace( k ).channel() ) {
	  if ( att != 0 ) {
	    if ( ! att->frequencyName().empty() )
	      stimuliref[j].insertNumber( att->frequencyName(), "",
					  Stimuli[j].carrierFreq(),
					  att->frequencyUnit(),
					  att->frequencyFormat() );
	    stimuliref[j].insertNumber( att->intensityName(), "",
					Stimuli[j].intensity(),
					att->intensityUnit(),
					att->intensityFormat() );
	  }
	  stimuliref[j].insertText( "OutputChannel", "",
				    RW->AQ->outTraceName( Stimuli[j].trace() ) );
	  stimuliref[j].insertNumber( "Delay", "", 1000.0*Stimuli[j].delay(), "ms" );
	  stimuliref[j].insertNumber( "Time", "", TraceFiles[0].Trace->signalTime() - SessionTime, "s" );
	  sopt.newSection( &stimuliref[j] );
	  break;
	}
      }
    }
    sopt.saveXML( *XF, 0, Options::FirstOnly, 1 );
  }

  Stimuli.clear();
}


void SaveFiles::save( const RePro &rp )
{
  //  cerr << "SaveFiles::save( const RePro &rp ) \n";

  QMutexLocker locker( &SaveMutex );

  if ( ReProData )
    RW->printlog( "! warning: SaveFiles::save( RePro & ) -> already RePro data there." );
  ReProData = true;
  ReProInfo.setText( "project", rp.projectOptions().text( "project" ) );
  ReProInfo.setText( "experiment", rp.projectOptions().text( "experiment" ) );
  ReProInfo.setText( "repro", rp.name() );
  ReProInfo.setText( "author", rp.author() );
  ReProInfo.setText( "version", rp.version() );
  ReProInfo.setText( "date", rp.date() );
  ReProInfo.setInteger( "run", rp.allRuns() );
  ReProSettings = rp;
  StimuliRePro = rp.name();
  DI.addRepro( rp );
}


void SaveFiles::saveRePro( void )
{
  //  cerr << "save RePro\n";

  if ( ReProData ) {

    ReProSettings.setFlags( 0 );
    ReProSettings.setValueTypeFlags( 1, -Parameter::Section );

    // stimulus indices file:
    if ( SF != 0 && saving() ) {
      *SF << '\n';
      ReProInfo.save( *SF, "# ", 0, Options::FirstOnly );
      if ( ! ReProSettings.empty() ) {
	ReProSettings.save( *SF, "# ", 1, Options::FirstOnly );
      }

      // save StimulusKey:
      *SF << '\n';
      StimulusKey.saveKey( *SF );
    }

    // xml metadata file:
    if ( XF != 0 && saving() ) {
      if ( DatasetOpen ) {
	if ( ReProFiles.size() > 0 ) {
	  string files = ReProFiles[0];
	  for ( unsigned int k=1; k<ReProFiles.size(); k++ )
	    files += ", " + ReProFiles[k];
	  Parameter p( "File", "", files );
	  p.saveXML( *XF, 2, 4 );
	}
	ReProFiles.clear();
	*XF << "  </section>\n";
      }
      string dataset = Str( path() ).preventedSlash().name()
	+ "-" + ReProInfo.text( "repro" ) + "-" + Str( ReProInfo.integer( "run" ) );
      *XF << "  <section>\n";
      *XF << "    <type>dataset</type>\n";
      *XF << "    <name>dataset-" << dataset << "</name>\n";
      ReProInfo.saveXML( *XF, 0, Options::FirstOnly, 2 );
      if ( ! ReProSettings.empty() ) {
	*XF << "    <section>\n";
	*XF << "      <type>settings</type>\n";
	*XF << "      <name>dataset-settings-" << dataset << "</name>\n";
	ReProSettings.saveXML( *XF, 1, Options::FirstOnly, 3 );
	*XF << "    </section>\n";
      }
      StimuliReProCount[ StimuliRePro ] = 0;
      DatasetOpen = true;
    }

    ReProData = false;
    ReProSettings.clear();
  }
}


bool SaveFiles::signalPending( void ) const
{
  return !Stimuli.empty();
}


void SaveFiles::clearSignal( void )
{
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
  for ( deque<string>::iterator h = RemoveFiles.begin();
	h != RemoveFiles.end();
	++h ) {
    remove( h->c_str() );
  }
  clearRemoveFiles();
}


ofstream *SaveFiles::openFile( const string &filename, int type )
{
  string fs = path() + filename;
  ofstream *f = new ofstream( fs.c_str(), ofstream::openmode( type ) );
  if ( ! f->good() ) {
    f = 0;
    RW->printlog( "! error in SaveFiles::openFile: can't open file '" + fs + "'!" );
  }
  else {
    addRemoveFile( fs );
    // add to recording section:
    Options &opt = RW->MTDT.section( "Recording" );
    if ( opt.exist( "File" ) )
      opt.pushText( "File", filename );
    else {
      opt.insertText( "File", "Date", filename,
		      MetaData::standardFlag(), Parameter::ListAlways );
    }
  }
  return f;
}


void SaveFiles::createTraceFiles( const InList &traces )
{
  SessionTime = traces.currentTime();

  string format = "%d";
  if ( traces.size() > 9 )
    format = "%02d";
  else if ( traces.size() > 99 )
    format = "%03d";

  TraceFiles.resize( traces.size() );

  for ( int k=0; k<traces.size(); k++ ) {

    // init trace variables:
    TraceFiles[k].Trace = &traces[k];
    TraceFiles[k].Index = traces[k].size();
    TraceFiles[k].Written = 0;
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
    EventFiles[k].Index = events[k].size();
    EventFiles[k].Written = 0;
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
  Stimuli.clear();
  StimuliReProCount.clear();
  ReProStimuli.clear();
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
    *SF << "# stimulus descriptions file: stimlus-descriptions.dat\n";
    *SF << '\n';

    // create key:
    StimulusKey.clear();
    StimulusKey.newSection( "traces" );
    for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
      if ( TraceFiles[k].Stream != 0 ) {
	StimulusKey.newSubSection( traces[k].ident() );
	StimulusKey.addNumber( "index", "float", "%10.0f" );
      }
    }
    StimulusKey.newSection( "events" );
    for ( unsigned int k=0; k<EventFiles.size(); k++ )
      if ( EventFiles[k].Stream != 0 ) {
	StimulusKey.newSubSection( events[k].ident() );
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
      StimulusKey.newSection( "data" );
      const Options &data = *this;
      for( int k=0; k<data.size(); k++ )
	StimulusKey.addNumber( data[k].name(), data[k].outUnit(), data[k].format() );
    }
    unlock();
    StimulusKey.newSection( "stimulus" );
    StimulusKey.newSubSection( "timing" );
    StimulusKey.addNumber( "time", "s", "%11.5f" );
    StimulusKey.addNumber( "delay", "ms", "%5.1f" );
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      StimulusKey.newSubSection( RW->AQ->outTraceName( k ) );
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


  // create file for stimuli:
  SDF = openFile( "stimulus-descriptions.dat", ios::out );
}


void SaveFiles::createXMLFile( const InList &traces,
			       const EventList &events )
{
  // create xml file for all data:
  XF = openFile( "metadata.xml", ios::out );

  if ( (*XF) ) {
    string name = Str( path() ).preventedSlash().name();
    *XF << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    *XF << "<?xml-stylesheet type=\"text/xsl\" href=\"odml.xsl\"  xmlns:odml=\"http://www.g-node.org/odml\"?>\n";
    *XF << "<odML version=\"1\">\n";
    *XF << "  <repository>http://portal.g-node.org/odml/terminologies/v1.0/terminologies.xml</repository>\n";
    *XF << "  <section>\n";
    *XF << "    <type>hardware</type>\n";
    *XF << "    <name>hardware-" << name << "</name>\n";
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
      *XF << "    <section>\n";
      *XF << "      <type>" << dts << "</type>\n";
      *XF << "      <name>hardware-" << dts << "-" << name << "</name>\n";
      opts.saveXML( *XF, 0, Options::FirstOnly, 3 );
      *XF << "    </section>\n";
    }
    *XF << "  </section>\n";
  }

  // create xml file for stimulus metadata:
  XSF = openFile( "stimulus-metadata.xml", ios::out );
  if ( (*XSF) ) {
    *XSF << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    *XSF << "<?xml-stylesheet type=\"text/xsl\" href=\"odml.xsl\"  xmlns:odml=\"http://www.g-node.org/odml\"?>\n";
    *XSF << "<odML version=\"1\">\n";
    *XSF << "<repository>http://portal.g-node.org/odml/terminologies/v1.0/terminologies.xml</repository>\n";
  }
}


string SaveFiles::pathName( void ) const
{
  int az = ('z'-'a'+1);
  Str pathname = PathTemplate;
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
  return pathname;
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
  ToggleOn = false;
  Saving = false;

  ReProData = false;
  ReProSettings.clear();
  ReProFiles.clear();
  DatasetOpen = false;

  setPath( defaultPath() );

  PathTime = RW->SN->startSessionTime();
  // pathname for same PathNumber changed?
  if ( pathName() != PrevPath ) {
    PathNumber = 0;
    RW->printlog( "SaveFiles::openFiles -> reset PathNumber to " + Str( PathNumber ) );
  }

  // generate unused name for new files/directory:
  PathNumber++;
  RW->printlog( "SaveFiles::openFiles -> incremented PathNumber to " + Str( PathNumber ) );
  QDir dir( QDir::current() );
  int az = ('z'-'a'+1);
  for ( ; PathNumber <= az*az; PathNumber++ ) {

    // create file name:
    string pathname = pathName();

    if ( pathname[pathname.size()-1] == '/' ) {
      // try to create new directory:
      if ( ! dir.exists( pathname.c_str() ) ) {
	if ( dir.mkpath( pathname.c_str() ) ) {
	  // valid base name found:
	  setPath( pathname );
	  break;
	}
	else
	  RW->printlog( "! error: SaveFiles::openFiles -> failed to make directory " + pathname + "!" );
      }
    }
    else {
      // try to open files:
      string fs = path() + "stimuli.dat";
      ifstream f( fs.c_str() );
      // files do not exist?
      if ( ! f.good() ) {
	// valid base name found:
	setPath( pathname );
	break;
      }
    }
  }
  // running out of names?
  if ( PathNumber > az*az ) {
    RW->printlog( "! panic: SaveFiles::openFiles -> can't create data path! PathNumber=" + Str( PathNumber ) );
  }

  // anyways, we try to save what we can:

  // open files:
  createTraceFiles( traces );
  createEventFiles( events );
  createStimulusFile( traces, events );
  createXMLFile( traces, events );
  FilesOpen = true;
  Hold = false;

  // add recording event:
  for ( int k=0; k<events.size(); k++ ) {
    if ( (events[k].mode() & RecordingEventMode) > 0 ) {
      events[k].push( traces.currentTime() );
      break;
    }
  }

  // message:
  RW->printlog( "save in " + path() );

  // tell the data index:
  DI.addSession( path() + "stimuli.dat", Options() );

  // update widget:
  FileLabel->setFont( HighlightFont );
  FileLabel->setPalette( HighlightPalette );
  FileLabel->setText( path().c_str() );
  SaveLabel->setPause( Saving );
  SaveLabel->setSpike( FilesOpen );
}


void SaveFiles::closeFiles( void )
{
  ToggleData = true;
  ToggleOn = false;
  Hold = false;

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
  if ( SDF != 0 )
    delete SDF;
  SDF = 0;
  if ( XF != 0 ) {
    if ( DatasetOpen ) {
      if ( ReProFiles.size() > 0 ) {
	string files = ReProFiles[0];
	for ( unsigned int k=1; k<ReProFiles.size(); k++ )
	  files += ", " + ReProFiles[k];
	Parameter p( "File", "", files );
	p.saveXML( *XF, 2, 4 );
      }
      ReProFiles.clear();
      *XF << "  </section>\n";
      DatasetOpen = false;
    }
    string name = Str( path() ).preventedSlash().name();
    RW->MTDT.saveXML( *XF, 1, name );
    *XF << "</odML>\n";
    delete XF;
  }
  XF = 0;
  if ( XSF != 0 ) {
    *XSF << "</odML>\n";
    delete XSF;
  }
  XSF = 0;

  if ( FilesOpen ) {
    RW->MTDT.section( "Recording" ).erase( "File" );
    FilesOpen = false;
  }

  SaveLabel->setSpike( false );
}


bool SaveFiles::removeDir( const QString &dirname )
{
  QDir dir( dirname );
  if ( dir.exists() ) {
    bool success = true;
    QFileInfoList list = dir.entryInfoList( QDir::NoDotAndDotDot|QDir::AllDirs|QDir::Files|QDir::Hidden,
					    QDir::DirsFirst );
    for ( int i=0; i<list.size(); i++ ) {
      QFileInfo info = list.at( i );
      if ( info.isDir() ) {
	if ( ! removeDir( info.absoluteFilePath() ) )
	  success = false;
      }
      else {
	if ( ! dir.remove( info.absoluteFilePath() ) ) {
	  success = false;
	  RW->printlog( "! error: failed to remove file " + string( info.absoluteFilePath().toStdString() ) );
	}
      }
    }
    if ( success ) {
      success = dir.rmdir( dir.absolutePath() );
      if ( ! success )
	RW->printlog( "! error: failed to remove directory " + string( dirname.toStdString() ) );
    }
    return success;
  }
  else {
    return false;
  }
}


void SaveFiles::deleteFiles( void )
{
  closeFiles();

  // remove all files:
  removeFiles();

  if ( path() != defaultPath() &&
       path() != "" &&
       path()[path().size()-1] == '/' ) {
    removeDir( path().c_str() );
  }

  // message:
  RW->printlog( "discarded " + path() );
  FileLabel->setPalette( NormalPalette );
  FileLabel->setFont( NormalFont );
  FileLabel->setText( "deleted" );

  // back to default path:
  setPath( defaultPath() );
  PathNumber--;

  // tell data index:
  DI.endSession( false );
}


void SaveFiles::completeFiles( void )
{
  closeFiles();

  // no files need to be deleted:
  clearRemoveFiles();

  // store path:
  PrevPath = path();

  // message:
  RW->printlog( "saved as " + path() );
  FileLabel->setPalette( NormalPalette );

  // back to default path:
  setPath( defaultPath() );

  // tell data index:
  DI.endSession( true );
}


DataIndex *SaveFiles::dataIndex( void )
{
  return &DI;
}


void SaveFiles::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {

  case 1: {
    StimuliEvent *se = dynamic_cast<StimuliEvent*>( qce );
    DI.addStimulus( se->Description );
    break;
  }

  }
}


}; /* namespace relacs */

#include "moc_savefiles.cc"

