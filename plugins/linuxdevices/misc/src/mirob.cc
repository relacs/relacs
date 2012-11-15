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
#include <relacs/misc/tinyxml2.h>
#include <sstream>

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
  result[0][1] = -(A[0][1]*A[2][2]-A[0][2]*A[2][1])*invdet;
  result[0][2] =  (A[0][1]*A[1][2]-A[0][2]*A[1][1])*invdet;
  result[1][0] = -(A[1][0]*A[2][2]-A[1][2]*A[2][0])*invdet;
  result[1][1] =  (A[0][0]*A[2][2]-A[0][2]*A[2][0])*invdet;
  result[1][2] = -(A[0][0]*A[1][2]-A[1][0]*A[0][2])*invdet;
  result[2][0] =  (A[1][0]*A[2][1]-A[2][0]*A[1][1])*invdet;
  result[2][1] = -(A[0][0]*A[2][1]-A[2][0]*A[0][1])*invdet;
  result[2][2] =  (A[0][0]*A[1][1]-A[1][0]*A[0][1])*invdet;

  return 0;
}


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
/*************************************************************/

int Mirob::loadConfigurationFile(){
  //  XMLDocument xml;
  xml.LoadFile( configFileName.c_str());
  XMLElement* root = xml.RootElement();

  // extract max speed
  double tmp;
  root->FirstChildElement("maxspeed")->QueryDoubleText(&tmp);
  MaxSpeed = (long)tmp;
  // extract maximal acceleration
  root->FirstChildElement("maxacceleration")->QueryDoubleText(&MaxAcc);

  robotDaemon_info.SetupFile = root->FirstChildElement("setupfile")->GetText();
  
  
  int tmp2;
  root->FirstChildElement("hostid")->QueryIntText(&tmp2);
  robotDaemon_info.HostID = (BYTE)tmp2;

  
  root->FirstChildElement("daemonsleeptime")->FirstChildElement("seconds")->QueryIntText(&tmp2);
  robotDaemon_info.sleeptime.tv_sec = tmp2;

  root->FirstChildElement("daemonsleeptime")->FirstChildElement("nanoseconds")->QueryIntText(&tmp2);
  robotDaemon_info.sleeptime.tv_nsec = tmp2;

  root->FirstChildElement("baudrate")->QueryIntText(&tmp2);
  robotDaemon_info.Baudrate = tmp2;

  int i = 0,j=0;
  for( const XMLElement* node=root->FirstChildElement("basis")->FirstChildElement("row"); 
       node; node=node->NextSiblingElement("row") ) {
    j = 0;
    for ( const XMLElement* node2=node->FirstChildElement("col"); 
	  node2; node2=node2->NextSiblingElement("col") ) {
      node2->QueryDoubleText(&B[i][j++]);
    }
    ++i;
  }
  i = 0;
  for ( const XMLElement* node2=
	  root->FirstChildElement("basis")->FirstChildElement("offspring")->FirstChildElement("col"); 
	node2; node2=node2->NextSiblingElement() ) {
      node2->QueryDoubleText(&b0[i++]);
  }



  inv3(B,iB);
  return 0;
}


/*************************************************************/

int Mirob::saveConfigurationFile(){
  XMLElement* root = xml.RootElement();
  if (root->BoolAttribute("overwrite")){
    stringstream ss;
   
    // save the basis matrix
    int i = 0,j=0;
    for(  XMLElement* node=root->FirstChildElement("basis")->FirstChildElement("row"); 
	 node; node=node->NextSiblingElement("row") ) {
      j = 0;
      for (  XMLElement* node2=node->FirstChildElement("col"); 
	    node2; node2=node2->NextSiblingElement("col") ) {
	node2->DeleteChildren();

	ss << B[i][j++];
	node2->InsertEndChild(xml.NewText(ss.str().c_str()));
        ss.str( "" );  ss.clear();

      }
      ++i;
    }

    // save the coordinate offstring
    i = 0;
    for ( XMLElement* node2=
    	    root->FirstChildElement("basis")->FirstChildElement("offspring")->FirstChildElement("col"); 
    	  node2; node2=node2->NextSiblingElement() ) {
	node2->DeleteChildren();

	ss << b0[i++];
	node2->InsertEndChild(xml.NewText(ss.str().c_str()));
        ss.str( "" );  ss.clear();

    }


    xml.SaveFile(configFileName.c_str());
  }
  return 0;

}

/*************************************************************/

int Mirob::open( const string &device, const Options &opts )
{
  cerr << LOGPREFIX << "Opening MIROB on " << device << '\n';
  if ( Opened )
    return 0;

  Info.clear();
  Settings.clear();

  CoordinateMode = MIROB_COORD_RAW;


  configFileName = opts.text("config");
  loadConfigurationFile();

  /* Start Robot Daemon */
  robotDaemon_info.Device = device.c_str();
  robotDaemon_info.active = false;
  robotDaemon_info.forbiddenZones = &forbiddenZones;
  robotDaemon_info.v[0] = 0.0;
  robotDaemon_info.v[1] = 0.0;
  robotDaemon_info.v[2] = 0.0;
  robotDaemon_info.vChanged = true;
  robotDaemon_info.toolClamped = false;
  robotDaemon_info.clampChanged = false;
  robotDaemon_info.state = ROBOT_HALT;
  robotDaemon_info.ChannelType = CHANNEL_RS232;
  robotDaemon =  new TMLRobotDaemon(&robotDaemon_info);

  cerr << LOGPREFIX << " waiting for daemon to start" << endl;
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

  void Mirob::setState(int state)
  { 
    pthread_mutex_lock( &robotDaemon_info.mutex );
    robotDaemon_info.state = state;
    pthread_mutex_unlock( &robotDaemon_info.mutex );
  }

/*************************************************************/


void Mirob::close( void )
{

  Info.clear();
  Settings.clear();

  robotDaemon->Shutdown();
  Opened = false;
  cerr << LOGPREFIX << "Closed! " << endl;
  cerr << LOGPREFIX << "Saving configuration ..." << endl;
  saveConfigurationFile();
}



/********************* activation and reactivation part *************/

int Mirob::reset( void )
{
  return robotDaemon->reset();
}


/****************** velocity part *******************************/
int Mirob::getV(double &x, double &y, double &z){
  x = robotDaemon_info.v[0];
  y = robotDaemon_info.v[1];
  z = robotDaemon_info.v[2];
  
  if (CoordinateMode == MIROB_COORD_TRANS){
    transformVelocities(x,y,z,TRANS2RAW);
  }
  return 0;
}

/*************************************************************/


int Mirob::setV(double v, int ax){

  // get current velocity along the axis 
  double currV[3];
  for (int i = 0; i != 3; ++i){
    currV[i] = robotDaemon_info.v[i];
  }
  
  if (CoordinateMode == MIROB_COORD_TRANS){
    transformVelocities(currV,RAW2TRANS);
    currV[ax-1] = v;
    transformVelocities(currV,TRANS2RAW);
  }else if (CoordinateMode == MIROB_COORD_RAW){
    currV[ax-1] = v;
  }else{
    cerr << LOGPREFIX << "setV(double, int) Coordinate Mode not known!\n";
    return 1;
  }

  // limit velocities to maxspeed
  double speed = sqrt(currV[0]*currV[0] + currV[1]*currV[1] + currV[2]*currV[2] );
  if (speed > MaxSpeed){
    for (int i = 0; i != 3; ++i){
      currV[i] *=MaxSpeed/speed;
    }
  }

  pthread_mutex_lock( &robotDaemon_info.mutex );
  
  for (int i=0; i != 3; ++i){
    robotDaemon_info.v[i] = currV[i];
  }
  robotDaemon_info.vChanged = true;
  pthread_mutex_unlock( &robotDaemon_info.mutex );

  return 0;
}
/*************************************************************/

int Mirob::setVX(double v){
  return setV(v,1);
}
/*************************************************************/

int Mirob::setVY(double v){
  return setV(v,2);
}
/*************************************************************/

int Mirob::setVZ(double v){
  return setV(v,3);
}
/*************************************************************/

int Mirob::setV(double vx, double  vy, double vz){
  
  if (CoordinateMode == MIROB_COORD_TRANS){
    transformVelocities(vx,vy,vz,TRANS2RAW);
  }


  return (setVX(vx) + setVY(vy) + setVZ(vz) > 0) ? 1 : 0; 

}


/***************** positioning part *****************************/

int Mirob::stop(){
  return robotDaemon->Stop();
}
/*************************************************************/
                            
int Mirob::step( double x, int axis )
{
  if (axis == 1){
    return setPos(pos(1) + x, pos(2), pos(3), MaxSpeed);
  }else if (axis == 2){
    return setPos(pos(1), pos(2) + x, pos(3), MaxSpeed);
  }else if (axis == 3){
    return setPos(pos(1), pos(2), pos(3) + x, MaxSpeed);
  }
  cerr << LOGPREFIX << "Axis " << axis << " in step(double, int) not known!" << '\n';
  return 1;
}
/*************************************************************/

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
/*************************************************************/

double* Mirob::transformVelocities(double* x, int direction){
  transformVelocities(x[0],x[1],x[2],direction);
  return x;
}
/*************************************************************/

double* Mirob::transformCoordinates(double* x, int direction){
  transformCoordinates(x[0],x[1],x[2],direction);
  return x;
}
/*************************************************************/

void Mirob::transformCoordinates(double& x, double& y, double& z, int direction){
  double tmp[3];
  if (direction == RAW2TRANS){
    x -=  b0[0]; y -=  b0[1]; z -=  b0[2];
    for (int i = 0; i != 3; ++i){
      tmp[i] = iB[i][0]*x + iB[i][1]*y + iB[i][2]*z;
    }
  }else if (direction == TRANS2RAW){
    for (int i = 0; i != 3; ++i){
      tmp[i] = B[i][0]*x + B[i][1]*y + B[i][2]*z;
      tmp[i] += b0[i];
    }
  }
  x = tmp[0]; y = tmp[1]; z = tmp[2];
}
/*************************************************************/

void Mirob::transformVelocities(double& x, double& y, double& z, int direction){
  double tmp[3];
  if (direction == RAW2TRANS){
    for (int i = 0; i != 3; ++i){
      tmp[i] = iB[i][0]*x + iB[i][1]*y + iB[i][2]*z;
    }
  }else if (direction == TRANS2RAW){
    for (int i = 0; i != 3; ++i){
      tmp[i] = B[i][0]*x + B[i][1]*y + B[i][2]*z;
    }
  }
  x = tmp[0]; y = tmp[1]; z = tmp[2];
  
}

/*************************************************************/

int Mirob::stepX( double x )
{
  return step( x, 1 );
}
/*************************************************************/

int Mirob::stepY( double y )
{
  return step( y, 2 );
}
/*************************************************************/

int Mirob::stepZ( double z )
{
  return step( z, 3 );
}
/*************************************************************/

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
/*************************************************************/

double Mirob::posX( void ) 
{
  return pos( 1 );
}
/*************************************************************/

double Mirob::posY( void ) 
{
  return pos( 2 );
}

double Mirob::posZ( void ) 
{
  return pos( 3 );
}
/*************************************************************/

int Mirob::clear(int axis)
{
  return 0;
}
/*************************************************************/

int Mirob::clearX( void )
{
  return clear(1);
}
/*************************************************************/

int Mirob::clearY( void )
{
  return clear(2);
}
/*************************************************************/

int Mirob::clearZ( void )
{
  return clear(3);
}

/*************************************************************/

int Mirob::clear( void )
{
  clearX();
  clearY();
  clearZ();
  return 0;
}

/*************************************************************/

int Mirob::homeX( void )
{

  return stepX(-posX());
}

/*************************************************************/

int Mirob::homeY( void )
{

  return stepY(-posY());
}
/*************************************************************/


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

// void Mirob::miroblog(const char* text){
  
// }

// void Mirob::miroblog(relacs::Str text){
// }


}; /* namespace misc */
