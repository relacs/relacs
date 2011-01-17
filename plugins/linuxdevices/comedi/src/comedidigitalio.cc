/*
  comedi/comedidigitalio.cc
  Interface for accessing analog input of a daq-board via comedi.

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
#include <unistd.h>
#include <fcntl.h>
#include <relacs/str.h>
#include <relacs/comedi/comedidigitalio.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediDigitalIO::ComediDigitalIO( void ) 
  : DigitalIO( "ComediDigitalIO" )
{
  DeviceP = NULL;
  SubDevice = 0;
  MaxLines = 0;
}


ComediDigitalIO::ComediDigitalIO( const string &device, const Options &opts ) 
  : DigitalIO( "ComediDigitalIO" )
{
  DeviceP = NULL;
  SubDevice = 0;
  MaxLines = 0;
  open( device, opts );
}

  
ComediDigitalIO::~ComediDigitalIO( void ) 
{
  close();
}


int ComediDigitalIO::open( const string &device, const Options &opts )
{ 
  if ( isOpen() )
    return -5;

  freeLines();
  Info.clear();
  Settings.clear();
  if ( device.empty() )
    return InvalidDevice;

  // open comedi device:
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    cerr << "! error: ComediDigitalIO::open() -> "
	 << "Device-file " << device << " could not be opened!\n";
    return NotOpen;
  }

  // get DIO subdevice:
  int subdev = opts.integer( "subdevice", 0, -1 );
  if ( subdev >= 0 ) {
    int diotype = comedi_get_subdevice_type( DeviceP, subdev );
    if ( diotype != COMEDI_SUBD_DI &&
	 diotype != COMEDI_SUBD_DO &&
	 diotype != COMEDI_SUBD_DIO ) {
      cerr << "! error: ComediDigitalIO::open() -> "
	   << "Subdevice " << subdev << "is not a digital I/o on device "
	   << device << '\n';
      comedi_close( DeviceP );
      DeviceP = NULL;
      return InvalidDevice;
    }
  }
  if ( subdev < 0 ) {
    int startsubdev = opts.integer( "startsubdevice", 0, 0 );
    subdev = comedi_find_subdevice_by_type( DeviceP, COMEDI_SUBD_DIO,
					    startsubdev );
    if ( subdev < 0 ) {
      cerr << "! error: ComediDigitalIO::open() -> "
	   << "No subdevice for DIO found on device "  << device
	   << " for startsubdevice >= " << startsubdev << '\n';
      comedi_close( DeviceP );
      DeviceP = NULL;
      return InvalidDevice;
    }
  }
  SubDevice = subdev;

  // lock DIO subdevice:
  if ( comedi_lock( DeviceP, SubDevice ) != 0 ) {
    cerr << "! error: ComediDigitalIO::open() -> "
	 << "Locking of AI subdevice failed on device " << device << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return NotOpen;
  }  

  // set basic device infos:
  setDeviceName( comedi_get_board_name( DeviceP ) );
  setDeviceVendor( comedi_get_driver_name( DeviceP ) );
  setDeviceFile( device );

  // get maximum number of output lines:
  MaxLines = comedi_get_n_channels( DeviceP, SubDevice );

  setInfo();
  
  return 0;
}


bool ComediDigitalIO::isOpen( void ) const 
{ 
  return ( DeviceP != NULL );
}


void ComediDigitalIO::close( void ) 
{
  if ( ! isOpen() )
    return;

  // unlock:
  int error = comedi_unlock( DeviceP,  SubDevice );
  if ( error < 0 )
    cerr << "! warning: ComediDigitalIO::close() -> "
	 << "Unlocking of AI subdevice on device " << deviceFile() << "failed\n";

  // close:
  error = comedi_close( DeviceP );
  if ( error )
    cerr << "! warning: ComediDigitalIO::close() -> "
	 << "Closing of AI subdevice on device " << deviceFile() << "failed.\n";

  // clear flags:
  DeviceP = NULL;
  SubDevice = 0;

  Info.clear();
  Settings.clear();
}


int ComediDigitalIO::lines( void ) const
{ 
  if ( !isOpen() )
    return -1;
  return MaxLines;
}


int ComediDigitalIO::configureLine( int line, bool output ) const
{
  int direction = output ? COMEDI_OUTPUT : COMEDI_INPUT;
  if ( comedi_dio_config( DeviceP, SubDevice, line, direction ) != 0 ) {
    cerr << "! error: ComediDigitalIO::configure() -> "
	 << "Configuring DIO line " << line
	 << " failed on subdevice " << SubDevice
	 << " for direction " << direction << '\n';
    return WriteError;
  }
  return 0;
}


int ComediDigitalIO::configureLines( int lines, int output ) const
{
  int bit = 1;
  for ( int channel=0; channel<32; channel++ ) {
    if ( ( lines & bit ) > 0 ) {
      int direction = COMEDI_INPUT;
      if ( ( output & bit ) > 0 )
	direction = COMEDI_OUTPUT;
      if ( comedi_dio_config( DeviceP, SubDevice, channel, direction ) != 0 ) {
	cerr << "! error: ComediDigitalIO::configure() -> "
	     << "Configuring DIO line " << channel
	     << " failed on subdevice " << SubDevice
	     << " for direction " << direction << '\n';
	return WriteError;
      }
    }
    bit *= 2;
  }
  return 0;
}


int ComediDigitalIO::write( int line, bool val )
{
  if ( comedi_dio_write( DeviceP, SubDevice, line, val ) != 1 ) {
    comedi_perror( "ComediDigitalIO::write()" );
    cerr << "! error: ComediDigitalIO::write() -> "
	 << "Writing on DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    return WriteError;
  }
  return 0;
}


int ComediDigitalIO::read( int line, bool &val ) const
{
  unsigned int bit = 0;
  if ( comedi_dio_read( DeviceP, SubDevice, line, &bit ) != 1 ) {
    comedi_perror( "ComediDigitalIO::read()" );
    cerr << "! error: ComediDigitalIO::read() -> "
	 << "Reading from DIO line " << line
	 << " failed on subdevice " << SubDevice << '\n';
    return ReadError;
  }
  val = ( bit > 0 );
  return 0;
}


int ComediDigitalIO::writeLines( int lines, int val )
{
  unsigned int ival = val;
  if ( comedi_dio_bitfield2( DeviceP, SubDevice, lines, &ival, 0 ) < 0 ) {
    comedi_perror( "ComediDigitalIO::write()" );
    cerr << "! error: ComediDigitalIO::write() -> "
	 << "Writing on DIO subdevice " << SubDevice << " failed\n";
    return WriteError;
  }
  return 0;
}


int ComediDigitalIO::readLines( int lines, int &val ) const
{
  unsigned int ival = 0;
  if ( comedi_dio_bitfield2( DeviceP, SubDevice, lines, &ival, 0 ) < 0 ) {
    comedi_perror( "ComediDigitalIO::read()" );
    cerr << "! error: ComediDigitalIO::read() -> "
	 << "Reading from DIO subdevice " << SubDevice
	 << " failed\n";
    return ReadError;
  }
  val = ival & lines;
  return 0;
}


}; /* namespace comedi */
