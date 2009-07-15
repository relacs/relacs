#include <unistd.h>
#include <relacs/digitalio.h>
#include <relacs/comedi/comedidigitalio.h>

using namespace relacs;
using namespace comedi;

int main( int argc, char *argv[] )
{
  DigitalIO *DIO = new ComediDigitalIO( "/dev/comedi0" );
  DIO->open( "/dev/comedi0" );
  DIO->configureLine( 0, true );
  DIO->write( 0, false );
  sleep( 1.0 );
  DIO->write( 0, true );
  sleep( 1.0 );
  DIO->write( 0, false );
  sleep( 1.0 );
  DIO->write( 0, true );
  sleep( 1.0 );
  DIO->write( 0, false );
  sleep( 1.0 );
  DIO->close();
  return 0;
}
