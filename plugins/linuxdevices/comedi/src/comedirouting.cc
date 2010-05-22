/*
  comedi/comedirouting.h
  Route a signal to a channel of a subdevice.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/comedi/comedirouting.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediRouting::ComediRouting( void ) 
  : Device( "ComediRouting" )
{
  DeviceP = NULL;
}


ComediRouting::ComediRouting( const string &deviceclass )
  : Device( deviceclass )
{
  DeviceP = NULL;
}


ComediRouting::ComediRouting( const string &device, const Options &opts ) 
  : Device( "ComediRouting" )
{
  DeviceP = NULL;
  open( device, opts );
}

  
ComediRouting::~ComediRouting( void ) 
{
  close();
}


int ComediRouting::open( const string &device, const Options &opts )
{ 
  Info.clear();
  Settings.clear();

  // get subdevice:
  int subdev = opts.integer( "subdevice", 0, -1 );
  if ( subdev < 0 ) {
    cerr << "! error: ComediRouting::open() -> "
	 << "Missing or invalid subdevice id for device "
	 << deviceIdent() << " !\n";
    return WriteError;
  }

  // get channel:
  int channel = opts.integer( "channel", 0, -1 );
  if ( channel < 0 ) {
    cerr << "! error: ComediRouting::open() -> "
	 << "Missing or invalid channel for device "
	 << deviceIdent() << " !\n";
    return WriteError;
  }

  // get routing:
  int routing = opts.integer( "routing", 0, -1 );
  if ( routing < 0 ) {
    cerr << "! error: ComediRouting::open() -> "
	 << "Missing or invalid routing parameter for device "
	 << deviceIdent() << " !\n";
    return WriteError;
  }

  return open( device, subdev, channel, routing );
}


int ComediRouting::open( const string &device, int subdev, int channel, int routing )
{
  if ( isOpen() )
    return -5;

  Info.clear();
  Settings.clear();

  if ( device.empty() )
    return InvalidDevice;

  // open comedi device:
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    cerr << "! error: ComediRouting::open() -> "
	 << "Device-file " << device << " could not be opened for device "
	 << deviceIdent() << "!\n";
    return NotOpen;
  }

  // set routing:
  if ( comedi_set_routing( DeviceP, subdev, channel, routing ) != 0 ) {
    cerr << "! error: ComediRouting::open() -> "
	 << "Routing failed on device " << deviceIdent() << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    return WriteError;
  }

  // configure pins:
  int diotype = comedi_get_subdevice_type( DeviceP, subdev );
  if ( diotype == COMEDI_SUBD_DIO ) {
    if ( comedi_dio_config( DeviceP, subdev, channel, COMEDI_OUTPUT ) != 0 ) {
      cerr << "! error: ComediRouting::open() -> "
	   << "DIO_CONFIG failed on device " << deviceIdent() << '\n';
      comedi_close( DeviceP );
      DeviceP = NULL;
      return WriteError;
    }
  }

  // set basic device infos:
  setDeviceName( comedi_get_board_name( DeviceP ) );
  setDeviceVendor( comedi_get_driver_name( DeviceP ) );
  setDeviceFile( device );
  addInfo();

  // set settings:
  Settings.addInteger( "subdevice", subdev );
  Settings.addInteger( "channel", channel );
  Settings.addInteger( "routing", routing );
  
  return 0;
}


bool ComediRouting::isOpen( void ) const 
{ 
  return ( DeviceP != NULL );
}


void ComediRouting::close( void ) 
{
  if ( ! isOpen() )
    return;

  // close:
  int error = comedi_close( DeviceP );
  if ( error )
    cerr << "! warning: ComediRouting::close() -> "
	 << "Closing of AI subdevice on device " << deviceFile() << "failed.\n";

  // clear flags:
  DeviceP = NULL;

  Info.clear();
  Settings.clear();
}


}; /* namespace comedi */
