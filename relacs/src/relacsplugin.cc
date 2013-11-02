/*
  relacsplugin.cc
  Adds specific functions for RELACS plugins to ConfigDialog

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

#include <QDesktopWidget>
#include <QDateTime>
#include <relacs/str.h>
#include <relacs/messagebox.h>
#include <relacs/metadata.h>
#include <relacs/model.h>
#include <relacs/acquire.h>
#include <relacs/control.h>
#include <relacs/controltabs.h>
#include <relacs/filter.h>
#include <relacs/filterdetectors.h>
#include <relacs/relacsdevices.h>
#include <relacs/repros.h>
#include <relacs/savefiles.h>
#include <relacs/session.h>
#include <relacs/relacswidget.h>
#include <relacs/relacsplugin.h>

namespace relacs {


const TraceSpec RELACSPlugin::DummyTrace;


RELACSPlugin::RELACSPlugin( const string &configident, int configgroup,
			    const string &name, const string &pluginset,
			    const string &author, 
			    const string &version, const string &date )
  : ConfigDialog( configident, configgroup,
		  name, author, version, date )
{
  setPluginSet( pluginset );
  UniqueName = name;
  Widget = 0;
  RW = 0;
  GlobalKeyEvents = false;
  addDialogStyle( OptWidget::BoldSectionsStyle );
}


const int RELACSPlugin::Core = 0;
const int RELACSPlugin::Plugins = 1;


RELACSPlugin::~RELACSPlugin( void )
{
  if ( Widget != 0 )
    delete Widget;
}


QWidget *RELACSPlugin::widget( void )
{
  return Widget;
}


void RELACSPlugin::setWidget( QWidget *widget )
{
  Widget = widget;
  connect( Widget, SIGNAL( destroyed( QObject* ) ),
	   this, SLOT( widgetDestroyed( QObject* ) ) );
  Widget->installEventFilter( this );
}


void RELACSPlugin::setLayout( QLayout *layout )
{
  if ( Widget == 0 ) {
    setWidget( new QWidget );
    Widget->setLayout( layout );
    Widget->setAutoFillBackground( true );
  }
  else {
    if ( Widget->layout() != 0 )
      delete Widget->layout();
    Widget->setLayout( layout );
  }
}


int RELACSPlugin::desktopWidth( void ) const
{
  if ( Widget != 0 )
    return QApplication::desktop()->availableGeometry( Widget ).width();
  else
    return QApplication::desktop()->availableGeometry().width();
}


int RELACSPlugin::desktopHeight( void ) const
{
  if ( Widget != 0 )
    return QApplication::desktop()->availableGeometry( Widget ).height();
  else
    return QApplication::desktop()->availableGeometry().height();
}


void RELACSPlugin::setName( const string &name )
{
  ConfigDialog::setName( name );
  updateUniqueName();
}


string RELACSPlugin::pluginSet( void ) const
{
  return PluginSet;
}


void RELACSPlugin::setPluginSet( const string &pluginset )
{
  PluginSet = pluginset;
}


string RELACSPlugin::uniqueName( void ) const
{
  return UniqueName;
}


void RELACSPlugin::setShortUniqueName( void )
{
  UniqueName = name();
}


void RELACSPlugin::setLongUniqueName( void )
{
  // this must be the same construct as in the Plugins::addPlugin macro!
  UniqueName = name() + "[" + pluginSet() + "]";
}


void RELACSPlugin::updateUniqueName( void )
{
  if ( UniqueName.find( '[' ) == string::npos )
    setShortUniqueName();
  else
    setLongUniqueName();
}


const Options &RELACSPlugin::settings( void ) const
{
  return Settings;
}


Options &RELACSPlugin::settings( void )
{
  return Settings;
}


void RELACSPlugin::setSettings( void )
{
  Settings = *this;
  Settings.setName( "Settings", "" );
}


string RELACSPlugin::helpFileName( void ) const
{
  return pluginSet() + '-' + name() + ".html";
}


void RELACSPlugin::printlog( const string &s ) const
{
  if ( RW == 0 )
    cerr << QTime::currentTime().toString().toStdString() << " "
	 << uniqueName() << ": " << s << endl;
  else
    RW->printlog( uniqueName() + ": " + s );
}


void RELACSPlugin::warning( const string &s, double timeout )
{
  Str ws = s;
  ws.eraseMarkup();
  printlog( "! warning: " + ws );

  WarningStr = s;
  WarningTimeout = timeout;
  postCustomEvent( 3 );
}


void RELACSPlugin::info( const string &s, double timeout )
{
  Str ws = s;
  ws.eraseMarkup();
  printlog( ws );

  InfoStr = s;
  InfoTimeout = timeout;
  postCustomEvent( 4 );
}


void RELACSPlugin::postCustomEvent( int type )
{
  QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+type ) ) );
}


void RELACSPlugin::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 3: {
    string ss = "RELACS: ";
    ss += uniqueName();
    MessageBox::warning( ss, WarningStr, WarningTimeout, RW );
    break;
  }
  case 4: {
    string ss = "RELACS: ";
    ss += uniqueName();
    MessageBox::information( ss, InfoStr, InfoTimeout, RW );
    break;
  }
  case 5: {
    if ( RW->DV != 0 )
      RW->DV->updateMenu();
    break;
  }
  case 6: {
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
    break;
  }
  case 7: {
    QApplication::restoreOverrideCursor();
    break;
  }
  default:
    ConfigDialog::customEvent( qce );
  }
}


void RELACSPlugin::widgetDestroyed( QObject *obj )
{
  Widget = 0;
}


void RELACSPlugin::lockAll( void )
{
  lock();
  lockMetaData();
  lockStimulusData();
}


void RELACSPlugin::unlockAll( void )
{
  unlockStimulusData();
  unlockMetaData();
  unlock();
}


void RELACSPlugin::readLockData( void )
{
  if ( RW != 0 )
    RW->readLockData();
}


void RELACSPlugin::unlockData( void )
{
  if ( RW != 0 )
    RW->unlockData(); 
}


QReadWriteLock &RELACSPlugin::dataMutex( void )
{
  return RW->dataMutex(); 
}


void RELACSPlugin::assignTraces( const InList *il )
{
  IL.assign( il );
}


void RELACSPlugin::assignTraces( void )
{
  IL.assign();
}


void RELACSPlugin::updateTraces( void )
{
  IL.update();
}


void RELACSPlugin::updateData( void )
{
  // get new data:
  if ( ! RW->updateData() ) {
    QMutex mutex;
    mutex.lock();
    RW->UpdateDataWait.wait( &mutex, 100 );
    mutex.unlock();
  }
  // make them available:
  readLockData();
  updateTraces();
  unlockData();
}


const InList &RELACSPlugin::traces( void ) const
{
  return IL;
}


const InData &RELACSPlugin::trace( int index ) const
{
  return IL[index];
}


const InData &RELACSPlugin::trace( const string &ident ) const
{
  return IL[ident];
}


int RELACSPlugin::traceIndex( const string &ident ) const
{
  return IL.index( ident );
}


string RELACSPlugin::traceNames( void ) const
{
  string its = "";
  for ( int k=0; k<IL.size(); k++ ) {
    if ( k > 0 )
      its += '|';
    its += IL[k].ident();
  }
  return its;
}


const EventList &RELACSPlugin::events( void ) const
{
  return RW->ED;
}


const EventData &RELACSPlugin::events( int index ) const
{
  return RW->ED[index];
}


const EventData &RELACSPlugin::events( const string &ident ) const
{
  return RW->ED[ident];
}


int RELACSPlugin::traceInputTrace( int trace ) const
{
  return RW->FD->traceInputTrace( trace );
}


int RELACSPlugin::traceInputTrace( const string &ident ) const
{
  return RW->FD->traceInputTrace( ident );
}


int RELACSPlugin::traceInputEvent( int trace ) const
{
  return RW->FD->traceInputEvent( trace );
}


int RELACSPlugin::traceInputEvent( const string &ident ) const
{
  return RW->FD->traceInputEvent( ident );
}


int RELACSPlugin::eventInputTrace( int event ) const
{
  return RW->FD->eventInputTrace( event );
}


int RELACSPlugin::eventInputTrace( const string &ident ) const
{
  return RW->FD->eventInputTrace( ident );
}


int RELACSPlugin::eventInputEvent( int event ) const
{
  return RW->FD->eventInputEvent( event );
}


int RELACSPlugin::eventInputEvent( const string &ident ) const
{
  return RW->FD->eventInputEvent( ident );
}


double RELACSPlugin::signalTime( void ) const
{
  return RW->SignalTime;
}


double RELACSPlugin::currentTime( void ) const
{
  return IL.currentTime();
}


void RELACSPlugin::setGain( const InData &data, int gainindex )
{
  if ( RW->AQ != 0 )
    RW->AQ->setGain( data, gainindex );
}


void RELACSPlugin::adjustGain( const InData &data, double maxvalue )
{
  if ( RW->AQ != 0 )
    RW->AQ->adjustGain( data, maxvalue );
}


void RELACSPlugin::adjustGain( const InData &data, double minvalue,
			       double maxvalue )
{
  if ( RW->AQ != 0 )
    RW->AQ->adjustGain( data, minvalue, maxvalue );
}


void RELACSPlugin::adjust( const InData &data, double tbegin, double tend,
			   double threshold )
{
  double max = data.maxAbs( tbegin, tend );
  adjustGain( data, max / threshold / threshold, max / threshold );
}


void RELACSPlugin::adjust( const InData &data, double duration,
			   double threshold )
{ 
  adjust( data, currentTime() - duration, currentTime(),
	  threshold ); 
}


void RELACSPlugin::activateGains( bool datalocked )
{
  RW->activateGains( datalocked );
}


int RELACSPlugin::outTracesSize( void ) const
{
  if ( RW->AQ == 0 )
    return 0;
  return RW->AQ->outTracesSize();
}


int RELACSPlugin::outTraceIndex( const string &name ) const
{
  if ( RW->AQ == 0 )
    return -1;
  return RW->AQ->outTraceIndex( name );
}


string RELACSPlugin::outTraceName( int index ) const
{
  if ( RW->AQ == 0 )
    return "";
  return RW->AQ->outTraceName( index );
}


string RELACSPlugin::outTraceNames( void ) const
{
  if ( RW->AQ == 0 )
    return "";
  string ots = "";
  for ( int k=0; k<RW->AQ->outTracesSize(); k++ ) {
    if ( k > 0 )
      ots += '|';
    ots += RW->AQ->outTraceName( k );
  }
  return ots;
}


const TraceSpec &RELACSPlugin::outTrace( int index ) const
{
  if ( RW->AQ == 0 )
    return DummyTrace;
  return RW->AQ->outTrace( index );
}


const TraceSpec &RELACSPlugin::outTrace( const string &name ) const
{
  if ( RW->AQ == 0 )
    return DummyTrace;
  return RW->AQ->outTrace( name );
}


bool RELACSPlugin::acquisition( void ) const
{
  return RW->acquisition();
}


bool RELACSPlugin::simulation( void ) const
{
  return RW->simulation();
}


bool RELACSPlugin::analysis( void ) const
{
  return RW->analysis();
}


bool RELACSPlugin::idle( void ) const
{
  return RW->idle();
}


string RELACSPlugin::modeStr( void ) const
{
  return RW->modeStr();
}


void RELACSPlugin::modeChanged( void )
{
}


Options &RELACSPlugin::relacsSettings( void )
{
  return RW->SS;
}


const Options &RELACSPlugin::relacsSettings( void ) const
{
  return RW->SS;
}


void RELACSPlugin::lockRelacsSettings( void ) const
{
  RW->SS.lock();
}


void RELACSPlugin::unlockRelacsSettings( void ) const
{
  RW->SS.unlock();
}


QMutex *RELACSPlugin::relacsSettingsMutex( void )
{
  return RW->SS.mutex();
}


string RELACSPlugin::path( void ) const
{
  return RW->SF->path();
}


string RELACSPlugin::defaultPath( void ) const
{
  return RW->SF->defaultPath();
}


string RELACSPlugin::addPath( const string &file ) const
{
  return RW->SF->addPath( file );
}


string RELACSPlugin::addDefaultPath( const string &file ) const
{
  return RW->SF->addDefaultPath( file );
}


bool RELACSPlugin::saving( void ) const
{
  return RW->SF->saving();
}


Options &RELACSPlugin::stimulusData( void )
{
  return *RW->SF;
}


const Options &RELACSPlugin::stimulusData( void ) const
{
  return *RW->SF;
}


int RELACSPlugin::stimulusDataTraceFlag( void ) const
{
  return RW->SF->TraceFlag;
}


void RELACSPlugin::lockStimulusData( void ) const
{
  RW->SF->lock();
}


void RELACSPlugin::unlockStimulusData( void ) const
{
  RW->SF->unlock();
}


QMutex *RELACSPlugin::stimulusDataMutex( void )
{
  return RW->SF->mutex();
}


void RELACSPlugin::notifyStimulusData( void )
{
}


MetaData &RELACSPlugin::metaData( void )
{
  return RW->MTDT;
}


const MetaData &RELACSPlugin::metaData( void ) const
{
  return RW->MTDT;
}


void RELACSPlugin::lockMetaData( void ) const
{
  RW->MTDT.lock();
}


void RELACSPlugin::unlockMetaData( void ) const
{
  RW->MTDT.unlock();
}


QMutex *RELACSPlugin::metaDataMutex( void )
{
  return RW->MTDT.mutex();
}


void RELACSPlugin::notifyMetaData( void )
{
}


AllDevices *RELACSPlugin::devices( void ) const
{
  return RW->ADV;
}


Device *RELACSPlugin::device( const string &ident )
{
  return RW->ADV == 0 ? 0 : RW->ADV->device( ident );
}


void RELACSPlugin::updateDeviceMenu( void )
{
  postCustomEvent( 5 );
}


DigitalIO *RELACSPlugin::digitalIO( const string &ident )
{
  return RW->DIOD == 0 ? 0 : RW->DIOD->device( ident );
}


Trigger *RELACSPlugin::trigger( const string &ident )
{
  return RW->TRIGD == 0 ? 0 : RW->TRIGD->device( ident );
}


Attenuate *RELACSPlugin::attenuator( const string &name )
{
  int index = outTraceIndex( name );
  if ( index < 0 )
    return 0;
  else {
    return RW->AQ->outTraceAttenuate( index );
  }
}


Filter *RELACSPlugin::filter( const string &name )
{
  return RW->FD == 0 ? 0 : RW->FD->filter( name );
}


Filter *RELACSPlugin::filterTrace( int index )
{
  return RW->FD == 0 ? 0 : RW->FD->filter( index );
}


Filter *RELACSPlugin::filterTrace( const string &name )
{
  int inx = IL.index( name );
  return RW->FD == 0 || inx < 0 ? 0 : RW->FD->filter( inx );
}


Options &RELACSPlugin::filterOpts( const string &name )
{
  Filter *fl = filter( name );
  if ( fl != 0 )
    return *fl;
  else
    return Dummy;
}


Options &RELACSPlugin::filterTraceOpts( int index )
{
  Filter *fl = filterTrace( index );
  if ( fl != 0 )
    return *fl;
  else
    return Dummy;
}


Options &RELACSPlugin::filterTraceOpts( const string &name )
{
  Filter *fl = filterTrace( name );
  if ( fl != 0 )
    return *fl;
  else
    return Dummy;
}


void RELACSPlugin::lockFilter( const string &name )
{
  Filter *fl = filter( name );
  if ( fl != 0 )
    fl->lock();
}


void RELACSPlugin::lockFilterTrace( int index )
{
  Filter *fl = filterTrace( index );
  if ( fl != 0 )
    fl->lock();
}


void RELACSPlugin::lockFilterTrace( const string &name )
{
  Filter *fl = filterTrace( name );
  if ( fl != 0 )
    fl->lock();
}


void RELACSPlugin::unlockFilter( const string &name )
{
  Filter *fl = filter( name );
  if ( fl != 0 )
    fl->unlock();
}


void RELACSPlugin::unlockFilterTrace( int index )
{
  Filter *fl = filterTrace( index );
  if ( fl != 0 )
    fl->unlock();
}


void RELACSPlugin::unlockFilterTrace( const string &name )
{
  Filter *fl = filterTrace( name );
  if ( fl != 0 )
    fl->unlock();
}


void RELACSPlugin::autoConfigureFilter( Filter *filter, double tbegin, double tend )
{
  if ( filter == 0 )
    return;
  RW->FD->autoConfigure( filter, tbegin, tend );
}


void RELACSPlugin::autoConfigureFilter( Filter *filter, double duration )
{
  if ( filter == 0 )
    return;
  RW->FD->autoConfigure( filter, duration );
}


void RELACSPlugin::autoConfigureFilter( double tbegin, double tend )
{
  RW->FD->autoConfigure( tbegin, tend );
}


void RELACSPlugin::autoConfigureFilter( double duration )
{
  RW->FD->autoConfigure( duration );
}


Filter *RELACSPlugin::detector( const string &name )
{
  return RW->FD == 0 ? 0 : RW->FD->detector( name );
}


Filter *RELACSPlugin::detectorEvents( int index )
{
  return RW->FD == 0 ? 0 : RW->FD->detector( index );
}


Filter *RELACSPlugin::detectorEvents( const string &name )
{
  int inx = RW->ED.index( name );
  return RW->FD == 0 || inx < 0 ? 0 : RW->FD->detector( inx );
}


Options &RELACSPlugin::detectorOpts( const string &name )
{
  Filter *fl = detector( name );
  if ( fl != 0 )
    return *fl;
  else
    return Dummy;
}


Options &RELACSPlugin::detectorEventsOpts( int index )
{
  Filter *fl = detectorEvents( index );
  if ( fl != 0 )
    return *fl;
  else
    return Dummy;
}


Options &RELACSPlugin::detectorEventsOpts( const string &name )
{
  Filter *fl = detectorEvents( name );
  if ( fl != 0 )
    return *fl;
  else
    return Dummy;
}


void RELACSPlugin::lockDetector( const string &name )
{
  Filter *fl = detector( name );
  if ( fl != 0 )
    fl->lock();
}


void RELACSPlugin::lockDetectorEvents( int index )
{
  Filter *fl = detectorEvents( index );
  if ( fl != 0 )
    fl->lock();
}


void RELACSPlugin::lockDetectorEvents( const string &name )
{
  Filter *fl = detectorEvents( name );
  if ( fl != 0 )
    fl->lock();
}


void RELACSPlugin::unlockDetector( const string &name )
{
  Filter *fl = detector( name );
  if ( fl != 0 )
    fl->unlock();
}


void RELACSPlugin::unlockDetectorEvents( int index )
{
  Filter *fl = detectorEvents( index );
  if ( fl != 0 )
    fl->unlock();
}


void RELACSPlugin::unlockDetectorEvents( const string &name )
{
  Filter *fl = detectorEvents( name );
  if ( fl != 0 )
    fl->unlock();
}


void RELACSPlugin::autoConfigureDetector( Filter *detector, double tbegin, double tend )
{
  if ( detector == 0 )
    return;
  RW->FD->autoConfigure( detector, tbegin, tend );
}


void RELACSPlugin::autoConfigureDetector( Filter *detector, double duration )
{
  if ( detector == 0 )
    return;
  RW->FD->autoConfigure( detector, duration );
}


void RELACSPlugin::autoConfigureDetectors( double tbegin, double tend )
{
  RW->FD->autoConfigure( tbegin, tend );
}


void RELACSPlugin::autoConfigureDetectors( double duration )
{
  RW->FD->autoConfigure( duration );
}


void RELACSPlugin::setRELACSWidget( RELACSWidget *rw )
{
  RW = rw;

  // main widget for the ConfigDialog:
  setMainWidget( RW );

  // pathes to help files of plugins:
  clearHelpPathes();
  for ( int k=0; k<RW->HelpPathes.size(); k++ )
    addHelpPath( RW->HelpPathes[k] );

  string img = RW->IconPath + "/relacstuxheader.png";
  ifstream imgfl( img.c_str() );
  if ( imgfl.good() )
    setHeaderImageFile( img );
  setHeaderBackgroundColor( "#ffdd33" );
  setHeaderForegroundColor( "#ff0000" );
}


void RELACSPlugin::sessionStarted( void )
{
}


void RELACSPlugin::sessionStopped( bool saved )
{
}


double RELACSPlugin::sessionTime( void ) const
{
  return RW->SN->sessionTime();
}


string RELACSPlugin::sessionTimeStr( void ) const
{
  if ( ! RW->SN->running() )
    return "-";

  double sec = sessionTime();
  double min = floor( sec/60.0 );
  sec -= min*60.0;
  double hour = floor( min/60.0 );
  min -= hour*60.0;

  struct tm time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  time.tm_sec = (int)sec;
  time.tm_min = (int)min;
  time.tm_hour = (int)hour;

  lockRelacsSettings();
  Str sts = relacsSettings().text( "sessiontimeformat" );
  unlockRelacsSettings();
  sts.format( &time );
  return sts;
}


bool RELACSPlugin::sessionRunning( void ) const
{
  return RW->SN->running();
}


int RELACSPlugin::reproCount( void ) const
{
  return RW->SN->reproCount();
}


void RELACSPlugin::startTheSession( void )
{
  RW->SN->startTheSession();
}


void RELACSPlugin::stopTheSession( void )
{
  RW->SN->stopTheSession();
}


void RELACSPlugin::toggleSession( void )
{
  RW->SN->toggleSession();
}


void RELACSPlugin::removeFocus( void )
{
  RW->window()->setFocus();
}


Control *RELACSPlugin::control( int index )
{
  return RW->CW->control( index );
}


Control *RELACSPlugin::control( const string &name )
{
  return RW->CW->control( name );
}


Options &RELACSPlugin::controlOpts( int index )
{
  Control *cn = control( index );
  if ( cn !=0 )
    return *cn;
  else
    return Dummy;
}


Options &RELACSPlugin::controlOpts( const string &name )
{
  Control *cn = control( name );
  if ( cn !=0 )
    return *cn;
  else
    return Dummy;
}


void RELACSPlugin::lockControl( int index )
{
  Control *cn = control( index );
  if ( cn !=0 )
    cn->lock();
}


void RELACSPlugin::lockControl( const string &name )
{
  Control *cn = control( name );
  if ( cn !=0 )
    cn->lock();
}


void RELACSPlugin::unlockControl( int index )
{
  Control *cn = control( index );
  if ( cn !=0 )
    cn->unlock();
}


void RELACSPlugin::unlockControl( const string &name )
{
  Control *cn = control( name );
  if ( cn !=0 )
    cn->unlock();
}


Model *RELACSPlugin::model( void )
{
  return RW->MD;
}


Options &RELACSPlugin::modelOpts( void )
{
  if ( model() != 0 )
    return *model();
  else
    return Dummy;
}


void RELACSPlugin::lockModel( void )
{
  if ( model() !=0 )
    model()->lock();
}


void RELACSPlugin::unlockModel( void )
{
  if ( model() !=0 )
    model()->unlock();
}


RePros *RELACSPlugin::repros( void )
{
  return RW->RP;
}


Options &RELACSPlugin::reprosDialogOpts( void )
{
  return RW->RP->dialogOptions();
}


RePro *RELACSPlugin::repro( int index )
{
  return RW->RP == 0 ? 0 : RW->RP->repro( index );
}


RePro *RELACSPlugin::repro( const string &name )
{
  return RW->RP == 0 ? 0 : RW->RP->repro( name );
}


Options &RELACSPlugin::reproOpts( int index )
{
  RePro *rp = repro( index );
  if ( rp != 0 )
    return *rp;
  else
    return Dummy;
}


Options &RELACSPlugin::reproOpts( const string &name )
{
  RePro *rp = repro( name );
  if ( rp != 0 )
    return *rp;
  else
    return Dummy;
}


void RELACSPlugin::lockRePro( int index )
{
  RePro *rp = repro( index );
  if ( rp != 0 )
    rp->lock();
}


void RELACSPlugin::lockRePro( const string &name )
{
  RePro *rp = repro( name );
  if ( rp != 0 )
    rp->lock();
}


void RELACSPlugin::unlockRePro( int index )
{
  RePro *rp = repro( index );
  if ( rp != 0 )
    rp->unlock();
}


void RELACSPlugin::unlockRePro( const string &name )
{
  RePro *rp = repro( name );
  if ( rp != 0 )
    rp->unlock();
}


RePro *RELACSPlugin::currentRePro( void )
{
  return RW == 0 ? 0 : RW->CurrentRePro;
}


Options &RELACSPlugin::currentReProOpts( void )
{
  RePro *rp = currentRePro();
  if ( rp != 0 )
    return *rp;
  else
    return Dummy;
}


void RELACSPlugin::lockCurrentRePro( void )
{
  RePro *rp = currentRePro();
  if ( rp != 0 )
    rp->lock();
}


void RELACSPlugin::unlockCurrentRePro( void )
{
  RePro *rp = currentRePro();
  if ( rp != 0 )
    rp->unlock();
}


void RELACSPlugin::keyPressEvent( QKeyEvent *event )
{
  event->ignore();
}


void RELACSPlugin::keyReleaseEvent( QKeyEvent *event )
{
  event->ignore();
}


bool RELACSPlugin::eventFilter( QObject *obj, QEvent *event )
{
  if ( event->type() == QEvent::KeyPress ) {
    QKeyEvent *ke = dynamic_cast<QKeyEvent *>( event );
    if ( ke != 0 ) {
      keyPressEvent( ke );
      if ( event->isAccepted() )
	return true;
    }
  }
  else if ( event->type() == QEvent::KeyRelease ) {
    QKeyEvent *ke = dynamic_cast<QKeyEvent *>( event );
    if ( ke != 0 ) {
      keyReleaseEvent( ke );
      if ( event->isAccepted() )
	return true;
    }
  }
  return QObject::eventFilter( obj, event );
}


bool RELACSPlugin::globalKeyEvents( void )
{
  return GlobalKeyEvents;
}


void RELACSPlugin::setGlobalKeyEvents( bool global )
{
  GlobalKeyEvents = global;
}


void RELACSPlugin::setWaitMouseCursor( void )
{
  postCustomEvent( 6 );
}


void RELACSPlugin::restoreMouseCursor( void )
{
  postCustomEvent( 7 );
}


void RELACSPlugin::saveDoxygenOptions( void )
{
  cout << "\n";
  cout << name() << " [" << pluginSet() << "]\n";
  cout << "\\author " << author() << '\n';
  cout << "\\version " << version() << " (" << date() << ")\n";
  cout << "\\par Options\n";
  Options::save( cout, "- \\c %i=%s: %r (\\c %T)",
		 "- \\c %i=%g%u: %r (\\c %T)",
		 "- \\c %i=%b: %r (\\c %T)",
		 "- \\c %i=%04Y-%02m-%02d: %r (\\c %T)",
		 "- \\c %i=%02H:%02M:%02S: %r (\\c %T)",
		 "- \\c %i" );
}


void RELACSPlugin::saveWidget( void )
{
  if ( Widget != 0 ) {
    string filename = Str( name() + ".png" ).lower();
    for ( int n=0; n<100; n++ ) {
      if ( n > 0 )
	filename = Str( name() + "-" + Str( n, 2, '0' ) + ".png" ).lower();
      ifstream f( filename.c_str() );
      if ( ! f.good() )
	break;
    }
    QPixmap::grabWidget( Widget ).save( filename.c_str() ); 
    info( "Saved widget of plugin <b>" + name() + "</b><br> to file <b>" + filename + "</b>" );
  }
}


}; /* namespace relacs */

#include "moc_relacsplugin.cc"

