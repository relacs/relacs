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
#include <TML_lib.h>
using namespace std;
using namespace relacs;

namespace misc {


typedef struct Point3D positionUpdate;

/******* thread to watch the robot ****** */
struct watchdog_data { 
  bool active, watchLimits,stopped;
  BYTE ChannelType;
  BYTE HostID;
  DWORD  Baudrate;

  Zones* forbiddenZones;

  struct timespec sleeptime;

  const char* Device;
  const char* SetupFile;

};
typedef struct watchdog_data watchdog_data;

void* watchdog(void* ptr);



/*********************************************/



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
  virtual double posX( void ) const;
    /*! Return the position of the y-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posY( void ) const;
    /*! Return the position of the z-axis.
        Depending on the implementation this can be raw steps
	or a specific distance or angle. */
  virtual double posZ( void ) const;

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

  int step(double dx, double dy, double dz, double v, bool wait);

  int clampTool(void);
  int releaseTool(void);

  int startRecording(void);
  int recordStep(void);
  int stopRecording(void);
  int executeRecordedTrajectory(double speed, bool forward, bool wait);
  int gotoNegLimitsAndSetHome(void);

  int recordPosition(void);
  int clearPositions(void);
  int makePositionsForbiddenZone(void);

private:

  int activateAxis(int ax);
  
  static const BYTE ChannelType = CHANNEL_RS232;
  static const BYTE HostID = 1;
  static const DWORD Baudrate = 115200;
  static const string SetupFile;
  static const long MaxSpeed = 50;
  static const double MaxAcc = 0.3183;

  static const long watchdog_sleep_sec = 0;
  static const long watchdog_sleep_nsec = 50000000;

  int syncTposApos(void );

  int step( double x, int axis );
  double pos( int axis ) const;
  int suspendUntilStop(void);
  int suspendUntilPositionReached(double x, double y, double z, double tol);
  bool Opened;

  double Speed[3];
  double Acceleration[3];
  pthread_t watchdog_thread;
  watchdog_data watchdog_info;
  
  vector <positionUpdate> recordedSteps;
  vector <Point3D> positions;

  
  positionUpdate record0;
  Zones forbiddenZones;

  
};

}; /* namespace misc */

#endif /* ! _RELACS_MISC_MIROB_H_ */
