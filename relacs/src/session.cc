/*
  ../include/relacs/session.cc
  Controls a recording session.

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

#include <cstdlib>
#include <cmath>
#include <qdatetime.h>
#include <qtooltip.h>
#include <relacs/str.h>
#include <relacs/relacswidget.h>
#include <relacs/session.h>


Session::Session( RELACSWidget *rw, int height,
		  QWidget *parent, const char *name )
  : QHBox( parent, name ),
    Options(),
    RW( rw )
{
  SaveData = false;
  SaveDialog = true;
  Running = false;
  SessionCounter = 0;
  SessionSeconds = 0.0;
  StartSessionTime = ::time( 0 );
  TotalTimer.start();
  MessageTimer = new QTimer( this );
  connect( MessageTimer, SIGNAL( timeout() ), this, SLOT( timeMessage() ) );

  TimeLabel = new QLabel( "-", this );
  TimeLabel->setTextFormat( PlainText );
  TimeLabel->setAlignment( AlignRight | AlignVCenter );
  TimeLabel->setIndent( 2 );
  TimeLabel->setFixedHeight( height );
  QToolTip::add( TimeLabel, "The elapsed time of a session in minutes" );
}


Session::~Session( void )
{
}

bool Session::running( void ) const
{
  return Running;
}


int Session::sessions( void ) const
{
  return SessionCounter;
}


time_t Session::startSessionTime( void ) const
{
  return StartSessionTime;
}


double Session::sessionTime( void ) const
{
  if ( Running ) 
    SessionSeconds = 0.001*SessionTimer.elapsed();
  return SessionSeconds;
}


string Session::sessionTimeStr( void ) const
{
  if ( !Running )
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

  RW->SS.lock();
  Str sts = RW->SS.text( "elapsedformat" );
  RW->SS.unlock();
  sts.format( &time );
  return sts;
}


double Session::totalTime( void ) const
{
  return 0.001*TotalTimer.elapsed();
}


void Session::polish( void )
{
  QWidget::polish();

  QFont f( fontInfo().family(), fontInfo().pointSize()*4/3, QFont::Bold );
  TimeLabel->setFont( f );
  TimeLabel->setFixedWidth( QFontMetrics( f ).boundingRect( "00:00" ).width() + 8 );
}


void Session::startTheSession( bool startmacro )
{
  if ( Running )
    return;

  StartSessionAction->setMenuText( "Stop Session" );

  SessionCounter++;
  StartSessionTime = ::time( 0 );
  SessionTimer.start();
  SessionSeconds = 0.0;
  timeMessage();

  RW->startSession( startmacro );

  QPalette p = palette();
  p.setColor( QPalette::Normal, QColorGroup::Foreground, red );
  p.setColor( QPalette::Inactive, QColorGroup::Foreground, red );
  setPalette( p );
  MessageTimer->start( 1000, false );

  Running = true;
}


void Session::startTheSession( void )
{
  startTheSession( true );
}


void Session::stopTheSession( void )
{
  if ( !Running )
    return;

  // the session might be stopped:
  RW->preStopSession();
  Running = false;

  // launch dialog:
  if ( SaveDialog ) {
    int r = RW->MTDT.dialog();
    if ( r < 0 ) {
      // cancel, the session is to be continued:
      Running = true;
      RW->resumeSession();
      return;
    }
    SaveData = ( r > 0 ); 
  }
  else {
    SaveData = false;
  }
  setenv( "RELACSSESSIONSAVED", SaveData ? "1" : "0", 1 );

  MessageTimer->stop();

  QPalette p = palette();
  p.setColor( QPalette::Normal, QColorGroup::Foreground, black );
  p.setColor( QPalette::Inactive, QColorGroup::Foreground, black );
  setPalette( p );

  timeMessage();

  StartSessionAction->setMenuText( "Start Session" );

  RW->stopSession( SaveData );

  Running = false;
}


void Session::toggleSession( void )
{
  if ( Running )
    stopTheSession();
  else
    startTheSession();
}


void Session::timeMessage( void )
{
  string st = sessionTimeStr();
  TimeLabel->setText( st.c_str() );
  setenv( "RELACSSESSIONTIME", st.c_str(), 1 );
  setenv( "RELACSSESSIONSECONDS", Str( SessionSeconds, 0, 0, 'f' ).c_str(), 1 );
}


void Session::addActions( QPopupMenu *menu )
{
  StartSessionAction = new QAction( this, "SessionAction" );
  StartSessionAction->setMenuText( "Start Session" );
  StartSessionAction->setAccel( Key_Enter );
  connect( StartSessionAction, SIGNAL( activated() ), 
	   this, SLOT( toggleSession() ) );
  StartSessionAction->addTo( menu );

  QAccel *sa = new QAccel( this, "SessionAccel" );
  sa->connectItem( sa->insertItem( Key_Return ),
		   this, SLOT( toggleSession() ) );
}


bool Session::saveData( void ) const
{
  return SaveData;
}


void Session::setSaveData( bool save )
{
  SaveData = save;
}


bool Session::saveDialog( void ) const
{
  return SaveDialog;
}


void Session::setSaveDialog( bool dialog )
{
  SaveDialog = dialog;
}


void Session::modeChanged( void )
{
  StartSessionAction->setEnabled( ( RW->simulation() || RW->acquisition() ) );
}

