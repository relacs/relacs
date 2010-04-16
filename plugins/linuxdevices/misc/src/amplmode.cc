/*
  misc/amplmode.cc
  Control the mode of an amplifier via NIDIO

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

#include <cmath>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <relacs/misc/amplmode.h>
using namespace relacs;

namespace misc {


AmplMode::AmplMode( DigitalIO &dio, const Options &opts )
  : Device( "AmplMode" ),
    DIO( 0 )
{
  Buzzer = 0x01;
  Resistance = 0x02;
  Bridge = 0x04;
  CurrentClamp = 0x08;
  VoltageClamp = 0x10;

  ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
  Mask = ModeMask + Buzzer;

  BuzzPulse = 10;

  MixerHandle = -1;
  MixerChannel = SOUND_MIXER_VOLUME;
  Volume = 0;

  open( dio, opts );
}


AmplMode::AmplMode( void )
  : Device( "AmplMode" ),
    DIO( 0 )
{
  Buzzer = 0x01;
  Resistance = 0x02;
  Bridge = 0x04;
  CurrentClamp = 0x08;
  VoltageClamp = 0x10;

  ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
  Mask = ModeMask + Buzzer;

  BuzzPulse = 10;

  MixerHandle = -1;
  MixerChannel = SOUND_MIXER_VOLUME;
  Volume = 0;
}


AmplMode::~AmplMode( void )
{
  close();
}


int AmplMode::open( DigitalIO &dio, const Options &opts )
{
  Info.clear();
  Settings.clear();

  DIO = &dio;

  if ( isOpen() ) {
    Bridge = 1 << opts.integer( "bridgepin", 0, 3 );
    Resistance = 1 << opts.integer( "resistancepin", 0, 2 );
    VoltageClamp = 1 << opts.integer( "vclamppin", 0, 5 );
    CurrentClamp = 1 << opts.integer( "cclamppin", 0, 4 );
    Buzzer = 1 << opts.integer( "buzzerpin", 0, 1 );
    ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
    Mask = ModeMask + Buzzer;
    DIOId = DIO->allocateLines( Mask );
    if ( DIOId < 0 ) {
      cerr << "! warning: AmplMode::open( dio ) -> cannot allocate pins.\n";
      DIO = 0;
      return InvalidDevice;
    }
    else {
      open( opts );
      setDeviceFile( DIO->deviceIdent() );
      return 0;
    }
  }
  else {
    if ( DIO != NULL && ! DIO->isOpen() )
      return NotOpen;
    else
      return InvalidDevice;
  }
}


int AmplMode::open( Device &device, const Options &opts )
{
  return open( dynamic_cast<DigitalIO&>( device ), opts );
}


void AmplMode::open( const Options &opts )
{
  if ( isOpen() ) {

    Info.clear();
    Settings.clear();

    // configure for parallel output: 
    DIO->configureLines( Mask, Mask );
    // manual mode selection, no buzz:
    DIO->write( Mask, 0x00 );

    MixerHandle = ::open( "/dev/mixer", O_RDWR );
    if ( MixerHandle != -1 ) {
      int mask = 0;
      int r = ::ioctl( MixerHandle, SOUND_MIXER_READ_DEVMASK, &mask );
      if ( r == -1 ||
	   MixerChannel < 0 || MixerChannel >= SOUND_MIXER_NRDEVICES ||
	   (mask & (1 << MixerChannel)) == 0 ) {
	::close( MixerHandle );
	MixerHandle = -1;
      }
    }
    BuzzPulse = (int)::rint( opts.number( "buzzpulse", (double)BuzzPulse, "ms" ) );
    Volume = 0;
    setDeviceVendor( "npi electronic GmbH (Tamm, Germany)" );
    setDeviceName( "SEC-05LX" );
    addInfo();
    Info.addInteger( "pinbridge", Bridge );
    Info.addInteger( "pinresistance", Resistance );
    Info.addInteger( "pinvclamp", VoltageClamp );
    Info.addInteger( "pincclamp", CurrentClamp );
    Info.addInteger( "pinbuzzer", Buzzer );
    Info.addNumber( "pinbuzzer", (double)BuzzPulse, "ms" );
  }
}


bool AmplMode::isOpen( void ) const
{
  return ( DIO != 0 && DIO->isOpen() );
}


void AmplMode::close( void )
{
  if ( isOpen() ) {
    // manual mode selection, no buzz:
    DIO->write( Mask, 0x00 );

    DIO->freeLines( DIOId );

    if ( MixerHandle != -1 ) {
      ::close( MixerHandle );
      MixerHandle = -1;
    }

  }

  DIO = 0;

  Info.clear();
  Settings.clear();

}


int AmplMode::bridge( void )
{
  if ( ! isOpen() )
    return NotOpen;

  return DIO->write( ModeMask, Bridge );
}


int AmplMode::resistance( void )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( MixerHandle != -1 ) {
    // get volume settings:
    if ( Volume == 0 )
      ::ioctl( MixerHandle, MIXER_READ(MixerChannel), &Volume );
    // mute sound card:
    int vol = 0;
    ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &vol );
  }
  return DIO->write( ModeMask, Resistance );
}


int AmplMode::voltageClamp( void )
{
  if ( ! isOpen() )
    return NotOpen;

  return DIO->write( ModeMask, VoltageClamp );
}


int AmplMode::currentClamp( void )
{
  if ( ! isOpen() )
    return NotOpen;

  return DIO->write( ModeMask, CurrentClamp );
}


int AmplMode::manual( void )
{
  if ( ! isOpen() )
    return NotOpen;

  int r = DIO->write( ModeMask, 0x00 );

  if ( MixerHandle != -1 && Volume != 0 ) {
    // reset volume:
    ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &Volume );
    Volume = 0;
  }
    
  return r;
}


int AmplMode::buzzer( void )
{
  if ( ! isOpen() )
    return NotOpen;
    
  if ( MixerHandle != -1 ) {
    // get volume settings:
    if ( Volume == 0 )
      ::ioctl( MixerHandle, MIXER_READ(MixerChannel), &Volume );
    // mute sound card:
    int vol = 0;
    ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &vol );
  }
  
  // buzz:
  DIO->write( Buzzer, Buzzer );
  usleep( BuzzPulse * 1000 );
  DIO->write( Buzzer, 0x00 );
  
  if ( MixerHandle != -1 && Volume != 0 ) {
    // reset volume:
    ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &Volume );
    Volume = 0;
  }
  
  return 0;
}


}; /* namespace misc */
