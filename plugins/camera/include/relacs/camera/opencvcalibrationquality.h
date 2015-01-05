/*
  camera/opencvcalibrationquality.h
  A RePro to check the quality of the camera calibration.

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

#ifndef _RELACS_CAMERA_OPENCVCALIBRATIONQUALITY_H_
#define _RELACS_CAMERA_OPENCVCALIBRATIONQUALITY_H_ 1

#include <relacs/repro.h>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLCDNumber>
#include <relacs/misc/opencvcamera.h>
#include <relacs/misc/opencvstereocamera.h>
#include <relacs/camera/cameracontrol.h>


using namespace relacs;
using namespace cv;

namespace camera {


/*!
\class OpenCVCalibrationQuality
\brief [RePro] A RePro to check the quality of the camera calibration.
\author Fabian Sinz
\version 1.0 (Jun 15, 2012)
*/


class OpenCVCalibrationQuality : public RePro
{
  Q_OBJECT

 public:

  OpenCVCalibrationQuality( void );
  virtual int main( void );

 protected:
  void timerEvent(QTimerEvent*);
  QCheckBox* DoReproj, *DoStereoTransform, *DoEpipolarError, *DoPrincipalPoints;
  QLabel* ImgLabel[2];
  QImage QtImg[2];
  //VideoCapture Capture[2];
  misc::OpenCVCamera * Cam[2];
  misc::OpenCVStereoCamera *SCam;
  Mat Image[2];
  Mat GrayImage[2];
  Mat ObjectPoints[2];
  Mat ImagePoints;
  
  Mat rvec[2];
  Mat tvec[2];

  vector<Point2f> Corners[2];

  Size BoardSize;

 private: 
  int InteriorPoints, BoardWidth, BoardHeight;
  double squareWidth, squareHeight;

};


}; /* namespace camera */

#endif /* ! _RELACS_CAMERA_OPENCVCALIBRATIONQUALITY_H_ */
