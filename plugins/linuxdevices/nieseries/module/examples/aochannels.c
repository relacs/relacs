/*
gcc -DNIDAQ_MAJOR=254 -o aochannels aochannels.c -lm
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "nidaq.h"


#define MAXPOINTS 3000
#define MAXBUFFERS 1
#define UPDATERATE 20000
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

  fprintf( stderr, "added channel %d\n", channel );

  ioctl( fd, NIDAQAOADDCHANNEL, u );
}


void init_ramp( signed short buf[], int n )
{
  int k;

  for ( k=0; k<MAXPOINTS; k++ )
    buf[k] = 4096*(k%PERIOD)/PERIOD - 2047;
  buf[MAXPOINTS-2] = 1000;
  buf[MAXPOINTS-1] = 0;
}


void init_sine( signed short buf[], int n )
{
  int k;

  for ( k=0; k<MAXPOINTS; k++ )
    buf[k] = 2047*sin( 2.0*M_PI*(k+1)/PERIOD );
  buf[MAXPOINTS-1] = 0;
}


void init_zero( signed short buf[], int n )
{
  int k;

  for ( k=0; k<MAXPOINTS; k++ )
    buf[k] = 0;
}


void save_buf( signed short buf[], int n )
{
  FILE *F;
  int k;

  F = fopen( "plot.dat", "w" );
  for ( k=0; k<n; k++ )
    fprintf( F, "%d  %d\n", k, buf[k] );
  fclose( F );
}


int main( int argc, char *argv[] )
{
  int ao, pfi;
  signed short *buf;

  buf = malloc( 4*MAXPOINTS);

  ao = open( "/dev/niao0", O_WRONLY );
  pfi = open( "/dev/nipfi0", 0 );

  ioctl( ao, NIDAQAODELAY, 0 );
  ioctl( ao, NIDAQAORATE, UPDATERATE );
  ioctl( ao, NIDAQAOBUFFERS, MAXBUFFERS );
  ioctl( pfi, NIDAQPFIOUT, 6 );

  AO_add_channel( ao, 0, 1, 0, 0, 0 );
  init_ramp( buf, MAXPOINTS );
  write( ao, buf, 2*MAXPOINTS );

  sleep( 1 );
  ioctl( ao, NIDAQAOCLEARCONFIG, 0 );
  init_sine( buf, MAXPOINTS );
  AO_add_channel( ao, 1, 1, 0, 0, 0 );
  write( ao, buf, 2*MAXPOINTS );
   
  close( ao );
  close( pfi );
  free( buf );

  return 0;
}
