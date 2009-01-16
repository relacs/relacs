/*
  attenuate.cc
  Attenuates a single output channel.

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
#include <relacs/attenuate.h>
using namespace std;

namespace relacs {


Attenuate::Attenuate( void )
  : Device( Type ),
    Att( 0 ),
    Index( -1 ),
    AODevice( "" ),
    AOChannel( -1 ),
    IntensityName( "intensity" ),
    IntensityUnit( "dB" ),
    IntensityFormat( "%6.2f" ),
    FrequencyName( "" ),
    FrequencyUnit( "Hz" ),
    FrequencyFormat( "%7.0f" )
{
}


Attenuate::Attenuate( const string &deviceclass,
		      const string &intensityname,
		      const string &intensityunit,
		      const string &intensityformat,
		      const string &frequencyname,
		      const string &frequencyunit,
		      const string &frequencyformat )
  : Device( deviceclass, Type ),
    Att( 0 ),
    Index( -1 ),
    AODevice( "" ),
    AOChannel( -1 ),
    IntensityName( intensityname ),
    IntensityUnit( intensityunit ),
    IntensityFormat( intensityformat ),
    FrequencyName( frequencyname ),
    FrequencyUnit( frequencyunit ),
    FrequencyFormat( frequencyformat )
{
}


Attenuate::~Attenuate( void )
{
  close();
}


int Attenuate::open( Device &att, long index )
{
  clearSettings();
  Att = &dynamic_cast<Attenuator&>( att );
  Index = index;
  if ( Att == NULL )
    return InvalidDevice;
  else if ( ! Att->isOpen() )
    return NotOpen;
  else {
    setDeviceFile( att.deviceIdent() );
    return 0;
  }
}


int Attenuate::open( const string &device, long index )
{
  clearSettings();
  return InvalidDevice;
}


bool Attenuate::isOpen( void ) const
{
  return ( Att != NULL && Att->isOpen() &&
	   Index >= 0 && Index < Att->lines() );
}


void Attenuate::close( void )
{
  if ( Att != 0 && Att->isOpen() )
    Att->close();
  Att = 0;
  Index = -1;
  clearSettings();
}


void Attenuate::clear( void )
{
  Att = 0;
  Index = -1;
  clearSettings();
}


string Attenuate::info( void ) const
{
  ostringstream ss;
  ss << ";analog output device: " << aoDevice();
  ss << ";analog output channel: " << aoChannel();
  ss << ";attenuator device: " << ( Att != 0 ? Att->deviceIdent() : "" );
  ss << ";attenuator line: " << Index;
  ss << ends;
  return Device::info() + ss.str();
}


void Attenuate::init( void )
{
}


void Attenuate::save( const string &path ) const
{
}


int Attenuate::write( double &intens, double frequency )
{
  // get attenuation level:
  double db = 0.0;
  int r = decibel( intens, frequency, db );
  if ( r < 0 )
    return r;

  // set attenuation level:
  if ( Att == 0 || !Att->isOpen() )
    r = Attenuator::NotOpen;
  else
    r = Att->attenuate( Index, db );

  // calculate intensity:
  intensity( intens, frequency, db );

  // settings:
  ostringstream ss;
  ss << "intensity: " << intens
     << ";frequency: " << frequency << ends;
  setSettings( ss.str() );

  return r;
}


int Attenuate::testWrite( double &intens, double frequency )
{
  // get attenuation level:
  double db = 0.0;
  int r = decibel( intens, frequency, db );
  if ( r < 0 )
    return r;

  // test attenuation level:
  if ( Att == 0 || !Att->isOpen() )
    r = Attenuator::NotOpen;
  else
    r = Att->testAttenuate( Index, db );

  // calculate intensity:
  intensity( intens, frequency, db );
  return r;
}


int Attenuate::mute( void )
{
  if ( Att == 0 || !Att->isOpen() )
    return Attenuator::NotOpen;

  // settings:
  ostringstream ss;
  ss << "intensity: muted" << ends;
  setSettings( ss.str() );

  return Att->mute( Index );
}


int Attenuate::testMute( void )
{
  if ( Att == 0 || !Att->isOpen() )
    return Attenuator::NotOpen;

  return Att->testMute( Index );
}


string Attenuate::intensityName( void ) const
{
  return IntensityName;
}


void Attenuate::setIntensityName( const string &name )
{
  IntensityName = name;
}


string Attenuate::intensityUnit( void ) const
{
  return IntensityUnit;
}


void Attenuate::setIntensityUnit( const string &unit )
{
  IntensityUnit = unit;
}


string Attenuate::intensityFormat( void ) const
{
  return IntensityFormat;
}


void Attenuate::setIntensityFormat( const string &format )
{
  IntensityFormat = format;
}


string Attenuate::frequencyName( void ) const
{
  return FrequencyName;
}


void Attenuate::setFrequencyName( const string &name )
{
  FrequencyName = name;
}


string Attenuate::frequencyUnit( void ) const
{
  return FrequencyUnit;
}


void Attenuate::setFrequencyUnit( const string &unit )
{
  FrequencyUnit = unit;
}


string Attenuate::frequencyFormat( void ) const
{
  return FrequencyFormat;
}


void Attenuate::setFrequencyFormat( const string &format )
{
  FrequencyFormat = format;
}


int Attenuate::aoChannel( void ) const
{
  return AOChannel;
}


void Attenuate::setAOChannel( int channel )
{
  AOChannel = channel;
}


string Attenuate::aoDevice( void ) const
{
  return AODevice;
}


void Attenuate::setAODevice( const string &deviceid )
{
  AODevice = deviceid;
}


}; /* namespace relacs */

