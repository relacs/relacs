/*
  ../include/relacs/repro.cc
  Parent class of all research programs.

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


RePro::RePro( const string &name, const string &titles,
	      const string &author, const string &version,
	      const string &date )
  : RELACSPlugin( "RePro: " + name, RELACSPlugin::Plugins,
	       name, titles, author, version, date ),
    OverwriteOpt()
{
  LastState = Continue;
  CompleteRuns = 0;
  TotalRuns = 0;
  AllRuns = 0;
  FailedRuns = 0;
  Saving = false;
  SoftStop = 0;
  SoftStopKey = Key_Space;

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
  ConfigClass::readConfig( sq );
  setToDefaults();
}


void RePro::saveConfig( ofstream &str )
{
  setDefaults();
  ConfigClass::saveConfig( str );
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
  SoftStop = 0;
  InterruptLock.lock();
  Interrupt = false;
  InterruptLock.unlock();
  lockAll();

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

#ifdef USEWAITCONDITION

  // wake up the RePro from sleeping:
  SleepWait.wakeAll();

#endif
}


bool RePro::sleep( double t )
{
  RW->updateRePro();

  // interrupt RePro:
  InterruptLock.lock();
  bool ir = Interrupt; 
  InterruptLock.unlock();
  if ( ir )
    return true;

  unlockAll();

#ifdef USEWAITCONDITION

  // sleep:
  if ( t > 0.0 ) {
    unsigned long ms = (unsigned long)::rint(1.0e3*t);
    if ( t < 0.001 || ms < 1 )
      QThread::usleep( (unsigned long)::rint(1.0e6*t) );
    else
      SleepWait.wait( ms );
  }

#else

  if ( t > 0.0 ) {
    if ( t < 0.001 )
      QThread::usleep( (unsigned long)::rint(1.0e6*t) );
    else if ( t < 1000.0 )
      QThread::msleep( (unsigned long)::rint(1.0e3*t) );
    else
      QThread::sleep( (unsigned long)::rint(t) );
  }

#endif

  RW->updateData();

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
}


bool RePro::sleepOn( double t )
{
  double st = 0.001 * SleepTime.elapsed();
  return RePro::sleep( t - st );
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

  lockSettings();
  Str rts = settings().text( "reprotimeformat" );
  unlockSettings();
  rts.format( &time );
  return rts;

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


void RePro::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == SoftStopKey )
    SoftStop++;
  else
    e->ignore();
}


void RePro::keyReleaseEvent( QKeyEvent *e )
{
  e->ignore();
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
  lockSettings();
  Str path = settings().text( "repropath" );
  unlockSettings();
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
  if ( dialogOpen() || empty() )
    return;

  setDialogOpen();

  // create and exec dialog:
  OptDialog *od = new OptDialog( false, this );
  od->setCaption( title() + " Settings" );
  // title:
  QGroupBox *gb = new QGroupBox( 2, Qt::Horizontal, this );
  gb->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  QLabel *rt = new QLabel( gb );
  string s = "<p align=\"center\">RePro <b>" + name() + "</b><br>version " + version() + 
    " (" + date() + ")<br>" + "by <b>" + author() + "</b></p>";
  rt->setText( s.c_str() );
  //  rt->setAlignment( Qt::AlignCenter );
  QPushButton *pb = new QPushButton( "&Help", gb );
  pb->setFixedSize( pb->sizeHint() );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( help( void ) ) );
  od->addWidget( gb );
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


string RePro::setOptions( const string &opttxt )
{ 
  string error;
  if ( opttxt.empty() )
    setDefaults();
  else {
    ConfigClass::read( opttxt ); 
    error = ConfigClass::warning();
  }

  return error;
}


bool RePro::saving( void ) const
{
  return Saving;
}


void RePro::setSaving( bool saving )
{
  Saving = saving;
}


void RePro::noSaving( void )
{
  RW->noSaving();
}

