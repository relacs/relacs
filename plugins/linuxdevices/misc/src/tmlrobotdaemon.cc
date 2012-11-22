/*
  misc/mirob.cc
  The Mirob module linear robot from MPH

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <relacs/misc/mirob.h>
#include <relacs/misc/tmlrobotdaemon.h>
#include <pthread.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <TML_lib.h>
#include <signal.h>
#include <relacs/misc/tinyxml2.h>

#define WAIT      1
#define STOP      1
#define DONT_WAIT 0
#define DONT_STOP 0
#define	NO_ADDITIVE	0
#define FORWARD true
#define BACKWARD false

using namespace std;
using namespace relacs;

namespace misc{

const char* TMLRobotDaemon::LOGPREFIX = "ROBOT DAEMON: ";


TMLRobotDaemon::TMLRobotDaemon(robotDaemon_data* ptr){
  info = ptr;
  //motionIssued = false;
  
  // create the mutex shared by TMLRobotDaemon and Mirob
  pthread_mutex_init(&info->mutex,NULL);
  pthread_cond_init(&info->cond,NULL);

}
/*****************************************************************/

/*
  Starts the robot daemon pthread. Waits 10 seconds for the thread to
  start or returns 1.
*/
int TMLRobotDaemon::Start(){

  // start the thread 
  log("Starting daemon");
  int code = pthread_create(&id, NULL, TMLRobotDaemon::EntryPoint, (void*) this);
  if (code){
    log("Could not create pthread! [FAIL]");
    return code;
  }

  return 0;
}
/*****************************************************************/

/* 
   Needed to start the class as pthread. Needs to be static.
 */
void * TMLRobotDaemon::EntryPoint(void * pthis){
   TMLRobotDaemon * pt = (TMLRobotDaemon*)pthis;
   pt->Run();
   return NULL;
}

/*****************************************************************/
int TMLRobotDaemon::Stop(){
    pthread_mutex_lock(&info->mutex);
    info->v[0] = 0.;
    info->v[1] = 0.;
    info->v[2] = 0.;
    info->state = ROBOT_HALT;
    // info->vChanged=true;
    // stopRobot = true;
    pthread_mutex_unlock(&info->mutex);
    
    return 0;
}

/*****************************************************************/

/*
  Stops the daemon. 
 */
int TMLRobotDaemon::Shutdown(){
  if ( pthread_kill(id, 0) == 0){
    log("Stopping daemon");
    info->active = false;
    
    // destroy mutex
    pthread_mutex_destroy(&info->mutex);
    pthread_cond_destroy(&info->cond);


    int code = pthread_join(id, NULL); // wait for robotDaemon to return
    if (code){
      log("Thread did not join properly!");
      return 1;
    }
    
    return 0;
  }else{
    log("Daemon already stopped!");
    return 0;
  }
}
/*****************************************************************/

/*
  Sets up the daemon, runs its main loop until the thread is
  cancelled, destorys all objects properly, and exits.
 */
int TMLRobotDaemon::Run()
{
   Setup();
   Execute();
   Exit();
   return 0;
}
/*****************************************************************/

/*
  Captures log messages.
 */
void TMLRobotDaemon::log(const char* text){
  cerr << LOGPREFIX << text << endl;
}
/*****************************************************************/

/*
  Same as log(const char* txt) but for relacs strings.
 */
void TMLRobotDaemon::log(relacs::Str text){
  cerr << LOGPREFIX << (string)text << endl;
}


/*****************************************************************/

/* 
   Initializes the robot daemon.
 */
void TMLRobotDaemon::Setup(){
  pthread_mutex_lock( &info->mutex ); // must be here and not in Start (start would be a different thread)

  /************ open robotDaemon ****************/
  // open device:
  if ( TS_OpenChannel( info->Device, info->ChannelType, info->HostID, info->Baudrate ) < 0 ) {
    cerr << LOGPREFIX << "Communication error! " << TS_GetLastErrorText() << endl;
  }else
    cerr << LOGPREFIX << "Channel opened [OK]" << endl;

   
  // load setup file:
  int setupindex = TS_LoadSetup( info->SetupFile );
  if ( setupindex < 0 ) {
    cerr << LOGPREFIX << "Failed to load setup file! " << TS_GetLastErrorText() << endl;
  }else
    cerr << LOGPREFIX << "Setup loaded [OK]" << endl;


  //   setup axis:
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SetupAxis( k, setupindex ) )  {
      cerr << LOGPREFIX << "Failed to setup axis " << k << "! " << TS_GetLastErrorText() << endl;
    }else
      cerr << LOGPREFIX << "Setup axis " << k << " [OK]" << endl;

    if ( ! TS_SelectAxis( k ) ) {
      cerr << LOGPREFIX << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << endl;
    }else
      cerr << LOGPREFIX << "Select axis " << k << " [OK]" << endl;

    if ( ! TS_SetTargetPositionToActual()){
      cerr << LOGPREFIX << "Failed to set target position to actual for axis "
	   << k << "! " << TS_GetLastErrorText() << endl;
    }else
      cerr << LOGPREFIX << "TPOS=APOS on axis " << k << " [OK]" << endl;


    if ( ! TS_DriveInitialisation() ) {
      cerr << LOGPREFIX << "Failed to initialize drive for axis " << k << "! " 
	   << TS_GetLastErrorText() << endl;
    }else
      cerr << LOGPREFIX << "Drive initialized on axis " << k << " [OK]" << endl;


    if ( ! TS_Power( POWER_ON ) ) {
      cerr << LOGPREFIX << "Failed to power on drive for axis " << k << "! " 
	   << TS_GetLastErrorText() << endl;
    }else
      cerr << LOGPREFIX << "Power up on axis " << k << " [OK]" << endl;


    WORD axison = 0;
    while ( axison == 0 ) {
      // Check the status of the power stage:
      if ( ! TS_ReadStatus( REG_SRL, axison ) ) {
	cerr << LOGPREFIX << "Failed to read status for axis " << k << "! " 
	     << TS_GetLastErrorText() << endl;
      }
      axison = ((axison & 1<<15) != 0 ? 1 : 0);
    }
  }
  // set active state true to signal Mirob that robotDaemon was created
  log("Daemon successfully started.");
  info->active = true;
  
  // set cond variable and unlock mutex
  pthread_cond_signal( &info->cond );
  pthread_mutex_unlock( &info->mutex );

}

  
/*****************************************************************/

/*

Main loop of pthread.

 */
void TMLRobotDaemon::Execute(){
  // initialize state variable
  old_state = info->state;
  for (int i = 0; i != 3; ++i){
    old_v[i] = info->v[i];
  }
  old_tool_state = info->toolClamped;
  old_queue_len = 0;

  
  while (info->active){

    updateInfo();
    
    // if the state has changed
    if (info->state != old_state){
      if (info->state == ROBOT_POS){ 
	if (old_state == ROBOT_FREE){
	  for (int i =1; i !=4; ++i){
	    if (activateAxis(i) > 0 || !TS_Stop()){
	      cerr << LOGPREFIX << "Could not stop robot!" << endl;
	    }
	  }	  
	}
	
      }else if(info->state == ROBOT_FREE){
	if (old_state == ROBOT_POS){
	  cerr << LOGPREFIX << "Clearing position queue" << endl;
	  queue<PositionUpdate*> empty;
	  swap(info->positionQueue , empty );
	  //motionIssued = false;

	}
      }else if(info->state == ROBOT_HALT){
	for (int i =1; i !=4; ++i){
	  if (activateAxis(i) > 0 || !TS_Stop()){
	    cerr << LOGPREFIX << "Could not stop robot!" << endl;
	  }
	}
      }else if(info->state == ROBOT_ERR){

      }
    }

    // -------- now act according to state ------------------

    if (info->state == ROBOT_FREE){
      // if a new velcity has been issued
      if ( (old_v[0] - info->v[0])*(old_v[0] - info->v[0]) + 
	   (old_v[1] - info->v[1])*(old_v[1] - info->v[1]) + 
	   (old_v[2] - info->v[2])*(old_v[2] - info->v[2]) > 1){
	for (int axis = 1; axis <=3; ++axis){
	  setV(info->v[axis-1],axis); 
	}
      }
    //---------------------------------------------
    }else if (info->state == ROBOT_POS){
      if (old_queue_len == 0 && info->positionQueue.size() > 0){// new movement issued
	if (info->positionQueue.size() > 0){
	  cerr << LOGPREFIX << "Going to " << *(info->positionQueue.front()) << endl;
	  pthread_mutex_lock( &info->mutex );
	  setPos(info->positionQueue.front()->x, info->positionQueue.front()->y, 
		 info->positionQueue.front()->z, info->positionQueue.front()->speed);
	  pthread_mutex_unlock( &info->mutex );
	}
	updateInfo();
      }

      if ( motionComplete()){
	//motionIssued = false;
	if (info->positionQueue.size() > 0){
	  cerr << LOGPREFIX << "Motion Completed." << endl;
	  info->positionQueue.pop();
	   
	  if (info->positionQueue.size() > 0){
	    cerr << LOGPREFIX << "Going to " << *(info->positionQueue.front()) << endl;
	    pthread_mutex_lock( &info->mutex );
	    setPos(info->positionQueue.front()->x, info->positionQueue.front()->y, 
		   info->positionQueue.front()->z, info->positionQueue.front()->speed);
	    pthread_mutex_unlock( &info->mutex );
	  }
	}
      }

    }else if (info->state == ROBOT_HALT){
      if (old_tool_state != info->toolClamped){
	if (info->toolClamped){
	  clampTool();
	}else{
	  releaseTool();
	}
	
      }

    }else if (info->state == ROBOT_ERR){

    }

    // update state variable
    old_state = info->state;
    for (int i = 0; i != 3; ++i){
      old_v[i] = info->v[i];
    }
    old_tool_state = info->toolClamped;
    old_queue_len=info->positionQueue.size();
    // sleep 
    nanosleep(&info->sleeptime,NULL);
  }

}
/*****************************************************************/

void TMLRobotDaemon::Exit(){
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SelectAxis( k ) )
      cerr << LOGPREFIX << "Failed to select axis " << k << "! " 
	   << TS_GetLastErrorText() << endl;
    if ( ! TS_Stop() )
      cerr << LOGPREFIX << "Failed to stop motion for axis " << k << "! " 
	   << TS_GetLastErrorText() << endl;
    if ( ! TS_Power( POWER_OFF ) )
      cerr << LOGPREFIX  << "Failed to power off drive for axis " << k << "! " 
	   << TS_GetLastErrorText() << endl;
  }
  TS_CloseChannel( -1 );
  log("Daemon successfully closed");

}

/*****************************************************************/

int TMLRobotDaemon::setV(double v, int ax){
  if (activateAxis(ax) > 0 || !TS_MoveVelocity(v,info->MaxAcc,UPDATE_IMMEDIATE,FROM_MEASURE)){
    cerr << LOGPREFIX << "Failed to set velocity "
	 << v << " for axis " << ax << "! " 
	 << TS_GetLastErrorText() << '\n';
    return 1;
  }

  return 0;
}
/*****************************************************************/

void TMLRobotDaemon::updateInfo(){
    pthread_mutex_lock( &info->mutex );

    /*------- extract current position ---------*/
    for (int axis = 1; axis <= 3; ++axis){
      // select axis:
      if ( ! TS_SelectAxis( axis ) ) {
	cerr << LOGPREFIX << "Failed to select axis " 
	     << axis << "! " << TS_GetLastErrorText() << '\n';
      }

      // read position:
      if ( ! TS_GetLongVariable( "APOS", tmp_apos) ) {
	cerr << LOGPREFIX << "Failed to read position of axis " 
	     << axis << "! " << TS_GetLastErrorText() << '\n';
      }
      tmp_apos2 = tmp_apos;
      info->pos[axis-1] = tmp_apos2;

      // // check low limit switch
      // if(!TS_GetInput(INPUT_24,limitNeg[axis-1])){
      // 	cerr << LOGPREFIX << "Failed to get INPUT_24 for axis " 
      // 	     << axis << "! " << TS_GetLastErrorText() << endl;
      // 	continue;
      // }
      // // check high limit switch
      // if(!TS_GetInput(INPUT_2,limitPos[axis-1])){
      // 	cerr << LOGPREFIX << "Failed to get INPUT_2 for axis " 
      // 	     << axis << "! " << TS_GetLastErrorText() << endl;
      // 	continue;
      // }

      // // check motio error register
      
      // if ( ! TS_ReadStatus( REG_MER, MER[axis-1] ) ) {
      // 	cerr << LOGPREFIX << "Failed to read status for axis " << axis << "! " 
      // 	     << TS_GetLastErrorText() << endl;
      // }
      // if ( ! TS_ReadStatus( REG_MCR, MCR[axis-1] ) ) {
      // 	cerr << LOGPREFIX << "Failed to read status for axis " << axis << "! " 
      // 	     << TS_GetLastErrorText() << endl;
      // }
      if ( ! TS_ReadStatus( REG_SRL, SRL[axis-1] ) ) {
      	cerr << LOGPREFIX << "Failed to read status for axis " << axis << "! " 
      	     << TS_GetLastErrorText() << endl;
      }
      // if ( ! TS_ReadStatus( REG_ISR, ISR[axis-1] ) ) {
      // 	cerr << LOGPREFIX << "Failed to read status for axis " << axis << "! " 
      // 	     << TS_GetLastErrorText() << endl;
      // }
      // if ( ! TS_ReadStatus( REG_SRH, SRH[axis-1] ) ) {
      // 	cerr << LOGPREFIX << "Failed to read status for axis " << axis << "! " 
      // 	     << TS_GetLastErrorText() << endl;
      // }



    }

    pthread_mutex_unlock( &info->mutex );

    // cerr << LOGPREFIX << "Position Vector Len " << info->positionQueue.size() << endl;

}
/*****************************************************************/

int TMLRobotDaemon::getPos( int axis ) const
{
  // select axis:
  if ( ! TS_SelectAxis( axis ) ) {
    cerr << LOGPREFIX << "Failed to select axis " 
	 << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  // read position:
  long apos = 0;
  if ( ! TS_GetLongVariable( "APOS", apos) ) {
    cerr << LOGPREFIX << "Failed to read position of axis " 
	 << axis << "! " << TS_GetLastErrorText() << '\n';
    return 0.0;
  }

  int apos2 = apos; // this is a hack to account for the bug in TML_lib
  return apos2;
}

/*****************************************************************/

int TMLRobotDaemon::setPos(double x, double y, double z, double speed){
  speed = (speed > info->MaxSpeed) ? info->MaxSpeed : speed;
  double d[3] = {x,y,z};
  
  double dx,dy,dz;
  dx = abs(getPos(1) - x);
  dy = abs(getPos(2) - y);
  dz = abs(getPos(3) - z);

  double v_len = sqrt(dx*dx + dy*dy + dz*dz );

  double vx = speed * dx/v_len;
  double vy = speed * dy/v_len;
  double vz = speed * dz/v_len;

  double v[3] = {vx,vy,vz};

  for (int axis = 1; axis <= 3; ++axis){
    if ( ! TS_SelectAxis( axis ) ) {
      cerr << "Failed to select axis " << axis << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    // move:
    long step = long( rint( d[axis - 1] ) );
    if ( ! TS_MoveAbsolute(step, v[axis-1], info->MaxAcc, UPDATE_IMMEDIATE, FROM_MEASURE)){
      cerr << LOGPREFIX << "Failed to move absolute on axis " 
	   << axis << "! " << TS_GetLastErrorText() << '\n';
      return 1;

    }
   
  }
  //motionIssued = true;
  return 0;

}

/*****************************************************************/
bool TMLRobotDaemon::motionComplete(){
  return ((SRL[0] & 1 << 10) != 0 )
    && ((SRL[1] & 1 << 10) != 0 )
    && ((SRL[2] & 1 << 10) != 0);
}
  
/*****************************************************************/
double TMLRobotDaemon::positionError(){
  double cx = (double)getPos(1);
  double cy = (double)getPos(2);
  double cz = (double)getPos(3);
  return (cx - info->pos[0])*(cx - info->pos[0]) + 
    (cy - info->pos[0])*(cy - info->pos[1]) + 
    (cz - info->pos[0])*(cz - info->pos[1]);
}

/*****************************************************************/
/*
  Activates a particular axis of the robot.
 */
int TMLRobotDaemon::activateAxis(int axis){
  // select axis:
  if ( ! TS_SelectAxis( axis ) ) {
    cerr << LOGPREFIX << "Failed to select axis " << axis << "! " << TS_GetLastErrorText() << endl;
    return 1;
  }
  return 0;
      
}

int TMLRobotDaemon::clampTool(void){
  if (activateAxis(3) > 0 || !TS_SetOutput(OUTPUT_30, IO_HIGH) ||!TS_SetOutput(OUTPUT_31, IO_LOW) ){
    cerr << "Failed to clamp tool!" << TS_GetLastErrorText() << '\n';
    return 1;
  }
  return 0;
}

int TMLRobotDaemon::releaseTool(void){

  if (activateAxis(3) > 0 || !TS_SetOutput(OUTPUT_31, IO_HIGH) ||!TS_SetOutput(OUTPUT_30, IO_LOW) ){
    cerr << "Failed to release tool!" << TS_GetLastErrorText() << '\n';
    return 1;
  }
  return 0;
}





bool TMLRobotDaemon::isInsideForbiddenZone(){
  return false;
}







}; /* namespace misc */
