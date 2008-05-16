/*
gcc -DNIDAQ_MAJOR=254 -o aosleep aosleep.c -lm
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


#define MAXPOINTS 600
#define MAXBUFFERS 1
#define UPDATERATE 20000
#define PERIOD 200
#define MULTIPLE_CH 0


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
}


void init_multiplebuf( signed short buf[], int n )
{
  int k, j;

  for ( j=k=0; k<2*MAXPOINTS; k+=2, j++ ) {
    buf[k] = 2047*sin( 2.0*M_PI*j/PERIOD );
    buf[k+1] = 4096*(j%PERIOD)/PERIOD - 2047;
  }
  buf[0] = 0;
  buf[1] = 0;
  buf[2*MAXPOINTS-2] = 0;
  buf[2*MAXPOINTS-1] = 0;
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
  int channel = 0;
  int multiple = 0;

  buf = malloc( 2*MAXPOINTS*sizeof(signed short) );
  if ( buf == NULL ) {
    fprintf( stderr, "out of memory\n" );
    return 0;
  }

  ao = open( "/dev/niao0", O_WRONLY );
  fprintf( stderr, "opened with ao=%d ...\n", ao );
  pfi = open( "/dev/nipfi0", 0 );

  if ( argc == 2 ) {
    sscanf( argv[1], " %d", &channel );
    AO_add_channel( ao, channel, 1, 0, 0, 0 );
  }
  else {
    AO_add_channel( ao, 0, 1, 0, 0, 0 );
    AO_add_channel( ao, 1, 1, 0, 0, 0 );
    multiple = 1;
  }

  ioctl( ao, NIDAQAODELAY, 0 );
  ioctl( ao, NIDAQAORATE, UPDATERATE );
  ioctl( ao, NIDAQAOBUFFERS, MAXBUFFERS );

  ioctl( pfi, NIDAQPFIOUT, 6 );

  sleep( 1 );

  fprintf( stderr, "write %d ...\n", ao );

  int r=0;
  if ( multiple ) {
    init_multiplebuf( buf, MAXPOINTS );
    r=write( ao, buf, 4*MAXPOINTS );
  }
  else {
    if ( channel == 0 )
      init_ramp( buf, MAXPOINTS );
    else
      init_sine( buf, MAXPOINTS );
    r=write( ao, buf, 2*MAXPOINTS );
  }
  if ( r < 0 )
    perror( "write failed" );

  fprintf( stderr, "close %d ...\n", ao );
 
  close( ao );
  close( pfi );
  free( buf );

  return 0;
}
