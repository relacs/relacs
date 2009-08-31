/*
  multiplot.h
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

#ifndef _RELACS_MULTIPLOT_H_
#define _RELACS_MULTIPLOT_H_ 1


#include "plot.h"
#include <vector>
#include <QWidget>
#include <QMutex>

namespace relacs {


/*! 
\class MultiPlot
\author Jan Benda
\version 0.2
\brief Having several Plots in a single widget.
*/


class MultiPlot : public QWidget
{
  Q_OBJECT

public:

    /*! Constructs \a plots plots with KeepMode \a keep.
        If you set \a keep to Plot::Pointer and
	you are using multible threads, then
	you need to provide a mutex for locking the data
	via setDataMutex(). */
  MultiPlot( int plots, int columns, bool horizontal=true, Plot::KeepMode keep=Plot::Copy,
	     QWidget *parent=0 );
    /*! Constructs \a plots plots. */
  MultiPlot( int plots, int columns, bool horizontal=true, QWidget *parent=0 );
    /*! Constructs \a plots plots with KeepMode \a keep.
        If you set \a keep to Plot::Pointer and
	you are using multible threads, then
	you need to provide a mutex for locking the data
	via setDataMutex(). */
  MultiPlot( int plots, Plot::KeepMode keep, QWidget *parent=0 );
  MultiPlot( int plots, QWidget *parent=0 );
  MultiPlot( QWidget *parent=0 );
  ~MultiPlot( void );

    /*! Lock the plot mutex. */
  void lock( void );
    /*! Unlock the plot mutex. */
  void unlock( void );

    /*! Provide a mutex that is used to lock
        access to all data while they are plotted. 
        Passing a '0' disabled the data mutex.
        \sa clearDataMutex(), lockData(), unlockData() */
  void setDataMutex( QMutex *mutex );
    /*! Disables the data mutex and the data mutexes of the subplots.
        \sa setDataMutex() */
  void clearDataMutex( void );
    /*! Lock the data mutex. \sa setDataMutex(), clearDataMutex() */
  void lockData( void );
    /*! Unlock the data mutex. \sa setDataMutex(), clearDataMutex() */
  void unlockData( void );

    /*! The number of Plots in the MultiPlot widget. */
  int size( void ) const;
    /*! True if there isn't any Plot in the MultiPlot widget. */
  bool empty( void ) const;

    /*! Change the number of plots to \a plots. */
  void resize( int plots, Plot::KeepMode keep=Plot::Copy );
    /*! Change the number of plots to \a plots
        and update the layout. */
  void resize( int plots, int columns, bool horizontal, Plot::KeepMode keep=Plot::Copy );

    /*! Remove all Plots from the MultiPlot widget. */
  void clear( void );
    /*! Remove Plot number \a index from the MultiPlot widget. */
  void clear( int index );

    /*! The Plot-widget \a i. */
  Plot &operator[]( int i ) { return *PlotList[i]; };
    /*! The last Plot in the list. */
  Plot &back( void ) { return *PlotList[PlotList.size()-1]; };
    /*! The first Plot in the list. */
  Plot &front( void ) { return *PlotList[0]; };

  void layout( int columns, bool horizontal=true );
  void layout( void );

  void setCommonXRange( int plot1, int plot2 );
  void setCommonXRange( void );
  void setCommonYRange( int plot1, int plot2 );
  void setCommonYRange( void );
  void setCommonRange( int plot1, int plot2 );
  void setCommonRange( void );

    /*! Give a hint for the minimum size of this widget. */
  virtual QSize minimumSizeHint( void ) const;

    /*! Draw the Plots. */
  void draw( void );

    /*! Clear each Plot from data, i.e. call Plot::clear() for each Plot. */
  void clearPlots( void );

    /*! Enable mouse support for all subplots. */
  void enableMouse( void );
    /*! Disable mouse support for all subplots. */
  void disableMouse( void );


signals:

    /*! Ranges of some plots where changes
        due to a change in plot \a id. */
  void changedRanges( int id );


protected:

    /*! Paints the entire plot. */
  void paintEvent( QPaintEvent *qpe );

    /*! The Qt mouse event handler for a mouse press event.
        Dispatches the event to the appropriate Plot. */
  virtual void mousePressEvent( QMouseEvent *qme );
    /*! The Qt mouse event handler for a mouse release event.
        Dispatches the event to the appropriate Plot. */
  virtual void mouseReleaseEvent( QMouseEvent *qme );
    /*! The Qt mouse event handler for a mouse doouble click event.
        Dispatches the event to the appropriate Plot. */
  virtual void mouseDoubleClickEvent( QMouseEvent *qme );
    /*! The Qt mouse event handler for a mouse move event.
        Dispatches the event to the appropriate Plot. */
  virtual void mouseMoveEvent( QMouseEvent *qme );


protected slots:

  void setRanges( int id );


private:

  void construct( int plots, int columns, bool horizontal, Plot::KeepMode keep );

  typedef vector < Plot* > PlotListType;
  PlotListType PlotList;

  typedef vector< vector< int > > CommonRangeType;
  CommonRangeType CommonXRange;
  CommonRangeType CommonYRange;

  QMutex PMutex;
  QMutex *DMutex;

  int Columns;
  bool Horizontal;

};


}; /* namespace relacs */

#endif /* ! _RELACS_MULTIPLOT_H_ */
