/*
  dynclampanaloginput.cc
  Interface for accessing analog input of a daq-board via the dynamic clamp 
  kernel module.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <cmath>
#include <ctime>

//#include "comedianaloginput.h"
#include <relacs/dynclampanaloginput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

DynClampAnalogInput::DynClampAnalogInput( void ) 
  : AnalogInput( DynClampAnalogInputType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  IsLoaded = false;
  IsKernelDaqOpened = false;
  Sigs = NULL;

  BufferElemSize = sizeof(float);
  Modulename = "";
  Channels = 0;
  Bits = 0;
  MaxRate = 50000.0;
  ComediBufferSize = 0;
  CAI = new ComediAnalogInput;
}


DynClampAnalogInput::DynClampAnalogInput( const string &deviceclass ) 
  : AnalogInput( deviceclass, DynClampAnalogInputType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsLoaded = false;
  IsKernelDaqOpened = false;
  IsRunning = false;
  Sigs = NULL;

  BufferElemSize = sizeof(float);
  Modulename = "";
  Channels = 0;
  Bits =  0;
  MaxRate = 0;
  ComediBufferSize = 0;
  CAI = new ComediAnalogInput;
}


DynClampAnalogInput::~DynClampAnalogInput( void ) 
{
  ::close( Modulefile ); 
  close();
  delete CAI;
}

int DynClampAnalogInput::open( const string &devicefile, long mode )
{ 
  if ( devicefile.empty() )
    return InvalidDevice;
  setDeviceFile( devicefile );

  int retVal = CAI->open( devicefile );
  
  // copy information not available after CAI->close()
  Subdevice = CAI->subdevice();
  Channels = CAI->channels();
  Bits =  CAI->bits();
  MaxRate = CAI->maxRate();  // XXX This is not the max freq of the real time loop!
  MaxRate = 50000.0;
  ComediBufferSize = CAI->bufferSize();
 
//  CAI->close();

  if ( retVal != 0 )
    return retVal;

  // open kernel module:
  Modulename = "/dev/dynclamp";
  Modulefile = ::open( Modulename.c_str(), O_RDONLY ); //O_RDONLY
  if( Modulefile == -1 ) {
  cerr << " DynClampAnalogInput::open(): opening dynclamp-module failed" 
       << endl;/////TEST/////
    return ErrorState;
  }


  return 0;
}


int DynClampAnalogInput::setModuleName( string modulename )
{
  Modulename = modulename;
  // TODO: test opening here?
  return 0;
}


void DynClampAnalogInput::close( void )
{ 
  if( CAI->isOpen() )
    CAI->close();


  reset();

}

int DynClampAnalogInput::reset( void ) 
{ 
  clearSettings();
  ErrorState = 0;
  IsPrepared = false;
  IsLoaded = false;
  IsRunning = false;
  IsKernelDaqOpened = false;

  if( CAI->isOpen() )
    return CAI->reset();

  int retVal;

  if( IsPrepared || IsLoaded ) {
   retVal = ioctl( Modulefile, IOC_STOP_SUBDEV, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogInput::stop -> ioctl command IOC_STOP_SUBDEV on device "
  	  << Modulename << " failed!" << endl;
      ErrorState = -2;
      return -1;
    }

    if( retVal )
      return retVal;  

    retVal = ::ioctl( Modulefile, IOC_RELEASE_SUBDEV, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogInput::close -> ioctl command IOC_RELEASE_SUBDEV on device "
  	 << Modulename << " failed!" << endl;
      ErrorState = -2;
      return -1;
    }
  }

  return retVal;
  
}


int DynClampAnalogInput::stop( void )
{ 
  if( CAI->isOpen() )
    return CAI->stop();

  if( !IsLoaded )
    return 0;

  int exchangeVal = SubdeviceID;
  int retVal = ::ioctl( Modulefile, IOC_CHK_RUNNING, &exchangeVal );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << Modulename << " failed!" << endl;
  }

  retVal = ::ioctl( Modulefile, IOC_STOP_SUBDEV, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    ErrorState = -2;
    return -1;
  }

  IsLoaded = false;
  IsRunning = false;
  return 0;
}

int DynClampAnalogInput::reload( void )
{
  if( CAI->isOpen() )
    return CAI->reload();

  /*
  cerr << " DynClampAnalogInput::reload()" << endl;/////TEST/////
  if( loaded() )
    return 0;

  if( !prepared() ||  comedi_command( DeviceP, &Cmd ) < 0 ) {
    cerr << "DynClampAnalogInput::reload()-> " << deviceFile()
	 <<" - execution of comedi_cmd failed: " << comedi_strerror( comedi_errno() );
	 cerr << " DynClampAnalogInput::reload() -> ERROR!" << endl;/////TEST/////
    return ReadError;
  }
  */

  return 0;
  
}

bool DynClampAnalogInput::isOpen( void ) const 
{ 
  if( IsLoaded )
      return true;
  return CAI->isOpen();
}

bool DynClampAnalogInput::prepared( void ) const 
{ 
  return IsPrepared;
}

bool DynClampAnalogInput::loaded( void ) const 
{
  return IsLoaded;
}

bool DynClampAnalogInput::running( void ) const
{
  if( !IsLoaded )
    return false;

  int exchangeVal = SubdeviceID;
  int retVal = ::ioctl( Modulefile, IOC_CHK_RUNNING, &exchangeVal );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << Modulename << " failed!" << endl;
    return false;
  }

  return exchangeVal;
}

void DynClampAnalogInput::setRunning( void )
{
  IsRunning = true;
}

string DynClampAnalogInput::moduleName( void ) const
{
  return Modulename;
}

int DynClampAnalogInput::subdevice( void ) const
{
  return Subdevice;
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

int DynClampAnalogInput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */

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


template < typename T >
int DynClampAnalogInput::convert( InList &sigs )
{

/* convert() aus AnalogInput.h ...
   => ODER: einfach setGain(1.0) & setOffset(0.0) ??? */
  
    // scale factors and offsets:
  double scale[sigs.size()];
  double offs[sigs.size()];
  for ( int k=0; k<sigs.size(); k++ ) {
    scale[k] = 1.0; //sigs[k].gain() * sigs[k].scale();
    offs[k] = 0.0;  //sigs[k].offset();
  }

  // buffer pointers and sizes:
  float *bp[sigs.size()];
  int bm[sigs.size()];
  int bn[sigs.size()];
  for ( int k=0; k<sigs.size(); k++ ) {
    bp[k] = sigs[k].pushBuffer();
    bm[k] = sigs[k].maxPush();
    bn[k] = 0;
  }

  // type cast for device buffer:
  T *db = (T *)sigs[0].deviceBuffer();
  int c = sigs[0].deviceBufferTrace();

  while ( sigs[0].deviceBufferConvert() < sigs[0].deviceBufferSize() ) {
    // convert:
    *bp[c] =  offs[c] + scale[c] * db[sigs[0].deviceBufferConvert()++];
    // update pointers:
    bp[c]++;
    bn[c]++;
    if ( bn[c] >= bm[c] ) {
cerr << "push " << bn[c] << " " << bm[c] << '\n';
      sigs[c].push( bn[c] );
      bp[c] = sigs[c].pushBuffer();
      bm[c] = sigs[c].maxPush();
      bn[c] = 0;
    }
    // next trace:
    c++;
    if ( c >= sigs.size() )
      c = 0;
  }

  // commit:
  for ( int k=0; k<sigs.size(); k++ )
    sigs[k].push( bn[k] );
  sigs[0].deviceBufferTrace() = c;


  return 0;
}

int DynClampAnalogInput::convertData( InList &sigs )
{
  /*
  if( CAI->LongSampleType )
    return convert<lsampl_t>( sigs );// default for NI E-series, since the mio-driver does not return a valid frequency)
  
  return convert<sampl_t>( sigs );
  */
  return convert<float>( sigs );
}

int DynClampAnalogInput::testReadDevice( InList &sigs )
{

  ErrorState = 0;
  
  if( IsKernelDaqOpened )
    return DaqError::Busy;

  // XXX open and close is only neede
  if( !CAI->isOpen() ) {
    int retVal = CAI->open( deviceFile() );
    if( retVal )
      return DaqError::DeviceNotOpen;
  }

  // ...USER-SPACE DEVICE TESTING STARTING...
  
  cerr << " DynClampAnalogInput::testRead(): 1" << endl;/////TEST/////


  memset( ChanList, 0, sizeof( ChanList ) );
  // find ranges for synchronous acquisition:
  for( int k = 0; k < sigs.size(); k++ ) {

    if( sigs[k].delay() > 1.0e-9 ) {
      sigs.addError( DaqError::InvalidDelay );
      sigs.addErrorStr( "delays are not supported and will be ignored!" );
    }

    int aref = -1;
    switch( sigs[k].reference() ) {
    case InData::RefCommon: 
      if( SDF_COMMON  & comedi_get_subdevice_flags( CAI->DeviceP, Subdevice ) )
      	aref = AREF_COMMON;
      break;
    case InData::RefDifferential:
      if( SDF_DIFF  & comedi_get_subdevice_flags( CAI->DeviceP, Subdevice ) )
      	aref = AREF_DIFF; 
      break;
    case InData::RefGround:
      if( SDF_GROUND  & comedi_get_subdevice_flags( CAI->DeviceP, Subdevice ) )
      	aref = AREF_GROUND; 
      break;
    case InData::RefOther: 
      if( SDF_OTHER  & comedi_get_subdevice_flags( CAI->DeviceP, Subdevice ) )
      	aref = AREF_OTHER;
      break;
    }
    if( aref == -1 )
      sigs[k].addError( DaqError::InvalidReference );

    if( sigs[k].unipolar() ) {
      double max = unipolarRange( sigs[k].gainIndex() );
      if( max < 0 )
	sigs[k].addError( DaqError::InvalidGain );
      sigs[k].setMaxVoltage( max );
      sigs[k].setMinVoltage( 0.0 );
      sigs[k].setGain( max / comedi_get_maxdata( CAI->DeviceP, Subdevice, 0 ) );
      ChanList[k] = CR_PACK( sigs[k].channel(), 
			     CAI->UnipolarRangeIndex[ sigs[k].gainIndex() ], aref );
    }
    else {
      double max = bipolarRange( sigs[k].gainIndex() );
      if( max < 0 )
	sigs[k].addError( DaqError::InvalidGain );
      sigs[k].setMaxVoltage( max );
      sigs[k].setMinVoltage( -max );
      sigs[k].setGain( 2.0*max / comedi_get_maxdata( CAI->DeviceP, Subdevice, 0 ) );
      ChanList[k] = CR_PACK( sigs[k].channel(), 
			     CAI->BipolarRangeIndex[ sigs[k].gainIndex() ], aref );
      cerr << " DynClampAnalogInput::testReadDevice(): max = " << max << endl;/////TEST/////  
    }

  }
  if( sigs.failed() )
    return -1;

  cerr << " DynClampAnalogInput::testRead(): success" << endl;/////TEST/////


  return 0;
}


int DynClampAnalogInput::prepareRead( InList &sigs )
{
  cerr << " DynClampAnalogInput::prepareRead(): 1" << endl;/////TEST/////

  reset();

  // copy and sort signal pointers:
  InList ol;
  ol.add( sigs );
  ol.sortByChannel();

  int error = testReadDevice( ol );
  CAI->close();
  if( error )
    return error;

  cerr << " DynClampAnalogInput::prepareRead(): 2" << endl;/////TEST/////

  if ( ol.success() )
    setSettings( ol );
  else 
    return -1;

  cerr << " DynClampAnalogInput::prepareRead(): 3" << endl;/////TEST/////

  // setup the buffer (don't free the buffer!):
  if ( sigs[0].deviceBuffer() == NULL ) {
    // size of buffer:
    sigs[0].reserveDeviceBuffer( sigs.size() * sigs[0].indices( sigs[0].updateTime() ),
				   BufferElemSize );
    if ( sigs[0].deviceBuffer() == NULL )
      sigs[0].reserveDeviceBuffer( sigs.size() * sigs[0].capacity(),
				     BufferElemSize );
  }
  // buffer overflow:
  if ( sigs[0].deviceBufferSize() >= sigs[0].deviceBufferCapacity() ) {
    sigs.addError( DaqError::BufferOverflow );
    return -1;
  }

  Sigs = &sigs;

  struct deviceIOCT deviceIOC;
  struct chanlistIOCT chanlistIOC;
  struct syncCmdIOCT syncCmdIOC;
  int retVal;
  ErrorState = -2;

  // get subdevice ID from module:
  retVal = ::ioctl( Modulefile, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::prepareRead -> ioctl command IOC_GET_SUBDEV_ID on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
  }

  // set device and subdevice:
  deviceIOC.subdevID = SubdeviceID;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = Subdevice;
  deviceIOC.isOutput = 0;
  retVal = ::ioctl( Modulefile, IOC_OPEN_SUBDEV, &deviceIOC );
  cerr << "prepareRead(): IOC_OPEN_SUBDEV request for address done!" /// TEST
       << &deviceIOC << endl;
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::prepareRead -> ioctl command IOC_OPEN_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
  }

  // set chanlist:
  chanlistIOC.subdevID = SubdeviceID;
  for( int k = 0; k < sigs.size(); k++ )
    chanlistIOC.chanlist[k] = ChanList[k];
  chanlistIOC.chanlistN = sigs.size();
  retVal = ::ioctl( Modulefile, IOC_CHANLIST, &chanlistIOC );
  cerr << "prepareRead(): IOC_CHANLIST done!" << endl; /// TEST
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::prepareRead -> ioctl command IOC_CHANLIST on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
  }

  // set up synchronous command:
  syncCmdIOC.subdevID = SubdeviceID;
  syncCmdIOC.frequency = (unsigned int)sigs[0].sampleRate();
  syncCmdIOC.duration = sigs[0].capacity() + sigs[0].indices( sigs[0].delay());
  syncCmdIOC.continuous = sigs[0].continuous();
  retVal = ::ioctl( Modulefile, IOC_SYNC_CMD, &syncCmdIOC );
  cerr << "prepareRead(): IOC_SYNC_CMD done!" << endl; /// TEST
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::prepareRead -> ioctl command IOC_SYNC_CMD on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
  }

  IsLoaded = true;
  IsPrepared = true;

  cerr << " DynClampAnalogInput::prepareRead(): success" << endl;/////TEST/////

  return 0; //elemRead;
}


int DynClampAnalogInput::startRead( InList &sigs )
{
  cerr << " DynClampAnalogInput::startRead(): 1" << endl;/////TEST/////

  if( !prepared() ) {
    sigs.addError( DaqError::Unknown );
    return -1;
  }

  ErrorState = 0;


  // start subdevice:
  int retVal = ::ioctl( Modulefile, IOC_START_SUBDEV, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::startRead -> ioctl command IOC_START_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    ErrorState = 2;
    return ErrorState;
  }
  
  return ErrorState; //elemRead;
}

 
int DynClampAnalogInput::fillReadBuffer( void )
{
  /*
  ErrorState = 0;
  cerr << "DynClampAnalogInput::readData: size of device buffer: " 
       << (*Sigs)[0].deviceBufferSize() << " - size of indata: " 
       << " - continuous: " << (*Sigs)[0].continuous() << endl;
  bool failed = false;
  int elemRead = 0;
  int bytesRead;

  if( (*Sigs)[0].deviceBufferMaxPop() <= 0 ) {
    //    ErrorState = 1;
    //    (*Sigs).addErrorStr( "DynClampAnalogInput::readData: " +
    //		      deviceFile() + " - buffer-underrun in inlist!" );
    //    (*Sigs).addError( DaqError::OverflowUnderrun );
    cerr << "DynClampAnalogInput::readData: buffer-underrun in inlist!"  
	 << endl;/////TEST/////
    //    (*Sigs)[0].deviceBufferReset();/////TEST////
    return 0;/////TEST////
  }
  // try to read twice
  for ( int tryit = 0;
	tryit < 2 && !failed && (*Sigs)[0].deviceBufferMaxPop() > 0; 
	tryit++ ){
    
    int retVal = ioctl( Modulefile, IOC_REQ_READ, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogInput::readData() -> ioctl command IOC_REQ_READ on device "
	   << Modulename << " failed!" << endl;
      ErrorState = -2;
      return ErrorState;
    }
    bytesRead = read( Modulefile, (*Sigs)[0].deviceBufferPopBuffer(),
			  (*Sigs)[0].deviceBufferMaxPop() * BufferElemSize );
    cerr << " DynClampAnalogInput::readData():  bytes read:" << bytesRead << endl;/////TEST/////

    if( bytesRead < 0 && errno != EAGAIN && errno != EINTR ) {
      (*Sigs).addErrorStr( errno );
      failed = true;
      cerr << " DynClampAnalogInput::readData(): error" << endl;/////TEST/////
    }
    else if ( bytesRead > 0 ) {
      (*Sigs)[0].deviceBufferPop( bytesRead / BufferElemSize );
      elemRead += bytesRead / BufferElemSize;
    }

  }

  if( failed || errno == EAGAIN || errno == EINTR )
    switch( errno ) {

    case EPIPE: 
      ErrorState = 1;
      (*Sigs).addErrorStr( deviceFile() + " - buffer-underrun: "
			+ comedi_strerror( comedi_errno() ) );
      (*Sigs).addError( DaqError::OverflowUnderrun );
      cerr << " DynClampAnalogInput::readData(): buffer-underrun: "
	   << comedi_strerror( comedi_errno() ) << endl;/////TEST/////
      return -1;

    case EBUSY:
      ErrorState = 2;
      (*Sigs).addErrorStr( deviceFile() + " - device busy: "
			+ comedi_strerror( comedi_errno() ) );
      (*Sigs).addError( DaqError::Busy );
      cerr << " DynClampAnalogInput::readData(): device busy: "
	   << comedi_strerror( comedi_errno() ) << endl;/////TEST/////
      return -1;

    default:
      ErrorState = 2;
      (*Sigs).addErrorStr( "Error while reading from device-file: " + deviceFile()
			+ "  comedi: " + comedi_strerror( comedi_errno() )
			+ "  system: " + strerror( errno ) );
      cerr << " DynClampAnalogInput::readData(): buffer-underrun: "
	   << "  comedi: " << comedi_strerror( comedi_errno() ) 
	   << "  system: " << strerror( errno )
	
	   << endl;/////TEST/////
      (*Sigs).addError( DaqError::Unknown );
      return -1;
    }
  
  cerr << " DynClampAnalogInput::readData(): out" << endl;/////TEST/////
  
  return elemRead;
  */
}

int DynClampAnalogInput::readData( InList &sigs )
{
  cerr << " DynClampAnalogInput::readData(): in" << endl;/////TEST/////

  //device stopped?
    /* // AI does not have to run in order to push data
  if( !running() ) {
    sigs.addErrorStr( "DynClampAnalogInput::readData: " +
		      deviceFile() + " is not running!" );
    cerr << "DynClampAnalogInput::readData: device is not running!"  << endl;/////TEST/////
    return 0;/////TEST/////
  }
    */

  ErrorState = 0;
  bool failed = false;
  int elemRead = 0;
  int bytesRead, retVal;

  if( sigs[0].deviceBufferMaxPush() <= 0 ) {
    //    ErrorState = 1;
    //    sigs.addErrorStr( "DynClampAnalogInput::readData: " +
    //		      deviceFile() + " - buffer-underrun in inlist!" );
    //    sigs.addError( DaqError::OverflowUnderrun );
    cerr << "DynClampAnalogInput::readData: buffer-underrun in inlist!"  
	 << endl;/////TEST/////
    //    sigs[0].deviceBufferReset();/////TEST////
    return 0;/////TEST////
  }
  // try to read twice
  for ( int tryit = 0;
	tryit < 2 && !failed && sigs[0].deviceBufferMaxPush() > 0; 
	tryit++ ){
    
    retVal = ioctl( Modulefile, IOC_REQ_READ, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogInput::readData() -> ioctl command IOC_REQ_READ on device "
	   << Modulename << " failed!" << endl;
      ErrorState = -2;
      return ErrorState;
    }
    void *deviceBuf = sigs[0].deviceBufferPushBuffer();
    bytesRead = ::read( Modulefile, deviceBuf,
                        sigs[0].deviceBufferMaxPush() * BufferElemSize );
  /*  cerr << " DynClampAnalogInput::readData():" << bytesRead << "of "
         << sigs[0].deviceBufferMaxPush() * BufferElemSize << "bytes read:"
         << endl;/////TEST/////
*/

    if( bytesRead < 0 && errno != EAGAIN && errno != EINTR ) {
      sigs.addErrorStr( errno );
      failed = true;
      cerr << " DynClampAnalogInput::readData(): error" << endl;/////TEST/////
    }
    else if ( bytesRead > 0 ) {
      sigs[0].deviceBufferPush( bytesRead / BufferElemSize );
      elemRead += bytesRead / BufferElemSize;
    }

  }

  if( failed || errno == EAGAIN || errno == EINTR )
    switch( errno ) {

    case EPIPE: 
      ErrorState = 1;
      sigs.addErrorStr( deviceFile() + " - buffer-underrun: "
			+ strerror( errno ) );
      sigs.addError( DaqError::OverflowUnderrun );
      cerr << " DynClampAnalogInput::readData(): buffer-overflow: "
      	   << strerror( errno ) << endl;/////TEST/////
      return -1;

    case EBUSY:
      ErrorState = 2;
      sigs.addErrorStr( deviceFile() + " - device busy: "
			+ strerror( errno ) );
      sigs.addError( DaqError::Busy );
      cerr << " DynClampAnalogInput::readData(): device busy: "
	         << strerror( errno ) << endl;/////TEST/////
      return -1;

    default:
      ErrorState = 2;
      sigs.addErrorStr( "Error while reading from device-file: " + deviceFile()
			+ "  system: " + strerror( errno ) );
      cerr << " DynClampAnalogInput::readData(): buffer-underrun: "
	   << "  system: " << strerror( errno )
     << " (device file descriptor " << Modulefile
	   << endl;/////TEST/////
      sigs.addError( DaqError::Unknown );
      return -1;
    }

  convert<float>( sigs );

  cerr << " DynClampAnalogInput::readData(): out" << endl;/////TEST/////
  
  return elemRead;
}

long DynClampAnalogInput::index( void )
{
  long index = 0;
  int retVal = ::ioctl( Modulefile, IOC_GETLOOPCNT, &index );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::index() -> ioctl command IOC_GETLOOPCNT on device "
	   << Modulename << " failed!" << endl;
    ErrorState = -2;
    return ErrorState;
  }
  return index;

}

void DynClampAnalogInput::take( vector< AnalogInput* > &ais, 
				                        vector< AnalogOutput* > &aos,
				                        vector< int > &aiinx, 
                                vector< int > &aoinx )
{
  /*
  cerr << " DynClampAnalogInput::take(): 1" << endl;/////TEST/////
  ComediAIs.clear();
  ComediAOs.clear();
  ComediAIsLink.clear();
  ComediAOsLink.clear();
  
  for ( unsigned int k=0; k<ais.size(); k++ ) {
    if ( ais[k]->analogInputType() == ComediAnalogInput::ComediAnalogInputType ) {
      aiinx.push_back( k );
      ComediAIs.push_back( dynamic_cast< ComediAnalogInput* >( ais[k] ) );
      ComediAIsLink.push_back( -1 );
    }
  }

  bool weAreMember = false;
  for ( unsigned int k=0; k<aos.size(); k++ ) {
    if ( aos[k]->analogInputType() == ComediAnalogOuput::DynClampAnalogInputType ) {
      aoinx.push_back( k );
      ComediAOs.push_back( dynamic_cast< DynClampAnalogOutput* >( aos[k] ) );
      ComediAOsLink.push_back( -1 );
      if( ComediAOs[k]->deviceName() == deviceName() )
	weAreMember = true;

    }
  }
  if( !weAreMember ) {
    ComediAOs.push_back( this );
    ComediAOsLink.push_back( -1 );
  }

  // find subdevices to be started together within the same instruction list
  for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
    for( unsigned int ai = 0; ai < ComediAIs.size(); ai++ )
      if( ComediAOs[ao]->deviceName() == ComediAOs[ai]->deviceName() ) {
	ComediAOsLink[ao] = ai;
	ComediAIsLink[ai] = ao;
      }
  */
}

#include "moc_dynclampanaloginput.cc"
