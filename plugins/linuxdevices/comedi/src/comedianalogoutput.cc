/*
  comedi/comedianalogoutput.cc
  Interface for accessing analog output of a daq-board via comedi.

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
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <relacs/comedi/comedianaloginput.h>
#include <relacs/comedi/comedianalogoutput.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediAnalogOutput::ComediAnalogOutput( void ) 
  : AnalogOutput( "Comedi Analog Output", ComediAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  AsyncMode = true;
  DeviceP = NULL;
  Sigs = NULL;
  UnipolarExtRefRangeIndex = -1;
  BipolarExtRefRangeIndex = -1;
}


ComediAnalogOutput::ComediAnalogOutput(  const string &device, long mode ) 
  : AnalogOutput( "Comedi Analog Output", ComediAnalogIOType )
{
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;
  AsyncMode = true;
  DeviceP = NULL;
  Sigs = NULL;
  UnipolarExtRefRangeIndex = -1;
  BipolarExtRefRangeIndex = -1;
  open( device, mode );
}


ComediAnalogOutput::~ComediAnalogOutput( void ) 
{
  close();
}


int ComediAnalogOutput::open( const string &device, long mode )
{ 
  if( DeviceP )
    close();
  clearSettings();
  if ( device.empty() )
    return InvalidDevice;

  int retVal;
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    cerr <</* currentTime() <<*/ " !  ComediAnalogOutput::open() -> "
	 << "Device-file " << device << " could not be opened!" << endl;
    return NotOpen;
  }

  Subdevice = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_AO, 0 );
  if( Subdevice < 0 ) {
    cerr << /*currentTime() <<*/ " !  ComediAnalogOutput::open() -> "
	 << "No Subdevice for analog output found on device "  << device
	 << endl;
    DeviceP = NULL;
    return InvalidDevice;
  }

  if( comedi_lock( DeviceP, Subdevice ) != 0 ) {
    cerr << /*currentTime() <<*/ " !  ComediAnalogOutput::open() -> "
	 << "Locking of analog output Subdevice failed on device " << device
	 << endl;
    DeviceP = NULL;
    return NotOpen;
  }  

  if( AsyncMode && 
      ! ( SDF_CMD & comedi_get_subdevice_flags( DeviceP, Subdevice ) ) ) {
    cerr << /*currentTime() <<*/ " !  ComediAnalogOutput::open() -> "
	 << "Device "  << device << " not supported! "
	 << "Subdevice needs to support async. commands!" << endl;
    return InvalidDevice;
  }
  setDeviceName( comedi_get_board_name( DeviceP ) );
  setDeviceVendor( "unknown" );
  setDeviceFile( device );

  // set comedi file-descriptor to non-blocking writing mode
  retVal = fcntl( comedi_fileno(DeviceP), F_SETFL, O_NONBLOCK );//|O_ASYNC
  if( retVal < 0 )
    cerr << /*currentTime() <<*/ " !  ComediAnalogOutput::open() -> "
	 << "Switching the device "  << device 
	 << " to non-blocking mode failed: " << strerror( errno ) << endl;

  // set size of comedi-internal buffer to maximum
  int bufSize = comedi_get_max_buffer_size( DeviceP, Subdevice );
  comedi_set_buffer_size( DeviceP, Subdevice, bufSize );

  // initialize ranges
  BipolarRange.clear();
  BipolarRangeIndex.clear();
  UnipolarRange.clear();
  UnipolarRangeIndex.clear();
  UnipolarExtRefRangeIndex = -1;
  BipolarExtRefRangeIndex = -1;
  int nRanges = comedi_get_n_ranges( DeviceP, Subdevice, 0 );  
  for ( int iRange = 0; iRange < nRanges; iRange++ ) {
    comedi_range *range = comedi_get_range( DeviceP, Subdevice, 0, iRange );
    // TODO: if a ranges is not supported but comedi thinks so: set max = -1.0
    // i.e. NI 6070E PCI: iRange 3&4 (-1..1V, 0..1V) not supported
    if( range->min < 0.0 ) {
      if( range->unit & RF_EXTERNAL )
	BipolarExtRefRangeIndex = iRange;
      else {	
	BipolarRange.push_back( *range );
	BipolarRangeIndex.push_back( BipolarRangeIndex.size() );
      }
    }
    else {
      if( range->unit & RF_EXTERNAL )
	UnipolarExtRefRangeIndex = iRange;
      else {	
	UnipolarRange.push_back( *range );
	UnipolarRangeIndex.push_back( UnipolarRangeIndex.size() );
      }
    }
  }

  // bubble-sort Uni/BipolarRangeIndex descendingly according to Uni/BipolarRange.max
  for( unsigned int i = 0; i < UnipolarRangeIndex.size(); i++ ) {
    for ( unsigned int j = i+1; j < UnipolarRangeIndex.size(); j++ )
      if(  UnipolarRange[ UnipolarRangeIndex[i] ].max
         < UnipolarRange[ UnipolarRangeIndex[j] ].max ) {
	unsigned int iSwap = UnipolarRangeIndex[i];
	UnipolarRangeIndex[i] = UnipolarRangeIndex[j];
	UnipolarRangeIndex[j] = iSwap;
      }
  }
  for( unsigned int i = 0; i < BipolarRangeIndex.size(); i++ ) {
    for ( unsigned int j = i+1; j < BipolarRangeIndex.size(); j++ )
      if(  BipolarRange[ BipolarRangeIndex[i] ].max
         < BipolarRange[ BipolarRangeIndex[j] ].max ) {
	unsigned int iSwap = BipolarRangeIndex[i];
	BipolarRangeIndex[i] = BipolarRangeIndex[j];
	BipolarRangeIndex[j] = iSwap;
      }
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
  else //set default for NI E-series (mio-driver doesn't return a valid frequency)
    MaxRate = 1.0e6; 
 
  return 0;
}


bool ComediAnalogOutput::isOpen( void ) const 
{ 
  return DeviceP != NULL;
}


void ComediAnalogOutput::close( void )
{ 
  reset();
  if( !isOpen() )
    return;
  comedi_unlock( DeviceP, Subdevice );
  int error = comedi_close( DeviceP );
  if( error )
    cerr << /*currentTime() <<*/ " !  ComediAnalogOutput::close() -> "
	 << "Closing of analog out subdevice on device " << deviceFile()
	 << "threw an error. Forcing close..."
	 << endl;
  DeviceP = NULL;
}


int ComediAnalogOutput::channels( void ) const
{ 
  if( !isOpen() )
    return -1;
  return comedi_get_n_channels( DeviceP, Subdevice);
}


int ComediAnalogOutput::bits( void ) const
{ 
  if( !isOpen() )
    return -1;
  int maxData = comedi_get_maxdata( DeviceP, Subdevice, 0 );
  return (int)( log( maxData+2.0 )/ log( 2.0 ) );
}


double ComediAnalogOutput::maxRate( void ) const 
{ 
  return MaxRate;
}


int ComediAnalogOutput::maxRanges( void ) const
{
  return ::std::max( UnipolarRangeIndex.size(), BipolarRangeIndex.size() );
}


double ComediAnalogOutput::unipolarRange( int index ) const
{
  if( (index < 0) || (index >= (int)UnipolarRangeIndex.size()) )
    return -1.0;
  return UnipolarRange[ UnipolarRangeIndex[index] ].max;
  
}


double ComediAnalogOutput::bipolarRange( int index ) const
{
  if( (index < 0) || (index >= (int)BipolarRangeIndex.size()) )
    return -1.0;
  return BipolarRange[ BipolarRangeIndex[index] ].max;
}


template < typename T >
int ComediAnalogOutput::convert( OutList &sigs )
{
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

  return 0;
}


int ComediAnalogOutput::convertData( OutList &sigs )
{
  if( LongSampleType )
    return convert<lsampl_t>( sigs );// default for NI E-series, since the mio-driver does not return a valid frequency)
  
  return convert<sampl_t>( sigs );
}


int ComediAnalogOutput::testWriteDevice( OutList &sigs )
{
  if( !isOpen() )
    return -1;

  ErrorState = 0;

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
  
  int aref = AREF_GROUND;

  memset( ChanList, 0, sizeof( ChanList ) );
  memset( &Cmd, 0, sizeof( comedi_cmd ) );

  // ranges:
  int maxrange = 1 << bits();
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
    double maxvolt = sigs[k].getVoltage( max );
    int index = unipolar ? UnipolarRangeIndex.size() - 1 
                         :  BipolarRangeIndex.size() - 1;
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
    if( index < 0 )
      sigs[k].addError( DaqError::InvalidReference );
        
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
	  index = unipolar ? UnipolarExtRefRangeIndex 
	                   :  BipolarExtRefRangeIndex;
	}
      }
      sigs[k].setGain( unipolar ? maxrange/maxboardvolt : maxrange/2/maxboardvolt );
    }
    else {
      if ( extref && externalReference() < 0.0 ) {
	sigs[k].addError( DaqError::InvalidReference );
	extref = false;
      }
      sigs[k].setGain( unipolar ? maxrange : maxrange/2 );
    }

    int gainIndex = index;
    if ( unipolar )
      gainIndex |= 1<<14;
    if ( extref )
      gainIndex |= 1<<15;
    
    // multiple references?
    if ( ( gainIndex & 1<<15 ) != ( sigs[0].gainIndex() & 1<<15 ) ) {
      sigs[k].addError( DaqError::MultipleReferences );
      gainIndex = sigs[0].gainIndex();
    }

    sigs[k].setGainIndex( gainIndex );
    sigs[k].setMinData( unipolar ? 0 : -maxrange/2 );
    sigs[k].setMaxData( unipolar ? maxrange - 1 : maxrange/2 - 1 );

    // set up channel in chanlist:
    if( unipolar )
      ChanList[k] = CR_PACK( sigs[k].channel(), UnipolarRangeIndex[ index ], aref );
    else
      ChanList[k] = CR_PACK( sigs[k].channel(), BipolarRangeIndex[ index ], aref );

  }


  /* // i'm not sure if testing works on running device...
  if( running() ) {
    sigs.addError( DaqError::Busy );
    return -1;
  }
  */

  if( sigs[0].sampleRate() * double( sigs.size() ) > maxRate() ) {
    cerr << " ComediAnalogOutput::testWriteDevice(): "
	 << "Requested sampling rate bigger than the supported " /////TEST/////
	 << maxRate() << "Hz for all channels together!" << endl;
    sigs.addError( DaqError::InvalidSampleRate );
  }

  if( sigs.failed() )
    return -1;
  
  // try automatic command generation
  unsigned int intervalLength = (int)( 1e9 * sigs[0].sampleInterval() );
  int retVal = comedi_get_cmd_generic_timed( DeviceP, Subdevice, &Cmd, sigs.size(), intervalLength );
  if( Cmd.scan_begin_arg < intervalLength ) {
    cerr << " ComediAnalogOutput::testWriteDevice(): "
	 << "Requested sampling rate bigger than supported! max "
	 << Cmd.scan_begin_arg << "ns sampling interval possible." << endl;/////TEST/////
    sigs.addError( DaqError::InvalidSampleRate );    
  }
  if( retVal < 0 ) {
    Cmd.scan_begin_src = TRIG_TIMER; // when to perform a scan is triggered by timer
    Cmd.scan_begin_arg = intervalLength; // nsec's between 2 scans (1/sampling-frequency)
    Cmd.convert_src = TRIG_TIMER;
    Cmd.convert_arg = intervalLength/ sigs.size();
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
  Cmd.scan_end_arg = sigs.size();
  
  // test if countinous-state is supported
  if( sigs[0].continuous() && !(testCmd.stop_src & TRIG_NONE) ) {
    cerr << " ComediAnalogOutput::testWriteDevice(): "
	 << " continuous mode not supported!" << endl;/////TEST/////
    sigs.addError( DaqError::InvalidContinuous );
    sigs.setContinuous( false );
  }
  if( !sigs[0].continuous() && !(testCmd.stop_src & TRIG_COUNT) ) {
    cerr << " ComediAnalogOutput::testWriteDevice(): "
	 << " only continuous mode supported!" << endl;/////TEST/////
    sigs.addError( DaqError::InvalidContinuous );
    sigs.setContinuous( true );
  }
    
  // set countinous-state
  if( sigs[0].continuous() ) {
      Cmd.stop_src = TRIG_NONE;
      Cmd.stop_arg = 0;
    }
  if( !sigs[0].continuous() ) {
      Cmd.stop_src = TRIG_COUNT;
      // set length of acquisition as number of scans:
      Cmd.stop_arg = sigs[0].size() + sigs[0].indices( sigs[0].delay());
  }

  Cmd.chanlist = ChanList;
  Cmd.chanlist_len = sigs.size();
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


int ComediAnalogOutput::prepareWrite( OutList &sigs )
{
  if( !isOpen() )
    return -1;

  reset();

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  int error = testWriteDevice( ol );
  if( error )
    return error;

  // hard-test command:  
  if( Cmd.start_src != TRIG_NOW ) {
    if( comedi_command( DeviceP, &Cmd ) < 0 ) {
      sigs.addErrorStr( deviceFile() + " - execution of comedi_cmd failed: "
			  + comedi_strerror( comedi_errno() ) );
      return -1;
    }
    stop();
  }

  if ( ol.success() )
    setSettings( ol );

    cerr << " ComediAnalogOutput::prepareWrite(): success" << endl;/////TEST/////

  IsPrepared = true;
  
  // prefill device buffer with data
  // PROBLEM: how to fill data if Cmd.start_src == TRIG_NOW ?

  Sigs = &sigs;

  return 0; //elemWritten;
}


int ComediAnalogOutput::startWrite( OutList &sigs )
{
  cerr << " ComediAnalogOutput::startWrite(): 1" << endl;/////TEST/////

  if( !prepared() ) {
    sigs.addError( DaqError::Unknown );
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
  //  vector< bool > comediAIsAdded( ComediAIs.size(), false );

  // setup start triggers for AOs - synchronize start triggers for AOs linked to an AI
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
      /* XXXX  revise XXXXX
      if( aiLinked >= 0
	  && ComediAIs[aiLinked]->prepared() && !ComediAIs[aiLinked]->running()
	  && ComediAIs[aiLinked]->reload() >= 0 ) {
	insn[insnlistNr][1].insn = INSN_INTTRIG;
	insn[insnlistNr][1].n = 1;
	insn[insnlistNr][1].data = dataAI;
	insn[insnlistNr][1].subdev = ComediAIs[aiLinked]->subdevice();
	insnN = 2;
	comediAIsAdded[aiLinked] = true;
	cerr << " ComediAnalogOutput::startWrite(): " 
	     << "Input device " << ComediAIs[aiLinked]->deviceFile() 
	     << " initialized" << endl;/////TEST/////
      }
      else /////TEST/////
	if( ComediAIs[aiLinked]->running() )
	  cerr << " ComediAnalogOutput::startWrite(): Error -> AI-device "
	     << aiLinked << "is already running!"  << endl;
      */
      il.n_insns = insnN;
      il.insns = insn[insnlistNr];
      insnlist.push_back( il );
      insnlistDevice.push_back( ComediAOs[ao]->device() );
      cerr << " ComediAnalogOutput::startWrite(): " 
	   << "Output device " << ComediAOs[ao]->deviceFile() 
	   << " initialized" << endl;/////TEST/////
    }
    else /////TEST/////
      if( ComediAOs[ao]->running() )
	cerr << " ComediAnalogOutput::startWrite(): Error -> AO-device "
	     << ao << "is already running!"  << endl;
  

      

  // setup start triggers for remaining (non-linked) AIs
  /* XXXX revise XXXX
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
      cerr << " ComediAnalogOutput::startWrite(): " 
	   << "Input device " << ComediAIs[ai]->deviceFile() 
	   << " initialized" << endl;/////TEST/////
  }
  else /////TEST/////
    if( ComediAIs[ai]->running() )
      cerr << " ComediAnalogOutput::startWrite(): Error -> AI-device "
	   << ai << "is already running!"  << endl;
  */  
  cerr << " ComediAnalogOutput::startWrite(): 2" << endl;/////TEST/////

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
    sigs.addErrorStr( "ComediAnalogOutput::startWrite()-> Instruction-setup for (multiple) daq start failed for at least one device: " );
    if( insError )
      sigs.addErrorStr( "  comedi -> " + (string)comedi_strerror( comedi_errno() ) );
    // usleep() here ???
    /* XXX revise
    for( unsigned int ai = 0; ai < ComediAIs.size(); ai++ )
      if( comediAIsAdded[ai] && !ComediAIs[ai]->loaded() )
	sigs.addErrorStr( "  Failure of analog Input on device "
			    + ComediAIs[ai]->deviceFile() );
    for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
      if( comediAIsAdded[ao] && !ComediAOs[ao]->loaded() )
	sigs.addErrorStr( "  Failure of analog Output on device "
			    + ComediAIs[ao]->deviceFile() );
    */
    return -1;
  }
  /* XXX revise
  for( unsigned int ai = 0; ai < ComediAIs.size(); ai++ )
    if( comediAIsAdded[ai] ) {
      ComediAIs[ai]->setRunning();
      cerr << " ComediAnalogOutput::startWrite(): " 
	   << "Device " << ComediAIs[ai]->deviceFile()
	   << " set up successfully for analog input"
	   << endl;/////TEST/////
	}
  for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
    if( comediAOsAdded[ao] ) {
      ComediAOs[ao]->setRunning();
      cerr << " ComediAnalogOutput::startWrite(): " 
	   << "Device " << ComediAIs[ao]->deviceFile()
	   << " set up successfully for analog output"
	   << endl;/////TEST/////
	}
  */
  
  cerr << " ComediAnalogOutput::startWrite(): 3" << endl;/////TEST/////
  
  return 0; //elemWritten;  
}


int ComediAnalogOutput::writeData( OutList &sigs )
{
  cerr << " ComediAnalogOutput::writeData(): in, comedi:" << comedi_strerror( comedi_errno() ) << endl;/////TEST/////

  //device stopped?
  if( !running() ) {
    sigs.addErrorStr( "ComediAnalogOutput::writeData: " +
		      deviceFile() + " is not running!" );
    cerr << "ComediAnalogOutput::writeData: device is not running!"  << endl;/////TEST/////
    return 0;/////TEST/////
  }

  return fillWriteBuffer();
}


int ComediAnalogOutput::reset( void ) 
{ 
  cerr << " ComediAnalogOutput::reset()" << endl;/////TEST/////

  if( !isOpen() )
    return NotOpen;
  if( comedi_cancel( DeviceP, Subdevice ) < 0 )
    return WriteError;

  clearSettings();
  ErrorState = 0;
  IsPrepared = false;
  IsRunning = false;

  return 0;
}


bool ComediAnalogOutput::running( void ) const
{   
  if( !loaded() ) {
    if( IsRunning )
      cerr << " ComediAnalogOutput::running(): stopped!"  << endl;
    IsRunning = false;
  }
  if( IsRunning )
    cerr << " ComediAnalogOutput::running(): running"  << endl;
  else
    cerr << " ComediAnalogOutput::running(): not running"  << endl;
  return IsRunning;
}


int ComediAnalogOutput::error( void ) const
{
  return ErrorState;
  /*
    0: ok
    1: OverflowUnderrun
    2: Unknown (device error)
  */

}


void ComediAnalogOutput::take( const vector< AnalogOutput* > &aos,
			       vector< int > &aoinx )
{
  cerr << " ComediAnalogOutput::take(): 1" << endl;/////TEST/////
  ComediAOs.clear();
  ComediAOsLink.clear();

  /* XXX Needs to be revised!!!! XXX
  bool weAreMember = false;
  for ( unsigned int k=0; k<aos.size(); k++ ) {
    if ( aos[k]->analogOutputType() == ComediAnalogIOType ) {
      aoinx.push_back( k );
      ComediAOs.push_back( dynamic_cast< ComediAnalogOutput* >( aos[k] ) );
      ComediAOsLink.push_back( -1 );
      if( ComediAOs[k]->deviceFile() == deviceFile() )
	weAreMember = true;

    }
  }
  if( !weAreMember ) {
    ComediAOs.push_back( this );
    ComediAOsLink.push_back( -1 );
  }

  // find subdevices to be started together within the same instruction list
  for( unsigned int ao = 0; ao < ComediAOs.size(); ao++ )
    if( ComediAOs[ao]->deviceFile() == ComediAOs[ai]->deviceFile() ) {
      ComediAOsLink[ao] = ai; /// XXXX
    }
  */
}


int ComediAnalogOutput::fillWriteBuffer( void )
{
  if( !isOpen() )
    return -1;

  ErrorState = 0;
  cerr << "ComediAnalogOutput::writeData: size of device buffer: " 
       << (*Sigs)[0].deviceBufferSize() << " - size of outdata: " 
       << " - continuous: " << (*Sigs)[0].continuous() << endl;
  bool failed = false;
  int elemWritten = 0;
  int bytesWritten;

  if( (*Sigs)[0].deviceBufferMaxPop() <= 0 ) {
    //    (*Sigs).addErrorStr( "ComediAnalogOutput::writeData: " +
    //		      deviceFile() + " - buffer-underrun in outlist!" );
    //    (*Sigs).addError( DaqError::OverflowUnderrun );
    cerr << "ComediAnalogOutput::writeData: buffer-underrun in outlist!"  
	 << endl;/////TEST/////
    //    (*Sigs)[0].deviceBufferReset();/////TEST////
    return 0;/////TEST////
  }
  // try to write twice
  for ( int tryit = 0;
	tryit < 2 && !failed && (*Sigs)[0].deviceBufferMaxPop() > 0; 
	tryit++ ){
    
    bytesWritten = write( comedi_fileno(DeviceP), (*Sigs)[0].deviceBufferPopBuffer(),
			  (*Sigs)[0].deviceBufferMaxPop() * BufferElemSize );
    cerr << " ComediAnalogOutput::writeData():  bytes written:" << bytesWritten << endl;/////TEST/////

    if( bytesWritten < 0 && errno != EAGAIN && errno != EINTR ) {
      (*Sigs).addErrorStr( errno );
      failed = true;
      cerr << " ComediAnalogOutput::writeData(): error" << endl;/////TEST/////
    }
    else if ( bytesWritten > 0 ) {
      (*Sigs)[0].deviceBufferPop( bytesWritten / BufferElemSize );
      elemWritten += bytesWritten / BufferElemSize;
    }

  }

  if( failed || errno == EINTR )
    switch( errno ) {

    case EPIPE: 
      ErrorState = 1;
      (*Sigs).addErrorStr( deviceFile() + " - buffer-underrun: "
			+ comedi_strerror( comedi_errno() ) );
      (*Sigs).addError( DaqError::OverflowUnderrun );
      cerr << " ComediAnalogOutput::writeData(): buffer-underrun: "
	   << comedi_strerror( comedi_errno() ) << endl;/////TEST/////
      return -1;

    case EBUSY:
      ErrorState = 2;
      (*Sigs).addErrorStr( deviceFile() + " - device busy: "
			+ comedi_strerror( comedi_errno() ) );
      (*Sigs).addError( DaqError::Busy );
      cerr << " ComediAnalogOutput::writeData(): device busy: "
	   << comedi_strerror( comedi_errno() ) << endl;/////TEST/////
      return -1;

    default:
      ErrorState = 2;
      (*Sigs).addErrorStr( "Error while writing to device-file: " + deviceFile()
			+ "  comedi: " + comedi_strerror( comedi_errno() )
			+ "  system: " + strerror( errno ) );
      cerr << " ComediAnalogOutput::writeData(): buffer-underrun: "
	   << "  comedi: " << comedi_strerror( comedi_errno() ) 
	   << "  system: " << strerror( errno )
	
	   << endl;/////TEST/////
      (*Sigs).addError( DaqError::Unknown );
      return -1;
    }
  
  cerr << " ComediAnalogOutput::writeData(): out" << endl;/////TEST/////
  return elemWritten;
}


comedi_t* ComediAnalogOutput::device( void ) const
{
  return DeviceP;
}


int ComediAnalogOutput::subdevice( void ) const
{
  if( !isOpen() )
    return -1;
  return Subdevice;
}


int ComediAnalogOutput::bufferSize( void ) const
{
  if( !isOpen() )
    return -1;
  return comedi_get_buffer_size( DeviceP, Subdevice ) / BufferElemSize;
}



int ComediAnalogOutput::reload( void )
{
  cerr << " ComediAnalogOutput::reload()" << endl;/////TEST/////
  if( !isOpen() )
    return -1;
  if( loaded() )
    return 0;

  if( !prepared() ||  comedi_command( DeviceP, &Cmd ) < 0 ) {
    cerr << "ComediAnalogOutput::reload()-> " << deviceFile()
	 <<" - execution of comedi_cmd failed: " << comedi_strerror( comedi_errno() );
    cerr << " ComediAnalogOutput::reload() -> ERROR!" << endl;/////TEST/////
    return WriteError;
  }
  return 0;
}


bool ComediAnalogOutput::prepared( void ) const 
{ 
  return IsPrepared;
}


bool ComediAnalogOutput::loaded( void ) const 
{ 
  if( !isOpen() )
    return false;
  return SDF_RUNNING & comedi_get_subdevice_flags( DeviceP, Subdevice );
}


void ComediAnalogOutput::setRunning( void )
{
  IsRunning = true;
}


}; /* namespace comedi */
