/*
gcc -DNIDAQ_MAJOR=254 -o aiblock aiblock.c

demonstrate continuous analog input with several blocking read calls.
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

  ai = open( "/dev/niai0", O_RDONLY );
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
  ioctl( ai, NIDAQAICONTINUOUS, 1 );

  ioctl( ai, NIDAQAIRESET, 0 );

  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  AI_add_channel( ai, 0, 1, 0, 2, 0, 1 );

  fprintf( stderr, "start reading now...\n" );
  n = read( ai, buf, MAXPOINTS*2 );
  fprintf( stderr, "read %d\n", n );

  fprintf( stderr, "read data... \n" );
  n = read( ai, buf, MAXPOINTS*2 );
  fprintf( stderr, "read %d\n", n );

  /* stop ai: */
  fprintf( stderr, "stop ai ...\n" );
  ioctl( ai, NIDAQAIENDONSCAN, 0 );

  /* data pending? should be 1 scan! */
  /*
  sleep( 1 );
  ioctl( ai, NIDAQAIDATA, &n );

  if ( n > 0 )
    {  
      fprintf( stderr, "read %d data... \n", n );
      n = read( ai, buf, MAXPOINTS*2 );
      fprintf( stderr, "read %d\n", n );
    }
  else
    fprintf( stderr, "no data pending in ai buffer\n" );


  if ( n >= MAXPOINTS ) {
    fprintf( stderr, "read more data... " );
    n = read( ai, buf, MAXPOINTS*2 );
    fprintf( stderr, "read %d\n", n );
  }
  */

  close( ai );

  F = fopen( "signal.dat", "w" );
  for ( j=k=1; k<n; j++ )
    fprintf( F, "%d  %d\n", j, buf[k++] );
  fclose( F );

  return( 0 );
}
