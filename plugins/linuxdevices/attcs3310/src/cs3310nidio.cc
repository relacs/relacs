/*
  attcs3310/cs3310nidio.cc
  Control the cs3310 attenuator via the nieseries/nidio software timed digital IO  

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
#include <cmath>
#include <unistd.h>
#include <time.h>
#include <relacs/attcs3310/cs3310nidio.h>
using namespace std;
using namespace relacs;
using namespace nieseries;

namespace attcs3310 {


CS3310NIDIO::CS3310NIDIO( const string &device )
  : Attenuator( "CS3310NIDIO" ),
    DIO( 0 ),
    Own( false )
{
  open( device );
}


CS3310NIDIO::CS3310NIDIO( NIDIO *nidio )
  : Attenuator( "CS3310NIDIO" ),
    DIO( 0 ),
    Own( false )
{
  open( *nidio );
}


CS3310NIDIO::CS3310NIDIO( void )
  : Attenuator( "CS3310NIDIO" ),
    DIO( 0 ),
    Own( false )
{
}


CS3310NIDIO::~CS3310NIDIO( void )
{
  if ( Own )
    delete DIO;
}


int CS3310NIDIO::open( const string &device, long mode )
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
	cerr << "! warning: CS3310NIDIO::open( device ) -> cannot allocate pins.\n";
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
      cerr << "! warning: CS3310NIDIO::open( device ) -> cannot open NIDIO.\n";
      if ( notopen )
	return NotOpen;
      else
	return InvalidDevice;
    }
  }
  return 0;
}


int CS3310NIDIO::open( NIDIO &nidio, long mode )
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
      cerr << "! warning: CS3310NIDIO::open( device ) -> cannot allocate pins.\n";
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


int CS3310NIDIO::open( Device &device, long mode )
{
  return open( dynamic_cast<NIDIO&>( device ) );
}


int CS3310NIDIO::open( void )
{
  if ( ! isOpen() )
    return NotOpen;

  // setup dio:

  // configure for parallel output: CS\ = DIO1, MUTE\ = DIO2, ZCEN = DIO3
  DIO->configure( CS | MUTE | ZCEN | DATAOUT, DioPins );
  DIO->out( CS | MUTE | ZCEN, CS | MUTE | ZCEN );

  calibrate();
  
  // check if attenuator is working:
  int ar = 0;
  // set attenuation:
  Level[0] = ZeroGain/2;
  Level[1] = ZeroGain/2;
  for ( int n=0; n<Tries; n++ ) {
    ar = write();
    if ( ar == 0 )
      break;
    Level[0]++;
    Level[1]++;
  }
  if ( ar != 0 ) {
    // attenuator is not active:
    close();
    return WriteError;
  }
  else {
    // mute attenuator:
    Level[0] = 0;
    Level[1] = 0;
    for ( int n=0; n<Tries; n++ ) {
      ar = write();
      if ( ar == 0 )
	break;
    }
    setDeviceVendor( "Crystal Semiconductor Corporation (Austin, TX)" );
    setDeviceName( "CS3310 stereo digital volume control" );
    return 0;
  }

}


bool CS3310NIDIO::isOpen( void ) const
{
  return ( DIO != 0 && DIO->isOpen() );
}


void CS3310NIDIO::close( void )
{
  if ( isOpen() ) {
    // CS=1, MUTE=0, ZCEN=1:
    DIO->out( CS | ZCEN, CS | MUTE | ZCEN );
    DIO->freePins( DioPins );
  }

  if ( DIO != 0 && Own ) {
    DIO->close();
    delete DIO;
  }
  DIO = 0;
  Own = false;
}


string CS3310NIDIO::settings( void ) const
{
  ostringstream ss;
  ss << "level1: " << (int)Level[0]
     << ";level2: " << (int)Level[1];
  return ss.str();
}


int CS3310NIDIO::lines( void ) const
{
  return 2;
}


int CS3310NIDIO::attenuate( int di, double &decibel )
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

  // write attenuation levels:
  int ar = 0;
  for ( int n=0; n<Tries; n++ ) {
    ar = write();
    if ( ar == 0 )
      break;
  }
  if ( ar != 0 )
    r = WriteError;

  // calculate adjusted intensity:
  if ( decibel != MuteAttenuationLevel )
    decibel = 0.5 * ( ZeroGain - att );

  return r;
}


int CS3310NIDIO::testAttenuate( int di, double &decibel )
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


int CS3310NIDIO::setZeroCrossing( bool enable )
{
  if ( !isOpen() )
    return NotOpen;

  DIO->out( enable ? ZCEN : 0x00, ZCEN );

  return 0;
}


int CS3310NIDIO::setMute( bool mute )
{
  if ( !isOpen() )
    return NotOpen;

  DIO->out( mute ? 0x00 : MUTE, MUTE );

  return 0;
}


int CS3310NIDIO::calibrate( void )
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


int CS3310NIDIO::write( void )
{
  if ( !isOpen() )
    return NotOpen;

  // write attenuation levels:
  //  struct timespec ts = { tv_sec: 0, tv_nsec: 5000 };
  //  struct timespec ts = { tv_sec: 0, tv_nsec: 0 };
  //#define NSLEEP nanosleep( &ts, NULL );
#define NSLEEP

  DIO->out( 0x00, CS );        // CS\ = 0 low
  DIO->setClock( false );      // Strobe (D0, Sclk) = 0 low

  // output levels:
  for ( int k=0; k<2; k++ ) {
    for ( int i=7; i>=0; i-- ) {
      int data = (Level[k] >> i) & 0x01;
      DIO->out( data > 0 ? DATAOUT : 0x00, DATAOUT ); // write one bit
      NSLEEP
      DIO->setClock( true );      // Strobe (D0, Sclk) = 1 high
      NSLEEP
      DIO->setClock( false );      // Strobe (D0, Sclk) = 0 low
      NSLEEP
    }
  }

  // output levels again and read back:
  unsigned char buffer[2];
  for ( int k=0; k<2; k++ ) {
    buffer[k] = 0;
    for ( int i=7; i>=0; i-- ) {
      int data = (Level[k] >> i) & 0x01;
      DIO->out( data > 0 ? DATAOUT : 0x00, DATAOUT ); // write one bit
      NSLEEP
      DIO->setClock( true );      // Strobe (D0, Sclk) = 1 high
      DIO->in( data );            // read one bit
      buffer[k] = buffer[k] << 1;
      buffer[k] |= (data & DATAIN) >> 4;
      NSLEEP
      DIO->setClock( false );      // Strobe (D0, Sclk) = 0 low
      NSLEEP
    }
  }

  NSLEEP
  DIO->out( CS, CS );        // CS\ = 0 high

  if ( buffer[0] != Level[0] || 
       buffer[1] != Level[1] )
    return  WriteError;

  return 0;

#undef NSLEEP
}


}; /* namespace attcs3310 */
