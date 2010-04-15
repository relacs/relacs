#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/spinlock.h>

#include <rtai.h>
#include <rtai_fifos.h>
#include <rtai_sched.h>
#include <rtai_shm.h>
#include <rtai_math.h>

#include "rtmodule.h"

MODULE_LICENSE( "GPL" );


///////////////////////////////////////////////////////////////////////////////
// *** TYPE DEFINITIONS ***
///////////////////////////////////////////////////////////////////////////////

// subdevice acquisition errors:
#define E_COMEDI    -1
#define E_NODATA    -2
#define E_UNDERRUN  -3
#define E_OVERFLOW  -4


//* DAQ-DEVICES:

struct deviceT {
  comedi_t *devP;
  char name[DEV_NAME_MAXLEN+1];
};

struct subdeviceT {
  int subdev;
  enum subdevTypes type;
  int devID;
  
  unsigned int fifo;

  unsigned int sampleSize;
  
  int asyncMode;
  
  unsigned int chanN;
  struct chanT *chanlist;

  unsigned int frequency;
  long delay;
  long duration;           // => relative to index of dynclamp-Task
  int continuous;
  int startsource;

  int used;
  int prepared;
  int running;
  int pending;
  int error;               // E_COMEDI, E_NODATA, ...
};

struct chanT {
  comedi_t *devP;
  int subdev;
  unsigned int chan;
  int aref;
  int rangeIndex;
  comedi_insn insn;
  lsampl_t lsample;
  struct converterT converter;
  float scale;
  unsigned int fifo;
  float voltage;
  float prevvoltage;
  int trigger;
  float alevel;
};


struct triggerT {
  int enabled;
  char devname[DEV_NAME_MAXLEN+1];
  int subdev;
  unsigned int chan;
  float alevel;
};


//* RTAI TASK:

struct dynClampTaskT {
  RT_TASK rtTask;
  unsigned int periodLengthNs;
  unsigned int reqFreq;
  unsigned int setFreq;
  unsigned long duration;
  int continuous;
  int running;
  unsigned long loopCnt;
  long aoIndex;
};



///////////////////////////////////////////////////////////////////////////////
// *** GLOBAL VARIABLES ***
///////////////////////////////////////////////////////////////////////////////

//* DAQ-DEVICES:

struct deviceT device[MAXDEV];
int deviceN = 0;

struct subdeviceT subdev[MAXSUBDEV];
int subdevN = 0;

int reqTraceSubdevID = -1;
int reqCloseSubdevID = -1;

struct triggerT trigger;


//* RTAI TASK:

struct dynClampTaskT dynClampTask;

char *moduleName = "/dev/dynclamp";


// for debug:

char *iocNames[RTMODULE_IOC_MAXNR] = {
  "dummy",
  "IOC_GET_SUBDEV_ID", "IOC_OPEN_SUBDEV", "IOC_CHANLIST", "IOC_COMEDI_CMD", "IOC_SYNC_CMD", 
  "IOC_START_SUBDEV", "IOC_CHK_RUNNING", "IOC_REQ_READ", "IOC_REQ_WRITE", "IOC_REQ_CLOSE",
  "IOC_STOP_SUBDEV", "IOC_RELEASE_SUBDEV", "IOC_SET_TRIGGER", "IOC_UNSET_TRIGGER",
  "IOC_GET_TRACE_INFO", "IOC_SET_TRACE_CHANNEL", "IOC_GETRATE", "IOC_GETLOOPCNT",
  "IOC_GETAOINDEX" 
};


///////////////////////////////////////////////////////////////////////////////
// *** PROTOTYPES ***
///////////////////////////////////////////////////////////////////////////////

int init_rt_task( void );
void cleanup_rt_task( void );
int rtmodule_open( struct inode *devFile, struct file *fModule );
int rtmodule_close( struct inode *devFile, struct file *fModule );
int rtmodule_ioctl( struct inode *devFile, struct file *fModule, 
		    unsigned int cmd, unsigned long arg );
static struct file_operations fops = {                     
  .owner= THIS_MODULE,
  .ioctl= rtmodule_ioctl,
  .open= rtmodule_open, 
  .release= rtmodule_close,
};



///////////////////////////////////////////////////////////////////////////////
// *** HELPER FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

/*
This function throws a "SSE register return with SSE disabled" error on compiltion!
It is inlined in the code to avoid this problem.
static inline float sample_to_value( struct chanT *pChan, lsampl_t sample )
{
  double value = 0.0;
  double term = 1.0;
  unsigned i;
  for ( i=0; i <= pChan->converter.order; ++i ) {
    value += pChan->converter.coefficients[i] * term;
    term *= sample - pChan->converter.expansion_origin;
  }
  return value*pChan->scale;
}
*/

static inline lsampl_t value_to_sample( struct chanT *pChan, float value )
{
  double sample = 0.0;
  double term = 1.0;
  unsigned i;
  value *= pChan->scale;
  for ( i=0; i <= pChan->converter.order; ++i ) {
    sample += pChan->converter.coefficients[i] * term;
    term *= value - pChan->converter.expansion_origin;
  }
  return (lsampl_t)sample;
}


void init_globals( void ) {
  deviceN = 0;
  subdevN = 0;
  reqCloseSubdevID = -1;
  reqTraceSubdevID = -1;
  memset( device, 0, sizeof(device) );
  memset( subdev, 0, sizeof(subdev) );
  memset( &dynClampTask, 0, sizeof(struct dynClampTaskT ) );
}



///////////////////////////////////////////////////////////////////////////////
// *** DAQ FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

int getSubdevID( void )
{
  int i = 0;
  //* find free slot in subdev[]:
  for ( i = 0; i < subdevN && subdev[i].used; i++ );
  if ( i == subdevN ) {
    if ( subdevN >= MAXSUBDEV ) {
      ERROR_MSG( "getSubdevID ERROR: number of requested subdevices exceeds MAXSUBDEV!\n" );
      return -1;
    }
    subdevN++;
  }
  memset( &(subdev[i]), 0, sizeof(struct subdeviceT) );
  subdev[i].used = 1;
  subdev[i].subdev = -1;
  subdev[i].devID = -1;
  subdev[i].sampleSize = sizeof(float);
  return i;
}


int openComediDevice( struct deviceIOCT *deviceIOC )
{
  int iS, retVal;
  int i = 0;
  int iDev = -1;
  int openDev = 1;
  int justOpened = 0;

  // scan device list for either the opened device or a free slot:
  for ( i = 0; i < deviceN; i++ ) {
    if ( device[i].devP ) {
      if ( strcmp( deviceIOC->devicename, device[i].name ) == 0 ) {
	DEBUG_MSG( "comediOpenDevice: device %s is already opened...", 
    		   device[i].name );
	iDev = i;
	openDev = 0;
	break;
      }
    }
    else {
      if ( iDev < 0 && !device[i].devP ) {
	iDev = i;
	break;
      }
    }
  }

  if ( i == deviceN ) {
    iDev = deviceN;
    if ( deviceN >= MAXDEV ) {
      ERROR_MSG( "comediOpenDevice ERROR: number of requested devices exceeds MAXDEV!\n" );
      return -1;
    }
    deviceN++;
  }

  DEBUG_MSG( "openComediDevice: found device slot..\n" );

  if ( openDev ) {
    // open comedi device:
    device[iDev].devP = comedi_open( deviceIOC->devicename );
    if ( !device[iDev].devP ) {
      ERROR_MSG( "comediOpenDevice: device %s could not be opened!\n",
		 deviceIOC->devicename );
      comedi_perror( "rtmodule: comedi_open" );    
      return -1;
    }
    justOpened = 1;
    DEBUG_MSG( "openComediDevice: opened device %s\n",  deviceIOC->devicename );
  }

  // lock requested subdevice:
  if ( deviceIOC->subdev >= comedi_get_n_subdevices( device[iDev].devP ) ||
       comedi_lock( device[iDev].devP, deviceIOC->subdev ) != 0 ) {
    ERROR_MSG( "comediOpenDevice: Subdevice %i on device %s could not be locked!\n",
	       deviceIOC->subdev, device[iDev].name );
    // locking failed => close just opened comedi device:
    if ( justOpened ) {
      if ( comedi_close( device[iDev].devP ) < 0 )
      	WARN_MSG( "comediOpenDevice WARNING: closing of device %s failed!\n",
		  device[iDev].name );
      else
	DEBUG_MSG( "comediOpenDevice: Closing of device %s was successful!\n",
		   device[iDev].name );
      device[iDev].devP = NULL;
    }    
    return -1;
  }

  // initialize device structure:
  strncpy( device[iDev].name, deviceIOC->devicename, DEV_NAME_MAXLEN );

  DEBUG_MSG( "openComediDevice: locked subdevice %i on device %s\n", 
             deviceIOC->subdev, device[iDev].name );
  
  // initialize subdevice structure:
  iS = deviceIOC->subdevID;
  subdev[iS].subdev = deviceIOC->subdev;
  subdev[iS].devID= iDev;
  subdev[iS].type = deviceIOC->subdevType;
  subdev[iS].delay = -1; 
  subdev[iS].duration = -1;
  subdev[iS].startsource = 0;

  // create FIFO for subdevice:
  subdev[iS].fifo = iS;
  retVal = rtf_create( subdev[iS].fifo, FIFO_SIZE );
  if ( retVal ) {
    ERROR_MSG( "openComediDevice ERROR: Creating FIFO with %d bytes buffer failed for subdevice %i, device %s\n",
               FIFO_SIZE, iS, device[subdev[iS].devID].name );
    return -1;
  }
  else
    DEBUG_MSG( "openComediDevice: Created FIFO with %d bytes buffer size for subdevice %i, device %s\n",
               FIFO_SIZE, iS, device[subdev[iS].devID].name );

  // pass FIFO properties to user:
  deviceIOC->fifoIndex = subdev[iS].fifo;
  deviceIOC->fifoSize = FIFO_SIZE;

  return 0;
}


int loadChanlist( struct chanlistIOCT *chanlistIOC )
{
  int iS = chanlistIOC->subdevID;
  int iD = subdev[iS].devID;
  int iC;
  int trig = 0;

  if ( subdev[iS].subdev < 0 || !subdev[iS].used ) {
    ERROR_MSG( "loadChanlist ERROR: First open an appropriate device and subdevice. Chanlist not loaded!\n" );
    return -1;
  }

  if ( chanlistIOC->chanlistN > MAXCHANLIST ) {
    ERROR_MSG( "loadChanlist ERROR: Invalid chanlist length for Subdevice %i on device %s. Chanlist not loaded!\n",
	       iS, device[subdev[iS].devID].name );
    return -1;
  }

  if ( subdev[iS].chanlist ) {
    vfree( subdev[iS].chanlist );
    subdev[iS].chanlist = 0;
  }

  // check for trigger:
  if ( trigger.enabled &&
       strcmp( device[subdev[iS].devID].name, trigger.devname ) == 0 ) {
    if ( trigger.subdev >= 0 ) {
      if ( subdev[iS].subdev == trigger.subdev )
	trig = 1;
    }
    else {
      if ( subdev[iS].type == SUBDEV_IN )
	trig = 1;
    }
    DEBUG_MSG( "rtmodule: checked for trigger on subdevice %d: %d\n", subdev[iS].subdev, trig );
  }

  // create and initialize chanlist for subdevice:
  subdev[iS].chanlist = vmalloc( chanlistIOC->chanlistN*sizeof(struct chanT) );
  if ( !subdev[iS].chanlist ) {
    ERROR_MSG( "loadChanlist ERROR: Memory allocation for Subdevice %i on device %s. Chanlist not loaded!\n",
	       iS, device[subdev[iS].devID].name );
    return -1;
  }
  subdev[iS].chanN = chanlistIOC->chanlistN;

  for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {
    subdev[iS].chanlist[iC].devP = device[iD].devP;
    subdev[iS].chanlist[iC].subdev = subdev[iS].subdev;
    subdev[iS].chanlist[iC].chan = CR_CHAN( chanlistIOC->chanlist[iC] );
    subdev[iS].chanlist[iC].aref = CR_AREF( chanlistIOC->chanlist[iC] );
    subdev[iS].chanlist[iC].rangeIndex = CR_RANGE( chanlistIOC->chanlist[iC] );
    subdev[iS].chanlist[iC].lsample = 0;
    memset( &subdev[iS].chanlist[iC].insn, 0, sizeof(comedi_insn) );
    if ( subdev[iS].type == SUBDEV_IN )
      subdev[iS].chanlist[iC].insn.insn = INSN_READ;
    else
      subdev[iS].chanlist[iC].insn.insn = INSN_WRITE;
    subdev[iS].chanlist[iC].insn.n = 1;
    subdev[iS].chanlist[iC].insn.data = &subdev[iS].chanlist[iC].lsample;
    subdev[iS].chanlist[iC].insn.subdev = subdev[iS].subdev;
    subdev[iS].chanlist[iC].insn.chanspec = chanlistIOC->chanlist[iC];
    memcpy( &subdev[iS].chanlist[iC].converter, &chanlistIOC->conversionlist[iC], sizeof(struct converterT) );
    subdev[iS].chanlist[iC].scale = chanlistIOC->scalelist[iC];
    subdev[iS].chanlist[iC].fifo = subdev[iS].fifo;
    subdev[iS].chanlist[iC].voltage = 0.0;
    subdev[iS].chanlist[iC].prevvoltage = 0.0;
    if ( trig && subdev[iS].chanlist[iC].chan == trigger.chan ) {
    DEBUG_MSG( "rtmodule: added trigger to channel %d id %d on subdevice %d with level %d\n", subdev[iS].chanlist[iC].chan, iC, subdev[iS].subdev, (int)(100.0*trigger.alevel) );
      subdev[iS].chanlist[iC].trigger = 1;
      subdev[iS].chanlist[iC].alevel = trigger.alevel;
    }
    else {
      subdev[iS].chanlist[iC].trigger = 0;
      subdev[iS].chanlist[iC].alevel = 0.0;
    }
  }

  return 0;
}


int loadSyncCmd( struct syncCmdIOCT *syncCmdIOC )
{
  int iS = syncCmdIOC->subdevID;

  DEBUG_MSG( "loadSyncCmd: running %d\n", subdev[iS].running );

  if ( subdev[iS].subdev < 0 || !subdev[iS].used ) {
    ERROR_MSG( "loadSyncCmd ERROR: First open an appropriate device and subdevice. Sync-command not loaded!\n" );
    return -EFAULT;
  }
  if ( !subdev[iS].chanlist ) {
    ERROR_MSG( "loadSyncCmd ERROR: First load Chanlist for Subdevice %i on device %s. Sync-command not loaded!\n",
	       iS, device[subdev[iS].devID].name );
    return -EFAULT;
  }
  if ( syncCmdIOC->frequency > MAX_FREQUENCY ) {
    ERROR_MSG( "loadSyncCmd ERROR: Requested frequency is above MAX_FREQUENCY (%d Hz). Sync-command not loaded!\n",
	       MAX_FREQUENCY );
    return -EINVAL;
  }

  // initialize sampling parameters for subdevice:
  subdev[iS].frequency = syncCmdIOC->frequency > 0 ? syncCmdIOC->frequency : dynClampTask.setFreq;
  subdev[iS].delay = syncCmdIOC->delay;
  subdev[iS].duration = syncCmdIOC->duration;
  subdev[iS].continuous = syncCmdIOC->continuous;
  subdev[iS].startsource = syncCmdIOC->startsource;

  DEBUG_MSG( "loadSyncCmd: loaded %d samples with startsource %d for subdevice %d\n", subdev[iS].duration, subdev[iS].startsource, iS );

  // test requested sampling-rate and set frequency for dynamic clamp task:
  if ( !dynClampTask.reqFreq ) {
    dynClampTask.reqFreq = subdev[iS].frequency;
  }
  else {
    if ( dynClampTask.setFreq != subdev[iS].frequency ) {
      ERROR_MSG( "loadSyncCmd ERROR: Requested frequency %u Hz of subdevice %i on device %s is inconsistent to frequency %u Hz of other subdevice. Sync-command not loaded!\n",
		 subdev[iS].frequency, iS, device[subdev[iS].devID].name, dynClampTask.setFreq );
      return -EINVAL;
    }
  }

  subdev[iS].prepared = 1;
  return 0;
}


int startSubdevice( int iS )
{ 
  int retVal = 0;
  unsigned long firstLoopCnt, tmpDelay, tmpDuration;

  if ( !subdev[iS].prepared || subdev[iS].running ) {
    ERROR_MSG( "startSubdevice ERROR:  Subdevice ID %i on device %s either not prepared or already running.\n",
	       iS, device[subdev[iS].devID].name );
    return -EBUSY;
  }

  subdev[iS].pending = 1;
  if ( !dynClampTask.running ) {
    dynClampTask.aoIndex = 0;
    dynClampTask.reqFreq = subdev[iS].frequency;

    // start dynamic clamp task: 
    retVal = init_rt_task();
    if ( retVal < 0 ) {
      subdev[iS].running = 0;
      return -ENOMEM;
    }
  }

  subdev[iS].running = 1;

  return 0;
}


int stopSubdevice( int iS )
{ 
  int i;

  if ( !subdev[iS].running )
    return 0;
  subdev[iS].running = 0;

  // if all subdevices stopped => halt dynclamp task:
  for ( i = 0; i < subdevN; i++ )
    if ( subdev[i].running )
      return 0;
  cleanup_rt_task();
  return 0;
}


void releaseSubdevice( int iS )
{
  int iD = subdev[iS].devID;
  int i;

  if ( !subdev[iS].used || subdev[iS].subdev < 0 ) {
    ERROR_MSG( "releaseSubdevice ERROR: Subdevice with ID %d not in use!\n", 
	       iS );
    return;
  }

  // stop subdevice:
  if ( subdev[iS].running )
    stopSubdevice( iS );
  
  // unlock subdevice:
  if ( device[iD].devP && comedi_unlock( device[iD].devP, subdev[iS].subdev ) < 0 )
    WARN_MSG( "releaseSubdevice WARNING: unlocking of subdevice %s failed!\n",
	      device[iD].name );
  else
    DEBUG_MSG( "releaseSubdevice: Unlocking of subdevice %s was successful!\n",
	       device[iD].name );

  if ( subdev[iS].chanlist ) {
    vfree( subdev[iS].chanlist );
    subdev[iS].chanlist = 0;
  }

  // delete FIFO and reset subdevice structure:
  rtf_destroy( subdev[iS].fifo );
  memset( &(subdev[iS]), 0, sizeof(struct subdeviceT) );
  if ( iS == subdevN - 1 )
    subdevN--;
  subdev[iS].devID = -1;
 
  // check if comedi device for subdevice is still in use:
  for ( i = 0; i < subdevN; i++ ) {
    if ( subdev[i].devID == iD )
      // device is still used by another subdevice => leave here:
      return;
  }

  // close comedi device:
  DEBUG_MSG( "YYYYYYY releaseSubdevice: released device for last subdev-ID %d\n", iS );
  if ( device[iD].devP && comedi_close( device[iD].devP ) < 0 )
    WARN_MSG( "releaseSubdevice WARNING: closing of device %s failed!\n",
	      device[iD].name );
  else
    DEBUG_MSG( "releaseSubdevice: Closing of device %s was successful!\n",
	       device[iD].name );

  // reset device structure:
  memset( &(device[iD]), 0, sizeof(struct deviceT) );
  if ( iD == deviceN-1 )
    deviceN--;
}


int setAnalogTrigger( struct triggerIOCT *triggerIOC )
{
  // disable trigger:
  trigger.enabled = 0;

  // setup trigger parameter:
  strcpy( trigger.devname, triggerIOC->devname );
  trigger.subdev = triggerIOC->subdev;
  trigger.chan = triggerIOC->channel;
  trigger.alevel = triggerIOC->alevel;

  DEBUG_MSG( "rtDynClamp: setup trigger for channel %d on device %s\n",
	     trigger.chan, trigger.devname );

  // enable trigger:
  trigger.enabled = 1;
  return 0;
}


int unsetAnalogTrigger( struct triggerIOCT *triggerIOC )
{
  int iS, iC;

  // disable trigger:
  trigger.enabled = 0;
  for ( iS = 0; iS < subdevN; iS++ ) {
    for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {
      subdev[iS].chanlist[iC].trigger = 0;
    }
  }

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// *** REAL-TIME TASKS *** 
///////////////////////////////////////////////////////////////////////////////


/*! Dynamic clamp task */
void rtDynClamp( long dummy )
{
  int retVal;
  int iS, iC;
  int subdevRunning = 1;
  unsigned long readCnt = 0;
  unsigned long fifoPutCnt = 0;
  struct chanT *pChan;
  unsigned ci;
  double term;
  int triggerevs[5] = { 1, 0, 0, 0, 0 };
  int prevtriggerevs[5] = { 0, 0, 0, 0, 0 };

  DEBUG_MSG( "rtDynClamp: starting dynamic clamp loop at %u Hz\n", 
	     1000000000/dynClampTask.periodLengthNs );

  dynClampTask.loopCnt = 0;
  dynClampTask.aoIndex = -1;
  dynClampTask.running = 1;

  while( subdevRunning ) {
    
    subdevRunning = 0;


    //******** WRITE TO ANALOG OUTPUT: *******************************************/
      //****************************************************************************/
      // TODO: implement efficiently as frequently updated chanT-list of used traces
      for ( iS = 0; iS < subdevN; iS++ )
	if ( subdev[iS].running && subdev[iS].type == SUBDEV_OUT ) {

          if ( subdev[iS].pending ) {
	    DEBUG_MSG( "REALTIMELOOP PENDING AO subdev=%d, startsrc=%d, prevtriger1=%d, triger1=%d, pv=%d, v=%d\n",
		       iS, subdev[iS].startsource, prevtriggerevs[1], triggerevs[1],
		       (int)(100.0*subdev[0].chanlist[2].prevvoltage), (int)(100.0*subdev[0].chanlist[2].voltage) );
	    if ( triggerevs[subdev[iS].startsource] &&
		 ! prevtriggerevs[subdev[iS].startsource] ) {
	      DEBUG_MSG( "REALTIMELOOP PENDING AO SETUP duration=%lu, loopCnt=%lu\n", subdev[iS].duration, dynClampTask.loopCnt );
	      subdev[iS].delay = dynClampTask.loopCnt + subdev[iS].delay; 
	      subdev[iS].duration = subdev[iS].delay + subdev[iS].duration;
	      dynClampTask.aoIndex = subdev[iS].delay;
	      subdev[iS].pending = 0;
	      DEBUG_MSG( "REALTIMELOOP PENDING AO STARTED duration=%lu delay=%lu, loopCnt=%lu\n", subdev[iS].duration, subdev[iS].delay, dynClampTask.loopCnt );
	    }
	    else
	      continue;
	  }

          // check duration:
          if ( !subdev[iS].continuous &&
	       subdev[iS].duration <= dynClampTask.loopCnt ) {
	    DEBUG_MSG( "rtDynClamp: finished subdevice %d at loop %lu\n", iS, dynClampTask.loopCnt );
	    rtf_reset( subdev[iS].fifo );
            subdev[iS].running = 0;
      	    continue;
          }

          subdevRunning = 1;

	  if ( dynClampTask.loopCnt >= subdev[iS].delay ) {

	    // for every chan...
	    for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {

	      pChan = &subdev[iS].chanlist[iC];
	      
	      // get data from FIFO:
	      retVal = rtf_get( pChan->fifo, &pChan->voltage, sizeof(float) );
	      if ( retVal != sizeof(float) ) {
		ERROR_MSG( "rtDynClamp: Data buffer underrun for AO subdevice ID %d at loopCnt %lu, duration=%lu\n",
			   iS, dynClampTask.loopCnt );
		if ( retVal == EINVAL ) {
		  ERROR_MSG( "rtDynClamp: No open FIFO for subdevice ID %d at loopCnt %lu\n",
			     iS, dynClampTask.loopCnt );
		  dynClampTask.running = 0;
		  dynClampTask.duration = 0;
		  return;
		}
		subdev[iS].error = E_UNDERRUN;
		subdev[iS].running = 0;
		continue;
	      }
	      
	      // write out Sample:
	      pChan->lsample = value_to_sample( pChan, pChan->voltage );
	      retVal = comedi_do_insn( pChan->devP, &pChan->insn );
	      if ( retVal < 1 ) {
		subdev[iS].running = 0;
		if ( retVal < 0 ) {
		  comedi_perror( "rtmodule: rtDynClamp: comedi_data_write" );
		  subdev[iS].error = E_COMEDI;
		  subdev[iS].running = 0;
		  //spin_unlock( &subdev[iS].bData.spinlock );
		  continue;
		}
		subdev[iS].error = E_NODATA;
		DEBUG_MSG( "rtDynClamp: failed to write data to subdevice ID %d channel %d at loopCnt %lu\n",
			   iS, iC, dynClampTask.loopCnt, subdev[iS].duration );
	      }
	      
	    } // end of chan loop

	  }

	} // end of device loop

    

      //******** SLEEP FOR NEURON TO REACT TO GIVEN OUTPUT: ************************/
	//****************************************************************************/
	//* PROBLEM: rt_sleep is timed using jiffies only (granularity = 1msec)
	    //* int retValSleep = rt_sleep( nano2count( INJECT_RECORD_DELAY ) );
      //      rt_busy_sleep( INJECT_RECORD_DELAY ); // TODO: just default


	//******** READ FROM ANALOG INPUT: *******************************************/
	//****************************************************************************/
	// TODO: implement efficiently as frequently updated chanT-list of used traces
	for ( iS = 0; iS < subdevN; iS++ )
	  if ( !subdev[iS].asyncMode && subdev[iS].running && 
	      subdev[iS].type == SUBDEV_IN ) {

	    if ( subdev[iS].pending ) {
	      if ( triggerevs[subdev[iS].startsource] &&
		   ! prevtriggerevs[subdev[iS].startsource] ) {
		subdev[iS].delay = dynClampTask.loopCnt + subdev[iS].delay; 
		subdev[iS].duration = subdev[iS].delay + subdev[iS].duration;
		subdev[iS].pending = 0;
	      }
	      else
		continue;
	    }
          
	    // check duration:
	    if ( !subdev[iS].continuous &&
		subdev[iS].duration <= dynClampTask.loopCnt ) {
	      subdev[iS].running = 0;
	    }
	    subdevRunning = 1;

	    // for every chan...
	    for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {

	      pChan = &subdev[iS].chanlist[iC];

	      // previous sample:
	      pChan->prevvoltage = pChan->voltage;

	      // acquire sample:
	      retVal = comedi_do_insn( pChan->devP, &pChan->insn );
	      if ( retVal < 0 ) {
		subdev[iS].running = 0;
		comedi_perror( "rtmodule: rtDynClamp: comedi_data_read" );
		subdev[iS].error = E_COMEDI;
		DEBUG_MSG( "rtDynClamp: failed to read from subdevice ID %d channel %d at loopCnt %lu\n",
			   iS, iC, dynClampTask.loopCnt );
		continue;
	      }
	      // convert to voltage:
	      pChan->voltage = 0.0;
	      term = 1.0;
	      for ( ci=0; ci <= pChan->converter.order; ++ci ) {
		pChan->voltage += pChan->converter.coefficients[ci] * term;
		term *= pChan->lsample - pChan->converter.expansion_origin;
	      }
	      pChan->voltage *= pChan->scale;
	      // pChan->voltage = sample_to_value( &subdev[iS].chanlist[iC], pChan->lsample );
	      // write to FIFO:
	      retVal = rtf_put( pChan->fifo, &pChan->voltage, sizeof(float) );
	      fifoPutCnt++;
	      if ( retVal != sizeof(float) ) {
		if ( retVal == EINVAL ) {
		  ERROR_MSG( "rtDynClamp: No open FIFO for subdevice ID %d at loopCnt %lu\n",
			     iS, dynClampTask.loopCnt );
		  dynClampTask.running = 0;
		  dynClampTask.duration = 0;
		  return;
		}
		subdev[iS].error = E_OVERFLOW;
		ERROR_MSG( "rtDynClamp: Data buffer overflow for AI subdevice ID %d at loopCnt %lu\n",
			   iS, dynClampTask.loopCnt );
		subdev[iS].running = 0;
		continue;
	      }

	      // trigger:
	      if ( pChan->trigger ) {
		prevtriggerevs[1] = triggerevs[1];
		if ( pChan->voltage > pChan->alevel && pChan->prevvoltage <= pChan->alevel ) {
		  triggerevs[1] = 1;
		}
		else if ( pChan->voltage < pChan->alevel && pChan->prevvoltage >= pChan->alevel ) {
		  triggerevs[1] = 0;
		}
	      }

	    } // end of chan loop
	    readCnt++; // FOR DEBUG
	  } // end of device loop

	  //****************************************************************************/
	  dynClampTask.loopCnt++;

	  //    start = rt_get_cpu_time_ns();
	  rt_task_wait_period();

  } // END OF DYNCLAMP LOOP

    
  dynClampTask.running = 0;
  dynClampTask.duration = 0;

  DEBUG_MSG( "rtDynClamp: left dynamic clamp loop after %lu cycles\n",
	     dynClampTask.loopCnt );

}



///////////////////////////////////////////////////////////////////////////////
// *** RTAI FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////


// TODO: seperate into init and start?
int init_rt_task( void )
{
  int stackSize = 20000;
  int priority;
  int usesFPU = 1;
  void* signal = NULL;
  int dummy = 23;
  int retVal;
  RTIME periodTicks;

  DEBUG_MSG( "init_rt_task: Trying to initialize dynamic clamp RTAI task...\n" );

  //* test if dynamic clamp frequency is valid:
  if ( dynClampTask.reqFreq <= 0 || dynClampTask.reqFreq > MAX_FREQUENCY )
    ERROR_MSG( "init_rt_task ERROR: %dHz -> invalid dynamic clamp frequency. Valid range is 1 .. %dHz\n", 
	       dynClampTask.reqFreq, MAX_FREQUENCY );

  //* initializing rt-task for dynamic clamp with high priority:
  priority = 1;
  retVal = rt_task_init( &dynClampTask.rtTask, rtDynClamp, dummy, stackSize, 
			 priority, usesFPU, signal );
  if ( retVal ) {
    ERROR_MSG( "init_rt_task ERROR: failed to initialize real-time task for dynamic clamp! stacksize was set to %d bytes.\n", 
	       stackSize );
    return -1;
  }
  DEBUG_MSG( "init_rt_task: Initialized dynamic clamp RTAI task. Trying to make it periodic...\n" );

    //* START rt-task for dynamic clamp as periodic:
  periodTicks = start_rt_timer( nano2count( 1000000000/dynClampTask.reqFreq ) );  
  if ( rt_task_make_periodic( &dynClampTask.rtTask, rt_get_time(), periodTicks ) 
      != 0 ) {
    printk( "init_rt_task ERROR: failed to start periodic real-time task for data acquisition! loading of module failed!\n" );
    return -3;
  }
  dynClampTask.periodLengthNs = count2nano( periodTicks );
  dynClampTask.setFreq = 1000000000 / dynClampTask.periodLengthNs;
  INFO_MSG( "init_rt_task: periodic task successfully started... requested freq: %d , accepted freq: ~%u (period=%uns)\n", 
	    dynClampTask.reqFreq, dynClampTask.setFreq, dynClampTask.periodLengthNs );

  // For now, the DynClampTask shall always run until any subdev is stopped:
  dynClampTask.continuous = 1;

  return 0;
}

// TODO: add stop_rt_task?
void cleanup_rt_task( void )
{
  stop_rt_timer();
  DEBUG_MSG( "cleanup_rt_task: stopped periodic task\n" );

  rt_task_delete( &dynClampTask.rtTask );
  memset( &dynClampTask, 0, sizeof(struct dynClampTaskT) );
}





///////////////////////////////////////////////////////////////////////////////
// *** IOCTL ***
///////////////////////////////////////////////////////////////////////////////

int rtmodule_ioctl( struct inode *devFile, struct file *fModule, 
		    unsigned int cmd, unsigned long arg )
{
  static struct deviceIOCT deviceIOC;
  static struct chanlistIOCT chanlistIOC;
  static struct syncCmdIOCT syncCmdIOC;
  static struct triggerIOCT triggerIOC;

  int tmp, subdevID;
  int retVal;
  unsigned long luTmp;

  if (_IOC_TYPE(cmd) != RTMODULE_MAJOR) return -ENOTTY;
  if (_IOC_NR(cmd) > RTMODULE_IOC_MAXNR) return -ENOTTY;

  DEBUG_MSG( "ioctl: user triggered ioctl %d %s\n",_IOC_NR( cmd ), iocNames[_IOC_NR( cmd )] );

  switch( cmd ) {
    

    // ******** GIVE INFORMATION TO USER SPACE: ***********************************
    
  case IOC_GETRATE:
    tmp = dynClampTask.setFreq;
    retVal = put_user( tmp, (int __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;

  case IOC_GETAOINDEX:
    luTmp = dynClampTask.aoIndex;
    if ( luTmp < 0 )
      return -ENOSPC;
    retVal = put_user( luTmp, (unsigned long __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;

  case IOC_GETLOOPCNT:
    luTmp = dynClampTask.loopCnt;
    if ( luTmp < 0 )
      return -ENOSPC;
    retVal = put_user( luTmp, (unsigned long __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;


    // ******* SET UP COMEDI: ***********************************************

  case IOC_GET_SUBDEV_ID:
    tmp = getSubdevID();
    if ( tmp < 0 )
      return -ENOSPC;
    retVal = put_user( tmp, (int __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;

  case IOC_OPEN_SUBDEV:
    retVal = copy_from_user( &deviceIOC, (void __user *)arg, sizeof(struct deviceIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to deviceIOCT-struct!\n" );
      return -EFAULT;
    }
    if ( deviceIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in deviceIOCT-struct!\n" );
      return -EFAULT;
    }
    retVal = openComediDevice( &deviceIOC );
    if ( retVal != 0 )
      return -EFAULT;
    retVal = copy_to_user( (void __user *)arg, &deviceIOC, sizeof(struct deviceIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to deviceIOCT-struct!\n" );
      return -EFAULT;
    }
    return 0;

  case IOC_CHANLIST:
    retVal = copy_from_user( &chanlistIOC, (void __user *)arg, sizeof(struct chanlistIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to chanlistIOCT-struct!\n" );
      return -EFAULT;
    }
    if ( chanlistIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in chanlistIOCT-struct!\n" );
      return -EFAULT;
    }
    retVal = loadChanlist( &chanlistIOC );
    return retVal == 0 ? 0 : -EFAULT;

  case IOC_SYNC_CMD:
    retVal = copy_from_user( &syncCmdIOC, (void __user *)arg, sizeof(struct syncCmdIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to syncCmdIOCT-struct!\n" );
      return -EFAULT;
    }
    if ( syncCmdIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in syncCmdIOCT-struct!\n" );
      return -EFAULT;
    }
    retVal = loadSyncCmd( &syncCmdIOC );
    return retVal;


  case IOC_GET_TRACE_INFO:
    return -ERANGE; // Ende der Liste signalisieren


  case IOC_SET_TRACE_CHANNEL:
    return 0;


  case IOC_START_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for start-query!" );
      return -EFAULT;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for start-query!\n" );
      return -EFAULT;
    }
    retVal = startSubdevice( subdevID );
    return retVal;


  case IOC_CHK_RUNNING:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for running-query!" );
      return -EFAULT;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for running-query!\n" );
      return -EFAULT;
    }
    tmp = subdev[subdevID].running;
    DEBUG_MSG( "rtmodule_ioctl: running = %d for subdevID %d\n", tmp, subdevID );
    retVal = put_user( tmp, (int __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;


  case IOC_REQ_CLOSE:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for close-request!" );
      return -EFAULT;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for close-request!\n" );
      return -EFAULT;
    }
    if ( reqCloseSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_CLOSE ERROR: Another close-request in progress!\n" );
      return -EAGAIN;
    }
    reqCloseSubdevID = subdevID;
    return 0;

  case IOC_REQ_READ: // Noch wichtig fuer tracename-List?
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for read-request!" );
      return -EFAULT;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for read-request!\n" );
      return -EFAULT;
    }
    if ( reqTraceSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ ERROR: Another read-request in progress! (reqTraceSubdevID=%d)\n", reqTraceSubdevID );
      return -EAGAIN;
    }
    ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ: Requested Read\n" );
    reqTraceSubdevID = subdevID;
    return 0;


  case IOC_STOP_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for stop-query!" );
      return -EFAULT;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for stop-query!\n" );
      return -EFAULT;
    }
    retVal = stopSubdevice( subdevID );
    DEBUG_MSG( "rtmodule_ioctl: stopSubdevice returned %u\n", retVal );
    return retVal == 0 ? 0 : -EFAULT;


  case IOC_RELEASE_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for release-query!" );
      return -EFAULT;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for release-query!\n" );
      return -EFAULT;
    }
    releaseSubdevice( subdevID );
    return 0;


    // ******* Trigger: ***********************************************


  case IOC_SET_TRIGGER:
    retVal = copy_from_user( &triggerIOC, (void __user *)arg, sizeof(struct triggerIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to triggerIOCT-struct!\n" );
      return -EFAULT;
    }
    return setAnalogTrigger( &triggerIOC );

  case IOC_UNSET_TRIGGER:
    retVal = copy_from_user( &triggerIOC, (void __user *)arg, sizeof(struct triggerIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to triggerIOCT-struct!\n" );
      return -EFAULT;
    }
    return unsetAnalogTrigger( &triggerIOC );


  }


  ERROR_MSG( "rtmodule_ioctl ERROR - Invalid IOCTL!\n" );

  return -EINVAL;
}



///////////////////////////////////////////////////////////////////////////////
// *** DRIVER FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

int init_module( void )
{
  // register module device file:
  // TODO: adapt to kernel 2.6 convention (see char-device chapter in Linux device drivers 3)
  if ( register_chrdev( RTMODULE_MAJOR, moduleName, &fops ) != 0 ) {
    WARN_MSG( "init_module: couldn't register driver's major number\n" );
    // return -1;
  }
  INFO_MSG( "module_init: dynamic clamp module %s loaded\n", moduleName );
  DEBUG_MSG( "module_init: debugging enabled\n" );

  comedi_loglevel( 3 ); 

  // initialize global variables:
  init_globals();

  return 0;
}


int rtmodule_open( struct inode *devFile, struct file *fModule )
{
  DEBUG_MSG( "open: user opened device file\n" );
  
  return 0;
}


int rtmodule_close( struct inode *devFile, struct file *fModule )
{
  int iS;
  // no subdevice specified? => stop & close all subdevices & comedi-devices:
  if ( reqCloseSubdevID < 0 ) {
    DEBUG_MSG( "close: no IOC_REQ_CLOSE request received - closing all subdevices...\n" );
    for ( iS = 0; iS < subdevN; iS++ ) {
      if ( stopSubdevice( iS ) )
        WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
      releaseSubdevice( iS );
    }
    init_globals();
    return 0;
  }

  // stop & close specified subdevice (and device):
  if ( stopSubdevice( reqCloseSubdevID ) )
    WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", reqCloseSubdevID );
  releaseSubdevice( reqCloseSubdevID );

  if ( deviceN == 0 )
    init_globals();
  reqCloseSubdevID = -1;

  DEBUG_MSG( "close: user closed device file\n" );
  return 0;
}


void cleanup_module( void )
{
  int iS;
  INFO_MSG( "cleanup_module: dynamic clamp module %s unloaded\n", moduleName );

  // stop and release all subdevices & comedi-devices:
  for ( iS = 0; iS < subdevN; iS++ ) {
    if ( stopSubdevice( iS ) )
      WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
    releaseSubdevice( iS );
  }

  // unregister module device file:
  unregister_chrdev( RTMODULE_MAJOR, moduleName );
}

