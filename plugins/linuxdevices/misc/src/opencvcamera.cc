/*
  misc/opencvcamera.cc
  The Opencvcamera module linear robot from MPH

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
#include <relacs/misc/opencvcamera.h>


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


OpenCVCamera::OpenCVCamera( void )
  : Camera( "OpenCVCamera" ){
  Opened = false;
  Calibrated = false;
}



OpenCVCamera::~OpenCVCamera( void )
{
  close();
  Opened = false;
}


int OpenCVCamera::open( const string &device, const Options &opts )
{
  Opened = true;
  calibFile = opts.text( "calibfile" );
  cameraNo = atoi(opts.text("device").c_str());
  source = cvCaptureFromCAM( cameraNo );
  
  return 0;
}


void OpenCVCamera::close( void )
{
  Opened = false;
  // Info.clear();
  // Settings.clear();
  cvReleaseCapture(&source);

}


int OpenCVCamera::reset( void )
{
  return 0;
}

IplImage* OpenCVCamera::grabFrame(void){
 return cvQueryFrame(source); 
}

QImage OpenCVCamera::grabQImage(void){
  return ConvertImage(grabFrame());
}



}; /* namespace misc */
