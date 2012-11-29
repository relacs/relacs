/*
  misc/mirob.h
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

#ifndef _RELACS_MISC_MIROB_H_
#define _RELACS_MISC_MIROB_H_ 1

#include <time.h>
#include <string>
#include <vector>
#include <relacs/zones.h>
#include <relacs/manipulator.h>
#include <relacs/misc/tmlrobotdaemon.h>
#include <relacs/misc/tinyxml2.h>
#include <TML_lib.h>
#include <queue>
#include<map>


#define MIROB_COORD_RAW 0
#define MIROB_COORD_TRANS 1

#define RAW2TRANS 0
#define TRANS2RAW 1

using namespace std;
using namespace relacs;
using namespace tinyxml2;
//class XMLDocument;

namespace misc {

class Trajectory
{

 public:
  Trajectory();
  ~Trajectory();
  Trajectory(const XMLElement * node);

  void setAnchor(PositionUpdate* a);
  void setStart(PositionUpdate* s);
  PositionUpdate* resetToAnchor(double x, double y, double z);
  PositionUpdate* resetToAnchor();
  PositionUpdate* resetToStart(double x, double y, double z);
  PositionUpdate* resetToStart();
  PositionUpdate* next();
  PositionUpdate* startPoint() const {return Start;};
  int  setCalibrated(bool val){Calibrated = val; return 0;};
  bool isCalibrated() const {return Calibrated;}
 private: 
  bool Calibrated;
  int AnchorIndex;
  vector<PositionUpdate*>::size_type CurrentIndex;
  PositionUpdate *Start, *Anchor;
  double Delta[3];
  vector<PositionUpdate*> Nodes;
};

int inv3(double A[3][3], double (&result)[3][3]);


/*!
\class Mirob
\author Jan Benda/ Fabian Sinz
\version 1.0
\brief [Manipulator] The %Mirob module linear robot from MPH
*/

class Mirob : public Manipulator
{

public:

  Mirob( const string &device );
  Mirob( void );
  ~Mirob( void );

  virtual int open( const string &device, const Options &opts );
  virtual bool isOpen( void ) const { return Opened; };
  virtual void close( void );
  virtual int reset( void );

    /*! Move x-axis by \a x.
        Depending on the implementation \a x can be raw steps
	or a specific distance or angle. */
  virtual int stepX( double x );
    /*! Move y-axis by \a y.
        Depending on the implementation \a y can be raw steps
	or a specific distance or angle. */
  virtual int stepY( double y );
    /*! Move z-axis by \a z.
        Depending on the implementation \a z can be raw steps
	or a specific distance or angle. */
  virtual int stepZ( double z );

    /*! Return the position of the x-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posX( void );
    /*! Return the position of the y-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posY( void );
    /*! Return the position of the z-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posZ( void );

    /*! Defines the current position of the axis as its home position. */
  virtual int clear( int axis );
    /*! Defines the current position of the x axis as the home position. */
  virtual int clearX( void );
    /*! Defines the current position of the y axis as the home position. */
  virtual int clearY( void );
    /*! Defines the current position of the z axis as the home position. */
  virtual int clearZ( void );
    /*! Defines the current position of all axis as the home position. */
  virtual int clear( void );

    /*! Move x axis back to its home position. */
  virtual int homeX( void );
    /*! Move y axis back to its home position. */
  virtual int homeY( void );
    /*! Move z axis back to its home position. */
  virtual int homeZ( void );
    /*! Move back to the home position. */
  virtual int home( void );

    /*! Set the amplitude of a step of the x-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplX( double posampl, double negampl=-1.0 );
    /*! Set the amplitude of a step of the y-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplY( double posampl, double negampl=-1.0 );
    /*! Set the amplitude of a step of the z-axis to \a posampl.
        If \a negampl >= 0.0 set the negative amplitude to \a negampl,
        otherwise set it equal to \a posampl. */
  virtual int setAmplZ( double posampl, double negampl=-1.0 );

    /*! The minimum possible amplitude for the x-axis. */
  virtual double minAmplX( void ) const;
    /*! The maximum possible amplitude for the x-axis. */
  virtual double maxAmplX( void ) const;

  int setV(double vx, double vy, double vz);
  int getV(double &vx, double &vy, double &vz);
  int setV(double v, int ax);
  int setVX(double v);
  int setVY(double v);
  int setVZ(double v);

  long getMaxSpeed() const {return MaxSpeed; } ;

  void setCoordinateSystem(int mode){CoordinateMode = mode;}
  int getCoordinateSystem(void) const {return CoordinateMode;}
  bool isCalibrated(void) const {return Calibrated;}
  void setCalibrated(bool c){Calibrated = c;};

  int getState() const {return RobotDaemon->getState();};
  void setState(int state);

  int setPos(double x, double y, double z, double speed);
  int setPos(double x, double y, double z) {
    return setPos(x,y,z,MaxSpeed);
  }

  int clampTool(void);
  int releaseTool(void);
  int switchClampState(void);  

  double pos( int axis );

  int setCoordinateFrame(double newB[3][3], double newOffspring[3]);
  void getCoordinateFrame(double (&newB)[3][3], double (&newOffspring)[3]);

  int stop(void);

  int loadTrajectoryFile(const char * filename);
  int runTrajectory(string name);
  int runTrajectory(string name, const double x, const double y, const double z);
  int goToTrajectoryStart(string name);
  vector<string> getTrajectoryKeys(void);
  int setTrajectoryCalibrated(string name, bool val);
  bool trajectoryCalibrated(string name);
  int setTrajectoryStart(string name, const double x, const double y, const double z);
  int setTrajectoryAnchor(string name, const double x, const double y, const double z);
  static const char* LOGPREFIX;
  int positionQueueLength(void);
  
private:

  /* static const BYTE ChannelType = CHANNEL_RS232; */
  /* static const BYTE HostID = 1; */
  /* static const DWORD Baudrate = 115200; */
  /* /\* static const string SetupFile; *\/ */
  /* /\* static const long MaxSpeed = 100; *\/ */
  /* /\* static const double MaxAcc = 0.3183; *\/ */
  /* const char* SetupFile; */


  long MaxSpeed;
  double MaxAcc; //  = 0.3183
  string ConfigFileName;

  int CoordinateMode;

  
  int loadConfigurationFile();
  int saveConfigurationFile();
  
  void transformCoordinates(double& x, double& y, double& z, int direction);
  double* transformCoordinates(double * x, int direction);

  void transformVelocities(double& x, double& y, double& z, int direction);
  double* transformVelocities(double * x, int direction);


  int step( double x, int axis );
  bool Opened;
  bool Calibrated;

  double Speed[3];
  double Acceleration[3];
  TMLRobotDaemon* RobotDaemon;
  robotDaemon_data RobotDaemonInfo;
  
  // coordinate system remapping
  double B[3][3]; // basis vectors in the raw coordinate system
  double iB[3][3]; // inverse of B
  double b0[3]; // coordinate system offspring in raw coordinate system
  
  
  Zones ForbiddenZones;
  XMLDocument XML;
  map<string, Trajectory* > Trajectories;
};

}; /* namespace misc */

#endif /* ! _RELACS_MISC_MIROB_H_ */
