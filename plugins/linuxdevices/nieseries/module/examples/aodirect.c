/*
gcc -DNIDAQ_MAJOR=254 -o aodirect aodirect.c
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "nidaq.h"

#define CHANNEL 0


void AO_add_channel( int fd, int channel, int bipolar,
		     int reglitch, int extref, int groundref )
{
  unsigned int u = 0;
  nidaq_info board;

  ioctl( fd, NIDAQINFO, &board );
  if ( channel >= board.aoch ) {
    fprintf( stderr, "%d is not a valid channel number.\n", channel );
  }

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

  ioctl( fd, NIDAQAOADDCHANNEL, u );
}


int main( int argc, char *argv[] )
{
  int ao;
  signed short buf[2];
  int u;

  ao = open( "/dev/niao0", O_WRONLY );
  if  ( ao == -1 ) {
    perror( "can't open niao0" );
    return 0;
  }

  int r=0;
  if ( argc < 2 )   /* write 0 to all channels */
    {
      AO_add_channel( ao, 0, 1, 0, 0, 0 );
      AO_add_channel( ao, 1, 1, 0, 0, 0 );
      buf[0] = 0;
      buf[1] = 0;
      r=write( ao, buf, 4 );
    }
  else if ( argc == 2 )                   /* write to a single channel */
    {
      AO_add_channel( ao, CHANNEL, 1, 0, 0, 0 );
      sscanf( argv[1], "%d", &u );
      buf[0] = u;
      r=write( ao, buf, 2 );
    }
  else             /* write two both channels */
    {
      AO_add_channel( ao, 0, 1, 0, 0, 0 );
      AO_add_channel( ao, 1, 1, 0, 0, 0 );
      sscanf( argv[1], "%d", &u );
      buf[0] = u;
      sscanf( argv[2], "%d", &u );
      buf[1] = u;
      r=write( ao, buf, 4 );
    }

  if ( r<0 )
    perror( "write failed" );

  close( ao );

  return 0;
}
