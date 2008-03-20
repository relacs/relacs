/*
  comedianaloginput.cc
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
#include <errno.h>
#include <relacs/comedianalogoutput.h>
#include <relacs/comedianaloginput.h>

ComediAnalogInput::ComediAnalogInput( void ) 
  : AnalogInput( ComediAnalogInputType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  Mode = 0;
  AsyncMode = true;
  DeviceP = NULL;
}

ComediAnalogInput::ComediAnalogInput( const string &devicename ) 
  : AnalogInput( devicename, ComediAnalogInputType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  Mode = 0;
  AsyncMode = true;
  DeviceP = NULL;
  open( devicename, Mode );
}

ComediAnalogInput::~ComediAnalogInput( void ) 
{
  close();
}

int ComediAnalogInput::open( const string &devicename, long mode )
{ 
  if( DeviceP )
    close();
  clearSettings();
  if ( devicename.empty() )
    return InvalidDevice;

  int retVal;
  Devicename = devicename;
  DeviceP = comedi_open( devicename.c_str() );
  if ( DeviceP == NULL ) {
    cerr <</* currentTime() <<*/ " !  ComediAnalogInput::open() -> "
	 << "Device-file " << devicename << " could not be opened!" << endl;
    return NotOpen;
  }

  Subdevice = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_AI, 0 );
  if( Subdevice < 0 ) {
    cerr << /*currentTime() <<*/ " !  ComediAnalogInput::open() -> "
	 << "No Subdevice for analog input found on device "  << devicename
	 << endl;
    DeviceP = NULL;
    return InvalidDevice;
  }

  if( comedi_lock( DeviceP, Subdevice ) != 0 ) {
    cerr << /*currentTime() <<*/ " !  ComediAnalogInput::open() -> "
	 << "Locking of analog input Subdevice failed on device " << devicename
	 << endl;
    DeviceP = NULL;
    return NotOpen;
  }  

  if( AsyncMode && 
      ! ( SDF_CMD & comedi_get_subdevice_flags( DeviceP, Subdevice ) ) ) {
    cerr << /*currentTime() <<*/ " !  ComediAnalogInput::open() -> "
	 << "Device "  << devicename << " not supported! "
	 << "Subdevice needs to support async. commands!" << endl;
    return InvalidDevice;
  }
  setDeviceName( comedi_get_board_name( DeviceP ) );

  // set comedi file-descriptor to non-blocking writing mode
  retVal = fcntl( comedi_fileno(DeviceP), F_SETFL, O_NONBLOCK );//|O_ASYNC
  if( retVal < 0 )
    cerr << /*currentTime() <<*/ " !  ComediAnalogInput::open() -> "
	 << "Switching the device "  << devicename 
	 << " to non-blocking mode failed: " << strerror( errno ) << endl;

  // set size of comedi-internal buffer to maximum
  int bufSize = comedi_get_max_buffer_size( DeviceP, Subdevice );
  comedi_set_buffer_size( DeviceP, Subdevice, bufSize );

  // initialize ranges
  int nRanges = comedi_get_n_ranges( DeviceP, Subdevice, 0 );  
  for ( int iRange = 0; iRange < nRanges; iRange++ ) {
    comedi_range *range = comedi_get_range( DeviceP, Subdevice, 0, iRange );
    // TODO: if a ranges is not supported but comedi thinks so: set max = -1.0
    // i.e. NI 6070E PCI: iRange 8 (0..20V) not supported
    if( range->min < 0.0 ) {
      BipolarRange.push_back( *range );
      BipolarRangeIndex.push_back( iRange );
    }
    else {
      UnipolarRange.push_back( *range );
      UnipolarRangeIndex.push_back( iRange );
    }
  }


  // bubble-sorting Uni/BipolarRangeIndex according to Uni/BipolarRange.max
  unsigned int iSwap;
  for( unsigned int i = 0; i < UnipolarRangeIndex.size(); i++ )
    for ( unsigned int j = i+1; j < UnipolarRangeIndex.size(); j++ )
      if(  UnipolarRange[ UnipolarRangeIndex[i] ].max
         < UnipolarRange[ UnipolarRangeIndex[j] ].max ) {
	iSwap = UnipolarRangeIndex[i];
	UnipolarRangeIndex[i] = UnipolarRangeIndex[j];
	UnipolarRangeIndex[j] = iSwap;
      }
  for( unsigned int i = 0; i < BipolarRangeIndex.size(); i++ )
    for ( unsigned int j = i+1; j < BipolarRangeIndex.size(); j++ )
      if(  BipolarRange[ BipolarRangeIndex[i] ].max
         < BipolarRange[ BipolarRangeIndex[j] ].max ) {
	iSwap = BipolarRangeIndex[i];
	BipolarRangeIndex[i] = BipolarRangeIndex[j];
	BipolarRangeIndex[j] = iSwap;
      }

  // set comedi out-of-range behavior on sample values
  comedi_set_global_oor_behavior( COMEDI_OOR_NUMBER ); // COMEDI_OOR_NAN

  // get size of datatype for sample values
  LongSampleType = SDF_LSAMPL & comedi_get_subdevice_flags( DeviceP, Subdevice );
  if( LongSampleType )
    BufferElemSize = sizeof(lsampl_t);
  else
    BufferElemSize = sizeof(sampl_t);

  // try to find out the maximum sampling rate
  comedi_cmd cmd;
  memset( &cmd,0, sizeof(comedi_cmd) );
  unsigned int chanlist = CR_PACK( 0, 0, AREF_GROUND );
  retVal = comedi_get_cmd_generic_timed( DeviceP, Subdevice, &cmd, 1/*chans*/, (unsigned int)1e8/*Hz*/ );
   if( retVal < 0 ){
    cmd.subdev = Subdevice;
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
  if(cmd.stop_src==TRIG_COUNT)
    cmd.stop_arg = 1;
  retVal = comedi_command_test( DeviceP, &cmd );
  if( retVal == 1 || retVal == 2 )
    retVal = comedi_command_test( DeviceP, &cmd );
  if( retVal == 1 || retVal == 2 )
    retVal = comedi_command_test( DeviceP, &cmd );
  if( cmd.scan_begin_arg > 0 )
    MaxRate = 1.0e9 / cmd.scan_begin_arg;
  else
    MaxRate = 1.0e9;
  
  return 0;
}

void ComediAnalogInput::close( void ) 
{
  reset();
  if( ! DeviceP )
    return;
  comedi_unlock( DeviceP,  Subdevice );
  int error = comedi_close( DeviceP );
  if( error )
    cerr << /*currentTime() <<*/ " !  ComediAnalogInput::close() -> "
	 << "Closing of analog input subdevice on device " << Devicename
	 << "threw an error. Forcing close..."
	 << endl;
  DeviceP = NULL;
}

int ComediAnalogInput::reset( void ) 
{ 
  int retVal = stop();
  clearSettings();

  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;

  return retVal;
}

int ComediAnalogInput::stop( void )
{ 
  cerr << " ComediAnalogInput::stop()" << endl;/////TEST/////
  if( !isOpen() )
    return NotOpen;
  if( comedi_cancel( DeviceP, Subdevice ) < 0 )
    return ReadError;
  IsRunning = false;
  return 0;
}

int ComediAnalogInput::reload( void )
{
  cerr << " ComediAnalogInput::reloadCommand()" << endl;/////TEST/////
  if( loaded() )
    return 0;

  if( !prepared() || comedi_command( DeviceP, &Cmd ) < 0 ) {
    cerr << "ComediAnalogInput::startCommand()-> " << Devicename
	 <<" - execution of comedi_cmd failed: " << comedi_strerror( comedi_errno() );
    cerr << " ComediAnalogInput::reloadCommand() -> ERROR!" << endl;/////TEST/////
    return ReadError;
  }
  return 0;
}

void ComediAnalogInput::setMode( int mode ) 
{ 
  Mode = mode; 
}

int ComediAnalogInput::mode( void ) const 
{ 
  return Mode; 
}

bool ComediAnalogInput::isOpen( void ) const 
{ 
  return DeviceP;
}

bool ComediAnalogInput::prepared( void ) const 
{ 
  return IsPrepared;
}

bool ComediAnalogInput::loaded( void ) const 
{ 
  if( !isOpen() )
    return false;
  return SDF_BUSY & comedi_get_subdevice_flags( DeviceP, Subdevice );
}

bool ComediAnalogInput::running( void ) const
{   
  if( !loaded() ) {
    if( IsRunning )
      cerr << " ComediAnalogInput::running(): stopped!"  << endl;
    IsRunning = false;
  }
  if( IsRunning )
    cerr << " ComediAnalogInput::running(): running"  << endl;
  else
    cerr << " ComediAnalogInput::running(): not running"  << endl;
  return IsRunning;
}

void ComediAnalogInput::setRunning( void )
{
  IsRunning = true;
}

string ComediAnalogInput::deviceName( void ) const
{
  return Devicename;
}

comedi_t* ComediAnalogInput::device( void ) const
{
  return DeviceP;
}

int ComediAnalogInput::subdevice( void ) const
{
  if( !isOpen() )
    return -1;
  return Subdevice;
}

int ComediAnalogInput::channels( void ) const
{ 
  if( !isOpen() )
    return -1;
  return comedi_get_n_channels( DeviceP, Subdevice);
}

int ComediAnalogInput::bits( void ) const
{ 
  if( !isOpen() )
    return -1;
  int maxData = comedi_get_maxdata( DeviceP, Subdevice, 0 );
  return (int)( log( maxData+2.0 )/ log( 2.0 ) );
}

double ComediAnalogInput::maxRate( void ) const 
{ 
  return MaxRate;
}

int ComediAnalogInput::bufferSize( void ) const
{
  if( !isOpen() )
    return -1;
  return comedi_get_buffer_size( DeviceP, Subdevice ) / BufferElemSize;
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

int ComediAnalogInput::maxRanges( void ) const
{
  return UnipolarRangeIndex.size() + BipolarRangeIndex.size();
}

double ComediAnalogInput::unipolarRange( int index ) const
{
  if( (index < 0) || (index >= (int)UnipolarRangeIndex.size()) )
    return -1.0;
  return UnipolarRange[ UnipolarRangeIndex[index] ].max;
  
}

double ComediAnalogInput::bipolarRange( int index ) const
{
  if( (index < 0) || (index >= (int)BipolarRangeIndex.size()) )
    return -1.0;
  return BipolarRange[ BipolarRangeIndex[index] ].max;
}


int ComediAnalogInput::testReadDevice( InList &traces )
{
  if( !isOpen() )
    return -1;

  cerr << " ComediAnalogInput::testReadDevice() " << endl;/////TEST/////  
  ErrorState = 0;
  memset( ChanList, 0, sizeof( ChanList ) );
  memset( &Cmd, 0, sizeof( comedi_cmd ) );

  for( int k = 0; k < traces.size(); k++ ) {

    if( traces[k].delay() > 1.0e-9 ) {
      traces.addError( DaqError::InvalidDelay );
      traces.addErrorStr( "delays are not supported and will be ignored!" );
    }

    int aref = -1;
    switch( traces[k].reference() ) {
    case InData::RefCommon: 
      if( SDF_COMMON  & comedi_get_subdevice_flags( DeviceP, Subdevice ) )
	aref = AREF_COMMON;
      break;
    case InData::RefDifferential:
      if( SDF_DIFF  & comedi_get_subdevice_flags( DeviceP, Subdevice ) )
	aref = AREF_DIFF; 
      break;
    case InData::RefGround:
      if( SDF_GROUND  & comedi_get_subdevice_flags( DeviceP, Subdevice ) )
	aref = AREF_GROUND; 
      break;
    case InData::RefOther: 
      if( SDF_OTHER  & comedi_get_subdevice_flags( DeviceP, Subdevice ) )
	aref = AREF_OTHER;
      break;
    }
    if( aref == -1 )
      traces[k].addError( DaqError::InvalidReference );

    if( traces[k].unipolar() ) {
      double max = unipolarRange( traces[k].gainIndex() );
      if( max < 0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( 0.0 );
      traces[k].setGain( max / comedi_get_maxdata( DeviceP, Subdevice, 0 ) );
      ChanList[k] = CR_PACK( traces[k].channel(), 
			     UnipolarRangeIndex[ traces[k].gainIndex() ], aref );
    }
    else {
      double max = bipolarRange( traces[k].gainIndex() );
      if( max < 0 )
	traces[k].addError( DaqError::InvalidGain );
      traces[k].setMaxVoltage( max );
      traces[k].setMinVoltage( -max );
      traces[k].setGain( 2.0*max / comedi_get_maxdata( DeviceP, Subdevice, 0 ) );
      ChanList[k] = CR_PACK( traces[k].channel(), 
			     BipolarRangeIndex[ traces[k].gainIndex() ], aref );
      cerr << " ComediAnalogInput::testReadDevice(): max = " << max << endl;/////TEST/////  
    }

  }

  /* // i'm not sure if testing works on running device...
  if( running() ) {
    traces.addError( DaqError::Busy );
    return -1;
  }
  */
      
  if( traces[0].sampleRate() * (double)traces.size() > maxRate() ) {
    cerr << " ComediAnalogInput::testReadDevice(): "
	 << "Requested sampling rate bigger than the supported "
	 << maxRate() << "Hz for all channels together!" << endl;/////TEST/////
    traces.addError( DaqError::InvalidSampleRate );
  }

  if( traces.failed() )
    return -1;

  // try automatic command generation
  unsigned int intervalLength = (int)( 1e9 * traces[0].sampleInterval() );  
  int retVal = comedi_get_cmd_generic_timed( DeviceP, Subdevice, &Cmd, traces.size(), intervalLength );
  if( Cmd.scan_begin_arg < intervalLength ) {
    cerr << " ComediAnalogInput::testReadDevice(): "
	 << "Requested sampling rate bigger than supported! max "
	 << Cmd.scan_begin_arg << "ns sampling interval possible." << endl;/////TEST/////
    traces.addError( DaqError::InvalidSampleRate );
  }
  if( retVal < 0 ) {
    Cmd.scan_begin_src = TRIG_TIMER; // when to perform a scan is triggered by timer
    Cmd.scan_begin_arg = intervalLength; // nsec's between 2 scans (1/sampling-frequency)
    Cmd.convert_src = TRIG_TIMER;
    Cmd.convert_arg = intervalLength/ traces.size();
    Cmd.scan_end_src = TRIG_COUNT;
  }

  // adapt command to our purpose
  comedi_cmd testCmd;
  comedi_get_cmd_src_mask( DeviceP, Subdevice, &testCmd );
  if( testCmd.start_src & TRIG_INT )
    Cmd.start_src = TRIG_INT;
  else
    Cmd.start_src = TRIG_NOW;
  Cmd.start_arg = 0;
  if( Cmd.convert_src & TRIG_NOW )
    Cmd.convert_arg = 1;
  Cmd.scan_end_arg = traces.size();

  // test if countinous-state is supported
  if( traces[0].continuous() && !(testCmd.stop_src & TRIG_NONE) ) {
    cerr << " ComediAnalogInput::testReadDevice(): "
	 << " continuous mode not supported!" << endl;/////TEST/////
      traces.addError( DaqError::InvalidContinuous );
      traces.setContinuous( false );
  }
  if( !traces[0].continuous() && !(testCmd.stop_src & TRIG_COUNT) ) {
    cerr << " ComediAnalogInput::testReadDevice(): "
	 << " only continuous mode supported!" << endl;/////TEST/////
      traces.addError( DaqError::InvalidContinuous );
      traces.setContinuous( true );
  }
    
  // set countinous-state
  if( traces[0].continuous() ) {
      Cmd.stop_src = TRIG_NONE;
      Cmd.stop_arg = 0;
    }
  if( !traces[0].continuous() ) {
      Cmd.stop_src = TRIG_COUNT;
      // set length of acquisition as number of scans:
      Cmd.stop_arg = traces[0].size(); // change this if delay should be simulated
    }

  Cmd.chanlist = ChanList;
  Cmd.chanlist_len = traces.size();
  // ATTENTION:  maybe TRIG_WAKE_EOS causes many interrupts!
  Cmd.flags = TRIG_WAKE_EOS | TRIG_RT;
  
  // test command
  retVal = comedi_command_test( DeviceP, &Cmd );
  retVal = comedi_command_test( DeviceP, &Cmd );
  if( retVal ) {
    Cmd.flags &= ~TRIG_RT;
    retVal = comedi_command_test( DeviceP, &Cmd );
    retVal = comedi_command_test( DeviceP, &Cmd );    
    if( retVal ) {
      Cmd.flags = 0;
      retVal = comedi_command_test( DeviceP, &Cmd );
      retVal = comedi_command_test( DeviceP, &Cmd );
    }
  }
  if( retVal != 0 )
    return -retVal;

  return 0;
}


int ComediAnalogInput::prepareRead( InList &traces )
{
  if( !isOpen() )
    return -1;

  reset();
  int error = testReadDevice( traces );
  if( error )
    return error;
  
  // hard-test command:  
  if( Cmd.start_src != TRIG_NOW ) {
    if( comedi_command( DeviceP, &Cmd ) < 0 ) {
      traces.addErrorStr( Devicename + " - execution of comedi_cmd failed: "
			  + comedi_strerror( comedi_errno() ) );
      return -1;
    }
    stop();
  }
  


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

  if ( traces.success() )
    cerr << " ComediAnalogInput::prepareRead(): success" << endl;/////TEST/////

  IsPrepared = traces.success();
  return traces.failed() ? -1 : 0;
}



int ComediAnalogInput::startRead( InList &traces )
{
  cerr << " ComediAnalogInput::startRead(): 1" << endl;/////TEST/////

  if( !prepared() ) {
    traces.addError( DaqError::Unknown );
    return -1;
  }

  ErrorState = 0;
  
 // * set up instructionlist *
  lsampl_t dataAO[1], dataAI[1];
  dataAO[0] = 0;
  dataAI[0] = 0;
  comedi_insnlist il;
  comedi_insn *insnP;
  vector< comedi_insnlist > insnlist;
  vector< comedi_insn* > insn;
  vector< comedi_t* > insnlistDevice;
  int insnlistNr = 0;
  int insnN;
  vector< bool > comediAOsAdded( ComediAOs.size(), false );
  vector< bool > comediAIsAdded( ComediAIs.size(), false );

  // setup start triggers for AOs - synchronize start triggers  for AOs linked to an AI
  for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
    if( ComediAOs[ao]->prepared() && !ComediAOs[ao]->running() 
	&& ComediAOs[ao]->reload() >= 0 ) {

      ComediAOs[ao]->fillWriteBuffer();
      insnP = new comedi_insn[2];
      memset( insnP, 0, sizeof(comedi_insn[2]) );
      insn.push_back( insnP );
      insnlistNr = insn.size() - 1;
      memset( insn[insnlistNr], 0, sizeof(comedi_insn[2]) );
      insn[insnlistNr][0].insn = INSN_INTTRIG;
      insn[insnlistNr][0].n = 1;
      insn[insnlistNr][0].data = dataAO;
      insn[insnlistNr][0].subdev = ComediAOs[ao]->subdevice();
      insnN = 1;
      comediAOsAdded[ao] = true;

      int aiLinked = ComediAOsLink[ao];
      if( aiLinked >= 0
	  && ComediAIs[aiLinked]->prepared() && !ComediAIs[aiLinked]->running()
	  && ComediAIs[aiLinked]->reload() >= 0 ) {
	insn[insnlistNr][1].insn = INSN_INTTRIG;
	insn[insnlistNr][1].n = 1;
	insn[insnlistNr][1].data = dataAI;
	insn[insnlistNr][1].subdev = ComediAIs[aiLinked]->subdevice();
	insnN = 2;
	comediAIsAdded[aiLinked] = true;
	cerr << " ComediAnalogInput::startRead(): " 
	     << "Input device " << ComediAIs[aiLinked]->deviceName() 
	     << " initialized" << endl;/////TEST/////
      }
      else /////TEST/////
	if( ComediAIs[aiLinked]->running() )
	  cerr << " ComediAnalogOutput::startWrite(): Error -> AI-device "
	     << aiLinked << "is already running!"  << endl;

      il.n_insns = insnN;
      il.insns = insn[insnlistNr];
      insnlist.push_back( il );
      insnlistDevice.push_back( ComediAOs[ao]->device() );
      cerr << " ComediAnalogInput::startRead(): " 
	   << "Output device " << ComediAOs[ao]->deviceName() 
	   << " initialized" << endl;/////TEST/////

    }
    else /////TEST/////
      if( ComediAOs[ao]->running() )
	cerr << " ComediAnalogOutput::startWrite(): Error -> AO-device "
	     << ao << "is already running!"  << endl;

  // setup start triggers for remaining (non-linked) AIs
for( unsigned int ai = 0; ai < ComediAIs.size(); ai++ )
  if( !comediAIsAdded[ai] 
      && ComediAIs[ai]->prepared() && !ComediAIs[ai]->running()
      && ComediAIs[ai]->reload() >= 0 ) {
      insnP = new comedi_insn[1];
      memset( insnP, 0, sizeof(comedi_insn[1]) );
      insn.push_back( insnP );
      insnlistNr = insn.size() - 1;
      memset( insn[insnlistNr], 0, sizeof(comedi_insn[1]) );
      insn[insnlistNr][0].insn = INSN_INTTRIG;
      insn[insnlistNr][0].n = 1;
      insn[insnlistNr][0].data = dataAI;
      insn[insnlistNr][0].subdev = ComediAIs[ai]->subdevice();
      comediAIsAdded[ai] = true;

      il.n_insns = 1;
      il.insns = insn[insnlistNr];
      insnlist.push_back( il );
      insnlistDevice.push_back( ComediAOs[ai]->device() );
      cerr << " ComediAnalogInput::startRead(): " 
	   << "Input device " << ComediAIs[ai]->deviceName() 
	   << " initialized" << endl;/////TEST/////
	
  }
  else /////TEST/////
    if( ComediAIs[ai]->running() )
      cerr << " ComediAnalogOutput::startWrite(): Error -> AI-device "
	   << ai << "is already running!"  << endl;


  cerr << " ComediAnalogInput::startRead(): 2" << endl;/////TEST/////
  
 // * start instructionlist *
  int notStarted = 0;
  int insError = 0;  
  for( unsigned int k = 0; k < insnlist.size(); k++ ) {    
    int retVal = comedi_do_insnlist( insnlistDevice[k], &insnlist[k] );
    if( retVal >= 0 )
      notStarted += il.n_insns - retVal;
    else {
      insError = comedi_errno();
    }
  }
  for( unsigned int k = 0; k < insn.size(); k++ )
    delete [] insn[k];

  if( insError || notStarted ) {
    traces.addErrorStr( "ComediAnalogInput::startRead()-> Instruction-setup for (multiple) daq start failed for at least one device: " );
    if( insError )
      traces.addErrorStr( "  comedi -> " + (string)comedi_strerror( comedi_errno() ) );
    for( unsigned int ai = 0; ai < ComediAIs.size(); ai++ )
      if( comediAIsAdded[ai] && !ComediAIs[ai]->loaded() )
	traces.addErrorStr( "  Failure of analog Input on device "
			    + ComediAIs[ai]->deviceName() );
    for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
      if( comediAIsAdded[ao] && !ComediAOs[ao]->loaded() )
	traces.addErrorStr( "  Failure of analog Output on device "
			    + ComediAIs[ao]->deviceName() );
    cerr << " ComediAnalogInput::startRead(): " 
	 << "set up of instruction list failed!"
	 << endl;/////TEST/////
    return -1;
  }
  
  for( unsigned int ai = 0; ai < ComediAIs.size(); ai++ )
    if( comediAIsAdded[ai] ) {
      ComediAIs[ai]->setRunning();
      cerr << " ComediAnalogInput::startRead(): " 
	   << "Device " << ComediAIs[ai]->deviceName()
	   << " set up successfully for analog input"
	   << endl;/////TEST/////
	}
  for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
    if( comediAOsAdded[ao] ) {
      ComediAOs[ao]->setRunning();
      cerr << " ComediAnalogInput::startRead(): " 
	   << "Device " << ComediAIs[ao]->deviceName()
	   << " set up successfully for analog output"
	   << endl;/////TEST/////
	}

  cerr << " ComediAnalogInput::startRead(): 3" << endl;/////TEST/////
  return 0;  
}


int ComediAnalogInput::readData( InList &traces )
{
  if( !isOpen() )
    return -1;

  cerr << " ComediAnalogInput::readData(): in, comedi:" << comedi_strerror( comedi_errno() ) << endl;/////TEST/////

  ErrorState = 0;
  
  bool failed = false;
  int elemRead = 0;
  int bytesRead;
  
  //  sampl_t *buf = NULL;/////TEST/////
  //  int maxElem = 1;/////TEST/////
  // try to read twice
  for ( int tryit = 0;
	tryit < 2 && ! failed && traces[0].deviceBufferMaxPush() > 0;
	tryit++ ){
    
    bytesRead = read( comedi_fileno(DeviceP), traces[0].deviceBufferPushBuffer(), 
    		      traces[0].deviceBufferMaxPush() * BufferElemSize );
    cerr << " ComediAnalogInput::readData():  bytes read:" << bytesRead << endl;/////TEST/////

    if( bytesRead < 0 && errno != EAGAIN && errno != EINTR ) {
      traces.addErrorStr( errno );
      failed = true;
      cerr << " ComediAnalogInput::readData(): error" << endl;/////TEST/////
    }
    else if ( bytesRead > 0 ) {
      traces[0].deviceBufferPush( bytesRead / BufferElemSize );
      elemRead += bytesRead / BufferElemSize;
    }

  }

  if( LongSampleType )
    convert<lsampl_t>( traces );
  else
    convert<sampl_t>( traces );

  if( failed || errno == EAGAIN || errno == EINTR ) {
    // check buffer underrun:
    if( errno == EPIPE ) {
      ErrorState = 1;
      traces.addErrorStr( Devicename + " - buffer-overflow: "
			  + comedi_strerror( comedi_errno() ) );
      traces.addError( DaqError::OverflowUnderrun );
    }    
    else {
      ErrorState = 2;
      traces.addErrorStr( "Error while reading from device-file: " + Devicename
			  + "  comedi: " + comedi_strerror( comedi_errno() ) 
			  + "  system: " + strerror( errno ) );
      traces.addError( DaqError::Unknown );
    }
    cerr << "ComediAnalogInput::readData(): Errorstate = " << ErrorState << endl;
    return -1;   
  }

  cerr << " ComediAnalogInput::readData(): out" << endl;/////TEST/////
  return elemRead;
}


void ComediAnalogInput::take( int syncmode, 
			      vector< AnalogInput* > &ais, 
			      vector< AnalogOutput* > &aos,
			      vector< int > &aiinx, vector< int > &aoinx )
{
  ComediAIs.clear();
  ComediAOs.clear();
  ComediAIsLink.clear();
  ComediAOsLink.clear();
  cerr << " ComediAnalogInput::take(): 1" << endl;/////TEST/////
  bool weAreMember = false;
  for ( unsigned int k=0; k<ais.size(); k++ ) {
    if ( ais[k]->analogInputType() == ComediAnalogInputType ) {
      aiinx.push_back( k );
      ComediAIs.push_back( dynamic_cast< ComediAnalogInput* >( ais[k] ) );
      ComediAIsLink.push_back( -1 );
      if( ComediAIs[k]->deviceName() == deviceName() )
	weAreMember = true;
    }
  }
  if( !weAreMember ) {
    ComediAIs.push_back( this );
    ComediAIsLink.push_back( -1 );
  }
  
  for ( unsigned int k=0; k<aos.size(); k++ ) {
    if ( aos[k]->analogOutputType() == ComediAnalogOutput::ComediAnalogOutputType ) {
      aoinx.push_back( k );
      ComediAOs.push_back( dynamic_cast< ComediAnalogOutput* >( aos[k] ) );
      ComediAOsLink.push_back( -1 );
    }
  }

  // find subdevices to be started together within the same instruction list
  for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
    for( unsigned int ai = 0; ai < ComediAIs.size(); ai++ )
      if( ComediAOs[ao]->deviceName() == ComediAOs[ai]->deviceName() ) {
	ComediAOsLink[ao] = ai;
	ComediAIsLink[ai] = ao;
      }
}
