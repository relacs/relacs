/*
  misc/xyzrobot.h
  High level interface with object avoidance for the mirob robot.

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

#ifndef _RELACS_MISC_XYZROBOT_H_
#define _RELACS_MISC_XYZROBOT_H_ 1


#include <deque>
#include <relacs/point.h>
#include <relacs/shape.h>
#include <relacs/misc/mirob.h>
#include <relacs/device.h>
using namespace relacs;

namespace misc {


/*!
\class XYZRobot
\brief [Device] High level interface with object avoidance for the mirob robot.
\author Alexander Ott
*/


class XYZRobot : public Device
{

public:

  XYZRobot( Mirob *robot );
  XYZRobot( void );
  virtual ~XYZRobot( void );

  virtual int open( Mirob &robot );
  virtual int open( Device &device ) override;
  virtual bool isOpen( void ) const;
  virtual void close( void );

  int xlength( void ) const { return XLength; };
  int ylength( void ) const { return YLength; };
  int zlength( void ) const { return ZLength; };

  bool test_point(const Point &p);
  bool test_way(const Point &pos, const Point &newP);
  bool PF_up_and_over(const Point &p);

  bool start_mirob();
  bool init_mirob();
  void close_mirob();

  void go_home();
  void search_reference(int firstAxis, int secondAxis, int thirdAxis);

  void go_to_point(double posX, double posY, double posZ);
  void go_to_point(const Point &coords, int speed = 0);

  void move_posX();
  void move_negX();
  void move_posY();
  void move_negY();
  void move_posZ();
  void move_negZ();

  void stop_X();
  void stop_Y();
  void stop_Z();

  void stop_all();

    /*! Sleep until motion completed. */
  void wait( void );

    /*! Moves all axis to the given limit(positive). */
  void go_to_reference( bool positive, int speed )
    { Robot->go_to_reference( positive, speed ); };

  bool modify_shape(bool area, int forb_index, int job, int change);
  void modify_cuboid(Cuboid* cuboid, int job, int change);

  bool has_area();
  void set_Area(Shape *area);

  const deque<Shape*> &forbiddenAreas( void ) const { return ForbiddenAreas; };
  void add_forbidden(Shape *forbidden);
  bool del_forbidden_at_index(int index);
  void clear_forbidden();
  void set_safe_distance(int dist);
  void set_home(const Point &newHome);
  void set_fish_head(const Point &head);
  void set_fish_tail(const Point &tail);

  Point get_fish_head();
  Point get_fish_tail();
  Point pos( void ) const;
  bool axis_in_pos_limit(int axis);
  bool axis_in_neg_limit(int axis);

  Point get_home();
  Shape* get_area();
  int get_axis_length(int axis);

  double calc_speed(int axis, double speed, double dist,
		    double maxTime, double precision);
  double calculate_intern_time(int axis, double axisSpeed, double distance);

  Point calculate_times(const Point &speeds, const Point &dists);
  int how_many_move(const Point &position, const Point &coords);
  Point axis_speeds(double speed);
  double get_max(double a, double b, double c);


protected:

  Mirob *Robot;

  const int XLength = 650;
  const int YLength = 450;
  const int ZLength = 250;

  deque<Shape*> ForbiddenAreas;

  Shape* area = NULL;
  Point fish_head;
  Point fish_tail;

  bool wasStarted = false;
  int maxSafeDist = 10;
  Point home = Point(0,0,0);

};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_XYZROBOT_H_ */
