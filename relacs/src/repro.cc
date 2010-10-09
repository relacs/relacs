/*
  repro.cc
  Parent class of all research programs.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#include <ctype.h>
#include <cmath>
#include <QDir>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QTextBrowser>
#include <QApplication>
#include <relacs/str.h>
#include <relacs/optdialog.h>
#include <relacs/relacswidget.h>
#include <relacs/repro.h>

namespace relacs {


RePro::RePro( const string &name, const string &pluginset,
	      const string &author,
	      const string &version, const string &date )
  : RELACSPlugin( "RePro: " + name, RELACSPlugin::Plugins,
		  name, pluginset, author, version, date ),
    OverwriteOpt(),
    ProjectOpt(),
    MyProjectOpt()
{
  LastState = Continue;
  CompleteRuns = 0;
  TotalRuns = 0;
  AllRuns = 0;
  FailedRuns = 0;
  ReProStartTime = 0.0;

  ProjectOpt.addText( "project", "Project", "" );
  ProjectOpt.addText( "experiment", "Experiment", "" );
  MyProjectOpt = ProjectOpt;

  SoftStop = 0;
  SoftStopKey = Qt::Key_Space;

  GrabKeys.clear();
  GrabKeys.reserve( 20 );
  GrabKeysBaseSize = 0;
  GrabKeysInstalled = false;
  GrabKeysAllowed = false;

  // start timer:
  ReProTime.start();
  
  PrintMessage = true;
}


RePro::~RePro( void )
{
  // XXX auto remove from RePros?
}


void RePro::setName( const string &name )
{
  ConfigDialog::setName( name );
  setConfigIdent( "RePro: " + name ); 
}


void RePro::readConfig( StrQueue &sq )
{
  for ( int k=0; k<sq.size(); k++ ) {
    if ( sq[k].find( "project:" ) >= 0 ) {
      MyProjectOpt.read( sq[k] );
      sq.erase( k );
      break;
    }
  }
  for ( int k=0; k<sq.size(); k++ ) {
    if ( sq[k].find( "experiment:" ) >= 0 ) {
      MyProjectOpt.read( sq[k] );
      sq.erase( k );
      break;
    }
  }
  ConfigClass::readConfig( sq );
  setToDefaults();
}


void RePro::saveConfig( ofstream &str )
{
  setDefaults();
  ConfigClass::saveConfig( str );
  MyProjectOpt.save( str, "  ", -1 );
}


void RePro::run( void )
{
  // start timer:
  ReProTime.start();
  timeStamp();

  // write message:
  if ( PrintMessage )
    message( "Running <b>" + name() + "</b> ..." );

  // init:
  ReProStartTime = sessionTime();
  SoftStop = 0;
  GrabKeysAllowed = true;
  GrabKeysBaseSize = GrabKeys.size();
  grabKeys();
  setSettings();
  InterruptLock.lock();
  Interrupt = false;
  InterruptLock.unlock();
  lockAll();
  RW->SF->holdOff();

  // run RePro:
  LastState = main();

  // update statistics:
  if ( LastState == Completed )
    CompleteRuns++;
  if ( LastState == Completed || LastState == Aborted )
    TotalRuns++;
  AllRuns++;
  if ( LastState == Failed )
    FailedRuns++;

  unlockAll();

  RW->KeyTime->unsetNoFocusWidget();
  GrabKeysAllowed = false;
  releaseKeys();

  // write message:
  if ( PrintMessage ) {
    if ( completed() )
      message( "<b>" + name() + "</b> succesfully completed after <b>" + reproTimeStr() + "</b>" );
    else if ( failed() )
      message( "<b>" + name() + "</b> stopped after <b>" + reproTimeStr() + "</b>" );
    else
      message( "<b>" + name() + "</b> aborted after <b>" + reproTimeStr() + "</b>" );
  }

  // start next RePro:
  if ( ! interrupt() ) {
    QApplication::postEvent( (QWidget*)RW, new QEvent( QEvent::Type( QEvent::User+1 ) ) );
  }
}


bool RePro::interrupt( void ) const
{
  InterruptLock.lock();
  bool ir = Interrupt; 
  InterruptLock.unlock();
  return ir;
}


void RePro::requestStop( void )
{
  // tell the RePro to interrupt:
  InterruptLock.lock();
  Interrupt = true;
  InterruptLock.unlock();

  // wake up the RePro from sleeping:
  SleepWait.wakeAll();

  // don't wait for the requested data:
  RW->setMinTraceTime( 0.0 );
}


bool RePro::sleep( double t, double tracetime )
{
  if ( tracetime < 0.0 )
    tracetime = currentTime() + t;

  RW->updateRePro();

  // interrupt RePro:
  InterruptLock.lock();
  bool ir = Interrupt; 
  InterruptLock.unlock();
  if ( ir )
    return true;

  unlockAll();

  // sleep:
  if ( t > 0.0 ) {
    unsigned long ms = (unsigned long)::rint(1.0e3*t);
    if ( t < 0.001 || ms < 1 )
      QThread::usleep( (unsigned long)::rint(1.0e6*t) );
    else {
      // XXX wait requires a mutex!
      QMutex mutex;
      mutex.lock();
      SleepWait.wait( &mutex, ms );
      mutex.unlock();
    }
  }

  // interrupt RePro:
  InterruptLock.lock();
  ir = Interrupt; 
  InterruptLock.unlock();

  // force data updates:
  RW->setMinTraceTime( ir ? 0.0 : tracetime );
  RW->ThreadSleepWait.wakeAll();
  // XXX wait requires a mutex!
  QMutex mutex;
  mutex.lock();
  RW->UpdateDataWait.wait( &mutex );
  mutex.unlock();

  lockAll();

  // interrupt RePro:
  InterruptLock.lock();
  ir = Interrupt; 
  InterruptLock.unlock();

  return ir;
}


void RePro::timeStamp( void )
{
  SleepTime.start();
  TraceTime = currentTime();
}


bool RePro::sleepOn( double t )
{
  double st = 0.001 * SleepTime.elapsed();
  return RePro::sleep( t - st, TraceTime + t );
}


bool RePro::sleepWait( double time )
{
  unlockAll();
  // XXX wait requires a mutex!
  QMutex mutex;
  mutex.lock();
  bool r = false;
  if ( time <= 0.0 )
    r = SleepWait.wait( &mutex );
  else {
    unsigned long ms = (unsigned long)::rint(1.0e3*time);
    r = SleepWait.wait( &mutex, ms );
  }
  mutex.unlock();
  lockAll();
  return r;
}


void RePro::wake( void )
{
  SleepWait.wakeAll();
}


void RePro::sessionStarted( void )
{
  CompleteRuns = 0;
  TotalRuns = 0;
  AllRuns = 0;
  FailedRuns = 0;
}


void RePro::sessionStopped( bool saved )
{
  CompleteRuns = 0;
  TotalRuns = 0;
  AllRuns = 0;
  FailedRuns = 0;
}


bool RePro::completed( void ) const
{
  return ( LastState == Completed );
}


bool RePro::aborted( void ) const
{
  return ( LastState == Aborted );
}


bool RePro::failed( void ) const
{
  return ( LastState == Failed );
}


int RePro::completeRuns( void ) const
{
  return CompleteRuns;
}


int RePro::totalRuns( void ) const
{
  return TotalRuns;
}


double RePro::reproTime( void ) const
{
  return 0.001*ReProTime.elapsed();
}


string RePro::reproTimeStr( void ) const
{
  double sec = reproTime();
  double min = floor( sec/60.0 );
  sec -= min*60.0;
  double hour = floor( min/60.0 );
  min -= hour*60.0;

  struct tm time = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  time.tm_sec = (int)sec;
  time.tm_min = (int)min;
  time.tm_hour = (int)hour;

  lockRelacsSettings();
  Str rts = relacsSettings().text( "reprotimeformat" );
  unlockRelacsSettings();
  rts.format( &time );
  return rts;

}


double RePro::reproStartTime( void ) const
{
  return ReProStartTime;
}


int RePro::testWrite( OutData &signal )
{
  return RW->AQ->testWrite( signal );
}


int RePro::testWrite( OutList &signal )
{
  return RW->AQ->testWrite( signal );
}

/*
int RePro::convert( OutData &signal )
{
  return RW->AQ->convert( signal );
}


int RePro::convert( OutList &signal )
{
  return RW->AQ->convert( signal );
}
*/

int RePro::write( OutData &signal )
{
  return RW->write( signal );
}


int RePro::write( OutList &signal )
{
  return RW->write( signal );
}


int RePro::directWrite( OutData &signal )
{
  return RW->directWrite( signal );
}


int RePro::directWrite( OutList &signal )
{
  return RW->directWrite( signal );
}


int RePro::writeZero( int channel, int device )
{
  return RW->AQ->writeZero( channel, device );
}


int RePro::writeZero( int index )
{
  return RW->AQ->writeZero( index );
}


int RePro::writeZero( const string &trace )
{
  return RW->AQ->writeZero( trace );
}


int RePro::stopWrite( void )
{
  return RW->AQ->stopWrite();
}


double RePro::minLevel( int trace ) const
{
  return RW->AQ->minLevel( trace );
}


double RePro::minLevel( const string &trace ) const
{
  return RW->AQ->minLevel( trace );
}


double RePro::maxLevel( int trace ) const
{
  return RW->AQ->maxLevel( trace );
}


double RePro::maxLevel( const string &trace ) const
{
  return RW->AQ->maxLevel( trace );
}


void RePro::levels( int trace, vector<double> &l ) const
{
  return RW->AQ->levels( trace, l );
}


void RePro::levels( const string &trace, vector<double> &l ) const
{
  return RW->AQ->levels( trace, l );
}


double RePro::minIntensity( int trace, double frequency ) const
{
  return RW->AQ->minIntensity( trace, frequency );
}


double RePro::minIntensity( const string &trace, double frequency ) const
{
  return RW->AQ->minIntensity( trace, frequency );
}


double RePro::maxIntensity( int trace, double frequency ) const
{
  return RW->AQ->maxIntensity( trace, frequency );
}


double RePro::maxIntensity( const string &trace, double frequency ) const
{
  return RW->AQ->maxIntensity( trace, frequency );
}


void RePro::intensities( int trace, vector<double> &ints, double frequency ) const
{
  return RW->AQ->intensities( trace, ints, frequency );
}


void RePro::intensities( const string &trace, vector<double> &ints, double frequency ) const
{
  return RW->AQ->intensities( trace, ints, frequency );
}


void RePro::setMessage( bool message )
{
  PrintMessage = message;
}


void RePro::noMessage( void )
{
  PrintMessage = false;
}


void RePro::message( const string &msg )
{
  if ( repros() != 0 )
    repros()->message( msg );
}


string RePro::addPath( const string &file ) const
{
  RW->SF->storeFile( file );
  return RW->SF->addPath( file );
}


void RePro::keepFocus( void )
{
  RW->KeyTime->setNoFocusWidget( widget() );
}


void RePro::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() == SoftStopKey ) {
    SoftStop++;
    event->accept();
  }
  else
    event->ignore();
}


void RePro::keyReleaseEvent( QKeyEvent *event )
{
  event->ignore();
}


void RePro::grabKey( int key )
{
  GrabKeyLock.lock();
  GrabKeys.push_back( key );
  GrabKeyLock.unlock();
  grabKeys();
}


void RePro::grabKeys( void )
{
  GrabKeyLock.lock();
  bool r = GrabKeys.empty() || GrabKeysInstalled || ! GrabKeysAllowed;
  GrabKeyLock.unlock();
  if ( r )
    return;

  GrabKeyLock.lock();
  qApp->installEventFilter( (RELACSPlugin*)this );
  GrabKeysInstalled = true;
  GrabKeyLock.unlock();
}


void RePro::releaseKey( int key )
{
  GrabKeyLock.lock();
  int inx = 0;
  vector<int>::iterator kp = GrabKeys.begin();
  while ( kp != GrabKeys.end() ) {
    if ( *kp == key ) {
      kp = GrabKeys.erase( kp );
      if ( inx < GrabKeysBaseSize )
	GrabKeysBaseSize--;
    }
    else {
      ++kp;
      ++inx;
    }
  }
  int empty = GrabKeys.empty();
  GrabKeyLock.unlock();
  if ( empty )
    releaseKeys();
}


void RePro::releaseKeys( void )
{
  GrabKeyLock.lock();
  GrabKeys.resize( GrabKeysBaseSize );
  if ( GrabKeysInstalled ) {
    qApp->removeEventFilter( (RELACSPlugin*)this );
    GrabKeysInstalled = false;
  }
  GrabKeyLock.unlock();
}


bool RePro::eventFilter( QObject *watched, QEvent *e )
{
  if ( watched == widget() )
    return RELACSPlugin::eventFilter( watched, e );   // this passes the keyevents to the RePros...

  if ( e->type() == QEvent::Shortcut ) {
    // check for grabbed keys:
    QShortcutEvent *se = dynamic_cast<QShortcutEvent *>(e);
    vector<int>::iterator kp = GrabKeys.begin();
    while ( kp != GrabKeys.end() ) {
      if ( se->key() == QKeySequence( *kp ) ) {
	int mask = Qt::META | Qt::SHIFT | Qt::CTRL | Qt::ALT;
	QKeyEvent ke( QEvent::KeyPress, se->key()[0] & ~mask,
		      Qt::KeyboardModifiers( se->key()[0] & mask ) );
	keyPressEvent( &ke );
	return true;
      }
      ++kp;
    }
  }
  return false;
}


int RePro::softStop( void )
{
  return SoftStop;
}


void RePro::setSoftStop( int s )
{
  SoftStop = s;
}


void RePro::clearSoftStop( void )
{
  SoftStop = 0;
}


void RePro::setSoftStopKey( int keycode )
{
  SoftStopKey = keycode;
}


void RePro::plotToggle( bool on, bool fixed, double length, double offs )
{
  RW->plotToggle( on, fixed, length, offs );
}


string RePro::macroName( void )
{
  return repros()->macroName();
}


string RePro::macroParam( void )
{
  return repros()->macroParam();
}


string RePro::reproPath( bool v )
{
  // construct path:
  lockRelacsSettings();
  Str path = relacsSettings().text( "repropath" );
  unlockRelacsSettings();
  path.provideSlash();
  path += Str( name() ).lower();
  path.provideSlash();
  if ( v ) {
    path += "v" + Str( version() ).lower();
    path.provideSlash();
  }

  // create path:
  QDir dir;  // current directory
  int sp = path.find( '/', 1 );
  while ( sp > 0 ) {
    string dirpath = path.left( sp );
    if ( ! dir.exists( dirpath.c_str() ) ) {
      if ( ! dir.mkdir( dirpath.c_str() ) )
	return "";
    }
    sp = path.find( '/', sp+1 );
  }

  return path;
}


string RePro::addReProPath( const string &file, bool v )
{
  return reproPath( v ) + file;
}


void RePro::dialog( void )
{ 
  if ( dialogOpen() )
    return;
  setDialogOpen();

  OptDialog *od = new OptDialog( false, RW );
  od->setCaption( dialogCaption() );
  dialogHeaderWidget( od );
  if ( Options::size( dialogSelectMask() ) <= 0 )
    dialogEmptyMessage( od );
  else {
    // repro options:
    Options::addStyle( OptWidget::LabelBlue, MacroFlag );
    Options::delStyle( OptWidget::LabelBlue, OverwriteFlag );
    Options::addStyle( OptWidget::LabelGreen, OverwriteFlag );
    Options::delStyle( OptWidget::LabelGreen, CurrentFlag );
    Options::addStyle( OptWidget::LabelRed, CurrentFlag );
    OptWidget *roptw = od->addOptions( *this, dialogSelectMask(), 
				       dialogReadOnlyMask(), dialogStyle(),
				       mutex() );
    if ( Options::styleSize( OptWidget::TabLabel ) == 0 &&
	 ( dialogStyle() & OptWidget::TabLabelStyle ) == 0 ) {
      roptw->setMargins( 2 );
      od->addSeparator();
    }
    OptWidget *doptw = od->addOptions( reprosDialogOpts() );
    doptw->setMargins( 2 );
    doptw->setVerticalSpacing ( 4 );
    OptWidget *poptw = od->addOptions( projectOptions() );
    poptw->setMargins( 2 );
    poptw->setVerticalSpacing ( 4 );
    // buttons:
    od->setRejectCode( 0 );
    od->addButton( "&Ok", OptDialog::Accept, 1 );
    od->addButton( "&Apply", OptDialog::Accept, 1, false );
    od->addButton( "&Run", OptDialog::Accept, 2, false );
    od->addButton( "&Reset", OptDialog::Defaults, 3, false );
    od->addButton( "&Close" );
    QWidget::connect( od, SIGNAL( dialogClosed( int ) ),
		      (QWidget*)this, SLOT( dClosed( int ) ) );
    QWidget::connect( od, SIGNAL( buttonClicked( int ) ),
		      (QWidget*)this, SIGNAL( dialogAction( int ) ) );
    QWidget::connect( od, SIGNAL( valuesChanged( void ) ),
		      (QWidget*)this, SIGNAL( dialogAccepted( void ) ) );
  }
  od->exec();
}


void RePro::dClosed( int r )
{
  ConfigDialog::dClosed( r );
  Options::delStyle( OptWidget::LabelBlue, MacroFlag );
  Options::delStyle( OptWidget::LabelGreen, OverwriteFlag );
  Options::delStyle( OptWidget::LabelRed, CurrentFlag );
  Options::delFlags( MacroFlag + OverwriteFlag + CurrentFlag );
}


Options &RePro::overwriteOptions( void )
{
  return OverwriteOpt;
}


Options &RePro::projectOptions( void )
{
  return ProjectOpt;
}


const Options &RePro::projectOptions( void ) const
{
  return ProjectOpt;
}


void RePro::setProjectOptions( void )
{
  MyProjectOpt = ProjectOpt;
}


void RePro::getProjectOptions( void )
{
  ProjectOpt = MyProjectOpt;
}


string RePro::checkOptions( const string &opttxt )
{ 
  Options opt = *(Options*)(this);
  opt.erase( "project" );
  opt.erase( "experiment" );
  opt.read( opttxt ); 
  return opt.warning();
}


void RePro::setSaving( bool saving )
{
  RW->SF->save( saving );
}


void RePro::noSaving( void )
{
  setSaving( false );
}



}; /* namespace relacs */

#include "moc_repro.cc"

