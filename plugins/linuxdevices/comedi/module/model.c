
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
char *inputNames[INPUT_N] = { "EOD" };
char *inputUnits[INPUT_N] = { "mV" };
  /*! The \a inputChannels and \a inputDevices are set automatically. */
int inputChannels[INPUT_N];
int inputDevices[INPUT_N];
  /*! \a input holds the current value that was read in from the DAQ board. */
float input[INPUT_N] = { 0.0 };

  /*! Analog output that is written to the DAQ board. */
#define OUTPUT_N 1
char *outputNames[OUTPUT_N] = { "GlobalEField" };
char *outputUnits[OUTPUT_N] = { "V" };
int outputChannels[OUTPUT_N];
int outputDevices[OUTPUT_N];
float output[OUTPUT_N] = { 0.0 };

  /*! Parameter that are provided by the model and can be read out. */
#define PARAMINPUT_N 1
char *paramInputNames[PARAMINPUT_N] = { "EODSignal" };
char *paramInputUnits[PARAMINPUT_N] = { "V" };
float paramInput[PARAMINPUT_N] = { 0.0 };

  /*! Parameter that are read by the model and are written to the model. */
#define PARAMOUTPUT_N 2
char *paramOutputNames[PARAMOUTPUT_N] = { "Amplitude", "Frequency" };
char *paramOutputUnits[PARAMOUTPUT_N] = { "V", "Hz" };
float paramOutput[PARAMOUTPUT_N] = { 0.0, 0.0 };

  /*! Variables used by the model. */
float phase = 0.0;

void initModel( void )
{
   moduleName = "/dev/dynclamp";
   phase = 0.0;
}

void computeModel( void )
{
  /*  phase += 700.0 * loopInterval;*/
  phase += paramOutput[1] * loopInterval;
#warning THE OTHER MODULE
  if ( phase > 1.0 )
    phase -= 1.0;
  paramInput[0] = paramOutput[0] * cos( 2.0*M_PI*phase );
  /*  paramInput[0] = 0.5 * cos( 2.0*M_PI*phase );*/
  output[0] = paramInput[0];
}
