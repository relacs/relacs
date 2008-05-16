#include "core.h"
#include "misc.h"


#ifdef NIDAQ_MISCDEBUG
#define DPRINT( x ) printk x
#else
#define DPRINT( x )
#endif


#define  CALDAC00 0x00
#define  CALDAC11 0x0B
#define  CALDAC12 0x0C

void Write_Serial_DAC( bp dev, unsigned char address, unsigned char uData );
void Write_One_pattern( bp dev );
void Write_Zero_pattern( bp dev );


void misc_init( bp dev )
{
  dev->lowvalue = 0;
  dev->highvalue = 0;
  dev->triggermode = 0;
  dev->triggersource = 0;
}


void misc_cleanup( bp dev )
{
}


int misc_open( bp dev, struct file *file )
{
  MSC_Clock_Configure( dev );             
  Analog_Trigger_Control( dev, 0 );
  return 0;
}


void misc_release( bp dev )
{
}


int misc_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg )
{
  unsigned long r = 0;

  switch( _IOC_NR( cmd ) ) {

  case _IOC_NR( NIDAQINFO ):
    r = copy_to_user( (void *)arg, dev->board, sizeof( nidaq_info ) );
    return r == 0 ? 0 : -EFAULT;

  case _IOC_NR( NIDAQTIMEBASE ):
    return dev->time_base;

  case _IOC_NR( NIDAQLOWVALUE ):
    dev->lowvalue = arg;
    DPRINT(( "nidaq: misc_ioctl -> low value = %d\n", dev->lowvalue ));
    return 0;

  case _IOC_NR( NIDAQHIGHVALUE ):
    dev->highvalue = arg;
    DPRINT(( "nidaq: misc_ioctl -> high value = %d\n", dev->highvalue ));
    return 0;

  case _IOC_NR( NIDAQTRIGGERMODE ):
    if ( arg < 0 || ( arg > 3 && arg != 6 ) )
      return -EINVAL;
    dev->triggermode = arg;
    return 0;

  case _IOC_NR( NIDAQTRIGGERSOURCE ):
    dev->triggersource = ( arg > 0 );
    return 0;

  }

  return -EINVAL;
}


void MSC_Clock_Configure( bp dev )
{ 
  /* Clock_and_FOUT_Register: */
  /*   Slow_Internal_Timebase = 1 */
  /*   Slow_Internal_Time_Divide_By_2 = 0 */
  /*   Clock_To_Board = 1 */
  /*   Clock_To_Board_Divide_By_2 = 0 */
  DAQSTC_Masked_Write( dev, Clock_and_FOUT_Register, 0x0900, 0x1B00 );
  dev->time_base = 20000000;
}  


void MSC_IO_Pin_Configure( bp dev, int pin, int output )
{
  int mask;

  if ( pin < 0 || pin > 9 )
    return;

  mask = 1 << pin;
  /* IO_Bidirection_Pin_Register: */
  /*   BD_x_Pin_Dir = output */
  DAQSTC_Masked_Write( dev, IO_Bidirection_Pin_Register, 
		       output ? mask : 0x0000, mask );

  /*
    If (BD_7_Pin_Dir is 1) then 
    AI_START_Output_Select = 0 (output AD START) or 1 (output AD SCAN_IN_PROG);
  */
}


void MSC_IrqGroupEnable( bp dev, int grp ) /* 0=A, 1=B */
{

  /* schedule() introduce delays which prevent (some) PXI systems to hang.
     I've tested no such system so I can't say which of them do it :-) */

  if ( dev->irq == -1 ) 
    return ; /* don't try to enable interrupts */

  schedule();
  /* Interrupt_control_Register: */
  /*   Interrupt_Output_Polarity = 1 */
  DAQSTC_Masked_Write( dev, Interrupt_Control_Register, 0x0001, 0x0001 );

  if ( grp == 0 ) { 
    /* irq grp A */
    /* Interrupt_Control_Register: */
    /*   Interrupt_A_Output_Select = irq_pin */
    schedule();
    DAQSTC_Masked_Write( dev, Interrupt_Control_Register, 
			 (dev->irq_pin<<8), 0x0700 );
    /* Interrupt_Control_Register: */
    /*   Interrupt_A_Enable = irq_enable (1) */
    schedule();
    DAQSTC_Masked_Write( dev, Interrupt_Control_Register,
			 0x0800, 0x0800 );
  } 
  else { 
    /* IRQ group B */
    schedule();
    DAQSTC_Masked_Write( dev, Interrupt_Control_Register,
			 (dev->irq_pin<<12), 0x7000 );
    schedule();
    DAQSTC_Masked_Write( dev, Interrupt_Control_Register,
			 0x8000,0x8000 );
  }
}


void Analog_Trigger_Control( bp dev, int enable )
{
  if ( ! ( dev->board->type == NI_PCI_MIO_16E_1 ||
	   dev->board->type == NI_PCI_MIO_16E_4 ||
	   dev->board->type == NI_PCI_6071E ) ) {
    if ( enable > 0 )
      printk( "nidaq: Analog_Trigger_Control -> board %s not supported!\n", 
	      dev->board->name );
    return;
  }
    
  if ( enable ) {
    DPRINT(( "nidaq: Analog_Trigger_Control -> enable analog trigger: mode=%d, source=%d\n", 
	     dev->triggermode, dev->triggersource ));
    /* Analog_Trigger_Etc_Register: */
    /*   Analog_Trigger_Drive = 0 */
    /*   Analog_Trigger_Enable = 1 */
    /*   Analog_Trigger_Mode = triggermode */
    /*     Enable the analog triggering circuitry of the DAQ-STC. */
    DAQSTC_Masked_Write( dev, Analog_Trigger_Etc_Register,
			 ( dev->triggermode & 0x7 ) + 0x0008, 0x001F );

    /* Mis_Command_Register: */
    /*  Int/Ext Trigger = triggersource */
    Board_Write_Byte( dev, Misc_Command, dev->triggersource ? 0x80 : 0x00 );
    
    if ( !dev->triggersource ) {
      /* set PFI0-Pin to input */
      MSC_IO_Pin_Configure( dev, 0, 0 );
    }

    /* set low limit: */
    /* CALDAC11 = lowvalue */
    Write_Serial_DAC( dev, CALDAC11, 0xff/2 - dev->lowvalue );

    /* CALDAC00 = lowvalue */
    Write_Serial_DAC( dev, CALDAC00, 0xff/2 - dev->lowvalue );

    /* set high limit: */
    /* CALDAC12 = highvalue */
    Write_Serial_DAC( dev, CALDAC12, 0xff/2 - dev->highvalue );
    
  }
  else {
    DPRINT(( "nidaq: Analog_Trigger_Control -> disable analog trigger\n" ));
    /* Analog_Trigger_Etc_Register: */
    /*   Analog_Trigger_Enable = 0 */
    /*     Disable the analog triggering circuitry of the DAQ-STC. */
    DAQSTC_Masked_Write( dev, Analog_Trigger_Etc_Register, 0x0000, 0x001F );
  }

}


void Write_Serial_DAC( bp dev, unsigned char address, unsigned char uData )
/* Call this function to write accurate data in the serial DAC */
{ 
  unsigned char Temp=0x01;

  DPRINT(( "nidaq: Write_Serial_DAC -> write %x at %x\n", uData, address ));
    
  /* writing address to serial DAC: */
  do {
    
    /* if the bit value is a 1, write  pattern  0x01 then 0x03: */
    if ( (Temp & address) > 0 )
      Write_One_pattern( dev );

    /* if the bit value is an 0, write pattern 0x00, then 0x02: */
    else
      Write_Zero_pattern( dev );
     
    Temp *= 0x02;
    
  }  while ( Temp <= 0x08 );
     
   
  /* writing data to serial DAC : */
  Temp=0x80;
  
  do { 
     
    /* if the bit value is a 1, write  pattern  0x01 then 0x03: */
    if ( (Temp & uData) > 0 )
      Write_One_pattern( dev );

    /* if the bit value is an 0, write  pattern  0x00 then 0x02: */
    else
      Write_Zero_pattern( dev );
    
    Temp /= 0x02;
    
  }  while ( Temp <= 0x01 );
  
  
  /* write the last bit pattern to end the write sequence: */

  /* Serial_Command_Register: */
  /*   SerClk = 0 */
  /*   SerData = 0 */
  /*   SerDacld = 1 */
  Board_Write_Byte( dev, Serial_Command, 0x80 ); 
  
  /* Serial_Command_Register: */
  /*   SerClk = 0 */
  /*   SerData = 0 */
  /*   SerDacld = 1 */
  Board_Write_Byte( dev, Serial_Command, 0x80 ); 
  
}    
     
        
void Write_One_pattern( bp dev )
{   
  /* Serial_Command_Register: */
  /*   SerClk = 0 */
  /*   SerData = 1 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x01 );
    
  /* Serial_Command_Register: */
  /*   SerClk = 0 */
  /*   SerData = 1 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x01 );
  
  /* Serial_Command_Register: */
  /*   SerClk = 1 */
  /*   SerData = 1 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x03 );
  
  /* Serial_Command_Register: */
  /*   SerClk = 1 */
  /*   SerData = 1 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x03 );
}       


void Write_Zero_pattern( bp dev )
{
  /* Serial_Command_Register: */
  /*   SerClk = 0 */
  /*   SerData = 0 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x00 );
  
  /* Serial_Command_Register: */
  /*   SerClk = 0 */
  /*   SerData = 0 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x00 );
  
  /* Serial_Command_Register: */
  /*   SerClk = 1 */
  /*   SerData = 0 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x02 );
  
  /* Serial_Command_Register: */
  /*   SerClk = 1 */
  /*   SerData = 0 */
  /*   SerDacld = 0 */
  Board_Write_Byte( dev, Serial_Command, 0x02 );
}
