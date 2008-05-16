/*
gcc -DNIDAQ_MAJOR=254 -o aimultiao aimultiao.c
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

#define AIMAXPOINTS 40000
#define MAXCHANNELS 3

#define AOMAXPOINTS 600
#define MAXBUFFERS 1
#define UPDATERATE 20000
#define PERIOD 200


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


void init_sine( signed short buf[], int n )
{
  int k;

  for ( k=0; k<AOMAXPOINTS; k++ )
    buf[k] = 2047*sin( 2.0*M_PI*(k+1)/PERIOD );
}


int main( void )
{
  int ai, ao, n, k, j, c;
  signed short buf[AIMAXPOINTS*MAXCHANNELS];
  signed short aobuf[AOMAXPOINTS];
  FILE *F;
  nidaq_info Board;

  ai = open( "/dev/niai0", O_RDONLY | O_NONBLOCK );
  if ( ai < 0 )
    perror( "can't open /dev/niai0" );
  ao = open( "/dev/niao0", O_WRONLY | O_NONBLOCK );
  if ( ao < 0 )
    perror( "can't open /dev/niao0" );
  if ( ai < 0 || ao < 0 )
    return 0;
  else {
    fprintf( stderr, "ai=%d\n", ai );
    fprintf( stderr, "ao=%d\n", ao );
  }

  ioctl( ai, NIDAQINFO, &Board );
  ioctl( ai, NIDAQAISCANSTART, 20 );
  ioctl( ai, NIDAQAISCANINTERVAL, 1000 );
  ioctl( ai, NIDAQAISAMPLEINTERVAL, 100 );
  ioctl( ai, NIDAQAICONTINUOUS, 1 );

  ioctl( ai, NIDAQAIRESETALL, 0 );

  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  for ( c=0; c<MAXCHANNELS-1; c++ )
    AI_add_channel( ai, c, 1, 0, 2, 0, 0 );
  AI_add_channel( ai, c, 1, 0, 2, 0, 1 );

  n = read( ai, buf, AIMAXPOINTS*MAXCHANNELS*2 );
  printf( "read %d\n", n );

  AO_add_channel( ao, 0, 1, 0, 0, 0 );
  ioctl( ao, NIDAQAOSTART, 1000 );
  ioctl( ao, NIDAQAORATE, UPDATERATE );
  ioctl( ao, NIDAQAOBUFFERS, MAXBUFFERS );
  init_sine( aobuf, AOMAXPOINTS );
  fprintf( stderr, "write signal now ...\n" );
  write( ao, aobuf, 2*AOMAXPOINTS );

  sleep( 1 );

  n = read( ai, buf, AIMAXPOINTS*MAXCHANNELS*2 );
  printf( "read %d\n", n );
  if ( n < 0 )
    perror( "error" );

  close( ai );
  close( ao );

  F = fopen( "signal.dat", "w" );
  for ( j=k=1; k<n; j++ )
    {
      fprintf( F, "%d", j );
      for ( c=0; c<MAXCHANNELS; c++ )
	fprintf( F, "  %d", buf[k++] );
      fprintf( F, "\n" );
    }
  fclose( F );

  return( 0 );
}
