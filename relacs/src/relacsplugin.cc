/*
  relacsplugin.cc
  Adds specific functions for RELACS plugins to ConfigDialog

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

#include <QDateTime>
#include <relacs/messagebox.h>
#include <relacs/metadata.h>
#include <relacs/model.h>
#include <relacs/filter.h>
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
  Widget = 0;
  RW = 0;
  GlobalKeyEvents = false;
}


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
  setWidget( new QWidget );
  Widget->setLayout( layout );
}


string RELACSPlugin::pluginSet( void ) const
{
  return PluginSet;
}


void RELACSPlugin::setPluginSet( const string &pluginset )
{
  PluginSet = pluginset;
}


string RELACSPlugin::helpFileName( void ) const
{
  return pluginSet() + '-' + name() + ".html";
}


void RELACSPlugin::printlog( const string &s ) const
{
  if ( RW == 0 )
    cerr << QTime::currentTime().toString().toLatin1().data() << " "
	 << name() << ": " << s << endl;
  else
    RW->printlog( name() + ": " + s );
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
  QApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+type ) ) );
}


void RELACSPlugin::customEvent( QEvent *qce )
{
  switch ( qce->type() ) {
  case QEvent::User+3: {
    string ss = "RELACS: ";
    ss += name();
    MessageBox::warning( ss, WarningStr, WarningTimeout, RW );
    break;
  }
  case QEvent::User+4: {
    string ss = "RELACS: ";
    ss += name();
    MessageBox::information( ss, InfoStr, InfoTimeout, RW );
    break;
  }
  case QEvent::User+5: {
    if ( RW->DV != 0 )
      RW->DV->updateMenu();
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
  if ( RW != 0 )
    RW->readLockData();
  lockMetaData();
  lockStimulusData();
}


void RELACSPlugin::unlockAll( void )
{
  unlockStimulusData();
  unlockMetaData();
  if ( RW != 0 )
    RW->unlockData();
  unlock();
}


void RELACSPlugin::readLockData( void )
{
  if ( RW != 0 )
    RW->readLockData();
}


void RELACSPlugin::writeLockData( void )
{
  if ( RW != 0 )
    RW->writeLockData();
}


void RELACSPlugin::unlockData( void )
{
  if ( RW != 0 )
    RW->unlockData(); 
}


int RELACSPlugin::dataMutexCount( void )
{
  return RW != 0 ? RW->dataMutexCount() : -1;
}


const InList &RELACSPlugin::traces( void ) const
{
  return RW->IL;
}


const InData &RELACSPlugin::trace( int index ) const
{
  return RW->IL[index];
}


const InData &RELACSPlugin::trace( const string &ident ) const
{
  return RW->IL[ident];
}


int RELACSPlugin::traceIndex( const string &ident ) const
{
  return RW->IL.index( ident );
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
  adjust( data, data.currentTime() - duration, data.currentTime(),
	  threshold ); 
}


void RELACSPlugin::activateGains( void )
{
  RW->activateGains();
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


int RELACSPlugin::applyOutTrace( OutData &signal ) const
{
  if ( RW->AQ == 0 )
    return 0;
  return RW->AQ->applyOutTrace( signal );
}


int RELACSPlugin::applyOutTrace( OutList &signal ) const
{
  if ( RW->AQ == 0 )
    return 0;
  return RW->AQ->applyOutTrace( signal );
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


Options &RELACSPlugin::settings( void )
{
  return RW->SS;
}


const Options &RELACSPlugin::settings( void ) const
{
  return RW->SS;
}


void RELACSPlugin::lockSettings( void ) const
{
  RW->SS.lock();
}


void RELACSPlugin::unlockSettings( void ) const
{
  RW->SS.unlock();
}


QMutex *RELACSPlugin::settingsMutex( void )
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


Options &RELACSPlugin::stimulusData( void )
{
  return *RW->SF;
}


const Options &RELACSPlugin::stimulusData( void ) const
{
  return *RW->SF;
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


Options &RELACSPlugin::metaData( const string &section )
{
  return RW->MTDT.section( section );
}


const Options &RELACSPlugin::metaData( const string &section ) const
{
  return RW->MTDT.section( section );
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


void RELACSPlugin::notifyMetaData( const string &section )
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


Attenuate *RELACSPlugin::attenuator( const string &device, int channel )
{
  return RW->ATI == 0 ? 0 : RW->ATI->attenuate( device, channel );
}


Attenuate *RELACSPlugin::attenuator( int index, int channel )
{
  return RW->ATI == 0 ? 0 : RW->ATI->attenuate( index, channel );
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
  int inx = RW->IL.index( name );
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

  lockSettings();
  Str sts = settings().text( "sessiontimeformat" );
  unlockSettings();
  sts.format( &time );
  return sts;
}


bool RELACSPlugin::sessionRunning( void ) const
{
  return RW->SN->running();
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


Control *RELACSPlugin::control( int index )
{
  if ( index >= 0 && index < (int)RW->CN.size() )
    return RW->CN[index];
  return 0;
}


Control *RELACSPlugin::control( const string &name )
{
  for ( unsigned int k=0; k<RW->CN.size(); k++ ) {
    if ( RW->CN[k]->name() == name )
      return RW->CN[k];
  }
  return 0;
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
    keyPressEvent( static_cast<QKeyEvent *>( event ) );
    return ( event->isAccepted() );
  }
  else if ( event->type() == QEvent::KeyRelease ) {
    keyReleaseEvent( static_cast<QKeyEvent *>( event ) );
    return ( event->isAccepted() );
  }
  else
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


}; /* namespace relacs */

#include "moc_relacsplugin.cc"

