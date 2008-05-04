/*
  nidio.cc
  Controls the digital I/O lines of a National Instruments E Series board.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>   
#include <relacs/hardware/nidaq.h>
#include <relacs/hardware/nidio.h>
using namespace std;
using namespace relacs;


NIDIO::NIDIO( const string &device )
  : Device( "NIDIO" ),
    Handle( -1 ),
    Pins( 0 )
{
  open( device );
}


NIDIO::NIDIO( void )
  : Device( "NIDIO" ),
    Handle( -1 ),
    Pins( 0 )
{
}


NIDIO::~NIDIO( void )
{
  close();
}


int NIDIO::open( const string &device, long mode )
{
  if ( Handle >= 0 )
    return 0;

  Handle = ::open( device.c_str(), O_RDWR );

  if ( Handle < 0 )
    return InvalidDevice;
  else {
    nidaq_info board;
    ::ioctl( Handle, NIDAQINFO, &board );
    setDeviceName( board.name );
    setDeviceVendor( "National Instruments" );
    setDeviceFile( device );
    return 0;
  }
}


void NIDIO::close( void )
{
  if ( Handle >= 0 ) {
    ::close( Handle );
    Handle = -1;
  }
}


int NIDIO::allocPins( int pins )
{
  int r = ( Pins & pins );
  Pins |= pins;
  return r;
}


void NIDIO::freePins( int pins )
{
  Pins &= ~pins;
}


int NIDIO::configure( int dios, int mask ) const
{
  if ( Handle >= 0 ) {
    ::ioctl( Handle, NIDAQDIOMASK, mask );
    ::ioctl( Handle, NIDAQDIOCONFIGURE, dios );
    return 0;
  }
  else
    return NotOpen;
}


int NIDIO::out( int dios, int mask )
{
  if ( Handle >= 0 ) {
    ::ioctl( Handle, NIDAQDIOMASK, mask );
    ::ioctl( Handle, NIDAQDIOPAROUT, dios );
    return 0;
  }
  else
    return NotOpen;
}


int NIDIO::in( int &dios ) const
{
  if ( Handle >= 0 ) {
    char buf;
    ::ioctl( Handle, NIDAQDIOPARIN, &buf );
    dios = buf;
    return 0;
  }
  else
    return NotOpen;
}


int NIDIO::setClock( bool high )
{
  if ( Handle >= 0 ) {
    ::ioctl( Handle, NIDAQDIOCLOCK, high );
     return 0;
  }
  else
    return NotOpen;
}


int NIDIO::setTiming( bool div2, bool slow ) const
{
  if ( Handle >= 0 ) {
    ::ioctl( Handle, NIDAQDIOTIMEDIV2, div2 ); // sets timebase divided by 2 (1=clock gets slower)
    ::ioctl( Handle, NIDAQDIOTIMEBASE, slow ); // selects timebase
    return 0;
  }
  else
    return NotOpen;
}


int NIDIO::write( unsigned char *buf, int n )
{
  if ( Handle >= 0 ) {
    return ::write( Handle, buf, n );
  }
  else
    return NotOpen;
}


int NIDIO::read( unsigned char *buf, int n ) const
{
  if ( Handle >= 0 ) {
    return ::read( Handle, buf, n );
  }
  else
    return NotOpen;
}

