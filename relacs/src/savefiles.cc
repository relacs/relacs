/*
  savefiles.cc
  Save data to files

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

#include <cstdio>
#include <QDir>
#include <QHostInfo>
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
#include <relacs/filterdetectors.h>

namespace relacs {


class StimuliEvent : public QEvent
{

public:

  StimuliEvent( const deque< OutDataInfo > &stimuli,
		const deque<int> &traceindex, const deque<int> &eventsindex,
		double time )
    : QEvent( Type( User+1 ) ),
      TraceIndex( traceindex ),
      EventsIndex( eventsindex ),
      Time( time )
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
  deque<int> TraceIndex;
  deque<int> EventsIndex;
  double Time;
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

  WriteRelacsFiles = true;
  WriteODMLFiles = true;
  WriteNIXFiles = true;
  FilesOpen = false;
  Saving = false;
  Hold = false;

  Stimuli.clear();
  ReProName = "";
  ReProStimulusCount.clear();
  ReProStimuli.clear();
  SignalTime = -1.0;
  PrevSignalTime = -1.0;

  clearRemoveFiles();

  ToggleOn = false;
  ToggleData = false;

  ReProInfo.clear();
  ReProFiles.clear();
  ReProData = false;

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

  Stimuli.clear();
  ReProStimulusCount.clear();
  ReProStimuli.clear();
}


bool SaveFiles::saving( void ) const
{
  QMutexLocker locker( &SaveMutex );
  return isSaving();
}


bool SaveFiles::isSaving( void ) const
{
  return FilesOpen && Saving;
}


bool SaveFiles::filesOpen( void ) const
{
  QMutexLocker locker( &SaveMutex );
  return FilesOpen;
}


void SaveFiles::holdOn( void )
{
  QMutexLocker locker( &SaveMutex );
  Hold = true;
}


void SaveFiles::holdOff( void )
{
  QMutexLocker locker( &SaveMutex );
  Hold = false;
}


string SaveFiles::path( void ) const
{
  QMutexLocker locker( &SaveMutex );
  return Path;
}


void SaveFiles::setPath( const string &path )
{
  QMutexLocker locker( &SaveMutex );
  setThePath( path );
}


void SaveFiles::setThePath( const string &path )
{
  Path = path;
  setenv( "RELACSDATAPATH", Path.c_str(), 1 );
}


string SaveFiles::addPath( const string &file ) const
{
  QMutexLocker locker( &SaveMutex );
  return Path + file;
}


void SaveFiles::storeFile( const string &file ) const
{
  QMutexLocker locker( &SaveMutex );
  for ( deque<string>::const_iterator sp = ReProFiles.begin(); sp != ReProFiles.end(); ++sp ) {
    if ( *sp == file )
      return;
  }
  ReProFiles.push_back( file );
}


string SaveFiles::pathTemplate( void ) const
{
  QMutexLocker locker( &SaveMutex );
  return PathTemplate;
}


void SaveFiles::setPathTemplate( const string &path )
{
  if ( path.empty() )
    return;

  SaveMutex.lock();
  PathTemplate = path;
  Str fn = PathTemplate;
  fn.format( localtime( &PathTime ) );
  fn.format( 99, 'n', 'd' );
  fn.format( "aa", 'a' );
  fn.format( "AA", 'A' );
  fn.format( QHostInfo::localHostName().toStdString(), 'h' );
  SaveMutex.unlock();

  FileLabel->setFixedWidth( QFontMetrics( HighlightFont ).boundingRect( fn.c_str() ).width() + 8 );
}


string SaveFiles::defaultPath( void ) const
{
  QMutexLocker locker( &SaveMutex );
  return DefaultPath;
}


void SaveFiles::setDefaultPath( const string &defaultpath )
{
  if ( defaultpath.empty() )
    return;

  QMutexLocker locker( &SaveMutex );
  if ( Path == DefaultPath )
    setThePath( defaultpath );
  DefaultPath = defaultpath;
  setenv( "RELACSDEFAULTPATH", DefaultPath.c_str(), 1 );
}


string SaveFiles::addDefaultPath( const string &file ) const
{
  QMutexLocker locker( &SaveMutex );
  return DefaultPath + file;
}


void SaveFiles::setWriteRelacsFiles( bool write )
{
  WriteRelacsFiles = write;
}


void SaveFiles::setWriteODMLFiles( bool write )
{
  WriteODMLFiles = write;
}


void SaveFiles::setWriteNIXFiles( bool write, bool compression )
{
  WriteNIXFiles = write;
  CompressNIXFiles = compression;
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


void SaveFiles::setTracesEvents( const InList &il, const EventList &el )
{
  QMutexLocker locker( &SaveMutex );
  IL.clear();
  IL.add( il );
  EL.clear();
  EL.add( el );
}


void SaveFiles::save( bool on  )
{
  // this function is called from RELACSWidget::setSaving() and
  // right before starting a RePro in RELACSWidget::startRepRo()

  //  cerr << "SaveFiles::save( bool on ): on=" << on << '\n';
  QMutexLocker locker( &SaveMutex );
  if ( ! FilesOpen ) {
    Saving = false;
    return;
  }

  ToggleData = true;
  ToggleOn = on;
}


void SaveFiles::writeToggle( void )
{
  // only called by saveTraces().

  //  cerr << "SaveFiles::writeToggle(): ToggleData=" << ToggleData
  //       << ", hold=" << Hold << ", on=" << ToggleOn << ", saving=" << isSaving() << '\n';

  if ( ToggleData && ! Hold ) {
    if ( ToggleOn && ! isSaving() ) {
      // RW->printlog( "SaveFiles::writeToggle(): switched saving on!" );
      RelacsIO.resetIndex( IL );
      RelacsIO.resetIndex( EL );
      #ifdef HAVE_NIX
      if ( WriteNIXFiles ) {
        NixIO.resetIndex( IL );
        NixIO.resetIndex( EL );
      }
      #endif    
      // Add recording event:
      for ( int k=0; k<EL.size(); k++ ) {
	if ( ( EL[k].mode() & RecordingEventMode ) > 0 ) {
	  EL[k].push( IL[0].pos( IL[0].size() ) );
	  break;
	}
      }
    }
    Saving = ToggleOn;
    SaveLabel->setPause( ! Saving );
    ToggleData = false;
  }
}


void SaveFiles::saveTraces( void )
{
  //  cerr << "SaveFiles::saveTraces(): saving=" << saving() << '\n';

  QMutexLocker locker( &SaveMutex );

  // this function is called from RELACSWidget::updateData()

  // update save status:
  writeToggle();

  // check for new signal:
  if ( ! Stimuli.empty() && ! EL.empty() && EL[0].size() > 0 ) {
    double st = EL[0].back();
    if ( isSaving() &&
	 ::fabs( IL[0].signalTime() - st ) >= IL[0].stepsize() )
      RW->printlog( "Warning in SaveFiles::saveTraces() -> SignalTime PROBLEM, trace: " +
		    Str( IL[0].signalTime(), 0, 5, 'f' ) +
		    " stimulus: " + Str( st, 0, 5, 'f' ) );
    if ( st > PrevSignalTime )
      SignalTime = st;
  }
  bool stimulus = !Stimuli.empty() && SignalTime >= 0.0;

  writeRePro();
  writeTraces( stimulus );
  writeEvents( stimulus );
  if ( stimulus )
    writeStimulus();
}


void SaveFiles::writeTraces( bool stimulus )
{
  // only called by saveTraces().

  //  cerr << "SaveFiles::writeTraces(): saving=" << isSaving() << "\n";

  if ( ! isSaving() )
    return;

  RelacsIO.writeTraces( IL, stimulus );

  #ifdef HAVE_NIX
  if ( WriteNIXFiles ) 
    NixIO.writeTraces( IL );
  #endif
}


void SaveFiles::writeEvents( bool stimulus )
{
  // only called by saveTraces().

  //  cerr << "SaveFiles::writeEvents(): offs=" << offs << ", saving=" << isSaving() << "\n";

  if ( ! isSaving() )
    return;

  // save event data:
  RelacsIO.writeEvents( IL, EL, stimulus );

  #ifdef HAVE_NIX
  // double noffs = IL[0].interval( NixIO.traces[0].index - NixIO.traces[0].offset[0] ) - SessionTime;
  /*
  double offs = 0.0;
  if ( ! TraceFiles.empty() )
    offs = IL[0].interval( TraceFiles[0].Index - TraceFiles[0].Written );
  */
  NixIO.writeEvents( IL, EL );
  #endif
}


void SaveFiles::save( const OutData &signal )
{
  // this function is called from RELACSWidget::write()
  // after a successfull call of Acquire::write()

  //  cerr << "SaveFiles::save( OutData &signal ): saving=" << saving() << "\n";

  if ( signal.failed() )
    return;

  QMutexLocker locker( &SaveMutex );

  if ( !Stimuli.empty() ) {
    if ( isSaving() )
      RW->printlog( "! warning: SaveFiles::save( OutData & ) -> already stimulus data there" );
    Stimuli.clear();
  }

  // StimulusDataLock is already locked from within the RePro!
  StimulusData = *this;  // values of all output traces before signal was written

  // store stimulus and update output channel setting:
  Stimuli.push_back( signal );
  setNumber( RW->AQ->outTraceName( signal.trace() ), signal.back() );
  // XXX why not using signal.traceName() ?

  // reset stimulus offset:
  PrevSignalTime = SignalTime;
  SignalTime = -1.0;
}


void SaveFiles::save( const OutList &signal )
{
  //  cerr << "SaveFiles::save( OutList &signal ): saving=" << saving() << "\n";

  if ( signal.empty() || signal.failed() )
    return;

  QMutexLocker locker( &SaveMutex );

  if ( !Stimuli.empty() ) {
    if ( isSaving() )
      RW->printlog( "! warning: SaveFiles::save( OutList& ) -> already stimulus data there" );
    Stimuli.clear();
  }

  // StimulusDataLock is already locked from within the RePro!
  StimulusData = *this;

  // store stimulus and update output channel settings:
  for ( int k=0; k<signal.size(); k++ ) {
    Stimuli.push_back( signal[k] );
    setNumber( RW->AQ->outTraceName( signal[k].trace() ), signal[k].back() );
    // XXX why not using signal[k].traceName() ?
  }
  
  // reset stimulus offset:
  PrevSignalTime = SignalTime;
  SignalTime = -1.0;
}


void SaveFiles::extractMutables( Options &stimulusdescription, Options &mutables ) const
{
  for ( Options::iterator pi = stimulusdescription.begin();
	pi != stimulusdescription.end();
	++pi ) {
    if ( (pi->flags() & OutData::Mutable) == OutData::Mutable ) {
            mutables.add( *pi );
      // stimulusdescription.erase( *pi );
      pi->setText( "" );
      pi->setUnit( "" );
      if ( pi->isNumber() )
	pi->setNumber( 0.0 );
    }
  }
  for ( Options::section_iterator si = stimulusdescription.sectionsBegin();
	si != stimulusdescription.sectionsEnd();
	++si ) {
    extractMutables( *(*si), mutables );
  }
}


void SaveFiles::writeStimulus( void )
{
  // only called by saveTraces()
  // with the SaveMutex locked.

  //  cerr << "SaveFiles::writeStimulus(): Stimuli.size()=" << Stimuli.size()
  //       << ", saving=" << isSaving() << "\n";

  // add to data index for browsing:
  deque<int> traceindex;
  RelacsIO.traceSignalIndices( traceindex );
  deque<int> eventsindex;
  RelacsIO.traceSignalIndices( eventsindex );
  // add signal to data browser:
  QCoreApplication::postEvent( this, new StimuliEvent( Stimuli, traceindex, eventsindex,
						       SignalTime ) );

  // extract mutable parameter from stimulus description to a subsection of stimuliref:
  deque< Options > stimuliref( Stimuli.size() );
  for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
    Options &mutables = stimuliref[j].newSection( "parameter" );
    extractMutables( Stimuli[j].description(), mutables );
    stimuliref[j].clearSections();
    // XXX once OutData does not have idents any more, the following lines can be erased:
    if ( ! Stimuli[j].ident().empty() ) {
      stimuliref[j].addText( "Description", Stimuli[j].ident() );
    }
  }

  // generate names for all stimulus traces:
  // i.e reproname-type1-type2 ...
  // where type1, type2 are the types of the stimulus descriptions with leading 'stimulus/' removed.
  deque< string > stimulinames( Stimuli.size() );
  for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
    if ( Stimuli[j].ident().empty() ) {
      string sn = ReProName;  // (name of the RePro from which the stimulus was written)
      // add type of stimulus description:
      Str tn = Stimuli[j].description().type();
      if ( tn.empty() )
        tn = Stimuli[j].description().name();
      else {
        tn.eraseFirst( "stimulus" );
        tn.preventFirst( '/' );
      }
      if ( ! tn.empty() )
        sn += '-' + tn;
      string pn = "";
      int pc = 0;
      for ( Options::const_section_iterator si=Stimuli[j].description().sectionsBegin();
            si != Stimuli[j].description().sectionsEnd();
            ++si ) {
        tn = (*si)->type();
        if ( tn.empty() )
          tn = (*si)->name();
        else {
          tn.eraseFirst( "stimulus" );
          tn.preventFirst( '/' );
        }
        if ( ! tn.empty() ) {
          // sqeeze repetitive types:
          if ( tn != pn ) {
            if ( pc > 1 )
              sn += '*' + Str( pc );
            sn += '-' + tn;
            pc = 1;
          }
          else
            pc++;
          pn = tn;
        }
      }
      if ( pc > 1 )
        sn += '*' + Str( pc );
      stimulinames[j] = sn;
    }
    else {
      stimulinames[j] = Stimuli[j].ident();
    }
  }

  // track stimulus traces:
  deque< bool > newstimuli( Stimuli.size(), false );
  for ( unsigned int j=0; j<Stimuli.size(); j++ ) {
    // get all stimulus descriptions for the stimulusname reproname-type1-type2,
    // i.e. get all stimuli of a given repro and type:
    map< Options, string > &rsd = ReProStimuli[ stimulinames[j] ];

    // retrieve the unique identifier for the specific stimulus description:
    string &stimulusid = rsd[ Stimuli[j].description() ];
    if ( stimulusid.empty() ) {
      // this stimulus description is new:
      newstimuli[j] = true;
      stimulusid = stimulinames[j];
      if ( ! Stimuli[j].description().name().empty() )
	stimulusid = Stimuli[j].description().name();
      // append the number of stimuli in this category to make the name unique:
      stimulusid += '-' + Str( rsd.size() );
    }
    // set name, type, and some additional information for each stimulus trace:
    Stimuli[j].description().setName( stimulusid );
    stimuliref[j].setName( stimulusid );
    if ( Stimuli[j].description().type().empty() ) {
      Stimuli[j].description().setType( "stimulus" );
      stimuliref[j].setType( "stimulus" );
    }
    Stimuli[j].description().insertNumber( "SamplingRate", "", 0.001*Stimuli[j].sampleRate(), "kHz" );
    Stimuli[j].description().insertText( "Modality", "", RW->AQ->outTrace( Stimuli[j].trace() ).modality() );
  }

  // track stimulus number per repro:
  int &rc = ReProStimulusCount[ ReProName ];
  rc++;
  string repronamecount = ReProName + "-" + Str( rc );

  if ( isSaving() ) {

    int stimulusindex[RW->AQ->outTracesSize()];
    for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
      stimulusindex[k] = -1;
      const Attenuate *att = RW->AQ->outTraceAttenuate( k );
      unsigned int j=0;
      for ( j=0; j<Stimuli.size(); j++ ) {
	if ( Stimuli[j].device() == RW->AQ->outTrace( k ).device() &&
	     Stimuli[j].channel() == RW->AQ->outTrace( k ).channel() ) {
	  Stimuli[j].setTrace( k ); // needed for XML
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
	  stimuliref[j].insertNumber( "Time", "", IL[0].signalTime() - SessionTime, "s" );
	  stimulusindex[k] = j;
	  break;
	}
      }
    }

    // stimuliref is an array of Options that contains the data
    // written for each stimulus in the line in stimuli.dat .
    
    RelacsIO.writeStimulus( IL, EL, Stimuli, newstimuli, StimulusData,
			    stimuliref, stimulusindex,
			    SessionTime, repronamecount, RW->AQ );
    OdmlIO.writeStimulus( IL, EL, Stimuli, newstimuli, StimulusData,
			    stimuliref, stimulusindex,
			    SessionTime, repronamecount, RW->AQ );
    #ifdef HAVE_NIX
    if ( WriteNIXFiles ) {
      NixIO.writeStimulus( IL, EL, Stimuli, newstimuli, StimulusData, stimuliref, stimulusindex,
                           SessionTime, repronamecount, RW->AQ );
    }
    #endif
  }
  Stimuli.clear();
}


void SaveFiles::save( const RePro &rp, Options &macrostack )
{
  //  cerr << "SaveFiles::save( const RePro &rp ): RePro=" << rp.name()
  //       << ", saving=" << saving() << "\n";

  QMutexLocker locker( &SaveMutex );

  if ( ReProData && isSaving() )
    RW->printlog( "! warning: SaveFiles::save( RePro& ) -> already RePro data there." );
  ReProData = true;
  string dataset = Str( Path ).preventedSlash().name()
    + "-" + rp.name() + "-" + Str( rp.allRuns() + 1 );
  ReProInfo.clear();
  ReProInfo.setName( "RePro-Info", "relacs/repro" );
  ReProInfo.addText( "RePro", rp.name() );
  ReProInfo.addText( "Author", rp.author() );
  ReProInfo.addText( "Version", rp.version() );
  ReProInfo.addText( "Date", rp.date() );
  ReProInfo.addInteger( "Run", rp.allRuns() + 1 );
  if ( macrostack.sectionsSize() > 0 ) {
    ReProInfo.newSection( "macros" , "relacs/repro/macros" );
    for ( Options::const_section_iterator sp=macrostack.sectionsBegin(); 
	  sp != macrostack.sectionsEnd(); ++sp ) {
      ReProInfo.addText( (*sp)->name(), (*sp)->save( 0, Options::NoName ) );
    }
    ReProInfo.clearSections();
  }
  ReProInfo.newSection( rp, 0, "settings", "relacs/repro/settings" );
  DI.addRepro( ReProInfo );
  ReProName = rp.name();
}


void SaveFiles::writeRePro( void )
{
  // only called by saveTraces().

  //  cerr << "SaveFiles::writeRePro(): ReProData=" << ReProData
  //       << ", saving=" << isSaving() << "\n";
  #ifdef HAVE_NIX
  if ( ReProData && NixIO.was_writing ) {
    NixIO.endRePro( NixIO.traces[0].written * IL[0].stepsize() );
    NixIO.was_writing = false;
  }
  #endif
  if ( ReProData && isSaving() && ! Hold ) {
    RelacsIO.writeRePro( ReProInfo, ReProFiles, IL, EL, *this, SessionTime );
    OdmlIO.writeRePro( ReProInfo, ReProFiles, IL, EL, *this, SessionTime );
    #ifdef HAVE_NIX
    if ( WriteNIXFiles )
      NixIO.writeRePro( ReProInfo, ReProFiles, IL, EL, *this, SessionTime );
    #endif
    // nix file:
    //    XXX write ReProInfo as odml tree

    ReProData = false;
    ReProStimulusCount[ ReProName ] = 0;
    ReProFiles.clear();
  }
}


bool SaveFiles::signalPending( void ) const
{
  QMutexLocker locker( &SaveMutex );
  return !Stimuli.empty();
}


void SaveFiles::clearSignal( void )
{
  QMutexLocker locker( &SaveMutex );
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


void SaveFiles::addFile( const string &filename )
{
  addRemoveFile( Path + filename );
  // add to recording section:
  RW->MTDT.lock();
  Options &opt = RW->MTDT.section( "Recording" );
  if ( opt.exist( "File" ) )
    opt.pushText( "File", filename );
  else {
    opt.insertText( "File", "Date", filename,
		    MetaData::standardFlag(), Parameter::ListAlways );
  }
  RW->MTDT.unlock();
}


ofstream *SaveFiles::openFile( const string &filename, int type )
{
  string filepath = Path + filename;
  ofstream *f = new ofstream( filepath.c_str(), ofstream::openmode( type ) );
  if ( ! f->good() ) {
    f = 0;
    RW->printlog( "! error in SaveFiles::openFile: can't open file '" + filepath + "'!" );
  }
  else
    addFile( filename );
  return f;
}


void SaveFiles::createRelacsFiles( void )
{
  if ( WriteRelacsFiles &&
       RelacsIO.open( IL, EL, *this, RW->AQ, Path, this, RW->ADV ) )
    FilesOpen = true;
}


void SaveFiles::createODMLFiles( void )
{
  if ( WriteODMLFiles &&
       OdmlIO.open( Path, this, RW->ADV ) )
    FilesOpen = true;
}


void SaveFiles::createNIXFile( void )
{
  #ifdef HAVE_NIX
  if ( WriteNIXFiles ) {
    string nix_path = NixIO.create( Path, CompressNIXFiles );
    if ( nix_path.empty() ) {
      RW->printlog( "! warning in SaveFiles::createNIXFile: could not create NIX data file in '" + Path + "'" );
      return;
    }
    addRemoveFile( nix_path );
    NixIO.initTraces( IL );
    NixIO.initEvents( EL, RW->FD );
    FilesOpen = true;
  }
  #endif
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
  pathname.format( QHostInfo::localHostName().toStdString(), 'h' );
  return pathname;
}


void SaveFiles::openFiles( void )
{
  QMutexLocker locker( &SaveMutex );

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
  ReProInfo.clear();
  ReProFiles.clear();
  SessionTime = IL.currentTime();

  // init stimulus variables:
  Stimuli.clear();
  ReProStimulusCount.clear();
  ReProStimuli.clear();
  SignalTime = -1.0;
  PrevSignalTime = -1.0;

  setThePath( DefaultPath );

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
	  setThePath( pathname );
	  break;
	}
	else
	  RW->printlog( "! error: SaveFiles::openFiles -> failed to make directory " + pathname + "!" );
      }
    }
    else {
      // try to open files:
      ifstream rf( string( pathname + "stimuli.dat" ).c_str() );
      ifstream xf( string( pathname + "metadata.xml" ).c_str() );
      ifstream nf( string( pathname + Str( pathname ).name() + ".nix" ).c_str() );
      // files do not exist?
      if ( ! rf.good() && ! xf.good() && ! nf.good() ) {
	// valid base name found:
	setThePath( pathname );
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
  lock();
  createRelacsFiles();
  createODMLFiles();
  #ifdef HAVE_NIX
  if ( WriteNIXFiles )
    createNIXFile();
  #endif
  unlock();
  Hold = false;

  // add recording event:
  for ( int k=0; k<EL.size(); k++ ) {
    if ( (EL[k].mode() & RecordingEventMode) > 0 ) {
      EL[k].push( IL.currentTime() );
      break;
    }
  }

  // message:
  RW->printlog( "save in " + Path );

  // update widget:
  FileLabel->setFont( HighlightFont );
  FileLabel->setPalette( HighlightPalette );
  FileLabel->setText( Path.c_str() );
  SaveLabel->setPause( Saving );
  SaveLabel->setSpike( FilesOpen );
}


void SaveFiles::closeFiles( void )
{
  ToggleData = true;
  ToggleOn = false;
  Hold = false;

  RelacsIO.close( Path, ReProFiles, RW->MTDT );
  OdmlIO.close( Path, ReProFiles, RW->MTDT );

  #ifdef HAVE_NIX
  if ( WriteNIXFiles && NixIO.fd.isOpen() ) {
    NixIO.saveMetadata( RW->MTDT  );
    NixIO.close();
  }
  #endif

  ReProFiles.clear();

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
  QMutexLocker locker( &SaveMutex );

  closeFiles();

  // remove all files:
  removeFiles();

  if ( Path != DefaultPath &&
       ! Path.empty() &&
       Path[Path.size()-1] == '/' ) {
    removeDir( Path.c_str() );
  }

  // message:
  RW->printlog( "discarded " + Path );
  FileLabel->setPalette( NormalPalette );
  FileLabel->setFont( NormalFont );
  FileLabel->setText( "deleted" );

  // back to default path:
  setThePath( DefaultPath );
  PathNumber--;

  // tell data index:
  DI.endSession( false );
}


void SaveFiles::completeFiles( void )
{
  QMutexLocker locker( &SaveMutex );

  closeFiles();

  // no files need to be deleted:
  clearRemoveFiles();

  // store path:
  PrevPath = Path;

  // message:
  RW->printlog( "saved as " + Path );
  FileLabel->setPalette( NormalPalette );

  // back to default path:
  setThePath( DefaultPath );

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
    DI.addStimulus( se->Description, se->TraceIndex, se->EventsIndex, se->Time );
    break;
  }

  }
}


SaveFiles::RelacsFiles::RelacsFiles( void )
{
  SF = 0;
  SDF = 0;
  TraceFiles.clear();
  EventFiles.clear();
  StimulusKey.clear();
}


bool SaveFiles::RelacsFiles::open( const InList &IL, const EventList &EL, 
				   const Options &data, const Acquire *acquire, 
				   const string &path, SaveFiles *save, 
				   const AllDevices *devices )
{
  openTraceFiles( IL, save );
  openEventFiles( EL, save );
  openStimulusFiles( IL, EL, data, acquire, save );

  // tell the data index:
  save->DI.addSession( path + "stimuli.dat", Options(), TraceFiles.size(), EventFiles.size() );

  return ( SF != 0 && SF->good() );
}


void SaveFiles::RelacsFiles::openTraceFiles( const InList &IL, SaveFiles *save )
{
  string format = "%d";
  if ( IL.size() > 9 )
    format = "%02d";
  else if ( IL.size() > 99 )
    format = "%03d";

  TraceFiles.resize( IL.size() );

  for ( int k=0; k<IL.size(); k++ ) {

    // init trace variables:
    TraceFiles[k].Index = IL[k].size();
    TraceFiles[k].Written = 0;
    TraceFiles[k].SignalOffset = -1;

    // create file:
    if ( IL[k].mode() & SaveFiles::SaveTrace ) {
      Str fn = IL[k].ident();
      TraceFiles[k].FileName = "trace-" + Str( k+1, format ) + ".raw";
      // TraceFiles[k].FileName = "trace-" + Str( k+1, format ) + ".au";
      TraceFiles[k].Stream = save->openFile( TraceFiles[k].FileName, ios::out | ios::binary );
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
	d.data = (long)::rint( IL[k].sampleRate() );
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


void SaveFiles::RelacsFiles::openEventFiles( const EventList &EL, SaveFiles *save )
{
  EventFiles.resize( EL.size() );

  for ( int k=0; k<EL.size(); k++ ) {

    // init event variables:
    EventFiles[k].Index = EL[k].size();
    EventFiles[k].Written = 0;
    EventFiles[k].SignalEvent = 0;

    // create file:
    if ( EL[k].mode() & SaveFiles::SaveTrace ) {
      Str fn = EL[k].ident();
      EventFiles[k].FileName = fn.lower() + "-events.dat";
      EventFiles[k].Stream = save->openFile( EventFiles[k].FileName, ios::out );
      if ( EventFiles[k].Stream->good() ) {
	// save header:
	*EventFiles[k].Stream << "# events: " << EL[k].ident() << '\n';
	*EventFiles[k].Stream << '\n';
	// init key:
	EventFiles[k].Key.clear();
	EventFiles[k].Key.addNumber( "t", "sec", "%0.5f" );
	if ( EL[k].sizeBuffer() )
	  EventFiles[k].Key.addNumber( EL[k].sizeName(), EL[k].sizeUnit(),
				       EL[k].sizeFormat() );
	if ( EL[k].widthBuffer() )
	  EventFiles[k].Key.addNumber( EL[k].widthName(), EL[k].widthUnit(),
				       EL[k].widthFormat() );
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


void SaveFiles::RelacsFiles::openStimulusFiles( const InList &IL, const EventList &EL, 
						const Options &data,
						const Acquire *acquire, SaveFiles *save )
{
  // create file for stimuli:
  SF = save->openFile( "stimuli.dat", ios::out );

  if ( (*SF) ) {
    // save header:
    *SF << "# analog input traces:\n";
    for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
      if ( ! TraceFiles[k].FileName.empty() ) {
	*SF << "#      identifier" + Str( k+1 ) + "     : " << IL[k].ident() << '\n';
	*SF << "#      data file" + Str( k+1 ) + "      : " << TraceFiles[k].FileName << '\n';
	*SF << "#      sample interval" + Str( k+1 ) + ": " << Str( 1000.0*IL[k].sampleInterval(), 0, 4, 'f' ) << "ms\n";
	*SF << "#      sampling rate" + Str( k+1 ) + "  : " << Str( IL[k].sampleRate(), 0, 2, 'f' ) << "Hz\n";
	*SF << "#      unit" + Str( k+1 ) + "           : " << IL[k].unit() << '\n';
      }
    }
    *SF << "# event lists:\n";
    for ( unsigned int k=0; k<EventFiles.size(); k++ ) {
      if ( ! EventFiles[k].FileName.empty() )
	*SF << "#      event file" + Str( k+1 ) + ": " << EventFiles[k].FileName << '\n';
    }
    *SF << "# analog output traces:\n";
    for ( int k=0; k<acquire->outTracesSize(); k++ ) {
      TraceSpec trace( acquire->outTrace( k ) );
      *SF << "#      identifier" + Str( k+1 ) + ": " << trace.traceName() << '\n';
      *SF << "#      device" + Str( k+1 ) + "    : " << trace.device() << '\n';
      *SF << "#      channel" + Str( k+1 ) + "   : " << trace.channel() << '\n';
      /* Get the following infos from the devices:
      *SF << "#      signal delay" + Str( k+1 ) + ": " << 1000.0*trace.signalDelay() << "ms\n";
      *SF << "#      maximum rate" + Str( k+1 ) + ": " << 0.001*trace.maxSampleRate() << "kHz\n";
      */
    }
    *SF << "# stimulus descriptions file: stimulus-descriptions.dat\n";

    // create key:
    StimulusKey.clear();
    StimulusKey.newSection( "traces" );
    for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
      if ( TraceFiles[k].Stream != 0 ) {
	StimulusKey.newSubSection( IL[k].ident() );
	StimulusKey.addNumber( "index", "float", "%10.0f" );
      }
    }
    StimulusKey.newSection( "events" );
    for ( unsigned int k=0; k<EventFiles.size(); k++ )
      if ( EventFiles[k].Stream != 0 ) {
	StimulusKey.newSubSection( EL[k].ident() );
	StimulusKey.addNumber( "index", "line", "%10.0f" );
	if ( (EL[k].mode() & ( StimulusEventMode | RestartEventMode | RecordingEventMode ) ) == 0 )
	  StimulusKey.addNumber( "freq", "Hz", "%6.1f" );
	if ( EL[k].sizeBuffer() )
	  StimulusKey.addNumber( EL[k].sizeName(), EL[k].sizeUnit(), EL[k].sizeFormat() );
	if ( EL[k].widthBuffer() && (EL[k].mode() & StimulusEventMode) == 0 )
	  StimulusKey.addNumber( EL[k].widthName(), EL[k].widthUnit(), EL[k].widthFormat() );
	EventFiles[k].SaveMeanQuality = ( EL[k].mode() & SaveMeanQuality );
	if ( EventFiles[k].SaveMeanQuality )
	  StimulusKey.addNumber( "quality", "%", "%3.0f" );
      }
    if ( !data.empty() ) {
      StimulusKey.newSection( "data" );
      for( int k=0; k<data.size(); k++ )
	StimulusKey.addNumber( data[k].name(), data[k].outUnit(), data[k].format() );
    }
    StimulusKey.newSection( "stimulus" );
    StimulusKey.newSubSection( "timing" );
    StimulusKey.addNumber( "time", "s", "%11.5f" );
    StimulusKey.addNumber( "delay", "ms", "%5.1f" );
    for ( int k=0; k<acquire->outTracesSize(); k++ ) {
      StimulusKey.newSubSection( acquire->outTraceName( k ) );
      StimulusKey.addNumber( "rate", "kHz", "%8.3f" );
      StimulusKey.addNumber( "duration", "ms", "%8.0f" );
      const Attenuate *att = acquire->outTraceAttenuate( k );
      if ( att != 0 ) {
	StimulusKey.addNumber( att->intensityName(), att->intensityUnit(),
			       att->intensityFormat() );
	if ( ! att->frequencyName().empty() )
	  StimulusKey.addNumber( att->frequencyName(), att->frequencyUnit(),
				 att->frequencyFormat() );
      }
      StimulusKey.addText( "signal", -30 );
      StimulusKey.addText( "parameter", -30 );
    }
  }

  // create file for stimuli:
  SDF = save->openFile( "stimulus-descriptions.dat", ios::out );
}


void SaveFiles::RelacsFiles::close( const string &path, const deque< string > &reprofiles,
				    MetaData &metadata )
{
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
}


void SaveFiles::RelacsFiles::resetIndex( const InList &IL )
{
  for ( unsigned int k=0; k<TraceFiles.size(); k++ )
    TraceFiles[k].Index = IL[k].size();
}


void SaveFiles::RelacsFiles::resetIndex( const EventList &EL )
{
  for ( unsigned int k=0; k<EventFiles.size(); k++ )
    EventFiles[k].Index = EL[k].size();
}


void SaveFiles::RelacsFiles::writeTraces( const InList &IL, bool stimulus )
{
  for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
    if ( TraceFiles[k].Stream != 0 ) {
      int n = IL[k].saveBinary( *TraceFiles[k].Stream, TraceFiles[k].Index );
      if ( n > 0 ) {
	TraceFiles[k].Written += n;
	TraceFiles[k].Index += n;
      }
      // there is a new stimulus:
      if ( stimulus && IL[k].signalIndex() >= 0 )
	TraceFiles[k].SignalOffset = IL[k].signalIndex() -
	  TraceFiles[k].Index + TraceFiles[k].Written;
    }
  }
}


void SaveFiles::RelacsFiles::writeEvents( const InList &IL, const EventList &EL, 
   				          bool stimulus )
{
  double offs = 0.0;
  if ( ! TraceFiles.empty() )
    offs = IL[0].interval( TraceFiles[0].Index - TraceFiles[0].Written );

  // save event data:
  for ( unsigned int k=0; k<EventFiles.size(); k++ ) {
    if ( EventFiles[k].Stream != 0 ) {
      // set index of last signal:
      if ( stimulus ) {
	double st = EL[k].signalTime();
	int index = EL[k].next( st );
	EventFiles[k].SignalEvent = index - EventFiles[k].Index + EventFiles[k].Written;
      }
      while ( EventFiles[k].Index < EL[k].size() ) {
	EventFiles[k].Key.save( *EventFiles[k].Stream, EL[k][EventFiles[k].Index] - offs, 0 );
	if ( EL[k].sizeBuffer() )
	  EventFiles[k].Key.save( *EventFiles[k].Stream,
				  EL[k].sizeScale() * EL[k].eventSize( EventFiles[k].Index ) );
	if ( EL[k].widthBuffer() )
	  EventFiles[k].Key.save( *EventFiles[k].Stream,
				  EL[k].widthScale() * EL[k].eventWidth( EventFiles[k].Index ) );
	*EventFiles[k].Stream << '\n';
	EventFiles[k].Written++;
	EventFiles[k].Index++;
      }
    }
  }
}


void SaveFiles::RelacsFiles::writeStimulus( const InList &IL, const EventList &EL, 
					    const deque< OutDataInfo > &stimuliinfo,
					    const deque< bool > &newstimuli,
					    const Options &data, 
					    const deque< Options > &stimuliref, 
					    int *stimulusindex,
					    double sessiontime,
					    const string &reproname,
					    const Acquire *acquire )
{
  if ( SF != 0 ) {
    // write stimulus description:
    if ( SDF != 0 ) {
      for ( unsigned int j=0; j<stimuliinfo.size(); j++ ) {
	if ( newstimuli[j] ) {
	  stimuliinfo[j].description().save( *SDF, "  " );
	  *SDF << endl;
	}
      }
    }
    // write entry in stimuli.dat:
    StimulusKey.resetSaveColumn();
    for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
      if ( TraceFiles[k].Stream != 0 )
	StimulusKey.save( *SF, TraceFiles[k].SignalOffset );
    }
    // events:
    for ( unsigned int k=0; k<EventFiles.size(); k++ ) {
      if ( EventFiles[k].Stream != 0 ) {
	StimulusKey.save( *SF, EventFiles[k].SignalEvent );
	if ( (EL[k].mode() & ( StimulusEventMode | RestartEventMode | RecordingEventMode ) ) == 0 )
	  StimulusKey.save( *SF, EL[k].meanRate() );  // XXX adaptive Time!
	if ( EL[k].sizeBuffer() )
	  StimulusKey.save( *SF, EL[k].sizeScale() * EL[k].meanSize() );
	if ( EL[k].widthBuffer() && (EL[k].mode() & StimulusEventMode) == 0 )
	  StimulusKey.save( *SF, EL[k].widthScale() * EL[k].meanWidth() );
	if ( EventFiles[k].SaveMeanQuality )
	  StimulusKey.save( *SF, 100.0*EL[k].meanQuality() );
      }
    }
    // data:
    if ( !data.empty() ) {
      for( int k=0; k<data.size(); k++ ) {
	StimulusKey.save( *SF, data[k] );
      }
    }
    StimulusKey.save( *SF, IL[0].signalTime() - sessiontime );
    // stimulus:
    StimulusKey.save( *SF, 1000.0*stimuliinfo[0].delay() );
    for ( int k=0; k<acquire->outTracesSize(); k++ ) {
      const Attenuate *att = acquire->outTraceAttenuate( k );
      int j = stimulusindex[k];
      if ( j >= 0 ) {
	StimulusKey.save( *SF, 0.001*stimuliinfo[j].sampleRate() );
	StimulusKey.save( *SF, 1000.0*stimuliinfo[j].length() ); // duration, resp. extent
	if ( att != 0 ) {
	  StimulusKey.save( *SF, stimuliinfo[j].intensity() );
	  if ( ! att->frequencyName().empty() )
	    StimulusKey.save( *SF, stimuliinfo[j].carrierFreq() );
	}
	StimulusKey.save( *SF, stimuliref[j].name() );
	const Options &paramopts = stimuliref[j].section( "parameter" );
	string mutablestring = paramopts.save( 0, Options::NoName + Options::FirstOnly );
	StimulusKey.save( *SF, '"' + mutablestring + '"' );
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
	StimulusKey.save( *SF, "" );
      }
    }
    *SF << endl;
  }
}


void SaveFiles::RelacsFiles::writeRePro( const Options &reproinfo, 
					 const deque< string > &reprofiles,
					 const InList &IL, const EventList &EL,
					 const Options &data, double sessiontime )
{
  // stimulus indices file:
  if ( SF != 0 ) {
    // save RePro info:
    *SF << "\n\n";
    reproinfo.save( *SF, "# ", 0, Options::FirstOnly );
    // save StimulusKey:
    *SF << '\n';
    StimulusKey.saveKey( *SF );
    StimulusKey.resetSaveColumn();
    // repro start indices:
    Options &traces = StimulusKey.subSection( "traces", 2 );
    int j = 0;
    for ( unsigned int k=0; k<TraceFiles.size(); k++ ) {
      if ( TraceFiles[k].Stream != 0 )
	traces.section(j++)[0].setInteger( TraceFiles[k].Written );
    }
    // events:
    Options &events = StimulusKey.subSection( "events", 2 );
    j = 0;
    for ( unsigned int k=0; k<EventFiles.size(); k++ ) {
      if ( EventFiles[k].Stream != 0 ) {
	Options &eventsec = events.section(j++);
	int i = 0;
	eventsec[i++].setInteger( EventFiles[k].Written );
	if ( (EL[k].mode() & ( StimulusEventMode | RestartEventMode | RecordingEventMode ) ) == 0 )
	  eventsec[i++].setNumber( EL[k].meanRate() );
	if ( EL[k].sizeBuffer() )
	  eventsec[i++].setNumber( EL[k].sizeScale() * EL[k].meanSize() );
	if ( EL[k].widthBuffer() && (EL[k].mode() & StimulusEventMode) == 0 )
	  eventsec[i++].setNumber( EL[k].widthScale() * EL[k].meanWidth() );
	if ( EventFiles[k].SaveMeanQuality )
	  eventsec[i++].setNumber( 100.0*EL[k].meanQuality() );
      }
    }
    // data:
    if ( !data.empty() ) {
      Options &sdata = StimulusKey.subSection( "data", 1 );
      for( int k=0; k<data.size(); k++ ) {
	sdata[k].setValue( data[k] );
      }
    }
    // stimuli:
    Options &stimuli = StimulusKey.subSection( "stimulus>timing", 2 );
    j = 0;
    stimuli.section(0)[j++].setNumber( IL[0].currentTime() - sessiontime );
    stimuli.section(0)[j++].setNumber( 0.0 );
    for ( int k=1; k<stimuli.sectionsSize(); k++ ) {
      for ( j=0; j<stimuli.section(k).parameterSize(); j++ ) {
	if ( stimuli.section(k)[j].isNumber() )
	  stimuli.section(k)[j].setNumber( NAN );
	else
	  stimuli.section(k)[j].setText( "" );
      }
    }
    // save line to stimuli.dat:
    StimulusKey.saveData( *SF );
    SF->flush();
  }
}


void SaveFiles::RelacsFiles::traceSignalIndices( deque<int> &traceindex )
{
  for ( unsigned int k=0; k<TraceFiles.size(); k++ )
    traceindex.push_back( TraceFiles[k].SignalOffset );
}


void SaveFiles::RelacsFiles::eventsSignalIndices( deque<int> &eventsindex )
{
  for ( unsigned int k=0; k<EventFiles.size(); k++ )
    eventsindex.push_back( EventFiles[k].SignalEvent );
}
  

SaveFiles::ODMLFiles::ODMLFiles( void )
{
  XF = 0;
  XSF = 0;
  DatasetOpen = false;
}


bool SaveFiles::ODMLFiles::open( const string &path, SaveFiles *save, 
				 const AllDevices *devices )
{
  DatasetOpen = false;

  // create xml file for all data:
  XF = save->openFile( "metadata.xml", ios::out );

  if ( (*XF) ) {
    string name = Str( path ).preventedSlash().name();
    *XF << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    *XF << "<?xml-stylesheet type=\"text/xsl\" href=\"odml.xsl\"  xmlns:odml=\"http://www.g-node.org/odml\"?>\n";
    *XF << "<odML version=\"1\">\n";
    *XF << "    <repository>http://portal.g-node.org/odml/terminologies/v1.0/terminologies.xml</repository>\n";
    *XF << "    <section>\n";
    *XF << "        <type>hardware</type>\n";
    *XF << "        <name>hardware-" << name << "</name>\n";
    for ( int k=0; k<devices->size(); k++ ) {
      const Device &dev = (*devices)[k];
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
      *XF << "        <section>\n";
      *XF << "            <type>" << dts << "</type>\n";
      *XF << "            <name>hardware-" << dts << "-" << name << "</name>\n";
      opts.saveXML( *XF, 0, Options::FirstOnly, 3 );
      *XF << "        </section>\n";
    }
    *XF << "    </section>\n";
  }

  // create xml file for stimulus metadata:
  XSF = save->openFile( "stimulus-metadata.xml", ios::out );
  if ( (*XSF) ) {
    *XSF << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    *XSF << "<?xml-stylesheet type=\"text/xsl\" href=\"odml.xsl\"  xmlns:odml=\"http://www.g-node.org/odml\"?>\n";
    *XSF << "<odML version=\"1\">\n";
    *XSF << "    <repository>http://portal.g-node.org/odml/terminologies/v1.0/terminologies.xml</repository>\n";
  }

  return ( (*XF) || (*XSF) );
}


void SaveFiles::ODMLFiles::close( const string &path, const deque< string > &reprofiles,
				    MetaData &metadata )
{
  if ( XF != 0 ) {
    if ( DatasetOpen ) {
      if ( reprofiles.size() > 0 ) {
	string files = reprofiles[0];
	for ( unsigned int k=1; k<reprofiles.size(); k++ )
	  files += ", " + reprofiles[k];
	Parameter p( "File", "", files );
	p.saveXML( *XF, 2, 4 );
      }
      *XF << "    </section>\n";
      DatasetOpen = false;
    }
    string name = Str( path ).preventedSlash().name();
    metadata.saveXML( *XF, 1, name );
    *XF << "</odML>\n";
    delete XF;
  }
  XF = 0;
  if ( XSF != 0 ) {
    *XSF << "</odML>\n";
    delete XSF;
  }
  XSF = 0;
}


void SaveFiles::ODMLFiles::writeStimulus( const InList &IL, const EventList &EL, 
					    const deque< OutDataInfo > &stimuliinfo,
					    const deque< bool > &newstimuli,
					    const Options &data, 
					    const deque< Options > &stimuliref, 
					    int *stimulusindex,
					    double sessiontime,
					    const string &reproname,
					    const Acquire *acquire )
{
  if ( XF != 0 ) {

    // stimulus description:
    if ( XSF != 0 ) {
      for ( unsigned int j=0; j<stimuliinfo.size(); j++ ) {
	if ( newstimuli[j] )
	  stimuliinfo[j].description().saveXML( *XSF, 0, Options::FirstOnly, 1 );
      }
    }

    // stimulus reference:
    Options sopt( reproname, "stimulus", 0, 0 );
    sopt.append( data );
    for ( int k=0; k<acquire->outTracesSize(); k++ ) {
      int j = stimulusindex[k];
      if ( j >= 0 ) {
	Options &ref = sopt.newSection( stimuliref[j] );
	ref.setInclude( "stimulus-metadata.xml", stimuliref[j].name() );
      }
    }
    sopt.saveXML( *XF, 0, Options::FirstOnly, 2 );
  }
}


void SaveFiles::ODMLFiles::writeRePro( const Options &reproinfo, 
					 const deque< string > &reprofiles,
					 const InList &IL, const EventList &EL,
					 const Options &data, double sessiontime )
{
  if ( XF != 0 ) {
    if ( DatasetOpen ) {
      if ( reprofiles.size() > 0 ) {
	string files = reprofiles[0];
	for ( unsigned int k=1; k<reprofiles.size(); k++ )
	  files += ", " + reprofiles[k];
	Parameter p( "File", "", files );
	p.saveXML( *XF, 2, 4 );
      }
      *XF << "    </section>\n";
    }
    reproinfo.saveXML( *XF, 0, Options::FirstOnly | Options::DontCloseSection, 1 );
    DatasetOpen = true;
  }
}


#ifdef HAVE_NIX
string SaveFiles::NixFile::create ( string path, bool compression )
{
  // TODO: path can be a directory (with trailing slash) or a stem of a filename!
  rid = Str( path ).preventedSlash().name();
  string nix_path = path + rid + ".nix";
  nix::Compression compr = compression ? nix::Compression::DeflateNormal : nix::Compression::None;
  fd = nix::File::open( nix_path, nix::FileMode::Overwrite, "hdf5", compr );
  root_block = fd.createBlock(rid, "recording");
  root_section = fd.createSection(rid, "recording");
  root_block.metadata( root_section );
  return nix_path;
}


void SaveFiles::NixFile::close ( )
{
  if ( fd.isOpen() ) {
    std::cerr << "Closing NIX File" << std::endl;
    if ( repro_tag && repro_tag.extent().size() == 0 ) {
      endRePro(traces[0].written * stepsize);
    }
    repro_start_time = 0.0;
    stimulus_start_time = 0.0;
    stimulus_duration = 0.0;
    was_writing = false;
    root_block = nix::none;
    root_section = nix::none;
    repro_tag = nix::none;
    stimulus_tag = nix::none;
    stimulus_positions = nix::none;
    stimulus_extents = nix::none;
    time_feat = nix::none;
    delay_feat = nix::none;
    amplitude_feat = nix::none;
    carrier_feat = nix::none;
    data_features.clear();
    traces.clear();
    events.clear();
    fd.close();
  }
}


static void saveNIXParameter(const Parameter &param, nix::Section &section, Options::SaveFlags flags)
{
  vector<nix::Value> values;
  bool first_only = (flags & Options::FirstOnly) > 0;
  for ( int i = 0;  i < (first_only ? 1 : param.size()); i++ ) {
    nix::Value val;
    if ( param.isNumber () || param.isInteger() ) {
      if ( param.isInteger () ) {
        val = nix::Value ( static_cast<int64_t>( param.number (i) ) );
      }
      else {
        val = nix::Value ( param.number ( i ) );
      }
      val.uncertainty = param.error( i );
    }
    else if ( param.isDate() ) {
      val = nix::Value( param.text( i, "%04Y-%02m-%02d" ) );
    }
    else if ( param.isTime() ) {
      val = nix::Value (param.text( i, "%02H:%02M:%02S" ));
    }
    else if ( param.isText() ) {
      val = nix::Value (param.text( i ));
    }
    else if ( param.isBoolean() ) {
      val = nix::Value( param.boolean( i ) );
    }
    if ( val.type() != nix::DataType::Nothing ) {
      values.push_back( std::move( val ) );
    }
    if ( param.size() > 1 &&
	 ( (flags & Options::FirstOnly) != 0 &&
	   !((param.style() & Parameter::ListAlways) == 0))) {
      break;
    }
  }
  if ( values.empty() ) {
    //property with no values, so we woudn't know the type
    //just don't save anything
    return;
  }

  string unit = nix::util::unitSanitizer( param.unit() );
  if ( unit == "°C" || unit == "C" || unit == "F" || unit == "°F" ) {
    std::for_each( values.begin(), values.end(), [&unit](nix::Value &v) {
        switch (v.type()) {
        case nix::DataType::Int64:
	  v = nix::Value(nix::util::convertToKelvin(unit, v.get<int64_t>()));
	  unit = "K";
	  break;
        case nix::DataType::Double:
	  v = nix::Value(nix::util::convertToKelvin(unit, v.get<double>()));
	  unit = "K";
          break;
        default:
          std::cerr << "[nix] Warning: temperature value but unkown type" << std::endl;
          unit = "";
        }
      });
  } else if ( unit == "min" || unit == "sec" || unit == "h" || unit == "seconds" ) {
    std::for_each( values.begin(), values.end(), [&unit](nix::Value &v) {
	if (unit == "seconds")
	  unit = "sec";
        switch (v.type()) {
        case nix::DataType::Int64:
          v = nix::Value(nix::util::convertToSeconds(unit, v.get<int64_t>()));
	  unit = "s";
          break;
        case nix::DataType::Double:
	  v = nix::Value(nix::util::convertToSeconds(unit, v.get<double>()));
	  unit = "s";
          break;
        default:
          std::cerr << "[nix] Warning: min value but unkown type" << std::endl;
          unit = "";
        }
      });
  } else if ( unit == "1" ) {
    unit = "";
  }
  if (section.hasProperty(param.name())) {
    std::cerr << "NIX Warning! Failed to store property " << param.name()
              << " A property with that name already exists!" << std::endl;
    return;
  }
  nix::Property prop = section.createProperty ( param.name(), values );
  if ( !unit.empty() && ( nix::util::isSIUnit( unit ) ||
	 nix::util::isCompoundSIUnit( unit ) ) ) {
    prop.unit ( unit );
  } else if ( !unit.empty() ) {
    std::cerr << "\t [nix] Warning: " << unit << " is no SI unit, not setting it!!!" << std::endl;
  }
  if ((param.flags() & OutData::Mutable) > 0) {
    prop.definition( "parameter is mutable, per trial data is stored as feature of stimulus tag!" );
  }
}


static void saveNIXOptions(const Options &opts, nix::Section section,
                           Options::SaveFlags flags, int selectmask)
{
  using OFlags = Options::SaveFlags;
  string ns = opts.name();
  string ts = opts.type();
  if ( ( flags & Options::SaveFlags::SwitchNameType ) ) {
    std::swap(ts, ns);
  }
  // This is a hack to fill in a valid name ... type-only
  // sections in the options are invalid! e.g. (stimulus/sinwave)
  // should be fixed somewhere else...
  if ( ns.empty() && !ts.empty() ) {
    ns = ts;
    std::vector<nix::Section> secs = section.sections( nix::util::NameFilter<nix::Section>( nix::util::nameSanitizer( ns ) ));
    if (secs.size() > 0)
      ns = ns + "_" + nix::util::numToStr(secs.size());
  }
  bool have_name = ( ( ! ns.empty() ) && ( ( flags & OFlags::NoName ) == 0 ) );
  bool have_type = ( ( ! ts.empty() ) && ( ( flags & OFlags::NoType ) == 0 ) );
  bool mk_section = ( opts.flag( selectmask ) && ( have_name && have_type ) );
  mk_section = mk_section && ( section.name() != ns || section.type() != ts );

  if ( mk_section ) {
    section = section.createSection ( nix::util::nameSanitizer(ns),
				      nix::util::nameSanitizer(ts) );
  }
  //save parameter
  for ( auto pp = opts.begin(); pp != opts.end(); ++pp ) {
    if ( pp->flags( selectmask ) ) {
      saveNIXParameter(*pp, section, flags);
      //std::cerr << "\t" << *pp << "\t is mutable: " << ((pp->flags() & OutData::Mutable) != 0 ) << std::endl;
    }
  }
  //now the subsections
  for ( auto sp = opts.sectionsBegin(); sp != opts.sectionsEnd(); ++sp ) {
    if ( (*sp)->flag( selectmask ) ) {
      const Options *cur_opt = *sp;
      saveNIXOptions(*cur_opt, section, flags, selectmask);
    }
  }
}


void SaveFiles::NixFile::saveMetadata (const AllDevices *devices)
{
  string fnname = rid;
  nix::Section hw = root_section.createSection("hardware-" + rid, "hardware");
  for ( int k=0; k < devices->size(); k++ ) {
    const Device &dev = (*devices)[k];
    int dt = dev.deviceType();
    if ( dt ==  Device::AttenuateType ) {
      continue;
    }
    string dts;
    switch(dt) {
    case Device::AnalogInputType:
      dts = "hardware/daq/analog_input";
      break;
    case Device::AnalogOutputType:
      dts = "hardware/daq/analog_output";
      break;
    case Device::DigitalIOType:
      dts = "DigitalIO";
      break;
    default:
      dts = dev.deviceTypeStr();
    }
    string sec_name = "hardware-" + dts + "-" + fnname;
    while (sec_name.find("/") != sec_name.npos) {
      sec_name.replace(sec_name.find("/"), 1, "_");
    }
    nix::Section s = hw.createSection(sec_name, dts);
    Options opts( dev.info() );
    opts.erase( "type" );
    saveNIXOptions(opts, s, Options::FirstOnly, 0);
  }
}


void SaveFiles::NixFile::saveMetadata (const MetaData &mtdt)
{
  Options::SaveFlags flags = static_cast<Options::SaveFlags>(Options::SwitchNameType | Options::FirstOnly);
  saveNIXOptions( mtdt, root_section, flags, mtdt.saveFlags() );
}


void SaveFiles::NixFile::writeRePro ( const Options &reproinfo, const deque< string > &reprofiles,
				      const InList &IL, const EventList &EL, const Options &data,
				      double sessiontime )
{
  stepsize =  IL[0].stepsize();
  repro_start_time = traces[0].written * stepsize;
  string repro_name = reproinfo["RePro"].text() + "_" + nix::util::numToStr(reproinfo["Run"].number());
  was_writing = true;
  currentRePro = repro_name;
  if ( !fd || traces.size() < 1 ) {
    return;
  }
  nix::Section s = fd.createSection( repro_name, "relacs.repro" );
  saveNIXOptions( reproinfo, s, Options::FirstOnly, 0 );
  if ( reprofiles.size() > 0 ) {
    std::vector<nix::Value> values;
    for (const std::string &s : reprofiles) {
      nix::Value val = nix::Value( s );
      values.push_back( std::move( val ) );
    }
    s.createProperty( "reprofiles", values );
  }
  // store data
  repro_tag = root_block.createTag( repro_name, "relacs.repro_run", {repro_start_time});
  repro_tag.units({"s"});
  repro_tag.metadata( s );
  for ( auto &trace : traces ) {
    repro_tag.addReference( trace.data );
  }
  for ( auto &event : events ) {
    if ( event.input_trace < 0 ) {
      continue;
    }
    repro_tag.addReference( event.data );
  }
}

void SaveFiles::NixFile::endRePro( double current_time )
{
  repro_tag.extent({ (current_time - repro_start_time)});
  // TODO maybe end the stimulus as well?
  if ( stimulus_tag && (stimulus_start_time + stimulus_duration) > current_time )
     std::cerr << "premature stop of stimulus presentation need to fix the stimulus tag!" << std::endl;
  fd.flush();
}

void SaveFiles::NixFile::initTraces ( const InList &IL )
{
  for ( int k=0; k<IL.size(); k++ ) {
    // std::cerr << "Ident: " << IL[k].ident() << std::endl;
    // std::cerr << "Device: " << IL[k].device() << std::endl;
    // std::cerr << "Channel: " << IL[k].channel() << std::endl;
    // std::cerr << "Unit: " << IL[k].unit() << std::endl;
    // std::cerr << "Scale: " << IL[k].scale() << std::endl;
    // std::cerr << "SampleRate: " << IL[k].sampleRate() << std::endl;
    NixTrace trace;
    string data_type = "nix.data.sampled." + IL[k].ident();
    trace.data = root_block.createDataArray(IL[k].ident(), data_type, nix::DataType::Float, {4096});
    std::string unit = IL[k].unit();
    nix::util::unitSanitizer(unit);
    if ( !unit.empty() && nix::util::isSIUnit(unit) ) {
      trace.data.unit( unit );
    } else if ( !unit.empty() ) {
      std::cerr << "NIX output Warning: Given unit " << unit << " is no valid SI unit, not saving it!" << std::endl;
    }
    if ( !IL[k].ident().empty() )
      trace.data.label(IL[k].ident() );
    nix::SampledDimension dim;
    dim = trace.data.appendSampledDimension(IL[k].sampleInterval());
    dim.unit("s");
    dim.label("time");
    trace.index = IL[k].size();
    trace.written = 0;
    trace.offset = {0};
    //^ NB:size() is the all-time number of bytes written
    //    string source_name = "device-" + nix::util::num2str(IL[k].device());
    //nix::Source s = root_block.createSource();
    traces.push_back(std::move(trace));
  }
}


void SaveFiles::NixFile::writeChunk(NixTrace   &trace,
                                    size_t      to_read,
                                    const void *data)
{
  typedef nix::NDSize::value_type value_type;
  nix::NDSize count = { static_cast<value_type>(to_read) };
  nix::NDSize size = trace.offset + count;
  trace.data.dataExtent( size );
  trace.data.setData( nix::DataType::Float, data, count, trace.offset );
  trace.index += to_read;
  trace.written += to_read;
  trace.offset = size;
}



void SaveFiles::NixFile::createStimulusTag( const std::string &repro_name, const Options &stim_options,
                                            const Options &stimulus_features, const deque< OutDataInfo > &stim_info,
                                            const Acquire *AQ, double start_time, double duration ) {
  nix::Section s;
  if ( repro_name.size() > 0 ) {
    string stim_name = stim_options.name();
    string stim_type = stim_options.type();
    s = fd.createSection( stim_name, stim_type );
    saveNIXOptions( stim_options, s, Options::FirstOnly, 0 );
  }
  stimulus_positions = root_block.createDataArray( repro_name + " onset times", "nix.event.positions",
                                                   nix::DataType::Double, {1} );

  stimulus_positions.setData( nix::DataType::Double, &start_time, {1}, {0} );
  stimulus_positions.appendSetDimension();
  stimulus_positions.unit( "s" );
  stimulus_positions.label( "time" );

  stimulus_extents = root_block.createDataArray( repro_name + " durations", "nix.event.extents",
                                                 nix::DataType::Double, {1} );
  stimulus_extents.setData( nix::DataType::Double, &duration, {1}, {0} );
  stimulus_extents.appendSetDimension();
  stimulus_extents.unit( "s" );
  stimulus_extents.label( "time" );

  stimulus_tag = root_block.createMultiTag( repro_name, "nix.event.stimulus", stimulus_positions );
  stimulus_tag.extents( stimulus_extents );
  stimulus_tag.metadata( s );
  for ( auto &trace : traces ) {
    stimulus_tag.addReference( trace.data );
  }
  for ( auto &event : events ) {
    if ( event.input_trace < 0 ) {
      continue;
    }
    stimulus_tag.addReference( event.data );
  }
  // add features
  std::string fname;
  std::string funit;
  std::string flabel;
  std::string ftype;
  data_features.clear();
  if ( !stimulus_features.empty() ) {
    for (auto o : stimulus_features) {
        fname = stimulus_tag.name() + "_" + o.name();
        funit = o.unit();
        flabel = o.name();
        ftype = "feature";
        nix::DataType dtype;
        if ( o.isNumber() ) {
          dtype = nix::DataType::Double;
        } else if ( o.isText() )  {
          dtype = nix::DataType::String;
        } else {
          continue;
        }
        nix::DataArray da = createFeature( root_block, stimulus_tag, fname, ftype,
                                           funit, flabel, nix::LinkType::Indexed, dtype );
        data_features.push_back(da);
    }
  }
  fname =  stimulus_tag.name() + "_abs_time";
  funit = "s";
  flabel = "time";
  ftype = "nix.time";
  time_feat = createFeature( root_block, stimulus_tag, fname, ftype, funit, flabel, nix::LinkType::Indexed,  nix::DataType::Double );
  fname =  stimulus_tag.name() + "_delay";
  flabel = "delay";
  delay_feat = createFeature( root_block, stimulus_tag, fname, ftype, funit, flabel, nix::LinkType::Indexed, nix::DataType::Double );
  std::string unit = "";
  for ( int k=0; k < AQ->outTracesSize(); k++ ) {
    if (stim_info[0].device() == AQ->outTrace(k).device() &&
        stim_info[0].channel() == AQ->outTrace(k).channel()) {
      const Attenuate *att = AQ->outTraceAttenuate( k );
      if ( att != 0 ) {
        unit = att->intensityUnit();
      } else {
        unit = AQ->outTrace(k).unit();
      }
    }
  }
  amplitude_feat = createFeature(root_block, stimulus_tag, stimulus_tag.name() + "_amplitude",
                                 "nix.time", unit, "intensity", nix::LinkType::Indexed, nix::DataType::Double);
  for (Parameter p : stim_options) {
    if ((p.flags() & OutData::Mutable) > 0) {
      fname =  stimulus_tag.name() + "_" + p.name();
      funit = p.unit();
      nix::util::unitSanitizer(funit);
      flabel = p.name();
      ftype = "relacs.mutable";
      nix::DataType dtype;
      if ( p.isNumber() ) {
        dtype = nix::DataType::Double;
      } else if ( p.isText() ) {
        dtype = nix::DataType::String;
      } else {
        continue;
      }
      createFeature(root_block, stimulus_tag, fname, ftype, funit, flabel, nix::LinkType::Indexed, dtype);
    }
  }
}


void SaveFiles::NixFile::writeStimulus( const InList &IL, const EventList &EL,
                    const deque< OutDataInfo > &stim_info,
                    const deque< bool > &newstimuli, const Options &stim_options,
                    const deque< Options > &stimuliref, int *stimulusindex,
                    double sessiontime, const string &reproname, const Acquire *acquire )
{
  if ( !fd )
    return;
  if ( IL[0].signalIndex() < 1 ) {
    return;
  }

  double abs_time = IL[0].signalTime() - sessiontime;
  double delay = stim_info[0].delay();
  double intensity = stim_info[0].intensity();

  NixTrace trace = traces[0];
  stimulus_start_time = (IL[0].signalIndex() - trace.index  + trace.written) * stepsize;
  stimulus_duration = stim_info[0].length();
  string tag_name = nix::util::nameSanitizer(stim_info[0].description().name());

  if ( stimulus_tag && stimulus_tag.name() != tag_name ) {
    stimulus_tag = root_block.getMultiTag(tag_name);
  }
  if ( stimulus_tag && stimulus_tag.name() == tag_name ) {
    stimulus_positions = stimulus_tag.positions();
    stimulus_extents = stimulus_tag.extents();
    data_features.clear();
    std::vector<nix::Feature> feats = stimulus_tag.features();
    for (nix::Feature f : feats) { // this can be switched to getting the feature by name after the next nix release is out...
      if ( f.data().name() == tag_name + "_abs_time")
        time_feat = f.data();
      else if (f.data().name() == tag_name + "_amplitude" )
        amplitude_feat = f.data();
      else if (f.data().name() == tag_name + "_delay" )
        delay_feat = f.data();
      else {
        for (auto o : stim_options) {
          if ( f.data().name() ==  tag_name + "_" + o.name()) {
            data_features.push_back(f.data());
          }
        }
      }
    }
    appendValue(stimulus_positions, stimulus_start_time);
    appendValue(stimulus_extents, stimulus_duration);
  }
  else { // There is no such tag, we need to create a new one
    createStimulusTag(tag_name, stim_info[0].description(), stim_options, stim_info, acquire, stimulus_start_time, stimulus_duration);
  }
  for ( auto o : stim_options ) { //TODO check if this can be simplified
    for ( auto da : data_features ) {
      if ( da.name() ==  tag_name + "_" + o.name()) {
        if ( o.isNumber() ) {
          double val = o.number();
          appendValue(da, val);
        } else if ( o.isText() ) {
          string val = o.text();
          appendValue(da, val);
        }
      }
    }
  }
  Options mutables = stimuliref[0].section( "parameter" );
  for (auto p : mutables) {
    if ( p.isNumber() ) {
      double val = p.number();
      nix::DataArray da =  root_block.getDataArray( tag_name + "_" + p.name() );

      appendValue( da, val );
    } else if ( p.isText() ) {
      string val = p.text();
      nix::DataArray da =  root_block.getDataArray( tag_name + "_" + p.name() );
      appendValue( da, val );
    }
  }

  appendValue( time_feat, abs_time);
  appendValue( delay_feat, delay);
  appendValue( amplitude_feat, intensity );

  fd.flush();
}


nix::DataArray SaveFiles::NixFile::createFeature( nix::Block &block,
						  nix::MultiTag &mtag,
						  std::string name, std::string type,
						  std::string unit, std::string label,
						  nix::LinkType link_type, nix::DataType dtype ) {
  nix::DataArray da = block.createDataArray(name, type, dtype, {0});
  da.appendSetDimension();
  da.label(label);
  nix::util::unitSanitizer(unit);
  if ( !unit.empty() && nix::util::isSIUnit(unit)) {
    da.unit(unit);
  } else if ( !unit.empty() ) {
    std::cerr << "NIX output Warning: Given unit " << unit << " is no valid SI unit, not saving it!" << std::endl;
  }
  mtag.createFeature( da, nix::LinkType::Indexed);
  return da;
}


void SaveFiles::NixFile::appendValue( nix::DataArray &array, double value ) {
  if ( !array )
    return;
  nix::NDSize size = array.dataExtent();
  array.dataExtent( size + 1 );
  array.setData( nix::DataType::Double, &value, {1}, size );
}


void SaveFiles::NixFile::appendValue( nix::DataArray &array, string value ) {
  if ( !array )
    return;
  nix::NDSize size = array.dataExtent();
  array.dataExtent( size + 1 );
  array.setData( value, size );
}


void SaveFiles::NixFile::writeTraces( const InList &IL ) {
  if ( ! fd )
    return;

  for ( int k=0; k < IL.size(); k++ ) {
    NixTrace &trace = traces[k];
    if ( trace.index >= static_cast<size_t>( IL[k].size() ) ) {
      continue;       //Nothing to write
    }

    int ndata = IL[k].size() - trace.index;
    int to_read = 0;
    const float *data = IL[k].readBuffer( trace.index, to_read );
    if ( to_read > 0 ) {
      writeChunk( trace, static_cast<size_t>(to_read), data );
      if ( to_read < ndata ) {
	data = IL[k].readBuffer( trace.index, to_read );
	if ( to_read > 0 )
	  writeChunk( trace, static_cast<size_t>(to_read), data );
      }
    }
  }
}


void SaveFiles::NixFile::initEvents( const EventList &EL, FilterDetectors *FD ) {
  for ( int i = 0; i < EL.size(); i++ ) {
    if ( (EL[i].mode() & SaveTrace) == 0 ) {
      continue;      //Nothing to save
    }
    NixEventData ed;
    ed.el_index = i;
    ed.index = EL[i].size();
    ed.offset = {0};
    std::string ident = EL[i].ident();
    std::string data_type = "nix.events.position." + ident;
    if ( root_block.hasDataArray(ident) )
       ident = EL[i].ident() + "_events";
    ed.data = root_block.createDataArray( ident, data_type, nix::DataType::Double, {256} );
    ed.data.unit( "s" );
    ed.data.label( "time" );
    ed.data.appendAliasRangeDimension();
    ed.input_trace =  FD->eventInputTrace( i );
    events.push_back(std::move(ed));
  }
}


void SaveFiles::NixFile::writeEvents( const InList &IL, const EventList &EL ) {
  if ( ! fd )
    return;
  double off = 0.0;
  if ( ! traces.empty() )
    off = IL[0].interval( traces[0].index - traces[0].written );

  //  double st = EL[0].size() > 0 ? EL[0].back() : EL[0].rangeBack();
  for(auto &ed : events ) {
    int k = ed.el_index;
    int len =  EL[k].size() - ed.index;
    if ( len < 1 || !ed.data ) {
      continue;
    }
    std::vector<double> ets ( len );
    for( int i = 0; i < len; i++ ) {
      int idx = ed.index + i;
      ets[i] = EL[k][idx] - off;
    }
    ed.index += len;
    typedef nix::NDSize::value_type ndsize_type;
    nix::NDSize count = { static_cast<ndsize_type>(len) };
    nix::NDSize size = ed.offset + count;
    ed.data.dataExtent( size  );
    ed.data.setData( nix::DataType::Double, ets.data(), count, ed.offset);
    ed.offset = size;
  }
}


void SaveFiles::NixFile::resetIndex( const InList &IL )
{
  for ( int k=0; k < IL.size(); k++ ) {
    traces[k].index = IL[k].size();
  }
}


void SaveFiles::NixFile::resetIndex( const EventList &EL )
{
  for ( auto &ed : events ) {
    ed.index = EL[ed.el_index].size();
  }
}
#endif


}; /* namespace relacs */

#include "moc_savefiles.cc"

