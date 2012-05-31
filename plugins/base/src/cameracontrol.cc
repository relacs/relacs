/*
  base/cameracontrol.cc
  Camera control plugin

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
#define V_MIRROR 1
#define H_MIRROR 2
#define IMGHEIGHT 200
#define IMGWIDTH  200

#include <relacs/base/cameracontrol.h>
using namespace relacs;

namespace base {


/****************************************************************************************/

CameraControl::CameraControl( void )
  : Control( "CameraControl", "base", "Fabian Sinz", "1.0", "Mai 29, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
 
 //  camera object
  currentCam = 0;

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


  /* start timer which calls the time event function below*/
  startTimer(30);
  
}

void CameraControl::initDevices( void )
{
  misc::OpenCVCamera * Cam;
  for ( unsigned int k=0; k<10; k++ ) {
    Cam = 0;
    Str ns( k+1, 0 );
    //Rob = dynamic_cast< Manipulator* >( device( "robot-" + ns ) );
    Cam = dynamic_cast< misc::OpenCVCamera* >( device( "camera-" + ns ) );
    if ( Cam != 0 ){
      Cams.push_back(Cam);
      cameraBox->addItem( QString( ((string)( "camera-" + ns)).c_str()));
      printlog("Found camera device-" + Str(ns));
    }
  }

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


addControl( CameraControl, base );

}; /* namespace base */

#include "moc_cameracontrol.cc"
