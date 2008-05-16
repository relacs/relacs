/*
  attcs3310/cs3310serialnidio.cc
  Control the cs3310 attenuator via the nieseries/nidio serial digital IO

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

#include <sstream>
#include <cmath>
#include <unistd.h>
#include <relacs/attcs3310/cs3310serialnidio.h>
using namespace std;
using namespace relacs;
using namespace nieseries;

namespace attcs3310 {


CS3310SerialNIDIO::CS3310SerialNIDIO( const string &device )
  : Attenuator( "CS3310SerialNIDIO" ),
    DIO( 0 ),
    Own( false )
{
  open( device );
}


CS3310SerialNIDIO::CS3310SerialNIDIO( NIDIO *nidio )
  : Attenuator( "CS3310SerialNIDIO" ),
    DIO( 0 ),
    Own( false )
{
  open( *nidio );
}


CS3310SerialNIDIO::CS3310SerialNIDIO( void )
  : Attenuator( "CS3310SerialNIDIO" ),
    DIO( 0 ),
    Own( false )
{
}


CS3310SerialNIDIO::~CS3310SerialNIDIO( void )
{
  if ( Own )
    delete DIO;
}


int CS3310SerialNIDIO::open( const string &device, long mode )
{
  if ( DIO != 0  && !DIO->isOpen() ) {
    if ( Own )
      delete DIO;
    DIO = 0;
  }

  if ( DIO == 0 ) {
    DIO = new NIDIO( device );
    if ( DIO->isOpen() ) {
      if ( DIO->allocPins( DioPins ) > 0 ) {
	cerr << "! warning: CS3310SerialNIDIO::open( device ) -> cannot allocate pins.\n";
	DIO->close();
	delete DIO;
	DIO = 0;
	Own = false;
	return InvalidDevice;
      }
      else {
	Own = true;
	setDeviceFile( device );
	return open();
      }
    }
    else {
      bool notopen = ( DIO != NULL && ! DIO->isOpen() );
      DIO = 0;
      Own = false;
      cerr << "! warning: CS3310SerialNIDIO::open( device ) -> cannot open NIDIO.\n";
      if ( notopen )
	return NotOpen;
      else
	return InvalidDevice;
    }
  }

  return 0;
}


int CS3310SerialNIDIO::open( NIDIO &nidio, long mode )
{
  if ( DIO != 0  && !DIO->isOpen() ) {
    if ( Own )
      delete DIO;
    DIO = 0;
  }

  if ( DIO == 0 ) {
    DIO = &nidio;
    Own = false;
  }

  if ( isOpen() ) {
    if ( DIO->allocPins( DioPins ) > 0 ) {
      cerr << "! warning: CS3310SerialNIDIO::open( device ) -> cannot allocate pins.\n";
      DIO = 0;
      Own = false;
      return InvalidDevice;
    }
    else {
      setDeviceFile( nidio.deviceIdent() );
      return open();
    }
  }
  else {
    if ( DIO != NULL && ! DIO->isOpen() )
      return NotOpen;
    else
      return InvalidDevice;
  }
}


int CS3310SerialNIDIO::open( Device &device, long mode )
{
  return open( dynamic_cast<NIDIO&>( device ) );
}


int CS3310SerialNIDIO::open( void )
{
  if ( ! isOpen() )
    return NotOpen;

  // setup dio:
  // DIO->setTiming( false, false );  // 0.6um period of EXSTRB
  DIO->setTiming( true, false );      // 1.2um period of EXSTRB
  //  DIO->setTiming( false, true );  // 5um period of EXSTRB
  //  DIO->setTiming( true, true );   // 5um period of EXSTRB
  
  // configure for parallel output: CS\ = DIO1, MUTE\ = DIO2, ZCEN = DIO3
  DIO->configure( Mask | DATAOUT, DioPins );
  DIO->out( CS | MUTE | ZCEN, Mask );

  calibrate();
    
  // check whether attenuator is working:
  bool failed = false;
  // set attenuation (software mute):
  unsigned char testatt = ZeroGain/2;
  unsigned char buffer[4];
  for ( int k=0; k<4; k++ )
    buffer[k] = testatt;
  // try:
  for ( int c=0; c<Tries; c++ ) {

    // CS=0
    DIO->out( 0x00, CS );
    // write levels:
    int wn = DIO->write( buffer, 4 );
    // CS=1
    DIO->out( CS, CS );
    if ( wn != 4 )
      failed = true;
      
    // read set attenuation:
    int rn = DIO->read( buffer, 4 );
    if ( rn != 4 ||
	 buffer[2] != testatt ||
	 buffer[3] != testatt )
      failed = true;
      
    if ( ! failed )
      break;
      
    usleep( Delay );

    // new values:
    testatt++;
    for ( int k=0; k<4; k++ )
      buffer[k] = testatt;

  }

  if ( failed ) {
    // attenuator is not active:
    close();
    return WriteError;
  }

  setDeviceVendor( "Crystal Semiconductor Corporation (Austin, TX)" );
  setDeviceName( "CS3310 stereo digital volume control" );
  return 0;
}


bool CS3310SerialNIDIO::isOpen( void ) const
{
  return ( DIO != 0 && DIO->isOpen() );
}


void CS3310SerialNIDIO::close( void )
{
  if ( isOpen() ) {
    // CS=1, MUTE=0, ZCEN=1:
    DIO->out( CS | ZCEN, Mask );
    DIO->freePins( DioPins );
  }

  if ( DIO != 0 && Own ) {
    DIO->close();
    delete DIO;
  }
  DIO = 0;
  Own = false;
}


string CS3310SerialNIDIO::settings( void ) const
{
  ostringstream ss;
  ss << "level1: " << (int)Level[0]
     << ";level2: " << (int)Level[1] << ends;
  return ss.str();
}


int CS3310SerialNIDIO::lines( void ) const
{
  return 2;
}


int CS3310SerialNIDIO::attenuate( int di, double &decibel )
{
  if ( di < 0 || di >= 2 )
    return InvalidDevice;

  if ( !isOpen() )
    return NotOpen;

  int r = 0;
  int att = MuteGain;

  if ( decibel != MuteAttenuationLevel ) {

    att = int( rint( ZeroGain - 2.0 * decibel ) );

    // overflow / underflow?
    if ( att > MaxGain ) {
      att = MaxGain;
      r = Overflow;
    }
    else if ( att < MinGain ) {
      att = MinGain;
      r = Underflow;
    }

  }

  // set attenuation:
  Level[1-di] = att;

  // try at maximum Tries times:
  int wr = 0;
  for ( int c=0; c<Tries; c++ ) {

    // write attenuation levels:
    unsigned char buffer[4];
    for ( int k=0; k<2;  k++ ) {
      buffer[k] = Level[k];
      buffer[k+2] = Level[k];
    }
    // CS=0
    DIO->out( 0x00, CS );
    int wn = DIO->write( buffer, 4 );
    // CS=1
    DIO->out( CS, CS );
    if ( wn != 4 )
      wr = WriteError;

    // read set attenuation:
    int rn = DIO->read( buffer, 4 );
    if ( rn != 4 ||
	 buffer[2] != Level[0] || 
	 buffer[3] != Level[1] )
      wr =  WriteError;

    if ( wr != WriteError )
      break;

    usleep( Delay );
  }
  if ( wr != 0 )
    r = wr;

  // calculate adjusted intensity:
  if ( decibel != MuteAttenuationLevel )
    decibel = 0.5 * ( ZeroGain - att );

  return r;
}


int CS3310SerialNIDIO::testAttenuate( int di, double &decibel )
{

  if ( di < 0 || di >= 2 )
    return InvalidDevice;

  if ( !isOpen() )
    return NotOpen;

  int r = 0;
  int att = MuteGain;

  if ( decibel != MuteAttenuationLevel ) {

    att = int( rint( ZeroGain - 2.0 * decibel ) );

    // overflow / underflow?
    if ( att > MaxGain ) {
      att = MaxGain;
      r = Overflow;
    }
    else if ( att < MinGain ) {
      att = MinGain;
      r = Underflow;
    }

    // calculate adjusted intensity:
    decibel = 0.5 * ( ZeroGain - att );

  }

  return r;
}


int CS3310SerialNIDIO::setZeroCrossing( bool enable )
{
  if ( !isOpen() )
    return NotOpen;

  DIO->out( enable ? ZCEN : 0x00, ZCEN );

  return 0;
}


int CS3310SerialNIDIO::setMute( bool mute )
{
  if ( !isOpen() )
    return NotOpen;

  DIO->out( mute ? 0x00 : MUTE, MUTE );

  return 0;
}


int CS3310SerialNIDIO::calibrate( void )
{
  if ( !isOpen() )
    return NotOpen;

  // calibrate chip:
  // MUTE=0
  DIO->out( 0x00, MUTE );
  usleep( 2500 );
  // MUTE=1
  DIO->out( MUTE, MUTE );

  return 0;
}


}; /* namespace attcs3310 */
