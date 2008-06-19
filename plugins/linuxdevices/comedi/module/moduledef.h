/* DEFINITIONS SHARED BETWEEN USER SPACE AND KERNEL SPACE */

#ifndef _MODULEDEF_H_
#define _MODULEDEF_H_

#include <linux/ioctl.h>

#ifdef __KERNEL__
#include <linux/comedilib.h>
#endif



// *** KERNEL LOGGING MODE ***

#define RTMODULE_DEBUG
#define RTMODULE_INFO




// *** DEVICE LINUX CONFIGURATION ***

#define RTMODULE_MAJOR 227




// *** DECLARATION OF CONSTANTS ***


//* String length definitions:

// (one byte reserved for null-termination!)
#define PARAM_NAME_MAXLEN 128
#define PARAM_UNIT_MAXLEN 10
#define DEV_NAME_MAXLEN   128


//* default waiting time for neuron to react to injected current
#define INJECT_RECORD_DELAY 5000 //nsec 


//* maximum supported dynamic clamp frequency ensuring a stable system
#define MAX_FREQUENCY 50000 //Hz


//* Data buffering:

#define FIFO_SIZE   200000 // bytes

//* DAQ-devices:

#define MAXDEV      4
#define MAXSUBDEV   8
#define MAXCHANLIST 128


//* Integration algorithms:

#define EULER       0
#define MIDPOINT    1
#define RK4         2
#define ALGO_PRESET EULER




// *** TYPE DEFINITIONS ***

//* DAQ-devices:

struct deviceIOCT {
  unsigned int subdevID;
  char devicename[DEV_NAME_MAXLEN+1];
  unsigned int subdev;
  int isOutput;
};

struct chanlistIOCT {
  unsigned int subdevID;
  float scalelist[MAXCHANLIST];
  unsigned int chanlist[MAXCHANLIST];
  unsigned int chanlistN;
  // ChanItems after this Index are reserved for Model output:
  unsigned int lastDataChanIndex; 
};

struct comediCmdIOCT {
  unsigned int subdevID;
  comedi_cmd cmd;
};

struct syncCmdIOCT {
  unsigned int subdevID;
  unsigned int frequency;
  unsigned int duration;
  int continuous;
};

// needed?
struct traceNameIOCT {
  char name[DEV_NAME_MAXLEN];
  int chanNr;
};


//* Trace-data:

struct paramDefT {
  char name[PARAM_NAME_MAXLEN];
  char unit[PARAM_UNIT_MAXLEN];
  float preset;
  float min;
  float max;
};




// *** IOCTL DEFINITIONS ***


// Give information to user space:
/*
#define IOC_RT_START       _IOR(RTMODULE_MAJOR,  5, int)
#define IOC_RT_STOP        _IO( RTMODULE_MAJOR,  6)

#define IOC_PARAM_N        _IOR(RTMODULE_MAJOR, 10, int)
#define IOC_PARAM_NAMES    _IOR(RTMODULE_MAJOR, 12, int)
#define IOC_PARAM_UNITS    _IOR(RTMODULE_MAJOR, 13, int)
#define IOC_PARAM_VALUES   _IOR(RTMODULE_MAJOR, 14, int)
#define IOC_PARAM_MINS     _IOR(RTMODULE_MAJOR, 15, int)
#define IOC_PARAM_MAXS     _IOR(RTMODULE_MAJOR, 16, int)

#define IOC_PARAM_TRACE_N           _IOR(RTMODULE_MAJOR, 21, int)
#define IOC_PARAM_TRACE_NAMES       _IOR(RTMODULE_MAJOR, 22, int)
#define IOC_DYNCLAMP_AI_TRACE_N     _IOR(RTMODULE_MAJOR, 23, int)
#define IOC_DYNCLAMP_AI_TRACE_NAMES _IOR(RTMODULE_MAJOR, 24, int)
#define IOC_DYNCLAMP_AO_TRACE_N     _IOR(RTMODULE_MAJOR, 25, int)
#define IOC_DYNCLAMP_AO_TRACE_NAMES _IOR(RTMODULE_MAJOR, 26, int)
*/

// control devices:

#define IOC_GET_SUBDEV_ID   _IOR(RTMODULE_MAJOR, 29, int)
#define IOC_GET_PARAM_ID    _IOR(RTMODULE_MAJOR, 30, int)
#define IOC_OPEN_SUBDEV     _IOW(RTMODULE_MAJOR, 31, int)
#define IOC_CHANLIST        _IOW(RTMODULE_MAJOR, 32, int)
#define IOC_COMEDI_CMD      _IOW(RTMODULE_MAJOR, 33, int)
#define IOC_SYNC_CMD        _IOW(RTMODULE_MAJOR, 34, int)
#define IOC_START_SUBDEV    _IOW(RTMODULE_MAJOR, 35, int)
//#define IOC_START_SUBDEVLIST _IOW(RTMODULE_MAJOR, 36, int)
#define IOC_CHK_RUNNING     _IOWR(RTMODULE_MAJOR,37, int)
#define IOC_REQ_READ        _IOW(RTMODULE_MAJOR, 38, int)
#define IOC_REQ_WRITE       _IOW(RTMODULE_MAJOR, 39, int)
#define IOC_REQ_CLOSE       _IOW(RTMODULE_MAJOR, 40, int)
#define IOC_STOP_SUBDEV     _IOW(RTMODULE_MAJOR, 41, int)
#define IOC_RELEASE_SUBDEV  _IOW(RTMODULE_MAJOR, 42, int)
#define IOC_TRACENAMELIST_N _IOW(RTMODULE_MAJOR, 43, int)
#define IOC_TRACENAMELIST   _IOW(RTMODULE_MAJOR, 44, int)
#define IOC_GETLOOPCNT      _IOR(RTMODULE_MAJOR, 45, int)
#define IOC_GETAOINDEX      _IOR(RTMODULE_MAJOR, 46, int)


/*
#define IOC_DEV_STIMUL_N       _IOW(RTMODULE_MAJOR, 50, int)
#define IOC_DEV_STIMUL_TRACES  _IOW(RTMODULE_MAJOR, 51, int)
#define IOC_DEV_DIROUT_N       _IOW(RTMODULE_MAJOR, 52, int)
#define IOC_DEV_DIROUT_TRACES  _IOW(RTMODULE_MAJOR, 53, int)
#define IOC_DEV_DYNCIN_TRACES  _IOW(RTMODULE_MAJOR, 54, int)
#define IOC_DEV_DYNCOUT_TRACES _IOW(RTMODULE_MAJOR, 55, int)


// Change fixed model parameters:

#define IOC_CH_PARAM_VALUES   _IOW(RTMODULE_MAJOR, 60, int)
*/



#define RTMODULE_IOC_MAXNR 60






#endif
