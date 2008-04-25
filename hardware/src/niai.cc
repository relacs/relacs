/*
  niai.cc
  Implementation of AnalogInput for National Instruments E-Series.

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

#include <cmath>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>   
#include <relacs/niao.h>
#include <relacs/niai.h>

using namespace std;


NIAI::NIAI( void ) 
  : AnalogInput( "NI E-Series Analog Input", NIAnalogInputType ),
    Handle( -1 )
{
}


NIAI::NIAI( const string &device, long mode )
  : AnalogInput( "NI E-Series Analog Input", NIAnalogInputType ),
    Handle( -1 )
{
  open( device, mode );
}


NIAI::~NIAI( void )
{
  close();
}


int NIAI::open( const string &device, long mode )
{
  clearSettings();
  if ( device.empty() )
    Handle = -1;
  else
    Handle = ::open( device.c_str(), O_RDONLY | O_NONBLOCK );

  MaxRanges = 0;
  if ( Handle >= 0 ) {
    ::ioctl( Handle, NIDAQINFO, &Board );
    // check ioctl error
    for ( int k=MAXRANGES-1; k >= 0; k-- )
      if ( Board.aiindices[k] >= 0 ) {
	MaxRanges = k+1;
	break;
      }
    setDeviceName( Board.name );
    setDeviceVendor( "National Instruments" );
    setDeviceFile( device );
  }

  return 0;
}


bool NIAI::isOpen( void ) const
{
  return ( Handle >= 0 );
}


void NIAI::close( void )
{
  reset();
  ::close( Handle );
  Handle = -1;
}


int NIAI::channels( void ) const
{
  return Board.aich;
}


int NIAI::bits( void ) const
{
  return Board.aibits;
}


double NIAI::maxRate( void ) const
{
  return double( Board.aimaxspl );
}


int NIAI::maxRanges( void ) const
{
  return MaxRanges;
}


double NIAI::unipolarRange( int index ) const
{
  long r = Board.aiuniranges[index];
  return r > 0 ? 0.001*r : -1.0;
}


double NIAI::bipolarRange( int index ) const
{
  long r = Board.aibiranges[index];
  return r > 0 ? 0.001*r : -1.0;
}


int NIAI::testReadDevice( InList &traces )
{
  // channel gains:
  for ( int k=0; k<traces.size(); k++ ) {
    // gain code:
    int gaincode = Board.aiindices[ traces[k].gainIndex() ];
    if ( gaincode < 0 )
      traces[k].addError( DaqError::InvalidGain );
    else {
      // ranges:
      long v = 1 << bits();
      if ( traces[k].unipolar() ) {
	double max = unipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( 0.0 );
	traces[k].setGain( max / v );
      }
      else {
	double max = bipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( -max );
	traces[k].setGain( 2.0 * max / v );
      }
    }
  }

  // timebase:
  long f = ::ioctl( Handle, NIDAQTIMEBASE );
  if ( f <= 0 ) {
    traces.addErrorStr( errno );
    return -1;
  }
  double bf = double( f );

  // set delay
  long dc = long( ::rint( traces[0].delay() * bf ) );
  traces.setDelay( double( dc ) / bf );

  // set scan rate:
  long sc = long( ::rint( bf / traces[0].sampleRate() ) );
  traces.setSampleRate( bf / double( sc ) );

  return traces.failed() ? -1 : 0;
}


int NIAI::prepareRead( InList &traces )
{
  // reset analog input device:
  if ( ::ioctl( Handle, NIDAQAIRESETALL, 0 ) != 0 )
    traces.addErrorStr( errno );

  // initialize channels:
  if ( ::ioctl( Handle, NIDAQAICLEARCONFIG, 0 ) != 0 )
    traces.addErrorStr( errno );
  else {
    for ( int k=0; k<traces.size(); k++ ) {
      unsigned int u = 0;

      // gain code:
      int gaincode = Board.aiindices[ traces[k].gainIndex() ];
      if ( gaincode < 0 ) {
	traces[k].addError( DaqError::InvalidGain );
	::ioctl( Handle, NIDAQAICLEARCONFIG, 0 );
	break;
      }
      gaincode &= 7;
      u |= gaincode;

      // polarity:
      if ( traces[k].unipolar() )
	u |= 0x0100;

      // dither:
      if ( traces[k].dither() )
	u |= 0x0200;

      // last channel:
      if ( k >= traces.size()-1 )
	u |= 0x8000;

      // channel number:
      int channel = traces[k].channel();
      channel &= 0xf;
      u |= channel << 16;

      // reference:
      unsigned int t = 0;
      switch ( traces[k].reference() ) {
      case InData::RefDifferential:  t = 1; break;
      case InData::RefCommon: t = 2; break;
      case InData::RefGround: t = 3; break;
      case InData::RefOther: t = 0; break;
      }
      if ( t == 0 ) {
	traces[k].addError( DaqError::InvalidReference );
	::ioctl( Handle, NIDAQAICLEARCONFIG, 0 );
	break;
      }
      u |= t << 28;

      if ( ::ioctl( Handle, NIDAQAIADDCHANNEL, u ) != 0 ) {
	traces[k].addError( DaqError::InvalidChannelType );
	::ioctl( Handle, NIDAQAICLEARCONFIG, 0 );
	break;
      };

      // ranges:
      long v = 1 << bits();
      if ( traces[k].unipolar() ) {
	double max = unipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( 0.0 );
	traces[k].setGain( max / v );
      }
      else {
	double max = bipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( -max );
	traces[k].setGain( 2.0 * max / v );
      }

    }
  }

  // continuous sampling mode:
  if ( ::ioctl( Handle, NIDAQAICONTINUOUS, traces[0].continuous() ) != 0 )
    traces.addErrorStr( errno );

  // start source:
  if ( ::ioctl( Handle, NIDAQAISTART1SOURCE, traces[0].startSource() ) != 0 ) {
    int ern = errno;
    if ( ern == EINVAL ) {
      // set valid start source?
      traces.addError( DaqError::InvalidStartSource );
    }
    else
      traces.addErrorStr( ern );
  }

  // timebase:
  long f = ::ioctl( Handle, NIDAQTIMEBASE );
  if ( f <= 0 ) {
    traces.addErrorStr( errno );
    return -1;
  }
  double bf = double( f );

  // set delay
  long dc = long( ::rint( traces[0].delay() * bf ) );
  if ( dc == 0 )
    dc = 1;
  if ( ::ioctl( Handle, NIDAQAISCANSTART, dc ) == 0 )
    traces.setDelay( double( dc ) / bf );
  else {
    int ern = errno;
    if ( ern == EINVAL )
      traces.addError( DaqError::InvalidDelay );
    else
      traces.addErrorStr( ern );
  }

  // set scan rate:
  long sc = long( ::rint( bf / traces[0].sampleRate() ) );
  if ( ::ioctl( Handle, NIDAQAISCANINTERVAL, sc ) == 0 )
    traces.setSampleRate( bf / double( sc ) );
  else {
    int ern = errno;
    if ( ern == EINVAL )
      traces.addError( DaqError::InvalidSampleRate );
    else
      traces.addErrorStr( ern );
  }

  // shortest possible sample delay:
  if ( ::ioctl( Handle, NIDAQAISAMPLESTART, 1 ) != 0 ) {
    int ern = errno;
    if ( ern == EINVAL )
      traces.addError( DaqError::InvalidSampleRate );
    else
      traces.addErrorStr( ern );
  }

  // sample rate:
  long ssc = long( ::rint( bf / traces[0].sampleRate() / double( traces.size() ) ) );
  if ( ::ioctl( Handle, NIDAQAISAMPLEINTERVAL, ssc ) != 0 ) {
    int ern = errno;
    if ( ern == EINVAL )
      traces.addError( DaqError::InvalidSampleRate );
    else
      traces.addErrorStr( ern );
  }

  // don't free the buffer!
  // setup the buffer:
  if ( traces[0].deviceBuffer() == NULL ) {
    // size of buffer:
    traces[0].reserveDeviceBuffer( traces.size() * traces[0].indices( traces[0].updateTime() ),
				   sizeof( signed short ) );
    if ( traces[0].deviceBuffer() == NULL )
      traces[0].reserveDeviceBuffer( traces.size() * traces[0].capacity(),
				     sizeof( signed short ) );
  }
  // buffer overflow:
  if ( traces[0].deviceBufferSize() >= traces[0].deviceBufferCapacity() ) {
    traces.addError( DaqError::BufferOverflow );
    return -1;
  }

  if ( traces.success() )
    setSettings( traces );

  return traces.failed() ? -1 : 0;
}


int NIAI::startRead( InList &traces )
{
  // start analog input:
  long n = ::read( Handle, traces[0].deviceBuffer(),
		   traces[0].deviceBufferCapacity()*sizeof( signed short ) );
  int ern = errno;
  if ( n < 0 && ern != EAGAIN ) {
    traces.addErrorStr( ern );
    return -1;
  }
  if ( n > 0 ) {
    traces.addErrorStr( "start read added data" );
    return -1;
  }
  return 0;
}


int NIAI::readData( InList &traces )
{
  // buffer overflow:
  if ( traces[0].deviceBufferSize() >= traces[0].deviceBufferCapacity() ) {
    traces.addError( DaqError::BufferOverflow );
    return -1;
  }

  bool failed = false;

  int n = 0;

  // try to read twice:
  for ( int r=0;
	r<2 && ! failed && traces[0].deviceBufferMaxPush() > 0;
	r++ ) {

    long m = 0;

    // data present?
    long nd = 0;
    if ( ::ioctl( Handle, NIDAQAIDATA, &nd ) != 0 ||
	 nd <= 0 )
      break;

    // read data:
    m = ::read( Handle, traces[0].deviceBufferPushBuffer(),
		traces[0].deviceBufferMaxPush()*sizeof( signed short ) );
    int ern = errno;
    if ( m < 0 && ern != EAGAIN ) {
      traces.addErrorStr( ern );
      failed = true;
    }
    else if ( m > 0 ) {
      traces[0].deviceBufferPush( m );
      n += m;
    }

  }

  convert<signed short>( traces );

  return failed ? -1 : n;
}


int NIAI::stop( void )
{
  return ::ioctl( Handle, NIDAQAIENDONSCAN, 0 );
}


int NIAI::reset( void )
{
  int r = stop();
  ::ioctl( Handle, NIDAQAIRESETALL, 0 );
  clearSettings();
  return r;
}


bool NIAI::running( void ) const
{
  return ( ::ioctl( Handle, NIDAQAIRUNNING, 0 ) > 0 );
}


int NIAI::error( void ) const
{
  int err;
  ::ioctl( Handle, NIDAQAIERROR, &err );
  /*
    bit 1: AI_Overflow_St
    bit 2: AI_Overrun_St
  */
  // XXX map error code
  return err;
  /*
    0: ok
    1: OverflowUnderrun
    other
  */
}

