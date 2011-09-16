/*
  comedi/dynclampdigitalio.cc
  Interface for accessing analog input of a daq-board via comedi and the dynclamp kernel module.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <relacs/str.h>
#include <relacs/comedi/comedidigitalio.h>
#include <relacs/comedi/dynclampdigitalio.h>
using namespace std;
using namespace relacs;

namespace comedi {


DynClampDigitalIO::DynClampDigitalIO( void ) 
  : DigitalIO( "DynClampDigitalIO" )
{
  CDIO = new ComediDigitalIO;
  SubDevice = 0;
  SubdeviceID = -1;
  MaxLines = 0;
  ModuleFd = -1;
}


DynClampDigitalIO::DynClampDigitalIO( const string &device, const Options &opts ) 
  : DigitalIO( "DynClampDigitalIO" )
{
  CDIO = new ComediDigitalIO;
  SubDevice = 0;
  SubdeviceID = -1;
  MaxLines = 0;
  ModuleFd = -1;
  open( device, opts );
}

  
DynClampDigitalIO::~DynClampDigitalIO( void ) 
{
  close();
  delete CDIO;
}


int DynClampDigitalIO::open( const string &device, const Options &opts )
{ 
  if ( isOpen() )
    return -5;

  freeLines();
  Info.clear();
  Settings.clear();
  if ( device.empty() )
    return InvalidDevice;
  setDeviceFile( device );

  // open user space coemdi:
  int retval = CDIO->open( device, opts );
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
  if( ModuleFd == -1 ) {
  cerr << " DynClampDigitalIO::open(): opening dynclamp-module failed\n";
    return -1;
  }

  // get subdevice ID from module:
  retval = ::ioctl( ModuleFd, IOC_GET_SUBDEV_ID, &SubdeviceID );
  if( retval < 0 ) {
    cerr << " DynClampDigitalIO::open -> ioctl command IOC_GET_SUBDEV_ID on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  // set device and subdevice:
  struct deviceIOCT deviceIOC;
  deviceIOC.subdevID = SubdeviceID;
  strcpy( deviceIOC.devicename, deviceFile().c_str() );
  deviceIOC.subdev = SubDevice;
  deviceIOC.subdevType = SUBDEV_DIO;
  deviceIOC.fifoSize = 0;
  retval = ::ioctl( ModuleFd, IOC_OPEN_SUBDEV, &deviceIOC );
  //  cerr << " DynClampAnalogInput::open(): IOC_OPEN_SUBDEV request for address done!" /// TEST
  //       << &deviceIOC << '\n';
  if( retval < 0 ) {
    cerr << " DynClampDigitalIO::open -> ioctl command IOC_OPEN_SUBDEV on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  setInfo();
  
  return 0;
}


bool DynClampDigitalIO::isOpen( void ) const 
{ 
  return ( ModuleFd >= 0 );
}


void DynClampDigitalIO::close( void ) 
{
  if ( ! isOpen() )
    return;

  ::ioctl( ModuleFd, IOC_REQ_CLOSE, &SubdeviceID );
  if( ::close( ModuleFd ) < 0 )
    cerr << "Close of module file failed!\n";

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


int DynClampDigitalIO::configureLine( int line, bool output ) const
{
  if ( !isOpen ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines )
    return WriteError;

  struct dioIOCT dioIOC;
  dioIOC.subdevID = SubdeviceID;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_CONFIGURE;
  dioIOC.lines = line;
  dioIOC.output = output;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::configureLine() -> "
	 << "Configuring DIO line " << line
	 << " failed on subdeviceid " << SubdeviceID
	 << " for direction " << output << '\n';
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::configureLines( int lines, int output ) const
{
  if ( !isOpen ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdevID = SubdeviceID;
  dioIOC.bitfield = 1;
  dioIOC.op = DIO_CONFIGURE;
  dioIOC.lines = lines;
  dioIOC.output = output;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::configureLines() -> "
	 << "Configuring DIO lines " << lines
	 << " failed on subdeviceid " << SubdeviceID
	 << " for direction " << output << '\n';
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::write( int line, bool val )
{
  if ( !isOpen ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines )
    return WriteError;

  struct dioIOCT dioIOC;
  dioIOC.subdevID = SubdeviceID;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_WRITE;
  dioIOC.lines = line;
  dioIOC.output = val ? 1 : 0;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::write() -> "
	 << "Writing to DIO line " << line
	 << " failed on subdeviceid " << SubdeviceID << '\n';
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::read( int line, bool &val ) const
{
  if ( !isOpen ) 
    return NotOpen;
  if ( line < 0 || line >= MaxLines )
    return WriteError;

  struct dioIOCT dioIOC;
  dioIOC.subdevID = SubdeviceID;
  dioIOC.bitfield = 0;
  dioIOC.op = DIO_READ;
  dioIOC.lines = line;
  dioIOC.output = 0;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::read() -> "
	 << "Reading from DIO line " << line
	 << " failed on subdeviceid " << SubdeviceID << '\n';
    return ReadError;
  }
  val = ( dioIOC.output > 0 );
  return 0;
}


int DynClampDigitalIO::writeLines( int lines, int val )
{
  if ( !isOpen ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdevID = SubdeviceID;
  dioIOC.bitfield = 1;
  dioIOC.op = DIO_WRITE;
  dioIOC.lines = lines;
  dioIOC.output = val;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::writeLines() -> "
	 << "Writing to DIO lines " << lines
	 << " failed on subdeviceid " << SubdeviceID << '\n';
    return WriteError;
  }
  return 0;
}


int DynClampDigitalIO::readLines( int lines, int &val ) const
{
  if ( !isOpen ) 
    return NotOpen;

  struct dioIOCT dioIOC;
  dioIOC.subdevID = SubdeviceID;
  dioIOC.bitfield = 1;
  dioIOC.op = DIO_READ;
  dioIOC.lines = lines;
  dioIOC.output = 0;
  int retval = ::ioctl( ModuleFd, IOC_DIO_CMD, &dioIOC );
  if( retval < 0 ) {
    cerr << "! error: DynClampDigitalIO::readLines() -> "
	 << "Reading from DIO lines " << lines
	 << " failed on subdeviceid " << SubdeviceID << '\n';
    return ReadError;
  }
  val = dioIOC.output;
  return 0;
}


}; /* namespace comedi */
