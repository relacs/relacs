/*
  base/savetraces.cc
  Saves data from selected input traces or events for each run into files

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

#include <deque>
#include <QVBoxLayout>
#include <relacs/tablekey.h>
#include <relacs/savefiles.h>
#include <relacs/base/savetraces.h>
using namespace relacs;

namespace base {


SaveTraces::SaveTraces( void )
  : RePro( "SaveTraces", "base", "Jan Benda", "1.0", "Jan 17, 2013" )
{
  // add some options:
  newSection( "General" );
  addNumber( "duration", "Duration", 1.0, 0.01, 1000.0, 0.02, "sec", "ms" );
  addInteger( "repeats", "Repeats", 0, 0, 10000, 2 );
  addBoolean( "savedata", "Save raw data", false );
  addBoolean( "split", "Save each run into a separate file", false );

  // widget:
  QVBoxLayout *vb = new QVBoxLayout;

  QLabel *label = new QLabel;
  vb->addWidget( label );

  RecordingLabel = new QLabel( "Recording:" );
  RecordingLabel->setAlignment( Qt::AlignHCenter );
  vb->addWidget( RecordingLabel );

  ElapsedTimeLabel = new QLabel;
  ElapsedTimeLabel->setAlignment( Qt::AlignHCenter );
  vb->addWidget( ElapsedTimeLabel );

  label = new QLabel;
  vb->addWidget( label );

  CommentLabel = new QLabel( "Press space to stop recording." );
  CommentLabel->setAlignment( Qt::AlignHCenter );
  vb->addWidget( CommentLabel );

  setLayout( vb );
}


void SaveTraces::preConfig( void )
{
  erase( "Analog input traces" );
  erase( "Events" );
  if ( traces().size() > 0 ) {
    newSection( "Analog input traces" );
    for ( int k=0; k<traces().size(); k++ )
      addBoolean( "trace-" + traces()[k].ident(), traces()[k].ident(), true );
  }
  int n = 0;
  for ( int k=0; k<events().size(); k++ ) {
    if ( ( events()[k].mode() &
	   ( StimulusEventMode + RestartEventMode + RecordingEventMode ) ) == 0 )
      n++;
  }
  if ( n > 0 ) {
    newSection( "Events" );
    for ( int k=0; k<events().size(); k++ ) {
      if ( ( events()[k].mode() &
	     ( StimulusEventMode + RestartEventMode + RecordingEventMode ) ) == 0 )
	addBoolean( "events-" + events()[k].ident(), events()[k].ident(), true );
    }
  }
}


class SaveTracesEvent : public QEvent
{

public:

  SaveTracesEvent( int num )
    : QEvent( Type( User+11 ) ),
      Num( num ),
      Value( 0.0 )
  {
  }
  SaveTracesEvent( double val )
    : QEvent( Type( User+12 ) ),
      Num( -1 ),
      Value( val )
  {
  }

  int Num;
  double Value;
};


int SaveTraces::main( void )
{
  // get options:
  double duration = number( "duration" );
  int repeats = integer( "repeats" );
  bool savedata = boolean( "savedata" );
  bool split = boolean( "split" );

  // don't print repro message:
  noMessage();

  // don't save files:
  if ( ! savedata )
    noSaving();

  // plot trace:
  tracePlotContinuous( duration );

  // init fonts:
  QCoreApplication::postEvent( this,
			       new SaveTracesEvent( split ? completeRuns()+1 : -1 ) );

  // open files:
  deque<int> tracenum;
  deque<ofstream*> tracefile;
  deque<int> traceindex;
  deque<double> tracetime;
  deque<TableKey> tracekey;
  for ( int k=0; k<traces().size(); k++ ) {
    if ( boolean( "trace-" + traces()[k].ident() ) ) {
      tracenum.push_back( k );
      if ( split ) {
	string fn = addPath( "savetrace-" + traces()[k].ident() +
			     '-' + Str( completeRuns()+1 ) + ".dat" );
	tracefile.push_back( new ofstream( fn.c_str() ) );
      }
      else {
	string fn = addPath( "savetrace-" + traces()[k].ident() + ".dat" );
	tracefile.push_back( new ofstream( fn.c_str(),
					   ofstream::out | ofstream::app ) );
      }
      traceindex.push_back( traces()[k].size() );
      tracetime.push_back( traces()[k].currentTime() );
      *tracefile.back() << "# trace: " << traces()[k].ident() << '\n';
      *tracefile.back() << '\n';
      tracekey.push_back( TableKey() );
      tracekey.back().addNumber( "t", "sec", "%11.6f" );
      tracekey.back().addNumber( traces()[k].ident(), traces()[k].unit(), "%11.5g" );
      tracekey.back().saveKey( *tracefile.back() );
    }
  }
  deque<int> eventsnum;
  deque<ofstream*> eventsfile;
  deque<int> eventsindex;
  double eventstime = currentTime();
  deque<TableKey> eventskey;
  deque<bool> eventssavesize;
  deque<bool> eventssavewidth;
  for ( int k=0; k<events().size(); k++ ) {
    if ( boolean( "events-" + events()[k].ident() ) ) {
      eventsnum.push_back( k );
      if ( split ) {
	string fn = addPath( "saveevents-" + events()[k].ident() +
			     '-' + Str( completeRuns()+1 ) + ".dat" );
	eventsfile.push_back( new ofstream( fn.c_str() ) );
      }
      else {
	string fn = addPath( "saveevents-" + events()[k].ident() + ".dat" );
	eventsfile.push_back( new ofstream( fn.c_str(),
					    ofstream::out | ofstream::app ) );
      }
      eventsindex.push_back( events()[k].size() );
      *eventsfile.back() << "# events: " << events()[k].ident() << '\n';
      *eventsfile.back() << '\n';
      eventskey.push_back( TableKey() );
      eventskey.back().addNumber( "t", "sec", "%11.6f" );
      eventssavesize.push_back( ( events()[k].sizeBuffer() && (events()[k].mode() & SaveFiles::SaveSize) ) );
      if ( eventssavesize.back() )
	eventskey.back().addNumber( events()[k].sizeName(), events()[k].sizeUnit(),
				    events()[k].sizeFormat() );
      eventssavewidth.push_back( events()[k].widthBuffer() && (events()[k].mode() & SaveFiles::SaveWidth) );
      if ( eventssavewidth.back() )
	eventskey.back().addNumber( events()[k].widthName(), events()[k].widthUnit(),
				    events()[k].widthFormat() );
      eventskey.back().saveKey( *eventsfile.back() );
    }
  }

  // run:
  for ( int count=0;
	( repeats <= 0 || count < repeats ) &&
	  softStop() == 0 &&
	  ! interrupt();
	count++ ) {

    sleep( duration );

    // save data:
    for ( unsigned int k=0; k<tracefile.size(); k++ ) {
      while ( traceindex[k] < traces()[tracenum[k]].size() ) {
	tracekey[k].save( *tracefile[k],
			  traces()[tracenum[k]].pos( traceindex[k] )
			  - tracetime[k], 0 );
	tracekey[k].save( *tracefile[k],
			  traces()[tracenum[k]][traceindex[k]] );
	*tracefile[k] << '\n';
	traceindex[k]++;
      }
    }
    for ( unsigned int k=0; k<eventsfile.size(); k++ ) {
      while ( eventsindex[k] < events()[eventsnum[k]].size() ) {
	eventskey[k].save( *eventsfile[k],
			   events()[eventsnum[k]][eventsindex[k]] - eventstime,
			   0 );
	if ( eventssavesize[k] )
	  eventskey[k].save( *eventsfile[k],
			     events()[eventsnum[k]].eventSize( eventsindex[k] )
			     * events()[eventsnum[k]].sizeScale() );
	if ( eventssavewidth[k] )
	  eventskey[k].save( *eventsfile[k],
			     events()[eventsnum[k]].eventWidth( eventsindex[k] )
			     * events()[eventsnum[k]].widthScale() );
	*eventsfile[k] << '\n';
	eventsindex[k]++;
      }
    }

    // update time:
    QCoreApplication::postEvent( this, new SaveTracesEvent( currentTime() -
							    eventstime ) );
  }

  // close files:
  for ( unsigned int k=0; k<tracefile.size(); k++ ) {
    *tracefile[k] << "\n\n";
    delete tracefile[k];
  }
  for ( unsigned int k=0; k<eventsfile.size(); k++ ) {
    *eventsfile[k] << "\n\n";
    delete eventsfile[k];
  }

  return Completed;
}


void SaveTraces::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    SaveTracesEvent *se = dynamic_cast<SaveTracesEvent*>( qce );
    if ( se->Num < 0 )
      RecordingLabel->setText( "Recording:" );
    else
      RecordingLabel->setText( string( "Recording " + Str( se->Num ) + ":" ).c_str() );
    QFont nf( widget()->font() );
    nf.setPointSize( 3 * widget()->fontInfo().pointSize() / 2 );
    RecordingLabel->setFont( nf );
    CommentLabel->setFont( nf );

    nf.setPointSize( 2 * widget()->fontInfo().pointSize() );
    nf.setBold( true );
    ElapsedTimeLabel->setFont( nf );
  }
  case 12: {
    SaveTracesEvent *se = dynamic_cast<SaveTracesEvent*>( qce );
    int secs = (int)::round( se->Value );
    int mins = secs / 60;
    secs -= mins * 60;
    int hours = mins / 60;
    mins -= hours * 60;
    string ts = "";
    if ( hours > 0 )
      ts += Str( hours ) + ":";
    ts += Str( mins, 2, '0' ) + ":";
    ts += Str( secs, 2, '0' );
    ElapsedTimeLabel->setText( ts.c_str() );
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( SaveTraces, base );

}; /* namespace base */

#include "moc_savetraces.cc"
