/*
  attcs3310/cs3310dio.cc
  Control the cs3310 attenuator via a DigitalIO device

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#include <cmath>
#include <unistd.h>
#include <time.h>
#include <relacs/attcs3310/cs3310dio.h>
using namespace std;
using namespace relacs;

namespace attcs3310 {


CS3310DIO::CS3310DIO( DigitalIO *dio )
  : Attenuator( "CS3310DIO" ),
    DIO( 0 )
{
  Options opts;
  open( *dio, opts );
}


CS3310DIO::CS3310DIO( void )
  : Attenuator( "CS3310DIO" ),
    DIO( 0 )
{
}


CS3310DIO::~CS3310DIO( void )
{
}


int CS3310DIO::open( DigitalIO &dio, const Options &opts )
{
  DIO = &dio;

  Info.clear();
  
  if ( isOpen() ) {
    DIOId = DIO->allocateLine( CS );
    if (DIOId <= 0 ) {
      cerr << "! warning: CS3310DIO::open( DigitalIO ) -> cannot allocate CS pin.\n";
      DIO = 0;
      return InvalidDevice;
    }
    else {
      bool failed = false;
      if ( DIO->allocateLine( MUTE, DIOId ) <= 0 )
	failed = true;
      if ( DIO->allocateLine( ZCEN, DIOId ) <= 0 )
	failed = true;
      if ( DIO->allocateLine( DATAOUT, DIOId ) <= 0 )
	failed = true;
      if ( DIO->allocateLine( DATAIN, DIOId ) <= 0 )
	failed = true;
      if ( DIO->allocateLine( STROBE, DIOId ) <= 0 )
	failed = true;
      if ( failed ) {
	DIO->freeLines( DIOId );
	cerr << "! warning: CS3310DIO::open( DigitalIO ) -> cannot allocate pins.\n";
	DIO = 0;
	return InvalidDevice;
      }
      else {
	setDeviceFile( dio.deviceIdent() );
	return open();
      }
    }
  }
  else {
    if ( DIO != 0 && ! DIO->isOpen() ) {
      DIO = 0;
      return NotOpen;
    }
    else {
      DIO = 0;
      return InvalidDevice;
    }
  }
}


int CS3310DIO::open( Device &device, const Options &opts )
{
  return open( dynamic_cast<DigitalIO&>( device ), opts );
}


int CS3310DIO::open( void )
{
  Info.clear();

  if ( ! isOpen() )
    return NotOpen;

  // setup dio:

  // configure digital I/O lines:
  DIO->configureLine( CS, true );
  DIO->configureLine( MUTE, true );
  DIO->configureLine( ZCEN, true );
  DIO->configureLine( DATAOUT, true );
  DIO->configureLine( DATAIN, false );
  DIO->configureLine( STROBE, true );

  // reset:
  DIO->write( CS, true );
  DIO->write( MUTE, true );
  DIO->write( ZCEN, true );

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
    Level[0] = MuteGain;
    Level[1] = MuteGain;
    for ( int n=0; n<Tries; n++ ) {
      ar = write();
      if ( ar == 0 )
	break;
    }
    setDeviceVendor( "Crystal Semiconductor Corporation (Austin, TX)" );
    setDeviceName( "CS3310 stereo digital volume control" );
    setInfo();
    Info.addNumber( "resolution", 0.5, "dB" );
    return 0;
  }

}


bool CS3310DIO::isOpen( void ) const
{
  return ( DIO != 0 && DIO->isOpen() );
}


void CS3310DIO::close( void )
{
  if ( isOpen() ) {
    // CS=1, MUTE=0, ZCEN=1:
    DIO->write( CS, true );
    DIO->write( MUTE, false );
    DIO->write( ZCEN, true );
    DIO->freeLines( DIOId );
  }

  Info.clear();
  DIO = 0;
}


const Options &CS3310DIO::settings( void ) const
{
  Settings.clear();
  if ( Level[1] == MuteGain )
    Settings.addText( "level1", "muted" );
  else
    Settings.addNumber( "level1", 0.5 * ( ZeroGain - Level[1] ), "dB" );
  if ( Level[0] == MuteGain )
    Settings.addText( "level2", "muted" );
  else
    Settings.addNumber( "level2", 0.5 * ( ZeroGain - Level[0] ), "dB" );
  return Settings;
}


int CS3310DIO::lines( void ) const
{
  return 2;
}


double CS3310DIO::minLevel( void ) const
{
  return 0.5 * ( ZeroGain - MaxGain );
}


double CS3310DIO::maxLevel( void ) const
{
  return 0.5 * ( ZeroGain - MinGain );
}


void CS3310DIO::levels( vector<double> &l ) const
{
  l.clear();
  l.reserve( MaxGain - MinGain + 1 );
  for ( int k=MaxGain; k>= MinGain; k-- )
    l.push_back( 0.5 * ( ZeroGain - MinGain ) );
}


int CS3310DIO::attenuate( int di, double &decibel )
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


int CS3310DIO::testAttenuate( int di, double &decibel )
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


int CS3310DIO::setZeroCrossing( bool enable )
{
  if ( !isOpen() )
    return NotOpen;

  DIO->write( ZCEN, enable );

  return 0;
}


int CS3310DIO::setMute( bool mute )
{
  if ( !isOpen() )
    return NotOpen;

  DIO->write( MUTE, mute );

  return 0;
}


int CS3310DIO::calibrate( void )
{
  if ( !isOpen() )
    return NotOpen;

  // calibrate chip:
  // MUTE=0
  DIO->write( MUTE, false );
  usleep( 2500 );
  // MUTE=1
  DIO->write( MUTE, true );

  return 0;
}


int CS3310DIO::write( void )
{
  if ( !isOpen() )
    return NotOpen;

  // write attenuation levels:
  //  struct timespec ts = { tv_sec: 0, tv_nsec: 5000 };
  //  struct timespec ts = { tv_sec: 0, tv_nsec: 0 };
  //#define NSLEEP nanosleep( &ts, NULL );
#define NSLEEP

  DIO->write( CS, false );          // CS\ = 0 low
  DIO->write( STROBE, false );      // Strobe (D0, Sclk) = 0 low

  // output levels:
  for ( int k=0; k<2; k++ ) {
    for ( int i=7; i>=0; i-- ) {
      bool data = (Level[k] >> i) & 0x01;
      DIO->write( DATAOUT, data );  // write one bit
      NSLEEP
      DIO->write( STROBE, true );   // Strobe (D0, Sclk) = 1 high
      NSLEEP
      DIO->write( STROBE, false );  // Strobe (D0, Sclk) = 0 low
      NSLEEP
    }
  }

  // output levels again and read back:
  unsigned char buffer[2];
  for ( int k=0; k<2; k++ ) {
    buffer[k] = 0;
    for ( int i=7; i>=0; i-- ) {
      bool data = (Level[k] >> i) & 0x01;
      DIO->write( DATAOUT, data );  // write one bit
      NSLEEP
      DIO->write( STROBE, true );   // Strobe (D0, Sclk) = 1 high
      bool val = false;
      DIO->read( DATAIN, val );     // read one bit
      buffer[k] = buffer[k] << 1;
      if ( val )
	buffer[k] |= 1;
      NSLEEP
      DIO->write( STROBE, false );  // Strobe (D0, Sclk) = 0 low
      NSLEEP
    }
  }

  NSLEEP
  DIO->write( CS, true );          // CS\ = 1 high

  if ( buffer[0] != Level[0] || 
       buffer[1] != Level[1] )
    return  WriteError;

  return 0;

#undef NSLEEP
}


}; /* namespace attcs3310 */
