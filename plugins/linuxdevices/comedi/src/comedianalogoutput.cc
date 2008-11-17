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
#include <cstdio>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <relacs/str.h>
#include <relacs/comedi/comedianaloginput.h>
#include <relacs/comedi/comedianalogoutput.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediAnalogOutput::ComediAnalogOutput( void ) 
  : AnalogOutput( "Comedi Analog Output", ComediAnalogIOType )
{
  ErrorState = 0;
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxRate = 1000.0;
  Sigs = NULL;
  UnipolarExtRefRangeIndex = -1;
  BipolarExtRefRangeIndex = -1;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
}


ComediAnalogOutput::ComediAnalogOutput(  const string &device, long mode ) 
  : AnalogOutput( "Comedi Analog Output", ComediAnalogIOType )
{
  ErrorState = 0;
  DeviceP = NULL;
  SubDevice = 0;
  LongSampleType = false;
  BufferElemSize = 0;
  MaxRate = 1000.0;
  Sigs = NULL;
  UnipolarExtRefRangeIndex = -1;
  BipolarExtRefRangeIndex = -1;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  open( device, mode );
  IsPrepared = false;
}


ComediAnalogOutput::~ComediAnalogOutput( void ) 
{
  close();
}


int ComediAnalogOutput::open( const string &device, long mode )
{ 
  if ( isOpen() )
    return -5;

  clearSettings();
  if ( device.empty() )
    return InvalidDevice;

  // open comedi device:
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    cerr << "! error: ComediAnalogOutput::open() -> "
	 << "Device-file " << device << " could not be opened!\n";
    return NotOpen;
  }

  // get AO subdevice:
  int subdev = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_AO, 0 );
  if ( subdev < 0 ) {
    cerr << "! error: ComediAnalogOutput::open() -> "
	 << "No SubDevice for AO found on device "  << device << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    return InvalidDevice;
  }
  SubDevice = subdev;

  // lock AI subdevice:
  if ( comedi_lock( DeviceP, SubDevice ) != 0 ) {
    cerr << "! error: ComediAnalogOutput::open() -> "
	 << "Locking of AO SubDevice failed on device " << device << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return NotOpen;
  }  

  // check for async. command support:
  if ( ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_CMD_WRITE ) == 0 ) {
    cerr << "! error: ComediAnalogOutput::open() -> "
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

  // make write calls non blocking:
  fcntl( comedi_fileno( DeviceP ), F_SETFL, O_NONBLOCK );

  // set size of comedi-internal buffer to maximum:
  int bufSize = comedi_get_max_buffer_size( DeviceP, SubDevice );
  comedi_set_buffer_size( DeviceP, SubDevice, bufSize );

  // initialize ranges:
  UnipolarRange.clear();
  BipolarRange.clear();
  UnipolarRangeIndex.clear();
  BipolarRangeIndex.clear();
  UnipolarExtRefRangeIndex = -1;
  BipolarExtRefRangeIndex = -1;
  int nRanges = comedi_get_n_ranges( DeviceP, SubDevice, 0 );  
  for ( int i = 0; i < nRanges; i++ ) {
    comedi_range *range = comedi_get_range( DeviceP, SubDevice, 0, i );
    // TODO: if a ranges is not supported but comedi thinks so: set max = -1.0
    // i.e. NI 6070E PCI: range #3&4 (-1..1V, 0..1V) not supported
    if ( range->min < 0.0 ) {
      if ( range->unit & RF_EXTERNAL )
	BipolarExtRefRangeIndex = i;
      else {	
	BipolarRange.push_back( *range );
	BipolarRangeIndex.push_back( i );
      }
    }
    else {
      if ( range->unit & RF_EXTERNAL )
	UnipolarExtRefRangeIndex = i;
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
  memset( &cmd,0, sizeof(comedi_cmd) );
  int retVal = comedi_get_cmd_generic_timed( DeviceP, SubDevice, &cmd,
					     1 /*chans*/, 1 /*ns*/ );
  if ( retVal < 0 ){
    cerr << "! error in ComediAnalogOutput::open -> cannot get maximum sampling rate from comedi_get_cmd_generic_timed failed: "
	 << comedi_strerror( comedi_errno() ) << endl;
    close();
    return -1;
  }
  else
    MaxRate = 1.0e9 / cmd.scan_begin_arg;

  // clear flags:
  ErrorState = 0;
  ComediAOs.clear();
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
 
  return 0;
}


bool ComediAnalogOutput::isOpen( void ) const 
{ 
  return ( DeviceP != NULL );
}


void ComediAnalogOutput::close( void )
{ 
  if ( ! isOpen() )
    return;

  reset();

  // unlock:
  int error = comedi_unlock( DeviceP, SubDevice );
  if ( error < 0 )
    cerr << "! warning: ComediAnalogOutput::close() -> "
	 << "Unlocking of AO subdevice on device " << deviceFile() << "failed\n";
  
  // close:
  error = comedi_close( DeviceP );
  if ( error )
    cerr << "! warning: ComediAnalogOutput::close() -> "
	 << "Closing of AI subdevice on device " << deviceFile() << "failed.\n";

  // clear flags:
  DeviceP = NULL;
  SubDevice = 0;
  ComediAOs.clear();
  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
}


int ComediAnalogOutput::channels( void ) const
{ 
  if ( !isOpen() )
    return -1;
  return comedi_get_n_channels( DeviceP, SubDevice );
}


int ComediAnalogOutput::bits( void ) const
{ 
  if ( !isOpen() )
    return -1;
  int maxData = comedi_get_maxdata( DeviceP, SubDevice, 0 );
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


template < typename T >
int ComediAnalogOutput::convert( OutList &sigs )
{
  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // set scaling factors:
  int iDelay = sigs[0].indices( sigs[0].delay() );
  double scale[ ol.size() ];
  double offs[ ol.size() ];
  for ( int k=0; k<ol.size(); k++ ) {
    scale[k] = ol[k].scale() * ol[k].gain();
    offs[k] = ol[k].offset() * ol[k].gain();
  }

  // allocate buffer:
  int nbuffer = ol.size() * ( sigs[0].size() + iDelay );
  T *buffer = new T [nbuffer];

  T *bp = buffer;
  // simulate delay:
  for ( int i=0; i<iDelay; i++ ) {
    for ( int k=0; k<ol.size(); k++ ) {
      *bp = (T) ::rint( offs[k] );
      ++bp;
    }
  }
  // convert data and multiplex into buffer:
  for ( int i=0; i<ol[0].size(); i++ ) {
    for ( int k=0; k<ol.size(); k++ ) {
      int v = (T) ::rint( ol[k][i] * scale[k] + offs[k] );
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
  if ( LongSampleType )
    return convert<lsampl_t>( sigs );
  else  
    return convert<sampl_t>( sigs );
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


int ComediAnalogOutput::setupCommand( OutList &sigs, comedi_cmd &cmd )
{
  if ( !isOpen() )
    return -1;

  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;
  unsigned int *chanlist = new unsigned int[512];
  memset( chanlist, 0, sizeof( chanlist ) );
  memset( &cmd, 0, sizeof( comedi_cmd ) );
  
  // ranges:
  int aref = AREF_GROUND;
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
    double minboardvolt = 0.0;
    double maxvolt = sigs[k].getVoltage( max );
    int index = -1;
    for ( int p=0; p<2 && index < 0; p++ ) {
      if ( unipolar ) {
	for( index = UnipolarRange.size() - 1; index >= 0; index-- ) {
	  if ( unipolarRange( index ) > maxvolt ) {
	    maxboardvolt = UnipolarRange[index].max;
	    minboardvolt = UnipolarRange[index].min;
	    break;
	  }
	}
      }
      else {
	for( index = BipolarRange.size() - 1; index >= 0; index-- ) {
	  if ( bipolarRange( index ) > maxvolt ) {
	    maxboardvolt = BipolarRange[index].max;
	    minboardvolt = BipolarRange[index].min;
	    break;
	  }
	}
      }
      // try other polarity?
      if ( index < 0 && p == 0 )
	unipolar = ! unipolar;
    }
    // none of the available ranges contains the requested range:
    if ( index < 0 ) {
      sigs[k].addError( DaqError::InvalidGain );
      break;
    }

    int maxdata = comedi_get_maxdata( DeviceP, SubDevice, sigs[k].channel() );
        
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
	  index = unipolar ? UnipolarExtRefRangeIndex 
	                   :  BipolarExtRefRangeIndex;
	}
      }
      sigs[k].setGain( maxdata/(maxboardvolt-minboardvolt), -minboardvolt );
    }
    else {
      if ( extref && externalReference() < 0.0 ) {
	sigs[k].addError( DaqError::InvalidReference );
	extref = false;
      }
      if ( unipolar )
	sigs[k].setGain( maxdata, 0.0 );
      else
	sigs[k].setGain( maxdata/2.0, 1.0 );
    }

    int gainIndex = index;
    if ( unipolar )
      gainIndex |= 1<<14;
    if ( extref )
      gainIndex |= 1<<15;

    sigs[k].setGainIndex( gainIndex );
    sigs[k].setMinData( 0 );
    sigs[k].setMaxData( maxdata );

    // set up channel in chanlist:
    if ( unipolar )
      chanlist[k] = CR_PACK( sigs[k].channel(),
			     UnipolarRangeIndex[index], aref );
    else
      chanlist[k] = CR_PACK( sigs[k].channel(),
			     BipolarRangeIndex[index], aref );
  }

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
  if ( testCmd.start_src & TRIG_INT )
    cmd.start_src = TRIG_INT;
  else {
    sigs.addError( DaqError::InvalidStartSource );
    sigs.addErrorStr( "Internal trigger not supported" );
  }
  cmd.start_arg = 0;
  cmd.scan_end_arg = sigs.size();
  
  // test if countinous-state is supported:
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
    cmd.stop_arg = sigs[0].size() + sigs[0].indices( sigs[0].delay() );
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
	sigs.addErrorStr( "unsupported trigger in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	sigs.addErrorStr( "unsupported trigger in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	sigs.addErrorStr( "unsupported trigger in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	sigs.addErrorStr( "unsupported trigger in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	sigs.addErrorStr( "unsupported trigger in stop_src" );
      break;
    case 2: // invalid trigger in *_src:
      if ( cmd.start_src != testCmd.start_src )
	sigs.addErrorStr( "invalid trigger in start_src" );
      if ( cmd.scan_begin_src != testCmd.scan_begin_src )
	sigs.addErrorStr( "invalid trigger in scan_begin_src" );
      if ( cmd.convert_src != testCmd.convert_src )
	sigs.addErrorStr( "invalid trigger in convert_src" );
      if ( cmd.scan_end_src != testCmd.scan_end_src )
	sigs.addErrorStr( "invalid trigger in scan_end_src" );
      if ( cmd.stop_src != testCmd.stop_src )
	sigs.addErrorStr( "invalid trigger in stop_src" );
      break;
    case 3: // *_arg out of range:
      if ( cmd.start_arg != testCmd.start_arg )
	sigs.addErrorStr( "start_arg out of range" );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg ) {
	cerr << "! error in ComediAnalogOutput::setupCommand() -> "
	     << "requested sampling period of " << testCmd.scan_begin_arg
	     << "ns smaller than supported! max " << cmd.scan_begin_arg
	     << "ns sampling interval possible." << endl;
	sigs.addError( DaqError::InvalidSampleRate );    
	sigs.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      }
      if ( cmd.convert_arg != testCmd.convert_arg )
	sigs.addErrorStr( "convert_arg out of range" );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	sigs.addErrorStr( "scan_end_arg out of range" );
      if ( cmd.stop_arg != testCmd.stop_arg )
	sigs.addErrorStr( "stop_arg out of range" );
      break;
    case 4: // adjusted *_arg:
      if ( cmd.start_arg != testCmd.start_arg )
	sigs.addErrorStr( "start_arg adjusted" );
      if ( cmd.scan_begin_arg != testCmd.scan_begin_arg ) {
	cerr << "! warning in ComediAnalogOutput::setupCommand() -> "
	     << "requested sampling period of " << testCmd.scan_begin_arg
	     << "ns was adjusted to " << cmd.scan_begin_arg
	     << "ns." << endl;
	//	sigs.addError( DaqError::InvalidSampleRate );    
	sigs.setSampleRate( 1.0e9 / cmd.scan_begin_arg );
      }
      if ( cmd.convert_arg != testCmd.convert_arg )
	sigs.addErrorStr( "convert_arg adjusted" );
      if ( cmd.scan_end_arg != testCmd.scan_end_arg )
	sigs.addErrorStr( "scan_end_arg adjusted" );
      if ( cmd.stop_arg != testCmd.stop_arg )
	sigs.addErrorStr( "stop_arg adjusted" );
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
	// multiple references?
	if ( ( sigs[k].gainIndex() & 1<<15 ) != ( sigs[0].gainIndex() & 1<<15 ) ) {
	  sigs[k].addError( DaqError::MultipleReferences );
	  sigs[k].setGainIndex( sigs[0].gainIndex() );
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
  comedi_cmd cmd;
  memset( &cmd, 0, sizeof( comedi_cmd ) );

  int retVal = setupCommand( sigs, cmd );

  if ( cmd.chanlist != 0 )
    delete [] cmd.chanlist;

  return retVal;
}


int ComediAnalogOutput::prepareWrite( OutList &sigs )
{
  if ( !isOpen() )
    return -1;

  reset();

  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();
  
  Sigs = &sigs;

  if ( setupCommand( ol, Cmd ) < 0 )
    return -1;

  IsPrepared = ol.success();

  if ( ol.success() )
    setSettings( ol );

  //  cerr << " ComediAnalogOutput::prepareWrite(): success" << endl;//////TEST/////

  return sigs.success() ? 0 : -1;
}


int ComediAnalogOutput::executeCommand( void )
{
  ErrorState = 0;
  if ( comedi_command( DeviceP, &Cmd ) < 0 ) {
    cerr << "AO command failed: " << comedi_strerror( comedi_errno() ) << endl;
    /*
    traces.addErrorStr( deviceFile() + " - execution of comedi_cmd failed: "
			+ comedi_strerror( comedi_errno() ) );
    */
    return -1;
  }
  fillWriteBuffer( *Sigs );
  return 0;
}


void ComediAnalogOutput::clearCommand( void )
{
  if ( ! IsPrepared )
    return;

  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;
}


int ComediAnalogOutput::startWrite( OutList &sigs )
{
  //  cerr << " ComediAnalogOutput::startWrite(): begin" << endl;/////TEST/////

  if ( !prepared() ) {
    cerr << "AO not prepared!\n";
    return -1;
  }

  // setup instruction list:
  lsampl_t insndata[1];
  insndata[0] = 0;
  comedi_insnlist insnlist;
  insnlist.n_insns = ComediAOs.size();
  insnlist.insns = new comedi_insn[insnlist.n_insns];
  for ( unsigned int k=0; k<insnlist.n_insns; k++ ) {
    insnlist.insns[k].insn = INSN_INTTRIG;
    insnlist.insns[k].subdev = -1;
    insnlist.insns[k].data = insndata;
    insnlist.insns[k].n = 1;
  }
  bool success = true;
  int ilinx = 0;
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


int ComediAnalogOutput::writeData( OutList &sigs )
{
  //device stopped?
  if ( !running() ) {
    if ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_BUSY ) {
      ErrorState = 1;
      sigs.addError( DaqError::OverflowUnderrun );
    }
    else {
      sigs.addErrorStr( "ComediAnalogOutput::writeData: " +
			deviceFile() + " is not running and not busy!" );
      cerr << "ComediAnalogOutput::writeData: device is not running and not busy! comedi_strerror: " << comedi_strerror( comedi_errno() ) << '\n';
    }
    return -1;
  }

  return fillWriteBuffer( sigs );
}


int ComediAnalogOutput::reset( void ) 
{ 
  //  cerr << " ComediAnalogOutput::reset()" << endl;/////TEST/////

  if ( !isOpen() )
    return NotOpen;
  if ( comedi_cancel( DeviceP, SubDevice ) < 0 )
    return WriteError;

  // clear buffers?
  // by closing and reopening comedi: XXX This closes the whole device, not only the subdevice!
  // the comedi_cancel seems to be sufficient!

  clearSettings();
  ErrorState = 0;
  if ( Cmd.chanlist != 0 )
    delete [] Cmd.chanlist;
  memset( &Cmd, 0, sizeof( comedi_cmd ) );
  IsPrepared = false;

  return 0;
}


bool ComediAnalogOutput::running( void ) const
{   
  return ( comedi_get_subdevice_flags( DeviceP, SubDevice ) & SDF_RUNNING );
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
  ComediAOs.clear();
  for ( unsigned int k=0; k<aos.size(); k++ ) {
    if ( aos[k]->analogOutputType() == ComediAnalogIOType &&
	 aos[k]->deviceFile() == deviceFile() ) {
      aoinx.push_back( k );
      ComediAOs.push_back( dynamic_cast< ComediAnalogOutput* >( aos[k] ) );
    }
  }
}


int ComediAnalogOutput::fillWriteBuffer( OutList &sigs )
{
  if ( !isOpen() ) {
    sigs.setError( DaqError::DeviceNotOpen );
    return -1;
  }

  ErrorState = 0;

  if ( sigs[0].deviceBufferMaxPop() <= 0 ) {
    sigs.addError( DaqError::NoData );
    return -1;
  }

  int ern = 0;
  int elemWritten = 0;

  // try to write twice
  for ( int tryit = 0;
	tryit < 2 && sigs[0].deviceBufferMaxPop() > 0; 
	tryit++ ){

    int bytesWritten = write( comedi_fileno(DeviceP),
			      sigs[0].deviceBufferPopBuffer(),
			      sigs[0].deviceBufferMaxPop() * BufferElemSize );
      
    if ( bytesWritten < 0 ) {
      ern = errno;
      if ( ern == EAGAIN || ern == EINTR ) {
	ern = 0;
	break;
      }
    }
    else if ( bytesWritten > 0 ) {
      sigs[0].deviceBufferPop( bytesWritten / BufferElemSize );
      elemWritten += bytesWritten / BufferElemSize;
    }
  }

  if ( ern == 0 ) {
    // no more data:
    if ( sigs[0].deviceBufferMaxPop() == 0 )
      return 0;
  }
  else {
    // error:
    switch( ern ) {

    case EPIPE: 
      ErrorState = 1;
      sigs.addError( DaqError::OverflowUnderrun );
      return -1;

    case EBUSY:
      ErrorState = 2;
      sigs.addError( DaqError::Busy );
      return -1;

    default:
      ErrorState = 2;
      sigs.addErrorStr( ern );
      sigs.addError( DaqError::Unknown );
      return -1;
    }
  }
  
  return elemWritten;
}


comedi_t* ComediAnalogOutput::comediDevice( void ) const
{
  return DeviceP;
}


int ComediAnalogOutput::comediSubdevice( void ) const
{
  if ( !isOpen() )
    return -1;
  return SubDevice;
}


int ComediAnalogOutput::bufferSize( void ) const
{
  if( !isOpen() )
    return -1;
  return comedi_get_buffer_size( DeviceP, SubDevice ) / BufferElemSize;
}


bool ComediAnalogOutput::prepared( void ) const 
{ 
  return IsPrepared;
}


}; /* namespace comedi */
