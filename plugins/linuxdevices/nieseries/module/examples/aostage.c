/*
gcc -DNIDAQ_MAJOR=254 -o aostage aostage.c -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>
#include "nidaq.h"

#define MAXPOINTS 200
#define UPDATERATE 40000
#define PERIOD 200


void AO_add_channel( int fd, int channel, int bipolar,
		     int reglitch, int extref, int groundref )
{
  unsigned int u = 0;

  if ( bipolar )
    u |= 0x0001;

  if ( reglitch )
    u |= 0x0002;

  if ( extref )
    u |= 0x0004;

  if ( groundref )
    u |= 0x0008;

  channel &= 0x1;
  u |= channel << 8;

  fprintf( stderr, "added channel %d: %4x\n", channel, u );

  ioctl( fd, NIDAQAOADDCHANNEL, u );
}


void init_ramp( signed short buf[], int n )
{
  int k;

  for ( k=0; k<MAXPOINTS; k++ )
    buf[k] = 4096*(k%PERIOD)/PERIOD - 2047;
  buf[MAXPOINTS-1] = 0;
}


void init_sine( signed short buf[], int n )
{
  int k;

  for ( k=0; k<MAXPOINTS; k++ )
    buf[k] = 2047*sin( 2.0*M_PI*(k+1)/PERIOD );
}


int main( int argc, char *argv[] )
{
  int ao;
  signed short *buf1, *buf2;

  buf1 = malloc( 4*MAXPOINTS);
  if ( buf1 == NULL ) {
    fprintf( stderr, "out of memory\n" );
    return 1;
  }
  buf2 = malloc( 4*MAXPOINTS);
  if ( buf2 == NULL ) {
    fprintf( stderr, "out of memory\n" );
    return 2;
  }
  init_sine( buf1, MAXPOINTS );
  init_ramp( buf2, MAXPOINTS );

  ao = open( "/dev/niao0", O_WRONLY | O_NONBLOCK );

  fprintf( stderr, "init analog output ...\n" );
  AO_add_channel( ao, 0, 1, 0, 0, 0 );
  ioctl( ao, NIDAQAOSTART, 1 );
  ioctl( ao, NIDAQAORATE, UPDATERATE );
  ioctl( ao, NIDAQAOSTAGING, 1 );

  fprintf( stderr, "write signal 1 now ...\n" );
  ioctl( ao, NIDAQAOBUFFERS, 10 );
  write( ao, buf1, 2*MAXPOINTS );

  fprintf( stderr, "write signal 2 now ...\n" );
  ioctl( ao, NIDAQAOBUFFERS, 40 );
  write( ao, buf2, 2*MAXPOINTS );

  fprintf( stderr, "write signal 3 now ...\n" );
  ioctl( ao, NIDAQAORATE, UPDATERATE/2 );
  ioctl( ao, NIDAQAOBUFFERS, 20 );
  write( ao, buf1, 2*MAXPOINTS );

  /*
  fprintf( stderr, "write signal 4 now ...\n" );
  ioctl( ao, NIDAQAORATE, 2*UPDATERATE/3 );
  ioctl( ao, NIDAQAOBUFFERS, 20 );
  write( ao, buf1, 2*MAXPOINTS );
  */
  fprintf( stderr, "go to sleep ...\n" );
  sleep( 4 );

  fprintf( stderr, "ready.\n" );

  close( ao );
  free( buf1 );
  free( buf2 );

  return 0;
}
