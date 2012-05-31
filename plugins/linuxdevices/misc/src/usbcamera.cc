/*
  misc/usbcamera.cc
  The Usbcamera module linear robot from MPH

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

// #include <sys/types.h>
// #include <sys/stat.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <cmath>
#include <cstdio>
// #include <cstring>
#include <iostream>
#include <relacs/misc/usbcamera.h>


using namespace std;
using namespace relacs;

namespace misc {


QImage ConvertImage( IplImage *source)
{
    // Bilddaten übernehmen
    unsigned char * data = ( unsigned char * ) source->imageData;

    // QImage mit Originaldaten erstellen
    QImage ret( data, source->width, source->height, QImage::Format_RGB888 );

    // Kanäle (BGR -> RGB) und Format (RGB888 -> RGB32) ändern
    return ret.rgbSwapped().convertToFormat( QImage::Format_RGB32 );
}

/*************************************************************************/


USBCamera::USBCamera( void )
  : Camera( "USBCamera" ){
  Opened = false;
  Calibrated = false;
}



USBCamera::~USBCamera( void )
{
  close();
  Opened = false;
}


int USBCamera::open( const string &device, const Options &opts )
{
  Opened = true;
  calibFile = opts.text( "calibfile" );
  cameraNo = atoi(opts.text("device").c_str());
  source = cvCaptureFromCAM( cameraNo );
  
  return 0;
}


void USBCamera::close( void )
{
  Opened = false;
  // Info.clear();
  // Settings.clear();
  cvReleaseCapture(&source);

}


int USBCamera::reset( void )
{
  return 0;
}

IplImage* USBCamera::grabFrame(void){
 return cvQueryFrame(source); 
}

QImage USBCamera::grabQImage(void){
  return ConvertImage(grabFrame());
}



}; /* namespace misc */
