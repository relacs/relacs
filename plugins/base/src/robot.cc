/*
  base/robot.cc
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

#include <relacs/base/robot.h>
#include <relacs/relacsdevices.h>

#include <QGroupBox>

using namespace relacs;

#define SPEED  50
#define WAIT   true
#define DONTWAIT false

namespace base {


Robot::Robot( void )
  : Control( "Robot", "base", "Jan Benda", "1.0", "Mar 05, 2010" )
{
  //  Mirob robot object
  Rob = 0;
  

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  
  QHBoxLayout *bb;
  QGroupBox * gbb;

  setLayout( vb );
  vb->setSpacing( 4 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );


  /************** Positions ***********************/
  // base layout
  QLabel *label;
  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );



  // ------------ radio buttons -------------------:
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout(bb);
  QVBoxLayout *vb2 = new QVBoxLayout;

  gbb = new QGroupBox("Robot State");

  StateGroup = new QButtonGroup();
  vModeButton = new QRadioButton("free steering state");
  posModeButton = new QRadioButton("positioning state");
  haltModeButton = new QRadioButton("halt state");
  errModeButton = new QRadioButton("error state");
  errModeButton->setEnabled(false);
  StateGroup->addButton(vModeButton);
  StateGroup->addButton(posModeButton);
  StateGroup->addButton(haltModeButton);
  StateGroup->addButton(errModeButton);
  vModeButton->setChecked(true);

  connect( vModeButton, SIGNAL( clicked() ),
	   this, SLOT( changeMode() ) );
  connect( posModeButton, SIGNAL( clicked() ),
	   this, SLOT( changeMode() ) );
  connect( haltModeButton, SIGNAL( clicked() ),
	   this, SLOT( changeMode() ) );
  connect( errModeButton, SIGNAL( clicked() ),
	   this, SLOT( changeMode() ) );

  vb2->addWidget(vModeButton);
  vb2->addWidget(posModeButton);
  vb2->addWidget(haltModeButton);
  vb2->addWidget(errModeButton);
  gbb->setLayout(vb2);
  bb->addWidget(gbb);
  //---------------------------------------------------------
  gbb = new QGroupBox("Coordinate System");
  vb2 = new QVBoxLayout;
  CoordGroup = new QButtonGroup();
  rawCoordButton = new QRadioButton("Raw Coordinates");
  transCoordButton = new QRadioButton("Mirob Coordinates");
  CoordGroup->addButton(rawCoordButton);
  CoordGroup->addButton(transCoordButton);

  rawCoordButton->setChecked(true);

  connect( rawCoordButton, SIGNAL( clicked() ),
	   this, SLOT( changeCoordinateSystem() ) );
  connect( transCoordButton, SIGNAL( clicked() ),
	   this, SLOT( changeCoordinateSystem() ) );

  vb2->addWidget(rawCoordButton);
  vb2->addWidget(transCoordButton);
  gbb->setLayout(vb2);
  bb->addWidget(gbb);


  //----------- positioning -------------------

  gbb = new QGroupBox("Positioning");
  QGridLayout *Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 10 );
  Positions->setVerticalSpacing( 10 );
  vb->addWidget(gbb);


  gbb->setLayout(Positions);
  label = new QLabel( "velocity");
  label->setAlignment( Qt::AlignLeft );
  Positions->addWidget( label, 0, 1 );
  label = new QLabel( "actual");
  label->setAlignment( Qt::AlignLeft );
  Positions->addWidget( label, 0, 2 );
  label = new QLabel( "input");
  label->setAlignment( Qt::AlignLeft );
  Positions->addWidget( label, 0, 3 );
  label = new QLabel( "x");
  label->setAlignment( Qt::AlignLeft );
  Positions->addWidget( label, 1, 0 );
  label = new QLabel( "y");
  label->setAlignment( Qt::AlignLeft );
  Positions->addWidget( label, 2, 0 );
  label = new QLabel( "z");
  label->setAlignment( Qt::AlignLeft );
  Positions->addWidget( label, 3, 0 );



  vX = new QSlider(Qt::Horizontal);
  vX->setEnabled(false);
  Positions->addWidget(vX,1,1);

  vY = new QSlider(Qt::Horizontal);
  vY->setEnabled(false);
  Positions->addWidget(vY,2,1);

  vZ = new QSlider(Qt::Horizontal);
  vZ->setEnabled(false);
  Positions->addWidget(vZ,3,1);


  // position LCDs
 
  posX = new QLCDNumber(6);
  posX->setDecMode();
  Positions->addWidget(posX,1,2);
  posX->setSegmentStyle( QLCDNumber::Filled );
  posX->setPalette( qp );
  posX->setAutoFillBackground( true );
 
  posY = new QLCDNumber(6);
  posY->setDecMode();
  Positions->addWidget(posY,2,2);
  posY->setSegmentStyle( QLCDNumber::Filled );
  posY->setPalette( qp );
  posY->setAutoFillBackground( true );

  posZ = new QLCDNumber(6);
  posZ->setDecMode();
  Positions->addWidget(posZ,3,2);
  posZ->setSegmentStyle( QLCDNumber::Filled );
  posZ->setPalette( qp );
  posZ->setAutoFillBackground( true );

  // postion double spin boxes
  setPosX = new QDoubleSpinBox;
  setPosX->setMaximum(1e6);
  setPosX->setMinimum(-1e6);
  Positions->addWidget(setPosX,1,3);
  
  setPosY = new QDoubleSpinBox;
  setPosY->setMaximum(1e6);
  setPosY->setMinimum(-1e6);
  Positions->addWidget(setPosY,2,3);

  setPosZ = new QDoubleSpinBox;
  setPosZ->setMaximum(1e6);
  setPosZ->setMinimum(-1e6);
  Positions->addWidget(setPosZ,3,3);

  
  updatePos = new QPushButton("update position");
  Positions->addWidget(updatePos,4,1);
  connect( updatePos, SIGNAL( clicked() ),
	   this, SLOT( updatePositions() ) );

  transferPos = new QPushButton("transfer position");
  Positions->addWidget(transferPos,4,2);
  connect( transferPos, SIGNAL( clicked() ),
	   this, SLOT( transferPositions() ) );

  Stop = new QPushButton("STOP");
  Positions->addWidget(Stop,4,3);
  connect( Stop, SIGNAL( clicked() ),
	   this, SLOT( stopRobot() ) );

  Reset = new QPushButton("Reset");
  connect( Reset, SIGNAL( clicked() ),
	   this, SLOT( reset() ) );
  vb->addWidget(Reset);

  
  // --------- trajectories ------------
  gbb = new QGroupBox("Trajectories");
  QHBoxLayout *bb2 = new QHBoxLayout;
  Trajectories = new QComboBox();
  bb2->addWidget(Trajectories);

  RunTrajectory = new QPushButton("Run");
  bb2->addWidget(RunTrajectory);
  connect( RunTrajectory, SIGNAL( clicked() ),
	   this, SLOT( runCurrentTrajectory() ) );
  RunTrajectory->setStyleSheet("* { background-color: rgb(255,100,100) }");
  gbb->setLayout(bb2);
  vb->addWidget(gbb);



  // start timer
  Timer = startTimer(30);
  







}

Robot::~Robot(void){
  killTimer(Timer);
  Timer = 0;
}

void Robot::reset(void){
  if (Rob != 0){
    Rob->reset();
  }
}


void Robot::changeCoordinateSystem(void){
  if (Rob != 0){
    if (rawCoordButton->isChecked()){
      Rob->setCoordinateSystem(MIROB_COORD_RAW);
    }else if (transCoordButton->isChecked()){
      Rob->setCoordinateSystem(MIROB_COORD_TRANS);
    }
  }
}


void Robot::changeMode(void){
  if (Rob != 0){
    if (vModeButton->isChecked()){
      Rob->setState(ROBOT_FREE);
      setPosX->setEnabled(false);
      setPosY->setEnabled(false);
      setPosZ->setEnabled(false);
      updatePos->setEnabled(false);
    }else if (posModeButton->isChecked()){
      Rob->setState(ROBOT_POS);
      setPosX->setEnabled(true);
      setPosY->setEnabled(true);
      setPosZ->setEnabled(true);
      updatePos->setEnabled(true);
    }else if (haltModeButton->isChecked()){
      Rob->stop();
      updatePos->setEnabled(false);
    }
  }
}

void Robot::updatePositions(void){
  if (Rob != 0){
    double v[3];
    v[0] = vX->value();
    v[1] = vY->value();
    v[2] = vZ->value();
    double speed = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    Rob->setPos(setPosX->value(),setPosY->value(),setPosZ->value(),speed);
  }

}

void Robot::transferPositions(void){
  if (Rob != 0){
    setPosX->setValue(posX->value());
    setPosY->setValue(posY->value());
    setPosZ->setValue(posZ->value());
    
  }
}


void Robot::initDevices( void )
{
  //cerr << *devices() << '\n';
  unsigned int k   ;
  for (  k=0; k<10; k++ ) {
     Str ns( k+1, 0 );
     Rob = dynamic_cast< ::misc::Mirob* >( device( "robot-" + ns ) );
     if (Rob != 0)
       break;
  }
  
  // set the slider values after ROB was initialized
  vX->setMaximum((int)Rob->getMaxSpeed());
  vX->setMinimum(0);
  vX->setValue((int)Rob->getMaxSpeed()/3);
  vY->setMaximum((int)Rob->getMaxSpeed());
  vY->setMinimum(0);
  vY->setValue((int)Rob->getMaxSpeed()/3);
  vZ->setMaximum((int)Rob->getMaxSpeed());
  vZ->setMinimum(0);
  vZ->setValue((int)Rob->getMaxSpeed()/3);
  vX->setEnabled(true);
  vY->setEnabled(true);
  vZ->setEnabled(true);

  // set state
  Rob->setState(ROBOT_FREE);
  vModeButton->setChecked(true);
  setPosX->setEnabled(false);
  setPosY->setEnabled(false);
  setPosZ->setEnabled(false);
  updatePos->setEnabled(false);

  //set trajectories
  vector<string> trajNames = Rob->getTrajectoryKeys();
  for (vector<string>::iterator it=trajNames.begin() ; it < trajNames.end(); it++ ){
    Trajectories->addItem(QString( it->c_str()));
  }

}

void Robot::stopRobot(){
  if (Rob != 0){
    Rob->stop();
  }

}




void Robot::timerEvent(QTimerEvent*)
{
  posX->display(Rob->pos(1));
  posY->display(Rob->pos(2));
  posZ->display(Rob->pos(3));
  
  if (Rob != 0){
    int state = Rob->getState();
    if (state == ROBOT_FREE){
      vModeButton->setChecked(true);
      rawCoordButton->setChecked(true);
      transCoordButton->setEnabled(false);
      Rob->setCoordinateSystem(MIROB_COORD_RAW);
    }else if (state == ROBOT_POS){
      transCoordButton->setEnabled(true);
      posModeButton->setChecked(true);
    }else if (state == ROBOT_HALT){
      transCoordButton->setEnabled(true);
      haltModeButton->setChecked(true);
    }else if (state == ROBOT_ERR){
      transCoordButton->setEnabled(true);
      errModeButton->setChecked(true);
    }else{
      transCoordButton->setEnabled(false);
      errModeButton->setChecked(true);
      vModeButton->setChecked(false);
      posModeButton->setChecked(false);
      haltModeButton->setChecked(false);
    }

    //-----------
    int coord = Rob->getCoordinateSystem();
    if (coord == MIROB_COORD_TRANS){
      transCoordButton->setChecked(true);
    }else if (coord == MIROB_COORD_RAW){
      rawCoordButton->setChecked(true);
    }else{
      transCoordButton->setChecked(false);
      rawCoordButton->setChecked(false);
    }

    if (!Rob->isCalibrated()){
      RunTrajectory->setEnabled(false);
      RunTrajectory->setStyleSheet("* { background-color: rgb(255,100,100) }");
  
    }else{
     
      if (Rob->trajectoryCalibrated(Trajectories->currentText().toStdString())){
	RunTrajectory->setEnabled(true);
	RunTrajectory->setStyleSheet("* { background-color: rgb(100,255,100) }");
	
      }else{
	RunTrajectory->setEnabled(false);
	RunTrajectory->setStyleSheet("* { background-color: rgb(255,100,100) }");
      }

    }

    
  }

}


void Robot::runCurrentTrajectory(void){
  if (Rob != 0){
    Rob->runTrajectory(Trajectories->currentText().toStdString());
  }
}


void Robot::keyReleaseEvent(QKeyEvent* e){
  /* only accept the event if it is not from a autorepeat key */
  if(e->isAutoRepeat() ) {    
    e->ignore();   
  } else {    
    e->accept();
  
    switch ( e->key() ) {



    case Qt::Key_U:
      if ( Rob != 0 ){
	Rob->setVZ(0 );
      }
      break;

    case Qt::Key_D:
      if ( Rob != 0 ){
	Rob->setVZ(0 );
      }
      break;

    case Qt::Key_Up:
      if ( Rob != 0 )
	Rob->setVX(0 );

      break;

    case Qt::Key_Down:
      if ( Rob != 0 )
	Rob->setVX(0);
      break;

    case Qt::Key_Left:
      if ( Rob != 0 )
	Rob->setVY( 0 );
      break;

    case Qt::Key_Right:
      if ( Rob != 0 )
	Rob->setVY( 0 );
      break;


    default:
      Control::keyReleaseEvent( e );

    }
  }
  
} 



void Robot::keyPressEvent( QKeyEvent *e )
{
  if(e->isAutoRepeat() ) {    
    e->ignore();   
  } else {    
    e->accept();
  
  
    switch ( e->key() ) {

    case Qt::Key_T:
      if ( Rob != 0 ){
	Rob->switchClampState();
      }
      break;


    case Qt::Key_U:
      if ( Rob != 0 ){
	Rob->setVZ(-vZ->value() );
      }
      break;

    case Qt::Key_D:
      if ( Rob != 0 ){
	Rob->setVZ( vZ->value() );
      }

      break;

    case Qt::Key_Up:
      if ( Rob != 0 )
	Rob->setVX(-vX->value() );
      break;

    case Qt::Key_Down:
      if ( Rob != 0 )
	Rob->setVX(vX->value());
      break;

    case Qt::Key_Left:
      if ( Rob != 0 )
	Rob->setVY( vY->value() );
      break;

    case Qt::Key_Right:
      if ( Rob != 0 )
	Rob->setVY( -vY->value() );
      break;


    default:
      Control::keyPressEvent( e );

    }
  }
}


addControl( Robot, base );

}; /* namespace base */

#include "moc_robot.cc"
