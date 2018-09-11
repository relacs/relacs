/*
  misc/mirob.h
  The Mirob module linear robot from MPH

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

#ifndef _RELACS_MISC_MIROB_H
#define _RELACS_MISC_MIROB_H 1

#include <relacs/point.h>
#include <relacs/device.h>
using namespace relacs;

namespace misc {


/*!
\class AmplMode
\author Alexander Ott
\version 1.0 (Nov 30, 2017)
\brief [Device] The Mirob module linear robot from MPH

You need to have permissions for the serial port:
\code
sudo adduser <login> dialout
\endcode
 */

class Mirob : public Device {

public:
  Mirob( const string &device );
  Mirob( void );
  ~Mirob( void );

  /*! inits the robot and the axes */
  bool start();

  virtual int open( const string &device ) override;
  virtual bool isOpen( void ) const { return Opened; };
  virtual void close( void );
  virtual int reset( void );

  /*! Absolute move of axis \a axis to position \a pos with speed \a speed.
    If \a speed is zero, a default value for the speed is used. */
  virtual int move( int axis, double pos, double speed=0.0 );
  /*! Relative move of axis \a axis by \a s with speed \a speed.
    If \a speed is zero, a default value for the speed is used. */
  virtual int step( int axis, double s, double speed=0.0 );

  /*! Immediately stop movement of axis \a axis. */
  virtual int stop( int axis );
  /*! Immediately stop movement of all axes. */
  virtual int stop( void );

  /*! Return the position of the axis \a axis. */
  virtual double pos( int axis ) const;
  /*! Return the position of the x,y, and z-axis. */
  virtual Point pos( void ) const;

  /*! Sleep until current movement finished. */
  virtual int wait( void ) const;

  int speed( void ) const;
  bool setSpeed( int speed );

  double acceleration( void ) const;
  bool setAcceleration( double acc );

  /*! Moves the given axis to the positive/negative limit and sets it
    position to 0. */
  void search_home( int mirobaxis, int speed, bool positive );

  /*! Moves all axis to the given limit(positive) */
  void go_to_reference(bool positive, int speed);

  /*! Initialises the communication with the robot. XXX should be private!!!
    Why is it used by other classes? */
  int init_mirob( void );
  /*! Returns true if the axis is in the positive limit. */
  bool check_pos_limit( int mirobaxis );
  /*! Returns true if the axis is in the negative limit. */
  bool check_neg_limit( int mirobaxis );

  /*! returns the step length of the given axis.*/
  double get_step_length(int axis) const;

  /*! returns the factor between the given axis and the x-axis */
  double get_axis_factor(int axis) const;

  /*! checks if the clamp is closed */
  bool toolClamped();

  /*! checks if a tool is in the clamp */
  bool toolPresent();

  /*! if the clamp is closed - opens the clamp */
  void toolRelease();

  /*! if clamp is open - closes the clamp */
  void toolFix();

  /*! Switch on all axes. Returns whether of not the operation succeeded.*/
  bool power_off( void );
  /*! Switch off all axes. Returns whether of not the operation succeeded.*/
  bool power_on( void );

  /*! Check whether an axis is turned on. If called with no argument all axes
    are checked. Numbers exceeding axes count will retrun the state of the
    last axis. */
  bool checkPowerState( int axis = -1);

  /*! Print out status bytes. XXX not used. */
  void check_all_reg( int axis );


private:
  int Speed = 40;
  double Acc = 1.5;
  bool Opened;
  int FileDescr;
  struct axis_limits {
    int mirobaxis = 1;
    bool positive = false;
    bool negative = false;
  };

  void set_intern_position( int mirobaxis, long int pos );

  /*! Check for active limits of the axis given in \a limits and updates 
    them. */
  void check_limit_switch( axis_limits &limits );

    /*! Read the setup file. */
  int read_setup( void );

  /*! Sets up and activates all 3 axis of the robot. */
  long setup_axes( int setupindex );

  /*! Activates all 3 axis. XXX this function is never used. XXX */
  bool switch_on_power( void );

  /*! Returns the max of the three given values. */
  double get_max(double a, double b, double c);
};

}; /* namespace misc */

#endif /* ! _RELACS_MISC_MIROB_H_ */
