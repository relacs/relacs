/*
  efish/movingobjects.h
  Use the robot to present moving objects as stimuli.

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

#ifndef _RELACS_EFISH_MOVINGOBJECTS_H_
#define _RELACS_EFISH_MOVINGOBJECTS_H_ 1

#include <relacs/repro.h>
#include <relacs/linearrange.h>
#include <relacs/plot.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/outdata.h>
#include <relacs/misc/xyzrobot.h>
#include <relacs/point.h>

using namespace relacs;

namespace efish {


/*!
\class MovingObjects
\brief [RePro] Use the robot to present moving objects as stimuli.
\author Jan Grewe
\version 1.0 (Sep 10, 2018)
*/


class MovingObjects : public RePro,
  public efield::Traces,
  public ephys::Traces
{
  Q_OBJECT

public:

  MovingObjects( void );
  virtual int main( void );

private:
  misc::XYZRobot *robot = NULL;
  Point fish_head, fish_tail, reset_position;
  std::vector<int> axis_map;
  std::vector<int> axis_invert;

  bool moveToPosition( const Point &p );
  Point convertAxes( const Point &p );

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_MOVINGOBJECTS_H_ */
