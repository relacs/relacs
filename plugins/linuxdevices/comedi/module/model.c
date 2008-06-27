
/*! Name, by which this module is known inside Linux: */
char *moduleName;

char *inputNames[] = { "input0", NULL };
char *outputNames[] = { "output0", NULL };
char *inputUnits[] = { "mV", NULL };
char *outputUnits[] = { "mA", NULL };

float input[] = { 0, 0 };
float output[] = { 0, 0 };

void initModel( void )
{
  moduleName = "/dev/dynclamp";
}

void calcDummyModel( void )
{
//  PARAM(1) = 
// OUTPUT = -input[0];
}
