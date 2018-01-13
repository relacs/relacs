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

Movement of the robot can be controlled either in raw steps of the
motors (the \c step functions) or in meter (\c move and \c pos
functions).  Movements can be relative (stebBy() and moveBy()
functions) or absolute (stepTo() and moveTo() functions).

The most basic movements are along the axis of the robot
(e.g. stepByX(), moveToY()).
More general movements are specified by 3-dimensional points
(e.g. stepTo(const Point &pos), moveBy(const Point &pos)).

You need to implement the following functions: 
Device::open(),
Device::close(),
stop( int ),
doStepBy( int, int, double, double ).

You might want to implement the functions
step( int ), 
pos( int ), 
doStepTo( int, int, double, double ),
doMoveTo( const Point&, double, double ),
doMoveBy( const Point&, double, double ).

The constructor, initOptions(), or open() need to initialize the following variables:
PosAmpl, NegAmpl, SpeedFac, AccFac, DefaultSpeed, DefaultAcc.

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
    /*! Construct a Manipulator with class name \a deviceclass.
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
  int stepBy( int axis, int steps, double speed=0, double acc=0 );
    /*! Relative move of x-axis by \a steps steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  int stepByX( int steps, double speed=0, double acc=0 )
  { return stepBy( 0, steps, speed, acc ); };
    /*! Relative move of y-axis by \a steps steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  int stepByY( int steps, double speed=0, double acc=0 )
  { return stepBy( 1, steps, speed, acc ); };
    /*! Relative move of z-axis by \a steps steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used. */
  int stepByZ( int steps, double speed=0, double acc=0 )
  { return stepBy( 2, steps, speed, acc ); };

    /*! Absolute move of axis \a axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	The first axis has the index 0.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int stepTo( int axis, int pos, double speed=0, double acc=0 );
    /*! Absolute move of x-axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int stepToX( int pos, double speed=0, double acc=0 )
  { return stepTo( 0, pos, speed, acc ); };
    /*! Absolute move of y-axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int stepToY( int pos, double speed=0, double acc=0 )
  { return stepTo( 1, pos, speed, acc ); };
    /*! Absolute move of z-axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If \a speed or \a acc are zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int stepToZ( int pos, double speed=0, double acc=0 )
  { return stepTo( 2, pos, speed, acc ); };

    /*! Return the current position of the axis \a axis in raw steps. */
  virtual double step( int axis ) const { return CurrentSteps[axis]; };
    /*! Return the current position of the x-axis in raw steps. */
  double stepX( void ) const { return step( 0 ); };
    /*! Return the current position of the y-axis in raw steps. */
  double stepY( void ) const { return step( 1 ); };
    /*! Return the current position of the z-axis in raw steps. */
  double stepZ( void ) const { return step( 2 ); };
    /*! Return the current position of the x,y, and z-axis in raw steps. */
  Point step( void ) const { return Point( stepX(), stepY(), stepZ() ); };

    /*! Relative move of axis \a axis by \a dist with speed \a speed
        and acceleration \a acc.
	The first axis has the index 0.
	The distance is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  int moveBy( int axis, double dist, double speed=0.0, double acc=0.0 );
    /*! Relative move of x-axis by \a x with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  int moveByX( double x, double speed=0.0, double acc=0.0 )
  { return moveBy( 0, x, speed, acc ); };
    /*! Relative move of y-axis by \a y with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  int moveByY( double y, double speed=0.0, double acc=0.0 )
  { return moveBy( 1, y, speed, acc ); };
    /*! Relative move of z-axis by \a z with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  int moveByZ( double z, double speed=0.0, double acc=0.0 )
  { return moveBy( 2, z, speed, acc ); };

    /*! Absolute move of axis \a axis to \a pos with speed \a speed
        and acceleration \a acc.
	The first axis has the index 0.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int moveTo( int axis, double pos, double speed=0.0, double acc=0.0 );
    /*! Absolute move of x-axis to \a x with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int moveToX( double x, double speed=0.0, double acc=0.0 )
  { return moveTo( 0, x, speed, acc ); };
    /*! Absolute move of y-axis to \a y with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int moveToY( double y, double speed=0.0, double acc=0.0 )
  { return moveTo( 1, y, speed, acc ); };
    /*! Absolute move of z-axis to \a z with speed \a speed
        and acceleration \a acc.
	The position is given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used.
        Returns NotSupported, if no absolute positions are supported. */
  int moveToZ( double z, double speed=0.0, double acc=0.0 )
  { return moveTo( 2, z, speed, acc ); };

    /*! Relative move of x, y, and z-axis by \a dist with speed \a speed
        and acceleration \a acc.
	The distance coordinates are given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If values in \a speed or \a acc are zero, a default value for 
	the speed or acceleration of the corresponding axis is used. */
  int moveBy( const Point &dist, double speed=0.0, double acc=0.0 );
    /*! Absolute move of x, y, and z-axis to \a pos with speed \a speed
        and acceleration \a acc.
	The position coordinates are given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If values in \a speed or \a acc are zero, a default value for 
	the speed or acceleration of the corresponding axis is used.
        Returns NotSupported, if no absolute positions are supported. */
  int moveTo( const Point &pos, double speed=0.0, double acc=0.0 );

    /*! Return the current position of the axis \a axis in meter. */
  virtual double pos( int axis ) const { return CurrentPos[axis]; };
    /*! Return the current position of the x-axis in meter. */
  double posX( void ) const { return pos( 0 ); };
    /*! Return the current position of the y-axis in meter. */
  double posY( void ) const { return pos( 1 ); };
    /*! Return the current position of the z-axis in meter. */
  double posZ( void ) const { return pos( 2 ); };
    /*! Return the current position of the x,y, and z-axis in meter. */
  Point pos( void ) const { return Point( posX(), posY(), posZ() ); };

    /*! Immediately stop movement of axis \a axis. */
  virtual int stop( int axis ) = 0;
    /*! Immediately stop movement of x-axis. */
  int stopX( void ) { return stop( 0 ); };
    /*! Immediately stop movement of y-axis. */
  int stopY( void ) { return stop( 1 ); };
    /*! Immediately stop movement of z-axis. */
  int stopZ( void ) { return stop( 2 ); };
    /*! Immediately stop movement of all axes. */
  int stop( void );

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

    /*! The distance in meter corresponding to \a steps raw steps of the x-axis. */
  double distanceX( int steps=1 ) const { return steps*PosAmpl[0]; };
    /*! The distance in meter corresponding to \a steps raw steps of the y-axis. */
  double distanceY( int steps=1 ) const { return steps*PosAmpl[1]; };
    /*! The distance in meter corresponding to \a steps raw steps of the z-axis. */
  double distanceZ( int steps=1 ) const { return steps*PosAmpl[2]; };

    /*! The number of raw steps of the x-axis corresponding to a distance of \a dist
        in meter. */
  int stepsX( double dist ) const { return (int) ::round( dist/PosAmpl[0] ); };
    /*! The number of raw steps of the x-axis corresponding to a distance of \a dist
        in meter. */
  int stepsY( double dist ) const { return (int) ::round( dist/PosAmpl[1] ); };
    /*! The number of raw steps of the x-axis corresponding to a distance of \a dist
        in meter. */
  int stepsZ( double dist ) const { return (int) ::round( dist/PosAmpl[2] ); };

    /*! Set the amplitude in meter of a single raw step of the x-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl.
        If \a negampl differs from \a posampl then absolute moves
	on the x-axis are not supported. */
  virtual int setStepAmplX( double posampl, double negampl=-1.0 );
    /*! Set the amplitude in meter of a single raw step of the y-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl.
        If \a negampl differs from \a posampl then absolute moves
	on the y-axis are not supported. */
  virtual int setStepAmplY( double posampl, double negampl=-1.0 );
    /*! Set the amplitude in meter of a single raw step of the z-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl.
        If \a negampl differs from \a posampl then absolute moves
	on the z-axis are not supported. */
  virtual int setStepAmplZ( double posampl, double negampl=-1.0 );

    /*! The minimum possible amplitude for the x-axis in meter. */
  virtual double minAmplX( void ) const;
    /*! The maximum possible amplitude for the x-axis in meter. */
  virtual double maxAmplX( void ) const;
    /*! The minimum possible amplitude for the y-axis in meter. */
  virtual double minAmplY( void ) const;
    /*! The maximum possible amplitude for the y-axis in meter. */
  virtual double maxAmplY( void ) const;
    /*! The minimum possible amplitude for the z-axis in meter. */
  virtual double minAmplZ( void ) const;
    /*! The maximum possible amplitude for the z-axis in meter. */
  virtual double maxAmplZ( void ) const;


protected:

    /*! Implement this function for a relative move of axis \a axis by
        \a steps steps with speed \a speed and acceleration \a acc.
        Steps, speed, and acceleration are given in raw values whose
        meaning depend on the robot interface. */
  virtual int doStepBy( int axis, int steps, double speed, double acc ) = 0;
    /*! Implement this function for an absolute move 
        of axis \a axis to \a pos steps with speed \a speed
        and acceleration \a acc.
	Steps, speed, and acceleration are given in raw values whose
	meaning depend on the robot interface.
        If no absolute positions are supported return NotSupported. */
  virtual int doStepTo( int axis, int pos, double speed, double acc ) { return NotSupported; };

    /*! Implement this function for a relative move of x, y, and z-axis
        by \a dist with speed \a speed and acceleration \a acc.
	The distance coordinates are given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used. */
  virtual int doMoveBy( const Point &dist, double speed, double acc ) { return NotSupported; };
    /*! Implement this function for an absolute move of x, y, and z-axis 
        to \a pos with speed \a speed and acceleration \a acc.
	The position coordinates are given in meter, the speed in meter per second
	and the acceleration in meter per second squared.
        If \a speed or \a acc is zero, a default value for 
	the speed or acceleration is used.
        If no absolute positions are supported return NotSupported. */
  virtual int doMoveTo( const Point &pos, double speed, double acc ) { return NotSupported; };

    /*! The distance in meter of a single positive step for each axis. */
  Point PosAmpl;
    /*! The distance in meter of a single negative step for each axis. */
  Point NegAmpl;
    /*! The factors for scaling speed values to meter per seconds for each axis. */
  Point SpeedFac;
    /*! The factors for scaling acceleration values to meter per
        seconds squared for each axis. */
  Point AccFac;

    /*! Default raw speed values. */
  Point DefaultSpeed;
    /*! Default raw acceleration values. */
  Point DefaultAcc;

    /*! The current position in raw steps for each axis. */
  Point CurrentSteps;
    /*! The current position in meters for each axis. */
  Point CurrentPos;

};


}; /* namespace relacs */

#endif /* ! _RELACS_MANIPULATOR_H_ */
