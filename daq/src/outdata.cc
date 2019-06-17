/*
  outdata.cc
  An output signal for a data acquisition board.

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
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <relacs/random.h>
#include <relacs/strqueue.h>
#include <relacs/acquire.h>
#include <relacs/outdata.h>
using namespace std;

namespace relacs {


const double OutData::MuteIntensity = Attenuate::MuteIntensity;
const double OutData::NoIntensity = -2.0e37;
const double OutData::NoLevel = -2.0e37;
const double OutData::AutoRange = -2.0e300;
const double OutData::ExtRef = -1.0e300;

double OutData::DefaultMinSampleInterval = 0.0001;
const Acquire *OutData::A = 0;


OutData::OutData( void ) 
  : SampleDataF()
{
  construct();
}


OutData::OutData( int n, double stepsize ) 
  : SampleDataF( n, 0.0, stepsize, 0.0 )
{
  construct();
}


OutData::OutData( double duration, double stepsize ) 
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
  FixedRate = od.FixedRate;
  Device = od.Device;
  Channel = od.Channel;
  Trace = od.Trace;
  TraceName = od.TraceName;
  Ident = od.Ident;
  Description = od.Description;
  RequestMinValue = od.RequestMinValue;
  RequestMaxValue = od.RequestMaxValue;
  GainData = 0;
  Scale = od.Scale;
  Unit = od.Unit;
  MinVoltage = od.MinVoltage;
  MaxVoltage = od.MaxVoltage;
  Intensity = od.Intensity;
  CarrierFreq = od.CarrierFreq;
  Level = od.Level;
  DeviceIndex = od.DeviceIndex;
  DeviceDelay = od.DeviceDelay;
  DeviceCount = od.DeviceCount;
  setError( od.error() );
}


OutData::~OutData( void )
{
  if ( GainData != 0 )
    delete [] GainData;
}


void OutData::construct( void )
{
  Delay = 0.0;
  StartSource = 0;
  Priority = false;
  Continuous = false;
  Restart = false;
  MaxRate = -1.0;
  FixedRate = false;
  Device = 0;
  Channel = 0;
  Trace = -1;
  TraceName = "";
  Ident = "";
  Description.clear();
  RequestMinValue = AutoRange;
  RequestMaxValue = AutoRange;
  GainData = 0;
  Scale = 1.0;
  Unit = "V";
  MinVoltage = -1.0;
  MaxVoltage = +1.0;
  Intensity = NoIntensity;
  CarrierFreq = 0.0;
  Level = NoLevel;
  DeviceIndex = 0;
  DeviceDelay = 0;
  DeviceCount = 0;
  clearError();
}


const OutData &OutData::operator=( const OutData &od )
{
  return assign( od );
}


/* Used by macro OUTDATAASSIGNSCALARDEF to generate
   definitions for unary assignment operators of class OutData 
   that take a scalar as argument. 
   \a SCALAR is the type of the scalar argument. */
#define OUTDATAASSIGNSCALAR( SCALAR ) \
  const OutData &OutData::operator=( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) = static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary assignment operators of class OutData 
   that take scalars as argument. */
#define OUTDATAASSIGNSCALARDEF \
  OUTDATAASSIGNSCALAR( float ) \
    OUTDATAASSIGNSCALAR( double ) \
    OUTDATAASSIGNSCALAR( long double ) \
    OUTDATAASSIGNSCALAR( signed char ) \
    OUTDATAASSIGNSCALAR( unsigned char ) \
    OUTDATAASSIGNSCALAR( signed int ) \
    OUTDATAASSIGNSCALAR( unsigned int ) \
    OUTDATAASSIGNSCALAR( signed long ) \
    OUTDATAASSIGNSCALAR( unsigned long ) \


OUTDATAASSIGNSCALARDEF

#undef OUTDATAASSIGNSCALAR
#undef OUTDATAASSIGNSCALARDEF


/* Used by macro OUTDATAADDSCALARDEF to generate
   definitions for unary addition operators of class OutData 
   that take a scalar as argument. 
   \a SCALAR is the type of the scalar argument. */
#define OUTDATAADDSCALAR( SCALAR ) \
  const OutData &OutData::operator+=( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) += static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary addition operators of class OutData 
   that take scalars as argument. */
#define OUTDATAADDSCALARDEF \
  OUTDATAADDSCALAR( float ) \
    OUTDATAADDSCALAR( double ) \
    OUTDATAADDSCALAR( long double ) \
    OUTDATAADDSCALAR( signed char ) \
    OUTDATAADDSCALAR( unsigned char ) \
    OUTDATAADDSCALAR( signed int ) \
    OUTDATAADDSCALAR( unsigned int ) \
    OUTDATAADDSCALAR( signed long ) \
    OUTDATAADDSCALAR( unsigned long ) \


OUTDATAADDSCALARDEF

#undef OUTDATAADDSCALAR
#undef OUTDATAADDSCALARDEF


/* Used by macro OUTDATASUBTRACTSCALARDEF to generate
   definitions for unary subtraction operators of class OutData 
   that take a scalar as argument. 
   \a SCALAR is the type of the scalar argument. */
#define OUTDATASUBTRACTSCALAR( SCALAR ) \
  const OutData &OutData::operator-=( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) -= static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary subtraction operators of class OutData 
   that take scalars as argument. */
#define OUTDATASUBTRACTSCALARDEF \
  OUTDATASUBTRACTSCALAR( float ) \
    OUTDATASUBTRACTSCALAR( double ) \
    OUTDATASUBTRACTSCALAR( long double ) \
    OUTDATASUBTRACTSCALAR( signed char ) \
    OUTDATASUBTRACTSCALAR( unsigned char ) \
    OUTDATASUBTRACTSCALAR( signed int ) \
    OUTDATASUBTRACTSCALAR( unsigned int ) \
    OUTDATASUBTRACTSCALAR( signed long ) \
    OUTDATASUBTRACTSCALAR( unsigned long ) \


OUTDATASUBTRACTSCALARDEF

#undef OUTDATASUBTRACTSCALAR
#undef OUTDATASUBTRACTSCALARDEF


/* Used by macro OUTDATAMULTIPLYSCALARDEF to generate
   definitions for unary multiplication operators of class OutData 
   that take a scalar as argument. 
   \a SCALAR is the type of the scalar argument. */
#define OUTDATAMULTIPLYSCALAR( SCALAR ) \
  const OutData &OutData::operator*=( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) *= static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary multiplication operators of class OutData 
   that take scalars as argument. */
#define OUTDATAMULTIPLYSCALARDEF \
    OUTDATAMULTIPLYSCALAR( float ) \
    OUTDATAMULTIPLYSCALAR( double ) \
    OUTDATAMULTIPLYSCALAR( long double ) \
    OUTDATAMULTIPLYSCALAR( signed char ) \
    OUTDATAMULTIPLYSCALAR( unsigned char ) \
    OUTDATAMULTIPLYSCALAR( signed int ) \
    OUTDATAMULTIPLYSCALAR( unsigned int ) \
    OUTDATAMULTIPLYSCALAR( signed long ) \
    OUTDATAMULTIPLYSCALAR( unsigned long ) \


OUTDATAMULTIPLYSCALARDEF

#undef OUTDATAMULTIPLYSCALAR
#undef OUTDATAMULTIPLYSCALARDEF


/* Used by macro OUTDATADIVIDESCALARDEF to generate
   definitions for unary division operators of class OutData 
   that take a scalar as argument. 
   \a SCALAR is the type of the scalar argument. */
#define OUTDATADIVIDESCALAR( SCALAR ) \
  const OutData &OutData::operator/=( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) /= static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary division operators of class OutData 
   that take scalars as argument. */
#define OUTDATADIVIDESCALARDEF \
    OUTDATADIVIDESCALAR( float ) \
    OUTDATADIVIDESCALAR( double ) \
    OUTDATADIVIDESCALAR( long double ) \
    OUTDATADIVIDESCALAR( signed char ) \
    OUTDATADIVIDESCALAR( unsigned char ) \
    OUTDATADIVIDESCALAR( signed int ) \
    OUTDATADIVIDESCALAR( unsigned int ) \
    OUTDATADIVIDESCALAR( signed long ) \
    OUTDATADIVIDESCALAR( unsigned long ) \


OUTDATADIVIDESCALARDEF

#undef OUTDATADIVIDESCALAR
#undef OUTDATADIVIDESCALARDEF


const OutData &OutData::operator+=( const OutData &od )
{
  if ( ::fabs( stepsize() - od.stepsize() ) > 1e-8 )
    return *this;

  iterator iter1 = begin() + index( od.offset() );
  iterator end1 = end();
  const_iterator iter2 = od.begin();
  const_iterator end2 = od.end();
  while ( iter1 != end1 && iter2 != end2 ) {
    (*iter1) += static_cast< value_type >( *iter2 );
    ++iter1;
    ++iter2;
  };

  Description.clearSections();
  Description.newSection( od.Description );

  return *this;
}


OutData &OutData::assign( const OutData &od )
{
  SampleDataF::assign( (SampleDataF&)od );
  Delay = od.Delay;
  StartSource = od.StartSource;
  Priority = od.Priority;
  Continuous = od.Continuous;
  Restart = od.Restart;
  MaxRate = od.MaxRate;
  FixedRate = od.FixedRate;
  Device = od.Device;
  Channel = od.Channel;
  Trace = od.Trace;
  TraceName = od.TraceName;
  Ident = od.Ident;
  Description = od.Description;
  RequestMinValue = od.RequestMinValue;
  RequestMaxValue = od.RequestMaxValue;
  GainData = 0;
  Scale = od.Scale;
  Unit = od.Unit;
  MinVoltage = od.MinVoltage;
  MaxVoltage = od.MaxVoltage;
  Intensity = od.Intensity;
  CarrierFreq = od.CarrierFreq;
  Level = od.Level;
  DeviceIndex = od.DeviceIndex;
  DeviceDelay = od.DeviceDelay;
  DeviceCount = od.DeviceCount;
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
  od.FixedRate = FixedRate;
  od.Device = Device;
  od.Channel = Channel;
  od.Trace = Trace;
  od.TraceName = TraceName;
  od.Ident = Ident;
  od.Description = Description;
  od.RequestMinValue = RequestMinValue;
  od.RequestMaxValue = RequestMaxValue;
  od.GainData = 0;
  od.Scale = Scale;
  od.Unit = Unit;
  od.MinVoltage = MinVoltage;
  od.MaxVoltage = MaxVoltage;
  od.Intensity = Intensity;
  od.CarrierFreq = CarrierFreq;
  od.Level = Level;
  od.DeviceIndex = DeviceIndex;
  od.DeviceDelay = DeviceDelay;
  od.DeviceCount = DeviceCount;
  od.setError( error() );
  return *this;
}


OutData &OutData::append( const OutData &od, const string &name )
{
  double tstart = length();

  SampleDataF::append( (SampleDataF&)od );

  if ( Description.type() != "stimulus" ) {
    Options myopt = Description;
    Description.clear();
    Description.setType( "stimulus" );
    Description.setName( name );
    // XXX if myopt only contains sections, we should add these sections!
    Description.newSection( myopt );
  }
  Description.clearSections();
  Options &opt = Description.newSection( od.Description );
  bool foundtstart = false;
  bool foundduration = false;
  for ( Options::iterator pp = opt.begin(); pp != opt.end(); ++pp ) {
    if ( pp->name() == "StartTime" ) {
      pp->setNumber( pp->number() + tstart );
      foundtstart = true;
    }
    else if ( pp->name() == "Duration" )
      foundduration = true;
  }
  if ( ! foundduration )
    opt.insertNumber( "Duration", "", od.length(), "s" );
  if ( ! foundtstart )
    opt.insertNumber( "StartTime", "", tstart, "s" );
  // stimulus sections need to have unique names:
  for ( Options::section_iterator sp = Description.sectionsBegin();
	sp != Description.sectionsEnd();
	++sp ) {
    if ( (*sp)->name().empty() ) {
      // set name to type with unique number:
      string name = (*sp)->type();
      if ( name.find( "stimulus/" ) == 0 )
	name.erase( 0, 9 );
      for ( int k=1; k<10000; k++ ) {
	string sname = name + '-' + Str( k );
	bool found = false;
	for ( Options::section_iterator ssp = sp+1;
	      ssp != Description.sectionsEnd();
	      ++ssp ) {
	  if ( (*ssp)->name() == sname ) {
	    found = true;
	    break;
	  }
	}
	if ( ! found ) {
	  (*sp)->setName( sname );
	  break;
	}
      }
    }
  }
  return *this;
}


OutData &OutData::shift( double time )
{
  SampleDataF::shift( time );
  Description.setNumber( "StartTime", SampleDataF::offset() );
  return *this;
}


OutData &OutData::repeat( int n, const string &name )
{
  double duration = length();
  SampleDataF::repeat( n );

  Options myopt = Description;
  Description.clear();
  Description.setType( "stimulus/periodic" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", length(), "s" );
  Description.addInteger( "Repeats", n );
  Description.addNumber( "Pause", 0.0, "s" );
  Description.addNumber( "Frequency", 1.0/duration, "Hz" );
  // XXX if myopt only contains sections, we should add these sections!
  Description.newSection( myopt );

  return *this;
}


void OutData::clear( void )
{
  SampleDataF::clear();
  Description.clear();
}


string OutData::errorMessage( void ) const
{
  if ( success() )
    return "";

  ostringstream ss;
  ss << "\"" << ident()
     << "\", channel " << channel() 
     << " on device " << device()
     << ": " << errorStr();
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


int OutData::setTrace( int index )
{
  Trace = index;
  TraceName = "";
  Device = -1;
  Channel = -1;
  if ( A != 0 )
    return A->applyOutTrace( *this );
  else
    return 0;
}


string OutData::traceName( void ) const
{
  return TraceName;
}


int OutData::setTraceName( const string &name )
{
  Trace = -1;
  TraceName = name;
  Device = -1;
  Channel = -1;
  if ( A != 0 )
    return A->applyOutTrace( *this );
  else
    return 0;
}


void OutData::setTrace( int index, const string &name )
{
  Trace = index;
  TraceName = name;
}


void OutData::setAcquire( const Acquire *a )
{
  A = a;
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


bool OutData::muted( void ) const
{
  return ( Level == MuteIntensity );
}


double OutData::carrierFreq( void ) const
{
  return CarrierFreq;
}


void OutData::setCarrierFreq( double carrierfreq )
{
  CarrierFreq = carrierfreq;
}


double OutData::level( void ) const
{
  return Level;
}


void OutData::setLevel( double level )
{
  Level = level;
}


void OutData::setNoLevel( void )
{
  Level = NoLevel;
}


bool OutData::noLevel( void ) const
{
  return ( Level == NoLevel );
}


string OutData::ident( void ) const
{
  return Ident;
}


void OutData::setIdent( const string &ident )
{
  Ident = ident;
}


const Options &OutData::description( void ) const
{
  return Description;
}


Options &OutData::description( void )
{
  return Description;
}


void OutData::setDescription( const Options &description )
{
  Description = description;
}


Parameter &OutData::setMutable( const string &name )
{
  return Description.addFlags( name, Mutable );
}


Parameter &OutData::setMutable( const string &name, Options &opt )
{
  return opt.addFlags( name, Mutable );
}


Parameter &OutData::setMutable( Parameter &p )
{
  return p.addFlags( Mutable );
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


char *OutData::gainData( void ) const
{
  return GainData;
}


void OutData::setGainData( char *data )
{
  GainData = data;
}


double OutData::voltage( int index ) const
{
  return operator[]( index ) * scale();
}


double OutData::getVoltage( double val ) const
{
  return val * scale();
}


double OutData::minVoltage( void ) const
{
  return MinVoltage;
}


double OutData::maxVoltage( void ) const
{
  return MaxVoltage;
}


void OutData::setMinVoltage( double minv )
{
  MinVoltage = minv;
}


void OutData::setMaxVoltage( double maxv )
{
  MaxVoltage = maxv;
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


string OutData::unit( void ) const
{
  return Unit;
}


void OutData::setUnit( const string &unit )
{
  Unit = unit;
}


void OutData::setUnit( double scale, const string &unit )
{
  Scale = scale;
  Unit = unit;
}


double OutData::minValue( void ) const
{
  if ( noLevel() )
    return MinVoltage/Scale;
  else
    return -1.0;
}


double OutData::maxValue( void ) const
{
  if ( noLevel() )
    return MaxVoltage/Scale;
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
  FixedRate = false;
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
  FixedRate = false;
}


void OutData::setFixedSampleRate( double rate )
{
  if ( rate > 0.0 )
    MaxRate = rate;
  FixedRate = true;
}


void OutData::setFixedSampleInterval( double interval )
{
  if ( interval > 0.0 )
    MaxRate = 1.0/interval;
  FixedRate = true;
}


bool OutData::fixedSampleRate( void ) const
{
  return FixedRate;
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


double OutData::bestSampleInterval( double carrierfreq )
{
  double msi = minSampleInterval();
  if ( fixedSampleRate() )
    return msi;

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


void OutData::fixSample( void )
{
  if ( fixedSampleRate() &&
       ::fabs( minSampleInterval() - sampleInterval() )/minSampleInterval() > 0.001 ) {
    SampleDataF sig( *this );
    SampleDataF::interpolate( sig, 0.0, bestSampleInterval( -1.0 ) );
  }
}


istream &OutData::load( istream &str, const string &filename )
{
  clear();

  // skip header and read key:
  StrQueue sq;
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
    else
      sq.add( s );
  }

  // load data:
  SampleDataF::load( str, "EMPTY", &s );

  if ( tfac != 0.0 )
    range() *= tfac;

  // metadata:
  sq.stripComments( "-#" );
  Description.clear();
  Description.load( sq );
  Description.insertText( "File", "", filename );
  if ( Description.type().empty() ) {
    Description.setType( "stimulus/file" );
  }

  setIdent( filename );
  clearError();

  return str;
}


OutData &OutData::load( const string &file, const string &filename )
{
  clear();

  string ext = Str( filename ).extension().lower();
  if ( ext == ".wav" ) {
#ifdef HAVE_LIBSNDFILE
    SampleDataF::loadSndFile( file );
    Description.clear();
    Description.insertText( "File", "", filename );
    Description.setType( "stimulus/file" );
    setIdent( filename );
    clearError();
#endif
  }
  else {
    // open file:
    ifstream str( file.c_str() );
    if ( str.bad() )
      return *this;
    
    // load:
    load( str, filename.empty() ? file : filename );
  }

  return *this;
}


double OutData::maximize( double max )
{
  float maxval = ::relacs::max( *this );
  float c = max/maxval;
  *this *= c;
  return c;
}


double OutData::minmaximize( double max )
{
  float minval = 0.0;
  float maxval = 0.0;
  ::relacs::minMax( minval, maxval, *this );
  if ( -minval > maxval )
    maxval = -minval;
  float c = max/maxval;
  *this *= c;
  return c;
}


double OutData::fill( const OutData &am, double carrierfreq, double lowfreq,
		      const string &ident )
{
  clear();

  setBestSample( carrierfreq );
  setRangeBack( am.length() );

  if ( am.size() < 2 )
    return 1.0;

  double fac = 1.0;
  if ( carrierfreq > 1.0e-8 ) {
    // create lookup-table for one sine wave:
    SampleDataF sinbuf;
    sinbuf.sin( LinearRange( int( rint( 1.0 / ( carrierfreq * stepsize() ) ) ), 
			     0.0, stepsize() ), carrierfreq );
    // fill am with carrier sine wave:
    double slope = (am[1]-am[0])/am.stepsize();
    for ( int i=0, j=0, k=1; i<size(); i++, j++ ) {
      double t = pos( i );
      while ( am.pos( k ) < t && k+1 < am.size() ) {
	k++;
	slope = (am[k]-am[k-1])/am.stepsize();
      }
      if ( j >= sinbuf.size() ) 
	j=0;
      operator[]( i ) = sinbuf[j] * ( am[k-1] + slope * ( t - am.pos(k-1) ) );
    }
  }
  else {
    // create noise:
    SampleDataF noisebuf;
    noisebuf.whiteNoise( size(), stepsize(), lowfreq, -carrierfreq, rnd );
    // fill am with carrier noise:
    double slope = (am[1]-am[0])/am.stepsize();
    for ( int i=0, j=0, k=1; i<size() && j<noisebuf.size(); i++, j++ ) {
      double t = pos( i );
      while ( am.pos( k ) < t && k+1 < am.size() ) {
	k++;
	slope = (am[k]-am[k-1])/am.stepsize();
      }
      operator[]( i ) = noisebuf[j] * ( am[k-1] + slope * ( t - am.pos(k-1) ) );
    }
    fac = 0.3;
    *this *= fac;
    ::relacs::clip( -1.0, 1.0, *this );
    fac *= ::sqrt( 2.0 ); // for the 3 dB of the rms sine wave
  }
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus" );
  Description.newSection( am.Description );
  Description.insertText( "Function", "", "AM" );
  if ( carrierfreq > 1.0e-8 ) {
    Description.newSection( "", "stimulus/sine_wave" );
    Description.addNumber( "Frequency", carrierfreq, "Hz" );
  }
  else {
    Description.newSection( "", "stimulus/white_noise" );
    Description.addNumber( "UpperCutoffFrequency", -carrierfreq, "Hz" );
    Description.addNumber( "LowerCutoffFrequency", 0.0, "Hz" );
    Description.addNumber( "Mean", 0.0 );
    Description.addNumber( "StDev", 1.0 );
  }
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", am.length(), "s" );
  Description.addText( "Function", "Carrier" );

  setIdent( ident );
  clearError();
  return fac;
}


void OutData::constWave( double value, const string &name )
{
  SampleDataF::resize( 1, 0.0, minSampleInterval() );
  *this = value;
  Description.clear();
  Description.setType( "stimulus/value" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", stepsize(), "s" );
  Description.addNumber( "Intensity", value, unit() );
  clearError();
}


void OutData::constWave( double duration, double stepsize, double value, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  SampleDataF::resize( 0.0, duration, stepsize );
  *this = value;
  Description.clear();
  Description.setType( "stimulus/value" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Intensity", value, unit() );
  clearError();
}


void OutData::pulseWave( double duration, double stepsize,
			 double value, double base, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  SampleDataF::resize( 0.0, duration, stepsize );
  *this = value;
  Description.clear();
  Description.setType( "stimulus/pulse" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Intensity", value, unit() );
  if ( fabs( value - base ) > 1e-8 ) {
    back() = base;
    Description.addNumber( "IntensityOffset", base, unit() );
  }
  clearError();
}


void OutData::rectangleWave( double duration, double stepsize, 
			     double period, double width, double ramp, double ampl,
			     const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  rectangle( 0.0, duration, stepsize, period, width, ramp );
  if ( ampl != 1.0 )
    array() *= ampl;
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/square_wave" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "Frequency", 1.0/period, "Hz" );
  Description.addNumber( "DutyCycle", width/period );
  Description.addNumber( "StartAmplitude", 0.0 );

  setCarrierFreq( 1.0/period );
  clearError();
}


void OutData::sineWave( double duration, double stepsize,
			double freq, double phase, double ampl,
			double r, const string &name )
{
  if ( fixedSampleRate() )
    stepsize = minSampleInterval();
  else if ( stepsize < minSampleInterval()  )
    stepsize = bestSampleInterval( freq );
  sin( 0.0, duration, stepsize, freq, phase );
  if ( ampl != 1.0 )
    array() *= ampl;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/sine_wave" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "Frequency", freq, "Hz" );
  Description.addNumber( "Phase", phase );

  setCarrierFreq( freq );
  clearError();
}


void OutData::noiseWave( double duration, double stepsize,
			 double cutofffreq, double stdev,
			 unsigned long *seed, double r, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  Random rand;
  if ( seed != 0 )
    *seed = rand.setSeed( *seed );
  whiteNoise( duration, stepsize, 0.0, cutofffreq, rand );
  if ( stdev != 1.0 )
    array() *= stdev;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/white_noise" );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.setName( name );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Mean", 0.0, unit() );
  Description.addNumber( "StDev", stdev, unit() );
  Description.addNumber( "UpperCutoffFrequency", cutofffreq, "Hz" );
  Description.addNumber( "LowerCutoffFrequency", 0.0, "Hz" );

  setCarrierFreq( cutofffreq );
  clearError();
}


void OutData::bandNoiseWave( double duration, double stepsize,
			     double cutofffreqlow, double cutofffreqhigh, double stdev, 
			     unsigned long *seed, double r, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  Random rand;
  if ( seed != 0 )
    *seed = rand.setSeed( *seed );
  whiteNoise( duration, stepsize, 
	      cutofffreqlow, cutofffreqhigh, rand );
  if ( stdev != 1.0 )
    array() *= stdev;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/white_noise" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Mean", 0.0, unit() );
  Description.addNumber( "StDev", stdev, unit() );
  Description.addNumber( "UpperCutoffFrequency", cutofffreqhigh, "Hz" );
  Description.addNumber( "LowerCutoffFrequency", cutofffreqlow, "Hz" );

  setCarrierFreq( cutofffreqhigh );
  clearError();
}


void OutData::ouNoiseWave( double duration, double stepsize,
			   double tau, double stdev, unsigned long *seed,
			   double r, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  Random rand;
  if ( seed != 0 )
    *seed = rand.setSeed( *seed );
  ouNoise( duration, stepsize, tau, rand );
  if ( stdev != 1.0 )
    array() *= stdev;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/colored_noise" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Mean", 0.0, unit() );
  Description.addNumber( "StDev", stdev, unit() );
  Description.addNumber( "CorrelationTime", tau, "s" );

  setCarrierFreq( 1/tau );
  clearError();
}


void OutData::sweepWave( double duration, double stepsize, 
			 double startfreq, double endfreq,
			 double ampl, double r, 
			 const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  sweep( 0.0, duration, stepsize, startfreq, endfreq );
  if ( ampl != 1.0 )
    array() *= ampl;
  if ( r > 0.0 )
    ramp( r );
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/sweep_wave" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "StartFrequency", startfreq, "Hz" );
  Description.addNumber( "EndFrequency", endfreq, "Hz" );
  Description.addNumber( "Phase", 0.0 );

  clearError();
}


void OutData::dampedOscillationWave( double duration, double stepsize, double tau,
				     double freq, double phase, double ampl,
				     const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  dampedOscillation( 0.0, duration, stepsize, tau, freq, phase );
  double max = maxAbs( array() );
  array() *= ampl/max;
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/damped_oscillation" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "Tau", tau, "s" );
  Description.addNumber( "Frequency", freq, "Hz" );
  Description.addNumber( "Phase", phase );

  setCarrierFreq( freq );
  clearError();
}


void OutData::rampWave( double duration, double stepsize,
			double first, double last, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  SampleDataF::resize( 0.0, duration, stepsize );
  for ( int k=0; k<size(); k++ )
    (*this)[k] = first + (last-first)*(k+1)/size();
  Description.clear();
  Description.setType( "stimulus/ramp" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "StartIntensity", first, unit() );
  Description.addNumber( "Intensity", last, unit() );
  clearError();
}


void OutData::sawUpWave( double duration, double stepsize, 
			 double period, double ramp, double ampl, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  sawUp( 0.0, duration, stepsize, period, ramp );
  if ( ampl != 1.0 )
    array() *= ampl;
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/sawtooth" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "Period", period, "s" );
  Description.addNumber( "UpstrokeWidth", period-ramp, "s" );
  Description.addNumber( "DownstrokeWidth", ramp, "s" );
  Description.addNumber( "Ramp", ramp, "s" );

  clearError();
}


void OutData::sawDownWave( double duration, double stepsize, 
			   double period, double ramp, double ampl,
			   const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  sawDown( 0.0, duration, stepsize, period, ramp );
  if ( ampl != 1.0 )
    array() *= ampl;
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/sawtooth" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "UpstrokeWidth", ramp, "s" );
  Description.addNumber( "DownstrokeWidth", period-ramp, "s" );
  Description.addNumber( "Ramp", ramp, "s" );
  Description.addNumber( "Period", period, "s" );

  clearError();
}


void OutData::triangleWave( double duration, double stepsize, 
			    double period, double ampl, const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  triangle( 0.0, duration, stepsize, period );
  if ( ampl != 1.0 )
    array() *= ampl;
  back() = 0.0;

  Description.clear();
  Description.setType( "stimulus/sawtooth" );
  Description.setName( name );
  Description.addNumber( "StartTime", 0.0, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "UpstrokeWidth", 0.5*period, "s" );
  Description.addNumber( "DownstrokeWidth", 0.5*period, "s" );
  Description.addNumber( "Period", period, "s" );

  clearError();
}


void OutData::alphaWave( double &duration, double stepsize, 
			 double period, double tau, double ampl, double delay,
			 const string &name )
{
  if ( stepsize < minSampleInterval() || fixedSampleRate() )
    stepsize = minSampleInterval();
  double d = duration;
  if ( period < duration ) 
    d = ::floor( duration / period )*period + delay + 5.0*tau;
  SampleDataF::resize( 0.0, d, stepsize );
  *this = 0.0;
  for ( int k=0; k<indices( duration ); k += indices( period ) ) {
    SampleDataF a;
    a.alpha( 0.0, duration, stepsize, tau, delay );
    for ( int i=0; i<a.size() && k+i<size(); i++ )
      (*this)[k+i] += a[i];
  }
  array() *= ::exp( 1.0 ) * ampl;
  back() = 0.0;
  duration = length();

  Description.clear();
  Description.setType( "stimulus/alpha" );
  Description.setName( name );
  Description.addNumber( "StartTime", delay, "s" );
  Description.addNumber( "Duration", duration, "s" );
  Description.addNumber( "Amplitude", ampl, unit() );
  Description.addNumber( "TimeConstant", tau, "s" );
  if ( period < duration )
    Description.addNumber( "Period", period, "s" );

  clearError();
}


ostream &operator<<( ostream &str, const OutData &od )
{
  //  str << SampleDataF( od );
  str << "Offset: " << od.offset() << '\n';
  str << "Stepsize: " << od.stepsize() << '\n';
  str << "Size: " << od.size() << '\n';
  str << DaqError( od );
  str << "Delay: " << od.Delay << '\n';
  str << "StartSource: " << od.StartSource << '\n';
  str << "Priority: " << od.Priority << '\n';
  str << "Continuous: " << od.Continuous << '\n';
  str << "Restart: " << od.Restart << '\n';
  str << "MaxRate: " << od.MaxRate << '\n';
  str << "FixedRate: " << od.FixedRate << '\n';
  str << "Device: " << od.Device << '\n';
  str << "Channel: " << od.Channel << '\n';
  str << "Trace: " << od.Trace << '\n';
  str << "TraceName: " << od.TraceName << '\n';
  str << "Ident: " << od.Ident << '\n';
  str << "RequestMinValue: " << od.RequestMinValue << '\n';
  str << "RequestMaxValue: " << od.RequestMaxValue << '\n';
  //  str << "GainData: " << od.GainData << '\n';
  str << "Scale: " << od.Scale << '\n';
  str << "Unit: " << od.Unit << '\n';
  str << "MinVoltage: " << od.MinVoltage << '\n';
  str << "MaxVoltage: " << od.MaxVoltage << '\n';
  str << "Intensity: " << od.Intensity << '\n';
  str << "CarrierFreq: " << od.CarrierFreq << '\n';
  str << "Level: " << od.Level << '\n';
  str << "DeviceIndex: " << od.DeviceIndex << '\n';
  str << "DeviceDelay: " << od.DeviceDelay << '\n';
  str << "DeviceCount: " << od.DeviceCount << '\n';
  str << "Description: \n";
  if ( od.Description.empty() )
    str << "  Description empty\n";
  else
    od.Description.save( str, "  " );
  return str;
}


}; /* namespace relacs */

