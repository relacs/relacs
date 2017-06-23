/*
  attsim.cc
  Implementation of the Attenuator class

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

#include <cmath>
#include <relacs/attsim.h>

using namespace std;

namespace relacs {


const double AttSim::AttStep = 0.5;
const double AttSim::AttMax = 100.0;
const double AttSim::AttMin = -25.0;


double AttSim::Decibel[AttSim::MaxDevices] = { 0, 0 };


AttSim::AttSim( void ) 
  : Attenuator( "Attenuator Simulation" )
{
  initOptions();
}


AttSim::~AttSim( void )
{
}


int AttSim::open( const string &device )
{
  Info.clear();
  setDeviceName( "Attenuator Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device );
  setInfo();
  Info.addNumber( "resolution", 0.5, "dB" );
  return 0;
}


bool AttSim::isOpen( void ) const
{
  return true;
}


void AttSim::close( void )
{
  Info.clear();
}


const Options &AttSim::settings( void ) const
{
  Settings.clear();
  if ( Decibel[0] == MuteAttenuationLevel )
    Settings.addText( "level1", "muted" );
  else
    Settings.addNumber( "level1", Decibel[0], "dB" );
  if ( Decibel[1] == MuteAttenuationLevel )
    Settings.addText( "level2", "muted" );
  else
    Settings.addNumber( "level2", Decibel[1], "dB" );
  return Settings;
}


int AttSim::lines( void ) const
{
  return MaxDevices;
}


double AttSim::minLevel( void ) const
{
  return AttMin;
}


double AttSim::maxLevel( void ) const
{
  return AttMax;
}


void AttSim::levels( vector<double> &l ) const
{
  l.clear();
  l.reserve( (int)::ceil((AttMax-AttMin)/AttStep) );
  for ( int k=0; AttMin + k*AttStep <= AttMax; k++ )
    l.push_back( AttMin + k*AttStep );
}


int AttSim::attenuate( int di, double &decibel )
{
  if ( di < 0 || di >= MaxDevices )
    return InvalidDevice;

  if ( decibel == MuteAttenuationLevel ) {
    Decibel[di] = decibel;
    return 0;
  }

  if ( decibel < AttMin ) {
    decibel = AttMin;
    return Overflow;
  }
  if ( decibel > AttMax ) {
    decibel = AttMax;
    return Underflow;
  }

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

  if ( decibel < AttMin ) {
    decibel = AttMin;
    return Overflow;
  }
  if ( decibel > AttMax ) {
    decibel = AttMax;
    return Underflow;
  }

  decibel = floor( (decibel+0.5*AttStep)/AttStep )*AttStep;
  return 0;
}


}; /* namespace relacs */

