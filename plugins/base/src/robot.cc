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



  // buttons:
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  StateGroup = new QButtonGroup();
  vModeButton = new QRadioButton("Free steering mode");
  posModeButton = new QRadioButton("Positioning mode");
  StateGroup->addButton(vModeButton);
  StateGroup->addButton(posModeButton);
  vModeButton->setChecked(true);

  connect( vModeButton, SIGNAL( clicked() ),
	   this, SLOT( changeMode() ) );
  connect( posModeButton, SIGNAL( clicked() ),
	   this, SLOT( changeMode() ) );

  bb->addWidget(vModeButton);
  bb->addWidget(posModeButton);

  //---------------------------------------------------------
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

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

  bb->addWidget(rawCoordButton);
  bb->addWidget(transCoordButton);


  QFrame* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  vb->addWidget(line);
 
  // sliders for  velocity
  QGridLayout *Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 2 );
  Positions->setVerticalSpacing( 4 );
  vb->addLayout( Positions );

  label = new QLabel( "x velocity");
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 0 );
  vX = new QSlider(Qt::Horizontal);
  vX->setEnabled(false);
  Positions->addWidget(vX,1,0);

  label = new QLabel( "y velocity");
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 1 );
  vY = new QSlider(Qt::Horizontal);
  vY->setEnabled(false);

  Positions->addWidget(vY,1,1);

  label = new QLabel( "z velocity");
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 2 );
  vZ = new QSlider(Qt::Horizontal);
  vZ->setEnabled(false);
  Positions->addWidget(vZ,1,2);

  // position LCDs
  label = new QLabel( "x position");
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 2, 0 );
  posX = new QLCDNumber(6);
  posX->setDecMode();
  Positions->addWidget(posX,3,0);
  posX->setSegmentStyle( QLCDNumber::Filled );
  posX->setPalette( qp );
  posX->setAutoFillBackground( true );

  label = new QLabel( "y position");
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 2, 1 );
  posY = new QLCDNumber(6);
  posY->setDecMode();
  Positions->addWidget(posY,3,1);
  posY->setSegmentStyle( QLCDNumber::Filled );
  posY->setPalette( qp );
  posY->setAutoFillBackground( true );

  label = new QLabel( "z position");
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 2, 2 );
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
  Positions->addWidget(setPosX,4,0);
  
  setPosY = new QDoubleSpinBox;
  setPosY->setMaximum(1e6);
  setPosY->setMinimum(-1e6);
  Positions->addWidget(setPosY,4,1);

  setPosZ = new QDoubleSpinBox;
  setPosZ->setMaximum(1e6);
  setPosZ->setMinimum(-1e6);
  Positions->addWidget(setPosZ,4,2);

  updatePos = new QPushButton("update position");
  Positions->addWidget(updatePos,5,1);
  connect( updatePos, SIGNAL( clicked() ),
	   this, SLOT( updatePositions() ) );
  

  // start timer
  Timer = startTimer(30);
  







}

Robot::~Robot(void){
  killTimer(Timer);
  Timer = 0;
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
    }
  }
}

void Robot::updatePositions(void){
  if (Rob != 0){
    Rob->setPos(setPosX->value(),setPosY->value(),setPosZ->value(),50);
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


}

void Robot::timerEvent(QTimerEvent*)
{
  posX->display(Rob->pos(1));
  posY->display(Rob->pos(2));
  posZ->display(Rob->pos(3));
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
