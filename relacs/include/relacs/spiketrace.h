/*
  spiketrace.h
  A nice, almost useless widget, showing an animated trace of a spike.

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

#ifndef _RELACS_SPIKETRACE_H_
#define _RELACS_SPIKETRACE_H_ 1

#include <QWidget>
#include <QThread>
#include <QMutex>

namespace relacs {


/*!
  \class SpikeTrace
  \author Jan Benda
  \version 1.0
  \brief A nice, almost useless widget, showing an animated trace of a spike.
  
  
  The SpikeTrace-widget shows a darkgreen voltage trace with a single spike.
  A green ball is running from left to right along the trace.
  At each repetition the position of the spike is changed randomly.
  This is like the trace on an oscilloscope.
  
  Use setSpike() to toggle this animation completly on and off.
  With setPause() the animation can be stopped and restarted.
  
  Reimplementate trace() to get a better trace.
  
*/


class SpikeTrace : public QWidget, public QThread
{ 
  Q_OBJECT

public:

    /*! Constructs the SpikeTrace-widget 
        with the width of the spike set to \a spikewidth pixels,
	the radius of the ball set to \a radius pixels,
	and the width of the trace set to \a linewidth pixels. */
  SpikeTrace( double spikewidth=1.0, int radius=6, int tracewidth=1,
		QWidget *parent=0 );
    /*! Constructs the SpikeTrace-widget with default parameters. */
  SpikeTrace( QWidget *parent=0 );
    /*! Destructs the SpikeTrace-widget. */
  ~SpikeTrace( void );

    /*! Give a hint for the prefered size of this widget. */
  QSize sizeHint( void ) const;
    /*! Give a hint for the minimum size of this widget. */
  QSize minimumSizeHint( void ) const;
    /*! Make this widget stretchable in both directions. */
  QSizePolicy sizePolicy( void ) const;

    /*! Handles the resize event. 
        It scales the position of the spike and its size appropriately. */
  void resizeEvent( QResizeEvent *qre );
    /*! Paints the spike trace. */
  void paintEvent( QPaintEvent *qpe );


public slots:

    /*! Switch the spike trace on or off. */
  void setSpike( bool on );
    /*! Stop and start animation. */
  void setPause( bool pause );


protected:

  virtual void run( void );

    /*! Animates the spike trace by increasing the position of the ball
        and replace the spike on a new run. */
  void animate( void );

    /*! A function providing the shape of the trace. 
        For a given position \a x in pixels it returns the 
        corresponding y-value (in pixels). */
  int trace( int x );

    /*! If true, the trace is painted. */
  bool Show;
    /*! If true, no animation is done. */
  bool Pause;
    /*! Position of the ball in pixel. */
  int Pos;
    /*! Previous position of the ball in pixel. */
  int pPos;
    /*! Increment of the ball's position in pixel. */
  int dPos;
    /*! Radius of the ball. */
  int Radius;
    /*! Width of the trace line. */
  int TraceWidth;
    /*! The current size of the spike in pixel. */
  double SpikeSize;
    /*! The position of the spike. */
  double SpikePos;
    /*! The width of the spike. */
  double SpikeWidth;

    /*! Locks the widget's variables. */
  QMutex SMutex;

};


}; /* namespace relacs */

#endif /* ! _RELACS_SPIKETRACE_H_ */

