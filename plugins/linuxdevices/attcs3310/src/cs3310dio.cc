/*
  attcs3310/cs3310dio.cc
  Control the cs3310 attenuator via a DigitalIO device

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

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unistd.h>
#include <time.h>
#include <relacs/attcs3310/cs3310dio.h>
using namespace std;
using namespace relacs;

namespace attcs3310 {


CS3310DIO::CS3310DIO( DigitalIO *dio )
  : CS3310DIO()
{
  open( *dio );
}


CS3310DIO::CS3310DIO( void )
  : Attenuator( "CS3310DIO" ),
    DIO( 0 )
{
  initOptions();
}


CS3310DIO::~CS3310DIO( void )
{
}

void CS3310DIO::initOptions()
{
  Attenuator::initOptions();

  addBoolean( "zerocrossing", "Set attenuation level only at a zero crossing of the input", false );
  addInteger( "strobepin", "DIO line for strobing data (STROBE)", STROBE );
  addInteger( "datainpin", "DIO line for writing data to the chip  (DATAIN)", DATAIN );
  addInteger( "dataoutpin", "DIO line for reading data from the chip  (DATAOUT)", DATAOUT );
  addInteger( "cspin", "DIO line for chip select (CS)", CS );
  addInteger( "mutepin", "DIO line for muting the chip (MUTE)", MUTE );
  addInteger( "zcenpin", "DIO line for enabling zero crossing (ZCEN)", ZCEN );
}

int CS3310DIO::open( DigitalIO &dio)
{
  clearError();
  DIO = &dio;

  Info.clear();
  
  if ( isOpen() ) {
    STROBE = integer( "strobepin", 0, STROBE );
    DATAIN = integer( "datainpin", 0, DATAIN );
    DATAOUT = integer( "dataoutpin", 0, DATAOUT );
    CS = integer( "cspin", 0, CS );
    MUTE = integer( "mutepin", 0, MUTE );
    ZCEN = integer( "zcenpin", 0, ZCEN );

    string failedpin = "";
    DIOId = DIO->allocateLine( STROBE );
    if ( DIOId <= 0 )
      failedpin = "STROBE=" + Str( STROBE );
    else {
      if ( DIO->allocateLine( DATAIN, DIOId ) <= 0 )
	failedpin = "DATAIN =" + Str( DATAIN );
      if ( DATAOUT >= 0 && DIO->allocateLine( DATAOUT, DIOId ) <= 0 )
	failedpin = "DATAOUT =" + Str( DATAOUT );
      if ( CS >= 0 && DIO->allocateLine( CS, DIOId ) <= 0 )
	failedpin += "CS =" + Str( CS );
      if ( DIO->allocateLine( MUTE, DIOId ) <= 0 )
	failedpin += "MUTE =" + Str( MUTE );
      if ( ZCEN >=0 && DIO->allocateLine( ZCEN, DIOId ) <= 0 )
	failedpin = "ZCEN =" + Str( ZCEN );
    }
    if ( ! failedpin.empty() ) {
      DIO->freeLines( DIOId );
      setErrorStr( "cannot allocate pins " + failedpin );
      DIO = 0;
      return InvalidDevice;
    }
    else {
      setDeviceFile( dio.deviceIdent() );
      return open( boolean( "zerocrossing", false ) );
    }
  }
  else {
    if ( DIO != 0 && ! DIO->isOpen() ) {
      DIO = 0;
      return NotOpen;
    }
    else {
      DIO = 0;
      setErrorStr( "no DIO device assigned" );
      return InvalidDevice;
    }
  }
}


int CS3310DIO::open( Device &device )
{
  return open( dynamic_cast<DigitalIO&>( device ) );
}


int CS3310DIO::open( bool zerocrossing )
{
  Info.clear();

  if ( ! isOpen() )
    return NotOpen;

  // setup dio:
  DIO->lock();

  // configure digital I/O lines:
  string failedconfigpin = "";
  if ( DIO->configureLineUnlocked( STROBE, true ) < 0 )
    failedconfigpin += "STROBE ";
  if ( DIO->configureLineUnlocked( DATAIN, true ) < 0 )
    failedconfigpin += "DATAIN ";
  if ( DATAOUT >= 0 && DIO->configureLineUnlocked( DATAOUT, false ) < 0 )
    failedconfigpin += "DATAOUT ";
  if ( CS >= 0 && DIO->configureLineUnlocked( CS, true ) < 0 )
    failedconfigpin += "CS ";
  if ( DIO->configureLineUnlocked( MUTE, true ) < 0 )
    failedconfigpin += "MUTE ";
  if ( ZCEN >= 0 && DIO->configureLineUnlocked( ZCEN, true ) < 0 )
    failedconfigpin += "ZCEN ";

  // reset:
  string failedwritepin = "";
  if ( CS >= 0 && DIO->writeUnlocked( CS, true ) < 0 )
    failedwritepin += "CS ";
  if ( DIO->writeUnlocked( MUTE, true ) < 0 )
    failedwritepin += "MUTE ";
  if ( ZCEN >= 0 && DIO->writeUnlocked( ZCEN, zerocrossing ) < 0 )
    failedwritepin += "ZCEN ";

  DIO->unlock();

  if ( ! failedconfigpin.empty() ) {
    close();
    setErrorStr( "failed to configure DIO lines " + failedconfigpin );
    return WriteError;
  }
  if ( ! failedwritepin.empty() ) {
    close();
    setErrorStr( "failed to write to DIO lines " + failedwritepin );
    return WriteError;
  }

  if ( calibrate() < 0 ) {
    close();
    setErrorStr( "failed to calibrate attenuator device" );
    return WriteError;
  }
  
  // check if attenuator is working:
  int ar = 0;
  // get some random attenuation level:
  srand ( ::time( NULL ) );
  int rl = rand() % 20 - 10;
  // set attenuation:
  Level[0] = ZeroGain/2 + rl;
  Level[1] = ZeroGain/2 + rl;
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
    setErrorStr( "attenuator is not active" );
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
    Info.addInteger( "strobepin", STROBE );
    Info.addInteger( "datainpin", DATAIN );
    Info.addInteger( "dataoutpin", DATAOUT );
    Info.addInteger( "cspin", CS );
    Info.addInteger( "mutepin", MUTE );
    Info.addInteger( "zcenpin", ZCEN );
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
    DIO->lock();
    // CS=1, MUTE=0, ZCEN=1:
    if ( CS >= 0 )
      DIO->writeUnlocked( CS, true );
    DIO->writeUnlocked( MUTE, false );
    if ( ZCEN >= 0 )
      DIO->writeUnlocked( ZCEN, true );
    DIO->freeLines( DIOId );
    DIO->unlock();
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

  if ( DIO == 0 )
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

  if ( DIO == 0 )
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
  if ( DIO == 0 )
    return NotOpen;

  if ( ZCEN >= 0 )
    return DIO->write( ZCEN, enable );
  else
    return 0;
}


int CS3310DIO::setMute( bool mute )
{
  if ( DIO == 0 )
    return NotOpen;

  return DIO->write( MUTE, mute );
}


int CS3310DIO::calibrate( void )
{
  if ( DIO == 0 )
    return NotOpen;

  // calibrate chip:
  int r = DIO->write( MUTE, false );
  if ( r == 0 ) {
    usleep( 2500 );
    r = DIO->write( MUTE, true );
  }
  return r;
}


int CS3310DIO::write( void )
{
  if ( DIO == 0 )
    return NotOpen;

  // write attenuation levels:
  //  struct timespec ts = { tv_sec: 0, tv_nsec: 5000 };
  //  struct timespec ts = { tv_sec: 0, tv_nsec: 0 };
  //#define NSLEEP nanosleep( &ts, NULL );
#define NSLEEP

  bool failed = false;
  DIO->lock();
  // CS\ = 1 low:
  if ( CS >= 0 && DIO->writeUnlocked( CS, false ) < 0 )
    failed = true;
  // Strobe (Sclk) low:
  if ( DIO->writeUnlocked( STROBE, false ) < 0 )
    failed = true;
  NSLEEP

  // output levels:
  for ( int k=0; k<2; k++ ) {
    for ( int i=7; i>=0; i-- ) {
      bool data = (Level[k] >> i) & 0x01;
      // write one bit:
      if ( DIO->writeUnlocked( DATAIN, data ) < 0 )
	failed = true;
      // Strobe (Sclk) = 1 high:
      if ( DIO->writeUnlocked( STROBE, true ) < 0 )
	failed = true;
      NSLEEP
	// Strobe (Sclk) = 0 low:
	if ( DIO->writeUnlocked( STROBE, false ) < 0 )
	  failed = true;
      NSLEEP
    }
  }
  
  // output levels again and read back:
  unsigned char buffer[2];
  if ( DATAOUT >= 0 && ! failed ) {
    for ( int k=0; k<2; k++ ) {
      buffer[k] = 0;
      for ( int i=7; i>=0; i-- ) {
	bool data = (Level[k] >> i) & 0x01;
	// write one bit:
	if ( DIO->writeUnlocked( DATAIN, data ) < 0 )
	  failed = true;
	// Strobe (Sclk) high:
	if ( DIO->writeUnlocked( STROBE, true ) < 0 )
	  failed = true;
	bool val = false;
	// read one bit:
	if ( DIO->readUnlocked( DATAOUT, val ) < 0 )
	  failed = true;
	buffer[k] = buffer[k] << 1;
	if ( val )
	  buffer[k] |= 1;
	NSLEEP
	  // Strobe (Sclk) low:
	if ( DIO->writeUnlocked( STROBE, false ) < 0 )
	  failed = true;
	NSLEEP
      }
    }
  }

  // CS\ = 1 high:
  if ( DIO->writeUnlocked( CS, true ) < 0 )
    failed = true;
  DIO->unlock();

  if ( failed || ( DATAOUT >= 0 && 
		   ( buffer[0] != Level[0] || buffer[1] != Level[1] ) ) )
    return  WriteError;

  return 0;

#undef NSLEEP
}


}; /* namespace attcs3310 */
