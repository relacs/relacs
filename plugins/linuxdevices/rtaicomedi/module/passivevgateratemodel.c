/*!
Dynamic clamp model for a voltage gated and a passive ionic current:
\f[ \begin{array}{rcl} 
  I_{inj} & = & -g \cdot (V-E) - C \cdot \frac{dV}{dt} - gvgate \cdot x \cdot (V-Evgate) \\
  \frac{dx}{dt} & = & \alpha(V) \cdot (1-x) - \beta(V) \cdot x
\end{array} \f]

The voltage gated current is modeled with transition rates:
\f[ \begin{array}{rcl}
  \alpha(V) & = &  \frac{1-\delta}{\bar\tau_x}  \left( \frac{\delta}{1-\delta} \right)^{\delta} e^{\delta k (V-V_1)} \\
  \beta(V) & = & \frac{1-\delta}{\bar\tau_x}  \left( \frac{\delta}{1-\delta} \right)^{\delta} e^{-(1-\delta) k (V-V_1)}
\end{array} \f]

\par Input/Output:
- V: Measured membrane potential in mV
- \f$ I_{inj} \f$ : Injected current in nA

\par Parameter:
- g: conductance of passive ionic current in nS
- E: reversal potential of passive ionic current in mV
- C: Additional capacity of the neuron in pF
- gvgate: conductance of voltage-gated ionic current in nS
- Evgate: reversal potential of voltage-gated ionic current in mV
- vgatevmid: midpoint potential of the steady-state activation function in mV
- vgateslope: slope factor of the steady-state activation function in 1/mV
- vgatetaumax: maximum time constant of the gating variable in ms
- vgatedelta: asymmetry of energy barrier (0 < delta < 1)
*/

#if defined (__KERNEL__) || defined (DYNCLAMPMODEL)

  /*! Name, by which this model is known inside Linux: */
const char *modelName;

  /*! The period length of the realtime periodic task in seconds. */
float loopInterval;
  /*! One over the period length of the realtime periodic task in Hertz. */
float loopRate;

  /*! Analog input that is read from the DAQ board. */
#define INPUT_N 1
  /*! The \a inputNames are used to match the \a input variables with
      analog input traces in Relacs. */
const char *inputNames[INPUT_N] = { "V-1" };
const char *inputUnits[INPUT_N] = { "mV" };
  /*! The \a inputChannels are set automatically. */
int inputChannels[INPUT_N];
  /*! \a input holds the current value that was read in from the DAQ board. */
float input[INPUT_N] = { 0.0 };

  /*! Analog output that is written to the DAQ board. */
#define OUTPUT_N 1
const char *outputNames[OUTPUT_N] = { "Current-1" };
const char *outputUnits[OUTPUT_N] = { "nA" };
int outputChannels[OUTPUT_N];
float output[OUTPUT_N] = { 0.0 };

  /*! Parameter that are provided by the model and can be read out. */
#define PARAMINPUT_N 3
const char *paramInputNames[PARAMINPUT_N] = { "Leak-current", "Capacitive-current", "Voltage-gated current" };
const char *paramInputUnits[PARAMINPUT_N] = { "nA", "nA", "nA" };
float paramInput[PARAMINPUT_N] = { 0.0, 0.0, 0.0 };

  /*! Parameter that are read by the model and are written to the model. */
#define PARAMOUTPUT_N 9
const char *paramOutputNames[PARAMOUTPUT_N] = { "g", "E", "C", "gvgate", "Evgate", "vgatevmid", "vgateslope", "vgatetaumax", "vgatedelta" };
const char *paramOutputUnits[PARAMOUTPUT_N] = { "nS", "mV", "pF", "nS", "mV", "mV", "/mV", "ms", "1" };
float paramOutput[PARAMOUTPUT_N] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 10.0, 0.5 };

  /*! Variables used by the model. */
#define MAXPREVINPUTS 1
float previnputs[MAXPREVINPUTS];
float vgate = 0.0;
float vmid = 0.0;
float vslope = 1.0;
float vtaumax = 10.0;
float vdelta = 0.5;
float vgamma = 0.0;
float valpha = 0.0;
float vbeta = 0.0;

void initModel( void )
{
   int k;

   modelName = "passive vgate rate";
   for ( k=0; k<MAXPREVINPUTS; k++ )
     previnputs[k] = 0.0;
   vgate = 0.0;
}

void computeModel( void )
{
  int k;

  // leak:
  paramInput[0] = -0.001*paramOutput[0]*(input[0]-paramOutput[1]);
   // capacitive current:
   paramInput[1] = -1e-6*paramOutput[2]*(input[0]-previnputs[0])*loopRate;
   for ( k=0; k<MAXPREVINPUTS-1; k++ )
     previnputs[k] = previnputs[k+1];
   previnputs[MAXPREVINPUTS-1] = input[0];
  // voltage gated channel:
  vmid = paramOutput[5];
  vslope = paramOutput[6];
  vtaumax = paramOutput[7];
  vdelta = paramOutput[8];
  if ( vtaumax < 0.1 )
    vtaumax = 0.1;
  vtaumax *= 0.001;
  vgamma = pow(vdelta/(1.0-vdelta), vdelta)*(1.0-vdelta)/vtaumax;
  valpha = vgamma*exp(vdelta*vslope*(input[0]-vmid));
  vbeta = vgamma*exp(-(1.0-vdelta)*vslope*(input[0]-vmid));
  if ( 1.0/(valpha + vbeta) < 5.0*loopInterval )
    vgate = valpha/(valpha + vbeta);
  else
    vgate += loopInterval*(valpha*(1.0-vgate) - vbeta*vgate);

  if ( vgate > 1.0 )
    vgate = 1.0
  else if ( vgate < 0.0 )
    vgate = 0.0

  paramInput[2] = -0.001*paramOutput[3]*vgate*(input[0]-paramOutput[4]);
  // total injected current:
  output[0] = paramInput[0] + paramInput[1] + paramInput[2];
}

#endif

#ifndef __KERNEL__
#ifdef ENABLE_LOOKUPTABLES

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

#endif
