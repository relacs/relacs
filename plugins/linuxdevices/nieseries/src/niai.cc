/*
  nieseries/niai.cc
  Implementation of AnalogInput for National Instruments E-Series.

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

#include <cmath>
#include <cerrno>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>   
#include <relacs/nieseries/niao.h>
#include <relacs/nieseries/niai.h>
using namespace std;
using namespace relacs;

namespace nieseries {


NIAI::NIAI( void ) 
  : AnalogInput( "NI E-Series Analog Input", NIAnalogIOType ),
    Handle( -1 )
{
  Traces = 0;
  ReadBufferSize = 0;
  BufferSize = 0;
  BufferN = 0;
  Buffer = NULL;
  TraceIndex = 0;
}


NIAI::NIAI( const string &device, const Options &opts )
  : AnalogInput( "NI E-Series Analog Input", NIAnalogIOType ),
    Handle( -1 )
{
  Traces = 0;
  ReadBufferSize = 0;
  BufferSize = 0;
  BufferN = 0;
  Buffer = NULL;
  TraceIndex = 0;
  open( device, opts );
}


NIAI::~NIAI( void )
{
  close();
}


int NIAI::open( const string &device, const Options &opts )
{
  if ( isOpen() )
    return -5;

  Info.clear();
  Settings.clear();
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
  else
    return InvalidDevice;

  setInfo();

  return 0;
}


bool NIAI::isOpen( void ) const
{
  return ( Handle >= 0 );
}


void NIAI::close( void )
{
  reset();
  Info.clear();
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
      // allocate gain factor:
      char *gaindata = traces[k].gainData();
      if ( gaindata != NULL )
	delete [] gaindata;
      gaindata = new char[sizeof(double)];
      traces[k].setGainData( gaindata );
      double *gainp = (double *)gaindata;
      // ranges:
      long v = 1 << bits();
      if ( traces[k].unipolar() ) {
	double max = unipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( 0.0 );
	*gainp = max / v;
      }
      else {
	double max = bipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( -max );
	*gainp = 2.0 * max / v;
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

  // delay
  long dc = long( ::rint( traces[0].delay() * bf ) );
  traces.setDelay( double( dc ) / bf );

  // scan rate:
  long sc = long( ::rint( bf / traces[0].sampleRate() ) );
  traces.setSampleRate( bf / double( sc ) );

  // check read buffer size:
  int readbufsize = traces.size() * traces[0].indices( traces[0].readTime() );
  if ( readbufsize <= 0 ) {
    traces.addError( DaqError::InvalidBufferTime );
    traces.setReadTime( 0.01 );
    readbufsize = traces.size() * traces[0].indices( traces[0].readTime() );
  }

  // check update buffer size:
  int bufsize = traces.size() * traces[0].indices( traces[0].updateTime() );
  if ( bufsize < readbufsize )
    traces.addError( DaqError::InvalidUpdateTime );

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

      // allocate gain factor:
      char *gaindata = traces[k].gainData();
      if ( gaindata != NULL )
	delete [] gaindata;
      gaindata = new char[sizeof(double)];
      traces[k].setGainData( gaindata );
      double *gainp = (double *)gaindata;
      // ranges:
      long v = 1 << bits();
      if ( traces[k].unipolar() ) {
	double max = unipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( 0.0 );
	*gainp = max / v;
      }
      else {
	double max = bipolarRange( traces[k].gainIndex() );
	traces[k].setMaxVoltage( max );
	traces[k].setMinVoltage( -max );
	*gainp = 2.0 * max / v;
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

  // size of driver buffer:
  ReadBufferSize = 5 * traces.size() * traces[0].indices( traces[0].readTime() );

  // init internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = NULL;
  BufferSize = 2 * traces.size() * traces[0].indices( traces[0].updateTime() );
  if ( BufferSize <= 0 )
    traces.addError( DaqError::InvalidUpdateTime );
  else
    Buffer = new signed short[BufferSize];
  BufferN = 0;

  if ( traces.success() ) {
    setSettings( traces, ReadBufferSize*sizeof( signed short ),
		 BufferSize*sizeof( signed short ) );
    Traces = &traces;
  }

  return traces.failed() ? -1 : 0;
}


int NIAI::startRead( void )
{
  if ( Traces == 0 )
    return -1;

  // start analog input:
  signed short *buffer[2024];
  long n = ::read( Handle, buffer, ReadBufferSize );
  int ern = errno;
  if ( n < 0 && ern != EAGAIN ) {
    Traces->addErrorStr( ern );
    return -1;
  }
  if ( n > 0 ) {
    Traces->addErrorStr( "start read added data" );
    return -1;
  }
  return 0;
}


void NIAI::convert( InList &traces, signed short *buffer, int n )
{
  // scale factors:
  double scale[traces.size()];
  for ( int k=0; k<traces.size(); k++ ) {
    double *gainp = (double *)traces[k].gainData();
    scale[k] = (*gainp) * traces[k].scale();
  }

  // trace buffer pointers and sizes:
  float *bp[traces.size()];
  int bm[traces.size()];
  int bn[traces.size()];
  for ( int k=0; k<traces.size(); k++ ) {
    bp[k] = traces[k].pushBuffer();
    bm[k] = traces[k].maxPush();
    bn[k] = 0;
  }

  // pointer for device buffer:
  signed short *db = buffer;

  for ( int k=0; k<n; k++ ) {
    // convert:
    *bp[TraceIndex] = scale[TraceIndex] * db[k];
    // update pointers:
    bp[TraceIndex]++;
    bn[TraceIndex]++;
    if ( bn[TraceIndex] >= bm[TraceIndex] ) {
      traces[TraceIndex].push( bn[TraceIndex] );
      bp[TraceIndex] = traces[TraceIndex].pushBuffer();
      bm[TraceIndex] = traces[TraceIndex].maxPush();
      bn[TraceIndex] = 0;
    }
    // next trace:
    TraceIndex++;
    if ( TraceIndex >= traces.size() )
      TraceIndex = 0;
  }

  // commit:
  for ( int c=0; c<traces.size(); c++ )
    traces[c].push( bn[c] );
}


int NIAI::readData( void )
{
  if ( Traces == 0 || Buffer == 0 )
    return -1;

  bool failed = false;
  int maxn = BufferSize - BufferN;

  // try to read twice:
  for ( int tryit=0; tryit<2 && ! failed && maxn > 0; tryit++ ) {

    // data present?
    long nd = 0;
    if ( ::ioctl( Handle, NIDAQAIDATA, &nd ) != 0 ||
	 nd <= 0 )
      break;

    // read data:
    long m = ::read( Handle, Buffer + BufferN, maxn*sizeof( signed short ) );

    int ern = errno;
    if ( m < 0 && ern != EAGAIN ) {
      Traces->addErrorStr( ern );
      failed = true;
    }
    else if ( m > 0 ) {
      m /= sizeof( signed short );
      maxn -= m;
      BufferN += m;
    }

  }

  if ( failed )
    return -1;

  // no more data to be read:
  if ( BufferN <= 0 && !running() )
    return -1;

  return BufferN;
}


int NIAI::convertData( void )
{
  if ( Traces == 0 || Buffer == 0 )
    return -1;

  convert( *Traces, Buffer, BufferN );

  int n = BufferN;
  BufferN = 0;

  return n;
}


int NIAI::stop( void )
{
  return ::ioctl( Handle, NIDAQAIENDONSCAN, 0 );
}


int NIAI::reset( void )
{
  int r = stop();
  ::ioctl( Handle, NIDAQAIRESETALL, 0 );


  // free internal buffer:
  if ( Buffer != 0 )
    delete [] Buffer;
  Buffer = NULL;
  BufferSize = 0;
  BufferN = 0;

  Settings.clear();
  Traces = 0;
  ReadBufferSize = 0;
  TraceIndex = 0;

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


}; /* namespace nieseries */
