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
\version 2.0
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

    /*! Relative move of axis \a axis by \a steps steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepBy( int axis, int steps, int speed=0, int acc=0 );
    /*! Relative move by \a step steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepBy( const Point &steps, int speed=0, int acc=0 );
    /*! Relative move of x-axis by \a steps steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepByX( int steps, int speed=0, int acc=0 )
  { return stepBy( 0, steps, speed, acc ); };
    /*! Relative move of y-axis by \a steps steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepByY( int steps, int speed=0, int acc=0 )
  { return stepBy( 1, steps, speed, acc ); };
    /*! Relative move of z-axis by \a steps steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepByZ( int steps, int speed=0, int acc=0 )
  { return stepBy( 2, steps, speed, acc ); };

    /*! Absolute move of axis \a axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepTo( int axis, int pos, int speed=0, int acc=0 );
    /*! Absolute move to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepTo( const Point &pos, int speed=0, int acc=0 );
    /*! Absolute move of x-axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepToX( int pos, int speed=0, int acc=0 )
  { return stepTo( 0, pos, speed, acc ); };
    /*! Absolute move of y-axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepToY( int pos, int speed=0, int acc=0 )
  { return stepTo( 1, pos, speed, acc ); };
    /*! Absolute move of z-axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  virtual int stepToZ( int pos, int speed=0, int acc=0 )
  { return stepTo( 2, pos, speed, acc ); };

    /*! Relative move of axis \a axis by \a dist with speed \a speed
        and acceleration \a acc.
	The first axis has the index 0.
	The distance is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveBy( int axis, double dist, double speed=0.0, double acc=0.0 );
    /*! Relative move of x, y, and z-axis by \a dist with speed \a speed
        and acceleration \a acc.
	The distance coordinates are given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If values in \a speed or \a acc are zero, a default value for 
	the speed or acceleration of the corresponding axis is used. */
  virtual int moveBy( const Point &dist, double speed=0.0, double acc=0.0 );
    /*! Relative move of x-axis by \a x with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveByX( double x, double speed=0.0, double acc=0.0 )
  { return moveBy( 0, x, speed, acc ); };
    /*! Relative move of y-axis by \a y with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveByY( double y, double speed=0.0, double acc=0.0 )
  { return moveBy( 1, y, speed, acc ); };
    /*! Relative move of z-axis by \a z with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveByZ( double z, double speed=0.0, double acc=0.0 )
  { return moveBy( 2, z, speed, acc ); };

    /*! Absolute move of axis \a axis to \a pos with speed \a speed
        and acceleration \a acc.
	The first axis has the index 0.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveTo( int axis, double pos, double speed=0.0, double acc=0.0 );
    /*! Absolute move of x, y, and z-axis to \a pos with speed \a speed
        and acceleration \a acc.
	The position coordinates are given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If values in \a speed or \a acc are zero, a default value for 
	the speed or acceleration of the corresponding axis is used. */
  virtual int moveTo( const Point &pos, double speed=0.0, double acc=0.0 );
    /*! Absolute move of x-axis to \a x with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveToX( double x, double speed=0.0, double acc=0.0 )
  { return moveTo( 0, x, speed, acc ); };
    /*! Absolute move of y-axis to \a y with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveToY( double y, double speed=0.0, double acc=0.0 )
  { return moveTo( 1, y, speed, acc ); };
    /*! Absolute move of z-axis to \a z with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int moveToZ( double z, double speed=0.0, double acc=0.0 )
  { return moveTo( 2, z, speed, acc ); };

    /*! Immediately stop movement of axis \a axis. */
  virtual int stop( int axis );
    /*! Immediately stop movement of all axes. */
  virtual int stop( void );
    /*! Immediately stop movement of x-axis. */
  virtual int stopX( void ) { return stop( 0 ); };
    /*! Immediately stop movement of y-axis. */
  virtual int stopY( void ) { return stop( 1 ); };
    /*! Immediately stop movement of z-axis. */
  virtual int stopZ( void ) { return stop( 2 ); };

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


protected:

    /*! The distance in meter of a single positive step for each axis. */
  double PosAmpl[3];
    /*! The distance in meter of a single negative step for each axis. */
  double NegAmpl[3];
    /*! The factors for scaling speed values to meter per seconds for each axis. */
  double SpeedFac[3];
    /*! The factors for scaling acceleration values to meter per
        seconds squared for each axis. */
  double AccFac[3];

    /*! Default raw speed values. */
  int DefaultSpeed[3];
    /*! Default raw acceleration values. */
  int DefaultAcc[3];

};


}; /* namespace relacs */

#endif /* ! _RELACS_MANIPULATOR_H_ */
