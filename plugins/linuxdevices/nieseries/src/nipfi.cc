/*
  nieseries/nipfi.cc
  Controls the "Programmable Function Inputs" (PFI)

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

#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>   
#include <relacs/nieseries/nidaq.h>
#include <relacs/nieseries/nipfi.h>
using namespace std;
using namespace relacs;

namespace nieseries {


NIPFI::NIPFI( const string &device, long mode )
  : Device( "NIPFI" ),
    Handle( -1 )
{
  open( device, mode );
}


NIPFI::NIPFI( void )
  : Device( "NIPFI" ),
    Handle( -1 )
{
}


NIPFI::~NIPFI( void )
{
  close();
}


int NIPFI::open( const string &device, long mode )
{
  if ( Handle >= 0 )
    return 0;

  Info.clear();
  Settings.clear();

  Handle = ::open( device.c_str(), O_RDWR );

  if ( Handle < 0 ) {
    perror( "NIPFI" );
    return InvalidDevice;
  }

  // programm PFI pins as given by mode:
  for ( int pin=0; pin<10; pin++ ) {
    if ( mode & (1<<pin) )
      pinOut( pin );
    else
      pinIn( pin );
  }

  nidaq_info board;
  ::ioctl( Handle, NIDAQINFO, &board );
  setDeviceName( board.name );
  setDeviceVendor( "National Instruments" );
  setDeviceFile( device );
  addInfo();

  return 0;
}


bool NIPFI::isOpen( void ) const
{
  return ( Handle >= 0 );
}


void NIPFI::close( void )
{
  if ( Handle >= 0 ) {
    ::close( Handle );
    Handle = -1;
  }
  Info.clear();
  Settings.clear();
}


int NIPFI::pinIn( int pin )
{
  if ( Handle < 0 )
    return InvalidDevice;
  return ::ioctl( Handle, NIDAQPFIIN, pin );
}


int NIPFI::pinOut( int pin )
{
  if ( Handle < 0 )
    return InvalidDevice;
  return ::ioctl( Handle, NIDAQPFIOUT, pin );
}


}; /* namespace nieseries */
