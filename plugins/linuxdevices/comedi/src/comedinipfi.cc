/*
  comedi/comedinipfi.cc
  Controlling the PFI pins of a NI daq-board via comedi.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/comedi/comedinipfi.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediNIPFI::ComediNIPFI( void ) 
  : Device( "ComediNIPFI" )
{
  DeviceP = NULL;
  SubDevice = 0;
}


ComediNIPFI::ComediNIPFI( const string &device, long mode ) 
  : Device( "ComediNIPFI" )
{
  DeviceP = NULL;
  SubDevice = 0;
  open( device, mode );
}

  
ComediNIPFI::~ComediNIPFI( void ) 
{
  close();
}


int ComediNIPFI::open( const string &device, long mode )
{ 
  if ( isOpen() )
    return -5;

  clearSettings();
  if ( device.empty() )
    return InvalidDevice;

  // open comedi device:
  DeviceP = comedi_open( device.c_str() );
  if ( DeviceP == NULL ) {
    cerr << "! error: ComediNIPFI::open() -> "
	 << "Device-file " << device << " could not be opened!\n";
    return NotOpen;
  }

  // check PFI subdevice:  7: 16 (PFI), 10: 8 (RTSI)
  int subdev = 7;
  int subdevtype = comedi_get_subdevice_type( DeviceP, subdev );
  if ( subdevtype != COMEDI_SUBD_DIO ) {
    cerr << "! error: ComediNIPFI::open() -> "
	 << "Subdevice " << subdev << " on device "  << device
	 << " is not a DIO subdevice.\n";
    comedi_close( DeviceP );
    DeviceP = NULL;
    return InvalidDevice;
  }
  SubDevice = subdev;

  // lock PFI subdevice:
  if ( comedi_lock( DeviceP, SubDevice ) != 0 ) {
    cerr << "! error: ComediNIPFI::open() -> "
	 << "Locking of AI subdevice failed on device " << device << '\n';
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return NotOpen;
  }  

  // set routing:
  if ( comedi_set_routing( DeviceP, SubDevice, 6, NI_PFI_OUTPUT_AO_START1 ) != 0 ) {
    cerr << "! error: ComediNIPFI::open() -> "
	 << "Routing failed on device " << device << '\n';
    comedi_unlock( DeviceP,  SubDevice );
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return WriteError;
  }

  // configure pins:
  if ( comedi_dio_config( DeviceP, SubDevice, 6, COMEDI_OUTPUT ) != 0 ) {
    cerr << "! error: ComediNIPFI::open() -> "
	 << "DIO_CONFIG failed on device " << device << '\n';
    comedi_unlock( DeviceP,  SubDevice );
    comedi_close( DeviceP );
    DeviceP = NULL;
    SubDevice = 0;
    return WriteError;
  }

  // set basic device infos:
  setDeviceName( comedi_get_board_name( DeviceP ) );
  setDeviceVendor( comedi_get_driver_name( DeviceP ) );
  setDeviceFile( device );
  
  return 0;
}


bool ComediNIPFI::isOpen( void ) const 
{ 
  return ( DeviceP != NULL );
}


void ComediNIPFI::close( void ) 
{
  if ( ! isOpen() )
    return;

  // unlock:
  int error = comedi_unlock( DeviceP,  SubDevice );
  if ( error < 0 )
    cerr << "! warning: ComediNIPFI::close() -> "
	 << "Unlocking of AI subdevice on device " << deviceFile() << "failed\n";

  // close:
  error = comedi_close( DeviceP );
  if ( error )
    cerr << "! warning: ComediNIPFI::close() -> "
	 << "Closing of AI subdevice on device " << deviceFile() << "failed.\n";

  // clear flags:
  DeviceP = NULL;
  SubDevice = 0;
}


}; /* namespace comedi */
