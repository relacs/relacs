/*
  camera/opencvcameracalibration.cc
  RePro for calibrating an OpenCVCamera

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
#include <iostream>
#include <relacs/camera/opencvcameracalibration.h>
using namespace relacs;

#define IMGWIDTH 400
#define IMGHEIGHT 400

namespace camera {


OpenCVCameraCalibration::OpenCVCameraCalibration( void )
  : RePro( "OpenCVCameraCalibration", "camera", "Fabian Sinz", "1.0", "Jun 01, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );

  addInteger("BoardWidth", 8); // number of interior points on the calibration board
  addInteger("BoardHeight", 6); // number of interior points on the calibration board
  addInteger("CalibrationFrames", 5); // number of snapshots taken for calibration
  addInteger("SkipFrames", 30); // number of frames to skip between snapshots
  addBoolean("EstimateDistortion",true); // whether to estimate DistortionCoeffs

  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *bb = new QHBoxLayout;

  setLayout( vb );
  vb->setSpacing( 4 );
  vb->addLayout(bb);


  ImgLabel = new QLabel;
  ImgLabel->setAlignment(Qt::AlignCenter);
  ImgLabel->setFixedSize ( IMGWIDTH, IMGHEIGHT );
  bb->addWidget(ImgLabel);

  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );

  QGridLayout *Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 2 );
  Positions->setVerticalSpacing( 2 );
  bb->addLayout( Positions );


  // position watch

  QLabel* label = new QLabel( "Calibration Frames Captured " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 0 );
 
  FrameLCD = new QLCDNumber( 3 );
  FrameLCD->setSegmentStyle( QLCDNumber::Filled );
  FrameLCD->setFixedHeight( label->sizeHint().height()*1.5 );

  FrameLCD->setPalette( qp );
  FrameLCD->setAutoFillBackground( true );
  Positions->addWidget( FrameLCD, 0, 1 );


}


void OpenCVCameraCalibration::timerEvent(QTimerEvent*)
{
    ImgLabel->setPixmap(QPixmap::fromImage(QtImg.scaled(IMGWIDTH, IMGHEIGHT,Qt::KeepAspectRatio)));  
    ImgLabel->show();
}


int OpenCVCameraCalibration::main( void )
{
  // get options:
  unlockData();



  int BoardWidth = integer("BoardWidth"); // Board width in squares
  int BoardHeight = integer("BoardHeight"); // Board height 
  int CalibrationFrames = integer("CalibrationFrames");
  int SkipFrames = integer("SkipFrames");

  int InteriorPoints = BoardWidth * BoardHeight;
  CvSize BoardSize = cvSize( BoardWidth, BoardHeight );

  CameraControl* CamContrl = 0;
  CamContrl = dynamic_cast< CameraControl* >( control("CameraControl") );
  if (CamContrl == 0 ) {
    cerr << "Could not get pointer to camera control" << endl;
    readLockData();
  
    return 1;
  }

  Cam = 0;
  Cam = dynamic_cast< misc::OpenCVCamera* >( device(CamContrl->currentCamera()) );
  if (Cam == 0 ) {
    cerr << "Could not get pointer to camera device " << CamContrl->currentCamera() << endl;
    readLockData();
  
    return 1;
  }
  

  Cam->setCalibrated(false); // set Camera to uncalibrated

  // lock camera control to stop it accessing the camera
  lockControl("CameraControl");
  int timer = startTimer(30); // start timer that causes timeEvent to be called which displays the image

  // get capture from camera object and take picture, QtImg will be displayed by timeEvent
  CvCapture* Capture = Cam->getCapture();
  assert( Capture );
  QtImg = misc::ConvertImage(cvQueryFrame(Capture));


  CvMat* ImagePoints = cvCreateMat( CalibrationFrames*InteriorPoints, 2, CV_32FC1 );
  CvMat* ObjectPoints = cvCreateMat( CalibrationFrames*InteriorPoints, 3, CV_32FC1 );
  CvMat* PointCounts = cvCreateMat( CalibrationFrames, 1, CV_32SC1 );


  vector<CvPoint2D32f*> FoundCorners;

  int CornerCount = 0;
  int Successes = 0;
  int Step, Frame = 0;
  int found = 0;

  IplImage *Image = cvQueryFrame( Capture );
  IplImage *GrayImage = cvCreateImage( cvGetSize( Image ), 8, 1 );

  // Capture Corner views loop until we've got CalibrationFrames
  // succesful captures (all corners on the board are found)

  while( Successes < CalibrationFrames ){
    if ( interrupt() ){
      readLockData();
      return Aborted;
    }

    // take pictures
    Image = cvQueryFrame( Capture );
 
    

    // Skp every SkipFrames frames to allow user to move chessboard
    if( Frame++ % SkipFrames == 0 ){
      CvPoint2D32f* Corners = new CvPoint2D32f[ InteriorPoints ];


      // Find chessboard corners:
      found = cvFindChessboardCorners( Image, BoardSize, Corners,
				       &CornerCount, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );
      
      if (found){
	// // Get subpixel accuracy on those corners
	cvCvtColor( Image, GrayImage, CV_BGR2GRAY );

      
	
	cvFindCornerSubPix( GrayImage, Corners, CornerCount, cvSize( 11, 11 ), 
			    cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

	FoundCorners.push_back(Corners);
	// Draw it
	for (vector<CvPoint2D32f*>::size_type i =0; i != FoundCorners.size(); ++i){
	  cvDrawChessboardCorners( Image, BoardSize, FoundCorners[i], CornerCount, found );
	}
	QtImg = misc::ConvertImage(Image);

	// If we got a good board, add it to our data
	if( CornerCount == InteriorPoints ){
	  Step = Successes * InteriorPoints;
	  for( int i=Step, j=0; j < InteriorPoints; ++i, ++j ){
	    CV_MAT_ELEM( *ImagePoints, float, i, 0 ) = Corners[j].x;
	    CV_MAT_ELEM( *ImagePoints, float, i, 1 ) = Corners[j].y;
	    CV_MAT_ELEM( *ObjectPoints, float, i, 0 ) = j / BoardWidth;
	    CV_MAT_ELEM( *ObjectPoints, float, i, 1 ) = j % BoardWidth;
	    CV_MAT_ELEM( *ObjectPoints, float, i, 2 ) = 0.0f;
	  }
	  CV_MAT_ELEM( *PointCounts, int, Successes, 0 ) = InteriorPoints;
	  Successes++;
	  FrameLCD->display(Successes);
	}
      } 
    }
    
  } // End collection while loop


  // Allocate matrices according to how many chessboards found
  CvMat* ObjectPoints2 = cvCreateMat( Successes * InteriorPoints, 3, CV_32FC1 );
  CvMat* ImagePoints2 = cvCreateMat( Successes * InteriorPoints, 2, CV_32FC1 );
  CvMat* PointCounts2 = cvCreateMat( Successes, 1, CV_32SC1 );
	
  // Transfer the points into the correct size matrices
  for( int i = 0; i < Successes*InteriorPoints; ++i ){
    CV_MAT_ELEM( *ImagePoints2, float, i, 0) = CV_MAT_ELEM( *ImagePoints, float, i, 0 );
    CV_MAT_ELEM( *ImagePoints2, float, i, 1) = CV_MAT_ELEM( *ImagePoints, float, i, 1 );
    CV_MAT_ELEM( *ObjectPoints2, float, i, 0) = CV_MAT_ELEM( *ObjectPoints, float, i, 0 );
    CV_MAT_ELEM( *ObjectPoints2, float, i, 1) = CV_MAT_ELEM( *ObjectPoints, float, i, 1 );
    CV_MAT_ELEM( *ObjectPoints2, float, i, 2) = CV_MAT_ELEM( *ObjectPoints, float, i, 2 );
  }
  
  for( int i=0; i < Successes; ++i ){
    CV_MAT_ELEM( *PointCounts2, int, i, 0 ) = CV_MAT_ELEM( *PointCounts, int, i, 0 );
  }
  cvReleaseMat( &ObjectPoints );
  cvReleaseMat( &ImagePoints );
  cvReleaseMat( &PointCounts );

  // calibrate Camera
  Cam->calibrate( ObjectPoints2, ImagePoints2, PointCounts2, cvGetSize( Image ), boolean("EstimateDistortion"));

  // kill timer to stop display
  killTimer(timer);
  // unlock camera control
  unlockControl("CameraControl");


  readLockData();
  FrameLCD->display(0);

  // release corner arrays
  for (vector<CvPoint2D32f*>::size_type i =0; i != FoundCorners.size(); ++i){
    delete  FoundCorners[i];
  }
  
  return Completed;
}


addRePro( OpenCVCameraCalibration, camera );

}; /* namespace camera */

#include "moc_opencvcameracalibration.cc"
