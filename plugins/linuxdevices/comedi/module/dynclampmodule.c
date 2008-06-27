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



#include <math.h>

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

// data types for buffer:
#define SAMPL_COMEDI 1
#define SAMPL_FLOAT  2


//* DAQ-DEVICES:

struct deviceT {
  comedi_t *devP;
  char name[DEV_NAME_MAXLEN+1];
};

struct chanT {
  comedi_t *devP;
  int subdev;
  unsigned int chan;
  int isParamChan;
  int aref;
  int rangeIndex;
  lsampl_t maxData;
  int minMuVolt, maxMuVolt;
  float scale;
};

enum subdevTypes { SUBDEV_OUT=0, SUBDEV_IN, SUBDEV_PARAM };

struct subdeviceT {
  int subdev;
  enum subdevTypes type;
  int devID;
  
  unsigned int fifo;

  unsigned int sampleType;  // SAMPL_COMEDI or SAMPL_FLOAT
  unsigned int sampleSize;
  
  int asyncMode;
  
  comedi_cmd *cmdP;
  unsigned int chanN;
  struct chanT *chanlist;

  unsigned int frequency;
  long duration;           // => relative to index of dynclamp-Task
  int continuous;

  int used;
  int prepared;
  int running;
  int error;               // E_COMEDI, E_NODATA, ...
};


//* RTAI TASK:

struct dynClampTaskT {
  RT_TASK rtTask;
  unsigned int periodLengthNs;
  int aquireMode;
  unsigned int reqFreq;
  unsigned long duration;
  int continuous;
  int running;
  unsigned long loopCnt;
  long aoIndex;
};

struct calcTaskT {
  RT_TASK rtTask;
  int algo;
  int initialized;
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

int inTraceIndex = 0;
int outTraceIndex = 0;

//* RTAI TASK:

struct dynClampTaskT dynClampTask;
struct calcTaskT calcTask;


// for debug:

char *iocNames[RTMODULE_IOC_MAXNR] = {
        "dummy",
        "IOC_GET_SUBDEV_ID", "IOC_GET_PARAM_ID", "IOC_OPEN_SUBDEV", "IOC_CHANLIST", "IOC_COMEDI_CMD", "IOC_SYNC_CMD", 
        "IOC_START_SUBDEV", "IOC_CHK_RUNNING", "IOC_REQ_READ", "IOC_REQ_WRITE", "IOC_REQ_CLOSE", "IOC_STOP_SUBDEV", 
        "IOC_RELEASE_SUBDEV", "IOC_GET_INTRACE_INFO", "IOC_GET_OUTTRACE_INFO", "IOC_GETLOOPCNT", "IOC_GETAOINDEX" 
};


///////////////////////////////////////////////////////////////////////////////
// *** PROTOTYPES ***
///////////////////////////////////////////////////////////////////////////////

int init_rt_task( int algorithm );
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
// *** MODEL INCLUDE ***
///////////////////////////////////////////////////////////////////////////////
#include "model.c"


///////////////////////////////////////////////////////////////////////////////
// *** HELPER FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

static inline float sample_to_value( struct chanT *pChan, lsampl_t sample )
{
  return 1.0e-6*pChan->scale * ( pChan->minMuVolt + 
         (pChan->maxMuVolt - pChan->minMuVolt) * 
         (sample/(float)pChan->maxData) );
}

static inline lsampl_t value_to_sample( struct chanT *pChan, float value )
{
  return (lsampl_t)( pChan->maxData * (value/pChan->scale*1.0e6 - pChan->minMuVolt) /
                     (pChan->maxMuVolt - pChan->minMuVolt) );
}

void init_globals( void ) {
  deviceN = 0;
  subdevN = 0;
  reqCloseSubdevID = -1;
  reqTraceSubdevID = -1;
  inTraceIndex = 0;
  outTraceIndex = 0;
  memset( device, 0, sizeof(device) );
  memset( subdev, 0, sizeof(subdev) );
  memset( &dynClampTask, 0, sizeof(struct dynClampTaskT ) );
  memset( &calcTask, 0, sizeof(struct calcTaskT ) );
  calcTask.algo = ALGO_PRESET;
}



///////////////////////////////////////////////////////////////////////////////
// *** DAQ FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

int getSubdevID( void )
{
  int i = 0;
  //* find free slot in subdev[]:
  for( i = 0; i < subdevN && subdev[i].used; i++ )
    ; // increment...
  if( i == subdevN ) {
    if( subdevN >= MAXSUBDEV ) {
      ERROR_MSG( "getSubdevID ERROR: number of requested subdevices exceeds MAXSUBDEV!\n" );
      return -1;
    }
    subdevN++;
  }
  memset( &(subdev[i]), 0, sizeof(struct subdeviceT) );
  subdev[i].used = 1;
  subdev[i].subdev = -1;
  subdev[i].devID = -1;
  subdev[i].sampleType = SAMPL_FLOAT; //SAMPL_COMEDI;
  subdev[i].sampleSize = sizeof(float); //sizeof(lsampl_t);
  return i;
}


int openComediDevice( struct deviceIOCT *deviceIOC )
{
  int iS, retVal;
  int i = 0;
  int iDev = -1;
  int justOpened = 0;

  // scan device list for either the opened device or a free slot:
  for( i = 0; i < deviceN; i++ ) {
    if( device[i].devP ) {
        if( strcmp( deviceIOC->devicename, device[i].name ) == 0 ) {
           DEBUG_MSG( "comediOpenDevice: device %s is already opened...", 
    		   device[i].name );
           iDev = i;
         	 goto locksubdevice;
        }
    }
    else {
       if( iDev < 0 && !device[i].devP ) {
           iDev = i;
           break;
       }
    }
  }

  if( i == deviceN ) {
    iDev = deviceN;
    if( deviceN >= MAXDEV ) {
      ERROR_MSG( "comediOpenDevice ERROR: number of requested devices exceeds MAXDEV!\n" );
      return -1;
    }
  }

  DEBUG_MSG( "openComediDevice: found device slot..\n" );

  // open comedi device:
  device[iDev].devP = comedi_open( deviceIOC->devicename );
  if( !device[iDev].devP ) {
    ERROR_MSG( "comediOpenDevice: device %s could not be opened!\n",
	        deviceIOC->devicename );
    comedi_perror( "rtmodule: rtmodule_ioctl" );    
    return -1;
  }
  justOpened = 1;

  DEBUG_MSG( "openComediDevice: opened device %s\n",  deviceIOC->devicename );

  // lock requested subdevice:
 locksubdevice:
  // TODO: check subdev list whether deviceIOC->subdev is already opened?
  //       => won't happen if userspace works correctly
  if( deviceIOC->subdev >= comedi_get_n_subdevices( device[iDev].devP ) ||
     comedi_lock( device[iDev].devP, deviceIOC->subdev ) != 0 ) {
    ERROR_MSG( "comediOpenDevice: Subdevice %i on device %s could not be locked!\n",
	       deviceIOC->subdev, device[iDev].name );
    // locking failed => close just opened comedi device:
    if( justOpened ) {
      if( comedi_close( device[iDev].devP ) < 0 )
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
  if( iDev == deviceN )
    deviceN++;

  DEBUG_MSG( "openComediDevice: locked subdevice %i on device %s\n", 
             deviceIOC->subdev, device[iDev].name );
  
  // initialize subdevice structure:
  iS = deviceIOC->subdevID;
  subdev[iS].subdev = deviceIOC->subdev;
  subdev[iS].devID= iDev;
  subdev[iS].type = deviceIOC->isOutput ? SUBDEV_OUT : SUBDEV_IN;

  // create FIFO for subdevice:
  subdev[iS].fifo = iS;
  retVal = rtf_create(subdev[iS].fifo, FIFO_SIZE);
  if( retVal )
    ERROR_MSG( "loadSyncCmd ERROR: Creating FIFO with %d bytes buffer failed for subdevice %i, device %s\n",
               FIFO_SIZE, iS, device[subdev[iS].devID].name );
  else
    DEBUG_MSG( "loadSyncCmd: Created FIFO with %d bytes buffer size for subdevice %i, device %s\n",
               FIFO_SIZE, iS, device[subdev[iS].devID].name );

  return 0;
}


int loadChanlist( struct chanlistIOCT *chanlistIOC )
{
  int iS = chanlistIOC->subdevID;
  int iD = subdev[iS].devID;
  int iC;
  comedi_krange krange;

  if( subdev[iS].subdev < 0 || !subdev[iS].used ) {
    ERROR_MSG( "loadChanlist ERROR: First open an appropriate device and subdevice. Chanlist not loaded!\n" );
    return -1;
  }

  if( chanlistIOC->chanlistN > MAXCHANLIST ) {
    ERROR_MSG( "loadChanlist ERROR: Invalid chanlist length for Subdevice %i on device %s. Chanlist not loaded!\n",
		 iS, device[subdev[iS].devID].name );
    return -1;
  }

  if( subdev[iS].chanlist )
    vfree( subdev[iS].chanlist );

  // create and initialize chanlist for subdevice:
  subdev[iS].chanlist =  vmalloc( chanlistIOC->chanlistN
                                  *sizeof(struct chanT) );
  if( !subdev[iS].chanlist ) {
    ERROR_MSG( "loadChanlist ERROR: Memory allocation for Subdevice %i on device %s. Chanlist not loaded!\n",
		 iS, device[subdev[iS].devID].name );
    return -1;
  }
  subdev[iS].chanN = chanlistIOC->chanlistN;

  for( iC = 0; iC < subdev[iS].chanN; iC++ ) {
    subdev[iS].chanlist[iC].devP = device[iD].devP;
    subdev[iS].chanlist[iC].subdev = subdev[iS].subdev;
    subdev[iS].chanlist[iC].chan = CR_CHAN( chanlistIOC->chanlist[iC] );
    subdev[iS].chanlist[iC].isParamChan = (subdev[iS].chanlist[iC].chan >= PARAM_CHAN_OFFSET);
    if( subdev[iS].chanlist[iC].isParamChan )
       subdev[iS].chanlist[iC].chan -= PARAM_CHAN_OFFSET;
    subdev[iS].chanlist[iC].aref = CR_AREF( chanlistIOC->chanlist[iC] );
    subdev[iS].chanlist[iC].rangeIndex = CR_RANGE( chanlistIOC->chanlist[iC] );
    subdev[iS].chanlist[iC].maxData = 
        comedi_get_maxdata( device[iD].devP, subdev[iS].subdev, 
                            CR_CHAN( chanlistIOC->chanlist[iC] ) );
    comedi_get_krange( device[iD].devP, subdev[iS].subdev, 
                       CR_CHAN( chanlistIOC->chanlist[iC] ),
                       CR_RANGE( chanlistIOC->chanlist[iC] ), &krange );
    subdev[iS].chanlist[iC].minMuVolt = krange.min;
    subdev[iS].chanlist[iC].maxMuVolt = krange.max;
    subdev[iS].chanlist[iC].scale = chanlistIOC->scalelist[iC];
  }

  return 0;
}


int loadSyncCmd( struct syncCmdIOCT *syncCmdIOC )
{
  int iS = syncCmdIOC->subdevID;

  if( subdev[iS].subdev < 0 || !subdev[iS].used ) {
    ERROR_MSG( "loadSyncCmd ERROR: First open an appropriate device and subdevice. Sync-command not loaded!\n" );
    return -EFAULT;
  }
  if( !subdev[iS].chanlist ) {
    ERROR_MSG( "loadSyncCmd ERROR: First load Chanlist for Subdevice %i on device %s. Sync-command not loaded!\n",
		 iS, device[subdev[iS].devID].name );
    return -EFAULT;
  }
  if( syncCmdIOC->frequency > MAX_FREQUENCY ) {
    ERROR_MSG( "LOAdSyncCmd ERROR: Requested frequency is above MAX_FREQUENCY (%d Hz). Sync-command not loaded!\n",
	       MAX_FREQUENCY );
    return -EINVAL;
  }

  // initialize sampling parameters for subdevice:
  subdev[iS].frequency= syncCmdIOC->frequency;
  subdev[iS].duration = syncCmdIOC->duration;
  subdev[iS].continuous = syncCmdIOC->continuous;

  // test requested sampling-rate and set frequency for dynamic clamp task:
  if( !dynClampTask.reqFreq || !dynClampTask.aquireMode ) {
    dynClampTask.reqFreq = subdev[iS].frequency;
    dynClampTask.aquireMode = 1;
  }
  else
    if( dynClampTask.reqFreq != subdev[iS].frequency ) {
      ERROR_MSG( "loadSyncCmd ERROR: Requested frequency %u Hz of subdevice %i on device %s is inconsistent to frequency %u Hz of other subdevice. Sync-command not loaded!\n",
		 subdev[iS].frequency, iS, device[subdev[iS].devID].name, dynClampTask.reqFreq );
      return -EINVAL;
    }


  subdev[iS].prepared = 1;
  return 0;
}


int startSubdevice( int iS )
{ 
  int retVal = 0;
  unsigned long firstLoopCnt, tmpDuration;

  if( !subdev[iS].prepared || subdev[iS].running ) {
    ERROR_MSG( "startSubdevice ERROR:  Subdevice ID %i on device %s either not prepared or already running.\n",
		 iS, device[subdev[iS].devID].name );
    return -EBUSY;
  }

  if( dynClampTask.running ) {
    // get current index of dynclamp loop in a thread-save way:
    do { 
    	firstLoopCnt = dynClampTask.loopCnt;
      tmpDuration = subdev[iS].duration + dynClampTask.loopCnt;
  	  dynClampTask.aoIndex = dynClampTask.loopCnt; 
    } 
    while( firstLoopCnt != dynClampTask.loopCnt );
    // set subdevice duration relative to index of dynclamp-Task
    subdev[iS].duration = tmpDuration; 
  }
  else {
	  dynClampTask.aoIndex = 0;
    dynClampTask.reqFreq = subdev[iS].frequency;

    // start dynamic clamp task: 
    retVal = init_rt_task( calcTask.algo );
    if( retVal < 0 ) {
      subdev[iS].running = 0;
      return -ENOMEM;
    }
  }

  subdev[iS].running = 1;
  return 0;

}


int subdevRunning( int iS )
{
  return subdev[iS].running;
}


int stopSubdevice( int iS )
{ 
  int i;

  if( !subdevRunning( iS ) )
    return 0;
  subdev[iS].running = 0;

  // if all subdevices stopped => halt dynclamp task:
  for( i = 0; i < subdevN; i++ )
    if( subdev[i].running )
    	return 0;
  cleanup_rt_task();
  return 0;
}


void releaseSubdevice( int iS )
{
  int iD = subdev[iS].devID;
  int i;

  if( !subdev[iS].used || subdev[iS].subdev < 0 ) {
    ERROR_MSG( "releaseSubdevice ERROR: Subdevice with ID %d not in use!\n", 
	       iS );
    return;
  }

  // stop subdevice:
  if( subdev[iS].running )
    stopSubdevice( iS );
  
  // unlock subdevice:
  if( device[iD].devP && comedi_unlock( device[iD].devP, subdev[iS].subdev ) < 0 )
    WARN_MSG( "releaseSubdevice WARNING: unlocking of subdevice %s failed!\n",
	      device[iD].name );
  else
    DEBUG_MSG( "releaseSubdevice: Unlocking of subdevice %s was successful!\n",
	       device[iD].name );

  if(  subdev[iS].chanlist )
    vfree(  subdev[iS].chanlist );
  if(  subdev[iS].cmdP )
    vfree(  subdev[iS].cmdP );

  // delete FIFO and reset subdevice structure:
  rtf_destroy( subdev[iS].fifo );
  memset( &(subdev[iS]), 0, sizeof(struct subdeviceT) );
  if( iS == subdevN - 1 )
    subdevN--;
  subdev[iS].devID = -1;
 
  // check if comedi device for subdevice is still in use:
  for( i = 0; i < subdevN; i++ ) {
    if( subdev[i].devID == iD )
      // device is still used by another subdevice => leave here:
      return;
  }

  // close comedi device:
  DEBUG_MSG( "YYYYYYY releaseSubdevice: released device for last subdev-ID %d\n", iS );
  if( device[iD].devP && comedi_close( device[iD].devP ) < 0 )
    WARN_MSG( "releaseSubdevice WARNING: closing of device %s failed!\n",
	      device[iD].name );
  else
    DEBUG_MSG( "releaseSubdevice: Closing of device %s was successful!\n",
	       device[iD].name );

  // reset device structure:
  memset( &(device[iD]), 0, sizeof(struct deviceT) );
  if( iD == deviceN-1 )
      deviceN--;
}



///////////////////////////////////////////////////////////////////////////////
// *** REAL-TIME TASKS *** 
///////////////////////////////////////////////////////////////////////////////


/*! Calcuclation task for Euler */
void rtEulerCalc( long dummy )
{
  // TODO: Aus Quellcode von Diplomarbeit rausholen...
}


/*! Dynamic clamp task */
void rtDynClamp( long dummy )
{
  int retVal;
  int iS, iC;
  int subdevRunning = 1;
  unsigned long readCnt = 0;
  float voltage;
  unsigned long fifoPutCnt = 0;
  lsampl_t lsample;

  DEBUG_MSG( "rtDynClamp: starting dynamic clamp loop at %u Hz\n", 
	           1000000000/dynClampTask.periodLengthNs );

  rt_sleep( nano2count( 1000000 ) ); // FOR TESTING...

  dynClampTask.loopCnt = 0;
  dynClampTask.aoIndex = -1;
  dynClampTask.running = 1;

  while( subdevRunning ) {
    
    subdevRunning = 0;


//******** WRITE TO ANALOG OUTPUT: *******************************************/
//****************************************************************************/
    // TODO: implement efficiently as frequently updated chanT-list of used traces
    for( iS = 0; iS < subdevN; iS++ )
      if( subdev[iS].running && subdev[iS].type == SUBDEV_OUT ) {

          // check duration:
          if( !subdev[iS].continuous &&
              subdev[iS].duration <= dynClampTask.loopCnt ) {
            subdev[iS].running = 0;
            DEBUG_MSG( "rtDynClamp: ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ loopCnt exceeded duration for subdevice ID %d at loopCnt %lu\n",
                 iS, dynClampTask.loopCnt );
      	    continue;
          }
      	  else // DEBUG
	          if( iS == 1 && dynClampTask.loopCnt%100 == 0 )
          		DEBUG_MSG( " duration = %lu, loopCnt = %lu\n", subdev[iS].duration, dynClampTask.loopCnt );

          subdevRunning = 1;
         
          // FOR EVERY CHAN...
          for( iC = 0; iC < subdev[iS].chanN; iC++ ) {

            // get data from FIFO:
            retVal = rtf_get( subdev[iS].fifo, &voltage, sizeof(voltage) );
            if( retVal != sizeof(voltage) ) {
              if( retVal == EINVAL ) {
                DEBUG_MSG( "rtDynClamp: No open FIFO for subdevice ID %d at loopCnt %lu\n",
                           iS, dynClampTask.loopCnt );
                dynClampTask.running = 0;
                dynClampTask.duration = 0;
                return;
              }
              subdev[iS].error = E_UNDERRUN;
              DEBUG_MSG( "rtDynClamp: Data buffer underrun for AO subdevice ID %d at loopCnt %lu\n",
                         iS, dynClampTask.loopCnt );
              subdev[iS].running = 0;
              continue;
            }

            // write out Sample:
            lsample = value_to_sample( &subdev[iS].chanlist[iC], voltage );
            retVal = comedi_data_write( device[subdev[iS].devID].devP, 
                                        subdev[iS].subdev, 
                                        subdev[iS].chanlist[iC].chan,
                                        subdev[iS].chanlist[iC].rangeIndex,
                                        subdev[iS].chanlist[iC].aref,
                                        lsample
                                       );
            if( retVal < 1 ) {
              subdev[iS].running = 0;
              if( retVal < 0 ) {
                comedi_perror( "rtmodule: rtDynClamp: comedi_data_write" );
                subdev[iS].error = E_COMEDI;
		            subdev[iS].running = 0;
                //spin_unlock( &subdev[iS].bData.spinlock );
                continue;
              }
              subdev[iS].error = E_NODATA;
              DEBUG_MSG( "rtDynClamp: E_NODATA for subdevice ID %d channel %d at loopCnt %lu\n",
                   iS, iC, dynClampTask.loopCnt );
            }

          } // end of chan loop

      } // end of device loop

    

//******** SLEEP FOR NEURON TO REACT TO GIVEN OUTPUT: ************************/
//****************************************************************************/
    //* PROBLEM: rt_sleep is timed using jiffies only (granularity = 1msec)
    //* int retValSleep = rt_sleep( nano2count( INJECT_RECORD_DELAY ) );
    rt_busy_sleep( INJECT_RECORD_DELAY ); // TODO: just default



    // DEBUG OUTPUT:
    if( dynClampTask.loopCnt % 10 == 0 ) {
      DEBUG_MSG( "%d subdevices registered:\n", subdevN );
      for( iS = 0; iS < subdevN; iS++ ) {
        DEBUG_MSG( "LOOP %lu: fifoPutCnt=%lu readCnt=%lu, subdevID=%d, run=%d, type=%d, error=%d, duration=%lu, contin=%d\n", 
            		   dynClampTask.loopCnt, fifoPutCnt, readCnt, iS, subdev[iS].running, 
                   subdev[iS].type, subdev[iS].error, subdev[iS].duration, subdev[iS].continuous  );
      }
    }




//******** READ FROM ANALOG INPUT: *******************************************/
//****************************************************************************/
    // TODO: implement efficiently as frequently updated chanT-list of used traces
    for( iS = 0; iS < subdevN; iS++ )
      if( !subdev[iS].asyncMode && subdev[iS].running && 
            subdev[iS].type == SUBDEV_IN ) {
          
          // check duration:
          if( !subdev[iS].continuous &&
              subdev[iS].duration <= dynClampTask.loopCnt ) {
            subdev[iS].running = 0;
            DEBUG_MSG( "rtDynClamp: loopCnt exceeded duration for subdevice ID %d at loopCnt %lu\n",
                 iS, dynClampTask.loopCnt );
          }
          subdevRunning = 1;

          // FOR EVERY CHAN...
          for( iC = 0; iC < subdev[iS].chanN; iC++ ) {

            // acquire sample:
            retVal = comedi_data_read( device[subdev[iS].devID].devP, 
                                       subdev[iS].subdev, 
                                       subdev[iS].chanlist[iC].chan,
                                       subdev[iS].chanlist[iC].rangeIndex,
                                       subdev[iS].chanlist[iC].aref,
                                       &lsample );
            if( retVal < 0 ) {
              subdev[iS].running = 0;
              comedi_perror( "rtmodule: rtDynClamp: comedi_data_write" );
              subdev[iS].error = E_COMEDI;
              DEBUG_MSG( "rtDynClamp: E_NODATA for subdevice ID %d channel %d at loopCnt %lu\n",
                   iS, iC, dynClampTask.loopCnt );
              continue;
            }
            // write to FIFO:
            voltage = sample_to_value( &subdev[iS].chanlist[iC], lsample );
            retVal = rtf_put( subdev[iS].fifo, &voltage, sizeof(voltage) );
            fifoPutCnt++;
            if( retVal != sizeof(voltage) ) {
              if( retVal == EINVAL ) {
                DEBUG_MSG( "rtDynClamp: No open FIFO for subdevice ID %d at loopCnt %lu\n",
                           iS, dynClampTask.loopCnt );
                dynClampTask.running = 0;
                dynClampTask.duration = 0;
                return;
              }
              subdev[iS].error = E_OVERFLOW;
              DEBUG_MSG( "rtDynClamp: Data buffer overflow for AI subdevice ID %d at loopCnt %lu\n",
                         iS, dynClampTask.loopCnt );
              subdev[iS].running = 0;
              continue;
            }

          } // end of chan loop
          readCnt++; // FOR DEBUG
      } // end of device loop



//******** WAIT FOR CALCULATION TASK TO COMPUTE RESULT: **********************/
//****************************************************************************/
    dynClampTask.loopCnt++;

    //    start = rt_get_cpu_time_ns();
    rt_task_wait_period();

    // ...as soon as RTAI scheduler wakes us up again:


//******** SUSPEND CALCULATION TASK: *****************************************/
//****************************************************************************/
    if( calcTask.initialized )
      rt_task_suspend( &calcTask.rtTask );



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
int init_rt_task( int algorithm )
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
  if( dynClampTask.reqFreq <= 0 || dynClampTask.reqFreq > MAX_FREQUENCY )
    ERROR_MSG( "init_rt_task ERROR: %dHz -> invalid dynamic clamp frequency. Valid range is 1 .. %dHz\n", 
	       dynClampTask.reqFreq, dynClampTask.reqFreq );

 //* initializing rt-task for dynamic clamp with high priority:
  priority = 1;
  retVal = rt_task_init( &dynClampTask.rtTask, rtDynClamp, dummy, stackSize, 
			 priority, usesFPU, signal );
  if( retVal ) {
    ERROR_MSG( "init_rt_task ERROR: failed to initialize real-time task for dynamic clamp! stacksize was set to %d bytes.\n", 
	       stackSize );
    return -1;
  }
  DEBUG_MSG( "init_rt_task: Initialized dynamic clamp RTAI task. Trying to make it periodic...\n" );

 //* initializing rt-task for calculation with lower:
/*  priority = 2;
  retVal = rt_task_init( &calcTask.rtTask, rtEulerCalc, dummy, stackSize, 
			 priority, usesFPU, signal );
  if( retVal ) {
    ERROR_MSG( "init_rt_task ERROR: failed to initialize real-time task for calculation! stacksize was set to %d bytes.\n", 
	       stackSize );
    return -2;
  }
*/
 //* START rt-task for dynamic clamp as periodic:
  periodTicks = start_rt_timer( nano2count( 1000000000/dynClampTask.reqFreq ) );  
  if( rt_task_make_periodic( &dynClampTask.rtTask, rt_get_time(), periodTicks ) 
      != 0 ) {
    printk( "init_rt_task ERROR: failed to start periodic real-time task for data acquisition! loading of module failed!\n" );
    return -3;
  }
  dynClampTask.periodLengthNs = count2nano( periodTicks );
  INFO_MSG( "init_rt_task: periodic task successfully started... requested freq: %d , accepted freq: ~%u (period=%uns)\n", 
	    dynClampTask.reqFreq, 1000000000 / dynClampTask.periodLengthNs, 
	    dynClampTask.periodLengthNs );

  // For now, the DynClampTask shall always run until any subdev is stopped:
  dynClampTask.continuous = 1;

  return 0;
}

// TODO: add stop_rt_task?
void cleanup_rt_task( void )
{
  stop_rt_timer();
  DEBUG_MSG( "cleanup_rt_task: stopped periodic task\n" );

  rt_task_delete( &calcTask.rtTask );
  calcTask.initialized = 0;
  rt_task_delete( &dynClampTask.rtTask );
  memset( &dynClampTask, 0, sizeof(struct dynClampTaskT) );
  memset( &calcTask, 0, sizeof(struct calcTaskT) );
}





///////////////////////////////////////////////////////////////////////////////
// *** IOCTL ***
///////////////////////////////////////////////////////////////////////////////

int rtmodule_ioctl( struct inode *devFile, struct file *fModule, 
		    unsigned int cmd, unsigned long arg )
{
  struct deviceIOCT deviceIOC;
  struct chanlistIOCT chanlistIOC;
  struct syncCmdIOCT syncCmdIOC;
  struct traceInfoIOCT traceInfo;

  int tmp, subdevID;
  int retVal;
  unsigned long luTmp;


  if( _IOC_TYPE(cmd) != RTMODULE_MAJOR || _IOC_NR(cmd) > RTMODULE_IOC_MAXNR) {
     ERROR_MSG( " ioctl: Major wrong or ioctl %d bigger than max %d\n", 
                 _IOC_TYPE(cmd), RTMODULE_IOC_MAXNR );
     return -ENOTTY;
  }
  DEBUG_MSG( "ioctl: user triggered ioctl %d %s\n",_IOC_NR( cmd ), iocNames[_IOC_NR( cmd )] );


  // TODO: use access_ok for verifying userspace mem. 
  //       => (see Linux-Device-Drivers-3-Book, Chapter 6)

  switch( cmd ) {
    

//******** GIVE INFORMATION TO USER SPACE: ***********************************/

  case IOC_GETAOINDEX:
    luTmp = dynClampTask.aoIndex;
    if( luTmp < 0 )
      return -ENOSPC;
    retVal = put_user( luTmp, (unsigned long __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;

  case IOC_GETLOOPCNT:
    luTmp = dynClampTask.loopCnt;
    if( luTmp < 0 )
      return -ENOSPC;
    retVal = put_user( luTmp, (unsigned long __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;


//******** SET UP COMEDI: ****************************************************/

  case IOC_GET_PARAM_ID:
    tmp = getSubdevID();
    subdev[tmp].type = SUBDEV_PARAM;
    subdev[tmp].sampleType = SAMPL_FLOAT;
    subdev[tmp].sampleSize = sizeof(float);
    if( tmp < 0 )
      return -ENOSPC;
    retVal = put_user( tmp, (int __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;


  case IOC_GET_SUBDEV_ID:
    tmp = getSubdevID();
    if( tmp < 0 )
      return -ENOSPC;
    retVal = put_user( tmp, (int __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;

    
  case IOC_OPEN_SUBDEV:
    retVal = copy_from_user( &deviceIOC, (void __user *)arg, sizeof(struct deviceIOCT) );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to deviceIOCT-struct!\n" );
      return -EFAULT;
    }
    if( deviceIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in deviceIOCT-struct!\n" );
      return -EFAULT;
    }
    retVal = openComediDevice( &deviceIOC );
    return retVal == 0 ? 0 : -EFAULT;


  case IOC_CHANLIST:
    retVal = copy_from_user( &chanlistIOC, (void __user *)arg, sizeof(struct chanlistIOCT) );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to chanlistIOCT-struct!\n" );
      return -EFAULT;
    }
    if( chanlistIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in chanlistIOCT-struct!\n" );
      return -EFAULT;
    }
    retVal = loadChanlist( &chanlistIOC );
    return retVal == 0 ? 0 : -EFAULT;
    

  case IOC_SYNC_CMD:
    retVal = copy_from_user( &syncCmdIOC, (void __user *)arg, sizeof(struct syncCmdIOCT) );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to syncCmdIOCT-struct!\n" );
      return -EFAULT;
    }
    if( syncCmdIOC.subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in syncCmdIOCT-struct!\n" );
      return -EFAULT;
    }
    retVal = loadSyncCmd( &syncCmdIOC );
    return retVal;


  case IOC_GET_INTRACE_INFO:
    DEBUG_MSG( "rtmodule_ioctl: inTraceIndex: %d (ERANGE=%d)\n", inTraceIndex, ERANGE );
    if( inputUnits[inTraceIndex] == NULL ) {
      inTraceIndex = 0;
      return -ERANGE; // Ende der Liste signalisieren
    }
    strncpy( traceInfo.name, inputNames[inTraceIndex], PARAM_NAME_MAXLEN );
    strncpy( traceInfo.unit, inputUnits[inTraceIndex], PARAM_NAME_MAXLEN );
    retVal = __copy_to_user( (void __user *)arg, &traceInfo, sizeof(struct traceInfoIOCT) );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid user pointer for traceInfoIOCT!\n" );
      return -EFAULT;
    }
    inTraceIndex++;
    return 0;

  case IOC_GET_OUTTRACE_INFO:
    if( outputUnits[outTraceIndex] == NULL ) {
      outTraceIndex = 0;
      return -ERANGE; // Ende der Liste signalisieren
    }
    strncpy( traceInfo.name, outputNames[outTraceIndex], PARAM_NAME_MAXLEN );
    strncpy( traceInfo.unit, outputUnits[outTraceIndex], PARAM_NAME_MAXLEN );
    retVal = __copy_to_user( (void __user *)arg, &traceInfo, sizeof(struct traceInfoIOCT) );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid user pointer for traceInfoIOCT!\n" );
      return -EFAULT;
    }
    outTraceIndex++;
    return 0;


  case IOC_START_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for start-query!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for start-query!\n" );
      return -EFAULT;
    }
    retVal = startSubdevice( subdevID );
    return retVal;


  case IOC_CHK_RUNNING:
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for running-query!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for running-query!\n" );
      return -EFAULT;
    }
    tmp = subdevRunning( subdevID );
    DEBUG_MSG( "rtmodule_ioctl: running = %d for subdevID %d\n", tmp, subdevID );
    retVal = put_user( tmp, (int __user *)arg );
    return retVal == 0 ? 0 : -EFAULT;


  case IOC_REQ_CLOSE:
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for close-request!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for close-request!\n" );
      return -EFAULT;
    }
    if( reqCloseSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_CLOSE ERROR: Another close-request in progress!\n" );
      return -EAGAIN;
    }
    reqCloseSubdevID = subdevID;
    return 0;

  case IOC_REQ_READ: // Noch wichtig fuer tracename-List?
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for read-request!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for read-request!\n" );
      return -EFAULT;
    }
    if( reqTraceSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ ERROR: Another read-request in progress! (reqTraceSubdevID=%d)\n", reqTraceSubdevID );
      return -EAGAIN;
    }
    ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ: Requested Read\n" );
    reqTraceSubdevID = subdevID;
    return 0;


  case IOC_STOP_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for stop-query!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for stop-query!\n" );
      return -EFAULT;
    }
    retVal = stopSubdevice( subdevID );
    DEBUG_MSG( "rtmodule_ioctl: stopSubdevice returned %u\n", retVal );
    return retVal == 0 ? 0 : -EFAULT;


  case IOC_RELEASE_SUBDEV:
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for release-query!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for release-query!\n" );
      return -EFAULT;
    }
    releaseSubdevice( subdevID );
    return 0;


  }


  ERROR_MSG( "rtmodule_ioctl ERROR - Invalid IOCTL!\n" );

  return -EINVAL;
}



///////////////////////////////////////////////////////////////////////////////
// *** DRIVER FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////

int init_module( void )
{

  // initialize model-specific variables:
  initModel();

  // register module device file:
  // TODO: adapt to kernel 2.6 convention (see char-device chapter in Linux device drivers 3)
  if(register_chrdev( RTMODULE_MAJOR, moduleName, &fops ) != 0) {
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
  if( reqCloseSubdevID < 0 ) {
    DEBUG_MSG( "close: no IOC_REQ_CLOSE request received - closing all subdevices...\n" );
    for( iS = 0; iS < subdevN; iS++ ) {
      if( stopSubdevice( iS ) )
        WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
      releaseSubdevice( iS );
    }
    init_globals();
    return 0;
  }

  // stop & close specified subdevice (and device):
  if( stopSubdevice( reqCloseSubdevID ) )
      WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", reqCloseSubdevID );
  releaseSubdevice( reqCloseSubdevID );

  if( deviceN == 0 )
    init_globals();
  reqCloseSubdevID = -1;

  DEBUG_MSG( "close: user closed device file\n" );
  return 0;
}


void cleanup_module( void )
{
  int iS;
  INFO_MSG( "cleanup_module: dynamic clamp module %s unloaded\n", moduleName );

  //if( paramValues )
  //  vfree( paramValues );

  // stop and release all subdevices & comedi-devices:
  for( iS = 0; iS < subdevN; iS++ ) {
    if( stopSubdevice( iS ) )
      WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
    releaseSubdevice( iS );
  }

  // unregister module device file:
  unregister_chrdev( RTMODULE_MAJOR, moduleName );
}

