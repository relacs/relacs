#include "core.h"
#include "misc.h"

#define WAITLOOP { for ( j=0; j<dev->ao_waitcount; j+=3 ) { j -= 2; }; }

#ifdef NIDAQ_AODEBUG
#define DPRINT( x ) printk x
#else
#define DPRINT( x )
#endif

static void ao_free_single_misb( bp dev );
static void ao_free_misb( bp dev );
static int  ao_alloc_misb( bp dev, const char *buf, int size );
static int  ao_direct_write( bp dev, char const *buf, int count );

static int  AO_Add_Channel( bp dev, int configuration );
static void AO_Clear_FIFO( bp dev );
static void AO_Reset_All( bp dev );  
static void AO_Board_Personalize( bp dev );
static void AO_Triggering( bp dev );
static void AO_Counting( bp dev );
static void AO_Updating( bp dev );
static void AO_Channels( bp dev );
static void AO_LDAC_Source_And_Update_Mode( bp dev, int timed );
static void AO_Errors_To_Stop_On( bp dev );
static void AO_FIFO( bp dev, int retransmit, int FIFOMode );
  /* FIFO_Mode: */
  /*   0 on empty, */
  /*   1 on less than half-full, */
  /*   2 on less than full, */
  /*   3 on less than half-full, but keep asserted until FIFO is full */
static void AO_Interrupt_Enable( bp dev, int FIFOInterrupt );
static int  AO_Arming( bp dev );
static void AO_Start_The_Acquisition( bp dev );


void ao_init( bp dev )
{
  unsigned long cj, j;

  if ( dev->board->aoch <= 0 )
    dev->ao_in_use = -1;
  else
    dev->ao_in_use = 0;

  cj = jiffies + 1;
  while ( jiffies < cj );
  cj++;
  for ( j=0; j<0xffffffff && jiffies < cj; j+=3 ) { j -= 2; };
  dev->ao_waitcount = j * HZ / 50000;
  DPRINT(( "nidaq: ao_init() -> waitcount = %d\n", dev->ao_waitcount ));
}


void ao_cleanup( bp dev )
{
}


int ao_open( bp dev, struct file *file )
{
  /* no analog output support: */
  if ( dev->ao_in_use < 0 )
    return EINVAL;

  /* analog output already opened: */
  if ( dev->ao_in_use > 0 )
    return EBUSY;

  dev->ao_in_use++;
  dev->ao_status = 0;
  dev->ao_running = 0;
  dev->ao_isstaging = 0;
  dev->ao_sleep = 0;
  dev->ao_error = 0;
  init_waitqueue_head( &(dev->ao_wait_queue) );
  dev->ao_stop = 0;
  dev->ao_fifo_interrupt = 0;

  dev->ao_channel_number = 0;
  dev->ao_channels = 0;

  dev->ao_repeats = 1;
  dev->ao_startc = 30; 
  dev->ao_intervalc = 2000;
  dev->ao_mute_repeats = 1;
  dev->ao_mute_updates = 0;
  dev->ao_staging = 0;
  dev->ao_fifo_mode = 1;

  dev->ao_start1_source = 0;
  dev->ao_start1_polarity = 0;
  dev->ao_analog_trigger = 0;

  dev->ao_bc_tc = 0;
  dev->ao_continuous = 0;

  dev->ao_update_interrupt = 0;
  dev->ao_next_load_register = 0;
  dev->ao_ai_scans = -1;

  dev->ao_buffer = NULL;
  dev->ao_buffer_size = 0;
  dev->ao_buffer_index = 0;
  dev->ao_buffer_counter = 0;

  dev->ao_misb = NULL;
  dev->ao_misb_param = NULL;
  dev->ao_misb_data = NULL;
  dev->ao_last_misb = NULL;
  dev->ao_misb_count = 0;

  SPINLOCK( dev->lock );
  AO_Reset_All( dev );
  AO_Board_Personalize( dev );
  SPINUNLOCK( dev->lock );

  return 0;
}


void ao_release( bp dev )
{
  if ( dev->ao_in_use > 0 ) {
    dev->ao_in_use = 0;
  }

  SPINLOCK( dev->lock );
  /* disable FIFO interrupts: */
  /* AI_FIFO_Interrupt_Enable = 0 */
  /*
    rewrite that for AO ?
  DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0x0000, 0x0080 );
  */
  AO_Reset_All( dev );
  AO_Board_Personalize( dev );
  SPINUNLOCK( dev->lock );
  ao_free_misb( dev );
  dev->ao_isstaging = 0;
}


int ao_write( bp dev, struct file *file, char const *buf, int count )
{
  unsigned fifo_full=0;            /* flag to indicate data FIFO full */
  int test, testrun, teststaging;

  DPRINT(( "nidaq: ao_write count=%d\n", count ));

  SPINLOCK( dev->lock );
  testrun = dev->ao_running;
  teststaging = dev->ao_isstaging;
  SPINUNLOCK( dev->lock );
  DPRINT(( "nidaq: ao_write: tr=%d, ts=%d, s=%d\n", testrun, teststaging, dev->ao_staging ));
  if ( testrun && !(teststaging && dev->ao_staging ) )
    return -EBUSY;

  dev->ao_error = 0;

  if ( count < 0 || ( count & 1 ) || 
       ( count == 0 && dev->ao_buffer == NULL ) ) {
    printk( "! nidaq: ao_write -> wrong count = %d\n", count );
    return -EINVAL;
  }
  if ( dev->ao_channels <= 0 ) {
    printk( "! nidaq: ao_write -> no channels = %d\n", dev->ao_channels );
    return -EINVAL;
  }

  /* direct write: */
  if ( count == 2*dev->ao_channels )
    return ao_direct_write( dev, buf, count );

  /* timed write: */

  if ( count > 0 ) {
    /* load new signal: */

    if ( !dev->ao_isstaging ) {
      /* free kernel space: */
      ao_free_misb( dev );
    }
    
    /* allocate kernel space for data misb and load data into misb: */
    test = ao_alloc_misb( dev, buf, count/2 );
    if ( test < 0 ) {
      printk( "! nidaq: ao_write -> failed to allocate memory\n" );
      return test;
    }
  }

  dev->ao_isstaging = dev->ao_staging;

  SPINLOCK( dev->lock );
  testrun = dev->ao_running;
  SPINUNLOCK( dev->lock );
  DPRINT(( "nidaq: ao_write -> appending? tr=%d, s=%d, mc=%d\n", testrun, dev->ao_isstaging, dev->ao_misb_count ));
  if ( testrun || ( dev->ao_isstaging && dev->ao_misb_count == 1 ) ) {
    DPRINT(( "nidaq: ao_write -> waveform staging appended signal.\n" ));
    return count;
  }
  
  /* reset the data buffer: */
  dev->ao_buffer = dev->ao_misb_data->buffer;
  dev->ao_current = dev->ao_buffer;
  dev->ao_buffer_write = 0;
  dev->ao_buffer_index = 0;
  dev->ao_buffer_counter = 0;
  dev->ao_buffer_size = dev->ao_misb_data->size;
  
  /* pre-load the data FIFO: */            
  SPINLOCK( dev->lock );
  AO_Clear_FIFO( dev );
  SPINUNLOCK( dev->lock );
  
  do {
    /* load data into the data FIFO: */
    SPINLOCK( dev->lock );
    Board_Write( dev, AO_DAC_FIFO_Data, 
		 dev->ao_current->buffer[dev->ao_buffer_write++] );
    SPINUNLOCK( dev->lock );
    dev->ao_buffer_index++;
    if ( dev->ao_buffer_write >= BUFFBLOCKSIZE ) {
      dev->ao_current = dev->ao_current->next;
      dev->ao_buffer_write = 0;
    }

    if ( dev->ao_staging &&
	 ( dev->ao_buffer_index >= dev->ao_buffer_size ||
	   dev->ao_current == NULL ) ) {
      DPRINT(( "nidaq: ao_write -> next buffer, buffer_counter=%d, staging=%d\n", dev->ao_buffer_counter, dev->ao_staging ));
      dev->ao_buffer_counter++;
      if ( dev->ao_buffer_counter >= dev->ao_misb_data->repeats &&
	   dev->ao_misb_data->next != NULL ) { /* next misb */
	DPRINT(( "nidaq: ao_write -> FIFO load next misb\n" ));
	dev->ao_misb_data->use &= ~2;
	dev->ao_misb_data = dev->ao_misb_data->next;
	dev->ao_buffer = dev->ao_misb_data->buffer;
	dev->ao_buffer_counter = 0;
	dev->ao_buffer_size = dev->ao_misb_data->size;
	ao_free_single_misb( dev );
      }
      dev->ao_current = dev->ao_buffer;
      dev->ao_buffer_index = 0;
      dev->ao_buffer_write = 0;
    }
    
    /* check for data FIFO is full: */
    SPINLOCK( dev->lock );
    fifo_full = DAQSTC_Read( dev, AO_Status_1_Register ) & 0x4000;
    SPINUNLOCK( dev->lock );
  }
  while( !fifo_full && 
	 ( dev->ao_buffer_index < dev->ao_buffer_size || dev->ao_staging ) );
  DPRINT(( "nidaq: ao_write -> buffer_index = %d, buffer_count = %d\n", 
	   dev->ao_buffer_index, dev->ao_buffer_counter ));
  
  /* reset variables: */
  dev->ao_bc_tc = 0;
  dev->ao_update_interrupt = 0;
  dev->ao_ai_scans = -1;
  dev->ao_stop = 0;
  dev->ao_fifo_interrupt = ( dev->ao_buffer_index < dev->ao_buffer_size || 
			     dev->ao_staging );
  DPRINT(( "nidaq: ao_write -> fifo_interrupt %d\n", dev->ao_fifo_interrupt ));
  dev->ao_continuous = 
    ( dev->ao_staging || ( dev->ao_mute_repeats > 0 && dev->ao_mute_updates > 0 ) );
  DPRINT(( "nidaq: ao_write -> continuous %d\n", dev->ao_continuous ));

  /* program the DAQ-STC: */
  SPINLOCK( dev->lock );
  AO_Reset_All( dev );
  AO_Board_Personalize( dev );
  if ( !dev->ao_analog_trigger )
    Analog_Trigger_Control( dev, 0 );
  AO_Triggering( dev );
  AO_Counting( dev ); 
  AO_Updating( dev );
  AO_Channels( dev );
  AO_LDAC_Source_And_Update_Mode( dev, 1 );
  AO_Errors_To_Stop_On( dev );
  AO_FIFO( dev, !dev->ao_fifo_interrupt, dev->ao_fifo_mode );
  if ( dev->ao_analog_trigger )
    Analog_Trigger_Control( dev, 1 );
  dev->ao_running = 1;
  AO_Interrupt_Enable( dev, dev->ao_fifo_interrupt );
  if ( AO_Arming( dev ) ) {
    SPINUNLOCK( dev->lock );
    return -EIO;
  }
  AO_Start_The_Acquisition( dev );

  /* set last misb parameters: */
  if ( dev->ao_misb_param != NULL ) {
    dev->ao_misb_param->use &= ~1;
    dev->ao_misb_param = dev->ao_misb_param->next;
  }
  if ( dev->ao_misb_param != NULL ) {
    dev->ao_misb_param->use &= ~1;
  }
  SPINUNLOCK( dev->lock );

  if ( !( file->f_flags & O_NONBLOCK ) && dev->ao_running ) {
    dev->ao_sleep = 1;
    DPRINT(( "nidaq: AO going to sleep\n" ));
    wait_event_interruptible( dev->ao_wait_queue, ( dev->ao_sleep == 0 ) );
    /*    INTERRUPTIBLE_SLEEP_ON( dev->ao_wait_queue );*/
    if ( signal_pending( current ) ) {
      DPRINT(( "nidaq: ao_write -> AO signaled!\n" ));
      dev->ao_sleep = 0;
      dev->ao_fifo_interrupt = 0;
      dev->ao_update_interrupt = 0;
      SPINLOCK( dev->lock );
      AO_Reset_All( dev );
      AO_Board_Personalize( dev );
      SPINUNLOCK( dev->lock );
      dev->ao_running = 0;
      dev->ao_isstaging = 0;
      ao_free_misb( dev );
    }
    DPRINT(( "nidaq: AO now woken up: bc_tc = %d  index = %d  counter = %d\n", 
	     dev->ao_bc_tc, dev->ao_buffer_index, dev->ao_buffer_counter ));
  }

  if ( dev->ao_error )
    count = -EIO;
  DPRINT(( "nidaq: ao_write -> return %d\n", count ));

  return count;
}


int ao_ioctl( bp dev, struct file *file, unsigned int cmd, 
	      unsigned long arg )
{
  long *lp = (long *)arg;
  int test;

  if ( dev->ao_in_use <= 0 )
    return -EINVAL;

  switch( _IOC_NR( cmd ) ) {

  case _IOC_NR( NIDAQAORESETALL ):
    SPINLOCK( dev->lock );
    AO_Clear_FIFO( dev );
    AO_Reset_All( dev );
    AO_Board_Personalize( dev );
    dev->ao_channels = 0;
    SPINUNLOCK( dev->lock );
    ao_free_misb( dev );
    dev->ao_isstaging = 0;
    return 0;

  case _IOC_NR( NIDAQAORESET ):
    SPINLOCK( dev->lock );
    AO_Clear_FIFO( dev );
    AO_Reset_All( dev );
    AO_Board_Personalize( dev );
    SPINUNLOCK( dev->lock );
    dev->ao_isstaging = 0;
    return 0;

  case _IOC_NR( NIDAQAORESETBUFFER ):
    SPINLOCK( dev->lock );
    AO_Clear_FIFO( dev );
    SPINUNLOCK( dev->lock );
    ao_free_misb( dev );
    return 0;


  case _IOC_NR( NIDAQAOCLEARCONFIG ):
    dev->ao_channels = 0;
    return 0;

  case _IOC_NR( NIDAQAOADDCHANNEL ):
    SPINLOCK( dev->lock );
    test = AO_Add_Channel( dev, arg );
    SPINUNLOCK( dev->lock );
    return test;


  case _IOC_NR( NIDAQAOSTART ):
    if ( arg < 0 || arg > 0x00ffffff )
      return -EINVAL;
    dev->ao_startc = arg;
    return 0;

  case _IOC_NR( NIDAQAODELAY ):
    if ( arg*(dev->time_base/1000000) < 1 || 
	 arg*(dev->time_base/1000000) > 0x00ffffff )
      return -EINVAL;
    dev->ao_startc = arg*(dev->time_base/1000000);
    return 0;

  case _IOC_NR( NIDAQAOINTERVAL ):
    if ( arg < 1 || dev->time_base/arg > dev->board->aomaxspl 
	 || arg > 0x00ffffff )
      return -EINVAL;
    dev->ao_intervalc = arg;
    return 0;

  case _IOC_NR( NIDAQAORATE ):
    if ( arg < 1 || arg > dev->board->aomaxspl 
	 || dev->time_base/arg > 0x00ffffff )
      return -EINVAL;
    dev->ao_intervalc = (dev->time_base + arg/2)/arg;
    DPRINT(( "nidaq: ao_ioctl -> ao_intervalc = %d\n", dev->ao_intervalc ));
    return dev->time_base/dev->ao_intervalc;

  case _IOC_NR( NIDAQAOBUFFERS ):
    if ( arg < 1 || arg > 0x00ffffff )
      return -EINVAL;
    dev->ao_repeats = arg;
    return 0;

  case _IOC_NR( NIDAQAOSTAGING ):
    dev->ao_staging = arg > 0 ? 1 : 0;
    return 0;

  case _IOC_NR( NIDAQAOMUTEUPDATES ):
    if ( arg <= 0 || arg > 0x00ffffff )
      return -EINVAL;
    dev->ao_mute_updates = arg;
    return 0;

  case _IOC_NR( NIDAQAOMUTEBUFFERS ):
    if ( arg < 1 || arg > 0x00ffffff )
      return -EINVAL;
    dev->ao_mute_repeats = arg;
    return 0;


  case _IOC_NR( NIDAQAOSTART1SOURCE ):
    if ( arg > 17 && arg != 19 && arg != 31 )
      return -EINVAL;
    dev->ao_start1_source = arg;
    dev->ao_analog_trigger = 0;
    return 0;

  case _IOC_NR( NIDAQAOSTART1POLARITY ):
    dev->ao_start1_polarity = ( arg > 0 );
    return 0;

  case _IOC_NR( NIDAQAOANALOGTRIGGER ):
    DPRINT(( "nidaq: ao_ioctl -> analog trigger\n" ));
    dev->ao_start1_source = 0;
    dev->ao_analog_trigger = 1;
    return 0;

  case _IOC_NR( NIDAQAOTRIGGERONCE ):
    SPINLOCK( dev->lock );
    /* AO_Mode_1_Register: */
    /*   AO_Trigger_Once = 1 */
    /*     1: stop output timing sequence on BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 0x0001, 0x0001 );
    /* AO_Mode_1_Register: */
    /*   AO_Continuous = 0 */
    /*     counters will not ignore BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 0x0000, 0x0002 );
    SPINUNLOCK( dev->lock );
    dev->ao_stop = 1;
    return 0;

  case _IOC_NR( NIDAQAOENDONBCTC ):
    SPINLOCK( dev->lock );
    /* AO_Command_2_Register: */
    /*  AO_End_On_BC_TC = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_2_Register, 0x8000 );
    dev->ao_stop = 1;
    SPINUNLOCK( dev->lock );
    return 0;


  case _IOC_NR( NIDAQAOBUFFERSTART ):
    *lp = dev->ao_ai_scans;
    return 0;

  case _IOC_NR( NIDAQAOERROR ):
    *lp = dev->ao_error;
    dev->ao_error = 0;
    return 0;

  case _IOC_NR( NIDAQAORUNNING ):
    SPINLOCK( dev->lock );
    test = dev->ao_running;
    SPINUNLOCK( dev->lock );
    return test;

  case _IOC_NR( NIDAQAOISSTAGING ):
    SPINLOCK( dev->lock );
    test = dev->ao_isstaging;
    SPINUNLOCK( dev->lock );
    return test;

  }
  return -EINVAL;
}


void ao_interrupt( bp dev )
{
  u16 l, h;
  u32 ticks;
  int k;
  unsigned long j;

  /***** Error *************************************************************/
  if ( dev->ao_status & 0x0200 ) {
    /* Acknowledge: */
    DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x2000 );
    PRINT(( "nidaq: AO_Overrun_St-> bc_tc=%d, index=%d, size=%d, status=%x\n", 
	     dev->ao_bc_tc, dev->ao_buffer_index, dev->ao_buffer_size, dev->ao_status ));
    if ( dev->ao_running ) {
      dev->ao_error |= 1;
      AO_Reset_All( dev );
      AO_Board_Personalize( dev );
      dev->ao_running = 0;
      dev->ao_isstaging = 0;
      printk( "! nidaq: AO_Overrun_St-> stopped output\n" );
    }
    if ( dev->ao_sleep ) {
      dev->ao_sleep = 0;
      wake_up_interruptible( &dev->ao_wait_queue );
    }
  }

  /**** BC_TC: end of buffer iterations **********************************/
  if ( dev->ao_status & 0x0080 ) {
    DPRINT(( "nidaq: ao_interrupt -> BC_TC no %d\n", dev->ao_bc_tc ));
    
    dev->ao_bc_tc++;
    /* get ai_scans at next UPDATE */
    /* dev->ao_update_interrupt = 1; */       
    /* enable UPDATE interrupts. */
    /* Interrupt_B_Enable_Register: */
    /*   AO_UPDATE_Interrupt_Enable 0x0004 = 1 */
    /*
      DAQSTC_Masked_Write( dev, Interrupt_B_Enable_Register, 
      0x0004, 0x0004 );  
    */

    if ( dev->ao_stop )	{
      DPRINT(( "nidaq: ao_interrupt -> BC_TC stop output of signal.\n" ));

      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x0100 );
 
      dev->ao_stop = 0;
      dev->ao_running = 0;
      dev->ao_isstaging = 0;
      dev->ao_fifo_interrupt = 0;

      /* disable FIFO interrupts: */
      /* AO_FIFO_Interrupt_Enable = 0 */
      DAQSTC_Masked_Write( dev, Interrupt_B_Enable_Register, 
			   0x0000, 0x0100 );

      if ( dev->ao_sleep ) {
	dev->ao_sleep = 0;
	wake_up_interruptible( &dev->ao_wait_queue );
      }
    }
    else {
      DPRINT(( "nidaq: ao_interrupt -> try to get next misb\n" ));
      /* set next misb-parameter: */
      if ( dev->ao_misb_param != NULL ) {
	dev->ao_misb_param = dev->ao_misb_param->next;
	ao_free_single_misb( dev );
      }
      
      /* no more misb-buffer: */
      if ( dev->ao_misb_param == NULL ) {
	/* stop on next BC_TC: */

	/* Acknowledge: */
	DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x0100 );
	
	DPRINT(( "nidaq: ao_interrupt -> stop on next BC_TC\n" ));
	
	/* AO_Command_2_Register: */
	/*  AO_End_On_BC_TC = 1 */
	DAQSTC_Strobe_Write( dev, AO_Command_2_Register, 0x8000 );
	dev->ao_stop = 1;
	dev->ao_isstaging = 0;
      }
      else {
	/* next misb-buffer parameter exist: */
	
	DPRINT(( "nidaq: ao_interrupt -> load next parameter set\n" ));
	
	if ( dev->ao_next_load_register == 1 ) {
	  /* AO_BC_Load_B_Registers: */
	  /*   AO_BC_Load_B = repeats-1 */
	  if ( dev->ao_misb_param->repeats > 0 )
	    ticks = dev->ao_misb_param->repeats - 1;
	  else
	    ticks = 0;
	  DAQSTC_Write( dev, AO_BC_Load_B_Registers, (ticks>>16) & 0x00ff );
	  DAQSTC_Write( dev, AO_BC_Load_B_Registers+1, ticks & 0xffff );
	  
	  /* AO_UC_Load_B_Registers: */
	  /*   AO_UC_Load_B = size - 1 */
	  if ( dev->ao_misb_param->size >= dev->ao_channels ) {
	    if ( dev->ao_channels > 0 )
	      ticks = dev->ao_misb_param->size/dev->ao_channels - 1;
	    else {
	      printk( "! nidaq: ao_interrupt -> ao_channels <= 0!\n" );
	      ticks = dev->ao_misb_param->size - 1;
	    }
	  }
	  else
	    ticks = 0;
	  DAQSTC_Write( dev, AO_UC_Load_B_Registers, (ticks>>16) & 0x00ff );
	  DAQSTC_Write( dev, AO_UC_Load_B_Registers+1, ticks & 0xffff );
	  
	  /* AO_UI_Load_B_Registers: */
	  /*   AO_UI_Load_B = intervals - 1 */
	  ticks = dev->ao_misb_param->interval;
	  ticks--;
	  if ( ticks < 20 )
	    ticks = 20;
	  DAQSTC_Write( dev, AO_UI_Load_B_Registers, (ticks>>16) & 0x00ff );
	  DAQSTC_Write( dev, AO_UI_Load_B_Registers+1, ticks & 0xffff );
	  
	  /* AO_Command_2_Register: */
	  /*   AO_Mute_B = 1 */
	  DAQSTC_Masked_Write( dev, AO_Command_2_Register, 
			       dev->ao_misb_param->mute ? 0x0008 : 0x0000, 0x0008 );
	  
	  dev->ao_next_load_register = 0;
	}
	else {
	  /* AO_BC_Load_A_Registers: */
	  /*   AO_BC_Load_A = repeats - 1 */
	  if ( dev->ao_misb_param->repeats > 0 )
	    ticks = dev->ao_misb_param->repeats - 1;
	  else
	    ticks = 0;
	  DAQSTC_Write( dev, AO_BC_Load_A_Registers, (ticks>>16) & 0x00ff );
	  DAQSTC_Write( dev, AO_BC_Load_A_Registers+1, ticks & 0xffff );
	  
	  /* AO_UC_Load_A_Registers: */
	  /*   AO_UC_Load_A = size - 1 */
	  if ( dev->ao_misb_param->size >= dev->ao_channels ) {
	    if ( dev->ao_channels > 0 )
	      ticks = dev->ao_misb_param->size/dev->ao_channels - 1;
	    else {
	      printk( "! nidaq: ao_interrupt -> ao_channels <= 0!\n" );
	      ticks = dev->ao_misb_param->size - 1;
	    }
	  }
	  else
	    ticks = 0;
	  DAQSTC_Write( dev, AO_UC_Load_A_Registers, (ticks>>16) & 0x00ff );
	  DAQSTC_Write( dev, AO_UC_Load_A_Registers+1, ticks & 0xffff );
	  
	  /* AO_UI_Load_A_Registers: */
	  /*   AO_UI_Load_A = intervals - 1 */
	  ticks = dev->ao_misb_param->interval;
	  ticks--;
	  if ( ticks < 20 )
	    ticks = 20;
	  DAQSTC_Write( dev, AO_UI_Load_A_Registers, (ticks>>16) & 0x00ff );
	  DAQSTC_Write( dev, AO_UI_Load_A_Registers+1, ticks & 0xffff );
	  
	  /* AO_Command_2_Register: */
	  /*   AO_Mute_A = dev->ao_misb_param->mute */
	  DAQSTC_Masked_Write( dev, AO_Command_2_Register, 
			       dev->ao_misb_param->mute ? 0x0004 : 0x0000, 
			       0x0004 );
	  
	  dev->ao_next_load_register = 1;
	}
	
	DPRINT(( "nidaq: ao_interrupt -> set up next parameter set done\n" ));
	dev->ao_misb_param->use &= ~1;
      }
      
      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x0100 );
      
      /* AO_Status_1_Register: */
      /*   AO_BC_TC_Error_St */
      if ( DAQSTC_Read( dev, AO_Status_1_Register ) & 0x0800 ) {
	/* Interrupt_B_Ack_Register: */
	/*   AO_BC_TC_Error_Confirm = 1 */
	DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x0010 );
	
	dev->ao_error |= 2;
	printk( "! nidaq: AO_BC_TC_Error-> stop output bc_tc=%d\n", 
		 dev->ao_bc_tc );
	AO_Reset_All( dev );
	AO_Board_Personalize( dev );
	dev->ao_running = 0;
	dev->ao_isstaging = 0;
	if ( dev->ao_sleep ) {
	  dev->ao_sleep = 0;
	  wake_up_interruptible( &dev->ao_wait_queue );
	}
      }
      
#warning check that only once on the very last BC_TC!
      /* AO_Status_2_Register: */
      /*   AO_BC_TC_Trigger_Error_St */
      if ( DAQSTC_Read( dev, AO_Status_2_Register ) & 0x0010 ) {
	/* Interrupt_B_Ack_Register: */
	/*   AO_BC_TC_Trigger_Error_Confirm = 1 */
	DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x0008 );
	
	dev->ao_error |= 4;
	printk( "! nidaq: AO_BC_TC_Trigger_Error-> stop output bc_tc=%d\n", 
		 dev->ao_bc_tc );
	AO_Reset_All( dev );
	AO_Board_Personalize( dev );
	dev->ao_running = 0;
	dev->ao_isstaging = 0;
	if ( dev->ao_sleep ) {
	  dev->ao_sleep = 0;
	  wake_up_interruptible( &dev->ao_wait_queue );
	}
      }
    }

    /* update status: */
    dev->ao_status = DAQSTC_Read( dev, AO_Status_1_Register );
  }

  /***** START1 ************************************************************/
  if ( dev->ao_status & 0x0100 ) {
    /* Acknowledge: */
    DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x0200 );
    
    DPRINT(( "nidaq: ao_interrupt -> START1\n" ));
    
    Analog_Trigger_Control( dev, 0 );

    if ( dev->ai_running && dev->ai_scan_interval > 0 ) {
      h = DAQSTC_Read( dev, AI_SC_Save_Registers );
      l = DAQSTC_Read( dev, AI_SC_Save_Registers+1 );
      dev->ao_ai_scans = dev->ai_scans - ( h<<16 ) - l - 1;
      dev->ao_ai_scans += dev->ai_sc_tcs*dev->ai_scans;
      dev->ao_ai_scans += dev->ao_startc/dev->ai_scan_interval;
      DPRINT(( "nidaq: ao_interrupt -> START1 ao_ai_scans: %x, sc_tcs: %x, scans: %x, startc: %x\n",
	       dev->ao_ai_scans, dev->ai_sc_tcs, dev->ai_scans, dev->ao_startc ));
    }

    /* Disable external trigger. */
    /* AO_Trigger_Select_Register: */
    /*   AO_START1_Select = 0 */
    /*     use AO_START1_Pulse */
    /*   AO_START1_Polarity = 0 */
    /*   AO_START1_Edge = 1 */
    /*   AO_START1_Sync = 1 */
    /*     enable internal synchronization of the START1 trigger */
    /*     to the BC source */
    /*   AO_Delayed_START1 = 0 */
    /*     use the START1 trigger immediately */
    DAQSTC_Masked_Write( dev, AO_Trigger_Select_Register, 0x0060, 0x607F );
  }

  /***** FIFO not full *****************************************************/
  if ( dev->ao_fifo_interrupt && (dev->ao_status & 0x4000) == 0 ) {
    /* poll to keep the AO FIFO full: */
    
    /* check for data FIFO is full: */
    dev->ao_status = DAQSTC_Read( dev, AO_Status_1_Register );
    while( (dev->ao_status & 0x4000) == 0 &&
	   dev->ao_buffer_counter < dev->ao_misb_data->repeats &&
	   dev->ao_buffer != NULL ) {
      /* load data into the data FIFO: */
      Board_Write( dev, AO_DAC_FIFO_Data, 
		   dev->ao_current->buffer[dev->ao_buffer_write++] );
      dev->ao_buffer_index++;
      if ( dev->ao_buffer_write >= BUFFBLOCKSIZE ) {
	dev->ao_current = dev->ao_current->next;
	dev->ao_buffer_write = 0;
      }
      if ( dev->ao_buffer_index >= dev->ao_buffer_size ||
	   dev->ao_current == NULL ) {
	dev->ao_buffer_counter++;
	if ( dev->ao_buffer_counter >= dev->ao_misb_data->repeats &&
	     dev->ao_misb_data->next != NULL ) { /* next misb */
	  DPRINT(( "nidaq: ao_interrupt -> FIFO load next misb\n" ));
	  dev->ao_misb_data->use &= ~2;
	  dev->ao_misb_data = dev->ao_misb_data->next;
	  dev->ao_buffer = dev->ao_misb_data->buffer;
	  dev->ao_buffer_counter = 0;
	  dev->ao_buffer_size = dev->ao_misb_data->size;
	  ao_free_single_misb( dev );
	}
	else {
	  WAITLOOP;
	}
	dev->ao_current = dev->ao_buffer;
	dev->ao_buffer_index = 0;
	dev->ao_buffer_write = 0;
      }
      /* check for data FIFO is full: */
      dev->ao_status = DAQSTC_Read( dev, AO_Status_1_Register );
    }
  
    /* buffer iterations completed: */
    if ( dev->ao_buffer_counter >= dev->ao_misb_data->repeats ) {
      /* no more data to write */

      dev->ao_fifo_interrupt = 0;
      /* disable interrupts on the FIFO condition. */
      /* Interrupt_B_Enable_Register: */
      /*   AO_FIFO_Interrupt_Enable = 0 */
      DAQSTC_Masked_Write( dev, Interrupt_B_Enable_Register, 
			   0x0000, 0x0100 );
      /* write one more data set to the board to avoid Overrun Error. */
#warning this might influence other channels
      for ( k=0; k<dev->ao_channels; k++ )
	Board_Write( dev, AO_DAC_FIFO_Data, 0 );
      DPRINT(( "nidaq: ao_interrupt -> finished writing data to FIFO  "
	       "index = %d  counter = %d  extra = %d\n",
	       dev->ao_buffer_index, dev->ao_buffer_counter, 
	       dev->ao_channels ));
    }
  }

  /***** UPDATE ************************************************************/
  if ( dev->ao_update_interrupt && (dev->ao_status & 0x0020) ) {
    DPRINT(( "nidaq: ao_interrupt -> UPDATE\n" ));

    DPRINT(( "nidaq: error=%x\n", dev->ao_status & 0x0200 ));
    
    h = DAQSTC_Read( dev, AI_SC_Save_Registers );
    l = DAQSTC_Read( dev, AI_SC_Save_Registers+1 );
    dev->ao_ai_scans = dev->ai_scans - ( h<<16 ) - l - 1;
    dev->ao_ai_scans += dev->ai_sc_tcs*dev->ai_scans;
    
    /* Acknowledge: */
    DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x0400 );
    
    /* disable UPDATE interrupts. */
    dev->ao_update_interrupt = 0;
    /* Interrupt_B_Enable_Register: */
    /*   AO_UPDATE_Interrupt_Enable 0x0004 = 0 */
    DAQSTC_Masked_Write( dev, Interrupt_B_Enable_Register, 
			 0x0000, 0x0004 );
  }

}


int ao_direct_write( bp dev, char const *buf, int count )
{
  u16 value;
  unsigned long r = 0;

  DPRINT(( "nidaq: AO write directly to the DAC\n" ));
  
  SPINLOCK( dev->lock );
  dev->ao_isstaging = 0;
  AO_Reset_All( dev );
  AO_Board_Personalize( dev );
  AO_LDAC_Source_And_Update_Mode( dev, 0 );

  /* get first word: */  
  SPINUNLOCK( dev->lock );
  r = copy_from_user( &value, buf, 2 );
  if ( r != 0 )
    return -EFAULT;

  SPINLOCK( dev->lock );
  if ( dev->ao_channels > 1 ) {                   /* multi-channel mode */
    Board_Write( dev, AO_DAC_0_Data_Register, value );
    if ( count >= 4 ) {
      SPINUNLOCK( dev->lock );
      r = copy_from_user( &value, buf+2, 2 );            /* get second word */
      if ( r != 0 )
	return -EFAULT;
      SPINLOCK( dev->lock );
    }
    Board_Write( dev, AO_DAC_1_Data_Register, value );
  }
  else {                                         /* single-channel-mode */
    if ( dev->ao_channel_number == 0 ) 
      Board_Write( dev, AO_DAC_0_Data_Register, value );
    else if ( dev->ao_channel_number == 1 )
      Board_Write( dev, AO_DAC_1_Data_Register, value );
  }
  SPINUNLOCK( dev->lock );
  
  return count;
}


void ao_free_single_misb( bp dev )
{
  misb *mp, *np;
  buffer_chain *bc, *sc;

  /* nothing to free: */
  if ( dev->ao_misb->use != 0 )
    return;

  DPRINT(( "nidaq: ao_free_single_misb ao_bc_tc=%d\n", dev->ao_bc_tc ));

  SPINLOCK( dev->lock );

  mp = dev->ao_misb;
  /* free buffer chain: */
  for ( bc=mp->buffer; bc != NULL; ) {
    sc = bc->next;
    kfree( bc );
    bc = sc;
  }

  /* free misb: */
  np = mp->next;
  kfree( mp );
  dev->ao_misb = np;

  SPINUNLOCK( dev->lock );
}


void ao_free_misb( bp dev )
{
  misb *mp, *np;
  buffer_chain *bc, *sc;

  DPRINT(( "nidaq: ao_free_misb\n" ));

  SPINLOCK( dev->lock );

  /* loop through all misb: */
  for ( mp=dev->ao_misb; mp != NULL; ) {

    /* free buffer chain: */
    for ( bc=mp->buffer; bc != NULL; ) {
      sc = bc->next;
      kfree( bc );
      bc = sc;
    }

    /* free misb: */
    np = mp->next;
    kfree( mp );
    mp = np;
  }

  dev->ao_misb = NULL;
  dev->ao_misb_data = NULL;
  dev->ao_misb_param = NULL;
  dev->ao_last_misb = NULL;
  dev->ao_misb_count = 0;
  dev->ao_buffer = NULL;
  dev->ao_current = NULL;
  dev->ao_buffer_index = 0;
  dev->ao_buffer_write = 0;
  dev->ao_buffer_counter = 0;
  dev->ao_buffer_size = 0;
  SPINUNLOCK( dev->lock );
}


int  ao_alloc_misb( bp dev, const char *buf, int size )
{
  u32 bs, cs;
  misb *mp;
  buffer_chain *bc, *sc;
  unsigned long test;

  DPRINT(( "nidaq: ao_alloc_misb -> alloc kernel space %d\n", size ));

  /* allocate misb: */
  mp = kmalloc( sizeof( misb ), GFP_KERNEL );
  if ( mp == NULL ) {
    printk( "! nidaq: ao_alloc_misb -> out of memory: misb" );
    return -ENOMEM;
  }
  mp->next = NULL;

  /* allocate memory for signal and load signal: */
  if ( buf != NULL ) {
    DPRINT(( "nidaq: ao_write -> allocate and load data\n" ));

    /* first buffer: */
    test = 0;
    cs = 0;
    bc = kmalloc( sizeof( buffer_chain ), GFP_KERNEL );
    if ( bc != NULL ) {
      cs = size;
      if ( cs > BUFFBLOCKSIZE )
	cs = BUFFBLOCKSIZE;
      test = copy_from_user( bc->buffer, buf, cs*sizeof( u16 ) );
      if ( test )
	kfree( bc );
    }

    /* first buffer failed: */
    if ( bc == NULL || test ) {
      printk( "! nidaq: ao_alloc_misb -> out of memory: first buffer_chain" );
      kfree( mp );
      if ( bc == NULL )
	return -ENOMEM;
      else
	return -EFAULT;
    }

    /* success: */
    bc->next = NULL;
    mp->buffer = bc;
    bs = cs;
    while ( bs < size ) {

      /* next buffer: */
      bc->next = kmalloc( sizeof( buffer_chain ), GFP_KERNEL );
      bc = bc->next;
      if ( bc != NULL ) {
	cs = size - bs;
	if ( cs > BUFFBLOCKSIZE )
	  cs = BUFFBLOCKSIZE;
	test = copy_from_user( bc->buffer, buf+bs*sizeof( u16 ), 
			       cs*sizeof( u16 ) );
      }

      /* next buffer failed: */
      if ( bc == NULL || test ) {
	for ( bc=mp->buffer; bc != NULL; ) {
	  sc = bc->next;
	  kfree( bc );
	  bc = sc;
	}
	mp->buffer = NULL;
	kfree( mp );
	printk( "! nidaq: ao_alloc_misb -> out of memory: buffer_chain" );
	if ( bc == NULL )
	  return -ENOMEM;
	else
	  return -EFAULT;
      }

      /* success: */
      bc->next = NULL;
      bs += cs;
    }
    DPRINT(( "nidaq: ao_alloc_misb -> loaded %d elements\n", cs ));
  }

  /* set parameters of misb: */
  mp->repeats = dev->ao_repeats;
  mp->interval = dev->ao_intervalc;
  mp->mute = ( buf == NULL );
  mp->use = 3;
  mp->size = size;
  if ( buf == NULL )
    mp->buffer = NULL;

  /* hang misb into misb chain: */
  SPINLOCK( dev->lock );
  if ( dev->ao_misb == NULL || dev->ao_last_misb == NULL ) {
    dev->ao_misb = mp;
    dev->ao_misb_data = mp;
    dev->ao_misb_param = mp;
  }
  else
    dev->ao_last_misb->next = mp;
  dev->ao_last_misb = mp;
  dev->ao_misb_count++;
  SPINUNLOCK( dev->lock );

  return 0;  
}


int AO_Add_Channel( bp dev, int configuration ) 
     /*
       add a channel configuration:
       bit 0: 1=bipolar
       bit 1: 1=reglitch
       bit 2: 1=extref
       bit 3: 1=groundref
       bit 8: channel number
      */
{
  if ( dev->ao_channels >= dev->board->aoch )
    return -ECHRNG;
  if ( ( configuration >> 8 ) >= dev->board->aoch )
    return -EINVAL;
  Board_Write( dev, AO_Configuration_Register, configuration );
  dev->ao_channel_number = configuration >> 8;
  dev->ao_channels++;
  DPRINT(( "nidaq: AO_Add_Channel -> configuration = %d, channel=%d from %d\n", 
	   configuration, dev->ao_channel_number, dev->ao_channels ));
  return 0;
}


void AO_Clear_FIFO( bp dev )
{
  DPRINT(( "nidaq: AO_Clear_FIFO\n" ));

  /* Reset the data FIFO. */
  /* Write_Strobe_2_Register = 1 */
  DAQSTC_Strobe_Write( dev, Write_Strobe_2_Register, 0x0001 );
}


void AO_Reset_All( bp dev )
/* reset the analog output functions of the DAQ-STC. */     
{
  DPRINT(( "nidaq: AO_Reset_All\n" ));

  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  /* AO_Command_1_Register: */
  /*   AO_Disarm = 1 */
  DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x2000 );

  /* The following is equal to strobe to AO_Reset, */
  /* except for not resetting AO_Interrupt_Control_Register */
  /* (which doesn't exist!?): */
  /* (But then the register backups have to be rest manually, too!) */
  /* AO_Personal_Register = 0: */
  DAQSTC_Write( dev, AO_Personal_Register, 0 );
  /* AO_Command1_Register = 0x0014 (timed update mode: this is necessary!) */
  DAQSTC_Write( dev, AO_Command_1_Register, 0x0014 );
  /* AO_Command_2_Register = 0 */
  DAQSTC_Write( dev, AO_Command_2_Register, 0 );
  /* AO_Mode_1_Register = 0 */
  DAQSTC_Write( dev, AO_Mode_1_Register, 0 );
  /* AO_Mode_2_Register = 0 */
  DAQSTC_Write( dev, AO_Mode_2_Register, 0 );
  /* AO_Mode_3_Register = 0 */
  DAQSTC_Write( dev, AO_Mode_3_Register, 0 );
  /* AO_Output_Control_Register = 0 */
  DAQSTC_Write( dev, AO_Output_Control_Register, 0 );
  /* AO_START_Select_Register = 0 */
  DAQSTC_Write( dev, AO_START_Select_Register, 0 );
  /* AO_Trigger_Select_Register = 0 */
  DAQSTC_Write( dev, AO_Trigger_Select_Register, 0 );

  /* Interrupt_B_Enable_Register: */
  /*   AO_BC_TC_Interrupt_Enable = 0 */
  /*   AO_START1_Interrupt_Enable = 0 */
  /*   AO_UPDATE_Interrupt_Enable = 0 */
  /*   AO_START_Interrupt_Enable = 0 */
  /*   AO_STOP_Interrupt_Enable = 0 */
  /*   AO_Error_Interrupt_Enable = 0 */
  /*   AO_UC_TC_Interrupt_Enable = 0 */
  /*   AO_UI2_TC_Interrupt_Enable = 0 */
  /*   AO_FIFO_Interrupt_Enable = 0 */
  /* disable all AO interrupts */
  DAQSTC_Masked_Write( dev, Interrupt_B_Enable_Register, 0x0000, 0x01FF );

  /* Interrupt_B_Ack_Register: */
  /*   AO_BC_TC_Trigger_Error_Confirm = 1 */
  /*   AO_BC_TC_Error_Confirm = 1 */
  /*   AO_UI2_TC_Error_Confirm = 1 */
  /*   AO_UI2_TC_Interrupt_Ack = 1 */
  /*   AO_UC_TC_Interrupt_Ack = 1 */
  /*   AO_BC_TC_Interrupt_Ack = 1 */
  /*   AO_START1_Interrupt_Ack = 1 */
  /*   AO_UPDATE_Interrupt_Ack = 1 */
  /*   AO_START_Interrupt_Ack = 1 */
  /*   AO_STOP_Interrupt_Ack = 1 */
  /*   AO_Error_Interrupt_Ack = 1 */
  DAQSTC_Strobe_Write( dev, Interrupt_B_Ack_Register, 0x3FF8 );

  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );

  /* Reset variables: */
  dev->ao_running = 0;
  dev->ao_stop = 0;
}

  
void AO_Board_Personalize( bp dev )
{
  int test;

  DPRINT(( "nidaq: AO_Board_Personalize\n" ));

  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  test = ( dev->type == NI_PCI_MIO_16E_1 || 
	   dev->type == NI_PCI_MIO_16E_4 || 
	   dev->type == NI_PCI_6071E ); 
  /* AO_Personal_Register: */
  /*   AO_Fast_CPU = 0 */
  /*   AO_UPDATE_Pulse_Timebase = 0 */
  /*   AO_UPDATE_Pulse_Width = 1, */
  /*                     for PCI-MIO-16E-1, PCI-6071E or PCI-MIO_16E-4: 0 */
  /*   AO_DMA_PIO_Control = 0 */
  /*   AO_AOFREQ_Polarity = 0 */
  /*   AO_TMRDACWR_Pulse_Width = 1 */
  /*   AO_FIFO_Enable = 1 */
  /*   AO_FIFO_Flags_Polarity = 0 */
  /*   AO_Number_Of_DAC_Packages = 0 */
  /*   AO_BC_Source_Select = 1 (0x10) */
  DAQSTC_Masked_Write( dev, AO_Personal_Register, 
		       test ? 0x1410 : 0x1430, 0x7F70 );

  /* Clock_and_FOUT_Register */
  /*   AO_Source_Divide_By_2 = 0 */
  /*   AO_Output_Divide_By_2 = 1 */
  DAQSTC_Masked_Write( dev, Clock_and_FOUT_Register, 0x0020, 0x0030 );

  /* AO_Output_Control_Register */
  /*   AO_UPDATE_Output_Select = 0 */
  DAQSTC_Masked_Write(dev, AO_Output_Control_Register, 0x0000, 0x0003 );

  /* AO_START_Select_Register: */
  /*   AO_AOFREQ_Enable = 0 */
  DAQSTC_Masked_Write(dev, AO_START_Select_Register, 0x0000, 0x1000 );
  
  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}                 

                                                                   
                                                                   
void AO_Triggering( bp dev )
/* program the trigger signal */
{ 
  DPRINT(( "nidaq: AO_Triggering -> continuous = %d, start1_source=%d, "
	   "polarity=%d\n", 
	   dev->ao_continuous, dev->ao_start1_source, 
	   dev->ao_start1_polarity ));

  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  /* AO_Mode_1_Register: */
  /*   AO_Trigger_Once = !ao_continuous */
  /*     1: stop output timing sequence on BC_TC */
  DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 
		       (dev->ao_continuous ? 0x0000 : 0x0001), 0x0001 );

  if ( dev->ao_start1_source == 0 ) {
    /* software triggered: */

    /* AO_Trigger_Select_Register: */
    /*   AO_START1_Select = 0 */
    /*     use AO_START1_Pulse */
    /*   AO_START1_Polarity = 0 */
    /*   AO_START1_Edge = 1 */
    /*   AO_START1_Sync = 1 */
    /*     enable internal synchronization of the START1 trigger to the BC source */
    /*   AO_Delayed_START1 = 0 */
    /*     use the START1 trigger immediately */
    DAQSTC_Masked_Write( dev, AO_Trigger_Select_Register, 0x0060, 0x607F );
  } 
  else {
    DPRINT(( "nidaq: AO_Triggering -> external START1-trigger\n" ));
    /* AO_Trigger_Select_Register: */
    /*   AO_START1_Select = ao_start1_source */
    /*   AO_START1_Polarity = ao_start1_polarity */
    /*   AO_START1_Edge = 1 */
    /*   AO_START1_Sync = 1 */
    /*     enable internal synchronization of the START1 trigger to the BC source */
    /*   AO_Delayed_START1 = 0 */
    /*     use the START1 trigger immediately */
    DAQSTC_Masked_Write( dev, AO_Trigger_Select_Register, 
			 (dev->ao_start1_source & 0x1F) + (dev->ao_start1_polarity ?  0x2060 : 0x0060), 0x607F );

    if ( dev->ao_start1_source >= 1 && dev->ao_start1_source <= 10 ) {
      /* set PFI-Pin to input */
      MSC_IO_Pin_Configure( dev, dev->ao_start1_source-1, 0 );
    }
  }

  /* AO_Mode_3_Register: */
  /*   AO_Trigger_Length = 1 */
  DAQSTC_Masked_Write( dev, AO_Mode_3_Register, 0x0800, 0x0800 );

  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}

                              
void AO_Counting( bp dev )
     /* program the number and size of local Buffers */
{
  u16 repeats;
  u32 buffersize;
  u32 muterepeats, muteupdates;

  if ( dev->ao_misb_param->size >= dev->ao_channels ) {
    if ( dev->ao_channels > 0 )
      buffersize = dev->ao_misb_param->size/dev->ao_channels;
    else {
      printk( "! nidaq: AO_Counting -> ao_channels <= 0!\n" );
      buffersize = dev->ao_misb_param->size;
    }
  }
  else
    buffersize = 1;

  DPRINT(( "nidaq: AO_Counting -> continuous = %d, buffersize = %d, misb-repeats = %d\n",
	   dev->ao_continuous, buffersize, dev->ao_misb_param->repeats ));
  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  muterepeats = dev->ao_mute_repeats;
  muteupdates = dev->ao_mute_updates;
  
  if ( dev->ao_staging ) {
    /* waveform staging: */

    DPRINT(( "nidaq: AO_Counting -> waveform staging \n" ));

    /* AO_Mode_1_Register: */
    /*   AO_Continuous = 1 */
    /*     counters will ignore BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 0x0002, 0x0002 );
    
    /* AO_Command_2_Register: */
    /*   AO_Mute_A = dev->ao_misb_param->mute */
    DAQSTC_Masked_Write( dev, AO_Command_2_Register, 
			 dev->ao_misb_param->mute ? 0x0004 : 0x0000, 0x0004 );

    /* AO_Mode_2_Register: */
    /*   AO_BC_Initial_Load_Source = A (0) */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0000, 0x0004 );
    
    /* AO_BC_Load_A_Registers: */
    /*   AO_BC_Load_A = repeats - 1 */
    if ( dev->ao_misb_param->repeats > 0 )
      repeats = dev->ao_misb_param->repeats - 1;
    else
      repeats = 0;
    DAQSTC_Write( dev, AO_BC_Load_A_Registers, (repeats>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_BC_Load_A_Registers+1, repeats & 0xffff );
    
    /* AO_Command_1_Register: */
    /*   AO_BC_Load = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0020 );
    
    /* AO_Mode_2_Register: */
    /*   AO_UC_Initial_Load_Source = A (0) */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0000, 0x0800 );
    
    /* AO_UC_Load_A_Registers: */
    /*   AO_UC_Load_A = buffersize */
    DAQSTC_Write( dev, AO_UC_Load_A_Registers, (buffersize>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_A_Registers+1, buffersize & 0xffff );
    
    /* AO_Command_1_Register: */
    /*   AO_UC_Load = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0080 );
    
    /* AO_UC_Load_A_Registers: */
    /*   AO_UC_Load_A = buffersize - 1 */
    buffersize--;
    DAQSTC_Write( dev, AO_UC_Load_A_Registers, (buffersize>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_A_Registers+1, buffersize & 0xffff );

    /* AO_Command_2_Register: */
    /*   AO_Mute_B = mute */
    DAQSTC_Masked_Write( dev, AO_Command_2_Register, 
      dev->ao_misb_param->next != NULL && dev->ao_misb_param->next->mute ? 0x0008 : 0x0000, 0x0008 );

    /* AO_BC_Load_B_Registers: */
    /*   AO_BC_Load_B = repeats-1 */
    if ( dev->ao_misb_param->next != NULL && dev->ao_misb_param->next->repeats > 0 )
      repeats = dev->ao_misb_param->next->repeats - 1;
    else {
      printk( "! nidaq: AO_Counting -> no valid ao_misb_param->next->repeats value!\n" );
      repeats = 0;
    }
    DAQSTC_Write( dev, AO_BC_Load_B_Registers, (repeats>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_BC_Load_B_Registers+1, repeats & 0xffff );

    /* AO_UC_Load_B_Registers: */
    /*   AO_UC_Load_B = size - 1 */
    if ( dev->ao_misb_param->next != NULL && dev->ao_misb_param->next->size >= dev->ao_channels ) {
      if ( dev->ao_channels > 0 )
	buffersize = dev->ao_misb_param->next->size/dev->ao_channels - 1;
      else {
	printk( "! nidaq: AO_Counting -> ao_channels <= 0!\n" );
	buffersize = dev->ao_misb_param->next->size -1;
      }
    }
    else {
      printk( "! nidaq: AO_Counting -> no valid ao_misb_param->next->size value!\n" );
      buffersize = 0;
    }
    DAQSTC_Write( dev, AO_UC_Load_B_Registers, (buffersize>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_B_Registers+1, buffersize & 0xffff );

    /* AO_Mode_2_Register: */
    /*   AO_BC_Reload_Mode = 1 */
    /*     switch load registers on BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0002, 0x0002 );
    
    /* AO_Mode_3_Register: */
    /*   AO_UC_Switch_Load_Every_BC_TC = 1 */
    /*     enable UC counter to switch load registers on BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_3_Register, 0x1000, 0x1000 );

    /* next registers to be loaded are A: */
    dev->ao_next_load_register = 0;
  }

  else if ( muterepeats > 0 && muteupdates > 0 ) { 

    /* local buffer mode with pauses: */

    DPRINT(( "nidaq: AO_Counting -> muterepeats = %d, muteupdates = %d\n", 
	     muterepeats, muteupdates ));

    /* AO_Mode_1_Register: */
    /*   AO_Continuous = 1 */
    /*     counters will ignore BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 0x0002, 0x0002 );
    
    /* AO_Command_2_Register: */
    /*   AO_Mute_A = 0 */
    /*   AO_Mute_B = 1 */
    DAQSTC_Masked_Write( dev, AO_Command_2_Register, 0x0008, 0x000C );

    /* AO_Mode_2_Register: */
    /*   AO_BC_Initial_Load_Source = A (0) */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0000, 0x0004 );
    
    /* AO_BC_Load_A_Registers: */
    /*   AO_BC_Load_A = Repeats - 1 */
    if ( dev->ao_misb_param->repeats > 0 )
      repeats = dev->ao_misb_param->repeats - 1;
    else
      repeats = 0;
    DAQSTC_Write( dev, AO_BC_Load_A_Registers, (repeats>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_BC_Load_A_Registers+1, repeats & 0xffff );
    
    /* AO_Command_1_Register: */
    /*   AO_BC_Load = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0020 );
    
    /* AO_Mode_2_Register: */
    /*   AO_UC_Initial_Load_Source = A (0) */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0000, 0x0800 );
    
    /* AO_UC_Load_A_Registers: */
    /*   AO_UC_Load_A = buffersize */
    DAQSTC_Write( dev, AO_UC_Load_A_Registers, (buffersize>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_A_Registers+1, buffersize & 0xffff );
    
    /* AO_Command_1_Register: */
    /*   AO_UC_Load = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0080 );
    
    /* AO_UC_Load_A_Registers: */
    /*   AO_UC_Load_A = buffersize - 1 */
    buffersize--;
    DAQSTC_Write( dev, AO_UC_Load_A_Registers, (buffersize>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_A_Registers+1, buffersize & 0xffff );
        
    /* AO_BC_Load_B_Registers: */
    /*   AO_BC_Load_B = muterepeats - 1 */
    if ( muterepeats > 0 )
      muterepeats--;
    DAQSTC_Write( dev, AO_BC_Load_B_Registers, (muterepeats>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_BC_Load_B_Registers+1, muterepeats & 0xffff );
    
    /* AO_UC_Load_B_Registers: */
    /*   AO_UC_Load_B = muteupdates - 1 */
    if ( muteupdates > 0 )
      muteupdates--;
    DAQSTC_Write( dev, AO_UC_Load_B_Registers, (muteupdates>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_B_Registers+1, muteupdates & 0xffff );
    
    /* AO_Mode_2_Register: */
    /*   AO_BC_Reload_Mode = 1 */
    /*     switch load registers on BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0002, 0x0002 );
    
    /* AO_Mode_3_Register: */
    /*   AO_UC_Switch_Load_Every_BC_TC = 1 */
    /*     enable UC counter to switch load registers on BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_3_Register, 0x1000, 0x1000 );
  }

  else {

    /* timed output of a single buffer: */

    /* AO_Mode_1_Register: */
    /*   AO_Continuous = ao_continuous */
    /*     counters will ignore BC_TC */
    DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 
			 dev->ao_continuous ? 2 : 0, 0x0002 );
    
    /* AO_Command_2_Register: */
    /*   AO_Mute_A = 0 */
    /*   AO_Mute_B = 0 */
    DAQSTC_Masked_Write( dev, AO_Command_2_Register, 0x0000, 0x000C );

    /* AO_Mode_2_Register: */
    /*   AO_BC_Initial_Load_Source = A (0) */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0000, 0x0004 );
  
    /* AO_BC_Load_A_Registers: */
    /*   AO_BC_Load_A = Repeats - 1 */
    if ( dev->ao_misb_param->repeats > 0 )
      repeats = dev->ao_misb_param->repeats - 1;
    else
      repeats = 0;
    DAQSTC_Write( dev, AO_BC_Load_A_Registers, (repeats>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_BC_Load_A_Registers+1, repeats & 0xffff );
    
    /* AO_Command_1_Register: */
    /*   AO_BC_Load = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0020 );
    
    /* AO_Mode_2_Register: */
    /*   AO_UC_Initial_Load_Source = A (0) */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0000, 0x0800 );
    
    /* AO_UC_Load_A_Registers: */
    /*   AO_UC_Load_A = buffersize */
    DAQSTC_Write( dev, AO_UC_Load_A_Registers, (buffersize>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_A_Registers+1, buffersize & 0xffff );
    
    /* AO_Command_1_Register: */
    /*   AO_UC_Load = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0080 );
    
    /* AO_UC_Load_A_Registers: */
    /*   AO_UC_Load_A = buffersize - 1 */
    buffersize--;
    DAQSTC_Write( dev, AO_UC_Load_A_Registers, (buffersize>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UC_Load_A_Registers+1, buffersize & 0xffff );

    /* we stop on BC_TC: */
    /* AO_Command_2_Register: */
    /*  AO_End_On_BC_TC = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_2_Register, 0x8000 );
    dev->ao_stop = 1;
  }
  
  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}

                                                                         
void AO_Updating( bp dev )
/* set the initial and update intervals */
{
  u32 startc;
  u32 intervalc;

  DPRINT(( "nidaq: AO_Updating -> startc = %d, misb-interval = %d\n", 
	   dev->ao_startc, dev->ao_misb_param->interval ));
  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  /* internal update mode: */

  /* AO_Command_2_Register: */
  /*   AO_BC_Gate_Enable = 0 */
  /*   disable the BC_GATE (external UPDATE pulses passes only when BC counter is enabled) */
  DAQSTC_Masked_Write( dev, AO_Command_2_Register, 0x0000, 0x0800 );
  
  /* UI source is AO_IN_TIMEBASE1: */
  
  /* AO_Mode_1_Register: */
  /*   AO_UPDATE_Source_Select = 0  */
  /*     internal signal UI_TC */
  /*   AO_UPDATE_Source_Polarity = 0 */
  /*     rising edge is acrive edge of the update source */
  /*   AO_UI_Source_Select = 0 */
  /*     internal signal AO_IN_TIMEBASE1 */
  /*   UI_Source_Polarity = 0 */
  /*     rising edge is acrive edge of the UI source */
  DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 0x0000, 0xffd8 );
  
  intervalc = dev->ao_misb_param->interval;
  intervalc--;
  if ( intervalc < 20 )
    intervalc = 20;
  
  if ( dev->ao_staging ) {
    /* waveform staging: */
    
    /* AO_Mode_2_Register: */
    /*   AO_UI_Initial_Load_Source = A (0) */
    /*     load register A as initial UI load register */
    /*   AO_UI_Reload_Mode = 7 */
    /*     no automatic change of the UI load register */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0070, 0x00F0  );
    
    /* AO_UI_Load_A_Registers: */
    /*   AO_UI_Load_A = dev->ao_intervalc-1 */
    DAQSTC_Write( dev, AO_UI_Load_A_Registers, (intervalc>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UI_Load_A_Registers+1, intervalc & 0xffff );
    
    /* AO_Command_1_Register: */
    /*   AO_UI_Load = 1 */
    /*     load the UI counter with the content of the selected UI load register (A) */
    DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0200 );
    
    /* AO_UI_Load_B_Registers: */
    /*   AO_UI_Load_B = dev->ao_misb_param->next->interval-1 */
    intervalc = dev->ao_misb_param->next->interval;
    intervalc--;
    if ( intervalc < 20 )
      intervalc = 20;
    DAQSTC_Write( dev, AO_UI_Load_B_Registers, (intervalc>>16) & 0x00ff );
    DAQSTC_Write( dev, AO_UI_Load_B_Registers+1, intervalc & 0xffff );
  }
  
  else {    
    
    /* AO_Mode_2_Register: */
    /*   AO_UI_Initial_Load_Source = A (0) */
    /*     load register A as initial UI load register */
    /*   AO_UI_Reload_Mode = 0 */
    /*     no automatic change of the UI load register */
    DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 0x0000, 0x00F0  );
    
    startc = dev->ao_startc;
    if ( startc > 0 ) {
      
      /* initial delay: */

      if ( startc > 1 )
	startc--;
      DPRINT(( "nidaq: AO_Updating -> startc = %d\n", startc ));
      /* AO_UI_Load_A_Registers: */
      /*   AO_UI_Load_A = startc-1 */
      DAQSTC_Write( dev, AO_UI_Load_A_Registers, (startc>>16) & 0x00ff );
      DAQSTC_Write( dev, AO_UI_Load_A_Registers+1, startc & 0xffff );
      
      /* AO_Command_1_Register: */
      /*   AO_UI_Load = 1 */
      /*     load the UI counter with the content of the selected UI load register (A) */
      DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0200 );
      
      /* AO_UI_Load_A_Registers: */
      /*   AO_UI_Load_A = dev->ao_misb_param->intervalc-1 */
      DAQSTC_Write( dev, AO_UI_Load_A_Registers, (intervalc>>16) & 0x00ff );
      DAQSTC_Write( dev, AO_UI_Load_A_Registers+1, intervalc & 0xffff );
    }
    
    else {
      
      /* no delay: */
      
      /* AO_UI_Load_A_Registers: */
      /*   AO_UI_Load_A = dev->ao_intervalc-1 */
      DAQSTC_Write( dev, AO_UI_Load_A_Registers, (intervalc>>16) & 0x00ff );
      DAQSTC_Write( dev, AO_UI_Load_A_Registers+1, intervalc & 0xffff );
      
      /* AO_Command_1_Register: */
      /*   AO_UI_Load = 1 */
      /*     load the UI counter with the content of the selected UI load register (A) */
      DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0200 );
    }
    
    /* no change of update count in pause mode!
       if ( dev->ao_mute_repeats > 0 && dev->ao_mute_updates > 0 ) { 
       / local buffer mode with pauses: /
       }
    */
  }

  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}

                                                  
void AO_Channels( bp dev )
/* configure the number of channels */
{
  u8 channelnumber;

  DPRINT(( "nidaq: AO_Channels -> channels = %d, channel_number = %d\n",
	   dev->ao_channels, dev->ao_channel_number ));
  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  /* AO_Mode_1_Register: */
  /*   AO_Multiple_Channels = ao_channels > 1 */
  DAQSTC_Masked_Write( dev, AO_Mode_1_Register, 
			 dev->ao_channels>1 ? 0x0020 : 0, 0x0020 );

  if ( dev->ao_channels > 1)
    channelnumber = dev->ao_channels-1;
  else
    channelnumber = dev->ao_channel_number;
  /* AO_Output_Control_Register: */
  /*   AO_Number_Of_Channels = channelnumber */
  DAQSTC_Masked_Write( dev, AO_Output_Control_Register, 
			 channelnumber << 6, 0x03C0 );

  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}
            
                                            
void AO_LDAC_Source_And_Update_Mode( bp dev, int timed )
  /* Set the source and update mode for the LDAC<0..1> signals. */
  /* timed: 0 immediate update mode, 1 timed update mode */
{
  DPRINT(( "nidaq: AO_LDAC_Source_And_Update_Mode -> timed = %d\n", timed ));

  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  /* AO_Command_1_Register: */
  /*   AO_LDAC0_Source_Select = 0 */
  /*     LDAC0 will output UPDATE */
  /*   AO_DAC0_Update_Mode = timed */
  /*     0: immediate, 1: timed update mode */
  /*   AO_LDAC1_Source_Select = 0 */
  /*     LDAC1 will output UPDATE */
  /*   AO_DAC1_Update_Mode = timed */
  /*     0: immediate, 1: timed update mode */
  DAQSTC_Masked_Write( dev, AO_Command_1_Register, 
			 timed ? 0x0014 : 0x0000, 0x001E );

  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}
          

void AO_Errors_To_Stop_On( bp dev )
/* Set the error conditions upon which the AOTM will stop. */          
{
  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  /* AO_Mode_3_Register: */
  /*   AO_Stop_On_BC_TC_Error = 1 */
  /*     0: continue, 1: stop on BC_TC_Error */
  /*   AO_Stop_On_BC_TC_Trigger_Error = 1 */
  /*     0: continue, 1: stop on BC_TC_Trigger_Error */
  /*   AO_Stop_On_Overrun_Error = 1 */
  /*     0: continue, 1: stop on overun error */
  DAQSTC_Masked_Write( dev, AO_Mode_3_Register, 0x0038, 0x0038 );

  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}
                                                                              

void AO_FIFO( bp dev, int retransmit, int FIFOMode )
/* Set the FIFO mode */
{

  DPRINT(( "nidaq: AO_FIFO -> retransmit = %d, FIFOMode = %d\n", 
	   retransmit, FIFOMode ));

  /* Joint_Reset_Register: */
  /*   AO_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0020 );

  /* AO_Mode_2_Register: */
  /*   AO_FIFO_Retransmit_Enable = retransmit */
  /*     0: disable, 1: enable local buffer mode */
  /*   AO_FIFO_Mode = FIFOMode */
  /*     FIFO-condition: */
  /*       0 on empty, */
  /*       1 on less than half-full, */
  /*       2 on less than full, */
  /*       3 on less than half-full, but keep asserted until FIFO is full */
  DAQSTC_Masked_Write( dev, AO_Mode_2_Register, 
			 (retransmit << 13) | (FIFOMode << 14), 0xE000 );

  /* Joint_Reset_Register: */
  /*   AO_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0200 );
}
                

void AO_Interrupt_Enable( bp dev, int FIFOInterrupt )
/* enable AO interrupts. */
{

  DPRINT(( "nidaq: AO_Interrupt_Enable -> enable FIFO %x\n", FIFOInterrupt ));

  /* Interrupt_A_Enable_Register: */
  /*   AO_FIFO_Interrupt_Enable 0x0100 = FIFOInterrupt */
  /*   AO_UI2_TC_Interrupt_Enable 0x0080 = 0 */
  /*   AO_UC_TC_Interrupt_Enable 0x0040 = 0 */
  /*   AO_Error_Interrupt_Enable 0x0020 = 1 */
  /*   AO_STOP_Interrupt_Enable 0x0010 = 0 (not supported) */
  /*   AO_START_Interrupt_Enable 0x0008 = 0 (not supported) */
  /*   AO_UPDATE_Interrupt_Enable 0x0004 = 0 */
  /*   AO_START1_Interrupt_Enable 0x0002 = 1 */
  /*   AO_BC_TC_Interrupt_Enable 0x0001 = 1 */
  DAQSTC_Masked_Write( dev, Interrupt_B_Enable_Register, 
		       0x0023 | (FIFOInterrupt << 8), 0x01FF );
}


int AO_Arming( bp dev )
/* arm the DAQ-STC. */
{ 
  int i;

  DPRINT(( "nidaq: AO_Arming\n" ));
  /* AO_Mode_3_Register: */
  /*   AO_Not_An_UPDATE = 1 */
  DAQSTC_Masked_Write( dev, AO_Mode_3_Register, 0x0004, 0x0004 );

  /* AO_Mode_3_Register: */
  /*   AO_Not_An_UPDATE = 0 */
  DAQSTC_Masked_Write( dev, AO_Mode_3_Register, 0x0000, 0x0004 );

  /* Joint_Status_2_Register: */
  for( i=0; 
       (DAQSTC_Read( dev, Joint_Status_2_Register ) & 0x1000) > 0;
       i++ )
    if ( i >= 100 ) {
      printk( "! nidaq: AO_Arming() -> failed\n" );
      return 1;
    }
    
  /* AO_Command_1_Register: */
  /*   AO_UI_Arm = 1 */
  /*   AO_UC_Arm = 1 */
  /*   AO_BC_Arm = 1 */
  DAQSTC_Strobe_Write( dev, AO_Command_1_Register, 0x0540 );

  return 0;
}                   
                                       
                                       
void AO_Start_The_Acquisition( bp dev )
     /* start the acquisition */
{ 
  DPRINT(( "nidaq: AO_Start\n" ));
  if ( dev->ao_start1_source == 0 ) {
    /* AO_Command_2_Register: */
    /*   AO_START1_Pulse = 1 */
    DAQSTC_Strobe_Write( dev, AO_Command_2_Register, 0x0001 );
  }
}

#undef WAITLOOP
