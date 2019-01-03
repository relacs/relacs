/*
  aisim.cc
  Implementation of AnalogInput simulating an analog input device

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

#include <relacs/aisim.h>

namespace relacs {


AISim::AISim( void )
  : AnalogInput( "Analog Input Simulation", SimAnalogInputType )
{
  MaxRanges = 8;
  AIuniRanges[0] = 10.0;
  AIbiRanges[0] = 10.0;
  AIuniRanges[1] = 5.0;
  AIbiRanges[1] = 5.0;
  AIuniRanges[2] = 2.0;
  AIbiRanges[2] = 2.5;
  AIuniRanges[3] = 1.0;
  AIbiRanges[3] = 1.0;
  AIuniRanges[4] = 0.5;
  AIbiRanges[4] = 0.5;
  AIuniRanges[5] = 0.2;
  AIbiRanges[5] = 0.25;
  AIuniRanges[6] = 0.1;
  AIbiRanges[6] = 0.1;
  AIuniRanges[7] = 0.05;
  AIbiRanges[7] = 0.05;
  AIuniRanges[8] = -1.0;
  AIbiRanges[8] = -1.0;
  IsRunning = false;

  initOptions();
}


AISim::~AISim( void )
{
}

void AISim::initOptions()
{
  AnalogInput::initOptions();

  addNumber("gainblacklist", "dummy description", 0);
}

int AISim::open( const string &device )
{
  Info.clear();
  Settings.clear();
  setDeviceName( "AI Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device );
  setInfo();
  vector<double> gainblacklist;
  numbers( "gainblacklist", gainblacklist );
  for ( int j=0; j<MaxRanges; j++ ) {
    for ( unsigned int k=0; k<gainblacklist.size(); k++ ) {
      if ( ::fabs( AIbiRanges[j] - gainblacklist[k] ) < 1e-6 ) {
	for ( int i=j+1; i<MaxRanges; i++ )
	  AIbiRanges[i-1] =AIbiRanges[i];
      }
      if ( ::fabs( AIuniRanges[j] - gainblacklist[k] ) < 1e-6 ) {
	for ( int i=j+1; i<MaxRanges; i++ )
	  AIuniRanges[i-1] =AIuniRanges[i];
      }
    }
  }
  IsRunning = false;
  return 0;
}


int AISim::open( Device &device )
{
  Info.clear();
  Settings.clear();
  setDeviceName( "AI Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device.deviceIdent() );
  setInfo();
  vector<double> gainblacklist;
  numbers( "gainblacklist", gainblacklist );
  for ( int j=0; j<MaxRanges; j++ ) {
    for ( unsigned int k=0; k<gainblacklist.size(); k++ ) {
      if ( ::fabs( AIbiRanges[j] - gainblacklist[k] ) < 1e-6 ) {
	for ( int i=j+1; i<MaxRanges; i++ )
	  AIbiRanges[i-1] =AIbiRanges[i];
      }
      if ( ::fabs( AIuniRanges[j] - gainblacklist[k] ) < 1e-6 ) {
	for ( int i=j+1; i<MaxRanges; i++ )
	  AIuniRanges[i-1] =AIuniRanges[i];
      }
    }
  }
  IsRunning = false;
  return 0;
}


bool AISim::isOpen( void ) const
{
  return true;
}


void AISim::close( void )
{
  Info.clear();
  Settings.clear();
  IsRunning = false;
}


int AISim::channels( void ) const
{
  return 32;
}


int AISim::bits( void ) const
{
  return 12;
}


double AISim::maxRate( void ) const
{
  return 500000.0;
}


int AISim::maxRanges( void ) const
{
  return MaxRanges;
}


double AISim::unipolarRange( int index ) const
{
  return AIuniRanges[index];
}


double AISim::bipolarRange( int index ) const
{
  return AIbiRanges[index];
}


int AISim::testReadDevice( InList &traces )
{
  // sample rate:
  double maxrate = maxRate()/traces.size();
  if( traces[0].sampleRate() > maxrate ) {
    traces.addError( DaqError::InvalidSampleRate );
    traces.setSampleRate( maxrate );
  }

  return traces.failed() ? -1 : 0;
}


int AISim::prepareRead( InList &traces )
{
  // ai still running:
  if ( running() ) {
    traces.addError( DaqError::Busy );
    return -1;
  }

  // success:
  setSettings( traces, -1, 2048*sizeof( signed short ) );

  return 0;
}


int AISim::startRead( QSemaphore *sp, QReadWriteLock *datamutex,
		      QWaitCondition *datawait, QSemaphore *aosp )
{
  IsRunning = true;
  return 0;
}


int AISim::readData( void )
{
  return 0;
}


int AISim::convertData( void )
{
  return 0;
}


int AISim::stop( void )
{
  IsRunning = false;
  return 0;
}


int AISim::reset( void )
{
  Settings.clear();
  IsRunning = false;
  return 0;
}


bool AISim::running( void ) const
{
  return IsRunning;
}


int AISim::error( void ) const
{
  return 0;
}


}; /* namespace relacs */

