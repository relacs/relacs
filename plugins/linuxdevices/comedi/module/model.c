
/*! Name, by which this module is known inside Linux: */
char *moduleName;

#define INPUT_N         1
#define OUTPUT_N        1
#define PARAMINPUT_N    1
#define PARAMOUTPUT_N   1
char *inputNames[INPUT_N] = { "V-1" };
char *outputNames[OUTPUT_N] = { "Current-1" };
char *inputUnits[INPUT_N] = { "mV" };
char *outputUnits[OUTPUT_N] = { "mA" };

char *paramInputNames[PARAMINPUT_N] = { "pinput0" };
char *paramOutputNames[PARAMOUTPUT_N] = { "poutput0" };
char *paramInputUnits[PARAMINPUT_N] = { "mV" };
char *paramOutputUnits[PARAMOUTPUT_N] = { "mA" };

float input[INPUT_N] = { 0 };
float output[OUTPUT_N] = { 0 };

int inputChannels[INPUT_N];
int inputDevices[INPUT_N];
int outputChannels[OUTPUT_N];
int outputDevices[OUTPUT_N];

float paramInput[PARAMINPUT_N] = { 0 };
float paramOutput[PARAMOUTPUT_N] = { 0 };

void initModel( void )
{
  moduleName = "/dev/dynclamp";
}

void computeModel( void )
{
  //  output[0] = paramOutput[0];
}
