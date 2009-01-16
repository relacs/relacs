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
#include <qpixmap.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qdatetime.h>
#include <qlabel.h>
#include <qkeycode.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <relacs/str.h>
#include <relacs/relacswidget.h>
#include <relacs/plottrace.h>

namespace relacs {


PlotTrace::PlotTrace( RELACSWidget *rw, QWidget* parent, const char* name )
  : MultiPlot( 1, Plot::Pointer, parent, "PlotTrace::Plot" ),
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

  ButtonBox = new QHBox( this );
  ButtonBox->setBackgroundMode( Qt::NoBackground );

  int s = fontInfo().pixelSize();
  FixedOffsetIcon.resize( s, s );
  QPainter p;
  p.begin( &FixedOffsetIcon, this );
  p.eraseRect( FixedOffsetIcon.rect() );
  p.setPen( QPen() );
  p.setBrush( black );
  QPointArray pa( 3 );
  pa.setPoint( 0, s-3, 2 );
  pa.setPoint( 1, s-3, s-2 );
  pa.setPoint( 2, 4, s/2 );
  p.drawPolygon( pa );
  p.setPen( QPen( black, 2 ) );
  p.drawLine( 3, 2, 3, s-1 );
  p.end();
  FixedOffsetIcon.setMask( FixedOffsetIcon.createHeuristicMask() );

  ContinuousOffsetIcon.resize( s, s );
  p.begin( &ContinuousOffsetIcon, this );
  p.eraseRect( ContinuousOffsetIcon.rect() );
  p.setPen( QPen() );
  p.setBrush( black );
  pa.setPoint( 0, 3, 2 );
  pa.setPoint( 1, 3, s-2 );
  pa.setPoint( 2, s-4, s/2 );
  p.drawPolygon( pa );
  p.setPen( QPen( black, 2 ) );
  p.drawLine( s-2, 2, s-2, s-1 );
  p.end();
  ContinuousOffsetIcon.setMask( ContinuousOffsetIcon.createHeuristicMask() );

  OffsetButton = new QPushButton ( ButtonBox );
  OffsetButton->setPixmap( FixedOffsetIcon );
  QToolTip::add( OffsetButton, "F: fixed (Pos1), C: continous (End)" );
  connect( OffsetButton, SIGNAL( clicked() ), this, SLOT( offsetToggle() ) );

  QBitmap manualmask( s, s, true );
  p.begin( &manualmask, this );
  p.setPen( QPen( color1 ) );
  p.setBrush( QBrush() );
  p.setFont( QFont( "Helvetica", s, QFont::Bold ) );
  p.drawText( manualmask.rect(), Qt::AlignCenter, "M" );
  p.end();
  QPixmap manualicon( s, s );
  p.begin( &manualicon, this );
  p.eraseRect( manualicon.rect() );
  p.setPen( QPen( black ) );
  p.setBrush( QBrush() );
  p.setFont( QFont( "Helvetica", s, QFont::Bold ) );
  p.drawText( manualicon.rect(), Qt::AlignCenter, "M" );
  p.end();
  //  manualicon.setMask( manualicon.createHeuristicMask() );
  manualicon.setMask( manualmask );

  ManualButton = new QPushButton( ButtonBox );
  ManualButton->setToggleButton( true );
  ManualButton->setPixmap( manualicon );
  ManualButton->setOn( Manual );
  QToolTip::add( ManualButton, "Manual or Auto" );
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

  OnOffButton = new QPushButton( ButtonBox );
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
  int columns = 1;
  if ( plots > 6 )
    columns = 2;
  MultiPlot::resize( plots, 1, true, Plot::Pointer );
  setCommonXRange();
  PlotElements.resize( plots, -1 );

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

  if ( plots > 0 ) {
    (*this)[0].setTMarg( 1.0 );
    (*this)[plots-1].setXTics();
    (*this)[plots-1].setXLabel( "msec" );
    (*this)[plots-1].setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, 
				   Plot::Left, 0.0 );
    (*this)[plots-1].setBMarg( 3.0 );
  }

  unlock();

  PlotChanged = true;
}


void PlotTrace::toggle( int i )
{
  lockData();
  bool nodata = ( IL == 0 || i < 0 || i >= IL->size() );
  unlockData();
  if ( nodata )
    return;

  lockData();
  int m = (*IL)[i].mode();
  if ( m & PlotTraceMode ) {
    for ( int k=0; k<IL->size(); k++ ) {
      if ( k != i && ( (*IL)[k].mode() & PlotTraceMode ) ) {
	m &= ~PlotTraceMode;
	Menu->setItemChecked( i, false );
	break;
      }
    }
  }
  else {
    m |= PlotTraceMode;
    Menu->setItemChecked( i, true );
  }
  (*IL)[i].setMode( m );
  unlockData();
  resize( *IL, *EL );
  plot( *IL, *EL );
}


void PlotTrace::init( const InList &data, const EventList &events )
{
  lock();
  lockData();

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
	     (events[s].mode() & StimulusEventMode) )
	  (*this)[plots].plot( events[s], origin, Offset, tfac,
			       0.0, Plot::Graph, 2,
			       Plot::StrokeUp, 1.0, Plot::GraphY,
			       Plot::White );
      }
      // plot restart events:
      for ( int s=0; s<events.size(); s++ ) {
	if ( (events[s].mode() & PlotTraceMode) &&
	     (events[s].mode() & RestartEventMode) ) {
	  (*this)[plots].plot( events[s], origin, Offset, tfac,
			       1.0, Plot::Graph, 1,
			       Plot::TriangleNorth, 0.07, Plot::GraphY,
			       Plot::Orange, Plot::Orange );
	}
      }

      // plot events:
      int sn = 0;
      for ( int s=0; s<events.size(); s++ ) 
	if ( (events[s].mode() & PlotTraceMode) &&
	     !(events[s].mode() & StimulusEventMode) &&
	     !(events[s].mode() & RestartEventMode) ) {
	  
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

  unlockData();
  unlock();
	
}


void PlotTrace::plot( const InList &data, const EventList &events )
{
  if ( !Plotting ) 
    return;

  if ( PlotChanged ) {
    init( data, events );
    PlotChanged = false;
  }

  lock();
  lockData();

  // set left- and rightmargin:
  double tfac = 1000.0;
  double leftwin = 0.0;
  double rightwin = tfac * TimeWindow;
  if ( OffsetMode == 0 ) {
    // offset fixed at signalTime():
    leftwin = -tfac * TimeOffs;
    rightwin = leftwin + tfac * TimeWindow;
    LeftTime = 0.001 * leftwin + data[0].signalTime();
    Offset = data[0].signalTime();
  }
  else if ( OffsetMode > 0 ) {
    // offset continuous at currentTime():
    rightwin = tfac * ( data[0].currentTime() - data[0].signalTime() );
    leftwin = rightwin - tfac * TimeWindow;
    LeftTime = 0.001 * leftwin + data[0].signalTime();
    Offset = data[0].signalTime();
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

  unlockData();
  unlock();

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


void PlotTrace::addMenu( QPopupMenu *menu )
{
  Menu = menu;

  Menu->insertItem( "Zoom &in", this, SLOT( zoomIn( void ) ), Key_Plus );
  Menu->insertItem( "Zoom &out", this, SLOT( zoomOut( void ) ), Key_Minus );
  Menu->insertItem( "Move &left", this, SLOT( moveLeft( void ) ), Key_PageUp );
  Menu->insertItem( "Move &right", this, SLOT( moveRight( void ) ), Key_PageDown );
  Menu->insertItem( "&Begin", this, SLOT( moveStart( void ) ), CTRL+Key_PageUp );
  Menu->insertItem( "&End", this, SLOT( moveEnd( void ) ), CTRL+Key_PageDown );
  Menu->insertItem( "&Signal", this, SLOT( moveSignal( void ) ), CTRL+Key_Home );
  Menu->insertItem( "&Fixed", this, SLOT( fixedSignal( void ) ), CTRL+Key_F );
  Menu->insertItem( "Move offset left", this, SLOT( moveOffsLeft( void ) ), SHIFT+Key_PageUp );
  Menu->insertItem( "Move offset right", this, SLOT( moveOffsRight( void ) ), SHIFT+Key_PageDown );
  Menu->insertItem( "&Continuous", this, SLOT( continuousEnd( void ) ), CTRL+Key_C );
  Menu->insertItem( "&Manual", this, SLOT( manualRange( void ) ), CTRL + Key_M );
  Menu->insertItem( "&Auto", this, SLOT( autoRange( void ) ), CTRL + Key_A );
  Menu->insertItem( "&Toggle Plot", this, SLOT( plotOnOff( void ) ) );

  Menu->insertSeparator();
  if ( IL != 0 ) {
    for ( int k=0; k<IL->size(); k++ ) {
      string s = "&" + Str( k+1 );
      s += " ";
      s += (*IL)[k].ident();
      Menu->insertItem( s.c_str(), k );
      Menu->setItemChecked( k, true );
    }
  }
  connect( Menu, SIGNAL( activated( int ) ),
	   this, SLOT( toggle( int ) ) );
}


void PlotTrace::updateMenu( void )
{
  if ( Menu != 0 ) {

    // remove old traces:
    for ( int k=0; k<Menu->idAt( Menu->count()-1 ); k++ ) {
      Menu->removeItem( k );
    }

    // add new traces:
    for ( int k=0; k<IL->size(); k++ ) {
      string s = "&" + Str( k+1 );
      s += " ";
      s += (*IL)[k].ident();
      Menu->insertItem( s.c_str(), k );
      Menu->setItemChecked( k, true );
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

  lock();

  // toggle plot:
  Plotting = on;
  QApplication::postEvent( this, new QCustomEvent( QEvent::User+1 ) );

  // toggle fixed offset:
  setOffset( fixed ? 0 : 1 );

  // length of total time window:
  TimeWindow = length;

  // left offset tot signal:
  TimeOffs = offs;

  // pointstyle:
  lockData();
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
  unlockData();
  unlock();
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
    lock();
    lockData();
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
    unlockData();
    unlock();
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
    lock();
    lockData();
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
    unlockData();
    unlock();
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
  lock();
  if ( OffsetMode >= 0 )
    setOffset( -1 );
  lockData();
  LeftTime = IL == 0 ? 0.0 : (*IL)[0].currentTime() - TimeWindow;
  unlockData();
  unlock();
  if ( RW->idle() )
    plot( *IL, *EL );
}


void PlotTrace::moveSignal( void )
{
  lock();
  if ( OffsetMode == 0 )
    TimeOffs = 0.0;
  else {
    if ( OffsetMode >= 0 )
      setOffset( -1 );
    lockData();
    LeftTime = IL == 0 ? 0.0 : (*IL)[0].signalTime();
    unlockData();
  }
  unlock();
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
    OnOffButton->setOn( ! p );
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
    ManualButton->setOn( Manual );
  unlock();
}


void PlotTrace::autoRange( void )
{
  lock();
  Manual = false;
  if ( ManualButton != 0 )
    ManualButton->setOn( Manual );
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
    QApplication::postEvent( this, new QCustomEvent( QEvent::User+2 ) );
  }
  unlock();
}


void PlotTrace::keyPressEvent( QKeyEvent* e )
{
  switch ( e->key() ) {

  case Key_1: case Key_2: case Key_3: case Key_4: case Key_5: 
  case Key_6: case Key_7: case Key_8: case Key_9: {
    toggle( e->key() - Key_1 );
    break;
  }

  case Key_Minus:
    zoomOut();
    break;
  case Key_Plus:
  case Key_Equal:
    zoomIn();
    break;

  case Key_End:
    if ( e->state() & ControlButton )
      moveEnd();
    else
      continuousEnd();
    break;
  case Key_Home:
    if ( e->state() & ControlButton )
      moveSignal();
    else 
      fixedSignal();
    break;

  case Key_PageUp:
    if ( e->state() & ControlButton ) 
      moveStart();
    else if ( e->state() & ShiftButton ) 
      moveOffsLeft();
    else 
      moveLeft();
    break;
  case Key_PageDown:
    if ( e->state() & ControlButton )
      moveEnd();
    else if ( e->state() & ShiftButton ) 
      moveOffsRight();
    else
      moveRight();
    break;

  default:
    e->ignore();

  }
}


void PlotTrace::resizeEvent( QResizeEvent *qre )
{
  MultiPlot::resizeEvent( qre );

  if ( ButtonBox == 0 )
    return;

  QSize bs = ButtonBox->sizeHint();
  ButtonBox->setGeometry( width() - bs.width(), 0,
			  bs.width(), bs.height() );
}


void PlotTrace::customEvent( QCustomEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 1 : {
    if ( OnOffButton != 0 ) {
      lock();
      bool plotting = Plotting;
      unlock();
      if ( plotting )
	OnOffButton->setOn( false );
      else
	OnOffButton->setOn( true );
      break;
    }
  }
  case 2: {
    if ( OffsetButton != 0 ) {
      if ( OffsetMode == 0 )
	OffsetButton->setPixmap( FixedOffsetIcon );
      else
	OffsetButton->setPixmap( ContinuousOffsetIcon );
    }
    break;
  }
  }
}



}; /* namespace relacs */

#include "moc_plottrace.cc"

