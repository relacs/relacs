#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include "nidaq.h"

#define USDELAY 50000

/*
Pins configured for input (default!) are high!
Pins configured for output are low by default.
 */

int main( int argc, char *argv[] )
{
  int daq;
  int pin, mask;
  char buf;

  daq = open( "/dev/nidio0", O_RDWR );

  if ( argc == 2 ) {
    sscanf( argv[1], " %d", &pin );
  }
  else {
    pin = 0;
  }

  /* set mask for output lines to the single pin where we want to write. */
  mask = 1 << pin;
  ioctl( daq, NIDAQDIOMASK, mask );
  printf( "write to dio pin %d with mask %x:\n", pin, mask );

  /* configure this pin for digital output. */
  printf( "configure dio pin %d for output\n", pin );
  ioctl( daq, NIDAQDIOCONFIGURE, mask );
  usleep( USDELAY );

  printf( "write 1\n" );
  ioctl( daq, NIDAQDIOPAROUT, 0xff );
  usleep( USDELAY );

  printf( "write 0\n" );
  ioctl( daq, NIDAQDIOPAROUT, 0x00 );
  usleep( USDELAY );

  printf( "write 1\n" );
  ioctl( daq, NIDAQDIOPAROUT, 0xff );
  usleep( USDELAY );

  printf( "write 0\n" );
  ioctl( daq, NIDAQDIOPAROUT, 0x00 );
  usleep( USDELAY );
  /*
  
  printf( "configure dio pin %d for input\n", pin );
  ioctl( daq, NIDAQDIOCONFIGURE, 0 );
  usleep( USDELAY );

  printf( "read dios:\n" );
  ioctl( daq, NIDAQDIOPARIN, &buf );
  printf( "read %x\n", buf );
*/
  close( daq );

  return 0;
}
