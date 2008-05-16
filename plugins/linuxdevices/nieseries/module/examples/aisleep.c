/*
gcc -DNIDAQ_MAJOR=254 -o aisleep aisleep.c

demonstrate non-continous analog input with a single blocking read call.
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
  int r;

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

  r = ioctl( fd, NIDAQAIADDCHANNEL, u );
  if ( r<0 )
    perror( "error in adding channel" );
}


int main( void )
{
#define MAXPOINTS 1000
  int ai, n, k, j;
  signed short buf[MAXPOINTS];
  FILE *F;

  /*  sleep( 10 );*/
  fprintf( stderr, "start now!\n" );

  ai = open( "/dev/niai0", O_RDONLY );
  if ( ai < 0 )
    {
      perror( "can't open /dev/niai0" );
      return 0;
    }
  else
    fprintf( stderr, "ai=%d\n", ai );

  ioctl( ai, NIDAQAISCANSTART, 1 );
  ioctl( ai, NIDAQAISCANRATE, 90000 );
  ioctl( ai, NIDAQAISAMPLERATE, 500000 );

  ioctl( ai, NIDAQAIRESET, 0 );

  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  AI_add_channel( ai, 0, 1, 0, 3, 0, 1 );

  printf( "start to read %d points now ... \n", MAXPOINTS );
  n = read( ai, buf, MAXPOINTS*2 );
  printf( "read %d\n", n );

  ioctl( ai, NIDAQPFIIN, 0 );

  close( ai );

  F = fopen( "signal.dat", "w" );
  for ( j=k=1; k<n; j++ )
    fprintf( F, "%d  %d\n", j, buf[k++] );
  fclose( F );

  return 0;
}
