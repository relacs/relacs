/*
  base/decibelattenuate.cc
  Direct conversion of intensity to attenuation level independent of carrier frequency.

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

#include <relacs/relacsplugin.h>
#include <relacs/base/decibelattenuate.h>
using namespace std;
using namespace relacs;

namespace base {


DecibelAttenuate::DecibelAttenuate( void )
  : Attenuate( "DecibelAttenuate", "V", "dB", "%g" ),
    ConfigClass( "DecibelAttenuate", RELACSPlugin::Plugins, ConfigClass::Save )
{
  // parameter:
  Gain = 1.0;
  Offset = 0.0;

  // add some parameter as options:
  addNumber( "gain", "Gain", Gain );
  addNumber( "offset", "Offset", Offset );
}


DecibelAttenuate::~DecibelAttenuate( void )
{
}


int DecibelAttenuate::decibel( double intensity, double frequency, double &db ) const
{
  if ( intensity == 0.0 )
    db = MuteAttenuationLevel;
  else
    db = intensity*Gain+Offset;

  return 0;
}


void DecibelAttenuate::intensity( double &intens, double frequency,
				  double decibel ) const
{
  if ( decibel != MuteAttenuationLevel )
    intens = ( decibel - Offset )/ Gain;
  else
    intens = 0.0;
}


void DecibelAttenuate::setGain( double gain )
{
  Gain = gain;
  setNumber( "gain", Gain );
}


void DecibelAttenuate::setOffset( double offset )
{
  Offset = offset;
  setNumber( "offset", Offset );
}


void DecibelAttenuate::setGain( double gain, double offset )
{
  setGain( gain );
  setOffset( offset );
}


void DecibelAttenuate::setDeviceIdent( const string &ident )
{
  Device::setDeviceIdent( ident );
  setConfigIdent( ident );
}


void DecibelAttenuate::notify( void )
{
  Gain = number( "gain" );
  Offset = number( "offset" );
}


addAttenuate( DecibelAttenuate );


}; /* namespace base */
