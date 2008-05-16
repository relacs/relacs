/*
gcc -DNIDAQ_MAJOR=254 -o aimulticont aimulticont.c
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
#define MAXPOINTS 1000
#define MAXCHANNELS 3
  int ai, m, n, k, j, c, err;
  signed short buf[MAXPOINTS*MAXCHANNELS];
  FILE *F;

  ai = open( "/dev/niai0", O_RDONLY | O_NONBLOCK );
  if ( ai < 0 )
    {
      perror( "can't open /dev/niai0" );
      return 0;
    }
  else
    fprintf( stderr, "ai=%d\n", ai );

  ioctl( ai, NIDAQAIRESETALL, 0 );
  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  for ( c=0; c<MAXCHANNELS-1; c++ )
    AI_add_channel( ai, c, 2, 0, 2, 0, 0 );
  AI_add_channel( ai, c, 2, 0, 2, 0, 1 );

  ioctl( ai, NIDAQAICONTINUOUS, 1 );
  ioctl( ai, NIDAQAISTART1SOURCE, 0 );
  /*  ioctl( ai, NIDAQAISCANSTART, 20 );*/
  ioctl( ai, NIDAQAISCANRATE, 20000 );
  ioctl( ai, NIDAQAISAMPLERATE, 80000 );

  n = 0;
  do {
    m = read( ai, buf+n, (MAXPOINTS*MAXCHANNELS-n)*2 );
    printf( "read %d  %d\n", m, n );
    if ( m <= 1 && n > 0 )
      break;
    if ( m < 0 && errno != EAGAIN ) {
      fprintf( stderr, "errno=%d\n", errno );
      perror( "read returned error" );
      break;
    }
    if ( m > 0 )
      n += m;
    usleep( 10000 );
    if ( ioctl( ai, NIDAQAIRUNNING, 0 ) == 0 ) {
      fprintf( stderr, "ai not running\n" );
      ioctl( ai, NIDAQAIERROR, &err );
      if ( err )
        fprintf( stderr, "ai stopped by error %d\n", err );
      break;
    }
    ioctl( ai, NIDAQAIERROR, &err );
  } while ( 1 );

  close( ai );

  F = fopen( "signal.dat", "w" );
  for ( j=k=0; k<n; j++ )
    {
      fprintf( F, "%d", j );
      for ( c=0; c<MAXCHANNELS; c++ )
	fprintf( F, "  %d", buf[k++] );
      fprintf( F, "\n" );
    }
  fclose( F );

  return( 0 );
}
