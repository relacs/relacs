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

#define IMGWIDTH 600
#define IMGHEIGHT 600
#define INVFRAMERATE 30
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
  addNumber("SquareWidth", 24, "Chess board square width in mm");
  addNumber("SquareHeight", 23, "Chess board square height in mm");

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
  QLabel* label = new QLabel( "Next capture in  " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 0 );

  CountLCD = new QLCDNumber( 3 );
  CountLCD->setSegmentStyle( QLCDNumber::Filled );
  CountLCD->setFixedHeight( label->sizeHint().height()*1.5 );
  CountLCD->setFixedWidth( 50 );

  CountLCD->setPalette( qp );
  CountLCD->setAutoFillBackground( true );
  Positions->addWidget( CountLCD, 0, 1 );

  label = new QLabel( "Calibration Frames Captured " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 1, 0 );
 
  FrameLCD = new QLCDNumber( 3 );
  FrameLCD->setSegmentStyle( QLCDNumber::Filled );
  FrameLCD->setFixedHeight( label->sizeHint().height()*1.5 );
  FrameLCD->setFixedWidth( 50 );

  FrameLCD->setPalette( qp );
  FrameLCD->setAutoFillBackground( true );
  Positions->addWidget( FrameLCD, 1, 1 );


  // a few other parameters
  disableStream = false;


}


void OpenCVCameraCalibration::timerEvent(QTimerEvent*)
{
  if (!disableStream){
    Capture >> TmpImg ;
    if (FoundCorners.size() > 0){
      	for (vector< Mat >::size_type i =0; i != FoundCorners.size(); ++i){
      	  drawChessboardCorners( TmpImg, BoardSize, FoundCorners[i], found );
      	}

    }

    QtImg = misc::Mat2QImage(TmpImg);
    ImgLabel->setPixmap(QPixmap::fromImage(QtImg.scaled(IMGWIDTH, IMGHEIGHT,Qt::KeepAspectRatio)));  
    ImgLabel->show();
  }
}


int OpenCVCameraCalibration::main( void )
{
  // get options:
  unlockData();

  int BoardWidth = integer("BoardWidth"); // Board width in squares
  int BoardHeight = integer("BoardHeight"); // Board height 
  int CalibrationFrames = integer("CalibrationFrames");
  int SkipFrames = integer("SkipFrames");
  double squareWidth = number("SquareWidth");
  double squareHeight = number("SquareHeight");

  int InteriorPoints = BoardWidth * BoardHeight;
  BoardSize = Size( BoardWidth, BoardHeight );
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
  CamContrl->disable();
  lockControl("CameraControl");
  

  
  // get capture from camera object and take picture
  Capture = Cam->getCapture();

  int timer = startTimer(INVFRAMERATE); // start timer that causes timeEvent to be called which displays the image

  vector< vector<Point3f> > ObjectPoints;
  vector< vector<Point2f> > ImagePoints;
  vector<Point2f> Corners;

  int Successes = 0, Frame = 0;
  found = false;

  Mat Image, GrayImage;
  Capture >> Image;
  cvtColor(Image, GrayImage, CV_BGR2GRAY);

  // // Capture Corner views loop until we've got CalibrationFrames
  // // succesful captures (all corners on the board are found)

  while( Successes < CalibrationFrames ){
    if ( interrupt() ){
      readLockData();
      return Aborted;
    }

    CountLCD->display(SkipFrames - Frame++ % SkipFrames );
    usleep(1000000/INVFRAMERATE);
 
    

    // Skp every SkipFrames frames to allow user to move chessboard
    if( Frame % SkipFrames == 0 ){
      

      disableStream = true;
      usleep(10000);
      Capture >> Image;
      disableStream = false;

      // Find chessboard corners:
      found = findChessboardCorners(Image, BoardSize, Corners, 
				    CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE);
      //CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
      if (found){
      	// Get subpixel accuracy on those corners
      	cvtColor( Image, GrayImage, CV_BGR2GRAY );
   
	cornerSubPix(GrayImage, Corners, Size(11,11),Size(-1,-1),
	     TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS,30,0.1));
	//	vector<Point2f> tmp = Corners;
	FoundCorners.push_back(Mat(Corners,true)); // true is for copying data

	// If we got a good board, add it to our data

	vector<Point3f> obj;
	for(int j=0;j<InteriorPoints;++j)
	  obj.push_back(Point3f(squareHeight * (float)(j/BoardWidth), squareWidth*(float)(j%BoardWidth), 0.0f));
	ImagePoints.push_back(vector<Point2f>(Corners));
	ObjectPoints.push_back(obj);

	Successes++;
	FrameLCD->display(Successes);
      } 
 
    }
    
  } // End collection while loop


  // // calibrate Camera
  //Cam->calibrate( ObjectPoints2, ImagePoints2, PointCounts2, cvGetSize( Image ), boolean("EstimateDistortion"));
  Cam->calibrate(ObjectPoints, ImagePoints, Image.size() );
  printlog("Calibration succeeded!");
  // // kill timer to stop display
  killTimer(timer);
  // unlock camera control
  unlockControl("CameraControl");
  CamContrl->startStream();


  readLockData();
  FrameLCD->display(0);
  return Completed;
}


addRePro( OpenCVCameraCalibration, camera );

}; /* namespace camera */

#include "moc_opencvcameracalibration.cc"
