/*
  rtaicomedi/dynclampdigitalio.cc
  Interface for accessing analog input of a daq-board via comedi and the dynclamp kernel module.

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

#include <cerrno>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <relacs/str.h>
#include <relacs/rtaicomedi/dynclampdigitalio.h>
using namespace std;
using namespace relacs;

namespace rtaicomedi {


  const string DynClampDigitalIO::TTLCommands[7] = { "startwrite", "endwrite", "startread", "endread", "startao", "endao", "none" };


DynClampDigitalIO::DynClampDigitalIO( void ) 
  : DigitalIO( "DynClampDigitalIO" )
{
  CDIO = new comedi::ComediDigitalIO;
  SubDevice = 0;
  MaxLines = 0;
  ModuleFd = -1;

  initOptions();
}


DynClampDigitalIO::DynClampDigitalIO( const string &device, const Options &opts ) 
  : DynClampDigitalIO()
{
  Options::read(opts);
  open( device );
}

  
DynClampDigitalIO::~DynClampDigitalIO( void ) 
{
  close();
  delete CDIO;
}

void DynClampDigitalIO::initOptions()
{
  DigitalIO::initOptions();

  DigitalIO::initOptions();

  addInteger( "subdevice", "Subdevice number of digital I/O device to be used", -1, -1, 100 );
  addInteger( "startsubdevice", "Start searching for digital I/O device at subdevice number", 0, 0, 100 ).setActivation( "subdevice", "<0" );
  string ttlpulsestr = TTLCommands[TTL_UNDEFINED];
  for ( int p = TTL_START_WRITE; p <= TTL_UNDEFINED; ++p )
    ttlpulsestr += '|' + TTLCommands[p];
  for (int i = 1; i < 5; ++i) {
    std::string idx = Str(i);
    addInteger( "ttlpulse" + idx + "line", "DIO line for generating a TTL pulse", -1 );
    addSelection( "ttlpulse" + idx + "high", "Event that sets TTL pulse high", ttlpulsestr );
    addSelection( "ttlpulse" + idx + "low", "Event that sets TTL pulse low", ttlpulsestr );
  }
}

int DynClampDigitalIO::open( const string &device )
{ 
  clearError();
  if ( isOpen() )
    return -5;

  DigitalIO::open( device );
  for ( unsigned int k=0; k<MaxDIOLines;  k++ ) {
    TTLPulseHigh[k] = TTL_UNDEFINED;
    TTLPulseLow[k] = TTL_UNDEFINED;
  }

  // open user space coemdi:
  CDIO->Options::read( *this );
  int retval = CDIO->open( device );
  if ( retval != 0 )
    return retval;

  // copy information not available after CDIO->close()
  SubDevice = CDIO->comediSubdevice();
  MaxLines = CDIO->lines();

  // set basic device infos:
  setDeviceName( CDIO->deviceName() );
  setDeviceVendor( CDIO->deviceVendor() );
  setDeviceFile( device );

  // close user space comedi:
  CDIO->close();

  // open kernel module:
  ModuleDevice = "/dev/dynclamp";
  ModuleFd = ::open( ModuleDevice.c_str(), O_RDONLY );
  if ( ModuleFd == -1 ) {
    setErrorStr( "opening dynclamp-module " + ModuleDevice + " failed" );
    return -1;
  }

  // set device and subdevice:
  struct deviceIOCT deviceIOC;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = SubDevice;
  deviceIOC.subdevType = SUBDEV_DIO;
  deviceIOC.errorstr[0] = '\0';
  retval = ::ioctl( ModuleFd, IOC_OPEN_SUBDEV, &deviceIOC );
  if ( retval < 0 ) {
    setErrorStr( "ioctl command IOC_OPEN_SUBDEV on device " + ModuleDevice + " failed: " +
		 deviceIOC.errorstr );
    ::ioctl( ModuleFd, IOC_REQ_CLOSE, SubDevice );
    ::close( ModuleFd );
    ModuleFd = -1;
    return -1;
  }

  setInfo();

  // set up TTL pulses:
  int id = -1;
  for ( int k=1; k<5; k++ ) {
    string ns = Str( k );
    int line = integer( "ttlpulse" + ns + "line", 0, -1 );
    if ( line < 0 )
      continue;
    int high = index( "ttlpulse" + ns + "high" );
    int low = index( "ttlpulse" + ns + "low" );
    if ( id < 0 ) {
      id = allocateLine( line );
      if ( id == WriteError )
	continue;
    }
    else {
      if ( allocateLine( line, id ) == WriteError )
	continue;
    }
    if ( configureLine( line, true ) < 0 )
      continue;
    addTTLPulse( line, (enum ttlPulses)high, (enum ttlPulses)low );
  }
  
  return 0;
}


bool DynClampDigitalIO::isOpen( void ) const 
{ 
  return ( ModuleFd >= 0 );
}


void DynClampDigitalIO::close( void ) 
{
  clearError();
  if ( ! isOpen() )
    return;

  ::ioctl( ModuleFd, IOC_REQ_CLOSE, SubDevice );
  if ( ::close( ModuleFd ) < 0 )
    setErrorStr( "closing of module file failed" );

  // clear flags:
  SubDevice = 0;
  ModuleFd = -1;

  Info.clear();
  Settings.clear();
}


int DynClampDigitalIO::lines( void ) const
{ 
  if ( !isOpen() )
    return 0;
  return MaxLines;
}


const Options &DynClampDigitalIO::settings( void ) const
{
  DigitalIO::settings();
  
  for ( unsigned int k=0; k<MaxDIOLines; k++ ) {
    if ( TTLPulseHigh[k] != TTL_UNDEFINED || 
	 TTLPulseLow[k] != TTL_UNDEFINED ) {
      Settings.addText( "line"+Str(k)+"_ttlpulsehigh", TTLCommands[TTLPulseHigh[k]] );
      Settings.addText( "line"+Str(k)+"_ttlpulselow", TTLCommands[TTLPulseLow[k]] );
    }
  }

  return Settings;
}


int DynClampDigitalIO::configureLineUnlocked( unsigned int line, bool output )
{
  unsigned int lines = 1 << line;
  unsigned int bits = 0;
  if ( output )
    bits = lines;
  return configureLines( lines, bits );
}


int DynClampDigitalIO::configureLines( unsigned int lines, unsigned int output )
{
  if ( !isOpen() ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_CONFIGURE;
  dioIOC.mask = lines;
  dioIOC.bits = output;
  dioIOC.maxlines = MaxLines;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    string es = "Configuring DIO lines " + Str( lines )
      + " failed on subdevice " + Str( SubDevice )
      + " for direction " + Str( output );
    cerr << "! error: DynClampDigitalIO::configureLines() -> " << es << '\n';
    setErrorStr( es + " with " );
    addErrorStr( comedi_strerror( errno ) );
    return WriteError;
  }
  return DigitalIO::configureLines( lines, output );
}


int DynClampDigitalIO::writeUnlocked( unsigned int line, bool val )
{
  unsigned int mask = 1 << line;
  unsigned int bits = val ? mask : 0;
  return writeLines( mask, bits );
}


int DynClampDigitalIO::writeLines( unsigned int lines, unsigned int val )
{
  if ( !isOpen() ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_WRITE;
  dioIOC.mask = lines;
  dioIOC.bits = val;
  dioIOC.maxlines = MaxLines;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    string es = "Writing to DIO lines " + Str( lines )
      + " with value " + Str( val )
      + " failed on subdevice " + Str( SubDevice );
    cerr << "! error: DynClampDigitalIO::writeLines() -> " << es << '\n';
    setErrorStr( es + " with " );
    addErrorStr( comedi_strerror( errno ) );
    return WriteError;
  }
  if ( (dioIOC.bits & lines ) != ( val & lines ) ) {
    string es = "Failed to write to DIO lines " + Str( lines )
      + " with value " + Str( val )
      + " on subdevice " + Str( SubDevice );
    cerr << "! error: DynClampDigitalIO::writeLines() -> " << es << '\n';
    setErrorStr( es );
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::readUnlocked( unsigned int line, bool &val )
{
  unsigned int mask = 1 << line;
  unsigned int bits = 0;
  val = false;
  int r = readLines( mask, bits );
  if ( r == 0 )
    val = ( bits & mask );
  return r;
}


int DynClampDigitalIO::readLines( unsigned int lines, unsigned int &val )
{
  if ( !isOpen() ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_READ;
  dioIOC.mask = 0;
  dioIOC.bits = 0;
  dioIOC.maxlines = MaxLines;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    string es = "Reading from DIO lines " + Str( lines )
      + " failed on subdevice " + Str( SubDevice );
    cerr << "! error: DynClampDigitalIO::readLines() -> " << es << '\n';
    setErrorStr( es + " with " );
    addErrorStr( comedi_strerror( errno ) );
    return ReadError;
  }
  val = ( dioIOC.bits & lines );
  return 0;
}


int DynClampDigitalIO::addTTLPulse( unsigned int line, enum ttlPulses high,
				    enum ttlPulses low, bool inithigh )
{
  if ( !isOpen() ) 
    return NotOpen;
  string es = "Adding ttl pulse for DIO line " + Str( line ) + " failed on subdevice " + Str( SubDevice ) + ": ";
  if ( line >= MaxLines ) {
    es += "Invalid line " + Str( line );
    addErrorStr( es );
    return WriteError;
  }
  if ( high == TTL_UNDEFINED || low == TTL_UNDEFINED ) {
    es += "Unset high " + TTLCommands[high] + " or low " + TTLCommands[ low ] + " condition.";
    addErrorStr( es );
    return WriteError;
  }
  if ( TTLPulseHigh[line] != TTL_UNDEFINED || 
       TTLPulseLow[line] != TTL_UNDEFINED ) {
    es += "High " + TTLCommands[TTLPulseHigh[line]] + " or low "  + TTLCommands[TTLPulseLow[line]] + " condition already set for line " + Str( line );
    addErrorStr( es );
    return WriteError;
  }
  if ( ! allocatedLine( line ) ) {
    es += "Line " + Str( line ) + " not allocated.";
    addErrorStr( es );
    return WriteError;
  }
  if ( ! lineConfiguration( line ) ) {
    es += "Line " + Str( line ) + " not configured for writing.";
    addErrorStr( es );
    return WriteError;
  }

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_ADD_TTLPULSE;
  dioIOC.mask = 1 << line;
  dioIOC.bits = dioIOC.mask;
  dioIOC.maxlines = MaxLines;
  dioIOC.pulseType = high;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    es += "Adding high pulse failed. ";
    if ( errno == ENOTTY ) {
      es += "Dynamic clamp module is not compiled with ttl pulse support. Enable ttl pulse support by making sure ENABLE_TTLPULSES is defined in moduledef.h";
      addErrorStr( es );
      return InvalidDevice;
    }
    addErrorStr( es );
    return WriteError;
  }
  if ( ! inithigh )
    dioIOC.bits = 0;
  dioIOC.pulseType = low;
  retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    es += "Adding low pulse failed.";
    addErrorStr( es );
    dioIOC.op = DIO_CLEAR_TTLPULSE;
    dioIOC.pulseType = TTL_UNDEFINED;
    ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
    return WriteError;
  }
  TTLPulseHigh[line] = high;
  TTLPulseLow[line] = low;
  return 0;
}


int DynClampDigitalIO::clearTTLPulse( unsigned int line, bool high )
{
  if ( !isOpen() ) 
    return NotOpen;
  string es = "Clearing ttl pulse for DIO line " + Str( line ) + " failed on subdevice " + Str( SubDevice ) + ": ";
  if ( line >= MaxLines ) {
    es += "Invalid line " + Str( line ) + ".";
    addErrorStr( es );
    return WriteError;
  }
  if ( TTLPulseHigh[line] == TTL_UNDEFINED || 
       TTLPulseLow[line] == TTL_UNDEFINED ) {
    es += "High " + TTLCommands[TTLPulseHigh[line]] + " or low "  + TTLCommands[TTLPulseLow[line]] + " condition is not set for line " + Str( line ) + ".";
    addErrorStr( es );
    return WriteError;
  }

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_CLEAR_TTLPULSE;
  dioIOC.mask = 1 << line;
  dioIOC.bits = high ? dioIOC.mask : 0;
  dioIOC.maxlines = MaxLines;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    if ( errno == ENOTTY ) {
      es += "Dynamic clamp module is not compiled with ttl pulse support. Enable ttl pulse support by making sure ENABLE_TTLPULSES is defined in moduledef.h";
      addErrorStr( es );
      return InvalidDevice;
    }
    addErrorStr( es );
    return WriteError;
  }
  TTLPulseHigh[line] = TTL_UNDEFINED;
  TTLPulseLow[line] = TTL_UNDEFINED;
  return 0;
}


int DynClampDigitalIO::setSyncPulse( int modemask, int modebits,
				     unsigned int line, double duration, int mode )
{
  if ( !isOpen() )
    return NotOpen;
  string es = "Setting sync pulse for DIO line " + Str( line ) + " failed on subdevice " + Str( SubDevice ) + ": ";
  long durationns = (long)::round( 1.0e9*duration );
  if ( durationns <= 0 ) {
    es += "Duration " + Str( durationns ) + " ns is not positive.";
    addErrorStr( es );
    return WriteError;
  }
  if ( line >= MaxLines ) {
    es += "Invalid line " + Str( line );
    addErrorStr( es );
    return WriteError;
  }
  if ( ! allocatedLine( line ) ) {
    es += "Line " + Str( line ) + " not allocated.";
    addErrorStr( es );
    return WriteError;
  }
  if ( ! lineConfiguration( line ) ) {
    es += "Line " + Str( line ) + " not configured for writing.";
    addErrorStr( es );
    return WriteError;
  }

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_SET_SYNCPULSE;
  dioIOC.mask = 1 << line;
  dioIOC.bits = dioIOC.mask;
  dioIOC.pulsewidth = durationns;
  dioIOC.intervalmode = mode;
  dioIOC.modemask = modemask;
  dioIOC.modebits = modebits;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    if ( errno == ENOTTY ) {
      es += "Dynamic clamp module is not compiled with syncpulse support. Enable syncpulse support by making sure ENABLE_SYNCSEC is defined in moduledef.h";
      addErrorStr( es );
      return InvalidDevice;
    }
    addErrorStr( es );
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::clearSyncPulse( int modemask, int modebits )
{
  if ( !isOpen() )
    return NotOpen;
  string es = "Clearing sync pulse failed on subdevice " + Str( SubDevice ) + ": ";
  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_CLEAR_SYNCPULSE;
  dioIOC.modemask = modemask;
  dioIOC.modebits = modebits;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    int ern = errno;
    if ( ern == ENOTTY ) {
      es += "Dynamic clamp module is not compiled with syncpulse support. Enable syncpulse support by making sure ENABLE_SYNCSEC is defined in moduledef.h";
      addErrorStr( es );
      return InvalidDevice;
    }
    addErrorStr( es );
    return WriteError;
  }
  return 0;
}


}; /* namespace rtaicomedi */
