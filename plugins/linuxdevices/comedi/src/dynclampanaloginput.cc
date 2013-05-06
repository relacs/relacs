/*
  comedi/dynclampanaloginput.cc
  Interface for accessing analog input of a daq-board via the dynamic clamp kernel module.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  RELACS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <relacs/comedi/comedianaloginput.h>
#include <relacs/comedi/dynclampanaloginput.h>
#include <rtai_fifos.h>
using namespace std;
using namespace relacs;

namespace comedi {


DynClampAnalogInput::DynClampAnalogInput( void ) 
  : AnalogInput( "DynClampAnalogInput", DynClampAnalogIOType )
{
  CAI = new ComediAnalogInput;
  CAISubDevFlags = 0;
  SubdeviceID = -1;
  ModuleDevice = "";
  ModuleFd = -1;
  FifoFd = -1;
  SubDevice = -1;
  BufferElemSize = sizeof(float);
  Channels = 0;
  Bits = 0;
  MaxRate = 50000.0;
  IsPrepared = false;
  IsRunning = false;
  ErrorState = 0;
  UnipConverter = 0;
  BipConverter = 0;
  Traces = 0;
  ReadBufferSize = 0;
  BufferSize = 0;
  BufferN = 0;
  Buffer = NULL;
  TraceIndex = 0;

}


DynClampAnalogInput::DynClampAnalogInput( const string &device, const Options &opts ) 
  : AnalogInput( "DynClampAnalogInput", DynClampAnalogIOType )
{
  CAI = new ComediAnalogInput;
  CAISubDevFlags = 0;
  SubdeviceID = -1;
  ModuleDevice = "";
  ModuleFd = -1;
  FifoFd = -1;
  SubDevice = -1;
  BufferElemSize = sizeof(float);
  Channels = 0;
  Bits = 0;
  MaxRate = 50000.0;
  IsPrepared = false;
  IsRunning = false;
  ErrorState = 0;
  UnipConverter = 0;
  BipConverter = 0;
  Traces = 0;
  ReadBufferSize = 0;
  BufferSize = 0;
  BufferN = 0;
  Buffer = NULL;
  TraceIndex = 0;

  open( device, opts );
}


DynClampAnalogInput::~DynClampAnalogInput( void ) 
{
  close();
  delete CAI;
}


int DynClampAnalogInput::open( const string &device, const Options &opts )
{ 
  Info.clear();
  Settings.clear();

  if ( device.empty() )
    return InvalidDevice;
  setDeviceFile( device );

  // open user space coemdi:
  int retval = CAI->open( device, opts );
  if ( retval != 0 )
    return retval;
  
  // copy information not available after CAI->close()
  SubDevice = CAI->comediSubdevice();
  Channels = CAI->channels();
  Bits =  CAI->bits();
  MaxRate = CAI->maxRate();  // XXX This is not the max freq of the real time loop!
  MaxRate = 50000.0;

  CAISubDevFlags = comedi_get_subdevice_flags( CAI->DeviceP, SubDevice );

  // set basic device infos:
  setDeviceName( CAI->deviceName() );
  setDeviceVendor( CAI->deviceVendor() );
  setDeviceFile( device );

  // get calibration:
  comedi_calibration_t *calibration;
  {
    char *calibpath = comedi_get_default_calibration_path( CAI->DeviceP );
    ifstream cf( calibpath );
    if ( cf.good() )
      calibration = comedi_parse_calibration_file( calibpath );
    else
      calibration = 0;
    free( calibpath );
  }

  // get conversion polynomials:
  bool softcal = ( ( CAISubDevFlags & SDF_SOFT_CALIBRATED ) > 0 );
  UnipConverter = new comedi_polynomial_t* [Channels];
  BipConverter = new comedi_polynomial_t* [Channels];
  for ( int c=0; c<channels(); c++ ) {
    UnipConverter[c] = new comedi_polynomial_t[CAI->UnipolarRangeIndex.size()];
    for ( unsigned int r=0; r<CAI->UnipolarRangeIndex.size(); r++ ) {
      if ( softcal && calibration != 0 ) {
	comedi_get_softcal_converter( SubDevice, c, CAI->UnipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL, calibration,
				      &UnipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( CAI->DeviceP, SubDevice, c,
				      CAI->UnipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL,
				      &UnipConverter[c][r] );
      }
    }
    BipConverter[c] = new comedi_polynomial_t[CAI->BipolarRangeIndex.size()];
    for ( unsigned int r=0; r<CAI->BipolarRangeIndex.size(); r++ ) {
      if ( softcal && calibration != 0 ) {
	comedi_get_softcal_converter( SubDevice, c, CAI->BipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL, calibration,
				      &BipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( CAI->DeviceP, SubDevice, c,
				      CAI->BipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL, 
				      &BipConverter[c][r] );
      }
    }
  }
  
  // cleanup calibration:
  if ( calibration != 0 )
    comedi_cleanup_calibration( calibration );

  // close user space comedi:
  CAI->close();

  // open kernel module:
  ModuleDevice = "/dev/dynclamp";
  ModuleFd = ::open( ModuleDevice.c_str(), O_RDONLY );
  if( ModuleFd == -1 ) {
  cerr << " DynClampAnalogInput::open(): opening dynclamp-module failed\n";
    return -1;
  }

  // get subdevice ID from module:
  retval = ::ioctl( ModuleFd, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if( retval < 0 ) {
    cerr << " DynClampAnalogInput::open -> ioctl command IOC_GET_SUBDEV_ID on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // set device and subdevice:
  struct deviceIOCT deviceIOC;
  deviceIOC.subdevID = SubdeviceID;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = SubDevice;
  deviceIOC.subdevType = SUBDEV_IN;
  deviceIOC.fifoSize = 0;
  retval = ::ioctl( ModuleFd, IOC_OPEN_SUBDEV, &deviceIOC );
  //  cerr << " DynClampAnalogInput::open(): IOC_OPEN_SUBDEV request for address done!" /// TEST
  //       << &deviceIOC << '\n';
  if( retval < 0 ) {
    cerr << " DynClampAnalogInput::open -> ioctl command IOC_OPEN_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // initialize connection to RTAI-FIFO:
  char fifoName[] = "/dev/rtfxxx";
  sprintf( fifoName, "/dev/rtf%u", deviceIOC.fifoIndex );
  FifoFd = ::open( fifoName, O_RDONLY | O_NONBLOCK );
  if( FifoFd < 0 ) {
    cerr << " DynClampAnalogInput::open() -> oping RTAI-FIFO " 
         << fifoName << " failed!\n";
    return -1;
  }
  ReadBufferSize = deviceIOC.fifoSize;

  IsPrepared = false;

  setInfo();

  return 0;
}


bool DynClampAnalogInput::isOpen( void ) const 
{ 
  return ( ModuleFd >= 0 );
}


void DynClampAnalogInput::close( void )
{ 
  if ( ! isOpen() )
    return;

  reset();

  ::ioctl( ModuleFd, IOC_REQ_CLOSE, &SubdeviceID );
  ::close( FifoFd );
  FifoFd = -1;
  if( ::close( ModuleFd ) < 0 )
    cerr << "Close of module file failed!\n";

  ModuleFd = -1;

  // cleanup converters:
  for ( int c=0; c<channels(); c++ ) {
    delete [] UnipConverter[c];
    delete [] BipConverter[c];
  }
  delete [] UnipConverter;
  delete [] BipConverter;
  UnipConverter = 0;
  BipConverter = 0;

  Info.clear();
}


int DynClampAnalogInput::setModuleName( string modulename )
{
  ModuleDevice = modulename;
  // TODO: test opening here?
  return 0;
}


string DynClampAnalogInput::moduleName( void ) const
{
  return ModuleDevice;
}


int DynClampAnalogInput::channels( void ) const
{ 
  return Channels;
}


int DynClampAnalogInput::bits( void ) const
{ 
  return Bits;
}


double DynClampAnalogInput::maxRate( void ) const 
{ 
  return MaxRate;
}


int DynClampAnalogInput::maxRanges( void ) const
{
  return CAI->maxRanges();
}


double DynClampAnalogInput::unipolarRange( int index ) const
{
  return CAI->unipolarRange( index );
}


double DynClampAnalogInput::bipolarRange( int index ) const
{
  return CAI->bipolarRange( index );
}


int DynClampAnalogInput::setupChanList( InList &traces,
					unsigned int *chanlist,
					int maxchanlist )
{
  memset( chanlist, 0, maxchanlist*sizeof( unsigned int ) );

  for( int k = 0; k < traces.size() && k < maxchanlist; k++ ) {

    // parameter traces don't have references and gains:
    if ( traces[k].channel() >= PARAM_CHAN_OFFSET ) {
      chanlist[k] = CR_PACK( traces[k].channel(), 0, 0 );
      continue;
    }

    // reference:
    int aref = -1;
    switch( traces[k].reference() ) {
    case InData::RefCommon: 
      if( CAISubDevFlags & SDF_COMMON )
	aref = AREF_COMMON;
      break;
    case InData::RefDifferential:
      if( CAISubDevFlags & SDF_DIFF )
	aref = AREF_DIFF; 
      break;
    case InData::RefGround:
      if( CAISubDevFlags & SDF_GROUND )
	aref = AREF_GROUND; 
      break;
    case InData::RefOther: 
      if( CAISubDevFlags & SDF_OTHER )
	aref = AREF_OTHER;
      break;
    }
    if( aref == -1 )
      traces[k].addError( DaqError::InvalidReference );

    // allocate gain factor:
    char *gaindata = traces[k].gainData();
    if ( gaindata != NULL )
      delete [] gaindata;
    gaindata = new char[sizeof(comedi_polynomial_t)];
    traces[k].setGainData( gaindata );
    comedi_polynomial_t *gainp = (comedi_polynomial_t *)gaindata;

    // ranges:
    if ( traces[k].unipolar() ) {
      double max = CAI->UnipolarRange[traces[k].gainIndex()].max;
      double min = CAI->UnipolarRange[traces[k].gainIndex()].min;
      if ( max < 0 || min < 0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( 0.0 );
      int gi = CAI->UnipolarRangeIndex[ traces[k].gainIndex() ];
      memcpy( gainp, &UnipConverter[traces[k].channel()][gi], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( traces[k].channel(), gi, aref );
    }
    else {
      double max = CAI->BipolarRange[traces[k].gainIndex()].max;
      double min = CAI->BipolarRange[traces[k].gainIndex()].min;
      if ( max < 0 || min >= 0.0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( min );
      int gi = CAI->BipolarRangeIndex[ traces[k].gainIndex() ];
      memcpy( gainp, &BipConverter[traces[k].channel()][gi], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( traces[k].channel(), gi, aref );
    }

  }

  return 0;
}


int DynClampAnalogInput::testReadDevice( InList &traces )
{
  ErrorState = 0;
 
  if( ! isOpen() ) {
    traces.setError( DaqError::DeviceNotOpen );
    return -1;
  }
  
  //  cerr << " DynClampAnalogInput::testRead(): 1\n";////TEST////


  // sampling rate must be the one of the running rt-loop:
  unsigned int rate = 0;
  int retval = ::ioctl( ModuleFd, IOC_GETRATE, &rate );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::testWriteDevice -> ioctl command IOC_GETRATE on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }
  unsigned int reqrate = (unsigned int)traces[0].sampleRate();
  if ( reqrate == 0 )
    if ( rate > 0 )
      traces.setSampleRate( (double)rate );
    else
      traces.addError( DaqError::InvalidSampleRate );
  else {
    if ( rate > 0 ) {
      if ( ::abs( reqrate - rate ) > 5 )
	traces.addError( DaqError::InvalidSampleRate );
      traces.setSampleRate( (double)rate );
    }
  }

  // start source:
  if ( traces[0].startSource() < 0 || traces[0].startSource() >= 5 ) {
    traces.setStartSource( 0 );
    traces.addError( DaqError::InvalidStartSource );
  }

  // channel configuration:
  for ( int k=0; k<traces.size(); k++ ) {
    traces[k].delError( DaqError::InvalidChannel );
    // check channel number:
    if( traces[k].channel() < 0 ) {
      traces[k].addError( DaqError::InvalidChannel );
      traces[k].setChannel( 0 );
    }
    else if( traces[k].channel() >= channels() && traces[k].channel() < PARAM_CHAN_OFFSET ) {
      traces[k].addError( DaqError::InvalidChannel );
      traces[k].setChannel( channels()-1 );
    }
  }

  for( int k = 0; k < traces.size(); k++ ) {

    // check delays:
    if( traces[k].delay() > 0.0 ) {
      traces[k].addError( DaqError::InvalidDelay );
      traces[k].addErrorStr( "delays are not supported for analog input!" );
      traces[k].setDelay( 0.0 );
    }

  }

  unsigned int chanlist[MAXCHANLIST];
  setupChanList( traces, chanlist, MAXCHANLIST );

  if( traces.failed() )
    return -1;

  //  cerr << " DynClampAnalogInput::testRead(): success\n";/////TEST/////

  retval = 0;

  // check read buffer size:
  int readbufsize = traces.size() * traces[0].indices( traces[0].readTime() ) * BufferElemSize;
  if ( readbufsize > ReadBufferSize ) {
    traces.addError( DaqError::InvalidBufferTime );
    traces.setReadTime( ReadBufferSize/traces.size()/BufferElemSize/traces[0].sampleRate() );
    retval = -1;
  }

  // check update buffer size:
  int bufsize = traces.size() * traces[0].indices( traces[0].updateTime() ) * BufferElemSize;
  if ( bufsize < readbufsize ) {
    traces.addError( DaqError::InvalidUpdateTime );
    retval = -1;
  }
  if ( bufsize > FIFO_SIZE ) {
    traces.addError( DaqError::InvalidUpdateTime );
    cerr << "DynClampAnalogInput::testRead(): FIFO_SIZE is too small for update time.\n";
    retval = -1;
  }


  return retval;
}


int DynClampAnalogInput::prepareRead( InList &traces )
{
  if ( !isOpen() )
    return -1;

  reset();

  if ( traces.size() <= 0 )
    return -1;

  setupChanList( traces, ChanList, MAXCHANLIST );

  // set chanlist:
  struct chanlistIOCT chanlistIOC;
  chanlistIOC.subdevID = SubdeviceID;
  for( int k = 0; k < traces.size(); k++ ) {
    chanlistIOC.chanlist[k] = ChanList[k];
    if ( traces[k].channel() < PARAM_CHAN_OFFSET ) {
      const comedi_polynomial_t* poly = 
	(const comedi_polynomial_t *)traces[k].gainData();
      chanlistIOC.conversionlist[k].order = poly->order;
      if ( poly->order >= MAX_CONVERSION_COEFFICIENTS )
	cerr << "ERROR in DynClampAnalogInput::prepareRead -> invalid order in converion polynomial!\n";
      chanlistIOC.conversionlist[k].expansion_origin = poly->expansion_origin;
      for ( int c=0; c<MAX_CONVERSION_COEFFICIENTS; c++ )
	chanlistIOC.conversionlist[k].coefficients[c] = poly->coefficients[c];
      chanlistIOC.scalelist[k] = traces[k].scale();
    }
  }
  chanlistIOC.userDeviceIndex = traces[0].device();
  chanlistIOC.chanlistN = traces.size();
  int retval = ::ioctl( ModuleFd, IOC_CHANLIST, &chanlistIOC );
  //  cerr << "prepareRead(): IOC_CHANLIST done!\n"; /// TEST
  if( retval < 0 ) {
    cerr << " DynClampAnalogInput::prepareRead -> ioctl command IOC_CHANLIST on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // set up synchronous command:
  struct syncCmdIOCT syncCmdIOC;
  syncCmdIOC.subdevID = SubdeviceID;
  syncCmdIOC.frequency = (unsigned int)traces[0].sampleRate();
  syncCmdIOC.duration = traces[0].capacity() + traces[0].indices( traces[0].delay());
  syncCmdIOC.continuous = traces[0].continuous();
  syncCmdIOC.startsource = traces[0].startSource();
  retval = ::ioctl( ModuleFd, IOC_SYNC_CMD, &syncCmdIOC );
  //  cerr << "prepareRead(): IOC_SYNC_CMD done!\n"; /// TEST
  if( retval < 0 ) {
    cerr << " DynClampAnalogInput::prepareRead -> ioctl command IOC_SYNC_CMD on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // init internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  BufferSize = 2 * traces.size() * traces[0].indices( traces[0].updateTime() ) * BufferElemSize;
  Buffer = new char[BufferSize];
  BufferN = 0;

  /*
    XXX this needs user space comedi????
    can this go into the kernel????
  // apply calibration:
  if ( Calibration != 0 ) {
    for( int k=0; k < traces.size(); k++ ) {
      unsigned int channel = CR_CHAN( Cmd.chanlist[k] );
      unsigned int range = CR_RANGE( Cmd.chanlist[k] );
      unsigned int aref = CR_AREF( Cmd.chanlist[k] );
      if ( comedi_apply_parsed_calibration( DeviceP, SubDevice, channel,
					    range, aref, Calibration ) < 0 )
	traces[k].addError( DaqError::CalibrationFailed );
    }
  }
  */
  
  if ( traces.success() ) {
    setSettings( traces, BufferSize, ReadBufferSize );
    Settings.addInteger( "number of periods", 0 );
    Settings.addNumber( "average period", 0.0, "us" );
    Settings.addNumber( "minimum period", 0.0, "us" );
    Settings.addNumber( "maximum period", 0.0, "us" );
    Traces = &traces;
  }

  IsPrepared = traces.success();

  return traces.success() ? 0 : -1;
}


int DynClampAnalogInput::startRead( void )
{
  //  cerr << " DynClampAnalogInput::startRead(): 1\n";/////TEST/////

  if ( !prepared() || Traces == 0 ) {
    cerr << "AI not prepared or no traces!\n";
    return -1;
  }

  // start subdevice:
  int retval = ::ioctl( ModuleFd, IOC_START_SUBDEV, &SubdeviceID );
  if( retval < 0 ) {
    cerr << " DynClampAnalogInput::startRead -> ioctl command IOC_START_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    int ern = errno;
    if ( ern == ENOMEM )
      cerr << " !!! No stack for kernel task !!!\n";
    Traces->addErrorStr( ern );
    return -1;
  }

  // get sampling rate:
  unsigned int rate = 0;
  retval = ::ioctl( ModuleFd, IOC_GETRATE, &rate );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::testWriteDevice -> ioctl command IOC_GETRATE on device "
	 << ModuleDevice << " failed!\n";
  }
  else
    Traces->setSampleRate( (double)rate );

  ErrorState = 0;
  
  return 0;
}

 
int DynClampAnalogInput::readData( void )
{
  //  cerr << " DynClampAnalogInput::readData(): in\n";/////TEST/////

  //device stopped?
    /* // AI does not have to run in order to push data
  if( !running() ) {
    traces.addErrorStr( "DynClampAnalogInput::readData: " +
		      deviceFile() + " is not running!" );
    cerr << "DynClampAnalogInput::readData: device is not running!"  << '\n';/////TEST/////
    return 0;/////TEST/////
  }
    */

  ErrorState = 0;
  /*  cerr << "DynClampAnalogInput::readData: size of device buffer: " 
       << traces[0].deviceBufferSize() << " - size of indata: " 
       << " - continuous: " << traces[0].continuous() << '\n';*/
  int oldbuffern = BufferN;
  bool failed = false;
  int readn = BufferN*BufferElemSize;
  int maxn = BufferSize - readn;

  // debug:
  if ( maxn < 0 )
    cerr << "DynClampAnalogInput::readData: buffer overflow! BufferN=" << BufferN
	 << " BufferSize=" << BufferSize << " readn=" << readn << " maxn=" << maxn << '\n';

  // try to read twice
  for ( int tryit = 0; tryit < 2 && ! failed && maxn > 0; tryit++ ) {
/*
    int retval = ioctl( ModuleFd, IOC_REQ_READ, &SubdeviceID );
    if( retval < 0 ) {
      cerr << " DynClampAnalogInput::readData() -> ioctl command IOC_REQ_READ on device "
	   << ModuleDevice << " failed!\n";
      return -2;
    }
*/

    // read data:
    ssize_t m = read( FifoFd, Buffer + readn, maxn );

    int ern = errno;
    if ( m < 0 && ern != EAGAIN && ern != EINTR ) {
      Traces->addErrorStr( ern );
      failed = true;
      cerr << "DynClampAnalogInput::readData(): error\n";
    }
    else if ( m > 0 ) {
      maxn -= m;
      readn += m;
    }

  }

  BufferN = readn / BufferElemSize;

  if( failed ) {
    /*
    switch( errnoSave ) {

    case EPIPE: 
      ErrorState = 1;
      cerr << " DynClampAnalogInput::readData(): buffer-overflow: "
      	   << strerror( errnoSave ) << '\n';/////TEST/////
      traces.addErrorStr( deviceFile() + " - buffer-underrun: "
			+ strerror( errnoSave ) );
      traces.addError( DaqError::OverflowUnderrun );
      return -1;

    case EBUSY:
      ErrorState = 2;
      cerr << " DynClampAnalogInput::readData(): device busy: "
	         << strerror( errnoSave ) << '\n';/////TEST/////
      traces.addErrorStr( deviceFile() + " - device busy: "
			+ strerror( errnoSave ) );
      traces.addError( DaqError::Busy );
      return -1;

    default:
      ErrorState = 2;
      cerr << " DynClampAnalogInput::readData(): buffer-underrun: "
	   << "  system: " << strerror( errnoSave )
	   << " (device file descriptor " << ModuleFd
	   << '\n';/////TEST/////
//      traces.addErrorStr( "Error while reading from device-file: " + deviceFile()
//			+ "  system: " + strerror( errnoSave ) );
      traces.addError( DaqError::Unknown );
      return -1;
    }
    */
  }

  // debug:
  if ( BufferN < oldbuffern )
    cerr << "DynClampAnalogInput::readData: buffer shrinking! BufferN=" << BufferN
	 << " BufferSize=" << BufferSize << " readn=" << readn << " maxn=" << maxn << " oldbuffern=" << oldbuffern << '\n';

  // no more data to be read:
  if ( BufferN <= 0 && !running() ) {
    if ( Traces->front().continuous() )
      Traces->addError( DaqError::Unknown );
    return -1;
  }

  //  cerr << "Comedi::readData() end " << BufferN << "\n";

  return BufferN;
}


int DynClampAnalogInput::convertData( void )
{
  // buffer pointers and sizes:
  float *bp[Traces->size()];
  int bm[Traces->size()];
  int bn[Traces->size()];
  for ( int k=0; k<Traces->size(); k++ ) {
    bp[k] = (*Traces)[k].pushBuffer();
    bm[k] = (*Traces)[k].maxPush();
    bn[k] = 0;
  }

  // type cast for device buffer:
  float *db = (float *)Buffer;

  // debug:
  if ( BufferN*(int)BufferElemSize > BufferSize )
    cerr << "DynClampAnalogInput::convertData: buffer overflow! BufferN=" << BufferN
	 << " BufferSize=" << BufferSize << " BufferN*BufferElemSize=" << BufferN*BufferElemSize << '\n';

  for ( int k=0; k<BufferN; k++ ) {
    // "convert:"
    *bp[TraceIndex] =  db[k];
    // update pointers:
    bp[TraceIndex]++;
    bn[TraceIndex]++;
    if ( bn[TraceIndex] >= bm[TraceIndex] ) {
      (*Traces)[TraceIndex].push( bn[TraceIndex] );
      bp[TraceIndex] = (*Traces)[TraceIndex].pushBuffer();
      bm[TraceIndex] = (*Traces)[TraceIndex].maxPush();
      bn[TraceIndex] = 0;
    }
    // next trace:
    TraceIndex++;
    if ( TraceIndex >= Traces->size() )
      TraceIndex = 0;
  }

  // commit:
  for ( int c=0; c<Traces->size(); c++ )
    (*Traces)[c].push( bn[c] );

  int n = BufferN;
  BufferN = 0;

  return n;
}


int DynClampAnalogInput::stop( void )
{ 
  if( !IsPrepared )
    return 0;

  int running = SubdeviceID;
  int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
  if( retval < 0 ) {
    cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  if ( running  > 0 ) {
    retval = ::ioctl( ModuleFd, IOC_STOP_SUBDEV, &SubdeviceID );
    if( retval < 0 ) {
      cerr << " DynClampAnalogInput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }
  }

  IsPrepared = false;
  IsRunning = false;
  return 0;
}


int DynClampAnalogInput::reset( void ) 
{ 
  int retval = stop();

  // XXX clear buffers by flushing FIFO:
  rtf_reset( FifoFd );

  // free internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = NULL;
  BufferSize = 0;
  BufferN = 0;

  Settings.clear();
  ErrorState = 0;

  return retval;
}


bool DynClampAnalogInput::running( void ) const
{
  if( !IsPrepared )
    return false;

  int exchangeVal = SubdeviceID;
  int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &exchangeVal );

  //  cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
  //       << ModuleDevice << " " << exchangeVal << '\n';

  if( retval < 0 ) {
    cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << ModuleDevice << " failed!\n";
    return false;
  }

  return exchangeVal;
}


int DynClampAnalogInput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */

}


void DynClampAnalogInput::take( vector< AnalogInput* > &ais, 
				vector< AnalogOutput* > &aos,
				vector< int > &aiinx, vector< int > &aoinx,
				vector< bool > &airate, vector< bool > &aorate )
{
  // needs to be implemented, for AO!!!!
}


void DynClampAnalogInput::addTraces( vector< TraceSpec > &traces, int deviceid ) const
{
  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = PARAM_IN;
  int channel = PARAM_CHAN_OFFSET;
  while ( 0 == ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) ) {
    traces.push_back( TraceSpec( traces.size(), traceInfo.name,
				 deviceid, channel++, 1.0, traceInfo.unit ) );
  }
  int ern = errno;
  if ( ern != ERANGE )
    cerr << "DynClampAnalogInput::addTraces() -> errno " << ern << '\n';
}


int DynClampAnalogInput::matchTraces( InList &traces ) const
{
  int foundtraces = 0;
  int tracefound[ traces.size() ];
  for ( int k=0; k<traces.size(); k++ )
    tracefound[k] = 0;

  // analog input traces:
  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = TRACE_IN;
  struct traceChannelIOCT traceChannel;
  traceChannel.traceType = TRACE_IN;
  string unknowntraces = "";
  while ( ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) == 0 ) {
    bool notfound = true;
    for ( int k=0; k<traces.size(); k++ ) {
      if ( traces[k].ident() == traceInfo.name ) {
	tracefound[k] = 1;
	if ( traces[k].unit() != traceInfo.unit )
	  traces[k].addErrorStr( "model input trace " + traces[k].ident() + " requires as unit '" + traceInfo.unit + "', not '" + traces[k].unit() + "'" );
	traceChannel.device = traces[k].device();
	traceChannel.channel = traces[k].channel();
	if ( ::ioctl( ModuleFd, IOC_SET_TRACE_CHANNEL, &traceChannel ) != 0 )
	  traces[k].addErrorStr( "failed to pass device and channel information to model input traces -> errno=" + Str( errno ) );
	notfound = false;
	foundtraces++;
	break;
      }
    }
    if ( notfound ) {
      unknowntraces += " ";
      unknowntraces += traceInfo.name;
    }
  }
  int ern = errno;
  if ( ern != ERANGE )
    traces.addErrorStr( "failure in getting model input traces -> errno=" + Str( ern ) );
  if ( ! unknowntraces.empty() )
    traces.addErrorStr( "unable to match model input traces" + unknowntraces );

  // parameter traces:
  traceInfo.traceType = PARAM_IN;
  int pchan = 0;
  while ( ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) == 0 ) {
    for ( int k=0; k<traces.size(); k++ ) {
      if ( traces[k].ident() == traceInfo.name ) {
	tracefound[k] = 1;
	if ( traces[k].unit() != traceInfo.unit )
	  traces[k].addErrorStr( "model input parameter trace " + traces[k].ident() + " requires as unit '" + traceInfo.unit + "', not '" + traces[k].unit() + "'" );
	traces[k].setChannel( PARAM_CHAN_OFFSET + pchan );
	pchan++;
	foundtraces++;
	break;
      }
    }
  }
  ern = errno;
  if ( ern != ERANGE )
    traces.addErrorStr( "failure in getting model input parameter traces -> errno=" + Str( ern ) );
  /*
  for ( int k=0; k<traces.size(); k++ ) {
    if ( tracefound[k] == 0 )
      traces[k].addErrorStr( "no matching trace found for trace " + traces[k].ident() );
  }
  */
  return traces.failed() ? -1 : foundtraces;
}


const Options &DynClampAnalogInput::settings( void ) const
{
  long long loopcnt = 0;
  double meanperiod = 0.0;
  double minperiod = 0.0;
  double maxperiod = 0.0;

  if ( ModuleFd >= 0 ) {
    long long val = 0;
    int r = ::ioctl( ModuleFd, IOC_GETLOOPCNT, &val );
    if ( r >= 0 )
      loopcnt = val;
    r = ::ioctl( ModuleFd, IOC_GETLOOPAVG, &val );
    if ( r >= 0 )
      meanperiod = 0.001*val;
    r = ::ioctl( ModuleFd, IOC_GETLOOPMIN, &val );
    if ( r >= 0 )
      minperiod = 0.001*val;
    r = ::ioctl( ModuleFd, IOC_GETLOOPMAX, &val );
    if ( r >= 0 )
      maxperiod = 0.001*val;
  }

  Settings.setInteger( "number of periods", loopcnt );
  Settings.setNumber( "average period", meanperiod, "us" );
  Settings.setNumber( "minimum period", minperiod, "us" );
  Settings.setNumber( "maximum period", maxperiod, "us" );
  return Settings;
}


bool DynClampAnalogInput::prepared( void ) const 
{ 
  return IsPrepared;
}


}; /* namespace comedi */
