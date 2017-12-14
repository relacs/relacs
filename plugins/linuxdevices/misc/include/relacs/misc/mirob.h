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
 */

class Mirob : public Device {

 public:

  struct axis_limits {
    int axis = 1;
    bool positive = false;
    bool negative = false;
  };

  Mirob( const string &device );
  Mirob( void );
  ~Mirob( void );

  virtual int open( const string &device ) override;
  virtual bool isOpen( void ) const { return Opened; };
  virtual void close( void );
  virtual int reset( void );

  // inits the robot and the axes
  bool start();

  // getter and setter:

  int speed( void ) const;
  bool setSpeed( int speed );
  double acceleration( void ) const;
  bool setAcceleration( double acc );

 // movement control:

 // returns the postion of the given axis:
 int get_axis_position(int axis);

 // returns the positions of all axis
 Point get_position();

 bool wait_motion_complete();


 void set_intern_position(int axis, long int pos);

 void stop_axis(int axis);

 // moves the given axis to the target with the given speed
 void move_axis_abs(int axis, double target, int speed);


 void move_axis_rel(int axis,  double length, int speed);


 // init functions:

 //Moves the given axis to the positive/negative limit and sets it position to 0
 void search_home(int axis, int speed, bool positive);

 //Moves all axis to the given limit(positive)
 void go_to_reference(bool positive, int speed);

 //Initialises the communication with the robot
 int init_mirob();

 //Tries to read the setup file
 int read_setup();

 //Sets up and activates all 3 axis of the robot
 long setup_axes(int setupindex);

 //activates all 3 axis
 bool switch_on_power();

 //Limits:

 //Checks for active limits of the axis given in "limits" and updates them
 void check_limit_switch(axis_limits &limits);

 //returns true if the axis is in the positive limit:
 bool check_pos_limit(int axis);

 //returns true if the axis is in the negative limit:
 bool check_neg_limit(int axis);

 void check_all_reg(int axis);


 //returns the step length of the given axis.
 double get_step_length(int axis);

 //returns the factor between the given axis and the x-axis
 double get_axis_factor(int axis);


 private:

  int Speed = 40;
  double Acc = 0.25;

  bool Opened;

  int FileDescr;


 //returns the max of the three given values.
 double get_max(double a, double b, double c);

};

}; /* namespace misc */

#endif /* ! _RELACS_MISC_MIROB_H_ */
