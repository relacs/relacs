/*
  base/wait.cc
  Wait until a specified (date and) time.

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

#include <QDateTime>
#include <QVBoxLayout>
#include <relacs/base/wait.h>
using namespace relacs;

namespace base {


Wait::Wait( void )
  : RePro( "Wait", "base", "Jan Benda", "1.0", "Jul 07, 2010" )
{
  // add some options:
  addBoolean( "absdate", "Specify absolute date", false );
  addDate( "date", "Date" ).setActivation( "absdate", "true" );
  addInteger( "days", "Days from start of RePro on", 0, 0, 100000, 1, "days" ).setActivation( "absdate", "false" );
  addTime( "time", "Time" );

  QVBoxLayout *vb = new QVBoxLayout;

  QLabel *label = new QLabel( "Wait until" );
  label->setAlignment( Qt::AlignHCenter );
  vb->addWidget( label );

  EndTimeLabel.setAlignment( Qt::AlignHCenter );
  vb->addWidget( &EndTimeLabel );

  label = new QLabel;
  vb->addWidget( label );

  label = new QLabel( "Remaining time:" );
  label->setAlignment( Qt::AlignHCenter );
  vb->addWidget( label );

  RemainingTimeLabel.setAlignment( Qt::AlignHCenter );
  vb->addWidget( &RemainingTimeLabel );

  QPB.setOrientation( Qt::Horizontal );
  vb->addWidget( &QPB );
  setLayout( vb );
}


class WaitEvent : public QEvent
{

public:

  WaitEvent( int min, int max )
    : QEvent( Type( User+11 ) ),
      Min( min ),
      Max( max ),
      Value( 0 ),
      TimeStr( "" )
  {
  }
  WaitEvent( int val )
    : QEvent( Type( User+12 ) ),
      Min( 0 ),
      Max( 0 ),
      Value( val ),
      TimeStr( "" )
  {
  }
  WaitEvent( const string &ts )
    : QEvent( Type( User+13 ) ),
      Min( 0 ),
      Max( 0 ),
      Value( 0 ),
      TimeStr( ts )
  {
  }

  int Min;
  int Max;
  int Value;
  string TimeStr;
};


int Wait::main( void )
{
  // get options:
  bool absdate = boolean( "absdate" );
  int hour, minutes, seconds;
  Options::time( "time", 0, hour, minutes, seconds );
  QDateTime endtime = QDateTime::currentDateTime();
  endtime.setTime( QTime( hour, minutes, seconds ) );
  if ( absdate ) {
    int year, month, day;
    Options::date( "date", 0, year, month, day );
    endtime.setDate( QDate( year, month, day ) );
  }
  else {
    int days = Options::integer( "days" );
    endtime = endtime.addDays( days );
  }

  // don't print repro message:
  noMessage();

  // don't save files:
  noSaving();

  message( "Time out at " + string( endtime.toString().toLatin1().data() ) );
  QDateTime currenttime = QDateTime::currentDateTime();
  int maxsecs = currenttime.secsTo( endtime );
  if ( maxsecs <= 0 ) {
    message( "Time out is already in the past" );
    return Failed;
  }
  QCoreApplication::postEvent( this, new WaitEvent( 0, maxsecs ) );
  string ws;
  if ( endtime.date() == currenttime.date() )
    ws = endtime.time().toString( Qt::ISODate ).toStdString();
  else
    ws = endtime.toString( Qt::ISODate ).toStdString();
  QCoreApplication::postEvent( this, new WaitEvent( ws ) );
  while ( softStop() == 0 ) {
    currenttime = QDateTime::currentDateTime();
    if ( currenttime >= endtime )
      break;
    QCoreApplication::postEvent( this, new WaitEvent( currenttime.secsTo( endtime ) ) );
    sleep( 0.1 );
    if ( interrupt() )
      return Aborted;
  }

  message( "Timed out at " + string( currenttime.toString().toLatin1().data() ) );
  return Completed;
}


void Wait::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    WaitEvent *we = dynamic_cast<WaitEvent*>( qce );
    QPB.setRange( we->Min, we->Max );
    QPB.reset();
    break;
  }
  case 12: {
    WaitEvent *we = dynamic_cast<WaitEvent*>( qce );
    QPB.setValue( QPB.maximum() - we->Value );
    int secs = we->Value;
    int mins = secs / 60;
    secs -= mins * 60;
    int hours = mins / 60;
    mins -= hours * 60;
    int days = hours / 24;
    hours -= days * 24;
    string ts = "";
    if ( days > 1 )
      ts += Str( days ) + "days, ";
    else if ( days > 0 )
      ts += Str( days ) + "day, ";
    if ( hours > 0 || days > 0 )
      ts += Str( hours ) + ":";
    ts += Str( mins, 2, '0' ) + ":";
    ts += Str( secs, 2, '0' );
    RemainingTimeLabel.setText( ts.c_str() );
    break;
  }
  case 13: {
    WaitEvent *we = dynamic_cast<WaitEvent*>( qce );
    EndTimeLabel.setText( we->TimeStr.c_str() );
    QFont nf( EndTimeLabel.font() );
    nf.setPointSize( 2 * widget()->fontInfo().pointSize() );
    nf.setBold( true );
    EndTimeLabel.setFont( nf );
    RemainingTimeLabel.setFont( nf );
    break;
  }
  default:
    RELACSPlugin::customEvent( qce );
  }
}


addRePro( Wait );

}; /* namespace base */

#include "moc_wait.cc"
