/*
  filterdetectors.cc
  Container organizing filter and event detectors.

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

#include <cmath>
#include <QKeyEvent>
#include <relacs/str.h>
#include <relacs/repros.h>
#include <relacs/filter.h>
#include <relacs/session.h>
#include <relacs/savefiles.h>
#include <relacs/relacsdevices.h>
#include <relacs/relacswidget.h>
#include <relacs/filterdetectors.h>

namespace relacs {


FilterDetectors::FilterDetectors( RELACSWidget *rw, QWidget *parent )
  : PluginTabs( Qt::Key_F, rw, parent ),
    ConfigClass( "FilterDetectors", RELACSPlugin::Core ),
    FL(),
    RestartEvents( 0 ),
    TraceInputTrace( 0 ),
    TraceInputEvent( 0 ),
    EventInputTrace( 0 ),
    EventInputEvent( 0 ),
    Menu( 0 ),
    NeedAdjust( false ),
    AdjustFlag( 0 )
{
}


FilterDetectors::~FilterDetectors( void )
{
  clear();
}


void FilterDetectors::clear( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( (*d)->FilterDetector != 0 ) {
      Plugins::destroy( (*d)->PluginName, RELACSPlugin::FilterId );
      delete (*d)->FilterDetector;
    }
    delete *d;
  }
  FL.clear();
  clearIndices();
}


void FilterDetectors::clearIndices( void )
{
  TraceInputTrace.clear();
  TraceInputEvent.clear();
  EventInputTrace.clear();
  EventInputEvent.clear();
}


void FilterDetectors::setTracesEvents( const InList &il, const EventList &el )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    (*d)->FilterDetector->setTracesEvents( il, el );
    for ( int j=0; j < (*d)->InTraces.size(); j++ ) {
      if ( (*d)->InTraces[j].source() == 0 )
	(*d)->InTraces.set( j, &(*d)->FilterDetector->trace( (*d)->InTraces[j].ident() ) );
    }
    for ( int j=0; j < (*d)->InEvents.size(); j++ ) {
      if ( (*d)->InEvents[j].source() == 0 )
	(*d)->InEvents.set( j, &(*d)->FilterDetector->events( (*d)->InEvents[j].ident() ) );
    }
    for ( int j=0; j < (*d)->OtherEvents.size(); j++ ) {
      if ( (*d)->OtherEvents[j].source() == 0 )
	(*d)->OtherEvents.set( j, &(*d)->FilterDetector->events( (*d)->OtherEvents[j].ident() ) );
    }
  }
}


void FilterDetectors::readConfig( StrQueue &sq )
{
  Options::clear();
  Options::load( sq, ":" );
}


string FilterDetectors::createFilters( void )
{
  string warning="";

  clear();

  int n = 5;
  bool taken = false;
  for ( int k=0; k<=n; k++ ) {
    // check for filter entry in options:
    Options *filteropts = 0;
    if ( ! taken && k == n ) {
      // take the options list as a single filter:
      filteropts = this;
      if ( filteropts->empty() )
	continue;
    }
    else {
      string search = "Filter" + Str( k );
      if ( Options::name() == search )
	filteropts = this;
      else {
	Options::section_iterator dp = findSection( search );
	if ( dp == Options::sectionsEnd() )
	  continue;
	filteropts = *dp;
      }
      n = k+5;
      taken = true;
    }

    // create filter instance:
    string ident = filteropts->text( "name" );
    string filter = filteropts->text( "filter" );
    Filter *fp = (Filter*)( Plugins::create( filter, RELACSPlugin::FilterId ) );
    if ( fp == 0 ) {
      warning += "<b>" + ident + "</b>: Plugin \"<b>" + filter + "</b>\" not found!<br>\n";
      continue;
    }

    fp->setRELACSWidget( RW );
    string fs = ( fp->type() & Filter::EventDetector ) ? "detector" : "filter";

    // filter parameter:
    int mode = 0;
    if ( filteropts->boolean( "save", false ) )
      mode |= SaveFiles::SaveTrace;
    if ( filteropts->boolean( "savemeanquality", false ) )
      mode |= SaveFiles::SaveMeanQuality;
    if ( filteropts->boolean( "plot", true ) )
      mode |= PlotTraceMode;
    if ( filteropts->boolean( "trigger", false ) )
      mode |= PlotTriggerMode;
    if ( filteropts->boolean( "center", false ) )
      mode |= PlotTraceCenterVertically;
    vector< string > intrace;
    Options::const_iterator ip = filteropts->find( "inputtrace" );
    if ( ip != filteropts->end() ) {
      intrace.resize( (*ip).size() );
      for ( unsigned int j=0; j<intrace.size(); j++ ) {
	intrace[j] = (*ip).text( j );
      }
    }
    else {
      warning += fs + " <b>" + ident + "</b>: no inputtrace specified!<br>\n";
      Plugins::destroy( filter, RELACSPlugin::FilterId );
      delete fp;
      continue;
    }

    vector< string > othertrace;
    Options::const_iterator op = filteropts->find( "othertrace" );
    if ( op != filteropts->end() ) {
      othertrace.resize( (*op).size() );
      for ( unsigned int j=0; j<othertrace.size(); j++ )
	othertrace[j] = (*op).text( j );
    }
    int buffersize = filteropts->integer( "buffersize", 0, 0 );
    bool storesize = filteropts->boolean( "storesize", false );
    bool storewidth = filteropts->boolean( "storewidth", false );
    string panel = filteropts->text( "panel", "" );
    int linewidth = filteropts->integer( "linewidth", 0, 1 );

    // check filter implementation:
    bool failed = false;
    {
      InList indata;
      indata.resize( 2, 10, 0.001 );
      InList outdata;
      outdata.resize( 2, 10, 0.001 );
      EventList inevents( 2, 10 );
      EventList outevents( 2, 10 );
      EventList otherevents( 2, 10 );
      EventData stimulusevents( 2, 10 );
      if ( ( fp->type() & Filter::SingleAnalogFilter ) != Filter::SingleAnalogFilter ) {
	if ( fp->init( indata[0], outdata[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( InData, InData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( indata[0], outdata[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: adjust( InData, InData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->filter( indata[0], outdata[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: filter( InData, InData ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }
      if ( ( fp->type() & Filter::MultipleAnalogFilter ) != Filter::MultipleAnalogFilter ) {
	if ( fp->init( indata, outdata ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( InList, InList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( indata, outdata ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: adjust( InList, InList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->filter( indata, outdata ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: filter( InList, InList ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }
      if ( ( fp->type() & Filter::SingleEventFilter ) != Filter::SingleEventFilter ) {
	if ( fp->init( inevents[0], outdata[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( EventData, InData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( inevents[0], outdata[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: adjust( EventData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->filter( inevents[0], outdata[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: filter( EventData, InData ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }
      if ( ( fp->type() & Filter::MultipleEventFilter ) != Filter::MultipleEventFilter ) {
	if ( fp->init( inevents, outdata ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( EventList, InList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( inevents, outdata ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: adjust( EventList, InList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->filter( inevents, outdata ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: filter( EventList, InList ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }

      if ( ( fp->type() & Filter::SingleAnalogDetector ) != Filter::SingleAnalogDetector ) {
	if ( fp->init( indata[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( InData, EventData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( indata[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( InData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->detect( indata[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: detect( InData, EventData ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }
      if ( ( fp->type() & Filter::MultipleAnalogDetector ) != Filter::MultipleAnalogDetector ) {
	if ( fp->init( indata, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( InList, EventList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( indata ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: adjust( InList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->detect( indata, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: detect( InList, EventList ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }
      if ( ( fp->type() & Filter::SingleEventDetector ) != Filter::SingleEventDetector ) {
	if ( fp->init( inevents[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( EventData, EventData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( inevents[0] ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: adjust( EventData ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->detect( inevents[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: detect( EventData, EventData ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }
      if ( ( fp->type() & Filter::MultipleEventDetector ) != Filter::MultipleEventDetector ) {
	if ( fp->init( inevents, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: init( EventList, EventList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->adjust( inevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: adjust( EventList ) function should not be implemented!<br>\n";
	  failed = true;
	}
	if ( fp->detect( inevents, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	  warning += fs + " <b>" + ident + "</b>: detect( EventList, EventList ) function should not be implemented!<br>\n";
	  failed = true;
	}
      }

      if ( ( fp->type() & Filter::MultipleTraces ) > 0 ) {
	if ( ( fp->type() & Filter::EventInput ) > 0 ) {
	  if ( fp->autoConfigure( indata[0], 0.0, 0.1 ) != INT_MIN ) {
	    warning += fs + " <b>" + ident + "</b>: indata( InData, 0.0, 0.1 ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	else {
	  if ( fp->autoConfigure( inevents[0], 0.0, 0.1 ) != INT_MIN ) {
	    warning += fs + " <b>" + ident + "</b>: autoConfigure( EventData, 0.0, 0.1 ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
      }
      else {
	if ( ( fp->type() & Filter::EventInput ) > 0 ) {
	  if ( fp->autoConfigure( indata, 0.0, 0.1 ) != INT_MIN ) {
	    warning += fs + " <b>" + ident + "</b>: autoConfiguer( InList, 0.0, 0.1 ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	else {
	  if ( fp->autoConfigure( inevents, 0.0, 0.1 ) != INT_MIN ) {
	    warning += fs + " <b>" + ident + "</b>: autoConfigure( EventList, 0.0, 0.1 ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
      }

    }
    if ( failed ) {
      Plugins::destroy( filter, RELACSPlugin::FilterId );
      delete fp;
      continue;
    }

    // take and setup filter:
    fp->setParent( this );
    fp->setIdent( ident );
    fp->setMode( mode );

    // add filter to list:
    FilterData *fd = new FilterData( fp, filter, intrace, othertrace,
             buffersize, storesize, storewidth, panel, linewidth, filteropts );
    FL.push_back( fd );

    // add detector to widget:
    if ( fp->widget() != 0 )
      addTab( fp->widget(), fp->ident().c_str() );

  }

  // check for duplicate Filter names:
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    for ( FilterList::iterator f = d+1; f != FL.end(); ++f ) {
      if ( (*d)->FilterDetector->name() == (*f)->FilterDetector->name() )
	(*f)->FilterDetector->setLongUniqueName();
    }
  }

  return warning;
}


int FilterDetectors::traces( void )
{
  int n = 0;

  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( !((*d)->FilterDetector->type() & Filter::EventDetector) )
      n += (*d)->NOut;
  }

  return n;
}


int FilterDetectors::events( void )
{
  int n = 0;

  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( (*d)->FilterDetector->type() & Filter::EventDetector )
      n += (*d)->NOut;
  }

  return n;
}


void FilterDetectors::createStimulusEvents( EventList &events,
					    deque< PlotEventStyle > &eventstyles )
{
  events.back().setMode( events.back().mode() | SaveFiles::SaveTrace | PlotTraceMode );

  eventstyles.push_back( PlotEventStyle() );
  eventstyles.back().setPanel( -1 );
  eventstyles.back().setLine( Plot::White, 2 );
  eventstyles.back().setPoint( Plot::StrokeUp, 1, Plot::White );
  eventstyles.back().setYPos( 0.0 );
  eventstyles.back().setSize( 1.0 );

  EventInputTrace.resize( EventInputTrace.size() + 1, -1 );
  EventInputEvent.resize( EventInputEvent.size() + 1, -1 );
}


void FilterDetectors::createRestartEvents( EventList &events,
					   deque< PlotEventStyle > &eventstyles )
{
  events.back().setMode( events.back().mode() | SaveFiles::SaveTrace | PlotTraceMode );
  RestartEvents = &events.back();
  
  eventstyles.push_back( PlotEventStyle() );
  eventstyles.back().setPanel( -1 );
  eventstyles.back().setLine( Plot::Orange, 1 );
  eventstyles.back().setPoint( Plot::TriangleNorth, 1, Plot::Orange, Plot::Orange );
  eventstyles.back().setYPos( 1.0 );
  eventstyles.back().setSize( 0.07 );

  EventInputTrace.resize( EventInputTrace.size() + 1, -1 );
  EventInputEvent.resize( EventInputEvent.size() + 1, -1 );
}


void FilterDetectors::createRecordingEvents( InList &data, EventList &events,
					     deque< PlotEventStyle > &eventstyles )
{
  EventData e( 6000, 0.0, 0.0, data[0].sampleInterval(),
	       false, false );
  e.setCyclic();
  e.setSource( 0 );
  e.setMode( SaveFiles::SaveTrace | PlotTraceMode | RecordingEventMode );
  e.setIdent( "Recording" );
  e.setWriteBufferCapacity( 1000 );
  events.push( e );

  eventstyles.push_back( PlotEventStyle() );
  eventstyles.back().setPanel( -1 );
  eventstyles.back().setLine( Plot::Red, 4 );
  eventstyles.back().setPoint( Plot::StrokeUp, 1, Plot::Red );
  eventstyles.back().setYPos( 0.0 );
  eventstyles.back().setSize( 1.0 );

  EventInputTrace.resize( EventInputTrace.size() + 1, -1 );
  EventInputEvent.resize( EventInputEvent.size() + 1, -1 );
}


string FilterDetectors::createTracesEvents( InList &data, EventList &events,
					    deque< PlotTraceStyle > &tracestyles,
					    deque< PlotEventStyle > &eventstyles )
{
  string warning = "";

  TraceInputTrace.resize( TraceInputTrace.size() + data.size() + FilterDetectors::traces(), -1 );
  TraceInputEvent.resize( TraceInputEvent.size() + data.size() + FilterDetectors::traces(), -1 );
  EventInputTrace.resize( EventInputTrace.size() + FilterDetectors::events(), -1 );
  EventInputEvent.resize( EventInputEvent.size() + FilterDetectors::events(), -1 );

  int dk = data.size();    // TraceInputTrace.size() and TraceInputEvent.size() was zero anyways.
  int ek = events.size();  // should be the same as EventInputTrace.size() and EventInputEvent.size() !!!

  int maxpanel = data.size();

  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {

    (*d)->InTraces.clear();
    (*d)->InEvents.clear();
    (*d)->OutTraces.clear();
    (*d)->OutEvents.clear();
    (*d)->OtherEvents.clear();

    string fs = ( (*d)->FilterDetector->type() & Filter::EventDetector ) ? "detector" : "filter";

    // inputs:
    if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
      // input events:
      bool store = true;
      for ( unsigned int j=0; j < (*d)->In.size(); j++ ) {
	int einx = events.index( (*d)->In[j] );
	if ( einx < 0 )
	  einx = (int)::rint( Str( (*d)->In[j] ).number( -1.0 ) );
	if ( einx >= 0 && einx < events.size() ) {
	  (*d)->InEvents.add( &events[einx] );
	  if ( store ) {
	    if ( (*d)->FilterDetector->type() & Filter::EventDetector ) {
	      EventInputTrace[ek] = eventInputTrace( einx );
	      EventInputEvent[ek] = einx;
	    }
	    else {
	      TraceInputTrace[dk] = eventInputTrace( einx );
	      TraceInputEvent[dk] = einx;
	    }
	    store = false;
	  }
	}
	else
	  warning += "Invalid input event index <b>" + (*d)->In[j]
	    + "</b> in " + fs + " <b>" + (*d)->FilterDetector->ident() + "</b><br>";
      }
    }
    else {
      // input traces:
      bool store = true;
      for ( unsigned int j=0; j < (*d)->In.size(); j++ ) {
	int dinx = data.index( (*d)->In[j] );
	if ( dinx < 0 )
	  dinx = (int)::rint( Str( (*d)->In[j] ).number( -1.0 ) );
	if ( dinx >= 0 && dinx < data.size() ) {
	  (*d)->InTraces.add( &data[dinx] );
	  if ( store ) {
	    if ( (*d)->FilterDetector->type() & Filter::EventDetector ) {
	      EventInputTrace[ek] = dinx;
	      EventInputEvent[ek] = -1;
	    }
	    else {
	      TraceInputTrace[dk] = dinx;
	      TraceInputEvent[dk] = -1;
	    }
	    store = false;
	  }
	}
	else
	  warning += "Invalid input trace index <b>" + (*d)->In[j]
	    + "</b> in " + fs + " <b>" + (*d)->FilterDetector->ident() + "</b><br>";
      }
    }
    
    if ( ! warning.empty() )
      continue;

    if ( (*d)->FilterDetector->type() & Filter::EventDetector ) {
      // detector:

      (*d)->Out = ek;

      // assemble output events:
      for ( int j=0; j < (*d)->NOut; j++ ) {
	int intrace = EventInputTrace[ek];
	if ( intrace < 0 )
	  intrace = 0;
	EventData e( 0, 0.0, 0.0, data[intrace].sampleInterval() );
	e.setCyclic();
	events.push( e );
	(*d)->OutEvents.add( &events[(*d)->Out+j] );

	// this is not really good yet...
	eventstyles.push_back( PlotEventStyle() );
	// eventstyles.back().setPanel( eventInputTrace( (*d)->Out+j ) );
	eventstyles.back().setPanel( -1 );  // default: to trace
	eventstyles.back().setLine( Plot::Red, 1 );
	eventstyles.back().setPoint( Plot::TriangleUp, 1, Plot::Red, Plot::Red );
	eventstyles.back().setYPos( 0.1 );
	eventstyles.back().setSize( 6.0, Plot::Pixel );
      }

      // assemble other events:
      for ( unsigned int j=0; j < (*d)->Other.size(); j++ ) {
	int oinx = events.index( (*d)->Other[j] );
	if ( oinx < 0 )
	  oinx = (int)::rint( Str( (*d)->Other[j] ).number( -1.0 ) );
	if ( oinx >= 0 && oinx < events.size() )
	  (*d)->OtherEvents.add( &events[oinx] );
	else if ( (*d)->Other[j] != "-1" )
	  warning += "Invalid other event trace index <b>" + (*d)->Other[j]
	    + "</b> in detector <b>" + (*d)->FilterDetector->ident() + "</b><br>";
      }

      // setup output events:
      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	int inevent = EventInputEvent[ek];
	if ( inevent < 0 )
	  inevent = 0;
	int nbuffer = (*d)->NBuffer;
	if ( nbuffer <= 0 )
	  nbuffer = events[inevent].capacity();
	// single / multiple event traces -> multiple event traces
	for ( int i=0; i<(*d)->NOut; i++, ek++ ) {
	  events[ek].setSizeBuffer( (*d)->SizeBuffer );
	  events[ek].setWidthBuffer( (*d)->WidthBuffer );
	  events[ek].reserve( nbuffer );
	  events[ek].setWriteBufferCapacity( nbuffer/10 );
	  events[ek].setSource( 2 );
	  events[ek].setMode( (*d)->FilterDetector->mode() );
	  if ( (*d)->FilterDetector->type() & Filter::MultipleTraces )
	    events[ek].setIdent( (*d)->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    events[ek].setIdent( (*d)->FilterDetector->ident() );
	  events[ek].setSizeUnit( (*d)->InEvents[0].sizeUnit() );
	}
      }
      else {
	int intrace = EventInputTrace[ek];
	if ( intrace < 0 )
	  intrace = 0;
	int nbuffer = (*d)->NBuffer;
	if ( nbuffer <= 0 )
	  nbuffer = data[intrace].capacity();
	// single / multiple analog -> multiple event traces
	for ( int i=0; i<(*d)->NOut; i++, ek++ ) {
	  events[ek].setSizeBuffer( (*d)->SizeBuffer );
	  events[ek].setWidthBuffer( (*d)->WidthBuffer );
	  events[ek].reserve( nbuffer );
	  events[ek].setWriteBufferCapacity( nbuffer/10 );
	  events[ek].setSource( 1 );
	  events[ek].setMode( (*d)->FilterDetector->mode() );
	  if ( (*d)->FilterDetector->type() & Filter::MultipleTraces )
	    events[ek].setIdent( (*d)->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    events[ek].setIdent( (*d)->FilterDetector->ident() );
	  events[ek].setSizeUnit( (*d)->InTraces[0].unit() );
	}
      }
    }
    else {
      // filter:

      (*d)->Out = dk;

      // assemble output traces:
      for ( int j=0; j < (*d)->NOut; j++ ) {
	int intrace = TraceInputTrace[dk];
	if ( intrace < 0 )
	  intrace = 0;
	int nbuffer = (*d)->NBuffer;
	if ( nbuffer <= 0 )
	  nbuffer = data[intrace].capacity();
	InData dt( nbuffer, data[intrace].writeBufferCapacity(),
		   data[intrace].sampleInterval() );
	dt.setDevice( -1 );
	dt.setChannel( -1 );
	data.push( dt );
	(*d)->OutTraces.add( &data[(*d)->Out+j] );

	tracestyles.push_back( PlotTraceStyle() );
	int p = -1;
	if ( ! (*d)->PanelTrace.empty() )
	  p = data.index( (*d)->PanelTrace );
	if ( p >= 0 && p < (int)tracestyles.size()-1 ) {
	  // add to existing panel:
	  p = tracestyles[p].panel();
	  // count traces in panel:
	  int cp = 0;
	  for ( unsigned int c=0; c<tracestyles.size(); c++ ) {
	    if ( tracestyles[c].panel() == p )
	      cp++;
	  }
	  tracestyles.back().setPanel( p );
	  if ( cp == 0 )
	    tracestyles.back().setLine( Plot::Green, (*d)->LineWidth );
	  else if ( cp == 1 )
	    tracestyles.back().setLine( Plot::DarkOrange, (*d)->LineWidth );
	  else
	    tracestyles.back().setLine( Plot::Cyan, (*d)->LineWidth );
	}
	else {
	  // extra panel:
	  tracestyles.back().setPanel( maxpanel );
	  tracestyles.back().setLine( Plot::Green, (*d)->LineWidth );
	  maxpanel++;
	}
      }

      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	// single/multiple event traces -> multiple traces
	for ( int i=0; i<(*d)->NOut; i++, dk++ ) {
	  data[dk].setSource( 2 );
	  data[dk].setMode( (*d)->FilterDetector->mode() );
	  if ( (*d)->FilterDetector->type() & Filter::MultipleTraces )
	    data[dk].setIdent( (*d)->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    data[dk].setIdent( (*d)->FilterDetector->ident() );
	  data[dk].setUnit( (*d)->InEvents[0].sizeUnit() );
	}
      }
      else {
	// single / multiple analog -> multiple traces
	for ( int i=0; i<(*d)->NOut; i++, dk++ ) {
	  data[dk].setSource( 1 );
	  data[dk].setMode( (*d)->FilterDetector->mode() );
	  if ( (*d)->FilterDetector->type() & Filter::MultipleTraces )
	    data[dk].setIdent( (*d)->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    data[dk].setIdent( (*d)->FilterDetector->ident() );
	  data[dk].setUnit( (*d)->InTraces[0].unit() );
	}
      }


    }

  }

  return warning;
}


string FilterDetectors::init( void )
{
  string warning = "";

  // init filter and event detectors:
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    (*d)->FilterDetector->lock();
    (*d)->FilterDetector->setSettings();
    (*d)->Init = true;
    string ident = (*d)->FilterDetector->ident();
    const EventData &stimulusevents = (*d)->FilterDetector->stimulusEvents();

    if ( (*d)->FilterDetector->type() & Filter::EventDetector ) {
      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single event trace
	if ( (*d)->FilterDetector->type() == Filter::SingleEventDetector ) {
	  if ( (*d)->FilterDetector->init( (*d)->InEvents[0], (*d)->OutEvents[0], 
					(*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: init( EventData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InEvents[0] );
	  }
	}
	// multiple event traces -> multiple event traces
	else {
	  if ( (*d)->FilterDetector->init( (*d)->InEvents, (*d)->OutEvents, 
					(*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: init( EventList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InEvents );
	  }
	}
      }
      else {
	// single analog -> single event trace
	if ( (*d)->FilterDetector->type() == Filter::SingleAnalogDetector ) {
	  if ( (*d)->FilterDetector->init( (*d)->InTraces[0], (*d)->OutEvents[0],
					(*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: init( InData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InTraces[0] );
	  }
	}
	// multiple analog -> multiple event traces
	else {
	  if ( (*d)->FilterDetector->init( (*d)->InTraces, (*d)->OutEvents, 
					(*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: init( InList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InTraces );
	  }
	}
      }
    }
    else {
      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single trace
	if ( (*d)->FilterDetector->type() == Filter::SingleEventFilter ) {
	  if ( (*d)->FilterDetector->init( (*d)->InEvents[0], (*d)->OutTraces[0] ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: init( EventData, InData ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InEvents[0], (*d)->OutTraces[0] );
	  }
	}
	// multiple event traces -> multiple traces
	else {
	  if ( (*d)->FilterDetector->init( (*d)->InEvents, (*d)->OutTraces ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: init( EventList, InList ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InEvents, (*d)->OutTraces );
	  }
	}
      }
      else {
	// single analog trace -> single trace
	if ( (*d)->FilterDetector->type() == Filter::SingleAnalogFilter ) {
	  if ( (*d)->FilterDetector->init( (*d)->InTraces[0], (*d)->OutTraces[0] ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: init( InData, EventData ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InTraces[0], (*d)->OutTraces[0] );
	  }
	}
	// multiple analog traces -> multiple traces
	else {
	  if ( (*d)->FilterDetector->init( (*d)->InTraces, (*d)->OutTraces ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: init( InList, EventList ) function must be implemented!<br>\n";
	  else {
	    (*d)->Init = false;
	    (*d)->FilterDetector->adjust( (*d)->InTraces, (*d)->OutTraces );
	  }
	}
      }
    }
    (*d)->FilterDetector->unlock();

  }

  return warning;
}


void FilterDetectors::setAdjustFlag( int flag )
{
  AdjustFlag = flag;
}


void FilterDetectors::scheduleAdjust( void )
{
  AdjustMutex.lock();
  NeedAdjust = true;
  AdjustMutex.unlock();
}


void FilterDetectors::adjust( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {

    (*d)->FilterDetector->lock();
    if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
      // check if this filter really has to be adjusted:
      bool c = false;
      if ( AdjustFlag >= 0 ) {
	for ( int k=0; k<(*d)->InEvents.size(); k++ ) {
	  if ( (*d)->InEvents[k].mode() & AdjustFlag ) {
	    c = true;
	    break;
	  }
	}
      }
      else
	c = true;
      // adjust:
      if ( c ) {
	if ( (*d)->FilterDetector->type() & Filter::MultipleTraces ) {
	  if ( (*d)->FilterDetector->type() & Filter::EventDetector )
	    (*d)->FilterDetector->adjust( (*d)->InEvents );
	  else
	    (*d)->FilterDetector->adjust( (*d)->InEvents, (*d)->OutTraces );
	}
	else {
	  if ( (*d)->FilterDetector->type() & Filter::EventDetector )
	    (*d)->FilterDetector->adjust( (*d)->InEvents[0] );
	  else
	    (*d)->FilterDetector->adjust( (*d)->InEvents[0], (*d)->OutTraces[0] );
	}
      }
    }
    else {
      // check if this filter really has to be adjusted:
      bool c = false;
      if ( AdjustFlag >= 0 ) {
	for ( int k=0; k<(*d)->InTraces.size(); k++ ) {
	  if ( (*d)->InTraces[k].mode() & AdjustFlag ) {
	    c = true;
	    break;
	  }
	}
      }
      else
	c = true;
      // adjust:
      if ( c ) {
	if ( (*d)->FilterDetector->type() & Filter::MultipleTraces ) {
	  if ( (*d)->FilterDetector->type() & Filter::EventDetector )
	    (*d)->FilterDetector->adjust( (*d)->InTraces );
	  else
	    (*d)->FilterDetector->adjust( (*d)->InTraces, (*d)->OutTraces );
	}
	else {
	  if ( (*d)->FilterDetector->type() & Filter::EventDetector )
	    (*d)->FilterDetector->adjust( (*d)->InTraces[0] );
	  else
	    (*d)->FilterDetector->adjust( (*d)->InTraces[0], (*d)->OutTraces[0] );
	}
      }
    }
    (*d)->FilterDetector->unlock();
  }

  AdjustMutex.lock();
  NeedAdjust = false;
  AdjustMutex.unlock();
}


void FilterDetectors::autoConfigure( void )
{
  autoConfigure( 1.0 );
}


void FilterDetectors::autoConfigure( double duration )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->autoConfigure( duration );
}


void FilterDetectors::autoConfigure( double tbegin, double tend )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->autoConfigure( tbegin, tend );
}


void FilterDetectors::autoConfigure( Filter *f, double duration )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( (*d)->FilterDetector == f ) {
      (*d)->autoConfigure( duration );
      break;
    }
  }
}


void FilterDetectors::autoConfigure( Filter *f, double tbegin, double tend )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( (*d)->FilterDetector == f ) {
      (*d)->autoConfigure( tbegin, tend );
      break;
    }
  }
}


void FilterDetectors::save( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->save();
}


string FilterDetectors::filter( double signaltime )
{
  // adjust necessary?
  AdjustMutex.lock();
  bool adj = NeedAdjust;
  AdjustMutex.unlock();
  if ( adj )
    adjust();

  string warning = "";

  // filter and detect events:
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {

    if ( signaltime >= 0.0 ) {
      (*d)->OutEvents.setSignalTime( signaltime );
      (*d)->OutTraces.setSignalTime( signaltime );
      if ( RestartEvents != 0 && ! RestartEvents->empty() )
	(*d)->OutTraces.setRestartTime( RestartEvents->back() );
    }

    (*d)->FilterDetector->updateDerivedTracesEvents();

    string ident = (*d)->FilterDetector->ident();
    const EventData &stimulusevents = (*d)->FilterDetector->stimulusEvents();

    (*d)->FilterDetector->lock();
    if ( (*d)->FilterDetector->type() & Filter::EventDetector ) {
      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single event trace
	if ( (*d)->FilterDetector->type() == Filter::SingleEventDetector ) {
	  if ( (*d)->Init ) {
	    (*d)->Init = false;
	    (*d)->FilterDetector->init( (*d)->InEvents[0], (*d)->OutEvents[0], 
				     (*d)->OtherEvents, stimulusevents );
	  }
	  if ( (*d)->FilterDetector->detect( (*d)->InEvents[0], (*d)->OutEvents[0], 
					  (*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: detect( EventData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    (*d)->OutEvents.setRangeBack( (*d)->InEvents[0].rangeBack() );
	}
	// multiple event traces -> multiple event traces
	else {
	  if ( (*d)->Init ) {
	    (*d)->Init = false;
	    (*d)->FilterDetector->init( (*d)->InEvents, (*d)->OutEvents, 
				     (*d)->OtherEvents, stimulusevents );
	  }
	  if ( (*d)->FilterDetector->detect( (*d)->InEvents, (*d)->OutEvents, 
					  (*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: detect( EventList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    (*d)->OutEvents.setRangeBack( (*d)->InEvents[0].rangeBack() );
	}
      }
      else {
	// single analog -> single event trace
	if ( (*d)->FilterDetector->type() == Filter::SingleAnalogDetector ) {
	  if ( (*d)->Init ) {
	    (*d)->FilterDetector->init( (*d)->InTraces[0], (*d)->OutEvents[0], 
				     (*d)->OtherEvents, stimulusevents );
	    (*d)->Init = false;
	  }
	  if ( (*d)->FilterDetector->detect( (*d)->InTraces[0], (*d)->OutEvents[0], 
					  (*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: detect( InData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    (*d)->OutEvents.setRangeBack( (*d)->InTraces[0].currentTime() );
	}
	// multiple analog -> multiple event traces
	else {
	  if ( (*d)->Init ) {
	    (*d)->Init = false;
	    (*d)->FilterDetector->init( (*d)->InTraces, (*d)->OutEvents, 
				     (*d)->OtherEvents, stimulusevents );
	  }
	  if ( (*d)->FilterDetector->detect( (*d)->InTraces, (*d)->OutEvents, 
					  (*d)->OtherEvents, stimulusevents ) == INT_MIN )
	    warning += "detector <b>" + ident + "</b>: detect( InList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    (*d)->OutEvents.setRangeBack( (*d)->InTraces.currentTime() );
	}
      }
    }
    else {
      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single trace
	if ( (*d)->FilterDetector->type() == Filter::SingleEventFilter ) {
	  if ( (*d)->Init ) {
	    (*d)->Init = false;
	    (*d)->FilterDetector->init( (*d)->InEvents[0], (*d)->OutTraces[0] );
	  }
	  if ( (*d)->FilterDetector->filter( (*d)->InEvents[0], (*d)->OutTraces[0] ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: filter( EventData, InData ) function must be implemented!<br>\n";
	}
	// multiple event traces -> multiple traces
	else {
	  if ( (*d)->Init ) {
	    (*d)->Init = false;
	    (*d)->FilterDetector->init( (*d)->InEvents, (*d)->OutTraces );
	  }
	  if ( (*d)->FilterDetector->filter( (*d)->InEvents, (*d)->OutTraces ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: filter( EventList, InList ) function must be implemented!<br>\n";
	}
      }
      else {
	// single analog -> single trace
	if ( (*d)->FilterDetector->type() == Filter::SingleAnalogFilter ) {
	  if ( (*d)->Init ) {
	    (*d)->FilterDetector->init( (*d)->InTraces[0], (*d)->OutTraces[0] );
	    (*d)->Init = false;
	  }
	  if ( (*d)->FilterDetector->filter( (*d)->InTraces[0], (*d)->OutTraces[0] ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: filter( InData, InData ) function must be implemented!<br>\n";
	}
	// multiple analog -> multiple traces
	else {
	  if ( (*d)->Init ) {
	    (*d)->Init = false;
	    (*d)->FilterDetector->init( (*d)->InTraces, (*d)->OutTraces );
	  }
	  if ( (*d)->FilterDetector->filter( (*d)->InTraces, (*d)->OutTraces ) == INT_MIN )
	    warning += "filter <b>" + ident + "</b>: filter( InList, InList ) function must be implemented!<br>\n";
	}
      }
    }
    
    (*d)->FilterDetector->unlock();

  }

  return warning;  
}


Filter *FilterDetectors::filter( int index )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    if ( index >= (*d)->Out &&
	 index < (*d)->Out + (*d)->NOut )
      return (*d)->FilterDetector;

  return 0;
}


Filter *FilterDetectors::filter( const string &ident )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( (*d)->FilterDetector->ident() == ident )
      return (*d)->FilterDetector;
  }
  return 0;
}


Filter *FilterDetectors::detector( int index )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    if ( index >= (*d)->Out &&
	 index < (*d)->Out + (*d)->NOut )
      return (*d)->FilterDetector;

  return 0;
}


Filter *FilterDetectors::detector( const string &ident )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( (*d)->FilterDetector->ident() == ident )
      return (*d)->FilterDetector;
  }
  return 0;
}


bool FilterDetectors::exist( const string &ident ) const
{
  for ( FilterList::const_iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( (*d)->FilterDetector->ident() == ident )
      return true;
  }
  return false;
}


int FilterDetectors::traceInputTrace( int trace ) const
{
  if ( trace < 0 || trace >= (int)TraceInputTrace.size() )
    return -1;

  return TraceInputTrace[ trace ];
}


int FilterDetectors::traceInputTrace( const string &ident ) const
{
  for ( int trace=0; 
	trace < RW->EData.size() && trace < (int)TraceInputTrace.size();
	trace++ ) {
    if ( RW->IData[trace].ident() == ident )
      return TraceInputTrace[ trace ];
  }
  return -1;
}


int FilterDetectors::traceInputEvent( int trace ) const
{
  if ( trace < 0 || trace >= (int)TraceInputEvent.size() )
    return -1;

  return TraceInputEvent[ trace ];
}


int FilterDetectors::traceInputEvent( const string &ident ) const
{
  for ( int trace=0; 
	trace < RW->IData.size() && trace < (int)TraceInputEvent.size();
	trace++ ) {
    if ( RW->IData[trace].ident() == ident )
      return TraceInputEvent[ trace ];
  }
  return -1;
}


int FilterDetectors::eventInputTrace( int trace ) const
{
  if ( trace < 0 || trace >= (int)EventInputTrace.size() )
    return -1;

  return EventInputTrace[ trace ];
}


int FilterDetectors::eventInputTrace( const string &ident ) const
{
  for ( int event=0; 
	event < RW->EData.size() && event < (int)EventInputTrace.size();
	event++ ) {
    if ( RW->EData[event].ident() == ident )
      return EventInputTrace[ event ];
  }
  return -1;
}


int FilterDetectors::eventInputEvent( int event ) const
{
  if ( event < 0 || event >= (int)EventInputEvent.size() )
    return -1;

  return EventInputEvent[ event ];
}


int FilterDetectors::eventInputEvent( const string &ident ) const
{
  for ( int event=0; 
	event < RW->EData.size() && event < (int)EventInputEvent.size();
	event++ ) {
    if ( RW->EData[event].ident() == ident )
      return EventInputEvent[ event ];
  }
  return -1;
}


void FilterDetectors::addMenu( QMenu *menu, bool doxydoc )
{
  if ( Menu == 0 )
    Menu = menu;
  Menu->clear();

  Menu->addAction( "&Auto configure", this, SLOT( autoConfigure() ), Qt::Key_A );
  Menu->addAction( "&Save", this, SLOT( save() ) );
  for ( unsigned int k=0; k<FL.size(); k++ ) {
    string s = "&";
    if ( k == 0 )
      s += '0';
    else if ( k < 10 )
      s += ( '1' + k - 1 );
    else
      s += ( 'a' + k - 10 );
    s += " ";
    s += FL[k]->FilterDetector->ident();
    QMenu *pop = Menu->addMenu( s.c_str() );
    pop->addAction( "&Options...", FL[k]->FilterDetector, SLOT( dialog() ) );
    pop->addAction( "&Help...", FL[k]->FilterDetector, SLOT( help() ) );
    pop->addAction( "&Auto configure...", FL[k], SLOT( autoConfigure() ) );
    pop->addAction( "&Save...", FL[k], SLOT( save() ) );
    pop->addAction( "S&creenshot", FL[k]->FilterDetector, SLOT( saveWidget() ) );
    if ( doxydoc )
      pop->addAction( "&Doxygen", FL[k]->FilterDetector, SLOT( saveDoxygenOptions() ) );
  }
}


void FilterDetectors::modeChanged( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->FilterDetector->modeChanged();
}


void FilterDetectors::notifyMetaData( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->FilterDetector->notifyMetaData();
}


void FilterDetectors::notifyStimulusData( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->FilterDetector->notifyStimulusData();
}


void FilterDetectors::sessionStarted( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->FilterDetector->sessionStarted();
}


void FilterDetectors::sessionStopped( bool saved )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    (*d)->FilterDetector->sessionStopped( saved );
}


ostream &operator<<( ostream &str, const FilterDetectors &fd )
{
  for ( FilterDetectors::FilterList::const_iterator d = fd.FL.begin(); 
	d != fd.FL.end(); 
	++d ) {
    if ( (*d)->FilterDetector->type() & Filter::EventDetector ) {
      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single event trace
	if ( (*d)->FilterDetector->type() == Filter::SingleEventDetector ) {
	  str << "Event Detector 1-1:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
	// multiple event traces -> multiple event traces
	else {
	  str << "Event Detector n-n:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
      }
      else {
	// single analog -> single event trace
	if ( (*d)->FilterDetector->type() == Filter::SingleAnalogDetector ) {
	  str << "Analog Detector 1-1:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
	// multiple analog -> multiple event traces
	else {
	  str << "Analog Detector n-n:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
      }
    }
    else {
      if ( (*d)->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single analog trace
	if ( (*d)->FilterDetector->type() == Filter::SingleEventFilter ) {
	  str << "Event Filter 1-1:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
	// multiple event traces -> multiple analog traces
	else {
	  str << "Event Filter n-n:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
      }
      else {
	// single analog -> single analog trace
	if ( (*d)->FilterDetector->type() == Filter::SingleAnalogFilter ) {
	  str << "Analog Filter 1-1:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
	// multiple analog -> multiple traces
	else {
	  str << "Analog Filter n-n:" << '\n';
	  (*d)->print( str );
	  str << '\n';
	}
      }
    }
  }

  return str;
}


void FilterDetectors::keyPressEvent( QKeyEvent *event )
{
  PluginTabs::keyPressEvent( event );

  if ( ! event->isAccepted() ) {  
    for ( FilterList::iterator d = FL.begin();
	  d != FL.end() && ! event->isAccepted();
	  ++d ) {
      if ( (*d)->FilterDetector != 0 &&
	   (*d)->FilterDetector->globalKeyEvents() &&
	   (*d)->FilterDetector->widget() != 0 )
	QCoreApplication::sendEvent( (*d)->FilterDetector->widget(), event );
    }
  }
}


void FilterDetectors::keyReleaseEvent( QKeyEvent *event )
{
  for ( FilterList::iterator d = FL.begin();
	d != FL.end() && ! event->isAccepted();
	++d ) {
    if ( (*d)->FilterDetector != 0 &&
	 (*d)->FilterDetector->globalKeyEvents() &&
	 (*d)->FilterDetector->widget() != 0 )
      QCoreApplication::sendEvent( (*d)->FilterDetector->widget(), event );
  }
}


FilterData::FilterData( Filter *filter,
			const string &pluginname,
			const vector<string> &in,
			const vector<string> &other,
			long n, bool size, bool width,
      const string &panel, int linewidth,
      Options* generalOptions)
  : PluginName( pluginname ), In( in ), Other( other ), 
    InTraces(), InEvents(), OutTraces(), OutEvents(), OtherEvents(),
    NBuffer( n ), SizeBuffer( size ), WidthBuffer( width ),
    PanelTrace( panel ), LineWidth( linewidth ), Init( true ),
    GeneralOptions(generalOptions)
{
  FilterDetector = filter;
  NOut = filter->outTraces();
  if ( NOut == 0 )
    NOut = in.size();
}


FilterData::FilterData( const FilterData &fd )
  : PluginName( fd.PluginName ), In( fd.In ), Other( fd.Other ), 
    InTraces(), InEvents( fd.InEvents ), OutTraces( fd.OutTraces ),
    OutEvents( fd.OutEvents ), OtherEvents( fd.OtherEvents ),
    NBuffer( fd.NBuffer ), SizeBuffer( fd.SizeBuffer ),
    WidthBuffer( fd.WidthBuffer ),
    PanelTrace( fd.PanelTrace ), LineWidth( fd.LineWidth ), Init( fd.Init ),
    GeneralOptions(fd.GeneralOptions)
{
  FilterDetector = fd.FilterDetector;
  Out = fd.Out;
  NOut = fd.NOut;
}


FilterData::~FilterData( void )
{
}


void FilterData::autoConfigure( void )
{
  autoConfigure( 1.0 );
}


void FilterData::autoConfigure( double duration )
{
  FilterDetector->lock();
  if ( FilterDetector->type() & Filter::EventInput ) {
    if ( InEvents.size() > 0 ) {
      double tend = InEvents[0].rangeBack();
      double tbegin = tend - duration;
      if ( FilterDetector->type() & Filter::MultipleTraces )
	FilterDetector->autoConfigure( InEvents, tbegin, tend );
      else
	FilterDetector->autoConfigure( InEvents[0], tbegin, tend );
    }
  }
  else {
    if ( InTraces.size() > 0 ) {
      double tend = InTraces.currentTime();
      double tbegin = tend - duration;
      if ( tbegin < InTraces[0].minTime() )
	tbegin = InTraces[0].minTime();
      if ( FilterDetector->type() & Filter::MultipleTraces )
	FilterDetector->autoConfigure( InTraces, tbegin, tend );
      else
	FilterDetector->autoConfigure( InTraces[0], tbegin, tend );
    }
  }
  FilterDetector->unlock();
}


void FilterData::autoConfigure( double tbegin, double tend )
{
  FilterDetector->lock();
  if ( FilterDetector->type() & Filter::EventInput ) {
    if ( InEvents.size() > 0 ) {
      if ( FilterDetector->type() & Filter::MultipleTraces )
	FilterDetector->autoConfigure( InEvents, tbegin, tend );
      else
	FilterDetector->autoConfigure( InEvents[0], tbegin, tend );
    }
  }
  else {
    if ( InTraces.size() > 0 ) {
      if ( FilterDetector->type() & Filter::MultipleTraces )
	FilterDetector->autoConfigure( InTraces, tbegin, tend );
      else
	FilterDetector->autoConfigure( InTraces[0], tbegin, tend );
    }
  }
  FilterDetector->unlock();
}


void FilterData::save( void )
{
  FilterDetector->save();
}


void FilterData::print( ostream &str ) const
{
  str << "      ident: " << FilterDetector->ident() << '\n'
      << " pluginname: " << PluginName << '\n'
      << "         in: " << In[0];
  for ( unsigned int k=1; k<In.size(); k++ )
    str << ", " << In[k];
  str << '\n'
      << "      other: ";
  if ( Other.size() > 0 ) {
    str << Other[0];
    for ( unsigned int k=1; k<Other.size(); k++ )
      str << ", " << Other[k];
  }
  else
    str << "-";
  str << '\n'
      << "        out: " << Out << '\n'
      << "       nout: " << NOut << '\n'
      << "    nbuffer: " << NBuffer << '\n'
      << " sizebuffer: " << SizeBuffer << '\n'
      << "widthbuffer: " << WidthBuffer << '\n'
      << " paneltrace: " << PanelTrace << '\n'
      << "  linewidth: " << LineWidth << '\n'
      << "       init: " << Init << '\n'
      << "   detector: " << FilterDetector << endl;
}



}; /* namespace relacs */

#include "moc_filterdetectors.cc"

