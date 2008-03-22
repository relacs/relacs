/*
  outdata.cc
  An output signal for a data acquisition board.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <iostream>
#include <fstream>
#include <iomanip>
#include <relacs/random.h>
#include <relacs/outdata.h>
using namespace std;


double OutData::DefaultMinSampleInterval = 0.001;


OutData::OutData( void ) 
  : SampleDataF()
{
  construct();
}


OutData::OutData( int n, const double stepsize ) 
  : SampleDataF( n, 0.0, stepsize, 0.0 )
{
  construct();
}


OutData::OutData( const double duration, const double stepsize ) 
  : SampleDataF( 0.0, duration, stepsize, 0.0 )
{
  construct();
}


OutData::OutData( const OutData  &od ) 
  : SampleDataF( od )
{
  Delay = od.Delay;
  StartSource = od.StartSource;
  Priority = od.Priority;
  Continuous = od.Continuous;
  Restart = od.Restart;
  MaxRate = od.MaxRate;
  Device = od.Device;
  Channel = od.Channel;
  Trace = od.Trace;
  TraceName = od.TraceName;
  Ident = od.Ident;
  Reglitch = od.Reglitch;
  RequestMinValue = od.RequestMinValue;
  RequestMaxValue = od.RequestMaxValue;
  GainIndex = od.GainIndex;
  Gain = od.Gain;
  Scale = od.Scale;
  Offset = od.Offset;
  Unit = od.Unit;
  MinData = od.MinData;
  MaxData = od.MaxData;
  SignalDelay = od.SignalDelay;
  Intensity = od.Intensity;
  CarrierFreq = od.CarrierFreq;
  DeviceBuffer = NULL;
  DeviceBufferSize = 0;
  DeviceDataSize = 2;
  DeviceBufferIndex = 0;
  AutoConvert = true;
  setError( od.error() );
}


OutData::~OutData( void )
{
}


void OutData::construct( void )
{
  Delay = 0.0;
  StartSource = 0;
  Priority = false;
  Continuous = false;
  Restart = false;
  MaxRate = -1.0;
  Device = 0;
  Channel = 0;
  Trace = -1;
  TraceName = "";
  Ident = "";
  Reglitch = false;
  RequestMinValue = AutoRange;
  RequestMaxValue = AutoRange;
  GainIndex = 0;
  Gain = 1.0;
  Scale = 1.0;
  Offset = 0.0;
  Unit = "V";
  MinData = -1;
  MaxData = +1;
  SignalDelay = 0.0;
  Intensity = NoIntensity;
  CarrierFreq = 0.0;
  DeviceBuffer = NULL;
  DeviceBufferSize = 0;
  DeviceDataSize = 2;
  DeviceBufferIndex = 0;
  AutoConvert = true;
  clearError();
}


const OutData &OutData::operator=( const OutData &od )
{
  return assign( od );
}


/* Used by macro OUTDATAOPS1SCALARDEF to generate
   definitions for unary operators of class OutData 
   that take a scalar as argument. 
   \a COPNAME is the operator name (like operator+= ),
   \a COP is the operator (like += ), and
   \a SCALAR is the type of the scalar argument. */
#define OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, SCALAR ) \
  const OutData &OutData::COPNAME( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) COP static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary operators of class OutData 
   that take scalars as argument.
   \a COPNAME is the operator name (like operator+= ), and
   \a COP is the operator name (like += ). */
#define OUTDATAOPS1SCALARDEF( COPNAME, COP ) \
  OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, float ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, double ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, long double ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, signed char ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, unsigned char ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, signed int ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, unsigned int ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, signed long ) \
    OUTDATAOPS1SINGLESCALARDEF( COPNAME, COP, unsigned long ) \


OUTDATAOPS1SCALARDEF( operator=, = )

#undef OUTDATAOPS1SINGLESCALARDEF
#undef OUTDATAOPS1SCALARDEF


const OutData &OutData::assign( const OutData &od )
{
  SampleDataF::assign( (SampleDataF&)od );
  Delay = od.Delay;
  StartSource = od.StartSource;
  Priority = od.Priority;
  Continuous = od.Continuous;
  Restart = od.Restart;
  MaxRate = od.MaxRate;
  Device = od.Device;
  Channel = od.Channel;
  Trace = od.Trace;
  TraceName = od.TraceName;
  Ident = od.Ident;
  Reglitch = od.Reglitch;
  RequestMinValue = od.RequestMinValue;
  RequestMaxValue = od.RequestMaxValue;
  GainIndex = od.GainIndex;
  Gain = od.Gain;
  Scale = od.Scale;
  Offset = od.Offset;
  Unit = od.Unit;
  MinData = od.MinData;
  MaxData = od.MaxData;
  SignalDelay = od.SignalDelay;
  Intensity = od.Intensity;
  CarrierFreq = od.CarrierFreq;
  if ( DeviceBuffer != NULL )
    delete [] DeviceBuffer;
  DeviceBuffer = NULL;
  DeviceBufferSize = 0;
  DeviceDataSize = 2;
  DeviceBufferIndex = 0;
  AutoConvert = true;
  setError( od.error() );
  return *this;
}


const OutData &OutData::copy( OutData &od ) const
{
  SampleDataF::copy( (SampleDataF&)od );
  od.Delay = Delay;
  od.StartSource = StartSource;
  od.Priority = Priority;
  od.Continuous = Continuous;
  od.Restart = Restart;
  od.MaxRate = MaxRate;
  od.Device = Device;
  od.Channel = Channel;
  od.Trace = Trace;
  od.TraceName = TraceName;
  od.Ident = Ident;
  od.Reglitch = Reglitch;
  od.RequestMinValue = RequestMinValue;
  od.RequestMaxValue = RequestMaxValue;
  od.GainIndex = GainIndex;
  od.Gain = Gain;
  od.Scale = Scale;
  od.Offset = Offset;
  od.Unit = Unit;
  od.MinData = MinData;
  od.MaxData = MaxData;
  od.SignalDelay = SignalDelay;
  od.Intensity = Intensity;
  od.CarrierFreq = CarrierFreq;
  if ( od.DeviceBuffer != NULL )
    delete [] od.DeviceBuffer;
  od.DeviceBuffer = NULL;
  od.DeviceBufferSize = 0;
  od.DeviceDataSize = 2;
  od.DeviceBufferIndex = 0;
  od.AutoConvert = true;
  od.setError( error() );
  return *this;
}


const OutData &OutData::append( float a, int n )
{
  SampleDataF::append( a, n );
  return *this;
}


const OutData &OutData::append( const OutData &od )
{
  SampleDataF::append( (SampleDataF&)od );
  return *this;
}


string OutData::errorMessage( void ) const
{
  if ( success() )
    return "";

  ostringstream ss;
  ss << "\"" << ident()
     << "\", channel " << channel() 
     << " on device " << device()
     << ": " << errorStr() << ends;
  return ss.str();
}


double OutData::sampleRate( void ) const
{
  return 1.0/stepsize();
}


void OutData::setSampleRate( double rate )
{
  if ( rate > 0.0 )
    setStepsize( 1.0/rate );
}


double OutData::sampleInterval( void ) const
{
  return stepsize();
}


void OutData::setSampleInterval( double step )
{
  setStepsize( step );
}


double OutData::delay( void ) const 
{
  return Delay;
}


void OutData::setDelay( double delay )
{
  Delay = delay >= 0.0 ? delay : 0.0;
}


int OutData::startSource( void ) const
{
  return StartSource;
}


void OutData::setStartSource( int startsource ) 
{
  StartSource = startsource;
}


bool OutData::priority( void ) const 
{
  return Priority;
}


void OutData::setPriority( const bool priority )
{
  Priority = priority;
}


bool OutData::continuous( void ) const 
{
  return Continuous;
}


void OutData::setContinuous( bool continuous )
{
  Continuous = continuous; 
}


bool OutData::restart( void ) const 
{
  return Restart;
}


void OutData::setRestart( bool restart )
{
  Restart = restart; 
}


int OutData::device( void ) const
{
  return Device;
}


void OutData::setDevice( int device )
{
  Device = device;
}


int OutData::channel( void ) const
{
  return Channel;
}


void OutData::setChannel( int channel )
{
  Channel = channel;
}


void OutData::setChannel( int channel, int device )
{
  Channel = channel;
  Device = device;
}


int OutData::trace( void ) const
{
  return Trace;
}


void OutData::setTrace( int index )
{
  Trace = index;
  TraceName = "";
  Device = -1;
  Channel = -1;
}


string OutData::traceName( void ) const
{
  return TraceName;
}


void OutData::setTraceName( const string &name )
{
  Trace = -1;
  TraceName = name;
  Device = -1;
  Channel = -1;
}


double OutData::signalDelay( void ) const
{
  return SignalDelay;
}


void OutData::setSignalDelay( double sigdelay )
{
  SignalDelay = sigdelay;
}


double OutData::intensity( void ) const
{
  return Intensity;
}


void OutData::setIntensity( double intensity )
{
  Intensity = intensity;
}


void OutData::setNoIntensity( void )
{
  Intensity = NoIntensity;
}


bool OutData::noIntensity( void ) const
{
  return ( Intensity == NoIntensity );
}


void OutData::mute( void )
{
  Intensity = MuteIntensity;
}


double OutData::carrierFreq( void ) const
{
  return CarrierFreq;
}


void OutData::setCarrierFreq( double carrierfreq )
{
  CarrierFreq = carrierfreq;
}


string OutData::ident( void ) const
{
  return Ident;
}


void OutData::setIdent( const string &ident )
{
  Ident = ident;
}


bool OutData::reglitch( void ) const
{
  return Reglitch;
}


void OutData::setReglitch( bool reglitch )
{
  Reglitch = reglitch;
}


double OutData::requestedMin( void ) const
{
  return RequestMinValue;
}


double OutData::requestedMax( void ) const
{
  return RequestMaxValue;
}


void OutData::request( double min, double max )
{
  RequestMinValue = min;
  RequestMaxValue = max;
}


void OutData::setExtRef( void )
{
  request( -1.0, ExtRef );
}


void OutData::setMinData( int min )
{
  MinData = min;
}


void OutData::setMaxData( int max )
{
  MaxData = max;
}


int OutData::gainIndex( void ) const
{
  return GainIndex;
}


void OutData::setGainIndex( int index )
{
  GainIndex = index;
}


double OutData::gain( void ) const
{
  return Gain;
}


void OutData::setGain( double gain )
{
  Gain = gain;
}


double OutData::voltage( int index ) const
{
  return ( operator[]( index ) + offset() ) * scale();
}


double OutData::getVoltage( double val ) const
{
  return ( val + offset() ) * scale();
}


double OutData::minVoltage( void ) const
{
  return MinData / gain();
}


double OutData::maxVoltage( void ) const
{
  return MaxData / gain();
}


double OutData::scale( void ) const
{
  return Scale;
}


void OutData::setScale( double scale )
{
  Scale = scale;
}


void OutData::multiplyScale( double fac )
{
  Scale *= fac;
}


double OutData::offset( void ) const
{
  return Offset;
}


void OutData::setOffset( double offset )
{
  Offset = offset;
}


string OutData::unit( void ) const
{
  return Unit;
}


void OutData::setUnit( const string &unit )
{
  Unit = unit;
}


void OutData::setUnit( double scale, double offset, const string &unit )
{
  Scale = scale;
  Offset = offset;
  Unit = unit;
}


double OutData::minValue( void ) const
{
  if ( noIntensity() )
    return MinData/Gain/Scale - Offset;
  else
    return -1.0;
}


double OutData::maxValue( void ) const
{
  if ( noIntensity() )
    return MaxData/Gain/Scale - Offset;
  else
    return 1.0;
}


double OutData::duration( void ) const
{
  return length();
}


double OutData::totalDuration( void ) const
{
  return Delay + duration();
}


double OutData::maxSampleRate( void )
{
  if ( MaxRate > 0.0 )
    return MaxRate;
  else
    return 1.0/DefaultMinSampleInterval;
}


void OutData::setMaxSampleRate( double maxrate )
{
  if ( maxrate > 0.0 )
    MaxRate = maxrate;
}


double OutData::minSampleInterval( void )
{
  if ( MaxRate > 0.0 )
    return 1.0/MaxRate;
  else
    return DefaultMinSampleInterval;
}


void OutData::setMinSampleInterval( double minsample )
{
  if ( minsample > 0.0 )
    MaxRate = 1.0/minsample;
}


double OutData::defaultMaxSampleRate( void )
{
  return 1.0/DefaultMinSampleInterval;
}


void OutData::setDefaultMaxSampleRate( double maxrate )
{
  if ( maxrate > 0.0 )
    DefaultMinSampleInterval = 1.0/maxrate;
}


double OutData::defaultMinSampleInterval( void )
{
  return DefaultMinSampleInterval;
}


void OutData::setDefaultMinSampleInterval( double minsample )
{
  if ( minsample > 0.0 )
    DefaultMinSampleInterval = minsample;
}


double OutData::bestSampleRate( double carrierfreq )
{
  double msi = minSampleInterval();
  if ( carrierfreq <= 1.0e-8 )
    return 1.0/msi;
  else {
    double rate = ::floor( 1.0/msi/4.0/carrierfreq )*4.0*carrierfreq;
    if ( rate <= 1.0e-8 )
      return 1.0/msi;
    else
      return rate;
  }
}


double OutData::bestSampleInterval( double carrierfreq )
{
  double msi = minSampleInterval();
  if ( carrierfreq <= 1.0e-8 )
    return msi;
  else {
    double rate = ::floor( 1.0/msi/4.0/carrierfreq )*4.0*carrierfreq;
    if ( rate <= 1.0e-8 )
      return msi;
    else
      return 1.0/rate;
  }
}


void OutData::setBestSample( double carrierfreq )
{
  setCarrierFreq( carrierfreq );
  setStepsize( bestSampleInterval( carrierfreq ) );
}


istream &OutData::load( istream &str,
			const string &ident,
			double carrierfreq )
{
  free();

  // skip header and read key:
  double tfac = 1.0;
  string s;
  while ( getline( str, s ) && 
	  ( s.empty() || s.find( '#' ) == 0 ) ) {
    if ( s.find( "#Key" ) == 0 ) {
      for ( int k=0; 
	    getline( str, s ) && 
	      ( s.empty() || s.find( '#' ) == 0 ); 
	    k++ ) {
	if ( k < 4 && s.find( "ms" ) != string::npos )
	  tfac = 0.001;
      }
      break;
    }
  }

  // load data:
  SampleDataF::load( str, "EMPTY", &s );

  if ( tfac != 0.0 )
    range() *= tfac;

  setIdent( ident );
  setCarrierFreq( carrierfreq );
  clearError();

  return str;
}


OutData &OutData::load( const string &filename, 
			const string &ident,
			double carrierfreq )
{
  free();

  // open file:
  ifstream str( filename.c_str() );
  if ( str.bad() )
    return *this;

  // load:
  load( str, ident.empty() ? filename : ident, carrierfreq );

  return *this;
}


double OutData::maximize( double max )
{
  double maxval = numerics::max( *this );
  double c = max/maxval;
  *this *= c;
  return c;
}


void OutData::fill( const SampleDataD &am, double carrierfreq,
		    const string &ident )
{
  free();

  if ( carrierfreq <= 0.0 )
    return;

  setStepsize( bestSampleInterval( carrierfreq ) );
  setRangeBack( am.length() );

  if ( am.size() < 2 )
    return;

  // create lookup-table for one sine wave:
  SampleDataF sinbuf;
  sinbuf.sin( LinearRange( int( rint( 1.0 / ( carrierfreq * stepsize() ) ) ), 
			   0.0, stepsize() ), carrierfreq );
  
  // fill am with carrier sine wave:
  double slope = (am[1]-am[0])/am.stepsize();
  for ( int i=0, j=0, k=1; i<size(); i++, j++ ) {
    double t = pos( i );
    while ( am.pos( k+1 ) < t && k+1 < am.size() ) {
      k++;
      slope = (am[k]-am[k-1])/am.stepsize();
    }
    if ( j >= sinbuf.size() ) 
      j=0;
    operator[]( i ) = sinbuf[j] * ( am[k] + slope * ( t - am.pos(k) ) );
  }

  setCarrierFreq( carrierfreq );
  setIdent( ident );
  clearError();
}


void OutData::loadAM( const string &filename, double carrierfreq, 
		      const string &ident, SampleDataD *stimulus )
{
  SampleDataD *am;
  if ( stimulus != 0 )
    am = stimulus;
  else
    am = new SampleDataD;

  am->load( filename );

  string s = ( ident == "" ? filename :  ident );

  fill( *am, carrierfreq, s );
}


void OutData::sineWave( double freq, double duration, double ampl, double r,
			const string &ident )
{
  sin( 0.0, duration, bestSampleInterval( freq ), freq );
  if ( ampl != 1.0 )
    array() *= ampl;
  if ( r > 0.0 )
    ramp( r );

  setCarrierFreq( freq );
  setIdent( ident );
  clearError();
}


void OutData::noiseWave( double cutofffreq, double duration, double stdev,
			 double r, const string &ident )
{
  whiteNoise( duration, bestSampleInterval( cutofffreq ), 0.0, 
	      cutofffreq, numerics::rnd );
  if ( stdev != 1.0 )
    array() *= stdev;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  setCarrierFreq( cutofffreq );
  setIdent( ident );
  clearError();
}


void OutData::bandNoiseWave( double cutofffreqlow, double cutofffreqhigh, 
			     double duration, double stdev,
			     double r, const string &ident )
{
  whiteNoise( duration, bestSampleInterval( cutofffreqhigh ), 
	      cutofffreqlow, cutofffreqhigh, numerics::rnd );
  if ( stdev != 1.0 )
    array() *= stdev;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  setCarrierFreq( cutofffreqhigh );
  setIdent( ident );
  clearError();
}


void OutData::ouNoiseWave( double tau, double duration, double stdev,
			   double r, const string &ident )
{
  ouNoise( duration, minSampleInterval(), tau, numerics::rnd );
  if ( stdev != 1.0 )
    array() *= stdev;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  setCarrierFreq( 1/tau );
  setIdent( ident );
  clearError();
}


void OutData::setDeviceBuffer( char *buffer, int nbuffer, int dsize )
{
  if ( DeviceBuffer != 0 )
    delete [] DeviceBuffer;
  DeviceBuffer = buffer;
  DeviceBufferSize = nbuffer;
  DeviceDataSize = dsize;
  DeviceBufferIndex = 0;
}


void OutData::freeDeviceBuffer( void )
{
  if ( DeviceBuffer != 0 )
    delete [] DeviceBuffer;
  DeviceBuffer = NULL;
  DeviceBufferSize = 0;
  DeviceDataSize = 2;
  DeviceBufferIndex = 0;
}


void OutData::setAutoConvert( void )
{
  AutoConvert = true;
}


void OutData::setManualConvert( void )
{
  AutoConvert = false;
}


ostream &operator<<( ostream &str, const OutData &od )
{
  str << SampleDataF( od );
  str << DaqError( od );
  str << "Delay: " << od.Delay << '\n';
  str << "StartSource: " << od.StartSource << '\n';
  str << "Priority: " << od.Priority << '\n';
  str << "Continuous: " << od.Continuous << '\n';
  str << "Restart: " << od.Restart << '\n';
  str << "MaxRate: " << od.MaxRate << '\n';
  str << "Device: " << od.Device << '\n';
  str << "Channel: " << od.Channel << '\n';
  str << "Trace: " << od.Trace << '\n';
  str << "TraceName: " << od.TraceName << '\n';
  str << "Ident: " << od.Ident << '\n';
  str << "Reglitch: " << od.Reglitch << '\n';
  str << "RequestMinValue: " << od.RequestMinValue << '\n';
  str << "RequestMaxValue: " << od.RequestMaxValue << '\n';
  str << "GainIndex: " << od.GainIndex << '\n';
  str << "Gain: " << od.Gain << '\n';
  str << "Scale: " << od.Scale << '\n';
  str << "Offset: " << od.Offset << '\n';
  str << "Unit: " << od.Unit << '\n';
  str << "MinData: " << od.MinData << '\n';
  str << "MaxData: " << od.MaxData << '\n';
  str << "SignalDelay: " << od.SignalDelay << '\n';
  str << "Intensity: " << od.Intensity << '\n';
  str << "CarrierFreq: " << od.CarrierFreq << '\n';
  str << "DeviceBuffer: " << od.DeviceBuffer << '\n';
  str << "DeviceBufferSize: " << od.DeviceBufferSize << '\n';
  str << "DeviceDataSize: " << od.DeviceDataSize << '\n';
  str << "DeviceBufferIndex: " << od.DeviceBufferIndex << '\n';
  str << "AutoConvert: " << od.AutoConvert << '\n';
  return str;
}

