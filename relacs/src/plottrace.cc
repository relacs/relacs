/*
  plottrace.cc
  Plot trace and spikes.

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
  : MultiPlot( 1, Plot::Pointer, parent ),
    IL( 0 ),
    EL( 0 ),
    PlotElements( 1, -1 ),
    RW( rw ),
    Menu( 0 )
{
  setDataMutex( &RW->DataMutex );

  OffsetMode = 0;
  setOffset( 1 );
  Manual = false;
  Plotting = true;

  connect( this, SIGNAL( changedRanges( int ) ),
	   this, SLOT( updateRanges( int ) ) );

  TimeWindow = 0.1;
  TimeOffs = 0.0;
  Offset = 0.0;

  AutoOn = true;
  AutoFixed = false;
  AutoTime = 0.1;
  AutoOffs = 0.0;

  ButtonBox = 0;
  OffsetButton = 0;
  ManualButton = 0;
  OnOffButton = 0;

  ButtonBox = new QWidget( this );
  ButtonBoxLayout = new QHBoxLayout;
  ButtonBoxLayout->setContentsMargins( 0, 0, 0, 0 );
  ButtonBoxLayout->setSpacing( 0 );
  ButtonBox->setLayout( ButtonBoxLayout );

  int s = fontInfo().pixelSize();

  FixedOffsetIcon = QPixmap( s, s );
  QPainter p;
  p.begin( &FixedOffsetIcon );
  p.eraseRect( FixedOffsetIcon.rect() );
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
  FixedOffsetIcon.setMask( FixedOffsetIcon.createHeuristicMask() );

  ContinuousOffsetIcon = QPixmap( s, s );
  p.begin( &ContinuousOffsetIcon );
  p.eraseRect( ContinuousOffsetIcon.rect() );
  p.setPen( QPen() );
  p.setBrush( Qt::black );
  pa.setPoint( 0, 3, 2 );
  pa.setPoint( 1, 3, s-2 );
  pa.setPoint( 2, s-4, s/2 );
  p.drawPolygon( pa );
  p.setPen( QPen( Qt::black, 2 ) );
  p.drawLine( s-2, 2, s-2, s-1 );
  p.end();
  ContinuousOffsetIcon.setMask( ContinuousOffsetIcon.createHeuristicMask() );

  OffsetButton = new QPushButton;
  ButtonBoxLayout->addWidget( OffsetButton );
  OffsetButton->setIcon( FixedOffsetIcon );
  OffsetButton->setToolTip( "F: fixed (Pos1), C: continous (End)" );
  connect( OffsetButton, SIGNAL( clicked() ), this, SLOT( offsetToggle() ) );

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

  ManualButton = new QPushButton;
  ButtonBoxLayout->addWidget( ManualButton );
  ManualButton->setCheckable( true );
  ManualButton->setIcon( manualicon );
  ManualButton->setDown( Manual );
  ManualButton->setToolTip( "Manual or Auto" );
  connect( ManualButton, SIGNAL( clicked() ), this, SLOT( toggleManual() ) );

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


void PlotTrace::resize( InList &data, const EventList &events )
{
  IL = &data;
  EL = &events;

  // count plots:
  lockData();
  int plots = 0;
  for ( int c=0; c<data.size(); c++ )
    if ( data[c].mode() & PlotTraceMode )
      plots++;
  unlockData();

  lock();

  // setup plots:
  PlotActions.clear();
  MultiPlot::resize( plots, Plot::Pointer );
  setDataMutex( &RW->DataMutex );
  setCommonXRange();
  PlotElements.resize( plots, -1 );

  if ( plots > 0 ) {

    double lmarg = 11.0;
    if ( plots == 1 )
      lmarg = 8.0;
    int o = 0;
    if ( plots > 1 && plots % 2 == 1 )
      o = 1;
    
    for ( int c=0; c<plots; c++ ) {
      (*this)[c].clear();
      (*this)[c].setLMarg( lmarg );
      (*this)[c].setRMarg( 2.0 );
      (*this)[c].setTMarg( 0.2 );
      (*this)[c].setBMarg( 0.2 );
      (*this)[c].noXTics();
      (*this)[c].setXLabel( "" );
      (*this)[c].setYTics();
      (*this)[c].setYLabelPos( 2.0 + ((c+o)%2)*3.0, Plot::FirstMargin,
			       0.5, Plot::Graph, 
			       Plot::Center, -90.0 );
    }
    
    (*this)[0].setTMarg( 1.0 );
    (*this)[plots-1].setXTics();
    (*this)[plots-1].setXLabel( "msec" );
    (*this)[plots-1].setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, 
				   Plot::Left, 0.0 );
    (*this)[plots-1].setBMarg( 2.5 );

    if ( plots > 6 ) {
      (*this)[(plots+1)/2].setTMarg( 1.0 );
      (*this)[(plots-1)/2].setXTics();
      (*this)[(plots-1)/2].setXLabel( "msec" );
      (*this)[(plots-1)/2].setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, 
				     Plot::Left, 0.0 );
      (*this)[(plots-1)/2].setBMarg( 2.5 );
    }
    
    resizeLayout();
  }

  unlock();

  PlotChanged = true;
}


void PlotTrace::toggle( QAction *trace )
{
  // check for valid trace:
  bool nodata = true;
  unsigned i=0;
  for ( i=0; i<PlotActions.size(); i++ ) {
    if ( PlotActions[i] == trace ) {
      nodata = false;
      break;
    }
  }
  if ( nodata )
    return;

  lockData();
  int m = (*IL)[i].mode();
  if ( m & PlotTraceMode ) {
    for ( unsigned int k=0; (int)k<IL->size(); k++ ) {
      if ( k != i && ( (*IL)[k].mode() & PlotTraceMode ) ) {
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
  (*IL)[i].setMode( m );
  unlockData();
  resize( *IL, *EL );
  plot( *IL, *EL );
}


void PlotTrace::init( const InList &data, const EventList &events )
{
  lockData();
  lock();

  int origin = OffsetMode < 0 ? 3 : 2;
  double tfac = 1000.0;
  string tunit = "ms";

  int plots = 0;
  for ( int c=0; c<data.size(); c++ )
    if ( data[c].mode() & PlotTraceMode ) {

      // clear plot:
      (*this)[plots].clear();

      // y-label:
      string s = data[c].ident() + " [" + data[c].unit() + "]";
      (*this)[plots].setYLabel( s );
	
      // plot stimulus events:
      for ( int s=0; s<events.size(); s++ ) {
	if ( (events[s].mode() & PlotTraceMode) &&
	     (events[s].mode() & StimulusEventMode) ) {
	  (*this)[plots].plot( events[s], origin, Offset, tfac,
			       0.0, Plot::Graph, 2,
			       Plot::StrokeUp, 1.0, Plot::GraphY,
			       Plot::White );
	  break;
	}
      }
      // plot restart events:
      for ( int s=0; s<events.size(); s++ ) {
	if ( (events[s].mode() & PlotTraceMode) &&
	     (events[s].mode() & RestartEventMode) ) {
	  (*this)[plots].plot( events[s], origin, Offset, tfac,
			       1.0, Plot::Graph, 1,
			       Plot::TriangleNorth, 0.07, Plot::GraphY,
			       Plot::Orange, Plot::Orange );
	  break;
	}
      }
      // plot recording events:
      for ( int s=0; s<events.size(); s++ ) {
	if ( (events[s].mode() & PlotTraceMode) &&
	     (events[s].mode() & RecordingEventMode) ) {
	  (*this)[plots].plot( events[s], origin, Offset, tfac,
			       0.0, Plot::Graph, 4,
			       Plot::StrokeUp, 1.0, Plot::GraphY,
			       Plot::Red );
	  break;
	}
      }

      // plot events:
      int sn = 0;
      for ( int s=0; s<events.size(); s++ ) 
	if ( (events[s].mode() & PlotTraceMode) &&
	     !(events[s].mode() & StimulusEventMode) &&
	     !(events[s].mode() & RestartEventMode) &&
	     !(events[s].mode() & RecordingEventMode) ) {
	  
	  if ( RW->FD->eventInputTrace( s ) == int( c ) ) {

	    if ( sn == 0 ) {
	      /*
	      (*this)[plots].plot( events[s], origin, Offset, tfac,
				   0.05, Plot::Graph, 1,
				   Plot::StrokeUp, 20, Plot::Pixel,
				   Plot::Red );
	      */
	      (*this)[plots].plot( events[s], data[c],
				   origin, Offset, tfac,
				   1, Plot::Circle, 6, Plot::Pixel,
				   Plot::Gold, Plot::Gold );
	    }
	    else if ( sn == 1 )
	      (*this)[plots].plot( events[s], origin, Offset, tfac,
				   0.1, Plot::Graph, 1,
				   Plot::Circle, 6, Plot::Pixel,
				   Plot::Yellow, Plot::Yellow );
	    else if ( sn == 2 )
	      (*this)[plots].plot( events[s], origin, Offset, tfac, 0.2,
				   Plot::Graph, 1,
				   Plot::Diamond, 6, Plot::Pixel,
				   Plot::Blue, Plot::Blue );
	    
	    else
	      (*this)[plots].plot( events[s], origin, Offset, tfac,
				   0.3, Plot::Graph, 1,
				   Plot::TriangleUp, 6, Plot::Pixel,
				   Plot::Red, Plot::Red );
	    
	    sn++;
	  }
	}

      // plot voltage trace:
      int inx = -1;
      if ( data[c].indices( TimeWindow ) > 80 )
	inx = (*this)[plots].plot( data[c], origin, Offset, tfac,
				   Plot::Green, 2, Plot::Solid,
				   Plot::Circle, 0, Plot::Green, Plot::Green );
      else
	inx = (*this)[plots].plot( data[c], origin, Offset, tfac,
				   Plot::Green, 2, Plot::Solid,
				   Plot::Circle, 4, Plot::Green, Plot::Green );
      PlotElements[plots] = inx;

      plots++;
    }

  // set xlabel:
  back().setXLabel( tunit );

  unlock();
  unlockData();
	
}


void PlotTrace::plot( const InList &data, const EventList &events )
{
  if ( !Plotting ) 
    return;

  if ( PlotChanged ) {
    init( data, events );
    PlotChanged = false;
  }

  lockData();
  lock();

  // set left- and rightmargin:
  double tfac = 1000.0;
  double leftwin = 0.0;
  double rightwin = tfac * TimeWindow;
  double sigtime = data[0].signalTime();
  if ( sigtime < 0.0 )
    sigtime = 0.0;
  if ( OffsetMode == 0 ) {
    // offset fixed at signalTime():
    leftwin = -tfac * TimeOffs;
    rightwin = leftwin + tfac * TimeWindow;
    LeftTime = 0.001 * leftwin + sigtime;
    Offset = sigtime;
  }
  else if ( OffsetMode > 0 ) {
    // offset continuous at currentTime():
    rightwin = tfac * ( data[0].currentTime() - sigtime );
    leftwin = rightwin - tfac * TimeWindow;
    LeftTime = 0.001 * leftwin + sigtime;
    Offset = sigtime;
  }
  else {
    // offset fixed at LeftTime:
    leftwin = tfac * ( LeftTime - Offset );
    rightwin = leftwin + tfac * TimeWindow;
  }
      
  // set xrange:
  int plots = 0;
  for ( int c=0; c<data.size(); c++ )
    if ( ( data[c].mode() & PlotTraceMode ) > 0 ) {

      // setting axis:
      (*this)[plots].setXRange( leftwin, rightwin );
      if ( ! (*this)[plots].zoomedYRange() )
	(*this)[plots].setYRange( data[c].minValue(), data[c].maxValue() );

      // pointstyle:
      if ( PlotElements[plots] >= 0 ) {
	if ( data[c].indices( TimeWindow ) > 80 )
	  (*this)[plots][PlotElements[plots]].setPoint( Plot::Circle, 0,
							Plot::Green, Plot::Green );
	else
	  (*this)[plots][PlotElements[plots]].setPoint( Plot::Circle, 4,
							Plot::Green, Plot::Green );
      }
      
      plots++;
      if ( plots >= size() )
	break;
      
    }

  unlock();
  unlockData();

  // plot:
  draw();

}


void PlotTrace::updateRanges( int id )
{
  lock();
  double tfac = 0.001;
  TimeWindow = tfac * ( (*this)[id].xmaxRange() - (*this)[id].xminRange() );
  TimeOffs = -tfac * (*this)[id].xminRange();
  LeftTime = Offset - TimeOffs;
  unlock();
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
  Menu->addAction( "&Signal", this, SLOT( moveSignal() ), Qt::CTRL + Qt::Key_Home );
  Menu->addAction( "&Fixed", this, SLOT( fixedSignal() ), Qt::CTRL + Qt::Key_F );
  Menu->addAction( "Move offset left", this, SLOT( moveOffsLeft() ), Qt::SHIFT + Qt::Key_PageUp );
  Menu->addAction( "Move offset right", this, SLOT( moveOffsRight() ), Qt::SHIFT + Qt::Key_PageDown );
  Menu->addAction( "&Continuous", this, SLOT( continuousEnd() ), Qt::CTRL + Qt::Key_C );
  Menu->addAction( "&Manual", this, SLOT( manualRange() ), Qt::CTRL + Qt::Key_M );
  Menu->addAction( "&Auto", this, SLOT( autoRange() ), Qt::CTRL + Qt::Key_A );
  Menu->addAction( "&Toggle Plot", this, SLOT( plotOnOff() ) );

  Menu->addSeparator();

  PlotActions.clear();
  if ( IL != 0 ) {
    for ( int k=0; k<IL->size(); k++ ) {
      string s = "&" + Str( k+1 );
      s += " ";
      s += (*IL)[k].ident();
      PlotActions.push_back( Menu->addAction( s.c_str() ) );
      PlotActions.back()->setChecked( true );
    }
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
    for ( int k=0; k<IL->size(); k++ ) {
      string s = "&" + Str( k+1 );
      s += " ";
      s += (*IL)[k].ident();
      PlotActions.push_back( Menu->addAction( s.c_str() ) );
      PlotActions.back()->setChecked( true );
    }
  }
}


void PlotTrace::setState( bool on, bool fixed, double length, double offs )
{
  lock();
  AutoOn = on;
  AutoFixed = fixed;
  AutoTime = length;
  AutoOffs = offs;
  bool man = Manual;
  unlock();

  if ( man )
    return;

  lockData();
  lock();

  // toggle plot:
  Plotting = on;
  QApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+1 ) ) );

  // toggle fixed offset:
  setOffset( fixed ? 0 : 1 );

  // length of total time window:
  TimeWindow = length;

  // left offset tot signal:
  TimeOffs = offs;

  // pointstyle:
  int plots=0;
  for ( int c=0; c<IL->size(); c++ ) {
    if ( PlotElements[plots] >= 0 ) {
      if ( (*IL)[c].indices( TimeWindow ) > 80 )
	(*this)[plots][PlotElements[plots]].setPoint( Plot::Circle, 0,
						      Plot::Green, Plot::Green );
      else
	(*this)[plots][PlotElements[plots]].setPoint( Plot::Circle, 4,
						      Plot::Green, Plot::Green );
      plots++;
      if ( plots >= size() )
	break;
    }
  }
  unlock();
  unlockData();
}


void PlotTrace::zoomOut( void )
{
  lock();
  TimeWindow *= 2;
  TimeOffs *= 2;
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
  else {
    lockData();
    lock();
    int plots=0;
    for ( int c=0; c<IL->size(); c++ ) {
      if ( PlotElements[plots] >= 0 ) {
	if ( (*IL)[c].indices( TimeWindow ) > 80 )
	  (*this)[plots][PlotElements[plots]].setPoint( Plot::Circle, 0,
							Plot::Green, Plot::Green );
	plots++;
	if ( plots >= size() )
	  break;
      }
    }
    unlock();
    unlockData();
  }
}


void PlotTrace::zoomIn( void )
{
  lock();
  TimeWindow /= 2.0;
  TimeOffs /= 2.0;
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
  else {
    lockData();
    lock();
    int plots=0;
    for ( int c=0; c<IL->size(); c++ ) {
      if ( PlotElements[plots] >= 0 ) {
	if ( (*IL)[c].indices( TimeWindow ) <= 80 )
	  (*this)[plots][PlotElements[plots]].setPoint( Plot::Circle, 4,
							Plot::Green, Plot::Green );
	plots++;
	if ( plots >= size() )
	  break;
      }
    }
    unlock();
    unlockData();
  }
}


void PlotTrace::moveLeft( void )
{
  lock();
  if ( OffsetMode >= 0 )
    setOffset( -1 );
  else
    LeftTime -= 0.5 * TimeWindow;
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::moveRight( void )
{
  lock();
  if ( OffsetMode >= 0 )
    setOffset( -1 );
  else
    LeftTime += 0.5 * TimeWindow;
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::moveStart( void )
{
  lock();
  if ( OffsetMode >= 0 )
    setOffset( -1 );
  LeftTime = 0.0;
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::moveEnd( void )
{
  lockData();
  lock();
  if ( OffsetMode >= 0 )
    setOffset( -1 );
  LeftTime = IL == 0 ? 0.0 : (*IL)[0].currentTime() - TimeWindow;
  unlock();
  unlockData();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::moveSignal( void )
{
  lockData();
  lock();
  if ( OffsetMode == 0 )
    TimeOffs = 0.0;
  else {
    if ( OffsetMode >= 0 )
      setOffset( -1 );
    double sigtime = IL == 0 ? 0.0 : (*IL)[0].signalTime();
    if ( sigtime < 0.0 )
      sigtime = 0.0;
    LeftTime = sigtime;
  }
  unlock();
  unlockData();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::fixedSignal( void )
{
  lock();
  setOffset( 0 );
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::moveOffsLeft( void )
{
  lock();
  if ( OffsetMode != 0 )
    setOffset( 0 );
  else
    TimeOffs += 0.5 * TimeWindow;
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::moveOffsRight( void )
{
  lock();
  if ( OffsetMode != 0 )
    setOffset( 0 );
  else
    TimeOffs -= 0.5 * TimeWindow;
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::continuousEnd( void )
{
  lock();
  setOffset( 1 );
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::plotOnOff( )
{
  lock();
  Plotting = ! Plotting;
  int p = Plotting;
  unlock();
  if ( OnOffButton != 0 )
    OnOffButton->setDown( ! p );
}


void PlotTrace::toggleManual( void )
{
  lock();
  bool man = Manual;
  unlock();
  if ( man ) 
    autoRange();
  else
    manualRange();
}


void PlotTrace::manualRange( void )
{
  lock();
  Manual = true;
  if ( ManualButton != 0 )
    ManualButton->setDown( Manual );
  unlock();
}


void PlotTrace::autoRange( void )
{
  lock();
  Manual = false;
  if ( ManualButton != 0 )
    ManualButton->setDown( Manual );
  setState( AutoOn, AutoFixed, AutoTime, AutoOffs );
  unlock();
}


void PlotTrace::offsetToggle( void )
{
  lock();
  int mode = OffsetMode + 1;
  if ( mode > 1 )
    mode = 0;
  setOffset( mode );
  unlock();
}


void PlotTrace::setOffset( int mode )
{
  if ( mode > 1 )
    mode = -1;
  if ( mode < -1 )
    mode = 1;

  lock();
  if ( OffsetMode != mode ) {
    OffsetMode = mode;
    PlotChanged = true;
    QApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
  }
  unlock();
}


void PlotTrace::keyPressEvent( QKeyEvent* e )
{
  switch ( e->key() ) {

  case Qt::Key_1: case Qt::Key_2: case Qt::Key_3: case Qt::Key_4: case Qt::Key_5: 
  case Qt::Key_6: case Qt::Key_7: case Qt::Key_8: case Qt::Key_9: {
    int n = e->key() - Qt::Key_1;
    if ( n >=0 && n < (int)PlotActions.size() )
      toggle( PlotActions[n] );
    break;
  }

  case Qt::Key_Minus:
    zoomOut();
    break;
  case Qt::Key_Plus:
  case Qt::Key_Equal:
    zoomIn();
    break;

  case Qt::Key_End:
    if ( e->modifiers() & Qt::ControlModifier )
      moveEnd();
    else
      continuousEnd();
    break;
  case Qt::Key_Home:
    if ( e->modifiers() & Qt::ControlModifier )
      moveSignal();
    else 
      fixedSignal();
    break;

  case Qt::Key_PageUp:
    if ( e->modifiers() & Qt::ControlModifier ) 
      moveStart();
    else if ( e->modifiers() & Qt::ShiftModifier ) 
      moveOffsLeft();
    else 
      moveLeft();
    break;
  case Qt::Key_PageDown:
    if ( e->modifiers() & Qt::ControlModifier )
      moveEnd();
    else if ( e->modifiers() & Qt::ShiftModifier ) 
      moveOffsRight();
    else
      moveRight();
    break;

  default:
    e->ignore();

  }
}


void PlotTrace::resizeLayout( void )
{
  int plots = MultiPlot::size();

  if ( plots == 1 ) {
    (*this)[0].setOrigin( 0.0, 0.0 );
    (*this)[0].setSize( 1.0, 1.0 );
    return;
  }

  int columns = 1;
  if ( plots > 6 )
    columns = 2;
  int rows = plots/columns;
  if ( plots%columns > 0 )
    rows++;
  double xsize = 1.0/columns;
  double yboffs = double( (*this)[0].fontPixel( 2.3 ) ) / double( height() );
  double ytoffs = double( (*this)[0].fontPixel( 0.8 ) ) / double( height() );
  double yheight = (1.0-yboffs-ytoffs)/rows;
    
  int c = 0;
  int r = 0;
  for ( int k=0; k<plots; k++ ) {
    (*this)[k].setOrigin( c*xsize, yboffs+(rows-r-1)*yheight );
    (*this)[k].setSize( xsize, yheight );
    r++;
    if ( r >= rows ) {
      c++;
      r = 0;
    }
  }
  (*this)[0].setSize( xsize, yheight+ytoffs );
  (*this)[plots-1].setOrigin( (columns-1)*xsize, 0.0 );
  (*this)[plots-1].setSize( xsize, yheight+yboffs );
  if ( columns > 1 ) {
    (*this)[(plots+1)/2].setSize( xsize, yheight+ytoffs );
    (*this)[(plots-1)/2].setOrigin( 0.0, 0.0 );
    (*this)[(plots-1)/2].setSize( xsize, yheight+yboffs );
  }
}


void PlotTrace::resizeEvent( QResizeEvent *qre )
{
  lock();
  resizeLayout();
  unlock();

  MultiPlot::resizeEvent( qre );

  if ( ButtonBox == 0 )
    return;

  QSize bs = ButtonBox->sizeHint();
  ButtonBox->setGeometry( width() - bs.width(), 0,
			  bs.width(), bs.height() );
}


void PlotTrace::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 1 : {
    if ( OnOffButton != 0 ) {
      lock();
      bool plotting = Plotting;
      unlock();
      if ( plotting )
	OnOffButton->setDown( false );
      else
	OnOffButton->setDown( true );
      break;
    }
  }
  case 2: {
    if ( OffsetButton != 0 ) {
      if ( OffsetMode == 0 )
	OffsetButton->setIcon( FixedOffsetIcon );
      else
	OffsetButton->setIcon( ContinuousOffsetIcon );
    }
    break;
  }
  }
}



}; /* namespace relacs */

#include "moc_plottrace.cc"

