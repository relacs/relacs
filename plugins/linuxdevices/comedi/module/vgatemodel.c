/*!
Dynamic clamp model for a voltage gated ionic current:
\f[ \begin{array}{rcl} I_{inj} & = & -gvgate \cdot x \cdot (V-Evgate) \\
vgatetau \cdot \frac{dx}{dt} & = & -x + \frac{1}{1+\exp(-vgateslope \cdot (V-vgatevmid))} \end{array} \f]

\par Input/Output:
- V: Measured membrane potential in mV
- \f$ I_{inj} \f$ : Injected current in nA

\par Parameter:
- gvgate: conductance of voltage-gated ionic current in nS
- Evgate: reversal potential of voltage-gated ionic current in mV
- vgatetau: time constant of the gating variable in ms
- vgatevmid: midpoint potential of the steady-state activation function in mV
- vgateslope: slope factor of the steady-state activation function in 1/mV
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
char *paramInputNames[PARAMINPUT_N] = { "Voltage-gated current" };
char *paramInputUnits[PARAMINPUT_N] = { "nA" };
float paramInput[PARAMINPUT_N] = { 0.0 };

  /*! Parameter that are read by the model and are written to the model. */
#define PARAMOUTPUT_N 5
char *paramOutputNames[PARAMOUTPUT_N] = { "gvgate", "Evgate", "vgatetau", "vgatevmid", "vgateslope" };
char *paramOutputUnits[PARAMOUTPUT_N] = { "nS", "mV", "ms", "mV", "1/mV" };
float paramOutput[PARAMOUTPUT_N] = { 10.0, 0.0, 50.0, 5.0, 1.0 };

  /*! Variables used by the model. */
float meaninput = 0.0;
float vgate = 0.0;
float xmin = 0.0;
float xmax = 0.0;
float dx = 1.0;

void initModel( void )
{
  moduleName = "/dev/dynclamp";
  vgate = 0.0;

  // steady-state activation from lookuptable:
  if ( lookupn[0] > 0 ) {
    xmin = lookupx[0][0];
    xmax = lookupx[0][lookupn[0]-1];
    dx = (xmax - xmin)/lookupn[0];
    xmax -= dx;
  }
  else {
    xmin = 0.0;
    xmax = 0.0;
    dx = 1.0;
  }
}

void computeModel( void )
{
  float x;
  int k;
  // voltage gated channel:
  if ( paramOutput[2] < 0.1 )
    paramOutput[2] = 0.1;
  // steady-state activation from lookuptable:
  x = paramOutput[4]*(input[0]-paramOutput[3]);
  k = 0;
  if ( x >= xmax )
    k = lookupn[0]-1;
  else if ( x >= xmin )
    k = (x-xmin)/dx;
  vgate += loopInterval*1000.0/paramOutput[2]*(-vgate+lookupy[0][k]);
  //  vgate += loopInterval*1000.0/paramOutput[2]*(-vgate+1.0/(1.0+exp(-paramOutput[4]*(input[0]-paramOutput[3]))));
  paramInput[0] = -0.001*paramOutput[0]*vgate*(input[0]-paramOutput[1]);
  // total injected current:
  output[0] = paramInput[0];
}

#else

/*! This function is called from DynClampAnalogOutput in user
    space/C++ context and can be used to create some lookuptables for
    nonlinear functions to be used by computeModel(). The implementation of this
    functions has to allocate an \a x and \a y array of floats of a sensible size \a n.
    \param[in] \a k : the index for the lookup table to be generated.
    \param[in] \a n : the size of the lookup table (the number of elements in \a x and \a y).
    \param[in] \a x : the x-values.
    \param[in] \a y : the corresponding y-values.
    \return: 0 if a lookuptable was generated, -1 otherwise.
*/
int generateLookupTable( int k, float **x, float **y, int *n )
{
  if ( k == 0 ) {
    const int nn = 100000;
    const float xmin = -10.0;
    const float xmax = 10.0;
    const float dx = xmax - xmin;
    *n = nn;
    *x = new float[nn];
    *y = new float[nn];
    for ( int j=0; j<nn; j++ ) {
      float xx = xmin + j*dx/nn;
      (*x)[j] = xx;
      (*y)[j] = 1.0/(1.0+exp(-xx));
    }
    return 0;
  }
  return -1;
}

#endif
