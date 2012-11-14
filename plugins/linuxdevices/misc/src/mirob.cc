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
#include <pthread.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <TML_lib.h>
#include <signal.h>
#include "tinyxml2.h"

#define WAIT      1
#define STOP      1
#define DONT_WAIT 0
#define DONT_STOP 0
#define	NO_ADDITIVE	0
#define FORWARD true
#define BACKWARD false

using namespace std;
using namespace relacs;
using namespace tinyxml2;

namespace misc{

const char* TMLRobotDaemon::LOGPREFIX = "ROBOT DAEMON: ";
const char* Mirob::LOGPREFIX = "MIROB: ";



int inv3(double A[3][3], double (&result)[3][3]){
  double determinant =    +A[0][0]*(A[1][1]*A[2][2]-A[2][1]*A[1][2])
    -A[0][1]*(A[1][0]*A[2][2]-A[1][2]*A[2][0])
    +A[0][2]*(A[1][0]*A[2][1]-A[1][1]*A[2][0]);

  if (determinant < 1e-12 && determinant > -1e-12){
    cerr << Mirob::LOGPREFIX << "Basis matrix is singular!" << endl;
    return 1;
  }
  double invdet = 1./determinant;
  result[0][0] =  (A[1][1]*A[2][2]-A[2][1]*A[1][2])*invdet;
  result[1][0] = -(A[0][1]*A[2][2]-A[0][2]*A[2][1])*invdet;
  result[2][0] =  (A[0][1]*A[1][2]-A[0][2]*A[1][1])*invdet;
  result[0][1] = -(A[1][0]*A[2][2]-A[1][2]*A[2][0])*invdet;
  result[1][1] =  (A[0][0]*A[2][2]-A[0][2]*A[2][0])*invdet;
  result[2][1] = -(A[0][0]*A[1][2]-A[1][0]*A[0][2])*invdet;
  result[0][2] =  (A[1][0]*A[2][1]-A[2][0]*A[1][1])*invdet;
  result[1][2] = -(A[0][0]*A[2][1]-A[2][0]*A[0][1])*invdet;
  result[2][2] =  (A[0][0]*A[1][1]-A[1][0]*A[0][1])*invdet;

  return 0;
}


TMLRobotDaemon::TMLRobotDaemon(robotDaemon_data* ptr){
  info = ptr;
  MaxSpeed = 50;
  MaxAcc = 0.3;
  // create the mutex shared by TMLRobotDaemon and Mirob
  pthread_mutex_init(&info->mutex,NULL);
  pthread_cond_init(&info->cond,NULL);
  positionTarget = NULL;

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
    info->mode = ROBOT_FREE;
    info->v[0] = 0.;
    info->v[1] = 0.;
    info->v[2] = 0.;
    info->vChanged = true;
    
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

  while (info->active){

    updateInfo();
    fprintf(stderr,"%04hX %04hX %04hX \r",SRL[0],SRL[1],SRL[2]);
    //cerr << LOGPREFIX << "Motion " << motionComplete() << endl;
    // if the clamp state has changed
    if (info->clampChanged){
      if (info->toolClamped){
	clampTool();
      }else{
	releaseTool();
      }
      info->clampChanged = false;
    }


    if (info->mode == ROBOT_FREE){
	
      // if a new velcity has been issued
      if (info->vChanged){
	for (int axis = 1; axis <=3; ++axis){
	  setV(info->v[axis-1],axis); 
	}
	info->vChanged = false;
      }
      

    //---------------------------------------------
    }else if (info->mode == ROBOT_POS){
      //cerr << LOGPREFIX << "Queue length" << info->positionQueue.size() << '\r';
      if (positionTarget != NULL){ // we still have a current target
	//if (positionError() < 100.){
	if (motionIssued && motionComplete()){
	  // cerr << positionError() <<"=Position Error < 100" << endl;
	  // cerr << "Position Target == NULL" << endl;
	  cerr << LOGPREFIX << "Motion Complete" << motionComplete() << endl;
	  delete positionTarget;
	  positionTarget = NULL;
	  motionIssued = false;
	}
      }else if (info->positionQueue.size() > 0){
	pthread_mutex_lock( &info->mutex );
	cerr << "Setting Position Target" << endl;

	positionTarget = info->positionQueue.front();
	motionIssued = false;
	setPos(positionTarget->x, positionTarget->y, positionTarget->z, positionTarget->speed);
	info->positionQueue.pop();
	log("Setting New Position");
	pthread_mutex_unlock( &info->mutex );
      }

    }else if (info->mode == ROBOT_HALT){

    }else if (info->mode == ROBOT_STOP){

    }

  
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
  if (activateAxis(ax) > 0 || !TS_MoveVelocity(v,MaxAcc,UPDATE_IMMEDIATE,FROM_MEASURE)){
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
  double MaxSpeed = 100;
  double MaxAcc = 0.3183;
  speed = (speed > MaxSpeed) ? MaxSpeed : speed;
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
    if ( ! TS_MoveAbsolute(step, v[axis-1], MaxAcc, UPDATE_IMMEDIATE, FROM_MEASURE)){
      cerr << LOGPREFIX << "Failed to move absolute on axis " 
	   << axis << "! " << TS_GetLastErrorText() << '\n';
      return 1;

    }
   
  }
  motionIssued = true;
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




int TMLRobotDaemon::reset( void ){
  return 0;
}


/********** MIROB **************************************************/

//const string Mirob::SetupFile = "mirob2.t.zip";


/********************* initialization part **************************/

Mirob::Mirob( const string &device )
  : Manipulator( "Mirob" )
{
  robotDaemon = 0;
  Opened = false;
 
  Options opts;

  open( device, opts );
}

Mirob::Mirob( void )
  : Manipulator( "Mirob" )
{
  robotDaemon = 0;
  
  Opened = false;
}

Mirob::~Mirob( void )
{
  close();
}

int Mirob::loadConfigurationFile(const char* filename){
  //  XMLDocument xml;
  // xml.LoadFile( "config.xml");
  // XMLElement* root = xml.RootElement();

  // // extract max speed
  // double tmp;
  // root->FirstChildElement("maxspeed")->QueryDoubleText(&tmp);
  // MaxSpeed = (long)tmp;
  // // extract maximal acceleration
  // root->FirstChildElement("maxacceleration")->QueryDoubleText(&MaxAcc);
  
  // SetupFile = root->FirstChildElement("setupfile")->FirstChildElement()->Value();

  // cerr << "Loaded "<< MaxSpeed << SetupFile << MaxAcc << endl;
  MaxSpeed = 100;
  MaxAcc = 0.3182;

  B[0][0] = 0.354888; B[0][1] = 0.085845; B[0][2] = 0.930959; 
  B[1][0] = 0.704112; B[1][1] = -0.679629; B[1][2] = -0.205743; 
  B[2][0] = 0.615045; B[2][1] = 0.728515; B[2][2] = -0.301637; 
  
  inv3(B,iB);

  // for (int i = 0; i !=3; ++i){
  //   cerr << iB[i][0] << '\t'
  // 	 << iB[i][1] << '\t'
  // 	 << iB[i][2] << '\n';
  // }
  // for (int i = 0; i !=3; ++i){
  //   cerr << B[i][0] << '\t'
  // 	 << B[i][1] << '\t'
  // 	 << B[i][2] << '\n';
  // }

  b0[0] = 0.0;
  b0[1] = 0.0;
  b0[2] = 0.0;

  return 0;
}

int Mirob::open( const string &device, const Options &opts )
{
  cerr << "Opening MIROB on " << device << '\n';
  if ( Opened )
    return 0;

  Info.clear();
  Settings.clear();


  CoordinateMode = MIROB_COORD_RAW;

  string tmp = opts.text("config");
  loadConfigurationFile(tmp.c_str());// TODO: Get the name from options
  SetupFile = "mirob2.t.zip";

  robotDaemon_info.ChannelType = ChannelType;
  robotDaemon_info.HostID = HostID;
  robotDaemon_info.Baudrate = Baudrate;
  robotDaemon_info.SetupFile = SetupFile;
  robotDaemon_info.Device = device.c_str();
  robotDaemon_info.active = false;
  robotDaemon_info.forbiddenZones = &forbiddenZones;
  robotDaemon_info.sleeptime.tv_sec = robotDaemon_sleep_sec;
  robotDaemon_info.sleeptime.tv_nsec = robotDaemon_sleep_nsec;
  robotDaemon_info.setNegLimitAsHome = false;

  robotDaemon_info.v[0] = 0.0;
  robotDaemon_info.v[1] = 0.0;
  robotDaemon_info.v[2] = 0.0;
  robotDaemon_info.vChanged = true;

  robotDaemon_info.toolClamped = false;
  robotDaemon_info.clampChanged = false;

  robotDaemon_info.mode = ROBOT_HALT;


  /* Start Robot Daemon */
  robotDaemon =  new TMLRobotDaemon(&robotDaemon_info);
  
  cerr << "MIROB waiting for daemon to start" << endl;
  pthread_mutex_lock( &robotDaemon_info.mutex );
  robotDaemon->Start();
  
  // wait until Setup has finished (sends the cond signal)
  pthread_cond_wait( &robotDaemon_info.cond, &robotDaemon_info.mutex );
  pthread_mutex_unlock( &robotDaemon_info.mutex );

  // finish open
  setDeviceName( "Mirob" );
  setDeviceVendor( "MPH" );
  setDeviceFile( device );
  Device::addInfo();
  Opened = true;

  return 0;
}

/*************************************************************/

  void Mirob::setState(int mode)
  { 
    pthread_mutex_lock( &robotDaemon_info.mutex );
    robotDaemon_info.mode = mode;
    pthread_mutex_unlock( &robotDaemon_info.mutex );
  }

/*************************************************************/


void Mirob::close( void )
{

  Info.clear();
  Settings.clear();

  robotDaemon->Shutdown();
  Opened = false;
  cerr << "MIROB closed " << endl;
}



/********************* activation and reactivation part *************/

int Mirob::reset( void )
{
  return robotDaemon->reset();
}


/****************** velocity part *******************************/
int Mirob::getV(double &x, double &y, double &z){
  // don't forget coordinate transform
  return 0;
}

int Mirob::setV(double v, int ax){

  double tmp[3];
  if (CoordinateMode == MIROB_COORD_TRANS){

    double proj =  robotDaemon_info.v[0]*iB[ax-1][0] +
      robotDaemon_info.v[1]*iB[ax-1][1] +
      robotDaemon_info.v[2]*iB[ax-1][2];

    tmp[0] = robotDaemon_info.v[0] + (v-proj)*B[0][ax-1];
    tmp[1] = robotDaemon_info.v[1] + (v-proj)*B[1][ax-1];
    tmp[2] = robotDaemon_info.v[2] + (v-proj)*B[2][ax-1];
  }else{
    tmp[0] = tmp[1] = tmp[2] = 0;
    tmp[ax-1] = v;
  }

  // don't forget coordinate transform
  pthread_mutex_lock( &robotDaemon_info.mutex );
  for (int i=0; i != 3; ++i){
    robotDaemon_info.v[i] = tmp[i];
  }
  robotDaemon_info.vChanged = true;
  pthread_mutex_unlock( &robotDaemon_info.mutex );
  cerr << LOGPREFIX << "v: " << robotDaemon_info.v[0] << ", "
    << robotDaemon_info.v[1] << ", "
       << robotDaemon_info.v[2] << endl;
  return 0;
}

int Mirob::setVX(double v){
  return setV(v,1);
}

int Mirob::setVY(double v){
  return setV(v,2);
}

int Mirob::setVZ(double v){
  return setV(v,3);
}

int Mirob::setV(double vx, double  vy, double vz){
  
  if (CoordinateMode == MIROB_COORD_TRANS){
    double tmpx, tmpy, tmpz;
    tmpx = vx*B[0][0] + vy*B[0][1] + vz*B[0][2];
    tmpy = vx*B[1][0] + vy*B[1][1] + vz*B[1][2];
    tmpz = vx*B[2][0] + vy*B[2][1] + vz*B[2][2];
    vx = tmpx; vy = tmpy; vz = tmpz;
  }

  double speed = sqrt(vx*vx + vy*vy + vz*vz );
  if (speed > MaxSpeed){
    vx *= MaxSpeed/speed;
    vy *= MaxSpeed/speed;
    vz *= MaxSpeed/speed;

  }
  return (setVX(vx) + setVY(vy) + setVZ(vz) > 0) ? 1 : 0; 

}


/***************** positioning part *****************************/

int Mirob::stop(){
  return robotDaemon->Stop();
}
                             
int Mirob::step( double x, int axis )
{
  return 0;
}

int Mirob::setPos( double x, double y, double z, double speed )
{

  if (CoordinateMode ==  MIROB_COORD_TRANS){
    transformCoordinates(x,y,z, TRANS2RAW);
  }else if (CoordinateMode != MIROB_COORD_RAW){
    cerr << "MIROB.setPos(double x, double y, doule z, double speed):"
	 << "Coordinate mode not known!" << '\n';
    return 1;
  }
  pthread_mutex_lock( &robotDaemon_info.mutex );
  robotDaemon_info.positionQueue.push(new PositionUpdate(x,y,z,speed));
  pthread_mutex_unlock( &robotDaemon_info.mutex );
 
  return 0;
}


double* Mirob::transformCoordinates(double* x, int direction){
  transformCoordinates(x[0],x[1],x[2],direction);
  return x;
}

void Mirob::transformCoordinates(double& x, double& y, double& z, int direction){
  if (direction == RAW2TRANS){
    double tmp[3];
    for (int i = 0; i != 3; ++i){
      tmp[i] = B[i][0]*x + B[i][1]*y + B[i][2]*z;
      tmp[i] += b0[i];
    }
    x = tmp[0]; y = tmp[1]; z = tmp[2];
  }else if (direction == TRANS2RAW){
    double tmp[3];
    for (int i = 0; i != 3; ++i){
      tmp[i] -= b0[i];
      tmp[i] = iB[i][0]*x + iB[i][1]*y + iB[i][2]*z;
    }
    x = tmp[0]; y = tmp[1]; z = tmp[2];
    
  }
}


int Mirob::stepX( double x )
{
  return step( x, 1 );
}

int Mirob::stepY( double y )
{
  return step( y, 2 );
}

int Mirob::stepZ( double z )
{
  return step( z, 3 );
}

double Mirob::pos( int axis ) 
{
  if (CoordinateMode == MIROB_COORD_RAW){
    return (double) robotDaemon_info.pos[axis-1];

  }else if (CoordinateMode ==  MIROB_COORD_TRANS){
    double coord[3];
    for (int i= 0; i != 3; ++i){
      coord[i] = (double)robotDaemon_info.pos[i];
    }

    transformCoordinates(coord, RAW2TRANS);
    return coord[axis-1];
  }else{
    cerr << "MIROB.pos(int axis): Coordinate mode not known!" << '\n';
    return 0.;
  }
}

double Mirob::posX( void ) 
{
  return pos( 1 );
}

double Mirob::posY( void ) 
{
  return pos( 2 );
}

double Mirob::posZ( void ) 
{
  return pos( 3 );
}

int Mirob::step(double dx, double dy, double dz, double speed, bool wait){

  cerr << "step coordinate transform missing!" << endl;
  return 0;
}

int Mirob::clear(int axis)
{
  return 0;
}

int Mirob::clearX( void )
{
  return clear(1);
}

int Mirob::clearY( void )
{
  return clear(2);
}

int Mirob::clearZ( void )
{
  return clear(3);
}


int Mirob::clear( void )
{
  clearX();
  clearY();
  clearZ();
  return 0;
}


int Mirob::homeX( void )
{

  return stepX(-posX());
}


int Mirob::homeY( void )
{

  return stepY(-posY());
}


int Mirob::homeZ( void )
{

  return stepZ(-posZ());
}


int Mirob::home( void )
{
  return homeX() + ( homeY() << 1 ) + ( homeZ() << 2 );
}


int Mirob::setAmplX( double posampl, double negampl )
{

  return 0;
}


int Mirob::setAmplY( double posampl, double negampl )
{

  return 0;
}


int Mirob::setAmplZ( double posampl, double negampl )
{

  return 0;
}


double Mirob::minAmplX( void ) const
{
  return 1.0;
}


double Mirob::maxAmplX( void ) const
{
  return 80.0;
}


/**************** tool control part ***************************/
int Mirob::clampTool(void){
  pthread_mutex_lock( &robotDaemon_info.mutex );
  robotDaemon_info.toolClamped = true;
  robotDaemon_info.clampChanged = true;
  pthread_mutex_unlock( &robotDaemon_info.mutex );
  return 0;
}

int Mirob::releaseTool(void){
  pthread_mutex_lock( &robotDaemon_info.mutex );
  robotDaemon_info.toolClamped = false;
  robotDaemon_info.clampChanged = true;
  pthread_mutex_unlock( &robotDaemon_info.mutex );
  return 0;
}

int Mirob::switchClampState(void){
  pthread_mutex_lock( &robotDaemon_info.mutex );
  robotDaemon_info.toolClamped = !robotDaemon_info.toolClamped;
  robotDaemon_info.clampChanged = true;
  pthread_mutex_unlock( &robotDaemon_info.mutex );
  return 0;
}

void Mirob::miroblog(const char* text){
  
}

void Mirob::miroblog(relacs::Str text){
}


}; /* namespace misc */
