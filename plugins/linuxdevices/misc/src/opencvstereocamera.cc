/*
  misc/opencvstereocamera.cc
  The Opencvstereocamera module linear robot from MPH

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
#include <relacs/misc/opencvstereocamera.h>

#define length(a) ( sizeof ( a ) / sizeof ( *a ) )

using namespace std;
using namespace relacs;
using namespace cv;

namespace misc {


/*************************************************************************/


OpenCVStereoCamera::OpenCVStereoCamera( void )
  : Camera( "OpenCVStereoCamera" ){
  Opened = false;
}



OpenCVStereoCamera::~OpenCVStereoCamera( void )
{
  close();
  Opened = false;
}




int OpenCVStereoCamera::open( const string &device, const Options &opts )
{
  Info.clear();
  Settings.clear();

  Opened = true;
  // // load camera number
  // CameraNo = atoi(opts.text("device").c_str());
  // Source = cvCaptureFromCAM( CameraNo );
  
  // Info.addInteger("device",CameraNo);
 
  LeftCamDevice =  opts.text( "leftcam" );
  Info.addText("leftcam", LeftCamDevice);

  RightCamDevice =  opts.text( "rightcam" );
  Info.addText("rightcam", RightCamDevice);
  
  ParamFile = opts.text( "parameters" );
  Info.addText("parameters", ParamFile);

  LeftParamFile = opts.text("leftparam");
  Info.addText("leftparam", LeftParamFile);

  RightParamFile = opts.text("rightparam");
  Info.addText("rightparam", RightParamFile);

  // load all intrinsics and fundamental parameters 
  // if they all exist set to calibrated, else set to uncalibrated
  FileStorage fs;
  if (fs.open(ParamFile, FileStorage::READ)){
    fs["rotation"] >> RotationMatrix;
    fs["essential"] >> EssentialMatrix;
    fs["fundamental"] >> FundamentalMatrix;
    fs["translation"] >> TranslationMatrix;
    fs.release();
    Calibrated = true;

  }else{
    EssentialMatrix = Mat(3, 3, CV_32FC1);
    FundamentalMatrix =  Mat(3, 3, CV_32FC1);
    RotationMatrix  = Mat(3, 3, CV_32FC1);
    TranslationMatrix = Mat(3, 1, CV_32FC1);

    Calibrated = false;
  }
 


  return 0;
}


void OpenCVStereoCamera::close( void ){
  Opened = false;
  Info.clear();
  Settings.clear();

}





int OpenCVStereoCamera::calibrate(void){
  return 0;
}


int OpenCVStereoCamera::calibrate(vector< vector<Point3f> > ObjectPoints, 
				  vector< vector<Point2f> > ImagePoints[], Size sz){

  FileStorage fs;
  if (fs.open(LeftParamFile, FileStorage::READ)){
    fs["intrinsic"] >> IntrinsicMatrix[0];
    fs["distortion"] >> DistortionCoeffs[0];
    fs.release();
  }
  if (fs.open(RightParamFile, FileStorage::READ)){
    fs["intrinsic"] >> IntrinsicMatrix[1];
    fs["distortion"] >> DistortionCoeffs[1];
    fs.release();
  }


  stereoCalibrate(ObjectPoints, ImagePoints[0], ImagePoints[1], IntrinsicMatrix[0], DistortionCoeffs[0], 
		  IntrinsicMatrix[0], DistortionCoeffs[0], sz, RotationMatrix, TranslationMatrix, 
		  EssentialMatrix, FundamentalMatrix, 
		  TermCriteria(TermCriteria::COUNT+  TermCriteria::EPS, 30, 1e-6), CALIB_FIX_INTRINSIC);

  saveParameters();

  return 0;
}
  
  
// void OpenCVStereoCamera::calibrate(CvMat* ObjectPoints,CvMat* ImagePoints[], 
// 				   CvMat* PointCounts, CvSize ImgSize){



//     // calibration quality check using Epipolar constraint
//     int N = length(ObjectPoints);
//     CvMat L1 = cvMat(N, 3, CV_32F); 
//     CvMat L2 = cvMat(N, 3, CV_32F);
//     // //Always work in undistorted space 
//     // cvUndistortPoints( ImagePoints[0], ImagePoints[0], IntrinsicMatrix[0], 
//     // 		       DistortionCoeffs[0], 0, IntrinsicMatrix[0] );
//     // cvUndistortPoints( ImagePoints[1], ImagePoints[1], IntrinsicMatrix[1], 
//     // 		       DistortionCoeffs[1], 0, IntrinsicMatrix[1] );
//     // cvComputeCorrespondEpilines( ImagePoints[0], 1, FundamentalMatrix, &L1 ); 
//     // cvComputeCorrespondEpilines( ImagePoints[1], 2, FundamentalMatrix, &L2 ); 
//     // double avgErr = 0, err = 0; 
//     // for(int i = 0; i < N; i++ ) {
//     //   err = fabs(CV_MAT_ELEM(*(ImagePoints[0]), float, i, 0) * CV_MAT_ELEM(L2,float,i,0)
//     // 		 + CV_MAT_ELEM(*(ImagePoints[0]), float, i, 1) * CV_MAT_ELEM(L2, float, i,1) 
//     // 		 + CV_MAT_ELEM(L2, float, i,2));
//     //   err += fabs(CV_MAT_ELEM(*(ImagePoints[1]), float, i, 0) * CV_MAT_ELEM(L1, float, i,0)
//     // 		  + CV_MAT_ELEM(*(ImagePoints[1]), float, i, 1) * CV_MAT_ELEM(L1, float, i,1) 
//     // 		  + CV_MAT_ELEM(L1, float, i,2));
//     //   avgErr += err; 
//     // } 
//     // cerr << "Stereo Calibration Average Epipolar Error is: " <<  avgErr/N << endl ;

//     // save calibration results
//     saveParameters();
//   }else{
//     cerr << "Stereocalibration Failed! You need to calibrate your cameras first!" << endl;
//   }
// }



int OpenCVStereoCamera::reset( void ){
  return 0;
}

void OpenCVStereoCamera::setCalibrated(bool toWhat){
  Calibrated = toWhat;
}

void OpenCVStereoCamera::saveParameters(void){
  cerr << "OpenCVStereoCamera: Saving parameters to " << ParamFile << endl;

  FileStorage fs(ParamFile, FileStorage::WRITE);
  fs << "fundamental" << FundamentalMatrix;
  fs << "essential" << EssentialMatrix;
  fs << "rotation" << RotationMatrix;
  fs << "translation" << TranslationMatrix;
  fs.release();
}





}; /* namespace misc */
