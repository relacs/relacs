/*
  efield/calibraterobot.h
  Calibrate forbitten zones of an xyz robot.

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

#ifndef _RELACS_EFIELD_CALIBRATEROBOT_H_
#define _RELACS_EFIELD_CALIBRATEROBOT_H_ 1


#include <QLCDNumber>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <relacs/point.h>
#include <relacs/shape.h>
#include <relacs/misc/xyzrobot.h>
#include <relacs/repro.h>

using namespace relacs;

namespace efield {


/*!
\class CalibrateRobot
\brief [RePro] Calibrate forbitten zones of an xyz robot.
\author Alexander Ott
\version 1.0 (May 19, 2017)
*/


class CalibrateRobot : public RePro
{
  Q_OBJECT

public:

  CalibrateRobot( void );
  ~CalibrateRobot( void );

  virtual int main( void );

  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);
  void customEvent(QEvent *qce);


public slots:

  void home_apply();
  void pos_apply();
  void slot_stop();
  void slot_modify();

  void calibrate();


 private:

  bool toggle = true;
  Shape* calibrate_area();

  QComboBox *homeBox;
  QSpinBox *posXBox;
  QSpinBox *posYBox;
  QSpinBox *posZBox;
  QComboBox *calibrateBox;
  QComboBox *modifyAreasBox;
  QComboBox *modifyChoicesBox;
  QSpinBox *modifyLengthBox;
  QTextEdit *errorBox;

  misc::XYZRobot *robot_control;

  // Flags: 

  bool gui_thread_init = false;
  bool main_thread_init = false;
  bool keyboard_active = false;

  bool cont = false;
  bool back = false;
  bool cali_area = false;
  bool cali_forb = false;
  bool cali_fish = false;
  bool stop_cali = false;
  bool start_test = false;

  void test_area();
  Point calibrate_point();

  void  test_point_distances();

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_CALIBRATEROBOT_H_ */
