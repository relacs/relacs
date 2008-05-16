#include "core.h"
#include "misc.h"
#include "eprom.h"

#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <asm/io.h>


board_data boards[MAX_BOARDS];


/* declare waitqueue: */
static DECLARE_WAIT_QUEUE_HEAD( nidaq_wait_queue );

static int nidaq_open( struct inode *inode, struct file *file );
static int nidaq_release( struct inode *inode, struct file *file );
static ssize_t nidaq_read( struct file *file, char *buffer, 
			   size_t length, loff_t *offset );
static ssize_t nidaq_write( struct file *file, const char *buffer, 
			    size_t length, loff_t *offset );
static int nidaq_ioctl( struct inode *inode, struct file *file, 
			unsigned int cmd, unsigned long arg );
static int nidaq_interrupt(int irq, void *dev_id, struct pt_regs *regs);


void ai_init( bp dev );
void ai_cleanup( bp dev );
int ai_open( bp dev, struct file *file );
void ai_release( bp dev );
int ai_read( bp dev, struct file *file, char *buf, int count );
int ai_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg );
void ai_interrupt( bp dev );

void ao_init( bp dev );
void ao_cleanup( bp dev );
int ao_open( bp dev, struct file *file );
void ao_release( bp dev );
int ao_write( bp dev, struct file *file, char const *buf, int count );
int ao_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg );
void ao_interrupt( bp dev );

void dio_init( bp dev );
void dio_cleanup( bp dev );
int dio_open( bp dev, struct file *file );
void dio_release( bp dev );
int dio_read( bp dev, struct file *file, char *buf, int count );
int dio_write( bp dev, struct file *file, char const *buf, int count );
int dio_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg );

void pfi_init( bp dev );
void pfi_cleanup( bp dev );
int pfi_open( bp dev, struct file *file );
void pfi_release( bp dev );
int pfi_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg );


struct file_operations nidaq_fops = 
{ 
  open   : nidaq_open,
  release: nidaq_release,
  read   : nidaq_read, 
  write  : nidaq_write,
  ioctl  : nidaq_ioctl,
};


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jan Benda");
MODULE_DESCRIPTION("Driver for NI E-Series boards used by OEL");


static int nidaq_open( struct inode *inode, struct file *file ) 
{ 
  int minor, subdev, r;
  bp dev;

  DPRINT(( "nidaq: nidaq_open\n" ));

  /* get device and subdevice: */
  minor = MINOR( inode->i_rdev );
  dev = boards + (minor>>4); 
  subdev = minor&0xf;

  /* no board exist for this device: */
  if ( !dev->type )
    return -EINVAL;

  misc_open( dev, file );

  switch ( subdev ) {

  case SUBDEV_AI:
    r = ai_open( dev, file );
    if ( r > 0 )
      return -r;
    break;

  case SUBDEV_AO:
    r = ao_open( dev, file );
    if ( r > 0 )
      return -r;
    break;

  case SUBDEV_DIO:
    r = dio_open( dev, file );
    if ( r > 0 )
      return -r;
    break;

  case SUBDEV_PFI:
    r = pfi_open( dev, file );
    if ( r > 0 )
      return -r;
    break;
  }

#ifndef NIDAQ_NO_USE_COUNT
  MOD_INC_USE_COUNT;
#endif

  return 0;
}


static int nidaq_release( struct inode *inode, struct file *file ) 
{
  int minor, subdev;
  bp dev;

  DPRINT(( "nidaq: nidaq_release\n" ));

  /* get device and subdevice: */
  minor = MINOR( inode->i_rdev );
  dev = boards + (minor>>4); 
  subdev = minor&0xf;

  misc_release( dev );
  
  switch ( subdev ) {
  case SUBDEV_AI:
    ai_release( dev );
    break;
  case SUBDEV_AO:
    ao_release( dev );
    break;
  case SUBDEV_DIO:
    dio_release( dev );
    break;
  case SUBDEV_PFI:
    pfi_release( dev );
    break;
  }

#ifndef NIDAQ_NO_USE_COUNT
  MOD_DEC_USE_COUNT;
#endif

  return 0;
}


static ssize_t nidaq_read( struct file *file, char *buffer, 
			   size_t length, loff_t *offset ) 
{
  int minor, subdev;
  bp dev;

  /* get device and subdevice: */
  minor = MINOR( file->f_dentry->d_inode->i_rdev );
  dev = boards + (minor>>4); 
  subdev = minor&0xf;

  DPRINT(( "nidaq: nidaq_read with minor %d\n", minor ));

  switch ( subdev ) {
  case SUBDEV_AI:
    return ai_read( dev, file, buffer, length );
  case SUBDEV_DIO:
    return dio_read( dev, file, buffer, length );
  }
  return -EBADRQC;
}


static ssize_t nidaq_write( struct file *file, const char *buffer, 
			    size_t length, loff_t *offset )
{
  int minor, subdev;
  bp dev;

  /* get device and subdevice: */
  minor = MINOR( file->f_dentry->d_inode->i_rdev );
  dev = boards + (minor>>4); 
  subdev = minor&0xf;

  DPRINT(( "nidaq: nidaq_write with minor %d\n", minor ));

  switch ( subdev ) {
  case SUBDEV_AO:
    return ao_write( dev, file, buffer, length );
  case SUBDEV_DIO:
    return dio_write( dev, file, buffer, length );
  }
  return -EBADRQC;
}


static int nidaq_ioctl( struct inode *inode, struct file *file, 
			unsigned int cmd, unsigned long arg )
{
  int minor, subdev;
  bp dev;

  if ( _IOC_TYPE( cmd ) != NIDAQ_MAJOR ) {
    printk( "nidaq: ioctl wrong driver code\n" );
    return -EINVAL;
  }

  /* get device and subdevice: */
  minor = MINOR( inode->i_rdev );
  dev = boards + (minor>>4); 
  subdev = minor&0xf;

  DPRINT(( "nidaq: nidaq_ioctl with minor %d\n", minor ));

  if ( _IOC_NR( cmd ) < 50 )
    return misc_ioctl( dev, file, cmd, arg );

  switch ( subdev ) {
  case SUBDEV_AI:
    return ai_ioctl( dev, file, cmd, arg );
  case SUBDEV_AO:
    return ao_ioctl( dev, file, cmd, arg );
  case SUBDEV_DIO:
    return dio_ioctl( dev, file, cmd, arg );
  case SUBDEV_PFI:
    return pfi_ioctl( dev, file, cmd, arg );
  }
  return -EINVAL;
}


static int init_device( bp dev )
{
  int i, irq_type;

  DPRINT(( "nidaq: init_device number %d\n", dev->number ));

  if ( dev->irq == 0 )
    dev->irq = -1;
  if ( dev->irq != -1 ) {
    irq_type = SA_SHIRQ;
    irq_type |= SA_INTERRUPT;
    dev->irq_pin = 0;
    if( request_irq( dev->irq, nidaq_interrupt, irq_type, NIDAQ_NAME, dev ) ) {
      printk( "nidaq: unable to register int no %u\n", dev->irq );
      return -1;
    }
  }

  if ( dev->type & NI_ESER ) {
    for ( i=0; i<256; i++ )
      dev->stccpy[i] = 0;

    /*
    initialize board subsystems (AI, AO, DIO, GPCT) here.
     */
    
    // DAQSTC_Strobe_Write(dev, Joint_Reset_Register,0x000F);  why not???
    // Software_Reset ???
    DAQSTC_Masked_Write( dev, Interrupt_A_Enable_Register, 0, 0xFFFF );  
    DAQSTC_Masked_Write( dev, Interrupt_B_Enable_Register, 0, 0xFFFF);
    DAQSTC_Masked_Write( dev, Interrupt_A_Ack_Register, 0xFFFF, 0xFFFF );
    DAQSTC_Masked_Write( dev, Interrupt_B_Ack_Register, 0xFFFF, 0xFFFF );
    writel( 0x0303, dev->mite + 0x0a );
    if ( dev->irq != -1 ) {
      MSC_IrqGroupEnable( dev, 0 ); /* Enable IRQ group A */
      MSC_IrqGroupEnable( dev, 1 ); /* Enable IRQ group B */ 
    }
    loadcalibration( dev );
  }

  return 0;
}


void init_pci( void )
{
  struct pci_dev *pcidev;
  nidaq_info *board;
  int bi, brd;
  unsigned int deviceid;
  bp dev;
  unsigned long addr;
  unsigned long offset;
  int shifted_flag = 0;
  
  DPRINT(( "nidaq: init_pci\n" ));

  /* first known board: */
  bi = 0;
  board = &nidaq_boards[bi];
  deviceid = board->deviceid;
  
  /* check for valid deviceid: */
  do {
    if ( deviceid == 0 )  {
      bi++;
      if ( bi < KNOWN_BOARDS )
	{
	  board = &nidaq_boards[bi];
	  deviceid = board->deviceid;
	}
      continue;
    }
    
    /* search for a specific board: */
    /*    DPRINT(( "nidaq: about to check for device ID 0x%x\n", deviceid ));*/
    pcidev = NULL;
    while ( (pcidev = pci_find_device( VENDORID, deviceid, pcidev ) ) ) {
      
      /* find an empty space */
      for ( brd=0; brd < MAX_BOARDS; brd++ )
	if ( boards[brd].type == 0 )
	  break;
      
      /* too many boards: */
      if ( brd == MAX_BOARDS ) {
	printk( "nidaq: can't alloc space for NI PCI type=0x%x. Too many boards!\n", board->type );
	continue;
      }
      
      DPRINT(( "nidaq: PCI board of type 0x%x found\n", board->type ));
      dev = &boards[brd];
      dev->board = board;
      dev->type = board->type;
      dev->pci = pcidev;
      
      /* the following is from comedi: */
      if ( pci_enable_device( pcidev ) ) {
	printk( "nidaq: error enabling mite\n" );
	continue;
      }
      pci_set_master( pcidev );
      
      /* setup mite address: */	
      addr = pcidev->resource[0].start;
      offset = addr & ~PAGE_MASK;
      dev->mitestart = addr & PAGE_MASK;
      dev->mitelength = PCI_MITE_SIZE + offset;
      /* request io memory region: */
      if ( request_mem_region( dev->mitestart, dev->mitelength, "mite" ) == NULL ) {
	printk( "nidaq: io memory region already in use\n" );
	continue;
      }
      dev->mite = ioremap( dev->mitestart, dev->mitelength ) + offset;
      DPRINT(( "nidaq: MITE: 0x%08lx mapped to %p\n", addr, dev->mite ));
      
      /* set io memory address: */	
      addr = pcidev->resource[1].start;
      offset = addr & ~PAGE_MASK;
      dev->basestart = addr & PAGE_MASK;
      dev->baselength = PCI_DAQ_SIZE + offset;
      /* request io memory region: */
      if ( request_mem_region( dev->basestart, dev->baselength, "mite (daq)" ) == NULL ) {
	printk( "nidaq: io memory region already in use\n" );
	continue;
      }
      dev->base = ioremap( dev->basestart, dev->baselength ) + offset;
      DPRINT(( "nidaq: DAQ: 0x%08lx mapped to %p\n", addr, dev->base ));
      /* XXX don't know what the 0xc0 and 0x80 mean */
      /* It must be here for the driver to work though */
      writel( addr | 0x80 , dev->mite + 0xc0 );
      
      /* set irq line: */	
      dev->irq = pcidev->irq;
      
      if ( init_device( dev ) < 0 ) {
	dev->type = 0; 
	continue;
      } 
      
      printk( "nidaq: board %d: %s at memory=%p irq=%d\n", 
	      brd, board->name, dev->mite, dev->irq );
    } /* while (pcidev = pci_find_device ...) */
    
    if ( shifted_flag == 0 ) { /* just checked a 16-bit NI PCI ID */
      deviceid = (board->deviceid) >> 4;
      shifted_flag = 1;
    }
    else  {
      bi++;
      if ( bi < KNOWN_BOARDS )
	{
	  board = &nidaq_boards[bi];
	    deviceid = board->deviceid;
	}
      shifted_flag = 0;
    }
    
  } while ( bi < KNOWN_BOARDS ); /* do/while loop throug known boards */
}


int __init init_module( void )
{
  int ret;
  int brd;

  DPRINT(( "\n\n\nnidaq: init_module\n" ));

  /* register driver: */
  ret = register_chrdev( NIDAQ_MAJOR, NIDAQ_NAME, &nidaq_fops );
  if ( ret < 0 ) {
    printk( "nidaq: unable to register %s device: %d\n", 
	    NIDAQ_NAME, ret );
    return ret;
  } 
  else 
    printk( "nidaq: %s version %s (%s) activated with major %d\n", 
	    NIDAQ_NAME, NIDAQ_VERSION, NIDAQ_DATE, NIDAQ_MAJOR );

  /* clear board list: */
  for ( brd=0; brd<MAX_BOARDS; brd++ ) {
    boards[brd].type = 0;
    boards[brd].number = brd;
  }

  init_pci();

  /* init board list: */
  for ( brd=0; brd<MAX_BOARDS; brd++ ) 
    if ( boards[brd].type != 0 ) {
      INITSPINLOCK( boards[brd].lock );
      misc_init( &boards[brd] );
      ai_init( &boards[brd] );
      ao_init( &boards[brd] );
      dio_init( &boards[brd] );
      pfi_init( &boards[brd] );
    }

  return 0;
}


void __exit cleanup_module( void )
{
  bp dev;
  int brd;
  int ret;

  DPRINT(( "nidaq: cleanup_module\n" ));
  for( brd=0; brd < MAX_BOARDS; brd++ ) {
    dev = &boards[brd];
    if ( dev->type == 0 )
      continue;
    DPRINT(( "nidaq: device %d is of type 0x%x...\n", brd, dev->type ));

    /* clean up sub devices: */
    misc_cleanup( &boards[brd] );
    ai_cleanup( &boards[brd] );
    ao_cleanup( &boards[brd] );
    dio_cleanup( &boards[brd] );

    if ( dev->irq != -1 )
      free_irq( dev->irq, dev );

    iounmap( dev->mite );
    release_mem_region( dev->mitestart, dev->mitelength );

    iounmap( dev->base );
    release_mem_region( dev->basestart, dev->baselength);

    pci_disable_device( dev->pci );
    DPRINT(( "nidaq: board %d uninstalled\n", brd ));
  }

  ret = unregister_chrdev( NIDAQ_MAJOR, NIDAQ_NAME );
  if ( ret < 0 ) 
    printk( "nidaq: unable to unregister driver\n" );
}


static int nidaq_interrupt( int irq, void *dev_id, struct pt_regs *regs )
{
  bp dev;
  int i;
  int r = IRQ_NONE;

  dev = dev_id;
  INTSPINLOCK( dev->lock );

  /* check for reason of interupt: */
  for( i=0; i<100000; i++ ) /* why this loop? */
    {

      dev->ai_status = DAQSTC_Read( dev, AI_Status_1_Register );
      if ( dev->ai_status & 0x8000 )       /* it was a group A interrupt */
	{
	  if ( dev->ai_status & 0x7ff2 )
	    ai_interrupt( dev );
	  /*
	  if ( dev->ai_status & 0x0004 ) 
	    gpct_interrupt( dev, 0 );                      G0 Gate int */
	  r = IRQ_HANDLED;
	  break;
	}

      dev->ao_status = DAQSTC_Read( dev, AO_Status_1_Register );
      if ( dev->ao_status & 0x8000 )       /* it was a group B interrupt */
	{
	  if ( dev->ao_status & 0x7ff2 )
	    ao_interrupt( dev );
	  /*
	  if( dev->ao_status & 0x0004 )
	    gpct_interrupt( dev, 1 );                      G1 Gate int */
	  r = IRQ_HANDLED;
	  break;
	}

    }

  INTSPINUNLOCK( dev->lock );

  return r;
}

