#include "core.h"
#include "misc.h"


#ifdef NIDAQ_DIODEBUG
#define DPRINT( x ) printk x
#else
#define DPRINT( x )
#endif


void pfi_init( bp dev )
{
  DPRINT(( "nidaq: pfi_init()\n" ));
  dev->pfi_in_use = 0;
}


void pfi_cleanup( bp dev )
{
}


int pfi_open( bp dev, struct file *file )
{
  /* pfi already opened: */
  if ( dev->pfi_in_use > 0 )
    return EBUSY;

  dev->pfi_in_use++;

  return 0;
}


void pfi_release( bp dev )
{
  if ( dev->pfi_in_use > 0 ) {
    dev->pfi_in_use = 0;
  }
}


int pfi_ioctl( bp dev, struct file *file, unsigned int cmd, unsigned long arg )
{
  if ( dev->pfi_in_use <= 0 )
    return -EINVAL;

  switch( _IOC_NR( cmd ) ) {

  case _IOC_NR( NIDAQPFIOUT ):
    if ( arg < 0 || arg > 9 )
      return -EINVAL;
    MSC_IO_Pin_Configure( dev, arg, 1 );
    return 0;

  case _IOC_NR( NIDAQPFIIN ):
    if ( arg < 0 || arg > 9 )
      return -EINVAL;
    MSC_IO_Pin_Configure( dev, arg, 0 );
    return 0;

  } 

  return -EINVAL;
}
