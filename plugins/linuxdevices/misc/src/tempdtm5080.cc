/*
  misc/tempdtm5080.cc
  The DTM 5080 temperature sensor via serial port  

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <sstream>
#include <relacs/misc/tempdtm5080.h>
using namespace std;
using namespace relacs;

namespace misc {


TempDTM5080::TempDTM5080( void )
  : Temperature( "TempDTM5080" ),
    Handle( -1 ),
    Probe( 0 )
{
  Settings.addInteger( "probe" );
}


TempDTM5080::TempDTM5080( const string &device, const Options &opts  )
  : Temperature( "TempDTM5080" ),
    Handle( -1 ),
    Probe( 0 )
{
  Settings.addInteger( "probe" );
  open( device, opts );
}


TempDTM5080::~TempDTM5080( void )
{
  close();
}


int TempDTM5080::open( const string &device, const Options &opts )
{
  if ( Handle >= 0 )
    return 0;

  // open device:
  Handle = ::open( device.c_str(), O_RDWR | O_NOCTTY );
  if ( Handle < 0 ) {
    perror( "! warning: TempDTM5080::open() -> Can't open device" );
    return InvalidDevice;
  }

  // save current TempDTM5080 port settings:
  tcgetattr( Handle, &OldTIO );

  // clear struct for new port settings:
  memset( &NewTIO, 0, sizeof( NewTIO ) );

  /* 
     BAUDRATE   : Set bps rate. You could also use cfsetispeed and cfsetospeed.
     CS8        : 8n1 (8bit,no parity,1 stopbit)
     CLOCAL     : local connection, no modem contol
     CREAD      : enable receiving characters
     IXON | XOFF: XON/XOFF handshaking
  */
  NewTIO.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
  NewTIO.c_iflag = IGNPAR | IGNBRK | IXON | IXOFF;
  NewTIO.c_oflag = 0;
  NewTIO.c_lflag = 0;
  NewTIO.c_cc[VTIME] = 1;  // inter-character timer (decisecond)
  NewTIO.c_cc[VMIN] = 0;   // blocking read until n character arrives
  
  // clean the modem line and activate the settings for the port:
  tcflush( Handle, TCIFLUSH );
  tcsetattr( Handle, TCSANOW, &NewTIO );

  int probe = opts.integer( "probe", 0, 1 );
  setProbe( probe );

  setDeviceName( "DTM5080" );
  setDeviceVendor( "LKM electronic GmbH (Geraberg, Germany)" );
  setDeviceFile( device );

  // add infos:
  Info.clear();
  addInfo();

  char com = 'a';
  char buf[10];

  com = 't';
  int n = write( Handle, &com, 1 );
  n = read( Handle, buf, 10 );
  buf[n] = '\0';
  Info.addText( "device type", buf );
  
  com = 'l';
  n = write( Handle, &com, 1 );
  n = read( Handle, buf, 10 );
  buf[n] = '\0';
  Info.addText( "serial number", buf );
  
  com = 'a';
  n = write( Handle, &com, 1 );
  n = read( Handle, buf, 10 );
  buf[n] = '\0';
  Info.addText( "resolution", buf );

  return 0;
}


bool TempDTM5080::isOpen( void ) const
{
  return ( Handle >= 0 );
}


void TempDTM5080::close( void )
{
  if ( Handle >= 0 )
    tcsetattr( Handle, TCSANOW, &OldTIO );
  Handle = -1;
  Info.clear();
}


int TempDTM5080::reset( void )
{
  tcflush( Handle, TCIFLUSH );
  return 0;
}


double TempDTM5080::temperature( void )
{
  if ( Handle < 0 ) {
    cout << "TempDTM5080 not opened!\n";
  }
  else {
    char com = 'd';
    write( Handle, &com, 1 );
    char buf[10];
    int n = read( Handle, buf, 10 );
    if ( n > 0 && buf[n-1] == ':' ) {
      buf[n-1] = '\0';
      char *ep = NULL;
      double temp = ::strtod( buf, &ep );
      if ( ep != buf ) {
	temp *= 0.01;
	Settings.setNumber( "temperature", temp );
	return temp;
      }
    }
  }
  return -1000.0;
}


void TempDTM5080::setProbe( int probe )
{
  if ( Handle < 0 ) {
    cout << "TempDTM5080 not opened!\n";
  }
  else {
    // XXX check probe value!
    char com[10];
    sprintf( com, "b%d", probe );
    write( Handle, com, 2 );
    char buf[10];
    int n = read( Handle, buf, 10 );
    if ( n != 1 && buf[0] != ':' )
      cout << "failed to set probe: " << buf << endl;
    Probe = probe;
    Settings.setInteger( "probe", Probe );
  }
}


}; /* namespace misc */
