#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include "nidaq.h"

#define MAXPOINTS 10000
#define MAXBUFFERS 1
#define PERIOD 1000
#define AORATE 40000
#define AIRATE 40000
#define INBUFFERSIZE 2*MAXPOINTS*MAXBUFFERS*(AIRATE/AORATE)
#define DELAY 12000

 
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


void init_buf( signed short buf[], int n )
{
  int k;

  for ( k=0; k<MAXPOINTS-1; k++ )
    buf[k] = 2047*sin( 2.0*M_PI*k/PERIOD );
  buf[MAXPOINTS-1] = 0;
}


int main( void )
{
  int ai, ao, pfi, n, m, k, j;
  signed short aobuf[MAXPOINTS];
  signed short aibuf[INBUFFERSIZE];
  long index;
  FILE *F;

  init_buf( aobuf, MAXPOINTS );

  ai = open( "/dev/niai", O_RDONLY | O_NONBLOCK );
  ao = open( "/dev/niao0", O_WRONLY | O_NONBLOCK );
  pfi = open( "/dev/nipfi0", 0 );

  ioctl( pfi, NIDAQPFIOUT, 6 );

  ioctl( ai, NIDAQAICLEARCONFIG, 0 );
  AI_add_channel( ai, 0, 0, 0, 2, 0, 1 );
  ioctl( ai, NIDAQAISCANSTART, 1000 );
  ioctl( ai, NIDAQAISCANRATE, AIRATE );
  ioctl( ai, NIDAQAISAMPLERATE, 3*AIRATE );
  ioctl( ai, NIDAQAICONTINUOUS, 1 );

  ioctl( ao, NIDAQAOBUFFERS, MAXBUFFERS );
  ioctl( ao, NIDAQAODELAY, DELAY );
  ioctl( ao, NIDAQAORATE, AORATE );
  AO_add_channel( ao, 0, 1, 0, 0, 0 );

  m = read( ai, aibuf, INBUFFERSIZE*2 );
  if ( m<0 )
    m=0;

  write( ao, aobuf, MAXPOINTS*2 );

  do {
    usleep( 1000 );
  } while ( ioctl( ao, NIDAQAORUNNING, 0 ) );

  ioctl( ai, NIDAQAIENDONSCAN, 0 );

  ioctl( ao, NIDAQAOBUFFERSTART, &index );
  printf( "signal start at: %ld\n", index );

  n = m;
  do {
    usleep( 1000 );
    m = read( ai, aibuf+n, (INBUFFERSIZE-n)*2 );
    if ( m < 0 && m != -EAGAIN ) {
      perror( "error in read" );
      printf( "error = %d\n", -m );
    }
    else {
      n += m;
      printf( "read %5d -> %5d\n", m, n );
    }
  } while ( m > 0 && n < INBUFFERSIZE && ioctl( ai, NIDAQAIRUNNING, 0 ));
  printf( "read %d from %d\n", n, INBUFFERSIZE );

  close( ai );
  close( ao );
  close( pfi );


  F = fopen( "signal.dat", "w" );
  for ( j=k=0; k<n; j++ )
    {
      fprintf( F, "%d  %d\n", j, aibuf[k++] );
    }
  fclose( F );

  return( 0 );
}
