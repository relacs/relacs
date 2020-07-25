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
  int aosubdevice;
  int aisubdevice;
  int pfisubdevice = 7;
  int pfi_aostartout_channel = 6;
  int pfi_aistartin_channel = 1;  /* connected via wire to pfi_aostartout_channel */
  comedi_cmd aicmd;
  comedi_cmd aocmd;
  int err;
  int n, m, k;
  comedi_t *dev;
  unsigned int aochanlist[1];
  unsigned int aichanlist[1];
  unsigned int maxdata;
  comedi_range *rng;
  int ret;
  struct parsed_options options;
  int fn = 10000;
  sampl_t *data, *dp;
  sampl_t *aodata;
  float v;

  int triggeronao = 1;  /* 0: internal trigger,
                           1: start AI on PFI channel. */

  init_parsed_options(&options);
  options.n_chan = 1;
  options.n_scan = fn;
  options.freq = 10000;
  parse_options(&options, argc, argv);

  dev = comedi_open( options.filename );
  if(dev == NULL){
    fprintf(stderr, "error opening %s\n", options.filename);
    return -1;
  }

  if ( triggeronao ) {
    /* Route AO_START1 TO PFI: */
    ret = comedi_set_routing( dev, pfisubdevice, pfi_aostartout_channel,
			      NI_PFI_OUTPUT_AO_START1 );
    if ( ret < 0 ) {
      comedi_perror("comedi_routing");
      exit(1);
    }
    ret = comedi_dio_config( dev, pfisubdevice, pfi_aostartout_channel,
			     COMEDI_OUTPUT );
    if ( ret < 0 ) {
      comedi_perror("comedi_dio_config");
      exit(1);
    }
    
    /* Configure other PFI channel for input: */
    ret = comedi_dio_config( dev, pfisubdevice, pfi_aistartin_channel,
			     COMEDI_INPUT );
    if ( ret < 0 ) {
      comedi_perror("comedi_dio_config");
      exit(1);
    }
  }

  /* Analog Output: */
  printf( "init ao ...\n" );
  aosubdevice = comedi_find_subdevice_by_type(dev, COMEDI_SUBD_AO, 0);

  maxdata = comedi_get_maxdata(dev, aosubdevice, options.channel);
  rng = comedi_get_range(dev, aosubdevice, options.channel, options.range);

  memset(&aocmd,0,sizeof(aocmd));
  aocmd.subdev = aosubdevice;
  aocmd.flags = 0;
  aocmd.start_src = TRIG_INT;
  aocmd.start_arg = 0;
  aocmd.scan_begin_src = TRIG_TIMER;
  aocmd.scan_begin_arg = 1e9 / options.freq;
  aocmd.convert_src = TRIG_NOW;
  aocmd.convert_arg = 0;
  aocmd.scan_end_src = TRIG_COUNT;
  aocmd.scan_end_arg = 1;
  aocmd.stop_src = TRIG_COUNT;
  aocmd.stop_arg = fn;
  aocmd.chanlist = aochanlist;
  aocmd.chanlist_len = 1;
  aochanlist[0] = CR_PACK(0, 0, AREF_GROUND );

  err = comedi_command_test(dev, &aocmd);
  if (err > 0 && err != 4 ) {
    fprintf( stderr, "comedi_command_test returned %d\n", err );
    dump_cmd(stdout, &aocmd);
    exit(1);
  }

  dump_cmd(stderr,&aocmd);

  /* init data buffer with sawtooth: */
  aodata = (sampl_t *)malloc( fn*sizeof( sampl_t ) );
  if(aodata == NULL ){
    perror("malloc\n");
    exit(1);
  }
  for ( n=0; n<fn; n++ )
    aodata[n] = (sampl_t)( maxdata*(double)n/(fn/4) );
  aodata[fn-1] = comedi_from_phys(0.0, rng, maxdata);
  
  fprintf( stderr, "execute ao command ...\n" );
  if ((err = comedi_command(dev, &aocmd)) < 0) {
    comedi_perror("comedi_command");
    exit(1);
  }

  // preload data:
  n = fn*sizeof( sampl_t );
  printf( "preload analog output buffer with %d bytes...\n", n );  
  m = write(comedi_fileno(dev), (void *)aodata, n );
  if(m < 0){
    perror("preload write");
    exit(1);
  }
  printf( "... took %d bytes\n", m );
  n -= m;  

  /* Analog Input: */
  printf( "init ai ...\n" );
  aisubdevice = comedi_find_subdevice_by_type(dev, COMEDI_SUBD_AI, 0);

  maxdata = comedi_get_maxdata(dev, aisubdevice, options.channel);
  rng = comedi_get_range(dev, aisubdevice, options.channel, options.range);

  memset(&aicmd,0,sizeof(aicmd));
  aicmd.subdev = aisubdevice;
  aicmd.flags = 0;
  if ( triggeronao ) {
    aicmd.start_src = TRIG_EXT;
    /*! Start on PFI trigger: */
    aicmd.start_arg = CR_EDGE | NI_EXT_PFI(pfi_aistartin_channel);
  }
  else {
    /*! Start on internal trigger: */
    aicmd.start_src = TRIG_INT;
    aicmd.start_arg = 0;
  }
  aicmd.scan_begin_src = TRIG_TIMER;
  aicmd.scan_begin_arg = 1e9 / options.freq;
  aicmd.convert_src = TRIG_TIMER;
  aicmd.convert_arg = 1e9 / options.freq / 10;
  aicmd.scan_end_src = TRIG_COUNT;
  aicmd.scan_end_arg = options.n_chan;
  aicmd.stop_src = TRIG_COUNT;
  aicmd.stop_arg = 2*fn;
  aicmd.chanlist = aichanlist;
  aicmd.chanlist_len = 1;
  aichanlist[0] = CR_PACK(options.n_chan, options.range, options.aref);

  err = comedi_command_test(dev, &aicmd);
  if (err > 0 && err != 4 ) {
    fprintf( stderr, "comedi_command_test returned %d\n", err );
    dump_cmd(stdout,&aicmd);
    exit(1);
  }

  dump_cmd(stderr, &aicmd);

  /* init data buffer: */
  data = (sampl_t *)malloc( 2*fn*sizeof( sampl_t ) );
  if(data == NULL ){
    perror("malloc\n");
    exit(1);
  }
  
  fprintf( stderr, "execute ai command ...\n" );
  if ((err = comedi_command(dev, &aicmd)) < 0) {
    comedi_perror("comedi_command");
    exit(1);
  }

  /* start ai (only if not waiting on AI_START1 trigger): */  
  if ( triggeronao == 0 ) {
    fprintf( stderr, "start analog input ...\n" );
    ret = comedi_internal_trigger(dev, aisubdevice, 0);
    if(ret < 0){
      perror("comedi_internal_trigger\n");
      exit(1);
    }
  }
  
  /* start ao: */  
  fprintf( stderr, "start analog output ...\n" );
  ret = comedi_internal_trigger(dev, aosubdevice, 0);
  if(ret < 0){
    perror("comedi_internal_trigger\n");
    exit(1);
  }

  /* read in analog input data: */
  n = 0;
  while( n < 2*fn ) {
    m = read(comedi_fileno(dev),(void *)(&data[n]),(2*fn-n)*sizeof( sampl_t));
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
      fprintf( stderr, "read %d samples, total %d\n", m, n);
      n+=m;
    }
  }
  
  comedi_cancel( dev, aicmd.subdev );
  comedi_cancel( dev, aocmd.subdev );

  fprintf( stderr, "finished\n" );

  /* save data: */
  FILE *df = fopen( "data.dat", "w" );
  dp = data;  
  for ( k=0; k<n; k++ ) {
    v = comedi_to_phys(*dp, rng, maxdata);
    fprintf( df, "%g\n", v );
    ++dp;
  }
  fclose( df );
  free( data );

  comedi_close( dev );
  
  return 0;
}
