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


DynClampDigitalIO::DynClampDigitalIO( void ) 
  : DigitalIO( "DynClampDigitalIO" )
{
  CDIO = new comedi::ComediDigitalIO;
  SubDevice = 0;
  MaxLines = 0;
  ModuleFd = -1;
  SyncLine = 0;

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

  for (int i = 1; i < 5; ++i)
  {
    std::string idx = Str(i);
    addInteger("ttlpulse" + idx + "line", "dummy description", -1);
    addText("ttlpulse" + idx + "high", "dummy description", -1);
    addText("ttlpulse" + idx + "low", "dummy description", -1);
  }
  addInteger("syncpulseline", "dummy description", -1);
  addNumber("syncpulsewidth", "dummy description", 0.0, "s");
}

int DynClampDigitalIO::open( const string &device)
{ 
  clearError();
  if ( isOpen() )
    return -5;

  DigitalIO::open( device );
  for ( int k=0; k<MaxDIOLines;  k++ ) {
    TTLPulseHigh[k] = TTL_UNDEFINED;
    TTLPulseLow[k] = TTL_UNDEFINED;
  }

  // open user space coemdi:
  int retval = CDIO->open( device );
  if ( retval != 0 ) {
    return retval;
  }

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
  deviceIOC.fifoSize = 0;
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
  const string ttlcoms[6] = { "startwrite", "endwrite", "startread", "endread", "startao", "endao" };
  int id = -1;
  for ( int k=1; k<5; k++ ) {
    string ns = Str( k );
    int line = integer( "ttlpulse" + ns + "line", 0, -1 );
    if ( line < 0 )
      continue;
    string highcom = text( "ttlpulse" + ns + "high", 0 );
    int high = TTL_START_WRITE;
    for ( ; high != TTL_UNDEFINED && highcom != ttlcoms[high]; ++high );
    string lowcom = text( "ttlpulse" + ns + "low", 0 );
    int low = TTL_START_WRITE;
    for ( ; low != TTL_UNDEFINED && lowcom != ttlcoms[low]; ++low );
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

  // set up SEC synchronization:
  {
    int line = integer( "syncpulseline", 0, -1 );
    if ( line >= 0 ) {
      double duration = number( "syncpulsewidth", 0.0, "s" );
      int id = allocateLine( line );
      if ( id == WriteError )
	setErrorStr( "failed to allocate line " + Str( line ) + " for sync pulse" );
      else if ( configureLine( line, true ) < 0 )
	setErrorStr( "failed to configure line " + Str( line ) + " for sync pulse for writing" );
      else {
	setSyncPulse( line, duration );
	SyncLine = line;
      }
    }
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
  
  const string ttlcoms[7] = { "startwrite", "endwrite", "startread", "endread", "startao", "endao", "none" };
  for ( int k=0; k<MaxDIOLines; k++ ) {
    if ( TTLPulseHigh[k] != TTL_UNDEFINED || 
	 TTLPulseLow[k] != TTL_UNDEFINED ) {
      Settings.addText( "line"+Str(k)+"_ttlpulsehigh", ttlcoms[TTLPulseHigh[k]] );
      Settings.addText( "line"+Str(k)+"_ttlpulselow", ttlcoms[TTLPulseLow[k]] );
    }
  }

  return Settings;
}


int DynClampDigitalIO::configureLine( int line, bool output )
{
  if ( !isOpen() ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines )
    return WriteError;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_CONFIGURE;
  dioIOC.lines = line;
  dioIOC.output = output;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::configureLine() -> "
	 << "Configuring DIO line " << line
	 << " failed on subdevice " << SubDevice
	 << " for direction " << output << '\n';
    return WriteError;
  }
  return DigitalIO::configureLine( line, output );
}


int DynClampDigitalIO::configureLines( int lines, int output )
{
  if ( !isOpen() ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 1;
  dioIOC.op = DIO_CONFIGURE;
  dioIOC.lines = lines;
  dioIOC.output = output;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::configureLines() -> "
	 << "Configuring DIO lines " << lines
	 << " failed on subdevice " << SubDevice
	 << " for direction " << output << '\n';
    return WriteError;
  }
  return DigitalIO::configureLines( lines, output );
}


int DynClampDigitalIO::write( int line, bool val )
{
  if ( !isOpen() ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines )
    return WriteError;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_WRITE;
  dioIOC.lines = line;
  dioIOC.output = val ? 1 : 0;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::write() -> "
	 << "Writing to DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::read( int line, bool &val ) const
{
  if ( !isOpen() ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines )
    return WriteError;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_READ;
  dioIOC.lines = line;
  dioIOC.output = 0;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::read() -> "
	 << "Reading from DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    return ReadError;
  }
  val = ( dioIOC.output > 0 );
  return 0;
}


int DynClampDigitalIO::writeLines( int lines, int val )
{
  if ( !isOpen() ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 1;
  dioIOC.op = DIO_WRITE;
  dioIOC.lines = lines;
  dioIOC.output = val;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::writeLines() -> "
	 << "Writing to DIO lines " << lines
	 << " failed on subdevice " << SubDevice << '\n';
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::readLines( int lines, int &val ) const
{
  if ( !isOpen() ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 1;
  dioIOC.op = DIO_READ;
  dioIOC.lines = lines;
  dioIOC.output = 0;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::readLines() -> "
	 << "Reading from DIO lines " << lines
	 << " failed on subdevice " << SubDevice << '\n';
    return ReadError;
  }
  val = dioIOC.output;
  return 0;
}


int DynClampDigitalIO::addTTLPulse( int line, enum ttlPulses high,
				    enum ttlPulses low, bool inithigh )
{
  if ( !isOpen() ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> invalid line " << line << '\n';
    return WriteError;
  }
  if ( high == TTL_UNDEFINED || low == TTL_UNDEFINED ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> unset high " << high << " or low " << low << " condition\n";
    return WriteError;
  }
  if ( TTLPulseHigh[line] != TTL_UNDEFINED || 
       TTLPulseLow[line] != TTL_UNDEFINED ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> high " << TTLPulseHigh[line] << " or low " << TTLPulseLow[line] << " condition already set for line " << line << "\n";
    return WriteError;
  }
  if ( ! allocatedLine( line ) ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> line " << line << " not allocated\n";
    return WriteError;
  }
  if ( ! lineConfiguration( line ) ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> line " << line << " not configured for writing\n";
    return WriteError;
  }

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_ADD_TTLPULSE;
  dioIOC.lines = line;
  dioIOC.output = 1;
  dioIOC.pulseType = high;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> "
	 << "Adding TTL pulse high to DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    return WriteError;
  }
  dioIOC.output = inithigh ? 1 : 0;
  dioIOC.pulseType = low;
  retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> "
	 << "Adding TTL pulse low to DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    dioIOC.op = DIO_CLEAR_TTLPULSE;
    dioIOC.pulseType = TTL_UNDEFINED;
    ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
    return WriteError;
  }
  TTLPulseHigh[line] = high;
  TTLPulseLow[line] = low;
  return 0;
}


int DynClampDigitalIO::clearTTLPulse( int line, bool high )
{
  if ( !isOpen() ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> invalid line " << line << '\n';
    return WriteError;
  }
  if ( TTLPulseHigh[line] == TTL_UNDEFINED || 
       TTLPulseLow[line] == TTL_UNDEFINED ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> unset high " << TTLPulseHigh[line] << " or low " << TTLPulseLow[line] << " condition\n";
    return WriteError;
  }
  if ( ! allocatedLine( line ) ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> line " << line << " not allocated\n";
    return WriteError;
  }
  if ( ! lineConfiguration( line ) ) {
    cerr << "! error: DynClampDigitalIO::addTTLPulse() -> line " << line << " not configured for writing\n";
    return WriteError;
  }

  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_CLEAR_TTLPULSE;
  dioIOC.lines = line;
  dioIOC.output = high ? 1 : 0;
  dioIOC.pulseType = TTL_UNDEFINED;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::clearTTLPulse() -> "
	 << "Clearing TTL pulse at DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    return WriteError;
  }
  TTLPulseHigh[line] = TTL_UNDEFINED;
  TTLPulseLow[line] = TTL_UNDEFINED;
  return 0;
}


int DynClampDigitalIO::setSyncPulse( int line, double duration )
{
  if ( !isOpen() )
    return NotOpen;
  long durationns = (long)::round( 1.0e9*duration );
  if ( durationns <= 0 ) {
    cerr << "! error: DynClampDigitalIO::setSyncPulse() -> duration " << durationns << " ns not positive\n";
    return WriteError;
  }
  if ( line < 0 || line >= MaxLines ) {
    cerr << "! error: DynClampDigitalIO::setSyncPulse() -> invalid line " << line << '\n';
    return WriteError;
  }
  if ( ! allocatedLine( line ) ) {
    cerr << "! error: DynClampDigitalIO::setSyncPulse() -> line " << line << " not allocated\n";
    return WriteError;
  }
  if ( ! lineConfiguration( line ) ) {
    cerr << "! error: DynClampDigitalIO::setSyncPulse() -> line " << line << " not configured for writing\n";
    return WriteError;
  }
  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_SET_SYNCPULSE;
  dioIOC.lines = line;
  dioIOC.pulsewidth = durationns;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::setSyncPulse() -> "
	 << "Setting pulse for DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::setSyncPulse( double duration )
{
  return setSyncPulse( SyncLine, duration );
}


int DynClampDigitalIO::clearSyncPulse( void )
{
  if ( !isOpen() )
    return NotOpen;
  struct dioIOCT dioIOC;
  dioIOC.subdev = SubDevice;
  dioIOC.op = DIO_CLEAR_SYNCPULSE;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if ( retval < 0 ) {
    int ern = errno;
    if ( ern == ENOTTY )
      return InvalidDevice;
    cerr << "! error: DynClampDigitalIO::clearSyncPulse() -> "
	 << "Clearing sync pulse failed on subdevice " << SubDevice << '\n';
    return WriteError;
  }
  return 0;
}


}; /* namespace rtaicomedi */
