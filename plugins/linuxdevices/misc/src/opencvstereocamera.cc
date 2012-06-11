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


using namespace std;
using namespace relacs;

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
  
  FundamentalMatrixFile = opts.text( "fundamental" );
  Info.addText("fundamental", FundamentalMatrixFile);

  EssentialMatrixFile = opts.text( "essential" );
  Info.addText("essential", EssentialMatrixFile);

  RotationMatrixFile = opts.text( "rotation" );
  Info.addText("rotation", RotationMatrixFile);

  TranslationMatrixFile = opts.text( "translation" );
  Info.addText("translation", TranslationMatrixFile);


  IntrinsicFileLeft = opts.text( "intrinsicleft" );
  Info.addText("intrinsicleft", IntrinsicFileLeft);

  DistortionFileLeft = opts.text( "distortionleft" );
  Info.addText("distortionleft", DistortionFileLeft);
  

  IntrinsicFileRight = opts.text( "intrinsicright" );
  Info.addText("intrinsicright", IntrinsicFileRight);

  DistortionFileRight = opts.text( "distortionright" );
  Info.addText("distortionright", DistortionFileRight);
 

  // load all intrinsics and fundamental parameters 
  // if they all exist set to calibrated, else set to uncalibrated

  EssentialMatrix = (CvMat*)cvLoad(EssentialMatrixFile.c_str());
  FundamentalMatrix = (CvMat*)cvLoad(FundamentalMatrixFile.c_str());
  

  if (EssentialMatrix && FundamentalMatrix && RotationMatrix && TranslationMatrix){
    Calibrated = true;
  }else{
    EssentialMatrix = cvCreateMat( 3, 3, CV_32FC1 );
    FundamentalMatrix = cvCreateMat( 3, 3, CV_32FC1 );
    RotationMatrix = cvCreateMat( 3, 3, CV_32FC1 );
    TranslationMatrix = cvCreateMat( 3, 1, CV_32FC1 );
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


  
void OpenCVStereoCamera::calibrate(CvMat* ObjectPoints,CvMat* ImagePoints[], 
				   CvMat* PointCounts, CvSize ImgSize){

  IntrinsicMatrix[0] = (CvMat*)cvLoad(IntrinsicFileLeft.c_str());
  DistortionCoeffs[0] = (CvMat*)cvLoad(DistortionFileLeft.c_str());

  IntrinsicMatrix[1] = (CvMat*)cvLoad(IntrinsicFileRight.c_str());
  DistortionCoeffs[1] = (CvMat*)cvLoad(DistortionFileRight.c_str());
  
  EssentialMatrix = cvCreateMat( 3, 3, CV_32FC1 );
  FundamentalMatrix = cvCreateMat( 3, 3, CV_32FC1 );
  RotationMatrix = cvCreateMat( 3, 3, CV_32FC1 );
  TranslationMatrix = cvCreateMat( 3, 1, CV_32FC1 );


  if (IntrinsicMatrix[0] && IntrinsicMatrix[1] && DistortionCoeffs[0] && DistortionCoeffs[1]){
    cvStereoCalibrate(ObjectPoints, ImagePoints[0], ImagePoints[1], PointCounts, 
		      IntrinsicMatrix[0], DistortionCoeffs[0], 
		      IntrinsicMatrix[1], DistortionCoeffs[1], 
		      ImgSize, RotationMatrix, TranslationMatrix, 
		      EssentialMatrix, FundamentalMatrix, 
		      cvTermCriteria(  CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 30, 1e-6), 
		      CV_CALIB_FIX_INTRINSIC);
    saveParameters();
  }else{
    cerr << "Stereocalibration Failed! You need to calibrate your cameras first!" << endl;
  }
}



int OpenCVStereoCamera::reset( void ){
  return 0;
}

void OpenCVStereoCamera::setCalibrated(bool toWhat){
  Calibrated = toWhat;
}


void OpenCVStereoCamera::saveParameters(void){
  cerr << "OpenCVStereoCamera: Saving parameters to " << EssentialMatrixFile << 
    " and " << FundamentalMatrixFile << 
    " and " << RotationMatrixFile << 
    " and " << TranslationMatrixFile << endl;
  cvSave(EssentialMatrixFile.c_str(), EssentialMatrix );
  cvSave(FundamentalMatrixFile.c_str(), FundamentalMatrix );
  cvSave(RotationMatrixFile.c_str(), RotationMatrix  );
  cvSave(TranslationMatrixFile.c_str(), TranslationMatrix );
}




}; /* namespace misc */
