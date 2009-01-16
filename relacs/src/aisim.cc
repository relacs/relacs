/*
  aisim.cc
  Implementation of AnalogInput simulating an analog input device

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
  AIbiRanges[0] = 5.0;
  AIuniRanges[1] = 5.0;
  AIbiRanges[1] = 2.5;
  AIuniRanges[2] = 2.0;
  AIbiRanges[2] = 1.0;
  AIuniRanges[3] = 1.0;
  AIbiRanges[3] = 0.5;
  AIuniRanges[4] = 0.5;
  AIbiRanges[4] = 0.25;
  AIuniRanges[5] = 0.2;
  AIbiRanges[5] = 0.1;
  AIuniRanges[6] = 0.1;
  AIbiRanges[6] = 0.05;
  AIuniRanges[7] = 0.05;
  AIbiRanges[7] = 0.025;
  AIuniRanges[8] = -1.0;
  AIbiRanges[8] = -1.0;
}


AISim::~AISim( void )
{
}


int AISim::open( const string &device, long mode )
{
  clearSettings();
  setDeviceName( "AI Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device );
  return 0;
}


int AISim::open( Device &device, long mode )
{
  setDeviceName( "AI Simulation" );
  setDeviceVendor( "RELACS" );
  setDeviceFile( device.deviceIdent() );
  return 0;
}


bool AISim::isOpen( void ) const
{
  return true;
}


void AISim::close( void )
{
}


int AISim::channels( void ) const
{
  return 16;
}


int AISim::bits( void ) const
{
  return 12;
}


double AISim::maxRate( void ) const
{
  return 100000.0;
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

  long bs = traces[0].indices( traces[0].updateTime() );
  if ( bs <= 0 || bs > traces[0].capacity() ) {
    if ( bs > traces[0].capacity() )
      traces.addError( DaqError::InvalidUpdateTime );
    bs = traces[0].capacity();
    traces.setUpdateTime( traces[0].interval( bs ) );
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
  int buffersize = 0;
  for ( int k=0; k<traces.size(); k++ ) {
    int bs = traces[k].indices( traces[k].updateTime() );
    if ( bs <= 0 || bs > traces[k].capacity() )
      bs = traces[k].capacity();
    else
      bs *= 6;
    buffersize += bs;
  }
  setSettings( traces, -1, buffersize*sizeof( signed short ) );

  return 0;
}


int AISim::startRead( void )
{
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
  return 0;
}


int AISim::reset( void )
{
  clearSettings();
  return 0;
}


bool AISim::running( void ) const
{
  return false;
}


int AISim::error( void ) const
{
  return 0;
}


}; /* namespace relacs */

