/*
  efield/robotfield.h
  Plugin to measure the field geometry using a robot for eletrode positioning.

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

#ifndef _RELACS_EFIELD_ROBOTFIELD_H_
#define _RELACS_EFIELD_ROBOTFIELD_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/efield/traces.h>
#include <relacs/misc/mirob.h>
#include <QPushButton>
#include <QStackedLayout>
#include <QLabel>
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QList>


using namespace relacs;
using namespace misc;
using namespace std; 



namespace efield {


/*!
\class RobotField
\brief [RePro] Plugin to measure the field geometry using a robot for eletrode positioning.
\author Fabian Sinz/ Jan Benda
\version 1.0 (Nov 27, 2012)
*/


class RobotField
  : public RePro,
    public efield::Traces
{
  Q_OBJECT

public:

  RobotField( void );
  virtual int main( void );

 public slots:
  void setLandMark();

 protected:
  void customEvent( QEvent *qce );
  void saveData( const SampleData< SampleDataD > &results );

 private:
  ::misc::Mirob *Rob;
  QPushButton *Set;
  QStackedLayout *Stack;
  QGraphicsScene *GoWhere;
  QGraphicsView *View;
  QLabel *Instructions;
  QWidget *Init;
  Plot P;
  int LandmarkCounter;
  QPen* pen;
  PositionUpdate *Landmarks[3];
  double resultion;
  QList< QGraphicsEllipseItem* > Dots;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_ROBOTFIELD_H_ */
