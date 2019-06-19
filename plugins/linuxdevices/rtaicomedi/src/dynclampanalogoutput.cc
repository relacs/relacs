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
using namespace std;
using namespace relacs;

namespace rtaicomedi {

DynClampAnalogOutput::DynClampAnalogOutput( void ) 
  : AnalogOutput( "DynClampAnalogOutput", DynClampAnalogIOType )
{
  ModuleDevice = "";
  ModuleFd = -1;
  SubDevice = -1;
  BufferElemSize = sizeof(float);
  Channels = 0;
  MaxRate = 100000.0;
  IsPrepared = false;
  NoMoreData = true;
  IsRunning = false;
  UnipConverter = 0;
  BipConverter = 0;
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;

  initOptions();
}


DynClampAnalogOutput::DynClampAnalogOutput( const string &device,
					    const Options &opts ) 
  : DynClampAnalogOutput()
{
  Options::read(opts);
  open( device );
}


DynClampAnalogOutput::~DynClampAnalogOutput( void ) 
{
  close();
}

void DynClampAnalogOutput::initOptions()
{
  AnalogOutput::initOptions();

  addNumber( "extref", "Voltage of external reference", -1.0, -1.0, 100.0, 0.1, "V" );
}

int DynClampAnalogOutput::open( const string &device )
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
  double extr = number( "extref", -1.0, "V" );
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

  if ( BipolarRange.size() > 0 ) {
    LargestRange = BipolarRange[0];
    LargestRangeIndex = BipolarRangeIndex[0];
    LargestRangeUnipolar = false;
  }
  else if ( UnipolarRange.size() > 0 ) {
    LargestRange = UnipolarRange[0];
    LargestRangeIndex = UnipolarRangeIndex[0];
    LargestRangeUnipolar = true;
  }
  else {
    addErrorStr( "No analog output range available." );
    return WriteError;
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

  // maximum data values:
  MaxData.clear();
  MinVoltage.clear();
  MaxVoltage.clear();
  for ( int k=0; k<channels(); k++ ) {
    MaxData.push_back( comedi_get_maxdata( DeviceP, SubDevice, k ) );
    MinVoltage.push_back( LargestRange.min );
    MaxVoltage.push_back( LargestRange.max );
  }

  // write zeros to all channels:
  writeZeros();

  // open kernel module:
  ModuleDevice = "/dev/dynclamp";
  ModuleFd = ::open( ModuleDevice.c_str(), O_WRONLY | O_NONBLOCK );
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

  // set the maximum possible sampling rate (of the rtai loop!):
  MaxRate = MAX_FREQUENCY;

  IsPrepared = false;
  NoMoreData = true;

  // publish information about the analog input device:
  setInfo();
  vector< TraceSpec > traces;
  traces.clear();
  addTraces( traces, 0 );
  for ( unsigned int k=0; k<traces.size(); k++ ) {
    if ( traces[k].channel() >= PARAM_CHAN_OFFSET )
      Info.addText( "Model parameter", traces[k].traceName() );
  }

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

  // close kernel module:
  if ( ModuleFd >= 0 ) {
    ::ioctl( ModuleFd, IOC_REQ_CLOSE, SubDevice );
    if ( ::close( ModuleFd ) < 0 )
      setErrorStr( "closing of module device file failed" );
    ModuleFd = -1;
  }

  // write zeros to all channels:
  writeZeros();

  // cleanup calibration:
  if ( Calibration != 0 )
    comedi_cleanup_calibration( Calibration );
  Calibration = 0;
  
  // close comedi:
  int error = comedi_close( DeviceP );
  if ( error )
    setErrorStr( "closing of AO subdevice on device " + deviceFile() + "failed" );
  DeviceP = 0;

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


void DynClampAnalogOutput::writeZeros( void )
{
  bool softcal = ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) &
		     SDF_SOFT_CALIBRATED ) > 0 );
  bool unipolar = ( BipolarRangeIndex.size() == 0 );
  comedi_polynomial_t polynomial;
  for ( int k=0; k<channels(); k++ ) {
    int index = unipolar ? UnipolarRangeIndex[0] : BipolarRangeIndex[0];
    if ( softcal && Calibration != 0 )
      comedi_get_softcal_converter( SubDevice, k, index,
				    COMEDI_FROM_PHYSICAL, Calibration, &polynomial );
    else
      comedi_get_hardcal_converter( DeviceP, SubDevice, k, index,
				    COMEDI_FROM_PHYSICAL, &polynomial );
    float v = 0.0;
    lsampl_t data = comedi_from_physical( v, &polynomial );
    int retval = comedi_data_write( DeviceP, SubDevice, k, index, AREF_GROUND, data );
    if ( retval < 1 )
      setErrorStr( string( "comedi_direct_write failed to write zero: " ) + comedi_strerror( comedi_errno() ) );
  }
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
					  bool setscale ) const
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

    // allocate gain factor:
    char *gaindata = sigs[k].gainData();
    if ( gaindata != NULL )
      delete [] gaindata;
    gaindata = new char[sizeof(comedi_polynomial_t)];
    sigs[k].setGainData( gaindata );
    comedi_polynomial_t *gainp = (comedi_polynomial_t *)gaindata;

    // set range:
    double maxvolt = sigs[k].getVoltage( max );
    double minvolt = sigs[k].getVoltage( min );
    if ( sigs[k].noLevel() ) {
      if ( minvolt < LargestRange.min )
	sigs[k].addError( DaqError::Underflow );
      if ( maxvolt > LargestRange.max )
	sigs[k].addError( DaqError::Overflow );
    }
    else {
      // signal must be within -1 and 1:
      if ( max > 1.0+1.0e-8 )
	sigs[k].addError( DaqError::Overflow );
      if ( min < -1.0-1.0e-8 )
	sigs[k].addError( DaqError::Underflow );
    }

    double maxboardvolt = LargestRange.max;
    double minboardvolt = LargestRange.min;
    if ( !sigs[k].noLevel() && setscale )
      sigs[k].multiplyScale( maxboardvolt );
    sigs[k].setMinVoltage( minboardvolt );
    sigs[k].setMaxVoltage( maxboardvolt );

    // reference:
    int aref = AREF_GROUND;

    // set up channel in chanlist:
    int gi = LargestRangeIndex;
    if ( LargestRangeUnipolar ) {
      memcpy( gainp, &UnipConverter[sigs[k].channel()][0], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( sigs[k].channel(), gi, aref );
    }
    else {
      memcpy( gainp, &BipConverter[sigs[k].channel()][0], sizeof(comedi_polynomial_t) );
      chanlist[k] = CR_PACK( sigs[k].channel(), gi, aref );
    }

    //    cerr << "DYNCLAMP ChanList channel " << sigs[k].channel() << " packed " << CR_CHAN( chanlist[k] ) << '\n';

  }
}


int DynClampAnalogOutput::loadChanList( OutList &sigs, int isused ) const
{
  unsigned int chanlist[MAXCHANLIST];
  setupChanList( sigs, chanlist, MAXCHANLIST, true );

  if ( sigs.failed() )
    return -1;

  // set chanlist:
  struct chanlistIOCT chanlistIOC;
  chanlistIOC.type = SUBDEV_OUT;
  for( int k = 0; k < sigs.size(); k++ ){
    chanlistIOC.chanlist[k] = chanlist[k];
    chanlistIOC.maxdata[k] = 0;
    chanlistIOC.minvoltage[k] = 0;
    chanlistIOC.maxvoltage[k] = 0;
    chanlistIOC.isused[k] = isused;
    if ( sigs[k].channel() < PARAM_CHAN_OFFSET ) {
      chanlistIOC.maxdata[k] = MaxData[sigs[k].channel()];
      chanlistIOC.minvoltage[k] = MinVoltage[sigs[k].channel()];
      chanlistIOC.maxvoltage[k] = MaxVoltage[sigs[k].channel()];
      const comedi_polynomial_t* poly = 
	(const comedi_polynomial_t *)sigs[k].gainData();
      chanlistIOC.conversionlist[k].order = poly->order;
      if ( poly->order >= MAX_CONVERSION_COEFFICIENTS )
	cerr << "ERROR in DynClampAnalogInput::prepareWrite -> invalid order in conversion polynomial!\n";
      chanlistIOC.conversionlist[k].expansion_origin = poly->expansion_origin;
      for ( int c=0; c<MAX_CONVERSION_COEFFICIENTS; c++ )
	chanlistIOC.conversionlist[k].coefficients[c] = poly->coefficients[c];
      chanlistIOC.scalelist[k] = sigs[k].scale();
      // apply calibration:
      if ( Calibration != 0 ) {
	unsigned int channel = CR_CHAN( chanlist[k] );
	unsigned int range = CR_RANGE( chanlist[k] );
	unsigned int aref = CR_AREF( chanlist[k] );
	if ( comedi_apply_parsed_calibration( DeviceP, SubDevice, channel,
					      range, aref, Calibration ) < 0 )
	  sigs[k].addError( DaqError::CalibrationFailed );
      }
    }
  }
  chanlistIOC.chanlistN = sigs.size();
  int retval = ::ioctl( ModuleFd, IOC_CHANLIST, &chanlistIOC );
  if ( retval < 0 ) {
    if ( errno == EINVAL )
      sigs.addErrorStr( "Channel unknown to kernel module." );
    else
      sigs.addErrorStr( "Failed to transfer channel list." );
    cerr << "DynClampAnalogOutput::prepareWrite -> ioctl command IOC_CHANLIST on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }
  return 0;
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

    int retval = loadChanList( ol, 1 );
    if ( retval < 0 )
      return -1;

    // set up synchronous command:
    struct syncCmdIOCT syncCmdIOC;
    syncCmdIOC.type = SUBDEV_OUT;
    syncCmdIOC.frequency = 0;
    syncCmdIOC.delay = 0;
    syncCmdIOC.duration = 1;
    syncCmdIOC.continuous = 0;
    syncCmdIOC.startsource = 0;
    syncCmdIOC.buffersize = ol.size()*BufferElemSize;
    retval = ::ioctl( ModuleFd, IOC_SYNC_CMD, &syncCmdIOC );
    if ( retval < 0 ) {
      cerr << "DynClampAnalogOutput::directWrite -> ioctl command IOC_SYNC_CMD on device "
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
  //  cerr << "DynClampAnalogOutput::directWrite -> fillWriteBuffer returned " << retval << '\n';

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
    cerr << "DynClampAnalogOutput::directWrite -> ioctl command IOC_START_SUBDEV on device "
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
    cerr << "DynClampAnalogOutput::testWriteDevice -> ioctl command IOC_GETRATE on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  double reqrate = sigs[0].sampleRate();
  if ( ::fabs( reqrate ) < 1.0e-8 ) {
    if ( rate > 0 )
      sigs.setSampleRate( (double)rate );
    sigs.addError( DaqError::InvalidSampleRate );
  }
  else {
    if ( rate > 0 ) {
      if ( ::fabs( reqrate - (double)rate )/rate > 0.005 )  // less than 5 promille deviation
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

  QMutexLocker aolocker( mutex() );

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  int retval = loadChanList( ol, 1 );
  if ( retval < 0 )
    return -1;

  // set buffer size for one second:
  BufferSize = sigs.deviceBufferSize()*BufferElemSize;
  int maxbuffersize = sigs.size() * sigs[0].indices( 2.0 ) * BufferElemSize;
  if ( BufferSize > maxbuffersize )
    BufferSize = maxbuffersize;

  // set up synchronous command:
  struct syncCmdIOCT syncCmdIOC;
  syncCmdIOC.type = SUBDEV_OUT;
  syncCmdIOC.frequency = (unsigned int)::rint( ol[0].sampleRate() );
  syncCmdIOC.delay = ol[0].indices( ol[0].delay() );
  syncCmdIOC.duration = ol[0].size();
  syncCmdIOC.continuous = ol[0].continuous();
  syncCmdIOC.startsource = ol[0].startSource();
  syncCmdIOC.buffersize = BufferSize;
  retval = ::ioctl( ModuleFd, IOC_SYNC_CMD, &syncCmdIOC );
  //  cerr << "prepareWrite(): IOC_SYNC_CMD done!\n";
  if ( retval < 0 ) {
    cerr << "DynClampAnalogOutput::prepareWrite -> ioctl command IOC_SYNC_CMD on device "
	 << ModuleDevice << " failed!\n";
    if ( errno == EINVAL )
      ol.addError( DaqError::InvalidSampleRate );
    else
      ol.addErrorStr( errno );
    return -1;
  }
  BufferSize = syncCmdIOC.buffersize;

  if ( ! ol.success() )
    return -1;

  for ( int k=0; k<ol.size(); k++ )
    ol[k].deviceReset( 0 );

  setSettings( ol, BufferSize );

  if ( ! ol.success() )
    return -1;

  Sigs = ol;
  Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!
  
  // set sleep duration:
  setWriteSleep( 5 );

  // fill buffer with initial data:
  int r = writeData();
  if ( r < -1 )
    return -1;

  IsPrepared = Sigs.success();
  NoMoreData = ( r == -1 );

  return 0;
}


int DynClampAnalogOutput::startWrite( QSemaphore *sp )
{
  QMutexLocker locker( mutex() );

  if ( ModuleFd < 0 || !IsPrepared || Sigs.empty() ) {
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
    cerr << "DynClampAnalogOutput::testWriteDevice -> ioctl command IOC_GETRATE on device "
	 << ModuleDevice << " failed!\n";
  }
  else
    Sigs.setSampleRate( (double)rate );

  startThread( sp );

  return NoMoreData ? 0 : 1;
}


int DynClampAnalogOutput::writeData( void )
{
  //  cerr << "DynClampAnalogOutput::writeData(): in\n";/////TEST/////
  if ( Sigs.empty() )
    return -2;

  // device stopped or error?
  if ( IsPrepared ) {
    int running = SUBDEV_OUT;
    int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
    if ( retval < 0 ) {
      //    cerr << "DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
      //	 << ModuleDevice << " failed!\n";
      return -2;
    }
    if ( running <= 0 ) {
      if ( running == E_UNDERRUN )
	Sigs.addError( DaqError::OverflowUnderrun );
      else if ( running < 0 )
	Sigs.addError( DaqError::DeviceError );
      else
	Sigs.addErrorStr( "DynClampAnalogOutput::writeData: " +
			  deviceFile() + " is not running!" );
      setErrorStr( Sigs );
      return -2;
    }
  }

  if ( Sigs[0].deviceWriting() ) {
    // multiplex data into buffer:
    float *bp = (float*)(Buffer+NBuffer);
    int maxn = (BufferSize-NBuffer)/BufferElemSize/Sigs.size();
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
    bytesConverted *= BufferElemSize;
    NBuffer += bytesConverted;
  }

  //  if ( ! Sigs[0].deviceWriting() && NBuffer == 0 )
  if ( NBuffer == 0 )
    return -1;

  // transfer buffer to kernel modul:
  int bytesWritten = ::write( ModuleFd, Buffer, NBuffer );
    
  int ern = 0;
  int datams = 0;
    
  if ( bytesWritten < 0 ) {
    ern = errno;
    if ( ern == EAGAIN || ern == EINTR )
      ern = 0;
  }
  else if ( bytesWritten > 0 ) {
    memmove( Buffer, Buffer+bytesWritten, NBuffer-bytesWritten );
    NBuffer -= bytesWritten;
    datams = (int)::floor( 1000.0*Sigs[0].interval( bytesWritten / BufferElemSize / Sigs.size() ) );
  }

  if ( ern == 0 ) {
    // no more data:
    if ( ! Sigs[0].deviceWriting() && NBuffer <= 0 ) {
      if ( Buffer != 0 )
	delete [] Buffer;
      Buffer = 0;
      BufferSize = 0;
      NBuffer = 0;
      return -1;
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
    return -2;
  }

  return datams;
}


int DynClampAnalogOutput::stop( void )
{ 
  int running = 0;
  {
    QMutexLocker locker( mutex() );

    if ( ModuleFd < 0 || !IsPrepared )
      return 0;

    running = SUBDEV_OUT;
    int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
    if ( retval < 0 ) {
      //    cerr << "DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
      //	 << ModuleDevice << " failed!\n";
      return -1;
    }
  }

  if ( running > 0 ) {
    stopWrite();
    QMutexLocker locker( mutex() );
    int retval = ::ioctl( ModuleFd, IOC_STOP_SUBDEV, SUBDEV_OUT );
    if ( retval < 0 ) {
      cerr << "DynClampAnalogOutput::stop -> ioctl command IOC_STOP_SUBDEV on device "
	   << ModuleDevice << " failed!\n";
      return -1;
    }
  }

  return 0;
}


int DynClampAnalogOutput::reset( void )
{ 
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

  return 0;
}


AnalogOutput::Status DynClampAnalogOutput::statusUnlocked( void ) const
{
  if ( ModuleFd < 0 || !IsPrepared )
    return Idle;

  int running = SUBDEV_OUT;
  int retval = ::ioctl( ModuleFd, IOC_CHK_RUNNING, &running );
  if ( retval < 0 ) {
    cerr << "DynClampAnalogOutput::running -> ioctl command IOC_CHK_RUNNING on device "
	 << ModuleDevice << " failed!\n";
    return UnknownError;
  }
  //  cerr << "DynClampAnalogOutput::running returned " << running << '\n';

  if ( running > 0 )
    return Running;
  else if ( running == 0 )
    return Idle;
  else if ( running == E_UNDERRUN )
    return Idle;
  else
    return UnknownError;
}


long DynClampAnalogOutput::index( void ) const
{
  if ( ModuleFd < 0 )
    return -1;

  long index = 0;
  int retval = ::ioctl( ModuleFd, IOC_GETAOINDEX, &index );
  //    cerr << "DynClampAnalogOutput::index() -> " << index << '\n';
  if ( retval < 0 ) {
    cerr << "DynClampAnalogOutput::index() -> ioctl command IOC_GETLOOPCNT on device "
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
  if ( ModuleFd < 0 )
    return;

  struct traceInfoIOCT traceInfo;
  traceInfo.traceType = PARAM_OUT;
  int channel = PARAM_CHAN_OFFSET;
  while ( 0 == ::ioctl( ModuleFd, IOC_GET_TRACE_INFO, &traceInfo ) ) {
    traces.push_back( TraceSpec( traces.size(), traceInfo.name,
				 deviceid, channel++, 1.0, traceInfo.unit, traceInfo.value ) );
  }
  int ern = errno;
  if ( ern != ERANGE )
    cerr << "DynClampAnalogOutput::addTraces() -> errno " << strerror( errno ) <<  '\n';

  // load all channels to the kernel:
  OutList sigs;
  for ( unsigned int k=0; k<traces.size(); k++ ) {
    if ( traces[k].device() == deviceid ) {
      OutData signal;
      signal.setTrace( traces[k].trace() );
      traces[k].apply( signal );
      signal.resize( 1, 0.0, signal.minSampleInterval() );
      signal = 0.0;
      sigs.push( signal );
    }
  }
  loadChanList( sigs, 0 );
}


int DynClampAnalogOutput::matchTraces( vector< TraceSpec > &traces ) const
{
  if ( ModuleFd < 0 )
    return -1;

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
	if ( traces[k].channel() >= PARAM_CHAN_OFFSET ) {
	  failed = true;
	  cerr << "! DynClampAnalogOutput::matchTraces -> output trace " << traces[k].traceName() << " matches model output, but its channel number " << traces[k].channel() << " is to large\n";
	  //	  traces[k].addErrorStr( "output trace " + traces[k].traceName() + " matches model output, but its channel number " << traces[k].channel() << " is to large" );
	}
	if ( traces[k].unit() != traceInfo.unit ) {
	  failed = true;
	  cerr << "! DynClampAnalogOutput::matchTraces -> output trace " << traces[k].traceName() << " requires as unit '" << traceInfo.unit << "', not '" << traces[k].unit() << "'\n";
	  //	  traces[k].addErrorStr( "output trace " + traces[k].traceName() + " requires as unit '" + traceInfo.unit + "', not '" + traces[k].unit() + "'" );
	}
	traceChannel.channel = traces[k].channel();
	if ( ::ioctl( ModuleFd, IOC_SET_TRACE_CHANNEL, &traceChannel ) != 0 ) {
	  failed = true;
	  cerr << "! DynClampAnalogOutput::matchTraces -> failed to pass channel information to model output trace -> errno=" << errno << '\n';
	  //	  traces[k].addErrorStr( "failed to pass channel information to model output trace -> errno=" + Str( errno ) );
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
