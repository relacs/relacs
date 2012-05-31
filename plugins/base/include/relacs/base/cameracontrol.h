/*
  base/cameracontrol.h
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

#ifndef _RELACS_BASE_CAMERACONTROL_H_
#define _RELACS_BASE_CAMERACONTROL_H_ 1

#include <relacs/misc/usbcamera.h>
#include <relacs/control.h>
#include <vector>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QRadioButton>

using namespace relacs;

namespace base {


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


protected: 
  vector<misc::USBCamera *> Cams;
  void timerEvent(QTimerEvent*); // Timer-Funktion zum Frames-auslesen und anzeigen
  QComboBox * cameraBox;

private:
  OptWidget SW;
  QLabel* _imgLabel;
  QImage _qtImg;
  int currentCam;
  QRadioButton * isCalibrated;
 

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_CAMERACONTROL_H_ */
