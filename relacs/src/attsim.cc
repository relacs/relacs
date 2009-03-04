/*
  attsim.cc
  Implementation of the Attenuator class

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
#include <sstream>
#include <relacs/attsim.h>

using namespace std;

namespace relacs {


const double AttSim::AttStep = 0.5;
const double AttSim::AttMax = 100.0;
const double AttSim::AttMin = -25.0;


double AttSim::Decibel[AttSim::MaxDevices] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


AttSim::AttSim( void ) 
  : Attenuator( "Attenuator Simulation" )
{
}


AttSim::~AttSim( void )
{
}


int AttSim::open( const string &device, long mode )
{
  setDeviceName( "Attenuator Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device );
  return 0;
}


bool AttSim::isOpen( void ) const
{
  return true;
}


void AttSim::close( void )
{
}


string AttSim::settings( void ) const
{
  ostringstream ss;
  ss << "level1: " << Decibel[0]
     << ";level2: " << Decibel[1];
  return ss.str();
}


int AttSim::lines( void ) const
{
  return MaxDevices;
}


int AttSim::attenuate( int di, double &decibel )
{
  if ( di < 0 || di >= MaxDevices )
    return InvalidDevice;

  if ( decibel == MuteAttenuationLevel ) {
    Decibel[di] = decibel;
    return 0;
  }

  if ( decibel < AttMin )
    return Underflow;
  if ( decibel > AttMax )
    return Overflow;

  decibel = floor( (decibel+0.5*AttStep)/AttStep )*AttStep;
  Decibel[di] = decibel;
  return 0;
}


int AttSim::testAttenuate( int di, double &decibel )
{
  if ( di < 0 || di >= MaxDevices )
    return InvalidDevice;

  if ( decibel == MuteAttenuationLevel ) {
    return 0;
  }

  if ( decibel < AttMin )
    return Underflow;
  if ( decibel > AttMax )
    return Overflow;

  decibel = floor( (decibel+0.5*AttStep)/AttStep )*AttStep;
  return 0;
}


}; /* namespace relacs */

