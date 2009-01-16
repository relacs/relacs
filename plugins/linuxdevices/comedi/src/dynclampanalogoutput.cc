/*
  coemdi/dynclampanalogoutput.cc
  Interface for accessing analog output of a daq-board via the dynamic clamp 
  kernel module.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
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
#include <relacs/comedi/dynclampanalogoutput.h>
#include <relacs/comedi/dynclampanaloginput.h>
#include <rtai_fifos.h>
using namespace std;
using namespace relacs;

namespace comedi {


DynClampAnalogOutput::DynClampAnalogOutput( void ) 
  : AnalogOutput( "Dyn Clamp Analog Output", DynClampAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  IsLoaded = false;
  IsKernelDaqOpened = false;
  Sigs = NULL;
  Channels = -1;
 
  BufferElemSize = sizeof(float);
  Modulename = "";
  FifoFd = -1;
  Channels = 0;
  Bits = 0;
  MaxRate = 50000.0;
  ComediBufferSize = 0;
  CAO = new ComediAnalogOutput;
}


DynClampAnalogOutput::DynClampAnalogOutput( const string &device, long mode ) 
  : AnalogOutput( "Dyn Clamp Analog Output", DynClampAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsLoaded = false;
  IsKernelDaqOpened = false;
  IsRunning = false;
  Sigs = NULL;
  Channels = -1;

  BufferElemSize = sizeof(float);
  Modulename = "";
  FifoFd = -1;
  Channels = 0;
  Bits =  0;
  MaxRate = 50000.0;
  ComediBufferSize = 0;
  CAO = new ComediAnalogOutput;
  open( device, mode );
}


DynClampAnalogOutput::~DynClampAnalogOutput( void ) 
{
  close();
  delete CAO;
}


int DynClampAnalogOutput::open( const string &device, long mode )
{ 
  if ( device.empty() )
    return InvalidDevice;
  setDeviceFile( device );

  int retVal = CAO->open( device );
  
  // copy information not available after CAO->close()
  Subdevice = CAO->comediSubdevice();
  Channels = CAO->channels();
  Bits =  CAO->bits();
  MaxRate = CAO->maxRate();  // XXX This is not the max freq of the real time loop!
  MaxRate = 50000.0;
  ComediBufferSize = CAO->bufferSize();
 
  CAO->close();

  if ( retVal != 0 )
    return retVal;

  // open kernel module:
  Modulename = "/dev/dynclamp";
  Modulefile = ::open( Modulename.c_str(), O_WRONLY ); //O_RDONLY
  if( Modulefile == -1 ) {
    cerr << " DynClampAnalogOutput::open(): opening dynclamp-module failed" 
       << endl;/////TEST/////
    return -1;
  }
  cerr << " DynClampAnalogOutput::open() success\n" ;

  // get subdevice ID from module:
  retVal = ::ioctl( Modulefile, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::open -> ioctl command IOC_GET_SUBDEV_ID on device "
	 << Modulename << " failed!" << endl;
    return -1;
  }

  // set device and subdevice:
  struct deviceIOCT deviceIOC;
  deviceIOC.subdevID = SubdeviceID;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = Subdevice;
  deviceIOC.subdevType = SUBDEV_OUT;
  retVal = ::ioctl( Modulefile, IOC_OPEN_SUBDEV, &deviceIOC );
  cerr << "open(): IOC_OPEN_SUBDEV request for address done!" /// TEST
       << &deviceIOC << endl;
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::open -> ioctl command IOC_OPEN_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    return -1;
  }
  IsKernelDaqOpened = 1;

  return 0;
}


bool DynClampAnalogOutput::isOpen( void ) const 
{ 
  return ( Modulefile >= 0 );
}


void DynClampAnalogOutput::close( void )
{ 
  reset();
  if( CAO->isOpen() ) {
    CAO->close();
    return;
  }
  ::ioctl( Modulefile, IOC_REQ_CLOSE, &SubdeviceID );
  ::close( FifoFd );
  if( ::close( Modulefile ) < 0 )
    cerr << "Close of module file failed!" << endl;
  Channels = -1;
  Modulefile = -1;

}


int DynClampAnalogOutput::setModuleName( string modulename )
{
  Modulename = modulename;
  // TODO: test opening here?
  return 0;
}


string DynClampAnalogOutput::moduleName( void ) const
{
  return Modulename;
}


int DynClampAnalogOutput::channels( void ) const
{ 
  return Channels;
}


int DynClampAnalogOutput::bits( void ) const
{ 
  return Bits;
}


double DynClampAnalogOutput::maxRate( void ) const 
{ 
  // TODO: 
  //         return "maximum supported dyn-clamp Rate for model 
  //                (provided by DynClamp class)"
  return MaxRate;
}


int DynClampAnalogOutput::maxRanges( void ) const
{
  return CAO->maxRanges();
}


double DynClampAnalogOutput::unipolarRange( int index ) const
{
  return CAO->unipolarRange( index );
}


double DynClampAnalogOutput::bipolarRange( int index ) const
{
  return CAO->bipolarRange( index );
}


int DynClampAnalogOutput::testWriteDevice( OutList &sigs )
{

  ErrorState = 0;
 
  if( Modulefile < 0 ) {
    sigs.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  // ...USER-SPACE DEVICE TESTING STARTING...

  cerr << " DynClampAnalogOutput::testWrite(): 1" << endl;////TEST////

  // channel configuration:
  if ( sigs[0].error() == DaqError::InvalidChannel ) {
    for ( int k=0; k<sigs.size(); k++ ) {
      sigs[k].delError( DaqError::InvalidChannel );
      // check channel number:
      if( sigs[k].channel() < 0 ) {
	sigs[k].addError( DaqError::InvalidChannel );
	sigs[k].setChannel( 0 );
      }
      else if( sigs[k].channel() >= channels() && sigs[k].channel() < PARAM_CHAN_OFFSET ) {
	sigs[k].addError( DaqError::InvalidChannel );
	sigs[k].setChannel( channels()-1 );
      }
    }
  }

  memset( ChanList, 0, sizeof( ChanList ) );
  // find ranges for synchronous acquisition:
  int aref = AREF_GROUND;
  int maxdata = ( 1 << bits() ) - 1;
  for ( int k=0; k<sigs.size(); k++ ) {
    // minimum and maximum values:
    double min = sigs[k].requestedMin();
    double max = sigs[k].requestedMax();
    if ( min == OutData::AutoRange || max == OutData::AutoRange ) {
      double smin = 0.0;
      double smax = 0.0;
      minMax( smin, smax, sigs[k] );
      if ( min == OutData::AutoRange )
	min = smin;
      if ( max == OutData::AutoRange )
	max = smax;
    }
    // reference and polarity:
    bool unipolar = false;
    if ( min >= 0.0 )
      unipolar = true;
    double extref = false;
    if ( max == OutData::ExtRef )
      extref = true;
    else {
      // maximum value:
      min = ::fabs( min );
      if ( min > max )
	max = min;
    }
    // set range:
    double maxboardvolt = -1.0;
    double minboardvolt = 0.0;
    double maxvolt = sigs[k].getVoltage( max );
    int index = -1;
    for ( int p=0; p<2 && index < 0; p++ ) {
      if ( unipolar ) {
	for( index = CAO->UnipolarRange.size() - 1; index >= 0; index-- ) {
	  if ( unipolarRange( index ) > maxvolt ) {
	    maxboardvolt = CAO->UnipolarRange[index].max;
	    minboardvolt = CAO->UnipolarRange[index].min;
	    break;
	  }
	}
      }
      else {
	for( index = CAO->BipolarRange.size() - 1; index >= 0; index-- ) {
	  if ( bipolarRange( index ) > maxvolt ) {
	    maxboardvolt = CAO->BipolarRange[index].max;
	    minboardvolt = CAO->BipolarRange[index].min;
	    break;
	  }
	}
      }
      // try other polarity?
      if ( index < 0 && p == 0 )
	unipolar = ! unipolar;
    }
    // none of the available ranges contains the requested range:
    if ( index < 0 ) {
      sigs[k].addError( DaqError::InvalidGain );
      break;
    }

    if ( sigs[k].noIntensity() ) {
      /*
      if ( ! extref && externalReference() > 0.0 ) {
      	if ( externalReference() < maxboardvolt ) {
      	  if ( maxvolt < externalReference() )
      	    extref = true;
      	}
      	else {
      	  if ( maxboardvolt == -1.0 )
      	    extref = true;
	}
      }
      if ( extref ) {
      	if ( externalReference() < 0.0 ) {
      	  sigs[k].addError( DaqError::InvalidReference );
          cerr << " DynClampAnalogOutput::testWrite(): ERROR - InvalidRef" 
               << " => external reference is not connected to the daq board"
               << endl;/////TEST/////
      	  extref = false;
      	}
      	else {
      	  if ( externalReference() == 0.0 )
	          maxboardvolt = 1.0;
      	  else
      	    maxboardvolt = externalReference();
      	  index = unipolar ? CAO->UnipolarExtRefRangeIndex 
	                         :  CAO->BipolarExtRefRangeIndex;
	}
      }
      */
      sigs[k].setGain( maxdata/(maxboardvolt-minboardvolt), -minboardvolt );
    }
    else {
      /*
      if ( extref && externalReference() < 0.0 ) {
      	sigs[k].addError( DaqError::InvalidReference );
        cerr << " DynClampAnalogOutput::testWrite(): ERROR - InvalidRef 2" << endl;/////TEST/////
      	extref = false;
      }
      */
      if ( unipolar )
	sigs[k].setGain( maxdata, 0.0 );
      else
	sigs[k].setGain( maxdata/2.0, 1.0 );
    }

    int gainIndex = index;
    if ( unipolar )
      gainIndex |= 1<<14;
    if ( extref )
      gainIndex |= 1<<15;
    

    sigs[k].setGainIndex( gainIndex );
    sigs[k].setMinData( 0 );
    sigs[k].setMaxData( maxdata );

    // set up channel in chanlist:
    if( unipolar )
      ChanList[k] = CR_PACK( sigs[k].channel(),
			     CAO->UnipolarRangeIndex[index], aref );
    else
      ChanList[k] = CR_PACK( sigs[k].channel(),
			     CAO->BipolarRangeIndex[index], aref );
    cerr << "DYNCLAMP ChanList channel " << sigs[k].channel() << " packed " << CR_CHAN( ChanList[k] ) << endl;

  }

  if( sigs.failed() )
    return -1;

  cerr << " DynClampAnalogOutput::testWrite(): success" << endl;/////TEST/////


  return 0;
}


int DynClampAnalogOutput::convertData( OutList &sigs )
{
  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // allocate buffer:
  int nbuffer = ol.size()*ol[0].size();
  float *buffer = new float [nbuffer];

  // multiplex data into buffer:
  float *bp = buffer;
  for ( int i=0; i<ol[0].size(); i++ ) {
    for ( int k=0; k<ol.size(); k++ ) {
      *bp = ol[k][i];
      ++bp;
    }
  }

  sigs[0].setDeviceBuffer( (char *)buffer, nbuffer, sizeof( float ) );

  return 0;
}


int DynClampAnalogOutput::prepareWrite( OutList &sigs )
{
  cerr << " DynClampAnalogOutput::prepareWrite(): 1" << endl;/////TEST/////

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  int error = testWriteDevice( ol );
  if( error )
    return error;

  cerr << " DynClampAnalogOutput::prepareWrite(): 2" << endl;/////TEST/////
  if ( ol.success() )
    setSettings( ol );
  else 
    return -1;

  cerr << " DynClampAnalogOutput::prepareWrite(): 3" << endl;////TEST/////

  Sigs = &sigs;

  struct chanlistIOCT chanlistIOC;
  struct syncCmdIOCT syncCmdIOC;
  int retVal;
  int errorFlag = -2;

  // set chanlist:
  chanlistIOC.subdevID = SubdeviceID;
  for( int k = 0; k < sigs.size(); k++ ){
    chanlistIOC.chanlist[k] = ChanList[k];
    chanlistIOC.scalelist[k] = sigs[k].scale();
  }
  chanlistIOC.chanlistN = sigs.size();
  retVal = ::ioctl( Modulefile, IOC_CHANLIST, &chanlistIOC );
  cerr << "prepareWrite(): IOC_CHANLIST done!" << endl; /// TEST
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::prepareWrite -> ioctl command IOC_CHANLIST on device "
	 << Modulename << " failed!" << endl;
    return errorFlag;
  }

  // set up synchronous command:
  syncCmdIOC.subdevID = SubdeviceID;
  syncCmdIOC.frequency = (unsigned int)sigs[0].sampleRate();
  syncCmdIOC.duration = sigs[0].size() + sigs[0].indices( sigs[0].delay());
  syncCmdIOC.continuous = sigs[0].continuous();
  retVal = ::ioctl( Modulefile, IOC_SYNC_CMD, &syncCmdIOC );
  cerr << "prepareWrite(): IOC_SYNC_CMD done!" << endl; /// TEST
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::prepareWrite -> ioctl command IOC_SYNC_CMD on device "
	 << Modulename << " failed!" << endl;
    if ( errno == EINVAL )
      sigs.addError( DaqError::InvalidSampleRate );
    else
      sigs.addErrorStr( errno );
    return errorFlag;
  }

  // initialize Connection to RTAI-FIFO:
  char fifoName[] = "/dev/rtf1";
  FifoFd = ::open( fifoName, O_WRONLY | O_NONBLOCK );
  if( FifoFd < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> oping RTAI-FIFO " 
         << fifoName << " failed!" << endl;
    return errorFlag;
  }

  IsLoaded = true;
  IsPrepared = true;

  cerr << " DynClampAnalogOutput::prepareWrite(): success" << endl;/////TEST/////

  return 0; //elemWritten;
}


int DynClampAnalogOutput::startWrite( OutList &sigs )
{
  cerr << " DynClampAnalogOutput::startWrite(): 1" << endl;/////TEST/////

  if( !prepared() ) {
    sigs.addError( DaqError::Unknown );
    return -1;
  }

  ErrorState = 0;

  // fill buffer with initial data:
  int retVal = writeData( sigs );

  if(retVal < 1 ) {
    return -2;
  }

  // start subdevice:
  retVal = ::ioctl( Modulefile, IOC_START_SUBDEV, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> ioctl command IOC_START_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    return -2;
  }
  
  return 0;
}


int DynClampAnalogOutput::writeData( OutList &sigs )
{
  cerr << " DynClampAnalogOutput::writeData(): in" << endl;/////TEST/////

  //device stopped?
    /* // AO does not have to run in order to push data
  if( !running() ) {
    sigs.addErrorStr( "DynClampAnalogOutput::writeData: " +
		      deviceFile() + " is not running!" );
    cerr << "DynClampAnalogOutput::writeData: device is not running!"  << endl;/////TEST/////
    return 0;/////TEST/////
  }
    */

  ErrorState = 0;
  cerr << "DynClampAnalogOutput::writeData: size of device buffer: " 
       << sigs[0].deviceBufferSize() << " - size of outdata: " << sigs[0].size() 
       << " - continuous: " << sigs[0].continuous() << endl;
  bool failed = false;
  int elemWritten = 0;
  int bytesWritten;

  if( sigs[0].deviceBufferMaxPop() <= 0 ) {
    //    sigs.addErrorStr( "DynClampAnalogOutput::writeData: " +
    //		      deviceFile() + " - buffer-underrun in outlist!" );
    //    sigs.addError( DaqError::OverflowUnderrun );
    cerr << "DynClampAnalogOutput::writeData: buffer-underrun in outlist!"  
	 << endl;/////TEST/////
    //    sigs[0].deviceBufferReset();/////TEST////
    return 0;/////TEST////
  }
  // try to write twice
  for ( int tryit = 0;
	tryit < 2 && !failed && sigs[0].deviceBufferMaxPop() > 0; 
	tryit++ ){
/*    
    int retVal = ::ioctl( Modulefile, IOC_REQ_WRITE, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogOutput::writeData() -> ioctl command IOC_REQ_WRITE on device "
	   << Modulename << " failed!" << endl;
      return -2;
    }
*/
    bytesWritten = ::write( FifoFd, sigs[0].deviceBufferPopBuffer(),
			                      sigs[0].deviceBufferMaxPop() * BufferElemSize );
    cerr << " DynClampAnalogOutput::writeData(): " << bytesWritten << " of "
         << sigs[0].deviceBufferMaxPop() * BufferElemSize << "bytes written:"
         << endl;/////TEST/////

    if( bytesWritten < 0 && errno != EAGAIN && errno != EINTR ) {
      sigs.addErrorStr( errno );
      failed = true;
      cerr << " DynClampAnalogOutput::writeData(): error" << endl;/////TEST/////
    }
    else if ( bytesWritten > 0 ) {
      sigs[0].deviceBufferPop( bytesWritten / BufferElemSize );
      elemWritten += bytesWritten / BufferElemSize;
    }

  }

  if( failed || errno == EINTR )
    switch( errno ) {

    case EPIPE: 
      ErrorState = 1;
      sigs.addErrorStr( deviceFile() + " - buffer-underrun: "
			+ strerror( errno ) );
      sigs.addError( DaqError::OverflowUnderrun );
      cerr << " DynClampAnalogOutput::writeData(): buffer-underrun: "
      	   << strerror( errno ) << endl;/////TEST/////
      return -1;

    case EBUSY:
      ErrorState = 2;
      sigs.addErrorStr( deviceFile() + " - device busy: "
			+ strerror( errno ) );
      sigs.addError( DaqError::Busy );
      cerr << " DynClampAnalogOutput::writeData(): device busy: "
	         << strerror( errno ) << endl;/////TEST/////
      return -1;

    default:
      ErrorState = 2;
      sigs.addErrorStr( "Error while writing to device-file: " + deviceFile()
			+ "  system: " + strerror( errno ) );
      cerr << " DynClampAnalogOutput::writeData(): buffer-underrun: "
	   << "  system: " << strerror( errno )
	
	   << endl;/////TEST/////
      sigs.addError( DaqError::Unknown );
      return -1;
    }
  
  cerr << " DynClampAnalogOutput::writeData(): out" << endl;/////TEST/////
  
  return elemWritten;
}


int DynClampAnalogOutput::reset( void )
{ 
  clearSettings();
  ErrorState = 0;

  if( !IsLoaded )
    return 0;

  int running = SubdeviceID;
  int retVal = ::ioctl( Modulefile, IOC_CHK_RUNNING, &running );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << Modulename << " failed!" << endl;
    return -1;
  }

  if ( running > 0 ) {
    retVal = ::ioctl( Modulefile, IOC_STOP_SUBDEV, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogOutput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	   << Modulename << " failed!" << endl;
      return -1;
    }
    rtf_reset( FifoFd );
  }

  IsPrepared = false;
  IsLoaded = false;
  IsRunning = false;

  return 0;
}


bool DynClampAnalogOutput::running( void ) const
{
  if( !IsLoaded )
    return false;

  int exchangeVal = SubdeviceID;
  int retVal = ::ioctl( Modulefile, IOC_CHK_RUNNING, &exchangeVal );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << Modulename << " failed!" << endl;
    return false;
  }
  cerr << "DynClampAnalogOutput::running returned " << exchangeVal << endl;

  return exchangeVal;
}


int DynClampAnalogOutput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */

}


long DynClampAnalogOutput::index( void ) const
{
  long index = 0;
  int retVal = ::ioctl( Modulefile, IOC_GETAOINDEX, &index );
    cerr << " DynClampAnalogOutput::index() -> " << index << endl;
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::index() -> ioctl command IOC_GETLOOPCNT on device "
	   << Modulename << " failed!" << endl;
    return -2;
  }
  return index;
}


int DynClampAnalogOutput::getAISyncDevice( const vector< AnalogInput* > &ais ) const
{
  for ( unsigned int k=0; k<ais.size(); k++ ) {
    if ( ais[k]->analogInputType() == DynClampAnalogIOType )
      return k;
  }
  return -1;
}


void DynClampAnalogOutput::addTraces( vector< TraceSpec > &traces, int deviceid ) const
{
  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = PARAM_OUT;
  int channel = PARAM_CHAN_OFFSET;
  while ( 0 == ::ioctl( Modulefile, IOC_GET_TRACE_INFO, &traceInfo ) ) {
    traces.push_back( TraceSpec( traces.size(), traceInfo.name,
				 deviceid, channel++, 1.0, traceInfo.unit ) );
  }
  int ern = errno;
  if ( ern != ERANGE )
    cerr << "DynClampAnalogOutput::addTraces() -> errno " << strerror( errno ) <<  endl;
}


int DynClampAnalogOutput::matchTraces( vector< TraceSpec > &traces ) const
{
  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = TRACE_OUT;
  struct traceChannelIOCT traceChannel;
  traceChannel.traceType = TRACE_OUT;
  string unknowntraces = "";
  int foundtraces = 0;
  while ( 0 == ::ioctl( Modulefile, IOC_GET_TRACE_INFO, &traceInfo ) ) {
    bool notfound = true;
    for ( unsigned int k=0; k<traces.size(); k++ ) {
      if ( traces[k].traceName() == traceInfo.name ) {
	traceChannel.device = traces[k].device();
	traceChannel.channel = traces[k].channel();
	if ( ::ioctl( Modulefile, IOC_SET_TRACE_CHANNEL, &traceChannel ) != 0 ) {
	  cerr << "DynClampAnalogOutput::matchTraces() set channels -> errno " << errno << endl;
	  return -1;
	}
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
  if ( ern != ERANGE ) {
    cerr << "DynClampAnalogOutput::matchTraces() get traces -> errno " << ern << endl;
    return -1;
  }
  if ( ! unknowntraces.empty() ) {
    //    traces.addErrorStr( "unable to match model input traces" + unknowntraces );
    return -1;
  }
  return foundtraces;
}


int DynClampAnalogOutput::subdevice( void ) const
{
  return Subdevice;
}


bool DynClampAnalogOutput::loaded( void ) const 
{
  return IsLoaded;
}


bool DynClampAnalogOutput::prepared( void ) const 
{ 
  return IsPrepared;
}


void DynClampAnalogOutput::setRunning( void )
{
  IsRunning = true;
}


}; /* namespace comedi */
