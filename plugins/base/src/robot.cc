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
  QHBoxLayout *bb;

  setLayout( vb );
  vb->setSpacing( 4 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );


  /************** Positions ***********************/
  // base layout
  QGridLayout *Positions = new QGridLayout;
  QLabel *label;

  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );

  Positions->setHorizontalSpacing( 2 );
  Positions->setVerticalSpacing( 2 );
  vb->addLayout( Positions );


  // position watch

  label = new QLabel( "Mirob X-Position " );
  label->setAlignment( Qt::AlignCenter );
  Positions->addWidget( label, 0, 0 );
 
  XPosLCD = new QLCDNumber( 8 );
  XPosLCD->setSegmentStyle( QLCDNumber::Filled );
  XPosLCD->setFixedHeight( label->sizeHint().height() );

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

  // limit switch LEDs
  // X
  XLowLimLCD = new QLCDNumber( 1 );
  XLowLimLCD->setSegmentStyle( QLCDNumber::Filled );
  XLowLimLCD->setFixedHeight( label->sizeHint().height() );
  XLowLimLCD->setFixedWidth( 20);

  XLowLimLCD->setPalette( qp );
  XLowLimLCD->setAutoFillBackground( true );
  Positions->addWidget( XLowLimLCD,0,2 );
  
  XHiLimLCD = new QLCDNumber( 1 );
  XHiLimLCD->setSegmentStyle( QLCDNumber::Filled );
  XHiLimLCD->setFixedHeight( label->sizeHint().height() );
  XHiLimLCD->setFixedWidth( 20);

  XHiLimLCD->setPalette( qp );
  XHiLimLCD->setAutoFillBackground( true );
  Positions->addWidget( XHiLimLCD,0,3);

  //Y
  YLowLimLCD = new QLCDNumber( 1 );
  YLowLimLCD->setSegmentStyle( QLCDNumber::Filled );
  YLowLimLCD->setFixedHeight( label->sizeHint().height() );
  YLowLimLCD->setFixedWidth( 20);

  YLowLimLCD->setPalette( qp );
  YLowLimLCD->setAutoFillBackground( true );
  Positions->addWidget( YLowLimLCD,1,2 );
  
  YHiLimLCD = new QLCDNumber( 1 );
  YHiLimLCD->setSegmentStyle( QLCDNumber::Filled );
  YHiLimLCD->setFixedHeight( label->sizeHint().height() );
  YHiLimLCD->setFixedWidth( 20);

  YHiLimLCD->setPalette( qp );
  YHiLimLCD->setAutoFillBackground( true );
  Positions->addWidget( YHiLimLCD,1,3);

  //Z
  ZLowLimLCD = new QLCDNumber( 1 );
  ZLowLimLCD->setSegmentStyle( QLCDNumber::Filled );
  ZLowLimLCD->setFixedHeight( label->sizeHint().height() );
  ZLowLimLCD->setFixedWidth( 20);

  ZLowLimLCD->setPalette( qp );
  ZLowLimLCD->setAutoFillBackground( true );
  Positions->addWidget( ZLowLimLCD,2,2 );
  
  ZHiLimLCD = new QLCDNumber( 1 );
  ZHiLimLCD->setSegmentStyle( QLCDNumber::Filled );
  ZHiLimLCD->setFixedHeight( label->sizeHint().height() );
  ZHiLimLCD->setFixedWidth( 20);

  ZHiLimLCD->setPalette( qp );
  ZHiLimLCD->setAutoFillBackground( true );
  Positions->addWidget( ZHiLimLCD,2,3);



  // buttons:
  bb = new QHBoxLayout;
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
  otherActionsBox = new QComboBox;
  bb->addWidget(otherActionsBox);
  otherActionsBox->addItem("Go to -lim and set home");
  otherActionsBox->addItem("Restart watchdog");
  otherActionsBox->addItem("Go home");

  otherActionsBox->addItem("Clamp  Tool");
  otherActionsBox->addItem("Release Tool");

  otherActionsBox->addItem("Start recording trajectory");
  otherActionsBox->addItem("Stop recording trajectory");

  otherActionsBox->addItem("Start recording forbidden zone");
  otherActionsBox->addItem("Stop recording forbidden zone");

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
    case 3: // clamp Tool
      Rob->clampTool();
      break;
    case 4: // release Tool
      Rob->releaseTool();
      break;
    case 5: // start recording trajectory
      Rob->startRecording( );
      errorBox->append("Key k records new position");
      break;
    case 6: // stop recording trajectory
      Rob->stopRecording( );
      break;
    case 7: // start recording forbidden zone
      Rob->clearPositions();
      errorBox->append("Move Mirob to the upper four corners of the forbidden zone");
      errorBox->append("Key n records current corner");
      break;
    case 8: // stop recording forbidden zone
      Rob->makePositionsForbiddenZone();
      break;
    default:
      break;
      
    };

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
       ::misc::GUICallback* gcb = new ::misc::GUICallback();
       gcb->XPosLCD = XPosLCD;
       gcb->YPosLCD = YPosLCD;
       gcb->ZPosLCD = ZPosLCD;

       gcb->XLowLimLCD = XLowLimLCD;
       gcb->XHiLimLCD = XHiLimLCD;
       gcb->YLowLimLCD = YLowLimLCD;
       gcb->YHiLimLCD = YHiLimLCD;
       gcb->ZLowLimLCD = ZLowLimLCD;
       gcb->ZHiLimLCD = ZHiLimLCD;


       gcb->logBox = errorBox;
       Rob->setGUICallback(gcb);
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
      if ( Rob != 0 ){
	Rob->setVZ( 0 );
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
  //  fprintf(stderr, " key pressed ");
  /* only accept the event if it is not from a autorepeat key */
  if(e->isAutoRepeat() ) {    
    e->ignore();   
  } else {    
    e->accept();
  
  
    switch ( e->key() ) {


    case Qt::Key_N:
      if ( Rob != 0 ){
	Rob->recordPosition();
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


    case Qt::Key_K:
      if ( Rob != 0 )
	Rob->recordStep( );
      break;


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
