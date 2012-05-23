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
#include <relacs/relacswidget.h>

#define WAIT      1
#define STOP      1
#define DONT_WAIT 0
#define DONT_STOP 0
#define	NO_ADDITIVE	0
#define FORWARD true
#define BACKWARD false



using namespace std;
using namespace relacs;

namespace misc {

void miroblog(const char* text, watchdog_data* info){
  if (info->logBox != NULL){
    info->logBox->append(QString(text));
  }
}

void *watchdog(void* ptr){
  long p[3] = {0,0,0};
  int p2[3] = {0,0,0};

  watchdog_data* info = (watchdog_data*)ptr;

  miroblog("Unleashing watchdog",info);
  /************ open watchdog ****************/
  // open device:
  if ( TS_OpenChannel( info->Device, info->ChannelType, info->HostID, info->Baudrate ) < 0 ) {
    cerr << "WATCHDOG Communication error! " << TS_GetLastErrorText() << endl;
    return NULL;
  }

  // load setup file:
  int setupindex = TS_LoadSetup( info->SetupFile );
  if ( setupindex < 0 ) {
    cerr << "WATCHDOG Failed to load setup file! " << TS_GetLastErrorText() << endl;
    return NULL;
  }

  //   setup axis:
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SetupAxis( k, setupindex ) )  {
      cerr << "WATCHDOG Failed to setup axis " << k << "! " << TS_GetLastErrorText() << endl;
      return NULL;
    }
    if ( ! TS_SelectAxis( k ) ) {
      cerr << "WATCHDOG Failed to select axis " << k << "! " << TS_GetLastErrorText() << endl;
      return NULL;
    }

    if ( ! TS_SetTargetPositionToActual()){
      cerr << "WATCHDOG Failed to set target position to actual for axis " << k << "! " << TS_GetLastErrorText() << endl;
      return NULL;
    }

    if ( ! TS_DriveInitialisation() ) {
      cerr << "WATCHDOG Failed to initialize drive for axis " << k << "! " << TS_GetLastErrorText() << endl;
      return NULL;
    }
    if ( ! TS_Power( POWER_ON ) ) {
      cerr << "WATCHDOG Failed to power on drive for axis " << k << "! " << TS_GetLastErrorText() << endl;
      return NULL;
    }

    WORD axison = 0;
    while ( axison == 0 ) {
      // Check the status of the power stage:
      if ( ! TS_ReadStatus( REG_SRL, axison ) ) {
	cerr << "WATCHDOG Failed to read status for axis " << k << "! " << TS_GetLastErrorText() << endl;
	return NULL;
      }
      axison = ((axison & 1<<15) != 0 ? 1 : 0);
    }
  }
  // set active state true to signal Mirob that watchdog was created
  info->active = true;
  

  /************ watchdog on the watch ****************/
  int axis = 1;
  BYTE limitNeg[3] = {1,1,1};
  BYTE limitPos[3] = {1,1,1};

  while (info->active){
    // --------- read important information -----------
    for (axis = 1; axis <= 3; ++axis){

      // select axis:
      if ( ! TS_SelectAxis( axis ) ) {
	cerr << "WATCHDOG Failed to select axis " << axis << "! " << TS_GetLastErrorText() << endl;
	continue;
      }

      // check low limit switch
      if(!TS_GetInput(INPUT_24,limitNeg[axis-1])){
	cerr << "WATCHDOG Failed to get INPUT_24 for axis " << axis << "! " << TS_GetLastErrorText() << endl;
	continue;
      }
      // check high limit switch
      if(!TS_GetInput(INPUT_2,limitPos[axis-1])){
	cerr << "WATCHDOG Failed to get INPUT_2 for axis " << axis << "! " << TS_GetLastErrorText() << endl;
	continue;
      }
      
      // read position:
      if ( ! TS_GetLongVariable( "APOS", p[axis-1]) ) {
 	cerr << "WATCHDOG Failed to read position of axis " << axis << "! " << TS_GetLastErrorText() << endl;
	continue;
      }
      p2[axis-1] = p[axis-1]; // hack to account for TML_Lib bug

//       if ( ! TS_ReadStatus( REG_MCR, reg[3] ) ) {
// 	cerr << "Failed to read status for axis " << axis << "! " << TS_GetLastErrorText() << endl;
// 	return NULL;
//       }
      
      
    }

    //-------- check for whether watchdog is in forbidden zone -----------
    if (info->forbiddenZones->insideZone((double)p2[0],(double)p2[1],(double)p2[2])){
      if (!info->stopped){
	miroblog("WATCHDOG Mirob entered forbidden zone! Stopping it!",info);
	for (axis = 1; axis <=3; ++axis){
	  if ( !TS_SelectAxis( axis ) ){
	    cerr << "WATCHDOG Problem with selecting axis! " << TS_GetLastErrorText() << endl;
	  }
	  if ( !TS_Stop( ) ){
	    cerr << "WATCHDOG Could not stop Mirob! " << TS_GetLastErrorText() << endl;
	  }
	}
	info->stopped = true;
	miroblog("WATCHDOG Carefully move Mirob ouside zone!",info);
      }
    }else if(info->stopped){
      miroblog("WATCHDOG Mirob ouside zone again! ",info);
      info->stopped = false;
    }
    
    //--------- call position callback if exists --------
     if (info->XPosLCD != NULL && info->YPosLCD != NULL && info->ZPosLCD != NULL){
       info->XPosLCD->display((double)p2[0]);
       info->YPosLCD->display((double)p2[1]);
       info->ZPosLCD->display((double)p2[2]);
     }
    //------------ check for errors ---------------------
    for (axis = 1; axis <=3; ++axis){

      if (info->watchLimits){
	// ---- handle negative  limit hit
	if ( (int)limitNeg[axis-1] == 0){
	  miroblog("WATCHDOG negative limit hit! Trying to fix this !",info);

	  if ( !TS_SelectAxis( axis ) ){
	    cerr << "WATCHDOG Problem with selecting axis! " << TS_GetLastErrorText() << endl;
	  }
	  if(!TS_Execute("var_i1 = 0x0832; (var_i1),dm=1")) 
	    cerr << "WATCHDOG could not issue execute! " << TS_GetLastErrorText() << endl;

	  // Execute jogging with the prescribed parameters; start motion immediately
	  if(!TS_MoveVelocity(30, 0.3 , UPDATE_IMMEDIATE, FROM_MEASURE)){
	    cerr << "WATCHDOG Failed to move to limit for axis " << axis \
		 << "! " << TS_GetLastErrorText() << endl;
	  }

	  //Wait for positive limit switch to be reached. Stop at limit switch reach.
	  if (!TS_SetEventOnLimitSwitch(LSW_NEGATIVE, TRANSITION_LOW_TO_HIGH, WAIT, STOP)){
	    cerr << "WATCHDOG Failed to wait for negative limit switch event for axis " \
		 << axis << "! " << TS_GetLastErrorText() << endl;
	  }

  	  if(!TS_Execute("var_i1 = 0x0832; (var_i1),dm=0")) 
	    cerr << "WATCHDOG could not issue execute! " << TS_GetLastErrorText() << endl;


	}
	// ------ handle positive limit hit
	if ( (int)limitPos[axis-1] == 0){
	  cerr << "WATCHDOG positive limit hit on axis " << axis << "! Moving axis back into limits!" << endl;

	  if ( !TS_SelectAxis( axis ) )
	    cerr << "WATCHDOG Problem with selecting axis! " << TS_GetLastErrorText() << endl;

	  if(!TS_Execute("var_i1 = 0x0832; (var_i1),dm=1")) 
	    cerr << "WATCHDOG could not issue execute! " << TS_GetLastErrorText() << endl;


	  // Execute jogging with the prescribed parameters; start motion immediately
	  if(!TS_MoveVelocity(-30, 0.3 , UPDATE_IMMEDIATE, FROM_MEASURE)){
	    cerr << "WATCHDOG Failed to move to limit for axis " << axis \
		 << "! " << TS_GetLastErrorText() << endl;
	  }

	  //Wait for positive limit switch to be reached. Stop at limit switch reach.
	  if (!TS_SetEventOnLimitSwitch(LSW_POSITIVE, TRANSITION_LOW_TO_HIGH, WAIT, STOP)){
	    cerr << "WATCHDOG Failed to wait for positive limit switch event for axis " \
		 << axis << "! " << TS_GetLastErrorText() << endl;
	  }

	  if(!TS_Execute("var_i1 = 0x0832; (var_i1),dm=0")) 
	    cerr << "WATCHDOG could not issue execute! " << TS_GetLastErrorText() << endl;

	}
      }
      

    }


    
    nanosleep(&info->sleeptime,NULL);
  }
  
  /************ close watchdog ****************/
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SelectAxis( k ) )
      cerr << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << endl;
    if ( ! TS_Stop() )
      cerr << "Failed to stop motion for axis " << k << "! " << TS_GetLastErrorText() << endl;
    if ( ! TS_Power( POWER_OFF ) )
      cerr << "Failed to power off drive for axis " << k << "! " << TS_GetLastErrorText() << endl;
  }
  TS_CloseChannel( -1 );
  
  cerr << "WATCHDOG closed " << endl;

  return NULL;
}


const string Mirob::SetupFile = "mirob2.t.zip";


/********************* initialization part **************************/

Mirob::Mirob( const string &device )
  : Manipulator( "Mirob" )
{
  Opened = false;
  for ( int k=0; k<3; k++ ) {
    Speed[k] = MaxSpeed;
    Acceleration[k] = MaxAcc;
  }
  Options opts;
  open( device, opts );
}

Mirob::Mirob( void )
  : Manipulator( "Mirob" )
{
  for ( int k=0; k<3; k++ ) {
    Speed[k] = MaxSpeed;
    Acceleration[k] = MaxAcc;
  }
  Opened = false;
}

Mirob::~Mirob( void )
{
  close();
}

int Mirob::open( const string &device, const Options &opts )
{
  cerr << "MIROB open " << device << '\n';
  if ( Opened )
    return 0;

  Info.clear();
  Settings.clear();


  // open device:
  if ( TS_OpenChannel( device.c_str(), ChannelType, HostID, Baudrate ) < 0 ) {
    cerr << "Communication error! " << TS_GetLastErrorText() << '\n';
    return InvalidDevice;
  }

  // load setup file:
  int setupindex = TS_LoadSetup( SetupFile.c_str() );
  if ( setupindex < 0 ) {
    cerr << "Failed to load setup file! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  // setup axis:
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SetupAxis( k, setupindex ) )  {
      cerr << "Failed to setup axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_SelectAxis( k ) ) {
      cerr << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    
    if ( ! TS_SetTargetPositionToActual()){
      cerr << "Failed to set target position to actual for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    

    if ( ! TS_DriveInitialisation() ) {
      cerr << "Failed to initialize drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    
    if ( ! TS_Power( POWER_ON ) ) {
      cerr << "Failed to power on drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    WORD axison = 0;
    while ( axison == 0 ) {
      // Check the status of the power stage:
      if ( ! TS_ReadStatus( REG_SRL, axison ) ) {
	cerr << "Failed to read status for axis " << k << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }
      axison = ((axison & 1<<15) != 0 ? 1 : 0);
    }
  }
  /**** deploy watchdog thread *********************************/
  watchdog_info.ChannelType = ChannelType;
  watchdog_info.HostID = HostID;
  watchdog_info.Baudrate = Baudrate;
  watchdog_info.SetupFile = SetupFile.c_str();
  watchdog_info.Device = device.c_str();
  watchdog_info.XPosLCD = NULL;
  watchdog_info.YPosLCD = NULL;
  watchdog_info.ZPosLCD = NULL;
  watchdog_info.logBox = NULL;
  
  startWatchdog();

  /*************************************************************/


  setDeviceName( "Mirob" );
  setDeviceVendor( "MPH" );
  setDeviceFile( device );
  Device::addInfo();

  

  Opened = true;

  return 0;
}


int Mirob::startWatchdog(void){
  watchdog_info.active = false;
  watchdog_info.active = false;
  watchdog_info.watchLimits = true;
  watchdog_info.forbiddenZones = &forbiddenZones;
  watchdog_info.sleeptime.tv_sec = watchdog_sleep_sec;
  watchdog_info.sleeptime.tv_nsec = watchdog_sleep_nsec;


  cerr << "Creating watchdog ";
  if ( pthread_create( &watchdog_thread, NULL, watchdog, (void*) &watchdog_info) > 0 ) return 1;

  int waitcycle = 0;
  struct timespec waitForDog;
  waitForDog.tv_sec = 1;
  waitForDog.tv_nsec = 0;

  while (watchdog_info.active == false && waitcycle++ < 10){
    cerr << ".";
    nanosleep(&waitForDog,NULL);
  }
  if (waitcycle == 11){
    cerr << "[FAILED]" <<  endl;
    return 1;
  }
  
  cerr << "[OK]" << endl;
  return 0;
}

int Mirob::stopWatchdog(void){
  // closing watchdog
  watchdog_info.active = false;
  pthread_join(watchdog_thread, NULL); // wait for watchdog to return
  return 0;
}


int Mirob::restartWatchdog(void){
  stopWatchdog();
  startWatchdog();
  return 0;
}


void Mirob::setPosLCDs(QLCDNumber* a, QLCDNumber* b, QLCDNumber*c ){
    watchdog_info.XPosLCD = a;
    watchdog_info.YPosLCD = b;
    watchdog_info.ZPosLCD = c;
}

void Mirob::setLogBox(QTextEdit* box){
    watchdog_info.logBox = box;
}


void Mirob::close( void )
{
  if ( Opened ) {
    for ( int k=1; k<=3; k++ ) {
      if ( ! TS_SelectAxis( k ) )
	cerr << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << '\n';
      if ( ! TS_Stop() )
	cerr << "Failed to stop motion for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      if ( ! TS_Power( POWER_OFF ) )
	cerr << "Failed to power off drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
    }
    TS_CloseChannel( -1 );
  }
  Opened = false;
  Info.clear();
  Settings.clear();

  stopWatchdog();
  cerr << "MIROB closed " << endl;
}


/********************* activation and reactivation part *************/

int Mirob::reset( void )
{
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SelectAxis( k ) ) {
      cerr << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_ResetFault() ) {
      cerr << "Failed to reset fault axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_Power( POWER_ON ) ) {
      cerr << "Failed to power on drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    WORD axison = 0;
    while ( axison == 0 ) {
      // Check the status of the power stage:
      if ( ! TS_ReadStatus( REG_SRL, axison ) ) {
	cerr << "Failed toread status for axis " << k << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }
      axison = ((axison & 1<<15) != 0 ? 1 : 0);
    }
    /*
    if ( ! TS_Reset() ) {
      cerr << "Failed to reset axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
   // Requires much more setup afterwards!
    */
  }

  return 0;
}

int Mirob::syncTposApos(void){
  cerr << "MIROB Setting target position to actual position!" << endl;
  for (int k = 1; k <= 3; ++k){
    if ( ! TS_SetTargetPositionToActual()){
      cerr << "Failed to set target position to actual for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
  }
  return 0;

}

int Mirob::activateAxis(int ax){
    if ( ! TS_SelectAxis(ax ) ) {
      cerr << "Failed to select axis " << ax << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    return 0;
}

/****************** velocity part *******************************/
int Mirob::getV(double &x, double &y, double &z){

  if (activateAxis(1) > 0 || !TS_GetFixedVariable("ASPD",x)){
    cerr << "Failed to get APOS for axis 1! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  if (activateAxis(2) > 0 || !TS_GetFixedVariable("ASPD",y)){
    cerr << "Failed to get APOS for axis 2! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  if (activateAxis(3) > 0 || !TS_GetFixedVariable("ASPD",z)){
    cerr << "Failed to get APOS for axis 3! " << TS_GetLastErrorText() << '\n';
    return 1;
  }
  
  return 0;
}

int Mirob::setV(double v, int ax){
  if (activateAxis(ax) > 0 || !TS_MoveVelocity(v,MaxAcc,UPDATE_IMMEDIATE,FROM_MEASURE)){
    cerr << "Failed to set velocity "<< v << " for axis " << ax << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }
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
  cerr << "MIROB Stop!" << endl;
  for (int axis = 3; axis >= 1; --axis){
    if (activateAxis(axis) > 0) return 1;

    if (!TS_Stop()){
	cerr << "Could not stop Mirob! " << TS_GetLastErrorText() << endl;
	return 1;
    }
    if (!TS_Power(POWER_OFF)){
	cerr << "Could not power off Mirob! " << TS_GetLastErrorText() << endl;
	return 1;
    }
    if (!TS_Power(POWER_ON)){
	cerr << "Could not power on Mirob! " << TS_GetLastErrorText() << endl;
	return 1;
    }


  }  
  return 0;
}
                             
int Mirob::gotoNegLimitsAndSetHome(void){
  BYTE var;
  var = 0;
  for (int axis = 3; axis >= 1; --axis){
    if (activateAxis(axis) > 0) return 1;

    if(!TS_GetInput(INPUT_24,var)){
      cerr << "Failed to get INPUT_24 for axis " << axis << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }

    if (var == 1){
 //      cerr << "MIROB axis "  << axis << " INPUT_24=" << (int)var << endl;

      watchdog_info.watchLimits = false; // switch off watchdog

      if(!TS_Execute("var_i1 = 0x0832; (var_i1),dm=1")) {
	cerr << "Could not issue execute! " << TS_GetLastErrorText() << endl;
	return 1;
      }

      // Execute jogging with the prescribed parameters; start motion immediately
      if(!TS_MoveVelocity(-MaxSpeed, MaxAcc , UPDATE_IMMEDIATE, FROM_REFERENCE)){
	cerr << "Failed to move to limit for axis " << axis \
	     << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }

      //Wait for positive limit switch to be reached. Stop at limit switch reach.
      if (!TS_SetEventOnLimitSwitch(LSW_NEGATIVE, TRANSITION_HIGH_TO_LOW, WAIT, STOP)){
	cerr << "Failed to wait for negative limit switch event for axis " \
	     << axis << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }


      // drive back
      if(!TS_MoveVelocity(10, MaxAcc , UPDATE_IMMEDIATE, FROM_REFERENCE)){
	cerr << "Failed to move to limit for axis " << axis \
	     << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }

      //Wait for positive limit switch to be reached. Stop at limit switch reach.
      if (!TS_SetEventOnLimitSwitch(LSW_NEGATIVE, TRANSITION_LOW_TO_HIGH, WAIT, STOP)){
	cerr << "Failed to wait for negative limit switch event for axis " \
	     << axis << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }

      if(!TS_Execute("var_i1 = 0x0832; (var_i1),dm=0")) {
	cerr << "Could not issue execute! " << TS_GetLastErrorText() << endl;
	return 1;
      }
      //      TS_Stop();

    }else{
      cerr << "MIROB already at negative limit switch for axis "\
	   << axis << "!" << endl;
    }
    if (!TS_SetPosition(0)){
	cerr << "Failed to set position to 0 for axis " << axis \
	     << "! " << TS_GetLastErrorText() << '\n';
	return 1;

    }
    
    watchdog_info.watchLimits = true; // switch on watchdog


  }

  return 0;
}


int Mirob::step( double x, int axis )
{
  cerr << "AXIS " << axis << " step by " << x << '\n';
  // select axis:
  if ( ! TS_SelectAxis( axis ) ) {
    cerr << "Failed to select axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }
  // move:
  long step = long( rint( x ) );
  if ( ! TS_MoveRelative( step, Speed[axis-1], Acceleration[axis-1],
			  NO_ADDITIVE, UPDATE_IMMEDIATE, FROM_REFERENCE ) ) {
    cerr << "Failed to move on axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }
  // wait:
  if( ! TS_SetEventOnMotionComplete( WAIT, DONT_STOP ) ) { 
    cerr << "Failed to wait on axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  return 0;
}

int Mirob::absPos( double x, double y, double z, double speed )
{
  speed = (speed > MaxSpeed) ? MaxSpeed : speed;
  double d[3] = {x,y,z};
  
  double dx,dy,dz;
  dx = abs(posX() - x);
  dy = abs(posY() - y);
  dz = abs(posZ() - z);

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
      cerr << "Failed to move absolute on axis " << axis << "! " << TS_GetLastErrorText() << '\n';
      return 1;

    }

//     // wait:
//     if( ! TS_SetEventOnMotionComplete( WAIT, DONT_STOP ) ) { 
//       cerr << "Failed to wait on axis " << axis << "! " << TS_GetLastErrorText() << '\n';
//       return 1;
//     }
    
  }

  return 0;
}


int Mirob::suspendUntilPositionReached(double x, \
      double y, double z, double tol){
  
  double apos[3] = {-1,-1,-1};
  double tpos[3] = {x,y,z};
  
  while (true){
    for (int axis = 1; axis <= 3; ++axis){
      if (activateAxis(axis) > 0) return 1;
      apos[axis-1] = pos(axis);
    }
    
    cerr << "MIROB Pos " << apos[0] << ", " \
	   << apos[1] << ", " \
	   << apos[2] << endl;
    
      
    if ( (abs(apos[0] - tpos[0]) > tol) | 
	 (abs(apos[1] - tpos[1]) > tol) | 
	 (abs(apos[2] - tpos[2]) > tol)){
      nanosleep(&watchdog_info.sleeptime,NULL);
    }else{
      cerr << "MIROB position reached! " << endl;
      return 0;
    }
    
  }
  return 0;
  
}

int Mirob::suspendUntilStop(){
  double vx, vy, vz;

  while (true){
    for (int axis = 1; axis <= 3; ++axis){
      if (activateAxis(axis) > 0) return 1;
      getV(vx,vy,vz);

      cerr << "\rMIROB Speed " << vx << ", " \
	   << vy << ", " << vz ;
      
      if (vx < 1 && vy < 1 && vz < 1){
	return 0;
      }else{
	nanosleep(&watchdog_info.sleeptime,NULL);
      }
    }
  }
  return 0;
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

double Mirob::pos( int axis ) const
{
  // select axis:
  if ( ! TS_SelectAxis( axis ) ) {
    cerr << "Failed to select axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  // read position:
  long apos = 0;
  if ( ! TS_GetLongVariable( "APOS", apos) ) {
    cerr << "Failed to read position of axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 0.0;
  }

  int apos2 = apos; // this is a hack to account for the bug in TML_lib
  return (double) apos2;
}

double Mirob::posX( void ) const
{
  return pos( 1 );
}

double Mirob::posY( void ) const
{
  return pos( 2 );
}

double Mirob::posZ( void ) const
{
  return pos( 3 );
}

int Mirob::step(double dx, double dy, double dz, double speed, bool wait){
  
  /*	Motion parameters for axis 1 */	
  double v_len = sqrt(dx*dx + dy*dy + dz*dz );


  if (speed > MaxSpeed)
    speed = MaxSpeed;

  double vx = speed * abs(dx)/v_len;
  double vy = speed * abs(dy)/v_len;
  double vz = speed * abs(dz)/v_len;
  double v[3] = {vx,vy,vz};
  long d[3] = {long(rint(dx)),long(rint(dy)),long(rint(dz))};


  for (int k = 1; k <= 3; ++k){
    if (activateAxis(k) > 0|| \
	!TS_MoveRelative(d[k-1], v[k-1], MaxAcc, NO_ADDITIVE, \
			 UPDATE_IMMEDIATE, FROM_REFERENCE)){
      cerr << "Failed to step at axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
  }

  if (wait){
    suspendUntilStop();
  }
  
  return 0;
}

int Mirob::clear(int axis)
{
  // select axis:
  if ( ! TS_SelectAxis( axis ) ) {
    cerr << "Failed to select axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }
  if (!TS_SetPosition(0)){
    cerr << "Failed to set home position for axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }
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
  /*
  double dist = -Pos[0];
  double steps = 0.0;
  if ( dist > 0.0 )
    steps = dist / PosAmplitude[0] / PosGain[0];
  else
    steps = dist / NegAmplitude[0] / NegGain[0];
  return stepX( steps );
  */
  return stepX(-posX());
}


int Mirob::homeY( void )
{
  /*
  double dist = -Pos[1];
  double steps = 0.0;
  if ( dist > 0.0 )
    steps = dist / PosAmplitude[1] / PosGain[1];
  else
    steps = dist / NegAmplitude[1] / NegGain[1];
  return stepY( steps );
  */
  return stepY(-posY());
}


int Mirob::homeZ( void )
{
  /*
  double dist = -Pos[2];
  double steps = 0.0;
  if ( dist > 0.0 )
    steps = dist / PosAmplitude[2] / PosGain[2];
  else
    steps = dist / NegAmplitude[2] / NegGain[2];
  return stepZ( steps );
  */
  return stepZ(-posZ());
}


int Mirob::home( void )
{
  return homeX() + ( homeY() << 1 ) + ( homeZ() << 2 );
}


int Mirob::setAmplX( double posampl, double negampl )
{
  /*
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 0, pa );
    amplitudeneg( 0, na );
    return 0;
  }
  else
    return 1;
  */
  return 0;
}


int Mirob::setAmplY( double posampl, double negampl )
{
  /*
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 1, pa );
    amplitudeneg( 1, na );
    return 0;
  }
  else
    return 2;
  */
  return 0;
}


int Mirob::setAmplZ( double posampl, double negampl )
{
  /*
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 2, pa );
    amplitudeneg( 2, na );
    return 0;
  }
  else
    return 4;
  */
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
  if (activateAxis(3) > 0 || !TS_SetOutput(OUTPUT_30, IO_HIGH) ||!TS_SetOutput(OUTPUT_31, IO_LOW) ){
    cerr << "Failed to clamp tool!" << TS_GetLastErrorText() << '\n';
    return 1;
  }
  return 0;
}

int Mirob::releaseTool(void){

  if (activateAxis(3) > 0 || !TS_SetOutput(OUTPUT_31, IO_HIGH) ||!TS_SetOutput(OUTPUT_30, IO_LOW) ){
    cerr << "Failed to release tool!" << TS_GetLastErrorText() << '\n';
    return 1;
  }
  return 0;
}



/**************** trajectory part  ***************************/
int Mirob::startRecording(void){
  syncTposApos();
  cerr << "MIROB position recording started! " << endl;
  record0 = *new positionUpdate(posX(), posY(), posZ());
  recordedSteps.clear();
  return 0;
}

int Mirob::recordStep(void){
  syncTposApos();
  //positionUpdate tmp;
  int l = recordedSteps.size();

  cerr << "posX " << posX() << endl;
  cerr << "posY " << posY() << endl;
  cerr << "posZ " << posZ() << endl;

  // record step
  positionUpdate tmp(posX() - record0.x, \
		     posY() - record0.y, \
		     posZ() - record0.z);

  recordedSteps.push_back(tmp);
  
  // update position
  record0.x = posX();
  record0.y = posY();
  record0.z = posZ();
  
  cerr << "Recorded step " << recordedSteps[l].x << ", " <<  recordedSteps[l].y << ", "<<  recordedSteps[l].z << endl;
  cerr << "New position is " << record0.x << ", " <<  record0.y << ", "<<  record0.z << endl;
  cerr << "Recorded " << l+1 << " updates!" << endl;
  return 0;
}

int Mirob::stopRecording(){
  cerr << "MIROB position recording stopped! " << endl;
  return 0;
}

int Mirob::executeRecordedTrajectory(double speed, bool forward, bool wait){
  syncTposApos();
  int l = recordedSteps.size(), start,stop;
  double sgn, update;
  if (forward){
    sgn = 1; 
    update = 1;
    start = 0;
    stop = l;
  }else{
    sgn = -1;
    update = -1;
    start = l-1;
    stop  = -1;
  }
  
  for (; start != stop; start += update){
    cerr << "Executing step " << sgn*recordedSteps[start].x\
	 << ", " << sgn*recordedSteps[start].y \
	 << ", " << sgn*recordedSteps[start].z << endl;
    step(sgn*recordedSteps[start].x, \
	 sgn*recordedSteps[start].y, \
	 sgn*recordedSteps[start].z, speed, wait);
  }

  return 0;
  

}

/******** forbidden zone stuff ************************************/

int Mirob::recordPosition(void){
  positions.push_back( Point3D(posX(), posY(), posZ()) );
  int l = positions.size();
  
  cerr << "MIROB: Recorded position " << positions[l-1].x
       << ", " << positions[l-1].y << ", " << positions[l-1].z
       << ": " << l << " positions in total!" <<  endl;
  return 0;
}
  
int Mirob::clearPositions(void){
  cerr << "MIROB clearing positions!" << endl;
  positions.clear();
  return 0;
}

int Mirob::makePositionsForbiddenZone(void){
  forbiddenZones.addZone(positions);
  clearPositions();
  return 0;
}



}; /* namespace misc */
