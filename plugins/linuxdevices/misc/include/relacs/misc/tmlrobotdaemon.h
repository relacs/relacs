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

#ifndef _RELACS_MISC_TMLROBOTDAEMON_H_
#define _RELACS_MISC_TMLROBOTDAEMON_H_ 1

#include <time.h>
#include <string>
#include <vector>
#include <relacs/zones.h>
#include <relacs/manipulator.h>
#include <TML_lib.h>
//#include <relacs/misc/mirob.h>
#include <queue>


#define ROBOT_HALT 0
#define ROBOT_FREE 1
#define ROBOT_POS  2
#define ROBOT_ERR 3

using namespace std;
using namespace relacs;


namespace misc {


class PositionUpdate {
 public:

  double x,y,z,speed;
  
  PositionUpdate(){
    x = y = z = speed = 0.0;
  }
  
  PositionUpdate(double xx, double yy, double zz, double s){
    x = xx;
    y = yy;
    z = zz;
    speed = s;
  }
  
  friend std::ostream& operator << (std::ostream& stream, PositionUpdate &p){
    stream << "\n\t x:" << p.x << "\ty: "<< p.y << "\tz: " << p.z << "\tspeed: " << p.speed << "\n";
    return stream;
  }    

  /* friend PositionUpdate operator+(const PositionUpdate &lhs, const PositionUpdate &rhs){ */
  /*   double s = lhs.speed > rhs.speed ? rhs.speed : lhs.speed; */
  /*   return PositionUpdate(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z,s); */

  /* } */

  /* friend PositionUpdate operator-(const PositionUpdate &lhs, const PositionUpdate &rhs){ */
  /*   double s = lhs.speed > rhs.speed ? rhs.speed : lhs.speed; */
  /*   return PositionUpdate(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z,s); */

  /* } */

  /* friend PositionUpdate operator-(const PositionUpdate& in){ */
  /*   return PositionUpdate(-in.x, -in.y , -in.z,in.speed); */
  /* } */
  


};
/* typedef struct PositionUpdate PositionUpdate; */


struct robotDaemon_data { 

  
  bool active;
  BYTE ChannelType;
  BYTE HostID;
  DWORD  Baudrate;
  

  Zones* forbiddenZones;
  
  struct timespec sleeptime;

  const char* Device;
  const char* SetupFile;

  // thread mutex variables
  pthread_mutex_t mutex;
  pthread_cond_t cond; 

  // current speed variables
  double v[3];

  // clamp tool states
  bool toolClamped;
  bool clampChanged;
  // mode state
  int state;

  // state information
  int pos[3];

  queue<PositionUpdate*> positionQueue;
  long MaxSpeed;
  double MaxAcc;

};
typedef struct robotDaemon_data robotDaemon_data;

/*********************************************/
class TMLRobotDaemon
{
   public:
      TMLRobotDaemon(robotDaemon_data* ptr);
      int Start();
      int Stop();
      int Shutdown();
      int clampTool(void);
      int releaseTool(void);
      
      int getState(void) const {return info->state;};
   protected:
      // thread entry point
      static void * EntryPoint(void*);

      // the following function are function of the thread which has access to the robot
      int Run();
      void Setup();
      void Execute();
      void Exit();

      // the following function are used by the thread and outside
      void log(const char* text);
      void log(relacs::Str text);

   private:
      //bool motionIssued;
      robotDaemon_data* info;
      pthread_t id;
      static const char* LOGPREFIX;

      int activateAxis(int axis);
 
      int getPos(int) const;
      double positionError();
      bool motionComplete();
      void updateInfo();

      int setPos(double x, double y, double z, double speed);

      bool isInsideForbiddenZone(void);
      /* int hack(int k); */
      BYTE limitNeg[3];
      BYTE limitPos[3];
      WORD MER[3], MCR[3], SRL[3], ISR[3], SRH[3];
      /* long readAPOS[3]; */
      /* int trueAPOS[3]; */

      int setV(double v, int ax); // this function should only be called from the thread

      // variables for position monitoring
      int tmp_apos2; 
      long tmp_apos;
      
      // tracking variables
      double old_v[3];
      int old_state;
      bool old_tool_state;
      queue<PositionUpdate*>::size_type old_queue_len;

};


//void* robotDaemon(void* ptr);



/*********************************************/

}; /* namespace misc */

#endif /* ! _RELACS_MISC_TMLROBOTDAEMON_H_ */
