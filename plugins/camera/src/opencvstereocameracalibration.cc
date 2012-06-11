/*
  camera/opencvstereocameracalibration.cc
  RePro to calibrate a stereo camera

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

#include <relacs/camera/opencvstereocameracalibration.h>
using namespace relacs;

#define IMGWIDTH 400
#define IMGHEIGHT 400
#define INVFRAMERATE 30

namespace camera {


OpenCVStereoCameraCalibration::OpenCVStereoCameraCalibration( void )
  : RePro( "OpenCVStereoCameraCalibration", "camera", "Fabian Sinz", "1.0", "Jun 08, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addInteger("BoardWidth", 8, "number of interior points on the calibration board"); // 
  addInteger("BoardHeight", 6, "number of interior points on the calibration board"); // 
  addInteger("CalibrationFrames", 10, "number of snapshots taken for calibration"); // 
  addInteger("SkipFrames", 100, "number of frames to skip between snapshots"); 
  addNumber("SquareWidth", 24, "Chess board square width in mm");
  addNumber("SquareHeight", 23, "Chess board square height in mm");
  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *bb = new QHBoxLayout;

  setLayout( vb );
  vb->setSpacing( 4 );
  vb->addLayout(bb);


  ImgLabel[0] = new QLabel;
  ImgLabel[0]->setAlignment(Qt::AlignCenter);
  ImgLabel[0]->setFixedSize ( IMGWIDTH, IMGHEIGHT );
  bb->addWidget(ImgLabel[0]);

  ImgLabel[1] = new QLabel;
  ImgLabel[1]->setAlignment(Qt::AlignCenter);
  ImgLabel[1]->setFixedSize ( IMGWIDTH, IMGHEIGHT );
  bb->addWidget(ImgLabel[1]);


  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );

  QGridLayout *Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 2 );
  Positions->setVerticalSpacing( 2 );
  vb->addLayout( Positions );


  // position watch
  QLabel* label = new QLabel( "Next capture in  " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 0 );

  CountLCD = new QLCDNumber( 3 );
  CountLCD->setSegmentStyle( QLCDNumber::Filled );
  CountLCD->setFixedHeight( label->sizeHint().height()*1.5 );

  CountLCD->setPalette( qp );
  CountLCD->setAutoFillBackground( true );
  Positions->addWidget( CountLCD, 0, 1 );

  label = new QLabel( "Calibration Frames Captured " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 2 );
 
  FrameLCD = new QLCDNumber( 3 );
  FrameLCD->setSegmentStyle( QLCDNumber::Filled );
  FrameLCD->setFixedHeight( label->sizeHint().height()*1.5 );

  FrameLCD->setPalette( qp );
  FrameLCD->setAutoFillBackground( true );
  Positions->addWidget( FrameLCD, 0, 3 );

  disableStream = false;

}

void OpenCVStereoCameraCalibration::timerEvent(QTimerEvent*){
  if (!disableStream){
    for (int i = 0; i != 2; ++i){
      ImgTmp = cvQueryFrame(Capture[i]);

      if (FoundCorners[i].size() > 0){
	// Draw it
	for (vector<CvPoint2D32f*>::size_type j =0; j != FoundCorners[i].size(); ++j){
	  cvDrawChessboardCorners( ImgTmp, BoardSize, FoundCorners[i][j], CornerCount[i], true );
	}
      }
      
      QtImg[i] = misc::ConvertImage(ImgTmp);
      ImgLabel[i]->setPixmap(QPixmap::fromImage(QtImg[i].scaled(IMGWIDTH, IMGHEIGHT,Qt::KeepAspectRatio)));  
      ImgLabel[i]->show();


    }
  }
}

int OpenCVStereoCameraCalibration::main( void )
{
  // get options:
  unlockData();


  // initialize some data
  int BoardWidth = integer("BoardWidth"); // Board width in squares
  int BoardHeight = integer("BoardHeight"); // Board height 
  int CalibrationFrames = integer("CalibrationFrames");
  int SkipFrames = integer("SkipFrames");
  double squareWidth = number("SquareWidth");
  double squareHeight = number("SquareHeight");

  int InteriorPoints = BoardWidth * BoardHeight;
  BoardSize = cvSize( BoardWidth, BoardHeight );

  
  // get stereo camera
  for ( unsigned int k=0; k<10; k++ ) {
    SCam = 0;
    Str ns( k+1, 0 );
    SCam = dynamic_cast< misc::OpenCVStereoCamera* >( device( "stereocamera-" + ns ) );
    if ( SCam != 0 ){
      printlog("Found stereo camera stereocamera-" + Str(ns));
      break;
    }
  }


  // get camera control
  CameraControl* CamContrl = 0;
  CamContrl = dynamic_cast< CameraControl* >( control("CameraControl") );
  if (CamContrl == 0 ) {
    cerr << "Could not get pointer to camera control" << endl;
    readLockData();
    return 1;
  }else{
    printlog("Disabling camera control");
    CamContrl->disable();
    lockControl("CameraControl");
  }
  


  // get captures from cameracontrols
  for (int i = 0; i != 2; ++i){
    Cam[i] = 0;
    if (i == 0){
      printlog("Accessing camera device " + SCam->getLeftCamDevice());
      Cam[i] = dynamic_cast< misc::OpenCVCamera* >( device(SCam->getLeftCamDevice()) );
    }else{
      printlog("Accessing camera device " + SCam->getRightCamDevice());
      Cam[i] = dynamic_cast< misc::OpenCVCamera* >( device(SCam->getRightCamDevice()) );
    }
      
    if (Cam[i] == 0 ) {
      cerr << "Could not get pointer to camera device " << i << endl;
      readLockData();
      return 1;
    }
    
    if (!Cam[i]->isCalibrated()){
      cerr << "Please calibrate each single camera before stereocalibration!" << endl;
      readLockData();
      return 1;
    }

    // get capture from camera object and take picture
    Capture[i] = Cam[i]->getCapture();
    assert( Capture[i] );
    QtImg[i] = misc::ConvertImage(cvQueryFrame( Capture[i] )); // properly initialize QtImg2 before starting timer

  }
  
  int timer = startTimer(INVFRAMERATE); // start timer that causes timeEvent to be called which displays the image

  CvMat* ImagePoints[2];
  for (int i = 0; i != 2; ++i){
    ImagePoints[i] = cvCreateMat( CalibrationFrames*InteriorPoints, 2, CV_32FC1 );
  }
    
  CvMat* ObjectPoints = cvCreateMat( CalibrationFrames*InteriorPoints, 3, CV_32FC1 );
  CvMat* PointCounts = cvCreateMat( CalibrationFrames, 1, CV_32SC1 );



  int Successes = 0;
  int Step, Frame = 0;
  int found[2] = {0, 0};
  CornerCount[0] = CornerCount[1] = 0;

  
  IplImage *Image[2];
  IplImage *GrayImage[2];
  for (int i = 0; i != 2; ++i){
    Image[i] = cvQueryFrame( Capture[i] );
    GrayImage[i] = cvCreateImage( cvGetSize( Image[i] ), 8, 1 );
  }
  

  // Capture Corner views loop until we've got CalibrationFrames
  // succesful captures (all corners on the board are found)
  int i = 0;
  bool needNewCorners = true;
  CvPoint2D32f* Corners[2];

  while( Successes < CalibrationFrames ){
    if ( interrupt() ){
      readLockData();
      return Aborted;
    }

    CountLCD->display(SkipFrames - Frame++ % SkipFrames );
    usleep(1000000/INVFRAMERATE);
 
    

    // Skp every SkipFrames frames to allow user to move chessboard
    if( Frame % SkipFrames == 0 ){
      if (needNewCorners){
	for (i = 0; i != 2; ++i)
	  Corners[i] = new CvPoint2D32f[ InteriorPoints ];
	needNewCorners  = false;
      }

      disableStream = true; // disable fetching in timerevent
      usleep(100000);
      for (i = 0; i != 2; ++i){
      	Image[i] = cvQueryFrame( Capture[i] );
      }
      disableStream = false; // enable fetching in timerevent

      // Find chessboard corners:
      for (i = 0; i != 2; ++i){
	found[i] = cvFindChessboardCorners( Image[i], BoardSize, Corners[i],
					 &CornerCount[i], CV_CALIB_CB_ADAPTIVE_THRESH |
					 CV_CALIB_CB_FILTER_QUADS );
      }
      
      if (found[0] == 1 && found[1] == 1){

      	// Get subpixel accuracy on those corners
      	for (i = 0; i != 2; ++i){
      	  cvCvtColor( Image[i], GrayImage[i], CV_BGR2GRAY );
	  cvFindCornerSubPix( GrayImage[i], Corners[i], CornerCount[i], cvSize( 11, 11 ), 
	   		      cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
	  needNewCorners = true;
	  FoundCorners[i].push_back(Corners[i]);

      	}

      
	


      	// If we got a good board, add it to our data
      	if( CornerCount[0] == InteriorPoints  && CornerCount[1] == InteriorPoints){
      	  Step = Successes * InteriorPoints;
      	  for( int i=Step, j=0; j < InteriorPoints; ++i, ++j ){
      	    for (int k = 0; k != 2; ++k){
      	      CV_MAT_ELEM( *ImagePoints[k], float, i, 0 ) = Corners[k][j].x;
      	      CV_MAT_ELEM( *ImagePoints[k], float, i, 1 ) = Corners[k][j].y;
	      // cerr << Corners[k][j].x << ", " << Corners[k][j].y << " | ";
      	    }
	    // cerr << (float)squareHeight * (float)(j / BoardWidth) << ", " 
	    // 	 << (float)squareWidth  * (float)(j % BoardWidth) << endl;
      	    CV_MAT_ELEM( *ObjectPoints, float, i, 0 ) = (float)squareHeight * (float)(j / BoardWidth);
      	    CV_MAT_ELEM( *ObjectPoints, float, i, 1 ) = (float)squareWidth  * (float)(j % BoardWidth);

      	    CV_MAT_ELEM( *ObjectPoints, float, i, 2 ) = 0.0f;
      	  }
      	  CV_MAT_ELEM( *PointCounts, int, Successes, 0 ) = InteriorPoints;
      	  Successes++;
      	  FrameLCD->display(Successes);
      	}
      }

    
    }
  }
    
  //// End collection while loop




  SCam->calibrate( ObjectPoints, ImagePoints, PointCounts, cvGetSize( Image[0] ));

  // kill timer to stop display
  killTimer(timer);

  // unlock camera control
  unlockControl("CameraControl");
  CamContrl->startStream();


  readLockData();
  FrameLCD->display(0);
  for (int j = 0; j != 2; ++j){
    for (vector<CvPoint2D32f*>::size_type i =0; i != FoundCorners[j].size(); ++i){
      delete (FoundCorners[j][i]);
    }
  }
  
  return Completed;

  }


addRePro( OpenCVStereoCameraCalibration, camera );

}; /* namespace camera */

#include "moc_opencvstereocameracalibration.cc"
