#include "core.h"
#include "misc.h"


#define WAITLOOP { for ( j=0; j<dev->dio_waitcount; j+=3 ) { j -= 2; }; }


#ifdef NIDAQ_DIODEBUG
#define DPRINT( x ) printk x
#else
#define DPRINT( x )
#endif


void dio_init( bp dev )
{
  unsigned long cj, j;

  if ( dev->board->dios <= 0 ) {
    DPRINT(( "nidaq: dio_init() -> no DIOs!\n" ));
    dev->dio_in_use = -1;
  }
  else {
    DPRINT(( "nidaq: dio_init() -> init DIOs\n" ));
    dev->dio_in_use = 0;
    dev->dio_cpy = 0;
    dev->dio_mask = 0xFF;
    dev->dio_buffer = 0;
    dev->dio_buffer_size = 0;

    SPINLOCK( dev->lock );
    /* DIO_Control_Register: */
    /*   DIO_Pins_Dir = DIOOUT */
    /* configure DIOOUT dio lines for output. */
    dev->dio_config = DIOOUT & 0x00FF;
    DAQSTC_Masked_Write( dev, DIO_Control_Register, dev->dio_config, 0x00FF );
    /* DIO_Output_Register: */
    /*   DIO_Parallel_Data_Out = 0x00 */
    /* and set them to low. */
    DAQSTC_Masked_Write( dev, DIO_Output_Register, 0x0000, 0x00FF );
    SPINUNLOCK( dev->lock );

    cj = jiffies + 1;
    while ( jiffies < cj );
    cj++;
    for ( j=0; j<0xffffffff && jiffies < cj; j+=3 ) { j -= 2; };
    /*    dev->dio_waitcount = j * HZ / 50000; */
    dev->dio_waitcount = j * HZ / 500000;  /* changed Mar 30 2006 */
    DPRINT(( "nidaq: dio_init() -> waitcount = %d\n", dev->dio_waitcount ));
  }
}


void dio_cleanup( bp dev )
{
  SPINLOCK( dev->lock );
  /* DIO_Control_Register: */
  /*   DIO_Pins_Dir = DIOOUT */
  /* configure DIOOUT dio lines for output. */
  dev->dio_config = DIOOUT & 0x00FF;
  DAQSTC_Masked_Write( dev, DIO_Control_Register, dev->dio_config, 0x00FF );
  /* DIO_Output_Register: */
  /*   DIO_Parallel_Data_Out = 0x00 */
  /* and set them to low. */
  DAQSTC_Masked_Write( dev, DIO_Output_Register, 0x0000, 0x00FF );
  SPINUNLOCK( dev->lock );

  /* free serial input buffer: */
  if ( dev->dio_buffer != 0 && dev->dio_buffer_size > 0 ) {
    kfree( dev->dio_buffer );
    dev->dio_buffer = 0;
    dev->dio_buffer_size = 0;
  }
}


int dio_open( bp dev, struct file *file )
{
  /* no digital input/output support: */
  if ( dev->dio_in_use < 0 )
    return EINVAL;

  /* digital input output already opened: */
  if ( dev->dio_in_use > 0 )
    return EBUSY;

  dev->dio_in_use++;

  dev->dio_rw = 0;
  if ( file->f_mode & FMODE_READ )
    dev->dio_rw |= 1;
  if ( file->f_mode & FMODE_WRITE )
    dev->dio_rw |= 2;
  dev->dio_cpy = 0;
  dev->dio_mask = 0xFF;
  dev->dio_buffer = 0;
  dev->dio_buffer_size = 0;

  SPINLOCK( dev->lock );
  /* DIO_Control_Register: */
  /*   DIO_Pins_Dir = DIOOUT */
  /* configure DIOOUT dio lines for output. */
  dev->dio_config = DIOOUT & 0x00FF;
  DAQSTC_Masked_Write( dev, DIO_Control_Register, dev->dio_config, 0x00FF );
  /* DIO_Output_Register: */
  /*   DIO_Parallel_Data_Out = 0x00 */
  /* and set them to low. */
  DAQSTC_Masked_Write( dev, DIO_Output_Register, 0x0000, 0x00FF );

  /* disable hardware controlled serial DIO: */
  /* DIO_Control_Register: */
  /*   DIO_HW_Serial_Enable = 0 */
  /* Disable hardware-controlled serial digital I/O. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, 0x0000, 0x0200 );

  /* Clock_and_FOUT_Register: */
  /*   DIO_Serial_Out_Divide_By_2 = 0 (do not divide by 2) */
  /* Do not divide the clock used for serial digital I/O timing by 2. */
  DAQSTC_Masked_Write( dev, Clock_and_FOUT_Register, 0x0000, 0x2000 );
  /* DIO_Control_Register: */
  /*   DIO_HW_Serial_Timebase = 0 (1.2 us clock) or 1 */
  /* Select the fast timebase used for the EXTSTROBE/SDCLK signal during hardware-controlled serial digital I/O. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, 0x0400, 0x0400 );
  SPINUNLOCK( dev->lock );

  return 0;
}


void dio_release( bp dev )
{
  SPINLOCK( dev->lock );
  /* DIO_Control_Register: */
  /*   DIO_Pins_Dir = DIOOUT */
  /* configure DIOOUT dio lines for output. */
  dev->dio_config = DIOOUT & 0x00FF;
  DAQSTC_Masked_Write( dev, DIO_Control_Register, dev->dio_config, 0x00FF );
  /* DIO_Output_Register: */
  /*   DIO_Parallel_Data_Out = 0x00 */
  /* and set them to low. */
  DAQSTC_Masked_Write( dev, DIO_Output_Register, 0x0000, 0x00FF );
  SPINUNLOCK( dev->lock );

  /* free serial input buffer: */
  if ( dev->dio_buffer != 0 && dev->dio_buffer_size > 0 ) {
    kfree( dev->dio_buffer );
    dev->dio_buffer = 0;
    dev->dio_buffer_size = 0;
  }

  if ( dev->dio_in_use > 0 ) {
    dev->dio_in_use = 0;
  }
}


int dio_read( bp dev, struct file *file, char *buf, int count )
{
  u8 u;
  int k;
  long j;
  int test;

  /* return content of read buffer: */
  if ( dev->dio_buffer != 0 && dev->dio_buffer_size > 0 ) {
    DPRINT(( "nidaq: dio_read() -> return content of read buffer\n" ));
    for ( k=0; k<count && k<dev->dio_buffer_size; k++ )
      buf[k] = dev->dio_buffer[k];
    if ( k >= dev->dio_buffer_size ) {
      kfree( dev->dio_buffer );
      dev->dio_buffer = 0;
      dev->dio_buffer_size = 0;
    }
    else {
      for ( ; k<dev->dio_buffer_size; k++ )
	dev->dio_buffer[k-count] = dev->dio_buffer[k];
      dev->dio_buffer_size -= count;
    }
    return k;
  }

  SPINLOCK( dev->lock );
  /* enable serial DIO: */
  /* configure DIO4 for input and DIO0 for output. */
  u = dev->dio_config | 0x01;
  u &= 0xEF;
  DPRINT(( "nidaq: dio_read() -> configure DIO with %x\n", u ));
  DAQSTC_Masked_Write( dev, DIO_Control_Register, u, 0x00FF );
  /* DIO_Control_Register: */
  /*   DIO_HW_Serial_Enable = 1 */
  /* Enable hardware-controlled serial digital I/O. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, 0x0200, 0x0200 );

  /* Joint_Status_1_Register: DIO_Serial_IO_In_Progress_St */
  test = DAQSTC_Read( dev, Joint_Status_1_Register ) & 0x1000;
  SPINUNLOCK( dev->lock );
  if ( test )
    return -EBUSY;

  WAITLOOP;

  for ( k=0; k<count; k++ ) {
    SPINLOCK( dev->lock );
    /* DIO_Control_Register: */
    /*   DIO_HW_Serial_Start = 1 */
    /* Start the hardware-controlled serial digital I/O if enabled. */
    DAQSTC_Strobe_Write( dev, DIO_Control_Register, 0x0100 );

    /* Joint_Status_1_Register: DIO_Serial_IO_In_Progress_St */
    test = DAQSTC_Read( dev, Joint_Status_1_Register ) & 0x1000;
    SPINUNLOCK( dev->lock );
    while ( test ) {
      WAITLOOP;
      SPINLOCK( dev->lock );
      /* Joint_Status_1_Register: DIO_Serial_IO_In_Progress_St */
      test = DAQSTC_Read( dev, Joint_Status_1_Register ) & 0x1000;
      SPINUNLOCK( dev->lock );
    }
    WAITLOOP;

    SPINLOCK( dev->lock );
    /* DIO_Serial_Input_Register: */
    /*   x = DIO_Serial_In_St */
    /* Read in serial digital input on DIO4. */
    buf[k] = DAQSTC_Read_Byte( dev, DIO_Serial_Input_Register );
    SPINUNLOCK( dev->lock );
    
  }

  SPINLOCK( dev->lock );
  /* disable serial DIO: */
  /* DIO_Control_Register: */
  /*   DIO_HW_Serial_Enable = 0 */
  /* Disable hardware-controlled serial digital I/O. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, 0x0000, 0x0200 );
  /* restore dio line configuration. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, dev->dio_config, 0x00FF );
  SPINUNLOCK( dev->lock );

  return count;
}



int dio_write( bp dev, struct file *file, char const *buf, int count )
{
  u8 u;
  int k;
  long j;
  int test;

  /* free serial input buffer: */
  if ( dev->dio_buffer != 0 && dev->dio_buffer_size > 0 ) {
    DPRINT(( "nidaq: dio_write() -> free read buffer\n" ));
    kfree( dev->dio_buffer );
    dev->dio_buffer = 0;
    dev->dio_buffer_size = 0;
  }
  /* allocate serial input buffer: */
  if ( dev->dio_rw & 1 ) {
    DPRINT(( "nidaq: dio_write() -> allocate read buffer\n" ));
    dev->dio_buffer = kmalloc( count, GFP_KERNEL );
    if ( dev->dio_buffer == NULL )
      return -ENOMEM;
    dev->dio_buffer_size = count;
  }

  /* enable serial DIO: */
  /* configure DIO4 for input and DIO0 for output. */
  u = dev->dio_config | 0x01;
  u &= 0xEF;
  DPRINT(( "nidaq: dio_write() -> configure DIO with %x\n", u ));
  SPINLOCK( dev->lock );
  DAQSTC_Masked_Write( dev, DIO_Control_Register, u, 0x00FF );
  /* DIO_Control_Register: */
  /*   DIO_HW_Serial_Enable = 1 */
  /* Enable hardware-controlled serial digital I/O. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, 0x0200, 0x0200 );

  /* Joint_Status_1_Register: DIO_Serial_IO_In_Progress_St */
  test = DAQSTC_Read( dev, Joint_Status_1_Register ) & 0x1000;
  SPINUNLOCK( dev->lock );
  if ( test )
    return -EBUSY;

  WAITLOOP;

  for ( k=0; k<count; k++ ) {
    DPRINT(( "nidaq: dio_write() -> write index %d: %x\n", k, buf[k] ));
  
    SPINLOCK( dev->lock );
    /* DIO_Output_Register: */
    /*   DIO_Serial_Data_Out = x */
    /* take x for serial output. */
    DAQSTC_Masked_Write( dev, DIO_Output_Register, buf[k] << 8, 0xFF00 );
    
    /* DIO_Control_Register: */
    /*   DIO_HW_Serial_Start = 1 */
    /* Start the hardware-controlled serial digital I/O if enabled. */
    DAQSTC_Strobe_Write( dev, DIO_Control_Register, 0x0100 );

    /* Joint_Status_1_Register: DIO_Serial_IO_In_Progress_St */
    test = DAQSTC_Read( dev, Joint_Status_1_Register ) & 0x1000;
    SPINUNLOCK( dev->lock );
    while ( test ) {
      WAITLOOP;
      SPINLOCK( dev->lock );
      /* Joint_Status_1_Register: DIO_Serial_IO_In_Progress_St */
      test = DAQSTC_Read( dev, Joint_Status_1_Register ) & 0x1000;
      SPINUNLOCK( dev->lock );
    }
    WAITLOOP;

    if ( dev->dio_rw & 1 ) {
      SPINLOCK( dev->lock );
      /* DIO_Serial_Input_Register: */
      /*   x = DIO_Serial_In_St */
      /* Read in serial digital input on DIO4. */
      dev->dio_buffer[k] = DAQSTC_Read( dev, DIO_Serial_Input_Register );
      SPINUNLOCK( dev->lock );
      DPRINT(( "nidaq: dio_write() -> read DIO4 %x\n", dev->dio_buffer[k] ));
    }
    
  }

  SPINLOCK( dev->lock );
  /* disable serial DIO: */
  /* DIO_Control_Register: */
  /*   DIO_HW_Serial_Enable = 0 */
  /* Disable hardware-controlled serial digital I/O. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, 0x0000, 0x0200 );
  /* restore dio line configuration. */
  DAQSTC_Masked_Write( dev, DIO_Control_Register, dev->dio_config, 0x00FF );
  SPINUNLOCK( dev->lock );

  return count;
}


int dio_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg )
{
  char *cp = (char *)arg;
  u8 u;

  if ( dev->dio_in_use <= 0 )
    return -EINVAL;

  switch( _IOC_NR( cmd ) ) {

  case _IOC_NR( NIDAQDIOCONFIGURE ):
    u = arg;
    u = ( u & dev->dio_mask ) | ( dev->dio_config & ~dev->dio_mask );
    dev->dio_config = u;
    SPINLOCK( dev->lock );
    /* DIO_Control_Register: */
    /*   DIO_Pins_Dir = dio_config */
    DAQSTC_Masked_Write( dev, DIO_Control_Register, dev->dio_config, 0x00FF );
    SPINUNLOCK( dev->lock );
    return 0;
 
  case _IOC_NR( NIDAQDIOMASK ):
    dev->dio_mask = arg;
    return 0;
 
  case _IOC_NR( NIDAQDIOPAROUT ):
    u = arg;
    u = ( u & dev->dio_mask ) | ( dev->dio_cpy & ~dev->dio_mask );
    dev->dio_cpy = u;
    DPRINT(( "nidaq: dio_ioctl PAROUT -> write %02x\n", u ));
    SPINLOCK( dev->lock );
    /* DIO_Output_Register: */
    /*   DIO_Parallel_Data_Out = u */
    DAQSTC_Masked_Write( dev, DIO_Output_Register, u, 0x00FF );
    SPINUNLOCK( dev->lock );
    return 0;
 
  case _IOC_NR( NIDAQDIOPARIN ):
    SPINLOCK( dev->lock );
    /* DIO_Parallel_Input_Register: */
    /*   u = DIO_Parallel_Data_In_St */
    u = DAQSTC_Read( dev, DIO_Parallel_Input_Register );
    SPINUNLOCK( dev->lock );
    DPRINT(( "nidaq: dio_ioctl PARIN -> read %02x\n", u ));
    if ( copy_to_user( cp, &u, 1 ) )
      return -EFAULT;
    else
      return 0;

  case _IOC_NR( NIDAQDIOCLOCK ):
    SPINLOCK( dev->lock );
    /* DIO_Control_Register: */
    /*   DIO_Software_Serial_Control = 0 (for logic low) or 1 (for logic high) */
    DAQSTC_Masked_Write( dev, DIO_Control_Register, arg > 0 ? 0x0000 : 0x0800, 0x0800 );
    SPINUNLOCK( dev->lock );
    return 0;

  case _IOC_NR( NIDAQDIOTIMEDIV2 ):
    SPINLOCK( dev->lock );
    /* Clock_and_FOUT_Register: */
    /*   DIO_Serial_Out_Divide_By_2 = 0 (do not divide by 2) or 1 (divide by 2) */
    /* Divide the clock used for serial digital I/O timing by 2. */
    DAQSTC_Masked_Write( dev, Clock_and_FOUT_Register, arg > 0 ? 0x2000 : 0x0000, 0x2000 );
    SPINUNLOCK( dev->lock );
    return 0;

  case _IOC_NR( NIDAQDIOTIMEBASE ):
    SPINLOCK( dev->lock );
    /* DIO_Control_Register: */
    /*   DIO_HW_Serial_Timebase = 0 (1.2 ms clock) or 1 (10 ms clock) */
    /* This bit selects the timebase used for the EXTSTROBE/SDCLK signal during hardware-controlled serial digital I/O. */
    DAQSTC_Masked_Write( dev, DIO_Control_Register, arg > 0 ? 0x0400 : 0x0000, 0x0400 );
    SPINUNLOCK( dev->lock );
    return 0;

  }

  return -EINVAL;
}

#undef WAITLOOP
