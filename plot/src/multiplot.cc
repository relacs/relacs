/*
  multiplot.cc
  Having several Plots in a single widget.

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

#include <cmath>
#include <iostream>
#include <QApplication>
#include <QMouseEvent>
#include <relacs/str.h>
#include <relacs/multiplot.h>

namespace relacs {


MultiPlot::MultiPlot( int plots, int columns, bool horizontal, Plot::KeepMode keep,
	      QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::Recursive )
{
  construct( plots, columns, horizontal, keep );
}


MultiPlot::MultiPlot( int plots, int columns, bool horizontal,
		      QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::Recursive )
{
  construct( plots, columns, horizontal, Plot::Copy );
}


MultiPlot::MultiPlot( int plots, Plot::KeepMode keep, QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::Recursive )
{
  construct( plots, 1, true, keep );
}


MultiPlot::MultiPlot( int plots, QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::Recursive )
{
  construct( plots, 1, true, Plot::Copy );
}


MultiPlot::MultiPlot( QWidget *parent )
  : QWidget( parent ),
    PMutex( QMutex::Recursive )
{
  construct( 0, 1, true, Plot::Copy );
}


MultiPlot::~MultiPlot( void )
{
  clear();

  PMutex.lock();
  PMutex.unlock();
}


void MultiPlot::construct( int plots, int columns, bool horizontal, Plot::KeepMode keep )
{
  Columns = columns;
  Horizontal = horizontal;

  DMutex = 0;

  PMutex.lock();
  
  for ( int k=0; k<plots; k++ ) {
    PlotList.push_back( new Plot( keep, true, k, this ) );
    PlotList.back()->setBackgroundColor( Plot::Transparent );
    connect( PlotList.back(), SIGNAL( changedRange( int ) ),
	     this, SLOT( setRanges( int ) ) );
    CommonXRange.push_back( vector< int >( 0 ) );
    CommonYRange.push_back( vector< int >( 0 ) );
  }

  layout();

  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
			      QSizePolicy::Expanding ) );
  
  PMutex.unlock();

  setAutoFillBackground( true );

  // draw() ??
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
  DMutex = mutex;
  for ( unsigned int k=0; k<PlotList.size(); k++ )
    PlotList[k]->setDataMutex( DMutex );
}


void MultiPlot::clearDataMutex( void )
{
  DMutex = 0;
  for ( unsigned int k=0; k<PlotList.size(); k++ )
    PlotList[k]->clearDataMutex();
}


void MultiPlot::lockData( void )
{
  if ( DMutex != 0 )
    DMutex->lock();
}


void MultiPlot::unlockData( void )
{
  if ( DMutex != 0 )
    DMutex->unlock();
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
  if ( plots <= 0 )
    clear();
  else if ( plots > size() ) {
    for ( int k=size(); k<plots; k++ ) {
      PlotList.push_back( new Plot( keep, true, k, this ) );
      PlotList.back()->setBackgroundColor( Plot::Transparent );
      connect( PlotList.back(), SIGNAL( changedRange( int ) ),
	       this, SLOT( setRanges( int ) ) );
      CommonXRange.push_back( vector< int >( 0 ) );
      CommonYRange.push_back( vector< int >( 0 ) );
      PlotList.back()->setDataMutex( DMutex );
    }
  }
  else if ( plots < size() ) {
    for ( int k=size()-1; k>=plots; k-- ) {
      delete PlotList.back();
      PlotList.pop_back();
      CommonXRange.pop_back();
      CommonYRange.pop_back();
    }
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
  qApp->removePostedEvents( this );
  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p )
    delete (*p);
  PlotList.clear();
  CommonXRange.clear();
  CommonYRange.clear();
}


void MultiPlot::clear( int index )
{
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
}


void MultiPlot::layout( int columns, bool horizontal )
{
  Columns = columns;
  Horizontal = horizontal;

  layout();
}


void MultiPlot::layout( void )
{
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
}


void MultiPlot::setCommonXRange( int plot1, int plot2 )
{
  CommonXRange[ plot1 ].push_back( plot2 );
  CommonXRange[ plot2 ].push_back( plot1 );
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
  CommonYRange[ plot1 ].push_back( plot2 );
  CommonYRange[ plot2 ].push_back( plot1 );
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


QSize MultiPlot::minimumSizeHint( void ) const
{
  QSize QS( 200, 200 );
  return QS;
}


void MultiPlot::draw( void )
{
  update();
}


void MultiPlot::paintEvent( QPaintEvent *qpe )
{
  // the order of locking is important here!
  // if the data are not available there is no need to lock the plot.
  lockData();
  PMutex.lock();

  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    (**p).scale( width(), height() );
    (**p).draw( this ); // this will not lock the data again!
  }

  PMutex.unlock();
  unlockData();
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
    if ( PlotList[k]->mouseGrabbed() ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mousePressEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mousePressEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
}


void MultiPlot::mouseReleaseEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->mouseGrabbed() ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mouseReleaseEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mouseReleaseEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
}


void MultiPlot::mouseDoubleClickEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->mouseGrabbed() ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mouseDoubleClickEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mouseDoubleClickEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
}


void MultiPlot::mouseMoveEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->mouseGrabbed() ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mouseMoveEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      lockData();
      PMutex.lock();
      PlotList[k]->mouseMoveEvent( qme );
      PMutex.unlock();
      unlockData();
      return;
    }
  }
}


}; /* namespace relacs */

#include "moc_multiplot.cc"

