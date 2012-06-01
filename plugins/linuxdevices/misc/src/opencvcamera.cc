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

QImage ConvertImage( IplImage *Source){
    // Bilddaten übernehmen
    unsigned char * data = ( unsigned char * ) Source->imageData;

    // QImage mit Originaldaten erstellen
    QImage ret( data, Source->width, Source->height, QImage::Format_RGB888 );

    // Kanäle (BGR -> RGB) und Format (RGB888 -> RGB32) ändern
    return ret.rgbSwapped().convertToFormat( QImage::Format_RGB32 );
}

/*************************************************************************/


OpenCVCamera::OpenCVCamera( void )
  : Camera( "OpenCVCamera" ){
  Opened = false;
  Calibrated = false;

  IntrinsicMatrix = NULL;
  DistortionCoeffs = NULL;

  EstimateDistortion = true;

  UDMapX = UDMapY = NULL;
}



OpenCVCamera::~OpenCVCamera( void )
{
  close();
  Opened = false;
}




int OpenCVCamera::open( const string &device, const Options &opts )
{
  Info.clear();
  Settings.clear();

  Opened = true;
  
  // load camera number
  CameraNo = atoi(opts.text("device").c_str());
  Source = cvCaptureFromCAM( CameraNo );
  Info.addInteger("device",CameraNo);
 
  IntrinsicFile =  opts.text( "intrinsic" );
  Info.addText("intrinsic", IntrinsicFile);
  
  DistortionFile = opts.text( "distortion" );
  Info.addText("distortion", DistortionFile);
  
  IntrinsicMatrix = (CvMat*)cvLoad(IntrinsicFile.c_str());
  DistortionCoeffs = (CvMat*)cvLoad(DistortionFile.c_str());
  
  if (IntrinsicMatrix && DistortionCoeffs){
    Calibrated = true;

    IplImage *Image = cvQueryFrame( Source );
    UDMapX = cvCreateImage( cvGetSize( Image ), IPL_DEPTH_32F, 1 );
    UDMapY = cvCreateImage( cvGetSize( Image ), IPL_DEPTH_32F, 1 );
    cvInitUndistortMap( IntrinsicMatrix, DistortionCoeffs, UDMapX, UDMapY );

  }else{
    IntrinsicMatrix = cvCreateMat( 3, 3, CV_64FC1 );
    DistortionCoeffs = cvCreateMat( 5, 1, CV_64FC1 );
    Calibrated = false;
  }
  

  return 0;
}


void OpenCVCamera::close( void )
{
  Opened = false;
  // Info.clear();
  // Settings.clear();
  cvReleaseCapture(&Source);

}


int OpenCVCamera::calibrate(CvMat* ObjectPoints2, CvMat*  ImagePoints2,CvMat* PointCounts2, CvSize ImgSize, 
			    bool estDist){
  CV_MAT_ELEM( *IntrinsicMatrix, float, 0, 0 ) = 1.0;
  CV_MAT_ELEM( *IntrinsicMatrix, float, 1, 1 ) = 1.0;
 
  if (estDist){
    cvCalibrateCamera2( ObjectPoints2, ImagePoints2, PointCounts2, ImgSize, 
  			IntrinsicMatrix, DistortionCoeffs, NULL, NULL,0 );
  }else{

    CV_MAT_ELEM( *DistortionCoeffs, float, 0, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 1, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 2, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 3, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 4, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 5, 0 ) = 0.0;

    cvCalibrateCamera2( ObjectPoints2, ImagePoints2, PointCounts2, ImgSize, 
    			IntrinsicMatrix, DistortionCoeffs, NULL, NULL, 
    			CV_CALIB_FIX_K1 + CV_CALIB_FIX_K2 + CV_CALIB_FIX_K3);

    CV_MAT_ELEM( *DistortionCoeffs, float, 0, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 1, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 2, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 3, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 4, 0 ) = 0.0;
    CV_MAT_ELEM( *DistortionCoeffs, float, 5, 0 ) = 0.0;

  }
  
  saveParameters();
  recomputeUndistortionMaps();
  Calibrated = true;
  return 0;
}

void OpenCVCamera::saveParameters(void){
  cerr << "OpenCVCamera: Saving parameters to " << IntrinsicFile << " and "
       << DistortionFile << endl;
  cvSave(IntrinsicFile.c_str(), IntrinsicMatrix );
  cvSave(DistortionFile.c_str(), DistortionCoeffs );
}


void OpenCVCamera::recomputeUndistortionMaps(void){
  if (Calibrated){
    IplImage *Image = cvQueryFrame( Source );
    UDMapX = cvCreateImage( cvGetSize( Image ), IPL_DEPTH_32F, 1 );
    UDMapY = cvCreateImage( cvGetSize( Image ), IPL_DEPTH_32F, 1 );
    cvInitUndistortMap( IntrinsicMatrix, DistortionCoeffs, UDMapX, UDMapY );
  }else{
    cerr << "Camera needs to be calibrated to recompute undistortion maps" << endl;
  }
}

int OpenCVCamera::reset( void ){
  return 0;
}

void OpenCVCamera::setCalibrated(bool toWhat){
  Calibrated = toWhat;
}

IplImage* OpenCVCamera::grabFrame(void){
  if (Calibrated){
    IplImage *Image = cvQueryFrame( Source );
    IplImage *t = cvCloneImage( Image );
    cvRemap( t, Image, UDMapX, UDMapY );
    cvReleaseImage( &t );
    return Image; 
  }

  return cvQueryFrame( Source );

}

QImage OpenCVCamera::grabQImage(void){
  return ConvertImage(grabFrame());
}



}; /* namespace misc */
