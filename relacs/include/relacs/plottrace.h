/*
  plottrace.h
  Plot trace and spikes.

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

#ifndef _RELACS_PLOTTRACE_H_
#define _RELACS_PLOTTRACE_H_ 1

#include <QWidget>
#include <QPushButton>
#include <QMenu>
#include <QTimer>
#include <deque>
#include <relacs/relacsplugin.h>
#include <relacs/multiplot.h>

namespace relacs {


class RELACSWidget;
class PlotTraceStyle;
class PlotEventStyle;

/*!
\class PlotTrace
\author Jan Benda
\author Christian Machens
\brief Plot trace and spikes.
\todo init(): user configurabel plot modes (paged, fixed, contiuous): origin and time units
*/

  /*! Flag for the modes of traces or events, indicating that they should be plotted. */
static const int PlotTraceMode = 0x0008;
  /*! Flag for the modes of events to be used as a trigger signal. */
static const int PlotTriggerMode = 0x0800;
  /*! Flag for the modes of traces, indicating that it should be centered vertically. */
static const int PlotTraceCenterVertically = 0x0100;

class PlotTrace : public RELACSPlugin
{
  Q_OBJECT

  friend class RELACSWidget;

public:

    /*! Construct a PlotTrace. */
  PlotTrace( RELACSWidget *rw, QWidget* parent=0 );
    /*! Destruct a PlotTrace. */
  ~PlotTrace( );

    /*! Switch plotting of raw traces on or off. */
  void setPlotOn( bool on=true );
    /*! Switch plotting of raw traces off. */
  void setPlotOff( void );
    /*! Plot raw traces relative to signal in a window of width \a length seconds
        and the start of the signal \a offs seconds from the left margin. */
  void setPlotSignal( double length, double offs=0.0 );
    /*! Plot raw traces relative to signal while leaving the window size unchanged. */
  void setPlotSignal( void );
    /*! Plot raw traces continuously in a window of width \a length seconds. */
  void setPlotContinuous( double length );
    /*! Plot raw traces continuously while leaving the window size unchanged. */
  void setPlotContinuous( void );

    /*! Clear the trace and events styles. */
  void clearStyles( void );
    /*! Add a PlotTraceStyle */
  void addTraceStyle( bool visible, int panel, int lcolor );
    /*! The list of styles for each trace. */
  const deque< PlotTraceStyle > &traceStyles( void ) const;
    /*! The list of styles for each trace. */
  deque< PlotTraceStyle > &traceStyles( void );
    /*! The list of styles for each events. */
  const deque< PlotEventStyle > &eventStyles( void ) const;
    /*! The list of styles for each events. */
  deque< PlotEventStyle > &eventStyles( void );

    /*! Set the number of plots necessary for the input traces and events. */
  void resize( void );
    /*! Initialize the plots with the data \a data and \a events. */
  void init( void );
    /*! Add menu entries controlling the time window to \a menu. */
  void addMenu( QMenu *menu );
    /*! Update menu entries toggeling the traces. */
  void updateMenu( void );
    /*! Start plotting with time interval \a time. */
  void start( double time );
    /*! Stop plotting. */
  void stop( void );

    /*! \return the time of the last signal. */
  double signalTime( void ) const;
    /*! \return the current recording time of the input buffers. */
  double currentTime( void ) const;


public slots:

    /*! Plot voltage traces and events. */
  void plot( void );

    /*! Toggle plot of trace \a trace. */
  void toggle( QAction *trace );

  void zoomOut( void );
  void zoomIn( void );
  void moveLeft( void );
  void moveRight( void );
  void moveStart( void );
  void moveEnd( void );
  void moveToSignal( void );
  void viewSignal( void );
  void moveSignalOffsLeft( void );
  void moveSignalOffsRight( void );
  void viewEnd( void );
  void viewWrapped( void );
  void toggleTrigger( void );
  void manualRange( void );
  void autoRange( void );
  void centerVertically( void );
  void centerZoomVertically( void );

  void plotOnOff( void );
  void viewToggle( void );
  void toggleManual( void );

  void print( void );

  void displayIndex( const string &path, const deque<int> &traceindex,
		     const deque<int> &eventsindex, double time );
  void displayData( void );


protected:

  virtual void resizeLayout( void );
  virtual void resizeEvent( QResizeEvent *qre );
  virtual void keyPressEvent( QKeyEvent *event );
  virtual void customEvent( QEvent *qce );

  QWidget *ButtonBox;
  QHBoxLayout *ButtonBoxLayout;
  QPushButton *OnOffButton;
  QPushButton *ViewButton;
  QPushButton *ManualButton;

  QPixmap SignalViewIcon;
  QPixmap EndViewIcon;


protected slots:

  void updateRanges( int id );
  void resizePlots( QResizeEvent *qre );


private:

    /*! Different view modes. */
  enum Views {
      /*! Keep the display fixed. */
    FixedView,
      /*! Show the traces relative to the current signal time. */
    SignalView,
      /*! Show the traces relative to the current data. */
    EndView,
      /*! Show the traces wrapped relative to the current data. */
    WrapView,
      /*! Show the traces either in EndView or WrapView according to
	  the ContinuousView variable. */
    ContView,
  };

  void setView( Views mode );
  void updateStyle( void );

  Views ContinuousView;

  double TimeWindow;
  double TimeOffs;

  Views ViewMode;
  bool PlotChanged;
  double LeftTime;
  double Offset;
  bool Trigger;
  int TriggerSource;
  bool Manual;

  bool Plotting;

  bool AutoOn;
  bool AutoFixed;
  double AutoTime;
  double AutoOffs;

  deque< int > VP;  // the indices of visible plots
  MultiPlot P;
  deque< PlotTraceStyle > TraceStyle; // additional data for each trace
  deque< PlotEventStyle > EventStyle; // additional data for each events

  QMenu *Menu;

  QTimer PlotTimer;

  bool FilePlot;
  string FilePath;
  Options FileHeader;
  InList FileTraces;
  deque< string > FileTracesNames;
  deque< int > FileSizes;
  EventList FileEvents;
  deque< string > FileEventsNames;

  InList PlotTraces;
  EventList PlotEvents;

};


/*!
\class PlotTraceStyle
\author Jan Benda
\brief Plot style and properties for a trace.
*/

class PlotTraceStyle
{

public:

  PlotTraceStyle( void );
  PlotTraceStyle( const PlotTraceStyle &pts );
  PlotTraceStyle( bool visible, int panel, int lcolor );

  bool visible( void ) const;
  void setVisible( bool visible );

  int panel( void ) const;
  void setPanel( int panel );

  int handle( void ) const;
  void setHandle( int handle );

  void clearPanel( void );

  const QAction *action( void ) const;
  QAction *action( void );
  void setAction( QAction *action );

  const Plot::LineStyle &line( void ) const;
  Plot::LineStyle &line( void );
  void setLine( const Plot::LineStyle &style );
  void setLine( int lcolor=Plot::Transparent, int lwidth=1,
		Plot::Dash ldash=Plot::Solid );


protected:

  QAction* Action;
  bool Visible;
  int Panel;   // index to plot widget
  int Handle;  // handle for updating plot styles
  Plot::LineStyle Line;

};


/*!
\class PlotEventStyle
\author Jan Benda
\brief Plot style and properties for a events.
*/

class PlotEventStyle : public PlotTraceStyle
{

public:

  PlotEventStyle( void );
  PlotEventStyle( const PlotEventStyle &pes );

  const Plot::PointStyle &point( void ) const;
  Plot::PointStyle &point( void );
  void setPoint( const Plot::PointStyle &style );
  void setPoint( Plot::Points ptype=Plot::Circle, int psize=10,
		 int pcolor=Plot::Transparent, int pfill=Plot::Transparent );

  void setStyle( const Plot::LineStyle &lstyle, 
		 const Plot::PointStyle &pstyle );
  void setStyle( int lcolor=Plot::Transparent, int lwidth=1,
		 Plot::Dash ldash=Plot::Solid, 
		 Plot::Points ptype=Plot::Circle, int psize=10,
		 int pcolor=Plot::Transparent, int pfill=Plot::Transparent );

  double yPos( void ) const;
  Plot::Coordinates yCoor( void ) const;
  void setYPos( double ypos, Plot::Coordinates ycoor=Plot::Graph );
  void setYData( void );

  double size( void ) const;
  Plot::Coordinates sizeCoor( void ) const;
  void setSize( double size, Plot::Coordinates sizecoor=Plot::GraphY );


protected:

  Plot::PointStyle Point;
  double YPos;
  Plot::Coordinates YCoor;
  bool YData;
  double Size;
  Plot::Coordinates SizeCoor;

};


/*!
\class PrintThread
\author Jan Benda
\brief Worker thread for printing traces.
*/

class PrintThread : public QThread
{
  Q_OBJECT

public:
  PrintThread( const string &printcommand );

protected:
  void run( void );

private:
  string PrintCommand;

};


}; /* namespace relacs */

#endif /* ! _RELACS_PLOTTRACE_H_ */

