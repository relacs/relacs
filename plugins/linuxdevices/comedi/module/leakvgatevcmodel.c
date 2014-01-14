
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
#define PARAMINPUT_N 3
char *paramInputNames[PARAMINPUT_N] = { "Leak-current", "VC-current", "Voltage-gated current" };
char *paramInputUnits[PARAMINPUT_N] = { "nA", "nA", "nA" };
float paramInput[PARAMINPUT_N] = { 0.0, 0.0, 0.0 };

  /*! Parameter that are read by the model and are written to the model. */
#define PARAMOUTPUT_N 9
char *paramOutputNames[PARAMOUTPUT_N] = { "g", "E", "VCgain", "VC", "gvgate", "Evgate", "vgatetau", "vgatevmid", "vgateslope" };
char *paramOutputUnits[PARAMOUTPUT_N] = { "nS", "mV", "mS", "mV", "nS", "mV", "ms", "mV", "1/mV" };
float paramOutput[PARAMOUTPUT_N] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0, 0.0, 1.0 };

  /*! Variables used by the model. */
float meaninput = 0.0;
float vgate = 0.0;

void initModel( void )
{
   moduleName = "/dev/dynclamp";
   meaninput = 0.0;
   vgate = 0.0;
}

void computeModel( void )
{
  // running average:
  meaninput += (input[0] - meaninput)/5.0;  // steps

  // leak:
  paramInput[0] = -0.001*paramOutput[0]*(input[0]-paramOutput[1]);
  // voltage clamp:
  paramInput[1] = -paramOutput[2]*(meaninput-paramOutput[3]);
  // voltage gated channel:
  if ( paramOutput[6] < 0.1 )
    paramOutput[6] = 0.1;
  vgate += loopInterval*1000.0/paramOutput[6]*(-vgate+1.0/(1.0+exp(-paramOutput[8]*(input[0]-paramOutput[7]))));
  paramInput[2] = -0.001*paramOutput[4]*vgate*(input[0]-paramOutput[5]);
  // total injected current:
  output[0] = paramInput[0] + paramInput[1] + paramInput[2];
}
