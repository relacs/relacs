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
  unsigned int maxbuffersize;
  unsigned int maxdata;
  comedi_range *rng;
  int ret;
  struct parsed_options options;
  int fn = 3;
  sampl_t *data;
  comedi_insn aotrigger;
  sampl_t tdata[5];


  init_parsed_options(&options);
  options.subdevice = -1;
  options.n_chan = 100000;/* default number of samples */
  parse_options(&options, argc, argv);

  /* Use n_chan to set number of data points */
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

  /* init data buffer (a pulse with a final 0): */
  data = (sampl_t *)malloc( fn*sizeof( sampl_t ) );
  if(data == NULL ){
    perror("malloc\n");
    exit(1);
  }
  for ( n=0; n<fn-1; n++ )
    //    data[n] = (sampl_t)( maxdata*(double)n/fn );
    data[n] = maxdata-1;
    //  data[n] = comedi_from_phys((double)n/fn, rng, maxdata);
  data[fn-1] = comedi_from_phys(0.0, rng, maxdata);

  dump_cmd(stdout,&cmd);
  printf( "buffer_size: %d\n", comedi_get_buffer_size( dev, cmd.subdev ) );

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

  /* Setup analog trigger: */
  memset(&aotrigger,0,sizeof(aotrigger));
  aotrigger.insn = INSN_CONFIG_ANALOG_TRIG;
  aotrigger.n = 5;
  aotrigger.data = &tdata;
  aotrigger.subdev = 0;
  aotrigger.chanspec = 0;
  tdata[0] = 0;
  tdata[1] = 1;
  tdata[2] = CR_PACK(6, 0, AREF_GROUND);
  tdata[3] = comedi_from_phys(0.0, rng, maxdata);
  tdata[4] = 0;
  ret = comedi_do_insn( dev, &aotrigger );  
  if(ret < 0){
    comedi_perror("comedi_do_insn");
    perror("comedi_do_insn");
    exit(1);
  }

  /* Route AO_START1 to PFI0: */
  //  comedi_set_routing( dev, 7, 0, NI_PFI_OUTPUT_AO_START1 );
  comedi_set_routing( dev, 7, 0, NI_PFI_OUTPUT_I_ATRIG );
  comedi_dio_config( dev, 7, 0, COMEDI_OUTPUT );

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
    m = write(comedi_fileno(dev),(void *)data+(fn*sizeof( sampl_t )-n),n);
    if(m<0){
      perror("write");
      exit(0);
    }
    else {
      printf("m=%d\n",m);
      n-=m;
    }
  }

  while ( comedi_get_subdevice_flags( dev, cmd.subdev ) & SDF_RUNNING )
    usleep( 100000 );

  printf( "finished\n" );

  free( data );

  comedi_close( dev );
  
  return 0;
}
