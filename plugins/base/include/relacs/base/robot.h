/*
  base/robot.h
  Control a robot

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

#ifndef _RELACS_BASE_ROBOT_H_
#define _RELACS_BASE_ROBOT_H_ 1

//#include <relacs/manipulator.h>
#include <relacs/misc/mirob.h>
#include <relacs/optwidget.h>
#include <relacs/control.h>
#include <QLCDNumber>
#include <QVBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>
#include <QTextEdit>
using namespace relacs;

namespace base {


/*!
\class Robot
\brief [Control] Control a robot
\author Jan Benda/ Fabian Sinz
\version 1.0 (Mar 05, 2010)
*/


class Robot : public Control
{
  Q_OBJECT

public:

  Robot( void );
  ~Robot( void );
  virtual void initDevices( void );

public slots:
  void clampTool(void);
  void releaseTool(void);

  void restartWatchdog(void);
  void stopMirob(void); 
  void startTrajRec(void);
  void recordTrajStep(void);
  void stopTrajRec(void);
  void apply(void);

protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void keyReleaseEvent( QKeyEvent *e );


  QLCDNumber *XPosLCD;
  QLCDNumber *YPosLCD;
  QLCDNumber *ZPosLCD;

  QComboBox* otherActionsBox;

  QTextEdit* errorBox;

  //Manipulator *Rob;
  ::misc::Mirob *Rob;

private:
  OptWidget SW;
  

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_ROBOT_H_ */
