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
#include <string>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <QMutexLocker>
#include <relacs/comedi/dynclampanaloginput.h>
#include <rtai_fifos.h>
using namespace std;
using namespace relacs;

namespace comedi {


DynClampAnalogInput::DynClampAnalogInput( void ) 
  : AnalogInput( "DynClampAnalogInput", DynClampAnalogIOType )
{
  SubdeviceID = -1;
  ModuleDevice = "";
  ModuleFd = -1;
  FifoFd = -1;
  SubDevice = -1;
  BufferElemSize = sizeof(float);
  Channels = 0;
  MaxRate = 50000.0;
  IsPrepared = false;
  IsRunning = false;
  Calibration = 0;
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
  SubdeviceID = -1;
  ModuleDevice = "";
  ModuleFd = -1;
  FifoFd = -1;
  SubDevice = -1;
  BufferElemSize = sizeof(float);
  Channels = 0;
  MaxRate = 50000.0;
  IsPrepared = false;
  IsRunning = false;
  Calibration = 0;
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
}


int DynClampAnalogInput::open( const string &device, const Options &opts )
{ 
  if ( isOpen() )
    return -5;

  Info.clear();
  Settings.clear();

  if ( device.empty() )
    return InvalidDevice;

  // open comedi device:
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    cerr << "! error: DynClampAnalogInput::open() -> "
	 << "Device-file " << device << " could not be opened!\n";
    return NotOpen;
  }

  // get AI subdevice:
  int subdev = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_AI, 0 );
  if ( subdev < 0 ) {
    cerr << "! error: DynClampAnalogInput::open() -> "
	 << "No subdevice for AI found on device "  << device << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    return InvalidDevice;
  }
  SubDevice = subdev;

  // set basic device infos:
  setDeviceName( comedi_get_board_name( DeviceP ) );
  setDeviceVendor( comedi_get_driver_name( DeviceP ) );
  setDeviceFile( device );
  
  Channels = comedi_get_n_channels( DeviceP, SubDevice );

  // get calibration:
  {
    char *calibpath = comedi_get_default_calibration_path( DeviceP );
    ifstream cf( calibpath );
    if ( cf.good() )
      Calibration = comedi_parse_calibration_file( calibpath );
    else
      Calibration = 0;
    free( calibpath );
  }

  // initialize ranges:
  UnipolarRange.clear();
  BipolarRange.clear();
  UnipolarRangeIndex.clear();
  BipolarRangeIndex.clear();
  vector<double> gainblacklist;
  opts.numbers( "gainblacklist", gainblacklist );
  // XXX: if a ranges is not supported but comedi thinks so: add max gain to the blacklist.
  // i.e. NI 6070E PCI and DAQCard-6062E: range #8 (0..20V) not supported
  int nRanges = comedi_get_n_ranges( DeviceP, SubDevice, 0 );  
  for ( int i = 0; i < nRanges; i++ ) {
    comedi_range *range = comedi_get_range( DeviceP, SubDevice, 0, i );
    bool add = range->max > 0.0 ? true : false;
    for ( unsigned int k=0; k<gainblacklist.size(); k++ ) {
      if ( ::fabs( range->max - gainblacklist[k] ) < 1e-6 ) {
	add = false;
	break;
      }
    }
    if ( add ) {
      if ( range->min < 0.0 ) {
	BipolarRange.push_back( *range );
	BipolarRangeIndex.push_back( i );
      }
      else {
	UnipolarRange.push_back( *range );
	UnipolarRangeIndex.push_back( i );
      }
    }
  }
  // bubble-sorting Uni/BipolarRange according to Uni/BipolarRange.max:
  for( unsigned int i = 0; i < UnipolarRangeIndex.size(); i++ ) {
    for ( unsigned int j = i+1; j < UnipolarRangeIndex.size(); j++ ) {
      if (  UnipolarRange[i].max < UnipolarRange[j].max ) {
	comedi_range rangeSwap = UnipolarRange[i];
	UnipolarRange[i] = UnipolarRange[j];
	UnipolarRange[j] = rangeSwap;
	unsigned int indexSwap = UnipolarRangeIndex[i];
	UnipolarRangeIndex[i] = UnipolarRangeIndex[j];
	UnipolarRangeIndex[j] = indexSwap;
      }
    }
  }
  for( unsigned int i = 0; i < BipolarRangeIndex.size(); i++ ) {
    for ( unsigned int j = i+1; j < BipolarRangeIndex.size(); j++ ) {
      if (  BipolarRange[i].max < BipolarRange[j].max ) {
	comedi_range rangeSwap = BipolarRange[i];
	BipolarRange[i] = BipolarRange[j];
	BipolarRange[j] = rangeSwap;
	unsigned int indexSwap = BipolarRangeIndex[i];
	BipolarRangeIndex[i] = BipolarRangeIndex[j];
	BipolarRangeIndex[j] = indexSwap;
      }
    }
  }

  // get conversion polynomials:
  bool softcal = ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_SOFT_CALIBRATED ) > 0 );
  UnipConverter = new comedi_polynomial_t* [Channels];
  BipConverter = new comedi_polynomial_t* [Channels];
  for ( int c=0; c<Channels; c++ ) {
    UnipConverter[c] = new comedi_polynomial_t[UnipolarRangeIndex.size()];
    for ( unsigned int r=0; r<UnipolarRangeIndex.size(); r++ ) {
      if ( softcal && Calibration != 0 ) {
	comedi_get_softcal_converter( SubDevice, c, UnipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL, Calibration,
				      &UnipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( DeviceP, SubDevice, c,
				      UnipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL,
				      &UnipConverter[c][r] );
      }
    }
    BipConverter[c] = new comedi_polynomial_t[BipolarRangeIndex.size()];
    for ( unsigned int r=0; r<BipolarRangeIndex.size(); r++ ) {
      if ( softcal && Calibration != 0 ) {
	comedi_get_softcal_converter( SubDevice, c, BipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL, Calibration,
				      &BipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( DeviceP, SubDevice, c,
				      BipolarRangeIndex[r],
				      COMEDI_TO_PHYSICAL, 
				      &BipConverter[c][r] );
      }
    }
  }

  // open kernel module:
  ModuleDevice = "/dev/dynclamp";
  ModuleFd = ::open( ModuleDevice.c_str(), O_RDONLY );
  if ( ModuleFd == -1 ) {
  cerr << " DynClampAnalogInput::open(): opening dynclamp-module failed\n";
    return -1;
  }

  // get subdevice ID from module:
  int retval = ::ioctl( ModuleFd, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if ( retval < 0 ) {
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
  if ( retval < 0 ) {
    cerr << " DynClampAnalogInput::open -> ioctl command IOC_OPEN_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // XXX Set the maximum possible sampling rate (of the rtai loop!):
  MaxRate = 50000.0;

  // initialize connection to RTAI-FIFO:
  ostringstream fifoname;
  fifoname << "/dev/rtf" << deviceIOC.fifoIndex;
  FifoFd = ::open( fifoname.str().c_str(), O_RDONLY );
  if ( FifoFd < 0 ) {
    cerr << " DynClampAnalogInput::open() -> oping RTAI-FIFO " 
         << fifoname.str() << " failed!\n";
    return -1;
  }
  ReadBufferSize = deviceIOC.fifoSize;

  IsPrepared = false;

  // publish information about the analog input device:
  setInfo();
  vector< TraceSpec > traces;
  traces.clear();
  addTraces( traces, 0 );
  for ( unsigned int k=0; k<traces.size(); k++ )
    Info.addText( "Model input", traces[k].traceName() );

  return 0;
}


bool DynClampAnalogInput::isOpen( void ) const 
{ 
  lock();
  bool o = ( ModuleFd >= 0 );
  unlock();
  return o;
}


void DynClampAnalogInput::close( void )
{ 
  if ( ! isOpen() )
    return;

  reset();

  if ( ModuleFd >= 0 ) {
    ::ioctl( ModuleFd, IOC_REQ_CLOSE, &SubdeviceID );
    if ( FifoFd >= 0 ) {
      ::close( FifoFd );
      FifoFd = -1;
    }
    if ( ::close( ModuleFd ) < 0 )
      cerr << "Close of module file failed!\n";
    ModuleFd = -1;
  }

  // cleanup calibration:
  if ( Calibration != 0 )
    comedi_cleanup_calibration( Calibration );
  Calibration = 0;

  // cleanup converters:
  for ( int c=0; c<Channels; c++ ) {
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
  if ( !isOpen() )
    return -1;
  lock();
  int maxData = comedi_get_maxdata( DeviceP, SubDevice, 0 );
  unlock();
  return (int)( log( maxData+2.0 )/ log( 2.0 ) );
}


double DynClampAnalogInput::maxRate( void ) const 
{ 
  return MaxRate;
}


int DynClampAnalogInput::maxRanges( void ) const
{
  return UnipolarRangeIndex.size() > BipolarRangeIndex.size() ?
    UnipolarRangeIndex.size() : BipolarRangeIndex.size();
}


double DynClampAnalogInput::unipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)UnipolarRangeIndex.size()) )
    return -1.0;
  return UnipolarRange[index].max;
}


double DynClampAnalogInput::bipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)BipolarRangeIndex.size()) )
    return -1.0;
  return BipolarRange[index].max;
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
    int subdeviceflags = comedi_get_subdevice_flags( DeviceP, SubDevice );
    switch( traces[k].reference() ) {
    case InData::RefCommon: 
      if ( subdeviceflags & SDF_COMMON )
	aref = AREF_COMMON;
      break;
    case InData::RefDifferential:
      if ( subdeviceflags & SDF_DIFF )
	aref = AREF_DIFF; 
      break;
    case InData::RefGround:
      if ( subdeviceflags & SDF_GROUND )
	aref = AREF_GROUND; 
      break;
    case InData::RefOther: 
      if ( subdeviceflags & SDF_OTHER )
	aref = AREF_OTHER;
      break;
    }
    if ( aref == -1 )
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
      double max = UnipolarRange[traces[k].gainIndex()].max;
      double min = UnipolarRange[traces[k].gainIndex()].min;
      if ( max < 0 || min < 0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( 0.0 );
      int gi = UnipolarRangeIndex[ traces[k].gainIndex() ];
      memcpy( gainp, &UnipConverter[traces[k].channel()][traces[k].gainIndex()], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( traces[k].channel(), gi, aref );
    }
    else {
      double max = BipolarRange[traces[k].gainIndex()].max;
      double min = BipolarRange[traces[k].gainIndex()].min;
      if ( max < 0 || min >= 0.0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( min );
      int gi = BipolarRangeIndex[ traces[k].gainIndex() ];
      memcpy( gainp, &BipConverter[traces[k].channel()][traces[k].gainIndex()], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( traces[k].channel(), gi, aref );
    }

  }

  return 0;
}


int DynClampAnalogInput::testReadDevice( InList &traces )
{
  if ( ! isOpen() ) {
    traces.setError( DaqError::DeviceNotOpen );
    return -1;
  }
  
  //  cerr << " DynClampAnalogInput::testRead(): 1\n";////TEST////
  QMutexLocker locker( mutex() );

  // sampling rate must be the one of the running rt-loop:
  unsigned int rate = 0;
  int retval = ::ioctl( ModuleFd, IOC_GETRATE, &rate );
  if ( retval < 0 ) {
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
    if ( traces[k].channel() < 0 ) {
      traces[k].addError( DaqError::InvalidChannel );
      traces[k].setChannel( 0 );
    }
    else if ( traces[k].channel() >= Channels && traces[k].channel() < PARAM_CHAN_OFFSET ) {
      traces[k].addError( DaqError::InvalidChannel );
      traces[k].setChannel( Channels-1 );
    }
  }

  for( int k = 0; k < traces.size(); k++ ) {

    // check delays:
    if ( traces[k].delay() > 0.0 ) {
      traces[k].addError( DaqError::InvalidDelay );
      traces[k].addErrorStr( "delays are not supported for analog input!" );
      traces[k].setDelay( 0.0 );
    }

  }

  unsigned int chanlist[MAXCHANLIST];
  setupChanList( traces, chanlist, MAXCHANLIST );

  if ( traces.failed() )
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

  QMutexLocker locker( mutex() );

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
      if ( Calibration != 0 ) {
	unsigned int channel = CR_CHAN( ChanList[k] );
	unsigned int range = CR_RANGE( ChanList[k] );
	unsigned int aref = CR_AREF( ChanList[k] );
	if ( comedi_apply_parsed_calibration( DeviceP, SubDevice, channel,
					      range, aref, Calibration ) < 0 )
	  traces[k].addError( DaqError::CalibrationFailed );
      }
    }
  }
  chanlistIOC.userDeviceIndex = traces[0].device();
  chanlistIOC.chanlistN = traces.size();
  int retval = ::ioctl( ModuleFd, IOC_CHANLIST, &chanlistIOC );
  //  cerr << "prepareRead(): IOC_CHANLIST done!\n"; /// TEST
  if ( retval < 0 ) {
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
  if ( retval < 0 ) {
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
  
  if ( traces.success() ) {
    setSettings( traces, BufferSize, ReadBufferSize );
    Settings.addInteger( "number of periods", 0 );
    Settings.addNumber( "average period", 0.0, "us" );
    Settings.addNumber( "stdev period", 0.0, "us" );
    Settings.addNumber( "minimum period", 0.0, "us" );
    Settings.addNumber( "maximum period", 0.0, "us" );
    Traces = &traces;
  }

  IsPrepared = traces.success();

  return traces.success() ? 0 : -1;
}


int DynClampAnalogInput::startRead( QSemaphore *sp, QMutex *datamutex,
				    QWaitCondition *datawait, QSemaphore *aosp )
{
  //  cerr << " DynClampAnalogInput::startRead(): 1\n";/////TEST/////
  QMutexLocker locker( mutex() );

  if ( !IsPrepared || Traces == 0 ) {
    cerr << "AI not prepared or no traces!\n";
    return -1;
  }

  // start subdevice:
  int retval = ::ioctl( ModuleFd, IOC_START_SUBDEV, &SubdeviceID );
  if ( retval < 0 ) {
    cerr << " DynClampAnalogInput::startRead -> ioctl command IOC_START_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    int ern = errno;
    if ( ern == ENOMEM )
      cerr << " !!! No stack for kernel task !!!\n";
    Traces->addErrorStr( ern );
    return -1;
  }

  // start analog input thread:
  startThread( sp, datamutex, datawait );

  // get sampling rate:
  unsigned int rate = 0;
  retval = ::ioctl( ModuleFd, IOC_GETRATE, &rate );
  if ( retval < 0 ) {
    cerr << " DynClampAnalogOutput::testWriteDevice -> ioctl command IOC_GETRATE on device "
	 << ModuleDevice << " failed!\n";
  }
  else
    Traces->setSampleRate( (double)rate );

  return 0;
}

 
int DynClampAnalogInput::readData( void )
{
  //  cerr << " DynClampAnalogInput::readData(): begin\n";/////TEST/////

  QMutexLocker locker( mutex() );

  //device stopped?
    /* // AI does not have to run in order to push data
  if ( !IsRunning ) {
    traces.addErrorStr( "DynClampAnalogInput::readData: " +
		      deviceFile() + " is not running!" );
    cerr << "DynClampAnalogInput::readData: device is not running!"  << '\n';/////TEST/////
    return 0;/////TEST/////
  }
    */

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
    if ( retval < 0 ) {
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

  if ( failed ) {
    /*
    switch( errnoSave ) {

    case EPIPE: 
      cerr << " DynClampAnalogInput::readData(): buffer-overflow: "
      	   << strerror( errnoSave ) << '\n';/////TEST/////
      traces.addErrorStr( deviceFile() + " - buffer-underrun: "
			+ strerror( errnoSave ) );
      traces.addError( DaqError::OverflowUnderrun );
      return -2;

    case EBUSY:
      cerr << " DynClampAnalogInput::readData(): device busy: "
	         << strerror( errnoSave ) << '\n';/////TEST/////
      traces.addErrorStr( deviceFile() + " - device busy: "
			+ strerror( errnoSave ) );
      traces.addError( DaqError::Busy );
      return -2;

    default:
      cerr << " DynClampAnalogInput::readData(): buffer-underrun: "
	   << "  system: " << strerror( errnoSave )
	   << " (device file descriptor " << ModuleFd
	   << '\n';/////TEST/////
//      traces.addErrorStr( "Error while reading from device-file: " + deviceFile()
//			+ "  system: " + strerror( errnoSave ) );
      traces.addError( DaqError::Unknown );
      return -2;
    }
    */
  }

  // debug:
  if ( BufferN < oldbuffern )
    cerr << "DynClampAnalogInput::readData warning: buffer shrinking! BufferN=" << BufferN
	 << " BufferSize=" << BufferSize << " readn=" << readn << " maxn=" << maxn << " oldbuffern=" << oldbuffern << '\n';

  // no more data to be read:
  if ( BufferN <= 0 && !IsRunning ) {
    if ( Traces->front().continuous() ) {
      Traces->addError( DaqError::Unknown );
      return -2;
    }
    return -1;
  }

  //  cerr << "Comedi::readData() end " << BufferN << "\n";

  return BufferN;
}


int DynClampAnalogInput::convertData( void )
{
  QMutexLocker locker( mutex() );

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
  int running = 0;

  {
    QMutexLocker locker( mutex() );

    if ( !IsPrepared )
      return 0;

    running = SubdeviceID;
    int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
    if ( retval < 0 ) {
      cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }
  } // unlock

  if ( running  > 0 ) {
    stopRead();
    QMutexLocker locker( mutex() );
    int retval = ::ioctl( ModuleFd, IOC_STOP_SUBDEV, &SubdeviceID );
    if ( retval < 0 ) {
      cerr << " DynClampAnalogInput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }
  }

  lock();
  IsPrepared = false;
  IsRunning = false;
  unlock();

  return 0;
}


int DynClampAnalogInput::reset( void ) 
{ 
  int retval = stop();

  QMutexLocker locker( mutex() );

  // XXX clear buffers by flushing FIFO:
  if ( FifoFd >= 0 )
    rtf_reset( FifoFd );

  // free internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = NULL;
  BufferSize = 0;
  BufferN = 0;

  Settings.clear();

  return retval;
}


bool DynClampAnalogInput::running( void ) const
{
  QMutexLocker locker( mutex() );

  if ( !IsPrepared )
    return false;

  int exchangeVal = SubdeviceID;
  int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &exchangeVal );

  //  cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
  //       << ModuleDevice << " " << exchangeVal << '\n';

  if ( retval < 0 ) {
    cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << ModuleDevice << " failed!\n";
    return false;
  }

  return ( exchangeVal > 0 && AnalogInput::running() );
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
  bool tracefound[ traces.size() ];
  for ( int k=0; k<traces.size(); k++ )
    tracefound[k] = false;

  // analog input traces:
  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = TRACE_IN;
  struct traceChannelIOCT traceChannel;
  traceChannel.traceType = TRACE_IN;
  string unknowntraces = "";
  while ( ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) == 0 ) {
    bool notfound = true;
    for ( int k=0; k<traces.size(); k++ ) {
      if ( traces[k].channel() < PARAM_CHAN_OFFSET && traces[k].ident() == traceInfo.name ) {
	tracefound[k] = true;
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
  for ( int pchan = 0; ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) == 0; pchan++ ) {
    for ( int k=0; k<traces.size(); k++ ) {
      if ( traces[k].channel() >= PARAM_CHAN_OFFSET && traces[k].ident() == traceInfo.name ) {
	tracefound[k] = true;
	if ( traces[k].unit() != traceInfo.unit )
	  traces[k].addErrorStr( "model input parameter trace " + traces[k].ident() + " requires as unit '" + traceInfo.unit + "', not '" + traces[k].unit() + "'" );
	traces[k].setChannel( PARAM_CHAN_OFFSET + pchan );
	foundtraces++;
	break;
      }
    }
  }
  ern = errno;
  if ( ern != ERANGE )
    traces.addErrorStr( "failure in getting model input parameter traces -> errno=" + Str( ern ) );
  for ( int k=0; k<traces.size(); k++ ) {
    if ( ! tracefound[k] && traces[k].channel() >= PARAM_CHAN_OFFSET )
      traces[k].addErrorStr( "no matching trace found for trace " + traces[k].ident() );
  }
  return traces.failed() ? -1 : foundtraces;
}


const Options &DynClampAnalogInput::settings( void ) const
{
#ifdef ENABLE_STATISTICS
  long long loopcnt = 0;
  double meanperiod = 0.0;
  double stdevperiod = 0.0;
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
    r = ::ioctl( ModuleFd, IOC_GETLOOPSQAVG, &val );
    if ( r >= 0 )
      stdevperiod = sqrt( 0.001*0.001*val - meanperiod*meanperiod );
    r = ::ioctl( ModuleFd, IOC_GETLOOPMIN, &val );
    if ( r >= 0 )
      minperiod = 0.001*val;
    r = ::ioctl( ModuleFd, IOC_GETLOOPMAX, &val );
    if ( r >= 0 )
      maxperiod = 0.001*val;
  }

  Settings.setInteger( "number of periods", loopcnt );
  Settings.setNumber( "average period", meanperiod, "us" );
  Settings.setNumber( "stdev period", stdevperiod, "us" );
  Settings.setNumber( "minimum period", minperiod, "us" );
  Settings.setNumber( "maximum period", maxperiod, "us" );
#endif

  return Settings;
}


bool DynClampAnalogInput::prepared( void ) const 
{ 
  lock();
  bool ip = IsPrepared;
  unlock();
  return ip;
}


}; /* namespace comedi */
