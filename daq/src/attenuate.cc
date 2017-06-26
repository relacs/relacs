/*
  attenuate.cc
  Attenuates a single output channel.

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

#include <relacs/attenuate.h>
using namespace std;

namespace relacs {


const double Attenuate::MuteAttenuationLevel = Attenuator::MuteAttenuationLevel;
const double Attenuate::MuteIntensity = -1.0e37;


Attenuate::Attenuate( void )
  : Device( AttenuateType ),
    Att( 0 ),
    Index( -1 ),
    NoAttenuator( false ),
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
  : Device( deviceclass, AttenuateType ),
    Att( 0 ),
    Index( -1 ),
    NoAttenuator( false ),
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


void Attenuate::initOptions()
{
  Device::initOptions();

  addInteger( "line", "Attenuator line", 0,0, 1000 );
  addText( "aodevice", "Analog output device", "ao-1" );
  addInteger( "aochannel", "Channel on analog output device", 0, 0, 1000 );
  addText( "intensityname", "Name of intensity variable", "intensity" );
  addText( "intensityunit", "Unit for intensity", "dB" );
  addText( "intensityformat", "Output format for intensity", "%6.2f" );
  addText( "frequencyname", "Name of frequency variable", "" );
  addText( "frequencyunit", "Unit for frequency", "Hz" );
  addText( "frequencyformat", "Output format for frequency", "%7.0f" );
}


int Attenuate::open( Device &att, int line )
{
  Info.clear();
  Settings.clear();
  NoAttenuator = false;
  Att = &dynamic_cast<Attenuator&>( att );
  Index = line;
  if ( Att == NULL )
    return InvalidDevice;
  else if ( ! Att->isOpen() )
    return NotOpen;
  else {
    setDeviceFile( att.deviceIdent() );
    return 0;
  }
}


int Attenuate::open( Device &att )
{
  int line = integer( "line", 0, 0 );
  int r = open( att, line );
  setAODevice( text( "aodevice", "ao-1" ) );
  setAOChannel( integer( "aochannel", 0, 0 ) );
  if ( exist( "intensityname" ) )
    setIntensityName( text( "intensityname" ) );
  if ( exist( "intensityunit" ) )
    setIntensityUnit( text( "intensityunit" ) );
  if ( exist( "intensityformat" ) )
    setIntensityFormat( text( "intensityformat" ) );
  if ( exist( "frequencyname" ) )
    setFrequencyName( text( "frequencyname" ) );
  if ( exist( "frequencyunit" ) )
    setFrequencyUnit( text( "frequencyunit" ) );
  if ( exist( "frequencyformat" ) )
    setFrequencyFormat( text( "frequencyformat" ) );
  return r;
}


int Attenuate::open( const string &device )
{
  Info.clear();
  Settings.clear();
  if ( device != "none" ) {
    NoAttenuator = false;
    return InvalidDevice;
  }

  int line = integer( "line", 0, 0 );
  Att = 0;
  NoAttenuator = true;
  Index = line;
  setDeviceFile( device );
  setAODevice( text( "aodevice", "ao-1" ) );
  setAOChannel( integer( "aochannel", 0, 0 ) );
  if ( exist( "intensityname" ) )
    setIntensityName( text( "intensityname" ) );
  if ( exist( "intensityunit" ) )
    setIntensityUnit( text( "intensityunit" ) );
  if ( exist( "intensityformat" ) )
    setIntensityFormat( text( "intensityformat" ) );
  if ( exist( "frequencyname" ) )
    setFrequencyName( text( "frequencyname" ) );
  if ( exist( "frequencyunit" ) )
    setFrequencyUnit( text( "frequencyunit" ) );
  if ( exist( "frequencyformat" ) )
    setFrequencyFormat( text( "frequencyformat" ) );
  return 0;
}


bool Attenuate::isOpen( void ) const
{
  return ( NoAttenuator ||
	   ( Att != NULL && Att->isOpen() &&
	     Index >= 0 && Index < Att->lines() ) );
}


void Attenuate::close( void )
{
  if ( Att != 0 && Att->isOpen() )
    Att->close();
  Att = 0;
  Index = -1;
  Info.clear();
  Settings.clear();
}


void Attenuate::clear( void )
{
  Att = 0;
  Index = -1;
  Info.clear();
  Settings.clear();
}


double Attenuate::minIntensity( double frequency ) const
{
  double minlevel = -100.0;
  double maxlevel = 200.0;
  if ( ! NoAttenuator ) {
    minlevel = Att->minLevel();
    maxlevel = Att->maxLevel();
  }

  double intens1 = 0.0;
  intensity( intens1, frequency, minlevel );
  double intens2 = 0.0;
  intensity( intens2, frequency, maxlevel );
  return intens1 < intens2 ? intens1 : intens2;
}


double Attenuate::maxIntensity( double frequency ) const
{
  double minlevel = -100.0;
  double maxlevel = 200.0;
  if ( ! NoAttenuator ) {
    minlevel = Att->minLevel();
    maxlevel = Att->maxLevel();
  }

  double intens1 = 0.0;
  intensity( intens1, frequency, minlevel );
  double intens2 = 0.0;
  intensity( intens2, frequency, maxlevel );
  return intens1 > intens2 ? intens1 : intens2;
}


void Attenuate::intensities( vector<double> &ints, double frequency ) const
{
  ints.clear();
  vector<double> l;
  l.reserve( 1000 );
  if ( NoAttenuator ) {
    for ( int k=0; k<600; k++ )
      l.push_back( -100.0 + k*0.5 );
  }
  else {
    Att->levels( l );
    if ( l.empty() )
      return;
  }

  ints.reserve( l.size() );
  double intens1 = 0.0;
  intensity( intens1, frequency, l.front() );
  double intens2 = 0.0;
  intensity( intens2, frequency, l.back() );
  if ( intens1 < intens2 ) {
    for ( unsigned int k=0; k<l.size(); k++ ) {
      intensity( intens1, frequency, l[k] );
      ints.push_back( intens1 );
    }
  }
  else {
    for ( unsigned int k=l.size()-1; k>=0; k-- ) {
      intensity( intens1, frequency, l[k] );
      ints.push_back( intens1 );
    }
  }
}


void Attenuate::init( void )
{
  Info.clear();
  Device::addInfo();
  Info.addText( "analog output device", aoDevice() );
  Info.addInteger( "analog output channel", aoChannel() );
  Info.addText( "attenuator device", NoAttenuator ? "None" : ( Att != 0 ? Att->deviceIdent() : "" ) );
  Info.addInteger( "attenuator line", Index );
  Info.addNumber( "minimum intensity", minIntensity( 0.0 ), IntensityUnit );
  Info.addNumber( "maximum intensity", maxIntensity( 0.0 ), IntensityUnit );
}


void Attenuate::save( const string &path ) const
{
}


int Attenuate::write( double &intens, double frequency, double &level )
{
  level = 0.0;

  // get attenuation level:
  double db = 0.0;
  int r1 = decibel( intens, frequency, db );

  // set attenuation level:
  int r2 = 0;
  if ( ! NoAttenuator ) {
    if ( Att == 0 || !Att->isOpen() )
      r2 = Attenuator::NotOpen;
    else
      r2 = Att->attenuate( Index, db );
  }

  // calculate intensity:
  intensity( intens, frequency, db );
  level = db;

  // settings:
  Settings.clear();
  Settings.addNumber( IntensityName, intens, IntensityUnit, IntensityFormat );
  if ( ! FrequencyName.empty() )
    Settings.addNumber( FrequencyName, frequency, FrequencyUnit, FrequencyFormat );

  return r2 == 0 ? r1 : r2;
}


int Attenuate::testWrite( double &intens, double frequency, double &level )
{
  level = 0.0;

  // get attenuation level:
  double db = 0.0;
  int r1 = decibel( intens, frequency, db );

  // test attenuation level:
  int r2 = 0;
  if ( NoAttenuator ) {
    if ( db < -100.0 ) {
      r2 = Overflow;
      db = -100.0;
    }
    else if ( db > 200.0 ) {
      r2 = Underflow;
      db = 200.0;
    }
  }
  else {
    if ( Att == 0 || !Att->isOpen() )
      r2 = Attenuator::NotOpen;
    else
      r2 = Att->testAttenuate( Index, db );
  }

  // calculate intensity:
  intensity( intens, frequency, db );
  level = db;

  return r2 == 0 ? r1 : r2;
}


int Attenuate::mute( void )
{
  if ( !NoAttenuator && ( Att == 0 || !Att->isOpen() ) )
    return Attenuator::NotOpen;

  // settings:
  Settings.clear();
  Settings.addText( IntensityName, "muted" );

  if ( NoAttenuator )
    return 0;
  else
    return Att->mute( Index );
}


int Attenuate::testMute( void )
{
  if ( NoAttenuator )
    return 0;

  if ( Att == 0 || !Att->isOpen() )
    return Attenuator::NotOpen;

  return Att->testMute( Index );
}


int Attenuate::attenuate( double &level )
{
  int r = 0;

  // set attenuation level:
  if ( !NoAttenuator ) {
    if ( Att == 0 || !Att->isOpen() )
      r = Attenuator::NotOpen;
    else
      r = Att->attenuate( Index, level );
  }

  // settings:
  Settings.clear();

  return r;
}


int Attenuate::testAttenuate( double &level )
{
  int r = 0;
  if ( NoAttenuator ) {
    if ( level < -100.0 ) {
      r = Overflow;
      level = -100.0;
    }
    else if ( level > 200.0 ) {
      r = Underflow;
      level = 200.0;
    }
  }
  else {
    if ( Att == 0 || !Att->isOpen() )
      r = Attenuator::NotOpen;
    else
      r = Att->testAttenuate( Index, level );
  }
  return r;
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


Attenuator *Attenuate::attenuator( void )
{
  return Att;
}


const Attenuator *Attenuate::attenuator( void ) const
{
  return Att;
}


bool Attenuate::noAttenuator( void ) const
{
  return NoAttenuator;
}


}; /* namespace relacs */

