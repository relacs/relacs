/*
  digitalio.cc
  Interface for accessing digital input and output lines of a data-aquisition board.

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

#include <sstream>
#include <relacs/digitalio.h>

using namespace std;

namespace relacs {


DigitalIO::DigitalIO( const string &deviceclass )
  : Device( deviceclass, Type )
{
  freeLines();
}


DigitalIO::~DigitalIO( void )
{
}      


int DigitalIO::open( const string &device, long mode )
{
  freeLines();
  clearSettings();
  setDeviceFile( device );
  return InvalidDevice;
}


int DigitalIO::open( Device &device, long mode )
{
  freeLines();
  clearSettings();
  setDeviceFile( device.deviceIdent() );
  return InvalidDevice;
}


string DigitalIO::info( void ) const
{
  ostringstream ss;
  ss << ";lines: " << lines();
  return Device::info() + ss.str();
}


int DigitalIO::allocateLines( unsigned long lines )
{
  // find unused id:
  int id = 0;
  int k=0;
  do {
    id++;
    int bit = 1;
    for ( k=0; k<MaxDIOLines; k++ ) {
      if ( (lines & bit) > 0 && DIOLines[k] > 0 )
	return WriteError;
      if ( DIOLines[k] == id )
	break;
      bit *= 2;
    }
  } while ( k<MaxDIOLines );

  // allocated lines:
  int bit = 1;
  for ( k=0; k<MaxDIOLines;  k++ ) {
    if ( (lines & bit) > 0 )
      DIOLines[k] = id;
    bit *= 2;
  }

  return id;
}


int DigitalIO::allocateLine( int line )
{
  if ( line < 0 || line >= MaxDIOLines )
    return WriteError;

  // find unused id:
  int id = 0;
  int k=0;
  do {
    id++;
    for ( k=0; k<MaxDIOLines; k++ ) {
      if ( k == line && DIOLines[k] > 0 )
	return WriteError;
      if ( DIOLines[k] == id )
	break;
    }
  } while ( k<MaxDIOLines );

  // allocated line:
  DIOLines[line] = id;

  return id;
}


int DigitalIO::allocateLine( int line, int id )
{
  if ( line < 0 || line >= MaxDIOLines )
    return WriteError;

  if ( DIOLines[line] > 0 )
    return WriteError;

  // allocated line:
  DIOLines[line] = id;

  return id;
}


void DigitalIO::freeLines( int id )
{
  for ( int k=0; k<MaxDIOLines;  k++ ) {
    if ( DIOLines[k] == id )
      DIOLines[k] = 0;
  }
}


void DigitalIO::freeLines( void )
{
  for ( int k=0; k<MaxDIOLines;  k++ )
    DIOLines[k] = 0;
}


bool DigitalIO::allocated( int line, int id )
{
  if ( line < 0 || line >= MaxDIOLines )
    return false;

  return ( DIOLines[line] == id );
}


bool DigitalIO::allocated( int line )
{
  if ( line < 0 || line >= MaxDIOLines )
    return false;

  return ( DIOLines[line] > 0 );
}


}; /* namespace relacs */
