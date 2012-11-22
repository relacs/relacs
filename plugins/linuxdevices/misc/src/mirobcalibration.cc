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
#include <QGroupBox>

namespace misc {


MirobCalibration::MirobCalibration( void )
  : RePro( "MirobCalibration", "misc", "Fabian Sinz", "1.0", "Nov 15, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );

  QVBoxLayout *vb = new QVBoxLayout;
  setLayout(vb);
  QGroupBox * gbb;

  //--------------- coordinate frame part ---------------------------
  gbb = new QGroupBox("Coordinate Frame");
  vb->addWidget(gbb);
  QVBoxLayout *vb2 = new QVBoxLayout;
  QGridLayout *Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 10 );
  Positions->setVerticalSpacing( 10 );
  vb2->addLayout( Positions );  
  gbb->setLayout(vb2);
  
  // coordinate frame 
  coordinateFrame = new QTableWidget(3, 4);
  QStringList labels;
  labels << tr("BV 1") << tr("BV 2") << tr("BV 3") << tr("offspring");
  coordinateFrame->setHorizontalHeaderLabels(labels);

  labels.clear();
  labels << tr("x") << tr("y") << tr("z") ;
  coordinateFrame->setVerticalHeaderLabels(labels);
  coordinateFrame->setShowGrid(false);

  connect(coordinateFrame,
    SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(basisItemChanged(QTableWidgetItem *)));

  Positions->addWidget(coordinateFrame,0,0,4,1);
  
  // basis items
  BasisVecs = new QComboBox();
  BasisVecs->addItem("offspring",0);
  BasisVecs->addItem("x - basis vector",1);
  BasisVecs->addItem("y - basis vector",2);
  BasisVecs->addItem("z - basis vector",3);
  Positions->addWidget(BasisVecs,0,1);


  // buttons
  // -------------------
  Set = new QPushButton("Set");
  connect( Set, SIGNAL( clicked() ),
	   this, SLOT( setValue() ) );
  Positions->addWidget(Set, 1, 1);  
  // -------------------
  go2 = new QPushButton("go to");
  connect( go2, SIGNAL( clicked() ),
	   this, SLOT( goTo() ) );
  Positions->addWidget(go2, 2,1);

  //--------------- trajectory part ---------------------------
  gbb = new QGroupBox("Trajectories");
  vb->addWidget(gbb);
  vb2 = new QVBoxLayout;
  Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 10 );
  Positions->setVerticalSpacing( 10 );
  vb2->addLayout( Positions );  
  gbb->setLayout(vb2);

  
  Trajectories = new QComboBox();
  Positions->addWidget(Trajectories,0,0,1,2);

  Go2StartingPoint = new QPushButton("Go To Starting Point");
  connect( Go2StartingPoint, SIGNAL( clicked() ),
	   this, SLOT( go2Start() ) );
  Positions->addWidget(Go2StartingPoint, 1, 0);  


  SetStart = new QPushButton("Set Start");
  connect( SetStart, SIGNAL( clicked() ),
	   this, SLOT( setStart() ) );
  Positions->addWidget(SetStart, 2, 0);  

  SetAnchor = new QPushButton("Set Anchor");
  connect( SetAnchor, SIGNAL( clicked() ),
	   this, SLOT( setAnchor() ) );
  Positions->addWidget(SetAnchor, 2, 1);  


  ConvexHull = new QPushButton("Run Trajectory From Here");
  connect( ConvexHull, SIGNAL( clicked() ),
	   this, SLOT( runTrajectoryFromHere() ) );
  Positions->addWidget(ConvexHull, 1, 1);  




  // -------      Done Button    ------------
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

  // load trajectories
  vector<string> trajNames = Rob->getTrajectoryKeys();
  Trajectories->clear();
  for (vector<string>::iterator it=trajNames.begin() ; it < trajNames.end(); it++ ){
    Trajectories->addItem(QString( it->c_str()));
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

  Rob->setCalibrated(true);
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

  if(BasisVecs->currentIndex() == 0){
    offspring[0] = (double)Rob->posX();
    offspring[1] = (double)Rob->posY();
    offspring[2] = (double)Rob->posZ();
  }else{
    if (BasisVecs->currentIndex() == 1){
      col = 0;
    }else if (BasisVecs->currentIndex() == 2){
      col = 1;
    }else if (BasisVecs->currentIndex() == 3){
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
void MirobCalibration::goTo(){
  Rob->setState(ROBOT_POS);
  double offspring[3];
  double basis[3][3];
  Rob->getCoordinateFrame(basis,offspring);

  Rob->setCoordinateSystem(MIROB_COORD_RAW);

  if(BasisVecs->currentIndex() == 0){
    Rob->setPos(offspring[0], offspring[1], offspring[2]);
  }else{
    int col = 0;
    if (BasisVecs->currentIndex() == 1){
      col = 0;
    }else if (BasisVecs->currentIndex() == 2){
      col = 1;
    }else if (BasisVecs->currentIndex() == 3){
      col = 2;
    }
    Rob->setPos(offspring[0]+basis[0][col], offspring[1]+basis[0][col], offspring[2]+basis[0][col]);

  }

}

//------------------------------------------------------
void MirobCalibration::setStart(){
  Rob->setState(ROBOT_POS);
  Rob->setCoordinateSystem(MIROB_COORD_TRANS);
  Rob->setTrajectoryStart(Trajectories->currentText().toStdString(),
			  Rob->posX(), Rob->posY(), Rob->posZ() );
}

//------------------------------------------------------
void MirobCalibration::setAnchor(){
  Rob->setState(ROBOT_POS);
  Rob->setCoordinateSystem(MIROB_COORD_TRANS);
  Rob->setTrajectoryAnchor(Trajectories->currentText().toStdString(),
			  Rob->posX(), Rob->posY(), Rob->posZ() );

}


//------------------------------------------------------
void MirobCalibration::runTrajectoryFromHere(){
  Rob->setCoordinateSystem(MIROB_COORD_TRANS);
  bool calib = Rob->isCalibrated();
  Rob->setCalibrated(true);
  Rob->runTrajectory(Trajectories->currentText().toStdString(), Rob->posX(), Rob->posY(), Rob->posZ());
  Rob->setCalibrated(calib);
}

//------------------------------------------------------
void MirobCalibration::go2Start(){
  Rob->setCoordinateSystem(MIROB_COORD_TRANS);
  bool calib = Rob->isCalibrated();
  Rob->setCalibrated(true);
  Rob->goToTrajectoryStart(Trajectories->currentText().toStdString());
  Rob->setCalibrated(calib);
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

  // load trajectories
  vector<string> trajNames = Rob->getTrajectoryKeys();
  for (vector<string>::iterator it=trajNames.begin() ; it < trajNames.end(); it++ ){
    Rob->setTrajectoryCalibrated( *it, false);
  }


}




addRePro( MirobCalibration, misc );

}; /* namespace misc */

#include "moc_mirobcalibration.cc"
