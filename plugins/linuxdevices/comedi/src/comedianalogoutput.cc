/*
  comedi/comedianalogoutput.cc
  Interface for accessing analog output of a daq-board via comedi.

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

#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <QMutexLocker>
#include <relacs/str.h>
#include <relacs/comedi/comedianaloginput.h>
#include <relacs/comedi/comedianalogoutput.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediAnalogOutput::ComediAnalogOutput( void ) 
  : AnalogOutput( "Comedi Analog Output", ComediAnalogIOType )
{
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxData.clear();
  MaxRate = 1000.0;
  UseNIPFIStart = -1;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  FillData = false;
  IsPrepared = false;
  NoMoreData = true;
  Calibration = 0;
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;
  ChannelValues = 0;
  ExtendedData = 0;
  initOptions();
}


ComediAnalogOutput::ComediAnalogOutput(  const string &device,
					 const Options &opts ) 
  : ComediAnalogOutput()
{
  read(opts);
  open( device );
  IsPrepared = false;
  NoMoreData = true;
  Calibration = 0;
  BufferSize = 0;
  Buffer = 0;
  NBuffer = 0;
}


ComediAnalogOutput::~ComediAnalogOutput( void ) 
{
  close();
}

void ComediAnalogOutput::initOptions()
{
  AnalogOutput::initOptions();

  addInteger( "usenipfistart", "Use as start source NI PFI channel", -1 );
  addNumber( "extref", "Voltage of external reference", -1.0, -1.0, 100.0, 0.1, "V" );
  addNumber( "delays", "Delay between analog input and output", 0.0, 0.0, 1.0, 0.0001, "s", "ms" ).setStyle( Parameter::MultipleSelection );
}

int ComediAnalogOutput::open( const string &device )
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

  // lock AI subdevice:
  if ( comedi_lock( DeviceP, SubDevice ) != 0 ) {
    setErrorStr( "locking of analog output subdevice failed on device " + device );
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return NotOpen;
  }  

  // check for async. command support:
  if ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_CMD_WRITE ) == 0 ) {
    setErrorStr( "device "  + device + " does not support async. commands" );
    comedi_unlock( DeviceP,  SubDevice );
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return InvalidDevice;
  }

  // set basic device infos:
  setDeviceName( comedi_get_board_name( DeviceP ) );
  setDeviceVendor( comedi_get_driver_name( DeviceP ) );
  setDeviceFile( device );

  // set size of comedi-internal buffer to maximum:
  int buffersize = comedi_get_max_buffer_size( DeviceP, SubDevice );
  comedi_set_buffer_size( DeviceP, SubDevice, buffersize );

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

  // get size of datatype for sample values:
  LongSampleType = ( comedi_get_subdevice_flags( DeviceP, SubDevice ) &
		     SDF_LSAMPL );
  if ( LongSampleType )
    BufferElemSize = sizeof( lsampl_t );
  else
    BufferElemSize = sizeof( sampl_t );
  MaxData.clear();
  for ( int k=0; k<channels(); k++ )
    MaxData.push_back( comedi_get_maxdata( DeviceP, SubDevice, k ) );

  // set and write default output values for channels:
  ChannelValues = new float[channels()];
  for ( int k=0; k<channels(); k++ )
    ChannelValues[k] = 0.0;
  writeZeros();

  // try to find out the maximum sampling rate:
  comedi_cmd cmd;
  memset( &cmd,0, sizeof(comedi_cmd) );
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice, &cmd,
					     1 /*chans*/, 1 /*ns*/ );
  if ( retVal < 0 ){
    setErrorStr( "cannot get maximum sampling rate from comedi_get_cmd_generic_timed(): " +
		 string( comedi_strerror( comedi_errno() ) ) );
    close();
    return -1;
  }
  else
    MaxRate = 1.0e9 / cmd.scan_begin_arg;

  UseNIPFIStart = integer( "usenipfistart" );

  // delays:
  vector< double > delays;
  numbers( "delays", delays, "s" );
  setDelays( delays );

  // clear flags:
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  NoMoreData = true;

  setInfo();
 
  return 0;
}


bool ComediAnalogOutput::isOpen( void ) const 
{ 
  lock();
  bool o = ( DeviceP != NULL );
  unlock();
  return o;
}


void ComediAnalogOutput::close( void )
{ 
  clearError();
  if ( ! isOpen() )
    return;

  reset();

  writeZeros();

  // clean up stored channel values:
  if ( ChannelValues != 0 )
    delete[] ChannelValues;
  ChannelValues = 0;

  // cleanup calibration:
  if ( Calibration != 0 )
    comedi_cleanup_calibration( Calibration );
  Calibration = 0;

  // unlock:
  int error = comedi_unlock( DeviceP, SubDevice );
  if ( error < 0 )
    setErrorStr( "unlocking of AO subdevice on device " + deviceFile() + "failed" );
  
  // close:
  error = comedi_close( DeviceP );
  if ( error )
    setErrorStr( "closing of AO subdevice on device " + deviceFile() + "failed" );

  // clear flags:
  DeviceP = NULL;
  SubDevice = 0;
  Info.clear();
}


void ComediAnalogOutput::writeZeros( void )
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


int ComediAnalogOutput::channels( void ) const
{ 
  if ( !isOpen() )
    return -1;
  lock();
  int n = comedi_get_n_channels( DeviceP, SubDevice );
  unlock();
  return n;
}


int ComediAnalogOutput::bits( void ) const
{ 
  if ( !isOpen() )
    return -1;
  lock();
  int maxData = comedi_get_maxdata( DeviceP, SubDevice, 0 );
  unlock();
  return (int)( log( maxData+2.0 )/ log( 2.0 ) );
}


double ComediAnalogOutput::maxRate( void ) const 
{ 
  return MaxRate;
}


int ComediAnalogOutput::maxRanges( void ) const
{
  return UnipolarRangeIndex.size() > BipolarRangeIndex.size() ?
    UnipolarRangeIndex.size() : BipolarRangeIndex.size();
}


double ComediAnalogOutput::unipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)UnipolarRangeIndex.size()) )
    return -1.0;
  return UnipolarRange[index].max;
}


double ComediAnalogOutput::bipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)BipolarRangeIndex.size()) )
    return -1.0;
  return BipolarRange[index].max;
}


int ComediAnalogOutput::directWrite( OutList &sigs )
{
  // no signals:
  if ( sigs.size() == 0 )
    return -1;

  // not open:
  if ( !isOpen() )
    return -1;

  // setup channel ranges:
  unsigned int *chanlist = new unsigned int[512];
  memset( chanlist, 0, sizeof( *chanlist ) );
  setupChanList( sigs, chanlist, 512, true );

  if ( sigs.failed() )
    return -1;

  QMutexLocker locker( mutex() );

  for ( int k=0; k<sigs.size(); k++ ) {

    // get range values:
    double minval = sigs[k].minValue();
    double maxval = sigs[k].maxValue();
    double scale = sigs[k].scale();
    const comedi_polynomial_t * polynomial = (const comedi_polynomial_t *)sigs[k].gainData();

    // apply range:
    float v = sigs[k].size() > 0 ? sigs[k][0] : 0.0;
    if ( v > maxval )
      v = maxval;
    else if ( v < minval ) 
      v = minval;
    v *= scale;
    lsampl_t data = comedi_from_physical( v, polynomial );
    if ( data > MaxData[sigs[k].channel()] )
      data = MaxData[sigs[k].channel()];

    // write data:
    int retval = comedi_data_write( DeviceP, SubDevice, CR_CHAN( chanlist[k] ),
				    CR_RANGE( chanlist[k] ), CR_AREF( chanlist[k] ), data );
    if ( retval < 1 ) {
      string emsg = "comedi_direct_write failed: ";
      emsg += comedi_strerror( comedi_errno() );
      sigs[k].addErrorStr( emsg );
    }
    else
      ChannelValues[sigs[k].channel()] = sigs[k].size() > 0 ? sigs[k][0] : 0.0;

  }

  IsPrepared = false;

  return ( sigs.success() ? 0 : -1 );
}


template < typename T >
int ComediAnalogOutput::convert( char *cbuffer, int nbuffer )
{
  if ( nbuffer < (int)sizeof( T ) )
    return 0;

  // conversion polynomials and scale factors:
  double minval[ Sigs.size() ];
  double maxval[ Sigs.size() ];
  T maxdata[ Sigs.size() ];
  double scale[ Sigs.size() ];
  const comedi_polynomial_t* polynomial[Sigs.size()];
  T zeros[ Sigs.size() ];
  for ( int k=0; k<Sigs.size(); k++ ) {
    minval[k] = Sigs[k].minValue();
    maxval[k] = Sigs[k].maxValue();
    maxdata[k] = MaxData[Sigs[k].channel()];
    scale[k] = Sigs[k].scale();
    polynomial[k] = (const comedi_polynomial_t *)Sigs[k].gainData();
    float v = ChannelValues[Sigs[k].channel()];
    if ( v > maxval[k] )
      v = maxval[k];
    else if ( v < minval[k] ) 
      v = minval[k];
    v *= scale[k];
    zeros[k] = comedi_from_physical( v, polynomial[k] );
    if ( zeros[k] > maxdata[k] )
      zeros[k] = maxdata[k];
  }

  // buffer pointer:
  T *bp = (T*)cbuffer;
  int maxn = nbuffer/sizeof( T )/Sigs.size();
  int n = 0;

  // convert data and multiplex into buffer:
  for ( int i=0; i<maxn && Sigs[0].deviceWriting(); i++ ) {
    for ( int k=0; k<Sigs.size(); k++ ) {
      if ( Sigs[k].deviceCount() < 0 ) {
	*bp = zeros[k];
	Sigs[k].incrDeviceIndex();
	if ( Sigs[k].deviceIndex() >= Sigs[k].deviceDelay() )
	  Sigs[k].incrDeviceCount();
      }
      else {
	float v = Sigs[k].deviceValue();
	if ( v > maxval[k] )
	  v = maxval[k];
	else if ( v < minval[k] ) 
	  v = minval[k];
	v *= scale[k];
	T d = comedi_from_physical( v, polynomial[k] );
	if ( d > maxdata[k] )
	  d = maxdata[k];
	*bp = d;	
	if ( Sigs[k].deviceIndex() >= Sigs[k].size() )
	  Sigs[k].incrDeviceCount();
      }
      ++bp;
      ++n;
    }
  }

  // memorize last values:
  for ( int k=0; k<Sigs.size(); k++ ) {
    if ( Sigs[k].deviceCount() >= 0 && Sigs[k].deviceIndex() > 0 )
      ChannelValues[Sigs[k].channel()] = Sigs[k][Sigs[k].deviceIndex()-1];
    else if ( Sigs[k].deviceCount() > 0 && Sigs[k].deviceIndex() == 0 )
      ChannelValues[Sigs[k].channel()] = Sigs[k].back();
  }

  return n * sizeof( T );
}


void ComediAnalogOutput::setupChanList( OutList &sigs, unsigned int *chanlist,
					int maxchanlist, bool setscale )
{
  bool softcal = ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) &
		     SDF_SOFT_CALIBRATED ) > 0 );
  
  int aref = AREF_GROUND;
  for ( int k=0; k<sigs.size() && k<maxchanlist; k++ ) {

    // check channel:
    int maxchannels = comedi_get_n_channels( DeviceP, SubDevice );
    if ( sigs[k].channel() < 0 || sigs[k].channel() >= maxchannels ) {
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
    if ( fabs(min) > fabs(max) && min >= 0.0 )
      unipolar = true;
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

    /*
    cerr << "UNIPOLAR=" << unipolar << '\n';
    cerr << "INDEX=" << index << '\n';
    cerr << "BIPOLARRANGE.SIZE=" << BipolarRange.size() << '\n';
    cerr << "UNIPOLARRANGE.SIZE=" << UnipolarRange.size() << '\n';
    cerr << "MAXBOARDVOLT=" << maxboardvolt << '\n';
    for ( unsigned int j=0; j<BipolarRange.size(); j++ ) {
      cerr << "BIPOLARRANGEMAX" << j << "=" << BipolarRange[j].max << '\n';
      cerr << "BIPOLARRANGEMIN" << j << "=" << BipolarRange[j].min << '\n';
    }
    */

    if ( !sigs[k].noLevel() && setscale )
      sigs[k].multiplyScale( maxboardvolt );

    if ( softcal && Calibration != 0 )
      comedi_get_softcal_converter( SubDevice, sigs[k].channel(),
				    unipolar ? UnipolarRangeIndex[ index ] : BipolarRangeIndex[ index ],
				    COMEDI_FROM_PHYSICAL, Calibration, gainp );
    else
      comedi_get_hardcal_converter( DeviceP, SubDevice, sigs[k].channel(),
				    unipolar ? UnipolarRangeIndex[ index ] : BipolarRangeIndex[ index ],
				    COMEDI_FROM_PHYSICAL, gainp );

    sigs[k].setMinVoltage( minboardvolt );
    sigs[k].setMaxVoltage( maxboardvolt );

    // set up channel in chanlist:
    if ( unipolar )
      chanlist[k] = CR_PACK( sigs[k].channel(),
			     UnipolarRangeIndex[index], aref );
    else
      chanlist[k] = CR_PACK( sigs[k].channel(),
			     BipolarRangeIndex[index], aref );
  }
}


int ComediAnalogOutput::setupCommand( OutList &sigs, comedi_cmd &cmd, bool setscale )
{
  // channels:
  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;
  unsigned int *chanlist = new unsigned int[512];
  memset( chanlist, 0, sizeof( *chanlist ) );
  memset( &cmd, 0, sizeof( comedi_cmd ) );

  setupChanList( sigs, chanlist, 512, setscale );

  if ( sigs.failed() )
    return -1;
  
  // try automatic command generation:
  cmd.scan_begin_src = TRIG_TIMER;
  //  cmd.flags = TRIG_ROUND_NEAREST | TRIG_WRITE;
  //  cmd.flags = TRIG_ROUND_NEAREST | TRIG_WRITE | TRIG_WAKE_EOS;
  unsigned int period = (int)( 1e9 * sigs[0].sampleInterval() );
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice, &cmd,
					     sigs.size(), period );
  if ( retVal < 0 ) {
    string emsg = "comedi_get_cmd_generic_timed failed: ";
    emsg += comedi_strerror( comedi_errno() );
    sigs.addErrorStr( emsg );
    cerr << "! error in ComediAnalogOutput::setupCommand -> comedi_get_cmd_generic_timed failed: "
	 << comedi_strerror( comedi_errno() ) << endl;
    return -1;
  }
  if ( cmd.scan_begin_src != TRIG_TIMER ) {
    sigs.addErrorStr( "acquisition timed by a daq-board counter not possible" );
    return -1;
  }
  cmd.scan_begin_arg = period;

  // adapt command to our purpose:
  comedi_cmd testCmd;
  comedi_get_cmd_src_mask( DeviceP, SubDevice, &testCmd );
  if ( UseNIPFIStart >= 0 ) {
    if ( testCmd.start_src & TRIG_EXT )
      cmd.start_src = TRIG_EXT;
    else {
      sigs.addError( DaqError::InvalidStartSource );
      sigs.addErrorStr( "External trigger not supported" );
    }
  }
  else {
    if ( testCmd.start_src & TRIG_INT )
      cmd.start_src = TRIG_INT;
    else {
      sigs.addError( DaqError::InvalidStartSource );
      sigs.addErrorStr( "Internal trigger not supported" );
    }
  }
  cmd.start_arg = 0;
  if ( UseNIPFIStart >= 0 ) {
    // cmd.start_arg = CR_EDGE | NI_USUAL_PFI_SELECT( UseNIPFIStart );
    cmd.start_arg = CR_EDGE | UseNIPFIStart;  // in ni_mio_common.cc this is incremented by one!
    // cmd.start_arg = 18;  // that should be AI_START1 !!!!
    cerr << "START_SRC = " << cmd.start_src << " START_ARG = " << cmd.start_arg << " PFI " << UseNIPFIStart << '\n';
  }
  cmd.scan_end_arg = sigs.size();
  
  // test if continous-state is supported:
  if ( sigs[0].continuous() && !(testCmd.stop_src & TRIG_NONE) ) {
    cerr << "! warning ComediAnalogOutput::setupCommand(): "
	 << "continuous mode not supported!" << endl;/////TEST/////
    sigs.addError( DaqError::InvalidContinuous );
    sigs.setContinuous( false );
  }
  if ( !sigs[0].continuous() && !(testCmd.stop_src & TRIG_COUNT) ) {
    cerr << "! warning ComediAnalogOutput::setupCommand(): "
	 << "only continuous mode supported!" << endl;/////TEST/////
    sigs.addError( DaqError::InvalidContinuous );
    sigs.setContinuous( true );
  }
    
  // set countinous-state
  if ( sigs[0].continuous() ) {
    cmd.stop_src = TRIG_NONE;
    cmd.stop_arg = 0;
  }
  if ( !sigs[0].continuous() ) {
    cmd.stop_src = TRIG_COUNT;
    // set length of acquisition as number of scans:
    cmd.stop_arg = sigs[0].size() + sigs[0].indices( sigs[0].delay() ) + ExtendedData;
    if ( deviceName() == "pci-6052e" )
      cmd.stop_arg -= 1; // XXX pci-6052e (all NI E Series ?) - comedi-bug? 
  }

  cmd.chanlist = chanlist;
  cmd.chanlist_len = sigs.size();

  // test command:
  memcpy( &testCmd, &cmd, sizeof( comedi_cmd ) ); // store original state
  for ( int k=0; k<=5; k++ ) {
    retVal = comedi_command_test( DeviceP, &cmd );
    if ( retVal == 0 )
      break;
    switch ( retVal ) {
    case 1: // unsupported trigger in *_src:
      if ( cmd.start_src != testCmd.start_src )
	sigs.addErrorStr( "unsupported trigger " + Str(testCmd.start_src) + " in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	sigs.addErrorStr( "unsupported trigger " + Str(testCmd.scan_begin_src) + " in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	sigs.addErrorStr( "unsupported trigger " + Str(testCmd.convert_src) + " in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	sigs.addErrorStr( "unsupported trigger " + Str(testCmd.scan_end_arg) + " in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	sigs.addErrorStr( "unsupported trigger " + Str(testCmd.stop_src) + " in stop_src" );
      break;
    case 2: // invalid trigger in *_src:
      if ( cmd.start_src != testCmd.start_src )
	sigs.addErrorStr( "invalid trigger " + Str(testCmd.start_src) + " in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	sigs.addErrorStr( "invalid trigger " + Str(testCmd.scan_begin_src) + " in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	sigs.addErrorStr( "invalid trigger " + Str(testCmd.convert_src) + " in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	sigs.addErrorStr( "invalid trigger " + Str(testCmd.scan_end_src) + " in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	sigs.addErrorStr( "invalid trigger " + Str(testCmd.stop_src) + " in stop_src" );
      break;
    case 3: // *_arg out of range:
      if ( cmd.start_arg != testCmd.start_arg )
	sigs.addErrorStr( "start_arg " + Str(testCmd.start_arg) + " out of range" );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg ) {
	sigs.addErrorStr( "requested sampling period of " + Str( testCmd.scan_begin_arg )
			  + "ns smaller than supported! min " + Str( cmd.scan_begin_arg )
			  + "ns sampling interval possible." );
	sigs.addError( DaqError::InvalidSampleRate );    
	sigs.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      }
      if ( cmd.convert_arg != testCmd.convert_arg )
	sigs.addErrorStr( "convert_arg " + Str(testCmd.convert_arg) + " out of range" );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	sigs.addErrorStr( "scan_end_arg " + Str(testCmd.scan_end_arg) + " out of range" );
      if ( cmd.stop_arg != testCmd.stop_arg ) {
	// sigs.addErrorStr( "stop_arg " + Str(testCmd.stop_arg) + " out of range" );
	if ( setscale ) {
	  cerr << "stop_arg=" << testCmd.stop_arg << " not supported, switch to continuous mode, extend data by " << ExtendedData << '\n';
	  cmd.stop_src = TRIG_NONE;
	  cmd.stop_arg = 0;
	  ExtendedData = 4*2048;
	}
      }
      break;
    case 4: // adjusted *_arg:
      if ( cmd.start_arg != testCmd.start_arg )
	sigs.addErrorStr( "start_arg adjusted to " + Str(cmd.start_arg) );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg )
	sigs.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      if ( cmd.convert_arg != testCmd.convert_arg )
	sigs.addErrorStr( "convert_arg adjusted to " + Str(cmd.convert_arg) );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	sigs.addErrorStr( "scan_end_arg adjusted to " + Str(cmd.scan_end_arg) );
      if ( cmd.stop_arg != testCmd.stop_arg ) {
	// sigs.addErrorStr( "stop_arg adjusted to " + Str(cmd.stop_arg) );
	if ( setscale ) {
	  cerr << "stop_arg=" << testCmd.stop_arg << " not supported, switch to continuous mode, extend data by " << ExtendedData << '\n';
	  cmd.stop_src = TRIG_NONE;
	  cmd.stop_arg = 0;
	  ExtendedData = 4*2048;
	}
      }
      break;
    case 5: // invalid chanlist:
      for ( int k=0; k<sigs.size(); k++ ) {
	// check channel ordering:
	if ( sigs.size() > 1 ) {
	  vector< unsigned int > chs( sigs.size() );
	  for ( int k=0; k<sigs.size(); k++ )
	    chs[k] = sigs[k].channel();
	  sort( chs.begin(), chs.end() );
	  for ( unsigned int k=0; k<chs.size(); k++ ) {
	    if ( chs[k] != k ) {
	      sigs.addError( DaqError::InvalidChannelSequence );
	      break;
	    }
	  }
	}
      }
      if ( sigs.success() )
	sigs.addErrorStr( "invalid chanlist" );
      break;
    default:
      cerr << "unknown return code from comedi_command_test\n";
    }
  }

  return retVal == 0 ? 0 : -1;
}


int ComediAnalogOutput::testWriteDevice( OutList &sigs )
{
  if ( !isOpen() ) {
    sigs.addError( DaqError::DeviceNotOpen );
    return -1;
  }

  QMutexLocker locker( mutex() );

  comedi_cmd cmd;
  memset( &cmd, 0, sizeof( comedi_cmd ) );
  int retVal = setupCommand( sigs, cmd, false );
  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;

  double buffertime = sigs[0].interval( bufferSize()/sigs.size() );
  if ( buffertime < 0.001 ) {
    sigs.addError( DaqError::InvalidBufferTime );
    retVal = -1;
  }

  return retVal;
}


int ComediAnalogOutput::prepareWrite( OutList &sigs )
{
  if ( !isOpen() ) {
    sigs.addError( DaqError::DeviceNotOpen );
    return -1;
  }

  // comedi_cancel is needed to clear the BUSY state of the subdevice!
  reset();

  // no signals:
  if ( sigs.size() == 0 )
    return -1;

  QMutexLocker locker( mutex() );

  ExtendedData = 0;

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  if ( deviceVendor() == "ni_mio_cs" ) {
    // Fix DAQCard bug: add 2k of zeros to the signals:
    ExtendedData = 2048;
  }

  if ( setupCommand( ol, Cmd, true ) < 0 ) {
    ExtendedData = 0;
    if ( Cmd.chanlist != 0 )
      delete [] Cmd.chanlist;
    memset( &Cmd, 0, sizeof( comedi_cmd ) );
    return -1;
  }

  if ( ExtendedData > 0 ) {
    // contiunous and DAQCard bug:
    for ( int k=0; k<ol.size(); k++ )
      ol[k].SampleDataF::append( ol[k].back(), ExtendedData );
  }

  // apply calibration:
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

  if ( ! ol.success() )
    return -1;

  int delayinx = ol[0].indices( ol[0].delay() );
  ol.deviceReset( delayinx );

  // set buffer size:
  BufferSize = bufferSize()*BufferElemSize;
  int nbuffer = sigs.deviceBufferSize()*BufferElemSize;
  if ( nbuffer < BufferSize )
    BufferSize = nbuffer;

  setSettings( ol, BufferSize );

  if ( ! ol.success() )
    return -1;

  Sigs = ol;
  Buffer = new char[ BufferSize ];  // Buffer was deleted in reset()!

  // execute command:
  cerr << "EXECUTE START_ARG = " << Cmd.start_arg << " PFI " << UseNIPFIStart << '\n';
  //ComediAnalogInput::dump_cmd( &Cmd );
  if ( comedi_command( DeviceP, &Cmd ) < 0 ) {
    int cerror = comedi_errno();
    cerr << "AO command failed: " << comedi_strerror( cerror ) << endl;
    ol.addErrorStr( deviceFile() + " - execution of comedi_cmd for analog output failed: "
		    + comedi_strerror( cerror ) );
    return -1;
  }

  // msleep( 1 ); ???

  // fill buffer with initial data:
  FillData = true;
  int r = writeData();
  FillData = false;
  if ( r < -1 )
    return -1;

  IsPrepared = Sigs.success();
  NoMoreData = ( r == -1 );

  return 0;
}


bool ComediAnalogOutput::noMoreData( void ) const
{
  lock();
  bool nmd = NoMoreData;
  unlock();
  return nmd;
}


int ComediAnalogOutput::startWrite( QSemaphore *sp )
{
  QMutexLocker locker( mutex() );

  if ( ! IsPrepared || Sigs.empty() ) {
    cerr << "AO not prepared or no signals!\n";
    return -1;
  }

  // setup instruction:
  lsampl_t insndata[1];
  insndata[0] = 0;
  comedi_insn insn;
  insn.insn = INSN_INTTRIG;
  insn.subdev = SubDevice;
  insn.chanspec = 0;
  insn.data = insndata;
  insn.n = 1;
  int r = comedi_do_insn( DeviceP, &insn );
  if ( r < 0 ) {
    int cerror = comedi_errno();
    cerr << "AO do_insn failed: " << comedi_strerror( cerror ) << endl;
    Sigs.addErrorStr( deviceFile() + " - execution of comedi_do_insn failed: "
		      + comedi_strerror( cerror ) );
    return -1;
  }

  startThread( sp );

  return NoMoreData ? 0 : 1;
}


int ComediAnalogOutput::writeData( void )
{
  if ( Sigs.empty() )
    return -2;
 
  // device not running anymore, but was not stopped?
  if ( ( ! FillData ) && ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING ) == 0 &&
       ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_BUSY ) ) {
    Sigs.addError( DaqError::OverflowUnderrun );
    setErrorStr( Sigs );
    IsPrepared = false;
    return -2;
  }

  if ( Sigs[0].deviceWriting() ) {
    // convert data into buffer:
    int bytesConverted = 0;
    if ( LongSampleType )
      bytesConverted = convert<lsampl_t>( Buffer+NBuffer, BufferSize-NBuffer );
    else  
      bytesConverted = convert<sampl_t>( Buffer+NBuffer, BufferSize-NBuffer );
    NBuffer += bytesConverted;
  }
  
  int datams = 0;
  if ( NBuffer > 0 ) {
    // transfer buffer to comedi:
    int bytesWritten = write( comedi_fileno( DeviceP ), Buffer, NBuffer );
  
    if ( bytesWritten < 0 ) {
      int ern = errno;
      if ( ern != EAGAIN && ern != EINTR ) {
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
	clearBuffers();
	return -2;
      }
    }
    else if ( bytesWritten > 0 ) {
      // update buffer:
      if ( bytesWritten < NBuffer )
	memmove( Buffer, Buffer+bytesWritten, NBuffer-bytesWritten );
      NBuffer -= bytesWritten;
      datams = (int)::floor( 1000.0*Sigs[0].interval( bytesWritten / BufferElemSize / Sigs.size() ) );
    }
  }
  
  // no more data:
  if ( ! Sigs[0].deviceWriting() && NBuffer <= 0 ) {
    clearBuffers();
    return -1;
  }
  
  return datams;
}


int ComediAnalogOutput::stop( void ) 
{ 
  {
    QMutexLocker locker( mutex() );
    if ( (comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING) == 0 )
      return 0;
    if ( comedi_cancel( DeviceP, SubDevice ) < 0 )
      return WriteError;
  }

  stopWrite();

  clearBuffers();
  
  return 0;
}


int ComediAnalogOutput::reset( void ) 
{ 
  lock();

  comedi_cancel( DeviceP, SubDevice );

  clearBuffers();
  Sigs.clear();
  Settings.clear();
  unlock();
  return 0;
}


void ComediAnalogOutput::clearBuffers( void ) 
{ 
  if ( ExtendedData > 0 ) {
    for ( int k=0; k<Sigs.size(); k++ )
      Sigs[k].resize( Sigs[k].size()-ExtendedData );
    ExtendedData = 0;
  }

  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = 0;
  BufferSize = 0;
  NBuffer = 0;

  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );

  IsPrepared = false;
  NoMoreData = true;
}


AnalogOutput::Status ComediAnalogOutput::statusUnlocked( void ) const
{   
  Status r = Idle;
  // Actually we should check for BUSY, but this is not cleared at the end of the command!
  if ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING ) {
    if ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_BUSY )
      r = Running;
    else {
      Sigs.addError( DaqError::OverflowUnderrun );
      r = Underrun;
    }
  }
  return r;
}


int ComediAnalogOutput::comediSubdevice( void ) const
{
  if ( DeviceP == NULL )
    return -1;
  return SubDevice;
}


bool ComediAnalogOutput::useAIStart( void ) const
{
  return ( UseNIPFIStart >= 0 );
}


int ComediAnalogOutput::bufferSize( void ) const
{
  if ( DeviceP == NULL )
    return -1;
  int n = comedi_get_buffer_size( DeviceP, SubDevice ) / BufferElemSize;
  return n;
}


bool ComediAnalogOutput::prepared( void ) const 
{ 
  lock();
  bool ip = IsPrepared;
  unlock();
  return ip;
}


}; /* namespace comedi */
