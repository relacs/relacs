/*
  camera/cameracontrol.cc
  Camera control plugin

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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
#define V_MIRROR 1
#define H_MIRROR 2
#define IMGHEIGHT 240
#define IMGWIDTH  320
#define INVFRAMERATE 30

#include <relacs/camera/cameracontrol.h>
using namespace relacs;

namespace camera {


/****************************************************************************************/

CameraControl::CameraControl( void )
  : Control( "CameraControl", "camera", "Fabian Sinz", "1.0", "Mai 29, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
 
 //  camera object
  currentCam = 0;
  Timer = 0;

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *bb;

  setLayout( vb );
  vb->setSpacing( 4 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );


  /* combo box holding the camera devices */
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  cameraBox = new QComboBox;
  bb->addWidget(cameraBox);


  /* image stream of the current camera*/
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );
  _imgLabel = new QLabel;
  _imgLabel->setAlignment(Qt::AlignCenter);
  _imgLabel->setFixedSize ( 200, 200 );
  bb->addWidget(_imgLabel);


  /*show whether camera is calibrated or not*/
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );
  isCalibrated = new QRadioButton;
  isCalibrated->setText("Calibrated");
  isCalibrated->setChecked(false);
  bb->addWidget(isCalibrated);

  /*show whether camera is calibrated or not*/
  bb = new QHBoxLayout;
  vb->addLayout(bb);

  StartButton = new QPushButton( "Start Stream" );
  bb->addWidget( StartButton );
  StartButton->setFixedHeight( StartButton->sizeHint().height() );
  connect( StartButton, SIGNAL( clicked() ),
	   this, SLOT( startStream() ) );

  StopButton = new QPushButton( "Stop Stream" );
  bb->addWidget( StopButton );
  StopButton->setFixedHeight( StopButton->sizeHint().height() );
  connect( StopButton, SIGNAL( clicked() ),
	   this, SLOT( stopStream() ) );
  StopButton->setDisabled(true);



  
}

void CameraControl::disable(){
  if (Timer){
    killTimer(Timer);
    Timer = 0;
  }
  StartButton->setDisabled(true);
  StopButton->setDisabled(true);

}

void CameraControl::startStream(){
  if (!Timer){
    Timer = startTimer(INVFRAMERATE);
  }
  StartButton->setDisabled(true);
  StopButton->setDisabled(false);
}

void CameraControl::stopStream(){
  if (Timer){
    killTimer(Timer);
    Timer = 0;
  }
  StartButton->setDisabled(false);
  StopButton->setDisabled(true);
}


void CameraControl::initDevices( void )
{
  misc::OpenCVCamera * Cam;
  for ( unsigned int k=0; k<10; k++ ) {
    Cam = 0;
    Str ns( k+1, 0 );
    Cam = dynamic_cast< misc::OpenCVCamera* >( device( "camera-" + ns ) );
    if ( Cam != 0 ){

      Cams.push_back(Cam);

      if (Cams.size() == 1){
	StartButton->setDisabled(false);
	StopButton->setDisabled(false);
	isCalibrated->setChecked(Cam->isCalibrated());
      }
      cameraBox->addItem( QString( ((string)( "camera-" + ns)).c_str()));
      printlog("Found camera device-" + Str(ns));
    }
  }

}


void CameraControl::clearDevices( void )
{
  Cam = 0;
}


string CameraControl::currentCamera(void) const{
  QString tmp = cameraBox->currentText();
  return tmp.toUtf8().constData();
}

CameraControl::~CameraControl( void ){
}

void CameraControl::timerEvent(QTimerEvent*)
{

  currentCam = cameraBox->currentIndex();
  if (Cams.size() != 0  && Cams[currentCam]->isOpen()){
    isCalibrated->setChecked(Cams[currentCam]->isCalibrated());

    _qtImg = Cams[currentCam]->grabQImage();
    _imgLabel->setPixmap(QPixmap::fromImage(_qtImg.scaled(IMGWIDTH, IMGHEIGHT,Qt::KeepAspectRatio)));  
    _imgLabel->show();

  }
}


void CameraControl::main( void )
{
  // get options:
  // double duration = number( "duration" );
}


addControl( CameraControl, camera );

}; /* namespace camera */

#include "moc_cameracontrol.cc"
