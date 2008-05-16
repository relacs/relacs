#include "core.h"
#include "misc.h"

#ifdef NIDAQ_AIDEBUG
#define DPRINT( x ) printk x
#else
#define DPRINT( x )
#endif

static void ai_free_buffer( bp dev );
static int  ai_alloc_buffer( bp dev, int size );
static int  ai_start( bp dev, int count );

static int  AI_Add_Channel( bp dev, unsigned long x );
static void AI_Clear_FIFO( bp dev );
static void AI_Clear_Configuration( bp dev );

static void AI_Reset_All( bp dev );
static void AI_Board_Personalize( bp dev );
static void AI_Initialize_Configuration_Memory_Output( bp dev );
static void AI_Board_Environmentalize( bp dev );
static void AI_FIFO_Request( bp dev );
static void AI_Hardware_Gating( bp dev );
static void AI_Trigger_Signals( bp dev );
static void AI_Number_of_Scans( bp dev );
static void AI_Scan_Start( bp dev );
static void AI_Scan_End( bp dev );
static void AI_Convert_Signal( bp dev );
static void AI_Interrupt_Enable( bp dev, int stopintr );
static void AI_Arming( bp dev );
static void AI_Start_Daq( bp dev );


void ai_init( bp dev )
{
  DPRINT(( "nidaq: ai_init()\n" ));
  if ( dev->board->aich <= 0 )
    dev->ai_in_use = -1;
  else
    dev->ai_in_use = 0;
}


void ai_cleanup( bp dev )
{
  DPRINT(( "nidaq: ai_cleanup()\n" ));
}


int ai_open( bp dev, struct file *file )
{
  DPRINT(( "nidaq: ai_open()\n" ));
  /* no analog input support: */
  if ( dev->ai_in_use < 0 )
    return EINVAL;

  /* analog input already opened: */
  if ( dev->ai_in_use > 0 )
    return EBUSY;

  dev->ai_in_use++;
  dev->ai_status = 0;
  dev->ai_running = 0;
  dev->ai_sleep = 0;
  dev->ai_stop = 0;
  dev->ai_error = 0;
  init_waitqueue_head( &(dev->ai_wait_queue) );

  dev->ai_channels = 0;
  dev->ai_scans = 1;
  dev->ai_scan_start = 1;
  dev->ai_scan_interval = 20000;
  dev->ai_sample_start = 1;
  dev->ai_sample_interval = 20;
  dev->ai_read_scans = 0;
  dev->ai_sc_tcs = 0;

  dev->ai_ext_mux = 0;
  dev->ai_ext_gating = 0;
  dev->ai_retrigger = 0;
  dev->ai_pretrigger = 0;

  dev->ai_start1_source = 0;
  dev->ai_start1_polarity = 0;
  dev->ai_analog_trigger = 0;

  dev->ai_fifo_mode = 0;
  dev->ai_continuous = 0;

  dev->ai_buffer = NULL;
  dev->ai_read_buffer = NULL;
  dev->ai_write_buffer = NULL;
  dev->ai_nbuffer = 0;
  dev->ai_read_index = 0;
  dev->ai_write_index = 0;
  dev->ai_nread = 0;
  dev->ai_to_be_read = 0;

  SPINLOCK( dev->lock );
  AI_Clear_FIFO( dev );
  AI_Reset_All( dev );
  AI_Board_Personalize( dev );
  AI_Clear_Configuration( dev );
  SPINUNLOCK( dev->lock );

  return 0;
}


void ai_release( bp dev )
{
  DPRINT(( "nidaq: ai_release()\n" ));
  if ( dev->ai_in_use > 0 ) {
    dev->ai_in_use = 0;
  }

  SPINLOCK( dev->lock );
  /* disable FIFO interrupts: */
  /* AI_FIFO_Interrupt_Enable = 0 */
  DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0x0000, 0x0080 );

  AI_Reset_All( dev );
  DPRINT(( "nidaq: ai_release -> all ai activity stopped.\n" ));
  AI_Clear_FIFO( dev );
  AI_Clear_Configuration( dev );
  ai_free_buffer( dev );
  SPINUNLOCK( dev->lock );
}


int ai_read( bp dev, struct file *file, char *buf, int count )
{
  /*  DECLARE_WAITQUEUE( wait, current ); */
  int n, m, sr, cycle;
  int test, retval;
  unsigned long r = 0;

  DPRINT(( "nidaq: ai_read( %p, %d ) -> run %u\n", buf, count, dev->ai_running ));

  /* start data aquisition: */
  SPINLOCK( dev->lock );
  test = ( !dev->ai_running && dev->ai_nread <= 0 );
  SPINUNLOCK( dev->lock );
  if ( test )
    {
      retval = ai_start( dev, count );
      if ( retval < 0 )
	return retval;
      /* do not copy data into user buffer in non-blocking mode: */
      if ( file->f_flags & O_NONBLOCK )
	return -EAGAIN;
    }

  /* check user buffer: */
  if ( !access_ok( VERIFY_WRITE, buf, count ) ) {
    printk( "! nidaq: ai_read -> check on user buffer failed!\n" );
    return -EFAULT;
  }

  /* number of data elements to be read: */
  count /= sizeof( s16 );
  dev->ai_to_be_read = count/2;   /* for continuous acquisition. */

  /* add to local wait queue: */
  /*
  add_wait_queue( &(dev->ai_wait_queue), &wait );
  current->state = TASK_INTERRUPTIBLE;
  */

  retval = 0;
  n = 0;
  cycle = 0;

  do {

    cycle++;

    /* read buffer: */

    /* buffer overflow? */
    SPINLOCK( dev->lock );
    if ( dev->ai_nread >= dev->ai_nbuffer*BUFFBLOCKSIZE ) {
      printk( "! nidaq: ai_read -> buffer overflow: nread=%d, nbuffer=%u, buffersize=%u, cycle = %d\n", 
	      dev->ai_nread, dev->ai_nbuffer, dev->ai_nbuffer*BUFFBLOCKSIZE, cycle );
      do {
	dev->ai_nread -= BUFFBLOCKSIZE;
      } while ( dev->ai_nread >= dev->ai_nbuffer*BUFFBLOCKSIZE );
      SPINUNLOCK( dev->lock );
      retval = -ENOBUFS;
      break;
    }
    else
      SPINUNLOCK( dev->lock );

    DPRINT(( "nidaq: ai_read -> read %d data so far from %d, nread=%d\n", n, count, dev->ai_nread ));

    /* copy kernel buffer to user buffer: */
    while ( n < count ) {
      /* are there data to be read? */
      SPINLOCK( dev->lock );
      test = ( dev->ai_buffer != NULL &&
	       dev->ai_nread > 0 );
      SPINUNLOCK( dev->lock );
      DPRINT(( "nidaq: ai_read -> read buffer %u  b=%p  rb=%p  wb=%p  ri=%u  wi=%u  nr=%u  nb=%u  sc_tcs=%u\n", 
	       test, dev->ai_buffer, dev->ai_read_buffer, dev->ai_write_buffer, 
	       dev->ai_read_index, dev->ai_write_index, dev->ai_nread, dev->ai_nbuffer, dev->ai_sc_tcs ));
      if ( !test )
	break;
      /* number of data to read at once: */
      SPINLOCK( dev->lock );
      if ( dev->ai_read_buffer == dev->ai_write_buffer &&
	   dev->ai_write_index >= dev->ai_read_index ) {
	m = dev->ai_write_index - dev->ai_read_index;
	sr = 1;
      }
      else {
	m = BUFFBLOCKSIZE - dev->ai_read_index;
	sr = 0;
      }
      if ( m > dev->ai_nread ) {
	m = dev->ai_nread;
	printk( "nidaq: ai_read ->  m %d > dev->ai_nread %d\n", m, dev->ai_nread );
      }
      SPINUNLOCK( dev->lock );
      if ( n+m > count ) {
	m = count-n;
	sr = 0;
      }
      /* no data to read: */
      if ( m <= 0 )
	break;
      /* copy data: */
      DPRINT(( "nidaq: ai_read -> copy_to_user ...\n" ));
      r = __copy_to_user( buf, (dev->ai_read_buffer->buffer) + dev->ai_read_index, 
			m * sizeof( s16 ) );
      if ( r != 0 ) {
	printk( "nidaq: ai_read -> copy_to_user failed %lu\n", r );
	/* What error handling is right??? */
      /*	SPINUNLOCK( dev->lock );
	retval = -EFAULT;
	break;*/
      }
      SPINLOCK( dev->lock );
      dev->ai_nread -= m;
      dev->ai_read_index += m;
      if ( dev->ai_read_index >= BUFFBLOCKSIZE ) {
	if ( dev->ai_read_buffer->next != NULL )
	  dev->ai_read_buffer = dev->ai_read_buffer->next;
	else
	  dev->ai_read_buffer = dev->ai_buffer;
	dev->ai_read_index = 0;  /* we read only from a single buffer! */
      }
      SPINUNLOCK( dev->lock );
      DPRINT(( "nidaq: ai_read -> copy_to_user ready\n" ));
      buf += m * sizeof( s16 );
      n += m;
      if ( sr )
	break;
    }

    SPINLOCK( dev->lock );
    test = ( dev->ai_nread < 0 );
    SPINUNLOCK( dev->lock );
    if ( test )
      printk( "! nidaq: ai_read -> read too many data: n=%u  nread=%d\n", n, dev->ai_nread );

    if ( n > count || n < 0 )
      printk( "! nidaq: ai_read -> read too many data: n=%u  count=%u\n", n, count );

    DPRINT(( "nidaq: ai_read -> read %d data so far\n", n ));
    retval = n;
  
    /* free buffer if not continuous mode and buffer is read completely: */
    SPINLOCK( dev->lock );
    if ( dev->ai_error ||
	 ( !dev->ai_continuous && !dev->ai_running &&
	   dev->ai_buffer != NULL &&
	   dev->ai_read_buffer == dev->ai_write_buffer &&
	   dev->ai_read_index == dev->ai_write_index ) ) 
      ai_free_buffer( dev );
    SPINUNLOCK( dev->lock );

    /* read enough data? */
    if ( n >= count || dev->ai_error )
      break;

    /* break if buffer is read completely and ai is not running: */
    SPINLOCK( dev->lock );
    test = ( !dev->ai_running && ( dev->ai_buffer == NULL ||
	     ( dev->ai_read_buffer == dev->ai_write_buffer &&
	       dev->ai_read_index == dev->ai_write_index ) ) );
    SPINUNLOCK( dev->lock );
    if ( test ) {
      DPRINT(( "nidaq: ai_read -> break due to no data and not running. return %d\n", retval ));
      break;
    }

    
    /* non-blocking mode: */
    if ( file->f_flags & O_NONBLOCK ) {
      if ( n <= 0 )
	retval = -EAGAIN;
      break;
    }


    /* A signal is pending: */
    if ( signal_pending( current ) ) {
      DPRINT(( "nidaq: AI signaled!\n" ));
      SPINLOCK( dev->lock );
      AI_Clear_FIFO( dev );
      AI_Reset_All( dev );
      AI_Board_Personalize( dev );
      dev->ai_running = 0;
      dev->ai_sleep = 0;
      dev->ai_stop = 0;
      ai_free_buffer( dev );
      SPINUNLOCK( dev->lock );
      retval = -ERESTARTSYS;
      break;
    }

    /* go to sleep: */
    DPRINT(( "nidaq: AI going to sleep\n" ));
    dev->ai_sleep = 1;
    wait_event_interruptible( dev->ai_wait_queue, ( dev->ai_sleep == 0 ) );
    DPRINT(( "nidaq: AI now woken up\n" ));

    /* go to sleep: */
    /*
    DPRINT(( "nidaq: AI going to sleep\n" ));
    dev->ai_sleep = 1;
    current->state = TASK_INTERRUPTIBLE;
    schedule();
    DPRINT(( "nidaq: AI now woken up\n" ));
    */

  } while ( 1 );

  /*
  current->state = TASK_RUNNING;
  remove_wait_queue( &(dev->ai_wait_queue), &wait );
  */

  DPRINT(( "nidaq: ai_read -> return with %d\n", retval ));

  return retval;
}


int ai_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg )
{
  long *lp = (long *)arg;
  int test;

  if ( dev->ai_in_use <= 0 )
    return -EINVAL;

  switch( _IOC_NR( cmd ) ) {

  case _IOC_NR( NIDAQAIRESETALL ):
    DPRINT(( "nidaq: ai_ioctl() -> AIRESETALL\n" ));
    SPINLOCK( dev->lock );
    AI_Clear_FIFO( dev );
    AI_Reset_All( dev );
    AI_Clear_Configuration( dev );
    ai_free_buffer( dev );
    SPINUNLOCK( dev->lock );
    return 0;

  case _IOC_NR( NIDAQAIRESET ):
    DPRINT(( "nidaq: ai_ioctl() -> AIRESET\n" ));
    SPINLOCK( dev->lock );
    AI_Reset_All( dev );
    SPINUNLOCK( dev->lock );
    return 0;

  case _IOC_NR( NIDAQAIRESETBUFFER ):
    DPRINT(( "nidaq: ai_ioctl() -> AIRESETBUFFER\n" ));
    SPINLOCK( dev->lock );
    AI_Clear_FIFO( dev );
    ai_free_buffer( dev );
    SPINUNLOCK( dev->lock );
    return 0;


  case _IOC_NR( NIDAQAICLEARCONFIG ):
    DPRINT(( "nidaq: ai_ioctl() -> AICLEARCONFIG\n" ));
    SPINLOCK( dev->lock );
    AI_Clear_Configuration( dev );
    SPINUNLOCK( dev->lock );
    return 0;

  case _IOC_NR( NIDAQAIADDCHANNEL ):
    DPRINT(( "nidaq: ai_ioctl() -> AIADDCHANNEL %lx\n", arg ));
    SPINLOCK( dev->lock );
    test = AI_Add_Channel( dev, arg );
    SPINUNLOCK( dev->lock );
    return test;


  case _IOC_NR( NIDAQAISCANSTART ):
    DPRINT(( "nidaq: ai_ioctl() -> AISCANSTART %ld\n", arg ));
    if ( arg < 1 || arg > 0x00ffffff )
      return -EINVAL;
    dev->ai_scan_start = arg;
    return 0;

  case _IOC_NR( NIDAQAISCANDELAY ):
    DPRINT(( "nidaq: ai_ioctl() -> AISCANDELAY %ld\n", arg ));
    if ( arg*(dev->time_base/1000000) < 1 || arg*(dev->time_base/1000000) > 0x00ffffff )
      return -EINVAL;
    dev->ai_scan_start = arg*(dev->time_base/1000000);
    return 0;

  case _IOC_NR( NIDAQAISCANINTERVAL ):
    DPRINT(( "nidaq: ai_ioctl() -> AISCANINTERVAL %ld\n", arg ));
    if ( arg < 1 || dev->time_base/arg > dev->board->aimaxspl || arg > 0x00ffffff )
      return -EINVAL;
    dev->ai_scan_interval = arg;
    return 0;

  case _IOC_NR( NIDAQAISCANRATE ):
    DPRINT(( "nidaq: ai_ioctl() -> AISCANRATE %ld\n", arg ));
    if ( arg < 1 || arg > dev->board->aimaxspl || dev->time_base/arg > 0x00ffffff )
      return -EINVAL;
    dev->ai_scan_interval = dev->time_base/arg;
    return dev->time_base/dev->ai_scan_interval;

  case _IOC_NR( NIDAQAISAMPLESTART ):
    DPRINT(( "nidaq: ai_ioctl() -> AISAMPLESTART %ld\n", arg ));
    if ( arg < 1 || arg > 0xffff )
      return -EINVAL;
    dev->ai_sample_start = arg;
    return 0;

  case _IOC_NR( NIDAQAISAMPLEDELAY ):
    DPRINT(( "nidaq: ai_ioctl() -> AISAMPLEDELAY %ld\n", arg ));
    if ( arg*(dev->time_base/1000000) < 1 || arg*(dev->time_base/1000000) > 0xffff )
      return -EINVAL;
    dev->ai_sample_start = arg*(dev->time_base/1000000);
    return 0;

  case _IOC_NR( NIDAQAISAMPLEINTERVAL ):
    DPRINT(( "nidaq: ai_ioctl() -> AISAMPLEINTERVAL %ld\n", arg ));
    if ( arg < 1 || dev->time_base/arg > dev->board->aimaxspl || arg > 0xffff )
      return -EINVAL;
    dev->ai_sample_interval = arg;
    return 0;

  case _IOC_NR( NIDAQAISAMPLERATE ):
    DPRINT(( "nidaq: ai_ioctl() -> AISAMPLERATE %ld\n", arg ));
    if ( arg < 1 || arg > dev->board->aimaxspl || dev->time_base/arg > 0xffff )
      return -EINVAL;
    dev->ai_sample_interval = dev->time_base/arg;
    return dev->time_base/dev->ai_sample_interval;


  case _IOC_NR( NIDAQAISTART1SOURCE ):
    DPRINT(( "nidaq: ai_ioctl() -> AISTART1SOURCE %ld\n", arg ));
    if ( arg > 18 && arg != 31 )
      return -EINVAL;
    dev->ai_start1_source = arg;
    dev->ai_analog_trigger = 0;
    return 0;

  case _IOC_NR( NIDAQAISTART1POLARITY ):
    DPRINT(( "nidaq: ai_ioctl() -> AISTART1POLARITY %ld\n", arg ));
    dev->ai_start1_polarity = ( arg > 0 );
    return 0;

  case _IOC_NR( NIDAQAIANALOGTRIGGER ):
    DPRINT(( "nidaq: ai_ioctl -> AIANALOGTRIGGER\n" ));
    dev->ai_start1_source = 0;
    dev->ai_analog_trigger = 1;
    return 0;


  case _IOC_NR( NIDAQAICONTINUOUS ):
    DPRINT(( "nidaq: ai_ioctl -> AICONTINUOUS %ld\n", arg ));
    dev->ai_continuous = ( arg > 0 );
    return 0;

  case _IOC_NR( NIDAQAIENDONSCAN ):
    DPRINT(( "nidaq: ai_ioctl -> AIENDONSCAN\n" ));
    SPINLOCK( dev->lock );
    /* enable AI_STOP interrupt: */
    /*   AI_STOP_Interrupt_Enable 0x0010 = 1 */
    /*   AI_START_Interrupt_Enable 0x0008 = 1 (needed to enable stop intrrupt) */
    DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0x0018, 0x0018 );
    /* AI_Command_2_Register: */
    /*  AI_End_On_End_of_Scan = 1 */
    DAQSTC_Strobe_Write( dev, AI_Command_2_Register, 0x4000 );
    SPINUNLOCK( dev->lock );
    dev->ai_stop = 1;
    return 0;


  case _IOC_NR( NIDAQAIERROR ):
    DPRINT(( "nidaq: ai_ioctl -> AIERROR\n" ));
    SPINLOCK( dev->lock );
    *lp = dev->ai_error;
    dev->ai_error = 0;
    SPINUNLOCK( dev->lock );
    return 0;

  case _IOC_NR( NIDAQAIDATA ):
    DPRINT(( "nidaq: ai_ioctl -> AIDATA\n" ));
    SPINLOCK( dev->lock );
    test = ( dev->ai_buffer != NULL &&
	     dev->ai_nread > 0 );
    if ( test )
      *lp = dev->ai_nread;
    else
      *lp = 0;
    SPINUNLOCK( dev->lock );
    return 0;

  case _IOC_NR( NIDAQAIRUNNING ):
    DPRINT(( "nidaq: ai_ioctl -> AIRUNNING\n" ));
    SPINLOCK( dev->lock );
    test = dev->ai_running;
    SPINUNLOCK( dev->lock );
    return test;
 
  }

  return -EINVAL;
}


void ai_interrupt( bp dev )
{
  /***** FIFO not empty ****************************************************/
  if ( ( dev->ai_status & 0x1000 ) == 0 ) {
    /* Read out FIFO: */
    while ( ( dev->ai_status & 0x1000 ) == 0 ) { 
      if ( dev->ai_write_index >= BUFFBLOCKSIZE ) {
	if ( dev->ai_write_buffer->next != NULL )
	  dev->ai_write_buffer = dev->ai_write_buffer->next;
	else
	  dev->ai_write_buffer = dev->ai_buffer;
	dev->ai_write_index = 0;
      }
      dev->ai_write_buffer->buffer[dev->ai_write_index++] = 
	Board_Read( dev, ADC_FIFO_Data_Register );
      dev->ai_nread++;
      dev->ai_read_scans++;
      if ( dev->ai_read_scans >= dev->ai_channels * dev->ai_scans ) {
	dev->ai_read_scans = 0;
	dev->ai_sc_tcs++;
	DPRINT(( "nidaq: ai_interrupt -> FIFO SC_TC: %x\n", dev->ai_sc_tcs ));
      }
      dev->ai_status = DAQSTC_Read( dev, AI_Status_1_Register );
    }
    if ( dev->ai_stop ) {
      DPRINT(( "nidaq: ai_interrupt -> stop on FIFO not empty\n" ));
      /* disable FIFO, START and STOP interrupts: */
      /*   AI_FIFO_Interrupt_Enable = 0 */
      /*   AI_STOP_Interrupt_Enable 0x0010 = 0 */
      /*   AI_START_Interrupt_Enable 0x0008 = 0 */
      DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0x0000, 0x0098 );
      dev->ai_stop = 0;
      dev->ai_running = 0;
      if ( dev->ai_sleep ) {
	DPRINT(( "nidaq: ai_interrupt -> wakeup due to ai_stop\n" ));
	dev->ai_sleep = 0;
	wake_up_interruptible( &dev->ai_wait_queue );
      }
    }
    if ( dev->ai_continuous && dev->ai_sleep && 
	 dev->ai_nread >= dev->ai_to_be_read ) {
      DPRINT(( "nidaq: ai_interrupt -> wakeup due to continuous\n" ));
      dev->ai_sleep = 0;
      wake_up_interruptible( &dev->ai_wait_queue );
    }
  }
  
  /***** Error *************************************************************/
  if ( dev->ai_status & 0x0c00 )
    {
      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_A_Ack_Register, 0x2000 );
      if ( dev->ai_status & 0x0400 )
	{
	  dev->ai_error |= 1;
	  printk( "nidaq: AI_Overflow_St\n" );
	}
      if ( dev->ai_status & 0x0800 )
	{
	  dev->ai_error |= 2;
	  printk( "nidaq: AI_Overrun_St\n" );
	}
      AI_Clear_FIFO( dev );
      AI_Reset_All( dev );
      AI_Board_Personalize( dev );
      dev->ai_running = 0;
      dev->ai_continuous = 0;
      if ( dev->ai_sleep )
	{
	  DPRINT(( "nidaq: ai_interrupt -> wakeup due to error\n" ));
	  dev->ai_sleep = 0;
	  wake_up_interruptible( &dev->ai_wait_queue );
	}
    }

  /***** SC_TC *************************************************************/
  if ( dev->ai_status & 0x0040 )
    {
      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_A_Ack_Register, 0x0100 );
      /* dev->ai_sc_tcs++; I do that in FIFO. */
      DPRINT(( "nidaq: ai_interrupt -> SC_TC: %x\n", dev->ai_sc_tcs ));
      if ( !(dev->ai_continuous) ) {
	/* disable FIFO interrupts: */
	/* AI_FIFO_Interrupt_Enable = 0 */
	DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0x0000, 0x0080 );
	dev->ai_running = 0;
	if ( dev->ai_sleep )
	  {
	    DPRINT(( "nidaq: ai_interrupt -> wakeup due to SC_TC\n" ));
	    dev->ai_sleep = 0;
	    wake_up_interruptible( &dev->ai_wait_queue );
	  }
      }
    }

  /***** STOP: stop scan ***************************************************/
  if ( dev->ai_status & 0x0010 )
    {
      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_A_Ack_Register, 0x1000 );
      DDPRINT(( "nidaq: ai_interrupt -> STOP\n" ));
      if ( dev->ai_stop )
	{
	  DPRINT(( "nidaq: ai_interrupt -> stop on stop scan\n" ));
	  /* disable FIFO, START and STOP interrupts: */
	  /*   AI_FIFO_Interrupt_Enable = 0 */
	  /*   AI_STOP_Interrupt_Enable 0x0010 = 0 */
	  /*   AI_START_Interrupt_Enable 0x0008 = 0 */
	  DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0x0000, 0x0098 );
	  dev->ai_stop = 0;
	  dev->ai_running = 0;
	  if ( dev->ai_sleep )
	    {
	      DPRINT(( "nidaq: ai_interrupt -> wakeup due to stop scan\n" ));
	      dev->ai_sleep = 0;
	      wake_up_interruptible( &dev->ai_wait_queue );
	    }
	}
    }

  /***** START *************************************************************/
  if ( dev->ai_status & 0x0020 )
    {
      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_A_Ack_Register, 0x0800 );
      DDPRINT(( "nidaq: ai_interrupt -> START\n" ));
    }

  /***** START1 ************************************************************/
  if ( dev->ai_status & 0x0080 )
    {
      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_A_Ack_Register, 0x0200 );

      DPRINT(( "nidaq: ai_interrupt -> START1\n" ));
      Analog_Trigger_Control( dev, 0 );

    }

#ifdef NIDAQ_AOINTERRUPTS   /* this is just to avoid the following code */
  /* the other interrupt events: */

  /***** START2 ************************************************************/
  if ( dev->ai_status & 0x0100 )
    {
      /* Acknowledge: */
      DAQSTC_Strobe_Write( dev, Interrupt_A_Ack_Register, 0x0400 );
    }
#endif
}


void ai_free_buffer( bp dev )
{
  buffer_chain *bc, *nc;

  DPRINT(( "nidaq: ai_free_buffer\n" ));
  bc = dev->ai_buffer;
  while ( bc != NULL )
    {
      nc = bc->next;
      kfree( bc ); 
      dev->ai_nbuffer--;
      bc = nc;
    }
  if ( dev->ai_nbuffer > 0 )
    printk( "nidaq: ai_free_buffer -> nbuffer > 0\n" );
  dev->ai_buffer = NULL;
  dev->ai_read_buffer = NULL;
  dev->ai_write_buffer = NULL;
  dev->ai_nbuffer = 0;
  dev->ai_read_index = 0;
  dev->ai_write_index = 0;
  dev->ai_nread = 0;
  dev->ai_to_be_read = 0;
}


int ai_alloc_buffer( bp dev, int size )
{
  buffer_chain *bc, *pc;
  int nb, try;

  if ( size <= 0 )
    ai_free_buffer( dev );
  else if ( dev->ai_nbuffer*BUFFBLOCKSIZE < size )
    {
      /* more memory has to be allocated */
      /* pointer to last buffer-segement: */
      for ( pc = dev->ai_buffer; 
	    pc != NULL && pc->next != NULL; 
	    pc = pc->next );
      do {
	/* try to get memory: */
	try = 0;
	do {
	  bc = kmalloc( sizeof( buffer_chain ), GFP_ATOMIC );
	  try++;
	} while ( bc == NULL && try < 5 );
	if ( bc == NULL )
	  {
	    ai_free_buffer( dev );
	    printk( "nidaq: ai_alloc_buffer -> out of memory\n" );
	    return 1;
	  }
	bc->next = NULL;
	if ( pc == NULL )
	  dev->ai_buffer = bc;
	else
	  pc->next = bc;
	pc = bc;
	dev->ai_nbuffer++;
      }	while ( dev->ai_nbuffer*BUFFBLOCKSIZE < size );
    }
  else if ( (dev->ai_nbuffer-1)*BUFFBLOCKSIZE >= size )
    {
      /* some memory could be freed */
      /* pointer to last necessary buffer-segement: */
      for ( nb=0, bc=NULL, pc=dev->ai_buffer; 
	    pc != NULL && nb*BUFFBLOCKSIZE < size; 
	    nb++ )
	{
	  bc = pc;
	  pc = pc->next;
	}
      /* truncate chain: */
      if ( bc != NULL )
	bc->next = NULL;
      /* free buffer-segments */
      while ( pc != NULL )
	{
	  bc = pc->next;
	  kfree( pc ); 
	  dev->ai_nbuffer--;
	  pc = bc;
	}
    }

  /* check (for debugging only): */
  for ( nb=0, pc = dev->ai_buffer; 
	pc != NULL && pc->buffer != NULL;
	pc = pc->next, nb++ );
  if ( nb != dev->ai_nbuffer )
    printk( "nidaq: ai_alloc_buffer -> something is wrong: nb=%d nbuffer=%u\n", 
	    nb, dev->ai_nbuffer );
  if ( nb*BUFFBLOCKSIZE < size )
    printk( "nidaq: ai_alloc_buffer -> not enough memory allocated, nb=%d\n", 
	    nb );

  dev->ai_read_buffer = dev->ai_buffer;
  dev->ai_write_buffer = dev->ai_buffer;
  dev->ai_read_index = 0;
  dev->ai_write_index = 0;
  dev->ai_nread = 0;
  dev->ai_to_be_read = 0;

  return 0;
}


int ai_start( bp dev, int count )
{
  int test;

  SPINLOCK( dev->lock );
  test = ( dev->ai_running );
  SPINUNLOCK( dev->lock );
  if ( test ) {
    DPRINT(( "nidaq: ai_start -> analog input already running\n" ));
    return -EPERM;              /* data aquisition is already running */
  }

  DPRINT(( "nidaq: ai_start -> start analog input\n" ));
  dev->ai_error = 0;
  dev->ai_stop = 0;
  dev->ai_read_scans = 0;
  dev->ai_sc_tcs = 0;
  dev->ai_to_be_read = 0;

  if ( dev->ai_sample_start + dev->ai_sample_interval*(dev->ai_channels-1) >= dev->ai_scan_interval ) {
    printk( "nidaq: ai_start -> invalid sample intervals\n" );
    return -EINVAL;
  }
  
  if ( dev->ai_channels <= 0 ) {
    printk( "nidaq: ai_start -> no channels specified\n" );
    return -EINVAL;
  }
  
  if ( dev->ai_continuous ) {
    dev->ai_scans = 0x1000000;
    DPRINT(( "nidaq: ai_start -> continuous\n" ));
  }
  else
    dev->ai_scans = count/2/dev->ai_channels;
  
  if ( dev->ai_channels <= 0 || dev->ai_scans <= 0 ) {
    printk( "nidaq: ai_start -> nothing to scan\n" );
    return -EINVAL;
  }
  
  /* allocate memory: */
  SPINLOCK( dev->lock );
  test = ai_alloc_buffer( dev, count/2 );
  SPINUNLOCK( dev->lock );
  if ( test ) {
    printk( "nidaq: ai_start -> failed to allocate memory\n" );
    return -ENOMEM;
  }
  
  DPRINT(( "nidaq: scans=%u  scanST=%u  scanINT=%u  sampleST=%u  sampleINT=%u\n", 
	   dev->ai_scans, dev->ai_scan_start, dev->ai_scan_interval, 
	   dev->ai_sample_start, dev->ai_sample_interval ));
  
  SPINLOCK( dev->lock );
  AI_Clear_FIFO( dev );
  AI_Reset_All( dev );
  AI_Board_Personalize( dev );
  if ( !dev->ai_analog_trigger )
    Analog_Trigger_Control( dev, 0 );
  AI_Initialize_Configuration_Memory_Output( dev );
  AI_Board_Environmentalize( dev );
  AI_FIFO_Request( dev );
  AI_Hardware_Gating( dev );
  AI_Trigger_Signals( dev );
  AI_Number_of_Scans( dev );
  AI_Scan_Start( dev );
  AI_Scan_End( dev );
  AI_Convert_Signal( dev );
  AI_Clear_FIFO( dev );
  if ( dev->ai_analog_trigger )
    Analog_Trigger_Control( dev, 1 );
  dev->ai_running = 1;
  dev->ao_ai_scans = -1;
  AI_Interrupt_Enable( dev, ( dev->ai_scans == 1 ) );
  AI_Arming( dev );
  AI_Start_Daq( dev );
  SPINUNLOCK( dev->lock );

  DPRINT(( "nidaq: AI data aquisition started\n" ));
  
  return 0;  /* success */
}

        	
int AI_Add_Channel( bp dev, unsigned long x )
     /* write into the configuration memory */
{
  int gain, k;

  /* to many channel configurations for configuration buffer: */
  if ( dev->ai_channels >= dev->board->aimaxch )
    return -ECHRNG;

  /* no valid channel number: */
  if ( ((x>>16)&0xf) >= dev->board->aich )
    return -EINVAL;

  /* check gain: */
  gain = x & 7;
  for ( k=0; k<MAXRANGES && dev->board->aiindices[k] >= 0; k++ )
    if ( dev->board->aiindices[k] == gain )
      break;
  /* no valid gain index: */
  if ( k >= MAXRANGES || dev->board->aiindices[k] < 0 )
    return -EINVAL;
  /* no valid range (unipolar mode): */
  if ( ( x & 0x0100 ) && ( dev->board->aiuniranges[k] < 0 ) )
    return -EINVAL;
  /* no valid range (bipolar mode): */
  if ( !( x & 0x0100 ) && ( dev->board->aibiranges[k] < 0 ) )
    return -EINVAL;

  DPRINT(( "nidaq: AI_Add_Channel\n" ));
  DPRINT(( "  Memory_High = %4x\n", (unsigned)(( x>>16 ) & 0xffff) ));
  DPRINT(( "  Memory_Low  = %4x\n", (unsigned)(x & 0xffff) ));
  Board_Write( dev, Configuration_Memory_High, ( x>>16 ) & 0xffff );
  Board_Write( dev, Configuration_Memory_Low, x & 0xffff );
  dev->ai_channels++;

  return 0;
}


void AI_Clear_FIFO( bp dev )
/* clear the AI FIFO. */
{
  /* Write_Strobe_1_Register: */
  /*   Write_Strobe_1 = 1 */
  DAQSTC_Strobe_Write( dev, Write_Strobe_1_Register, 0x0001 );
}


void AI_Clear_Configuration( bp dev )
/* clear the AI configuration memory. */
{
  DPRINT(( "nidaq: AI_Clear_Configuration\n" ));
  /* Write_Strobe_0_Register: */
  /*   Write_Strobe_0 = 1 */
  DAQSTC_Strobe_Write( dev, Write_Strobe_0_Register, 0x0001 );
  dev->ai_channels = 0;
}


void AI_Reset_All( bp dev )
     /* stop any activities in progress */
{
  /* Joint_Reset_Register: */
  /*   AI_Reset = 1 */
  /*     resets all AI-registers to their power on state */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0001 );

  /* Joint_Reset_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  /* Interrupt_A_Enable_Register: */
  /*   AI_SC_TC_Interrupt_Enable = 0 */
  /*   AI_START1_Interrupt_Enable = 0 */
  /*   AI_START2_Interrupt_Enable = 0 */
  /*   AI_START_Interrupt_Enable = 0 */
  /*   AI_STOP_Interrupt_Enable = 0 */
  /*   AI_Error_Interrupt_Enable = 0 */
  /*   AI_FIFO_Interrupt_Enable = 0 */
  /* disable all AI interrupts */
  DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0, 0x00BF );

  /* Interrupt_A_Ack_Register: */
  /*   AI_SC_TC_Error_Confirm = 1 */
  /*   AI_SC_TC_Interrupt_Ack = 1 */
  /*   AI_START1_Interrupt_Ack = 1 */
  /*   AI_START2_Interrupt_Ack = 1 */
  /*   AI_START_Interrupt_Ack = 1 */
  /*   AI_STOP_Interrupt_Ack = 1 */
  /*   AI_Error_Interrupt_Ack = 1 */
  DAQSTC_Masked_Write( dev, Interrupt_A_Ack_Register, 0x3F80, 0x3F80 );

  dev->stccpy[AI_Command_1_Register] = 0;
  dev->stccpy[AI_Command_2_Register] = 0;
  dev->stccpy[AI_Mode_1_Register] = 0;
  dev->stccpy[AI_Mode_2_Register] = 0;
  dev->stccpy[AI_Mode_3_Register] = 0;
  dev->stccpy[AI_Output_Control_Register] = 0;
  dev->stccpy[AI_Personal_Register] = 0;
  dev->stccpy[AI_START_STOP_Select_Register] = 0;
  dev->stccpy[AI_Trigger_Select_Register] = 0;

  /* AI_Mode_1_Register: */
  /*   Reserved_One = 1 */
  /*     always has to be set to one */
  /*   AI_Start_Stop = 1 */
  /*     enable START and STOP control of the analog input operation */
  DAQSTC_Masked_Write( dev, AI_Mode_1_Register, 0x000C, 0x000C );

  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}
 

void AI_Board_Personalize( bp dev )
     /* setup the AITM */
{
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  /* Clock_and_FOUT_Register: */
  /*   AI_Source_Divide_By_2 = 0 */
  /*   AI_Output_Divide_By_2 = 1 */
  DAQSTC_Masked_Write( dev, Clock_and_FOUT_Register, 0x0080, 0x00C0 );

  /* AI_Personal_Register: */
  /*   AI_CONVERT_Pulse_Timebase = 0 */
  /*   AI_CONVERT_Pulse_Width = 1 (depends on AI_Output_Divide_By_2 !?) */
  /*   AI_FIFO_Flags_Polarity = 0 */
  /*   AI_LOCALMUX_CLK_Pulse_Width = 1 (depends on AI_Output_Divide_By_2 !?) */
  /*   AI_AIFREQ_Polarity = 0 */
  /*   AI_SHIFTIN_Polarity = 0 */
  /*   AI_SHIFTIN_Pulse_Width = 1 (depends on AI_Output_Divide_By_2 !?) */
  /*   AI_EOC_Polarity = 0 */
  /*   AI_SOC_Polarity = 1 */
  /*   AI_Overrun_Mode = 1 */
  DAQSTC_Masked_Write( dev, AI_Personal_Register,0xA4A0, 0xFDB0 );

  /* AI_Output_Control_Register: */
  /*   AI_CONVERT_Output_Select = 2 */
  /*   AI_SC_TC_Output_Select = 3 */
  /*   AI_SCAN_IN_PROG_Output_Select = 3 */
  /*   AI_LOCALMUX_CLK_Output_Select = 2 */
  DAQSTC_Masked_Write( dev, AI_Output_Control_Register, 0x032E, 0x033F );

  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}
 


void AI_Initialize_Configuration_Memory_Output( bp dev )
/* access the first value in the configuration FIFO. */
{
  if ( dev->ai_ext_mux ) {
    /* Joint_Rest_Register: */
    /*   AI_Configuration_Start = 1 */
    DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

    printk( "nidaq: ai_ext_mux is not implemented!\n" );

    /* Joint_Reset_Register: */
    /*   AI_Configuration_End = 1 */
    DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
  } 
  else {
    /**** caution! this bit is disabled if AI_Configuration_Start = 1!!! */
    /* AI_Command_1_Register: */
    /*   AI_CONVERT_Pulse = 1 */
    DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x0001 );
  }
}


void AI_Board_Environmentalize( bp dev )
     /* setup for external hardware */
{
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  if ( dev->ai_ext_mux ) {
    printk( "nidaq: ai_ext_mux is not implemented!\n" );
  } 
  else {
    /* AI_Mode_2_Register: */
    /*   AI_External_MUX_Present = 0 */
    DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0000, 0x1000 );

    /* AI_Output_Control_Register: */
    /*   AI_EXTMUX_CLK_Output_Select = 0 */
    DAQSTC_Masked_Write( dev, AI_Output_Control_Register, 0x0000, 0x00C0 );
  }

  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}


void AI_FIFO_Request( bp dev ) 
     /* configure the FIFO condition for which interrupts */
     /* and DMA requests will be generated. */
{
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  /* AI_Mode_3_Register: */
  /*   AI_FIFO_Mode = ai_fifo_mode */
  /*     0: FIFO not empty */
  /*     1: FIFO half-full */
  /*     2: FIFO full */
  /*     3: FIFO half_full until FIFO empty */
  DAQSTC_Masked_Write( dev, AI_Mode_3_Register, ((dev->ai_fifo_mode & 3) << 6), 0x00C0 );
 
  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}


void AI_Hardware_Gating( bp dev )
     /* enable/disable hardware gating. */ 
{
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  if ( dev->ai_ext_gating ) {
    printk( "nidaq: ai_axt_gating is not implemented!\n" );
  } 
  else {
    /* AI_Mode_3_Register: */
    /*   AI_External_Gate_Select = 0 */
    /*   AI_External_Gate_Polarity = 0 */
    DAQSTC_Masked_Write( dev, AI_Mode_3_Register, 0x0000, 0x003F );
  }

  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}


void AI_Trigger_Signals( bp dev )
     /* enable or disable retriggering, configure START1 source. */
{ 
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  if ( dev->ai_retrigger )
    /* AI_Mode_1_Register: */
    /*   AI_Trigger_Once = 0 */
    DAQSTC_Masked_Write( dev, AI_Mode_1_Register, 0x0000, 0x0001 );
  else
    /* AI_Mode_1_Register: */
    /*   AI_Trigger_Once = 1 */
    DAQSTC_Masked_Write( dev, AI_Mode_1_Register, 0x0001, 0x0001 );

  if ( dev->ai_pretrigger ) {
    printk( "nidaq: ai_pretrigger is not implemented!\n" );
  } 
  else {
    if ( dev->ai_start1_source == 0 ) {
      /* AI_Trigger_Select_Register: */
      /*   AI_START1_Select = 0 */
      /*   AI_START1_Polarity = 0 */
      /*   AI_START1_Edge = 1 */
      /*   AI_START1_Sync = 1 */
      DAQSTC_Masked_Write( dev, AI_Trigger_Select_Register, 0x0060, 0x807F );
    }
    else {
      DPRINT(( "nidaq: AI_Trigger_Signals -> external START1-trigger\n" ));
      /* AI_Trigger_Select_Register: */
      /*   AI_START1_Select = ai_start1_source */
      /*   AI_START1_Polarity = ai_start1_polarity */
      /*   AI_START1_Edge = 1 */
      /*   AI_START1_Sync = 1 */
      DAQSTC_Masked_Write( dev, AI_Trigger_Select_Register, 
			   0x0060+(dev->ai_start1_source & 0x1F)+(dev->ai_start1_polarity > 0 ? 0x8000 : 0), 
			   0x807F );

      if ( dev->ai_start1_source >= 1 && dev->ai_start1_source <= 10 ) {
	/* set PFI-Pin to input */
	MSC_IO_Pin_Configure( dev, dev->ai_start1_source-1, 0 );
      }

    }
  }

  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}

 
void AI_Number_of_Scans( bp dev )
     /* select the number of scans */
{
  long ScanNumber;
 
  DPRINT(( "nidaq: AI_Number_of_Scans\n" ));
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  /* AI_Mode_1_Register: */
  /*   AI_Continuous = dev->ai_continuous */
  DAQSTC_Masked_Write( dev, AI_Mode_1_Register, 
			 dev->ai_continuous > 0 ? 0x0002 : 0x0000, 0x0002 );
  
  if ( dev->ai_pretrigger ) {
    printk( "nidaq: ai_pretrigger is not implemented!\n" );
  } 
  else {
    if ( dev->ai_scans > 1 || dev->ai_continuous )
      {
	/* AI_Mode_2_Register: */
	/*   AI_Pretrigger = 0 */
	/*   AI_SC_Initial_Load_Source = 0 */
	/*   AI_SC_Reload_Mode = 0 */
	/*   AI_SC_Write_Switch = 0 */
	DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0000, 0x2007 );
	
	/* AI_SC_Load_A_Register: */
	/*   AI_SC_Load_A = dev->ai_scans - 1 */
	ScanNumber = dev->ai_scans-1;
	DAQSTC_Write( dev, AI_SC_Load_A_Registers, (ScanNumber>>16) & 0x00ff );
	DAQSTC_Write( dev, AI_SC_Load_A_Registers+1, ScanNumber & 0xffff );
	
	/* AI_Command_1_Register: */
	/*   AI_SC_Load = 1 */
	DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x0020 );
	
	/* count scans???? */
	/* AI_Command_2_Register: */
	/*   AI_SC_Save_Trace = 0 */
	DAQSTC_Masked_Write( dev, AI_Command_2_Register, 0x0000, 0x0400 );
	
	/* not a staged aquisition... */

    }
  }
  
  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}
 

void AI_Scan_Start( bp dev )
     /* select the scan start event. */
{
  u32 start_interv;
  u32 scan_interv;

  DPRINT(( "nidaq: AI_Scan_Start\n" ));
    
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  /* internal start source ST_TC: */

  /* AI_Mode_3_Register: */
  /*   AI_SI_Special_Trigger_Delay = 0 */
  DAQSTC_Masked_Write( dev, AI_Mode_3_Register, 0x0000, 0x1000 );
  
  if ( dev->ai_scans == 1 ) {
    /* AI_START_STOP_Select_Register: */
    /*   AI_START_Select = 31 */
    /*   AI_START_Edge = 1 */
    /*   AI_START_Sync = 1 */
    /*   AI_START_Polarity = 0 */
    DAQSTC_Masked_Write( dev, AI_START_STOP_Select_Register, 
			 0x007F, 0x807F );
  }
  else {
    /* AI_START_STOP_Select_Register: */
    /*   AI_START_Select = 0 */
    /*   AI_START_Edge = 1 */
    /*   AI_START_Sync = 1 */
    /*   AI_START_Polarity = 0 */
    DAQSTC_Masked_Write( dev, AI_START_STOP_Select_Register, 
			 0x0060, 0x807F );
    
    /* AI_Mode_1_Register: */
    /*   AI_SI_Source_Select = 0 (AI_IN_TIMEBASE1) */
    /*   AI_SI_Source_Polarity = 0 */
    DAQSTC_Masked_Write( dev, AI_Mode_1_Register, 0x0000, 0x07D0 );
    
    if ( dev->ai_scan_start > 1 )
      start_interv = dev->ai_scan_start - 1;
    else
      start_interv = 1;
    if ( dev->ai_scan_interval > 1 )
      scan_interv = dev->ai_scan_interval - 1;
    else
      scan_interv = 1;
    
    if ( dev->ai_retrigger ) {
      
      /* AI_SI_Load_A_Registers: */
      /*   AI_SI_Load_A = start_interval-1 */
      DAQSTC_Write( dev, AI_SI_Load_A_Registers,
		    (start_interv>>16) & 0x00ff );
      DAQSTC_Write( dev, AI_SI_Load_A_Registers+1,
		    start_interv & 0xffff );
      
      /* AI_Mode_2_Register: */
      /*   AI_SI_Initial_Load_Source = 0 */
      DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0000, 0x0080 );
      
      /* AI_Command_1_Register: */
      /*   AI_SI_Load = 1 */
      DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x0200 );
      
	/* AI_SI_Load_A_Registers: */
	/*   AI_SI_Load_A = scan_interval-1 */
      DAQSTC_Write( dev, AI_SI_Load_A_Registers,
		    (scan_interv>>16) & 0x00ff);
      DAQSTC_Write( dev, AI_SI_Load_A_Registers+1,
		    scan_interv & 0xffff );
      
      /* AI_Mode_2_Register: */
      /*   AI_SI_Initial_Load_Source = 0 */
      /*   AI_SI_Reload_Mode = 0 */
      DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0000, 0x00F0 );
    } 
    else {
      /* AI_SI_Load_B_Registers: */
      /*   AI_SI_Load_B = start_interval-1 */
      DAQSTC_Write( dev, AI_SI_Load_B_Registers,
		    (start_interv>>16) & 0x00ff );
      DAQSTC_Write( dev, AI_SI_Load_B_Registers+1,
		    start_interv & 0xffff );
      
      /* AI_SI_Load_A_Registers: */
      /*   AI_SI_Load_A = scan_interval-1 */
      DAQSTC_Write( dev, AI_SI_Load_A_Registers,
		    (scan_interv>>16) & 0x00ff);
      DAQSTC_Write( dev, AI_SI_Load_A_Registers+1,
		    scan_interv & 0xffff );
      
      /* AI_Mode_2_Register: */
      /*   AI_SI_Initial_Load_Source = 1 */
      DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0080, 0x0080 );
      
      /* AI_Command_1_Register: */
      /*   AI_SI_Load = 1 */
      DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x0200 );
      
      /* AI_Mode_2_Register: */
      /*   AI_SI_Initial_Load_Source = 0 */
	/*   AI_SI_Reload_Mode = 6 (alternate first period on every SC_TC) */
      DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0060, 0x00F0 );
    }
  }
  
  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}
  

void AI_Scan_End( bp dev )
/* select the end of scan event. */
{
  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  /* AI_START_STOP_Select_Register: */
  /*   AI_STOP_Select = 19 */
  /*   AI_STOP_Edge = 0 */
  /*   AI_STOP_Polarity = 0 */
  /*   AI_STOP_Sync = 1 */
  DAQSTC_Masked_Write( dev, AI_START_STOP_Select_Register,
			 0x2980, 0x7F80 );

  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}
 

void AI_Convert_Signal( bp dev )
/* select the convert signal. */
{ 
  u16 spl_start;
  u16 spl_interv;

  DPRINT(( "nidaq: AI_Convert_Signal\n" )); 

  if ( dev->ai_scans == 1 && dev->ai_channels == 1 )
    return;

  /* Joint_Rest_Register: */
  /*   AI_Configuration_Start = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0010 );

  if ( dev->ai_sample_start > 1 )
    spl_start = dev->ai_sample_start - 1;
  else
    spl_start = 1;
  if ( dev->ai_sample_interval > 1 )
    spl_interv = dev->ai_sample_interval - 1;
  else
    spl_interv = 1;

  /* AI_Mode_2_Register: */
  /*   AI_SC_Gate_Enable = 0 */
  /*   AI_Start_Stop_Gate_Enable = 0 */
  /*   AI_SI2_Initial_Load_Source = 0 */
  /*   AI_SI2_Reload_Mode = 1 */
  DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0100, 0xC300 );

  /* AI_Mode_3_Register: */
  /*   AI_SI2_Source_Select_Mode = 0 */
  /*     use the same signal as selected for SI */
  DAQSTC_Masked_Write( dev, AI_Mode_3_Register, 0x0000, 0x0800 );

  /* AI_SI2_Load_A_Register: */
  /*   AI_SI2_Load_A = spl_start-1 */
  DAQSTC_Write( dev, AI_SI2_Load_A_Register, spl_start );

  /* AI_SI2_Load_B_Register: */
  /*   AI_SI2_Load_B = spl_interv-1 */
  DAQSTC_Write( dev, AI_SI2_Load_B_Register, spl_interv );

  /* AI_Command_1_Register: */
  /*   AI_SI2_Load = 1 */
  DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x0800 );

  /* AI_Mode_2_Register: */
  /*   AI_SI2_Initial_Load_Source = 1 */
  DAQSTC_Masked_Write( dev, AI_Mode_2_Register, 0x0200, 0x0200 );

  /* Joint_Reset_Register: */
  /*   AI_Configuration_End = 1 */
  DAQSTC_Strobe_Write( dev, Joint_Reset_Register, 0x0100 );
}
 

void AI_Interrupt_Enable( bp dev, int stopintr )
     /* enable AI interrupts. */
{
  DPRINT(( "nidaq: AI_Interrupt_Enable\n" )); 
  /* Interrupt_A_Enable_Register: */
  /*   AI_FIFO_Interrupt_Enable 0x0080 = 1 */
  /*   AI_Error_Interrupt_Enable 0x0020 = 1 */
  /*   AI_STOP_Interrupt_Enable 0x0010 = stopintr */
  /*   AI_START_Interrupt_Enable 0x0008 = stopintr (needed to enable stop intrrupt) */
  /*   AI_START2_Interrupt_Enable 0x0004 = 0 */
  /*   AI_START1_Interrupt_Enable 0x0002 = 1 */
  /*   AI_SC_TC_Interrupt_Enable 0x0001 = 1 */
  DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 
			 stopintr ? 0x00BB : 0x00A3, 0x00BF );
}


void AI_Arming( bp dev )
/* arm the analog input counters. */
{
  DPRINT(( "nidaq: AI_Arming\n" )); 

  if ( dev->ai_scans == 1 && dev->ai_channels == 1 )
    return;

  if ( dev->ai_scans == 1 )
    {
      /* AI_Command_1_Register: */
      /*   AI_DIV_Arm = 1 */
      DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x0100 );
      /* AI_Command_2_Register: */
      /*   AI_End_On_End_Of_Scan = 1 */
      DAQSTC_Strobe_Write( dev, AI_Command_2_Register, 0x4000 );
    }

  /* AI_Command_1_Register: */
  /*   AI_SC_Arm = 1 */
  /*   AI_SI_Arm = 1 */
  /*   AI_SI2_Arm = 1 */
  /*   AI_DIV_Arm = 1 */
  DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x1540 );
}


void AI_Start_Daq( bp dev )
/* start the acquistion. */
{
  if ( dev->ai_pretrigger ) {
    printk( "nidaq: ai_pretrigger is not implemented!\n" );
  } 
  else {
    if ( dev->ai_scans == 1 && dev->ai_channels == 1 ) {
      /* AI_Command_1_Register: */
      /*   AI_CONVERT_Pulse = 1 */
      DAQSTC_Strobe_Write( dev, AI_Command_1_Register, 0x0001 );
    }
    else if ( dev->ai_start1_source == 0 ) {
      /* AI_Command_2_Register: */
      /*   AI_START1_Pulse = 1 */
      DAQSTC_Strobe_Write( dev, AI_Command_2_Register, 0x0001 );
      
      if ( dev->ai_scans == 1 ) {
	DPRINT(( "nidaq: start single scan\n" ));
	dev->ai_stop = 1;

	/* AI_START_STOP_Select_Register: */
	/*   AI_START_Polarity = 1 */
	DAQSTC_Masked_Write( dev, AI_START_STOP_Select_Register,
			     0x8000, 0x8000 );
	/* AI_START_STOP_Select_Register: */
	/*   AI_START_Polarity = 0 */
	DAQSTC_Masked_Write( dev, AI_START_STOP_Select_Register,
			     0x0000, 0x8000 );
      }
    }
  }
}
