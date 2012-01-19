/*
  misc/amplmode.cc
  Control the mode of an amplifier via DigitalIO

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
  BuzzerPin = 1;
  ResistancePin = 2;
  BridgePin = 3;
  CurrentClampPin = 4;
  VoltageClampPin = 5;

  Buzzer = 0;
  Resistance = 0;
  Bridge = 0;
  CurrentClamp = 0;
  VoltageClamp = 0;

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
  BuzzerPin = 1;
  ResistancePin = 2;
  BridgePin = 3;
  CurrentClampPin = 4;
  VoltageClampPin = 5;

  Buzzer = 0;
  Resistance = 0;
  Bridge = 0;
  CurrentClamp = 0;
  VoltageClamp = 0;

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
    BridgePin = opts.integer( "bridgepin", 0, BridgePin );
    ResistancePin = opts.integer( "resistancepin", 0, ResistancePin );
    VoltageClampPin = opts.integer( "vclamppin", 0, VoltageClampPin );
    CurrentClampPin = opts.integer( "cclamppin", 0, CurrentClampPin );
    BuzzerPin = opts.integer( "buzzerpin", 0, BuzzerPin );

    Bridge = 1 << BridgePin;
    Resistance = 1 << ResistancePin;
    VoltageClamp = 1 << VoltageClampPin;
    CurrentClamp = 1 << CurrentClampPin;
    Buzzer = 1 << BuzzerPin;

    ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
    Mask = ModeMask + Buzzer;
    DIOId = DIO->allocateLines( Mask );
    if ( DIOId < 0 ) {
      cerr << "! warning: AmplMode::open( dio ) -> cannot allocate pins on lines " << -DIOId << ".\n";
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
    DIO = 0;
    if ( DIO != 0 && ! DIO->isOpen() )
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
    DIO->writeLines( Mask, 0x00 );

    MixerHandle = ::open( "/dev/mixer", O_RDWR );
    if ( MixerHandle < 0 ) {
      cerr << "Failed to open mixer device.\n";
    }
    else {
      int mask = 0;
      int r = ::ioctl( MixerHandle, SOUND_MIXER_READ_DEVMASK, &mask );
      if ( r == -1 ||
	   MixerChannel < 0 || MixerChannel >= SOUND_MIXER_NRDEVICES ||
	   (mask & (1 << MixerChannel)) == 0 ) {
	::close( MixerHandle );
	MixerHandle = -1;
      }
    }
    BuzzPulse = (int)::rint( opts.number( "buzzerpulse", (double)BuzzPulse, "ms" ) );
    Volume = 0;
    setDeviceVendor( "npi electronic GmbH (Tamm, Germany)" );
    setDeviceName( "SEC-05LX" );
    addInfo();
    Info.addInteger( "bridgepin", BridgePin );
    Info.addInteger( "resistancepin", ResistancePin );
    Info.addInteger( "vclamppin", VoltageClampPin );
    Info.addInteger( "cclamppin", CurrentClampPin );
    Info.addInteger( "buzzerpin", BuzzerPin );
    Info.addNumber( "buzzerpulse", (double)BuzzPulse, "ms" );
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
    DIO->writeLines( Mask, 0x00 );

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

  return DIO->writeLines( ModeMask, Bridge );
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
  return DIO->writeLines( ModeMask, Resistance );
}


int AmplMode::voltageClamp( void )
{
  if ( ! isOpen() )
    return NotOpen;

  return DIO->writeLines( ModeMask, VoltageClamp );
}


int AmplMode::currentClamp( void )
{
  if ( ! isOpen() )
    return NotOpen;

  return DIO->writeLines( ModeMask, CurrentClamp );
}


int AmplMode::manual( void )
{
  if ( ! isOpen() )
    return NotOpen;

  int r = DIO->writeLines( ModeMask, 0x00 );

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
  DIO->write( BuzzerPin, true );
  usleep( BuzzPulse * 1000 );
  DIO->write( BuzzerPin, false );
  
  if ( MixerHandle != -1 && Volume != 0 ) {
    // reset volume:
    ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &Volume );
    Volume = 0;
  }
  
  return 0;
}


}; /* namespace misc */
