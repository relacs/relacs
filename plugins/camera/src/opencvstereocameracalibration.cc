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

#define IMGWIDTH 500
#define IMGHEIGHT 500
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
  bb->addLayout( Positions );


  // position watch
  QLabel* label = new QLabel( "Next capture in  " );
  label->setAlignment( Qt::AlignRight );
  Positions->addWidget( label, 0, 0 );

  CountLCD = new QLCDNumber( 3 );
  CountLCD->setSegmentStyle( QLCDNumber::Filled );
  CountLCD->setFixedHeight( label->sizeHint().height()*1.5 );
  CountLCD->setFixedWidth(50);

  CountLCD->setPalette( qp );
  CountLCD->setAutoFillBackground( true );
  Positions->addWidget( CountLCD, 0, 1 );

  label = new QLabel( "Calibration Frames Captured " );
  label->setAlignment( Qt::AlignRight );
  Positions->addWidget( label, 1, 0 );
 
  FrameLCD = new QLCDNumber( 3 );
  FrameLCD->setSegmentStyle( QLCDNumber::Filled );
  FrameLCD->setFixedHeight( label->sizeHint().height()*1.5 );
  FrameLCD->setFixedWidth(50);

  FrameLCD->setPalette( qp );
  FrameLCD->setAutoFillBackground( true );
  Positions->addWidget( FrameLCD, 1, 1 );

  disableStream = false;

}

void OpenCVStereoCameraCalibration::timerEvent(QTimerEvent*){
  if (!disableStream){
    for (int i = 0; i != 2; ++i){
      
      Capture[i] >> ImgTmp;

      if (FoundCorners[i].size() > 0){
      	for (vector< Mat >::size_type j =0; j != FoundCorners[i].size(); ++j){
      	  drawChessboardCorners( ImgTmp, BoardSize, FoundCorners[i][j], found[i] );
      	}
      }

      QtImg[i] = misc::Mat2QImage(ImgTmp);
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
  BoardSize = Size( BoardWidth, BoardHeight );

  
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
  }
  
   int timer = startTimer(INVFRAMERATE); // start timer that causes timeEvent to be called which displays the image

   vector< vector<Point3f> > ObjectPoints;
   vector< vector<Point2f> > ImagePoints[2]; //this is an array of a vector of vectors, weird, I know
   vector<Point2f> Corners[2];

   int Successes = 0, Frame = 0;

  
   Mat Image[2];
   Mat GrayImage[2];
   for (int i = 0; i != 2; ++i){
     Capture[i] >> Image[i];
     cvtColor(Image[i], GrayImage[i], CV_BGR2GRAY);
   }
  

  // Capture Corner views loop until we've got CalibrationFrames
  // succesful captures (all corners on the board are found)
  int i = 0;

  while( Successes < CalibrationFrames ){
    if ( interrupt() ){
      readLockData();
      return Aborted;
    }

    CountLCD->display(SkipFrames - Frame++ % SkipFrames );
    usleep(1000000/INVFRAMERATE);
 
    

    // Skp every SkipFrames frames to allow user to move chessboard
    if( Frame % SkipFrames == 0 ){


      disableStream = true; // disable fetching in timerevent
      usleep(100000);
      for (i = 0; i != 2; ++i){
	Capture[i] >> Image[i];
      }
      disableStream = false; // enable fetching in timerevent

      // Find chessboard corners:
      for (i = 0; i != 2; ++i){
	found[i] = findChessboardCorners(Image[i], BoardSize, Corners[i], 
					 CV_CALIB_CB_ADAPTIVE_THRESH + CV_CALIB_CB_NORMALIZE_IMAGE);
	//CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);	
      }
      if (found[0] && found[1]){

      	// Get subpixel accuracy on those corners
      	for (i = 0; i != 2; ++i){
      	  cvtColor( Image[i], GrayImage[i], CV_BGR2GRAY );
	  cornerSubPix(GrayImage[i], Corners[i], Size(11,11),Size(-1,-1),
		       TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS,30,0.1));
	  FoundCorners[i].push_back(Mat(Corners[i],true)); // true is for copying data


      	}

      
	// If we got a good board, add it to our data
	vector<Point3f> obj;
	for(int j=0;j<InteriorPoints;++j)
	  obj.push_back(Point3f(squareHeight * (float)(j/BoardWidth), squareWidth*(float)(j%BoardWidth), 0.0f));
	for (i = 0; i != 2; ++i)
	  ImagePoints[i].push_back(vector<Point2f>(Corners[i]));
	ObjectPoints.push_back(obj);

	Successes++;
	FrameLCD->display(Successes);
	
      }

    
    }
  }
    
  //// End collection while loop

  SCam->calibrate( ObjectPoints, ImagePoints, Image[0].size() );

  // kill timer to stop display
  killTimer(timer);

  // unlock camera control
  unlockControl("CameraControl");
  CamContrl->startStream();


  readLockData();
  FrameLCD->display(0);
 
  return Completed;

}


addRePro( OpenCVStereoCameraCalibration, camera );

}; /* namespace camera */

#include "moc_opencvstereocameracalibration.cc"
