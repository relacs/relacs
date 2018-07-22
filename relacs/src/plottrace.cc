/*
  plottrace.cc
  Plot traces and events.

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

#include <iostream>
#include <fstream>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QPolygon>
#include <QDateTime>
#include <QLabel>
#include <QToolTip>
#include <QApplication>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/filterdetectors.h>
#include <relacs/relacswidget.h>
#include <relacs/plottrace.h>

namespace relacs {


PlotTrace::PlotTrace( RELACSWidget *rw, QWidget* parent )
  : RELACSPlugin( "PlotTrace: PlotTrace", RELACSPlugin::Plugins,
		  "PlotTrace", "base" ),
    P( 1, Plot::Pointer, parent ),
    Menu( 0 ),
    PlotTimer( this )
{
  setRELACSWidget( rw );

  ViewMode = SignalView;
  ContinuousView = WrapView;
  setView( WrapView );
  DataView = NoView;
  Manual = false;
  ShowGrid = 3;
  Trigger = true;
  TriggerSource = -1;
  Plotting = true;

  connect( &PlotTimer, SIGNAL( timeout() ),
	   this, SLOT( plot() ) );
  connect( &P, SIGNAL( changedRanges( int ) ),
	   this, SLOT( updateRanges( int ) ) );
  connect( &P, SIGNAL( resizePlots( QResizeEvent* ) ),
	   this, SLOT( resizePlots( QResizeEvent* ) ) );

  TimeWindow = 1.0;
  TimeOffs = 0.0;
  Offset = 0.0;

  DoSignalLength = 0.0;
  DoSignalOffset = 0.0;
  DoSignalTime = -1.0;

  AutoOn = true;
  AutoFixed = false;
  AutoTime = 0.1;
  AutoOffs = 0.0;

  ButtonBox = 0;
  ViewButton = 0;
  ManualButton = 0;
  OnOffButton = 0;

  CenterMode = 0;
  CenterTime.start();

  FilePlot = false;
  FilePath = "";
  FileTraces.clear();
  FileTracesNames.clear();
  FileSizes.clear();
  FileEvents.clear();
  FileEventsNames.clear();

  PlotTraces.clear();
  PlotEvents.clear();

  TraceStyle.clear();
  EventStyle.clear();

  setWidget( &P );

  VP.clear();

  ButtonBox = 0;
  /*
  ButtonBox = new QWidget( this );
  ButtonBoxLayout = new QHBoxLayout;
  ButtonBoxLayout->setContentsMargins( 0, 0, 0, 0 );
  ButtonBoxLayout->setSpacing( 0 );
  ButtonBox->setLayout( ButtonBoxLayout );
  */

  int s = P.fontInfo().pixelSize();

  SignalViewIcon = QPixmap( s, s );
  QPainter p;
  p.begin( &SignalViewIcon );
  p.eraseRect( SignalViewIcon.rect() );
  p.setPen( QPen() );
  p.setBrush( Qt::black );
  QPolygon pa( 3 );
  pa.setPoint( 0, s-3, 2 );
  pa.setPoint( 1, s-3, s-2 );
  pa.setPoint( 2, 4, s/2 );
  p.drawPolygon( pa );
  p.setPen( QPen( Qt::black, 2 ) );
  p.drawLine( 3, 2, 3, s-1 );
  p.end();
  SignalViewIcon.setMask( SignalViewIcon.createHeuristicMask() );

  EndViewIcon = QPixmap( s, s );
  p.begin( &EndViewIcon );
  p.eraseRect( EndViewIcon.rect() );
  p.setPen( QPen() );
  p.setBrush( Qt::black );
  pa.setPoint( 0, 3, 2 );
  pa.setPoint( 1, 3, s-2 );
  pa.setPoint( 2, s-4, s/2 );
  p.drawPolygon( pa );
  p.setPen( QPen( Qt::black, 2 ) );
  p.drawLine( s-2, 2, s-2, s-1 );
  p.end();
  EndViewIcon.setMask( EndViewIcon.createHeuristicMask() );

  /*
  ViewButton = new QPushButton;
  ButtonBoxLayout->addWidget( ViewButton );
  ViewButton->setIcon( SignalViewIcon );
  ViewButton->setToolTip( "F: fixed (Pos1), C: continous (End)" );
  connect( ViewButton, SIGNAL( clicked() ), this, SLOT( viewToggle() ) );
  */

  QBitmap manualmask( s, s );
  p.begin( &manualmask );
  p.setPen( QPen( Qt::color1 ) );
  p.setBrush( QBrush() );
  p.setFont( QFont( "Helvetica", s, QFont::Bold ) );
  p.drawText( manualmask.rect(), Qt::AlignCenter, "M" );
  p.end();
  QPixmap manualicon( s, s );
  p.begin( &manualicon );
  p.eraseRect( manualicon.rect() );
  p.setPen( QPen( Qt::black ) );
  p.setBrush( QBrush() );
  p.setFont( QFont( "Helvetica", s, QFont::Bold ) );
  p.drawText( manualicon.rect(), Qt::AlignCenter, "M" );
  p.end();
  manualicon.setMask( manualmask );

  /*
  ManualButton = new QPushButton;
  ButtonBoxLayout->addWidget( ManualButton );
  ManualButton->setCheckable( true );
  ManualButton->setIcon( manualicon );
  ManualButton->setDown( Manual );
  ManualButton->setToolTip( "Manual or Auto" );
  connect( ManualButton, SIGNAL( clicked() ), this, SLOT( toggleManual() ) );
  */

  /*
  QPixmap onofficon( s, s );
  p.begin( &onofficon, this );
  p.eraseRect( onofficon.rect() );
  p.setPen( QPen( black, 2 ) );
  p.setBrush( QBrush() );
  p.drawArc( 2, 2, s-4, s-4, 1920, 4800 );
  p.drawLine( s/2, 0, s/2, s/2 );
  p.end();
  onofficon.setMask( onofficon.createHeuristicMask() );

  OnOffButton = new QPushButton;
  ButonBoxLayout->addWidget( OnOffButton );
  OnOffButton->setToggleButton( true );
  OnOffButton->setPixmap( onofficon );
  OnOffButton->setOn( ! Plotting );
  QToolTip::add( OnOffButton, "Switch the plot on or off" );
  connect( OnOffButton, SIGNAL( clicked() ), this, SLOT( plotOnOff() ) );
  */
}


PlotTrace::~PlotTrace( void )
{
}


void PlotTrace::clearStyles( void )
{
  P.lock();
  for ( unsigned int k=0; k<TraceStyle.size(); k++ )
    Menu->removeAction( TraceStyle[k].action() );
  TraceStyle.clear();
  EventStyle.clear();
  P.unlock();
}


void PlotTrace::addTraceStyle( bool visible, int panel, int lcolor )
{
  TraceStyle.push_back( PlotTraceStyle( visible, panel, lcolor ) );
}


const deque< PlotTraceStyle > &PlotTrace::traceStyles( void ) const
{
  return TraceStyle;
}


deque< PlotTraceStyle > &PlotTrace::traceStyles( void )
{
  return TraceStyle;
}


const deque< PlotEventStyle > &PlotTrace::eventStyles( void ) const
{
  return EventStyle;
}


deque< PlotEventStyle > &PlotTrace::eventStyles( void )
{
  return EventStyle;
}


void PlotTrace::resizeLayout( void )
{
  if ( VP.empty() )
    return;

  if ( VP.size() == 1 ) {
    P[VP.front()].setOrigin( 0.0, 0.0 );
    P[VP.front()].setSize( 1.0, 1.0 );
    return;
  }

  int columns = 1;
  if ( VP.size() > 6 )
    columns = 2;
  int rows = VP.size()/columns;
  if ( VP.size()%columns > 0 )
    rows++;
  double xsize = 1.0/columns;
  double yboffs = double( P[VP.front()].fontPixel( 2.3 ) ) / double( P.height() );
  double ytoffs = double( P[VP.front()].fontPixel( 0.8 ) ) / double( P.height() );
  double yheight = (1.0-yboffs-ytoffs)/rows;
    
  int c = 0;
  int r = 0;
  for ( unsigned int k=0; k<VP.size(); k++ ) {
    P[VP[k]].setOrigin( c*xsize, yboffs+(rows-r-1)*yheight );
    P[VP[k]].setSize( xsize, yheight );
    r++;
    if ( r >= rows ) {
      c++;
      r = 0;
    }
  }
  P[VP.front()].setSize( xsize, yheight+ytoffs );
  P[VP.back()].setOrigin( (columns-1)*xsize, 0.0 );
  P[VP.back()].setSize( xsize, yheight+yboffs );
  if ( columns > 1 ) {
    P[VP[(VP.size()+1)/2]].setSize( xsize, yheight+ytoffs );
    P[VP[(VP.size()-1)/2]].setOrigin( 0.0, 0.0 );
    P[VP[(VP.size()-1)/2]].setSize( xsize, yheight+yboffs );
  }
}


void PlotTrace::resize( void )
{
  P.lock();

  // count active plots:
  deque<int> pc;
  pc.resize( P.size(), 0 );
  for ( unsigned int c=0; c<TraceStyle.size(); c++ ) {
    if ( TraceStyle[c].visible() &&
	 TraceStyle[c].panel() >= 0 &&
	 TraceStyle[c].panel() < P.size() ) {
      pc[TraceStyle[c].panel()]++;
    }
  }

  // setup plots:
  VP.clear();
  for ( int c=0; c<P.size(); c++ ) {
    P[c].clear();
    P[c].setSkip( pc[c] == 0 );
    if ( pc[c] > 0 )
      VP.push_back( c );
  }

  if ( P.size() > 0 ) {
    double lmarg = 11.0;
    if ( VP.size() == 1 )
      lmarg = 8.0;
    int o = 0;
    if ( VP.size() > 1 && VP.size() % 2 == 1 )
      o = 1;
    
    for ( unsigned int c=0; c<VP.size(); c++ ) {
      P[VP[c]].setLMarg( lmarg );
      P[VP[c]].setRMarg( 2.0 );
      P[VP[c]].setTMarg( 0.2 );
      P[VP[c]].setBMarg( 0.2 );
      P[VP[c]].noXTics();
      P[VP[c]].setXLabel( "" );
      P[VP[c]].setYTics();
      P[VP[c]].setYLabelPos( 2.0 + ((c+o)%2)*3.0, Plot::FirstMargin,
			     0.5, Plot::Graph, Plot::Center, -90.0 );
    }
    
    P[VP.front()].setTMarg( 1.0 );
    P[VP.back()].setXTics();
    P[VP.back()].setXLabel( "msec" );
    P[VP.back()].setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, 
			       Plot::Left, 0.0 );
    P[VP.back()].setBMarg( 2.5 );
    
    if ( VP.size() > 6 ) {
      P[VP[(VP.size()+1)/2]].setTMarg( 1.0 );
      P[VP[(VP.size()-1)/2]].setXTics();
      P[VP[(VP.size()-1)/2]].setXLabel( "msec" );
      P[VP[(VP.size()-1)/2]].setXLabelPos( 1.0, Plot::FirstMargin,
				      0.0, Plot::FirstAxis, Plot::Left, 0.0 );
      P[VP[(VP.size()-1)/2]].setBMarg( 2.5 );
    }
    
    resizeLayout();
  }
  
  PlotChanged = true;
  
  P.unlock();
}


void PlotTrace::toggle( QAction *mtrace )
{
  // check for valid trace:
  bool nodata = true;
  unsigned int i=0;
  for ( i=0; i<TraceStyle.size(); i++ ) {
    if ( TraceStyle[i].action() == mtrace ) {
      nodata = false;
      break;
    }
  }
  if ( nodata )
    return;

  if ( TraceStyle[i].visible() ) {
    for ( unsigned int k=0; k<TraceStyle.size(); k++ ) {
      if ( k != i && TraceStyle[k].visible() ) {
	TraceStyle[i].setVisible( false );
	break;
      }
    }
  }
  else
    TraceStyle[i].setVisible( true );
  TraceStyle[i].action()->setChecked( TraceStyle[i].visible() );
  resize();
  plot();
}


void PlotTrace::init( void )
{
  P.lock();

  int origin = ViewMode == FixedView ? 3 : 2;
  double tfac = 1000.0;
  string tunit = "ms";

  for ( unsigned int c=0; c<VP.size(); c++ ) {

    int vp = VP[c];

    // clear plot:
    P[vp].clear();

    // y-label:
    for ( unsigned int k=0; k<TraceStyle.size(); k++ ) {
      if ( TraceStyle[k].panel() == vp ) {
	string s = PlotTraces[k].ident() + " [" + PlotTraces[k].unit() + "]";
	P[vp].setYLabel( s );
	break;
      }
    }

    // grid:
    if ( ShowGrid & 1 )
      P[vp].setXGrid();
    else
      P[vp].noXGrid();
    if ( ShowGrid & 2 )
      P[vp].setYGrid();
    else
      P[vp].noYGrid();

    // plot stimulus, restart, and recording events:
    for ( int s=0; s<PlotEvents.size(); s++ ) {
      if ( (PlotEvents[s].mode() & PlotTraceMode) &&
	   (PlotEvents[s].mode() & (StimulusEventMode+RestartEventMode+RecordingEventMode)) ) {
	P[vp].plot( PlotEvents[s], origin, Offset, tfac,
		    EventStyle[s].yPos(), EventStyle[s].yCoor(), EventStyle[s].line().width(),
		    EventStyle[s].point().type(), EventStyle[s].size(), EventStyle[s].sizeCoor(),
		    EventStyle[s].point().color(), EventStyle[s].point().fillColor() );
	break;
      }
    }
  }

  for ( unsigned int k=0; k<TraceStyle.size(); k++ ) {
    int vp = TraceStyle[k].panel();
    if ( TraceStyle[k].visible() && vp >= 0 ) {
      // plot events:
      int sn = 0;
      for ( int s=0; s<PlotEvents.size(); s++ ) {
	if ( (PlotEvents[s].mode() & PlotTraceMode) &&
	     !(PlotEvents[s].mode() & StimulusEventMode) &&
	     !(PlotEvents[s].mode() & RestartEventMode) &&
	     !(PlotEvents[s].mode() & RecordingEventMode) ) {
	
	  if ( RW->FD->eventInputTrace( s ) == (int)k ) {

	    if ( sn == 0 ) {
	      /*
		P[vp].plot( PlotEvents[s], origin, Offset, tfac,
		0.05, Plot::Graph, 1,
		Plot::StrokeUp, 20, Plot::Pixel,
		Plot::Red );
	      */
	      P[vp].plot( PlotEvents[s], PlotTraces[k],
			  origin, Offset, tfac,
			  1, Plot::Circle, 6, Plot::Pixel,
			  Plot::Gold, Plot::Gold );
	    }
	    else if ( sn == 1 )
	      P[vp].plot( PlotEvents[s], origin, Offset, tfac,
			  0.1, Plot::Graph, 1,
			  Plot::Circle, 6, Plot::Pixel,
			  Plot::Yellow, Plot::Yellow );
	    else if ( sn == 2 )
	      P[vp].plot( PlotEvents[s], origin, Offset, tfac, 0.2,
			  Plot::Graph, 1,
			  Plot::Diamond, 6, Plot::Pixel,
			  Plot::Blue, Plot::Blue );
	  
	    else
	      P[vp].plot( PlotEvents[s], origin, Offset, tfac,
			  0.3, Plot::Graph, 1,
			  Plot::TriangleUp, 6, Plot::Pixel,
			  Plot::Red, Plot::Red );
	  
	    sn++;
	  }
	}
      }
      // plot voltage trace:
      int color = TraceStyle[k].line().color();
      int lwidth = TraceStyle[k].line().width();
      TraceStyle[k].setHandle( P[vp].plot( PlotTraces[k], origin, Offset, tfac,
					   color, lwidth, Plot::Solid,
					   Plot::Circle, 0, color, color ) );
    }
  }

  updateStyle();

  // set xlabel:
  if ( VP.size() > 0 )
    P[VP.back()].setXLabel( tunit );

  // center mode:
  CenterMode = 0;
  CenterTime.start();

  // trigger source:
  TriggerSource = -1;
  for ( int s=0; s<PlotEvents.size(); s++ ) {
    if ( (PlotEvents[s].mode() & PlotTriggerMode) ) {
      TriggerSource = s;
      break;
    }
  }

  P.unlock();
}


void PlotTrace::plot( void )
{
  P.lock();
  bool plotting = Plotting;
  P.unlock();
  if ( !plotting ) 
    return;

  // get data:
  lock();
  getData();
  double sigtime = signalTime();
  if ( sigtime < 0.0 )
    sigtime = 0.0;
  unlock();

  // delayed setPlotSignal():
  if ( DoSignalTime >= 0.0 && DoSignalTime <= currentTime() ) {
    if ( DoSignalLength > 0.0 )
      setPlotSignal( DoSignalLength, DoSignalOffset );
    else
      setPlotSignal();
    DoSignalTime = -1.0;
  }

  if ( PlotChanged ) {
    init();
    PlotChanged = false;
  }

  P.lock();

  // set left- and rightmargin:
  double tfac = 1000.0;
  double leftwin = 0.0;
  double rightwin = tfac * TimeWindow;
  if ( ViewMode == SignalView ) {
    // offset fixed at signalTime():
    leftwin = -tfac * TimeOffs;
    rightwin = leftwin + tfac * TimeWindow;
    LeftTime = leftwin/tfac + sigtime;
    Offset = sigtime;
  }
  else if ( ViewMode == EndView || ( ViewMode == WrapView && TimeWindow < 0.5 ) ) {
    // offset continuous at currentTime():
    rightwin = tfac * ( currentTime() - sigtime );
    leftwin = rightwin - tfac * TimeWindow;
    LeftTime = leftwin/tfac + sigtime;
    Offset = sigtime;
  }
  else if ( ViewMode == WrapView ) {
    // offset wrapped at signalTime():
    leftwin = tfac * ::floor( ( currentTime() - sigtime ) / TimeWindow ) * TimeWindow;
    LeftTime = leftwin/tfac + sigtime;
    rightwin += leftwin;
    Offset = sigtime;
  }
  else {
    // offset fixed at LeftTime:
    leftwin = tfac * ( LeftTime - Offset );
    rightwin = leftwin + tfac * TimeWindow;
  }

  // align to trigger:
  if ( ( ViewMode == EndView || ViewMode == WrapView ) &&
       Trigger && TriggerSource >= 0 ) {
    int rinx = PlotEvents[TriggerSource].previous( LeftTime + TimeWindow );
    if ( rinx < 0 )
      rinx = 0;
    int pinx = PlotEvents[TriggerSource].previous( LeftTime );
    if ( pinx < 0 )
      pinx = 0;
    int nmin = 20;
    if ( rinx > nmin ) {
      int linx = pinx;
      if ( rinx - linx < nmin )
	linx = rinx - nmin;
      if ( linx < 0 )
	linx = 0;
      // CV of event intervals:
      ArrayD intervals;
      for ( int k=0; k<rinx-linx-1; k++ )
	intervals.push( PlotEvents[TriggerSource][linx+k+1] - PlotEvents[TriggerSource][linx+k] );
      double std = 0.0;
      double dt = meanStdev( std, intervals );
      double cv = std/dt;
      // we trigger only on sufficiently periodic events:
      if ( cv < 0.5 ) {
	// trigger time closest to LeftTime:
	int inx = pinx;
	int ninx = PlotEvents[TriggerSource].next( LeftTime );
	if ( ninx < PlotEvents[TriggerSource].size() &&
	     PlotEvents[TriggerSource][ninx] - LeftTime < LeftTime - PlotEvents[TriggerSource][pinx] )
	  inx = ninx;
	double tt = PlotEvents[TriggerSource][inx];
	if ( fabs( tt - LeftTime ) <= 1.7*dt ) {
	  LeftTime = tt;
	  leftwin = (LeftTime - sigtime)*tfac;
	  rightwin = leftwin + tfac * TimeWindow;
	}
      }
    }
  }

  // set xrange:
  deque<bool> tsunset;
  tsunset.resize( TraceStyle.size(), true );
  for ( unsigned int k=0; k<TraceStyle.size(); k++ ) {
    int vp = TraceStyle[k].panel();
    if ( TraceStyle[k].visible() && vp >= 0 && tsunset[k] ) {
      P[vp].setXRange( leftwin, rightwin );
      tsunset[k] = false;
      double min = PlotTraces[k].minValue();
      double max = PlotTraces[k].maxValue();
      for ( unsigned int j=k+1; j<TraceStyle.size(); j++ ) {
	if ( TraceStyle[j].visible() && TraceStyle[j].panel() == vp ) {
	  if ( min > PlotTraces[j].minValue() )
	    min = PlotTraces[j].minValue();
	  if ( max < PlotTraces[j].maxValue() )
	    max = PlotTraces[j].maxValue();
	  tsunset[j] = false;
	}
      }
      if ( ! P[vp].zoomedYRange() )
	P[vp].setYRange( min, max );
    }
  }

  updateStyle();

  // plot:
  P.draw();

  P.unlock();
}


void PlotTrace::updateStyle( void )
{
  lock();
  // line and pointstyle:
  for ( unsigned int k=0; k<TraceStyle.size(); k++ ) {
    int c = TraceStyle[k].panel();
    int h = TraceStyle[k].handle();
    if ( TraceStyle[k].visible() && c >= 0 && h >= 0 ) {
      double width = P[c].pixelPlotWidth();
      if ( width < 10.0 )
	width = P[c].pixelScreenWidth();
      if ( width < 10.0 )
	width = 10.0;
      int color = TraceStyle[k].line().color();
      int lwidth = TraceStyle[k].line().width();
      if ( PlotTraces[k].indices( TimeWindow )/width > 0.2 )
	P[c][h].setPoint( Plot::Circle, 0, color, color );
      else if ( PlotTraces[k].indices( TimeWindow )/width > 0.05 )
	P[c][h].setPoint( Plot::Circle, 4, color, color );
      else
	P[c][h].setPoint( Plot::Circle, 8, color, color );
      if ( PlotTraces[k].indices( TimeWindow )/width > 2.0 )
	P[c][h].setLine( color, lwidth, Plot::Solid );
      else
	P[c][h].setLine( color, 2*lwidth, Plot::Solid );
    }
  }
  unlock();
}


void PlotTrace::updateRanges( int id )
{
  // P is already locked!
  double tfac = 0.001;
  TimeWindow = tfac * ( P[id].xmaxRange() - P[id].xminRange() );
  TimeOffs = -tfac * P[id].xminRange();
  LeftTime = Offset - TimeOffs;
}


void PlotTrace::addMenu( QMenu *menu )
{
  Menu = menu;

  Menu->addAction( "Zoom &in", this, SLOT( zoomIn() ), Qt::Key_Plus );
  Menu->addAction( "Zoom &out", this, SLOT( zoomOut() ), Qt::Key_Minus );
  Menu->addAction( "Move &left", this, SLOT( moveLeft() ), Qt::Key_PageUp );
  Menu->addAction( "Move &right", this, SLOT( moveRight() ), Qt::Key_PageDown );
  Menu->addAction( "&Begin", this, SLOT( moveStart() ), Qt::CTRL + Qt::Key_PageUp );
  Menu->addAction( "&End", this, SLOT( moveEnd() ), Qt::CTRL + Qt::Key_PageDown );
  Menu->addAction( "Move to current signal", this, SLOT( moveToSignal() ), Qt::CTRL + Qt::Key_Home );
  Menu->addAction( "&Signal view", this, SLOT( viewSignal() ), Qt::Key_Home );
  Menu->addAction( "Move offset left", this, SLOT( moveSignalOffsLeft() ), Qt::SHIFT + Qt::Key_PageUp );
  Menu->addAction( "Move offset right", this, SLOT( moveSignalOffsRight() ), Qt::SHIFT + Qt::Key_PageDown );
  Menu->addAction( "&End view", this, SLOT( viewEnd() ), Qt::Key_End );
  Menu->addAction( "&Wrapped view", this, SLOT( viewWrapped() ), Qt::Key_Insert );
  Menu->addAction( "&Trigger", this, SLOT( toggleTrigger() ), Qt::CTRL + Qt::Key_T );
  Menu->addAction( "Toggle &grid", this, SLOT( toggleGrid() ), Qt::CTRL + Qt::Key_G );
  Menu->addAction( "&Manual", this, SLOT( manualRange() ), Qt::CTRL + Qt::Key_M );
  Menu->addAction( "&Auto", this, SLOT( autoRange() ), Qt::CTRL + Qt::Key_A );
  Menu->addAction( "Center &vertically", this, SLOT( centerVertically() ), Qt::Key_V );
  Menu->addAction( "&Zoom vertically", this, SLOT( centerZoomVertically() ), Qt::SHIFT + Qt::Key_V );
  Menu->addAction( "Zoom in &Y-axis", this, SLOT( zoomInVertically() ), Qt::SHIFT + Qt::Key_Y );
  Menu->addAction( "Zoom out &Y-axis", this, SLOT( zoomOutVertically() ), Qt::Key_Y );
  Menu->addAction( "&Toggle plotting", this, SLOT( plotOnOff() ) );
  Menu->addAction( "Zoom back", this, SLOT( zoomBack() ), Qt::ALT + Qt::Key_Left );
  Menu->addAction( "Reset zoom", this, SLOT( resetZoom() ), Qt::ALT + Qt::Key_Right );
  Menu->addAction( "&Print", this, SLOT( print() ), Qt::CTRL + Qt::Key_P );

  Menu->addSeparator();

  TraceStyle.clear();
  EventStyle.clear();
  connect( Menu, SIGNAL( triggered( QAction* ) ),
	   this, SLOT( toggle( QAction* ) ) );
}


void PlotTrace::updateMenu( void )
{
  if ( Menu != 0 ) {

    P.lock();

    // get traces and events:
    PlotTraces.clear();
    PlotEvents.clear();
    if ( FilePlot ) {
      PlotTraces.add( FileTraces );
      PlotEvents.add( FileEvents );
    }
    else {
      PlotTraces.add( traces() );
      PlotEvents.add( events() );
    }

    // setup plots:
    if ( P.size() != PlotTraces.size() )
      P.resize( PlotTraces.size(), Plot::Pointer );
    P.setDataMutex( mutex() );
    P.setCommonXRange();

    // add new traces to menu:
    for ( int k=0; k<PlotTraces.size(); k++ ) {
      string s = "&" + Str( k+1 );
      s += " ";
      s += PlotTraces[k].ident();
      TraceStyle[k].setAction( Menu->addAction( s.c_str() ) );
      TraceStyle[k].action()->setCheckable( true );
      TraceStyle[k].setVisible( true );
      TraceStyle[k].action()->setChecked( TraceStyle[k].visible() );
    }

    P.unlock();
  }
}


void PlotTrace::start( double time )
{
  PlotTimer.start( (int)::rint( 1000.0*time ) );
}


void PlotTrace::stop( void )
{
  PlotTimer.stop();
}


double PlotTrace::signalTime( void ) const
{
  if ( FilePlot && FileTraces.size() > 0 )
    return FileTraces[0].signalTime();
  else
    return RELACSPlugin::signalTime();
}


double PlotTrace::currentTime( void ) const
{
  if ( FilePlot && FileTraces.size() > 0 )
    return FileTraces.currentTimeRaw();
  else
    return RELACSPlugin::currentTimeRaw();
}


void PlotTrace::setPlotOn( bool on )
{
  P.lock();
  AutoOn = on;
  bool man = Manual;
  P.unlock();

  if ( man )
    return;

  P.lock();

  // toggle plot:
  Plotting = on;
  postCustomEvent( 11 );  // animate on/off button
  P.unlock();
}


void PlotTrace::setPlotOff( void )
{
  setPlotOn( false );
}


void PlotTrace::setPlotNextSignal( double length, double offs )
{
  DoSignalLength = length;
  DoSignalOffset = offs;
  lock();
  DoSignalTime = currentTime();
  unlock();
}


void PlotTrace::setPlotNextSignal( void )
{
  DoSignalLength = -1.0;
  DoSignalOffset = -1.0;
  lock();
  DoSignalTime = currentTime();
  unlock();
}


void PlotTrace::setPlotSignal( double length, double offs )
{
  P.lock();
  AutoOn = true;
  AutoFixed = true;
  AutoTime = length;
  AutoOffs = offs;
  bool man = Manual;
  P.unlock();

  if ( man )
    return;

  P.lock();

  // toggle plot:
  Plotting = true;
  postCustomEvent( 11 );  // animate on/off button

  // toggle fixed offset:
  setView( SignalView );

  // length of total time window:
  TimeWindow = length;

  // left offset to signal:
  TimeOffs = offs;

  updateStyle();

  P.unlock();
}


void PlotTrace::setPlotSignal( void )
{
  P.lock();
  AutoOn = true;
  AutoFixed = true;
  bool man = Manual;
  P.unlock();

  if ( man )
    return;

  P.lock();

  // toggle plot:
  Plotting = true;
  postCustomEvent( 11 );  // animate on/off button

  // toggle fixed offset:
  setView( SignalView );

  P.unlock();
}


void PlotTrace::setPlotContinuous( double length )
{
  P.lock();
  AutoOn = true;
  AutoFixed = false;
  AutoTime = length;
  bool man = Manual;
  P.unlock();

  if ( man )
    return;

  P.lock();

  // toggle plot:
  Plotting = true;
  postCustomEvent( 11 );  // animate on/off button

  // toggle fixed offset:
  setView( ContView );

  // length of total time window:
  TimeWindow = length;

  updateStyle();

  P.unlock();
}


void PlotTrace::setPlotContinuous( void )
{
  P.lock();
  AutoOn = true;
  AutoFixed = false;
  bool man = Manual;
  P.unlock();

  if ( man )
    return;

  P.lock();

  // toggle plot:
  Plotting = true;
  postCustomEvent( 11 );  // animate on/off button

  // toggle fixed offset:
  setView( ContView );

  P.unlock();
}


void PlotTrace::zoomOut( void )
{
  P.lock();
  TimeWindow *= 2;
  TimeOffs *= 2;
  P.unlock();
  if ( RW->idle() )
    plot();
  else {
    P.lock();
    updateStyle();
    P.unlock();
  }
}


void PlotTrace::zoomIn( void )
{
  P.lock();
  TimeWindow /= 2.0;
  TimeOffs /= 2.0;
  P.unlock();
  if ( RW->idle() )
    plot();
  else {
    P.lock();
    updateStyle();
    P.unlock();
  }
}


void PlotTrace::moveLeft( void )
{
  P.lock();
  if ( ViewMode != FixedView )
    setView( FixedView );
  else
    LeftTime -= 0.5 * TimeWindow;
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveRight( void )
{
  P.lock();
  if ( ViewMode != FixedView )
    setView( FixedView );
  else
    LeftTime += 0.5 * TimeWindow;
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveStart( void )
{
  P.lock();
  if ( ViewMode != FixedView )
    setView( FixedView );
  LeftTime = 0.0;
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveEnd( void )
{
  P.lock();
  if ( ViewMode != FixedView )
    setView( FixedView );
  LeftTime = currentTime() - TimeWindow;
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveToSignal( void )
{
  P.lock();
  if ( ViewMode == SignalView )
    TimeOffs = 0.0;
  else {
    if ( ViewMode != FixedView )
      setView( FixedView );
    double sigtime = signalTime();
    if ( sigtime < 0.0 )
      sigtime = 0.0;
    LeftTime = sigtime - TimeOffs;
  }
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::viewSignal( void )
{
  P.lock();
  setView( SignalView );
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveSignalOffsLeft( void )
{
  P.lock();
  if ( ViewMode == SignalView || ViewMode == FixedView ) {
    TimeOffs += 0.25 * TimeWindow;
    if ( ViewMode == FixedView )
      LeftTime = Offset - TimeOffs;
  }
  else
    setView( SignalView );
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveSignalOffsRight( void )
{
  P.lock();
  if ( ViewMode == SignalView || ViewMode == FixedView ) {
    TimeOffs -= 0.25 * TimeWindow;
    if ( ViewMode == FixedView )
      LeftTime = Offset - TimeOffs;
  }
  else
    setView( SignalView );
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::viewEnd( void )
{
  P.lock();
  setView( EndView );
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::viewWrapped( void )
{
  P.lock();
  setView( WrapView );
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::toggleGrid( void )
{
  P.lock();
  ShowGrid++;
  if ( ShowGrid > 3 )
    ShowGrid = 0;
  PlotChanged = true;
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::toggleTrigger( void )
{
  P.lock();
  Trigger = ! Trigger;
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::plotOnOff( )
{
  P.lock();
  Plotting = ! Plotting;
  int p = Plotting;
  P.unlock();
  if ( OnOffButton != 0 )
    OnOffButton->setDown( ! p );
}


void PlotTrace::toggleManual( void )
{
  P.lock();
  bool man = Manual;
  P.unlock();
  if ( man ) 
    autoRange();
  else
    manualRange();
}


void PlotTrace::manualRange( void )
{
  P.lock();
  Manual = true;
  if ( ManualButton != 0 )
    ManualButton->setDown( Manual );
  P.unlock();
}


void PlotTrace::autoRange( void )
{
  P.lock();
  Manual = false;
  if ( ManualButton != 0 )
    ManualButton->setDown( Manual );

  // toggle plot:
  Plotting = AutoOn;
  if ( OnOffButton != 0 ) {
    if ( Plotting )
      OnOffButton->setDown( false );
    else
      OnOffButton->setDown( true );
  }

  // toggle fixed offset:
  setView( AutoFixed ? SignalView : WrapView );

  // length of total time window:
  TimeWindow = AutoTime;

  // left offset to signal:
  TimeOffs = AutoOffs;

  updateStyle();

  P.unlock();
}


void PlotTrace::centerVertically( void )
{
  P.lock();
  vector<bool> pcenter( P.size(), false );
  vector<double> pmin( P.size(), 0.0 );
  vector<double> pmax( P.size(), 0.0 );
  vector<double> pminrange( P.size(), 0.0 );
  vector<double> pmaxrange( P.size(), 0.0 );

  // get minimum and maximum values for each plot:
  double tfac = 1000.0;
  for ( unsigned int c=0; c<TraceStyle.size(); c++ ) {
    if ( TraceStyle[c].visible() &&
	 TraceStyle[c].panel() >= 0 &&
	 PlotTraces[c].mode() & PlotTraceCenterVertically ) {
      int vp = TraceStyle[c].panel();
      double xmin = P[vp].xminRange()/tfac + Offset;
      double xmax = P[vp].xmaxRange()/tfac + Offset;
      // make sure, there are enough data shown in the window:
      if ( PlotTraces[c].currentTime() < 0.5*(xmin+xmax) ) {
	xmin = PlotTraces[c].currentTime() - (xmax-xmin);
	xmax = PlotTraces[c].currentTime();
      }
      float min = 0.0;
      float max = 0.0;
      PlotTraces[c].minMax( min, max, xmin, xmax );
      if ( pcenter[vp] ) {
	if ( pmin[vp] > min )
	  pmin[vp] = min;
	if ( pmax[vp] < max )
	  pmax[vp] = max;
	if ( pminrange[vp] > PlotTraces[c].minValue() )
	  pminrange[vp] = PlotTraces[c].minValue();
	if ( pmaxrange[vp] < PlotTraces[c].maxValue() )
	  pmaxrange[vp] = PlotTraces[c].maxValue();
      }
      else {
	pcenter[vp] = true;
	pmin[vp] = min;
	pmax[vp] = max;
	pminrange[vp] = PlotTraces[c].minValue();
	pmaxrange[vp] = PlotTraces[c].maxValue();
      }
    }
  }

  // reset center mode:
  if ( CenterTime.restart() > 2000 )
    CenterMode = 0;

  // center each plot:
  for ( int vp=0; vp<P.size(); vp++ ) {
    if ( pcenter[vp] ) {
      double center = 0.5*(pmin[vp]+pmax[vp]);
      double range = P[vp].ymaxRange() - P[vp].yminRange();
      double nmin = center - (0.5+0.4*CenterMode)*range;
      double nmax = center + (0.5-0.4*CenterMode)*range;
      if ( nmin < pminrange[vp] ) {
	nmin = pminrange[vp];
	nmax = nmin + range;
      }
      if ( nmax > pmaxrange[vp] ) {
	nmax = pmaxrange[vp];
	nmin = nmax - range;
      }
      if ( P[vp].ranges() == 0 )
	P[vp].pushRanges();
      P[vp].setYRange( nmin, nmax );
    }
  }

  // update center mode:
  CenterMode++;
  if ( CenterMode > 1 )
    CenterMode = -1;

  P.unlock();
}


void PlotTrace::centerZoomVertically( void )
{
  P.lock();
  lock();
  vector<bool> pcenter( P.size(), false );
  vector<double> pmin( P.size(), 0.0 );
  vector<double> pmax( P.size(), 0.0 );
  vector<double> pminrange( P.size(), 0.0 );
  vector<double> pmaxrange( P.size(), 0.0 );

  // get minimum and maximum values for each plot:
  double tfac = 1000.0;
  for ( unsigned int c=0; c<TraceStyle.size(); c++ ) {
    if ( TraceStyle[c].visible() &&
	 TraceStyle[c].panel() >= 0 &&
	 PlotTraces[c].mode() & PlotTraceCenterVertically ) {
      int vp = TraceStyle[c].panel();
      double xmin = P[vp].xminRange()/tfac + Offset;
      double xmax = P[vp].xmaxRange()/tfac + Offset;
      // make sure, there are enough data shown in the window:
      if ( PlotTraces[c].currentTime() < 0.5*(xmin+xmax) ) {
	xmin = PlotTraces[c].currentTime() - (xmax-xmin);
	xmax = PlotTraces[c].currentTime();
      }
      float min = 0.0;
      float max = 0.0;
      PlotTraces[c].minMax( min, max, xmin, xmax );
      if ( pcenter[vp] ) {
	if ( pmin[vp] > min )
	  pmin[vp] = min;
	if ( pmax[vp] < max )
	  pmax[vp] = max;
	if ( pminrange[vp] > PlotTraces[c].minValue() )
	  pminrange[vp] = PlotTraces[c].minValue();
	if ( pmaxrange[vp] < PlotTraces[c].maxValue() )
	  pmaxrange[vp] = PlotTraces[c].maxValue();
      }
      else {
	pcenter[vp] = true;
	pmin[vp] = min;
	pmax[vp] = max;
	pminrange[vp] = PlotTraces[c].minValue();
	pmaxrange[vp] = PlotTraces[c].maxValue();
      }
    }
  }

  // scale and center each plot:
  for ( int vp=0; vp<P.size(); vp++ ) {
    if ( pcenter[vp] ) {
      double center = 0.5*(pmin[vp]+pmax[vp]);
      double range = 0.6*(pmax[vp]-pmin[vp]);
      double nmin = center-range;
      double nmax = center+range;
      if ( nmin < pminrange[vp] ) {
	nmin = pminrange[vp];
	nmax = nmin + 2.0*range;
      }
      if ( nmax > pmaxrange[vp] ) {
	nmax = pmaxrange[vp];
	nmin = nmax - 2.0*range;
      }
      if ( P[vp].ranges() == 0 )
	P[vp].pushRanges();
      P[vp].setYRange( nmin, nmax );
    }
  }

  unlock();
  P.unlock();
}


void PlotTrace::zoomInVertically( void )
{
  P.lock();
  lock();

  // zoom yaxis of each visible and enabled plot:
  for ( unsigned int c=0; c<TraceStyle.size(); c++ ) {
    if ( TraceStyle[c].visible() &&
	 TraceStyle[c].panel() >= 0 &&
	 PlotTraces[c].mode() & PlotTraceCenterVertically ) {
      int vp = TraceStyle[c].panel();
      double ymin = P[vp].yminRange();
      double ymax = P[vp].ymaxRange();
      double center = 0.5*(ymin+ymax);
      double range = 0.25*(ymax-ymin);
      double nmin = center-range;
      double nmax = center+range;
      if ( P[vp].ranges() == 0 )
	P[vp].pushRanges();
      P[vp].setYRange( nmin, nmax );
    }
  }

  unlock();
  P.unlock();
}


void PlotTrace::zoomOutVertically( void )
{
  P.lock();
  lock();

  // zoom yaxis of each visible and enabled plot:
  for ( unsigned int c=0; c<TraceStyle.size(); c++ ) {
    if ( TraceStyle[c].visible() &&
	 TraceStyle[c].panel() >= 0 &&
	 PlotTraces[c].mode() & PlotTraceCenterVertically ) {
      int vp = TraceStyle[c].panel();
      double ymin = P[vp].yminRange();
      double ymax = P[vp].ymaxRange();
      double center = 0.5*(ymin+ymax);
      double range = ymax-ymin;
      double nmin = center-range;
      double nmax = center+range;
      if ( nmin < PlotTraces[c].minValue() ) {
	nmin = PlotTraces[c].minValue();
	nmax = nmin + 2.0*range;
	if ( nmax > PlotTraces[c].maxValue() )
	  nmax = PlotTraces[c].maxValue();
      }
      if ( nmax > PlotTraces[c].maxValue() ) {
	nmax = PlotTraces[c].maxValue();
	nmin = nmax - 2.0*range;
	if ( nmin < PlotTraces[c].minValue() )
	  nmin = PlotTraces[c].minValue();
      }
      if ( P[vp].ranges() == 0 )
	P[vp].pushRanges();
      P[vp].setYRange( nmin, nmax );
    }
  }

  unlock();
  P.unlock();
}


void PlotTrace::zoomBack( void )
{
  P.lock();
  P.popRanges();
  P.unlock();
}


void PlotTrace::resetZoom( void )
{
  P.lock();
  P.resetRanges();
  P.unlock();
}


void PlotTrace::viewToggle( void )
{
  P.lock();
  if ( ViewMode == EndView )
    setView( SignalView );
  else if ( ViewMode == SignalView )
    setView( WrapView );
  else if ( ViewMode == WrapView )
    setView( EndView );
  P.unlock();
}


void PlotTrace::setView( Views mode )
{
  if ( ViewMode != mode ) {
    if ( mode == EndView || mode == WrapView )
      ContinuousView = mode;
    if ( mode == ContView )
      mode = ContinuousView;
    DoSignalTime = -1.0;
    ViewMode = mode;
    PlotChanged = true;
    postCustomEvent( 12 );
  }
}


PrintThread::PrintThread( const string &printcommand )
  : PrintCommand( printcommand )
{
}


void PrintThread::run( void )
{
  if ( system( PrintCommand.c_str() ) != 0 )
    cerr << "failed to execute " << PrintCommand << '\n';
}


void PlotTrace::print( void )
{
  TableKey datakey;
  datakey.addNumber( "Time", "ms", "%7.3f" );
  for ( unsigned int k=0; k<TraceStyle.size(); k++ ) {
    if ( TraceStyle[k].visible() )
      datakey.addNumber( PlotTraces[k].ident(), PlotTraces[k].unit(), "%7.3f" );
  }

  ofstream df( "traces.dat" );
  lockMetaData();
  metaData().Options::save( df, "# ", 0, Options::FirstOnly );
  unlockMetaData();
  lockStimulusData();
  stimulusData().save( df, "# " );
  unlockStimulusData();
  df << '\n';
  datakey.saveKey( df );
  bool validdata = true;
  for ( int j=PlotTraces[0].index( LeftTime );
	j<PlotTraces[0].index( LeftTime + TimeWindow ) && validdata;
	j++ ) {
    datakey.save( df, 1000.0*( PlotTraces[0].pos(j) - LeftTime ), 0 );
    for ( unsigned int k=0; k<TraceStyle.size(); k++ ) {
      if ( TraceStyle[k].visible() ) {
	if ( j >= PlotTraces[k].size() ) {
	  validdata = false;
	  break;
	}
	datakey.save( df, PlotTraces[k][j] );
      }
    }
    df << '\n';
  }
  df << "\n\n";
  df.close();

  printlog( "saved currently visible traces to traces.dat" );

  string printcommand = relacsSettings().text( "printcommand" );
  if ( ! printcommand.empty() ) {
    printlog( "execute " + printcommand );
    PrintThread *printthread = new PrintThread( printcommand );
    connect( printthread, SIGNAL( finished() ), printthread, SLOT( deleteLater() ) );
    printthread->start();
  }
}


void PlotTrace::displayIndex( const string &fpath, const deque<int> &traceindex,
			      const deque<int> &eventsindex, double time )
{
  bool success = true;
  bool newfile = false;
  P.lock();
  // XXX this comparison needs the absolute path of both sides:
  if ( Str( fpath ).dir() == defaultPath() ) {
    if ( FilePlot ) {
      newfile = true;
      FilePlot = false;
      FileHeader.clear();
      FileTraces.clear();
      FileTracesNames.clear();
      FileSizes.clear();
      FileEvents.clear();
      FileEventsNames.clear();
    }
  }
  else {
    if ( FilePath != fpath ) {
      newfile = true;
      FileHeader.clear();
      FileTraces.clear();
      FileTracesNames.clear();
      FileSizes.clear();
      FileEvents.clear();
      FileEventsNames.clear();

      // read in meta data:
      DataFile sf( fpath );
      sf.readMetaData();
      Options header = sf.metaDataOptions( sf.levels()-1 );
      if ( header.empty() )
	success = false;
      else
	FileHeader = header;

      // create traces:
      const Options *aiopts = &FileHeader.section( "analog input traces" );
      if ( aiopts->empty() )
	aiopts = &FileHeader;
      for ( int k=1; ; k++ ) {
	string tracename = aiopts->text( "identifier" + Str( k ), "" );
	if ( tracename.empty() )
	  break;
	string tracefile = aiopts->text( "data file" + Str( k ) );
	double tracestep = aiopts->number( "sample interval" + Str( k ), "ms" );
	// we need to guess the exact rate:
	double rate = ::round( 1.0/tracestep );
	tracestep = 0.001/rate;
	string traceunit = aiopts->text( "unit" + Str( k ) );

	InData id;
	id.setIdent( tracename );
	id.setTrace( k-1 );
	id.setStepsize( tracestep );
	id.setStartSource( 0 );
	id.setUnipolar( false );
	id.setUnit( 1.0, traceunit );
	id.setChannel( k-1 );
	id.setMinValue( -150.0 );  // XXX get this somehow from the configuration file?
	id.setMaxValue( +150.0 );
	id.setDevice( 0 );
	id.setContinuous();
	int m = PlotTraceMode;
	/*
	  if ( boolean( "inputtracecenter", k, false ) )
	  m |= PlotTraceCenterVertically;
	*/
	id.setMode( m );
	// id.setReference( text( "inputtracereference", k, InData::referenceStr( InData::RefGround ) ) );
	id.setGainIndex( 0 );
	FileTraces.push( id );
	// FileTraces.back().reserve( id.indices( number( "inputtracecapacity", 0, 1000.0 ) ) );
	FileTraces.back().reserve( 60.0 );
	FileTracesNames.push_back( Str( fpath ).dir() + tracefile );
	ifstream is( FileTracesNames.back().c_str() );
	if ( k-1 >= (int)FileSizes.size() ) {
	  is.seekg( 0, is.end );
	  FileSizes.push_back( is.tellg()/sizeof(float) );
	}
      }
      for ( int k=1; ; k++ ) {
	string eventfile = FileHeader.text( "event file" + Str( k ), "" );
	if ( eventfile.empty() )
	  break;
	FileEventsNames.push_back( Str( fpath ).dir() + eventfile );
	DataFile sf( FileEventsNames.back() );
	sf.readMetaData();
	Options header = sf.metaDataOptions( sf.levels()-1 );
	bool eventsizes = ( sf.key().columns() > 1 );
	bool eventwidths = ( sf.key().columns() > 2 );
	EventData ed( 1000, eventsizes, eventwidths );
	ed.setCyclic();
	string eventname = header.text( "events" );
	ed.setIdent( eventname );
	if ( eventname == "Stimulus" )
	  ed.setMode( StimulusEventMode );
	else if ( eventname == "Restart" )
	  ed.setMode( RestartEventMode );
	else if ( eventname == "Recording" )
	  ed.setMode( RecordingEventMode );
	ed.setMode( ed.mode() | PlotTraceMode );
	if ( eventsizes ) {
	  ed.setSizeName( sf.key().name( 1 ) );
	  ed.setSizeUnit( sf.key().unit( 1 ) );
	}
	if ( eventwidths ) {
	  ed.setWidthName( sf.key().name( 2 ) );
	  ed.setWidthUnit( sf.key().unit( 2 ) );
	}
	FileEvents.push( ed );
      }
    }
    if ( FilePlot && FileTraces.size() > 0 && FileTraces[0].size() > 0 &&
	 FileTraces[0].minIndex() <= traceindex[0] && 
	 ( FileTraces[0].size() == FileSizes[0] ||
	   FileTraces[0].size() >= traceindex[0] + FileTraces[0].indices( 10.0 ) ) ) {
      time = FileTraces[0].pos( traceindex[0] );
    }
    else {
      // load data:
      for ( int k=0; k<FileTraces.size(); k++ ) {
	int index = traceindex[k];
	index -= FileTraces.back().indices( 10.0 );  // we want to have some time before as well, at least TimeOffs!
	if ( index + FileTraces.back().capacity() > FileSizes[k] )
	  index = FileSizes[k] - FileTraces.back().capacity();
	if ( index < 0 )
	  index = 0;
	ifstream is( FileTracesNames[k].c_str() );
	FileTraces[k].loadBinary( is, index );
	FileTraces[k].setSignalIndex( traceindex[k] );
	time = FileTraces[k].pos( traceindex[k] );
      }
      for ( int k=0; k<FileEvents.size(); k++ ) {
	DataFile sf( FileEventsNames[k] );
	sf.read( 10 );
	int index = eventsindex[k];
	if ( index >=0 && index < sf.data().rows() )
	  for ( ; index>=0 && sf.data( 0, index ) > time-10.0; index-- );
	if ( index + FileEvents[k].capacity() > sf.data().rows() )
	  index = sf.data().rows() - FileEvents[k].capacity();
	if ( index < 0 )
	  index = 0;
	FileEvents[k].set( index, sf.col( 0 ), sf.col( 1 ), sf.col( 2 ) );  
      }
    }
    FilePlot = true;
  }
  if ( success ) {
    if ( DataView == NoView )
      DataView = ViewMode;
    if ( ViewMode != FixedView ) {
      TimeOffs = 0.0;
      setView( FixedView );
    }
    Offset = time;
    LeftTime = Offset - TimeOffs;
    PlotChanged = true;
  }
  P.unlock();
  if ( success && newfile ) {
    updateMenu();
    resize();
  }
  FilePath = fpath;
  if ( RW->idle() )
    plot();
}


void PlotTrace::displayData( void )
{
  P.lock();
  bool fp = FilePlot;
  P.unlock();
  if ( fp ) {
    P.lock();
    FilePlot = false;
    FilePath = path();
    FileHeader.clear();
    FileTraces.clear();
    FileTracesNames.clear();
    FileSizes.clear();
    FileEvents.clear();
    FileEventsNames.clear();
    if ( DataView == NoView )
      setView( WrapView );
    else
      setView( DataView );
    DataView = NoView;
    Offset = trace( 0 ).currentTime() - TimeWindow;
    LeftTime = Offset - TimeOffs;
    PlotChanged = true;
    P.unlock();
    updateMenu();
    resize();
    if ( RW->idle() )
      plot();
  }
}


void PlotTrace::keyPressEvent( QKeyEvent *event )
{
  event->accept();
  switch ( event->key() ) {

  case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4: case Qt::Key_5: 
  case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9: {
    int n = event->key() - Qt::Key_1;
    if ( n >=0 && n < (int)TraceStyle.size() )
      toggle( TraceStyle[n].action() );
    break;
  }

  case Qt::Key_Equal:
  case Qt::Key_Plus:
    zoomIn();
    break;

  case Qt::Key_Minus:
    zoomOut();
    break;

  case Qt::Key_Y:
    if ( event->modifiers() & Qt::SHIFT )
      zoomInVertically();
    else
      zoomOutVertically();
    break;

  default: {
    RELACSPlugin::keyPressEvent( event );
  }

  }
}


void PlotTrace::resizeEvent( QResizeEvent *qre )
{
  /*
  P.lock();
  resizeLayout();
  P.unlock();

  P.resizeEvent( qre );
  */
  if ( ButtonBox == 0 )
    return;

  QSize bs = ButtonBox->sizeHint();
  /*
  ButtonBox->setGeometry( width() - bs.width(), 0,
			  bs.width(), bs.height() );
  */
  ButtonBox->setGeometry( 0, 0, bs.width(), bs.height() );
}


void PlotTrace::resizePlots( QResizeEvent *qre )
{
  P.lock();
  resizeLayout();
  P.unlock();
}


void PlotTrace::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {

  case 11 :
    if ( OnOffButton != 0 ) {
      P.lock();
      bool plotting = Plotting;
      P.unlock();
      if ( plotting )
	OnOffButton->setDown( false );
      else
	OnOffButton->setDown( true );
    }
    break;

  case 12:
    if ( ViewButton != 0 ) {
      if ( ViewMode == SignalView )
	ViewButton->setIcon( SignalViewIcon );
      else if ( ViewMode == WrapView )
	printlog( "warnig: PlotTrace WrapIcon not implemented yet!" );
      else
	ViewButton->setIcon( EndViewIcon );
    }
    break;

  }
}


PlotTraceStyle::PlotTraceStyle( void )
  : Action( 0 ),
    Visible( true ),
    Panel( -1 ),
    Handle( -1 ),
    Line( Plot::Green, 1, Plot::Solid )
{
}


PlotTraceStyle::  PlotTraceStyle( const PlotTraceStyle &pts )
  : Action( pts.Action ),
    Visible( pts.Visible ),
    Panel( pts.Panel ),
    Handle( pts.Handle ),
    Line( pts.Line )
{
}


PlotTraceStyle::PlotTraceStyle( bool visible, int panel, int lcolor )
  : Action( 0 ),
    Visible( visible ),
    Panel( panel ),
    Handle( -1 ),
    Line( lcolor, 1, Plot::Solid )
{
}


bool PlotTraceStyle::visible( void ) const
{
  return Visible;
}

void PlotTraceStyle::setVisible( bool visible )
{
  Visible = visible;
}


int PlotTraceStyle::panel( void ) const
{
  return Panel;
}


void PlotTraceStyle::setPanel( int panel )
{
  Panel = panel;
}


int PlotTraceStyle::handle( void ) const
{
  return Handle;
}


void PlotTraceStyle::setHandle( int handle )
{
  Handle = handle;
}


void PlotTraceStyle::clearPanel( void )
{
  Panel = -1;
  Handle = -1;
}


const QAction *PlotTraceStyle::action( void ) const
{
  return Action;
}


QAction *PlotTraceStyle::action( void )
{
  return Action;
}


void PlotTraceStyle::setAction( QAction *action )
{
  Action = action;
}


const Plot::LineStyle &PlotTraceStyle::line( void ) const
{
  return Line;
}


Plot::LineStyle &PlotTraceStyle::line( void )
{
  return Line;
}


void PlotTraceStyle::setLine( const Plot::LineStyle &style )
{
  Line = style;
}


void PlotTraceStyle::setLine( int lcolor, int lwidth, Plot::Dash ldash )
{
  setLine( Plot::LineStyle( lcolor, lwidth, ldash ) );
}


PlotEventStyle::PlotEventStyle( void )
  : PlotTraceStyle(),
    Point( Plot::Circle, 1, Plot::Yellow, Plot::Yellow ),
    YPos( 0.1 ),
    YCoor( Plot::Graph ),
    YData( false ),
    Size( 6.0 ),
    SizeCoor( Plot::Pixel )
{
  setLine( Plot::Transparent, 0, Plot::Solid );
}


PlotEventStyle::PlotEventStyle( const PlotEventStyle &pes )
  : PlotTraceStyle( pes ),
    Point( pes.Point ),
    YPos( pes.YPos ),
    YCoor( pes.YCoor ),
    YData( pes.YData ),
    Size( pes.Size ),
    SizeCoor( pes.SizeCoor )
{
}


const Plot::PointStyle &PlotEventStyle::point( void ) const
{
  return Point;
}


Plot::PointStyle &PlotEventStyle::point( void )
{
  return Point;
}


void PlotEventStyle::setPoint( const Plot::PointStyle &style )
{
  Point = style;
}


void PlotEventStyle::setPoint( Plot::Points ptype, int psize, int pcolor, int pfill )
{
  setPoint( Plot::PointStyle( ptype, psize, pcolor, pfill ) );
}


void PlotEventStyle::setStyle( const Plot::LineStyle &lstyle, 
			       const Plot::PointStyle &pstyle )
{ 
  Line = lstyle;
  Point = pstyle;
}


void PlotEventStyle::setStyle( int lcolor, int lwidth, Plot::Dash ldash, 
			       Plot::Points ptype, int psize, int pcolor, 
			       int pfill )
{
  setLine( lcolor, lwidth, ldash );
  setPoint( ptype, psize, pcolor, pfill );
}


double PlotEventStyle::yPos( void ) const
{
  return YPos;
}


Plot::Coordinates PlotEventStyle::yCoor( void ) const
{
  return YCoor;
}


void PlotEventStyle::setYPos( double ypos, Plot::Coordinates ycoor )
{
  YPos = ypos;
  YCoor = ycoor;
  YData = false;
}


void PlotEventStyle::setYData( void )
{
  YData = true;
}


double PlotEventStyle::size( void ) const
{
  return Size;
}


Plot::Coordinates PlotEventStyle::sizeCoor( void ) const
{
  return SizeCoor;
}


void PlotEventStyle::setSize( double size, Plot::Coordinates sizecoor )
{
  Size = size;
  SizeCoor = sizecoor;
}


}; /* namespace relacs */

#include "moc_plottrace.cc"

