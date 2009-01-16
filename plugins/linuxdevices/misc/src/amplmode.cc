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

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/soundcard.h>
#include <relacs/misc/amplmode.h>
using namespace relacs;

namespace misc {


AmplMode::AmplMode( const string &device, long mode )
  : Device( "AmplMode" ),
    DIO( 0 ),
    Own( false )
{
  FirstPin = 0;

  Buzzer = 0x01;
  Resistance = 0x02;
  Bridge = 0x04;
  CurrentClamp = 0x08;
  VoltageClamp = 0x10;

  ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
  Mask = ModeMask + Buzzer;

  MixerHandle = -1;
  MixerChannel = SOUND_MIXER_VOLUME;
  Volume = 0;

  open( device, mode );
}


AmplMode::AmplMode( NIDIO *nidio, long mode )
  : Device( "AmplMode" ),
    DIO( 0 ),
    Own( false )
{
  FirstPin = 0;

  Buzzer = 0x01;
  Resistance = 0x02;
  Bridge = 0x04;
  CurrentClamp = 0x08;
  VoltageClamp = 0x10;

  ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
  Mask = ModeMask + Buzzer;

  open( *nidio, mode );
}


AmplMode::AmplMode( void )
  : Device( "AmplMode" ),
    DIO( 0 ),
    Own( false )
{
  FirstPin = 0;

  Buzzer = 0x01;
  Resistance = 0x02;
  Bridge = 0x04;
  CurrentClamp = 0x08;
  VoltageClamp = 0x10;

  ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
  Mask = ModeMask + Buzzer;
}


AmplMode::~AmplMode( void )
{
  if ( Own )
    delete DIO;
}


int AmplMode::open( const string &device, long mode )
{
  if ( DIO != 0  && !DIO->isOpen() ) {
    if ( Own )
      delete DIO;
    DIO = 0;
  }

  if ( DIO == 0 ) {
    DIO = new NIDIO( device );
    if ( DIO->isOpen() ) {
      if ( DIO->allocPins( Mask << mode ) > 0 ) {
	cerr << "! warning: AmplMode::open( device ) -> cannot allocate pins.\n";
	DIO->close();
	delete DIO;
	DIO = 0;
	Own = false;
	return InvalidDevice;
      }
      else {
	Own = true;
	open( mode );
	setDeviceFile( device );
	return 0;
      }
    }
    else {
      bool notopen = ( DIO != NULL && ! DIO->isOpen() );
      DIO = 0;
      Own = false;
      cerr << "! warning: AmplMode::open( device ) -> cannot open NIDIO.\n";
      if ( notopen )
	return NotOpen;
      else
	return InvalidDevice;
    }
  }
  return 0;
}


int AmplMode::open( NIDIO &nidio, long mode )
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
    if ( DIO->allocPins( Mask << mode ) > 0 ) {
      cerr << "! warning: AmplMode::open( device ) -> cannot allocate pins.\n";
      DIO = 0;
      Own = false;
      return InvalidDevice;
    }
    else {
      open( mode );
      setDeviceFile( nidio.deviceIdent() );
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


int AmplMode::open( Device &device, long mode )
{
  return open( dynamic_cast<NIDIO&>( device ), mode );
}


void AmplMode::open( long mode )
{
  if ( isOpen() ) {

    FirstPin = mode;
    
    Buzzer = 0x01 << FirstPin;
    Resistance = 0x02 << FirstPin;
    Bridge = 0x04 << FirstPin;
    CurrentClamp = 0x08 << FirstPin;
    VoltageClamp = 0x10 << FirstPin;

    ModeMask = Resistance + Bridge + CurrentClamp + VoltageClamp ;
    Mask = Buzzer + ModeMask;

    // configure for parallel output: 
    DIO->configure( Mask, Mask );
    // manual mode selection, no buzz:
    DIO->out( 0x00, Mask );

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
    Volume = 0;
    setDeviceVendor( "npi electronic GmbH (Tamm, Germany)" );
    setDeviceName( "SEC-05LX" );
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
    DIO->out( 0x00, Mask );

    DIO->freePins( Mask );

    FirstPin = 0;

    Buzzer = 0x01;
    Resistance = 0x02;
    Bridge = 0x04;
    CurrentClamp = 0x08;
    VoltageClamp = 0x10;
    
    ModeMask = Bridge + Resistance + VoltageClamp + CurrentClamp;
    Mask = ModeMask + Buzzer;

    if ( MixerHandle != -1 ) {
      ::close( MixerHandle );
      MixerHandle = -1;
    }

  }

  if ( DIO != 0 && Own ) {
    DIO->close();
    delete DIO;
  }
  DIO = 0;
  Own = false;
}


int AmplMode::bridge( void )
{
  if ( isOpen() ) {
    return DIO->out( Bridge, ModeMask );
  }
  return NotOpen;
}


int AmplMode::resistance( void )
{
  if ( isOpen() ) {
    if ( MixerHandle != -1 ) {
      // get volume settings:
      if ( Volume == 0 )
	::ioctl( MixerHandle, MIXER_READ(MixerChannel), &Volume );
      // mute sound card:
      int vol = 0;
      ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &vol );
    }
    return DIO->out( Resistance, ModeMask );
  }
  return NotOpen;
}


int AmplMode::voltageClamp( void )
{
  if ( isOpen() ) {
    return DIO->out( VoltageClamp, ModeMask );
  }
  return NotOpen;
}


int AmplMode::currentClamp( void )
{
  if ( isOpen() ) {
    return DIO->out( CurrentClamp, ModeMask );
  }
  return NotOpen;
}


int AmplMode::manual( void )
{
  if ( isOpen() ) {
    int r = DIO->out( 0x00, ModeMask );

    if ( MixerHandle != -1 && Volume != 0 ) {
      // reset volume:
      ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &Volume );
      Volume = 0;
    }
    
    return r;
  }
  return NotOpen;
}


int AmplMode::buzzer( void )
{
  if ( isOpen() ) {
    if ( MixerHandle != -1 ) {
      // get volume settings:
      if ( Volume == 0 )
	::ioctl( MixerHandle, MIXER_READ(MixerChannel), &Volume );
      // mute sound card:
      int vol = 0;
      ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &vol );
    }

    // buzz:
    DIO->out( Buzzer, Buzzer );
    usleep( BuzzPulse * 1000 );
    DIO->out( 0x00, Buzzer );

    if ( MixerHandle != -1 && Volume != 0 ) {
      // reset volume:
      ::ioctl( MixerHandle, MIXER_WRITE(MixerChannel), &Volume );
      Volume = 0;
    }

    return 0;
  }
  return NotOpen;
}


}; /* namespace misc */
