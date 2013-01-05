/*
  scaledimage.cc
  A label that displays an image scaled to the maximum available size.

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


#include <QResizeEvent>
#include <relacs/scaledimage.h>

namespace relacs {


ScaledImage::ScaledImage( QWidget *parent )
  : QLabel( parent )
{
  //setAutoFillBackground( true );
}


ScaledImage::ScaledImage( const QPixmap &image, QWidget *parent )
  : QLabel( parent )
{
  setImage( image );
}


ScaledImage::ScaledImage( const QImage &image, QWidget *parent )
  : QLabel( parent )
{
  setImage( image );
}


void ScaledImage::setImage( const QPixmap &image )
{
  Image = image;
}


void ScaledImage::setImage( const QImage &image )
{
  Image = QPixmap::fromImage( image );
}


void ScaledImage::resizeEvent( QResizeEvent *event )
{
  if ( event->size().width() <= 0 || Image.width() <= 0 )
    return;

  if ( (double)event->size().height()/(double)event->size().width() > 
       (double)Image.height()/(double)Image.width() ) {
    setPixmap( Image.scaledToWidth( event->size().width()-2, Qt::SmoothTransformation ) );
  }
  else {
    setPixmap( Image.scaledToHeight( event->size().height()-2, Qt::SmoothTransformation ) );
  }
}

}; /* namespace relacs */

#include "moc_scaledimage.cc"

