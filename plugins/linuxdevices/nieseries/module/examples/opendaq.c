/*
gcc -o opendaq opendaq.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "nidaq.h"


int main( int argc, char *argv[] )
{
  int ai;
  nidaq_info board;

  char device[100];
  if ( argc > 1 )
    strcpy( device, argv[1] );
  else
    strcpy( device, "/dev/niai0" );

  fprintf( stderr, "open %s now...\n", device );
  ai = open( device, 0 );
  if ( ai < 0 )
    {
      perror( "can't open device" );
      exit( 0 );
    }
  else
    fprintf( stderr, "ai=%d\n", ai );

  ioctl( ai, NIDAQINFO, &board );
  fprintf( stderr, "\n" );
  fprintf( stderr, "board %s with\n", board.name );
  fprintf( stderr, "  %d analog input channels at %dbits and %gkHz\n", board.aich, board.aibits, 0.001*board.aimaxspl );
  fprintf( stderr, "    FIFO-size: %d, configuration-buffer size: %d\n", board.aififo, board.aimaxch );
  fprintf( stderr, "  %d analog output channels at %dbits and %gkHz\n", board.aoch, board.aobits, 0.001*board.aomaxspl );
  fprintf( stderr, "    FIFO-size: %d\n", board.aofifo );
  fprintf( stderr, "\n" );

  fprintf( stderr, "close /dev/niai0 now...\n" );
  close( ai );

  fprintf( stderr, "ready\n" );

  return( 0 );
}
