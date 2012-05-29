/*
  camera.h
  Virtual class for controlling a Camera. 

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

#ifndef _RELACS_CAMERA_H_
#define _RELACS_CAMERA_H_ 1


#include <relacs/device.h>
using namespace std;

namespace relacs {


/*!
\class Camera
\brief Virtual class for controlling a Camera. 
\author Fabian Sinz
\version 1.0
\todo implement info() and settings()
\todo add something for indicating existing axis

The Camera class defines an interface for basic operations
to control a camera. 

In case you want to use a camera device within RELACS, your
Attenuate implementation needs to provide a void default constructor
(i.e. with no parameters) that does not open the device.  Also,
include the header file \c <relacs/relacsplugin.h> and make the
Camera device known to RELACS with the \c addDevice(
ClassNameOfYourCameraImplementation, PluginSetName ) macro.
*/
  

class Camera : public Device
{

public:

    /*! Construct a Camera. */
  Camera( void );
    /*! Construct a Camera with class \a deviceclass.
        \sa setDeviceClass() */
  Camera( const string &deviceclass );
    /*! Destroy a Camera. In case it is open, close it. */
  virtual ~Camera( void );


};


}; /* namespace relacs */

#endif /* ! _RELACS_CAMERA_H_ */
