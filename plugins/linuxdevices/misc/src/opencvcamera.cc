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
    IntrinsicMatrix = Mat(3, 3, CV_64FC1);
    DistortionCoeffs = Mat(1, 5, CV_64FC1);

    Calibrated = false;
  }
  

  return 0;
}

bool OpenCVCamera::findChessboardCorners(const Mat& Image, const Size BoardSize, vector<Point2f>& Corners){
  bool found = cv::findChessboardCorners(Image, BoardSize, Corners, 
					 CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE);
  if (found){
    Mat GrayImage;
    cvtColor( Image, GrayImage, CV_BGR2GRAY );
    cornerSubPix(GrayImage, Corners, Size(11,11),Size(-1,-1),
		 TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS,30,0.1));
  }
  return found;
  
}

bool OpenCVCamera::findChessboardCorners(const Size BoardSize, vector<Point2f>& Corners){
  Mat Image = grabFrame();
  bool found = cv::findChessboardCorners(Image, BoardSize, Corners, 
					 CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE);
  if (found){
    Mat GrayImage;
    cvtColor( Image, GrayImage, CV_BGR2GRAY );
    cornerSubPix(GrayImage, Corners, Size(11,11),Size(-1,-1),
		 TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS,30,0.1));
  }
  return found;
  
}

Mat OpenCVCamera::findChessboard3D(const Mat ObjectPoints, const Mat Corners){
  return findChessboard3D(ObjectPoints, Corners, false);
}


Mat OpenCVCamera::findChessboard3D(const Mat ObjectPoints, const Mat Corners, bool undistort){
  /* The image points must be recorded with grabFrame */
  Mat rvec, tvec, R;
	  
  Mat d;
  if (undistort)
    d = DistortionCoeffs;
  else
    d = Mat::zeros(1,5,DistortionCoeffs.type());
  solvePnP(ObjectPoints, Corners, IntrinsicMatrix, d, rvec, tvec );

  Rodrigues(rvec, R);
  Mat O = ObjectPoints.clone().reshape(1); 
  R.convertTo(R, O.type());
  tvec.convertTo(tvec, O.type());


  Mat tmp = O*R.t();
  Mat tmp2;
  for (int j = 0; j != tmp.rows; ++j){
    tmp2 = tmp.row(j);
    tmp2 = tmp2 + tvec.t();
  }
  return tmp;
}

Mat OpenCVCamera::project(const Mat Points){
  Mat M = IntrinsicMatrix.clone();
  M.convertTo(M,Points.type());

  Mat tmp = (Points*M.t());
  Mat tmp2 = Mat::ones((int)Points.rows, 3, Points.type());

  for (int i = 0; i != tmp.rows; ++i){
    // tmp2.at<float>(i,0) = tmp.at<float>(i,0)/tmp.at<float>(i,2);
    // tmp2.at<float>(i,1) = tmp.at<float>(i,1)/tmp.at<float>(i,2);
    if (tmp2.type() == CV_32F){
      tmp2.at<float>(i,0) = tmp.at<float>(i,0)/tmp.at<float>(i,2);
      tmp2.at<float>(i,1) = tmp.at<float>(i,1)/tmp.at<float>(i,2);

    }else if(tmp2.type() == CV_64F){
      tmp2.at<double>(i,0) = tmp.at<double>(i,0)/tmp.at<double>(i,2);
      tmp2.at<double>(i,1) = tmp.at<double>(i,1)/tmp.at<double>(i,2);

    }else{
      cerr << "OpenCVCamera.project: Type not kown!" << endl;
    }
    
  }

  return tmp2;
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
		  rvecs, tvecs,0);
  //		  CV_CALIB_FIX_K1 + CV_CALIB_FIX_K2 + CV_CALIB_FIX_K3);
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
