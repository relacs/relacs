#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/mutex.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>

#include <rtai.h>
#include <rtai_fifos.h>
#include <rtai_sched.h>
#include <rtai_shm.h>

#include "moduledef.h"

#ifdef ENABLE_COMPUTATION
#include <rtai_math.h>
#endif

MODULE_LICENSE( "GPL" );


///////////////////////////////////////////////////////////////////////////////
// *** TYPE DEFINITIONS ***
///////////////////////////////////////////////////////////////////////////////

// subdevice acquisition errors:
#define E_COMEDI    -1
#define E_NODATA    -2
#define E_UNDERRUN  -3
#define E_OVERFLOW  -4


// DAQ-DEVICES:

struct deviceT {
  comedi_t *devP;
  char name[DEV_NAME_MAXLEN+1];
};

struct subdeviceT {
  int subdev;
  int userSubdevIndex;
  enum subdevTypes type;
  int devID;
  
  unsigned int fifo;

  unsigned int sampleSize;
  
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
  int isParamChan;
  int modelIndex;
  int isUsed;
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


// RTAI TASK:

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

struct mutex mutex;

// DAQ-DEVICES:

struct deviceT device[MAXDEV];
int deviceN = 0;

struct subdeviceT subdev[MAXSUBDEV];
int subdevN = 0;

int reqTraceSubdevID = -1;
int reqCloseSubdevID = -1;

struct triggerT trigger;

#ifdef ENABLE_COMPUTATION
int traceIndex = 0;
int chanIndex = 0;
#endif

// RTAI TASK:

struct dynClampTaskT dynClampTask;

char *moduleName = "/dev/dynclamp";

// TTL pulse generation:
#ifdef ENABLE_TTLPULSE
comedi_t *ttlStartWriteDevice[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_insn *ttlStartWriteInsn[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_t *ttlEndWriteDevice[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_insn *ttlEndWriteInsn[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_t *ttlStartReadDevice[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_insn *ttlStartReadInsn[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_t *ttlEndReadDevice[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_insn *ttlEndReadInsn[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_t *ttlStartAODevice[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_insn *ttlStartAOInsn[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_t *ttlEndAODevice[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };
comedi_insn *ttlEndAOInsn[MAXTTLPULSES] = { 0, 0, 0, 0, 0 };

comedi_t **ttlDevices[MAXTTLPULSETYPES];
comedi_insn **ttlInsns[MAXTTLPULSETYPES];

lsampl_t ttlLow = 0;
lsampl_t ttlHigh = 1;
#endif

// for debug:

char *iocNames[RTMODULE_IOC_MAXNR] = {
  "dummy",

  "IOC_GET_SUBDEV_ID", "IOC_OPEN_SUBDEV", "IOC_CHANLIST", "IOC_COMEDI_CMD",
  "IOC_SYNC_CMD", "IOC_START_SUBDEV", "IOC_CHK_RUNNING", "IOC_REQ_READ",
  "IOC_REQ_WRITE", "IOC_REQ_CLOSE", "IOC_STOP_SUBDEV", "IOC_RELEASE_SUBDEV",
  "IOC_DIO_CMD", "IOC_SET_TRIGGER", "IOC_UNSET_TRIGGER", "IOC_GET_TRACE_INFO",
  "IOC_SET_TRACE_CHANNEL", "IOC_GETRATE", "IOC_GETLOOPCNT", "IOC_GETAOINDEX" 
};


///////////////////////////////////////////////////////////////////////////////
// *** PROTOTYPES ***
///////////////////////////////////////////////////////////////////////////////

int init_rt_task( void );
void cleanup_rt_task( void );
int rtmodule_open( struct inode *devFile, struct file *fModule );
int rtmodule_close( struct inode *devFile, struct file *fModule );
#ifdef HAVE_UNLOCKED_IOCTL
long rtmodule_unlocked_ioctl( struct file *fModule, unsigned int cmd,
			      unsigned long arg );
#else
int rtmodule_ioctl( struct inode *devFile, struct file *fModule, 
		    unsigned int cmd, unsigned long arg );
#endif
static struct file_operations fops = {                     
  .owner = THIS_MODULE,
#ifdef HAVE_UNLOCKED_IOCTL
  .unlocked_ioctl = rtmodule_unlocked_ioctl,
#else
  .ioctl = rtmodule_ioctl,
#endif
  .open = rtmodule_open, 
  .release = rtmodule_close,
};



///////////////////////////////////////////////////////////////////////////////
// *** MODEL INCLUDE ***
///////////////////////////////////////////////////////////////////////////////
#ifdef ENABLE_COMPUTATION
#include "model.c"
#endif

///////////////////////////////////////////////////////////////////////////////
// *** HELPER FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

static inline void sample_to_value( struct chanT *pChan )
{
  double term = 1.0;
  double sample = pChan->lsample - pChan->converter.expansion_origin;
  unsigned i;

  pChan->voltage = 0.0;
  for ( i=0; i <= pChan->converter.order; ++i ) {
    pChan->voltage += pChan->converter.coefficients[i] * term;
    term *= sample;
  }
  pChan->voltage *= pChan->scale;
}


static inline void value_to_sample( struct chanT *pChan, float value )
{
  double sample = 0.0;
  double term = 1.0;
  unsigned i;
  value *= pChan->scale;
  value -= pChan->converter.expansion_origin;
  for ( i=0; i <= pChan->converter.order; ++i ) {
    sample += pChan->converter.coefficients[i] * term;
    term *= value;
  }
  pChan->lsample = (lsampl_t)sample;
}


void init_globals( void )
{
  deviceN = 0;
  subdevN = 0;
  reqCloseSubdevID = -1;
  reqTraceSubdevID = -1;
#ifdef ENABLE_COMPUTATION
  traceIndex = 0;
  chanIndex = 0;
#endif
  memset( device, 0, sizeof(device) );
  memset( subdev, 0, sizeof(subdev) );
  memset( &dynClampTask, 0, sizeof(struct dynClampTaskT ) );

#ifdef ENABLE_TTLPULSE
  ttlDevices[0] = ttlStartWriteDevice;
  ttlDevices[1] = ttlEndWriteDevice;
  ttlDevices[2] = ttlStartReadDevice;
  ttlDevices[3] = ttlEndReadDevice;
  ttlDevices[4] = ttlStartAODevice;
  ttlDevices[5] = ttlEndAODevice;
  ttlInsns[0] = ttlStartWriteInsn;
  ttlInsns[1] = ttlEndWriteInsn;
  ttlInsns[2] = ttlStartReadInsn;
  ttlInsns[3] = ttlEndReadInsn;
  ttlInsns[4] = ttlStartAOInsn;
  ttlInsns[5] = ttlEndAOInsn;
#endif
}


///////////////////////////////////////////////////////////////////////////////
// *** DAQ FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

int getSubdevID( void )
{
  int i = 0;
  // find free slot in subdev[]:
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
      comedi_perror( "dynclampmodule: comedi_open" );    
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
  subdev[iS].userSubdevIndex = -1;
  subdev[iS].devID= iDev;
  subdev[iS].type = deviceIOC->subdevType;
  subdev[iS].delay = -1; 
  subdev[iS].duration = -1;
  subdev[iS].startsource = 0;
  subdev[iS].chanN = 0;
  subdev[iS].chanlist = 0;
  subdev[iS].running = 0;

  if ( subdev[iS].type == SUBDEV_IN || subdev[iS].type == SUBDEV_OUT ) {

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
  }
  else {
    deviceIOC->fifoIndex = 0;
    deviceIOC->fifoSize = 0;
  }

  return 0;
}


int loadChanlist( struct chanlistIOCT *chanlistIOC )
{
  int iS = chanlistIOC->subdevID;
  int iD = subdev[iS].devID;
  int iC, isC;
  int trig = 0;
#ifdef ENABLE_COMPUTATION
  int i;
#endif

  if ( subdev[iS].subdev < 0 || !subdev[iS].used ) {
    ERROR_MSG( "loadChanlist ERROR: First open an appropriate device and subdevice. Chanlist not loaded!\n" );
    return -1;
  }

  if ( chanlistIOC->chanlistN > MAXCHANLIST ) {
    ERROR_MSG( "loadChanlist ERROR: Invalid chanlist length for Subdevice %i on device %s. Chanlist not loaded!\n",
	       iS, device[subdev[iS].devID].name );
    return -1;
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
    DEBUG_MSG( "checked for trigger on subdevice %d: %d\n", subdev[iS].subdev, trig );
  }

  if ( subdev[iS].chanlist ) {
    // subdev chanlist already exist
    for ( iC = 0; iC < chanlistIOC->chanlistN; iC++ ) {
      for ( isC = 0; isC < subdev[iS].chanN; isC++ ) {
        if ( CR_CHAN(chanlistIOC->chanlist[iC]) == 
	     subdev[iS].chanlist[isC].chan + PARAM_CHAN_OFFSET*subdev[iS].chanlist[isC].isParamChan ) {
	  subdev[iS].chanlist[isC].isUsed = 1;
	  if ( trig && subdev[iS].chanlist[iC].chan == trigger.chan ) {
	    DEBUG_MSG( "set trigger for channel %d id %d on subdevice %d with level %d\n", subdev[iS].chanlist[iC].chan, iC, subdev[iS].subdev, (int)(100.0*trigger.alevel) );
	    subdev[iS].chanlist[iC].trigger = 1;
	    subdev[iS].chanlist[iC].alevel = trigger.alevel;
	  }
	  else {
	    subdev[iS].chanlist[iC].trigger = 0;
	    subdev[iS].chanlist[iC].alevel = 0.0;
	  }
	  if ( ! subdev[iS].chanlist[isC].isParamChan ) {
	    subdev[iS].chanlist[isC].aref = CR_AREF( chanlistIOC->chanlist[iC] );
	    subdev[iS].chanlist[isC].rangeIndex = CR_RANGE( chanlistIOC->chanlist[iC] );
	    subdev[iS].chanlist[isC].insn.chanspec = chanlistIOC->chanlist[iC];
	    memcpy( &subdev[iS].chanlist[iC].converter, &chanlistIOC->conversionlist[iC], sizeof(struct converterT) );
	    subdev[iS].chanlist[isC].scale = chanlistIOC->scalelist[iC];
	  }
	  break;
        }
      }
    }
  }
  else {
    
    // create and initialize chanlist for subdevice:
    subdev[iS].chanlist = vmalloc( chanlistIOC->chanlistN
				   *sizeof(struct chanT) );
    if ( !subdev[iS].chanlist ) {
      ERROR_MSG( "loadChanlist ERROR: Memory allocation for Subdevice %i on device %s. Chanlist not loaded!\n",
		 iS, device[subdev[iS].devID].name );
      return -1;
    }
    subdev[iS].chanN = chanlistIOC->chanlistN;
    subdev[iS].userSubdevIndex = chanlistIOC->userDeviceIndex;
    
    for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {
      subdev[iS].chanlist[iC].devP = device[iD].devP;
      subdev[iS].chanlist[iC].subdev = subdev[iS].subdev;
      subdev[iS].chanlist[iC].chan = CR_CHAN( chanlistIOC->chanlist[iC] );
      subdev[iS].chanlist[iC].lsample = 0;
      memset( &subdev[iS].chanlist[iC].insn, 0, sizeof(comedi_insn) );
      subdev[iS].chanlist[iC].isParamChan = (subdev[iS].chanlist[iC].chan >= PARAM_CHAN_OFFSET);
      subdev[iS].chanlist[iC].modelIndex = -1;
      subdev[iS].chanlist[iC].isUsed = 1; 
      subdev[iS].chanlist[iC].voltage = 0.0; 
      subdev[iS].chanlist[iC].prevvoltage = 0.0;
      subdev[iS].chanlist[iC].fifo = subdev[iS].fifo;
      if ( trig && subdev[iS].chanlist[iC].chan == trigger.chan ) {
	DEBUG_MSG( "added trigger to channel %d id %d on subdevice %d with level %d\n", subdev[iS].chanlist[iC].chan, iC, subdev[iS].subdev, (int)(100.0*trigger.alevel) );
	subdev[iS].chanlist[iC].trigger = 1;
	subdev[iS].chanlist[iC].alevel = trigger.alevel;
      }
      else {
	subdev[iS].chanlist[iC].trigger = 0;
	subdev[iS].chanlist[iC].alevel = 0.0;
      }
#ifdef ENABLE_COMPUTATION
      if ( subdev[iS].chanlist[iC].isParamChan ) {
	subdev[iS].chanlist[iC].chan -= PARAM_CHAN_OFFSET;
	subdev[iS].chanlist[iC].aref = 0;
	subdev[iS].chanlist[iC].rangeIndex = 0;
	memset( &subdev[iS].chanlist[iC].converter, 0, sizeof( struct converterT ) );
	subdev[iS].chanlist[iC].scale = 1.0;
      }
      else {
#endif
	if ( subdev[iS].type == SUBDEV_IN ) {
	  subdev[iS].chanlist[iC].insn.insn = INSN_READ;
#ifdef ENABLE_COMPUTATION
	  for ( i = 0; i < INPUT_N; i++ ) {
	    if ( inputDevices[i] == subdev[iS].userSubdevIndex && 
		 inputChannels[i] == subdev[iS].chanlist[iC].chan )
	      subdev[iS].chanlist[iC].modelIndex = i;
	  }
#endif
	}
	else {
	  subdev[iS].chanlist[iC].insn.insn = INSN_WRITE;
#ifdef ENABLE_COMPUTATION
	  for ( i = 0; i < OUTPUT_N; i++ ) {
	    if ( outputDevices[i] == subdev[iS].userSubdevIndex && 
		 outputChannels[i] == subdev[iS].chanlist[iC].chan )
	      subdev[iS].chanlist[iC].modelIndex = i;
	  }
#endif
	}
	subdev[iS].chanlist[iC].aref = CR_AREF( chanlistIOC->chanlist[iC] );
	subdev[iS].chanlist[iC].rangeIndex = CR_RANGE( chanlistIOC->chanlist[iC] );
	subdev[iS].chanlist[iC].insn.n = 1;
	subdev[iS].chanlist[iC].insn.data = &subdev[iS].chanlist[iC].lsample;
	subdev[iS].chanlist[iC].insn.subdev = subdev[iS].subdev;
	subdev[iS].chanlist[iC].insn.chanspec = chanlistIOC->chanlist[iC];
	memcpy( &subdev[iS].chanlist[iC].converter, &chanlistIOC->conversionlist[iC], sizeof(struct converterT) );
	subdev[iS].chanlist[iC].scale = chanlistIOC->scalelist[iC];
#ifdef ENABLE_COMPUTATION
      }
#endif
    }
  }    
  return 0;
}


int loadSyncCmd( struct syncCmdIOCT *syncCmdIOC )
{
  int iS = syncCmdIOC->subdevID;

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
    ERROR_MSG( "LOAdSyncCmd ERROR: Requested frequency is above MAX_FREQUENCY (%d Hz). Sync-command not loaded!\n",
	       MAX_FREQUENCY );
    return -EINVAL;
  }

  // initialize sampling parameters for subdevice:
  subdev[iS].frequency = syncCmdIOC->frequency > 0 ? syncCmdIOC->frequency : dynClampTask.setFreq;
  subdev[iS].delay = syncCmdIOC->delay;
  subdev[iS].duration = syncCmdIOC->duration;
  subdev[iS].continuous = syncCmdIOC->continuous;
  subdev[iS].startsource = syncCmdIOC->startsource;
  subdev[iS].pending = 0;

  DEBUG_MSG( "loadSyncCmd: loaded %ld samples with startsource %d for subdevice %d\n", subdev[iS].duration, subdev[iS].startsource, iS );

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
      ERROR_MSG( "startSubdevice: failed to start dynamic clamp loop for subdevice %d type %s!\n",
		 iS, subdev[iS].type == SUBDEV_IN ? "AI" : "AO" );
      subdev[iS].running = 0;
      return -ENOMEM;
    }
    SDEBUG_MSG( "startSubdevice: successfully started rt_task!\n" );
  }

  subdev[iS].running = 1;

  SDEBUG_MSG( "startSubdevice: successfully started subdevice %d type %s!\n",
	     iS, subdev[iS].type == SUBDEV_IN ? "AI" : "AO" );

  return 0;
}


int stopSubdevice( int iS, int kill )
{ 
  int i;

  if ( !subdev[iS].running )
    return 0;
  subdev[iS].running = 0;
  for ( i = 0; i < subdev[iS].chanN; i++ )
    subdev[iS].chanlist[i].isUsed = 0;
  SDEBUG_MSG( "stopSubdevice %d with kill=%d\n", iS, kill );

  if ( !kill )
    return 0;

  // if all subdevices stopped => halt dynclamp task:
  for ( i = 0; i < subdevN; i++ )
    if ( subdev[i].running )
      return 0;
  SDEBUG_MSG( "stopSubdevice halts dynclamp task\n" );
  cleanup_rt_task();
  return 0;
}


void releaseSubdevice( int iS )
{
  int iD = subdev[iS].devID;
  int i;
#ifdef ENABLE_TTLPULSE
  int pT;
  int iT;
  int k;
  comedi_t *devP;
  comedi_insn *insn;
  int retVal;
#endif

  if ( !subdev[iS].used || subdev[iS].subdev < 0 ) {
    ERROR_MSG( "releaseSubdevice ERROR: Subdevice with ID %d not in use!\n", iS );
    return;
  }

  // stop subdevice:
  if ( subdev[iS].running ) {
    SDEBUG_MSG( "releaseSubdevice stops and potentially kills subdevice %d\n", iS );
    stopSubdevice( iS, /*kill=*/1 );
  }
  
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

  if ( subdev[iS].type == SUBDEV_IN || subdev[iS].type == SUBDEV_OUT ) {
    // delete FIFO
    rtf_destroy( subdev[iS].fifo );
  }
#ifdef ENABLE_TTLPULSE
  else if ( subdev[iS].type == SUBDEV_DIO ) {
    // remove ttl pulses:
    for ( pT = 0; pT < MAXTTLPULSETYPES; pT++ ) {
      for ( iT = 0; iT < MAXTTLPULSES && ttlDevices[pT][iT] != 0; ) {
	if ( ttlDevices[pT][iT] == device[iD].devP &&
	     ttlInsns[pT][iT]->subdev == subdev[iS].subdev ) {
	  /* remove from list: */
	  devP = ttlDevices[pT][iT];
	  insn = ttlInsns[pT][iT];
	  for ( k = iT+1; k < MAXTTLPULSES; k++ ) {
	    ttlDevices[pT][k-1] = ttlDevices[pT][k];
	    ttlInsns[pT][k-1] = ttlInsns[pT][k];
	  }
	  ttlDevices[pT][MAXTTLPULSES-1] = 0;
	  ttlInsns[pT][MAXTTLPULSES-1] = 0;
	  /* set low: */
	  insn->data = &ttlLow;
	  retVal = comedi_do_insn( devP, insn );
	  if ( retVal < 1 ) {
	    if ( retVal < 0 )
	      comedi_perror( "dynclampmodule: releaseSubdevice() -> clearing ttl pulse: comedi_do_insn" );
	    ERROR_MSG( "releaseSubdevice() -> ERROR! failed to set TTL pulse %d low\n", iT );
	  }
	  vfree( insn );
	}
	else
	  iT++;
      }
    }
  }
#endif

  // reset subdevice structure:
  memset( &(subdev[iS]), 0, sizeof(struct subdeviceT) );
  SDEBUG_MSG( "releaseSubdevice released subdevice %d\n", iS );
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
  DEBUG_MSG( "releaseSubdevice: released device for last subdev-ID %d\n", iS );
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


int setDigitalIO( struct dioIOCT *dioIOC )
{
  int iS = dioIOC->subdevID;
  int iD = subdev[iS].devID;
  int subdevice = subdev[iS].subdev;
  comedi_t *devP = device[iD].devP;
  unsigned int bit = 0;
  int channel = 0;
  int direction = 0;
#ifdef ENABLE_TTLPULSE
  int pT = dioIOC->pulseType;
  int iT = 0;
  int k = 0;
  int found = 0;
#endif

  if ( dioIOC->op == DIO_CONFIGURE ) {
    if ( dioIOC->bitfield ) {
      bit = 1;
      for ( channel=0; channel<32; channel++ ) {
	if ( ( dioIOC->lines & bit ) > 0 ) {
	  direction = COMEDI_INPUT;
	  if ( ( dioIOC->output & bit ) > 0 )
	    direction = COMEDI_OUTPUT;
	  if ( comedi_dio_config( devP, subdevice, channel, direction ) < 0 ) {
	    comedi_perror( "setDigitalIO() -> DIO_CONFIGURE" );
	    ERROR_MSG( "setDigitalIO: comedi_dio_config bitfield on device %s, subdevice %d, channel %d, direction %d failed!\n",
		       device[iD].name, subdevice, channel, direction );
	    return -EFAULT;
	  }
	}
	bit *= 2;
      }
    }
    else {
      direction = dioIOC->output ? COMEDI_OUTPUT : COMEDI_INPUT;
      if ( comedi_dio_config( devP, subdevice, dioIOC->lines, direction ) < 0 ) {
	comedi_perror( "setDigitalIO() -> DIO_CONFIGURE" );
	ERROR_MSG( "setDigitalIO: comedi_dio_config single channel on device %s, subdevice %d, channel %d, direction %d failed!\n",
		   device[iD].name, subdevice, dioIOC->lines, direction );
	return -EFAULT;
      }
    }
  }
  else if ( dioIOC->op == DIO_READ ) {
    if ( dioIOC->bitfield ) {
      bit = 0;
      if ( comedi_dio_bitfield( devP, subdevice, dioIOC->lines, &bit ) < 0 ) {
	comedi_perror( "setDigitalIO() -> DIO_READ" );
	ERROR_MSG( "setDigitalIO: comedi_dio_read on device %s subdevice %d failed!\n",
		  device[iD].name, subdevice );
	return -EFAULT;
      }
      dioIOC->output = bit & dioIOC->lines;
    }
    else {
      bit = 0;
      if ( comedi_dio_read( devP, subdevice, dioIOC->lines, &bit ) != 1 ) {
	comedi_perror( "setDigitalIO() -> DIO_READ" );
	ERROR_MSG( "setDigitalIO: comedi_dio_read on device %s subdevice %d failed!\n",
		  device[iD].name, subdevice );
	return -EFAULT;
      }
      dioIOC->output = bit;
    }
  }
  else if ( dioIOC->op == DIO_WRITE ) {
    if ( dioIOC->bitfield ) {
      bit = dioIOC->output;
      if ( comedi_dio_bitfield( devP, subdevice, dioIOC->lines, &bit ) < 0 ) {
	comedi_perror( "setDigitalIO() -> DIO_WRITE" );
	ERROR_MSG( "setDigitalIO: comedi_dio_write on device %s subdevice %d failed!\n",
		  device[iD].name, subdevice );
	return -EFAULT;
      }
    }
    else {
      if ( comedi_dio_write( devP, subdevice, dioIOC->lines, dioIOC->output ) != 1 ) {
	comedi_perror( "setDigitalIO() -> DIO_WRITE" );
	ERROR_MSG( "setDigitalIO: comedi_dio_write on device %s subdevice %d failed!\n",
		  device[iD].name, subdevice );
	return -EFAULT;
      }
    }
  }
#ifdef ENABLE_TTLPULSE
  else if ( dioIOC->op == DIO_ADD_TTLPULSE ) {
    if ( pT < TTL_START_WRITE || pT >= MAXTTLPULSETYPES )
      return -EINVAL;
    for ( iT = 0; iT < MAXTTLPULSES && ttlDevices[pT][iT] != 0; iT++ );
    if ( iT >= MAXTTLPULSES )
      return -ENOMEM;
    if ( comedi_dio_write( devP, subdevice, dioIOC->lines, dioIOC->output ) != 1 ) {
      comedi_perror( "setDigitalIO() -> DIO_ADD_TTLPULSE" );
      ERROR_MSG( "setDigitalIO: comedi_dio_write on device %s subdevice %d failed!\n",
		 device[iD].name, subdevice );
      return -EFAULT;
    }
    ttlDevices[pT][iT] = devP;
    ttlInsns[pT][iT] = vmalloc( sizeof(comedi_insn) );
    memset( ttlInsns[pT][iT], 0, sizeof(comedi_insn) );
    ttlInsns[pT][iT]->insn = INSN_WRITE;
    ttlInsns[pT][iT]->n = 1;
    ttlInsns[pT][iT]->data = ( dioIOC->output ? &ttlHigh : &ttlLow );
    ttlInsns[pT][iT]->subdev = subdevice;
    ttlInsns[pT][iT]->chanspec = CR_PACK( dioIOC->lines, 0, 0 );
    DEBUG_MSG( "add pulse pT=%d  iT=%d  output=%d subdev=%d lines=%d\n", pT, iT, ttlInsns[pT][iT]->data[0], ttlInsns[pT][iT]->subdev, ttlInsns[pT][iT]->chanspec );
  }
  else if ( dioIOC->op == DIO_CLEAR_TTLPULSE ) {
    found = 0;
    for ( pT = 0; pT < MAXTTLPULSETYPES; pT++ ) {
      for ( iT = 0; iT < MAXTTLPULSES && ttlDevices[pT][iT] != 0; ) {
	if ( ttlDevices[pT][iT] == devP &&
	     ttlInsns[pT][iT]->subdev == subdevice &&
	     ttlInsns[pT][iT]->chanspec == CR_PACK( dioIOC->lines, 0, 0 ) ) {
	  found = 1;
	  vfree( ttlInsns[pT][iT] );
	  for ( k = iT+1; k < MAXTTLPULSES; k++ ) {
	    ttlDevices[pT][k-1] = ttlDevices[pT][k];
	    ttlInsns[pT][k-1] = ttlInsns[pT][k];
	  }
	  ttlDevices[pT][MAXTTLPULSES-1] = 0;
	  ttlInsns[pT][MAXTTLPULSES-1] = 0;
	}
	else
	  iT++;
      }
    }
    if ( found ) {
      if ( comedi_dio_write( devP, subdevice, dioIOC->lines, dioIOC->output ) != 1 ) {
	comedi_perror( "setDigitalIO() -> DIO_CLEAR_TTLPULSE" );
	ERROR_MSG( "setDigitalIO: comedi_dio_write on device %s subdevice %d failed!\n",
		   device[iD].name, subdevice );
	return -EFAULT;
      }
    }
  }
#endif
  else
    return -EINVAL;
  return 0;
}


int setAnalogTrigger( struct triggerIOCT *triggerIOC )
{
#ifdef ENABLE_TRIGGER
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
#else
  return -EINVAL;
#endif
}


int unsetAnalogTrigger( struct triggerIOCT *triggerIOC )
{
#ifdef ENABLE_TRIGGER
  int iS, iC;

  // disable trigger:
  trigger.enabled = 0;
  for ( iS = 0; iS < subdevN; iS++ ) {
    for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {
      subdev[iS].chanlist[iC].trigger = 0;
    }
  }

  return 0;
#else
  return -EINVAL;
#endif
}


///////////////////////////////////////////////////////////////////////////////
// *** REAL-TIME TASKS *** 
///////////////////////////////////////////////////////////////////////////////


/*! Dynamic clamp task */
void rtDynClamp( long dummy )
{
  int retVal;
  int iS, iC;
#ifdef ENABLE_TTLPULSE
  int iT;
#endif
  int subdevRunning = 1;
  unsigned long readCnt = 0;
  struct chanT *pChan;
  float voltage;
  int triggerevs[5] = { 1, 0, 0, 0, 0 };
  int prevtriggerevs[5] = { 0, 0, 0, 0, 0 };

  //  int vi, oi, pi; // DEBUG

  SDEBUG_MSG( "rtDynClamp: starting dynamic clamp loop at %u Hz\n", 
	     1000000000/dynClampTask.periodLengthNs );

  dynClampTask.loopCnt = 0;
  dynClampTask.aoIndex = -1;
  dynClampTask.running = 1;
  
  /**************************************************************************/
  /******** LOOP START: *****************************************************/
  /**************************************************************************/
  while( subdevRunning ) {
    
    subdevRunning = 0;

#ifdef ENABLE_TTLPULSE
    for ( iT = 0; iT < MAXTTLPULSES && ttlStartWriteDevice[iT] != 0; iT++ ) {
      retVal = comedi_do_insn( ttlStartWriteDevice[iT] ,ttlStartWriteInsn[iT] );
      if ( retVal < 1 ) {
	if ( retVal < 0 )
	  comedi_perror( "dynclampmodule: rtDynClamp ttl pulse at start write: comedi_do_insn" );
	ERROR_MSG( "rtDynClamp: ERROR! failed to write TTL pulse %d at start write\n", iT );
      }
    }
#endif

    /******** WRITE TO ANALOG OUTPUT: ******************************************/
    /****************************************************************************/
    // AO Subdevice loop:
    for ( iS = 0; iS < subdevN; iS++ ) {
      if ( subdev[iS].type == SUBDEV_OUT ) {

	// check for pending start trigger:
	if ( subdev[iS].running && subdev[iS].pending ) {
	  DEBUG_MSG( "REALTIMELOOP PENDING AO subdev=%d, startsrc=%d, prevtriger1=%d, triger1=%d, pv=%d, v=%d\n",
		     iS, subdev[iS].startsource, prevtriggerevs[1], triggerevs[1],
		     (int)(100.0*subdev[0].chanlist[0].prevvoltage), (int)(100.0*subdev[0].chanlist[0].voltage) );
	  if ( triggerevs[subdev[iS].startsource] &&
	       ! prevtriggerevs[subdev[iS].startsource] ) {
	    DEBUG_MSG( "REALTIMELOOP PENDING AO SETUP duration=%lu, loopCnt=%lu\n", subdev[iS].duration, dynClampTask.loopCnt );
	    subdev[iS].delay = dynClampTask.loopCnt + subdev[iS].delay; 
	    subdev[iS].duration = subdev[iS].delay + subdev[iS].duration;
	    dynClampTask.aoIndex = subdev[iS].delay;
	    subdev[iS].pending = 0;
	    DEBUG_MSG( "REALTIMELOOP PENDING AO STARTED duration=%lu delay=%lu, loopCnt=%lu\n", subdev[iS].duration, subdev[iS].delay, dynClampTask.loopCnt );
#ifdef ENABLE_TTLPULSE
	    for ( iT = 0; iT < MAXTTLPULSES && ttlStartAODevice[iT] != 0; iT++ ) {
	      retVal = comedi_do_insn( ttlStartAODevice[iT] ,ttlStartAOInsn[iT] );
	      if ( retVal < 1 ) {
		if ( retVal < 0 )
		  comedi_perror( "dynclampmodule: rtDynClamp ttl pulse at start ao: comedi_do_insn" );
		ERROR_MSG( "rtDynClamp: ERROR! failed to write TTL pulse %d at start ao\n", iT );
	      }
	    }
#endif
	  }
	}

	if ( subdev[iS].running && ! subdev[iS].pending ) {
	  
	  // check end of stimulus:
	  if ( !subdev[iS].continuous &&
	       subdev[iS].duration <= dynClampTask.loopCnt ) {
	    SDEBUG_MSG( "rtDynClamp: finished subdevice %d at loop %lu\n", iS, dynClampTask.loopCnt );
	    rtf_reset( subdev[iS].fifo );
	    stopSubdevice( iS, /*kill=*/0 );
#ifdef ENABLE_TTLPULSE
	    for ( iT = 0; iT < MAXTTLPULSES && ttlEndAODevice[iT] != 0; iT++ ) {
	      retVal = comedi_do_insn( ttlEndAODevice[iT] ,ttlEndAOInsn[iT] );
	      if ( retVal < 1 ) {
		if ( retVal < 0 )
		  comedi_perror( "dynclampmodule: rtDynClamp ttl pulse at end ao: comedi_do_insn" );
		ERROR_MSG( "rtDynClamp: ERROR! failed to write TTL pulse %d at end ao\n", iT );
	      }
	    }
#endif
	  }
	  else if ( dynClampTask.loopCnt >= subdev[iS].delay ) {
 	    // read output from FIFO:
	    for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {
	      pChan = &subdev[iS].chanlist[iC];
	      if ( pChan->isUsed ) {
		// get data from FIFO:
		retVal = rtf_get( pChan->fifo, &pChan->voltage, sizeof(float) );
		if ( retVal != sizeof(float) ) {
		  if ( retVal == EINVAL ) {
		    ERROR_MSG( "rtDynClamp: ERROR! No open FIFO for AO subdevice ID %d at loopCnt %lu\n",
			       iS, dynClampTask.loopCnt );
		    ERROR_MSG( "Stop dynClampTask." );
		    dynClampTask.running = 0;
		    dynClampTask.duration = 0;
		    return;
		  }
		  subdev[iS].error = E_UNDERRUN;
		  ERROR_MSG( "rtDynClamp: ERROR! Data buffer underrun for AO subdevice ID %d at loopCnt %lu\n",
			     iS, dynClampTask.loopCnt );
		  subdev[iS].running = 0;
		  continue;
		}
#ifdef ENABLE_COMPUTATION
		if ( pChan->isParamChan ) {
		  paramOutput[pChan->chan] = pChan->voltage;
		}
#endif
	      }
	    }
	  }

	}  // subdev[iS].running && ! subdev[iS].pending
	
	subdevRunning = 1;
	
	// write output to daq board:
	for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {
	  pChan = &subdev[iS].chanlist[iC];
	  // this is an output to the DAQ board:
#ifdef ENABLE_COMPUTATION
	  if ( !pChan->isParamChan ) {
#endif
	    voltage = pChan->voltage;
#ifdef ENABLE_COMPUTATION
	    // add model output to sample:
	    if ( pChan->modelIndex >= 0 )
	      voltage += output[pChan->modelIndex];
#endif
	    // write out Sample:
	    value_to_sample( pChan, voltage ); // sets pChan->lsample
	    retVal = comedi_do_insn( pChan->devP, &pChan->insn );
	    if ( retVal < 1 ) {
	      subdev[iS].running = 0;
	      subdev[iS].error = E_NODATA;
	      ERROR_MSG( "rtDynClamp: ERROR! failed to write data to AO subdevice ID %d channel %d at loopCnt %lu\n",
			 iS, iC, dynClampTask.loopCnt );
	      if ( retVal < 0 ) {
		comedi_perror( "dynclampmodule: rtDynClamp: comedi_data_write" );
		subdev[iS].error = E_COMEDI;
		subdev[iS].running = 0;
		ERROR_MSG( "rtDynClamp: ERROR! failed to write to AO subdevice ID %d channel %d at loopCnt %lu\n",
			   iS, iC, dynClampTask.loopCnt );
	      }
	    }
#ifdef ENABLE_COMPUTATION
	  }
#endif
	} // end of chan loop

      }
    } // end of device loop


#ifdef ENABLE_TTLPULSE
    for ( iT = 0; iT < MAXTTLPULSES && ttlEndWriteDevice[iT] != 0; iT++ ) {
      retVal = comedi_do_insn( ttlEndWriteDevice[iT] ,ttlEndWriteInsn[iT] );
      if ( retVal < 1 ) {
	if ( retVal < 0 )
	  comedi_perror( "dynclampmodule: rtDynClamp ttl pulse at end write: comedi_do_insn" );
	ERROR_MSG( "rtDynClamp: ERROR! failed to write TTL pulse %d at end write\n", iT );
      }
    }
#endif

    /******** SLEEP FOR NEURON TO REACT TO GIVEN OUTPUT: ************************/
    /****************************************************************************/
    // PROBLEM: rt_sleep is timed using jiffies only (granularity = 1msec)
	// int retValSleep = rt_sleep( nano2count( INJECT_RECORD_DELAY ) );
    rt_busy_sleep( INJECT_RECORD_DELAY ); // TODO: just default


#ifdef ENABLE_TTLPULSE
    for ( iT = 0; iT < MAXTTLPULSES && ttlStartReadDevice[iT] != 0; iT++ ) {
      retVal = comedi_do_insn( ttlStartReadDevice[iT] ,ttlStartReadInsn[iT] );
      if ( retVal < 1 ) {
	if ( retVal < 0 )
	  comedi_perror( "dynclampmodule: rtDynClamp ttl pulse at start read: comedi_do_insn" );
	ERROR_MSG( "rtDynClamp: ERROR! failed to write TTL pulse %d at start read\n", iT );
      }
    }
#endif
    
    /******** FROM ANALOG INPUT: **********************************************/
    /****************************************************************************/
    for ( iS = 0; iS < subdevN; iS++ ) {
      if ( subdev[iS].type == SUBDEV_IN && subdev[iS].running ) {
	
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
	  stopSubdevice(iS, /*kill=*/0 );
	}
	subdevRunning = 1;

	// FOR EVERY CHAN...
	for ( iC = 0; iC < subdev[iS].chanN; iC++ ) {

	  pChan = &subdev[iS].chanlist[iC];

	  // previous sample:
	  pChan->prevvoltage = pChan->voltage;

	  // acquire sample:
#ifdef ENABLE_COMPUTATION
	  if ( !pChan->isParamChan ) {
#endif
	    retVal = comedi_do_insn( pChan->devP, &pChan->insn );     
	    if ( retVal < 1 ) {
	      subdev[iS].running = 0;
	      subdev[iS].error = E_NODATA;
	      ERROR_MSG( "rtDynClamp: ERROR! failed to read data from AI subdevice ID %d channel %d at loopCnt %lu\n",
			 iS, iC, dynClampTask.loopCnt );
	      if ( retVal < 0 ) {
		comedi_perror( "dynclampmodule: rtDynClamp: comedi_data_read" );
		subdev[iS].running = 0;
		subdev[iS].error = E_COMEDI;
		ERROR_MSG( "rtDynClamp: ERROR! failed to read from AI subdevice ID %d channel %d at loopCnt %lu\n",
			   iS, iC, dynClampTask.loopCnt );
		continue;
	      }
	    }
	    // convert to voltage:
	    sample_to_value( pChan ); // sets pChan->voltage from pChan->lsample
#ifdef ENABLE_COMPUTATION
	    if ( pChan->modelIndex >= 0 )
	      input[pChan->modelIndex] = pChan->voltage;
	  }
	  else {
	    pChan->voltage = paramInput[pChan->chan];
	  }
#endif
	  
	  // debug:
	  if ( subdev[iS].running == 0 )
	    ERROR_MSG( "rtDynClamp: ERROR! subdevice %d somehow not running\n", iS);
	  // write to FIFO:
	  retVal = rtf_put( pChan->fifo, &pChan->voltage, sizeof(float) );
	  // debug:
	  if ( subdev[iS].running == 0 )
	    ERROR_MSG( "rtDynClamp: ERROR! rtf_put turned subdevice %d not running\n", iS);

	  if ( retVal != sizeof(float) ) {
	    SDEBUG_MSG( "rtDynClamp: ERROR! rtf_put failed, return value=%d\n", retVal );
	    if ( retVal == EINVAL ) {
	      ERROR_MSG( "rtDynClamp: ERROR! No open FIFO for AI subdevice ID %d at loopCnt %lu\n",
			 iS, dynClampTask.loopCnt );
	      ERROR_MSG( "Stop dynClampTask." );
	      dynClampTask.running = 0;
	      dynClampTask.duration = 0;
	      return;
	    }
	    subdev[iS].error = E_OVERFLOW;
	    ERROR_MSG( "rtDynClamp: ERROR! FIFO buffer overflow for AI subdevice ID %d at loopCnt %lu\n",
		       iS, dynClampTask.loopCnt );
	    subdev[iS].running = 0;
	    continue;
	  }

	  if ( subdev[iS].running == 0 )
	    ERROR_MSG( "rtDynClamp: ERROR! rtf_put error handling turned subdevice %d not running\n", iS);

#ifdef ENABLE_TRIGGER
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
#endif
	  
	} // end of chan loop
	readCnt++; // FOR DEBUG
      }
    } // end of device loop


#ifdef ENABLE_TTLPULSE
    for ( iT = 0; iT < MAXTTLPULSES && ttlEndReadDevice[iT] != 0; iT++ ) {
      retVal = comedi_do_insn( ttlEndReadDevice[iT] ,ttlEndReadInsn[iT] );
      if ( retVal < 1 ) {
	if ( retVal < 0 )
	  comedi_perror( "dynclampmodule: rtDynClamp ttl pulse at end read: comedi_do_insn" );
	ERROR_MSG( "rtDynClamp: ERROR! failed to write TTL pulse %d at end read\n", iT );
      }
    }
#endif


    /****************************************************************************/
#ifdef ENABLE_COMPUTATION
    computeModel();
#endif

    /******** WAIT FOR CALCULATION TASK TO COMPUTE RESULT: **********************/
    /****************************************************************************/
    dynClampTask.loopCnt++;

    //    start = rt_get_cpu_time_ns();
    rt_task_wait_period();

  } // END OF DYNCLAMP LOOP
    
  dynClampTask.running = 0;
  dynClampTask.duration = 0;

  SDEBUG_MSG( "rtDynClamp: left dynamic clamp loop after %lu cycles\n",
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
  const int usesFPU = 1;
  void* signal = NULL;
  int dummy = 23;
  int retVal;
  RTIME periodTicks;

  DEBUG_MSG( "init_rt_task: Trying to initialize dynamic clamp RTAI task...\n" );

  // test if dynamic clamp frequency is valid:
  if ( dynClampTask.reqFreq <= 0 || dynClampTask.reqFreq > MAX_FREQUENCY ) {
    ERROR_MSG( "init_rt_task ERROR: %dHz -> invalid dynamic clamp frequency. Valid range is 1 .. %dHz\n", 
	       dynClampTask.reqFreq, MAX_FREQUENCY );
    return -1;
  }

  // initializing rt-task for dynamic clamp with high priority:
  priority = 1;
  retVal = rt_task_init( &dynClampTask.rtTask, rtDynClamp, dummy, stackSize, 
			 priority, usesFPU, signal );
  if ( retVal ) {
    ERROR_MSG( "init_rt_task ERROR: failed to initialize real-time task for dynamic clamp! stacksize was set to %d bytes.\n", 
	       stackSize );
    return -1;
  }
  DEBUG_MSG( "init_rt_task: Initialized dynamic clamp RTAI task. Trying to make it periodic...\n" );

    // START rt-task for dynamic clamp as periodic:
  periodTicks = start_rt_timer( nano2count( 1000000000/dynClampTask.reqFreq ) );  
  if ( rt_task_make_periodic( &dynClampTask.rtTask, rt_get_time(), periodTicks ) 
      != 0 ) {
    printk( "init_rt_task ERROR: failed to start periodic real-time task for data acquisition! loading of module failed!\n" );
    return -3;
  }
  dynClampTask.periodLengthNs = count2nano( periodTicks );
  dynClampTask.setFreq = 1000000000 / dynClampTask.periodLengthNs;
#ifdef ENABLE_COMPUTATION
  loopInterval = 1.0e-9*dynClampTask.periodLengthNs;
  loopRate = 1.0e9/dynClampTask.periodLengthNs;
#endif
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

#ifdef HAVE_UNLOCKED_IOCTL
long rtmodule_unlocked_ioctl( struct file *fModule, unsigned int cmd,
			      unsigned long arg )
#else
int rtmodule_ioctl( struct inode *devFile, struct file *fModule, 
		    unsigned int cmd, unsigned long arg )
#endif
{
  static struct deviceIOCT deviceIOC;
  static struct chanlistIOCT chanlistIOC;
  static struct syncCmdIOCT syncCmdIOC;
  static struct traceInfoIOCT traceInfo;
  static struct traceChannelIOCT traceChannel;
  static struct dioIOCT dioIOC;
  static struct triggerIOCT triggerIOC;

  int tmp, subdevID;
  int retVal;
  int rc = 0;
  unsigned long luTmp;


  if ( _IOC_TYPE(cmd) != RTMODULE_MAJOR || _IOC_NR(cmd) > RTMODULE_IOC_MAXNR) {
    ERROR_MSG( " ioctl: Major wrong or ioctl %d bigger than max %d\n", 
	       _IOC_TYPE(cmd), RTMODULE_IOC_MAXNR );
    return -ENOTTY;
  }
  DEBUG_MSG( "ioctl: user triggered ioctl %d %s\n",_IOC_NR( cmd ), iocNames[_IOC_NR( cmd )] );

  mutex_lock( &mutex );

  switch( cmd ) {
    

    /******** GIVE INFORMATION TO USER SPACE: ***********************************/
    
  case IOC_GETRATE:
    tmp = dynClampTask.setFreq;
    retVal = put_user( tmp, (int __user *)arg );
    rc = retVal == 0 ? 0 : -EFAULT;
    break;

  case IOC_GETAOINDEX:
    luTmp = dynClampTask.aoIndex;
    if ( luTmp < 0 ) {
      rc = -ENOSPC;
      break;
    }
    retVal = put_user( luTmp, (unsigned long __user *)arg );
    rc = retVal == 0 ? 0 : -EFAULT;
    break;

  case IOC_GETLOOPCNT:
    luTmp = dynClampTask.loopCnt;
    if ( luTmp < 0 ) {
      rc = -ENOSPC;
      break;
    }
    retVal = put_user( luTmp, (unsigned long __user *)arg );
    rc = retVal == 0 ? 0 : -EFAULT;
    break;

    /******** SET UP COMEDI: ****************************************************/

  case IOC_GET_SUBDEV_ID:
    tmp = getSubdevID();
    if ( tmp < 0 ) {
      rc = -ENOSPC;
      break;
    }
    retVal = put_user( tmp, (int __user *)arg );
    rc = retVal == 0 ? 0 : -EFAULT;
    break;

  case IOC_OPEN_SUBDEV:
    retVal = copy_from_user( &deviceIOC, (void __user *)arg, sizeof(struct deviceIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to deviceIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    if ( deviceIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in deviceIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    retVal = openComediDevice( &deviceIOC );
    if ( retVal != 0 ) {
      rc = -EFAULT;
      break;
    }
    retVal = copy_to_user( (void __user *)arg, &deviceIOC, sizeof(struct deviceIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to deviceIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    rc = 0;
    break;

  case IOC_CHANLIST:
    retVal = copy_from_user( &chanlistIOC, (void __user *)arg, sizeof(struct chanlistIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to chanlistIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    if ( chanlistIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in chanlistIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    retVal = loadChanlist( &chanlistIOC );
    rc = retVal == 0 ? 0 : -EFAULT;
    break;

  case IOC_SYNC_CMD:
    retVal = copy_from_user( &syncCmdIOC, (void __user *)arg, sizeof(struct syncCmdIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to syncCmdIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    if ( syncCmdIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in syncCmdIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    retVal = loadSyncCmd( &syncCmdIOC );
    rc = retVal;
    break;


  case IOC_GET_TRACE_INFO:
#ifdef ENABLE_COMPUTATION
    retVal = copy_from_user( &traceInfo, (void __user *)arg, sizeof(struct traceInfoIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid user pointer for traceInfoIOCT!\n" );
      rc = -EFAULT;
      break;
    }
    rc = 0;
    switch( traceInfo.traceType ) {
    case TRACE_IN:
      if ( traceIndex >= INPUT_N ) {
	traceIndex = 0;
	rc = -ERANGE; // signal end of list
	break;
      }
      strncpy( traceInfo.name, inputNames[traceIndex], PARAM_NAME_MAXLEN );
      strncpy( traceInfo.unit, inputUnits[traceIndex], PARAM_NAME_MAXLEN );
      break;
    case TRACE_OUT:
      if ( traceIndex >= OUTPUT_N ) {
	traceIndex = 0;
	rc = -ERANGE; // signal end of list
	break;
      }
      strncpy( traceInfo.name, outputNames[traceIndex], PARAM_NAME_MAXLEN );
      strncpy( traceInfo.unit, outputUnits[traceIndex], PARAM_NAME_MAXLEN );
      break;
    case PARAM_IN:
      if ( traceIndex >= PARAMINPUT_N ) {
	traceIndex = 0;
	rc = -ERANGE; // signal end of list
	break;
      }
      strncpy( traceInfo.name, paramInputNames[traceIndex], PARAM_NAME_MAXLEN );
      strncpy( traceInfo.unit, paramInputUnits[traceIndex], PARAM_NAME_MAXLEN );
      break;
    case PARAM_OUT:
      if ( traceIndex >= PARAMOUTPUT_N ) {
	traceIndex = 0;
	rc = -ERANGE; // signal end of list
	break;
      }
      strncpy( traceInfo.name, paramOutputNames[traceIndex], PARAM_NAME_MAXLEN );
      strncpy( traceInfo.unit, paramOutputUnits[traceIndex], PARAM_NAME_MAXLEN );
      break;
    default: ;
    }
    if ( rc != 0 )
      break;
    retVal = copy_to_user( (void __user *)arg, &traceInfo, sizeof(struct traceInfoIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid user pointer for traceInfoIOCT!\n" );
      rc = -EFAULT;
      break;
    }
    traceIndex++;
    rc = 0;
    break;
#else
    rc = -ERANGE; // signal end of list
    break;
#endif

  case IOC_SET_TRACE_CHANNEL:
#ifdef ENABLE_COMPUTATION
    retVal = copy_from_user( &traceChannel, (void __user *)arg, sizeof(struct traceChannelIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid user pointer for traceChannelIOCT!\n" );
      rc = -EFAULT;
      break;
    }
    switch( traceChannel.traceType ) {
    case TRACE_IN:
      inputDevices[chanIndex] = traceChannel.device;
      inputChannels[chanIndex] = traceChannel.channel;
      break;
    case TRACE_OUT:
      outputDevices[chanIndex] = traceChannel.device;
      outputChannels[chanIndex] = traceChannel.channel;
      break;
    default: ;
    }
    chanIndex++;
    if ( chanIndex >= INPUT_N )
      chanIndex = 0;
    rc = 0;
    break;
#else
    rc = -EFAULT; // Nothing done
    break;
#endif


  case IOC_START_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for start-query!" );
      rc = -EFAULT;
      break;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for start-query!\n" );
      rc = -EFAULT;
      break;
    }
    retVal = startSubdevice( subdevID );
    rc = retVal;
    break;


  case IOC_CHK_RUNNING:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for running-query!" );
      rc = -EFAULT;
      break;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for running-query!\n" );
      rc = -EFAULT;
      break;
    }
    tmp = subdev[subdevID].running;
    DEBUG_MSG( "rtmodule_ioctl: running = %d for subdevID %d\n", tmp, subdevID );
    retVal = put_user( tmp, (int __user *)arg );
    rc = retVal == 0 ? 0 : -EFAULT;
    break;


  case IOC_REQ_CLOSE:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for close-request!" );
      rc = -EFAULT;
      break;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for close-request!\n" );
      rc = -EFAULT;
      break;
    }
    if ( reqCloseSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_CLOSE ERROR: Another close-request in progress!\n" );
      rc = -EAGAIN;
      break;
    }
    reqCloseSubdevID = subdevID;
    rc = 0;
    break;

  case IOC_REQ_READ: // Noch wichtig fuer tracename-List?
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for read-request!" );
      rc = -EFAULT;
      break;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for read-request!\n" );
      rc = -EFAULT;
      break;
    }
    if ( reqTraceSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ ERROR: Another read-request in progress! (reqTraceSubdevID=%d)\n", reqTraceSubdevID );
      rc = -EAGAIN;
      break;
    }
    ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ: Requested Read\n" );
    reqTraceSubdevID = subdevID;
    rc = 0;
    break;


  case IOC_STOP_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for stop-query!" );
      rc = -EFAULT;
      break;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for stop-query!\n" );
      rc = -EFAULT;
      break;
    }
    SDEBUG_MSG( "ioctl: user requests to stop and potentially kill subdevice %d\n", subdevID );
    retVal = stopSubdevice( subdevID, /*kill=*/1 );
    DEBUG_MSG( "rtmodule_ioctl: stopSubdevice returned %u\n", retVal );
    rc = retVal == 0 ? 0 : -EFAULT;
    break;


  case IOC_RELEASE_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for release-query!" );
      rc = -EFAULT;
      break;
    }
    if ( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for release-query!\n" );
      rc = -EFAULT;
      break;
    }
    SDEBUG_MSG( "ioctl: user requests to release subdevice %d\n", subdevID );
    releaseSubdevice( subdevID );
    rc = 0;
    break;


    // ******* Digital IO: ********************************************
  case IOC_DIO_CMD:
    retVal = copy_from_user( &dioIOC, (void __user *)arg, sizeof(struct dioIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to dioIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    if ( dioIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in dioIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    retVal = setDigitalIO( &dioIOC );
    if ( retVal != 0 ) {
      rc = retVal;
      break;
    }
    retVal = copy_to_user( (void __user *)arg, &dioIOC, sizeof(struct dioIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to dioIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    rc = 0;
    break;

    // ******* Trigger: ***********************************************

  case IOC_SET_TRIGGER:
    retVal = copy_from_user( &triggerIOC, (void __user *)arg, sizeof(struct triggerIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to triggerIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    rc = setAnalogTrigger( &triggerIOC );
    break;

  case IOC_UNSET_TRIGGER:
    retVal = copy_from_user( &triggerIOC, (void __user *)arg, sizeof(struct triggerIOCT) );
    if ( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to triggerIOCT-struct!\n" );
      rc = -EFAULT;
      break;
    }
    rc = unsetAnalogTrigger( &triggerIOC );
    break;


  default:
    ERROR_MSG( "rtmodule_ioctl ERROR - Invalid IOCTL!\n" );
    rc = -EINVAL;

  }

  mutex_unlock( &mutex );

  return rc;
}



///////////////////////////////////////////////////////////////////////////////
// *** DRIVER FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

int init_module( void )
{
  mutex_init( &mutex );

  // initialize model-specific variables (this also sets the modulename):
#ifdef ENABLE_COMPUTATION
  initModel();
#endif

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

  // initialize model-specific variables:
#ifdef ENABLE_COMPUTATION
  initModel();
#endif
  
  return 0;
}


int rtmodule_close( struct inode *devFile, struct file *fModule )
{
  int iS;
  // no subdevice specified? => stop & close all subdevices & comedi-devices:
  if ( reqCloseSubdevID < 0 ) {
    DEBUG_MSG( "close: no IOC_REQ_CLOSE request received - closing all subdevices...\n" );
    mutex_lock( &mutex );
    for ( iS = 0; iS < subdevN; iS++ ) {
      if ( stopSubdevice( iS, /*kill=*/1 ) )
        WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
      releaseSubdevice( iS );
    }
    mutex_unlock( &mutex );
    init_globals();
    return 0;
  }

  // stop & close specified subdevice (and device):
  mutex_lock( &mutex );
  if ( stopSubdevice( reqCloseSubdevID, 1 ) )
    WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", reqCloseSubdevID );
  releaseSubdevice( reqCloseSubdevID );
  mutex_unlock( &mutex );

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
  mutex_lock( &mutex );
  for ( iS = 0; iS < subdevN; iS++ ) {
    if ( stopSubdevice( iS, 1 ) )
      WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
    releaseSubdevice( iS );
  }
  mutex_unlock( &mutex );

  mutex_destroy( &mutex );

  // unregister module device file:
  unregister_chrdev( RTMODULE_MAJOR, moduleName );
}

