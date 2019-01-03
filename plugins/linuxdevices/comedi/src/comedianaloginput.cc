/*
  comedi/comedianaloginput.cc
  Interface for accessing analog input of a daq-board via comedi.

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
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <QMutexLocker>
#include <relacs/str.h>
#include <relacs/comedi/comedianalogoutput.h>
#include <relacs/comedi/comedianaloginput.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediAnalogInput::ComediAnalogInput( void ) 
  : AnalogInput( "ComediAnalogInput", ComediAnalogIOType )
{
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxRate = 1000.0;
  TakeAO = true;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  AboutToStop = false;
  Calibration = 0;
  Traces = 0;
  ReadBufferSize = 0;
  BufferSize = 0;
  BufferN = 0;
  Buffer = NULL;
  TraceIndex = 0;
  TotalSamples = 0;
  CurrentSamples = 0;

  initOptions();
}


ComediAnalogInput::ComediAnalogInput( const string &device, const Options &opts ) 
  : ComediAnalogInput()
{
  read(opts);
  open( device );
}

  
ComediAnalogInput::~ComediAnalogInput( void ) 
{
  close();
}

void ComediAnalogInput::initOptions()
{
  AnalogInput::initOptions();

  addNumber( "gainblacklist", "Ranges not to be used", 0.0, 0.0, 100.0, 0.1, "V" ).setStyle( Parameter::MultipleSelection );
  addBoolean( "takeao", "Start analog output in a single instruction", true );
}

int ComediAnalogInput::open( const string &device )
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

  // get AI subdevice:
  int subdev = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_AI, 0 );
  if ( subdev < 0 ) {
    setErrorStr( "device "  + device + " does not support analog input" );
    comedi_close( DeviceP );
    DeviceP = NULL;
    return InvalidDevice;
  }
  SubDevice = subdev;

  // lock AI subdevice:
  if ( comedi_lock( DeviceP, SubDevice ) != 0 ) {
    setErrorStr( "locking of analog input subdevice failed on device " + device );
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return NotOpen;
  }  

  // check for async. command support:
  if ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_CMD_READ ) == 0 ) {
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
  ReadBufferSize = comedi_get_max_buffer_size( DeviceP, SubDevice );
  comedi_set_buffer_size( DeviceP, SubDevice, ReadBufferSize );
  ReadBufferSize = comedi_get_buffer_size( DeviceP, SubDevice );

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
  numbers( "gainblacklist", gainblacklist );
  // XXX: if a range is not supported but comedi thinks so: add max gain to the blacklist.
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

  // get size of datatype for sample values:
  LongSampleType = ( comedi_get_subdevice_flags( DeviceP, SubDevice ) &
		     SDF_LSAMPL );
  if ( LongSampleType )
    BufferElemSize = sizeof( lsampl_t );
  else
    BufferElemSize = sizeof( sampl_t );

  // try to find out the maximum sampling rate:
  comedi_cmd cmd;
  memset( &cmd, 0, sizeof(comedi_cmd) );
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice, &cmd,
					     1 /*chans*/, 1 /*ns*/ );
  if ( retVal < 0 ) {
    setErrorStr( "cannot get maximum sampling rate from comedi_get_cmd_generic_timed(): " +
		 string( comedi_strerror( comedi_errno() ) ) );
    close();
    return -1;
  }
  else
    MaxRate = 1.0e9 / cmd.scan_begin_arg;

  // For debugging:
  TakeAO = boolean( "takeao", true );

  // clear flags:
  ComediAO = 0;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  AboutToStop = false;
  TotalSamples = 0;
  CurrentSamples = 0;

  setInfo();
  
  return 0;
}


bool ComediAnalogInput::isOpen( void ) const 
{ 
  lock();
  bool o = ( DeviceP != NULL );
  unlock();
  return o;
}


void ComediAnalogInput::close( void ) 
{
  clearError();
  if ( ! isOpen() )
    return;

  reset();

  // cleanup calibration:
  if ( Calibration != 0 )
    comedi_cleanup_calibration( Calibration );
  Calibration = 0;

  // unlock:
  int error = comedi_unlock( DeviceP,  SubDevice );
  if ( error < 0 )
    setErrorStr( "unlocking of AI subdevice on device " + deviceFile() + "failed" );

  // close:
  error = comedi_close( DeviceP );
  if ( error )
    setErrorStr( "closing of AI subdevice on device " + deviceFile() + "failed" );

  // clear flags:
  DeviceP = NULL;
  SubDevice = 0;
  ComediAO = 0;
  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  TraceIndex = 0;
  TotalSamples = 0;
  CurrentSamples = 0;
  Info.clear();
  TakeAO = true;
}


int ComediAnalogInput::channels( void ) const
{ 
  if ( !isOpen() )
    return -1;
  lock();
  int n = comedi_get_n_channels( DeviceP, SubDevice );
  unlock();
  return n;
}


int ComediAnalogInput::bits( void ) const
{ 
  if ( !isOpen() )
    return -1;
  lock();
  int maxData = comedi_get_maxdata( DeviceP, SubDevice, 0 );
  unlock();
  return (int)( log( maxData+2.0 )/ log( 2.0 ) );
}


double ComediAnalogInput::maxRate( void ) const 
{ 
  return MaxRate;
}


int ComediAnalogInput::maxRanges( void ) const
{
  return UnipolarRangeIndex.size() > BipolarRangeIndex.size() ?
    UnipolarRangeIndex.size() : BipolarRangeIndex.size();
}


double ComediAnalogInput::unipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)UnipolarRangeIndex.size()) )
    return -1.0;
  return UnipolarRange[index].max;
}

  
double ComediAnalogInput::bipolarRange( int index ) const
{
  if ( (index < 0) || (index >= (int)BipolarRangeIndex.size()) )
    return -1.0;
  return BipolarRange[index].max;
}
  

string ComediAnalogInput::cmd_src( int src )
{
  string buf = "";

  if ( src & TRIG_NONE ) buf += "none|";
  if ( src & TRIG_NOW ) buf += "now|";
  if ( src & TRIG_FOLLOW ) buf += "follow|";
  if ( src & TRIG_TIME ) buf += "time|";
  if ( src & TRIG_TIMER ) buf += "timer|";
  if ( src & TRIG_COUNT ) buf += "count|";
  if ( src & TRIG_EXT ) buf += "ext|";
  if ( src & TRIG_INT ) buf += "int|";
#ifdef TRIG_OTHER
  if ( src & TRIG_OTHER ) buf +=  "other|";
#endif

  if ( buf.empty() )
    //    buf = "unknown(" << setw( 8 ) << src ")";
    buf="unknown";
  else
    buf.erase( buf.size()-1 );
  
  return buf;
}


void ComediAnalogInput::dump_cmd( comedi_cmd *cmd )
{
  cerr << "subdevice:      " << cmd->subdev << '\n';
  cerr << "start:      " << Str( cmd_src(cmd->start_src), 8 ) << "  " << cmd->start_arg << '\n';
  cerr << "scan_begin: " << Str( cmd_src(cmd->scan_begin_src), 8 ) << "  " << cmd->scan_begin_arg << '\n';
  cerr << "convert:    " << Str( cmd_src(cmd->convert_src), 8 ) << "  " << cmd->convert_arg << '\n';
  cerr << "scan_end:   " << Str( cmd_src(cmd->scan_end_src), 8 ) << "  " << cmd->scan_end_arg << '\n';
  cerr << "stop:       " << Str( cmd_src(cmd->stop_src), 8 ) << "  " << cmd->stop_arg << '\n';
}

  
int ComediAnalogInput::setupCommand( InList &traces, comedi_cmd &cmd )
{
  // channels:
  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;
  unsigned int *chanlist = new unsigned int[512];
  memset( chanlist, 0, sizeof( *chanlist ) );
  memset( &cmd, 0, sizeof( comedi_cmd ) );

  bool softcal = ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) &
		     SDF_SOFT_CALIBRATED ) > 0 );

  for( int k = 0; k < traces.size(); k++ ) {

    // delay:
    if ( traces[k].delay() > 0.0 ) {
      traces[k].addError( DaqError::InvalidDelay );
      traces[k].addErrorStr( "delays are not supported by comedi!" );
      traces[k].setDelay( 0.0 );
    }

    // reference:
    int aref = -1;
    int subdeviceflags = comedi_get_subdevice_flags( DeviceP, SubDevice );
    switch ( traces[k].reference() ) {
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
      if ( softcal && Calibration != 0 )
	comedi_get_softcal_converter( SubDevice, traces[k].channel(),
				      UnipolarRangeIndex[ traces[k].gainIndex() ],
				      COMEDI_TO_PHYSICAL, Calibration, gainp );
      else
	comedi_get_hardcal_converter( DeviceP, SubDevice, traces[k].channel(),
				      UnipolarRangeIndex[ traces[k].gainIndex() ],
				      COMEDI_TO_PHYSICAL, gainp );
      chanlist[k] = CR_PACK( traces[k].channel(), 
			     UnipolarRangeIndex[ traces[k].gainIndex() ],
			     aref );
    }
    else {
      double max = BipolarRange[traces[k].gainIndex()].max;
      double min = BipolarRange[traces[k].gainIndex()].min;
      if ( max < 0 || min >= 0.0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( min );
      if ( softcal && Calibration != 0 )
	comedi_get_softcal_converter( SubDevice, traces[k].channel(),
				      BipolarRangeIndex[ traces[k].gainIndex() ],
				      COMEDI_TO_PHYSICAL, Calibration, gainp );
      else
	comedi_get_hardcal_converter( DeviceP, SubDevice, traces[k].channel(),
				      BipolarRangeIndex[ traces[k].gainIndex() ],
				      COMEDI_TO_PHYSICAL, gainp );
      chanlist[k] = CR_PACK( traces[k].channel(), 
			     BipolarRangeIndex[ traces[k].gainIndex() ],
			     aref );
    }
  }

  if ( traces.failed() )
    return -1;

  // try automatic command generation:
  cmd.scan_begin_src = TRIG_TIMER;
  cmd.flags = TRIG_ROUND_NEAREST;
  unsigned int period = (int)::rint( 1.0e9 * traces[0].sampleInterval() );  
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice,
					     &cmd, traces.size(), period );
  if ( retVal < 0 ) {
    string emsg = "comedi_get_cmd_generic_timed failed: ";
    emsg += comedi_strerror( comedi_errno() );
    traces.addErrorStr( emsg );
    cerr << "! error in ComediAnalogInput::setupCommand -> comedi_get_cmd_generic_timed failed: "
	 << comedi_strerror( comedi_errno() ) << endl;
    return -1;
  }
  if ( cmd.scan_begin_src != TRIG_TIMER ) {
    traces.addErrorStr( "acquisition timed by a daq-board counter not possible" );
    return -1;
  }
  cmd.scan_begin_arg = period;

  // adapt command to our purpose:
  comedi_cmd testCmd;
  comedi_get_cmd_src_mask( DeviceP, SubDevice, &testCmd );
  if ( testCmd.start_src & TRIG_INT )
    cmd.start_src = TRIG_INT;
  else {
    traces.addError( DaqError::InvalidStartSource );
    traces.addErrorStr( "Internal trigger not supported" );
  }
  cmd.start_arg = 0;
  cmd.scan_end_arg = traces.size();

  // test if countinous-state is supported
  if ( traces[0].continuous() && !(testCmd.stop_src & TRIG_NONE) ) {
    cerr << "! warning ComediAnalogInput::setupCommand(): "
	 << "continuous mode not supported!\n";//////TEST/////
    traces.addError( DaqError::InvalidContinuous );
    traces.setContinuous( false );
  }
  if ( !traces[0].continuous() && !(testCmd.stop_src & TRIG_COUNT) ) {
    cerr << "! warning ComediAnalogInput::setupCommand(): "
	 << "only continuous mode supported!\n";//////TEST/////
    traces.addError( DaqError::InvalidContinuous );
    traces.setContinuous( true );
  }
    
  // set countinous-state:
  if ( traces[0].continuous() ) {
    cmd.stop_src = TRIG_NONE;
    cmd.stop_arg = 0;
  }
  else {
    cmd.stop_src = TRIG_COUNT;
    // set length of acquisition as number of scans:
    cmd.stop_arg = traces[0].size();
  }

  cmd.chanlist = chanlist;
  cmd.chanlist_len = traces.size();

  cmd.data = 0;
  cmd.data_len = 0;
  
  // test command:
  memcpy( &testCmd, &cmd, sizeof( comedi_cmd ) ); // store original state
  for ( int k=0; k<=5; k++ ) {
    retVal = comedi_command_test( DeviceP, &cmd );
    if ( retVal == 0 )
      break;
    switch ( retVal ) {
    case 1: // unsupported trigger in *_src:
      if ( cmd.start_src != testCmd.start_src )
	traces.addErrorStr( "unsupported trigger " + Str(testCmd.start_src) + " in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	traces.addErrorStr( "unsupported trigger " + Str(testCmd.scan_begin_src) + " in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	traces.addErrorStr( "unsupported trigger " + Str(testCmd.convert_src) + " in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	traces.addErrorStr( "unsupported trigger " + Str(testCmd.scan_end_arg) + " in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	traces.addErrorStr( "unsupported trigger " + Str(testCmd.stop_src) + " in stop_src" );
      break;
    case 2: // invalid trigger in *_src:
      if ( cmd.start_src != testCmd.start_src )
	traces.addErrorStr( "invalid trigger " + Str(testCmd.start_src) + " in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	traces.addErrorStr( "invalid trigger " + Str(testCmd.scan_begin_src) + " in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	traces.addErrorStr( "invalid trigger " + Str(testCmd.convert_src) + " in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	traces.addErrorStr( "invalid trigger " + Str(testCmd.scan_end_src) + " in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	traces.addErrorStr( "invalid trigger " + Str(testCmd.stop_src) + " in stop_src" );
      break;
    case 3: // *_arg out of range:
      if ( cmd.start_arg != testCmd.start_arg )
	traces.addErrorStr( "start_arg " + Str(testCmd.start_arg) + " out of range" );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg ) {
	traces.addErrorStr( "requested sampling period of " + Str( testCmd.scan_begin_arg )
			  + "ns smaller than supported! min " + Str( cmd.scan_begin_arg )
			  + "ns sampling interval possible." );
	traces.addError( DaqError::InvalidSampleRate );    
	traces.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      }
      if ( cmd.convert_arg != testCmd.convert_arg )
	traces.addErrorStr( "convert_arg " + Str(testCmd.convert_arg) + " out of range" );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	traces.addErrorStr( "scan_end_arg out " + Str(testCmd.scan_end_arg) + " of range" );
      if ( cmd.stop_arg != testCmd.stop_arg )
	traces.addErrorStr( "stop_arg " + Str(testCmd.stop_arg) + " out of range" );
      break;
    case 4: // adjusted *_arg:
      if ( cmd.start_arg != testCmd.start_arg )
	traces.addErrorStr( "start_arg adjusted to " + Str(cmd.start_arg) );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg )
	traces.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      if ( cmd.convert_arg != testCmd.convert_arg )
	traces.addErrorStr( "convert_arg adjusted to " + Str(cmd.convert_arg) );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	traces.addErrorStr( "scan_end_arg adjusted to " + Str(cmd.scan_end_arg) );
      if ( cmd.stop_arg != testCmd.stop_arg )
	traces.addErrorStr( "stop_arg adjusted to " + Str(cmd.stop_arg) );
      break;
    case 5: // invalid chanlist:
      traces.addErrorStr( "invalid chanlist" );
      break;
    }
  }

  return retVal == 0 ? 0 : -1;
}


int ComediAnalogInput::testReadDevice( InList &traces )
{
  if ( !isOpen() ) {
    traces.addError( DaqError::DeviceNotOpen );
    return -1;
  }

  for ( int k=0; k<traces.size(); k++ ) {
    if ( traces[k].gainIndex() < 0 ) {
      traces[k].addError( DaqError::InvalidGain );
      traces[k].setGainIndex( 0 );
    }
    if ( traces[k].unipolar() ) {
      if ( traces[k].gainIndex() >= (int)UnipolarRange.size() ) {
	traces[k].addError( DaqError::InvalidGain );
	traces[k].setGainIndex( UnipolarRange.size()-1 );
      }
    }
    else {
      if ( traces[k].gainIndex() >= (int)BipolarRange.size() ) {
	traces[k].addError( DaqError::InvalidGain );
	traces[k].setGainIndex( BipolarRange.size()-1 );
      }
    }
  }

  comedi_cmd cmd;
  memset( &cmd, 0, sizeof( comedi_cmd ) );
  lock();
  int retVal = setupCommand( traces, cmd );
  unlock();
  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;

  return retVal;
}


int ComediAnalogInput::prepareRead( InList &traces )
{
  if ( !isOpen() ) {
    traces.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  QMutexLocker locker( mutex() );

  // reset:
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = NULL;
  BufferSize = 0;
  BufferN = 0;
  TotalSamples = 0;
  CurrentSamples = 0;
  Settings.clear();
  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  Traces = 0;
  TraceIndex = 0;

  // command:
  int error = setupCommand( traces, Cmd );
  if ( error )
    return error;

  // init internal buffer:
  // XXX We need something more sensible for setting the buffer size:
  BufferSize = 2 * traces.size() * traces[0].indices( 1.0 ) * BufferElemSize;
  Buffer = new char[BufferSize];
  BufferN = 0;

  TotalSamples = Cmd.stop_arg * Cmd.chanlist_len;
  CurrentSamples = 0;

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

  if ( traces.success() ) {
    setSettings( traces, ReadBufferSize, BufferSize );
    Traces = &traces;
    IsPrepared = true;
    return 0;
  }
  else
    return -1;
}


int ComediAnalogInput::startRead( QSemaphore *sp, QReadWriteLock *datamutex,
				  QWaitCondition *datawait, QSemaphore *aosp )
{
  QMutexLocker locker( mutex() );

  if ( !IsPrepared || Traces == 0 ) {
    cerr << "AI not prepared or no traces!\n";
    return -1;
  }

  // setup instruction list:
  lsampl_t insndata[1];
  insndata[0] = 0;
  comedi_insnlist insnlist;
  insnlist.n_insns = 2;
  insnlist.insns = new comedi_insn[insnlist.n_insns];
  for ( unsigned int k=0; k<insnlist.n_insns; k++ ) {
    insnlist.insns[k].insn = INSN_INTTRIG;
    insnlist.insns[k].subdev = -1;
    insnlist.insns[k].chanspec = 0;
    insnlist.insns[k].data = insndata;
    insnlist.insns[k].n = 1;
  }
  int ilinx = 0;
  
  // execute AI command:
  TraceIndex = 0;
  if ( comedi_command( DeviceP, &Cmd ) < 0 ) {
    int cerror = comedi_errno();
    cerr << "AI command failed: " << comedi_strerror( cerror ) << endl;
    Traces->addErrorStr( deviceFile() + " - execution of comedi_cmd failed: "
			 + comedi_strerror( cerror ) );
    return -1;
  }
  else  
    insnlist.insns[ilinx++].subdev = SubDevice;
  
  // add AO to instruction list:
  bool tookao = false;
  if ( TakeAO && aosp != 0 && ComediAO != 0 && ComediAO->prepared() ) {
    if ( ! ComediAO->useAIStart() )
      insnlist.insns[ilinx++].subdev = ComediAO->comediSubdevice();
    tookao = true;
  }
  
  // execute instruction list:
  insnlist.n_insns = ilinx;
  int ninsns = comedi_do_insnlist( DeviceP, &insnlist );
  delete [] insnlist.insns;
  if ( ninsns == ilinx ) {
    // success:
    bool finished = true;
    // start analog input thread:
    startThread( sp, datamutex, datawait );
    // start analog output thread:
    if ( tookao ) {
      ComediAO->startThread( aosp );
      finished = ComediAO->noMoreData();
    }
    return finished ? 0 : 1;
  }
  else {
    // failed to start:
    cerr << "! error in ComediAnalogInput::startRead -> comedi_do_insnlist executed "
	 << ninsns << " from " << ilinx << " instructions\n";
    Traces->addErrorStr( deviceFile() + " - comedi_do_insnlist executed " + Str( ninsns ) + " from " +
			 Str( ilinx ) + " instructions" );
    return -1;
  }
  return 0;
}


template< typename T >
void ComediAnalogInput::convert( InList &traces, char *buffer, int n )
{
  // conversion polynomials and scale factors:
  double scale[traces.size()];
  const comedi_polynomial_t* polynomial[traces.size()];
  for ( int k=0; k<traces.size(); k++ ) {
    scale[k] = traces[k].scale();
    polynomial[k] = (const comedi_polynomial_t *)traces[k].gainData();
  }

  // trace buffer pointers and sizes:
  float *bp[traces.size()];
  int bm[traces.size()];
  int bn[traces.size()];
  for ( int k=0; k<traces.size(); k++ ) {
    bp[k] = traces[k].pushBuffer();
    bm[k] = traces[k].maxPush();
    bn[k] = 0;
  }

  // type cast for device buffer:
  T *db = (T *)buffer;

  for ( int k=0; k<n; k++ ) {
    // convert:
    *bp[TraceIndex] = comedi_to_physical( db[k], polynomial[TraceIndex] );
    *bp[TraceIndex] *= scale[TraceIndex];
    // update pointers:
    bp[TraceIndex]++;
    bn[TraceIndex]++;
    if ( bn[TraceIndex] >= bm[TraceIndex] ) {
      traces[TraceIndex].push( bn[TraceIndex] );
      bp[TraceIndex] = traces[TraceIndex].pushBuffer();
      bm[TraceIndex] = traces[TraceIndex].maxPush();
      bn[TraceIndex] = 0;
    }
    // next trace:
    TraceIndex++;
    if ( TraceIndex >= traces.size() )
      TraceIndex = 0;
  }

  // commit:
  for ( int c=0; c<traces.size(); c++ )
    traces[c].push( bn[c] );
}


int ComediAnalogInput::readData( void )
{
  if ( Traces == 0 || Buffer == 0 )
    return -1;

  if ( AboutToStop )
    comedi_poll( DeviceP, SubDevice );
    
  // read data:
  int ern = 0;
  int buffern = BufferN*BufferElemSize;
  //  cerr << "BUFFERN " << buffern << '\n';
  ssize_t readn = ::read( comedi_fileno( DeviceP ), Buffer + buffern, BufferSize - buffern );
  //  cerr << "READN " << readn << '\n';

  ern = errno;
  if ( readn < 0 && ern != EAGAIN && ern != EINTR ) {
    if ( ! AboutToStop ) {
      cerr << "COMEDI READ ERROR: " << string( comedi_strerror( comedi_errno() ) ) << '\n';
      Traces->addErrorStr( "Error while reading from device-file: " + deviceFile() );
      Traces->addErrorStr( ern );
      if ( errno == EPIPE )
	Traces->addError( DaqError::OverflowUnderrun );
      return -2;   
    }
    return 0;
  }
  else if ( readn > 0 ) {
    readn /= BufferElemSize;
    BufferN += readn;
    CurrentSamples += readn;
    return readn;
  }
  else if ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING ) == 0 ) {
    // no data have been read:
    if ( ( ! AboutToStop ) && ( TotalSamples <=0 || CurrentSamples < TotalSamples ) ) {
      Traces->addError( DaqError::OverflowUnderrun );
      cerr << " ComediAnalogInput::readData(): no data and not running\n";
      cerr << " ComediAnalogInput::readData(): comedi_error: " << comedi_strerror( comedi_errno() ) << "\n";
      return -2;
    }
    AboutToStop = false;
    return -1;
  }
  return 0;
}


int ComediAnalogInput::convertData( void )
{
  if ( Traces == 0 || Buffer == 0 )
    return -1;

  if ( LongSampleType )
    convert<lsampl_t>( *Traces, Buffer, BufferN );
  else
    convert<sampl_t>( *Traces, Buffer, BufferN );

  int n = BufferN;
  BufferN = 0;

  return n;
}


int ComediAnalogInput::stop( void )
{ 
  if ( !isOpen() )
    return NotOpen;

  {
    QMutexLocker locker( mutex() );
    if ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING ) == 0 )
      return 0;
    AboutToStop = true;
  }
  
  stopRead();

  int r = 0;
  lock();
  if ( comedi_cancel( DeviceP, SubDevice ) < 0 )
    r = ReadError;
  unlock();
  return r;
}


int ComediAnalogInput::reset( void ) 
{ 
  if ( !isOpen() )
    return NotOpen;

  lock();

  // free internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = NULL;
  BufferSize = 0;
  BufferN = 0;
  TotalSamples = 0;
  CurrentSamples = 0;

  Settings.clear();

  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  AboutToStop = false;
  Traces = 0;
  TraceIndex = 0;

  unlock();

  return 0;
}


bool ComediAnalogInput::running( void ) const
{   
  lock();
  bool r = ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING );
  if ( ! AnalogInput::running() )
    r = false;
  unlock();
  return r;
}


void ComediAnalogInput::take( const vector< AnalogInput* > &ais,
			      const vector< AnalogOutput* > &aos,
			      vector< int > &aiinx, vector< int > &aoinx,
			      vector< bool > &airate, vector< bool > &aorate )
{
  ComediAO = 0;

  if ( TakeAO ) {
    // check for analog output device:
    for ( unsigned int k=0; k<aos.size(); k++ ) {
      if ( aos[k]->analogOutputType() == ComediAnalogIOType &&
	   aos[k]->deviceFile() == deviceFile() ) {
	aoinx.push_back( k );
	aorate.push_back( false );
	ComediAO = dynamic_cast< ComediAnalogOutput* >( aos[k] );
	break;
      }
    }
  }
}


int ComediAnalogInput::comediSubdevice( void ) const
{
  if ( !isOpen() )
    return -1;
  return SubDevice;
}


int ComediAnalogInput::bufferSize( void ) const
{
  if ( !isOpen() )
    return -1;
  lock();
  int n = comedi_get_buffer_size( DeviceP, SubDevice ) / BufferElemSize;
  unlock();
  return n;
}


bool ComediAnalogInput::prepared( void ) const 
{
  lock();
  bool ip = IsPrepared;
  unlock();
  return ip;
}


}; /* namespace comedi */
