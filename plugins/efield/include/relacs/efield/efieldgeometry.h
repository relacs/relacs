/*
  efield/efieldgeometry.h
  Measure electric field amplitude on a grid.

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

#ifndef _RELACS_EFIELD_EFIELDGEOMETRY_H_
#define _RELACS_EFIELD_EFIELDGEOMETRY_H_ 1

#include <vector>
#include <QLCDNumber>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <relacs/point.h>
#include <relacs/misc/xyzrobot.h>
#include <relacs/repro.h>

using namespace relacs;

namespace efield {


/*!
\class EFieldGeometry
\brief [RePro] Measure electric field amplitude on a grid.
\author Alexander Ott
\version 1.0 (Jun 19, 2017)
*/


class EFieldGeometry : public RePro
{
  Q_OBJECT

public:

  EFieldGeometry( void );

  void keyPressEvent(QKeyEvent *e);
  void keyReleaseEvent(QKeyEvent *e);

  bool build_grid();
  bool point_safe(const Point &p);

  virtual int main( void );


public slots:

  void slot_pause();
  void slot_continue();
  void slot_stop();


protected:

  void customEvent( QEvent *qce );

  bool paused = false;

  QLCDNumber *totalPoints;
  QLCDNumber *currentPoint;
  QTextEdit *textBox;
  QPushButton *stopButton;
  QPushButton *pauseButton;
  QPushButton *continueButton;

  bool contMoving = false;

  /*
  QComboBox *homeBox;
  QSpinBox *posXBox;
  QSpinBox *posYBox;
  QSpinBox *posZBox;
  QComboBox *calibrateBox;
  QSpinBox *stepLengthBox;
  QTextEdit *errorBox;
*/
  vector<Point> grid;

  misc::XYZRobot *robot_control;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_EFIELDGEOMETRY_H_ */
