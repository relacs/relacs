/*
  daqflex/daqflexdigitalio.cc
  Interface for accessing digital I/O lines of a DAQFlex board from Measurement Computing.

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
#include <QMutexLocker>
#include <relacs/daqflex/daqflexdigitalio.h>
using namespace std;
using namespace relacs;

namespace daqflex {


DAQFlexDigitalIO::DAQFlexDigitalIO( void ) 
  : DigitalIO( "DAQFlexDigitalIO" )
{
  DAQFlexDevice = NULL;
  Levels = 0;
  initOptions();
}


DAQFlexDigitalIO::DAQFlexDigitalIO( DAQFlexCore &daqflexdevice, const Options &opts ) 
  : DAQFlexDigitalIO()
{
  Options::read(opts);
  open( daqflexdevice );
}

  
DAQFlexDigitalIO::~DAQFlexDigitalIO( void ) 
{
  close();
}


int DAQFlexDigitalIO::open( DAQFlexCore &daqflexdevice)
{ 
  if ( isOpen() )
    return -5;

  DAQFlexDevice = &daqflexdevice;
  if ( !DAQFlexDevice->isOpen() ) {
    setErrorStr( "Daqflex core device " + DAQFlexDevice->deviceName() + " is not open." );
    return NotOpen;
  }

  DigitalIO::open( *this );

  Levels = 0;

  // set basic device infos:
  setDeviceName( DAQFlexDevice->deviceName() );
  setDeviceVendor( DAQFlexDevice->deviceVendor() );
  setDeviceFile( DAQFlexDevice->deviceFile() );

  setInfo();
  
  return 0;
}


int DAQFlexDigitalIO::open( Device &device)
{
  return open( dynamic_cast<DAQFlexCore&>( device ) );
}


bool DAQFlexDigitalIO::isOpen( void ) const 
{ 
  lock();
  bool o = ( DAQFlexDevice != NULL && DAQFlexDevice->isOpen() );
  unlock();
  return o;
}


void DAQFlexDigitalIO::close( void ) 
{
  if ( ! isOpen() )
    return;

  // clear flags:
  DAQFlexDevice = NULL;

  Info.clear();
  Settings.clear();
}


int DAQFlexDigitalIO::lines( void ) const
{ 
  if ( DAQFlexDevice == NULL )
    return 0;
  return DAQFlexDevice->dioLines();
}


int DAQFlexDigitalIO::configureLineUnlocked( unsigned int line, bool output )
{
  string r = DAQFlexDevice->sendMessage( "DIO{0/" + Str( line ) + "}:DIR=" + ( output ? "OUT" : "IN" ) );
  return DAQFlexDevice->failed() || r.empty() ? WriteError : 0;
}


int DAQFlexDigitalIO::configureLines( unsigned int lines, unsigned int output )
{
  QMutexLocker diolocker( mutex() );
  unsigned int bit = 1;
  for ( int channel=0; channel<DAQFlexDigitalIO::lines(); channel++ ) {
    if ( ( lines & bit ) > 0 ) {
      bool direction = false;
      if ( ( output & bit ) > 0 )
	direction = true;
      string r = DAQFlexDevice->sendMessage( "DIO{0/" + Str( channel ) + "}:DIR=" + ( direction ? "OUT" : "IN" ) );
      if ( DAQFlexDevice->failed() || r.empty() )
	return WriteError;
    }
    bit *= 2;
  }
  return 0;
}


int DAQFlexDigitalIO::writeUnlocked( unsigned int line, bool val )
{
  if ( (int)line >= lines() )
    return WriteError;
  string rs = "DIO{0/" + Str( line ) + "}:VALUE";
  string r = DAQFlexDevice->sendMessage( rs + "=" + ( val ? "1" : "0" ) );
  if ( DAQFlexDevice->failed() || r != rs )
    return WriteError;
  if ( val )
    Levels |= 1 << line;
  else
    Levels &= ~(1 << line);
  return 0;
}


int DAQFlexDigitalIO::readUnlocked( unsigned int line, bool &val )
{
  if ( (int)line >= lines() )
    return ReadError;
  string rs = "DIO{0/" + Str( line ) + "}:VALUE";
  string r = DAQFlexDevice->sendMessage( "?" + rs );
  if ( DAQFlexDevice->failed() || r.substr(0, rs.size() ) != rs )
    return ReadError;
  val = ( stoi( r.substr( rs.size() + 1 ) ) > 0 );
  return 0;
}


int DAQFlexDigitalIO::writeLines( unsigned int lines, unsigned int val )
{
  QMutexLocker diolocker( mutex() );
  unsigned int rv = Levels;
  rv &= ~lines; // keep the levels of the non-specified lines
  rv |= val & lines;  // set the values of the specified lines
  string rs = "DIO{0}:VALUE";
  string r = DAQFlexDevice->sendMessage( rs + "=" + Str( rv ) );
  if ( DAQFlexDevice->failed() || r != rs )
    return WriteError;
  Levels = rv;
  return 0;
}


int DAQFlexDigitalIO::readLines( unsigned int lines, unsigned int &val )
{
  QMutexLocker diolocker( mutex() );
  string rs = "DIO{0}:VALUE";
  string r = DAQFlexDevice->sendMessage( "?" + rs );
  if ( DAQFlexDevice->failed() || r.substr(0, rs.size() ) != rs )
    return ReadError;
  val = stoi( r.substr( rs.size() + 1 ) );
  return 0;
}


}; /* namespace daqflex */
