/*
gcc -DNIDAQ_MAJOR=254 -o ainonblock ainonblock.c

demonstrate non-continous analog input with three non-blocking read calls.
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
}


int main( void )
{
#define MAXPOINTS 10000
  int ai, n, k, j;
  signed short buf[MAXPOINTS];
  FILE *F;

  ai = open( "/dev/niai0", O_RDONLY | O_NONBLOCK );
  if ( ai < 0 )
    {
      perror( "can't open /dev/niai0" );
      return 0;
    }
  else
    fprintf( stderr, "ai=%d\n", ai );

  ioctl( ai, NIDAQAISCANSTART, 20 );
  ioctl( ai, NIDAQAISCANINTERVAL, 4000 );
  ioctl( ai, NIDAQAISAMPLEINTERVAL, 100 );

  ioctl( ai, NIDAQAIRESET, 0 );

  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  AI_add_channel( ai, 0, 1, 0, 2, 0, 1 );

  /* start reading process without getting data back: */
  fprintf( stderr, "start reading of %d points now...\n", MAXPOINTS );
  n = read( ai, buf, MAXPOINTS*2 );
  fprintf( stderr, "read %d\n", n );
  if ( n < 0 && errno != EAGAIN ) {
    perror( "read returned error" );
  }

  fprintf( stderr, "sleep...\n" );
  sleep( 1 );
  /* read out data: */
  fprintf( stderr, "read data... " );
  n = read( ai, buf, MAXPOINTS );
  fprintf( stderr, "read %d\n", n );

  fprintf( stderr, "sleep...\n" );
  sleep( 1 );
  /* read out remaining data: */
  fprintf( stderr, "read data... " );
  n = read( ai, buf, MAXPOINTS );
  fprintf( stderr, "read %d\n", n );

  fprintf( stderr, "sleep...\n" );
  sleep( 1 );
  /* this starts a new reading process: */
  fprintf( stderr, "read data... " );
  n = read( ai, buf, MAXPOINTS );
  fprintf( stderr, "read %d\n", n );

  close( ai );

  F = fopen( "signal.dat", "w" );
  for ( j=k=1; k<n; j++ )
    fprintf( F, "%d  %d\n", j, buf[k++] );
  fclose( F );

  return( 0 );
}
