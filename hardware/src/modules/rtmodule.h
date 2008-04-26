#ifndef _RTMODUL_H_
#define _RTMODULE_H_

#include "moduledef.h"




struct pIndexT {
  int iS;
  int chan;
};


extern struct pIndexT pIndexParam[MAXCHANLIST*MAXSUBDEV]; //or: ...[PARAM_N];
extern struct pIndexT pIndexOut[MAXCHANLIST*MAXSUBDEV];
extern struct pIndexT pIndexIn[MAXCHANLIST*MAXSUBDEV];

#define PARAM(INDEX) OUTDATA( subdev[pIndexParam[INDEX].iS].bData, \
                              pIndexParam[INDEX].chan )

#define OUTPUT(INDEX) OUTDATA( subdev[pIndexOut[INDEX].iS].bData, \
                              pIndexOut[INDEX].chan )

#define INPUT(INDEX) INDATA( subdev[pIndexIn[INDEX].iS].bData, \
                             pIndexIn[INDEX].chan )





// *** DECLARATION OF VARIABLES ***


//ARRAYS:

/*! Fixed parameter values for dynamic clamp, given by model. */
float *paramValues;

/*! Time-dependent parameter traces for dynamic clamp, given by RePro. */
float **paramTrace;

/*! Number of time-dependent parameter traces for dynamic clamp. */
// int paramTraceN = 0; // see: PARAM_TRACE_N

/*! Traces for direct output written in dynamic clamp loop, given by Repro. */
float **directOutTrace;

/*! Number of traces for direct output written in dynamic clamp loop. */
int directOutTraceN = 0;

/*! Direct output Traces for stimulus data, given by Repro. */
float **stimulusTrace;

/*! Number of direct output Traces for stimulus data. */
int stimulusTraceN = 0;

/*! Names of input traces for dynamic clamp. Important for 
    identifing the corresponding input device and channel. */
char **dynClampInTraceNames;

/*! Number of input traces for dynamic clamp. */
int dynClampInTraceN = 0;

/*! Dynamic clamp input value at current cycle for each recorded channel. */
float *dynClampIn;

/*! Names of output traces for dynamic clamp. Important for 
    identifing the corresponding output device and channel. */
char **dynClampOutTraceNames;

/*! Number of output traces for dynamic clamp. */
int dynClampOutTraceN = 0;

/*! Dynamic clamp output value at current cycle for each channel calculated 
    by model. */
float *dynClampOut;









// *** KERNEL LOGGING STYLE ***


#define ERROR_MSG(msg, args...) printk( KERN_ERR "rtmodule: " msg, ## args)
#define WARN_MSG(msg, args...) printk( KERN_WARNING "rtmodule: " msg, ## args)

#ifdef RTMODULE_INFO
#  ifdef __KERNEL__
#    define INFO_MSG(msg, args...) printk( "rtmodule: " msg, ## args)
#  else
#    define INFO_MSG(msg, args...) fprintf(stderr, msg, ## args)
#  endif
#else
#  define INFO_MSG(msg, args...) printk( KERN_INFO "rtmodule: " msg, ## args)

#endif

#ifdef RTMODULE_DEBUG
#  ifdef __KERNEL__
#    define DEBUG_MSG(msg, args...) printk( "rtmodule: " msg, ## args)
#  else
#    define DEBUG_MSG(msg, args...) fprintf(stderr, msg, ## args)
#  endif
#else
#  define DEBUG_MSG(msg, args...) printk( KERN_DEBUG "rtmodule: " msg, ## args)
#endif


#endif
