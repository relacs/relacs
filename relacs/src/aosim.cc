/*
  aosim.cc
  Implementation of AnalogOutput simulating an analog output device

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

#include <relacs/analoginput.h>
#include <relacs/aosim.h>

namespace relacs {


AOSim::AOSim( void ) 
  : AnalogOutput( "Analog Output Simulation", SimAnalogOutputType )
{
  initOptions();
}


AOSim::~AOSim( void )
{
}

void AOSim::initOptions()
{
  AnalogOutput::initOptions();

  addNumber("extref", "dummy description", -1.0);
}


int AOSim::open( const string &device )
{
  Info.clear();
  Settings.clear();
  setDeviceName( "AO Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device );
  // external reference:
  double extr = number( "extref", -1.0, "V" );
  setExternalReference( extr );
  setInfo();
  return 0;
}


int AOSim::open( Device &device )
{
  Info.clear();
  Settings.clear();
  setDeviceName( "AO Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device.deviceIdent() );
  // external reference:
  double extr = number( "extref", -1.0, "V" );
  setExternalReference( extr );
  setInfo();
  return 0;
}


bool AOSim::isOpen( void ) const
{
  return true;
}


void AOSim::close( void )
{
  Info.clear();
  Settings.clear();
}


int AOSim::stop( void )
{
  return 0;
}


int AOSim::reset( void )
{
  Settings.clear();
  return 0;
}


int AOSim::channels( void ) const
{
  return 4;
}


int AOSim::bits( void ) const
{
  return 16;
}


double AOSim::maxRate( void ) const
{
  return 500000.0;
}


int AOSim::testWriteDevice( OutList &sigs )
{
  for ( int k=0; k<sigs.size(); k++ ) {
    sigs[k].setMinVoltage( -10.0 );
    sigs[k].setMaxVoltage( 10.0 );
  }
  return sigs.failed() ? -1 : 0;
}


int AOSim::directWrite( OutList &sigs )
{
  for ( int k=0; k<sigs.size(); k++ ) {
    sigs[k].setMinVoltage( -10.0 );
    sigs[k].setMaxVoltage( 10.0 );
  }
  return 0;  
}


int AOSim::prepareWrite( OutList &sigs )
{
  // ao still running:
  if ( status() == Running ) {
    sigs.addError( DaqError::Busy );
    return -1;
  }

  for( int k=0; k<sigs.size(); k++ ) {
    // minimum and maximum values:
    double min = sigs[k].requestedMin();
    double max = sigs[k].requestedMax();
    if ( min == OutData::AutoRange || max == OutData::AutoRange ) {
      float smin = 0.0;
      float smax = 0.0;
      minMax( smin, smax, sigs[k] );
      if ( min == OutData::AutoRange )
	min = smin;
      if ( max == OutData::AutoRange )
	max = smax;
    }
    // we use only the largest range and there is only one range:
    sigs[k].setMinVoltage( -10.0 );
    sigs[k].setMaxVoltage( 10.0 );
    if ( ! sigs[k].noLevel() )
      sigs[k].multiplyScale( 10.0 );
    // check for signal overflow/underflow:
    if ( sigs[k].noLevel() ) {
      if ( min < sigs[k].minValue() )
	sigs[k].addError( DaqError::Underflow );
      else if ( max > sigs[k].maxValue() )
	sigs[k].addError( DaqError::Overflow );
    }
    else {
      if ( max > 1.0+1.0e-8 )
	sigs[k].addError( DaqError::Overflow );
      else if ( min < -1.0-1.0e-8 )
	sigs[k].addError( DaqError::Underflow );
    }
  }

  if ( ! sigs.success() )
    return -1;

  // success:
  setSettings( sigs, -1 );
  return 0;
}


int AOSim::convertData( OutList &sigs )
{
  return sigs.failed() ? -1 : 0;
}


int AOSim::startWrite( QSemaphore *sp )
{
  return 0;
}


int AOSim::writeData( void )
{
  return 0;
}


AnalogOutput::Status AOSim::statusUnlocked( void ) const
{
  return Idle;
}


int AOSim::error( void ) const
{
  return 0;
}


int AOSim::getAISyncDevice( const vector< AnalogInput* > &ais ) const
{
  for ( unsigned int k=0; k<ais.size(); k++ ) {
    if ( ais[k]->deviceFile().size() > 0 && deviceFile().size() > 0 &&
	 ais[k]->deviceFile()[ais[k]->deviceFile().size()-1] == deviceFile()[deviceFile().size()-1] ) {
      return k;
    }
  }
  return -1;
}


}; /* namespace relacs */

