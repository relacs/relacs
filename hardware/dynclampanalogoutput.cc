/*
  dynclampanalogoutput.cc
  Interface for accessing analog output of a daq-board via the dynamic clamp 
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
#include <relacs/dynclampanalogoutput.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

DynClampAnalogOutput::DynClampAnalogOutput( void ) 
  : AnalogOutput( DynClampAnalogOutputType )
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
  CAO = new ComediAnalogOutput;
}


DynClampAnalogOutput::DynClampAnalogOutput( const string &deviceclass ) 
  : AnalogOutput( deviceclass, DynClampAnalogOutputType )
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
  MaxRate = 50000.0;
  ComediBufferSize = 0;
  CAO = new ComediAnalogOutput;
}


DynClampAnalogOutput::~DynClampAnalogOutput( void ) 
{
  ::close( Modulefile ); // TODO: How to access unistd.h: close() ???
  close();
  delete CAO;
}

int DynClampAnalogOutput::open( const string &devicefile, long mode )
{ 
  if ( devicefile.empty() )
    return InvalidDevice;
  setDeviceFile( devicefile );

  int retVal = CAO->open( devicefile );
  
  // copy information not available after CAO->close()
  Subdevice = CAO->subdevice();
  Channels = CAO->channels();
  Bits =  CAO->bits();
  MaxRate = CAO->maxRate();  // XXX This is not the max freq of the real time loop!
  MaxRate = 50000.0;
  ComediBufferSize = CAO->bufferSize();
 
//  CAO->close();

  if ( retVal != 0 )
    return retVal;

  // open kernel module:
  Modulename = "/dev/dynclamp";
  Modulefile = ::open( Modulename.c_str(), O_WRONLY ); //O_RDONLY
  if( Modulefile == -1 ) {
  cerr << " DynClampAnalogOutput::open(): opening dynclamp-module failed" 
       << endl;/////TEST/////
    return ErrorState;
  }

  return 0;
}


int DynClampAnalogOutput::setModuleName( string modulename )
{
  Modulename = modulename;
  // TODO: test opening here?
  return 0;
}


void DynClampAnalogOutput::close( void )
{ 
  if( CAO->isOpen() ) {
    CAO->close();
    return;
  }
  reset();

}

int DynClampAnalogOutput::reset( void ) 
{ 
  clearSettings();
  ErrorState = 0;
  IsPrepared = false;
  IsLoaded = false;
  IsRunning = false;
  IsKernelDaqOpened = false;

  if( CAO->isOpen() )
    return CAO->reset();

  int retVal;

  if( IsPrepared || IsLoaded ) {
    retVal = ::ioctl( Modulefile, IOC_STOP_SUBDEV, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogOutput::stop -> ioctl command IOC_STOP_SUBDEV on device "
  	  << Modulename << " failed!" << endl;
      ErrorState = -2;
      return -1;
    }
 
    if( retVal )
      return retVal;  

    retVal = ::ioctl( Modulefile, IOC_RELEASE_SUBDEV, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogOutput::close -> ioctl command IOC_RELEASE_SUBDEV on device "
  	 << Modulename << " failed!" << endl;
      ErrorState = -2;
      return -1;
    }
  }

  return retVal;
  
}


int DynClampAnalogOutput::stop( void )
{ 
  if( CAO->isOpen() )
    return CAO->stop();

  if( !IsLoaded )
    return 0;

  int exchangeVal = SubdeviceID;
  int retVal = ::ioctl( Modulefile, IOC_CHK_RUNNING, &exchangeVal );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << Modulename << " failed!" << endl;
  }

  retVal = ::ioctl( Modulefile, IOC_STOP_SUBDEV, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    ErrorState = -2;
    return -1;
  }

  IsLoaded = false;
  IsRunning = false;
  return 0;
}



bool DynClampAnalogOutput::isOpen( void ) const 
{ 
  if( IsLoaded )
      return true;
  return CAO->isOpen();
}

bool DynClampAnalogOutput::prepared( void ) const 
{ 
  return IsPrepared;
}

bool DynClampAnalogOutput::loaded( void ) const 
{
  return IsLoaded;
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

  return exchangeVal;
}

void DynClampAnalogOutput::setRunning( void )
{
  IsRunning = true;
}

string DynClampAnalogOutput::moduleName( void ) const
{
  return Modulename;
}

int DynClampAnalogOutput::subdevice( void ) const
{
  return Subdevice;
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

int DynClampAnalogOutput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */

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


template < typename T >
int DynClampAnalogOutput::convert( OutList &sigs )
{
  /*
XXXX Add multiplexing floats into deviceBuffer here!
  XXX First test: just a single channel!
  */
  sigs[0].deviceBufferReset(); // Zum Test (wegen fehlender Initialisierung von DeviceBufferIndex)
  sigs[0].setDeviceBuffer( (char *)sigs[0].data(), sigs[0].size(), sizeof( T ) );
  BufferElemSize = sizeof(T);

  /*
keep this for future use!
  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // set scaling factors:
  unsigned int iDelay = sigs[0].indices( sigs[0].delay() );
  double scale[ ol.size() ];
  for ( int k=0; k<ol.size(); k++ )
    scale[k] = ol[k].scale() * ol[k].gain();

  // allocate buffer:
  int nbuffer = ol.size() * ( sigs[0].size() + iDelay );
  T *buffer = new T [nbuffer];

  // convert data and multiplex into buffer:
  T *bp = buffer;
  for ( int i=-iDelay; i<ol[0].size(); i++ ) {
    for ( int k=0; k<ol.size(); k++ ) {
      int v;
      if( i < 0 ) // simulate delay
	v = (T) ::rint( ( 0.0 + ol[k].offset() ) * scale[k] );
      else
	v = (T) ::rint( ( ol[k][i] + ol[k].offset() ) * scale[k] );
      if ( v > ol[k].maxData() )
	v = ol[k].maxData();
      else if ( v < ol[k].minData() ) 
	v = ol[k].minData();
      *bp = v;
      ++bp;
    }
  }

  sigs[0].setDeviceBuffer( (char *)buffer, nbuffer, sizeof( T ) );
  */
  return 0;
}

int DynClampAnalogOutput::convertData( OutList &sigs )
{
  /*
  if( CAO->LongSampleType )
    return convert<lsampl_t>( sigs );// default for NI E-series, since the mio-driver does not return a valid frequency)
  
  return convert<sampl_t>( sigs );
  */
  return convert<float>( sigs );
}

int DynClampAnalogOutput::testWriteDevice( OutList &sigs )
{

  ErrorState = 0;
  
  if( IsKernelDaqOpened )
    return DaqError::Busy;

  // XXX open and close is only neede
  if( !CAO->isOpen() ) {
    int retVal = CAO->open( deviceFile() );
    if( retVal )
      return DaqError::DeviceNotOpen;
  }

  // ...USER-SPACE DEVICE TESTING STARTING...
  
  cerr << " DynClampAnalogOutput::testWrite(): 1" << endl;/////TEST/////


  memset( ChanList, 0, sizeof( ChanList ) );
  // find ranges for synchronous acquisition:
  int aref = AREF_GROUND;
  int maxrange = 1 << bits();
  for ( int k=0; k<sigs.size(); k++ ) {
    // minimum and maximum values:
    double min = sigs[k].requestedMin();
    double max = sigs[k].requestedMax();
    if ( min == OutData::AutoRange || max == OutData::AutoRange ) {
      double smin = 0.0;
      double smax = 0.0;
      numerics::minMax( smin, smax, sigs[k] );
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
    double maxvolt = sigs[k].getVoltage( max );
    int index = unipolar ? CAO->UnipolarRangeIndex.size() - 1 
                         :  CAO->BipolarRangeIndex.size() - 1;
    for( ; index >= 0; index-- ) {
      if( unipolar && unipolarRange( index ) > maxvolt ) {
      	maxboardvolt = unipolarRange( index );
      	break;
      }
      if( !unipolar && bipolarRange( index ) > maxvolt ){
      	maxboardvolt = bipolarRange( index );
      	break;
      }
    }
    if( index < 0 ) {
      sigs[k].addError( DaqError::InvalidGain );
      cerr << " DynClampAnalogOutput::testWrite(): ERROR - InvalidGain" << endl;/////TEST/////
    }
        
    if ( sigs[k].noIntensity() ) {
      if ( ! extref ) {
      	if ( externalReference() < maxboardvolt ) {
      	  if ( maxvolt < externalReference() )
      	    extref = true;
      	}
      	else
      	  if ( maxboardvolt == -1.0 )
      	    extref = true;
      }
      if ( extref ) {
      	if ( externalReference() < 0.0 ) {
      	  sigs[k].addError( DaqError::InvalidReference );
          cerr << " DynClampAnalogOutput::testWrite(): ERROR - InvalidRef 1" << endl;/////TEST/////
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
      sigs[k].setGain( unipolar ? maxrange/maxboardvolt : maxrange/2/maxboardvolt );
    }
    else {
      if ( extref && externalReference() < 0.0 ) {
      	sigs[k].addError( DaqError::InvalidReference );
        cerr << " DynClampAnalogOutput::testWrite(): ERROR - InvalidRef 2" << endl;/////TEST/////
      	extref = false;
      }
      sigs[k].setGain( unipolar ? maxrange : maxrange/2 );
    }

    int gainIndex = index;
    if ( unipolar )
      gainIndex |= 1<<14;
    if ( extref )
      gainIndex |= 1<<15;
    

    sigs[k].setGainIndex( gainIndex );
    sigs[k].setMinData( unipolar ? 0 : -maxrange/2 );
    sigs[k].setMaxData( unipolar ? maxrange - 1 : maxrange/2 - 1 );

    // set up channel in chanlist:
    if( !sigs.failed() )
      if( unipolar )
      	ChanList[k] = CR_PACK( sigs[k].channel(), CAO->UnipolarRangeIndex[ index ], aref );
      else
      	ChanList[k] = CR_PACK( sigs[k].channel(), CAO->BipolarRangeIndex[ index ], aref );

  }


  if( sigs.failed() )
    return -1;

  cerr << " DynClampAnalogOutput::testWrite(): success" << endl;/////TEST/////


  return 0;
}


int DynClampAnalogOutput::prepareWrite( OutList &sigs )
{
  cerr << " DynClampAnalogOutput::prepareWrite(): 1" << endl;/////TEST/////

  reset();

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  int error = testWriteDevice( ol );
  CAO->close();
  if( error )
    return error;

  cerr << " DynClampAnalogOutput::prepareWrite(): 2" << endl;/////TEST/////
  if ( ol.success() )
    setSettings( ol );
  else 
    return -1;

  cerr << " DynClampAnalogOutput::prepareWrite(): 3" << endl;////TEST/////

  Sigs = &sigs;

  struct deviceIOCT deviceIOC;
  struct chanlistIOCT chanlistIOC;
  struct syncCmdIOCT syncCmdIOC;
  int retVal;
  ErrorState = -2;

  // get subdevice ID from module:
  retVal = ::ioctl( Modulefile, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> ioctl command IOC_GET_SUBDEV_ID on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
  }

  // set device and subdevice:
  deviceIOC.subdevID = SubdeviceID;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = Subdevice;
  deviceIOC.isOutput = 1; // TODO: change for input!!!
  retVal = ::ioctl( Modulefile, IOC_OPEN_SUBDEV, &deviceIOC );
  cerr << "prepareWrite(): IOC_OPEN_SUBDEV request for address done!" /// TEST
       << &deviceIOC << endl;
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> ioctl command IOC_OPEN_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
  }

  // set chanlist:
  chanlistIOC.subdevID = SubdeviceID;
  for( int k = 0; k < sigs.size(); k++ )
    chanlistIOC.chanlist[k] = ChanList[k];
  chanlistIOC.chanlistN = sigs.size();
  retVal = ::ioctl( Modulefile, IOC_CHANLIST, &chanlistIOC );
  cerr << "prepareWrite(): IOC_CHANLIST done!" << endl; /// TEST
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> ioctl command IOC_CHANLIST on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
  }

  // set up synchronous command:
  syncCmdIOC.subdevID = SubdeviceID;
  syncCmdIOC.frequency = (unsigned int)sigs[0].sampleRate();
  syncCmdIOC.duration = sigs[0].size() + sigs[0].indices( sigs[0].delay());
  syncCmdIOC.continuous = sigs[0].continuous();
  retVal = ::ioctl( Modulefile, IOC_SYNC_CMD, &syncCmdIOC );
  cerr << "prepareWrite(): IOC_SYNC_CMD done!" << endl; /// TEST
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> ioctl command IOC_SYNC_CMD on device "
	 << Modulename << " failed!" << endl;
    return ErrorState;
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
    ErrorState = 2;
    return ErrorState;
  }

  // start subdevice:
  retVal = ::ioctl( Modulefile, IOC_START_SUBDEV, &SubdeviceID );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> ioctl command IOC_START_SUBDEV on device "
	 << Modulename << " failed!" << endl;
    ErrorState = 2;
    return ErrorState;
  }
  
  return ErrorState; //elemWritten;
}

 
int DynClampAnalogOutput::fillWriteBuffer( void )
{
  /*
  ErrorState = 0;
  cerr << "DynClampAnalogOutput::writeData: size of device buffer: " 
       << (*Sigs)[0].deviceBufferSize() << " - size of outdata: " 
       << " - continuous: " << (*Sigs)[0].continuous() << endl;
  bool failed = false;
  int elemWritten = 0;
  int bytesWritten;

  if( (*Sigs)[0].deviceBufferMaxPop() <= 0 ) {
    //    ErrorState = 1;
    //    (*Sigs).addErrorStr( "DynClampAnalogOutput::writeData: " +
    //		      deviceFile() + " - buffer-underrun in outlist!" );
    //    (*Sigs).addError( DaqError::OverflowUnderrun );
    cerr << "DynClampAnalogOutput::writeData: buffer-underrun in outlist!"  
	 << endl;/////TEST/////
    //    (*Sigs)[0].deviceBufferReset();/////TEST////
    return 0;/////TEST////
  }
  // try to write twice
  for ( int tryit = 0;
	tryit < 2 && !failed && (*Sigs)[0].deviceBufferMaxPop() > 0; 
	tryit++ ){
    
    int retVal = ::ioctl( Modulefile, IOC_REQ_WRITE, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogOutput::writeData() -> ioctl command IOC_REQ_WRITE on device "
	   << Modulename << " failed!" << endl;
      ErrorState = -2;
      return ErrorState;
    }
    bytesWritten = write( Modulefile, (*Sigs)[0].deviceBufferPopBuffer(),
			  (*Sigs)[0].deviceBufferMaxPop() * BufferElemSize );
    cerr << " DynClampAnalogOutput::writeData():  bytes written:" << bytesWritten << endl;/////TEST/////

    if( bytesWritten < 0 && errno != EAGAIN && errno != EINTR ) {
      (*Sigs).addErrorStr( errno );
      failed = true;
      cerr << " DynClampAnalogOutput::writeData(): error" << endl;/////TEST/////
    }
    else if ( bytesWritten > 0 ) {
      (*Sigs)[0].deviceBufferPop( bytesWritten / BufferElemSize );
      elemWritten += bytesWritten / BufferElemSize;
    }

  }

  if( failed || errno == EAGAIN || errno == EINTR )
    switch( errno ) {

    case EPIPE: 
      ErrorState = 1;
      (*Sigs).addErrorStr( deviceFile() + " - buffer-underrun: "
			+ comedi_strerror( comedi_errno() ) );
      (*Sigs).addError( DaqError::OverflowUnderrun );
      cerr << " DynClampAnalogOutput::writeData(): buffer-underrun: "
	   << comedi_strerror( comedi_errno() ) << endl;/////TEST/////
      return -1;

    case EBUSY:
      ErrorState = 2;
      (*Sigs).addErrorStr( deviceFile() + " - device busy: "
			+ comedi_strerror( comedi_errno() ) );
      (*Sigs).addError( DaqError::Busy );
      cerr << " DynClampAnalogOutput::writeData(): device busy: "
	   << comedi_strerror( comedi_errno() ) << endl;/////TEST/////
      return -1;

    default:
      ErrorState = 2;
      (*Sigs).addErrorStr( "Error while writing to device-file: " + deviceFile()
			+ "  comedi: " + comedi_strerror( comedi_errno() )
			+ "  system: " + strerror( errno ) );
      cerr << " DynClampAnalogOutput::writeData(): buffer-underrun: "
	   << "  comedi: " << comedi_strerror( comedi_errno() ) 
	   << "  system: " << strerror( errno )
	
	   << endl;/////TEST/////
      (*Sigs).addError( DaqError::Unknown );
      return -1;
    }
  
  cerr << " DynClampAnalogOutput::writeData(): out" << endl;/////TEST/////
  
  return elemWritten;
  */
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
       << sigs[0].deviceBufferSize() << " - size of outdata: " 
       << " - continuous: " << sigs[0].continuous() << endl;
  bool failed = false;
  int elemWritten = 0;
  int bytesWritten, retVal;

  if( sigs[0].deviceBufferMaxPop() <= 0 ) {
    //    ErrorState = 1;
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
    
    retVal = ::ioctl( Modulefile, IOC_REQ_WRITE, &SubdeviceID );
    if( retVal < 0 ) {
      cerr << " DynClampAnalogOutput::writeData() -> ioctl command IOC_REQ_WRITE on device "
	   << Modulename << " failed!" << endl;
      ErrorState = -2;
      return ErrorState;
    }
    bytesWritten = ::write( Modulefile, sigs[0].deviceBufferPopBuffer(),
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

  if( failed || errno == EAGAIN || errno == EINTR )
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

long DynClampAnalogOutput::index( void )
{
  long index = 0;
  int retVal = ::ioctl( Modulefile, IOC_GETLOOPCNT, &index );
  if( retVal < 0 ) {
    cerr << " DynClampAnalogOutput::index() -> ioctl command IOC_GETLOOPCNT on device "
	   << Modulename << " failed!" << endl;
    ErrorState = -2;
    return ErrorState;
  }
  return index;

}

int DynClampAnalogOutput::getAISyncDevice( const vector< AnalogInput* > &ais ) const
{
  for ( unsigned int k=0; k<ais.size(); k++ ) {
    if ( ais[k]->analogInputType() == DynClampAnalogOutputType )
      return k;
  }
  return -1;
}
