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
  BridgePin = 0;
  CurrentClampPin = 1;
  VoltageClampPin = 2;
  ResistancePin = 3;
  BuzzerPin = 4;

  BridgeMask = 0;
  CurrentClampMask = 0;
  VoltageClampMask = 0;
  ResistanceMask = 0;
  BuzzerMask = 0;

  ModeMask = 0;
  Mask = 0;

  CurrentMode = 0;

  MixerHandle = -1;
  MixerChannel = SOUND_MIXER_VOLUME;
  Volume = 0;

  open( dio, opts );
}


AmplMode::AmplMode( void )
  : Device( "AmplMode" ),
    DIO( 0 )
{
  BridgePin = 0;
  CurrentClampPin = 1;
  VoltageClampPin = 2;
  ResistancePin = 3;
  BuzzerPin = 4;

  BridgeMask = 0;
  CurrentClampMask = 0;
  VoltageClampMask = 0;
  ResistanceMask = 0;
  BuzzerMask = 0;

  ModeMask = 0;
  Mask = 0;

  CurrentMode = 0;

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
    CurrentClampPin = opts.integer( "cclamppin", 0, CurrentClampPin );
    VoltageClampPin = opts.integer( "vclamppin", 0, VoltageClampPin );
    ResistancePin = opts.integer( "resistancepin", 0, ResistancePin );
    BuzzerPin = opts.integer( "buzzerpin", 0, BuzzerPin );

    BridgeMask = 1 << BridgePin;
    CurrentClampMask = 1 << CurrentClampPin;
    VoltageClampMask = 1 << VoltageClampPin;
    ResistanceMask = 1 << ResistancePin;
    BuzzerMask = 1 << BuzzerPin;

    ModeMask =  BridgeMask + CurrentClampMask + VoltageClampMask + ResistanceMask;
    Mask = ModeMask + BuzzerMask;
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
    CurrentMode = 0x00;

    // full duplex mode:
    int fd = ::open( "/dev/dsp", O_RDWR, 0 );
    if ( fd >= 0 ) {
      if ( ioctl( fd, SNDCTL_DSP_SETDUPLEX, NULL ) >= 0 )
	cerr << "AmplMode: Enabled full duplex sound\n";
    }

    // open mixer:
    MixerHandle = ::open( "/dev/mixer", O_RDWR );
    if ( MixerHandle < 0 ) {
      cerr << "AmplMode: Failed to open mixer device.\n";
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
    Volume = 0;
    setDeviceVendor( "npi electronic GmbH (Tamm, Germany)" );
    setDeviceName( "SEC-05LX" );
    addInfo();
    Info.addInteger( "bridgepin", BridgePin );
    Info.addInteger( "cclamppin", CurrentClampPin );
    Info.addInteger( "vclamppin", VoltageClampPin );
    Info.addInteger( "resistancepin", ResistancePin );
    Info.addInteger( "buzzerpin", BuzzerPin );
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


int AmplMode::setBridgeMode( void )
{
  if ( ! isOpen() )
    return NotOpen;

  CurrentMode = BridgeMask;
  return DIO->writeLines( ModeMask, BridgeMask );
}


int AmplMode::setCurrentClampMode( void )
{
  if ( ! isOpen() )
    return NotOpen;

  CurrentMode = CurrentClampMask;
  return DIO->writeLines( ModeMask, CurrentClampMask );
}


int AmplMode::setVoltageClampMode( void )
{
  if ( ! isOpen() )
    return NotOpen;

  CurrentMode = VoltageClampMask;
  return DIO->writeLines( ModeMask, VoltageClampMask );
}


int AmplMode::setManualSelection( void )
{
  if ( ! isOpen() )
    return NotOpen;

  CurrentMode = 0x00;
  int r = DIO->writeLines( ModeMask, 0x00 );    
  return r;
}


int AmplMode::startResistance( void )
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
  return DIO->writeLines( ModeMask, ResistanceMask );
}


int AmplMode::stopResistance( void )
{
  if ( ! isOpen() )
    return NotOpen;

  int r = DIO->writeLines( ModeMask, CurrentMode );
  
  if ( MixerHandle != -1 && Volume != 0 ) {
    // reset volume:
    ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &Volume );
    Volume = 0;
  }
  
  return r;
}


int AmplMode::startBuzz( void )
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
  return DIO->write( BuzzerPin, true );
}


int AmplMode::stopBuzz( void )
{
  if ( ! isOpen() )
    return NotOpen;

  int r = DIO->write( BuzzerPin, false );
  
  if ( MixerHandle != -1 && Volume != 0 ) {
    // reset volume:
    ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &Volume );
    Volume = 0;
  }
  
  return r;
}


}; /* namespace misc */
