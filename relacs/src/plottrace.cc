/*
  plottrace.cc
  Plot traces and events.

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

#include <iostream>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QPolygon>
#include <QDateTime>
#include <QLabel>
#include <QToolTip>
#include <QApplication>
#include <relacs/str.h>
#include <relacs/relacswidget.h>
#include <relacs/plottrace.h>

namespace relacs {


PlotTrace::PlotTrace( RELACSWidget *rw, QWidget* parent )
  : RELACSPlugin( "PlotTrace: PlotTrace", RELACSPlugin::Plugins,
		  "PlotTrace", "base", "Jan Benda", "1.0", "Dec 1, 2009" ),
    PlotElements( 1, -1 ),
    Menu( 0 ),
    P( 1, Plot::Pointer, parent )
{
  setRELACSWidget( rw );

  ViewMode = SignalView;
  setView( WrapView );
  Manual = false;
  Plotting = true;

  connect( &P, SIGNAL( changedRanges( int ) ),
	   this, SLOT( updateRanges( int ) ) );
  connect( &P, SIGNAL( resizePlots( QResizeEvent* ) ),
	   this, SLOT( resizePlots( QResizeEvent* ) ) );

  TimeWindow = 0.1;
  TimeOffs = 0.0;
  Offset = 0.0;

  AutoOn = true;
  AutoFixed = false;
  AutoTime = 0.1;
  AutoOffs = 0.0;

  ButtonBox = 0;
  ViewButton = 0;
  ManualButton = 0;
  OnOffButton = 0;

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


void PlotTrace::resize( void )
{
  // count active plots:
  P.lockData();
  VP.clear();
  VP.reserve( traces().size() );
  for ( int c=0; c<traces().size(); c++ ) {
    if ( trace(c).mode() & PlotTraceMode )
      VP.push_back( c );
  }
  P.unlockData();

  P.lock();

  // setup plots:
  if ( traces().size() != P.size() )
    P.resize( traces().size(), Plot::Pointer );
  P.setDataMutex( &RW->DataMutex );
  P.setCommonXRange();
  PlotElements.clear();
  PlotElements.resize( VP.size(), -1 );

  if ( traces().size() > 0 ) {

    for ( int c=0; c<traces().size(); c++ ) {
      P[c].clear();
      if ( trace(c).mode() & PlotTraceMode )
	P[c].setSkip( false );
      else
	P[c].setSkip( true );
    }
    
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
    
    if ( VP.size() >= 0 ) {
      P[VP.front()].setTMarg( 1.0 );
      P[VP.back()].setXTics();
      P[VP.back()].setXLabel( "msec" );
      P[VP.back()].setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, 
				 Plot::Left, 0.0 );
      P[VP.back()].setBMarg( 2.5 );
    }
    
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
  
  P.unlock();
  
  PlotChanged = true;
}


void PlotTrace::toggle( QAction *mtrace )
{
  // check for valid trace:
  bool nodata = true;
  unsigned int i=0;
  for ( i=0; i<PlotActions.size(); i++ ) {
    if ( PlotActions[i] == mtrace ) {
      nodata = false;
      break;
    }
  }
  if ( nodata )
    return;

  P.lockData();
  int m = trace(i).mode();
  if ( m & PlotTraceMode ) {
    for ( unsigned int k=0; (int)k<traces().size(); k++ ) {
      if ( k != i && ( trace(k).mode() & PlotTraceMode ) ) {
	m &= ~PlotTraceMode;
	PlotActions[i]->setChecked( false );
	break;
      }
    }
  }
  else {
    m |= PlotTraceMode;
    PlotActions[i]->setChecked( true );
  }
  const_cast<InData&>(trace(i)).setMode( m );
  P.unlockData();
  resize();
  plot();
}


void PlotTrace::init( void )
{
  P.lockData();
  P.lock();

  int origin = ViewMode == FixedView ? 3 : 2;
  double tfac = 1000.0;
  string tunit = "ms";

  for ( unsigned int c=0; c<VP.size(); c++ ) {

    int vp = VP[c];

    // clear plot:
    P[vp].clear();

    // y-label:
    string s = trace(vp).ident() + " [" + trace(vp).unit() + "]";
    P[vp].setYLabel( s );
	
    // plot stimulus events:
    for ( int s=0; s<events().size(); s++ ) {
      if ( (events(s).mode() & PlotTraceMode) &&
	   (events(s).mode() & StimulusEventMode) ) {
	P[vp].plot( events(s), origin, Offset, tfac,
		    0.0, Plot::Graph, 2,
		    Plot::StrokeUp, 1.0, Plot::GraphY,
		    Plot::White );
	break;
      }
    }
    // plot restart events:
    for ( int s=0; s<events().size(); s++ ) {
      if ( (events(s).mode() & PlotTraceMode) &&
	   (events(s).mode() & RestartEventMode) ) {
	P[vp].plot( events(s), origin, Offset, tfac,
		    1.0, Plot::Graph, 1,
		    Plot::TriangleNorth, 0.07, Plot::GraphY,
		    Plot::Orange, Plot::Orange );
	break;
      }
    }
    // plot recording events:
    for ( int s=0; s<events().size(); s++ ) {
      if ( (events(s).mode() & PlotTraceMode) &&
	   (events(s).mode() & RecordingEventMode) ) {
	P[vp].plot( events(s), origin, Offset, tfac,
		    0.0, Plot::Graph, 4,
		    Plot::StrokeUp, 1.0, Plot::GraphY,
		    Plot::Red );
	break;
      }
    }
    // plot events:
    int sn = 0;
    for ( int s=0; s<events().size(); s++ ) 
      if ( (events(s).mode() & PlotTraceMode) &&
	   !(events(s).mode() & StimulusEventMode) &&
	   !(events(s).mode() & RestartEventMode) &&
	   !(events(s).mode() & RecordingEventMode) ) {
	
	if ( RW->FD->eventInputTrace( s ) == int( vp ) ) {

	  if ( sn == 0 ) {
	    /*
	      P[plots].plot( events(s), origin, Offset, tfac,
	      0.05, Plot::Graph, 1,
	      Plot::StrokeUp, 20, Plot::Pixel,
	      Plot::Red );
	    */
	    P[vp].plot( events(s), trace(vp),
			origin, Offset, tfac,
			1, Plot::Circle, 6, Plot::Pixel,
			Plot::Gold, Plot::Gold );
	  }
	  else if ( sn == 1 )
	    P[vp].plot( events(s), origin, Offset, tfac,
			0.1, Plot::Graph, 1,
			Plot::Circle, 6, Plot::Pixel,
			Plot::Yellow, Plot::Yellow );
	  else if ( sn == 2 )
	    P[vp].plot( events(s), origin, Offset, tfac, 0.2,
			Plot::Graph, 1,
			Plot::Diamond, 6, Plot::Pixel,
			Plot::Blue, Plot::Blue );
	  
	  else
	    P[vp].plot( events(s), origin, Offset, tfac,
			0.3, Plot::Graph, 1,
			Plot::TriangleUp, 6, Plot::Pixel,
			Plot::Red, Plot::Red );
	  
	  sn++;
	}
      }
      // plot voltage trace:
      int inx = -1;
      if ( trace(vp).indices( TimeWindow ) > 80 )
	inx = P[vp].plot( trace(vp), origin, Offset, tfac,
			  Plot::Green, 2, Plot::Solid,
			  Plot::Circle, 0, Plot::Green, Plot::Green );
      else
	inx = P[vp].plot( trace(vp), origin, Offset, tfac,
			  Plot::Green, 2, Plot::Solid,
			  Plot::Circle, 4, Plot::Green, Plot::Green );
      PlotElements[c] = inx;
    }

  // set xlabel:
  P[VP.back()].setXLabel( tunit );

  P.unlock();
  P.unlockData();
	
}


void PlotTrace::plot( void )
{
  P.lock();
  bool plotting = Plotting;
  P.unlock();
  if ( !plotting ) 
    return;

  if ( PlotChanged ) {
    init();
    PlotChanged = false;
  }

  P.lockData();
  P.lock();

  // set left- and rightmargin:
  double tfac = 1000.0;
  double leftwin = 0.0;
  double rightwin = tfac * TimeWindow;
  double sigtime = trace(0).signalTime();
  if ( sigtime < 0.0 )
    sigtime = 0.0;
  if ( ViewMode == SignalView ) {
    // offset fixed at signalTime():
    leftwin = -tfac * TimeOffs;
    rightwin = leftwin + tfac * TimeWindow;
    LeftTime = leftwin/tfac + sigtime;
    Offset = sigtime;
  }
  else if ( ViewMode == EndView ) {
    // offset continuous at currentTime():
    rightwin = tfac * ( trace(0).currentTime() - sigtime );
    leftwin = rightwin - tfac * TimeWindow;
    LeftTime = leftwin/tfac + sigtime;
    Offset = sigtime;
  }
  else if ( ViewMode == WrapView ) {
    // offset wrapped at signalTime():
    LeftTime = ::floor( ( trace(0).currentTime() - sigtime ) / TimeWindow ) * TimeWindow;
    leftwin = tfac * LeftTime;
    rightwin += leftwin;
    Offset = sigtime;
  }
  else {
    // offset fixed at LeftTime:
    leftwin = tfac * ( LeftTime - Offset );
    rightwin = leftwin + tfac * TimeWindow;
  }
      
  // set xrange:
  for ( unsigned int c=0; c<VP.size(); c++ ) {

    // setting axis:
    P[VP[c]].setXRange( leftwin, rightwin );
    if ( ! P[VP[c]].zoomedYRange() )
      P[VP[c]].setYRange( trace(VP[c]).minValue(), trace(VP[c]).maxValue() );
    
    // pointstyle:
    if ( PlotElements[c] >= 0 ) {
      if ( trace(VP[c]).indices( TimeWindow ) > 80 )
	P[VP[c]][PlotElements[c]].setPoint( Plot::Circle, 0,
					    Plot::Green, Plot::Green );
      else
	P[VP[c]][PlotElements[c]].setPoint( Plot::Circle, 4,
					    Plot::Green, Plot::Green );
    }
  }

  // plot:
  P.draw();

  P.unlock();
  P.unlockData();
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
  Menu->addAction( "Move to signal", this, SLOT( moveToSignal() ), Qt::CTRL + Qt::Key_Home );
  Menu->addAction( "&Signal view", this, SLOT( viewSignal() ), Qt::Key_Home );
  Menu->addAction( "Move offset left", this, SLOT( moveSignalOffsLeft() ), Qt::SHIFT + Qt::Key_PageUp );
  Menu->addAction( "Move offset right", this, SLOT( moveSignalOffsRight() ), Qt::SHIFT + Qt::Key_PageDown );
  Menu->addAction( "&End view", this, SLOT( viewEnd() ), Qt::Key_End );
  Menu->addAction( "&Wrapped view", this, SLOT( viewWrapped() ), Qt::Key_Insert );
  Menu->addAction( "&Manual", this, SLOT( manualRange() ), Qt::CTRL + Qt::Key_M );
  Menu->addAction( "&Auto", this, SLOT( autoRange() ), Qt::CTRL + Qt::Key_A );
  Menu->addAction( "&Toggle Plot", this, SLOT( plotOnOff() ) );

  Menu->addSeparator();

  PlotActions.clear();
  for ( int k=0; k<traces().size(); k++ ) {
    string s = "&" + Str( k+1 );
    s += " ";
    s += trace(k).ident();
    PlotActions.push_back( Menu->addAction( s.c_str() ) );
    PlotActions.back()->setCheckable( true );
    PlotActions.back()->setChecked( true );
  }
  connect( Menu, SIGNAL( triggered( QAction* ) ),
	   this, SLOT( toggle( QAction* ) ) );
}


void PlotTrace::updateMenu( void )
{
  if ( Menu != 0 ) {

    // remove old traces:
    for ( unsigned int k=0; k<PlotActions.size(); k++ )
      Menu->removeAction( PlotActions[k] );
    PlotActions.clear();

    // add new traces:
    for ( int k=0; k<traces().size(); k++ ) {
      string s = "&" + Str( k+1 );
      s += " ";
      s += trace(k).ident();
      PlotActions.push_back( Menu->addAction( s.c_str() ) );
      PlotActions.back()->setCheckable( true );
      PlotActions.back()->setChecked( (trace(k).mode() & PlotTraceMode) > 0 );
    }
  }
}


void PlotTrace::setState( bool on, bool fixed, double length, double offs )
{
  P.lock();
  AutoOn = on;
  AutoFixed = fixed;
  AutoTime = length;
  AutoOffs = offs;
  bool man = Manual;
  P.unlock();

  if ( man )
    return;

  P.lock();

  // toggle plot:
  Plotting = on;
  postCustomEvent( 11 );  // animate on/off button

  // toggle fixed offset:
  setView( fixed ? SignalView : WrapView );

  // length of total time window:
  TimeWindow = length;

  // left offset to signal:
  TimeOffs = offs;

  // pointstyle:
  for ( unsigned int c=0; c<VP.size(); c++ ) {
    if ( PlotElements[c] >= 0 ) {
      if ( trace(VP[c]).indices( TimeWindow ) > 80 )
	P[VP[c]][PlotElements[c]].setPoint( Plot::Circle, 0,
					    Plot::Green, Plot::Green );
      else
	P[VP[c]][PlotElements[c]].setPoint( Plot::Circle, 4,
					    Plot::Green, Plot::Green );
    }
  }
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
    P.lockData();
    P.lock();
    for ( unsigned int c=0; c<VP.size(); c++ ) {
      if ( PlotElements[c] >= 0 ) {
	if ( trace(VP[c]).indices( TimeWindow ) > 80 )
	  P[VP[c]][PlotElements[c]].setPoint( Plot::Circle, 0,
					      Plot::Green, Plot::Green );
      }
    }
    P.unlock();
    P.unlockData();
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
    P.lockData();
    P.lock();
    for ( unsigned int c=0; c<VP.size(); c++ ) {
      if ( PlotElements[c] >= 0 ) {
	if ( trace(VP[c]).indices( TimeWindow ) <= 80 )
	  P[VP[c]][PlotElements[c]].setPoint( Plot::Circle, 4,
					      Plot::Green, Plot::Green );
      }
    }
    P.unlock();
    P.unlockData();
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
  P.lockData();
  P.lock();
  if ( ViewMode != FixedView )
    setView( FixedView );
  LeftTime = trace(0).currentTime() - TimeWindow;
  P.unlock();
  P.unlockData();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveToSignal( void )
{
  P.lockData();
  P.lock();
  if ( ViewMode == SignalView )
    TimeOffs = 0.0;
  else {
    if ( ViewMode != FixedView )
      setView( FixedView );
    double sigtime = trace(0).signalTime();
    if ( sigtime < 0.0 )
      sigtime = 0.0;
    LeftTime = sigtime;
  }
  P.unlock();
  P.unlockData();
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
  if ( ViewMode != SignalView )
    setView( SignalView );
  else
    TimeOffs += 0.5 * TimeWindow;
  P.unlock();
  if ( RW->idle() )
    plot();
}


void PlotTrace::moveSignalOffsRight( void )
{
  P.lock();
  if ( ViewMode != SignalView )
    setView( SignalView );
  else
    TimeOffs -= 0.5 * TimeWindow;
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
  setState( AutoOn, AutoFixed, AutoTime, AutoOffs );
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
    ViewMode = mode;
    PlotChanged = true;
    postCustomEvent( 12 );
  }
}


void PlotTrace::keyPressEvent( QKeyEvent *event )
{
  event->accept();
  switch ( event->key() ) {

  case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4: case Qt::Key_5: 
  case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9: {
    int n = event->key() - Qt::Key_1;
    if ( n >=0 && n < (int)PlotActions.size() )
      toggle( PlotActions[n] );
    break;
  }

  case Qt::Key_Equal:
    zoomIn();
    break;

  default:
    event->ignore();

  }
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



}; /* namespace relacs */

#include "moc_plottrace.cc"

