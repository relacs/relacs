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
using namespace cv;

namespace misc {

QImage ConvertImage( IplImage *Source){
    // Bilddaten übernehmen
    unsigned char * data = ( unsigned char * ) Source->imageData;

    // QImage mit Originaldaten erstellen
    QImage ret( data, Source->width, Source->height, QImage::Format_RGB888 );

    // Kanäle (BGR -> RGB) und Format (RGB888 -> RGB32) ändern
    return ret.rgbSwapped().convertToFormat( QImage::Format_RGB32 );
}

QImage Mat2QImage(const cv::Mat src){
  QImage qtFrame(src.data, src.size().width, src.size().height, src.step, QImage::Format_RGB888);
  qtFrame = qtFrame.rgbSwapped();

  return qtFrame;
}

/*************************************************************************/


OpenCVCamera::OpenCVCamera( void )
  : Camera( "OpenCVCamera" ){
  Opened = false;
  Calibrated = false;

  EstimateDistortion = true;

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
  Source = VideoCapture(CameraNo);

  Info.addInteger("device",CameraNo);
 
  ParamFile =  opts.text( "parameters" );
  Info.addText("parameters", ParamFile);
  
  FileStorage fs;
  if (fs.open(ParamFile, FileStorage::READ)){
    fs["intrinsic"] >> IntrinsicMatrix;
    fs["distortion"] >> DistortionCoeffs;
    fs.release();
    Calibrated = true;
    Mat Image;
    Source >> Image;
   
    recomputeUndistortionMaps();


  }else{
    IntrinsicMatrix = Mat(3, 3, CV_32FC1);

    Calibrated = false;
  }
  

  return 0;
}


  void OpenCVCamera::close( void ){
    Opened = false;
    Info.clear();
    Settings.clear();
    Source.release();
  }
  
  int OpenCVCamera::calibrate(vector < vector<Point3f> > ObjectPoints, 
			      vector< vector<Point2f> > ImagePoints, 
			      Size sz){
    
    vector<Mat> rvecs;
    vector<Mat> tvecs;
    calibrateCamera(ObjectPoints, ImagePoints, sz, 
		    IntrinsicMatrix, DistortionCoeffs, 
		    rvecs, tvecs,
		    CV_CALIB_FIX_K1 + CV_CALIB_FIX_K2 + CV_CALIB_FIX_K3);
    saveParameters();   
    recomputeUndistortionMaps();
    Calibrated = true;

    return 0;
  }


void OpenCVCamera::saveParameters(void){
  cerr << "OpenCVCamera: Saving parameters to " << ParamFile << endl;

  FileStorage fs(ParamFile, FileStorage::WRITE);
  fs << "intrinsic" << IntrinsicMatrix;
  fs << "distortion" << DistortionCoeffs;
  fs.release();
}


void OpenCVCamera::recomputeUndistortionMaps(void){
  if (Calibrated){
    Mat Image;
    Source >> Image;


    UDMapX = Mat( Image.size() ,CV_32FC1, 1 );
    UDMapY = Mat( Image.size() , CV_32FC1, 1 );
    initUndistortRectifyMap( IntrinsicMatrix, DistortionCoeffs, 
			     Mat::eye(3,3, CV_32F), IntrinsicMatrix, Image.size(),
			     CV_32FC1, UDMapX, UDMapY );

   
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


Mat OpenCVCamera::grabFrame(bool undistort){

  if (Opened){
    Mat Image;
    Source >> Image;
    if (Calibrated && undistort){
      Mat t = Image.clone();
      remap( t, Image, UDMapX, UDMapY, INTER_NEAREST,BORDER_CONSTANT, 0 );
      return Image; 
    }
    return Image;
  }
  return Mat();

}


Mat OpenCVCamera::grabFrame(void){
  return grabFrame(true);
}

QImage OpenCVCamera::grabQImage(void){
  if (Opened)
    return Mat2QImage(grabFrame());
  return QImage();
}



}; /* namespace misc */
