/*
  misc/opencvcamera.h
  The Opencvcamera module 

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

#ifndef _RELACS_MISC_OPENCVCAMERA_H_
#define _RELACS_MISC_OPENCVCAMERA_H_ 1

#include <string>
#include <vector>
#include <cv.h>
#include <highgui.h>
#include <relacs/camera.h>
#include <QGraphicsScene>
#include <QGraphicsView>

using namespace std;
using namespace relacs;
using namespace cv;

namespace misc {

  QImage ConvertImage( IplImage *Source);
  QImage Mat2QImage(const cv::Mat src);

/*!
\class OpenCVCamera
\author Fabian Sinz
\version 1.0
\brief [Camera] The %OpenCVCamera module 
*/

class OpenCVCamera : public Camera
{

 public:

  OpenCVCamera( const string &device );
  OpenCVCamera( void );
  ~OpenCVCamera( void );

  

  virtual int open( const string &device, const Options &opts );
  virtual bool isOpen( void ) const {return Opened; };
  virtual void close( void );
  virtual int reset( void );

  bool isCalibrated( void ) const {return Calibrated; };
  const VideoCapture getCapture(void) const {return Source; };
  Mat getIntrinsicMatrix(void) const{return IntrinsicMatrix.clone();}
  Mat getDistortionCoeffs(void) const{return DistortionCoeffs.clone();}

  int calibrate(vector< vector<Point3f> > ObjectPoints, vector< vector<Point2f> > ImagePoints, Size sz);
  void saveParameters(void);
  void recomputeUndistortionMaps(void);
  void setCalibrated(bool toWhat);


  Mat grabFrame(void);
  Mat grabFrame(bool undistort);
  QImage grabQImage(void);

 protected:
  bool Opened, Calibrated;
  VideoCapture Source;
  string ParamFile;
  int CameraNo;

  Mat UDMapX, UDMapY;


 private:
  
  Mat IntrinsicMatrix, DistortionCoeffs;
  bool EstimateDistortion;

};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_OPENCVCAMERA_H_ */
