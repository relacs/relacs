/*
  camera/opencvstereocameracalibration.h
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

#ifndef _RELACS_CAMERA_OPENCVSTEREOCAMERACALIBRATION_H_
#define _RELACS_CAMERA_OPENCVSTEREOCAMERACALIBRATION_H_ 1

#include <relacs/repro.h>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <relacs/misc/opencvcamera.h>
#include <relacs/misc/opencvstereocamera.h>
#include <relacs/camera/cameracontrol.h>
using namespace relacs;

namespace camera {


/*!
\class OpenCVStereoCameraCalibration
\brief [RePro] RePro to calibrate a stereo camera
\author Fabian Sinz
\version 1.0 (Jun 08, 2012)
*/


class OpenCVStereoCameraCalibration : public RePro
{
  Q_OBJECT

public:

  OpenCVStereoCameraCalibration( void );
  virtual int main( void );

 protected:
  
  void timerEvent(QTimerEvent*);


  QLabel* ImgLabel[2];
  QImage QtImg[2];
  QLCDNumber* FrameLCD, *CountLCD;
  CvCapture* Capture[2];
  misc::OpenCVCamera * Cam[2];
  misc::OpenCVStereoCamera *SCam;

 private:
  bool disableStream;
  vector<CvPoint2D32f*> FoundCorners[2];
  IplImage *ImgTmp;
  CvSize BoardSize;
  int CornerCount[2];
};


}; /* namespace camera */

#endif /* ! _RELACS_CAMERA_OPENCVSTEREOCAMERACALIBRATION_H_ */
