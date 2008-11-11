#include <stdio.h>
#include <comedilib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include "examples.h"

int main(int argc, char *argv[])
{
  comedi_cmd cmd;
  int err;
  int n,m;
  comedi_t *dev;
  unsigned int chanlist[16];
  unsigned int maxdata;
  comedi_range *rng;
  int ret;
  struct parsed_options options;
  int fn;
  sampl_t *data;

  init_parsed_options(&options);
  options.subdevice = -1;
  options.n_chan = 100000;/* default number of samples */
  parse_options(&options, argc, argv);

  /* Use n_chan to set buffer size */
  fn = options.n_chan;
  if(fn <= 0){
    fprintf(stderr, "invalid buffer size %d\n", fn );
    return -1;
  }
  
  /* Force n_chan to be 1 */
  options.n_chan = 1;

  dev = comedi_open( options.filename );
  if(dev == NULL){
    fprintf(stderr, "error opening %s\n", options.filename);
    return -1;
  }
  if(options.subdevice < 0)
    options.subdevice = comedi_find_subdevice_by_type(dev, COMEDI_SUBD_AO, 0);

  /*! This is important to make write calls non blocking! */
  fcntl(comedi_fileno(dev), F_SETFL, O_NONBLOCK);

  maxdata = comedi_get_maxdata(dev, options.subdevice, options.channel);
  rng = comedi_get_range(dev, options.subdevice, options.channel, options.range);

  memset(&cmd,0,sizeof(cmd));
  cmd.subdev = options.subdevice;
  cmd.flags = CMDF_WRITE;
  cmd.start_src = TRIG_INT;
  cmd.start_arg = 0;
  cmd.scan_begin_src = TRIG_TIMER;
  cmd.scan_begin_arg = 1e9 / options.freq;
  cmd.convert_src = TRIG_NOW;
  cmd.convert_arg = 0;
  cmd.scan_end_src = TRIG_COUNT;
  cmd.scan_end_arg = options.n_chan;
  cmd.stop_src = TRIG_COUNT;
  cmd.stop_arg = fn;
  
  cmd.chanlist = chanlist;
  cmd.chanlist_len = options.n_chan;

  chanlist[0] = CR_PACK(options.channel, options.range, options.aref);

  /* init data buffer (a ramp with a final 0): */
  data = (sampl_t *)malloc( fn*sizeof( sampl_t ) );
  if(data == NULL ){
    perror("malloc\n");
    exit(1);
  }
  for ( n=0; n<fn; n++ )
    data[n] = (sampl_t)( maxdata*(double)n/fn );
  data[fn-1] = comedi_from_phys(0.0, rng, maxdata);

  /* Let's make a small buffer: */
  comedi_set_buffer_size( dev, cmd.subdev, 10000 );
  printf( "new buffer size: %d\n\n", comedi_get_buffer_size( dev, cmd.subdev ) );

  err = comedi_command_test(dev, &cmd);
  if (err > 0 && err != 4 ) {
    fprintf( stderr, "comedi_command_test returned %d\n", err );
    dump_cmd(stdout,&cmd);
    exit(1);
  }
  
  err = comedi_command_test(dev, &cmd);
  if (err > 0 && err != 4 ) {
    fprintf( stderr, "comedi_command_test returned %d\n", err );
    dump_cmd(stdout,&cmd);
    exit(1);
  }

  dump_cmd(stdout,&cmd);
  
  if ((err = comedi_command(dev, &cmd)) < 0) {
    comedi_perror("comedi_command");
    exit(1);
  }

  n = fn*sizeof( sampl_t );
  printf( "preload analog output buffer with %d bytes...\n", n );  
  m = write(comedi_fileno(dev), (void *)data, n );
  if(m < 0){
    perror("preload write");
    exit(1);
  }
  printf( "... took %d bytes\n", m );
  n -= m;  
  
  ret = comedi_internal_trigger(dev, options.subdevice, 0);
  if(ret < 0){
    perror("comedi_internal_trigger\n");
    exit(1);
  }
  
  while( n > 0 ) {
    printf( "\nsleep for 1sec...\n" );
    usleep( 100000 );
    printf( "busy: %d\n", ((comedi_get_subdevice_flags( dev, options.subdevice ) & SDF_BUSY) > 0 ) );
    printf( "running: %d\n", ((comedi_get_subdevice_flags( dev, options.subdevice ) & SDF_RUNNING) > 0 ) );
    do {
      printf( "try to write %d bytes ...\n", n );
      m = write(comedi_fileno(dev),(void *)data+(fn*sizeof( sampl_t )-n),n);
      if(m<0){
	if ( errno == EAGAIN )
	  fprintf( stderr, "... no more data can be filled! Try later.\n" );
	else if ( errno == EPIPE ) {
	  fprintf( stderr, "... buffer underrun with EPIPE.\n" );
	  perror("write");
	  exit(0);
	}
	else {
	  perror("write");
	  exit(0);
	}
      }
      else {
	printf("wrote %d bytes\n",m);
	n-=m;
      }
    } while ( m > 0 );
  }
  
  while ( comedi_get_subdevice_flags( dev, cmd.subdev ) & SDF_RUNNING )
    usleep( 500000 );

  printf( "finished\n" );

  free( data );

  comedi_close( dev );
  
  return 0;
}
