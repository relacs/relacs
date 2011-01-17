/*
  comedi/comedinipfi.cc
  Controlls the PFI pins of a NI daq-board via comedi.

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
#include <relacs/comedi/comedinipfi.h>
using namespace std;
using namespace relacs;

namespace comedi {


ComediNIPFI::ComediNIPFI( void ) 
  : ComediRouting( "ComediNIPFI" )
{
}


ComediNIPFI::ComediNIPFI( const string &device, const Options &opts ) 
  : ComediRouting( "ComediNIPFI" )
{
  open( device, opts );
}

  
ComediNIPFI::~ComediNIPFI( void ) 
{
}


int ComediNIPFI::open( const string &device, const Options &opts )
{ 
  Info.clear();
  Settings.clear();

  // check PFI subdevice no. 7
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

  return ComediRouting::open( device, subdev, channel, routing );
}


}; /* namespace comedi */
