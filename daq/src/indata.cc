/*
  indata.cc
  A cyclic buffer for data acquired from a data acquisition board.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#include <limits.h>
#include <sstream>
#include <relacs/outdata.h>
#include <relacs/indata.h>
using namespace std;

namespace relacs {


const string InData::RefStr[4] =
  { "differential", "common", "ground", "other" };
int InData::DefaultDevice = 0;


InData::InData( void )
  : BufferArrayF()
{
  construct();
}


InData::InData( int n, int m, double step )
  : BufferArrayF( n, m )
{
  construct();
  Stepsize = step;
}


InData::InData( const InData &data )
  : BufferArrayF( data )
{
  RestartIndex = data.RestartIndex;
  SignalIndex = data.SignalIndex;
  Stepsize = data.Stepsize;
  Delay = data.Delay;
  StartSource = data.StartSource;
  Priority = data.Priority;
  Continuous = data.Continuous;
  Device = data.Device;
  Channel = data.Channel;
  Trace = data.Trace;
  Ident = data.Ident;
  Reference = data.Reference;
  Dither = data.Dither;
  Unipolar = data.Unipolar;
  GainIndex = data.GainIndex;
  GainData = NULL;
  Scale = data.Scale;
  Unit = data.Unit;
  ReadTime = data.ReadTime;
  UpdateTime = data.UpdateTime;
  MinValue = data.MinValue;
  MaxValue = data.MaxValue;
  Mode = data.Mode;
  Source = data.Source;
}


InData::~InData( void )
{
  if ( GainData != NULL )
    delete [] GainData;
}


void InData::construct( void )
{
  RestartIndex = 0;
  SignalIndex = -1;
  Stepsize = 1.0;
  Delay = 0.0;
  StartSource = 0;
  Priority = false;
  Continuous = false;
  Device = DefaultDevice;
  Channel = 0;
  Trace = 0;
  Ident = "";
  Reference = RefDifferential;
  Dither = false;
  Unipolar = false;
  GainIndex = 0;
  GainData = NULL;
  Scale = 1.0;
  Unit = "V";
  ReadTime = 0.0;
  UpdateTime = 0.0;
  MinValue = -1.0;
  MaxValue = +1.0;
  Source = 0;
}


const InData &InData::operator=( const InData &data )
{
  if ( &data == this )
    return *this;

  BufferArrayF::assign( data );
  RestartIndex = data.RestartIndex;
  SignalIndex = data.SignalIndex;
  Stepsize = data.Stepsize;
  Delay = data.Delay;
  StartSource = data.StartSource;
  Priority = data.Priority;
  Continuous = data.Continuous;
  Device = data.Device;
  Channel = data.Channel;
  Trace = data.Trace;
  Ident = data.Ident;
  Reference = data.Reference;
  Dither = data.Dither;
  Unipolar = data.Unipolar;
  GainIndex = data.GainIndex;
  GainData = NULL;
  Scale = data.Scale;
  Unit = data.Unit;
  ReadTime = data.ReadTime;
  UpdateTime = data.UpdateTime;
  MinValue = data.MinValue;
  MaxValue = data.MaxValue;
  Source = data.Source;

  return *this;
}


void InData::copy( int first, int last, OutData &data ) const
{
  data.clear();
  if ( last - first <= 0 ||
       last > size() )
    return;
  data.resize( last - first );

  for ( int k=0; k<data.size(); k++ )
    data[ k ] = operator[]( first + k );

  data.setSampleRate( sampleRate() );
  data.setIdent( ident() );
}


void InData::copy( double tbegin, double tend, OutData &data ) const
{
  int first = index( tbegin );
  int last = index( tend );

  copy( first, last, data );
}


void InData::copy( double time, SampleDataF &trace ) const
{
  int inx = index( time + trace.rangeFront() );
  if ( fabs( sampleInterval() - trace.stepsize() ) < 1.0e-8 ) {
    for ( int k=0; k < trace.size(); k++ ) {
      if ( inx+k < size() )
	trace[k] = operator[]( inx+k );
      else {
	trace.resize( k );
	break;
      }
    }
  }
  else {
    for ( int k=0; k < trace.size(); k++ ) {
      double t = time + trace.pos( k );
      for ( ; inx < size() && pos( inx ) < t; inx++ );
      if ( inx < size() - 1 ) {
	double m = ( operator[]( inx+1 ) - operator[]( inx ) ) / sampleInterval();
	trace[k] = m * ( t - pos( inx ) ) + operator[]( inx );
      }
      else {
	trace.resize( k );
	break;
      }
    }
  }
}


void InData::copy( double time, SampleDataD &trace ) const
{
  int inx = index( time + trace.rangeFront() );
  if ( fabs( sampleInterval() - trace.stepsize() ) < 1.0e-8 ) {
    for ( int k=0; k < trace.size(); k++ ) {
      if ( inx+k < size() )
	trace[k] = operator[]( inx+k );
      else {
	trace.resize( k );
	break;
      }
    }
  }
  else {
    for ( int k=0; k < trace.size(); k++ ) {
      double t = time + trace.pos( k );
      for ( ; inx < size() && pos( inx ) < t; inx++ );
      if ( inx < size() - 1 ) {
	double m = ( operator[]( inx+1 ) - operator[]( inx ) ) / sampleInterval();
	trace[k] = m * ( t - pos( inx ) ) + operator[]( inx );
      }
      else {
	trace.resize( k );
	break;
      }
    }
  }
}


string InData::errorMessage( void ) const
{
  if ( success() )
    return "";

  ostringstream ss;
  ss << "Channel " << channel() 
     << " on device " << device()
     << ": " << errorStr();
  return ss.str();
}


double InData::pos( int i ) const
{
  return Stepsize*i;
}


double InData::interval( int indices ) const
{
  return Stepsize*indices;
}


int InData::index( double pos ) const
{
  return int( ::floor( pos/Stepsize + 1.0e-6 ) );
}


int InData::indices( double iv ) const
{
  return int( ::floor( iv/Stepsize + 1.0e-6 ) );
}


double InData::length( void ) const
{
  return pos( BufferArrayF::size() );
}


void InData::clearBuffer( void )
{
  resize( 0 );
  RestartIndex = 0;
  SignalIndex = -1;
}


int InData::currentIndex( void ) const
{
  return BufferArrayF::size();
}


double InData::currentTime( void ) const
{
  return pos( BufferArrayF::size() );
}


int InData::minIndex( void ) const
{
  return BufferArrayF::minIndex();
}


double InData::minTime( void ) const
{
  return pos( minIndex() );
}


int InData::maxIndex( void ) const
{
  return INT_MAX;
}


double InData::maxTime( void ) const
{
  return pos( maxIndex() );
}


int InData::signalIndex( void ) const
{
  return SignalIndex;
}


double InData::signalTime( void ) const
{
  return SignalIndex < 0 ? -1.0 : pos( SignalIndex );
}


void InData::setSignalIndex( int index )
{
  SignalIndex = index;
}


void InData::setSignalTime( double time )
{
  SignalIndex = index( time );
}


int InData::restartIndex( void ) const
{
  return RestartIndex;
}


double InData::restartTime( void ) const
{
  return pos( RestartIndex );
}


void InData::setRestart( void )
{
  RestartIndex = size();
}


double InData::minValue( void ) const
{
  return MinValue;
}


double InData::maxValue( void ) const
{
  return MaxValue;
}


void InData::setMinValue( double minv )
{
  MinValue = minv;
}


void InData::setMaxValue( double maxv )
{
  MaxValue = maxv;
}


double InData::voltage( int index ) const
{
  return operator[]( index ) / scale();
}


double InData::getVoltage( double val ) const
{
  return val/scale();
}


double InData::minVoltage( void ) const
{
  return MinValue/scale();
}


double InData::maxVoltage( void ) const
{
  return MaxValue/scale();
}


void InData::setMinVoltage( double minv )
{
  MinValue = minv*scale();
}


void InData::setMaxVoltage( double maxv )
{
  MaxValue = maxv*scale();
}


InData::const_iterator InData::begin( void ) const
{
  return const_iterator( *this, 0 );
}


InData::const_iterator InData::begin( double time ) const
{
  return const_iterator( *this, index( time ) );
}


InData::const_iterator InData::minBegin( void ) const
{
  return const_iterator( *this, minIndex() );
}


InData::const_iterator InData::end( void ) const
{
  return const_iterator( *this, size() );
}


InDataTimeIterator InData::timeBegin( void ) const
{
  return InDataTimeIterator( *this, 0 );
}


InDataTimeIterator InData::timeBegin( double time ) const
{
  return InDataTimeIterator( *this, index( time ) );
}


InDataTimeIterator InData::minTimeBegin( void ) const
{
  return InDataTimeIterator( *this, minIndex() );
}


InDataTimeIterator InData::timeEnd( void ) const
{
  return InDataTimeIterator( *this, size() );
}


double InData::sampleRate( void ) const
{
  return 1.0/Stepsize;
}


void InData::setSampleRate( double rate )
{
  if ( rate > 0.0 )
    Stepsize = 1.0/rate;
}


double InData::sampleInterval( void ) const
{
  return Stepsize;
}


void InData::setSampleInterval( double step )
{
  Stepsize = step;
}


double InData::stepsize( void ) const
{
  return Stepsize;
}


void InData::setStepsize( double step )
{
  Stepsize = step;
}


double InData::delay( void ) const 
{
  return Delay;
}


void InData::setDelay( double delay )
{
  Delay = delay >= 0.0 ? delay : 0.0;
}


int InData::startSource( void ) const
{
  return StartSource;
}


void InData::setStartSource( int startsource ) 
{
  StartSource = startsource;
}


bool InData::priority( void ) const 
{
  return Priority;
}


void InData::setPriority( const bool priority )
{
  Priority = priority;
}


bool InData::continuous( void ) const 
{
  return Continuous;
}


void InData::setContinuous( bool continuous )
{
  Continuous = continuous; 
}


int InData::device( void ) const
{
  return Device;
}


void InData::setDevice( int device )
{
  Device = device;
}


int InData::channel( void ) const
{
  return Channel;
}


void InData::setChannel( int channel )
{
  Channel = channel;
}


int InData::trace( void ) const
{
  return Trace;
}


void InData::setTrace( int trace )
{
  Trace = trace;
}


void InData::setChannel( int channel, int device )
{
  Channel = channel;
  Device = device;
}


string InData::ident( void ) const
{
  return Ident;
}


void InData::setIdent( const string &ident )
{
  Ident = ident;
}


InData::RefType InData::reference( void ) const
{
  return Reference;
}


string InData::referenceStr( void ) const
{
  return RefStr[Reference];
}


string InData::referenceStr( RefType ref )
{
  return RefStr[ref];
}


void InData::setReference( InData::RefType ref )
{
  Reference = ref;
}


void InData::setReference( const string &ref )
{
  for ( RefType i=RefDifferential; i<=RefOther; i=RefType(i+1) ) {
    if ( RefStr[i] == ref ) {
      Reference = i;
      break;
    }
  }
}


bool InData::dither( void ) const
{
  return Dither;
}


void InData::setDither( bool dither )
{
  Dither = dither;
}


bool InData::unipolar( void ) const
{
  return Unipolar;
}


void InData::setUnipolar( bool unipolar )
{
  Unipolar = unipolar;
}


int InData::gainIndex( void ) const
{
  return GainIndex;
}


void InData::setGainIndex( int gainindex )
{
  GainIndex = gainindex;
}


char *InData::gainData( void ) const
{
  return GainData;
}


void InData::setGainData( char *data )
{
  GainData = data;
}


double InData::scale( void ) const
{
  return Scale;
}


void InData::setScale( double scale )
{
  Scale = scale;
}


string InData::unit( void ) const
{
  return Unit;
}


void InData::setUnit( const string &unit )
{
  Unit = unit;
}


void InData::setUnit( double scale, const string &unit )
{
  Scale = scale;
  Unit = unit;
}


int InData::source( void ) const
{
  return Source;
}


void InData::setSource( int source )
{
  Source = source;
}


double InData::readTime( void ) const
{
  return ReadTime;
}


void InData::setReadTime( double time )
{
  ReadTime = time;
}


double InData::updateTime( void ) const
{
  return UpdateTime;
}


void InData::setUpdateTime( double time )
{
  UpdateTime = time;
}


int InData::mode( void ) const
{
  return Mode;
}


void InData::clearMode( void )
{
  Mode = 0;
}


void InData::setMode( int flags )
{
  Mode = flags; 
}


void InData::addMode( int flags )
{
  Mode |= flags;
}


void InData::delMode( int flags )
{
  Mode &= ~flags;
}


void InData::mean( double time, SampleDataD &md, double width ) const
{
  if ( width <= 0.0 )
    width = md.stepsize();
  long wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<md.size(); i++ ) {
    long from = indices( time + md.pos( i ) );
    long upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > size() )
      upto = size();

    // mean:
    double mean = 0.0;
    long n = 0;
    for ( long k=from; k<upto; k++ )
      mean += ( operator[]( k ) - mean ) / (++n);

    md[i] = mean;
  }
}


void InData::variance( double time, SampleDataD &vd, double width ) const
{
  if ( width <= 0.0 )
    width = vd.stepsize();
  long wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<vd.size(); i++ ) {
    long from = indices( time + vd.pos( i ) );
    long upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > size() )
      upto = size();

    // mean:
    double mean = 0.0;
    long n = 0;
    for ( long k=from; k<upto; k++ )
      mean += ( operator[]( k ) - mean ) / (++n);

    // mean squared diffference from mean:
    double var = 0.0;
    n = 0;
    for ( long k=from; k<upto; k++ ) {
      // subtract mean:
      double d = operator[]( k ) - mean;
      // average over squares:
      var += ( d*d - var ) / (++n);
    }

    // variance:
    vd[i] = var;
  }
}


void InData::stdev( double time, SampleDataD &sd, double width ) const
{
  if ( width <= 0.0 )
    width = sd.stepsize();
  long wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<sd.size(); i++ ) {
    long from = indices( time + sd.pos( i ) );
    long upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > size() )
      upto = size();

    // mean:
    double mean = 0.0;
    long n = 0;
    for ( long k=from; k<upto; k++ )
      mean += ( operator[]( k ) - mean ) / (++n);

    // mean squared diffference from mean:
    double var = 0.0;
    n = 0;
    for ( long k=from; k<upto; k++ ) {
      // subtract mean:
      double d = operator[]( k ) - mean;
      // average over squares:
      var += ( d*d - var ) / (++n);
    }

    // square root:
    sd[i] = sqrt( var );
  }
}


void InData::rms( double time, SampleDataD &rd, double width ) const
{
  if ( width <= 0.0 )
    width = rd.stepsize();
  long wi = indices( width );
  if ( wi <= 0 )
    wi = 1;

  for ( int i=0; i<rd.size(); i++ ) {
    long from = indices( time + rd.pos( i ) );
    long upto = from + wi;
    if  ( from < minIndex() )
      from = minIndex();
    if ( upto > size() )
      upto = size();

    // mean squared diffference from mean:
    double var = 0.0;
    long n = 0;
    for ( long k=from; k<upto; k++ ) {
      // subtract mean:
      double d = operator[]( k );
      // average over squares:
      var += ( d*d - var ) / (++n);
    }

    // square root:
    rd[i] = sqrt( var );
  }
}


ostream &operator<<( ostream &str, const InData &id )
{
  str << BufferArrayF( id );
  str << DaqError( id );
  str << "RestartIndex: " << id.RestartIndex << '\n';
  str << "SignalIndex: " << id.SignalIndex << '\n';
  str << "Stepsize: " << id.Stepsize << '\n';
  str << "Delay: " << id.Delay << '\n';
  str << "StartSource: " << id.StartSource << '\n';
  str << "Priority: " << id.Priority << '\n';
  str << "Continuous: " << id.Continuous << '\n';
  str << "Device: " << id.Device << '\n';
  str << "Channel: " << id.Channel << '\n';
  str << "Trace: " << id.Trace << '\n';
  str << "Ident: " << id.Ident << '\n';
  str << "Reference: " << id.Reference << '\n';
  str << "Dither: " << id.Dither << '\n';
  str << "Unipolar: " << id.Unipolar << '\n';
  str << "GainIndex: " << id.GainIndex << '\n';
  str << "GainData: " << (void*)id.GainData << '\n';
  str << "Scale: " << id.Scale << '\n';
  str << "Unit: " << id.Unit << '\n';
  str << "Source: " << id.Source << '\n';
  str << "ReadTime: " << id.ReadTime << '\n';
  str << "UpdateTime: " << id.UpdateTime << '\n';
  str << "MinValue: " << id.MinValue << '\n';
  str << "MaxValue: " << id.MaxValue << '\n';
  str << "Mode: " << id.Mode << '\n';
  return str;
}


InDataIterator &InDataIterator::operator=( const InDataIterator &p )
{
  if ( &p == this )
    return *this;

  Index = p.Index;
  ID = p.ID;

  return *this;
}


InDataDiffIterator &InDataDiffIterator::operator=( const InDataDiffIterator &p )
{
  if ( &p == this )
    return *this;

  Index = p.Index;
  ID = p.ID;
  DiffWidth = p.DiffWidth;

  return *this;
}


InDataDiffIterator &InDataDiffIterator::operator=( const InDataIterator &p )
{
  Index = p.Index;
  ID = p.ID;
  return *this;
}


InDataTimeIterator &InDataTimeIterator::operator=( const InDataTimeIterator &p )
{
  if ( &p == this )
    return *this;

  Index = p.Index;
  ID = p.ID;

  return *this;
}


InDataTimeIterator &InDataTimeIterator::operator=( const InDataIterator &p )
{
  Index = p.Index;
  ID = p.ID;
  return *this;
}


}; /* namespace relacs */

