/*
  camera/cameracontrol.h
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

#ifndef _RELACS_CAMERA_CAMERACONTROL_H_
#define _RELACS_CAMERA_CAMERACONTROL_H_ 1

#include <relacs/misc/opencvcamera.h>
#include <relacs/control.h>
#include <vector>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>

using namespace relacs;

namespace camera {


/*!
\class CameraControl
\brief [Control] Camera control plugin
\author Fabian Sinz
\version 1.0 (Mai 29, 2012)
*/


class CameraControl : public Control
{
  Q_OBJECT

public:

  CameraControl( void );
  ~CameraControl(void);
  virtual void main( void );
  virtual void initDevices( void );
  virtual void clearDevices( void );
  string currentCamera() const;

public slots:
  void startStream(void); 
  void stopStream(void); 
  void disable(void); 

protected: 
  vector<misc::OpenCVCamera *> Cams;
  void timerEvent(QTimerEvent*); // Timer-Funktion zum Frames-auslesen und anzeigen
  QComboBox * cameraBox;

private:
  OptWidget SW;
  QLabel* _imgLabel;
  QImage _qtImg;
  int currentCam;
  QRadioButton * isCalibrated;
  int Timer;
  QPushButton *StartButton, *StopButton;
};


}; /* namespace camera */

#endif /* ! _RELACS_CAMERA_CAMERACONTROL_H_ */
