/*
  outdatainfo.cc
  Stores some properties of an OutData.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/outdata.h>
#include <relacs/outdatainfo.h>
using namespace std;

namespace relacs {


OutDataInfo::OutDataInfo( void )
  : Device( 0 ),
    Channel( 0 ),
    Trace( 0 ),
    TraceName( "" ),
    Delay( 0.0 ),
    SampleRate( 0.0 ),
    Length( 0.0 ),
    Intensity( 0.0 ),
    Level( 0.0 ),
    CarrierFreq( 0.0 ),
    Ident( "" )
{
  Descriptions.clear();
  Types.clear();
  TypeNames.clear();
}



OutDataInfo::OutDataInfo( const OutDataInfo &signal )
  : Device( signal.Device ),
    Channel( signal.Channel ),
    Trace( signal.Trace ),
    TraceName( signal.TraceName ),
    Delay( signal.Delay ),
    SampleRate( signal.SampleRate ),
    Length( signal.Length ),
    Intensity( signal.Intensity ),
    Level( signal.Level ),
    CarrierFreq( signal.CarrierFreq ),
    Ident( signal.Ident ),
    Descriptions( signal.Descriptions ),
    Types( signal.Types ),
    TypeNames( signal.TypeNames )
{
}


OutDataInfo::OutDataInfo( const OutData &signal )
{
  Device = signal.device();
  Channel = signal.channel();
  Trace = signal.trace();
  TraceName = signal.traceName();
  Delay = signal.delay();
  SampleRate = signal.sampleRate();
  Length = signal.length();
  Intensity = signal.intensity();
  Level = signal.level();
  CarrierFreq = signal.carrierFreq();
  Ident = signal.ident();
  Types.clear();
  TypeNames.clear();
  for ( int k=0; k<signal.descriptions(); k++ ) {
    Descriptions.push_back( signal.description( k ) );
    Str ts = Descriptions[k].text( "type", "stimulus/unknown" );
    Types.push_back( ts );
    Descriptions[k].erase( "type" );
    ts.preventFirst( "stimulus/" ).strip();
    TypeNames.push_back( ts );
    // XXX once OutData does not have idents any more, the following lines can be erased:
    if ( ! signal.ident().empty() )
      Descriptions[k].addText( "Description", signal.ident() );
  }
}


int OutDataInfo::device( void ) const
{
  return Device;
}


void OutDataInfo::setDevice( int device )
{
  Device = device;
}


int OutDataInfo::channel( void ) const
{
  return Channel;
}


void OutDataInfo::setChannel( int channel )
{
  Channel = channel;
}


void OutDataInfo::setChannel( int channel, int device )
{
  Channel = channel;
  Device = device;
}


int OutDataInfo::trace( void ) const
{
  return Trace;
}


void OutDataInfo::setTrace( int index )
{
  Trace = index;
}


string OutDataInfo::traceName( void ) const
{
  return TraceName;
}


void OutDataInfo::setTraceName( const string &name )
{
  TraceName = name;
}


double OutDataInfo::delay( void ) const 
{
  return Delay;
}


void OutDataInfo::setDelay( double delay )
{
  Delay = delay >= 0.0 ? delay : 0.0;
}


double OutDataInfo::sampleRate( void ) const
{
  return SampleRate;
}


void OutDataInfo::setSampleRate( double rate )
{
  SampleRate = rate;
}


double OutDataInfo::length( void ) const
{
  return Length;
}


void OutDataInfo::setLength( double length )
{
  Length = length;
}


double OutDataInfo::intensity( void ) const
{
  return Intensity;
}


void OutDataInfo::setIntensity( double intensity )
{
  Intensity = intensity;
}


void OutDataInfo::setNoIntensity( void )
{
  Intensity = OutData::NoIntensity;
}


bool OutDataInfo::noIntensity( void ) const
{
  return ( Intensity == OutData::NoIntensity );
}


void OutDataInfo::mute( void )
{
  Intensity = OutData::MuteIntensity;
}


double OutDataInfo::carrierFreq( void ) const
{
  return CarrierFreq;
}


void OutDataInfo::setCarrierFreq( double carrierfreq )
{
  CarrierFreq = carrierfreq;
}


double OutDataInfo::level( void ) const
{
  return Level;
}


void OutDataInfo::setLevel( double level )
{
  Level = level;
}


void OutDataInfo::setNoLevel( void )
{
  Level = OutData::NoLevel;
}


bool OutDataInfo::noLevel( void ) const
{
  return ( Level == OutData::NoLevel );
}


string OutDataInfo::ident( void ) const
{
  return Ident;
}


void OutDataInfo::setIdent( const string &ident )
{
  Ident = ident;
}


int OutDataInfo::descriptions( void ) const
{
  return Descriptions.size();
}


const Options &OutDataInfo::description( int i ) const
{
  if ( i < 0 || i >= (int)Descriptions.size() ) {
    Dummy.clear();
    return Dummy;
  }
  else
    return Descriptions[ i ];
}


Options &OutDataInfo::description( int i )
{
  if ( i < 0 || i >= (int)Descriptions.size() ) {
    Dummy.clear();
    return Dummy;
  }
  else
    return Descriptions[ i ];
}


const Options &OutDataInfo::description( void ) const
{
  if ( Descriptions.empty() ) {
    Dummy.clear();
    return Dummy;
  }
  else
    return Descriptions.back();
}


Options &OutDataInfo::description( void )
{
  if ( Descriptions.empty() ) {
    Dummy.clear();
    return Dummy;
  }
  else
    return Descriptions.back();
}


const deque< Options > &OutDataInfo::allDescriptions( void ) const
{
  return Descriptions;
}


deque< Options > &OutDataInfo::allDescriptions( void )
{
  return Descriptions;
}


Options &OutDataInfo::addDescription( const string &type )
{
  Descriptions.push_back( Options() );
  Descriptions.back().addText( "type", type );
  return Descriptions.back();
}


void OutDataInfo::clearDescriptions( void )
{
  Descriptions.clear();
}


string OutDataInfo::type( int i ) const
{
  if ( i < 0 || i >= (int)Types.size() )
    return "";
  else
    return Types[ i ];
}


string OutDataInfo::typeName( int i ) const
{
  if ( i < 0 || i >= (int)TypeNames.size() )
    return "";
  else
    return TypeNames[ i ];
}


}; /* namespace relacs */
