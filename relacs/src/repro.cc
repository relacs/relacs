/*
  repro.cc
  Parent class of all research programs.

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

#include <ctype.h>
#include <cmath>
#include <qdir.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qtextbrowser.h>
#include <qapplication.h>
#include <relacs/str.h>
#include <relacs/optdialog.h>
#include <relacs/relacswidget.h>
#include <relacs/repro.h>

namespace relacs {


RePro::RePro( const string &name, const string &titles,
	      const string &pluginset, const string &author,
	      const string &version, const string &date )
  : RELACSPlugin( "RePro: " + name, RELACSPlugin::Plugins,
		  name, titles, pluginset, author, version, date ),
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
  SoftStopKey = Key_Space;

  GrabKeys.clear();
  GrabKeys.reserve( 20 );
  GrabKeysModifier.clear();
  GrabKeysModifier.reserve( 20 );
  GrabKeysBaseSize = 0;
  GrabKeysAlt = false;
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
    message( "Running <b>" + title() + "</b> ..." );

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
      message( "<b>" + title() + "</b> succesfully completed after <b>" + reproTimeStr() + "</b>" );
    else if ( failed() )
      message( "<b>" + title() + "</b> stopped after <b>" + reproTimeStr() + "</b>" );
    else
      message( "<b>" + title() + "</b> aborted after <b>" + reproTimeStr() + "</b>" );
  }

  // start next RePro:
  if ( ! interrupt() ) {
    QApplication::postEvent( RW, new QCustomEvent( QEvent::User+1 ) );
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
    tracetime = trace( 0 ).currentTime() + t;

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
    else
      SleepWait.wait( ms );
  }

  // interrupt RePro:
  InterruptLock.lock();
  ir = Interrupt; 
  InterruptLock.unlock();

  // force data updates:
  RW->setMinTraceTime( ir ? 0.0 : tracetime );
  RW->ThreadSleepWait.wakeAll();
  RW->UpdateDataWait.wait();

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
  TraceTime = trace( 0 ).currentTime();
}


bool RePro::sleepOn( double t )
{
  double st = 0.001 * SleepTime.elapsed();
  return RePro::sleep( t - st, TraceTime + t );
}


bool RePro::sleepWait( double time )
{
  unlockAll();
  bool r = false;
  if ( time <= 0.0 )
    r = SleepWait.wait();
  else {
    unsigned long ms = (unsigned long)::rint(1.0e3*time);
    r = SleepWait.wait( ms );
  }
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


int RePro::convert( OutData &signal )
{
  return RW->AQ->convert( signal );
}


int RePro::convert( OutList &signal )
{
  return RW->AQ->convert( signal );
}


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
  RW->KeyTime->setNoFocusWidget( this );
}


void RePro::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == SoftStopKey ) {
    SoftStop++;
    e->accept();
  }
  else
    e->ignore();
}


void RePro::keyReleaseEvent( QKeyEvent *e )
{
  e->ignore();
}


void RePro::grabKey( int key )
{
  int modifier = 0;
  switch ( key & MODIFIER_MASK ) {
  case META : modifier = MetaButton; break;
  case SHIFT : modifier = ShiftButton; break;
  case CTRL : modifier = ControlButton; break;
  case ALT : modifier = AltButton; GrabKeysAlt = true; break;
  default: modifier = 0; break;
  }
  GrabKeyLock.lock();
  GrabKeys.push_back( key & ~MODIFIER_MASK );
  GrabKeysModifier.push_back( modifier );
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
  qApp->installEventFilter( this );
  GrabKeysInstalled = true;
  GrabKeyLock.unlock();
}


void RePro::releaseKey( int key )
{
  int modifier = 0;
  switch ( key & MODIFIER_MASK ) {
  case META : modifier = MetaButton; break;
  case SHIFT : modifier = ShiftButton; break;
  case CTRL : modifier = ControlButton; break;
  case ALT : modifier = AltButton; break;
  default: modifier = 0; break;
  }
  int keycode = key & ~ MODIFIER_MASK;

  GrabKeyLock.lock();
  int inx = 0;
  vector<int>::iterator kp = GrabKeys.begin();
  vector<int>::iterator mp = GrabKeysModifier.begin();
  while ( kp != GrabKeys.end() ) {
    if ( *kp == keycode && *mp == modifier ) {
      kp = GrabKeys.erase( kp );
      mp = GrabKeysModifier.erase( mp );
      if ( inx < GrabKeysBaseSize )
	GrabKeysBaseSize--;
    }
    else {
      ++kp;
      ++mp;
      ++inx;
    }
  }
  GrabKeysAlt = false;
  for ( mp = GrabKeysModifier.begin(); mp != GrabKeysModifier.end(); ++mp ) {
    if ( *mp == AltButton ) {
      GrabKeysAlt = true;
      break;
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
  GrabKeysModifier.resize( GrabKeysBaseSize );
  GrabKeysAlt = false;
  for ( vector<int>::iterator mp = GrabKeysModifier.begin();
	mp != GrabKeysModifier.end();
	++mp ) {
    if ( *mp == AltButton ) {
      GrabKeysAlt = true;
      break;
    }
  }
  if ( GrabKeysInstalled ) {
    qApp->removeEventFilter( this );
    GrabKeysInstalled = false;
  }
  GrabKeyLock.unlock();
}


bool RePro::eventFilter( QObject *watched, QEvent *e )
{
  if ( e->type() == QEvent::KeyPress ||
       e->type() == QEvent::KeyRelease ||
       e->type() == QEvent::Accel ||
       e->type() == QEvent::AccelOverride ) {
    QKeyEvent *k = (QKeyEvent*)e;
    // no Alt-release event:
    if ( e->type() == QEvent::KeyRelease &&
	 GrabKeysAlt &&
	 k->key() == Key_Alt )
      return true;
    // check for grabbed keys:
    vector<int>::iterator kp = GrabKeys.begin();
    vector<int>::iterator mp = GrabKeysModifier.begin();
    while ( kp != GrabKeys.end() ) {
      if ( *kp == k->key() && *mp == k->state() ) {
	if ( e->type() == QEvent::KeyRelease )
	  keyReleaseEvent( k );
	else
	  keyPressEvent( k );
	return true;
      }
      ++kp;
      ++mp;
    }
  }
  return RELACSPlugin::eventFilter( watched, e );
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

  OptDialog *od = new OptDialog( false, this );
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
    OptWidget *row = od->addOptions( *this, dialogSelectMask(), 
				     dialogReadOnlyMask(), dialogStyle(),
				     mutex() );
    od->addOptions( reprosDialogOpts() );
    od->addOptions( projectOptions() );
    od->setSpacing( int(9.0*exp(-double(row->lines())/14.0))+1 );
    od->setMargin( 10 );
    // buttons:
    od->setRejectCode( 0 );
    od->addButton( "&Ok", OptDialog::Accept, 1 );
    od->addButton( "&Apply", OptDialog::Accept, 1, false );
    od->addButton( "&Run", OptDialog::Accept, 2, false );
    od->addButton( "&Defaults", OptDialog::Defaults, 3, false );
    od->addButton( "&Close" );
    connect( od, SIGNAL( dialogClosed( int ) ),
	     this, SLOT( dClosed( int ) ) );
    connect( od, SIGNAL( buttonClicked( int ) ),
	     this, SIGNAL( dialogAction( int ) ) );
    connect( od, SIGNAL( valuesChanged( void ) ),
	     this, SIGNAL( dialogAccepted( void ) ) );
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

