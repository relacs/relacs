/*
  efish/robottofishposition.h
  move the robot in fish coordinates

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

#ifndef _RELACS_EFISH_ROBOTTOFISHPOSITION_H_
#define _RELACS_EFISH_ROBOTTOFISHPOSITION_H_ 1

#include <relacs/repro.h>
#include <relacs/misc/xyzrobot.h>
#include <relacs/point.h>
#include <QPushButton>
#include <atomic>
using namespace relacs;

namespace efish {


/*!
\class RobotToFishPosition
\brief [RePro] move the robot in fish coordinates
\author Jan Grewe
\version 1.0 (Aug 20, 2018)
*/


class RobotToFishPosition : public RePro
{
  Q_OBJECT

public slots:
  void go( void );
  void cancel( void );

public:

  RobotToFishPosition( void );
  virtual int main( void );

private:
  misc::XYZRobot *robot = NULL;
  std::atomic<bool> done;
  std::atomic<bool> start;
  QPushButton *goBtn;
  QPushButton *cancelBtn;
  std::vector<int> axis_map;
  std::vector<int> axis_invert;
  bool moveToPosition( );
  double getYSlope( );
};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_ROBOTTOFISHPOSITION_H_ */
