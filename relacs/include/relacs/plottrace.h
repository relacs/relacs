/*
  plottrace.h
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

#ifndef _RELACS_PLOTTRACE_H_
#define _RELACS_PLOTTRACE_H_ 1

#include <QWidget>
#include <QPushButton>
#include <QMenu> 
#include <vector>
#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/multiplot.h>

namespace relacs {


class RELACSWidget;

/*!
\class PlotTrace
\author Jan Benda
\author Christian Machens
\version 2.0
\brief Plot trace and spikes.
\todo init(): user configurabel plot modes (paged, fixed, contiuous): origin and time units
*/

  /*! Flag for the modes of traces or events, indicating that they should be plotted. */
static const int PlotTraceMode = 0x0008;

class PlotTrace : public MultiPlot
{
  Q_OBJECT

  friend class RELACSWidget;

public:

  struct TraceStyle
  {
    TraceStyle( void )
      : PlotWindow( 0 ),
	Line( Plot::Green, 2, Plot::Solid ),
	Point( Plot::Circle, 0, Plot::Green, Plot::Green )
    {};
    int PlotWindow;
    Plot::LineStyle Line;
    Plot::PointStyle Point;
  };

  struct EventStyle
  {
    EventStyle( void )
      : PlotWindow( 0 ),
	Line( Plot::Transparent, 0, Plot::Solid ),
	Point( Plot::Circle, 1, Plot::Yellow, Plot::Yellow ),
	YPos( 0.1 ),
	YCoor( Plot::Graph ),
	YData( false ),
	Size( 6.0 ),
	SizeCoor( Plot::Pixel )
    {};
    int PlotWindow;
    Plot::LineStyle Line;
    Plot::PointStyle Point;
    double YPos;
    Plot::Coordinates YCoor;
    bool YData;
    double Size;
    Plot::Coordinates SizeCoor;
  };

    /*! Construct a PlotTrace. */
  PlotTrace( RELACSWidget *ow, QWidget* parent=0, const char* name=0 );
    /*! Destruct a PlotTrace. */
  ~PlotTrace( );

  void keyPressEvent( QKeyEvent* e);


public slots:

    /*! Plot voltage traces and spike trains. */
  void plot( const InList &data, const EventList &events );
    /*! Specify some properties of PlotTrace. 
        Set total time window to \a length seconds
	and part preceeding stimulus to \a offs seconds. */
  void setState( bool on, bool fixed, double length, double offs );

    /*! Toggle plot of trace \a i. */
  void toggle( int i );
    /*! Set the number of plots necessary for \a data and \a events. */
  void resize( InList &data, const EventList &events );
    /*! Initialize the plots with the data \a data and \a events. */
  void init( const InList &data, const EventList &events );
    /*! Add menu entries controlling the time window to \a menu. */
  void addMenu( QPopupMenu *menu );
    /*! Update menu entries toggeling the traces. */
  void updateMenu( void );

  void zoomOut( void );
  void zoomIn( void );
  void moveLeft( void );
  void moveRight( void );
  void moveStart( void );
  void moveEnd( void );
  void moveSignal( void );
  void fixedSignal( void );
  void moveOffsLeft( void );
  void moveOffsRight( void );
  void continuousEnd( void );
  void manualRange( void );
  void autoRange( void );

  void plotOnOff( void );
  void offsetToggle( void );
  void toggleManual( void );
  void setOffset( int mode );


protected:

  virtual void resizeLayout( void );
  virtual void resizeEvent( QResizeEvent *qre );
  virtual void customEvent( QEvent *qce );

  QHBox *ButtonBox;
  QPushButton *OnOffButton;
  QPushButton *OffsetButton;
  QPushButton *ManualButton;

  QPixmap FixedOffsetIcon;
  QPixmap ContinuousOffsetIcon;


protected slots:

  void updateRanges( int id );


private:

  double TimeWindow;
  double TimeOffs;

  int OffsetMode;
  bool PlotChanged;
  double LeftTime;
  double Offset;
  bool Manual;

  bool Plotting;

  bool AutoOn;
  bool AutoFixed;
  double AutoTime;
  double AutoOffs;

  InList *IL;
  const EventList *EL;

  vector< int > PlotElements;

  RELACSWidget *RW;

  QPopupMenu *Menu;

};


}; /* namespace relacs */

#endif /* ! _RELACS_PLOTTRACE_H_ */

