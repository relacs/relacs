
/*! Name, by which this module is known inside Linux: */
char *moduleName;

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
char *outputUnits[OUTPUT_N] = { "mA" };
int outputChannels[OUTPUT_N];
int outputDevices[OUTPUT_N];
float output[OUTPUT_N] = { 0.0 };

/*! Parameter that are provided by the model and can be read out. */
#define PARAMINPUT_N 1
char *paramInputNames[PARAMINPUT_N] = { "pinput0" };
char *paramInputUnits[PARAMINPUT_N] = { "mV" };
float paramInput[PARAMINPUT_N] = { 0.0 };

/*! Parameter that are read by the model and are written to the model. */
#define PARAMOUTPUT_N 1
char *paramOutputNames[PARAMOUTPUT_N] = { "poutput0" };
char *paramOutputUnits[PARAMOUTPUT_N] = { "mA" };
float paramOutput[PARAMOUTPUT_N] = { 0.0 };

void initModel( void )
{
  moduleName = "/dev/dynclamp";
}

void computeModel( void )
{
  output[0] = paramOutput[0];
}
