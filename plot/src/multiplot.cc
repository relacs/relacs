/*
  multiplot.cc
  Having several Plots in a single widget.

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

#include <cmath>
#include <iostream>
#include <QApplication>
#include <QThread>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <relacs/str.h>
#include <relacs/multiplot.h>

namespace relacs {


class MultiPlotEvent : public QEvent
{

public:

  MultiPlotEvent( int type, int plots=0, Plot::KeepMode keep=Plot::Copy )
    : QEvent( Type( User+type ) ),
      Plots( plots ),
      Keep( keep )
  {
  }

  int Plots;
  Plot::KeepMode Keep;
};


MultiPlot::MultiPlot( int plots, int columns, bool horizontal, Plot::KeepMode keep,
		      QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::NonRecursive )
{
  construct( plots, columns, horizontal, keep );
}


MultiPlot::MultiPlot( int plots, int columns, bool horizontal,
		      QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::NonRecursive )
{
  construct( plots, columns, horizontal, Plot::Copy );
}


MultiPlot::MultiPlot( int plots, Plot::KeepMode keep, QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::NonRecursive )
{
  construct( plots, 1, true, keep );
}


MultiPlot::MultiPlot( int plots, QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::NonRecursive )
{
  construct( plots, 1, true, Plot::Copy );
}


MultiPlot::MultiPlot( QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::NonRecursive )
{
  construct( 0, 1, true, Plot::Copy );
}


MultiPlot::~MultiPlot( void )
{
  clear();
}


void MultiPlot::construct( int plots, int columns, bool horizontal, Plot::KeepMode keep )
{
  PMutex.lock();

  GUIThread = QThread::currentThread();

  setAttribute( Qt::WA_OpaquePaintEvent );

  Columns = columns;
  Horizontal = horizontal;

  DMutex = 0;
  DRWMutex = 0;
  Painting = false;
  
  for ( int k=0; k<plots; k++ ) {
    PlotList.push_back( new Plot( keep, true, k, this ) );
    //    PlotList.back()->setBackgroundColor( Plot::Transparent );
    connect( PlotList.back(), SIGNAL( changedRange( int ) ),
	     this, SLOT( setRanges( int ) ) );
    CommonXRange.push_back( vector< int >( 0 ) );
    CommonYRange.push_back( vector< int >( 0 ) );
  }
  DrawData = false;
  DrawBackground = true;
  UpdatePlotList.clear();

  layout();

  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
			      QSizePolicy::Expanding ) );
  setFocusPolicy( Qt::NoFocus );
  
  PMutex.unlock();

  // XXX draw() ??
}


void MultiPlot::lock( void )
{
  PMutex.lock();
}


void MultiPlot::unlock( void )
{
  PMutex.unlock();
}


void MultiPlot::setDataMutex( QMutex *mutex )
{
  if ( DMutex == 0 && DRWMutex == 0 ) {
    DRWMutex = 0;
    DMutex = mutex;
    for ( unsigned int k=0; k<PlotList.size(); k++ )
      PlotList[k]->setDataMutex( DMutex );
  }
}


void MultiPlot::setDataMutex( QReadWriteLock *mutex )
{
  if ( DMutex == 0 && DRWMutex == 0 ) {
    DMutex = 0;
    DRWMutex = mutex;
    for ( unsigned int k=0; k<PlotList.size(); k++ )
      PlotList[k]->setDataMutex( DRWMutex );
  }
}


void MultiPlot::clearDataMutex( void )
{
  DMutex = 0;
  DRWMutex = 0;
  for ( unsigned int k=0; k<PlotList.size(); k++ )
    PlotList[k]->clearDataMutex();
}


int MultiPlot::size( void ) const
{
  return PlotList.size();
}


bool MultiPlot::empty( void ) const
{
  return ( PlotList.size() == 0 );
}


void MultiPlot::resize( int plots, Plot::KeepMode keep )
{
  if ( QThread::currentThread() != GUIThread ) {
    qApp->removePostedEvents( this );
    QCoreApplication::postEvent( this, new MultiPlotEvent( 101, plots, keep ) );
    WaitGUI.wait( &PMutex );
  }
  else
    doResize( plots, keep );
}


void MultiPlot::doResize( int plots, Plot::KeepMode keep )
{
  if ( plots <= 0 )
    doClear();
  else {
    Painting = false;
    if ( plots > size() ) {
      for ( int k=size(); k<plots; k++ ) {
	PlotList.push_back( new Plot( keep, true, k, this ) );
	//      PlotList.back()->setBackgroundColor( Plot::Transparent );
	connect( PlotList.back(), SIGNAL( changedRange( int ) ),
		 this, SLOT( setRanges( int ) ) );
	CommonXRange.push_back( vector< int >( 0 ) );
	CommonYRange.push_back( vector< int >( 0 ) );
	if ( DMutex != 0 )
	  PlotList.back()->setDataMutex( DMutex );
	else if ( DRWMutex != 0 )
	  PlotList.back()->setDataMutex( DRWMutex );
      }
      for ( unsigned int k=0; k<PlotList.size(); k++ ) {
	PlotList[k]->resetRanges();
	CommonXRange[k].clear();
	CommonYRange[k].clear();
      }
    }
    else if ( plots < size() ) {
      for ( int k=size()-1; k>=plots; k-- ) {
	delete PlotList.back();
	PlotList.pop_back();
	CommonXRange.pop_back();
	CommonYRange.pop_back();
      }
      for ( unsigned int k=0; k<PlotList.size(); k++ ) {
	PlotList[k]->resetRanges();
	CommonXRange[k].clear();
	CommonYRange[k].clear();
      }
    }
    UpdatePlotList.clear();
    DrawBackground = true;
  }
}


void MultiPlot::resize( int plots, int columns, bool horizontal, Plot::KeepMode keep )
{
  resize( plots, keep );

  Columns = columns;
  Horizontal = horizontal;

  layout();
}


void MultiPlot::clear( void )
{
  if ( QThread::currentThread() != GUIThread ) {
    qApp->removePostedEvents( this );
    QCoreApplication::postEvent( this, new MultiPlotEvent( 102 ) );
    WaitGUI.wait( &PMutex );
  }
  else
    doClear();
}


void MultiPlot::doClear( void )
{
  Painting = false;
  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p )
    delete (*p);
  PlotList.clear();
  UpdatePlotList.clear();
  CommonXRange.clear();
  CommonYRange.clear();
  DrawBackground = true;
}


void MultiPlot::erase( int index )
{
  if ( QThread::currentThread() != GUIThread ) {
    qApp->removePostedEvents( this );
    QCoreApplication::postEvent( this, new MultiPlotEvent( 103, index ) );
    WaitGUI.wait( &PMutex );
  }
  else
    doErase( index );
}


void MultiPlot::doErase( int index )
{
  Painting = false;
  int k;
  PlotListType::iterator p;
  CommonRangeType::iterator cx;
  CommonRangeType::iterator cy;
  for ( k=0, p = PlotList.begin(), cx = CommonXRange.begin(), cy = CommonYRange.begin();
	k<index && p != PlotList.end(); ++p, ++cx, ++cy, ++k );
  if ( k == index && p != PlotList.end() ) {
    delete (*p);
    PlotList.erase( p );
    CommonXRange.erase( cx );
    CommonYRange.erase( cy );
  }
  UpdatePlotList.clear();
  DrawBackground = true;
}


void MultiPlot::layout( int columns, bool horizontal )
{
  Columns = columns;
  Horizontal = horizontal;

  layout();
}


void MultiPlot::layout( void )
{
  Painting = false;

  unsigned int n = PlotList.size();

  if ( n == 0 )
    return;

  int columns = Columns;
  int rows = ( n - 1 ) / columns + 1;

  if ( !Horizontal ) {
    int swap = rows;
    rows = columns;
    columns = swap;
  }

  double xsize = 1.0/columns;
  double ysize = 1.0/rows;

  int c = 0;
  int r = rows - 1;
  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    (**p).setSize( xsize, ysize );
    (**p).setOrigin( c*xsize, r*ysize );
    if ( Horizontal ) {
      c++;
      if ( c >= columns ) {
	c = 0;
	r--;
      }
    }
    else {
      r--;
      if ( r < 0 ) {
	r = rows - 1;
	c++;
      }
    }
  }
  UpdatePlotList.clear();
  DrawBackground = true;
}


void MultiPlot::setCommonXRange( int plot1, int plot2 )
{
  if ( plot1 != plot2 ) {
    CommonXRange[ plot1 ].push_back( plot2 );
    CommonXRange[ plot2 ].push_back( plot1 );
  }
}


void MultiPlot::setCommonXRange( void )
{
  for ( unsigned int k=0; k<CommonXRange.size(); k++ ) {
    CommonXRange[k].clear();
    for ( unsigned int j=0; j<CommonXRange.size(); j++ ) {
      if ( j != k )
	CommonXRange[ k ].push_back( j );
    }
  }
}


void MultiPlot::setCommonYRange( int plot1, int plot2 )
{
  if ( plot1 != plot2 ) {
    CommonYRange[ plot1 ].push_back( plot2 );
    CommonYRange[ plot2 ].push_back( plot1 );
  }
}


void MultiPlot::setCommonYRange( void )
{
  for ( unsigned int k=0; k<CommonYRange.size(); k++ ) {
    CommonYRange[k].clear();
    for ( unsigned int j=0; j<CommonYRange.size(); j++ ) {
      if ( j != k )
	CommonYRange[ k ].push_back( j );
    }
  }
}


void MultiPlot::setCommonRange( int plot1, int plot2 )
{
  setCommonXRange( plot1, plot2 );
  setCommonYRange( plot1, plot2 );
}


void MultiPlot::setCommonRange( void )
{
  setCommonXRange();
  setCommonYRange();
}


QSize MultiPlot::sizeHint( void ) const
{
  PMutex.lock();
  double meanw = 0.0;
  double meanh = 0.0;
  int n = 0;
  for ( PlotListType::const_iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    n++;
    (**p).lock();
    double w=0.0,h=0.0;
    (**p).size( w, h );
    (**p).unlock();
    meanw += ( w - meanw ) / n;
    meanh += ( h - meanh ) / n;
  }
  PMutex.unlock();
  if ( meanw < 0.02 )
    meanw = 0.5;
  if ( meanh < 0.02 )
    meanh = 0.5;
  QSize qs( (int)::ceil( 180.0/meanw ), (int)::ceil( 120.0/meanh ) );
  return qs;
}


QSize MultiPlot::minimumSizeHint( void ) const
{
  PMutex.lock();
  double meanw = 0.0;
  double meanh = 0.0;
  int n = 0;
  for ( PlotListType::const_iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    n++;
    (**p).lock();
    double w=0.0,h=0.0;
    (**p).size( w, h );
    (**p).unlock();
    meanw += ( w - meanw ) / n;
    meanh += ( h - meanh ) / n;
  }
  PMutex.unlock();
  if ( meanw < 0.02 )
    meanw = 0.5;
  if ( meanh < 0.02 )
    meanh = 0.5;
  QSize qs( (int)::ceil( 120.0/meanw ), (int)::ceil( 80.0/meanh ) );
  return qs;
}


void MultiPlot::draw( void )
{
  Painting = false;
  UpdatePlotList.clear();
  DrawData = true;
  if ( QThread::currentThread() != GUIThread )
    QCoreApplication::postEvent( this, new MultiPlotEvent( 100 ) ); // update
  else
    update();
}


void MultiPlot::paintEvent( QPaintEvent *qpe )
{
  PMutex.lock();
  Painting = true;
  bool newlist = false;
  // initialize list of plots that need to be updated:
  if ( UpdatePlotList.empty() ) {
    UpdatePlotList = PlotList;
    for ( PlotListType::iterator p = UpdatePlotList.begin(); 
	  p != UpdatePlotList.end(); 
	  ) {
      if ( (**p).skip() )
	p = UpdatePlotList.erase( p );
      else
	++p;
    }
    newlist = true;
  }

  // loop through the subplots:
  PlotListType::iterator p = UpdatePlotList.begin(); 
  while ( p != UpdatePlotList.end() ) {

    PMutex.unlock();

    Plot *cp = *p;

    // if the data are not available there is no need to lock the plot.
    if ( ! cp->tryLockData( 5 ) ) {
      // we do not get the lock for the data now,
      // so we check the other plots first:
      PMutex.lock();
      if ( ! Painting )  // the subplots have been changed in the meantime!
	break;
      for ( ++p; p != UpdatePlotList.end() && (**p).equalDataMutex( *cp ); ++p );
      continue;
    }

    PMutex.lock();
    if ( ! Painting ) {  // the subplots have been changed in the meantime!
      cp->unlockData();
      break;
    }

    if ( newlist ) {
      // redraw background:
      if ( DrawBackground || ! DrawData ) {
	QPainter qp( this );
	qp.eraseRect( rect() );
      }
    }

    // draw subplot:
    cp->scale( width(), height() );
    cp->draw( this, DrawData );

    // draw subplots with the same or no data lock:
    PlotListType::iterator sp = UpdatePlotList.erase( p ); 
    bool setp = false;
    while ( sp != UpdatePlotList.end() ) {
      if ( (**sp).equalDataMutex( *cp ) || (**sp).noDataMutex() ) {
	(**sp).scale( width(), height() );
	(**sp).draw( this, DrawData );
	sp = UpdatePlotList.erase( sp );
      }
      else {
	if ( ! setp )
	  p = sp;
	setp = true;
	++sp;
      }
    }
    if ( ! setp )
      p = UpdatePlotList.end();

    PMutex.unlock();
    cp->unlockData();

    PMutex.lock();
    if ( ! Painting )  // the subplots have been changed in the meantime!
      break;
  }

  if ( Painting && ! UpdatePlotList.empty() ) {
    Painting = false;
    PMutex.unlock();
    // we did not get the lock for the data of some plots now,
    // so we repost the paintEvent() for the remaining subplots to a later time.
    update();
    return;
  }

  DrawBackground = false;
  DrawData = false;
  Painting = false;
  PMutex.unlock();
}


void MultiPlot::resizeEvent( QResizeEvent *qre )
{
  emit resizePlots( qre );
  PMutex.lock();
  Painting = false;
  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    if ( ! (**p).skip() )
      (**p).resizeEvent( qre );
  }
  UpdatePlotList.clear();
  DrawBackground = true;
  PMutex.unlock();
  QWidget::resizeEvent( qre );
}


void MultiPlot::setDrawBackground( void )
{
  DrawBackground = true;
}


void MultiPlot::clearPlots( void )
{
  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    (**p).clear();
  }
}


void MultiPlot::setRanges( int id )
{
  for ( unsigned int j=0; j<CommonXRange[id].size(); j++ ) {
    PlotList[CommonXRange[id][j]]->setXRange( PlotList[id]->xminRange(), 
					      PlotList[id]->xmaxRange() );
  }
  for ( unsigned int j=0; j<CommonYRange[id].size(); j++ ) {
    PlotList[CommonYRange[id][j]]->setYRange( PlotList[id]->yminRange(),
					      PlotList[id]->ymaxRange() );
  }

  emit changedRanges( id );
}


void MultiPlot::enableMouse( void )
{
  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    (**p).enableMouse();
  }
}


void MultiPlot::disableMouse( void )
{
  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    (**p).disableMouse();
  }
}


void MultiPlot::mousePressEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->mouseGrabbed() ) {
      PlotList[k]->mousePressEvent( qme );
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PlotList[k]->mousePressEvent( qme );
      return;
    }
  }
}


void MultiPlot::mouseReleaseEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->mouseGrabbed() ) {
      PlotList[k]->mouseReleaseEvent( qme );
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PlotList[k]->mouseReleaseEvent( qme );
      return;
    }
  }
}


void MultiPlot::mouseDoubleClickEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->mouseGrabbed() ) {
      PlotList[k]->mouseDoubleClickEvent( qme );
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PlotList[k]->mouseDoubleClickEvent( qme );
      return;
    }
  }
}


void MultiPlot::mouseMoveEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->mouseGrabbed() ) {
      PlotList[k]->mouseMoveEvent( qme );
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( !PlotList[k]->skip() && PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PlotList[k]->mouseMoveEvent( qme );
      return;
    }
  }
}


void MultiPlot::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 100: {
    update();
    break;
  }
  case 101: {
    MultiPlotEvent *mpe = dynamic_cast<MultiPlotEvent*>( qce );
    PMutex.lock();
    doResize( mpe->Plots, mpe->Keep );
    PMutex.unlock();
    WaitGUI.wakeAll();
    break;
  }
  case 102: {
    PMutex.lock();
    doClear();
    PMutex.unlock();
    WaitGUI.wakeAll();
    break;
  }
  case 103: {
    MultiPlotEvent *mpe = dynamic_cast<MultiPlotEvent*>( qce );
    PMutex.lock();
    doErase( mpe->Plots );
    PMutex.unlock();
    WaitGUI.wakeAll();
    break;
  }
  default:
    QWidget::customEvent( qce );
  }
}


}; /* namespace relacs */

#include "moc_multiplot.cc"

