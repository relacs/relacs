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

#define PARAM_CHAN_OFFSET 1000

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

struct traceInfoIOCT {
  char name[PARAM_NAME_MAXLEN];
  char unit[PARAM_NAME_MAXLEN];
};




// *** IOCTL DEFINITIONS ***


// Give information to user space:

// control devices:

#define IOC_GET_SUBDEV_ID       _IOR(RTMODULE_MAJOR,  1, int)
#define IOC_GET_PARAM_ID        _IOR(RTMODULE_MAJOR,  2, int)
#define IOC_OPEN_SUBDEV         _IOW(RTMODULE_MAJOR,  3, int)
#define IOC_CHANLIST            _IOW(RTMODULE_MAJOR,  4, int)
#define IOC_COMEDI_CMD          _IOW(RTMODULE_MAJOR,  5, int)
#define IOC_SYNC_CMD            _IOW(RTMODULE_MAJOR,  6, int)
#define IOC_START_SUBDEV        _IOW(RTMODULE_MAJOR,  7, int)
#define IOC_CHK_RUNNING         _IOWR(RTMODULE_MAJOR, 8, int)
#define IOC_REQ_READ            _IOW(RTMODULE_MAJOR,  9, int)
#define IOC_REQ_WRITE           _IOW(RTMODULE_MAJOR, 10, int)
#define IOC_REQ_CLOSE           _IOW(RTMODULE_MAJOR, 11, int)
#define IOC_STOP_SUBDEV         _IOW(RTMODULE_MAJOR, 12, int)
#define IOC_RELEASE_SUBDEV      _IOW(RTMODULE_MAJOR, 13, int)
                                                       
// exchange info:

#define IOC_GET_INTRACE_INFO    _IOR(RTMODULE_MAJOR, 14, int)
#define IOC_GET_OUTTRACE_INFO   _IOR(RTMODULE_MAJOR, 15, int)
#define IOC_GETLOOPCNT          _IOR(RTMODULE_MAJOR, 16, int)
#define IOC_GETAOINDEX          _IOR(RTMODULE_MAJOR, 17, int)


#define RTMODULE_IOC_MAXNR 18


#endif
