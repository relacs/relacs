/*
  digitalio.cc
  Interface for accessing digital input and output lines of a data-aquisition board.

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

#include <relacs/str.h>
#include <relacs/digitalio.h>

using namespace std;

namespace relacs {


DigitalIO::DigitalIO( const string &deviceclass )
  : Device( deviceclass, DigitalIOType )
{
  freeLines();
  for ( int k=0; k<MaxDIOLines;  k++ )
    DIOLineWriteable[k] = false;
}


DigitalIO::~DigitalIO( void )
{
}      


int DigitalIO::open( const string &device )
{
  freeLines();
  for ( int k=0; k<MaxDIOLines;  k++ )
    DIOLineWriteable[k] = false;
  Info.clear();
  Settings.clear();
  setDeviceFile( device );
  return InvalidDevice;
}


int DigitalIO::open( Device &device )
{
  freeLines();
  for ( int k=0; k<MaxDIOLines;  k++ )
    DIOLineWriteable[k] = false;
  Info.clear();
  Settings.clear();
  setDeviceFile( device.deviceIdent() );
  return InvalidDevice;
}


const Options &DigitalIO::settings( void ) const
{
  Settings.clear();
  for ( int k=0; k<lines(); k++ ) {
    if ( DIOLineIDs[k] > 0 ) {
      Settings.addInteger( "line"+Str(k)+"_id", DIOLineIDs[k] );
      Settings.addText( "line"+Str(k)+"_config", DIOLineWriteable[k] ? "write" : "read" );
    }
  }
  return Settings;
}


int DigitalIO::configureLine( int line, bool output )
{
  if ( line < 0 || line >= MaxDIOLines )
    return WriteError;
  DIOLineWriteable[line] = output;
  return 0;
}


int DigitalIO::configureLines( int lines, int output )
{
  int bit = 1;
  for ( int k=0; k<MaxDIOLines; k++ ) {
    if ( (lines & bit) > 0 )
      DIOLineWriteable[k] = ( (output & bit) > 0 );
    bit *= 2;
  }
  return 0;
}


bool DigitalIO::lineConfiguration( int line ) const
{
  if ( line < 0 || line >= MaxDIOLines )
    return false;
  return DIOLineWriteable[line];
}


int DigitalIO::allocateLines( int lines )
{
  int errorlines = 0;

  // find unused id:
  bool foundid = false;
  int id = 0;
  do {
    id++;
    int bit = 1;
    foundid = false;
    for ( int k=0; k<MaxDIOLines; k++ ) {
      if ( (lines & bit) > 0 && DIOLineIDs[k] > 0 )
	errorlines |= bit;
      if ( DIOLineIDs[k] == id )
	foundid = true;
      bit *= 2;
    }
  } while ( errorlines == 0 && foundid );

  if ( errorlines > 0 )
    return -errorlines;

  // allocated lines:
  int bit = 1;
  for ( int k=0; k<MaxDIOLines;  k++ ) {
    if ( (lines & bit) > 0 )
      DIOLineIDs[k] = id;
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
  int k = 0;
  do {
    id++;
    for ( k=0; k<MaxDIOLines; k++ ) {
      if ( k == line && DIOLineIDs[k] > 0 )
	return WriteError;
      if ( DIOLineIDs[k] == id )
	break;
    }
  } while ( k<MaxDIOLines );

  // allocated line:
  DIOLineIDs[line] = id;

  return id;
}


int DigitalIO::allocateLine( int line, int id )
{
  if ( line < 0 || line >= MaxDIOLines )
    return WriteError;

  if ( DIOLineIDs[line] > 0 )
    return WriteError;

  // allocated line:
  DIOLineIDs[line] = id;

  return id;
}


void DigitalIO::freeLines( int id )
{
  for ( int k=0; k<MaxDIOLines;  k++ ) {
    if ( DIOLineIDs[k] == id )
      DIOLineIDs[k] = 0;
  }
}


void DigitalIO::freeLines( void )
{
  for ( int k=0; k<MaxDIOLines;  k++ )
    DIOLineIDs[k] = 0;
}


bool DigitalIO::allocatedLines( int lines, int id )
{
  int bit = 1;
  for ( int k=0; k<MaxDIOLines;  k++ ) {
    if ( (lines & bit) > 0 &&
	 ( DIOLineIDs[k] <= 0 || DIOLineIDs[k] != id ) )
      return false;
    bit *= 2;
  }
  return true;
}


bool DigitalIO::allocatedLines( int lines )
{
  int bit = 1;
  for ( int k=0; k<MaxDIOLines;  k++ ) {
    if ( (lines & bit) > 0 &&
	 DIOLineIDs[k] <= 0 )
      return false;
    bit *= 2;
  }
  return true;
}


bool DigitalIO::allocatedLine( int line, int id )
{
  if ( line < 0 || line >= MaxDIOLines )
    return false;

  return ( DIOLineIDs[line] == id );
}


bool DigitalIO::allocatedLine( int line )
{
  if ( line < 0 || line >= MaxDIOLines )
    return false;

  return ( DIOLineIDs[line] > 0 );
}


void DigitalIO::setInfo( void )
{
  Info.clear();
  Device::addInfo();
  Info.addInteger( "lines", lines() );
}


int DigitalIO::setSyncPulse( double duration )
{
  return InvalidDevice;
}


int DigitalIO::clearSyncPulse( void )
{
  return InvalidDevice;
}


}; /* namespace relacs */

