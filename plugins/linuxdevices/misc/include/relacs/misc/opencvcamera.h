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

namespace misc {

  QImage ConvertImage( IplImage *Source);


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
  CvCapture* getCapture(void) const {return Source; };

  int calibrate(CvMat* ObjectPoints2, CvMat*  ImagePoints2,CvMat* PointCounts2, CvSize ImgSize, bool estDist);
  void saveParameters(void);
  void recomputeUndistortionMaps(void);
  void setCalibrated(bool toWhat);


  IplImage* grabFrame(void);
  QImage grabQImage(void);

 protected:
  bool Opened;
  CvCapture *Source;
  string IntrinsicFile, DistortionFile;
  int CameraNo;

  IplImage* UDMapX;
  IplImage* UDMapY;


 private:
  bool Calibrated;

  CvMat *IntrinsicMatrix, *DistortionCoeffs;
  bool EstimateDistortion;

};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_OPENCVCAMERA_H_ */
