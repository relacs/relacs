/*
gcc -o aisingle aisingle.c
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "nidaq.h"


void AI_add_channel( int fd, int channel, int gain, int unipolar, int type, 
		     int dither, int last )
     /* gain: 0=0.5, 1=1, 2=2, 3=5, 4=10, 5=20, 6=50, 7=100 */
     /* type: 0=Calibration, 1=Differential, 2=NRSE, 3=RSE, 5=Aux, 7=Ghost */
{
  unsigned int u = 0;
  nidaq_info board;

  ioctl( fd, NIDAQINFO, &board );
  if ( channel >= board.aich ) {
    fprintf( stderr, "%d is not a valid channel number.\n", channel );
  }

  gain &= 7;
  u |= gain;

  if ( unipolar )
    u |= 0x0100;

  if ( dither )
    u |= 0x0200;

  if ( last )
    u |= 0x8000;

  channel &= 0xf;
  u |= channel << 16;

  type &= 7;
  u |= type << 28;

  ioctl( fd, NIDAQAIADDCHANNEL, u );
  fprintf( stderr, "added channel %d\n", channel );
}


int main( int argc, char *argv[] )
{
  int ai, n, c, maxch;
  char ch;
  signed short buf[100];

  ai = open( "/dev/niai0", O_RDONLY );

  ioctl( ai, NIDAQAISCANSTART, 20 );
  ioctl( ai, NIDAQAISCANINTERVAL, 200 );
  ioctl( ai, NIDAQAIRESET, 0 );
  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  maxch = argc-1;
  if ( maxch > 0 ) {
    for ( c=0; c<maxch-1; c++ ) {
      sscanf( argv[1+c], " %d", &n );
      AI_add_channel( ai, n, 1, 0, 2, 0, 0 );
    }
    sscanf( argv[argc-1], " %d", &n );
    AI_add_channel( ai, n, 1, 0, 2, 0, 1 );
  }
  else {
    maxch = 1;
    AI_add_channel( ai, 0, 1, 0, 2, 0, 1 );
  }

  for ( ch=0; ch != 'q'; )
    {
      n = read( ai, buf, maxch*2 );
      printf( "read %d from %d: ", n, maxch );
      for ( c=0; c<maxch && c<n; c++ )
	fprintf( stdout, " %6d ", buf[c] );
      ch=getchar();
    }

  close( ai );

  return 0;
}
