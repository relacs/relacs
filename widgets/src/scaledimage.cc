/*
  scaledimage.cc
  A label that displays an image scaled to the maximum available size.

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


#include <QResizeEvent>
#include <relacs/scaledimage.h>
#include <cmath>


namespace relacs {


ScaledImage::ScaledImage( QWidget *parent )
  : QLabel( parent )
{
  //setAutoFillBackground( true );
}


ScaledImage::ScaledImage( const QPixmap &image, QWidget *parent )
  : QLabel( parent )
{
  setContentsMargins( 0, 0, 0, 0 );
  setMinimumSize( 0, 0 );
  setImage( image );
  setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
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

  setContentsMargins(0, 0, 0, 0);
  setPixmap( Image.scaled( width() - 20, height() - 20,
			   Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
}

}; /* namespace relacs */

#include "moc_scaledimage.cc"

