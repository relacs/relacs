/*!
Dynamic clamp model for a passive ionic current:
\f[ I_{inj} = -g \cdot (V-E) \f]

\par Input/Output
- V: Measured membrane potential in mV
- \f$ I_{inj} \f$ : Injected current in nA

\par Parameter
- g: conductance of passive ionic current in nS
- E: reversal potential of passive ionic current in  mV
*/


#ifdef __KERNEL__

  /*! Name, by which this module is known inside Linux: */
char *moduleName;

  /*! The period length of the realtime periodic task in seconds. */
float loopInterval;
  /*! One over the period length of the realtime periodic task in Hertz. */
float loopRate;

  /*! Analog input that is read from the DAQ board. */
#define INPUT_N 1
  /*! The \a inputNames are used to match the \a input variables with
      analog input traces in Relacs. */
char *inputNames[INPUT_N] = { "V-1" };
char *inputUnits[INPUT_N] = { "mV" };
  /*! The \a inputChannels and \a inputDevices are set automatically. */
int inputChannels[INPUT_N];
int inputDevices[INPUT_N];
  /*! \a input holds the current value that was read in from the DAQ board. */
float input[INPUT_N] = { 0.0 };

  /*! Analog output that is written to the DAQ board. */
#define OUTPUT_N 1
char *outputNames[OUTPUT_N] = { "Current-1" };
char *outputUnits[OUTPUT_N] = { "nA" };
int outputChannels[OUTPUT_N];
int outputDevices[OUTPUT_N];
float output[OUTPUT_N] = { 0.0 };

  /*! Parameter that are provided by the model and can be read out. */
#define PARAMINPUT_N 1
char *paramInputNames[PARAMINPUT_N] = { "Leak-Current" };
char *paramInputUnits[PARAMINPUT_N] = { "nA" };
float paramInput[PARAMINPUT_N] = { 0.0 };

  /*! Parameter that are read by the model and are written to the model. */
#define PARAMOUTPUT_N 2
char *paramOutputNames[PARAMOUTPUT_N] = { "g", "E" };
char *paramOutputUnits[PARAMOUTPUT_N] = { "nS", "mV" };
float paramOutput[PARAMOUTPUT_N] = { 0.0, 0.0 };

  /*! Variables used by the model. */
float meaninput = 0.0;

void initModel( void )
{
   moduleName = "/dev/dynclamp";
   meaninput = 0.0;
}

void computeModel( void )
{
  // leak:
  paramInput[0] = -0.001*paramOutput[0]*(input[0]-paramOutput[1]);
  // total injected current:
  output[0] = paramInput[0];
}

#else

/*! This function is called from DynClampAnalogOutput in user
    space/C++ context and can be used to create some lookuptables for
    nonlinear functions to be used by computeModel(). The implementation of this
    functions has to allocate an \a x and \a y array of floats of a sensible size \a n.
    \param[in] \a k : the index for the lookup table to be generated.
    \param[out] \a n : the size of the lookup table (the number of elements in \a x and \a y).
    \param[out] \a x : the x-values.
    \param[out] \a y : the corresponding y-values.
    \return: 0 if a lookuptable was generated, -1 otherwise.
*/
int generateLookupTable( int k, float **x, float **y, int *n )
{
  return -1;
}

#endif
