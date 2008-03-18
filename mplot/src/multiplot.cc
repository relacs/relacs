/*
  multiplot.cc
  Having several Plots in a single widget.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <qapplication.h>
#include <relacs/str.h>
#include <relacs/multiplot.h>


MultiPlot::MultiPlot( int plots, int columns, bool horizontal, Plot::KeepMode keep,
	      QWidget *parent, const char *name )
  : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ),
    PMutex( true )
{
  construct( plots, columns, horizontal, keep );
}


MultiPlot::MultiPlot( int plots, int columns, bool horizontal,
	      QWidget *parent, const char *name )
  : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ),
    PMutex( true )
{
  construct( plots, columns, horizontal, Plot::Copy );
}


MultiPlot::MultiPlot( int plots, Plot::KeepMode keep, QWidget *parent, const char *name )
  : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ),
    PMutex( true )
{
  construct( plots, 1, true, keep );
}


MultiPlot::MultiPlot( int plots, QWidget *parent, const char *name )
  : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ),
    PMutex( true )
{
  construct( plots, 1, true, Plot::Copy );
}


MultiPlot::MultiPlot( QWidget *parent, const char *name )
  : QWidget( parent, name, WRepaintNoErase | WResizeNoErase ),
    PMutex( true )
{
  construct( 0, 1, true, Plot::Copy );
}


MultiPlot::~MultiPlot( void )
{
  clear();

  PMutex.lock();
  delete PixMap;
  PMutex.unlock();
}


void MultiPlot::construct( int plots, int columns, bool horizontal, Plot::KeepMode keep )
{
  Columns = columns;
  Horizontal = horizontal;

  DMutex = 0;

  PMutex.lock();
  
  for ( int k=0; k<plots; k++ ) {
    string n = name() + Str( k );
    PlotList.push_back( new Plot( keep, true, k, this, n.c_str() ) );
    PlotList.back()->setBackgroundColor( Plot::Transparent );
    connect( PlotList.back(), SIGNAL( changedRange( int ) ),
	     this, SLOT( setRanges( int ) ) );
    CommonXRange.push_back( vector< int >( 0 ) );
    CommonYRange.push_back( vector< int >( 0 ) );
  }
  
  PixMap = new QPixmap;

  layout();

  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
			      QSizePolicy::Expanding ) );
  
  PMutex.unlock();

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
      string n = name() + Str( k );
      PlotList.push_back( new Plot( keep, true, k, this, n.c_str() ) );
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


QSize MultiPlot::sizeHint( void ) const
{
  QSize QS( 400, 400 );
  return QS;
}


QSize MultiPlot::minimumSizeHint( void ) const
{
  QSize QS( 200, 200 );
  return QS;
}


void MultiPlot::draw( void )
{
  QApplication::postEvent( this, new QPaintEvent( rect(), false ) );
}


void MultiPlot::paintEvent( QPaintEvent *qpe )
{
  PMutex.lock();
  PixMap->fill( paletteBackgroundColor() );

  for ( PlotListType::iterator p = PlotList.begin(); 
	p != PlotList.end(); 
	++p ) {
    (**p).setDataMutex( DMutex );
    (**p).scale( width(), height() );
    (**p).draw( PixMap );
  }
  bitBlt( this, 0, 0, PixMap, 0, 0, PixMap->width(), PixMap->height() );
  PMutex.unlock();
}


void MultiPlot::resizeEvent( QResizeEvent *qre )
{
  PMutex.lock();
  PixMap->resize( width(), height() );
  PMutex.unlock();
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
      PMutex.lock();
      PlotList[k]->mousePressEvent( qme );
      PMutex.unlock();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PMutex.lock();
      PlotList[k]->mousePressEvent( qme );
      PMutex.unlock();
      return;
    }
  }
}


void MultiPlot::mouseReleaseEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->mouseGrabbed() ) {
      PMutex.lock();
      PlotList[k]->mouseReleaseEvent( qme );
      PMutex.unlock();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PMutex.lock();
      PlotList[k]->mouseReleaseEvent( qme );
      PMutex.unlock();
      return;
    }
  }
}


void MultiPlot::mouseDoubleClickEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->mouseGrabbed() ) {
      PMutex.lock();
      PlotList[k]->mouseDoubleClickEvent( qme );
      PMutex.unlock();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PMutex.lock();
      PlotList[k]->mouseDoubleClickEvent( qme );
      PMutex.unlock();
      return;
    }
  }
}


void MultiPlot::mouseMoveEvent( QMouseEvent *qme )
{
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->mouseGrabbed() ) {
      PMutex.lock();
      PlotList[k]->mouseMoveEvent( qme );
      PMutex.unlock();
      return;
    }
  }
  for ( unsigned int k=0; k<PlotList.size(); k++ ) {
    if ( PlotList[k]->inside( qme->x(), qme->y() ) ) {
      PMutex.lock();
      PlotList[k]->mouseMoveEvent( qme );
      PMutex.unlock();
      return;
    }
  }
}
