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
#include <relacs/daqflex/daqflexdigitalio.h>
using namespace std;
using namespace relacs;

namespace daqflex {


DAQFlexDigitalIO::DAQFlexDigitalIO( void ) 
  : DigitalIO( "DAQFlexDigitalIO" )
{
  DAQFlexDevice = NULL;
}


DAQFlexDigitalIO::DAQFlexDigitalIO( DAQFlexCore &daqflexdevice, const Options &opts ) 
  : DigitalIO( "DAQFlexDigitalIO" )
{
  DAQFlexDevice = NULL;
  open( daqflexdevice, opts );
}

  
DAQFlexDigitalIO::~DAQFlexDigitalIO( void ) 
{
  close();
}


int DAQFlexDigitalIO::open( DAQFlexCore &daqflexdevice, const Options &opts )
{ 
  if ( isOpen() )
    return -5;

  if ( &daqflexdevice == NULL )
    return InvalidDevice;

  DAQFlexDevice = &daqflexdevice;
  if ( !DAQFlexDevice->isOpen() ) {
    setErrorStr( "Daqflex core device " + DAQFlexDevice->deviceName() + " is not open." );
    return NotOpen;
  }

  DigitalIO::open( *this, opts );

  // set basic device infos:
  setDeviceName( DAQFlexDevice->deviceName() );
  setDeviceVendor( DAQFlexDevice->deviceVendor() );
  setDeviceFile( DAQFlexDevice->deviceFile() );

  setInfo();
  
  return 0;
}


int DAQFlexDigitalIO::open( Device &device, const Options &opts )
{
  return open( dynamic_cast<DAQFlexCore&>( device ), opts );
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
  if ( !isOpen() )
    return 0;
  return DAQFlexDevice->dioLines();
}


int DAQFlexDigitalIO::configureLine( int line, bool output )
{
  string r = DAQFlexDevice->sendMessage( "DIO{0/" + Str( line ) + "}:DIR=" + ( output ? "OUT" : "IN" ) );
  return r.empty() ? WriteError : 0;
}


int DAQFlexDigitalIO::configureLines( int lines, int output )
{
  int bit = 1;
  for ( int channel=0; channel<32; channel++ ) {
    if ( ( lines & bit ) > 0 ) {
      bool direction = false;
      if ( ( output & bit ) > 0 )
	direction = true;
      string r = DAQFlexDevice->sendMessage( "DIO{0/" + Str( channel ) + "}:DIR=" + ( direction ? "OUT" : "IN" ) );
      if ( r.empty() )
	return WriteError;
    }
    bit *= 2;
  }
  return 0;
}


int DAQFlexDigitalIO::write( int line, bool val )
{
  string r = DAQFlexDevice->sendMessage( "DIO{0/" + Str( line ) + "}:VALUE=" + ( val ? "1" : "0" ) );
  return r.empty() ? WriteError : 0;
}


int DAQFlexDigitalIO::read( int line, bool &val ) const
{
  string r = DAQFlexDevice->sendMessage( "DIO{0/" + Str( line ) + "}:VALUE" );
  if ( r.empty() )
    return ReadError;
  // XXX READ THE RESULT
  return 0;
}


int DAQFlexDigitalIO::writeLines( int lines, int val )
{
  /*
  unsigned int ival = val;
  if ( daqflex_dio_bitfield2( DeviceP, SubDevice, lines, &ival, 0 ) < 0 ) {
    daqflex_perror( "DAQFlexDigitalIO::write()" );
    cerr << "! error: DAQFlexDigitalIO::write() -> "
	 << "Writing on DIO subdevice " << SubDevice << " failed\n";
    return WriteError;
  }
  */
  return 0;
}


int DAQFlexDigitalIO::readLines( int lines, int &val ) const
{
  /*
  unsigned int ival = 0;
  if ( daqflex_dio_bitfield2( DeviceP, SubDevice, lines, &ival, 0 ) < 0 ) {
    daqflex_perror( "DAQFlexDigitalIO::read()" );
    cerr << "! error: DAQFlexDigitalIO::read() -> "
	 << "Reading from DIO subdevice " << SubDevice
	 << " failed\n";
    return ReadError;
  }
  val = ival & lines;
  */
  return 0;
}


}; /* namespace daqflex */
