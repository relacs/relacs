/*
gcc -DNIDAQ_MAJOR=254 -o aiinfinitely aiinfinitely.c

demonstrate continuous analog input with many non-blocking read calls.
Use this to test very long runs of analog input and the ST_TC counter.
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
#define MAXPOINTS 1000000
  int ai, n, err, k;
  unsigned long sc;
  signed short buf[MAXPOINTS];

  ai = open( "/dev/niai0", O_RDONLY | O_NONBLOCK );
  if ( ai < 0 )
    {
      perror( "can't open /dev/niai0" );
      return 0;
    }
  else
    fprintf( stderr, "ai=%d\n", ai );

  ioctl( ai, NIDAQAISCANSTART, 10000 );
  ioctl( ai, NIDAQAISCANRATE, 60000 );
  ioctl( ai, NIDAQAISAMPLERATE, 500000 );
  ioctl( ai, NIDAQAICONTINUOUS, 1 );

  ioctl( ai, NIDAQAIRESET, 0 );

  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  AI_add_channel( ai, 0, 1, 0, 2, 0, 0 );
  AI_add_channel( ai, 1, 1, 0, 2, 0, 0 );
  AI_add_channel( ai, 2, 1, 0, 2, 0, 1 );

  sleep( 5 );

  fprintf( stderr, "start reading now...\n" );

  k = 0;
  do {
    n = read( ai, buf, MAXPOINTS*2 );
    if ( n < 0 && errno != EAGAIN ) {
      fprintf( stderr, "errno=%d\n", errno );
      perror( "read returned error" );
      break;
    }
    usleep( 10000 );
    if ( ioctl( ai, NIDAQAIRUNNING, 0 ) == 0 ) {
      fprintf( stderr, "ai not running\n" );
      ioctl( ai, NIDAQAIERROR, &err );
      if ( err )
	fprintf( stderr, "ai stopped by error %d\n", err );
      break;
    }
  } while ( 1 );

  close( ai );

  return( 0 );
}
