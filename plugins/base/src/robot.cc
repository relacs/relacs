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
  setLayout( vb );
  vb->setSpacing( 4 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );

  QGridLayout *Positions = new QGridLayout;
  Positions->setHorizontalSpacing( 2 );
  Positions->setVerticalSpacing( 2 );
  vb->addLayout( Positions );
  QLabel *label;

  label = new QLabel( "Mirob X-Position " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 0 );
 
  XPosLCD = new QLCDNumber( 8 );
  XPosLCD->setSegmentStyle( QLCDNumber::Filled );
  XPosLCD->setFixedHeight( label->sizeHint().height() );
  QColor fg( Qt::green );
  QColor bg( Qt::black );

  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );
  XPosLCD->setPalette( qp );
  XPosLCD->setAutoFillBackground( true );
  Positions->addWidget( XPosLCD, 0, 1 );

  label = new QLabel( "Mirob Y-Position " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 1, 0 );
 
  YPosLCD = new QLCDNumber( 8 );
  YPosLCD->setSegmentStyle( QLCDNumber::Filled );
  YPosLCD->setFixedHeight( label->sizeHint().height() );
  YPosLCD->setPalette( qp );
  YPosLCD->setAutoFillBackground( true );
  Positions->addWidget( YPosLCD, 1, 1 );

  label = new QLabel( "Mirob Z-Position " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 2, 0 );
 
  ZPosLCD = new QLCDNumber( 8 );
  ZPosLCD->setSegmentStyle( QLCDNumber::Filled );
  ZPosLCD->setFixedHeight( label->sizeHint().height() );
  ZPosLCD->setPalette( qp );
  ZPosLCD->setAutoFillBackground( true );
  Positions->addWidget( ZPosLCD, 2, 1 );




  // buttons:
  QHBoxLayout *bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  QFrame* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);

  vb->addWidget(line);
 


  QPushButton *StopButton = new QPushButton( "Stop!" );
  bb->addWidget( StopButton );
  StopButton->setFixedHeight( StopButton->sizeHint().height() );
  connect( StopButton, SIGNAL( clicked() ),
	   this, SLOT( stopMirob() ) );




  vb->addWidget(line);

  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  QPushButton *ClampButton = new QPushButton( "Clamp Tool" );
  bb->addWidget( ClampButton );
  ClampButton->setFixedHeight( ClampButton->sizeHint().height() );
  connect( ClampButton, SIGNAL( clicked() ),
	   this, SLOT( clampTool() ) );

  QPushButton *ReleaseButton = new QPushButton( "Release Tool" );
  bb->addWidget( ReleaseButton );
  ReleaseButton->setFixedHeight( ReleaseButton->sizeHint().height() );
  connect( ReleaseButton, SIGNAL( clicked() ),
	   this, SLOT( releaseTool() ) );

  
//   vb->addWidget(line);

//   bb = new QHBoxLayout;
//   bb->setSpacing( 4 );
//   vb->addLayout( bb );

//   QPushButton *TrajRecButton = new QPushButton( "Start Trajectory\nRecording" );
//   bb->addWidget( TrajRecButton );
//   TrajRecButton->setFixedHeight( TrajRecButton->sizeHint().height() );
//   connect( TrajRecButton, SIGNAL( clicked() ),
// 	   this, SLOT( startTrajRec() ) );

//   QPushButton *StepRecButton = new QPushButton( "Record\nStep" );
//   bb->addWidget( StepRecButton );
//   StepRecButton->setFixedHeight( StepRecButton->sizeHint().height() );
//   connect( StepRecButton, SIGNAL( clicked() ),
// 	   this, SLOT( recordTrajStep() ) );
  
//   QPushButton *TrajStopButton = new QPushButton( "Stop Trajectory\nRecording" );
//   bb->addWidget( TrajStopButton );
//   TrajStopButton->setFixedHeight( TrajStopButton->sizeHint().height() );
//   connect( TrajStopButton, SIGNAL( clicked() ),
// 	   this, SLOT( stopTrajRec() ) );


  
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );
  otherActionsBox = new QComboBox;
  bb->addWidget(otherActionsBox);
  otherActionsBox->addItem("Go to -lim and set home");
  otherActionsBox->addItem("Restart watchdog");
  otherActionsBox->addItem("Go home");

  QPushButton *ApplyButton = new QPushButton( "Apply" );
  bb->addWidget( ApplyButton );
  ApplyButton->setFixedHeight( ApplyButton->sizeHint().height() );
  connect( ApplyButton, SIGNAL( clicked() ),
	   this, SLOT( apply() ) );


  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  errorBox = new QTextEdit();
  errorBox->setFontPointSize(8);
  bb->addWidget(errorBox);
}

Robot::~Robot(void){

}

void Robot::apply(void){
  if (Rob != 0){
    switch(otherActionsBox->currentIndex()){
    case 0: // goto net limit and home
      Rob->gotoNegLimitsAndSetHome();
      break;
    case 1: // restart watchdog
      Rob->restartWatchdog();
      break;
    case 2: // goto home
      Rob->absPos(0,0,0,50);
      break;
    default:
      break;
      
    };

  }
 
}
  


void Robot::clampTool(void){
  if (Rob != 0){
    Rob->clampTool();
  }
}

void Robot::releaseTool(void){
  if (Rob != 0){
    Rob->releaseTool();
  }
}

void Robot::startTrajRec(void){
  if (Rob != 0){
    Rob->startRecording( );
  }

}

void Robot::recordTrajStep(void){
  if (Rob != 0){
    Rob->recordStep( );
  }
}

void Robot::stopTrajRec(void){
  if (Rob != 0){
    Rob->stopRecording( );
  }
}


void Robot::restartWatchdog(void){
  if (Rob != 0){
    Rob->restartWatchdog();
  }
}

void Robot::stopMirob(void){
  if (Rob != 0){
    Rob->stop();
  }
}

void Robot::initDevices( void )
{
  for ( unsigned int k=0; k<10; k++ ) {
     Str ns( k+1, 0 );
     //Rob = dynamic_cast< Manipulator* >( device( "robot-" + ns ) );
     Rob = dynamic_cast< ::misc::Mirob* >( device( "robot-" + ns ) );
     if ( Rob != 0 ){
       Rob->setPosLCDs(XPosLCD, YPosLCD, ZPosLCD);
       Rob->setLogBox(errorBox);
       break;
     }
       
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
      if ( Rob != 0 )
	Rob->setVZ(0 );
      break;

    case Qt::Key_D:
      if ( Rob != 0 )
	Rob->setVZ( 0 );

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
  //  fprintf(stderr, " key pressed ");
  /* only accept the event if it is not from a autorepeat key */
  if(e->isAutoRepeat() ) {    
    e->ignore();   
  } else {    
    e->accept();
  
  
    switch ( e->key() ) {

    case Qt::Key_F:
      if ( Rob != 0 ){
	Rob->clearPositions();
      }
      break;


    case Qt::Key_N:
      if ( Rob != 0 ){
	Rob->recordPosition();
      }
      break;

    case Qt::Key_M:
      if ( Rob != 0 ){
	Rob->makePositionsForbiddenZone();
      }
      break;


    case Qt::Key_P:
      if ( Rob != 0 ){
	fprintf(stderr,"ROBOT: Position %f, %f, %f\n",Rob->posX(), Rob->posY(),Rob->posZ());
      }
      break;

    case Qt::Key_H:
      if ( Rob != 0 )
	Rob->gotoNegLimitsAndSetHome();
	
      break;

    case Qt::Key_G:
      if ( Rob != 0 )
	//Rob->executeRecordedTrajectory(30, false, true );
	Rob->executeRecordedTrajectory(30, true, true );

      break;

    case Qt::Key_J:
      if ( Rob != 0 )
	Rob->startRecording( );
      break;

    case Qt::Key_K:
      if ( Rob != 0 )
	Rob->recordStep( );
      break;

    case Qt::Key_L:
      if ( Rob != 0 )
	Rob->stopRecording( );
      break;

    case Qt::Key_O:
      if ( Rob != 0 ){
	Rob->step(10000,10000,10000,50, WAIT);
	Rob->step(-10000,-10000,-10000, 50, DONTWAIT);
      }
      break;

//     case Qt::Key_T:
//       if ( Rob != 0 )
// 	Rob->clampTool();
//       break;

//     case Qt::Key_R:
//       if ( Rob != 0 )
// 	Rob->releaseTool();
//       break;


    case Qt::Key_U:
      if ( Rob != 0 )
	Rob->setVZ(-SPEED );
      break;

    case Qt::Key_D:
      if ( Rob != 0 )
	Rob->setVZ( SPEED );

      break;

    case Qt::Key_Up:
      if ( Rob != 0 )
	Rob->setVX(-SPEED );
      break;

    case Qt::Key_Down:
      if ( Rob != 0 )
	Rob->setVX(SPEED);
      break;

    case Qt::Key_Left:
      if ( Rob != 0 )
	Rob->setVY( SPEED );
      break;

    case Qt::Key_Right:
      if ( Rob != 0 )
	Rob->setVY( -SPEED );
      break;


    default:
      Control::keyPressEvent( e );

    }
  }
}


addControl( Robot, base );

}; /* namespace base */

#include "moc_robot.cc"
