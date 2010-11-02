/*
  coemdi/dynclampanalogoutput.cc
  Interface for accessing analog output of a daq-board via the dynamic clamp 
  kernel module.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/analoginput.h>
#include <relacs/comedi/comedianalogoutput.h>
#include <relacs/comedi/dynclampanalogoutput.h>
#include <rtai_fifos.h>
using namespace std;
using namespace relacs;

namespace comedi {


DynClampAnalogOutput::DynClampAnalogOutput( void ) 
  : AnalogOutput( "DynClampAnalogOutput", DynClampAnalogIOType )
{
  CAO = new ComediAnalogOutput;
  CAOSubDevFlags = 0;
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
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;
}


DynClampAnalogOutput::DynClampAnalogOutput( const string &device,
					    const Options &opts ) 
  : AnalogOutput( "DynClampAnalogOutput", DynClampAnalogIOType )
{
  CAO = new ComediAnalogOutput;
  CAOSubDevFlags = 0;
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
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;

  open( device, opts );
}


DynClampAnalogOutput::~DynClampAnalogOutput( void ) 
{
  close();
  delete CAO;
}


int DynClampAnalogOutput::open( const string &device, const Options &opts )
{ 
  Info.clear();
  Settings.clear();

  if ( device.empty() )
    return InvalidDevice;
  setDeviceFile( device );

  // open user space coemdi:
  int retval = CAO->open( device, opts );
  if ( retval != 0 )
    return retval;
  
  // copy information not available after CAO->close()
  SubDevice = CAO->comediSubdevice();
  Channels = CAO->channels();
  Bits =  CAO->bits();
  MaxRate = CAO->maxRate();  // XXX This is not the max freq of the real time loop!
  MaxRate = 50000.0;

  CAOSubDevFlags = comedi_get_subdevice_flags( CAO->DeviceP, SubDevice );

  // set basic device infos:
  setDeviceName( CAO->deviceName() );
  setDeviceVendor( CAO->deviceVendor() );
  setDeviceFile( device );

  // get calibration:
  comedi_calibration_t *calibration;
  {
    char *calibpath = comedi_get_default_calibration_path( CAO->DeviceP );
    ifstream cf( calibpath );
    if ( cf.good() )
      calibration = comedi_parse_calibration_file( calibpath );
    else
      calibration = 0;
    free( calibpath );
  }

  // get conversion polynomials:
  bool softcal = ( ( CAOSubDevFlags & SDF_SOFT_CALIBRATED ) > 0 );
  UnipConverter = new comedi_polynomial_t* [Channels];
  BipConverter = new comedi_polynomial_t* [Channels];
  for ( int c=0; c<channels(); c++ ) {
    UnipConverter[c] = new comedi_polynomial_t[CAO->UnipolarRangeIndex.size()];
    for ( unsigned int r=0; r<CAO->UnipolarRangeIndex.size(); r++ ) {
      if ( softcal && calibration != 0 ) {
	comedi_get_softcal_converter( SubDevice, c, CAO->UnipolarRangeIndex[r],
				      COMEDI_FROM_PHYSICAL, calibration,
				      &UnipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( CAO->DeviceP, SubDevice, c,
				      CAO->UnipolarRangeIndex[r],
				      COMEDI_FROM_PHYSICAL,
				      &UnipConverter[c][r] );
      }
    }
    BipConverter[c] = new comedi_polynomial_t[CAO->BipolarRangeIndex.size()];
    for ( unsigned int r=0; r<CAO->BipolarRangeIndex.size(); r++ ) {
      if ( softcal && calibration != 0 ) {
	comedi_get_softcal_converter( SubDevice, c, CAO->BipolarRangeIndex[r],
				      COMEDI_FROM_PHYSICAL, calibration,
				      &BipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( CAO->DeviceP, SubDevice, c,
				      CAO->BipolarRangeIndex[r],
				      COMEDI_FROM_PHYSICAL, 
				      &BipConverter[c][r] );
      }
    }
  }
  
  // cleanup calibration:
  if ( calibration != 0 )
    comedi_cleanup_calibration( calibration );
 
  // close user space comedi:
  CAO->close();

  // open kernel module:
  ModuleDevice = "/dev/dynclamp";
  ModuleFd = ::open( ModuleDevice.c_str(), O_WRONLY ); //O_RDONLY
  if( ModuleFd == -1 ) {
    cerr << " DynClampAnalogOutput::open(): opening dynclamp-module failed\n";
    return -1;
  }
  cerr << " DynClampAnalogOutput::open() success\n" ;

  // get subdevice ID from module:
  retval = ::ioctl( ModuleFd, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::open -> ioctl command IOC_GET_SUBDEV_ID on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // set device and subdevice:
  struct deviceIOCT deviceIOC;
  deviceIOC.subdevID = SubdeviceID;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = SubDevice;
  deviceIOC.subdevType = SUBDEV_OUT;
  retval = ::ioctl( ModuleFd, IOC_OPEN_SUBDEV, &deviceIOC );
  cerr << "open(): IOC_OPEN_SUBDEV request for address done!" /// TEST
       << &deviceIOC << '\n';
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::open -> ioctl command IOC_OPEN_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // initialize Connection to RTAI-FIFO:
  char fifoName[] = "/dev/rtfxxx";
  sprintf( fifoName, "/dev/rtf%u", deviceIOC.fifoIndex );
  FifoFd = ::open( fifoName, O_WRONLY | O_NONBLOCK );
  if( FifoFd < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> oping RTAI-FIFO " 
         << fifoName << " failed!\n";
    return -1;
  }
  FIFOSize = deviceIOC.fifoSize;

  IsPrepared = false;

  setInfo();

  return 0;
}


bool DynClampAnalogOutput::isOpen( void ) const 
{ 
  return ( ModuleFd >= 0 );
}


void DynClampAnalogOutput::close( void )
{ 
  if ( ! isOpen() )
    return;

  reset();

  ::ioctl( ModuleFd, IOC_REQ_CLOSE, &SubdeviceID );
  ::close( FifoFd );
  if( ::close( ModuleFd ) < 0 )
    cerr << "Close of module file failed!\n";

  ModuleFd = -1;

  Info.clear();
}


int DynClampAnalogOutput::setModuleName( string modulename )
{
  ModuleDevice = modulename;
  // TODO: test opening here?
  return 0;
}


string DynClampAnalogOutput::moduleName( void ) const
{
  return ModuleDevice;
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


int DynClampAnalogOutput::setupChanList( OutList &sigs,
					 unsigned int *chanlist,
					 int maxchanlist )
{
  memset( chanlist, 0, maxchanlist * sizeof( unsigned int ) );

  for ( int k=0; k<sigs.size() && k < maxchanlist; k++ ) {

    // parameter signals don't have references and gains:
    if ( sigs[k].channel() >= PARAM_CHAN_OFFSET ) {
      chanlist[k] = CR_PACK( sigs[k].channel(), 0, 0 );
      continue;
    }

    // minimum and maximum values:
    double min = sigs[k].requestedMin();
    double max = sigs[k].requestedMax();
    if ( min == OutData::AutoRange || max == OutData::AutoRange ) {
      float smin = 0.0;
      float smax = 0.0;
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
    bool extref = false;
    bool minislarger = false;
    if ( max == OutData::ExtRef )
      extref = true;
    else {
      // maximum value:
      if ( ::fabs( min ) > max ) {
	max = ::fabs( min );
	minislarger = true;
      }
    }

    // allocate gain factor:
    char *gaindata = sigs[k].gainData();
    if ( gaindata != NULL )
      delete [] gaindata;
    gaindata = new char[sizeof(comedi_polynomial_t)];
    sigs[k].setGainData( gaindata );
    comedi_polynomial_t *gainp = (comedi_polynomial_t *)gaindata;

    // set range:
    double maxvolt = sigs[k].getVoltage( max );
    int index = -1;
    if ( sigs[k].noIntensity() ) {
      for ( int p=0; p<2 && index < 0; p++ ) {
	if ( unipolar ) {
	  for( index = CAO->UnipolarRange.size() - 1; index >= 0; index-- ) {
	    if ( unipolarRange( index ) > maxvolt )
	      break;
	  }
	}
	else {
	  for( index = CAO->BipolarRange.size() - 1; index >= 0; index-- ) {
	    if ( bipolarRange( index ) > maxvolt )
	      break;
	  }
	}
	// try other polarity?
	if ( index < 0 && p == 0 )
	  unipolar = ! unipolar;
      }
      if ( index < 0 ) {
	if ( minislarger )
	  sigs[k].addError( DaqError::Underflow );
	else
	  sigs[k].addError( DaqError::Overflow );
      }
    }
    else {
      index = 0;
      if ( unipolar && index >= (int)CAO->UnipolarRange.size() )
	unipolar = false;
      if ( ! unipolar && index >= (int)CAO->BipolarRange.size() )
	unipolar = true;
      if ( index >= unipolar ? (int)CAO->UnipolarRange.size() : (int)CAO->BipolarRange.size() )
	index = -1;
      if ( max > 1.0+1.0e-8 )
	sigs[k].addError( DaqError::Overflow );
      else if ( min < -1.0-1.0e-8 )
	sigs[k].addError( DaqError::Underflow );
    }

    // none of the available ranges contains the requested range:
    if ( index < 0 ) {
      sigs[k].addError( DaqError::InvalidGain );
      break;
    }

    double maxboardvolt = unipolar ? CAO->UnipolarRange[index].max : CAO->BipolarRange[index].max;
    double minboardvolt = unipolar ? CAO->UnipolarRange[index].min : CAO->BipolarRange[index].min;

    // external reference:
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
	  extref = false;
	}
	else {
	  if ( externalReference() == 0.0 )
	    maxboardvolt = 1.0;
	  else
	    maxboardvolt = externalReference();
	  minboardvolt = unipolar ? 0.0 : -maxboardvolt;
	  index = unipolar ? CAO->UnipolarExtRefRangeIndex 
	    : CAO->BipolarExtRefRangeIndex;
	}
      }
    }
    else {
      if ( extref && externalReference() < 0.0 ) {
	sigs[k].addError( DaqError::InvalidReference );
	extref = false;
      }
      sigs[k].setScale( maxboardvolt );
    }

    int gainIndex = index;
    if ( unipolar )
      gainIndex |= 1<<14;
    if ( extref )
      gainIndex |= 1<<15;

    sigs[k].setGainIndex( gainIndex );
    sigs[k].setMinVoltage( minboardvolt );
    sigs[k].setMaxVoltage( maxboardvolt );

    // reference:
    int aref = AREF_GROUND;

    // set up channel in chanlist:
    int gi = unipolar ? CAO->UnipolarRangeIndex[ index ] : CAO->BipolarRangeIndex[ index ];
    if ( unipolar ) {
      memcpy( gainp, &UnipConverter[sigs[k].channel()][gi], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( sigs[k].channel(), gi, aref );
    }
    else {
      memcpy( gainp, &BipConverter[sigs[k].channel()][gi], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( sigs[k].channel(), gi, aref );
    }

    //    cerr << "DYNCLAMP ChanList channel " << sigs[k].channel() << " packed " << CR_CHAN( chanlist[k] ) << '\n';

  }

  return 0;
}


int DynClampAnalogOutput::directWrite( OutList &sigs )
{
  if ( sigs.size() <= 0 )
    return -1;

  if ( !isOpen() )
    return -1;

  // XXX make sure that all signal have size 1!

  reset();

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  unsigned int chanlist[MAXCHANLIST];
  setupChanList( ol, chanlist, MAXCHANLIST );

  // set chanlist:
  struct chanlistIOCT chanlistIOC;
  chanlistIOC.subdevID = SubdeviceID;
  for( int k = 0; k < ol.size(); k++ ){
    chanlistIOC.chanlist[k] = chanlist[k];
    if ( ol[k].channel() < PARAM_CHAN_OFFSET ) {
      const comedi_polynomial_t* poly = 
	(const comedi_polynomial_t *)ol[k].gainData();
      chanlistIOC.conversionlist[k].order = poly->order;
      if ( poly->order >= MAX_CONVERSION_COEFFICIENTS )
	cerr << "ERROR in DynClampAnalogInput::prepareWrite -> invalid order in converion polynomial!\n";
      chanlistIOC.conversionlist[k].expansion_origin = poly->expansion_origin;
      for ( int c=0; c<MAX_CONVERSION_COEFFICIENTS; c++ )
	chanlistIOC.conversionlist[k].coefficients[c] = poly->coefficients[c];
      chanlistIOC.scalelist[k] = ol[k].scale();
    }
  }
  chanlistIOC.userDeviceIndex = ol[0].device();
  chanlistIOC.chanlistN = ol.size();
  int retval = ::ioctl( ModuleFd, IOC_CHANLIST, &chanlistIOC );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::directWrite -> ioctl command IOC_CHANLIST on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // set up synchronous command:
  struct syncCmdIOCT syncCmdIOC;
  syncCmdIOC.subdevID = SubdeviceID;
  syncCmdIOC.frequency = 0;
  syncCmdIOC.delay = 0;
  syncCmdIOC.duration = 1;
  syncCmdIOC.continuous = 0;
  syncCmdIOC.startsource = 0;
  retval = ::ioctl( ModuleFd, IOC_SYNC_CMD, &syncCmdIOC );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::directWrite -> ioctl command IOC_SYNC_CMD on device "
	 << ModuleDevice << " failed!\n";
    if ( errno == EINVAL )
      ol.addError( DaqError::InvalidSampleRate );
    else
      ol.addErrorStr( errno );
    return -1;
  }

  // apply calibration:
  /*
  XXX this requires user space comedi!
  if ( Calibration != 0 ) {
    for( int k=0; k < ol.size(); k++ ) {
      unsigned int channel = CR_CHAN( Cmd.chanlist[k] );
      unsigned int range = CR_RANGE( Cmd.chanlist[k] );
      unsigned int aref = CR_AREF( Cmd.chanlist[k] );
      if ( comedi_apply_parsed_calibration( DeviceP, SubDevice, channel,
					    range, aref, Calibration ) < 0 )
	ol[k].addError( DaqError::CalibrationFailed );
    }
  }
  */

  if ( ol.failed() )
    return -1;

  BufferSize = ol.size() * BufferElemSize;
  Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

  // fill buffer with data:
  for ( int k=0; k<ol.size(); k++ )
    ol[k].deviceReset( 0 );
  Sigs = ol;
  retval = fillWriteBuffer();
  //  cerr << " DynClampAnalogOutput::directWrite -> fillWriteBuffer returned " << retval << '\n';

  delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  if ( retval < 0 )
    return -1;

  // start subdevice:
  retval = ::ioctl( ModuleFd, IOC_START_SUBDEV, &SubdeviceID );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::directWrite -> ioctl command IOC_START_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    int ern = errno;
    if ( ern == ENOMEM )
      cerr << " !!! No stack for kernel task !!!\n";
    sigs.addErrorStr( ern );
    return -1;
  }
  
  return 0;
}


int DynClampAnalogOutput::testWriteDevice( OutList &sigs )
{
  ErrorState = 0;
 
  if( ! isOpen() ) {
    sigs.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  // sampling rate must be the one of the running rt-loop:
  unsigned int rate = 0;
  int retval = ::ioctl( ModuleFd, IOC_GETRATE, &rate );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::testWriteDevice -> ioctl command IOC_GETRATE on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }
  unsigned int reqrate = (unsigned int)sigs[0].sampleRate();
  if ( reqrate == 0 ) {
    if ( rate > 0 )
      sigs.setSampleRate( (double)rate );
    else
      sigs.addError( DaqError::InvalidSampleRate );
  }
  else {
    if ( rate > 0) {
      int dr = ::abs( reqrate - rate );
      if ( dr > 0 && rate/dr < 200 )  /* less than 5 promille deviation */
	sigs.addError( DaqError::InvalidSampleRate );
      sigs.setSampleRate( (double)rate );
    }
  }

  // start source:
  if ( sigs[0].startSource() < 0 || sigs[0].startSource() >= 5 ) {
    sigs.setStartSource( 0 );
    sigs.addError( DaqError::InvalidStartSource );
  }

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // channel configuration:
  if ( ol[0].error() == DaqError::InvalidChannel ) {
    for ( int k=0; k<ol.size(); k++ ) {
      ol[k].delError( DaqError::InvalidChannel );
      // check channel number:
      if( ol[k].channel() < 0 ) {
	ol[k].addError( DaqError::InvalidChannel );
	ol[k].setChannel( 0 );
      }
      else if( ol[k].channel() >= channels() && ol[k].channel() < PARAM_CHAN_OFFSET ) {
	ol[k].addError( DaqError::InvalidChannel );
	ol[k].setChannel( channels()-1 );
      }
    }
  }

  unsigned int chanlist[MAXCHANLIST];
  setupChanList( ol, chanlist, MAXCHANLIST );

  double buffertime = sigs[0].interval( FIFOSize/BufferElemSize/sigs.size() );
  if ( buffertime < sigs[0].writeTime() )
    ol.addError( DaqError::InvalidBufferTime );

  if( ol.failed() )
    return -1;

  return 0;
}


int DynClampAnalogOutput::prepareWrite( OutList &sigs )
{
  //  cerr << " DynClampAnalogOutput::prepareWrite(): 1\n";/////TEST/////

  if ( !isOpen() )
    return -1;

  reset();

  if ( sigs.size() <= 0 )
    return -1;

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  unsigned int chanlist[MAXCHANLIST];
  setupChanList( ol, chanlist, MAXCHANLIST );

  // set chanlist:
  struct chanlistIOCT chanlistIOC;
  chanlistIOC.subdevID = SubdeviceID;
  for( int k = 0; k < ol.size(); k++ ){
    chanlistIOC.chanlist[k] = chanlist[k];
    if ( ol[k].channel() < PARAM_CHAN_OFFSET ) {
      const comedi_polynomial_t* poly = 
	(const comedi_polynomial_t *)ol[k].gainData();
      chanlistIOC.conversionlist[k].order = poly->order;
      if ( poly->order >= MAX_CONVERSION_COEFFICIENTS )
	cerr << "ERROR in DynClampAnalogInput::prepareWrite -> invalid order in converion polynomial!\n";
      chanlistIOC.conversionlist[k].expansion_origin = poly->expansion_origin;
      for ( int c=0; c<MAX_CONVERSION_COEFFICIENTS; c++ )
	chanlistIOC.conversionlist[k].coefficients[c] = poly->coefficients[c];
      chanlistIOC.scalelist[k] = ol[k].scale();
    }
  }
  chanlistIOC.userDeviceIndex = ol[0].device();
  chanlistIOC.chanlistN = ol.size();
  int retval = ::ioctl( ModuleFd, IOC_CHANLIST, &chanlistIOC );
  //  cerr << "prepareWrite(): IOC_CHANLIST done!\n"; /// TEST
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::prepareWrite -> ioctl command IOC_CHANLIST on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // set up synchronous command:
  struct syncCmdIOCT syncCmdIOC;
  syncCmdIOC.subdevID = SubdeviceID;
  syncCmdIOC.frequency = (unsigned int)::rint( ol[0].sampleRate() );
  syncCmdIOC.delay = ol[0].indices( ol[0].delay() );
  syncCmdIOC.duration = ol[0].size();
  syncCmdIOC.continuous = ol[0].continuous();
  syncCmdIOC.startsource = ol[0].startSource();
  retval = ::ioctl( ModuleFd, IOC_SYNC_CMD, &syncCmdIOC );
  //  cerr << "prepareWrite(): IOC_SYNC_CMD done!\n"; /// TEST
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::prepareWrite -> ioctl command IOC_SYNC_CMD on device "
	 << ModuleDevice << " failed!\n";
    if ( errno == EINVAL )
      ol.addError( DaqError::InvalidSampleRate );
    else
      ol.addErrorStr( errno );
    return -1;
  }

  // apply calibration:
  /*
  XXX this requires user space comedi!
  if ( Calibration != 0 ) {
    for( int k=0; k < ol.size(); k++ ) {
      unsigned int channel = CR_CHAN( Cmd.chanlist[k] );
      unsigned int range = CR_RANGE( Cmd.chanlist[k] );
      unsigned int aref = CR_AREF( Cmd.chanlist[k] );
      if ( comedi_apply_parsed_calibration( DeviceP, SubDevice, channel,
					    range, aref, Calibration ) < 0 )
	ol[k].addError( DaqError::CalibrationFailed );
    }
  }
  */

  IsPrepared = ol.success();

  if ( ! ol.success() )
    return -1;

  for ( int k=0; k<ol.size(); k++ )
    ol[k].deviceReset( 0 );

  // set buffer size:
  BufferSize = 5*sigs.size()*sigs[0].indices( sigs[0].writeTime() )*BufferElemSize;
  int nbuffer = sigs.deviceBufferSize()*BufferElemSize;
  if ( nbuffer < BufferSize )
    BufferSize = nbuffer;
  if ( BufferSize > FIFOSize/(int)BufferElemSize )
    sigs.addError( DaqError::InvalidBufferTime );

  setSettings( ol, BufferSize );

  if ( ! ol.success() )
    return -1;

  Sigs = ol;
  Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

  return 0;
}


int DynClampAnalogOutput::fillWriteBuffer( void )
{
  if ( !isOpen() ) {
    Sigs.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  ErrorState = 0;

  if ( ! Sigs[0].deviceWriting() ) {
    Sigs.addError( DaqError::NoData );
    return -1;
  }

  int maxntry = 2;
  int ern = 0;
  int elemWritten = 0;

  // try to write twice
  for ( int tryit = 0;
	tryit < maxntry && Sigs[0].deviceWriting(); 
	tryit++ ){

    // multiplex data into buffer:
    float *bp = (float*)(Buffer+NBuffer);
    int maxn = (BufferSize-NBuffer)/sizeof( float )/Sigs.size();
    int bytesConverted = 0;
    for ( int i=0; i<maxn && Sigs[0].deviceWriting(); i++ ) {
      for ( int k=0; k<Sigs.size(); k++ ) {
	*bp = Sigs[k].deviceValue();
	if ( Sigs[k].deviceIndex() >= Sigs[k].size() )
	  Sigs[k].incrDeviceCount();
	++bp;
	++bytesConverted;
      }
    }
    bytesConverted *= sizeof( float );
    NBuffer += bytesConverted;

    // transfer buffer to kernel modul:
    int bytesWritten = ::write( FifoFd, Buffer, NBuffer );
      
    if ( bytesWritten < 0 ) {
      ern = errno;
      if ( ern == EAGAIN || ern == EINTR ) {
	ern = 0;
	break;
      }
    }
    else if ( bytesWritten > 0 ) {
      memmove( Buffer, Buffer+bytesWritten, NBuffer-bytesWritten );
      NBuffer -= bytesWritten;
      elemWritten += bytesWritten / BufferElemSize;
    }
  }

  if ( ern == 0 ) {
    // no more data:
    if ( Sigs[0].deviceWriting() == 0 ) {
      if ( Buffer != 0 )
	delete [] Buffer;
      Buffer = 0;
      BufferSize = 0;
      NBuffer = 0;
      return 0;
    }
  }
  else {
    // error:
    switch( ern ) {

    case EPIPE: 
      ErrorState = 1;
      Sigs.addError( DaqError::OverflowUnderrun );
      return -1;

    case EBUSY:
      ErrorState = 2;
      Sigs.addError( DaqError::Busy );
      return -1;

    default:
      ErrorState = 2;
      Sigs.addErrorStr( ern );
      Sigs.addError( DaqError::Unknown );
      return -1;
    }
  }
  
  return elemWritten;
}


int DynClampAnalogOutput::startWrite( void )
{
  //  cerr << " DynClampAnalogOutput::startWrite(): 1\n";/////TEST/////

  if( !prepared() || Sigs.empty() ) {
    cerr << "AO not prepared or no signals!\n";
    return -1;
  }

  // fill buffer with initial data:
  int retval = fillWriteBuffer();
  //  cerr << " DynClampAnalogOutput::startWrite -> fillWriteBuffer returned " << retval << '\n';

  if ( retval < 0 )
    return -1;

  // start subdevice:
  retval = ::ioctl( ModuleFd, IOC_START_SUBDEV, &SubdeviceID );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::startWrite -> ioctl command IOC_START_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    int ern = errno;
    if ( ern == ENOMEM )
      cerr << " !!! No stack for kernel task !!!\n";
    Sigs.addErrorStr( ern );
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
    Sigs.setSampleRate( (double)rate );

  ErrorState = 0;
  
  return 0;
}


int DynClampAnalogOutput::writeData( void )
{
  //  cerr << " DynClampAnalogOutput::writeData(): in\n";/////TEST/////

  if ( Sigs.empty() )
    return -1;

  //device stopped?
  if( !running() ) {
    Sigs.addErrorStr( "DynClampAnalogOutput::writeData: " +
		      deviceFile() + " is not running!" );
    cerr << "DynClampAnalogOutput::writeData: device is not running!"  << '\n';/////TEST/////
    return -1;
  }

  return fillWriteBuffer();
}


int DynClampAnalogOutput::reset( void )
{ 
  Sigs.clear();
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  Settings.clear();
  ErrorState = 0;

  if( !IsPrepared )
    return 0;

  int running = SubdeviceID;
  int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
  if( retval < 0 ) {
    //    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
    //	 << ModuleDevice << " failed!\n";
    return -1;
  }

  if ( running > 0 ) {
    retval = ::ioctl( ModuleFd, IOC_STOP_SUBDEV, &SubdeviceID );
    if( retval < 0 ) {
      cerr << " DynClampAnalogOutput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }
    rtf_reset( FifoFd );
  }

  IsPrepared = false;
  IsRunning = false;

  return 0;
}


bool DynClampAnalogOutput::running( void ) const
{
  if( !IsPrepared )
    return false;

  int running = SubdeviceID;
  int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << ModuleDevice << " failed!\n";
    return false;
  }
  //  cerr << "DynClampAnalogOutput::running returned " << running << '\n';

  return running;
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
  int retval = ::ioctl( ModuleFd, IOC_GETAOINDEX, &index );
  //    cerr << " DynClampAnalogOutput::index() -> " << index << '\n';
  if( retval < 0 ) {
    cerr << " DynClampAnalogOutput::index() -> ioctl command IOC_GETLOOPCNT on device "
	   << ModuleDevice << " failed!\n";
    return -1;
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


bool DynClampAnalogOutput::useAIRate( void ) const
{
  return true;
}


void DynClampAnalogOutput::addTraces( vector< TraceSpec > &traces, int deviceid ) const
{
  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = PARAM_OUT;
  int channel = PARAM_CHAN_OFFSET;
  while ( 0 == ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) ) {
    traces.push_back( TraceSpec( traces.size(), traceInfo.name,
				 deviceid, channel++, 1.0, traceInfo.unit ) );
  }
  int ern = errno;
  if ( ern != ERANGE )
    cerr << "DynClampAnalogOutput::addTraces() -> errno " << strerror( errno ) <<  '\n';
}


int DynClampAnalogOutput::matchTraces( vector< TraceSpec > &traces ) const
{
  int failed = false;
  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = TRACE_OUT;
  struct traceChannelIOCT traceChannel;
  traceChannel.traceType = TRACE_OUT;
  string unknowntraces = "";
  int foundtraces = 0;
  while ( ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) == 0 ) {
    bool notfound = true;
    for ( unsigned int k=0; k<traces.size(); k++ ) {
      if ( traces[k].traceName() == traceInfo.name ) {
	if ( traces[k].unit() != traceInfo.unit ) {
	  failed = true;
	  cerr << "! DynClampAnalogOutput::matchTraces -> model input trace " << traces[k].traceName() << " requires as unit '" << traceInfo.unit << "', not '" << traces[k].unit() << "'\n";
	  //	  traces[k].addErrorStr( "model input trace " + traces[k].traceName() + " requires as unit '" + traceInfo.unit + "', not '" + traces[k].unit() + "'" );
	}
	traceChannel.device = traces[k].device();
	traceChannel.channel = traces[k].channel();
	if ( ::ioctl( ModuleFd, IOC_SET_TRACE_CHANNEL, &traceChannel ) != 0 ) {
	  failed = true;
	  cerr << "! DynClampAnalogOutput::matchTraces -> failed to pass device and channel information to model output trace -> errno=" << errno << '\n';
	  //	  traces[k].addErrorStr( "failed to pass device and channel information to model output trace -> errno=" + Str( errno ) );
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
    failed = true;
    cerr << "! DynClampAnalogOutput::matchTraces -> failure in getting model output traces -> errno=" << ern << '\n';
    //    traces.addErrorStr( "failure in getting model output traces -> errno=" + Str( ern ) );
  }
  if ( ! unknowntraces.empty() ) {
    failed = true;
    cerr << "! DynClampAnalogOutput::matchTraces -> unable to match model output traces" << unknowntraces << '\n';
    //    traces.addErrorStr( "unable to match model output traces" + unknowntraces );
  }

  return failed ? -1 : foundtraces;
}


bool DynClampAnalogOutput::prepared( void ) const 
{ 
  return IsPrepared;
}


}; /* namespace comedi */
