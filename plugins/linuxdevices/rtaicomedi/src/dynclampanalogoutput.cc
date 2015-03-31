/*
  rtaicomedi/dynclampanalogoutput.cc
  Interface for accessing analog output of a daq-board via the dynamic clamp 
  kernel module.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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
#include <QMutexLocker>
#include <relacs/analoginput.h>
#include <relacs/rtaicomedi/dynclampanalogoutput.h>
#include <rtai_fifos.h>
using namespace std;
using namespace relacs;

namespace rtaicomedi {

#ifdef ENABLE_COMPUTATION
#include "module/model.c"
#endif

DynClampAnalogOutput::DynClampAnalogOutput( void ) 
  : AnalogOutput( "DynClampAnalogOutput", DynClampAnalogIOType )
{
  ModuleDevice = "";
  ModuleFd = -1;
  FifoFd = -1;
  SubDevice = -1;
  BufferElemSize = sizeof(float);
  Channels = 0;
  MaxRate = 50000.0;
  IsPrepared = false;
  NoMoreData = true;
  IsRunning = false;
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
  ModuleDevice = "";
  ModuleFd = -1;
  FifoFd = -1;
  SubDevice = -1;
  BufferElemSize = sizeof(float);
  Channels = 0;
  MaxRate = 50000.0;
  IsPrepared = false;
  NoMoreData = true;
  IsRunning = false;
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
}


int DynClampAnalogOutput::open( const string &device, const Options &opts )
{ 
  clearError();
  if ( isOpen() )
    return -5;

  Info.clear();
  Settings.clear();
  if ( device.empty() )
    return InvalidDevice;

  // open comedi device:
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    setErrorStr( "device file " + device + " could not be opened. Check permissions." );
    return NotOpen;
  }

  // get AO subdevice:
  int subdev = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_AO, 0 );
  if ( subdev < 0 ) {
    setErrorStr( "device "  + device + " does not support analog output" );
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

  // external reference:
  double extr = opts.number( "extref", -1.0, "V" );
  setExternalReference( extr );

  // initialize ranges:
  UnipolarRange.clear();
  BipolarRange.clear();
  UnipolarRangeIndex.clear();
  BipolarRangeIndex.clear();
  int nRanges = comedi_get_n_ranges( DeviceP, SubDevice, 0 );  
  for ( int i = 0; i < nRanges; i++ ) {
    comedi_range *range = comedi_get_range( DeviceP, SubDevice, 0, i );
    if ( range->min < 0.0 ) {
      if ( range->unit & RF_EXTERNAL ) {
	if ( extr > 0.0 ) {
	  range->max = extr;
	  range->min = -extr;
	}
	else
	  continue;
      }
      BipolarRange.push_back( *range );
      BipolarRangeIndex.push_back( i );
    }
    else {
      if ( range->unit & RF_EXTERNAL ) {
	if ( extr > 0.0 ) {
	  range->max = extr;
	  range->min = 0.0;
	}
	else
	  continue;
      }
      UnipolarRange.push_back( *range );
      UnipolarRangeIndex.push_back( i );
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
				      COMEDI_FROM_PHYSICAL, Calibration,
				      &UnipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( DeviceP, SubDevice, c,
				      UnipolarRangeIndex[r],
				      COMEDI_FROM_PHYSICAL,
				      &UnipConverter[c][r] );
      }
    }
    BipConverter[c] = new comedi_polynomial_t[BipolarRangeIndex.size()];
    for ( unsigned int r=0; r<BipolarRangeIndex.size(); r++ ) {
      if ( softcal && Calibration != 0 ) {
	comedi_get_softcal_converter( SubDevice, c, BipolarRangeIndex[r],
				      COMEDI_FROM_PHYSICAL, Calibration,
				      &BipConverter[c][r] );
      }
      else {
	comedi_get_hardcal_converter( DeviceP, SubDevice, c,
				      BipolarRangeIndex[r],
				      COMEDI_FROM_PHYSICAL, 
				      &BipConverter[c][r] );
      }
    }
  }

  // open kernel module:
  ModuleDevice = "/dev/dynclamp";
  ModuleFd = ::open( ModuleDevice.c_str(), O_WRONLY ); //O_RDONLY
  if ( ModuleFd == -1 ) {
    setErrorStr( "opening dynclamp-module " + ModuleDevice + " failed" );
    return -1;
  }

  // set device and subdevice:
  struct deviceIOCT deviceIOC;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = SubDevice;
  deviceIOC.subdevType = SUBDEV_OUT;
  deviceIOC.errorstr[0] = '\0';
  int retval = ::ioctl( ModuleFd, IOC_OPEN_SUBDEV, &deviceIOC );
  if ( retval < 0 ) {
    setErrorStr( "ioctl command IOC_OPEN_SUBDEV on device " + ModuleDevice + " failed: " +
		 deviceIOC.errorstr );
    ::ioctl( ModuleFd, IOC_REQ_CLOSE, SubDevice );
    ::close( ModuleFd );
    ModuleFd = -1;
    return -1;
  }

  // initialize connection to RTAI-FIFO:
  ostringstream fifoname;
  fifoname << "/dev/rtf" << deviceIOC.fifoIndex;
  FifoFd = ::open( fifoname.str().c_str(), O_WRONLY );
  if ( FifoFd < 0 ) {
    setErrorStr( "oping RTAI-FIFO " + fifoname.str() + " failed" );
    return -1;
  }
  FIFOSize = deviceIOC.fifoSize;

  // XXX Set the maximum possible sampling rate (of the rtai loop!):
  MaxRate = MAX_FREQUENCY;

  // compute lookup tables:
#ifdef ENABLE_COMPUTATION
#ifdef ENABLE_LOOKUPTABLES
  for ( int k=0; ; k++ ) {
    float *x = 0;
    float *y = 0;
    int n = 0;
    if ( generateLookupTable( k, &x, &y, &n ) < 0 ) 
      break;
    // transfer to kernel:
    retval = ::ioctl( ModuleFd, IOC_SET_LOOKUP_K, &k );
    if ( retval < 0 ) {
      setErrorStr( "ioctl command IOC_SET_LOOKUP_K on device " +
		   ModuleDevice + " failed" );
      return -1;
    }
    retval = ::ioctl( ModuleFd, IOC_SET_LOOKUP_N, &n );
    if ( retval < 0 ) {
      setErrorStr( "ioctl command IOC_SET_LOOKUP_N on device " +
		   ModuleDevice + " failed" );
      return -1;
    }
    retval = ::ioctl( ModuleFd, IOC_SET_LOOKUP_X, x );
    if ( retval < 0 ) {
      setErrorStr( "ioctl command IOC_SET_LOOKUP_X on device " +
		   ModuleDevice + " failed" );
      return -1;
    }
    retval = ::ioctl( ModuleFd, IOC_SET_LOOKUP_Y, y );
    if ( retval < 0 ) {
      setErrorStr( "ioctl command IOC_SET_LOOKUP_Y on device " +
		   ModuleDevice + " failed" );
      return -1;
    }
    delete [] x;
    delete [] y;
  }
#endif
#endif

  IsPrepared = false;
  NoMoreData = true;

  setInfo();

  return 0;
}


bool DynClampAnalogOutput::isOpen( void ) const 
{ 
  lock();
  bool o = ( ModuleFd >= 0 );
  unlock();
  return o;
}


void DynClampAnalogOutput::close( void )
{ 
  clearError();
  if ( ! isOpen() )
    return;

  reset();

  // cleanup calibration:
  if ( Calibration != 0 )
    comedi_cleanup_calibration( Calibration );
  Calibration = 0;

  if ( ModuleFd >= 0 ) {
    ::ioctl( ModuleFd, IOC_REQ_CLOSE, SubDevice );
    if ( FifoFd >= 0 )
      ::close( FifoFd );
    if ( ::close( ModuleFd ) < 0 )
      setErrorStr( "closing of module file failed" );
    ModuleFd = -1;
  }

  for ( int c=0; c<Channels; c++ ) {
    delete [] UnipConverter[c];
    delete [] BipConverter[c];
  }
  delete [] UnipConverter;
  delete [] BipConverter;
  UnipConverter = 0;
  BipConverter = 0;

  IsPrepared = false;
  NoMoreData = true;

  Info.clear();
}


int DynClampAnalogOutput::channels( void ) const
{ 
  return Channels;
}


int DynClampAnalogOutput::bits( void ) const
{ 
  if ( !isOpen() )
    return -1;
  lock();
  int maxData = comedi_get_maxdata( DeviceP, SubDevice, 0 );
  unlock();
  return (int)( log( maxData+2.0 )/ log( 2.0 ) );
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
  return UnipolarRangeIndex.size() > BipolarRangeIndex.size() ?
    UnipolarRangeIndex.size() : BipolarRangeIndex.size();
}


double DynClampAnalogOutput::unipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)UnipolarRangeIndex.size()) )
    return -1.0;
  return UnipolarRange[index].max;
}


double DynClampAnalogOutput::bipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)BipolarRangeIndex.size()) )
    return -1.0;
  return BipolarRange[index].max;
}


void DynClampAnalogOutput::setupChanList( OutList &sigs,
					  unsigned int *chanlist,
					  int maxchanlist,
					  bool setscale )
{
  memset( chanlist, 0, maxchanlist * sizeof( unsigned int ) );

  for ( int k=0; k<sigs.size() && k < maxchanlist; k++ ) {

    // parameter signals don't have references and gains:
    if ( sigs[k].channel() >= PARAM_CHAN_OFFSET ) {
      chanlist[k] = CR_PACK( sigs[k].channel(), 0, 0 );
      continue;
    }

    // check channel:
    if ( sigs[k].channel() < 0 || sigs[k].channel() >= channels() ) {
      sigs[k].addError( DaqError::InvalidChannel );
      return;
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
    /*
    // whatever the following was ment to be, it ensures that unipolar never gets true!
    // if it gets true, we get errors elsewhere.
    bool unipolar = ( min >= 0.0 );
    */
    bool minislarger = false;
    // maximum value:
    if ( ::fabs( min ) > max ) {
      max = ::fabs( min );
      minislarger = true;
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
    if ( sigs[k].noLevel() ) {
      // check for suitable range:
      if ( unipolar ) {
	for( index = UnipolarRange.size() - 1; index >= 0; index-- ) {
	  if ( unipolarRange( index ) >= maxvolt )
	    break;
	}
      }
      else {
	for( index = BipolarRange.size() - 1; index >= 0; index-- ) {
	  if ( bipolarRange( index ) >= maxvolt )
	    break;
	}
      }
      if ( index < 0 ) {
	if ( minislarger )
	  sigs[k].addError( DaqError::Underflow );
	else
	  sigs[k].addError( DaqError::Overflow );
      }
    }
    else {
      // use largest range:
      index = 0;
      if ( unipolar && index >= (int)UnipolarRange.size() )
	index = -1;
      if ( ! unipolar && index >= (int)BipolarRange.size() )
	index = -1;
      // signal must be within -1 and 1:
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

    double maxboardvolt = unipolar ? UnipolarRange[index].max : BipolarRange[index].max;
    double minboardvolt = unipolar ? UnipolarRange[index].min : BipolarRange[index].min;
    if ( !sigs[k].noLevel() && setscale )
      sigs[k].multiplyScale( maxboardvolt );

    int gainIndex = index;
    // XXX Where the hack is the following used? Shouldn't we just use the plain index?
    if ( unipolar )
      gainIndex |= 1<<14;
    /*
    if ( extref )
      gainIndex |= 1<<15;
    */

    sigs[k].setGainIndex( gainIndex );
    sigs[k].setMinVoltage( minboardvolt );
    sigs[k].setMaxVoltage( maxboardvolt );

    // reference:
    int aref = AREF_GROUND;

    // set up channel in chanlist:
    int gi = unipolar ? UnipolarRangeIndex[ index ] : BipolarRangeIndex[ index ];
    if ( unipolar ) {
      memcpy( gainp, &UnipConverter[sigs[k].channel()][index], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( sigs[k].channel(), gi, aref );
    }
    else {
      memcpy( gainp, &BipConverter[sigs[k].channel()][index], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( sigs[k].channel(), gi, aref );
    }

    //    cerr << "DYNCLAMP ChanList channel " << sigs[k].channel() << " packed " << CR_CHAN( chanlist[k] ) << '\n';

  }
}


int DynClampAnalogOutput::directWrite( OutList &sigs )
{
  // not open:
  if ( !isOpen() )
    return -1;

  lock();
  Sigs.clear();
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;
  Settings.clear();
  IsPrepared = false;
  NoMoreData = true;
  IsRunning = false;
  unlock();

  // no signals:
  if ( sigs.size() <= 0 )
    return -1;

  // XXX make sure that all signal have size 1!

  {
    QMutexLocker locker( mutex() );

    // copy and sort signal pointers:
    OutList ol;
    ol.add( sigs );
    ol.sortByChannel();

    unsigned int chanlist[MAXCHANLIST];
    setupChanList( ol, chanlist, MAXCHANLIST, true );

    if ( ol.failed() )
      return -1;

    // set chanlist:
    struct chanlistIOCT chanlistIOC;
    chanlistIOC.type = SUBDEV_OUT;
    for( int k = 0; k < ol.size(); k++ ){
      chanlistIOC.chanlist[k] = chanlist[k];
      if ( ol[k].channel() < PARAM_CHAN_OFFSET ) {
	const comedi_polynomial_t* poly = 
	  (const comedi_polynomial_t *)ol[k].gainData();
	chanlistIOC.conversionlist[k].order = poly->order;
	if ( poly->order >= MAX_CONVERSION_COEFFICIENTS )
	  cerr << "ERROR in DynClampAnalogOutput::directWrite -> order=" << poly->order << " in conversion polynomial too large!\n";
	chanlistIOC.conversionlist[k].expansion_origin = poly->expansion_origin;
	for ( int c=0; c<MAX_CONVERSION_COEFFICIENTS; c++ )
	  chanlistIOC.conversionlist[k].coefficients[c] = poly->coefficients[c];
	chanlistIOC.scalelist[k] = ol[k].scale();
	// apply calibration:
	if ( Calibration != 0 ) {
	  unsigned int channel = CR_CHAN( chanlist[k] );
	  unsigned int range = CR_RANGE( chanlist[k] );
	  unsigned int aref = CR_AREF( chanlist[k] );
	  if ( comedi_apply_parsed_calibration( DeviceP, SubDevice, channel,
						range, aref, Calibration ) < 0 )
	    ol[k].addError( DaqError::CalibrationFailed );
	}
      }
    }
    chanlistIOC.chanlistN = ol.size();
    int retval = ::ioctl( ModuleFd, IOC_CHANLIST, &chanlistIOC );
    if ( retval < 0 ) {
      cerr << " DynClampAnalogOutput::directWrite -> ioctl command IOC_CHANLIST on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }

    // set up synchronous command:
    struct syncCmdIOCT syncCmdIOC;
    syncCmdIOC.type = SUBDEV_OUT;
    syncCmdIOC.frequency = 0;
    syncCmdIOC.delay = 0;
    syncCmdIOC.duration = 1;
    syncCmdIOC.continuous = 0;
    syncCmdIOC.startsource = 0;
    retval = ::ioctl( ModuleFd, IOC_SYNC_CMD, &syncCmdIOC );
    if ( retval < 0 ) {
      cerr << " DynClampAnalogOutput::directWrite -> ioctl command IOC_SYNC_CMD on device "
	   << ModuleDevice << " failed!\n";
      if ( errno == EINVAL )
	ol.addError( DaqError::InvalidSampleRate );
      else
	ol.addErrorStr( errno );
      return -1;
    }

    if ( ol.failed() )
      return -1;

    BufferSize = ol.size() * BufferElemSize;
    Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

    ol.deviceReset( 0 );
    Sigs = ol;

  } // unlock

  int retval = writeData();
  //  cerr << " DynClampAnalogOutput::directWrite -> fillWriteBuffer returned " << retval << '\n';

  QMutexLocker locker( mutex() );

  delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  if ( retval < 0 )
    return -1;

  // start subdevice:
  retval = ::ioctl( ModuleFd, IOC_START_SUBDEV, SUBDEV_OUT );
  if ( retval < 0 ) {
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
  if ( ! isOpen() ) {
    sigs.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  QMutexLocker locker( mutex() );
  // sampling rate must be the one of the running rt-loop:
  unsigned int rate = 0;
  int retval = ::ioctl( ModuleFd, IOC_GETRATE, &rate );
  if ( retval < 0 ) {
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
  for ( int k=0; k<ol.size(); k++ ) {
    ol[k].delError( DaqError::InvalidChannel );
    // check channel number:
    if ( ol[k].channel() < 0 ) {
      ol[k].addError( DaqError::InvalidChannel );
      ol[k].setChannel( 0 );
    }
    else if ( ol[k].channel() >= channels() && ol[k].channel() < PARAM_CHAN_OFFSET ) {
      ol[k].addError( DaqError::InvalidChannel );
      ol[k].setChannel( channels()-1 );
    }
  }

  unsigned int chanlist[MAXCHANLIST];
  setupChanList( ol, chanlist, MAXCHANLIST, false );

  double buffertime = sigs[0].interval( FIFOSize/BufferElemSize/sigs.size() );
  if ( buffertime < 0.001 )
    ol.addError( DaqError::InvalidBufferTime );

  if ( ol.failed() )
    return -1;

  return 0;
}


int DynClampAnalogOutput::prepareWrite( OutList &sigs )
{
  if ( !isOpen() )
    return -1;

  lock();
  Sigs.clear();
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;
  Settings.clear();
  IsPrepared = false;
  NoMoreData = true;
  IsRunning = false;
  unlock();

  if ( sigs.size() <= 0 )
    return -1;

  {
    QMutexLocker locker( mutex() );

    // copy and sort signal pointers:
    OutList ol;
    ol.add( sigs );
    ol.sortByChannel();

    unsigned int chanlist[MAXCHANLIST];
    setupChanList( ol, chanlist, MAXCHANLIST, true );

    if ( sigs.failed() )
      return -1;

    // set chanlist:
    struct chanlistIOCT chanlistIOC;
    chanlistIOC.type = SUBDEV_OUT;
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
	// apply calibration:
	if ( Calibration != 0 ) {
	  unsigned int channel = CR_CHAN( chanlist[k] );
	  unsigned int range = CR_RANGE( chanlist[k] );
	  unsigned int aref = CR_AREF( chanlist[k] );
	  if ( comedi_apply_parsed_calibration( DeviceP, SubDevice, channel,
						range, aref, Calibration ) < 0 )
	    ol[k].addError( DaqError::CalibrationFailed );
	}
      }
    }
    chanlistIOC.chanlistN = ol.size();
    int retval = ::ioctl( ModuleFd, IOC_CHANLIST, &chanlistIOC );
    //  cerr << "prepareWrite(): IOC_CHANLIST done!\n"; /// TEST
    if ( retval < 0 ) {
      cerr << " DynClampAnalogOutput::prepareWrite -> ioctl command IOC_CHANLIST on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }

    // set up synchronous command:
    struct syncCmdIOCT syncCmdIOC;
    syncCmdIOC.type = SUBDEV_OUT;
    syncCmdIOC.frequency = (unsigned int)::rint( ol[0].sampleRate() );
    syncCmdIOC.delay = ol[0].indices( ol[0].delay() );
    syncCmdIOC.duration = ol[0].size();
    syncCmdIOC.continuous = ol[0].continuous();
    syncCmdIOC.startsource = ol[0].startSource();
    retval = ::ioctl( ModuleFd, IOC_SYNC_CMD, &syncCmdIOC );
    //  cerr << "prepareWrite(): IOC_SYNC_CMD done!\n"; /// TEST
    if ( retval < 0 ) {
      cerr << " DynClampAnalogOutput::prepareWrite -> ioctl command IOC_SYNC_CMD on device "
	   << ModuleDevice << " failed!\n";
      if ( errno == EINVAL )
	ol.addError( DaqError::InvalidSampleRate );
      else
	ol.addErrorStr( errno );
      return -1;
    }

    if ( ! ol.success() )
      return -1;

    for ( int k=0; k<ol.size(); k++ )
      ol[k].deviceReset( 0 );

    // set buffer size:
    BufferSize = FIFOSize;
    int nbuffer = sigs.deviceBufferSize()*BufferElemSize;
    if ( nbuffer < BufferSize )
      BufferSize = nbuffer;

    setSettings( ol, BufferSize );

    if ( ! ol.success() )
      return -1;

    Sigs = ol;
    Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

  } //  unlock

  // fill buffer with initial data:
  int r = writeData();
  if ( r < 0 )
    return -1;

  lock();
  IsPrepared = Sigs.success();
  NoMoreData = ( r == 0 );
  unlock();

  return 0;
}


int DynClampAnalogOutput::startWrite( QSemaphore *sp )
{
  QMutexLocker locker( mutex() );

  if ( !IsPrepared || Sigs.empty() ) {
    cerr << "AO not prepared or no signals!\n";
    return -1;
  }


  // start subdevice:
  int retval = ::ioctl( ModuleFd, IOC_START_SUBDEV, SUBDEV_OUT );
  if ( retval < 0 ) {
    int ern = errno;
    Sigs.addErrorStr( ern );
    if ( ern == ENOMEM )
      Sigs.addErrorStr( "no stack for kernel task" );
    return -1;
  }

  // get sampling rate:
  unsigned int rate = 0;
  retval = ::ioctl( ModuleFd, IOC_GETRATE, &rate );
  if ( retval < 0 ) {
    cerr << " DynClampAnalogOutput::testWriteDevice -> ioctl command IOC_GETRATE on device "
	 << ModuleDevice << " failed!\n";
  }
  else
    Sigs.setSampleRate( (double)rate );

  startThread( sp );

  return NoMoreData ? 0 : 1;
}


int DynClampAnalogOutput::writeData( void )
{
  //  cerr << " DynClampAnalogOutput::writeData(): in\n";/////TEST/////

  QMutexLocker locker( mutex() );

  if ( Sigs.empty() )
    return -1;

  // device stopped?
  if ( IsPrepared ) {
    int running = SUBDEV_OUT;
    int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
    if ( retval < 0 ) {
      //    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
      //	 << ModuleDevice << " failed!\n";
      return -1;
    }
    if ( !running ) {
      Sigs.addErrorStr( "DynClampAnalogOutput::writeData: " +
			deviceFile() + " is not running!" );
      cerr << "DynClampAnalogOutput::writeData: device is not running!"  << '\n';/////TEST/////
      setErrorStr( Sigs );
      return -1;
    }
  }

  if ( Sigs[0].deviceWriting() ) {
    // multiplex data into buffer:
    float *bp = (float*)(Buffer+NBuffer);
    int maxn = (BufferSize-NBuffer)/sizeof( float )/Sigs.size();
    // XXX lets keep the number of transfered data small:
    // XXX this should be translated to the FIFO buffer size!!!
    if ( maxn > Sigs[0].indices( 0.01 ) )
      maxn = Sigs[0].indices( 0.01 );
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
  }

  if ( ! Sigs[0].deviceWriting() && NBuffer == 0 )
    return 0;

  // transfer buffer to kernel modul:
  int bytesWritten = ::write( FifoFd, Buffer, NBuffer );

  int ern = 0;
  int elemWritten = 0;
      
  if ( bytesWritten < 0 ) {
    ern = errno;
    if ( ern == EAGAIN || ern == EINTR )
      ern = 0;
  }
  else if ( bytesWritten > 0 ) {
    memmove( Buffer, Buffer+bytesWritten, NBuffer-bytesWritten );
    NBuffer -= bytesWritten;
    elemWritten += bytesWritten / BufferElemSize;
  }

  if ( ern == 0 ) {
    // no more data:
    if ( ! Sigs[0].deviceWriting() && NBuffer <= 0 ) {
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
      Sigs.addError( DaqError::OverflowUnderrun );
      break;

    case EBUSY:
      Sigs.addError( DaqError::Busy );
      break;

    default:
      Sigs.addErrorStr( ern );
      Sigs.addError( DaqError::Unknown );
      break;
    }

    setErrorStr( Sigs );
    return -1;
  }

  return elemWritten;
}


int DynClampAnalogOutput::stop( void )
{ 
  int running = 0;
  {
    QMutexLocker locker( mutex() );

    if ( !IsPrepared )
      return 0;

    running = SUBDEV_OUT;
    int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
    if ( retval < 0 ) {
      //    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
      //	 << ModuleDevice << " failed!\n";
      return -1;
    }
  }

  if ( running > 0 ) {
    stopWrite();
    QMutexLocker locker( mutex() );
    int retval = ::ioctl( ModuleFd, IOC_STOP_SUBDEV, SUBDEV_OUT );
    if ( retval < 0 ) {
      cerr << " DynClampAnalogOutput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }
    rtf_reset( FifoFd );
  }

  return 0;
}


int DynClampAnalogOutput::reset( void )
{ 
  lock();

  rtf_reset( FifoFd );

  Sigs.clear();
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  Settings.clear();

  IsPrepared = false;
  NoMoreData = true;
  IsRunning = false;

  unlock();

  return 0;
}


AnalogOutput::Status DynClampAnalogOutput::status( void ) const
{
  QMutexLocker locker( mutex() );

  if ( !IsPrepared )
    return Idle;

  int running = SUBDEV_OUT;
  int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
  if ( retval < 0 ) {
    cerr << " DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << ModuleDevice << " failed!\n";
    return UnknownError;
  }
  //  cerr << "DynClampAnalogOutput::running returned " << running << '\n';

  return running>0 ? Running : Idle;
}


long DynClampAnalogOutput::index( void ) const
{
  QMutexLocker locker( mutex() );

  long index = 0;
  int retval = ::ioctl( ModuleFd, IOC_GETAOINDEX, &index );
  //    cerr << " DynClampAnalogOutput::index() -> " << index << '\n';
  if ( retval < 0 ) {
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
      if ( traces[k].channel() < PARAM_CHAN_OFFSET && traces[k].traceName() == traceInfo.name ) {
	if ( traces[k].unit() != traceInfo.unit ) {
	  failed = true;
	  cerr << "! DynClampAnalogOutput::matchTraces -> model input trace " << traces[k].traceName() << " requires as unit '" << traceInfo.unit << "', not '" << traces[k].unit() << "'\n";
	  //	  traces[k].addErrorStr( "model input trace " + traces[k].traceName() + " requires as unit '" + traceInfo.unit + "', not '" + traces[k].unit() + "'" );
	}
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
  lock();
  bool ip = IsPrepared;
  unlock();
  return ip;
}


}; /* namespace rtaicomedi */
