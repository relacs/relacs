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


  Trajectory::Trajectory(){
    Calibrated = false;
    AnchorIndex = 0;
    Start = new PositionUpdate();
    Anchor = new PositionUpdate();

  }

  Trajectory::~Trajectory(){
    Calibrated = false;
    AnchorIndex = 0;
    delete Start;
    delete Anchor;
    for (vector<PositionUpdate*>::size_type k = 0; k != Nodes.size(); ++k){
      delete Nodes[k];
    }
  }

  

  Trajectory::Trajectory(const XMLElement* node){
    Calibrated = false;
    AnchorIndex = 0;
    Start = new PositionUpdate();
    Anchor = new PositionUpdate();

    bool isAnchor = false;
    int i = 0;
    PositionUpdate *tmp2;
    
    for( const XMLElement* node2=node->FirstChildElement("node"); 
	 node2; node2=node2->NextSiblingElement("node") ){
      tmp2 = new PositionUpdate();
      node2->FirstChildElement("x")->QueryDoubleText(&(tmp2->x));
      node2->FirstChildElement("y")->QueryDoubleText(&(tmp2->y));
      node2->FirstChildElement("z")->QueryDoubleText(&(tmp2->z));
      node2->FirstChildElement("v")->QueryDoubleText(&(tmp2->speed));
      Nodes.push_back(tmp2);
      
      node2->QueryBoolAttribute( "anchor", &isAnchor );
      if (isAnchor){
	AnchorIndex = i;
	isAnchor = false;
      }
      ++i;
    }
  }

  
  void Trajectory::setAnchor(PositionUpdate* a){
    delete Anchor;
    Anchor = a;
    Start->x = a->x - Nodes[AnchorIndex]->x;
    Start->y = a->y - Nodes[AnchorIndex]->y;
    Start->z = a->z - Nodes[AnchorIndex]->z;
    //cerr << "Setting Start and anchor to " << *Start << *Anchor << endl;
    Calibrated = true;
  }

  void Trajectory::setStart(PositionUpdate* s){
    delete Start;
    Start = s;
    Anchor->x = s->x + Nodes[AnchorIndex]->x;
    Anchor->y = s->y + Nodes[AnchorIndex]->y;
    Anchor->z = s->z + Nodes[AnchorIndex]->z;
    
    Calibrated = true;
  }

  PositionUpdate* Trajectory::resetToAnchor(double x, double y, double z){
    CurrentIndex = 0;
    Delta[0] = x - Nodes[AnchorIndex]->x;
    Delta[1] = y - Nodes[AnchorIndex]->y;
    Delta[2] = z - Nodes[AnchorIndex]->z;

      
    return new PositionUpdate(Nodes[0]->x + Delta[0],
			      Nodes[0]->y + Delta[1],
			      Nodes[0]->z + Delta[2], Nodes[0]->speed);

    
  }

  PositionUpdate* Trajectory::resetToAnchor(){
    if (!Calibrated){
      return NULL;
    }else{
      return resetToAnchor(Anchor->x, Anchor->y, Anchor->z);
    }
  }
 
  PositionUpdate* Trajectory::resetToStart(){
    if (!Calibrated){
      return NULL;
    }else{
      return resetToStart(Start->x, Start->y, Start->z);
    }
  }
 
  PositionUpdate* Trajectory::resetToStart(double x, double y, double z){
      CurrentIndex = 0;
      Delta[0] = x;
      Delta[1] = y;
      Delta[2] = z;
      return new PositionUpdate(Nodes[0]->x + Delta[0],
			    Nodes[0]->y + Delta[1],
			    Nodes[0]->z + Delta[2], Nodes[0]->speed);
  }

  PositionUpdate* Trajectory::next(){
    if (CurrentIndex == Nodes.size()-1){
      return NULL;
    }else{
      ++CurrentIndex;
      return new PositionUpdate(Nodes[CurrentIndex]->x + Delta[0],
			    Nodes[CurrentIndex]->y + Delta[1],
			    Nodes[CurrentIndex]->z + Delta[2], Nodes[CurrentIndex]->speed);
    }    
  }

//===================================================================

const char* Mirob::LOGPREFIX = "MIROB: ";

  typedef map<string, Trajectory* >::iterator trajIter;

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
  RobotDaemon = 0;
  Opened = false;
 
  Options opts;

  open( device, opts );
}

Mirob::Mirob( void )
  : Manipulator( "Mirob" )
{
  RobotDaemon = 0;
  
  Opened = false;
}

Mirob::~Mirob( void )
{
  for(map<string,Trajectory* >::iterator it = Trajectories.begin(); 
      it != Trajectories.end(); ++it) {
    delete it->second;
  }

  delete RobotDaemon;

  close();
}
/*************************************************************/

int Mirob::loadTrajectoryFile(const char * filename){
  XMLDocument txml;
  txml.LoadFile( filename);

  for( const XMLElement* node=txml.FirstChildElement("trajectory"); 
       node; node=node->NextSiblingElement("trajectory") ) {
      
      Trajectories.insert(pair<string, Trajectory* > 
			    (string(node->Attribute("name")),new Trajectory(node)));
  }
  return 0;
}
  
vector<string> Mirob::getTrajectoryKeys(){
  vector<string> retval;
  for(map<string,Trajectory* >::iterator it = Trajectories.begin(); 
      it != Trajectories.end(); ++it) {
    retval.push_back(it->first);
  }
  return retval;
}


/*************************************************************/
 int Mirob::setTrajectoryStart(string name, const double x, const double y, const double z){
   trajIter it = Trajectories.find(name);
   if(it == Trajectories.end()){
     return 1;
   }
   Trajectories[name]->setStart(new PositionUpdate(x,y,z,MaxSpeed));
   return 0;
 }

/*************************************************************/
 int Mirob::setTrajectoryAnchor(string name, const double x, const double y, const double z){
   trajIter it = Trajectories.find(name);
   if(it == Trajectories.end()){
     return 1;
   }
   Trajectories[name]->setAnchor(new PositionUpdate(x,y,z,0));
   return 0;
 }

/*************************************************************/
 int Mirob::setTrajectoryCalibrated(string name, bool val){
   trajIter it = Trajectories.find(name);
   if(it == Trajectories.end()){
     return 1;
   }
   Trajectories[name]->setCalibrated(val);
   return 0;
 }

/*************************************************************/
bool Mirob::trajectoryCalibrated(string name){
   trajIter it = Trajectories.find(name);
   if(it == Trajectories.end()){
     return false;
   }
   return Trajectories[name]->isCalibrated();
 }
 

/*************************************************************/

int Mirob::loadConfigurationFile(){
  //  XMLDocument xml;
  XML.LoadFile( ConfigFileName.c_str());
  XMLElement* root = XML.RootElement();

  // extract max speed
  double tmp;
  root->FirstChildElement("maxspeed")->QueryDoubleText(&tmp);
  MaxSpeed = (long)tmp;
  RobotDaemonInfo.MaxSpeed = tmp;
  // extract maximal acceleration
  root->FirstChildElement("maxacceleration")->QueryDoubleText(&MaxAcc);
  RobotDaemonInfo.MaxAcc = MaxAcc;

  RobotDaemonInfo.SetupFile = root->FirstChildElement("setupfile")->GetText();
  
  
  int tmp2;
  root->FirstChildElement("hostid")->QueryIntText(&tmp2);
  RobotDaemonInfo.HostID = (BYTE)tmp2;

  
  root->FirstChildElement("daemonsleeptime")->FirstChildElement("seconds")->QueryIntText(&tmp2);
  RobotDaemonInfo.sleeptime.tv_sec = tmp2;

  root->FirstChildElement("daemonsleeptime")->FirstChildElement("nanoseconds")->QueryIntText(&tmp2);
  RobotDaemonInfo.sleeptime.tv_nsec = tmp2;

  root->FirstChildElement("baudrate")->QueryIntText(&tmp2);
  RobotDaemonInfo.Baudrate = tmp2;

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
  XMLElement* root = XML.RootElement();
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
	node2->InsertEndChild(XML.NewText(ss.str().c_str()));
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
	node2->InsertEndChild(XML.NewText(ss.str().c_str()));
        ss.str( "" );  ss.clear();

    }


    XML.SaveFile(ConfigFileName.c_str());
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


  ConfigFileName = opts.text("config");
  loadConfigurationFile();
  
  loadTrajectoryFile(opts.text("trajectories").c_str());
  getTrajectoryKeys();
  /* Start Robot Daemon */
  RobotDaemonInfo.Device = device.c_str();
  RobotDaemonInfo.active = false;
  RobotDaemonInfo.ForbiddenZones = &ForbiddenZones;
  RobotDaemonInfo.v[0] = 0.0;
  RobotDaemonInfo.v[1] = 0.0;
  RobotDaemonInfo.v[2] = 0.0;
  RobotDaemonInfo.toolClamped = false;
  RobotDaemonInfo.clampChanged = false;
  RobotDaemonInfo.state = ROBOT_HALT;
  RobotDaemonInfo.ChannelType = CHANNEL_RS232;
  RobotDaemon =  new TMLRobotDaemon(&RobotDaemonInfo);

  cerr << LOGPREFIX << " waiting for daemon to start" << endl;
  pthread_mutex_lock( &RobotDaemonInfo.mutex );
  RobotDaemon->Start();
  // wait until Setup has finished (sends the cond signal)
  pthread_cond_wait( &RobotDaemonInfo.cond, &RobotDaemonInfo.mutex );
  pthread_mutex_unlock( &RobotDaemonInfo.mutex );


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
    pthread_mutex_lock( &RobotDaemonInfo.mutex );
    // if (state != ROBOT_POS){
    // }
    RobotDaemonInfo.state = state;
    pthread_mutex_unlock( &RobotDaemonInfo.mutex );
  }

/*************************************************************/


void Mirob::close( void )
{

  Info.clear();
  Settings.clear();

  RobotDaemon->Shutdown();
  Opened = false;
  cerr << LOGPREFIX << "Closed! " << endl;
  cerr << LOGPREFIX << "Saving configuration ..." << endl;
  saveConfigurationFile();
}



/********************* activation and reactivation part *************/

int Mirob::reset( void )
{
  RobotDaemon->Shutdown();
  delete RobotDaemon;
  RobotDaemon =  new TMLRobotDaemon(&RobotDaemonInfo);

  cerr << LOGPREFIX << " waiting for daemon to start" << endl;
  pthread_mutex_lock( &RobotDaemonInfo.mutex );
  RobotDaemon->Start();
  // wait until Setup has finished (sends the cond signal)
  pthread_cond_wait( &RobotDaemonInfo.cond, &RobotDaemonInfo.mutex );
  pthread_mutex_unlock( &RobotDaemonInfo.mutex );

  return 0;
}


/****************** velocity part *******************************/
int Mirob::getV(double &x, double &y, double &z){
  x = RobotDaemonInfo.v[0];
  y = RobotDaemonInfo.v[1];
  z = RobotDaemonInfo.v[2];
  
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
    currV[i] = RobotDaemonInfo.v[i];
  }
  
  if (CoordinateMode == MIROB_COORD_TRANS){
    transformVelocities(currV,RAW2TRANS);
    currV[ax-1] = v; ///sqrt(B[0,ax-1]*B[0,ax-1] + B[1,ax-1]*B[1,ax-1] + B[2,ax-1]*B[2,ax-1])
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

  pthread_mutex_lock( &RobotDaemonInfo.mutex );
  
  for (int i=0; i != 3; ++i){
    RobotDaemonInfo.v[i] = currV[i];
  }
  pthread_mutex_unlock( &RobotDaemonInfo.mutex );

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

/*************************************************************/

int Mirob::setCoordinateFrame(double newB[3][3], double newOffspring[3]){
  // cerr << LOGPREFIX << "Setting new coordinate frame" << endl;
  for (int i = 0; i != 3; ++i){
    for (int j = 0; j != 3; ++j){
      B[i][j] = newB[i][j];
      // cerr << B[i][j] << " ";
    }
    b0[i] = newOffspring[i];
    // cerr << b0[i] << endl;
  }

  return 0;
}

/*************************************************************/

void Mirob::getCoordinateFrame(double (&newB)[3][3], double (&newOffspring)[3]){
  for (int i = 0; i != 3; ++i){
    for (int j = 0; j != 3; ++j){
      newB[i][j] = B[i][j];
    }
    newOffspring[i] = b0[i];
  }
}

/***************** positioning part *****************************/

int Mirob::stop(){
  return RobotDaemon->Stop();
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
  pthread_mutex_lock( &RobotDaemonInfo.mutex );
  RobotDaemonInfo.positionQueue.push(new PositionUpdate(x,y,z,speed));
  pthread_mutex_unlock( &RobotDaemonInfo.mutex );
 
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
  double norm[3];
  for (int i = 0; i != 3; ++i){
    norm[i] = sqrt(B[0][i]*B[0][i] + B[1][i]*B[1][i] + B[2][i]*B[2][i]);
  }

  if (direction == RAW2TRANS){
    for (int i = 0; i != 3; ++i){
      tmp[i] = iB[i][0]*x + iB[i][1]*y + iB[i][2]*z;
    }
  }else if (direction == TRANS2RAW){
    for (int i = 0; i != 3; ++i){
      tmp[i] = B[i][0]*x + B[i][1]*y + B[i][2]*z;
    }
  }
  x = tmp[0]/norm[0]; y = tmp[1]/norm[1]; z = tmp[2]/norm[2];


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
    return (double) RobotDaemonInfo.pos[axis-1];

  }else if (CoordinateMode ==  MIROB_COORD_TRANS){
    double coord[3];
    for (int i= 0; i != 3; ++i){
      coord[i] = (double)RobotDaemonInfo.pos[i];
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
  cerr << LOGPREFIX << "clear(int axis) is not supported!" << endl;
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

  cerr << LOGPREFIX << "setAmplX is not supported!" << endl;
  return 0;
}


int Mirob::setAmplY( double posampl, double negampl )
{
  cerr << LOGPREFIX << "setAmplY is not supported!" << endl;
  return 0;
}


int Mirob::setAmplZ( double posampl, double negampl )
{
  cerr << LOGPREFIX << "setAmplZ is not supported!" << endl;

  return 0;
}


double Mirob::minAmplX( void ) const
{
  cerr << LOGPREFIX << "minAmplX is not supported!" << endl;

  return 0;
}


double Mirob::maxAmplX( void ) const
{
  cerr << LOGPREFIX << "maxAmplX is not supported!" << endl;
  return 0;
}


/**************** tool control part ***************************/
int Mirob::clampTool(void){
  pthread_mutex_lock( &RobotDaemonInfo.mutex );
  RobotDaemonInfo.toolClamped = true;
  pthread_mutex_unlock( &RobotDaemonInfo.mutex );
  return 0;
}

int Mirob::releaseTool(void){
  pthread_mutex_lock( &RobotDaemonInfo.mutex );
  RobotDaemonInfo.toolClamped = false;
  pthread_mutex_unlock( &RobotDaemonInfo.mutex );
  return 0;
}

int Mirob::switchClampState(void){
  pthread_mutex_lock( &RobotDaemonInfo.mutex );
  RobotDaemonInfo.toolClamped = !RobotDaemonInfo.toolClamped;
  pthread_mutex_unlock( &RobotDaemonInfo.mutex );
  return 0;
}

/*******************************************************/
int Mirob::goToTrajectoryStart(string name){
  if (Calibrated){
    PositionUpdate* tmp = Trajectories[name]->startPoint();
    setCoordinateSystem(MIROB_COORD_TRANS);
    setState(ROBOT_POS);
    //cerr << "Trajectory start is " << *tmp << endl;

    setPos(tmp->x,tmp->y,tmp->z,MaxSpeed);
    delete tmp;
    return 0;
  }else{
    return 1;
  }

}


/*******************************************************/
 int Mirob::runTrajectory(string name){
    PositionUpdate* tmp = Trajectories[name]->startPoint();
    return runTrajectory(name, tmp->x ,tmp->y ,tmp->z);
}

/*******************************************************/
 int Mirob::runTrajectory(string name, const double x, const double y, const double z){
  if (Calibrated){
    setCoordinateSystem(MIROB_COORD_TRANS);
    setState(ROBOT_POS);
    for (PositionUpdate* tmp = Trajectories[name]->resetToStart(x,y,z); 
	 tmp != NULL; tmp = Trajectories[name]->next()){
      setPos(tmp->x,tmp->y,tmp->z,tmp->speed);
      delete tmp;
    }
    return 0;
  }else{
    return 1;
  }

}

/*******************************************************/
  int Mirob::positionQueueLength(){
    int ret = -1;
    pthread_mutex_lock( &RobotDaemonInfo.mutex );
    ret = (int)RobotDaemonInfo.positionQueue.size();
    pthread_mutex_unlock( &RobotDaemonInfo.mutex );
    return ret;

  }


// void Mirob::miroblog(const char* text){
  
// }

// void Mirob::miroblog(relacs::Str text){
// }


}; /* namespace misc */
