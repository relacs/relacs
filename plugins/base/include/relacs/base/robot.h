/*
  base/robot.h
  Shows the state of a robot.

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

#ifndef _RELACS_BASE_ROBOT_H_
#define _RELACS_BASE_ROBOT_H_ 1

#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QLCDNumber>
#include <QComboBox>
#include <QTextEdit>
//additionally needed for RenderArea:
#include <vector>
#include <QRect>
#include <relacs/point.h>
#include <relacs/misc/xyzrobot.h>
#include <relacs/control.h>


using namespace relacs;

namespace base {


class RenderArea;


/*!
\class Robot
\brief [Control] Shows the state of a robot.
\author Alexander Ott
\version 1.0 (Jun 02, 2017)
*/

class Robot : public Control
{
  Q_OBJECT

public:

  Robot( void );
  ~Robot( void );
  virtual void main( void );

protected:

  void customEvent( QEvent *qce );
  QRect prepare_cuboid_plot(Cuboid* cuboid);
  bool test_height(Cuboid* cuboid);

  misc::XYZRobot *robot_control;

  RenderArea* plot;
  QComboBox* heightBox;
  QLCDNumber* xPos;
  QLCDNumber* yPos;
  QLCDNumber* zPos;
  QTextEdit* errorBox;

};


class RenderArea: public QWidget
{

 public:

   RenderArea(QWidget *parent = 0);

   QSize minimumSizeHint() const;
   QSize sizeHint() const;

   void setAllowed(QRect allowed);
   void addForbidden(QRect forb);
   void clearForbidden();
   void setPosition(const Point &p);


 protected:

   void paintEvent(QPaintEvent *event);


 private:

   QRect allowed = QRect(0,0,0,0);
   vector<QRect> forbidden;
   Point position;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_ROBOT_H_ */
