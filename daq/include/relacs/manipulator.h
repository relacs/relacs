/*
  manipulator.h
  Virtual class for controlling a Manipulator. 

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

#ifndef _RELACS_MANIPULATOR_H_
#define _RELACS_MANIPULATOR_H_ 1


#include <relacs/point.h>
#include <relacs/device.h>
using namespace std;

namespace relacs {


/*!
\class Manipulator
\brief Virtual class for controlling a manipulator - a robot with several linear axis. 
\author Jan Benda
\version 1.0
\todo implement info() and settings()
\todo add something for indicating existing axis

The Manipulator class defines an interface for basic operations
to control a manipulator. 

In case you want to use a manipulator device within RELACS, your
Manipulator implementation needs to provide a void default constructor
(i.e. with no parameters) that does not open the device.  Also,
include the header file \c <relacs/relacsplugin.h> and make the
Manipulator device known to RELACS with the \c addDevice(
ClassNameOfYourManipulatorImplementation, PluginSetName ) macro.
*/
  

class Manipulator : public Device
{

public:

    /*! Construct a Manipulator. */
  Manipulator( void );
    /*! Construct a Manipulator with class \a deviceclass.
        \sa setDeviceClass() */
  Manipulator( const string &deviceclass );
    /*! Destroy a Manipulator. In case it is open, close it. */
  virtual ~Manipulator( void );

    /*! Relative move of x-axis by \a x with speed \a speed.
        Depending on the implementation \a x can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int stepX( double x, double speed=0.0 );
    /*! Relative move of y-axis by \a y with speed \a speed.
        Depending on the implementation \a y can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int stepY( double y, double speed=0.0 );
    /*! Relative move of z-axis by \a z with speed \a speed.
        Depending on the implementation \a z can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int stepZ( double z, double speed=0.0 );
    /*! Relative move of axis \a axis by \a s with speed \a speed.
        Depending on the implementation \a z can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int step( int axis, double s, double speed=0.0 );
    /*! Relative move of x, y, and z-axis by \a step with speed \a speed.
        Depending on the implementation \a s can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int step( const Point &s, double speed=0.0 );

    /*! Absolute move of x-axis to \a x with speed \a speed.
        Depending on the implementation \a x can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int moveX( double x, double speed=0.0 );
    /*! Absolute move of y-axis to \a y with speed \a speed.
        Depending on the implementation \a y can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int moveY( double y, double speed=0.0 );
    /*! Absolute move of z-axis to \a z with speed \a speed.
        Depending on the implementation \a z can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int moveZ( double z, double speed=0.0 );
    /*! Absolute move of axis \a axis to position \a pos with speed \a speed.
        Depending on the implementation \a pos can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int move( int axis, double pos, double speed=0.0 );
    /*! Absolute move of x, y, and z-axis to \a pos with speed \a speed.
        Depending on the implementation \a step can be raw steps
	or a specific distance or angle.
        If \a speed is zero, a default value for the speed is used. */
  virtual int move( const Point &pos, double speed=0.0 );

    /*! Return the position of the x-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posX( void ) const;
    /*! Return the position of the y-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posY( void ) const;
    /*! Return the position of the z-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posZ( void ) const;
    /*! Return the position of the axis \a axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double pos( int axis ) const;
    /*! Return the position of the x,y, and z-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual Point pos( void ) const;

    /*! Sleep until current movement finished. Return 0 on success. */
  virtual int wait( void ) const;

    /*! Defines the current position of the x axis as the home position. */
  virtual int clearX( void );
    /*! Defines the current position of the y axis as the home position. */
  virtual int clearY( void );
    /*! Defines the current position of the z axis as the home position. */
  virtual int clearZ( void );
    /*! Defines the current position of all axis as the home position. */
  virtual int clear( void );

    /*! Move x axis back to its home position. */
  virtual int homeX( void );
    /*! Move y axis back to its home position. */
  virtual int homeY( void );
    /*! Move z axis back to its home position. */
  virtual int homeZ( void );
    /*! Move back to the home position. */
  virtual int home( void );

    /*! Set the amplitude of a step of the x-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplX( double posampl, double negampl=-1.0 );
    /*! Set the amplitude of a step of the y-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplY( double posampl, double negampl=-1.0 );
    /*! Set the amplitude of a step of the z-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplZ( double posampl, double negampl=-1.0 );

    /*! The minimum possible amplitude for the x-axis. */
  virtual double minAmplX( void ) const;
    /*! The maximum possible amplitude for the x-axis. */
  virtual double maxAmplX( void ) const;
    /*! The minimum possible amplitude for the y-axis. */
  virtual double minAmplY( void ) const;
    /*! The maximum possible amplitude for the y-axis. */
  virtual double maxAmplY( void ) const;
    /*! The minimum possible amplitude for the z-axis. */
  virtual double minAmplZ( void ) const;
    /*! The maximum possible amplitude for the z-axis. */
  virtual double maxAmplZ( void ) const;

};


}; /* namespace relacs */

#endif /* ! _RELACS_MANIPULATOR_H_ */
