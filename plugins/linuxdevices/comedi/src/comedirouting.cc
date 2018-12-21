/*
  comedi/comedirouting.h
  Route a signal to a channel of a subdevice.

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
  : ComediRouting( "ComediRouting" )
{
}


ComediRouting::ComediRouting( const string &device, const Options &opts ) 
  : ComediRouting( "ComediRouting" )
{
  Options::read(opts);
  open( device );
}


ComediRouting::ComediRouting( const string &deviceclass )
  : Device( deviceclass )
{
  DeviceP = NULL;
  initOptions();
}

  
ComediRouting::~ComediRouting( void ) 
{
  close();
}


void ComediRouting::initOptions()
{
  Device::initOptions();

  addInteger( "subdevice", "Subdevice number", 0, 0, 100 );
  addInteger( "channel", "Channel", 0, 0, 100 );
  addInteger( "routing", "Routed signal", 0, 0, 100 );
}


int ComediRouting::open( const string &device )
{ 
  clearError();
  Info.clear();
  Settings.clear();

  // get subdevice:
  int subdev = integer( "subdevice", 0, -1 );
  if ( subdev < 0 ) {
    setErrorStr( "missing or invalid subdevice id for device " + deviceIdent() );
    return WriteError;
  }

  // get channel:
  int channel = integer( "channel", 0, -1 );
  if ( channel < 0 ) {
    setErrorStr( "missing or invalid channel for device " + deviceIdent() );
    return WriteError;
  }

  // get routing:
  int routing = integer( "routing", 0, -1 );
  if ( routing < 0 ) {
    setErrorStr( "Missing or invalid routing parameter for device " + deviceIdent() );
    return WriteError;
  }

  return open( device, subdev, channel, routing );
}


int ComediRouting::open( const string &device, int subdev, int channel,
			 int routing, const string &signal )
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
  else if ( diotype != COMEDI_SUBD_DIO && diotype != COMEDI_SUBD_DO ) {
    const char *subdevstrings[20] = { "unused", "analog input" , "analog output",
				      "digital input", "digital output", "digital input/output",
				      "counter", "timer", "memory", "calibration", "processor",
				      "serial IO", "pulse width modulation" };
    if ( diotype < 0 )
      cerr << "! error: ComediRouting::open() -> "
	   << "Failed to retrieve type of subdevice " << subdev << " on device "  << device
	   << ": " << comedi_strerror( comedi_errno() ) << "\n";
    else
      cerr << "! error: ComediRouting::open() -> "
	   << "Subdevice " << subdev << " on device "  << device
	   << " is a " << subdevstrings[diotype] << " and not a digital output subdevice.\n";
    comedi_close( DeviceP );
    DeviceP = NULL;
    return InvalidDevice;
  }

  // set routing:
  if ( comedi_set_routing( DeviceP, subdev, channel, routing ) != 0 ) {
    cerr << "! error: ComediRouting::open() -> "
	 << "Routing failed on device " << deviceIdent() << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    return WriteError;
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
  if ( ! signal.empty() )
    Settings.addText( "signal", signal );
  
  return 0;
}


bool ComediRouting::isOpen( void ) const 
{ 
  return ( DeviceP != NULL );
}


void ComediRouting::close( void ) 
{
  clearError();
  if ( ! isOpen() )
    return;

  // close:
  int error = comedi_close( DeviceP );
  if ( error )
    setErrorStr( "closing of routing subdevice on device " + deviceFile() + "failed" );

  // clear flags:
  DeviceP = NULL;

  Info.clear();
  Settings.clear();
}


}; /* namespace comedi */
