/*
  base/linearattenuate.cc
  Linear conversion of intensity to attenuation level independent of carrier frequency.

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

#include <cerrno>
#include <cmath>
#include <relacs/relacsplugin.h>
#include <relacs/base/linearattenuate.h>
using namespace std;
using namespace relacs;

namespace base {


LinearAttenuate::LinearAttenuate( void )
  : Attenuate( "LinearAttenuate", "V", "V", "%g" ),
    ConfigClass( "LinearAttenuate", RELACSPlugin::Plugins, ConfigClass::Save )
{
  // parameter:
  Gain = 1.0;
  Offset = 0.0;

  // add some parameter as options:
  addNumber( "gain", "Gain", Gain );
  addNumber( "offset", "Offset", Offset );
}


LinearAttenuate::~LinearAttenuate( void )
{
}


int LinearAttenuate::decibel( double intensity, double frequency, double &db )
{
  errno = 0;
  if ( intensity == 0.0 )
    db = MuteAttenuationLevel;
  else
    db = -20.0*::log10( intensity*Gain+Offset );

  if ( errno == EDOM || errno == ERANGE )
    return Attenuate::IntensityUnderflow;

  return 0;
}


void LinearAttenuate::intensity( double &intens, double frequency,
				 double decibel )
{
  if ( decibel != MuteAttenuationLevel )
    intens = ( ::pow( 10.0, -decibel/20.0 ) - Offset )/ Gain;
  else
    intens = 0.0;
}


void LinearAttenuate::setGain( double gain )
{
  Gain = gain;
  setNumber( "gain", Gain );
}


void LinearAttenuate::setOffset( double offset )
{
  Offset = offset;
  setNumber( "offset", Offset );
}


void LinearAttenuate::setGain( double gain, double offset )
{
  setGain( gain );
  setOffset( offset );
}


void LinearAttenuate::setDeviceIdent( const string &ident )
{
  Attenuate::setDeviceIdent( ident );
  setConfigIdent( ident );
}


void LinearAttenuate::notify( void )
{
  Gain = number( "gain" );
  Offset = number( "offset" );
}


addAttenuate( LinearAttenuate );


}; /* namespace base */
