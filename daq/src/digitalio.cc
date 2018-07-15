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
  for ( unsigned int k=0; k<MaxDIOLines;  k++ )
    DIOLineWriteable[k] = false;
}


DigitalIO::~DigitalIO( void )
{
}      


int DigitalIO::open( const string &device )
{
  freeLines();
  for ( unsigned int k=0; k<MaxDIOLines;  k++ )
    DIOLineWriteable[k] = false;
  Info.clear();
  Settings.clear();
  setDeviceFile( device );
  return InvalidDevice;
}


int DigitalIO::open( Device &device )
{
  freeLines();
  for ( unsigned int k=0; k<MaxDIOLines;  k++ )
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
      string ds = Str( DIOLineIDs[k] ) + " ";
      ds += DIOLineWriteable[k] ? "write" : "read";
      Settings.addText( "line"+Str(k), ds );
    }
  }
  return Settings;
}


void DigitalIO::setInfo( void )
{
  Info.clear();
  Device::addInfo();
  Info.addInteger( "lines", lines() );
}


int DigitalIO::allocateLines( unsigned int lines )
{
  int errorlines = 0;

  // find unused id:
  bool foundid = false;
  int id = 0;
  do {
    id++;
    unsigned int bit = 1;
    foundid = false;
    for ( unsigned int k=0; k<MaxDIOLines; k++ ) {
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
  unsigned int bit = 1;
  for ( unsigned int k=0; k<MaxDIOLines;  k++ ) {
    if ( (lines & bit) > 0 )
      DIOLineIDs[k] = id;
    bit *= 2;
  }

  return id;
}


int DigitalIO::allocateLine( unsigned int line )
{
  if ( line >= MaxDIOLines )
    return WriteError;

  // find unused id:
  int id = 0;
  unsigned int k = 0;
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


int DigitalIO::allocateLine( unsigned int line, int id )
{
  if ( line >= MaxDIOLines )
    return WriteError;

  if ( DIOLineIDs[line] > 0 )
    return WriteError;

  // allocated line:
  DIOLineIDs[line] = id;

  return id;
}


void DigitalIO::freeLines( int id )
{
  for ( unsigned int k=0; k<MaxDIOLines;  k++ ) {
    if ( DIOLineIDs[k] == id )
      DIOLineIDs[k] = 0;
  }
}


void DigitalIO::freeLines( void )
{
  for ( unsigned int k=0; k<MaxDIOLines;  k++ )
    DIOLineIDs[k] = 0;
}


bool DigitalIO::allocatedLines( unsigned int lines, int id )
{
  unsigned int bit = 1;
  for ( unsigned int k=0; k<MaxDIOLines;  k++ ) {
    if ( (lines & bit) > 0 &&
	 ( DIOLineIDs[k] <= 0 || DIOLineIDs[k] != id ) )
      return false;
    bit *= 2;
  }
  return true;
}


bool DigitalIO::allocatedLines( unsigned int lines )
{
  unsigned int bit = 1;
  for ( unsigned int k=0; k<MaxDIOLines;  k++ ) {
    if ( (lines & bit) > 0 &&
	 DIOLineIDs[k] <= 0 )
      return false;
    bit *= 2;
  }
  return true;
}


bool DigitalIO::allocatedLine( unsigned int line, int id )
{
  if ( line >= MaxDIOLines )
    return false;

  return ( DIOLineIDs[line] == id );
}


bool DigitalIO::allocatedLine( unsigned int line )
{
  if ( line >= MaxDIOLines )
    return false;

  return ( DIOLineIDs[line] > 0 );
}


int DigitalIO::configureLine( unsigned int line, bool output )
{
  lock();
  int r = configureLineUnlocked( line, output );
  unlock();
  return r;
}


int DigitalIO::configureLineUnlocked( unsigned int line, bool output )
{
  if ( line >= MaxDIOLines )
    return WriteError;
  DIOLineWriteable[line] = output;
  return 0;
}


int DigitalIO::configureLines( unsigned int lines, unsigned int output )
{
  unsigned int bit = 1;
  for ( unsigned int k=0; k<MaxDIOLines; k++ ) {
    if ( (lines & bit) > 0 )
      DIOLineWriteable[k] = ( (output & bit) > 0 );
    bit *= 2;
  }
  return 0;
}


bool DigitalIO::lineConfiguration( unsigned int line ) const
{
  if ( line >= MaxDIOLines )
    return false;
  return DIOLineWriteable[line];
}


int DigitalIO::write( unsigned int line, bool val )
{
  lock();
  int r = writeUnlocked( line, val );
  unlock();
  return r;
}


int DigitalIO::read( unsigned int line, bool &val )
{
  lock();
  int r = readUnlocked( line, val );
  unlock();
  return r;
}


int DigitalIO::setSyncPulse( int modemask, int modebits, unsigned int line,
			     double duration, int mode )
{
  return InvalidDevice;
}


int DigitalIO::clearSyncPulse( int modemask, int modebits )
{
  return InvalidDevice;
}


}; /* namespace relacs */

