/*
  comedi/dynclampanaloginput.cc
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
//#include <relacs/comedi/comedianaloginput.h>
#include <relacs/comedi/dynclampanaloginput.h>
using namespace std;
using namespace relacs;

namespace comedi {


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
  FifoFd = -1;
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
  FifoFd = -1;
  Channels = 0;
  Bits =  0;
  MaxRate = 0;
  ComediBufferSize = 0;
  CAI = new ComediAnalogInput;
}


DynClampAnalogInput::~DynClampAnalogInput( void ) 
{
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

  CAIFlags = comedi_get_subdevice_flags( CAI->DeviceP, Subdevice );
 
  CAI->close();

  if ( retVal != 0 )
    return retVal;

  // open kernel module:
  Modulename = "/dev/dynclamp";
  Modulefile = ::open( Modulename.c_str(), O_RDONLY ); //O_RDONLY
  if( Modulefile == -1 ) {
  cerr << " DynClampAnalogInput::open(): opening dynclamp-module failed" 
       << endl;/////TEST/////
    return -1;
  }


  // get subdevice ID from module:
  retVal = ::ioctl( Modulefile, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::open -> ioctl command IOC_GET_SUBDEV_ID on device "
	 << Modulename << " failed!" << endl;
    return -1;
  }

  // set device and subdevice:
  struct deviceIOCT deviceIOC;
  deviceIOC.subdevID = SubdeviceID;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = Subdevice;
  deviceIOC.isOutput = 0;
  retVal = ::ioctl( Modulefile, IOC_OPEN_SUBDEV, &deviceIOC );
  cerr << " DynClampAnalogInput::open(): IOC_OPEN_SUBDEV request for address done!" /// TEST
       << &deviceIOC << endl;
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::open -> ioctl command IOC_OPEN_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    return -1;
  }
  IsKernelDaqOpened = true;

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
  reset();

  ::ioctl( Modulefile, IOC_REQ_CLOSE, &SubdeviceID );
  ::close( FifoFd );
  if( ::close( Modulefile ) < 0 )
    cerr << "Close of module file failed!" << endl;
 

}

int DynClampAnalogInput::reset( void ) 
{ 
  clearSettings();
  ErrorState = 0;
  IsPrepared = false;
  IsLoaded = false;
  IsRunning = false;
  IsKernelDaqOpened = false;

  int retVal;

  if( IsPrepared || IsLoaded ) {
   retVal = ::ioctl( Modulefile, IOC_STOP_SUBDEV, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogInput::reset -> ioctl command IOC_STOP_SUBDEV on device "
  	  << Modulename << " failed!" << endl;
      return -1;
    }

    if( retVal )
      return retVal;  

    retVal = ::ioctl( Modulefile, IOC_RELEASE_SUBDEV, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogInput::reset -> ioctl command IOC_RELEASE_SUBDEV on device "
  	 << Modulename << " failed!" << endl;
      return -1;
    }
  }

  return retVal;
  
}


int DynClampAnalogInput::stop( void )
{ 
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
    return -1;
  }

  IsPrepared = false;
  IsLoaded = false;
  IsRunning = false;
  return 0;
}


bool DynClampAnalogInput::isOpen( void ) const 
{ 
  return ( Channels >= 0 );
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

  cerr << " DynClampAnalogInput::running -> ioctl command IOC_CHK_RUNNING on device "
       << Modulename << " " << exchangeVal << endl;

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


int DynClampAnalogInput::testReadDevice( InList &sigs )
{

  ErrorState = 0;
 
  if( Modulefile < 0 ) {
    sigs.setError( DaqError::DeviceNotOpen );
    return -1;
  }
  
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
      if( SDF_COMMON  & CAIFlags )
      	aref = AREF_COMMON;
      break;
    case InData::RefDifferential:
      if( SDF_DIFF  & CAIFlags )
      	aref = AREF_DIFF; 
      break;
    case InData::RefGround:
      if( SDF_GROUND  & CAIFlags )
      	aref = AREF_GROUND; 
      break;
    case InData::RefOther: 
      if( SDF_OTHER  & CAIFlags )
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
      sigs[k].setGain( 1.0 );
      ChanList[k] = CR_PACK( sigs[k].channel(), 
			     CAI->UnipolarRangeIndex[ sigs[k].gainIndex() ], aref );
    }
    else {
      double max = bipolarRange( sigs[k].gainIndex() );
      if( max < 0 )
	sigs[k].addError( DaqError::InvalidGain );
      sigs[k].setMaxVoltage( max );
      sigs[k].setMinVoltage( -max );
      sigs[k].setGain( 1.0 );
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


  // copy and sort signal pointers:
  InList ol;
  ol.add( sigs );
  ol.sortByChannel();

  int error = testReadDevice( ol );
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

  struct chanlistIOCT chanlistIOC;
  struct syncCmdIOCT syncCmdIOC;
  int retVal;
  int errorFlag = -2;

  // set chanlist:
  chanlistIOC.subdevID = SubdeviceID;
  for( int k = 0; k < sigs.size(); k++ ) {
    chanlistIOC.chanlist[k] = ChanList[k];
    chanlistIOC.scalelist[k] = sigs[k].scale();
  }
  chanlistIOC.chanlistN = sigs.size();
  retVal = ::ioctl( Modulefile, IOC_CHANLIST, &chanlistIOC );
  cerr << "prepareRead(): IOC_CHANLIST done!" << endl; /// TEST
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::prepareRead -> ioctl command IOC_CHANLIST on device "
	 << Modulename << " failed!" << endl;
    return errorFlag;
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
    return errorFlag;
  }

  // initialize Connection to RTAI-FIFO:
  char fifoName[] = "/dev/rtf0";
  FifoFd = ::open( fifoName, O_RDONLY | O_NONBLOCK );
  if( FifoFd < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> oping RTAI-FIFO " 
         << fifoName << " failed!" << endl;
    return errorFlag;
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

  // start subdevice:
  int retVal = ::ioctl( Modulefile, IOC_START_SUBDEV, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::startRead -> ioctl command IOC_START_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    if ( errno == ENOMEM )
      cerr << " !!! No stack for kernel task !!!" << endl;
    sigs.addErrorStr( errno );
    return -1;
  }
  
  return 0;
}

 
int DynClampAnalogInput::readData( InList &sigs )
{
  //  cerr << " DynClampAnalogInput::readData(): in" << endl;/////TEST/////

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
  /*  cerr << "DynClampAnalogInput::readData: size of device buffer: " 
       << sigs[0].deviceBufferSize() << " - size of indata: " 
       << " - continuous: " << sigs[0].continuous() << endl;*/
  bool failed = false;
  int elemRead = 0;
  int bytesRead, retVal;
  int errnoSave = 0;

  if( sigs[0].deviceBufferMaxPush() <= 0 ) {
    //    sigs.addErrorStr( "DynClampAnalogInput::readData: " +
    //		      deviceFile() + " - buffer-underrun in inlist!" );
    //    sigs.addError( DaqError::OverflowUnderrun );
    /*    cerr << "DynClampAnalogInput::readData: buffer-underrun in inlist!"  
	  << endl;/////TEST/////*/
    //    sigs[0].deviceBufferReset();/////TEST////
    return 0;/////TEST////
  }
  // try to read twice
  for ( int tryit = 0;
	tryit < 2 && !failed && sigs[0].deviceBufferMaxPush() > 0; 
	tryit++ ){
/*
    retVal = ioctl( Modulefile, IOC_REQ_READ, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogInput::readData() -> ioctl command IOC_REQ_READ on device "
	   << Modulename << " failed!" << endl;
      return -2;
    }
*/
    void *deviceBuf = sigs[0].deviceBufferPushBuffer();
    bytesRead = ::read( FifoFd, deviceBuf,
                        sigs[0].deviceBufferMaxPush() * BufferElemSize );
    /*
        cerr << " DynClampAnalogInput::readData(): " << tryit << " " << bytesRead << "of "
         << sigs[0].deviceBufferMaxPush() * BufferElemSize << "bytes read:"
         << endl;/////TEST/////
    */
    errnoSave = errno;
    
    if( bytesRead < 0 && errnoSave != EAGAIN && errnoSave != EINTR ) {
      sigs.addErrorStr( errnoSave );
      failed = true;
      cerr << " DynClampAnalogInput::readData(): error" << endl;/////TEST/////
    }
    else if ( bytesRead > 0 ) {
      sigs[0].deviceBufferPush( bytesRead / BufferElemSize );
      elemRead += bytesRead / BufferElemSize;
    }

  }

  if( failed || errnoSave == EINTR )
    switch( errnoSave ) {

    case EPIPE: 
      ErrorState = 1;
      cerr << " DynClampAnalogInput::readData(): buffer-overflow: "
      	   << strerror( errnoSave ) << endl;/////TEST/////
      sigs.addErrorStr( deviceFile() + " - buffer-underrun: "
			+ strerror( errnoSave ) );
      sigs.addError( DaqError::OverflowUnderrun );
      return -1;

    case EBUSY:
      ErrorState = 2;
      cerr << " DynClampAnalogInput::readData(): device busy: "
	         << strerror( errnoSave ) << endl;/////TEST/////
      sigs.addErrorStr( deviceFile() + " - device busy: "
			+ strerror( errnoSave ) );
      sigs.addError( DaqError::Busy );
      return -1;

    default:
      ErrorState = 2;
      cerr << " DynClampAnalogInput::readData(): buffer-underrun: "
	   << "  system: " << strerror( errnoSave )
	   << " (device file descriptor " << Modulefile
	   << endl;/////TEST/////
//      sigs.addErrorStr( "Error while reading from device-file: " + deviceFile()
//			+ "  system: " + strerror( errnoSave ) );
      sigs.addError( DaqError::Unknown );
      return -1;
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
  float *db = (float *)sigs[0].deviceBuffer();
  int c = sigs[0].deviceBufferTrace();

  while ( sigs[0].deviceBufferConvert() < sigs[0].deviceBufferSize() ) {
    // convert:
    *bp[c] =  db[sigs[0].deviceBufferConvert()++];
    // update pointers:
    bp[c]++;
    bn[c]++;
    if ( bn[c] >= bm[c] ) {
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
  
  //  cerr << " DynClampAnalogInput::readData(): out" << endl;/////TEST/////
  
  return elemRead;
}

long DynClampAnalogInput::index( void )
{
  long index = 0;
  int retVal = ::ioctl( Modulefile, IOC_GETLOOPCNT, &index );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogInput::index() -> ioctl command IOC_GETLOOPCNT on device "
	   << Modulename << " failed!" << endl;
    return -1;
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


}; /* namespace comedi */
