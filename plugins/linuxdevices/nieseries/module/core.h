#ifndef _CORE_H_
#define _CORE_H_

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/module.h>
#include <asm/uaccess.h>

#ifndef KERNEL_VERSION 
#define KERNEL_VERSION(a,b,c) ((a)*65536+(b)*256+(c)) 
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION( 2, 6, 0 )
#define KERNEL260
#elif LINUX_VERSION_CODE >= KERNEL_VERSION( 2, 4, 0 )
#define KERNEL240
#elif LINUX_VERSION_CODE >= KERNEL_VERSION( 2, 2, 0 )
#define KERNEL220
#else
#error unknown kernel version!
#endif

#if defined(KERNEL220) || defined(KERNEL240)
#include <linux/modversions.h>
#endif

/* sleep_on and wait_on: */
#if defined(KERNEL260)

#include <linux/wait.h>

#define INTERRUPTIBLE_SLEEP_ON( x ) wait_event_interruptible( x, ( 0 ) )

#else

#define INTERRUPTIBLE_SLEEP_ON( x ) interruptible_sleep_on( x )

#endif

/* spin locks: */
#if defined(KERNEL240) || defined(KERNEL260)

#include <linux/spinlock.h>

/* non-interrupt context: */
#define SPINLOCK( x ) spin_lock_irq( &(x) )
#define SPINUNLOCK( x ) spin_unlock_irq( &(x) )
/* interrupt context: */
#define INTSPINLOCK( x ) spin_lock( &(x) )
#define INTSPINUNLOCK( x ) spin_unlock( &(x) )
/* initialize lock: */
#define INITSPINLOCK( x ) spin_lock_init( &(x) )

#else

/* non-interrupt context: */
#define SPINLOCK( x ) cli()
#define SPINUNLOCK( x ) sti()
/* interrupt context: */
#define INTSPINLOCK( x ) cli()
#define INTSPINUNLOCK( x ) sti()
/* initialize lock: */
#define INITSPINLOCK( x )

#endif

#define PRINT( x ) printk x
#define DDPRINT( x )


#include <linux/pci.h>
#include "register.h"
#include "nidaq.h"

#ifndef NIDAQBOARDSDEF
extern nidaq_info nidaq_boards[KNOWN_BOARDS];
#endif

#define MAX_BOARDS 4

#define SUBDEV_AI 0
#define SUBDEV_AO 1
#define SUBDEV_DIO 2
#define SUBDEV_PFI 3

#define PCI_MITE_SIZE 4096
#define PCI_DAQ_SIZE  4096


#define BUFFBLOCKSIZE 2040


typedef struct buffer_chain_struct {
  struct buffer_chain_struct *next;
  u16 buffer[BUFFBLOCKSIZE];
} buffer_chain;

typedef struct misb_struct {
  struct misb_struct *next;
  u16 repeats;          /* number of repeats of the whole signal. */
  u32 interval;         /* number of clocks between two UPDATEs. */
  u8 mute;              /* mute the buffer? */ 
  u8 use;               /* bit 0: parameter are not yet set, bit 1: data are not yet written. */
  buffer_chain *buffer; /* pointer to buffer_chain holding the signal */
  u32 size;             /* total number of elements of the signal. */
} misb;

typedef struct {
  nidaq_info *board;         /* properties of this board. */
  struct pci_dev *pci;       /* the pci device structure. */
  u32 type;                  /* type of the board (NI_PCI, NI_ESER, ...) */
  int number;                /* number of this board. */

  void *mite;                /* addres of the mapped mite memory */
  unsigned long mitestart;
  unsigned long mitelength;
  void *base;                /* addres of the mapped io memory. */
  unsigned long basestart;
  unsigned long baselength;
  unsigned int irq;          /* irq line */
  unsigned int irq_pin;
  spinlock_t lock;           /* a lock */

  u16 stccpy[256];           /* a copy of all DAQ-STC registers. */

  /* variables for misc: */
  u32 time_base;             /* rate in Hz of the time base */

  /* analog trigger: */
  s8 lowvalue;               /* low value for analog trigger = volts*128/10, -10V < volts < 10V */
  s8 highvalue;              /* high value for analog trigger = volts*128/10, -10V < volts < 10V */
  u8 triggermode;            /* 0: low window, 1: high window, 2: middle window, 3: high hysteresis, 6: low hysteresis */
  u8 triggersource;          /* 0: TRIG1, 1: PGIA2 is trigger source. */

  /* variables for analog input: */
  s8 ai_in_use;                     /* 1 if ai is opened. */
  u16 ai_status;
  u8 ai_running;                    /* 1 if ai is running. */
  u8 ai_sleep;                      /* 1 if ai is sleeping. */   
  u8 ai_error;                      /* 1: overflow, 2: overrun */
  wait_queue_head_t ai_wait_queue;
  u8 ai_stop;                       /* 1 if ai has to be stopped at next STOP. */ 

  u16 ai_channels;                  /* number of channels used for ai */
  u32 ai_scans;                     /* number of scans */
  u32 ai_scan_start;                /* scan start interval */
  u32 ai_scan_interval;             /* scan interval */
  u16 ai_sample_start;              /* sample start interval */
  u16 ai_sample_interval;           /* sample interval */
  u32 ai_read_scans;              /* number of processed scans. */
  u32 ai_sc_tcs;                    /* number of SC_TC interrupts. */

  u8 ai_continuous;
  u8 ai_fifo_mode;
  u8 ai_ext_mux;
  u8 ai_ext_gating;
  u8 ai_pretrigger;
  u8 ai_retrigger;         /* retriggerable analog input */
  u8 ai_start1_source;     /* 0: AI_START1_Pulse, 1-10: PFI,0..9>, 11-17: RTSI_TRIGGER<0..6>, 18: G_OUT, 31: logic low */
  u8 ai_start1_polarity;   /* Polarity of START1 trigger: 0: Active high or rising edge, 1: Active low or falling edge. */
  u8 ai_analog_trigger;    /* use internal analog trigger circuitry. */

  buffer_chain *ai_buffer;        /* pointer to the buffer chain. */
  buffer_chain *ai_read_buffer;   /* pointer to the buffer block where data have to be written. */
  buffer_chain *ai_write_buffer;  /* pointer to the data block where data have to be read. */
  u16 ai_read_index;              /* index to the data element that has to be written in the current buffer block. */
  u16 ai_write_index;             /* index to the data element that has to be read in the current buffer block. */
  u16 ai_nbuffer;                 /* number of buffer blocks. */
  s32 ai_nread;                   /* number of data elements that have not yet been read. */
  u32 ai_to_be_read;              /* number of data points that have to be read until wakeup in continuous mode. */

  /* variables for analog output: */
  s8 ao_in_use;                   /* 1 if ao is opened. */
  u16 ao_status;                   
  u8 ao_running;                  /* 1 if ao is running. */
  u8 ao_sleep;                    /* 1 if ao is sleeping. */
  u8 ao_isstaging;                /* 1 if ao is in waveform staging mode. */
  u8 ao_error;                    /* 1: overrun, 2: BC_TC not served in time, 4: START1 before BC_TC acknowledge. */
  wait_queue_head_t ao_wait_queue;
  u8 ao_stop;                     /* 1 if ao has to be stopped. */
  long ao_waitcount;              /* count for 2us wait */

  u16 ao_continuous;
  u16 ao_fifo_mode;
  u8 ao_start1_source;      /* 0: AO_START1_Pulse, 1-10: PFI<0..9>, 11-17: RTSI_TRIGGER<0..6>. 19: internal START1, 31: Logic low. */
                              /* Caution: PFI0 does not work as an external trigger source! */
  u8 ao_start1_polarity;    /* Polarity of START1 trigger: 0: Active high or rising edge, 1: Active low or falling edge. */
  u8 ao_analog_trigger;    /* use internal analog trigger circuitry. */
  u32 ao_bc_tc;
  u8 ao_fifo_interrupt;
  u8 ao_update_interrupt;
  u8 ao_next_load_register;
  s32 ao_ai_scans;

  u16 ao_channel_number;   /* channel used for output. */
  u16 ao_channels;         /* number of output channels used for the signal. */

  u16 ao_repeats;          /* number of repeats of the whole signal. */
  u32 ao_startc;           /* number of clocks for delay from START1 to first UPDATE */
  u32 ao_intervalc;        /* number of clocks between two UPDATEs. */
  u8 ao_staging;           /* waveform staging? */
  u16 ao_mute_repeats; 
  u32 ao_mute_updates;

  buffer_chain *ao_buffer;  /* pointer to buffer_chain holding the signal */
  buffer_chain *ao_current; /* pointer to the current buffer block. */
  u32 ao_buffer_size;       /* total number of elements of the signal. */
  u32 ao_buffer_index;      /* index of the current data element. */
  u32 ao_buffer_write;      /* index of the current data element in the current buffer block. */
  u16 ao_buffer_counter;    /* number of completed buffer cycles. */

  misb *ao_misb;            /* pointer to the misb chain. */
  misb *ao_misb_data;       /* pointer to the misb whose data are currently written. */
  misb *ao_misb_param;      /* pointer to the misb whose parameter have to be set next. */
  misb *ao_last_misb;       /* pointer to the last element in the misb chain. */
  u32  ao_misb_count;       /* number of misb buffer. */

  /* variables for digital input/output: */
  s8 dio_in_use;            /* 1 if dio is opened. */
  u8 dio_cpy;               /* copy of the dio lines. */
  u8 dio_mask;              /* mask for writing on dio lines. */
  u8 dio_config;            /* configuration of dio lines. */
  u8 dio_rw;                /* read only (1), write only (2), or both (3). */
  u8 *dio_buffer;           /* serial input buffer. */
  u16 dio_buffer_size;      /* size of serial input buffer. */
  long dio_waitcount;       /* count for 2us wait */

  /* variables for pfi: */
  s8 pfi_in_use;            /* 1 if dio is opened. */

} board_data, *bp;

extern board_data boards[MAX_BOARDS];


inline static void DAQSTC_Write_Byte( bp dev, u32 regaddr, u8 data )
{
  writew( regaddr, (dev->base) + Window_Address_Register*2 );
  writeb( data, (dev->base) + Window_Data_Write_Register*2 ); 
  dev->stccpy[regaddr] = data;
}


inline static void DAQSTC_Write( bp dev, u32 regaddr, u16 data )
{
  writew( regaddr, (dev->base) + Window_Address_Register*2 );
  writew( data, (dev->base) + Window_Data_Write_Register*2 ); 
  dev->stccpy[regaddr] = data;
}


inline static void DAQSTC_Masked_Write( bp dev, u32 regaddr, u16 data, u16 mask )
{
  unsigned short scp;

  scp = ( dev->stccpy[regaddr] & (~mask)) | (data & mask);
  dev->stccpy[regaddr] = scp;
  writew(  regaddr, (dev->base) + Window_Address_Register*2 );
  writew( scp, (dev->base) + Window_Data_Write_Register*2 ); 
}


inline static void DAQSTC_Strobe_Write( bp dev, u32 regaddr, u16 data )
{
  unsigned short scp;

  scp = dev->stccpy[regaddr] | data;
  writew( regaddr, (dev->base) + Window_Address_Register*2 );
  writew( scp, (dev->base) + Window_Data_Write_Register*2 ); 
}


inline static u8 DAQSTC_Read_Byte( bp dev, u32 regaddr )
{
  writew( regaddr, (dev->base) + Window_Address_Register*2 );
  return readb( (dev->base) + Window_Data_Read_Register*2 ); 
}


inline static u16 DAQSTC_Read( bp dev, u32 regaddr )
{
  writew( regaddr, (dev->base) + Window_Address_Register*2 );
  return readw( (dev->base) + Window_Data_Read_Register*2 ); 
}


/* This function writes to an E-Series board register             */
 
inline static void Board_Write( bp dev, u32 regaddr, u16 data )
{
  writew( data,(dev->base) + regaddr );
}


inline static void Board_Write_Byte( bp dev, u32 regaddr, u8 data )
{
  writeb( data, (dev->base) + regaddr );
}


/* This function reads from an E-Series board register            */

inline static u16 Board_Read( bp dev, u32 regaddr )
{
  return readw( (dev->base) + regaddr );
}


inline static u8 Board_Read_Byte( bp dev, u32 regaddr)
{
  return readb( (dev->base) + regaddr );
}


#endif
