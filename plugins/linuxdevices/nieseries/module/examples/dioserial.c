#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include "nidaq.h"


int main( int argc, char *argv[] )
{
  int daq;
  int out;
  char buf[10];
  int count, k;

  daq = open( "/dev/nidio0", O_RDWR );

  if ( argc > 1 ) {
    for ( count = 0; 1+count < argc; count++ ) {
      sscanf( argv[1+count], " %x", &out );
      buf[count] = out;
    }
  }
  else {
    buf[0] = 0x42;
    count = 1;
  }

  ioctl( daq, NIDAQDIOCONFIGURE, 0x0E );
  ioctl( daq, NIDAQDIOMASK, 0x0E );
  // CS=0, MUTE=1, ZCEN=1
  ioctl( daq, NIDAQDIOPAROUT, 0x04 );

  printf( " write on DIO0:" );
  for ( k=0; k<count; k++ )
    printf( " %x", buf[k] );
  printf( "\n" );
  write( daq, buf, count );

  read( daq, buf, count );
  printf( "read from DIO4:" );
  for ( k=0; k<count; k++ )
    printf( " %x", buf[k] );
  printf( "\n" );

  close( daq );

  return 0;
}
