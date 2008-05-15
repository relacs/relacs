#include "model.h"

/*! Name, by which this module is known inside Linux: */
char *moduleName;

char *paramnames[PARAM_N];
char *inputnames[INPUT_N];
char *outputnames[OUTPUT_N];

void initModel( void )
{
  moduleName = "/dev/dynclamp";
  paramnames[0] = "paramSum"; // RESERVED FOR DIRECT OUT (E.G. PROVIDED WITH ZERO DATA)
  paramnames[1] = "param1";
  inputnames[0] = "input0";
  outputnames[0] = "output0";
}

void calcDummyModel( void )
{
//  PARAM(1) = 

}
