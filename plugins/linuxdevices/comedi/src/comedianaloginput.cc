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
#include <relacs/comedi/comedianalogoutput.h>
#include <relacs/comedi/comedianaloginput.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediAnalogInput::ComediAnalogInput( void ) 
  : AnalogInput( "Comedi Analog Input", ComediAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxRate = 1000.0;
}


ComediAnalogInput::ComediAnalogInput( const string &device, long mode ) 
  : AnalogInput( "Comedi Analog Input", ComediAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxRate = 1000.0;
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
  if ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_CMD ) == 0 ) {
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

  /* XXX
  // set comedi file-descriptor to non-blocking writing mode
  int retVal = fcntl( comedi_fileno(DeviceP), F_SETFL, O_NONBLOCK );//|O_ASYNC
  if ( retVal < 0 )
    cerr << "! warning: ComediAnalogInput::open() -> "
	 << "Switching the device "  << device 
	 << " to non-blocking mode failed: " << strerror( errno ) << '\n';
  */

  // set size of comedi-internal buffer to maximum:
  // XXXX maybe use an internal maximum as well 
  // (in case comedi max is way too much)?
  int bufSize = comedi_get_max_buffer_size( DeviceP, SubDevice );
  comedi_set_buffer_size( DeviceP, SubDevice, bufSize );

  // initialize ranges:
  UnipolarRange.clear();
  BipolarRange.clear();
  UnipolarRangeIndex.clear();
  BipolarRangeIndex.clear();
  int nRanges = comedi_get_n_ranges( DeviceP, SubDevice, 0 );  
  for ( int i = 0; i < nRanges; i++ ) {
    comedi_range *range = comedi_get_range( DeviceP, SubDevice, 0, i );
    // TODO: if a ranges is not supported but comedi thinks so: set max = -1.0
    // i.e. NI 6070E PCI: range #8 (0..20V) not supported
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
  memset( &cmd,0, sizeof(comedi_cmd) );
  unsigned int chanlist = CR_PACK( 0, 0, AREF_GROUND );
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice, &cmd,
					     1/*chans*/,
					     (unsigned int)1e8/*Hz*/ );
  if ( retVal < 0 ) {
    cmd.subdev = SubDevice;
    cmd.start_src =        TRIG_NOW;
    cmd.start_arg =        0;
    cmd.scan_begin_src =   TRIG_TIMER;
    cmd.convert_src =      TRIG_TIMER;
    cmd.convert_arg =      1;
    cmd.scan_end_src =     TRIG_COUNT;
    cmd.stop_src =         TRIG_COUNT;
  }
  cmd.scan_begin_arg =   1;
  cmd.chanlist           = &chanlist;
  cmd.chanlist_len       = 1;
  cmd.scan_end_arg = 1;
  if ( cmd.stop_src == TRIG_COUNT )
    cmd.stop_arg = 1;
  retVal = comedi_command_test( DeviceP, &cmd );
  if ( retVal == 1 || retVal == 2 )
    retVal = comedi_command_test( DeviceP, &cmd );
  if ( retVal == 1 || retVal == 2 )
    retVal = comedi_command_test( DeviceP, &cmd );
  if ( cmd.scan_begin_arg > 0 )
    MaxRate = 1.0e9 / cmd.scan_begin_arg;
  else
    MaxRate = 1.0e9;

  // clear flags:
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  ComediAIs.clear();
  ComediAOs.clear();
  
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


int ComediAnalogInput::testReadDevice( InList &traces )
{
  if ( !isOpen() )
    return -1;

  cerr << " ComediAnalogInput::testReadDevice() " << endl;/////TEST/////  
  memset( ChanList, 0, sizeof( ChanList ) );
  memset( &Cmd, 0, sizeof( comedi_cmd ) );

  for( int k = 0; k < traces.size(); k++ ) {

    if ( traces[k].delay() > 1.0e-9 ) {
      traces.addError( DaqError::InvalidDelay );
      traces.addErrorStr( "delays are not supported and will be ignored!" );
    }

    int aref = -1;
    switch( traces[k].reference() ) {
    case InData::RefCommon: 
      if ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_COMMON )
	aref = AREF_COMMON;
      break;
    case InData::RefDifferential:
      if ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_DIFF )
	aref = AREF_DIFF; 
      break;
    case InData::RefGround:
      if ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_GROUND )
	aref = AREF_GROUND; 
      break;
    case InData::RefOther: 
      if ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_OTHER )
	aref = AREF_OTHER;
      break;
    }
    if ( aref == -1 )
      traces[k].addError( DaqError::InvalidReference );

    if ( traces[k].unipolar() ) {
      double max = UnipolarRange[traces[k].gainIndex()].max;
      double min = UnipolarRange[traces[k].gainIndex()].min;
      if ( max < 0 || min < 0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( 0.0 );
      traces[k].setGain( (max-min)/comedi_get_maxdata( DeviceP, SubDevice, 0 ),
			 min );
      ChanList[k] = CR_PACK( traces[k].channel(), 
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
      traces[k].setGain( (max-min)/comedi_get_maxdata( DeviceP, SubDevice, 0 ),
			 min );
      ChanList[k] = CR_PACK( traces[k].channel(), 
			     BipolarRangeIndex[ traces[k].gainIndex() ],
			     aref );
      cerr << " ComediAnalogInput::testReadDevice(): max = " << max << endl;/////TEST/////  
    }

  }

  /* // XXX i'm not sure if testing works on running device...
  if ( running() ) {
    traces.addError( DaqError::Busy );
    return -1;
  }
  */
      
  if ( traces[0].sampleRate() * (double)traces.size() > maxRate() )
    traces.addError( DaqError::InvalidSampleRate );

  if ( traces.failed() )
    return -1;

  // try automatic command generation:
  unsigned int intervalLength = (int)::rint( 1.0e9 * traces[0].sampleInterval() );  
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice, &Cmd, traces.size(), intervalLength );
  /*
  from QTScope:
        if ((cmd->convert_src ==  TRIG_TIMER)&&(cmd->convert_arg)) {
                freq=1E9 / cmd->convert_arg;
        }
        if ((cmd->scan_begin_src ==  TRIG_TIMER)&&(cmd->scan_begin_arg)) {
                freq=1E9 / cmd->scan_begin_arg;
        }
  */
  if ( Cmd.scan_begin_arg < intervalLength ) {
    cerr << " ComediAnalogInput::testReadDevice(): "
	 << "Requested sampling rate bigger than supported! max "
	 << Cmd.scan_begin_arg << "ns sampling interval possible.\n";/////TEST/////
    traces.addError( DaqError::InvalidSampleRate );
  }
  if ( retVal < 0 ) {
    Cmd.scan_begin_src = TRIG_TIMER; // when to perform a scan is triggered by timer
    Cmd.scan_begin_arg = intervalLength; // nsec's between 2 scans (1/sampling-frequency)
    Cmd.convert_src = TRIG_TIMER;
    Cmd.convert_arg = intervalLength/ traces.size();
    Cmd.scan_end_src = TRIG_COUNT;
  }

  // adapt command to our purpose:
  comedi_cmd testCmd;
  comedi_get_cmd_src_mask( DeviceP, SubDevice, &testCmd );
  if ( testCmd.start_src & TRIG_INT )
    Cmd.start_src = TRIG_INT;
  else
    Cmd.start_src = TRIG_NOW;
  Cmd.start_arg = 0;
  if ( Cmd.convert_src & TRIG_NOW )
    Cmd.convert_arg = 1;
  Cmd.scan_end_arg = traces.size();

  // test if countinous-state is supported
  if ( traces[0].continuous() && !(testCmd.stop_src & TRIG_NONE) ) {
    cerr << " ComediAnalogInput::testReadDevice(): "
	 << " continuous mode not supported!\n";//////TEST/////
    traces.addError( DaqError::InvalidContinuous );
    traces.setContinuous( false );
  }
  if ( !traces[0].continuous() && !(testCmd.stop_src & TRIG_COUNT) ) {
    cerr << " ComediAnalogInput::testReadDevice(): "
	 << " only continuous mode supported!\n";//////TEST/////
    traces.addError( DaqError::InvalidContinuous );
    traces.setContinuous( true );
  }
    
  // set countinous-state:
  if ( traces[0].continuous() ) {
      Cmd.stop_src = TRIG_NONE;
      Cmd.stop_arg = 0;
    }
  if ( !traces[0].continuous() ) {
      Cmd.stop_src = TRIG_COUNT;
      // set length of acquisition as number of scans:
      Cmd.stop_arg = traces[0].size(); // XXX change this if delay should be simulated
    }

  Cmd.chanlist = ChanList;
  Cmd.chanlist_len = traces.size();
  // XXX ATTENTION:  maybe TRIG_WAKE_EOS causes many interrupts!
  //  Cmd.flags = TRIG_WAKE_EOS | TRIG_RT;
  Cmd.flags = TRIG_ROUND_NEAREST;
  
  // test command:
  // XXX analyse errors!
  // after first call pf comedi_command_test Cmd should be ok:
  retVal = comedi_command_test( DeviceP, &Cmd );
  retVal = comedi_command_test( DeviceP, &Cmd );
  // if this second call is not successfull, then something is really wrong:
  if ( retVal ) {
    Cmd.flags &= ~TRIG_RT;
    retVal = comedi_command_test( DeviceP, &Cmd );
    retVal = comedi_command_test( DeviceP, &Cmd );    
    if ( retVal ) {
      Cmd.flags = 0;
      retVal = comedi_command_test( DeviceP, &Cmd );
      retVal = comedi_command_test( DeviceP, &Cmd );
    }
  }
  /* from QTScope:

        if ((cmd->convert_src ==  TRIG_TIMER)&&(cmd->convert_arg)) {
                freq=1E9 / cmd->convert_arg;
        }
        if ((cmd->scan_begin_src ==  TRIG_TIMER)&&(cmd->scan_begin_arg)) {
                freq=1E9 / cmd->scan_begin_arg;
        }
  */
  if ( retVal != 0 )
    return -retVal;

  return 0;
}


int ComediAnalogInput::prepareRead( InList &traces )
{
  if ( !isOpen() )
    return -1;

  reset();

  int error = testReadDevice( traces );
  if ( error )
    return error;

  // setup the buffer (don't free the buffer!):
  if ( traces[0].deviceBuffer() == NULL ) {
    // size of buffer:
    traces[0].reserveDeviceBuffer( traces.size() * traces[0].indices( traces[0].updateTime() ),
				   BufferElemSize );
    if ( traces[0].deviceBuffer() == NULL )
      traces[0].reserveDeviceBuffer( traces.size() * traces[0].capacity(),
				     BufferElemSize );
  }
  // buffer overflow:
  if ( traces[0].deviceBufferSize() >= traces[0].deviceBufferCapacity() ) {
    traces.addError( DaqError::BufferOverflow );
    return -1;
  }
  
  if ( traces.success() )
    setSettings( traces );

  IsPrepared = traces.success();

  if ( traces.success() )
    cerr << " ComediAnalogInput::prepareRead(): success" << endl;/////TEST/////

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
  return 0;
}


int ComediAnalogInput::startRead( InList &traces )
{
  //  cerr << " ComediAnalogInput::startRead(): begin" << endl;/////TEST/////

  if ( !prepared() ) {
    cerr << "AI not prepared!\n";
    return -1;
  }

  // setup instruction list:
  comedi_insnlist insnlist;
  insnlist.n_insns = ComediAIs.size() + ComediAOs.size();
  insnlist.insns = new comedi_insn[insnlist.n_insns];
  for ( unsigned int k=0; k<insnlist.n_insns; k++ ) {
    insnlist.insns[k].insn = INSN_INTTRIG;
    insnlist.insns[k].subdev = -1;
    InsnData[0] = 0;
    insnlist.insns[k].data = InsnData;
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
      for ( unsigned int k=0; k<ComediAIs.size(); k++ )
	ComediAIs[k]->setRunning();
      for ( unsigned int k=0; k<ComediAOs.size(); k++ )
 	ComediAOs[k]->setRunning();
    }
    else
      success = false;
  }
  delete [] insnlist.insns;

  return success ? 0 : -1;
}


int ComediAnalogInput::readData( InList &traces )
{
  // buffer overflow:
  if ( traces[0].deviceBufferSize() >= traces[0].deviceBufferCapacity() ) {
    cerr << " ComediAnalogInput::readData():  buffer overflow\n";//////TEST/////
    return -1;
  }

  ErrorState = 0;
  bool failed = false;
  int n = 0;
  
  // try to read twice:
  for ( int tryit = 0;
	tryit < 2 && ! failed && traces[0].deviceBufferMaxPush() > 0;
	tryit++ ) {

    // data present?
    if ( comedi_get_buffer_contents( DeviceP, SubDevice ) <= 0 )
      break;
    
    // read data:
    ssize_t m = read( comedi_fileno( DeviceP ),
		      traces[0].deviceBufferPushBuffer(), 
		      traces[0].deviceBufferMaxPush() * BufferElemSize );

    int ern = errno;
    if ( m < 0 && ern != EAGAIN && ern != EINTR ) {
      traces.addErrorStr( ern );
      failed = true;
      cerr << " ComediAnalogInput::readData(): error" << endl;/////TEST/////
    }
    else if ( m > 0 ) {
      m /= BufferElemSize;
      traces[0].deviceBufferPush( m );
      n += m;
    }

  }

  if ( LongSampleType )
    convert<lsampl_t>( traces );
  else
    convert<sampl_t>( traces );

  if ( failed ) {
    /* XXX
    // check buffer underrun:
    if ( errno == EPIPE ) {
      ErrorState = 1;
      traces.addErrorStr( deviceFile() + " - buffer-overflow: "
			  + comedi_strerror( comedi_errno() ) );
      traces.addError( DaqError::OverflowUnderrun );
    }    
    else {
      ErrorState = 2;
      traces.addErrorStr( "Error while reading from device-file: " + deviceFile()
			  + "  comedi: " + comedi_strerror( comedi_errno() ) 
			  + "  system: " + strerror( errno ) );
      traces.addError( DaqError::Unknown );
    }
    cerr << "ComediAnalogInput::readData(): Errorstate = " << ErrorState << endl;
    */
    return -1;   
  }

  return n;
}


int ComediAnalogInput::stop( void )
{ 
  cerr << " ComediAnalogInput::stop()" << endl;/////TEST/////
  if ( !isOpen() )
    return NotOpen;

  if ( comedi_cancel( DeviceP, SubDevice ) < 0 )
    return ReadError;

  IsRunning = false;
  return 0;
}


int ComediAnalogInput::reset( void ) 
{ 
  int retVal = stop();
  // XXX clear buffers!
  clearSettings();

  ErrorState = 0;
  IsPrepared = false;

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


int ComediAnalogInput::reload( void )
{
  cerr << " ComediAnalogInput::reloadCommand()" << endl;/////TEST/////
  if ( loaded() )
    return 0;

  if ( !prepared() || comedi_command( DeviceP, &Cmd ) < 0 ) {
    cerr << "ComediAnalogInput::startCommand()-> " << deviceFile()
	 <<" - execution of comedi_cmd failed: " << comedi_strerror( comedi_errno() );
    cerr << " ComediAnalogInput::reloadCommand() -> ERROR!" << endl;/////TEST/////
    return ReadError;
  }
  return 0;
}


bool ComediAnalogInput::loaded( void ) const 
{ 
  if ( !isOpen() )
    return false;
  return SDF_BUSY & comedi_get_subdevice_flags( DeviceP, SubDevice );
}


bool ComediAnalogInput::prepared( void ) const 
{ 
  return IsPrepared;
}


void ComediAnalogInput::setRunning( void )
{
  IsRunning = true;
  ErrorState = 0;
}


}; /* namespace comedi */
