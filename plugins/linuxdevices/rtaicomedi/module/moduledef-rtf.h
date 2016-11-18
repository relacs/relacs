/* DEFINITIONS SHARED BETWEEN USER SPACE AND KERNEL SPACE */

// Whenever something in this file is modified, you need to call
// "make" from linuxdevices/rtaicomedi to recompile the kernel modules
// and the user space classes. And you need to reload the kernel
// module with modul/reloadmodule.sh .

#ifndef _MODULEDEF_H_
#define _MODULEDEF_H_

#include <linux/ioctl.h>


// *** FEATURES *** 
// You may use configure to select features, e.g. ./configure --enable-dynclamp-computation 
// For a list of all --enable-dynclamp-* options see ./configure --help
// Alternatively, you can enable the following defines by uncommenting them.

  /*! Compute a model. */
// #define ENABLE_COMPUTATION

  /*! Include rtai_math.h . */
// #define ENABLE_MATHH

  /*! Generate lookup tables and do not use rtai_math module
      (--enable-dynclamp-lookuptables). */
// #define ENABLE_LOOKUPTABLES

  /*! Generates internal trigger events on which analog output signals
      can be started (--enable-dynclamp-trigger). */
// #define ENABLE_TRIGGER

  /*! Sets digitial outputs high or low at various time points of the
      dynamic clamp loop (--enable-dynclamp-ttlpulse). */
// #define ENABLE_TTLPULSE

  /*! Generates TTL Pulses for synchronizing switch cycle of the npi
      SEC amplifier with dynamic clamp loop
      (--enable-dynclamp-syncsec). */
// #define ENABLE_SYNCSEC

  /*! Intervals of the dynamic clamp loop are always measured and are
      available as "Interval". */

  /*! Measure total duration of analog inputs per cycle and make it
      available as "AI-time" (--enable-dynclamp-aitime). */
// #define ENABLE_AITIME
  /*! Measure duration of analog input acquisition per cycle and make
      it available as "AI-acquisition-time"
      (--enable-dynclamp-aiacquisitiontime). */
// #define ENABLE_AIACQUISITIONTIME
  /*! Measure total duration of analog outputs per cycle and make it
      available as "AO-time" (--enable-dynclamp-aotime). */
// #define ENABLE_AOTIME
  /*! Measure duration of model computation per cycle and make it
      available as "Model-time" (--enable-dynclamp-modeltime). */
// #define ENABLE_MODELTIME
  /*! Measure duration of rt_task_wait_period() per cycle and make it
      available as "Wait-time" (--enable-dynclamp-waittime). */
// #define ENABLE_WAITTIME

  /*! Use RTAI one-shot mode instead of periodic mode (more jitter but
      stable, --enable-dynclamp-oneshot). */
// #define ONESHOT_MODE


// *** KERNEL LOGGING MODE ***

#define RTMODULE_INFO
#define RTMODULE_DEBUG


// *** DEVICE LINUX CONFIGURATION ***

#define RTMODULE_MAJOR 227


// *** DECLARATION OF CONSTANTS ***

//* String length definitions:

// (one byte reserved for null-termination!)
#define PARAM_NAME_MAXLEN 128
#define DEV_NAME_MAXLEN   128
#define DEV_ERROR_MAXLEN  128

//* default waiting time for neuron to react to injected current
//* #define INJECT_RECORD_DELAY 5000 //nsec 
#define INJECT_RECORD_DELAY 1000 //nsec 

//* maximum supported dynamic clamp frequency ensuring a stable system
#define MAX_FREQUENCY 90000 //Hz

//* DAQ-devices:
#define MAXSUBDEV    8
#define MAXCHANLIST  64
#define MAXTTLPULSETYPES 6

#define PARAM_CHAN_OFFSET 1000

// subdevice acquisition errors:
#define E_COMEDI      -1
#define E_NODATA      -2
#define E_UNDERRUN    -3
#define E_OVERFLOW    -4
#define E_NOFIFO      -5
#define E_STOPPEDBYAI -6

//* Lookup tables:
#define MAXLOOKUPTABLES 100

//* Integration algorithms:

#define EULER       0
#define MIDPOINT    1
#define RK4         2
#define ALGO_PRESET EULER


// *** TYPE DEFINITIONS ***

//* DAQ-devices:

enum subdevTypes { SUBDEV_IN=0, SUBDEV_OUT, SUBDEV_DIO };

struct deviceIOCT {
  char devicename[DEV_NAME_MAXLEN+1];
  unsigned int subdev;
  enum subdevTypes subdevType;
  unsigned int fifoIndex;
  char errorstr[DEV_ERROR_MAXLEN+1];
};

#define MAX_CONVERSION_COEFFICIENTS 4
struct converterT {
  unsigned int order;
  double expansion_origin;
  double coefficients[MAX_CONVERSION_COEFFICIENTS];
};

struct chanlistIOCT {
  enum subdevTypes type;
  unsigned int chanlistN;
  unsigned int chanlist[MAXCHANLIST];
  int isused[MAXCHANLIST];
  unsigned int maxdata[MAXCHANLIST];
  float minvoltage[MAXCHANLIST];
  float maxvoltage[MAXCHANLIST];
  float scalelist[MAXCHANLIST];
  struct converterT conversionlist[MAXCHANLIST];
};

struct syncCmdIOCT {
  enum subdevTypes type;
  unsigned int frequency;
  unsigned long delay;
  unsigned long duration;
  int startsource;
  int continuous;
  int buffersize;
};

enum dioOps { DIO_CONFIGURE, DIO_READ, DIO_WRITE,
	      DIO_ADD_TTLPULSE, DIO_CLEAR_TTLPULSE,
	      DIO_SET_SYNCPULSE, DIO_CLEAR_SYNCPULSE };
enum ttlPulses { TTL_START_WRITE=0, TTL_END_WRITE, TTL_START_READ, TTL_END_READ,
		 TTL_START_AO, TTL_END_AO, TTL_UNDEFINED };

#define SYNCSEC_HIGH TTL_END_WRITE
#define SYNCSEC_LOW TTL_END_READ

struct dioIOCT {
  int subdev;
  enum dioOps op;
  unsigned int mask;
  unsigned int bits;
  unsigned int maxlines;
  enum ttlPulses pulseType; /* only for op == DIO_ADD_TTLPULSE or DIO_CLEAR_TTLPULSE */
  long pulsewidth;          /* only for op == DIO_SET_SYNCPULSE */
  int intervalmode;          /* only for op == DIO_SET_SYNCPULSE: 0: fixed period, 1: each difftime, n>1: difftime averaged over n cycles */
  int modemask;          /* only for op == DIO_SET_SYNCPULSE: mask for lines setting modes of amplifer. */
  int modebits;          /* only for op == DIO_SET_SYNCPULSE: bits to set high in amplifier mode. */
};

struct triggerIOCT {
  char devname[DEV_NAME_MAXLEN+1];
  int subdev;   // -1: assing the first analog input subdevice.
  unsigned int channel;
  float alevel;
};


//* Trace-data:
enum traceTypes { TRACE_IN, TRACE_OUT, PARAM_IN, PARAM_OUT, STATUS_IN };

struct traceInfoIOCT {
  enum traceTypes traceType;
  char name[PARAM_NAME_MAXLEN];
  char unit[PARAM_NAME_MAXLEN];
  float value;
};

struct traceChannelIOCT {
  enum traceTypes traceType;
  int channel;
};


// *** IOCTL DEFINITIONS ***

// Give information to user space:

// control devices:

#define IOC_OPEN_SUBDEV         _IOWR(RTMODULE_MAJOR, 1, int)
#define IOC_CHANLIST            _IOW(RTMODULE_MAJOR,  2, int)
#define IOC_SYNC_CMD            _IOW(RTMODULE_MAJOR,  3, int)
#define IOC_START_SUBDEV        _IOW(RTMODULE_MAJOR,  4, int)
#define IOC_CHK_RUNNING         _IOWR(RTMODULE_MAJOR, 5, int)
#define IOC_REQ_CLOSE           _IOW(RTMODULE_MAJOR,  6, int)
#define IOC_STOP_SUBDEV         _IOW(RTMODULE_MAJOR,  7, int)

#define IOC_DIO_CMD             _IOWR(RTMODULE_MAJOR, 8, int)
#define IOC_SET_TRIGGER         _IOW(RTMODULE_MAJOR,  9, int)
#define IOC_UNSET_TRIGGER       _IOW(RTMODULE_MAJOR, 10, int)

// exchange info:

#define IOC_GET_TRACE_INFO      _IOWR(RTMODULE_MAJOR, 11, int)
#define IOC_SET_TRACE_CHANNEL   _IOW(RTMODULE_MAJOR,  12, int)
#define IOC_GETRATE             _IOR(RTMODULE_MAJOR,  13, int)
#define IOC_GETLOOPCNT          _IOR(RTMODULE_MAJOR,  14, int)
#define IOC_GETAOINDEX          _IOR(RTMODULE_MAJOR,  15, int)

// lookuptables:
#define IOC_SET_LOOKUP_K        _IOW(RTMODULE_MAJOR,  16, int)
#define IOC_SET_LOOKUP_N        _IOW(RTMODULE_MAJOR,  17, int)
#define IOC_SET_LOOKUP_X        _IOW(RTMODULE_MAJOR,  18, int)
#define IOC_SET_LOOKUP_Y        _IOW(RTMODULE_MAJOR,  19, int)

#define RTMODULE_IOC_MAXNR 20


// *** KERNEL LOGGING STYLE ***

#ifdef __KERNEL__
#  define ERROR_MSG(msg, args...) rt_printk( KERN_ERR "dynclampmodule: " msg, ## args )
#else
#  define ERROR_MSG(msg, args...) fprintf( stderr, msg, ## args )
#endif

#ifdef __KERNEL__
#  define WARN_MSG(msg, args...) rt_printk( KERN_WARNING "dyclampmodule: " msg, ## args )
#else
#  define WARN_MSG(msg, args...) fprintf( stderr, msg, ## args )
#endif

#ifdef RTMODULE_INFO
#  ifdef __KERNEL__
#    define INFO_MSG(msg, args...) rt_printk( "dynclampmodule: " msg, ## args )
#  else
#    define INFO_MSG(msg, args...) fprintf( stderr, msg, ## args )
#  endif
#else
#  define INFO_MSG(msg, args...)
#endif

#ifdef RTMODULE_DEBUG
#  ifdef __KERNEL__
#    define DEBUG_MSG(msg, args...) rt_printk( "dynclampmodule: " msg, ## args )
#  else
#    define DEBUG_MSG(msg, args...) fprintf( stderr, msg, ## args )
#  endif
#else
#  define DEBUG_MSG(msg, args...)
#endif


#endif
