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
  int maxboards = 1;
  comedi_cmd cmd[10];
  int err;
  int b, n, m, k;
  char devstr[100];
  comedi_t *dev[10];
  int subdev[10];
  unsigned int chanlist[10][100];
  unsigned int maxdata;
  comedi_range *rng[10];
  int ret;
  struct parsed_options options;
  int fn = 256*256;
  sampl_t *data, *dp;
  float v;

  init_parsed_options(&options);
  options.subdevice = -1;
  options.n_chan = 100000;/* default number of samples */
  parse_options(&options, argc, argv);

  for ( b=0; b<maxboards; b++ ) {

    sprintf( devstr, "/dev/comedi%d", b );
    
    dev[b] = comedi_open( devstr );
    if(dev[b] == NULL){
      fprintf(stderr, "error opening %s\n", devstr);
      return -1;
    }
    if(options.subdevice < 0)
      subdev[b] = comedi_find_subdevice_by_type(dev[b], COMEDI_SUBD_AI, 0);

    maxdata = comedi_get_maxdata(dev[b], subdev[b], options.channel);
    rng[b] = comedi_get_range(dev[b], subdev[b], options.channel, options.range);
    
    memset(&cmd[b],0,sizeof(cmd[b]));
    
    cmd[b].scan_begin_src = TRIG_TIMER;
    cmd[b].flags = TRIG_ROUND_NEAREST;
    err = comedi_get_cmd_generic_timed( dev[0], subdev[b],
					&cmd[b], options.n_chan,
					1e9 / options.freq );
    cmd[b].start_src = TRIG_INT;
    cmd[b].start_arg = 0;
    cmd[b].scan_end_arg = options.n_chan;
    cmd[b].stop_src = TRIG_NONE;
    cmd[b].stop_arg = 0;
    
    cmd[b].chanlist = chanlist[b];
    cmd[b].chanlist_len = options.n_chan;
    
    for ( k=0; k<options.n_chan; k++ )
      chanlist[b][k] = CR_PACK(k, options.range, options.aref);
    
    dump_cmd(stderr,&cmd[b]);
    
    if ( cmd[b].scan_begin_arg > 1e9 / options.freq ) {
      fprintf( stderr, "frequency too high! Maximum possible is %g Hz\n", 1.0e9/cmd[b].scan_begin_arg );
      comedi_close( dev[0] );
      exit(1);
    }
    
    err = comedi_command_test(dev[0], &cmd[b]);
    if (err > 0 && err != 4 ) {
      fprintf( stderr, "comedi_command_test returned %d\n", err );
      dump_cmd(stdout,&cmd[b]);
      exit(1);
    }
    
    err = comedi_command_test(dev[0], &cmd[b]);
    if (err > 0 && err != 4 ) {
      fprintf( stderr, "comedi_command_test returned %d\n", err );
      dump_cmd(stdout,&cmd[b]);
      exit(1);
    }
    
    dump_cmd(stderr,&cmd[b]);
    
    /* init data buffer: */
    data = (sampl_t *)malloc( fn*sizeof( sampl_t ) );
    if(data == NULL ){
      perror("malloc\n");
      exit(1);
    }
    
    fprintf( stderr, "execute command ...\n" );
    if ((err = comedi_command(dev[0], &cmd[b])) < 0) {
      comedi_perror("comedi_command");
      exit(1);
    }
  }
    
  fprintf( stderr, "start analog input ...\n" );
  for ( b=0; b<maxboards; b++ ) {
    ret = comedi_internal_trigger(dev[0], subdev[b], 0);
    if(ret < 0){
      perror("comedi_internal_trigger\n");
      exit(1);
    }
  }
    
  n = 0;
  while( 1 ) {
    for ( b=0; b<maxboards; b++ ) {
      m = read(comedi_fileno(dev[0]),(void *)data,fn*sizeof( sampl_t));
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
	fprintf( stderr, "board %d read %d samples\n",b,m);
	n+=m;
      }
    }
  }
  for ( b=0; b<maxboards; b++ ) {
    comedi_cancel( dev[b], cmd[b].subdev );
    comedi_close( dev[b] );
  }

  fprintf( stderr, "finished\n" );

  /* save data: */

  /*
  dp = data;  
  for ( k=0; k<n; k++ ) {
    v = comedi_to_phys(*dp, rng[0], maxdata);
    printf( "%g\n", v );
    ++dp;
  }
  */

  free( data );
  
  return 0;
}
