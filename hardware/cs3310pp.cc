/*
  cs3310pp.cc
  Control the cs3310 attenuator via the Linux ppdev parallel-port device driver.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>   
#include <linux/ppdev.h>
#include <time.h>
#include <sstream>
#include <cmath>
#include "cs3310pp.h"

using namespace std;


CS3310PP::CS3310PP( const string &device )
  : Attenuator( "CS3310PP" ),
    Handle( -1 )
{
  open( device );
}


CS3310PP::CS3310PP( void )
  : Attenuator( "CS3310PP" ),
    Handle( -1 )
{
}


CS3310PP::~CS3310PP( void )
{
  close();
}


int CS3310PP::open( const string &device, long mode )
{
  // open ppdev:
  Handle = ::open( device.c_str(), O_RDWR );
  // claim port:
  if ( Handle >= 0 && ::ioctl( Handle, PPCLAIM ) ) {
    ::close( Handle );
    Handle = -1;
  }
  if ( Handle < 0 )
    return InvalidDevice;

  calibrate();
  
  setZeroCrossing( true );
  
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
    ::close( Handle );
    Handle = -1;
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
    setDeviceFile( device );
    setDeviceVendor( "Crystal Semiconductor Corporation (Austin, TX)" );
    setDeviceName( "CS3310 stereo digital volume control" );
    return 0;
  }

}


bool CS3310PP::isOpen( void ) const
{
  return ( Handle >= 0 );
}


void CS3310PP::close( void )
{
  if ( Handle < 0 )
    return;

  setMute( true );

  ::ioctl( Handle, PPRELEASE );
  ::close( Handle );
  Handle = -1;
}


string CS3310PP::settings( void ) const
{
  ostringstream ss;
  ss << "level1: " << (int)Level[0]
     << ";level2: " << (int)Level[1] << ends;
  return ss.str();
}


int CS3310PP::lines( void ) const
{
  return 2;
}


int CS3310PP::attenuate( int di, double &decibel )
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


int CS3310PP::testAttenuate( int di, double &decibel )
{

  if ( di < 0 || di > 1 )
    return InvalidDevice;

  if ( Handle < 0 )
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


int CS3310PP::setZeroCrossing( bool enable )
{
  if ( !isOpen() )
    return NotOpen;

  struct ppdev_frob_struct frob;
  frob.mask = 0x08;

  if ( enable ) {
    // enable zero crossing:
    // Control-Bit 3 (ZCEN)=0 high, ZCEN aktiv
    frob.val = 0x00;
    ::ioctl( Handle, PPFCONTROL, &frob );
  }
  else {
    // disable zero crossing:
    // Control-Bit 3 (ZCEN)=1 low, ZCEN inaktiv
    frob.val = 0x08;
    ::ioctl( Handle, PPFCONTROL, &frob );
  }

  return 0;
}


int CS3310PP::setMute( bool mute )
{
  if ( !isOpen() )
    return NotOpen;

  struct ppdev_frob_struct frob;
  frob.mask = 0x04;

  if ( mute ) {
    // Control-Bit 2 (Mute\)=0 low, Mute aktiv 
    frob.val = 0x00;
    ::ioctl( Handle, PPFCONTROL, &frob );
  }
  else {
    // Control-Bit 2 (Mute\)=1 high, Mute inaktiv 
    frob.val = 0x04;
    ::ioctl( Handle, PPFCONTROL, &frob );
  }

  return 0;
}


int CS3310PP::calibrate( void )
{
  if ( !isOpen() )
    return NotOpen;

  // mute on:
  struct ppdev_frob_struct frob = { mask: 0x04, val: 0x00 };
  ::ioctl( Handle, PPFCONTROL, &frob ); // Control-Bit 2 (Mute\)=0 low, Mute aktiv 
  
  // sleep more than 2ms, like explained above:
  struct timespec ts = { tv_sec: 0, tv_nsec: 2500000 };
  nanosleep( &ts, NULL );
  
  // mute off:
  frob.val = 0x04;
  ::ioctl( Handle, PPFCONTROL, &frob ); // Control-Bit 2 (Mute\)=1 high, Mute inaktiv 

  return 0;
}


int CS3310PP::write( void )
{
  if ( !isOpen() )
    return NotOpen;

  // write attenuation levels:
  struct ppdev_frob_struct cson = { mask: 0x02, val: 0x02 };
  struct ppdev_frob_struct csoff = { mask: 0x02, val: 0x00 };
  struct ppdev_frob_struct sclkon = { mask: 0x01, val: 0x01 };
  struct ppdev_frob_struct sclkoff = { mask: 0x01, val: 0x00 };
  struct timespec ts = { tv_sec: 0, tv_nsec: 50000 };

  ::ioctl( Handle, PPFCONTROL, &cson );   // Control-Bit 1(CS\)=1 low
  ::ioctl( Handle, PPFCONTROL, &sclkon ); // Strobe (D0, Sclk)=1 low

  // output levels:
  for ( int k=0; k<2; k++ ) {
    for ( int i=7; i>=0; i-- ) {
      unsigned char data = (Level[k] >> i) & 0x01;
      ::ioctl( Handle, PPWDATA, &data );       // write one bit
      nanosleep( &ts, NULL );
      ::ioctl( Handle, PPFCONTROL, &sclkoff ); // Strobe (D0, Sclk)=0 high
      nanosleep( &ts, NULL );
      ::ioctl( Handle, PPFCONTROL, &sclkon );  // Strobe (D0, Sclk)=1 low
      nanosleep( &ts, NULL );
    }
  }

  // output levels again and read back:
  unsigned char buffer[2];
  for ( int k=0; k<2; k++ ) {
    buffer[k] = 0;
    for ( int i=7; i>=0; i-- ) {
      unsigned char data = (Level[k] >> i) & 0x01;
      ::ioctl( Handle, PPWDATA, &data );       // write one bit
      nanosleep( &ts, NULL );
      ::ioctl( Handle, PPFCONTROL, &sclkoff ); // Strobe (D0, Sclk)=0 high
      ::ioctl( Handle, PPRSTATUS, &data );       // read one bit
      buffer[k] = buffer[k] << 1;
      buffer[k] |= (data & 0x20) >> 5;
      nanosleep( &ts, NULL );
      ::ioctl( Handle, PPFCONTROL, &sclkon );  // Strobe (D0, Sclk)=1 low
      nanosleep( &ts, NULL );
    }
  }

  nanosleep( &ts, NULL );
  ::ioctl( Handle, PPFCONTROL, &csoff );   // Control-Bit 1(CS\)=0 high

  if ( buffer[0] != Level[0] || 
       buffer[1] != Level[1] )
    return  WriteError;

  return 0;
}
