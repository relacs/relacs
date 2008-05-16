/*
gcc -DNIDAQ_MAJOR=254 -o aotrigger aotrigger.c -lm
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


#define MAXPOINTS 600
#define MAXBUFFERS 40
#define UPDATERATE 100000
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


void init_sine( signed short buf[], int n )
{
  int k;

  for ( k=0; k<MAXPOINTS; k++ )
    buf[k] = 2047*sin( 2.0*M_PI*(k+1)/PERIOD );
}


int main( int argc, char *argv[] )
{
  int ao;
  signed short *buf;

  buf = malloc( 4*MAXPOINTS);
  if ( buf == NULL ) {
    fprintf( stderr, "out of memory\n" );
    exit( 0 );
  }

  ao = open( "/dev/niao0", O_WRONLY );

  AO_add_channel( ao, 0, 1, 0, 0, 0 );

  ioctl( ao, NIDAQAOSTART, 1 );
  ioctl( ao, NIDAQAORATE, UPDATERATE );
  ioctl( ao, NIDAQAOBUFFERS, MAXBUFFERS );

  /*
  ioctl( ao, NIDAQLOWVALUE, 0 );
  ioctl( ao, NIDAQHIGHVALUE, 0 );
  ioctl( ao, NIDAQTRIGGERMODE, 1 );
  ioctl( ao, NIDAQTRIGGERSOURCE, 0 );
  ioctl( ao, NIDAQAOANALOGTRIGGER );
  */

  ioctl( ao, NIDAQAOSTART1SOURCE, 2 );

  init_sine( buf, MAXPOINTS );
  write( ao, buf, 2*MAXPOINTS );

 
  close( ao );
  free( buf );

  return 0;
}
