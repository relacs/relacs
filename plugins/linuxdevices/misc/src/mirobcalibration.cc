/*
  misc/mirobcalibration.cc
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

#include <relacs/misc/mirobcalibration.h>
using namespace relacs;

#include <QString>
#include <QStringList>
#include <QHeaderView>
#include <QTableWidgetItem>
namespace misc {


MirobCalibration::MirobCalibration( void )
  : RePro( "MirobCalibration", "misc", "Fabian Sinz", "1.0", "Nov 15, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );

  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *bb = new QHBoxLayout;
  setLayout( vb );
  vb->setSpacing( 4 );
  vb->addLayout(bb);




  // show basis
  coordinateFrame = new QTableWidget(3, 4);
  QStringList labels;
  labels << tr("BV 1") << tr("BV 2") << tr("BV 3") << tr("offspring");
  coordinateFrame->setHorizontalHeaderLabels(labels);
  // coordinateFrame->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch); // Interactive, Fixed, Stretch, ResizeToContents, Custom

  labels.clear();
  labels << tr("x") << tr("y") << tr("z") ;
  coordinateFrame->setVerticalHeaderLabels(labels);
  // coordinateFrame->verticalHeader()->setResizeMode(0, QHeaderView::Stretch);
  coordinateFrame->setShowGrid(false);

  connect(coordinateFrame,
    SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(basisItemChanged(QTableWidgetItem *)));

  bb->addWidget(coordinateFrame);

  //----------------

  QGridLayout *Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 2 );
  Positions->setVerticalSpacing( 2 );
  bb->addLayout( Positions );

  qB0 = new QRadioButton("offspring");
  qB0->setChecked(true);
  qB1 = new QRadioButton("basis vec. 1");
  qB2 = new QRadioButton("basis vec. 2");
  qB3 = new QRadioButton("basis vec. 3");
  
  Positions->addWidget(qB0,0,0);
  Positions->addWidget(qB1,1,0);
  Positions->addWidget(qB2,2,0);
  Positions->addWidget(qB3,3,0);
  


  // -------------------
  Set = new QPushButton("Set");
  connect( Set, SIGNAL( clicked() ),
	   this, SLOT( setValue() ) );
  Positions->addWidget(Set, 4, 0);  
  // -------------------
  toOffspring = new QPushButton("go to offspring");
  connect( toOffspring, SIGNAL( clicked() ),
	   this, SLOT( gotoOffspring() ) );
  Positions->addWidget(toOffspring, 5,0);

  // -------------------
  Set = new QPushButton("Trace Out Coord. System");
  connect( Set, SIGNAL( clicked() ),
	   this, SLOT( traceOutCoord() ) );
  Positions->addWidget(Set, 6,0);  


  // -------------------
  Done = new QPushButton("Done");
  connect( Done, SIGNAL( clicked() ),
  	   this, SLOT( calibDone() ) );
  vb->addWidget(Done);

}


int MirobCalibration::main( void )
{
  done = false;
  unlockData();

  // get the robot device
  unsigned int k   ;
  for (  k=0; k<10; k++ ) {
    Str ns( k+1, 0 );
     Rob = dynamic_cast< ::misc::Mirob* >( device( "robot-" + ns ) );
     if (Rob != 0){
       break;
     }
  }
  if (Rob == 0){
    printlog("No robot found");
    return 1;
  }
  // -----------------
  // set coordinate frame to table
  double basis[3][3];
  double offspring[3];
  Rob->getCoordinateFrame(basis, offspring);
  coordinateFrame->setRowCount(3);
  coordinateFrame->setColumnCount(4);

  for (int ridx = 0 ; ridx < 3; ridx++ ){
    for (int cidx = 0 ; cidx < 3; cidx++){
      QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(basis[ridx][cidx],4));
      coordinateFrame->setItem(ridx,cidx,item);
    }
    QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(offspring[ridx],4));
    coordinateFrame->setItem(ridx,3,item);
  }

  // -----------------

  while (!done){
    if ( interrupt() ){
      readLockData();
      return Aborted;
    }
    usleep(100000);
  }


  // get options:
  // double duration = number( "duration" );
  return Completed;
}

  //------------------------------------------------------
void MirobCalibration::calibDone(void){
  done = true;
}
  //------------------------------------------------------
void MirobCalibration::setValue(void){
  int mode = Rob->getCoordinateSystem();
  Rob->setCoordinateSystem(MIROB_COORD_RAW);

  int col = -1;
  double offspring[3];
  double basis[3][3];
  Rob->getCoordinateFrame(basis,offspring);

  if (qB0->isChecked()){
    offspring[0] = (double)Rob->posX();
    offspring[1] = (double)Rob->posY();
    offspring[2] = (double)Rob->posZ();

  }else{
    if (qB1->isChecked()){
      col = 0;
    }else if (qB2->isChecked()){
      col = 1;
    }else if (qB3->isChecked()){
      col = 2;
    }
    
    basis[0][col] = (double)Rob->posX() - offspring[0];
    basis[1][col] = (double)Rob->posY() - offspring[1];
    basis[2][col] = (double)Rob->posZ() - offspring[2];
  }



  Rob->setCoordinateFrame(basis,offspring);

  Rob->getCoordinateFrame(basis,offspring);
  for (int ridx = 0 ; ridx < 3; ridx++ ){
    for (int cidx = 0 ; cidx < 3; cidx++){
      QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(basis[ridx][cidx],4));
      coordinateFrame->setItem(ridx,cidx,item);
    }
    QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(offspring[ridx],4));
    coordinateFrame->setItem(ridx,3,item);
  }


  Rob->setCoordinateSystem(mode);

  
}

//------------------------------------------------------
void MirobCalibration::traceOutCoord(){
  Rob->setState(ROBOT_POS);
  Rob->setCoordinateSystem(MIROB_COORD_TRANS);
  Rob->setPos(0,0,0);
  Rob->setPos(1,0,0);
  Rob->setPos(0,0,0);
  Rob->setPos(0,1,0);
  Rob->setPos(0,0,0);
  Rob->setPos(0,0,1);
  Rob->setPos(0,0,0);
}


//------------------------------------------------------
void MirobCalibration::gotoOffspring(){
  Rob->setState(ROBOT_POS);
  double offspring[3];
  double basis[3][3];
  Rob->getCoordinateFrame(basis,offspring);

  Rob->setCoordinateSystem(MIROB_COORD_RAW);
  Rob->setPos(offspring[0], offspring[1], offspring[2]);
}

//------------------------------------------------------

void MirobCalibration::basisItemChanged(QTableWidgetItem * item){
  double offspring[3];
  double basis[3][3];
  Rob->getCoordinateFrame(basis,offspring);
  
  for (int i = 0; i < 3; ++i){
    for (int j = 0; j < 3; ++j){
      basis[i][j] = coordinateFrame->item(i, j)->text().toDouble();
    }
    offspring[i] = coordinateFrame->item(i, 3)->text().toDouble();
  }
  Rob->setCoordinateFrame(basis,offspring);

}

addRePro( MirobCalibration, misc );

}; /* namespace misc */

#include "moc_mirobcalibration.cc"
