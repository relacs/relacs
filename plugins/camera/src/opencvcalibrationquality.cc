/*
  camera/opencvcalibrationquality.cc
  A RePro to check the quality of the camera calibration.

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

#include <relacs/camera/opencvcalibrationquality.h>
#include <sstream>
#define IMGWIDTH 500
#define IMGHEIGHT 500
#define INVFRAMERATE 50

using namespace relacs;
using namespace cv;

namespace camera {


OpenCVCalibrationQuality::OpenCVCalibrationQuality( void )
  : RePro( "OpenCVCalibrationQuality", "camera", "Fabian Sinz", "1.0", "Jun 15, 2012" )
{
  // add some options:
  addInteger("BoardWidth", 9, "number of interior points on the calibration board"); // 
  addInteger("BoardHeight", 7, "number of interior points on the calibration board"); // 
  addNumber("SquareWidth", 24, "Chess board square width in mm");
  addNumber("SquareHeight", 23, "Chess board square height in mm");


  // layout

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
  
  
  DoReproj = new QCheckBox("display reprojections");
  Positions->addWidget(DoReproj,0,0);

  DoStereoTransform = new QCheckBox("transform between cameras");
  Positions->addWidget(DoStereoTransform,0,1);

  DoEpipolarError = new QCheckBox("Compute Epipolar Error");
  Positions->addWidget(DoEpipolarError,0,2);

  DoPrincipalPoints = new QCheckBox("Show principal points");
  Positions->addWidget(DoPrincipalPoints,0,3);

}

void OpenCVCalibrationQuality::timerEvent(QTimerEvent*)
{
  bool found[2] = {false, false};
  for (int i = 0; i != 2; ++i){
    Image[i] = Cam[i]->grabFrame();
    
  }

  /*****************************************************************************************/
  if ( DoPrincipalPoints->isChecked() ){
    Mat tmp;
    for (int i = 0; i != 2; ++i){
      tmp = Cam[i]->getIntrinsicMatrix();

      circle(Image[i], Point(tmp.at<double>(0,2), tmp.at<double>(1,2)), 3, Scalar(0,0,255) ,2 );
    }
  }

  /*****************************************************************************************/
  for (int i = 0; i != 2; ++i){
    found[i] = Cam[i]->findChessboardCorners(Image[i], BoardSize, Corners[i]);


	if (found[i]){
	  // Get subpixel accuracy on those corners
	  cvtColor( Image[i], GrayImage[i], CV_BGR2GRAY );
	  cornerSubPix(GrayImage[i], Corners[i], Size(11,11),Size(-1,-1),
		       TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS,100,0.001));


	  drawChessboardCorners( Image[i], BoardSize, Mat(Corners[i]), found[i] );

	  // find coordinates in the corresponding camera systems
	  vector<Point3f> obj;
	  for(int j=0;j<InteriorPoints;++j)
	    obj.push_back(Point3f(squareHeight * (float)(j/BoardWidth), squareWidth*(float)(j%BoardWidth), 0.0f));
	  ObjectPoints[i] = Cam[i]->findChessboard3D(Mat(obj), Mat(Corners[i]));
	 
	  // display reprojections for each individual camera
	  if (  DoReproj->isChecked() ){
	    ImagePoints = Cam[i]->project(ObjectPoints[i]); 

	    for (int j = 0; j != ImagePoints.rows; ++j){
	      circle(Image[i], Point(ImagePoints.at<float>(j,0), ImagePoints.at<float>(j,1)),
		     3, Scalar(255,0,0) ,2 );
	    }

	  }

	}
  }

  
  /*****************************************************************************************/
  if ( found[0] && found[1] &&  DoStereoTransform->isChecked() ){
    Mat P;
    // ImagePoints = Cam[0]->project(ObjectPoints[0]); 
    P = ObjectPoints[0].clone();
    SCam->transformLeftToRight(P); // cam 0 --> cam 1
    ImagePoints = Cam[1]->project(P);
    for (int j = 0; j != ImagePoints.rows; ++j){
      circle(Image[1], Point(ImagePoints.at<float>(j,0), ImagePoints.at<float>(j,1)),
	     2, Scalar(0,255,0) ,2 );
    }

    
    // ImagePoints = Cam[1]->project(ObjectPoints[1]); 
    P = ObjectPoints[1].clone();
    SCam->transformRightToLeft(P); // cam 0 --> cam 1
    ImagePoints = Cam[0]->project(P);
    for (int j = 0; j != ImagePoints.rows; ++j){
      circle(Image[0], Point(ImagePoints.at<float>(j,0), ImagePoints.at<float>(j,1)),
	     2, Scalar(0,255,0) ,2 );
    }

   
  }
  /*****************************************************************************************/
  if ( found[0] && found[1] &&  DoEpipolarError->isChecked()){
    vector<Vec3f> L1, L2;
    Mat F;
   F = SCam->getFundamentalMatrix();
    computeCorrespondEpilines( Mat(Corners[0]), 1, F, L1 ); 
    computeCorrespondEpilines( Mat(Corners[1]), 2, F, L2 );

    double avgErr = 0, err = 0; 
    for(vector<Vec3f>::size_type j = 0; j != L1.size(); ++j ) {
      err = fabs(Corners[0][j].x * L2[j][0] + Corners[0][j].y * L2 [j][1] + L2[j][2]);
      err += fabs(Corners[1][j].x * L1 [j][0] + Corners[1][j].y * L1[j][1] + L1[j][2]);
      avgErr += err; 
    } 

    ostringstream buffer;
    buffer << "Epipolar Error/ Point: ";
    buffer << avgErr/(double) L1.size();
    putText(Image[0], buffer.str(), Point(10.f,30.f), FONT_HERSHEY_SIMPLEX, 1., Scalar(0,255,0), 2);

  }

  for (int i = 0; i != 2; ++i){

    QtImg[i] = misc::Mat2QImage(Image[i]);
    ImgLabel[i]->setPixmap(QPixmap::fromImage(QtImg[i].scaled(IMGWIDTH, IMGHEIGHT,Qt::KeepAspectRatio)));  
    ImgLabel[i]->show();
  }
}


int OpenCVCalibrationQuality::main( void )
{
  unlockData();

  // initialize some data
  BoardWidth = integer("BoardWidth"); // Board width in squares
  BoardHeight = integer("BoardHeight"); // Board height 
  squareWidth = number("SquareWidth");
  squareHeight = number("SquareHeight");

  InteriorPoints = BoardWidth * BoardHeight;
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
  }

  // get cameras from cameracontrols
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


  }


  int timer = startTimer(INVFRAMERATE);


  // run the repro (most of the job will be done by timerevent)
  while (true){
    if (interrupt()){
      printlog("Termination signal caught");
      readLockData();
      killTimer(timer);

      break;
    }

    usleep(1000000/INVFRAMERATE);
  }

  return Completed;
}


addRePro( OpenCVCalibrationQuality, camera );

}; /* namespace camera */

#include "moc_opencvcalibrationquality.cc"
