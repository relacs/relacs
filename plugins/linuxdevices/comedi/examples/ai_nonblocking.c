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
  int n, m, k;
  comedi_t *dev;
  unsigned int chanlist[16];
  unsigned int maxbuffersize;
  unsigned int maxdata;
  comedi_range *rng;
  int ret;
  struct parsed_options options;
  int fn;
  sampl_t *data, *dp;
  float v;

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
    options.subdevice = comedi_find_subdevice_by_type(dev, COMEDI_SUBD_AI, 0);

  /*! This is important to make read calls non blocking! */
  fcntl(comedi_fileno(dev), F_SETFL, O_NONBLOCK);

  /*
  maxbuffersize = comedi_get_max_buffer_size( dev, options.subdevice );
  fprintf( stderr, "maximum buffer_size: %d\n", maxbuffersize );
  */

  /* you must be root!
  ret = comedi_set_max_buffer_size( dev, options.subdevice, 10*maxbuffersize );
  if ( ret < 0) {
    comedi_perror("comedi_set_max_buffer_size");
    exit(1);
  }
  */

  comedi_set_buffer_size( dev, options.subdevice, fn );
  fprintf( stderr, "buffer_size: %d\n", comedi_get_buffer_size( dev, options.subdevice ) );

  maxdata = comedi_get_maxdata(dev, options.subdevice, options.channel);
  rng = comedi_get_range(dev, options.subdevice, options.channel, options.range);

  memset(&cmd,0,sizeof(cmd));
  cmd.subdev = options.subdevice;
  cmd.flags = 0;
  cmd.start_src = TRIG_INT;
  cmd.start_arg = 0;
  cmd.scan_begin_src = TRIG_TIMER;
  cmd.scan_begin_arg = 1e9 / options.freq;
  cmd.convert_src = TRIG_TIMER;
  cmd.convert_arg = 1e9 / options.freq;
  cmd.scan_end_src = TRIG_COUNT;
  cmd.scan_end_arg = options.n_chan;
  cmd.stop_src = TRIG_COUNT;
  cmd.stop_arg = fn;

  cmd.stop_src = TRIG_NONE;
  cmd.stop_arg = 0;
  
  cmd.chanlist = chanlist;
  cmd.chanlist_len = options.n_chan;

  chanlist[0] = CR_PACK(options.channel, options.range, options.aref);

  dump_cmd(stderr,&cmd);

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

  dump_cmd(stderr,&cmd);

  /* init data buffer: */
  data = (sampl_t *)malloc( fn*sizeof( sampl_t ) );
  if(data == NULL ){
    perror("malloc\n");
    exit(1);
  }
  
  fprintf( stderr, "execute command ...\n" );
  if ((err = comedi_command(dev, &cmd)) < 0) {
    comedi_perror("comedi_command");
    exit(1);
  }
  
  fprintf( stderr, "start analog input ...\n" );
  ret = comedi_internal_trigger(dev, options.subdevice, 0);
  if(ret < 0){
    perror("comedi_internal_trigger\n");
    exit(1);
  }
  
  n = 0;
  while( n < options.n_chan ) {
    fprintf( stderr, "buffer_contents: %d\n", comedi_get_buffer_contents( dev, cmd.subdev ) );
    m = read(comedi_fileno(dev),(void *)data+n,(fn-n)*sizeof( sampl_t));
    if(m<0){
      if ( errno != EAGAIN ) {
	perror("read");
	exit(0);
      }
      else {
	fprintf( stderr, "... no more data can be read! Try later.\n" );
	usleep( 100000 );
      }
    }
    else {
      m /= sizeof( sampl_t);
      fprintf( stderr, "m=%d\n",m);
      n+=m;
    }
  }
  comedi_cancel( dev, cmd.subdev );

  fprintf( stderr, "finished\n" );

  /* save data: */

  dp = data;  
  for ( k=0; k<n; k++ ) {
    v = comedi_to_phys(*dp, rng, maxdata);
    printf( "%g\n", v );
    /*    printf( "%d\n", *dp );*/
    ++dp;
  }

  free( data );

  comedi_close( dev );
  
  return 0;
}
