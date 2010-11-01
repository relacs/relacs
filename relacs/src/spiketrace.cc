/*
  spiketrace.cc
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

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <QApplication>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QResizeEvent>
#include <relacs/spiketrace.h>

using namespace std;


namespace relacs {


SpikeTrace::SpikeTrace( double spikewidth, int radius, int tracewidth, 
			QWidget *parent )
  : QWidget( parent ),
    SMutex()
{
  SpikeWidth = spikewidth;
  Radius = radius;
  TraceWidth = tracewidth;
  SpikeSize = double( height() - Radius );
  SpikePos = 0.3*width();

  Show = false;
  Pause = false;
  Pos = Radius/2;
  dPos = 2;

  Thread = new SpikeTraceThread( this );
}


SpikeTrace::SpikeTrace( QWidget *parent )
  : QWidget( parent )
{
  SpikeWidth = 1.0;
  Radius = 6;
  TraceWidth = 1;
  SpikePos = 0.3*width();
  SpikeSize = double( height() - Radius );

  Show = false;
  Pause = false;
  Pos = Radius/2;
  pPos = Pos;
  dPos = 4;

  Thread = new SpikeTraceThread( this );
}


SpikeTrace::~SpikeTrace( void )
{
}


QSize SpikeTrace::sizeHint( void ) const 
{ 
  QSize qs( 16*Radius, 5*Radius ); 
  return qs; 
}


QSize SpikeTrace::minimumSizeHint( void ) const 
{ 
  QSize qs( 10*Radius, 3*Radius ); 
  return qs; 
}


QSizePolicy SpikeTrace::sizePolicy( void ) const 
{ 
  QSizePolicy qsp( QSizePolicy::Expanding, QSizePolicy::Expanding ); 
  return qsp; 
}


void SpikeTrace::resizeEvent( QResizeEvent *qre )
{
  SMutex.lock();
  SpikeSize = double( height() - Radius );
  SpikePos *= double( width() ) / double( qre->oldSize().width() );
  SpikePos = rint( SpikePos );
  SMutex.unlock();
}


int SpikeTrace::trace( int x )
{
  int y;

  y = int( SpikeSize*exp(-(x-SpikePos)*(x-SpikePos)/2.0/SpikeWidth/SpikeWidth) );
  return y;
}


void SpikeTrace::paintEvent( QPaintEvent *qpe )
{
  SMutex.lock();

  if ( Show ) {

    animate();

    int b = height() - Radius/2;
    
    // draw line:
    QPainterPath path;
    int x = 0;
    path.moveTo( x, b - trace( x ) );
    for ( x++; x<Pos; x++ )
      path.lineTo( x, b - trace( x ) ); 

    QPainter paint( this );
    paint.setPen( QPen( Qt::darkGreen, TraceWidth ) );
    paint.drawPath( path );
    
    // draw ball:
    paint.setPen( Qt::green );
    paint.setBrush( Qt::green );
    paint.drawEllipse( Pos - Radius/2, b - trace( Pos ) - Radius/2, 
		       Radius, Radius );
  }

  SMutex.unlock();
}


void SpikeTrace::setSpike( bool on )
{
  SMutex.lock();
  Show = on;
  Pos = Radius/2;
  pPos = Pos;
  SMutex.unlock();
  if ( on )
    Thread->start();
  else
    Thread->wait();
}


void SpikeTrace::setPause( bool pause )
{
  SMutex.lock();
  Pause = pause;
  SMutex.unlock();
}


void SpikeTrace::animate( void )
{
  pPos = Pos;
  Pos += dPos;
  if ( Pos >= width()-Radius/2 ) {
    Pos = Radius/2;
    double r = rand();
    r /= RAND_MAX;
    SpikePos = rint( 2*SpikeWidth + ( width() - 4*SpikeWidth ) * r );
  }
}


void SpikeTrace::run( void )
{
  bool showw = true;
  do {
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+1 ) ) );
    bool pausew = true;
    do {
      Thread->msleep( 50 );
      SMutex.lock();
      showw = Show;
      pausew = Pause;
      SMutex.unlock();
    } while ( showw && pausew );
  } while ( showw );
  QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+1 ) ) );
}


void SpikeTrace::customEvent( QEvent *qce )
{
  if ( qce->type() - QEvent::User == 1 )
    update();
  else
    QWidget::customEvent( qce );
}


SpikeTraceThread::SpikeTraceThread( SpikeTrace *s )
  : QThread( this ),
    S( s )
{
}


void SpikeTraceThread::run( void )
{
  S->run();
}


void SpikeTraceThread::msleep( unsigned long msecs )
{
  QThread::msleep( msecs );
}


}; /* namespace relacs */

#include "moc_spiketrace.cc"

