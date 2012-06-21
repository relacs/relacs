/*
  camera/opencvcameracalibration.h
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

#ifndef _RELACS_CAMERA_OPENCVCAMERACALIBRATION_H_
#define _RELACS_CAMERA_OPENCVCAMERACALIBRATION_H_ 1


#include <relacs/repro.h>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <relacs/misc/opencvcamera.h>
#include <relacs/camera/cameracontrol.h>

using namespace relacs;

namespace camera {


/*!
\class OpenCVCameraCalibration
\brief [RePro] RePro for calibrating an OpenCVCamera
\author Fabian Sinz
\version 1.0 (Jun 01, 2012)
*/


class OpenCVCameraCalibration : public RePro
{
  Q_OBJECT


 public:
  
  OpenCVCameraCalibration( void );
  virtual int main( void );

 protected:
  void timerEvent(QTimerEvent*);
  
  QLabel* ImgLabel, *ImgLabel2;
  QImage QtImg, QtImg2;
  QLCDNumber* FrameLCD, *CountLCD;
  misc::OpenCVCamera * Cam;

 private:
  bool disableStream;
  Mat TmpImg;
  vector< Mat > FoundCorners;

  bool found;
  Size BoardSize;
};


}; /* namespace camera */

#endif /* ! _RELACS_CAMERA_OPENCVCAMERACALIBRATION_H_ */
