/*
  comedi/comedianaloginput.cc
  Interface for accessing analog input of a daq-board via comedi.

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
#include <unistd.h>
#include <fcntl.h>
#include <relacs/str.h>
#include <relacs/comedi/comedianalogoutput.h>
#include <relacs/comedi/comedianaloginput.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediAnalogInput::ComediAnalogInput( void ) 
  : AnalogInput( "Comedi Analog Input", ComediAnalogIOType )
{
  ErrorState = 0;
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxRate = 1000.0;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  Calibration = 0;
  Traces = 0;
  BufferSize = 0;
  TraceIndex = 0;
}


ComediAnalogInput::ComediAnalogInput( const string &device, long mode ) 
  : AnalogInput( "Comedi Analog Input", ComediAnalogIOType )
{
  ErrorState = 0;
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxRate = 1000.0;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  Calibration = 0;
  Traces = 0;
  BufferSize = 0;
  TraceIndex = 0;
  open( device, mode );
}

  
ComediAnalogInput::~ComediAnalogInput( void ) 
{
  close();
}


int ComediAnalogInput::open( const string &device, long mode )
{ 
  if ( isOpen() )
    return -5;

  clearSettings();
  if ( device.empty() )
    return InvalidDevice;

  // open comedi device:
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    cerr << "! error: ComediAnalogInput::open() -> "
	 << "Device-file " << device << " could not be opened!\n";
    return NotOpen;
  }

  // get AI subdevice:
  int subdev = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_AI, 0 );
  if ( subdev < 0 ) {
    cerr << "! error: ComediAnalogInput::open() -> "
	 << "No subdevice for AI found on device "  << device << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    return InvalidDevice;
  }
  SubDevice = subdev;

  // lock AI subdevice:
  if ( comedi_lock( DeviceP, SubDevice ) != 0 ) {
    cerr << "! error: ComediAnalogInput::open() -> "
	 << "Locking of AI subdevice failed on device " << device << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return NotOpen;
  }  

  // check for async. command support:
  if ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_CMD_READ ) == 0 ) {
    cerr << "! error: ComediAnalogInput::open() -> "
	 << "Device "  << device << " not supported! "
	 << "SubDevice needs to support async. commands!" << endl;
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
  BufferSize = comedi_get_max_buffer_size( DeviceP, SubDevice );
  comedi_set_buffer_size( DeviceP, SubDevice, BufferSize );
  BufferSize = comedi_get_buffer_size( DeviceP, SubDevice );
  // XXX add this to settings?

  // make read calls non blocking:
  fcntl( comedi_fileno( DeviceP ), F_SETFL, O_NONBLOCK );

  // get calibration:
  {
    char *calibpath = comedi_get_default_calibration_path( DeviceP );
    ifstream cf( calibpath );
    if ( cf.good() )
      Calibration = comedi_parse_calibration_file( calibpath );
    else
      Calibration = 0;
    delete [] calibpath;
  }

  // initialize ranges:
  UnipolarRange.clear();
  BipolarRange.clear();
  UnipolarRangeIndex.clear();
  BipolarRangeIndex.clear();
  int nRanges = comedi_get_n_ranges( DeviceP, SubDevice, 0 );  
  for ( int i = 0; i < nRanges; i++ ) {
    comedi_range *range = comedi_get_range( DeviceP, SubDevice, 0, i );
    // XXX: if a ranges is not supported but comedi thinks so: set max = -1.0
    // i.e. NI 6070E PCI and DAQCard-6062E: range #8 (0..20V) not supported
    if ( range->min < 0.0 ) {
      BipolarRange.push_back( *range );
      BipolarRangeIndex.push_back( i );
    }
    else {
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

  // try to find out the maximum sampling rate:
  comedi_cmd cmd;
  memset( &cmd, 0, sizeof(comedi_cmd) );
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice, &cmd,
					     1 /*chans*/, 1 /*ns*/ );
  if ( retVal < 0 ) {
    cerr << "! error in ComediAnalogInput::open -> cannot get maximum sampling rate from comedi_get_cmd_generic_timed failed: "
	 << comedi_strerror( comedi_errno() ) << endl;
    close();
    return -1;
  }
  else
    MaxRate = 1.0e9 / cmd.scan_begin_arg;

  // clear flags:
  ErrorState = 0;
  ComediAIs.clear();
  ComediAOs.clear();
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  
  return 0;
}


bool ComediAnalogInput::isOpen( void ) const 
{ 
  return ( DeviceP != NULL );
}


void ComediAnalogInput::close( void ) 
{
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
    cerr << "! warning: ComediAnalogInput::close() -> "
	 << "Unlocking of AI subdevice on device " << deviceFile() << "failed\n";

  // close:
  error = comedi_close( DeviceP );
  if ( error )
    cerr << "! warning: ComediAnalogInput::close() -> "
	 << "Closing of AI subdevice on device " << deviceFile() << "failed.\n";

  // clear flags:
  DeviceP = NULL;
  SubDevice = 0;
  ComediAIs.clear();
  ComediAOs.clear();
  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  TraceIndex = 0;
}


int ComediAnalogInput::channels( void ) const
{ 
  if ( !isOpen() )
    return -1;
  return comedi_get_n_channels( DeviceP, SubDevice );
}


int ComediAnalogInput::bits( void ) const
{ 
  if ( !isOpen() )
    return -1;
  int maxData = comedi_get_maxdata( DeviceP, SubDevice, 0 );
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
  

string cmd_src( int src )
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


void dump_cmd( comedi_cmd *cmd )
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
  if ( !isOpen() ) {
    traces.addError( DaqError::DeviceNotOpen );
    return -1;
  }

  // channels:
  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;
  unsigned int *chanlist = new unsigned int[512];
  memset( chanlist, 0, sizeof( chanlist ) );
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
  if ( !traces[0].continuous() ) {
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
	traces.addErrorStr( "unsupported trigger in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	traces.addErrorStr( "unsupported trigger in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	traces.addErrorStr( "unsupported trigger in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	traces.addErrorStr( "unsupported trigger in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	traces.addErrorStr( "unsupported trigger in stop_src" );
      break;
    case 2: // invalid trigger in *_src:
      if ( cmd.start_src != testCmd.start_src )
	traces.addErrorStr( "invalid trigger in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	traces.addErrorStr( "invalid trigger in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	traces.addErrorStr( "invalid trigger in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	traces.addErrorStr( "invalid trigger in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	traces.addErrorStr( "invalid trigger in stop_src" );
      break;
    case 3: // *_arg out of range:
      if ( cmd.start_arg != testCmd.start_arg )
	traces.addErrorStr( "start_arg out of range" );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg ) {
	cerr << "! warning in ComediAnalogInput::setupCommand() -> "
	     << "requested sampling period of " << testCmd.scan_begin_arg
	     << "ns smaller than supported! max " << cmd.scan_begin_arg
	     << "ns sampling interval possible." << endl;
	traces.addError( DaqError::InvalidSampleRate );    
	traces.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      }
      if ( cmd.convert_arg != testCmd.convert_arg )
	traces.addErrorStr( "convert_arg out of range" );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	traces.addErrorStr( "scan_end_arg out of range" );
      if ( cmd.stop_arg != testCmd.stop_arg )
	traces.addErrorStr( "stop_arg out of range" );
      break;
    case 4: // adjusted *_arg:
      if ( cmd.start_arg != testCmd.start_arg )
	traces.addErrorStr( "start_arg adjusted" );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg )
	traces.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      if ( cmd.convert_arg != testCmd.convert_arg )
	traces.addErrorStr( "convert_arg adjusted" );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	traces.addErrorStr( "scan_end_arg adjusted" );
      if ( cmd.stop_arg != testCmd.stop_arg )
	traces.addErrorStr( "stop_arg adjusted" );
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
  int retVal = setupCommand( traces, cmd );
  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;

  int bufsize = traces.size()*BufferElemSize*traces[0].indices( traces[0].updateTime() );
  int maxbufsize = comedi_get_max_buffer_size( DeviceP, SubDevice );
  if ( bufsize > maxbufsize ) {
    traces.addError( DaqError::InvalidUpdateTime );
    traces.setUpdateTime( maxbufsize/traces.size()/BufferElemSize/traces[0].sampleRate() );
    retVal = -1;
  }

  return retVal;
}


int ComediAnalogInput::prepareRead( InList &traces )
{
  if ( !isOpen() )
    return -1;

  reset();

  int error = setupCommand( traces, Cmd );
  if ( error )
    return error;

  // check buffer size:
  int minbufsize = traces.size() * traces[0].indices( traces[0].updateTime() ) * BufferElemSize;
  if ( minbufsize > BufferSize )
    traces.addError( DaqError::InvalidUpdateTime );

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
    setSettings( traces );
    Traces = &traces;
  }

  IsPrepared = traces.success();

  return traces.success() ? 0 : -1;
}


int ComediAnalogInput::executeCommand( void )
{
  ErrorState = 0;
  if ( comedi_command( DeviceP, &Cmd ) < 0 ) {
    cerr << "AI command failed: " << comedi_strerror( comedi_errno() ) << endl;
    /*
    traces.addErrorStr( deviceFile() + " - execution of comedi_cmd failed: "
			+ comedi_strerror( comedi_errno() ) );
    */
    return -1;
  }
  TraceIndex = 0;
  return 0;
}


int ComediAnalogInput::startRead( void )
{
  //  cerr << " ComediAnalogInput::startRead(): begin" << endl;/////TEST/////

  if ( !prepared() || Traces == 0 ) {
    cerr << "AI not prepared or no traces!\n";
    return -1;
  }

  // setup instruction list:
  lsampl_t insndata[1];
  insndata[0] = 0;
  comedi_insnlist insnlist;
  insnlist.n_insns = ComediAIs.size() + ComediAOs.size();
  insnlist.insns = new comedi_insn[insnlist.n_insns];
  for ( unsigned int k=0; k<insnlist.n_insns; k++ ) {
    insnlist.insns[k].insn = INSN_INTTRIG;
    insnlist.insns[k].subdev = -1;
    insnlist.insns[k].data = insndata;
    insnlist.insns[k].n = 1;
  }
  bool success = true;
  int ilinx = 0;
  for ( unsigned int k=0; k<ComediAIs.size() && success; k++ ) {
    if ( ComediAIs[k]->prepared() ) {
      if ( ComediAIs[k]->executeCommand() < 0 )
	success = false;
      else
	insnlist.insns[ilinx++].subdev = ComediAIs[k]->comediSubdevice();
    }
  }
  for ( unsigned int k=0; k<ComediAOs.size() && success; k++ ) {
    if ( ComediAOs[k]->prepared() ) {
      if ( ComediAOs[k]->executeCommand() < 0 )
	success = false;
      else
	insnlist.insns[ilinx++].subdev = ComediAOs[k]->comediSubdevice();
    }
  }
  insnlist.n_insns = ilinx;
  if ( success ) {
    int ninsns = comedi_do_insnlist( DeviceP, &insnlist );
    if ( ninsns == ilinx ) {
      for ( unsigned int k=0; k<ComediAOs.size() && success; k++ )
	ComediAOs[k]->clearCommand();
    }
    else
      success = false;
  }
  delete [] insnlist.insns;

  return success ? 0 : -1;
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
  if ( Traces == 0 )
    return -1;

  ErrorState = 0;
  bool failed = false;
  char buffer[BufferSize];
  int maxn = BufferSize;
  int readn = 0;

  // try to read twice:
  for ( int tryit = 0; tryit < 2 && ! failed && maxn > 0; tryit++ ) {

    // data present?
    if ( comedi_get_buffer_contents( DeviceP, SubDevice ) <= 0 )
      break;
    
    // read data:
    ssize_t m = read( comedi_fileno( DeviceP ), buffer + readn, maxn );

    int ern = errno;
    if ( m < 0 && ern != EAGAIN && ern != EINTR ) {
      Traces->addErrorStr( ern );
      failed = true;
      cerr << " ComediAnalogInput::readData(): error" << endl;
    }
    else if ( m > 0 ) {
      maxn -= m;
      readn += m;
    }

  }

  readn /= BufferElemSize;

  if ( LongSampleType )
    convert<lsampl_t>( *Traces, buffer, readn );
  else
    convert<sampl_t>( *Traces, buffer, readn );

  if ( failed ) {
    /* XXX
    // check buffer underrun:
    if ( errno == EPIPE ) {
      ErrorState = 1;
      Traces->addErrorStr( deviceFile() + " - buffer-overflow: "
			  + comedi_strerror( comedi_errno() ) );
      Traces->addError( DaqError::OverflowUnderrun );
    }    
    else {
      ErrorState = 2;
      Traces->addErrorStr( "Error while reading from device-file: " + deviceFile()
			  + "  comedi: " + comedi_strerror( comedi_errno() ) 
			  + "  system: " + strerror( errno ) );
      Traces->addError( DaqError::Unknown );
    }
    cerr << "ComediAnalogInput::readData(): Errorstate = " << ErrorState << endl;
    */
    return -1;   
  }

  return readn;
}


int ComediAnalogInput::convertData( void )
{
  if ( Traces == 0 )
    return -1;

  return 0; // number of data points
}


int ComediAnalogInput::stop( void )
{ 
  if ( !isOpen() )
    return NotOpen;

  if ( comedi_cancel( DeviceP, SubDevice ) < 0 )
    return ReadError;

  return 0;
}


int ComediAnalogInput::reset( void ) 
{ 
  int retVal = stop();

  // clear buffers by reading:
  while ( comedi_get_buffer_contents( DeviceP, SubDevice ) > 0 ) {
    char buffer[BufferSize];
    read( comedi_fileno( DeviceP ), buffer, BufferSize );
  }

  clearSettings();

  ErrorState = 0;
  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
  Traces = 0;
  TraceIndex = 0;

  return retVal;
}


bool ComediAnalogInput::running( void ) const
{   
  return ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING );
}


int ComediAnalogInput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */
}


void ComediAnalogInput::take( const vector< AnalogInput* > &ais,
			      const vector< AnalogOutput* > &aos,
			      vector< int > &aiinx,
			      vector< int > &aoinx )
{
  ComediAIs.clear();
  ComediAOs.clear();

  // check for analog input devices:
  for ( unsigned int k=0; k<ais.size(); k++ ) {
    if ( ais[k]->analogInputType() == ComediAnalogIOType &&
	 ais[k]->deviceFile() == deviceFile() ) {
      aiinx.push_back( k );
      ComediAIs.push_back( dynamic_cast< ComediAnalogInput* >( ais[k] ) );
    }
  }

  // check for analog output devices:
  for ( unsigned int k=0; k<aos.size(); k++ ) {
    if ( aos[k]->analogOutputType() == ComediAnalogIOType &&
	 aos[k]->deviceFile() == deviceFile() ) {
      aoinx.push_back( k );
      ComediAOs.push_back( dynamic_cast< ComediAnalogOutput* >( aos[k] ) );
    }
  }
}


comedi_t* ComediAnalogInput::comediDevice( void ) const
{
  return DeviceP;
}


int ComediAnalogInput::comediSubdevice( void ) const
{
  if ( !isOpen() )
    return -1;
  return SubDevice;
}


int ComediAnalogInput::bufferSize( void ) const
{
  if( !isOpen() )
    return -1;
  return comedi_get_buffer_size( DeviceP, SubDevice ) / BufferElemSize;
}


bool ComediAnalogInput::prepared( void ) const 
{ 
  return IsPrepared;
}


}; /* namespace comedi */
