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
#include <QVBoxLayout>
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
  vb->setSpacing( 0 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );


}


void Robot::initDevices( void )
{
  for ( unsigned int k=0; k<10; k++ ) {
     Str ns( k+1, 0 );
     //Rob = dynamic_cast< Manipulator* >( device( "robot-" + ns ) );
     Rob = dynamic_cast< ::misc::Mirob* >( device( "robot-" + ns ) );
     if ( Rob != 0 )
       break;
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
	Rob->executeRecordedTrajectory(30, false, true );
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

    case Qt::Key_T:
      if ( Rob != 0 )
	Rob->clampTool();
      break;

    case Qt::Key_R:
      if ( Rob != 0 )
	Rob->releaseTool();
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
