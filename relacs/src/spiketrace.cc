/*
  spiketrace.cc
  A nice, almost useless widget, showing an animated trace of a spike.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <qapplication.h>
#include <qpainter.h>
#include <qapplication.h>
#include <relacs/spiketrace.h>

namespace relacs {


SpikeTrace::SpikeTrace( double spikewidth, int radius, int tracewidth, 
			QWidget *parent, const char *name )
  : QWidget( parent, name ), QThread()
{
  SpikeWidth = spikewidth;
  Radius = radius;
  TraceWidth = tracewidth;
  SpikeSize = double( height() - Radius );
  SpikePos = 0.3*width();

  Show = false;
  Pause = false;
  Pos = Radius/2;
  pPos = Pos;
  dPos = 4;
}


SpikeTrace::SpikeTrace( QWidget *parent, const char *name )
  : QWidget( parent, name )
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
  SpikeSize = double( height() - Radius );
  SpikePos *= double( width() ) / double( qre->oldSize().width() );
  SpikePos = rint( SpikePos );
}


int SpikeTrace::trace( int x )
{
  int y;

  y = int( SpikeSize*exp(-(x-SpikePos)*(x-SpikePos)/2.0/SpikeWidth/SpikeWidth) );
  return y;
}


void SpikeTrace::paintEvent( QPaintEvent *qpe )
{
  QPainter paint( this );

  if ( Show ) {

    animate();

    int b = height() - Radius/2;
    int x;
    
    // clear widget:
    if ( !qpe->erased() && Pos == Radius/2 )
      erase();
    else
      // clear ball:
      paint.eraseRect( pPos - Radius/2, b - trace( pPos ) - Radius/2, 
		       Radius, Radius );
    
    // draw line:
    paint.setPen( QPen( darkGreen, TraceWidth ) );
    x = qpe->erased() ? 0 : pPos-Radius/2-1;
    paint.moveTo( x, b - trace( x ) );
    for ( x++; x<Pos; x++ )
      paint.lineTo( x, b - trace( x ) ); 
    
    // draw ball:
    paint.setPen( Qt::green );
    paint.setBrush( Qt::green );
    paint.drawEllipse( Pos - Radius/2, b - trace( Pos ) - Radius/2, 
		       Radius, Radius );
  }
  else
    paint.eraseRect( rect() );
}


void SpikeTrace::setSpike( bool on )
{
  Show = on;
  Pos = Radius/2;
  pPos = Pos;
  if ( Show ) {
    start();
  }
  else {
    wait();
  }
}


void SpikeTrace::setPause( bool pause )
{
  Pause = pause;
}


void SpikeTrace::animate( void )
{
  pPos = Pos;
  Pos += dPos;
  if ( Pos >= width()-Radius/2 )
    {
      Pos = Radius/2;
      double r = rand();
      r /= RAND_MAX;
      SpikePos = rint( 2*SpikeWidth + ( width() - 4*SpikeWidth ) * r );
    }
}


void SpikeTrace::run( void )
{
  do {
    QApplication::postEvent( this, new QPaintEvent( rect(), false ) );
    do {
      msleep( 100 );
    } while ( Show && Pause );
  } while ( Show );
  QApplication::postEvent( this, new QPaintEvent( rect(), false ) );
}



}; /* namespace relacs */

#include "moc_spiketrace.cc"

