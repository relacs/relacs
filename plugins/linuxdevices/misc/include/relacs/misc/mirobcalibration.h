/*
  misc/mirobcalibration.h
  Repro to calibrate the coordinate system of Mirob.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_MISC_MIROBCALIBRATION_H_
#define _RELACS_MISC_MIROBCALIBRATION_H_ 1

#include <relacs/repro.h>
#include <relacs/misc/mirob.h>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QComboBox>

using namespace relacs;

namespace misc {


/*!
\class MirobCalibration
\brief [RePro] Repro to calibrate the coordinate system of Mirob.
\author Fabian Sinz
\version 1.0 (Nov 15, 2012)
*/


class MirobCalibration : public RePro
{
  Q_OBJECT

 public:

  MirobCalibration( void );
  virtual int main( void );

 public slots:
  void calibDone();
  void goTo();
  void setValue();
  void basisItemChanged(QTableWidgetItem *);
  void setStart();
  void runTrajectoryFromHere();
  void go2Start();
 private:
    ::misc::Mirob *Rob;
  QTableWidget* coordinateFrame;
  double B[3][3];
  double b0[3];
  QPushButton *Done, * go2, *Set, *SetStart, *ConvexHull, *Go2StartingPoint;
  bool done;
  QComboBox *Trajectories, *BasisVecs;
};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_MIROBCALIBRATION_H_ */
