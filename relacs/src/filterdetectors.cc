/*
  filterdetectors.cc
  Container organizing filter and event detectors.

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

#include <cmath>
#include <QKeyEvent>
#include <relacs/str.h>
#include <relacs/repros.h>
#include <relacs/filter.h>
#include <relacs/session.h>
#include <relacs/relacsdevices.h>
#include <relacs/relacswidget.h>
#include <relacs/filterdetectors.h>

namespace relacs {


FilterDetectors::FilterDetectors( RELACSWidget *rw, QWidget *parent )
  : QTabWidget( parent ),
    ConfigClass( "FilterDetectors", RELACSPlugin::Core ),
    FL(),
    TraceInputTrace( 0 ),
    TraceInputEvent( 0 ),
    EventInputTrace( 0 ),
    EventInputEvent( 0 ),
    StimulusEvents( 0 ),
    Menu( 0 ),
    NeedAdjust( false ),
    AdjustFlag( 0 ),
    RW( rw )
{
}


FilterDetectors::~FilterDetectors( void )
{
  clear();
}


void FilterDetectors::clear( void )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( d->FilterDetector != 0 ) {
      Plugins::destroy( d->PluginName, RELACSPlugin::FilterId );
      delete d->FilterDetector;
    }
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


void FilterDetectors::readConfig( StrQueue &sq )
{
  Options::clear();
  Options::load( sq, ":" );
}


string FilterDetectors::createFilters( void )
{
  string warning="";
  int n=10;
  for ( int k=0; k<n; k++ ) {

    // another filter?
    string label = "Filter" + Str( k );
    if ( Options::exist( label ) ) {
      n = k+10;
      label = label + '>';

      // create filter instance:
      string ident = text( label + "name" );
      string filter = text( label + "filter" );
      Filter *fp = (Filter*)( Plugins::create( filter,
					       RELACSPlugin::FilterId ) );
      if ( fp == 0 ) {
	warning += "<b>" + ident + "</b>: Plugin \"<b>" + filter + "</b>\" not found!<br>\n";
	continue;
      }

      fp->setRELACSWidget( RW );

      // filter parameter:
      int mode = 0;
      if ( boolean( label + "save", false ) )
	mode |= SaveFiles::SaveTrace;
      if ( boolean( label + "savesize", false ) )
	mode |= SaveFiles::SaveSize;
      if ( boolean( label + "savewidth", false ) )
	mode |= SaveFiles::SaveWidth;
      if ( boolean( label + "savemeanrate", false ) )
	mode |= SaveFiles::SaveMeanRate;
      if ( boolean( label + "savemeansize", false ) )
	mode |= SaveFiles::SaveMeanSize;
      if ( boolean( label + "savemeanwidth", false ) )
	mode |= SaveFiles::SaveMeanWidth;
      if ( boolean( label + "savemeanquality", false ) )
	mode |= SaveFiles::SaveMeanQuality;
      if ( boolean( label + "plot", true ) )
	mode |= PlotTraceMode;
      vector< string > intrace;
      Options::const_iterator ip = Options::find( label + "inputtrace" );
      if ( ip != Options::end() ) {
	intrace.resize( (*ip).size() );
	for ( unsigned int j=0; j<intrace.size(); j++ ) {
	  intrace[j] = (*ip).text( j );
	}
      }
      else
	warning += "<b>" + ident + "</b>: no inputtrace specified!<br>\n";

      vector< string > othertrace;
      Options::const_iterator op = Options::find( label + "othertrace" );
      if ( op != Options::end() ) {
	othertrace.resize( (*op).size() );
	for ( unsigned int j=0; j<othertrace.size(); j++ )
	  othertrace[j] = (*op).text( j );
      }
      int buffersize = integer( label + "buffersize", 0, 1000 );
      bool storesize = boolean( label + "storesize", false );
      bool storewidth = boolean( label + "storewidth", false );

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
	    warning += "<b>" + ident + "</b>: init( InData, InData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->filter( indata[0], outdata[0] ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: filter( InData, InData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	if ( ( fp->type() & Filter::MultipleAnalogFilter ) != Filter::MultipleAnalogFilter ) {
	  if ( fp->init( indata, outdata ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: init( InList, InList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->filter( indata, outdata ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: filter( InList, InList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	if ( ( fp->type() & Filter::SingleEventFilter ) != Filter::SingleEventFilter ) {
	  if ( fp->init( inevents[0], outdata[0] ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: init( EventData, InData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->filter( inevents[0], outdata[0] ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: filter( EventData, InData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	if ( ( fp->type() & Filter::MultipleEventFilter ) != Filter::MultipleEventFilter ) {
	  if ( fp->init( inevents, outdata ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: init( EventList, InList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->filter( inevents, outdata ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: filter( EventList, InList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}

	if ( ( fp->type() & Filter::SingleAnalogDetector ) != Filter::SingleAnalogDetector ) {
	  if ( fp->init( indata[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: init( InData, EventData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->detect( indata[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: detect( InData, EventData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	if ( ( fp->type() & Filter::MultipleAnalogDetector ) != Filter::MultipleAnalogDetector ) {
	  if ( fp->init( indata, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: init( InList, EventList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->detect( indata, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: detect( InList, EventList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	if ( ( fp->type() & Filter::SingleEventDetector ) != Filter::SingleEventDetector ) {
	  if ( fp->init( inevents[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: init( EventData, EventData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->detect( inevents[0], outevents[0], otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: detect( EventData, EventData ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}
	if ( ( fp->type() & Filter::MultipleEventDetector ) != Filter::MultipleEventDetector ) {
	  if ( fp->init( inevents, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: init( EventList, EventList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	  if ( fp->detect( inevents, outevents, otherevents, stimulusevents ) != INT_MIN ) {
	    warning += "<b>" + ident + "</b>: detect( EventList, EventList ) function should not be implemented!<br>\n";
	    failed = true;
	  }
	}

	if ( ( fp->type() & Filter::MultipleTraces ) > 0 ) {
	  if ( ( fp->type() & Filter::EventInput ) > 0 ) {
	    if ( fp->adjust( indata[0] ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: adjust( InData ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	    if ( fp->autoConfigure( indata[0], 0.0, 0.1 ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: indata( InData, 0.0, 0.1 ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	  }
	  else {
	    if ( fp->adjust( inevents[0] ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: adjust( EventData ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	    if ( fp->autoConfigure( inevents[0], 0.0, 0.1 ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: autoConfigure( EventData, 0.0, 0.1 ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	  }
	}
	else {
	  if ( ( fp->type() & Filter::EventInput ) > 0 ) {
	    if ( fp->adjust( indata ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: adjust( InList ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	    if ( fp->autoConfigure( indata, 0.0, 0.1 ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: autoConfiguer( InList, 0.0, 0.1 ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	  }
	  else {
	    if ( fp->adjust( inevents ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: adjust( EventList ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	    if ( fp->autoConfigure( inevents, 0.0, 0.1 ) != INT_MIN ) {
	      warning += "<b>" + ident + "</b>: autoConfigure( EventList, 0.0, 0.1 ) function should not be implemented!<br>\n";
	      failed = true;
	    }
	  }
	}

      }
      if ( failed )
	continue;

      // take and setup filter:
      fp->setParent( this );
      fp->setIdent( ident );
      fp->setMode( mode );
      
      // insert detector in list:
      FL.push_back( FilterData( fp, filter, intrace, othertrace,
				buffersize, storesize, storewidth ) );


      // add detector to widget:
      if ( fp->widget() != 0 )
	addTab( fp->widget(), fp->ident().c_str() );

    }
  }

  return warning;
}


int FilterDetectors::traces( void )
{
  int n = 0;

  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( !(d->FilterDetector->type() & Filter::EventDetector) )
      n += d->NOut;
  }

  return n;
}


int FilterDetectors::events( void )
{
  int n = 0;

  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( d->FilterDetector->type() & Filter::EventDetector )
      n += d->NOut;
  }

  return n;
}


void FilterDetectors::createStimulusEvents( InList &data, EventList &events,
					    vector< PlotTrace::EventStyle > &eventstyles )
{
  EventData e( 6000, 0.0, 0.0, data[0].sampleInterval(),
	       false, true );  // as the width we store the signal durations
  e.setCyclic();
  e.setSource( 0 );
  e.setMode( SaveFiles::SaveTrace | SaveFiles::SaveWidth | PlotTraceMode | StimulusEventMode );
  e.setIdent( "Stimulus" );
  events.push( e );
  StimulusEvents = &events.back();

  eventstyles.push_back( PlotTrace::EventStyle() );
  eventstyles.back().PlotWindow = -2;
  eventstyles.back().Line.setColor( Plot::White );
  eventstyles.back().Line.setWidth( 2 );
  eventstyles.back().Point.setType( Plot::StrokeUp );
  eventstyles.back().Point.setColor( Plot::White );
  eventstyles.back().YPos = 0.0;
  eventstyles.back().YCoor = Plot::Graph;
  eventstyles.back().Size = 1.0;
  eventstyles.back().SizeCoor = Plot::GraphY;

  EventInputTrace.resize( EventInputTrace.size() + 1, -1 );
  EventInputEvent.resize( EventInputEvent.size() + 1, -1 );
}


void FilterDetectors::createRestartEvents( InList &data, EventList &events,
					   vector< PlotTrace::EventStyle > &eventstyles )
{
  EventData e( 6000, 0.0, 0.0, data[0].sampleInterval(),
	       false, false );
  e.setCyclic();
  e.setSource( 0 );
  e.setMode( SaveFiles::SaveTrace | PlotTraceMode | RestartEventMode );
  e.setIdent( "Restart" );
  events.push( e );

  eventstyles.push_back( PlotTrace::EventStyle() );
  eventstyles.back().PlotWindow = -2;
  eventstyles.back().Line.setColor( Plot::Orange );
  eventstyles.back().Line.setWidth( 1 );
  eventstyles.back().Point.setType( Plot::TriangleNorth );
  eventstyles.back().Point.setColor( Plot::Orange );
  eventstyles.back().Point.setFillColor( Plot::Orange );
  eventstyles.back().YPos = 1.0;
  eventstyles.back().YCoor = Plot::Graph;
  eventstyles.back().Size = 0.07;
  eventstyles.back().SizeCoor = Plot::GraphY;

  EventInputTrace.resize( EventInputTrace.size() + 1, -1 );
  EventInputEvent.resize( EventInputEvent.size() + 1, -1 );
}


void FilterDetectors::createRecordingEvents( InList &data, EventList &events,
					     vector< PlotTrace::EventStyle > &eventstyles )
{
  EventData e( 6000, 0.0, 0.0, data[0].sampleInterval(),
	       false, false );
  e.setCyclic();
  e.setSource( 0 );
  e.setMode( SaveFiles::SaveTrace | PlotTraceMode | RecordingEventMode );
  e.setIdent( "Recording" );
  events.push( e );

  eventstyles.push_back( PlotTrace::EventStyle() );
  eventstyles.back().PlotWindow = -2;
  eventstyles.back().Line.setColor( Plot::Red );
  eventstyles.back().Line.setWidth( 4 );
  eventstyles.back().Point.setType( Plot::StrokeUp );
  eventstyles.back().Point.setColor( Plot::Red );
  eventstyles.back().YPos = 0.0;
  eventstyles.back().YCoor = Plot::Graph;
  eventstyles.back().Size = 1.0;
  eventstyles.back().SizeCoor = Plot::GraphY;

  EventInputTrace.resize( EventInputTrace.size() + 1, -1 );
  EventInputEvent.resize( EventInputEvent.size() + 1, -1 );
}


string FilterDetectors::createTracesEvents( InList &data, EventList &events,
					    vector< PlotTrace::TraceStyle > &tracestyles,
					    vector< PlotTrace::EventStyle > &eventstyles )
{
  string warning = "";

  TraceInputTrace.resize( TraceInputTrace.size() + FilterDetectors::traces(), -1 );
  TraceInputEvent.resize( TraceInputEvent.size() + FilterDetectors::traces(), -1 );
  EventInputTrace.resize( EventInputTrace.size() + FilterDetectors::events(), -1 );
  EventInputEvent.resize( EventInputEvent.size() + FilterDetectors::events(), -1 );

  int dk = data.size();
  int ek = events.size();

  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {

    // inputs:
    if ( d->FilterDetector->type() & Filter::EventInput ) {
      // input events:
      d->InEvents.clear();
      bool store = true;
      for ( unsigned int j=0; j < d->In.size(); j++ ) {
	int einx = events.index( d->In[j] );
	if ( einx < 0 )
	  einx = (int)::rint( Str( d->In[j] ).number( -1.0 ) );
	if ( einx >= 0 && einx < events.size() ) {
	  d->InEvents.add( &events[einx] );
	  if ( store ) {
	    if ( d->FilterDetector->type() & Filter::EventDetector ) {
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
	  warning += "Invalid input event index <b>" + d->In[j]
	    + "</b> in <b>" + d->FilterDetector->ident() + "</b><br>";
      }
    }
    else {
      // input traces:
      d->InTraces.clear();
      bool store = true;
      for ( unsigned int j=0; j < d->In.size(); j++ ) {
	int dinx = data.index( d->In[j] );
	if ( dinx < 0 )
	  dinx = (int)::rint( Str( d->In[j] ).number( -1.0 ) );
	if ( dinx >= 0 && dinx < data.size() ) {
	  d->InTraces.add( &data[dinx] );
	  if ( store ) {
	    if ( d->FilterDetector->type() & Filter::EventDetector ) {
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
	  warning += "Invalid input trace index <b>" + d->In[j]
	    + "</b> in <b>" + d->FilterDetector->ident() + "</b><br>";
      }
    }

    if ( ! warning.empty() )
      continue;

    if ( d->FilterDetector->type() & Filter::EventDetector ) {
      // detector:

      d->Out = ek;

      // assemble output events:
      d->OutEvents.clear();
      for ( int j=0; j < d->NOut; j++ ) {
	int intrace = EventInputTrace[ek];
	if ( intrace < 0 )
	  intrace = 0;
	EventData e( 0, 0.0, 0.0, data[intrace].sampleInterval() );
	e.setCyclic();
	events.push( e );
	d->OutEvents.add( &events[d->Out+j] );

	// this is not really good yet...
	eventstyles.push_back( PlotTrace::EventStyle() );
	eventstyles.back().PlotWindow = eventInputTrace( d->Out+j );
	eventstyles.back().Line.setColor( Plot::Red );
	eventstyles.back().Line.setWidth( 1 );
	eventstyles.back().Point.setType( Plot::TriangleUp );
	eventstyles.back().Point.setColor( Plot::Red );
	eventstyles.back().Point.setFillColor( Plot::Red );
	eventstyles.back().YPos = 0.1;
	eventstyles.back().YCoor = Plot::Graph;
	eventstyles.back().Size = 6.0;
	eventstyles.back().SizeCoor = Plot::Pixel;
      }

      // assemble other events:
      d->OtherEvents.clear();
      for ( unsigned int j=0; j < d->Other.size(); j++ ) {
	int oinx = events.index( d->Other[j] );
	if ( oinx < 0 )
	  oinx = (int)::rint( Str( d->Other[j] ).number( -1.0 ) );
	if ( oinx >= 0 && oinx < events.size() )
	  d->OtherEvents.add( &events[oinx] );
	else if ( d->Other[j] != "-1" )
	  warning += "Invalid other event trace index <b>" + d->Other[j]
	    + "</b> in <b>" + d->FilterDetector->ident() + "</b><br>";
      }

      // setup output events:
      if ( d->FilterDetector->type() & Filter::EventInput ) {
	// single / multiple event traces -> multiple event traces
	for ( int i=0; i<d->NOut; i++, ek++ ) {
	  events[ek].setSizeBuffer( d->SizeBuffer );
	  events[ek].setWidthBuffer( d->WidthBuffer );
	  events[ek].reserve( d->NBuffer );
	  events[ek].setSource( 2 );
	  events[ek].setMode( d->FilterDetector->mode() );
	  if ( d->FilterDetector->type() & Filter::MultipleTraces )
	    events[ek].setIdent( d->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    events[ek].setIdent( d->FilterDetector->ident() );
	  events[ek].setSizeUnit( d->InEvents[0].sizeUnit() );
	}
      }
      else {
	// single / multiple analog -> multiple event traces
	for ( int i=0; i<d->NOut; i++, ek++ ) {
	  events[ek].setSizeBuffer( d->SizeBuffer );
	  events[ek].setWidthBuffer( d->WidthBuffer );
	  events[ek].reserve( d->NBuffer );
	  events[ek].setSource( 1 );
	  events[ek].setMode( d->FilterDetector->mode() );
	  if ( d->FilterDetector->type() & Filter::MultipleTraces )
	    events[ek].setIdent( d->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    events[ek].setIdent( d->FilterDetector->ident() );
	  events[ek].setSizeUnit( d->InTraces[0].unit() );
	}
      }
    }
    else {
      // filter:

      d->Out = dk;

      // assemble output traces:
      d->OutTraces.clear();
      for ( int j=0; j < d->NOut; j++ ) {
	int intrace = TraceInputTrace[dk];
	if ( intrace < 0 )
	  intrace = 0;
	int nbuffer = d->NBuffer;
	if ( nbuffer < 0 )
	  nbuffer = data[intrace].capacity();
	InData dt( nbuffer, data[intrace].sampleInterval() );
	dt.setDevice( -1 );
	dt.setChannel( -1 );
	data.push( dt );
	d->OutTraces.add( &data[d->Out+j] );

	tracestyles.push_back( PlotTrace::TraceStyle() );
	tracestyles.back().PlotWindow = tracestyles[intrace].PlotWindow;
	tracestyles.back().Line.setColor( Plot::Blue );
      }

      if ( d->FilterDetector->type() & Filter::EventInput ) {
	// single/multiple event traces -> multiple traces
	for ( int i=0; i<d->NOut; i++, dk++ ) {
	  data[dk].setSource( 2 );
	  data[dk].setMode( d->FilterDetector->mode() );
	  if ( d->FilterDetector->type() & Filter::MultipleTraces )
	    data[dk].setIdent( d->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    data[dk].setIdent( d->FilterDetector->ident() );
	  data[dk].setUnit( d->InEvents[0].sizeUnit() );
	}
      }
      else {
	// single / multiple analog -> multiple traces
	for ( int i=0; i<d->NOut; i++, dk++ ) {
	  data[dk].setSource( 1 );
	  data[dk].setMode( d->FilterDetector->mode() );
	  if ( d->FilterDetector->type() & Filter::MultipleTraces )
	    data[dk].setIdent( d->FilterDetector->ident() + "-" + Str( i+1 ) );
	  else
	    data[dk].setIdent( d->FilterDetector->ident() );
	  data[dk].setUnit( d->InTraces[0].unit() );
	}
      }


    }

  }

  return warning;
}


string FilterDetectors::init( const InList &data, EventList &events )
{
  string warning = "";

  // init filter and event detectors:
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    d->FilterDetector->setSettings();
    d->Init = true;
    string ident = d->FilterDetector->ident();

    if ( d->FilterDetector->type() & Filter::EventDetector ) {
      if ( d->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single event trace
	if ( d->FilterDetector->type() == Filter::SingleEventDetector ) {
	  if ( d->FilterDetector->init( d->InEvents[0], d->OutEvents[0], 
					d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( EventData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
	// multiple event traces -> multiple event traces
	else {
	  if ( d->FilterDetector->init( d->InEvents, d->OutEvents, 
					d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( EventList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
      }
      else {
	// single analog -> single event trace
	if ( d->FilterDetector->type() == Filter::SingleAnalogDetector ) {
	  if ( d->FilterDetector->init( d->InTraces[0], d->OutEvents[0],
					d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( InData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
	// multiple analog -> multiple event traces
	else {
	  if ( d->FilterDetector->init( d->InTraces, d->OutEvents, 
					d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( InList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
      }
    }
    else {
      if ( d->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single trace
	if ( d->FilterDetector->type() == Filter::SingleEventFilter ) {
	  if ( d->FilterDetector->init( d->InEvents[0], d->OutTraces[0] ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( EventData, InData ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
	// multiple event traces -> multiple traces
	else {
	  if ( d->FilterDetector->init( d->InEvents, d->OutTraces ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( EventList, InList ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
      }
      else {
	// single analog trace -> single trace
	if ( d->FilterDetector->type() == Filter::SingleAnalogFilter ) {
	  if ( d->FilterDetector->init( d->InTraces[0], d->OutTraces[0] ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( InData, EventData ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
	// multiple analog traces -> multiple traces
	else {
	  if ( d->FilterDetector->init( d->InTraces, d->OutTraces ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: init( InList, EventList ) function must be implemented!<br>\n";
	  else
	    d->Init = false;
	}
      }
    }

  }

  return warning;
}


void FilterDetectors::adjust( const InList &data, const EventList &events,
			      int flag )
{
  NeedAdjust = true;
  AdjustFlag = flag;
}


void FilterDetectors::adjust( const InList &data, const EventList &events )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {

    d->FilterDetector->lock();
    if ( d->FilterDetector->type() & Filter::EventDetector ) {
      // check if this detector really has to be adjusted:
      bool c = false;
      if ( AdjustFlag >= 0 ) {
	for ( int k=0; k<d->InTraces.size(); k++ ) {
	  if ( d->InTraces[k].mode() & AdjustFlag ) {
	    c = true;
	    break;
	  }
	}
      }
      else
	c = true;
      // adjust detector:
      if ( c ) {
	if ( d->FilterDetector->type() & Filter::MultipleTraces )
	  d->FilterDetector->adjust( d->InTraces );
	else
	  d->FilterDetector->adjust( d->InTraces[0] );
      }
    }
    else {
      // check if this detector really has to be adjusted:
      bool c = false;
      if ( AdjustFlag >= 0 ) {
	for ( int k=0; k<d->InEvents.size(); k++ ) {
	  if ( d->InEvents[k].mode() & AdjustFlag ) {
	    c = true;
	    break;
	  }
	}
      }
      else
	c = true;
      // adjust detector:
      if ( c ) {
	if ( d->FilterDetector->type() & Filter::MultipleTraces )
	  d->FilterDetector->adjust( d->InEvents );
	else
	  d->FilterDetector->adjust( d->InEvents[0] );
      }
    }
    d->FilterDetector->unlock();
  }

  NeedAdjust = false;
}


void FilterDetectors::autoConfigure( double duration )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    d->FilterDetector->lock();
    if ( d->FilterDetector->type() & Filter::EventDetector ) {
      double tend = d->InTraces[0].currentTime();
      double tbegin = tend - duration;
      if ( tbegin < d->InTraces[0].minTime() )
	tbegin = d->InTraces[0].minTime();
      if ( d->FilterDetector->type() & Filter::MultipleTraces )
	d->FilterDetector->autoConfigure( d->InTraces, tbegin, tend );
      else
	d->FilterDetector->autoConfigure( d->InTraces[0], tbegin, tend );
    }
    else {
      double tend = d->InEvents[0].rangeBack();
      double tbegin = tend - duration;
      if ( d->FilterDetector->type() & Filter::MultipleTraces )
	d->FilterDetector->autoConfigure( d->InEvents, tbegin, tend );
      else
	d->FilterDetector->autoConfigure( d->InEvents[0], tbegin, tend );
    }
    d->FilterDetector->unlock();
  }
}


void FilterDetectors::autoConfigure( double tbegin, double tend )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    d->FilterDetector->lock();
    if ( d->FilterDetector->type() & Filter::EventDetector ) {
      if ( d->FilterDetector->type() & Filter::MultipleTraces )
	d->FilterDetector->autoConfigure( d->InTraces, tbegin, tend );
      else
	d->FilterDetector->autoConfigure( d->InTraces[0], tbegin, tend );
    }
    else {
      if ( d->FilterDetector->type() & Filter::MultipleTraces )
	d->FilterDetector->autoConfigure( d->InEvents, tbegin, tend );
      else
	d->FilterDetector->autoConfigure( d->InEvents[0], tbegin, tend );
    }
    d->FilterDetector->unlock();
  }
}


void FilterDetectors::autoConfigure( Filter *f, double duration )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp ) {
    if ( fp->FilterDetector == f ) {
      fp->FilterDetector->lock();
      if ( fp->FilterDetector->type() & Filter::EventDetector ) {
	double tend = fp->InTraces[0].currentTime();
	double tbegin = tend - duration;
	if ( tbegin < fp->InTraces[0].minTime() )
	  tbegin = fp->InTraces[0].minTime();
	if ( fp->FilterDetector->type() & Filter::MultipleTraces )
	  fp->FilterDetector->autoConfigure( fp->InTraces, tbegin, tend );
	else
	  fp->FilterDetector->autoConfigure( fp->InTraces[0], tbegin, tend );
      }
      else {
	double tend = fp->InEvents[0].rangeBack();
	double tbegin = tend - duration;
	if ( fp->FilterDetector->type() & Filter::MultipleTraces )
	  fp->FilterDetector->autoConfigure( fp->InEvents, tbegin, tend );
	else
	  fp->FilterDetector->autoConfigure( fp->InEvents[0], tbegin, tend );
      }
      fp->FilterDetector->unlock();
      break;
    }
  }
}


void FilterDetectors::autoConfigure( Filter *f, double tbegin, double tend )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp ) {
    if ( fp->FilterDetector == f ) {
      fp->FilterDetector->lock();
      if ( fp->FilterDetector->type() & Filter::EventDetector ) {
	if ( fp->FilterDetector->type() & Filter::MultipleTraces )
	  fp->FilterDetector->autoConfigure( fp->InTraces, tbegin, tend );
	else
	  fp->FilterDetector->autoConfigure( fp->InTraces[0], tbegin, tend );
      }
      else {
	if ( fp->FilterDetector->type() & Filter::MultipleTraces )
	  fp->FilterDetector->autoConfigure( fp->InEvents, tbegin, tend );
	else
	  fp->FilterDetector->autoConfigure( fp->InEvents[0], tbegin, tend );
      }
      fp->FilterDetector->unlock();
      break;
    }
  }
}


string FilterDetectors::filter( const InList &data, EventList &events )
{
  // adjust necessary?
  if ( NeedAdjust )
    adjust( data, events );

  string warning = "";

  // filter and detect events:
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    string ident = d->FilterDetector->ident();

    d->FilterDetector->lock();
    if ( d->FilterDetector->type() & Filter::EventDetector ) {
      if ( d->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single event trace
	if ( d->FilterDetector->type() == Filter::SingleEventDetector ) {
	  if ( d->Init ) {
	    d->Init = false;
	    d->FilterDetector->init( d->InEvents[0], d->OutEvents[0], 
				     d->OtherEvents, *StimulusEvents );
	  }
	  if ( d->FilterDetector->detect( d->InEvents[0], d->OutEvents[0], 
					  d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: detect( EventData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	}
	// multiple event traces -> multiple event traces
	else {
	  if ( d->Init ) {
	    d->Init = false;
	    d->FilterDetector->init( d->InEvents, d->OutEvents, 
				     d->OtherEvents, *StimulusEvents );
	  }
	  if ( d->FilterDetector->detect( d->InEvents, d->OutEvents, 
					  d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: detect( EventList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	}
      }
      else {
	// single analog -> single event trace
	if ( d->FilterDetector->type() == Filter::SingleAnalogDetector ) {
	  if ( d->Init ) {
	    d->FilterDetector->init( d->InTraces[0], d->OutEvents[0], 
				     d->OtherEvents, *StimulusEvents );
	    d->Init = false;
	  }
	  if ( d->FilterDetector->detect( d->InTraces[0], d->OutEvents[0], 
					  d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: detect( InData, EventData, EventList, EventData ) function must be implemented!<br>\n";
	}
	// multiple analog -> multiple event traces
	else {
	  if ( d->Init ) {
	    d->Init = false;
	    d->FilterDetector->init( d->InTraces, d->OutEvents, 
				     d->OtherEvents, *StimulusEvents );
	  }
	  if ( d->FilterDetector->detect( d->InTraces, d->OutEvents, 
					  d->OtherEvents, *StimulusEvents ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: detect( InList, EventList, EventList, EventData ) function must be implemented!<br>\n";
	}
      }
    }
    else {
      if ( d->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single trace
	if ( d->FilterDetector->type() == Filter::SingleEventFilter ) {
	  if ( d->Init ) {
	    d->Init = false;
	    d->FilterDetector->init( d->InEvents[0], d->OutTraces[0] );
	  }
	  if ( d->FilterDetector->filter( d->InEvents[0], d->OutTraces[0] ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: filter( EventData, InData ) function must be implemented!<br>\n";
	}
	// multiple event traces -> multiple traces
	else {
	  if ( d->Init ) {
	    d->Init = false;
	    d->FilterDetector->init( d->InEvents, d->OutTraces );
	  }
	  if ( d->FilterDetector->filter( d->InEvents, d->OutTraces ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: filter( EventList, InList ) function must be implemented!<br>\n";
	}
      }
      else {
	// single analog -> single trace
	if ( d->FilterDetector->type() == Filter::SingleAnalogFilter ) {
	  if ( d->Init ) {
	    d->FilterDetector->init( d->InTraces[0], d->OutTraces[0] );
	    d->Init = false;
	  }
	  if ( d->FilterDetector->filter( d->InTraces[0], d->OutTraces[0] ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: filter( InData, InData ) function must be implemented!<br>\n";
	}
	// multiple analog -> multiple traces
	else {
	  if ( d->Init ) {
	    d->Init = false;
	    d->FilterDetector->init( d->InTraces, d->OutTraces );
	  }
	  if ( d->FilterDetector->filter( d->InTraces, d->OutTraces ) == INT_MIN )
	    warning += "<b>" + ident + "</b>: filter( InList, InList ) function must be implemented!<br>\n";
	}
      }
    }
    
    d->FilterDetector->unlock();

  }

  return warning;  
}


Filter *FilterDetectors::filter( int index )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp )
    if ( index >= fp->Out &&
	 index < fp->Out + fp->NOut )
      return fp->FilterDetector;

  return 0;
}


Filter *FilterDetectors::filter( const string &ident )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp ) {
    if ( fp->FilterDetector->ident() == ident )
      return fp->FilterDetector;
  }
  return 0;
}


Filter *FilterDetectors::detector( int index )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d )
    if ( index >= d->Out &&
	 index < d->Out + d->NOut )
      return d->FilterDetector;

  return 0;
}


Filter *FilterDetectors::detector( const string &ident )
{
  for ( FilterList::iterator d = FL.begin(); d != FL.end(); ++d ) {
    if ( d->FilterDetector->ident() == ident )
      return d->FilterDetector;
  }
  return 0;
}


bool FilterDetectors::exist( const string &ident ) const
{
  for ( FilterList::const_iterator fd = FL.begin(); fd != FL.end(); ++fd ) {
    if ( fd->FilterDetector->ident() == ident )
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
	trace < RW->ED.size() && trace < (int)TraceInputTrace.size();
	trace++ ) {
    if ( RW->IL[trace].ident() == ident )
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
	trace < RW->IL.size() && trace < (int)TraceInputEvent.size();
	trace++ ) {
    if ( RW->IL[trace].ident() == ident )
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
	event < RW->ED.size() && event < (int)EventInputTrace.size();
	event++ ) {
    if ( RW->ED[event].ident() == ident )
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
	event < RW->ED.size() && event < (int)EventInputEvent.size();
	event++ ) {
    if ( RW->ED[event].ident() == ident )
      return EventInputEvent[ event ];
  }
  return -1;
}


void FilterDetectors::addMenu( QMenu *menu )
{
  if ( Menu == 0 )
    Menu = menu;
  Menu->clear();

  for ( unsigned int k=0; k<FL.size(); k++ ) {
    string s = "&";
    if ( k == 0 )
      s += '0';
    else if ( k < 10 )
      s += ( '1' + k - 1 );
    else
      s += ( 'a' + k - 10 );
    s += " ";
    s += FL[k].FilterDetector->ident();
    QMenu *pop = Menu->addMenu( s.c_str() );
    pop->addAction( "&Options...", FL[k].FilterDetector, SLOT( dialog() ) );
    pop->addAction( "&Help...", FL[k].FilterDetector, SLOT( help() ) );
  }
}


void FilterDetectors::modeChanged( void )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp )
    (*fp).FilterDetector->modeChanged();
}


void FilterDetectors::notifyMetaData( const string &section )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp )
    (*fp).FilterDetector->notifyMetaData( section );
}


void FilterDetectors::notifyStimulusData( void )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp )
    (*fp).FilterDetector->notifyStimulusData();
}


void FilterDetectors::sessionStarted( void )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp )
    (*fp).FilterDetector->sessionStarted();
}


void FilterDetectors::sessionStopped( bool saved )
{
  for ( FilterList::iterator fp = FL.begin(); fp != FL.end(); ++fp )
    (*fp).FilterDetector->sessionStopped( saved );
}


ostream &operator<<( ostream &str, const FilterDetectors &fd )
{
  for ( FilterDetectors::FilterList::const_iterator fp = fd.FL.begin(); 
	fp != fd.FL.end(); 
	++fp ) {
    if ( fp->FilterDetector->type() & Filter::EventDetector ) {
      if ( fp->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single event trace
	if ( fp->FilterDetector->type() == Filter::SingleEventDetector ) {
	  str << "Event Detector 1-1:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
	// multiple event traces -> multiple event traces
	else {
	  str << "Event Detector n-n:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
      }
      else {
	// single analog -> single event trace
	if ( fp->FilterDetector->type() == Filter::SingleAnalogDetector ) {
	  str << "Analog Detector 1-1:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
	// multiple analog -> multiple event traces
	else {
	  str << "Analog Detector n-n:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
      }
    }
    else {
      if ( fp->FilterDetector->type() & Filter::EventInput ) {
	// singel event trace -> single analog trace
	if ( fp->FilterDetector->type() == Filter::SingleEventFilter ) {
	  str << "Event Filter 1-1:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
	// multiple event traces -> multiple analog traces
	else {
	  str << "Event Filter n-n:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
      }
      else {
	// single analog -> single analog trace
	if ( fp->FilterDetector->type() == Filter::SingleAnalogFilter ) {
	  str << "Analog Filter 1-1:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
	// multiple analog -> multiple traces
	else {
	  str << "Analog Filter n-n:" << '\n';
	  fp->print( str );
	  str << '\n';
	}
      }
    }
  }

  return str;
}


void FilterDetectors::keyPressEvent( QKeyEvent *event )
{
  if ( event->key() != Qt::Key_Return )
    event->ignore();
  for ( FilterList::iterator d = FL.begin();
	d != FL.end() && ! event->isAccepted();
	++d ) {
    if ( d->FilterDetector != 0 &&
	 d->FilterDetector->globalKeyEvents() &&
	 d->FilterDetector->widget() != 0 )
      d->FilterDetector->widget()->QObject::event( event );
  }
}


void FilterDetectors::keyReleaseEvent( QKeyEvent *event )
{
  for ( FilterList::iterator d = FL.begin();
	d != FL.end() && ! event->isAccepted();
	++d ) {
    if ( d->FilterDetector != 0 &&
	 d->FilterDetector->globalKeyEvents() &&
	 d->FilterDetector->widget() != 0 )
      d->FilterDetector->widget()->QObject::event( event );
  }
}


FilterDetectors::FilterData::FilterData( Filter *filter,
					 const string &pluginname,
					 const vector<string> &in,
					 const vector<string> &other,
					 long n, bool size, bool width )
  : PluginName( pluginname ), In( in ), Other( other ), 
    InTraces(), InEvents(), OutTraces(), OutEvents(), OtherEvents(),
    NBuffer( n ), SizeBuffer( size ), WidthBuffer( width ), Init( true )
{
  FilterDetector = filter;
  NOut = filter->outTraces();
  if ( NOut == 0 )
    NOut = in.size();
}


FilterDetectors::FilterData::FilterData( const FilterData &fd )
  : PluginName( fd.PluginName ), In( fd.In ), Other( fd.Other ), 
    InTraces(), InEvents( fd.InEvents ), OutTraces( fd.OutTraces ),
    OutEvents( fd.OutEvents ), OtherEvents( fd.OtherEvents ),
    NBuffer( fd.NBuffer ), SizeBuffer( fd.SizeBuffer ),
    WidthBuffer( fd.WidthBuffer ), Init( fd.Init )
{
  FilterDetector = fd.FilterDetector;
  Out = fd.Out;
  NOut = fd.NOut;
}


FilterDetectors::FilterData::~FilterData( void )
{
}


void FilterDetectors::FilterData::print( ostream &str ) const
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
      << "       init: " << Init << '\n'
      << "   detector: " << FilterDetector << endl;
}



}; /* namespace relacs */

#include "moc_filterdetectors.cc"

