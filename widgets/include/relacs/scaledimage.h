/*
  scaledimage.h
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

#ifndef _RELACS_SCALEDIMAGE_H_
#define _RELACS_SCALEDIMAGE_H_ 1

#include <QLabel>

using namespace std;


namespace relacs {


/*! 
\class ScaledImage
\author Jan Benda
\brief A label that displays an image scaled to the maximum available size.
*/

class ScaledImage : public QLabel
{
  Q_OBJECT

public:

    /*! Constructs an empty ScaledImage. */
  ScaledImage( QWidget *parent=0 );
    /*! Constructs an ScaledImage with image \a image. */
  ScaledImage( const QPixmap &image, QWidget *parent=0 );
    /*! Constructs an ScaledImage with image \a image. */
  ScaledImage( const QImage &image, QWidget *parent=0 );

    /*! Set the image to be displayed to \a image. */
  void setImage( const QPixmap &image );
    /*! Set the image to be displayed to \a image. */
  void setImage( const QImage &image );


protected:

  virtual void resizeEvent( QResizeEvent *event );


private:

  QPixmap Image;

};


}; /* namespace relacs */

#endif /* ! _RELACS_SCALEDIMAGE_H_ */
