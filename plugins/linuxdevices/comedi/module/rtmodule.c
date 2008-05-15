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

#include "model.h"
#include "rtmodule.h"
#include "blocklist.h"


MODULE_LICENSE( "GPL" );



// subdevice acquisition errors:
#define E_COMEDI -1
#define E_NODATA -2

// data types for buffer:
#define SAMPL_COMEDI 1
#define SAMPL_FLOAT  2


///////////////////////////////////////////////////////////////////////////////
// *** TYPE DEFINITIONS ***
///////////////////////////////////////////////////////////////////////////////


//* DEVICE DATA:


//* DAQ-DEVICES:

struct deviceT {
  comedi_t *devP;
  char name[DEV_NAME_MAXLEN+1];
  int asyncUsed;
};

struct chanT {
  comedi_t *devP;
  int subdev;
  unsigned int chan;
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

  struct BlockDataT bData;
  unsigned int sampleType;  // SAMPL_COMEDI or SAMPL_FLOAT
  unsigned int sampleSize;
  
  int asyncMode;
  
  comedi_cmd *cmdP;
  unsigned int *comedi_chanlist;
  unsigned int chanN;
  struct chanT *chanlist;

  unsigned int frequency;
  long duration;
  int continuous;
  void *dmaBuf;            // only for async. acquisition...
  int dmaBufSize;

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
  unsigned int dmaUpdateRatio; // 0 = no async. acquisition 
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
// *** GLOBAL DECLARATIONS ***
///////////////////////////////////////////////////////////////////////////////


//* DAQ-DEVICES:

struct deviceT device[MAXDEV];
int deviceN = 0;

struct subdeviceT subdev[MAXSUBDEV];
int subdevN = 0;

int reqCloseSubdevID = -1;
int reqReadSubdevID = -1;
int reqWriteSubdevID = -1;

// index-pointers for PARAM(), INPUT(), OUTPUT():
struct pIndexT pIndexParam[MAXCHANLIST*MAXSUBDEV];
struct pIndexT pIndexOut[MAXCHANLIST*MAXSUBDEV];
struct pIndexT pIndexIn[MAXCHANLIST*MAXSUBDEV];

extern char *moduleName;
extern char *paramnames[PARAM_N];
extern char *inputnames[INPUT_N];
extern char *outputnames[OUTPUT_N];

//* RTAI TASK:

struct dynClampTaskT dynClampTask;
struct calcTaskT calcTask;

// *** HELPER FUNCTIONS ***

// TODO: reimplement as #define using channel-specific range-info from struct


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



int init_rt_task( int algorithm );
void cleanup_rt_task( void );
static int updateDMABuf( int iS );


///////////////////////////////////////////////////////////////////////////////
// *** DAQ FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////


int getSubdevID( void )
{
  int i;
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
  int i;
  int iDev = -1;
  int justOpened = 0;

  DEBUG_MSG( "openComediDevice: entering..\n" );
   //* find free slot in device[] and check if device is already opened:
  for( i = 0; i < deviceN; i++ ) {
    if( iDev < 0 && !device[i].devP )
      iDev = i;
    if( strcmp( deviceIOC->devicename, device[i].name ) == 0 &&
	      device[i].devP ) {
       DEBUG_MSG( "comediOpenDevice: device %s is already opened...", 
		   device[i].name );
       iDev = i;
     	 goto locksubdevice;
    }
  }
  if( i == deviceN ) {
    iDev = deviceN;
    if( deviceN >= MAXDEV ) {
      ERROR_MSG( "comediOpenDevice ERROR: number of requested devices exceeds MAXDEV!\n" );
      return -1;
    }
    deviceN++;
  }

  DEBUG_MSG( "openComediDevice: found device slot..\n" );

  //* open device:
  device[iDev].devP = comedi_open( deviceIOC->devicename );
  if( !device[iDev].devP ) {
    ERROR_MSG( "comediOpenDevice: device %s could not be opened!\n",
	        deviceIOC->devicename );
    comedi_perror( "rtmodule: rtmodule_ioctl" );    
    return -1;
  }
  justOpened = 1;

  DEBUG_MSG( "openComediDevice: opened device %s\n",  deviceIOC->devicename );

 locksubdevice:
  // TODO: check via subdev[] whether deviceIOC->subdev 
  //       is already opened?
  if( deviceIOC->subdev >= comedi_get_n_subdevices( device[iDev].devP ) ||
     comedi_lock( device[iDev].devP, deviceIOC->subdev ) != 0 ) {
    ERROR_MSG( "comediOpenDevice: Subdevice %i on device %s could not be locked!\n",
	       deviceIOC->subdev, device[iDev].name );
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

  strncpy( device[iDev].name, deviceIOC->devicename, DEV_NAME_MAXLEN );

  DEBUG_MSG( "openComediDevice: locked subdevice %i on device %s\n", 
             deviceIOC->subdev, device[iDev].name );
  
  subdev[deviceIOC->subdevID].subdev = deviceIOC->subdev;
  subdev[deviceIOC->subdevID].devID= iDev;
  subdev[deviceIOC->subdevID].type = deviceIOC->isOutput ? SUBDEV_OUT : SUBDEV_IN;

  DEBUG_MSG( "openComediDevice: successfully leaving..\n" );
  
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

  if( chanlistIOC->chanlistN > MAXCHANLIST )
    ERROR_MSG( "loadChanlist ERROR: Invalid chanlist length for Subdevice %i on device %s. Chanlist not loaded!\n",
		 iS, device[subdev[iS].devID].name );

/***************************************************************************************************************************
  if( subdev[iS].comedi_chanlist )
    vfree( subdev[iS].comedi_chanlist );
  if( subdev[iS].chanlist )
    vfree( subdev[iS].chanlist );
***************************************************************************************************************************/

  subdev[iS].comedi_chanlist =  vmalloc( chanlistIOC->chanlistN
                                         *sizeof(unsigned int) );
  subdev[iS].chanlist =  vmalloc( chanlistIOC->chanlistN
                                  *sizeof(struct chanT) );
  if( !subdev[iS].comedi_chanlist || !subdev[iS].chanlist ) {
    ERROR_MSG( "loadChanlist ERROR: Memory allocation for Subdevice %i on device %s. Chanlist not loaded!\n",
		 iS, device[subdev[iS].devID].name );
    return -1;
  }
  memcpy( subdev[iS].comedi_chanlist, &chanlistIOC->chanlist, 
	  chanlistIOC->chanlistN*sizeof(unsigned int) );
  subdev[iS].chanN = chanlistIOC->chanlistN;

  for( iC = 0; iC < subdev[iS].chanN; iC++ ) {
    subdev[iS].chanlist[iC].devP = device[iD].devP;
    subdev[iS].chanlist[iC].subdev = subdev[iS].subdev;
    subdev[iS].chanlist[iC].chan = CR_CHAN( chanlistIOC->chanlist[iC] );
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


int loadComediCmd( struct comediCmdIOCT *comediCmdIOC )
{
  int iS = comediCmdIOC->subdevID;
  int iD = subdev[iS].devID;
  int retVal, blockNum;
  
  if( subdev[iS].subdev < 0 || !subdev[iS].used ) {
    ERROR_MSG( "loadComediCmd ERROR: First open an appropriate device and subdevice. Comedi-command not loaded!\n" );
    return -1;
  }
  if( !subdev[iS].comedi_chanlist ) {
    ERROR_MSG( "loadComediCmd ERROR: First load Chanlist for subdevice %i on device %s. Comedi-command not loaded!\n",
		 iS, device[iD].name );
    return -1;
  }

  // map DMA-buffer to our pointer:
  retVal = comedi_map( device[iD].devP, subdev[iS].subdev, &subdev[iS].dmaBuf );
  if( retVal < 0 || !subdev[iS].dmaBuf ) {
    ERROR_MSG( "loadComediCmd ERROR: Mapping of DMA-buffer for subdevice %i on device %s failed. Comedi-command not loaded!\n",
		 iS, device[iD].name );
    return -1;
  }
  subdev[iS].dmaBufSize = comedi_get_buffer_size( device[iD].devP, 
						  subdev[iS].subdev );
  if( subdev[iS].dmaBufSize <= 0 ) {
    ERROR_MSG( "loadComediCmd ERROR: No success while retrieving the size of the DMA-buffer for subdevice %i on device %s failed. Comedi-command not loaded!\n",
		 iS, device[iD].name );
    return -1;
  }

   
  // test command structure given from user space:
  if( !subdev[iS].cmdP )
    subdev[iS].cmdP =  vmalloc( sizeof(comedi_cmd) );
  if( !subdev[iS].cmdP ) {
    ERROR_MSG( "loadComediCmd ERROR: Memory allocation for subdevice %i on device %s failed. Comedi-command not loaded!\n",
		 iS, device[iD].name );
    return -1;
  }
  memcpy( subdev[iS].cmdP, &(comediCmdIOC->cmd), sizeof(comedi_cmd) );
  subdev[iS].cmdP->chanlist = subdev[iS].comedi_chanlist;
  subdev[iS].cmdP->chanlist_len = subdev[iS].chanN;
  
  retVal = comedi_command_test( device[iD].devP, subdev[iS].cmdP );
  if( retVal )
    retVal = comedi_command_test( device[iD].devP, subdev[iS].cmdP );
  if( retVal )
    retVal = comedi_command_test( device[iD].devP, subdev[iS].cmdP );
  if( retVal ) {    
    ERROR_MSG( "loadComediCmd ERROR: Testing of the comedi command for subdevice %i on device %s failed. Comedi-command not loaded!\n",
		 iS, device[iD].name );
    return -1;
  }

  subdev[iS].frequency = 1000000000 / subdev[iS].cmdP->scan_begin_arg;
  subdev[iS].duration = subdev[iS].cmdP->stop_arg;
  subdev[iS].continuous = ( subdev[iS].cmdP->stop_src == TRIG_NONE );
  
  // reserve memory for data block-list:
  if( !subdev[iS].bData.readBlock ) {
	  blockNum = ( BUFFERLEN/1000*subdev[iS].frequency*subdev[iS].chanN
		       *subdev[iS].sampleSize ) / DATABLOCKSIZE + 1;  
	  if( blockNum > MAXBUFFERBLOCKS )
	    blockNum = MAXBUFFERBLOCKS;
	  if( blockNum < 1 )
	    blockNum = 1;
	    retVal = createBlockList( &subdev[iS].bData, blockNum, 
				      subdev[iS].chanN, subdev[iS].sampleSize );
	  if( retVal < 1 ) {
	    WARN_MSG( "loadComediCmd ERROR: No data block could be reserved for subdevice %i on device %s\n", 
		      iS, device[subdev[iS].devID].name );
	    return -1;
	  }
	  if( retVal < blockNum )
	    WARN_MSG( "loadComediCmd WARNING: Requested number %d of data blocks for subdevice %i on device %s\n", 
		      blockNum, iS, device[subdev[iS].devID].name );
  }
  subdev[iS].asyncMode = 1;
  subdev[iS].prepared = 1;
  return 0;
}


int loadSyncCmd( struct syncCmdIOCT *syncCmdIOC )
{
  int iS = syncCmdIOC->subdevID;
  int blockNum, retVal;
  unsigned long fifoSize;

  if( subdev[iS].subdev < 0 || !subdev[iS].used ) {
    ERROR_MSG( "loadSyncCmd ERROR: First open an appropriate device and subdevice. Sync-command not loaded!\n" );
    return -EFAULT;
  }
  if( !subdev[iS].comedi_chanlist ) {
    ERROR_MSG( "loadSyncCmd ERROR: First load Chanlist for Subdevice %i on device %s. Sync-command not loaded!\n",
		 iS, device[subdev[iS].devID].name );
    return -EFAULT;
  }

  if( syncCmdIOC->frequency > MAX_FREQUENCY ) {
    ERROR_MSG( "LOAdSyncCmd ERROR: Requested frequency is above MAX_FREQUENCY (%d Hz). Sync-command not loaded!\n",
	       MAX_FREQUENCY );
    return -EINVAL;
  }

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

  fifoSize = (BUFFERLEN*subdev[iS].frequency*subdev[iS].chanN*subdev[iS].sampleSize)/1000;
  subdev[iS].fifo = iS;
  retVal = rtf_create(subdev[iS].fifo, 200000);
  if( retVal )
    ERROR_MSG( "loadSyncCmd ERROR: Creating FIFO with %lu bytes buffer failed for subdevice %i, device %s\n",
               fifoSize, iS, device[subdev[iS].devID].name );
  else
    DEBUG_MSG( "loadSyncCmd: Created FIFO with %lu bytes buffer size for subdevice %i, device %s\n",
               fifoSize, iS, device[subdev[iS].devID].name );


  // reserve memory for data block-list:
/*  DEBUG_MSG( "loadSyncCmd: ReadBlock status: %p\n", subdev[iS].bData.readBlock );
  if( !subdev[iS].bData.readBlock ) {
	blockNum = ( BUFFERLEN/1000*subdev[iS].frequency*(subdev[iS].chanN)
	       *subdev[iS].sampleSize ) / DATABLOCKSIZE + 1;  
	if( blockNum > MAXBUFFERBLOCKS )
	blockNum = MAXBUFFERBLOCKS;
	if( blockNum < 1 )
	blockNum = 1;
	retVal = createBlockList( &subdev[iS].bData, blockNum, 
					  subdev[iS].chanN, subdev[iS].sampleSize );
  	DEBUG_MSG( "loadSyncCmd: ReadBlock allocated: %p\n", subdev[iS].bData.readBlock );
	if( retVal < 1 ) {
	WARN_MSG( "loadSyncCmd ERROR: No data block could be reserved for subdevice %i on device %s\n", 
	      iS, device[subdev[iS].devID].name );
	return -ENOMEM;
	}
	if( retVal < blockNum )
	WARN_MSG( "loadSyncCmd WARNING: Requested number %d of data blocks for subdevice %i on device %s\n", 
	      blockNum, iS, device[subdev[iS].devID].name );  
  }*/
  subdev[iS].asyncMode = 0;
  subdev[iS].prepared = 1;
  return 0;
}


int mapTracenames( int iS, struct traceNameIOCT *traces, int traceN )
{
  int iTrace, iDef;
  struct pIndexT *pIndex = pIndexParam;
  char **defTracenames = paramnames;
  int defTraceN = PARAM_N;
  //TODO: Support paramnames
  if( subdev[iS].type == SUBDEV_OUT ) {
    pIndex = pIndexOut;
    defTracenames = outputnames;
    defTraceN = OUTPUT_N;
  }
  if( subdev[iS].type == SUBDEV_IN ) {
    pIndex = pIndexIn;
    defTracenames = inputnames;
    defTraceN = INPUT_N;
  }
   for( iTrace = 0; iTrace < traceN; iTrace++ )
    for( iDef = 0; iDef < defTraceN; iDef++ )
      if( strcmp( traces[iTrace].name, defTracenames[iDef] ) == 0 ) {
        pIndex[iDef].iS = iS;
        pIndex[iDef].chan = traces[iTrace].chanNr;
        break;
      }

   return 0;
}


int startSubdevice( int iS )
{ 
  comedi_insn insn;
  lsampl_t data[1];
  int retVal = 0;
  int i, dmaUpdateFreq;
  unsigned long firstLoopCnt, tmpDuration;

  if( !subdev[iS].prepared || subdev[iS].running ) {
    ERROR_MSG( "startSubdevice ERROR:  Subdevice ID %i on device %s either not prepared or already running.\n",
		 iS, device[subdev[iS].devID].name );
    return -EBUSY;
  }

  if( subdev[iS].asyncMode ) {

    // if AO: prefill DMA-buffer (hoping that user has called module_write() already...):
    if( subdev[iS].type == SUBDEV_OUT ) {
/*/////////////////////// TEST *//////////////////
      DEBUG_MSG( "startSubdevice: comedi_mark_buffer_written(y%d, %d, %d ):\n", 
		 comedi_get_buffer_size( device[subdev[iS].devID].devP, 
					 subdev[iS].subdev ),
		 subdev[iS].subdev, subdev[iS].dmaBufSize );
      
      //    retVal = comedi_mark_buffer_written( device[subdev[iS].devID].devP,
      //					   subdev[iS].subdev, 4/*subdev[iS].dmaBufSize*/ );
      if( retVal < 0 ) {
	ERROR_MSG( "startSubdevice ERROR:  comedi_mark_buffer_written returned %d for subdevice comedi %d\n", 
		   retVal, subdev[iS].subdev );
	comedi_perror( "rtmodule: startSubdevice" );    
      }
      retVal = updateDMABuf( iS );
      if( retVal < 0 ) {
	ERROR_MSG( "startSubdevice ERROR:  Prefill of DMA-Buffer for AO failed on Subdevice ID %i on device %s failed with error %d.\n",
		   iS, device[subdev[iS].devID].name, retVal );
	return -EFAULT;
      }
    }
    /*///////////////////TEST.////////////*/
    // load & start comedi command:
    retVal = comedi_command( device[subdev[iS].devID].devP, subdev[iS].cmdP );
    if( retVal < 0 ){
    ERROR_MSG( "startSubdevice ERROR:  Loading of asynchronous command (comedi_command) on Subdevice ID %i on device %s failed with error %d.\n",
	       iS, device[subdev[iS].devID].name, retVal );
      return -EFAULT;
    }

    // if start=TRIG_NOW was not possible -> start via instruction:
    if( !retVal && subdev[iS].cmdP->start_src == TRIG_INT ) {
      memset( &insn, 0, sizeof(insn) );
      insn.insn = INSN_INTTRIG;
      insn.n = 1;
      insn.data = data;
      insn.subdev = subdev[iS].subdev;
      data[0] = 0;
      retVal = comedi_do_insn( device[subdev[iS].devID].devP, &insn );
      if( retVal < 0 ) {
	ERROR_MSG( "startSubdevice ERROR: Starting of command (comedi_do_insn) on Subdevice ID %i on device %s failed with error %d.\n",
		   iS, device[subdev[iS].devID].name, retVal );
        return -EFAULT;
      }
    }


  }
  else {
    // syncMode: make duration relative to dynClampTask.loopCnt:

    do {
    	firstLoopCnt = dynClampTask.loopCnt;
        tmpDuration = subdev[iS].duration + dynClampTask.loopCnt;
    	dynClampTask.aoIndex = dynClampTask.loopCnt; 
	if( firstLoopCnt != dynClampTask.loopCnt )
           DEBUG_MSG( "XXXXX firstLoopCnt != dynClampTask.loopCnt! => %ld\n",
	   	      dynClampTask.loopCnt = firstLoopCnt );
    } 
    while( firstLoopCnt != dynClampTask.loopCnt );
    subdev[iS].duration = tmpDuration;

  }

  subdev[iS].running = 1;

  if( !dynClampTask.running ) {
/*
    // obtain update rate for DMA-Buffer relative to frequency of dynClampTask:
    dynClampTask.reqFreq = 0;
    dmaUpdateFreq = 0;
    for( i = 0; i < subdevN; i++ )
      if( subdev[i].asyncMode )
      	if( dmaUpdateFreq < subdev[i].frequency /   // .../#Samples
	          ( subdev[i].dmaBufSize/sizeof(lsampl_t)/subdev[i].chanN ) ){
       	  dmaUpdateFreq = subdev[i].frequency /
                          ( subdev[i].dmaBufSize/sizeof(lsampl_t)/subdev[i].chanN );
    ///////////////////////////////////////////////////////////TEST//////////
	  DEBUG_MSG( "1 DMAUPDATEFREQ: %d, SUBDEV_FREQUENCY: %dDMABUFSIZE: %d\n",
		     dmaUpdateFreq, subdev[i].frequency, subdev[i].dmaBufSize );
  	    }
    if( dmaUpdateFreq < 1 )
      dmaUpdateFreq = 1; // 1Hz is minimum
    ///////////////////////////////////////////////////////////TEST//////////
    DEBUG_MSG( "2 DMAUPDATEFREQ: %d, SUBDEV_FREQUENCY: %dDMABUFSIZE: %d\n",
		     dmaUpdateFreq, subdev[iS].frequency, subdev[iS].dmaBufSize );
    if( !dynClampTask.aquireMode ) {
      // dynClampTask only updates DMA-Buffer - set frequency:
      dynClampTask.reqFreq = 2 * dmaUpdateFreq; // for only half empty Buffers
      dynClampTask.dmaUpdateRatio = 1;
    }
    else {
      // simultaneous dynamic clamp acquisition => get max. sync acquisition frequency:
      // TODO: reqFreq has to be multiple ("kgV") of common frequencies!!!
      //       For now, only one frequency for all sync. devices supported
      for( i = 0; i < subdevN; i++ )
        if( !subdev[i].asyncMode )
          if( dynClampTask.reqFreq < subdev[i].frequency )
            dynClampTask.reqFreq = subdev[i].frequency;
      // set update ratio sync - async acquisition:
      dynClampTask.dmaUpdateRatio = dynClampTask.reqFreq / (2*dmaUpdateFreq);
      if( dynClampTask.dmaUpdateRatio <= 0 ) {
      	dynClampTask.dmaUpdateRatio = dynClampTask.reqFreq / dmaUpdateFreq;
      	if( dynClampTask.dmaUpdateRatio <= 0 ) {
	  //ERROR_MSG( "startSubdevice ERROR: Sampling rate of dynamic clamp too low for reliable DMA-buffer update of asynchronous acquisition\n: " );
	  //return -1; // TODO FUNKTIONIERT NOCH NICHT
	      }
      	else
      	  WARN_MSG( "startSubdevice WARNING: Sampling rate of dynamic clamp quite low for reliable DMA-buffer update of asynchronous acquisition\n" );
      }
    }
    */

    // ANSTELLE DESSEN EINFACH:
    dynClampTask.reqFreq = subdev[iS].frequency;
    dynClampTask.dmaUpdateRatio = 0; // => no updating of DMA buffers

    // start dynamic clamp task with correct frequency:
    retVal = init_rt_task( calcTask.algo );
    if( retVal < 0 ) {
      subdev[iS].running = 0;
      return -ENOMEM;
    }

      DEBUG_MSG( "SIMULATED START OF DYN_CLAMP TASK AT %d HZ RATIO %d\n",
		 dynClampTask.reqFreq, dynClampTask.dmaUpdateRatio );
    return 0;
  }

/*
  // adjust DMA-Buffer update ratio for current subdevice if faster:
  dmaUpdateFreq = subdev[iS].frequency /
                  ( subdev[iS].dmaBufSize/sizeof(lsampl_t)/subdev[iS].chanN );
  if( dynClampTask.reqFreq / (2*dmaUpdateFreq) >= dynClampTask.dmaUpdateRatio )
    // update ratio fast enough for current subdevice -> leave:
    return 0;
  if( dynClampTask.reqFreq / (2*dmaUpdateFreq) > 0 ) {
    // subdevice faster -> adjust update ratio:
    dynClampTask.dmaUpdateRatio = dynClampTask.reqFreq / (2*dmaUpdateFreq);
    return 0;
  }
  // faster but failed - try inreliable update ratio:
  WARN_MSG( "startSubdevice WARNING: Sampling rate of dynamic clamp quite low for reliable DMA-buffer update of asynchronous acquisition\n" );
  if( dynClampTask.reqFreq / dmaUpdateFreq < dynClampTask.dmaUpdateRatio &&
      dynClampTask.reqFreq / dmaUpdateFreq > 0 ) {
    dynClampTask.dmaUpdateRatio = dynClampTask.reqFreq / dmaUpdateFreq;
    return 0;
  }
  // failed again (dynamic clamp task too slow):
  //ERROR_MSG( "startSubdevice ERROR: Sampling rate of dynamic clamp too low for reliable DMA-buffer update of asynchronous acquisition\n: " );
  return 0;//return -1;/ TODO FUNKTIONIERT NOCH NICHT
*/

  return 0;

}


int subdevRunning( int iS )
{
  if( !device[subdev[iS].devID].devP || ( subdev[iS].asyncMode && subdev[iS].subdev >= 0 &&
      !( SDF_RUNNING & 
	 comedi_get_subdevice_flags( device[subdev[iS].devID].devP, 
				     subdev[iS].subdev) )  )   )
    subdev[iS].running = 0;
    
  return subdev[iS].running;
}


int stopSubdevice( int iS )
{ 
  int retVal, i;

  if( !subdevRunning( iS ) )
    return 0;

  if( subdev[iS].asyncMode ) {
    retVal = comedi_cancel( device[subdev[iS].devID].devP, subdev[iS].subdev );
    if( retVal < 0 ) {
      ERROR_MSG( "stopSubdevice ERROR: Asynchronous acquisition on Subdevice %i, device %s could not be stopped!\n",
		 iS, device[subdev[iS].devID].name );
      return -1;
    }
  }

  subdev[iS].running = 0;

  if( !subdev[iS].asyncMode ) {
    for( i = 0; i < subdevN; i++ )
      if( subdev[i].running )
	return 0;
    cleanup_rt_task();
  }

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

  if( subdev[iS].running )
    stopSubdevice( iS );
  
  if( device[iD].devP && subdev[iS].asyncMode )
    // unmap DMA-buffer:
    comedi_unmap( device[iD].devP, subdev[iS].subdev );

  if( device[iD].devP && comedi_unlock( device[iD].devP, subdev[iS].subdev ) < 0 )
    WARN_MSG( "releaseSubdevice WARNING: unlocking of subdevice %s failed!\n",
	      device[iD].name );
  else
    DEBUG_MSG( "releaseSubdevice: Unlocking of subdevice %s was successful!\n",
	       device[iD].name );
/****************************************************************************************************************
  if(  subdev[iS].comedi_chanlist )
    vfree(  subdev[iS].comedi_chanlist );
  if(  subdev[iS].cmdP )
    vfree(  subdev[iS].cmdP );
****************************************************************************************************************/
/***************************************************************************************************************************
  deleteBlockList( &subdev[iS].bData );
***************************************************************************************************************************/
  rtf_destroy( subdev[iS].fifo );
  memset( &(subdev[iS]), 0, sizeof(struct subdeviceT) );
  if( iS == subdevN - 1 )
    subdevN--;
  subdev[iS].devID = -1;
  
  for( i = 0; i < subdevN; i++ )
    if( subdev[i].devID == iD )
      // device is still used by another subdevice => leave here:
      return;
  DEBUG_MSG( "YYYYYYY releaseSubdevice: released device for last subdev-ID %d\n", iS );
  if( device[iD].devP && comedi_close( device[iD].devP ) < 0 )
    WARN_MSG( "releaseSubdevice WARNING: closing of device %s failed!\n",
	      device[iD].name );
  else
    DEBUG_MSG( "releaseSubdevice: Closing of device %s was successful!\n",
	       device[iD].name );
  memset( &(device[iD]), 0, sizeof(struct deviceT) );
}


static int updateDMABuf( int iS )
{
  static int offset, bytesToCopy, iD, bytesCopied, newBytes;
  iD = subdev[iS].devID;

  offset = comedi_get_buffer_offset( device[iD].devP, subdev[iS].subdev );
  bytesToCopy = comedi_get_buffer_contents( device[iD].devP, 
					    subdev[iS].subdev );
  bytesCopied = 0;

  if( subdev[iS].type == SUBDEV_OUT ) {
    // put output data:
    while( bytesToCopy-bytesCopied > 0 ) {
      newBytes = popBlockToBuff( &subdev[iS].bData, 
				 (void*)(subdev[iS].dmaBuf+offset),
				 bytesToCopy-bytesCopied );
      if( newBytes < 0 )
	return -1;
      bytesCopied += newBytes;
    }
    if( comedi_mark_buffer_written( device[iD].devP, subdev[iS].subdev,
				bytesCopied ) < 0 ) {
      ERROR_MSG( "updateDMABuf ERROR: comedi_mark_buffer_written failed on subdevice %d!\n",
		 iS );
      return -1;
    }
  }
  else {
    // get input data:
    while( bytesToCopy-bytesCopied > 0 ) {
      newBytes = pushBuffToBlock( &subdev[iS].bData,
				  (void*)(subdev[iS].dmaBuf+offset),
				  bytesToCopy-bytesCopied );
      if( newBytes < 0 )
	return -1;
      bytesCopied += newBytes;
    }
    if( comedi_mark_buffer_read( device[iD].devP, subdev[iS].subdev, 
				 bytesCopied ) < 0 ) {
      ERROR_MSG( "updateDMABuf ERROR: comedi_mark_buffer_read failed on subdevice %d!\n",
		 iS );
      return -1;
    }
  }

  DEBUG_MSG( "updateDMABuf: copied %d of %d bytes (DMA-offset %d) for subdevice %d\n", 
	     bytesCopied, bytesToCopy, offset, iS );
  return bytesCopied;
}



///////////////////////////////////////////////////////////////////////////////
// *** REAL-TIME TASKS *** 
///////////////////////////////////////////////////////////////////////////////


/*! Calcuclation task for Euler */
void rtEulerCalc( int dummy )
{ 
}


/*! Dynamic clamp task */
void rtDynClamp( int dummy )
{
  int retVal;
  int iS, iC;
  int subdevRunning = 1;
  unsigned long writeCnt = 0;
  float voltage;
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

    // TODO: implement efficiently as frequently updated chanT-list of used traces
    for( iS = 0; iS < subdevN; iS++ )
      if( !subdev[iS].asyncMode && subdev[iS].running && 
            subdev[iS].type == SUBDEV_OUT ) {


          if( !subdev[iS].continuous &&
              subdev[iS].duration <= dynClampTask.loopCnt ) {
            subdev[iS].running = 0;
            DEBUG_MSG( "rtDynClamp: ZZZZZZZZZZZZZZZZZZZZZZZZZZZZZZ loopCnt exceeded duration for subdevice ID %d at loopCnt %lu\n",
                 iS, dynClampTask.loopCnt );
      	    continue;
          }
      	  else 
	          if( iS == 1 && dynClampTask.loopCnt%100 == 0 )
          		DEBUG_MSG( " duration = %lu, loopCnt = %lu\n", subdev[iS].duration, dynClampTask.loopCnt );

          ////spin_lock( &subdev[iS].bData.spinlock );

/*          if( READDATAERROR(subdev[iS].bData) ) {
            subdev[iS].error = E_UNDERRUN;
            DEBUG_MSG( "rtDynClamp: Data buffer underrun for AO subdevice ID %d at loopCnt %lu\n",
                 iS, dynClampTask.loopCnt );
            DEBUG_MSG( "rtDynClamp: readBlock: %p writeBlock: %p readBlockPos: %d writeBlockPos: %d\n",
                 subdev[iS].bData.readBlock, subdev[iS].bData.writeBlock,
                 subdev[iS].bData.readBlockPos, subdev[iS].bData.writeBlockPos );

            subdev[iS].running = 0;
              //spin_unlock( &subdev[iS].bData.spinlock );
              continue;
          }
*/
          subdevRunning = 1;
          
          for( iC = 0; iC < subdev[iS].chanN; iC++ ) {
            // FIFO auslesen:
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
            // Sample ausschreiben:
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
          }

          ////MOVE_READPOS( subdev[iS].bData );

          //spin_unlock( &subdev[iS].bData.spinlock );

      }

    

//******** SLEEP FOR NEURON TO REACT TO GIVEN OUTPUT: ************************/

    //* PROBLEM: rt_sleep is timed using jiffies only (granularity = 1msec)
    //* int retValSleep = rt_sleep( nano2count( INJECT_RECORD_DELAY ) );
    rt_busy_sleep( INJECT_RECORD_DELAY ); // TODO: just default



    if( dynClampTask.loopCnt % 100 == 0 ) {
      DEBUG_MSG( "%d subdevices registered:\n", subdevN );
      for( iS = 0; iS < subdevN; iS++ ) {
        DEBUG_MSG( "LOOP %lu: reqReadID=%d writeCnt=%lu, dBlock=%d, subdevID=%d, asyncMode=%d, run=%d, type=%d, error=%d, duration=%lu, contin=%d\n", 
		   dynClampTask.loopCnt, reqReadSubdevID, writeCnt, 
		   subdev[iS].bData.writeBlockPos - subdev[iS].bData.readBlockPos,
                   iS, subdev[iS].asyncMode, subdev[iS].running, 
                   subdev[iS].type, subdev[iS].error, subdev[iS].duration, subdev[iS].continuous  );
	   printk( "LOOP: rbp=%d wpp=%d rb=%p wb=%p\n",
            subdev[iS].bData.readBlockPos, subdev[iS].bData.writeBlockPos,
            subdev[iS].bData.readBlock, subdev[iS].bData.writeBlock );
      }
    }




//******** READ FROM ANALOG INPUT: *******************************************/
    // TODO: implement efficiently as frequently updated chanT-list of used traces
    for( iS = 0; iS < subdevN; iS++ )
      if( !subdev[iS].asyncMode && subdev[iS].running && 
            subdev[iS].type == SUBDEV_IN ) {

 
          
          if( !subdev[iS].continuous &&
              subdev[iS].duration <= dynClampTask.loopCnt ) {
            subdev[iS].running = 0;
            DEBUG_MSG( "rtDynClamp: loopCnt exceeded duration for subdevice ID %d at loopCnt %lu\n",
                 iS, dynClampTask.loopCnt );
          }

          //spin_lock( &subdev[iS].bData.spinlock );
/*
          if( WRITEDATAERROR(subdev[iS].bData) ) {
            subdev[iS].error = E_OVERFLOW;
            DEBUG_MSG( "rtDynClamp: Data buffer overflow for AI subdevice ID %d at loopCnt %lu\n",
                 iS, dynClampTask.loopCnt );
            subdev[iS].running = 0;
              //spin_unlock( &subdev[iS].bData.spinlock );
              continue;
          }
*/
          subdevRunning = 1;

          for( iC = 0; iC < subdev[iS].chanN; iC++ ) {
            // Sample lesen:
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
              //spin_unlock( &subdev[iS].bData.spinlock );
              continue;
            }
            // Wert in FIFO schreiben:
            voltage = sample_to_value( &subdev[iS].chanlist[iC], lsample );
            retVal = rtf_get( subdev[iS].fifo, &voltage, sizeof(voltage) );
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
            //// WRITEDATA( subdev[iS].bData, iC ) = sample_to_value( &subdev[iS].chanlist[iC], lsample );
          }
          //// MOVE_WRITEPOS( subdev[iS].bData );
      	  writeCnt++;

          //spin_unlock( &subdev[iS].bData.spinlock );

      }

//******** UPDATE DMA-BUFFER (only if asynchronous acquisition is running) ***/

      if( dynClampTask.dmaUpdateRatio ) {
          if( dynClampTask.dmaUpdateRatio < subdevN ) {
            // we are slow - update all DMA-buffers at once:
            if( ((unsigned long)dynClampTask.loopCnt) % 
                dynClampTask.dmaUpdateRatio == 0 )
              for( iS = 0; iS < subdevN; iS++ )
                if( subdev[iS].asyncMode )
            updateDMABuf( iS );
          }
          else 
            // spread update over several cycles (minimize jitter):
            if( ((unsigned long)dynClampTask.loopCnt) % 
                dynClampTask.dmaUpdateRatio == iS &&
                subdev[iS].asyncMode )
              updateDMABuf( iS );
      }
    

    dynClampTask.loopCnt++;


//******** WAIT FOR CALCULATION TASK TO COMPUTE RESULT: **********************/

    //    start = rt_get_cpu_time_ns();
    rt_task_wait_period();

    // ...as soon as RTAI scheduler wakes us up again:


//******** SUSPEND CALCULATION TASK: *****************************************/

    if( calcTask.initialized )
      rt_task_suspend( &calcTask.rtTask );





  }// while

    
  dynClampTask.running = 0;
  dynClampTask.duration = 0;

//  for( iS = 0; iS < subdevN; iS++ )
//    deleteBlockList( &subdev[iS].bData );
  

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

 //* initializing rt-task for calculation with lower:
  priority = 2;
  retVal = rt_task_init( &calcTask.rtTask, rtEulerCalc, dummy, stackSize, 
			 priority, usesFPU, signal );
  if( retVal ) {
    ERROR_MSG( "init_rt_task ERROR: failed to initialize real-time task for calculation! stacksize was set to %d bytes.\n", 
	       stackSize );
    return -2;
  }

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
// *** FILE OPERATION FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////


int rtmodule_open( struct inode *devFile, struct file *fModule )
{
  DEBUG_MSG( "open: user opened device file\n" );
  
  return 0;
}


int rtmodule_close( struct inode *devFile, struct file *fModule )
{
  int iS;
  if( reqCloseSubdevID < 0 ) {
    DEBUG_MSG( "close: no IOC_REQ_CLOSE request received - closing all subdevices...\n" );
    for( iS = 0; iS < subdevN; iS++ ) {
      if( stopSubdevice( iS ) )
        WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
      releaseSubdevice( iS );
    }
    return 0;
  }

  if( stopSubdevice( reqCloseSubdevID ) )
      WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", reqCloseSubdevID );
  releaseSubdevice( reqCloseSubdevID );

  reqCloseSubdevID = -1;
  reqReadSubdevID = -1;
  reqWriteSubdevID = -1;

  DEBUG_MSG( "close: user closed device file\n" );
  return 0;
}


int rtmodule_ioctl( struct inode *devFile, struct file *fModule, 
		    unsigned int cmd, unsigned long arg )
{
  static struct deviceIOCT deviceIOC;
  static struct chanlistIOCT chanlistIOC;
  static struct comediCmdIOCT comediCmdIOC;
  static struct syncCmdIOCT syncCmdIOC;
  struct traceNameIOCT *traceNames;

  int tmp, subdevID;
  int retVal;
  unsigned long luTmp;

  DEBUG_MSG( "ioctl: user triggered ioctl with id: %d\n", _IOC_NR( cmd ) );

  if (_IOC_TYPE(cmd) != RTMODULE_MAJOR) return -ENOTTY;
  if (_IOC_NR(cmd) > RTMODULE_IOC_MAXNR) return -ENOTTY;


  // TODO: use access_ok for verifying userspace mem. 
  //       => (see Linux-Device-Drivers-3-Book, Chapter 6)

  switch( cmd ) {
    

 //* Give information to user space:

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


 //* Set up devices:

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
    

  case IOC_COMEDI_CMD:
    //if( reqReadSubdevID >= 0 ) // rtmodule_read()- or IOC_CMD-call in progress
    //  return -EAGAIN;
    retVal = copy_from_user( &comediCmdIOC, (void __user *)arg, sizeof(struct comediCmdIOCT) );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to comediCmdIOCT-struct!\n" );
      return -EFAULT;
    }
    if( comediCmdIOC.subdevID >= subdevN || 
	subdev[comediCmdIOC.subdevID].subdev != comediCmdIOC.cmd.subdev ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID in comediCmdIOCT-struct!\n" );
      return -EFAULT;
    }
    retVal = loadComediCmd( &comediCmdIOC );
    if( retVal != 0 )
      return -EFAULT;
    //reqReadSubdevID = comediCmdIOC.subdevID; // expect IOC_TRACENAMELIST right now!
    return 0;

    
  case IOC_SYNC_CMD:
    //if( reqReadSubdevID >= 0 ) // rtmodule_read()- or IOC_CMD-call in progress
    //  return -EAGAIN;

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
    //reqReadSubdevID = syncCmdIOC.subdevID; // expect IOC_TRACENAMELIST right now!


  case IOC_TRACENAMELIST:
    if( reqReadSubdevID < 0 )
      return -EFAULT; // no IOC_XXX_CMD-call noticed right before!
    traceNames = vmalloc( subdev[reqReadSubdevID].chanN*sizeof(struct traceNameIOCT) );
    if( !traceNames ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: Out of memory during IOC_TRACENAMELIST\n" );
      return -EFAULT;
    }
    retVal = copy_from_user( traceNames, (void __user *)arg, 
                             subdev[reqReadSubdevID].chanN*sizeof(struct traceNameIOCT) );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to traceNameIOCT-array!\n" );
      return -EFAULT;
    }
    retVal = mapTracenames( reqReadSubdevID, traceNames, subdev[reqReadSubdevID].chanN );
/***************************************************************************************************************************
    vfree( traceNames );
***************************************************************************************************************************/
    return retVal == 0 ? 0 : -EFAULT;


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

  case IOC_REQ_READ:
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for read-request!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for read-request!\n" );
      return -EFAULT;
    }
    if( reqReadSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ ERROR: Another read-request in progress! (reqReadSubdevID=%d)\n", reqReadSubdevID );
      return -EAGAIN;
    }
    ERROR_MSG( "rtmodule_ioctl IOC_REQ_READ: Requested Read\n" );
    reqReadSubdevID = subdevID;
    return 0;

  case IOC_REQ_WRITE:
    retVal = get_user( subdevID, (int __user *)arg );
    if( retVal ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid pointer to subdevice ID for write-request!" );
      return -EFAULT;
    }
    if( subdevID >= subdevN ) {
      ERROR_MSG( "rtmodule_ioctl ERROR: invalid subdevice ID for write-request!\n" );
      return -EFAULT;
    }
    if( reqWriteSubdevID >= 0 ) {
      ERROR_MSG( "rtmodule_ioctl IOC_REQ_WRITE ERROR: Another write-request in progress!\n" );
      return -EAGAIN;
    }
    reqWriteSubdevID = subdevID;
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



  return -EINVAL;
}


  /*! Transfer data acquired by dynamic clamp loop to userspace */
ssize_t rtmodule_read( struct file *fModule, char __user *userBuff, 
		       size_t userBuffSize, loff_t *offset )
{
  long bytesCopied, newBytes;
  int iS = reqReadSubdevID;
//  DEBUG_MSG( "rtmodule:read: user requested %lu bytes of data from subdevice %d on device %s\n",
//	     userBuffSize, iS, device[subdev[iS].devID].name );
  if( iS < 0 || !subdev[iS].prepared ) { // TODO: or !.running ???
    DEBUG_MSG( "rtmodule:read: ERROR: No read for any subdevice requested\n" );
    reqReadSubdevID = -1;
    return -EAGAIN;
  }

  // TODO: sleep einbauen fuer page-miss? 
  //       -> siehe Kapitel 6 "Linux Device Drivers 3" Buch

  // copy block-wise to user-buffer:
  bytesCopied = 0;
  DEBUG_MSG( "rtmodule_read: running status=%d\n", subdev[iS].running );
  printk( "rtmodule_read: iS=%d rbp=%d wpp=%d rb=%p wb=%p\n",
            iS, subdev[iS].bData.readBlockPos, subdev[iS].bData.writeBlockPos,
            subdev[iS].bData.readBlock, subdev[iS].bData.writeBlock );
  do {
    newBytes = popBlockToBuff( &subdev[iS].bData, 
			                         (void*)(userBuff + bytesCopied), 
			                         userBuffSize - bytesCopied );
    bytesCopied += newBytes;
  } while( bytesCopied < userBuffSize && newBytes > 0 );
  DEBUG_MSG( "rtmodule_read: running status=%d\n", subdev[iS].running );
  printk( "rtmodule_read: rbp=%d wpp=%d rb=%p wb=%p\n",
            subdev[iS].bData.readBlockPos, subdev[iS].bData.writeBlockPos,
            subdev[iS].bData.readBlock, subdev[iS].bData.writeBlock );
  
  if(  newBytes < 0 || (!bytesCopied && subdev[iS].error) ) {
    DEBUG_MSG( "rtmodule:read: ERROR: bytesCopied=%d  newBytes=%d error=%d\n", 
	       bytesCopied, newBytes, subdev[iS].error );
    reqReadSubdevID = -1;
    return -EAGAIN;
  }

  DEBUG_MSG( "rtmodule:read: ...%ld bytes copied\n", bytesCopied );
  reqReadSubdevID = -1;
  return bytesCopied;
}




  /*! Transfer data from userspace for:
      - time-dependent parameters for dynamic clamp: paramDynClamp
      - direct output for in dynamic clamp loop: outputDynClamp
      - direct output for stimulus data: outputStimulus
      Start dynamic clamp task and stimulus-output simultaneously */
ssize_t rtmodule_write( struct file *fModule, const char __user *userBuff, 
			size_t userBuffSize, loff_t *offset )
{
  long bytesCopied, newBytes;
  int iS = reqWriteSubdevID;
  DEBUG_MSG( "rtmodule:write: user provided %lu bytes of data for subdevice %d on device %s\n",
	     userBuffSize, iS, device[subdev[iS].devID].name );
  if( iS < 0 || !subdev[iS].prepared ) {
    reqWriteSubdevID = -1;
    return -EAGAIN;\
  }

  // TODO: sleep einbauen fuer page-miss? 
  //       -> siehe Kapitel 6 "Linux Device Drivers 3" Buch

  // copy block-wise from user-buffer:
  bytesCopied = 0;
  do { 
    newBytes = pushBuffToBlock( &subdev[iS].bData, 
			                          (void*)(userBuff + bytesCopied), 
			                          userBuffSize - bytesCopied );
    bytesCopied += newBytes;
  } while( bytesCopied < userBuffSize && newBytes > 0 );
 

  if( newBytes < 0 || (!bytesCopied && subdev[iS].error) ) {
    reqWriteSubdevID = -1;
    return -EAGAIN;
  }

  DEBUG_MSG( "rtmodule:write: ...%ld bytes copied\n", bytesCopied );
  reqWriteSubdevID = -1;
  return bytesCopied;
}




///////////////////////////////////////////////////////////////////////////////
// *** DRIVER FUNCTIONS ***
///////////////////////////////////////////////////////////////////////////////


static struct file_operations fops = {                     
  .owner= THIS_MODULE,
  .read= rtmodule_read,
  .write= rtmodule_write,
  .ioctl= rtmodule_ioctl,
  .open= rtmodule_open, 
  .release= rtmodule_close,
};


int init_module( void )
{
  // initialize model-specific variables:
  initModel();

  // TODO: adapt to kernel 2.6 convention (see char-device chapter in lld3)
  if(register_chrdev( RTMODULE_MAJOR, moduleName, &fops ) != 0) {
    WARN_MSG( "init_module: couldn't register driver's major number\n" );
    // return -1;
  }
  INFO_MSG( "module_init: dynamic clamp module %s loaded\n", moduleName );
  DEBUG_MSG( "module_init: debugging enabled\n" );

  comedi_loglevel( 3 ); 


  // initialize global variables:
  memset( &dynClampTask, 0, sizeof(struct dynClampTaskT ) );
  calcTask.initialized = 0;
  calcTask.algo = ALGO_PRESET;


  return 0;
}

void cleanup_module( void )
{
  int iS;
  INFO_MSG( "cleanup_module: dynamic clamp module %s unloaded\n", moduleName );


  //if( paramValues )
  //  vfree( paramValues );

  for( iS = 0; iS < subdevN; iS++ ) {
    if( stopSubdevice( iS ) )
      WARN_MSG( "cleanup_module: Stopping subdevice with ID %d failed\n", iS );
    releaseSubdevice( iS );
  }


  unregister_chrdev( RTMODULE_MAJOR, moduleName );

}


