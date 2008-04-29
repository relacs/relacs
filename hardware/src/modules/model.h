#ifndef _RELACS_HARDWARE_MODEL_H_
#define _RELACS_HARDWARE_MODEL_H_ 1

#include "moduledef.h"


#define PARAM_N  2
#define INPUT_N  1
#define OUTPUT_N 1



void initModel( void );

//struct paramDefT paramDefTest;

#define PARAMS 2
/*! Name, unit, preset, minimum and maximum values for each fixed model 
    parameter. */
/*
struct paramDefT paramDef[PARAMS] = {
//{ "name", "unit", preset, min, max }
  { "Value1", "mV", 0.0, -70.0, 40.0 },
  { "Value2",  "V", 1.0,   0.0,  1.0 }
};*/

#define PARAM_TRACES 2
/*! Tracename, unit, preset, minimum and maximum values for all 
    time-dependent parameters of the model. */
/*
struct paramDefT paramTraceDef[PARAM_TRACES] = {
//{ "tracename", "unit", preset, min, max }
  { "pTrace0", "mV", 0.0, -70.0, 40.0 },
  { "pTrace1",  "V", 1.0,   0.0,  1.0 }
};*/


// TODO: do we really need this?
#define DYN_CLAMP_IN_TRACES 1
#define DYN_CLAMP_OUT_TRACES 1




/************* don't edit anything from here on (just use it...) *************/



/*! Fixed parameter values for dynamic clamp, given by model. */
extern float *paramValues;

/*! Time-dependent parameter traces for dynamic clamp, given by RePro. */
extern float **paramTrace;

/*! Number of time-dependent parameter traces for dynamic clamp. */
// extern int paramTraceN; // see: PARAM_TRACE_N

/*! Traces for direct output written in dynamic clamp loop, given by Repro. */
extern float **directOutTrace;

/*! Number of traces for direct output written in dynamic clamp loop. */
extern int directOutTraceN;

/*! Direct output Traces for stimulus data, given by Repro. */
extern float **stimulusTrace;

/*! Number of direct output Traces for stimulus data. */
extern int stimulusTraceN;

/*! Names of input traces for dynamic clamp. Important for 
    identifing the corresponding input device and channel. */
extern char **dynClampInTraceNames;

/*! Number of input traces for dynamic clamp. */
extern int dynClampInTraceN;

/*! Dynamic clamp input value at current cycle for each recorded channel. */
extern float *dynClampIn;

/*! Names of output traces for dynamic clamp. Important for 
    identifing the corresponding output device and channel. */
extern char **dynClampOutTraceNames;

/*! Number of output traces for dynamic clamp. */
extern int dynClampOutTraceN;

/*! Dynamic clamp output value at current cycle for each channel calculated 
    by model. */
extern float *dynClampOut;






#endif /* ! _RELACS_HARDWARE_MODEL_H_ */
