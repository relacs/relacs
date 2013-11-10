/*
  eventdata.cc
  Class for event times that can also be an infinite ring-buffer.

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

#include <cmath>
#include <iomanip>
#include <relacs/sampledata.h>
#include <relacs/map.h>
#include <relacs/kernel.h>
#include <relacs/stats.h>
#include <relacs/eventdata.h>

namespace relacs {


const double EventData::MaxInterval = 1.0e12;


EventData::EventData( void )
  : Range( 0, -HUGE_VAL, 0.0001 )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  ED = 0;
  NWrite = 0;
  Cyclic = false;
  UseSizeBuffer = false;
  UseWidthBuffer = false;
  Mode = 0;
  Ident = "";
  SizeName = "size";
  SizeScale = 1.0;
  SizeUnit = "V";
  SizeFormat = "%g";
  WidthName = "width";
  WidthScale = 1.0;
  WidthUnit = "s";
  WidthFormat = "%g";
  Source = -1;
  MeanRatio = 0.03;
  SignalTime = -HUGE_VAL;
  clear();
}


EventData::EventData( int n, bool sizebuffer, bool widthbuffer )
  : Range( 0, -HUGE_VAL, 0.0001 )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  ED = 0;
  NWrite = 0;
  Cyclic = false;
  UseSizeBuffer = sizebuffer;
  UseWidthBuffer = widthbuffer;
  Mode = 0;
  Ident = "";
  SizeName = "size";
  SizeScale = 1.0;
  SizeUnit = "V";
  SizeFormat = "%g";
  WidthName = "width";
  WidthScale = 1.0;
  WidthUnit = "s";
  WidthFormat = "%g";
  Source = -1;
  MeanRatio = 0.03;
  SignalTime = -HUGE_VAL;
  clear();
  reserve( n );
}


EventData::EventData( int n, double tbegin, double tend, double stepsize,
		      bool sizebuffer, bool widthbuffer )
  : Range( tbegin, tend, stepsize )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  ED = 0;
  NWrite = 0;
  Cyclic = false;
  UseSizeBuffer = sizebuffer;
  UseWidthBuffer = widthbuffer;
  Mode = 0;
  Ident = "";
  SizeName = "size";
  SizeScale = 1.0;
  SizeUnit = "V";
  SizeFormat = "%g";
  WidthName = "width";
  WidthScale = 1.0;
  WidthUnit = "s";
  WidthFormat = "%g";
  Source = -1;
  MeanRatio = 0.03;
  SignalTime = -HUGE_VAL;
  clear();
  reserve( n );
}


EventData::EventData( const EventData &events )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  Cyclic = false;
  assign( events );
}


EventData::EventData( const EventData *events )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  Cyclic = false;
  assign( events );
}


EventData::EventData( const EventData &events, double tbegin, double tend )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  Cyclic = false;
  assign( events, tbegin, tend );
}


EventData::EventData( const EventData &events, double tbegin, double tend,
		      double tref )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  Cyclic = false;
  assign( events, tbegin, tend, tref );
}


EventData::EventData( const ArrayD &times, double tbegin,
		      double tend, double stepsize )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  Cyclic = false;
  assign( times, tbegin, tend, stepsize );
}


EventData::EventData( const ArrayD &times, const ArrayD &sizes, double tbegin,
		      double tend, double stepsize )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  Cyclic = false;
  assign( times, sizes, tbegin, tend, stepsize );
}


EventData::EventData( const ArrayD &times, const ArrayD &sizes, const ArrayD &widths,
		      double tbegin, double tend, double stepsize )
{
  Own = false;
  TimeBuffer = 0;
  SizeBuffer = 0;
  WidthBuffer = 0;
  NBuffer = 0;
  Cyclic = false;
  assign( times, sizes, widths, tbegin, tend, stepsize );
}


EventData::~EventData( void )
{
  if ( Own ) {
    if ( TimeBuffer != 0 )
      delete [] TimeBuffer;
    if ( SizeBuffer != 0 )
      delete [] SizeBuffer;
    if ( WidthBuffer != 0 )
      delete [] WidthBuffer;
  }
}


int EventData::size( void ) const
{
  return Index + R;
}


bool EventData::empty( void ) const
{
  return ( R <= 0 && ( Index <= 0 || !Cyclic ));
}


void EventData::resize( int nevents, double dflt )
{
  if ( nevents <= 0 ) {
    clear();
    return;
  }

  // non cyclic data?
  if ( !Cyclic ) {
    if ( nevents > NBuffer )
      reserve( nevents, dflt );
    R = nevents;
    Index = 0;
    Cycles = 0;
    MeanSize = 0.0;
    MeanWidth = 0.0;
    MeanInterval = MaxInterval;
    MeanQuality = 0.0;
  }

  ErrorMessage = "";
}


void EventData::clear( void )
{
  R = 0;
  Index = 0;
  Cycles = 0;
  MeanSize = 0.0;
  MeanWidth = 0.0;
  MeanInterval = MaxInterval;
  MeanQuality = 0.0;
  ErrorMessage = "";
}


int EventData::capacity( void ) const
{
  return NBuffer;
}


void EventData::reserve( int nevents, double dflt )
{
  if ( nevents > NBuffer ) {

    NBuffer = nevents;
    int onb = NBuffer;
    double *otb = TimeBuffer;
    double *osb = SizeBuffer;
    double *owb = WidthBuffer;

    TimeBuffer = new double[NBuffer];
    int k = 0;
    if ( otb != 0 ) {
      if ( Cyclic && Index > 0 )
	for ( int i=R; i<onb && k<NBuffer; i++, k++ )
	  TimeBuffer[k] = otb[i];
      for ( int i=0; i<R && k<NBuffer; i++, k++ )
	TimeBuffer[k] = otb[i];
    }
    for ( ; k<NBuffer; k++ )
      TimeBuffer[k] = dflt;

    if ( UseSizeBuffer ) {
      SizeBuffer = new double[NBuffer];
      k = 0;
      if ( osb != 0 ) {
	if ( Cyclic && Index > 0 )
	  for ( int i=R; i<onb && k<NBuffer; i++, k++ )
	    SizeBuffer[k] = osb[i];
	for ( int i=0; i<R && k<NBuffer; i++, k++ )
	  SizeBuffer[k] = osb[i];
      }
      for ( ; k<NBuffer; k++ )
	SizeBuffer[k] = 0.0;
    }

    if ( UseWidthBuffer ) {
      WidthBuffer = new double[NBuffer];
      k = 0;
      if ( owb != 0 ) {
	if ( Cyclic && Index > 0 )
	  for ( int i=R; i<onb && k<NBuffer; i++, k++ )
	    WidthBuffer[k] = osb[i];
	for ( int i=0; i<R && k<NBuffer; i++, k++ )
	  WidthBuffer[k] = osb[i];
      }
      for ( ; k<NBuffer; k++ )
	WidthBuffer[k] = 0.0;
    }

    if ( Index > 0 ) {
      Index += R;
      R = onb;
      if ( R > NBuffer )
	R = NBuffer;
    }
    else {
      if ( R > NBuffer )
	R = NBuffer;
      if ( ! Cyclic ) {
	Index = 0;
	Cycles = 0;
      }
    }

    if ( Own ) {
      if ( otb != 0 )
	delete [] otb;
      if ( osb != 0 )
	delete [] osb;
      if ( owb != 0 )
	delete [] owb;
    }
    Own = true;

  }

  ErrorMessage = "";
}


void EventData::free( int nevents, double dflt )
{
  if ( nevents != NBuffer ) {

    int onb = NBuffer;
    double *otb = TimeBuffer;
    double *osb = SizeBuffer;
    double *owb = WidthBuffer;

    NBuffer = nevents;
    if ( NBuffer > 0 ) {
      TimeBuffer = new double[NBuffer];
      int k = 0;
      if ( otb != 0 ) {
	if ( Cyclic && Index > 0 )
	  for ( int i=R; i<onb && k<NBuffer; i++, k++ )
	    TimeBuffer[k] = otb[i];
	for ( int i=0; i<R && k<NBuffer; i++, k++ )
	  TimeBuffer[k] = otb[i];
      }
      for ( ; k<NBuffer; k++ )
	TimeBuffer[k] = dflt;

      if ( UseSizeBuffer ) {
	SizeBuffer = new double[NBuffer];
	k = 0;
	if ( osb != 0 ) {
	  if ( Cyclic && Index > 0 )
	    for ( int i=R; i<onb && k<NBuffer; i++, k++ )
	      SizeBuffer[k] = osb[i];
	  for ( int i=0; i<R && k<NBuffer; i++, k++ )
	    SizeBuffer[k] = osb[i];
	}
	for ( ; k<NBuffer; k++ )
	  SizeBuffer[k] = 0.0;
      }

      if ( UseWidthBuffer ) {
	WidthBuffer = new double[NBuffer];
	k = 0;
	if ( owb != 0 ) {
	  if ( Cyclic && Index > 0 )
	    for ( int i=R; i<onb && k<NBuffer; i++, k++ )
	      WidthBuffer[k] = osb[i];
	  for ( int i=0; i<R && k<NBuffer; i++, k++ )
	    WidthBuffer[k] = osb[i];
	}
	for ( ; k<NBuffer; k++ )
	  WidthBuffer[k] = 0.0;
      }

      if ( Index > 0 ) {
	Index += R;
	R = onb;
	if ( R > NBuffer )
	  R = NBuffer;
      }
      else {
	if ( R > NBuffer )
	  R = NBuffer;
	if ( ! Cyclic ) {
	  Index = 0;
	  Cycles = 0;
	}
      }

    }
    else {
      NBuffer = 0;
      TimeBuffer = 0;
      SizeBuffer = 0;
      WidthBuffer = 0;
      clear();
    }

    if ( Own ) {
      if ( otb != 0 )
	delete [] otb;
      if ( osb != 0 )
	delete [] osb;
      if ( owb != 0 )
	delete [] owb;
    }

  }

  ErrorMessage = "";
}


int EventData::writeBufferCapacity( void ) const
{
  return NWrite;
}


void EventData::setWriteBufferCapacity( int m )
{
  NWrite = m;
  if ( NWrite > capacity() )
    NWrite = capacity();
}


bool EventData::cyclic( void ) const
{
  return Cyclic;
}


void EventData::setCyclic( bool cyclic )
{
  Cyclic = cyclic;
  if ( ! Cyclic )
    NWrite = 0;
}


bool EventData::sizeBuffer( void ) const
{
  return ( SizeBuffer != 0 );
}


bool EventData::useSizeBuffer( void ) const
{
  return UseSizeBuffer;
}


void EventData::setSizeBuffer( bool use )
{
  UseSizeBuffer = use;
}


bool EventData::widthBuffer( void ) const
{
  return ( WidthBuffer != 0 );
}


bool EventData::useWidthBuffer( void ) const
{
  return UseWidthBuffer;
}


void EventData::setWidthBuffer( bool use )
{
  UseWidthBuffer = use;
}


LinearRange &EventData::range( void )
{
  return Range;
}


const LinearRange &EventData::range( void ) const
{
  return Range;
}


double EventData::offset( void ) const
{
  return Range.offset();
}


void EventData::setOffset( double offset )
{
  Range.setOffset( offset );
}


double EventData::length( void ) const
{
  return Range.length();
}


void EventData::setLength( double duration )
{
  Range.setLength( duration );
}


double EventData::stepsize( void ) const
{
  return Range.stepsize();
}


void EventData::setStepsize( double stepsize )
{
  Range.setStepsize( stepsize );
}


double EventData::rangeFront( void ) const
{
  return Range.front();
}


void EventData::setRangeFront( double front )
{
  Range.setFront( front );
}


double EventData::rangeBack( void ) const
{
  return Range.back();
}


void EventData::setRangeBack( double back )
{
  Range.setBack( back );
}


double EventData::signalTime( void ) const
{
  return SignalTime;
}


void EventData::setSignalTime( double s )
{
  SignalTime = s;
}


EventData &EventData::operator=( const EventData &events )
{
  if ( &events != this )
    assign( events );

  return *this;
}


EventData &EventData::operator=( const ArrayD &times )
{
  assign( times );

  return *this;
}


void EventData::assign( const EventData &events )
{
  if ( &events == this )
    return;

  if ( ! Own ) {
    NBuffer = 0;
    TimeBuffer = 0;
    SizeBuffer = 0;
    WidthBuffer = 0;
    Own = true;
  }
  clear();

  UseSizeBuffer = events.UseSizeBuffer;
  UseWidthBuffer = events.UseWidthBuffer;
  int ns = events.capacity();
  reserve( ns );
  if ( NBuffer < ns )
    ns = NBuffer;

  for ( int k=0; k<ns; k++ )
    TimeBuffer[k] = events.TimeBuffer[k];
  if ( UseSizeBuffer ) {
    for ( int k=0; k<ns; k++ )
      SizeBuffer[k] = events.SizeBuffer[k];
  }
  if ( UseWidthBuffer ) {
    for ( int k=0; k<ns; k++ )
      WidthBuffer[k] = events.WidthBuffer[k];
  }

  ED = events.ED;
  NWrite = events.NWrite;
  Cyclic = events.Cyclic;
  R = events.R;
  Index = events.Index;
  Cycles = events.Cycles;
  Mode = events.Mode;
  Ident = events.Ident;
  SizeName = events.SizeName;
  SizeUnit = events.SizeUnit;
  SizeScale = events.SizeScale;
  SizeFormat = events.SizeFormat;
  WidthName = events.WidthName;
  WidthScale = events.WidthScale;
  WidthUnit = events.WidthUnit;
  WidthFormat = events.WidthFormat;
  MeanRatio = events.MeanRatio;
  MeanSize = events.MeanSize;
  MeanWidth = events.MeanWidth;
  MeanInterval = events.MeanInterval;
  MeanQuality = events.MeanQuality;
  Range = events.Range;
  Source = events.Source;
  SignalTime = events.SignalTime;
  ErrorMessage = "";
}


void EventData::assign( const EventData &events,
			double tbegin, double tend )
{
  int n = events.next( tbegin );
  int p = events.previous( tend );
  int ns = p - n + 1;
  if ( p < n || p < 0 || tend <= tbegin ) 
    ns = 0;

  if ( ! Own ) {
    NBuffer = 0;
    TimeBuffer = 0;
    SizeBuffer = 0;
    WidthBuffer = 0;
    Own = true;
  }
  clear();

  UseSizeBuffer = events.UseSizeBuffer;
  UseWidthBuffer = events.UseWidthBuffer;
  reserve( ns );

  if ( ns > 0 && NBuffer > 0 ) {

    if ( NBuffer < ns )
      ns = NBuffer;

    for ( int k=0; k<ns; k++ )
      TimeBuffer[k] = events[n+k] - tbegin;

    if ( UseSizeBuffer ) {
      for ( int k=0; k<ns; k++ )
	SizeBuffer[k] = events.eventSize( n + k );
    }

    if ( UseWidthBuffer ) {
      for ( int k=0; k<ns; k++ )
	WidthBuffer[k] = events.eventWidth( n + k );
    }

  }

  ED = 0;
  NWrite = 0;
  R = ns;
  Cyclic = false;
  Index = 0;
  Cycles = 0;
  Mode = events.Mode;
  Ident = events.Ident;
  SizeName = events.SizeName;
  SizeScale = events.SizeScale;
  SizeUnit = events.SizeUnit;
  SizeFormat = events.SizeFormat;
  WidthName = events.WidthName;
  WidthScale = events.WidthScale;
  WidthUnit = events.WidthUnit;
  WidthFormat = events.WidthFormat;

  MeanRatio = events.MeanRatio;
  if ( UseSizeBuffer ) {
    MeanSize = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanSize += ( SizeBuffer[k] - MeanSize ) / ( k+1 );
  }
  else
    MeanSize = events.MeanSize;
  if ( UseWidthBuffer ) {
    MeanWidth = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanWidth += ( WidthBuffer[k] - MeanWidth ) / ( k+1 );
  }
  else
    MeanWidth = events.MeanWidth;
  if ( ns > 1 )
    MeanInterval = ( TimeBuffer[ns-1] - TimeBuffer[0] ) / ( ns - 1 );
  else
    MeanInterval = MaxInterval;
  MeanQuality = events.MeanQuality;

  Range = events.Range;
  Range.setOffset( 0.0 );
  Range.setLength( tend - tbegin );
  Source = events.Source;
  SignalTime = events.SignalTime > -HUGE_VAL ? events.SignalTime - tbegin : -HUGE_VAL;
  ErrorMessage = "";
  Dummy = 0.0;
}


void EventData::assign( const EventData &events,
			double tbegin, double tend, double tref )
{
  int n = events.next( tbegin );
  int p = events.previous( tend );
  int ns = p - n + 1;
  if ( p < n || p < 0 ) 
    ns = 0;

  if ( ! Own ) {
    NBuffer = 0;
    TimeBuffer = 0;
    SizeBuffer = 0;
    WidthBuffer = 0;
    Own = true;
  }
  clear();

  UseSizeBuffer = events.UseSizeBuffer;
  UseWidthBuffer = events.UseWidthBuffer;
  reserve( ns );

  if ( ns > 0 && NBuffer > 0 ) {

    if ( NBuffer < ns )
      ns = NBuffer;

    for ( int k=0; k<ns; k++ )
      TimeBuffer[k] = events[n+k] - tref;

    if ( UseSizeBuffer ) {
      for ( int k=0; k<ns; k++ )
	SizeBuffer[k] = events.eventSize( n + k );
    }

    if ( UseWidthBuffer ) {
      for ( int k=0; k<ns; k++ )
	WidthBuffer[k] = events.eventWidth( n + k );
    }

  }

  ED = 0;
  NWrite = 0;
  R = ns;
  Cyclic = false;
  Index = 0;
  Cycles = 0;
  Mode = events.Mode;
  Ident = events.Ident;
  SizeName = events.SizeName;
  SizeScale = events.SizeScale;
  SizeUnit = events.SizeUnit;
  SizeFormat = events.SizeFormat;
  WidthName = events.WidthName;
  WidthScale = events.WidthScale;
  WidthUnit = events.WidthUnit;
  WidthFormat = events.WidthFormat;

  MeanRatio = events.MeanRatio;
  if ( UseSizeBuffer ) {
    MeanSize = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanSize += ( SizeBuffer[k] - MeanSize ) / ( k+1 );
  }
  else
    MeanSize = events.MeanSize;
  if ( UseWidthBuffer ) {
    MeanWidth = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanWidth += ( WidthBuffer[k] - MeanWidth ) / ( k+1 );
  }
  else
    MeanWidth = events.MeanWidth;
  if ( ns > 1 )
    MeanInterval = ( TimeBuffer[ns-1] - TimeBuffer[0] ) / ( ns - 1 );
  else
    MeanInterval = MaxInterval;
  MeanQuality = events.MeanQuality;

  Range = events.Range;
  Range.setOffset( tbegin - tref );
  Range.setLength( tend - tref );
  Source = events.Source;
  SignalTime = events.SignalTime > -HUGE_VAL ? events.SignalTime - tref : -HUGE_VAL;
  ErrorMessage = "";
  Dummy = 0.0;
}


void EventData::assign( const ArrayD &times, 
			double tbegin, double tend, double stepsize )
{
  if ( ! Own ) {
    NBuffer = 0;
    TimeBuffer = 0;
    SizeBuffer = 0;
    WidthBuffer = 0;
    Own = true;
  }
  clear();

  UseSizeBuffer = false;
  UseWidthBuffer = false;
  int ns = times.size();
  reserve( ns );
  if ( NBuffer < ns )
    ns = NBuffer;

  for ( int k=0; k<ns; k++ )
    TimeBuffer[k] = times[k];

  ED = 0;
  NWrite = 0;
  Cyclic = false;
  R = ns;
  Index = 0;
  Cycles = 0;
  Mode = 0;
  Ident = "";
  MeanSize = 0.0;
  MeanWidth = 0.0;
  MeanInterval = times.size() > 1 ? ( times.back() - times.front() ) / double( ns - 1 ) : MaxInterval;
  MeanQuality = 0.0;
  if ( tbegin == -HUGE_VAL && size() > 0 )
    tbegin = front();
  if ( tend == HUGE_VAL && size() > 0 )
    tend = back();
  Range = LinearRange( tbegin, tend, stepsize );
  Source = 0;
  SignalTime = -HUGE_VAL;
  ErrorMessage = "";
}


void EventData::assign( const ArrayD &times, const ArrayD &sizes, 
			double tbegin, double tend, double stepsize )
{
  if ( ! Own ) {
    NBuffer = 0;
    TimeBuffer = 0;
    SizeBuffer = 0;
    WidthBuffer = 0;
    Own = true;
  }
  clear();

  UseSizeBuffer = true;
  UseWidthBuffer = false;
  int ns = times.size();
  reserve( ns );
  if ( NBuffer < ns )
    ns = NBuffer;

  for ( int k=0; k<ns; k++ ) {
    TimeBuffer[k] = times[k];
    SizeBuffer[k] = sizes[k];
  }

  ED = 0;
  NWrite = 0;
  Cyclic = false;
  R = ns;
  Index = 0;
  Cycles = 0;
  Mode = 0;
  Ident = "";
  MeanSize = mean( SizeBuffer, SizeBuffer+ns );
  MeanWidth = 0.0;
  MeanInterval = times.size() > 1 ? ( times.back() - times.front() ) / double( ns - 1 ) : MaxInterval;
  MeanQuality = 0.0;
  if ( tbegin == -HUGE_VAL && size() > 0 )
    tbegin = front();
  if ( tend == HUGE_VAL && size() > 0 )
    tend = back();
  Range = LinearRange( tbegin, tend, stepsize );
  Source = 0;
  SignalTime = -HUGE_VAL;
  ErrorMessage = "";
}


void EventData::assign( const ArrayD &times, const ArrayD &sizes, const ArrayD &widths, 
			double tbegin, double tend, double stepsize )
{
  if ( ! Own ) {
    NBuffer = 0;
    TimeBuffer = 0;
    SizeBuffer = 0;
    WidthBuffer = 0;
    Own = true;
  }
  clear();

  UseSizeBuffer = true;
  UseWidthBuffer = true;
  int ns = times.size();
  reserve( ns );
  if ( NBuffer < ns )
    ns = NBuffer;

  for ( int k=0; k<ns; k++ ) {
    TimeBuffer[k] = times[k];
    SizeBuffer[k] = sizes[k];
    WidthBuffer[k] = widths[k];
  }

  ED = 0;
  NWrite = 0;
  Cyclic = false;
  R = ns;
  Index = 0;
  Cycles = 0;
  Mode = 0;
  Ident = "";
  MeanSize = mean( SizeBuffer, SizeBuffer+ns );
  MeanWidth = mean( WidthBuffer, WidthBuffer+ns );
  MeanInterval = times.size() > 1 ? ( times.back() - times.front() ) / double( ns - 1 ) : MaxInterval;
  MeanQuality = 0.0;
  if ( tbegin == -HUGE_VAL && size() > 0 )
    tbegin = front();
  if ( tend == HUGE_VAL && size() > 0 )
    tend = back();
  Range = LinearRange( tbegin, tend, stepsize );
  Source = 0;
  SignalTime = -HUGE_VAL;
  ErrorMessage = "";
}


const EventData &EventData::assign( const EventData *events )
{
  if ( events == this )
    return *this;

  if ( Own ) {
    if ( TimeBuffer != 0 )
      delete [] TimeBuffer;
    if ( SizeBuffer != 0 )
      delete [] SizeBuffer;
    if ( WidthBuffer != 0 )
      delete [] WidthBuffer;
  }
  Own = false;
  NBuffer = events->NBuffer;
  TimeBuffer = events->TimeBuffer;
  UseSizeBuffer = events->UseSizeBuffer;
  SizeBuffer = events->SizeBuffer;
  UseWidthBuffer = events->UseWidthBuffer;
  WidthBuffer = events->WidthBuffer;
  ED = events;
  NWrite = events->NWrite;
  Cyclic = events->Cyclic;
  R = events->R;
  Index = events->Index;
  Cycles = events->Cycles;
  Mode = events->Mode;
  Ident = events->Ident;
  SizeName = events->SizeName;
  SizeUnit = events->SizeUnit;
  SizeScale = events->SizeScale;
  SizeFormat = events->SizeFormat;
  WidthName = events->WidthName;
  WidthScale = events->WidthScale;
  WidthUnit = events->WidthUnit;
  WidthFormat = events->WidthFormat;
  MeanRatio = events->MeanRatio;
  MeanSize = events->MeanSize;
  MeanWidth = events->MeanWidth;
  MeanInterval = events->MeanInterval;
  MeanQuality = events->MeanQuality;
  Range = events->Range;
  Source = events->Source;
  SignalTime = events->SignalTime;
  ErrorMessage = "";
  return *this;
}


const EventData &EventData::assign( void )
{
  return assign( ED );
}


void EventData::update( void )
{
  if ( ED != 0 ) {
    R = ED->R;
    Index = ED->Index;
    Cycles = ED->Cycles;
    Mode = ED->Mode;
    MeanRatio = ED->MeanRatio;
    MeanSize = ED->MeanSize;
    MeanWidth = ED->MeanWidth;
    MeanInterval = ED->MeanInterval;
    MeanQuality = ED->MeanQuality;
    Range = ED->Range;
    SignalTime = ED->SignalTime;
  }
}


void EventData::append( const EventData &events, double tbegin, double tend )
{
  int n = events.next( tbegin );
  int p = events.previous( tend );
  int ns = p - n + 1;
  if ( p < n || p < 0 ) 
    ns = 0;

  int nn = size();
  if ( ! Cyclic )
    reserve( nn + ns );

  // nothing was assigend yet:
  if ( Range.offset() == -HUGE_VAL ) {
    Index = 0;
    Cycles = 0;
    R = 0;
    Mode = events.Mode;
    Ident = events.Ident;
    SizeName = events.SizeName;
    SizeScale = events.SizeScale;
    SizeUnit = events.SizeUnit;
    SizeFormat = events.SizeFormat;
    WidthName = events.WidthName;
    WidthScale = events.WidthScale;
    WidthUnit = events.WidthUnit;
    WidthFormat = events.WidthFormat;
  }

  if ( ns > 0 && NBuffer > 0 ) {
    for ( int k=0; k<ns; k++ ) {
      if ( R >= NBuffer ) {
	if ( Cyclic ) {
	  // set indices for new cycle:
	  R = 0;
	  Index += NBuffer;
	  Cycles++;
	}
	else {
	  ns = k;
	  break;
	}
      }
      TimeBuffer[R] = events[n+k];
      if ( UseSizeBuffer )
	SizeBuffer[R] = events.eventSize( n + k );
      if ( UseWidthBuffer )
	WidthBuffer[R] = events.eventWidth( n + k );
      R++;
    }
  }

  MeanRatio = events.MeanRatio;
  if ( UseSizeBuffer ) {
    MeanSize = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanSize += ( SizeBuffer[nn+k] - MeanSize ) / ( k+1 );
  }
  else
    MeanSize = events.MeanSize;
  if ( UseWidthBuffer ) {
    MeanWidth = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanWidth += ( WidthBuffer[nn+k] - MeanWidth ) / ( k+1 );
  }
  else
    MeanWidth = events.MeanWidth;
  if ( ns > 1 )
    MeanInterval = ( TimeBuffer[nn+ns-1] - TimeBuffer[nn] ) / ( ns - 1 );
  else
    MeanInterval = MaxInterval;
  MeanQuality = events.MeanQuality;

  if ( Range.offset() == -HUGE_VAL ) {
    Range = events.Range;
    Range.setOffset( tbegin );
  }
  Range.setLength( tend );
  SignalTime = events.SignalTime > -HUGE_VAL ? events.SignalTime : -HUGE_VAL;
  ErrorMessage = "";
  Dummy = 0.0;
}


void EventData::append( const EventData &events, double tbegin, 
			double tend, double tref )
{
  int n = events.next( tbegin );
  int p = events.previous( tend );
  int ns = p - n + 1;
  if ( p < n || p < 0 ) 
    ns = 0;

  int nn = size();
  if ( ! Cyclic )
    reserve( nn + ns );

  // nothing was assigend yet:
  if ( Range.offset() == -HUGE_VAL ) {
    Index = 0;
    Cycles = 0;
    R = 0;
    Mode = events.Mode;
    Ident = events.Ident;
    SizeName = events.SizeName;
    SizeScale = events.SizeScale;
    SizeUnit = events.SizeUnit;
    SizeFormat = events.SizeFormat;
    WidthName = events.WidthName;
    WidthScale = events.WidthScale;
    WidthUnit = events.WidthUnit;
    WidthFormat = events.WidthFormat;
  }

  if ( ns > 0 && NBuffer > 0 ) {
    for ( int k=0; k<ns; k++ ) {
      if ( R >= NBuffer ) {
	if ( Cyclic ) {
	  // set indices for new cycle:
	  R = 0;
	  Index += NBuffer;
	  Cycles++;
	}
	else {
	  ns = k;
	  break;
	}
      }
      TimeBuffer[R] = events[n+k] - tref;
      if ( UseSizeBuffer )
	SizeBuffer[R] = events.eventSize( n + k );
      if ( UseWidthBuffer )
	WidthBuffer[R] = events.eventWidth( n + k );
      R++;
    }
  }

  MeanRatio = events.MeanRatio;
  if ( UseSizeBuffer ) {
    MeanSize = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanSize += ( SizeBuffer[nn+k] - MeanSize ) / ( k+1 );
  }
  else
    MeanSize = events.MeanSize;
  if ( UseWidthBuffer ) {
    MeanWidth = 0.0;
    for ( int k=0; k<ns; k++ )
      MeanWidth += ( WidthBuffer[nn+k] - MeanWidth ) / ( k+1 );
  }
  else
    MeanWidth = events.MeanWidth;
  if ( ns > 1 )
    MeanInterval = ( TimeBuffer[nn+ns-1] - TimeBuffer[nn] ) / ( ns - 1 );
  else
    MeanInterval = MaxInterval;
  MeanQuality = events.MeanQuality;

  if ( Range.offset() == -HUGE_VAL ) {
    Range = events.Range;
    Range.setOffset( tbegin - tref );
  }
  Range.setLength( tend - tref );
  SignalTime = events.SignalTime > -HUGE_VAL ? events.SignalTime - tref : -HUGE_VAL;
  ErrorMessage = "";
  Dummy = 0.0;
}


void EventData::copy( double tbegin, double tend,
		      double *events, int &nevents ) const
{
  copy( tbegin, tend, tbegin, events, nevents );
}


void EventData::copy( double tbegin, double tend, double tref, 
		      double *events, int &nevents ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  int oldn = nevents;
  nevents = 0;
  if ( p < n || p < 0 )
    return;
  for ( int k=n; k <= p && nevents < oldn; k++ ) {
    events[nevents] = (*this)[k] - tref;
    nevents++;
  }
}


void EventData::copy( double tbegin, double tend,
		      vector<double> &events ) const
{
  copy( tbegin, tend, tbegin, events );
}


void EventData::copy( double tbegin, double tend, double tref, 
		      vector<double> &events ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  events.clear();
  if ( p < n || p < 0 )
    return;
  events.reserve( p-n+1 );
  for ( int k=n; k<=p; k++ )
    events.push_back( (*this)[k] - tref );
}


void EventData::copy( double tbegin, double tend, ArrayD &events ) const
{
  copy( tbegin, tend, tbegin, events );
}


void EventData::copy( double tbegin, double tend, double tref, 
		      ArrayD &events ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  events.clear();
  if ( p < n || p < 0 )
    return;
  events.reserve( p-n+1 );
  for ( int k=n; k<=p; k++ )
    events.push( (*this)[k] - tref );
}


void EventData::copy( double tbegin, double tend, MapD &events ) const
{
  copy( tbegin, tend, tbegin, events );
}


void EventData::copy( double tbegin, double tend, double tref, 
		      MapD &events ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  events.clear();
  if ( p < n || p < 0 )
    return;
  events.reserve( p-n+1 );
  if ( sizeBuffer() ) {
    for ( int k=n; k<=p; k++ )
      events.push( (*this)[k] - tref, eventSize( k ) );
  }
  else if ( widthBuffer() ) {
    for ( int k=n; k<=p; k++ )
      events.push( (*this)[k] - tref, eventWidth( k ) );
  }
  else {
    for ( int k=n; k<=p; k++ )
      events.push( (*this)[k] - tref, 0 );
  }
}


void EventData::copy( double tbegin, double tend, EventData &events ) const
{
  copy( tbegin, tend, tbegin, events );
}


void EventData::copy( double tbegin, double tend, double tref, 
		      EventData &events ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  events.clear();
  events.setCyclic( false );
  if ( p < n || p < 0 )
    return;
  events.reserve( p-n+1 );
  for ( int k=n; k<=p; k++ )
    events.push( (*this)[k] - tref, eventSize( k ), eventWidth( k ) );
  events.Range = LinearRange( tbegin - tref, tend - tref, stepsize() );
  events.SignalTime = -HUGE_VAL;
}


double EventData::front( void ) const
{
  return operator[]( minEvent() );
}


double &EventData::front( void )
{
  return operator[]( minEvent() );
}


double EventData::front( int n ) const
{
  return operator[]( minEvent() + n );
}


double &EventData::front( int n )
{
  return operator[]( minEvent() + n );
}


double EventData::back( void ) const
{
  return operator[]( size()-1 );
}


double &EventData::back( void )
{
  return operator[]( size()-1 );
}


double EventData::back( int n ) const
{
  return operator[]( size()-1-n );
}


double &EventData::back( int n )
{
  return operator[]( size()-1-n );
}


EventData::const_iterator EventData::begin( void ) const
{
  return EventData::const_iterator( *this, 0 );
}


EventData::const_iterator EventData::begin( double time ) const
{
  return EventData::const_iterator( *this, next( time ) );
}


EventData::const_iterator EventData::minBegin( void ) const
{
  return EventData::const_iterator( *this, minEvent() );
}


EventData::const_iterator EventData::end( void ) const
{
  return EventData::const_iterator( *this, size() );
}


double EventData::frontSize( void ) const
{
  return eventSize( minEvent() );
}


double &EventData::frontSize( void )
{
  return eventSize( minEvent() );
}


double EventData::frontSize( int n ) const
{
  return eventSize( minEvent() + n );
}


double &EventData::frontSize( int n )
{
  return eventSize( minEvent() + n );
}


double EventData::backSize( void ) const
{
  return eventSize( size()-1 );
}


double &EventData::backSize( void )
{
  return eventSize( size()-1 );
}


double EventData::backSize( int n ) const
{
  return eventSize( size()-1-n );
}


double &EventData::backSize( int n )
{
  return eventSize( size()-1-n );
}


double EventData::frontWidth( void ) const
{
  return eventWidth( minEvent() );
}


double &EventData::frontWidth( void )
{
  return eventWidth( minEvent() );
}


double EventData::frontWidth( int n ) const
{
  return eventWidth( minEvent() + n );
}


double &EventData::frontWidth( int n )
{
  return eventWidth( minEvent() + n );
}


double EventData::backWidth( void ) const
{
  return eventWidth( size()-1 );
}


double &EventData::backWidth( void )
{
  return eventWidth( size()-1 );
}


double EventData::backWidth( int n ) const
{
  return eventWidth( size()-1-n );
}


double &EventData::backWidth( int n )
{
  return eventWidth( size()-1-n );
}


void EventData::push( double time, double esize, double ewidth )
{
  double interv = MaxInterval;
  if ( size() > 0 ) {
#ifndef NDEBUG
    if ( time < back() )
      cerr << "! warning in EventData('" << Ident << "'):: push() -> time " << time << " < back() " << back() << '\n';
    else if ( fabs( time - back() ) < 1.0e-8 )
      cerr << "! warning in EventData('" << Ident << "'):: push() -> time " << time << " == back() " << back() << '\n';
#endif
    interv = time - back();
  }
  else if ( Range.offset() == -HUGE_VAL ) {
    Range.setOffset( time );
  }
  else if ( Range.front() > time ) {
    Range.setFront( time );
  }

#ifndef NDEBUG
  if ( R < 0 || R > NBuffer ) {
    cerr << "! fatal error in EventData('" << Ident << "')::push() -> R out of range" << '\n' 
	 << *this << '\n';
    assert( R >= 0 || R <= NBuffer );
  }
#endif

  // new cycle?
  if ( R >= NBuffer ) {
    if ( Cyclic ) {
      // set indices for new cycle:
      R = 0;
      Index += NBuffer;
      Cycles++;
    }
    else {
      int n = NBuffer >= 10 ? 3*NBuffer/2 : 10;
      reserve( n );
    }
  }

  // add event:
  TimeBuffer[R] = time;
  if ( SizeBuffer != 0 )
    SizeBuffer[R] = esize;
  if ( WidthBuffer != 0 )
    WidthBuffer[R] = ewidth;

  // update index:
  R++;

  // update mean size, width, interval and quality:
  MeanSize = MeanSize * (1.0 - MeanRatio ) + esize * MeanRatio;
  MeanWidth = MeanWidth * (1.0 - MeanRatio ) + ewidth * MeanRatio;
  MeanInterval = MeanInterval * (1.0 - MeanRatio ) + interv * MeanRatio;
  MeanQuality = MeanQuality * (1.0 - MeanRatio ) + MeanRatio;

  // update range:
  if ( Range.back() < time )
    Range.setBack( time );
}


void EventData::push( const ArrayD &time, double esize, double ewidth )
{
  if ( time.empty() )
    return;

  double interv = MaxInterval;
  if ( size() > 0 ) {
#ifndef NDEBUG
    if ( time.front() < back() )
      cerr << "! warning in EventData('" << Ident << "'):: push( ArrayD ) -> time[0] " << time.front() << " < back() " << back() << '\n';
    else if ( fabs( time.front() - back() ) < 1.0e-8 )
      cerr << "! warning in EventData('" << Ident << "'):: push( ArrayD ) -> time[0] " << time.front() << " == back() " << back() << '\n';
#endif
    interv = time.front() - back();
  }
  else if ( Range.offset() == -HUGE_VAL ) {
    Range.setOffset( time.front() );
  }
  else if ( Range.front() > time.front() ) {
    Range.setFront( time.front() );
  }

#ifndef NDEBUG
  if ( R < 0 || R > NBuffer ) {
    cerr << "! fatal error in EventData('" << Ident << "')::push( ArrayD ) -> R out of range" << '\n' 
	 << *this << '\n';
    assert( R >= 0 || R <= NBuffer );
  }
#endif

  // reserve memory:
  if ( ! Cyclic && R + time.size() > NBuffer ) {
    reserve( R + time.size() );
  }


  for ( int k=0; k<time.size(); k++ ) {

    // new cycle?
    if ( Cyclic && R >= NBuffer ) {
      // set indices for new cycle:
      R = 0;
      Index += NBuffer;
      Cycles++;
    }

    // add event:
    TimeBuffer[R] = time[k];
    if ( SizeBuffer != 0 )
      SizeBuffer[R] = esize;
    if ( WidthBuffer != 0 )
      WidthBuffer[R] = ewidth;
    
    // update index:
    R++;

    if ( k > 0.0 )
      interv = time[k] - time[k-1];

    // update mean size, width, interval and quality:
    MeanSize = MeanSize * (1.0 - MeanRatio ) + esize * MeanRatio;
    MeanWidth = MeanWidth * (1.0 - MeanRatio ) + ewidth * MeanRatio;
    MeanInterval = MeanInterval * (1.0 - MeanRatio ) + interv * MeanRatio;
    MeanQuality = MeanQuality * (1.0 - MeanRatio ) + MeanRatio;

  }

  // update range:
  if ( Range.back() < back() )
    Range.setBack( back() );

}


void EventData::insert( double time, double size, double width )
{
  if ( cyclic() )
    return;

  int n = next( time );

  if ( n >= R ) {
    push( time, size, width );
  }
  else {
    reserve( R+1 );
    // shift events and insert event:
    for ( int k=R; k>n; k-- )
      TimeBuffer[k] = TimeBuffer[k-1];
    TimeBuffer[n] = time;
    if ( SizeBuffer != 0 ) {
      for ( int k=R; k>n; k-- )
	SizeBuffer[k] = SizeBuffer[k-1];
      SizeBuffer[n] = size;
    }
    if ( WidthBuffer != 0 ) {
      for ( int k=R; k>n; k-- )
	WidthBuffer[k] = WidthBuffer[k-1];
      WidthBuffer[n] = width;
    }
    // update index:
    R++;
  }
  if ( Range.offset() == -HUGE_VAL )
      Range.setOffset( time );
  else if ( Range.front() > time )
      Range.setFront( time );
}


void EventData::insert( const EventData &e )
{
  if ( Cyclic ) {
    cerr << "warning: EventData::insert( const EventData &e ) does not support cyclic mode.\n";
    return;
  }

  // enlarge range:
  if ( e.offset() < offset() )
    setOffset( e.offset() );
  if ( e.stepsize() < stepsize() )
    setStepsize( e.stepsize() );
  if ( e.rangeBack() > rangeBack() )
    setRangeBack( e.rangeBack() );

  // size:
  int k1 = size()-1;
  int k2 = e.size()-1;
  resize( size() + e.size() );

  // merge:
  for ( int k=size()-1; k>=0; k-- ) {
    if ( k1 >= 0 && k2 >=0 ) {
      if ( operator[]( k1 ) > e[k2] )
	TimeBuffer[k] = operator[]( k1-- );
      else
	TimeBuffer[k] = e[ k2-- ];
    }
    else if ( k2 >= 0 )
      TimeBuffer[k] = e[ k2-- ];
    else
      break;
  }
}


void EventData::erase( int index )
{
  if ( cyclic() )
    return;

  if ( index < 0 || index >= R )
    return;

  if ( index == R-1 ) {
    R--;
  }
  else {
    // shift events:
    for ( int k=index+1; k<R; k++ )
      TimeBuffer[k-1] = TimeBuffer[k];
    if ( SizeBuffer != 0 ) {
      for ( int k=index+1; k<R; k++ )
	SizeBuffer[k-1] = SizeBuffer[k];
    }
    if ( WidthBuffer != 0 ) {
      for ( int k=index+1; k<R; k++ )
	WidthBuffer[k-1] = WidthBuffer[k];
    }
    // update index:
    R--;
  }
}


void EventData::erase( const_iterator iter )
{
  erase( iter - begin() );
}


void EventData::pop( void )
{
  if ( size() > 0 ) {
    if ( R <= 0 ) {
      R = NBuffer;
      Index -= NBuffer;
      Cycles--;
    }
    R--;
    if ( R <= 0 ) {
      R = NBuffer;
      Index -= NBuffer;
      Cycles--;
    }
  }
}


void EventData::sum( const EventData &e, EventData &all ) const
{
  // init all:
  double offs = offset() < e.offset() ? offset() : e.offset();
  double step = stepsize() < e.stepsize() ? stepsize() : e.stepsize();
  double back = rangeBack() > e.rangeBack() ? rangeBack() : e.rangeBack();
  all.clear();
  all.setOffset( offs );
  all.setStepsize( step );
  all.setRangeBack( back );
  all.reserve( size() + e.size() );

  // merge:
  for ( int k1=0, k2=0; ; ) {
    if ( k1<size() && k2<e.size() ) {
      if ( operator[]( k1 ) < e[k2] )
	all.push( operator[]( k1++ ) );
      else
	all.push( e[k2++] );
    }
    else if ( k1 < size() )
      all.push( operator[]( k1++ ) );
    else if ( k2 < e.size() )
      all.push( e[k2++] );
    else
      break;
  }
}


void EventData::sync( const EventData &e, EventData &s, double bin ) const
{
  // init s:
  double offs = offset() < e.offset() ? offset() : e.offset();
  double step = stepsize() < e.stepsize() ? stepsize() : e.stepsize();
  double rback = rangeBack() > e.rangeBack() ? rangeBack() : e.rangeBack();
  int n = size() < e.size() ? size() : e.size();
  s.clear();
  s.setOffset( offs );
  s.setStepsize( step );
  s.setRangeBack( rback );
  s.reserve( n );

  if ( empty() )
    return;

  double tstart = offs;
  double tend = rback;
  if ( size() > 0 ) {
    if ( front() < tstart )
      tstart = front();
    if ( back() > tend )
      tend = back();
  }
  if ( e.size() > 0 ) {
    if ( e.front() < tstart )
      tstart = e.front();
    if ( e.back() > tend )
      tend = e.back();
  }

  // init iterators:
  EventData::const_iterator k1 = begin();
  EventData::const_iterator k2 = e.begin();

  // select synchronous events:
  double t0 = ::floor( tstart/bin + 1.0e-6 ) * bin;
  double t1 = t0;
  for ( int i=1; t1<=tend; i++ ) {
    double t2=t0+i*bin;
    int c = 0;
    while ( k1 < end() && *k1 < t1 )
      ++k1;
    if ( k1 < end() && *k1 < t2 )
      ++c;
    while ( k2 < e.end() && *k2 < t1 )
      ++k2;
    if ( k2 < e.end() && *k2 < t2 )
      ++c;
    if ( c >= 2 )
      s.push( t1 );
    t1 = t2;
  }

}


const EventData &EventData::operator+=( double x )
{
  int m = R;
  if ( Cyclic )
    m = NBuffer;

  for ( int k=0; k<m; k++ )
    TimeBuffer[k] += x;

  Range += x;
  if ( SignalTime > -HUGE_VAL )
    SignalTime += x;

  return *this;
}


const EventData &EventData::operator-=( double x )
{
  int m = R;
  if ( Cyclic )
    m = NBuffer;

  for ( int k=0; k<m; k++ )
    TimeBuffer[k] -= x;

  Range -= x;
  if ( SignalTime > -HUGE_VAL )
    SignalTime -= x;

  return *this;
}


const EventData &EventData::operator*=( double x )
{
  int m = R;
  if ( Cyclic )
    m = NBuffer;

  for ( int k=0; k<m; k++ )
    TimeBuffer[k] *= x;

  Range *= x;
  if ( SignalTime > -HUGE_VAL )
    SignalTime *= x;

  return *this;
}


const EventData &EventData::operator/=( double x )
{
  int m = R;
  if ( Cyclic )
    m = NBuffer;

  for ( int k=0; k<m; k++ )
    TimeBuffer[k] /= x;

  Range /= x;
  if ( SignalTime > -HUGE_VAL )
    SignalTime /= x;

  return *this;
}


bool EventData::check( void ) const
{
  bool r = false;

  ErrorMessage = "";

  if ( NBuffer < 0 )
    ErrorMessage = "NBuffer < 0";
  else if ( R < 0 )
    ErrorMessage = "R < 0";
  else if ( R > NBuffer )
    ErrorMessage = "R > NBuffer";
  else
    r = true;

  return r;
}


ostream &operator<< ( ostream &str, const EventData &events )
{
  str << "         Ident: " << events.Ident << '\n'
      << "       NBuffer: " << events.NBuffer << '\n'
      << "           Own: " << events.Own << '\n'
      << " UseSizeBuffer: " << events.UseSizeBuffer << '\n'
      << "UseWidthBuffer: " << events.UseWidthBuffer << '\n'
      << "    SizeBuffer: " << ( events.SizeBuffer != 0 ) << '\n'
      << "   WidthBuffer: " << ( events.WidthBuffer != 0 ) << '\n';
  if ( events.ED == 0 )
    str << "            ED: none\n";
  else
    str << "   ED->ident(): " << events.ED->ident() << '\n';
  str << "        NWrite: " << events.NWrite << '\n'
      << "        Cyclic: " << events.Cyclic << '\n'
      << "             R: " << events.R << '\n'
      << "         Index: " << events.Index << '\n'
      << "        Cycles: " << events.Cycles << '\n'
      << "    SignalTime: " << events.SignalTime << '\n'
      << "        Source: " << events.Source << '\n'
      << "      MeanSize: " << events.MeanSize << '\n'
      << "     MeanWidth: " << events.MeanWidth << '\n'
      << "  MeanInterval: " << events.MeanInterval << '\n'
      << "   MeanQuality: " << events.MeanQuality << '\n'
      << "      SizeName: " << events.SizeName << '\n'
      << "     SizeScale: " << events.SizeScale << '\n'
      << "      SizeUnit: " << events.SizeUnit << '\n'
      << "    SizeFormat: " << events.SizeFormat << '\n'
      << "     WidthName: " << events.WidthName << '\n'
      << "    WidthScale: " << events.WidthScale << '\n'
      << "     WidthUnit: " << events.WidthUnit << '\n'
      << "   WidthFormat: " << events.WidthFormat << '\n'
      << "          Mode: " << events.Mode << '\n'
      << "  ErrorMessage: " << events.ErrorMessage << '\n';
  return str;
}


string EventData::message( void ) const
{
  return ErrorMessage;
}


int EventData::currentEvent( void ) const
{
  return Index + R;
}


int EventData::minEvent( void ) const
{
  int n = Index + R - NBuffer + NWrite;
  return !Cyclic || n < 0 ? 0 : n;
}


double EventData::minTime( void ) const
{
  return operator[]( minEvent() );
}


int EventData::mode( void ) const
{
  return Mode;
}


void EventData::setMode( int mode )
{
  Mode = mode;
}


int EventData::source( void ) const
{
  return Source;
}


void EventData::setSource( int source )
{
  Source = source;
}


const string &EventData::ident( void ) const
{
  return Ident;
}


void EventData::setIdent( const string &ident )
{
  Ident = ident;
}


const string &EventData::sizeName( void ) const
{
  return SizeName;
}


void EventData::setSizeName( const string &name )
{
  SizeName = name;
}


double EventData::sizeScale( void ) const
{
  return SizeScale;
}


void EventData::setSizeScale( double scale )
{
  SizeScale = scale;
}


const string &EventData::sizeUnit( void ) const
{
  return SizeUnit;
}


void EventData::setSizeUnit( const string &unit )
{
  SizeUnit = unit;
}


const string &EventData::sizeFormat( void ) const
{
  return SizeFormat;
}


void EventData::setSizeFormat( const string &format )
{
  SizeFormat = format;
}


double EventData::minSize( double tbegin, double tend ) const
{
  if ( SizeBuffer == 0 )
    return MeanSize;

  int n = next( tbegin );
  int p = previous( tend );
  if ( n > p || p < 0 || tend <= tbegin )
    return MeanSize;

  double min = eventSize( n );
  for ( int k = n+1; k <= p; k++ )
    if ( min > eventSize( k ) )
      min = eventSize( k );
  return min;
}


double EventData::maxSize( double tbegin, double tend ) const
{
  if ( SizeBuffer == 0 )
    return MeanSize;

  int n = next( tbegin );
  int p = previous( tend );
  if ( n > p || p < 0 || tend <= tbegin )
    return MeanSize;

  double max = eventSize( n );
  for ( int k = n+1; k <= p; k++ )
    if ( max < eventSize( k ) )
      max = eventSize( k );
  return max;
}


void EventData::minMaxSize( double tbegin, double tend,
			    double &min, double &max ) const
{
  min = MeanSize;
  max = MeanSize;

  if ( SizeBuffer == 0 )
    return;

  int n = next( tbegin );
  int p = previous( tend );
  if ( n > p || p < 0 || tend <= tbegin )
    return;

  min = eventSize( n );
  max = min;
  for ( int k = n+1; k <= p; k++ )
    if ( max < eventSize( k ) )
      max = eventSize( k );
    else if ( min > eventSize( k ) )
      min = eventSize( k );
}


double EventData::meanSize( double tbegin, double tend, 
			    double &stdev ) const
{
  stdev = 0.0;
  if ( SizeBuffer == 0 )
    return MeanSize;

  int n = next( tbegin );
  int p = previous( tend );
  double ms = 0.0;
  for ( int k=n; k<=p; k++ )
    ms += ( eventSize( k ) - ms ) / ( k-n+1 );
  double var = 0.0;
  for ( int k=n; k<=p; k++ ) {
    double s = eventSize( k ) - ms;
    var += ( s*s - var ) / ( k-n+1 );
  }
  stdev = sqrt( var );
  return ms;
}


double EventData::meanSize( double tbegin, double tend ) const
{
  if ( SizeBuffer == 0 )
    return MeanSize;

  int n = next( tbegin );
  int p = previous( tend );
  double ms = 0.0;
  for ( int k=n; k<=p; k++ )
    ms += ( eventSize( k ) - ms ) / ( k-n+1 );
  return ms;
}


void EventData::updateMeanSize( int n, double size )
{
  for ( int k=0; k<n; k++ )
    MeanSize = MeanSize * (1.0 - MeanRatio ) + size * MeanRatio;
}


void EventData::sizeHist( double tbegin, double tend, SampleDataD &hist ) const
{
  hist = 0.0;

  int n = next( tbegin );
  int p = previous( tend );
  if ( p < n || p < 0 || tend <= tbegin )
    return;

  double l = hist.rangeFront();
  double s = hist.stepsize();

  for ( int k=n; k <= p; k++ ) {
    int b = (int)rint( ( eventSize( k ) - l ) / s );
    if ( b >= 0 && b < hist.size() )
      hist[b] += 1.0;
  }
}


const string &EventData::widthName( void ) const
{
  return WidthName;
}


void EventData::setWidthName( const string &name )
{
  WidthName = name;
}


double EventData::widthScale( void ) const
{
  return WidthScale;
}


void EventData::setWidthScale( double scale )
{
  WidthScale = scale;
}


const string &EventData::widthUnit( void ) const
{
  return WidthUnit;
}


void EventData::setWidthUnit( const string &unit )
{
  WidthUnit = unit;
}


const string &EventData::widthFormat( void ) const
{
  return WidthFormat;
}


void EventData::setWidthFormat( const string &format )
{
  WidthFormat = format;
}


double EventData::minWidth( double tbegin, double tend ) const
{
  if ( WidthBuffer == 0 )
    return MeanWidth;

  int n = next( tbegin );
  int p = previous( tend );
  if ( n > p || p < 0 || tend <= tbegin )
    return MeanWidth;

  double min = eventWidth( n );
  for ( int k = n+1; k <= p; k++ )
    if ( min > eventWidth( k ) )
      min = eventWidth( k );
  return min;
}


double EventData::maxWidth( double tbegin, double tend ) const
{
  if ( WidthBuffer == 0 )
    return MeanWidth;

  int n = next( tbegin );
  int p = previous( tend );
  if ( n > p || p < 0 || tend <= tbegin )
    return MeanWidth;

  double max = eventWidth( n );
  for ( int k = n+1; k <= p; k++ )
    if ( max < eventWidth( k ) )
      max = eventWidth( k );
  return max;
}


void EventData::minMaxWidth( double tbegin, double tend,
			    double &min, double &max ) const
{
  min = MeanWidth;
  max = MeanWidth;

  if ( WidthBuffer == 0 )
    return;

  int n = next( tbegin );
  int p = previous( tend );
  if ( n > p || p < 0 || tend <= tbegin )
    return;

  min = eventWidth( n );
  max = min;
  for ( int k = n+1; k <= p; k++ )
    if ( max < eventWidth( k ) )
      max = eventWidth( k );
    else if ( min > eventWidth( k ) )
      min = eventWidth( k );
}


double EventData::meanWidth( double tbegin, double tend ) const
{
  if ( WidthBuffer == 0 )
    return MeanWidth;

  int n = next( tbegin );
  int p = previous( tend );
  double ms = 0.0;
  for ( int k=n; k<=p; k++ )
    ms += ( eventWidth( k ) - ms ) / ( k-n+1 );
  return ms;
}


void EventData::updateMeanWidth( int n, double width )
{
  for ( int k=0; k<n; k++ )
    MeanWidth = MeanWidth * (1.0 - MeanRatio ) + width * MeanRatio;
}


void EventData::widthHist( double tbegin, double tend, SampleDataD &hist ) const
{
  hist = 0.0;

  int n = next( tbegin );
  int p = previous( tend );
  if ( p < n || p < 0 || tend <= tbegin )
    return;

  double l = hist.rangeFront();
  double s = hist.stepsize();

  for ( int k=n; k <= p; k++ ) {
    int b = (int)rint( ( eventWidth( k ) - l ) / s );
    if ( b >= 0  && b < hist.size() )
      hist[b] += 1.0;
  }
}


double EventData::meanRate( void ) const
{
  return MeanInterval > 0.0 ? 1.0/MeanInterval : 0.0;
}


void EventData::setMeanRate( double meanrate )
{
  MeanInterval = meanrate > 0.0 ? 1.0/meanrate : MaxInterval;
}


void EventData::updateMeanRate( int n, double rate )
{
  double interv = rate > 0.0 ? 1.0/rate : MaxInterval;
  for ( int k=0; k<n; k++ )
    MeanInterval = MeanInterval * (1.0 - MeanRatio ) + interv * MeanRatio;
}


void EventData::updateMean( int n, double size, 
			    double width, double rate )
{ 
  double interv = rate > 0.0 ? 1.0/rate : MaxInterval;
  for ( int k=0; k<n; k++ ) {
    MeanSize = MeanSize * (1.0 - MeanRatio ) + size * MeanRatio;
    MeanWidth = MeanWidth * (1.0 - MeanRatio ) + width * MeanRatio;
    MeanInterval = MeanInterval * (1.0 - MeanRatio ) + interv * MeanRatio; 
  }
};


double EventData::meanQuality( void ) const
{
  return MeanQuality;
}


void EventData::setMeanQuality( double meanquality )
{
  MeanQuality = meanquality;
}


void EventData::updateMeanQuality( bool good )
   
{
  MeanQuality = MeanQuality * (1.0 - MeanRatio ) + ( good ? MeanRatio : 0.0 );
}


double EventData::meanRatio( void ) const
{
  return MeanRatio;
}


void EventData::setMeanRatio( double ratio )
   
{
  if ( ratio > 1.0 )
    MeanRatio = 1.0;
  else if ( ratio <= 0.0 )
    MeanRatio = 1.0e-8;
  else MeanRatio = ratio;
}


int EventData::next( double time ) const
{
  int l = 0;
  int r = 0;

  if ( !Cyclic || Index == 0 || R == NBuffer ||
       Index-1 < minEvent() || TimeBuffer[NBuffer-1] < time ) {
    // bisect in lower readable part of buffer:
    int n = R - NBuffer + NWrite;
    l = n < 0 ? 0 : n;
    r = R-1;
  }
  else {
    // bisect in upper part of buffer:
    l = R + NWrite;
    r = NBuffer-1;
    if ( r < l )
      return Index+R;
  }

  // there is no next event?
  if ( r < l || TimeBuffer[r] < time ) {
#ifndef NDEBUG
    if ( Index+R-1 >= minEvent() && operator[]( Index+R-1 ) >= time )
      cerr << "! warning in next( " << ident() << " ): time=" << time
	   << " [R-1]=" << operator[]( Index+R-1 )
	   << " [r]=" << operator[]( Index+r )
	   << " Cyclic=" << Cyclic << " Index=" << Index
	   << " R=" << R << " NBuffer=" << NBuffer
	   << " l=" << l << " r=" << r << '\n';
    //    assert( Index+R-1 < minEvent() || operator[]( Index+R-1 ) < time);
#endif
    return Index+R;
  }
  // first event is already next event?
  else if ( TimeBuffer[l] >= time ) {
    if ( l >= R )
      l -= NBuffer;
#ifndef NDEBUG
    if ( ( Index+l>=minEvent() && Index+l<size() && operator[]( Index+l ) < time ) || ( Index+l-1>=minEvent() && Index+l-1<size() && operator[]( Index+l-1 ) >= time ) )
      cerr << "! warning in next( " << ident() << " ): time=" << time
	   << " l=" << operator[]( Index+l )
	   << " l-1=" << operator[]( Index+l-1 ) << '\n';
#endif
    return Index+l;
  }

  // bisect:
  while ( r-l > 1 ) {
    int h = (l+r)/2;
    if ( TimeBuffer[h] < time )
      l = h;
    else
      r = h;
  }

  if ( r >= R )
    r -= NBuffer;
#ifndef NDEBUG
  if ( ( Index+r>=minEvent() && Index+r<size() && operator[]( Index+r ) < time ) || ( Index+r-1>=minEvent() && Index+r-1<size() && operator[]( Index+r-1 ) >= time ) )
    cerr << "! warning in next( " << ident() << " ): time=" << time
	 << " r=" << operator[]( Index+r )
	 << " r-1=" << operator[]( Index+r-1 ) << '\n';
#endif
  return Index+r;
}


double EventData::nextTime( double time, double dflt ) const
{
  int n = next( time );
  if ( n >= 0 && n < size() )
    return operator[]( n );
  else
    return dflt;
}


int EventData::previous( double time ) const
{
  int l = 0;
  int r = 0;

  if ( !Cyclic || Index == 0 || R == NBuffer ||
       Index < minEvent() || TimeBuffer[0] < time ) {
    // bisect in lower readable part of buffer:
    int n = R - NBuffer + NWrite;
    l = n < 0 ? 0 : n;
    r = R-1;
  }
  else {
    // bisect in upper part of buffer:
    l = R + NWrite;
    r = NBuffer-1;
    if ( r < l )
      return -1;
  }

  // there is no previous event?
  if ( r < l || TimeBuffer[l] > time ) {
#ifndef NDEBUG
    if ( size() > 0 && operator[]( minEvent() ) <= time )
      cerr << "! warning in previous( " << ident() << " ): time=" << time
	   << " minEvent=" << operator[]( minEvent() ) << '\n';
#endif
    return -1;
  }
  // right event is already previous event?
  else if ( TimeBuffer[r] <= time ) {
    if ( r >= R )
      r -= NBuffer;
#ifndef NDEBUG
    if ( ( Index+r>=minEvent() && Index+r<size() && operator[]( Index+r ) > time ) || ( Index+r+1>=minEvent() && Index+r+1<size() && operator[]( Index+r+1 ) <= time ) )
      cerr << "! warning in previous( " << ident() << " ): time=" << time
	   << " r=" << operator[]( Index+r )
	   << " r+1=" << operator[]( Index+r+1 ) << '\n';
#endif
    return Index+r;
  }

  // bisect:
  while ( r-l > 1 ) {
    int h = (l+r)/2;
    if ( TimeBuffer[h] <= time )
      l = h;
    else
      r = h;
  }
  
  if ( l >= R )
    l -= NBuffer;
#ifndef NDEBUG
  if ( ( Index+l>=minEvent() && operator[]( Index+l ) > time ) ||
       ( Index+l+1>=minEvent() && Index+l+1<size() && operator[]( Index+l+1 ) <= time ) )
    cerr << "! warning in previous( " << ident() << " ): time=" << time
	 << " l=" << operator[]( Index+l )
	 << " l+1=" << operator[]( Index+l+1 ) << '\n';
#endif
  return Index+l;
}


double EventData::previousTime( double time, double dflt ) const
{
  int p = previous( time );
  if ( p >= 0 && p < size() )
    return operator[]( p );
  else
    return dflt;
}


bool EventData::within( double time, double distance ) const
{
  int n = next( time );
  if ( n < size() && 
       operator[]( n ) - time < distance )
    return true;

  int p = previous( time );
  if ( p >= 0 && 
       time - operator[]( p ) < distance )
    return true;

  return false;
}


int EventData::count( double tbegin, double tend ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( p < n || p < 0 || tend <= tbegin )
    return 0;
  else
    return p - n + 1;
}


int EventData::count( double time ) const
{
  return size() - next( time );
}


double EventData::rate( double tbegin, double tend ) const
{
  int n = next( tbegin );
  int p = previous( tend );
  
  if ( p < n || p < 0 || tend <= tbegin )
    return 0.0;
  else
    return (p-n+1)/(tend-tbegin);
}


double EventData::rate( int n ) const
{
  if ( n > 0 && size() > n )
    return double( n ) / ( back() - back(n) );

  return 0.0;
} 


double EventData::rate( double time ) const
{
  double T = Range.back() - time;
  if ( T >= 0.0 )
    return ( size() - next( time ) )/T;

  return 0.0;
} 


void EventData::rate( SampleDataD &rate, double width, double time ) const
{
  rate = 0.0;
  int trials = 0;
  addRate( rate, trials, width, time );
}


void EventData::addRate( SampleDataD &rate, int &trials, double width,
			 double time ) const
{
  trials++;

  if ( width <= 0.0 )
    width = rate.stepsize();

  double tt = time + rate.pos( 0 );
  int kl = next( tt );
  tt += width;
  int kr = previous( tt );
  if ( kr > 0 && (*this)[kr] >= tt )
    kr--;

  for ( int i=0; i<rate.size(); i++ ) {

    double left = time + rate.pos( i );
    for ( ; kl < size() && (*this)[kl] < left; kl++ );

    double right = left + width;
    for ( ; kr+1 < size() && (*this)[kr+1] < right; kr++ );

    double r = 0.0;
    if ( kl < size() && (*this)[kl] < right &&
	 kr >= minEvent() && (*this)[kr] >= left )
      r = (kr-kl+1)/width;

    rate[i] += ( r - rate[i] )/trials;

  }

}


void EventData::cyclicRate( SampleDataD &rate, double width,
			    double time ) const
{
  rate = 0.0;
  int trials = 0;
  addCyclicRate( rate, trials, width, time );
}


void EventData::addCyclicRate( SampleDataD &rate, int &trials, double width,
			       double time ) const
{
  trials++;

  if ( width <= 0.0 )
    width = rate.stepsize();
  if ( width > rate.length() )
    width = rate.length();

  double lmarg = time + rate.rangeFront();
  int klm = next( lmarg );
  double rmarg = time + rate.rangeBack();

  int kl = next( rmarg );
  double left = lmarg;
  int kr = previous( lmarg + width );
  if ( kr > 0 && (*this)[kr] >= lmarg + width )
    kr--;
  double right = rmarg;

  for ( int i=0; i<rate.size(); i++ ) {

    double pl = left;
    left = time + rate.pos( i );
    if ( left < lmarg )
      left += rate.length();
    if ( left < pl )
      kl = klm;
    for ( ; kl < size() && (*this)[kl] < left; kl++ );

    double pr = right;
    double right = time + rate.pos( i ) + width;
    if ( right > rmarg )
      right -= rate.length();
    if ( right < pr )
      kr = klm-1;
    for ( ; kr+1 < size() && (*this)[kr+1] < right; kr++ );

    double r = 0.0;
    if ( left >= right ) {
      int n = 0;
      for ( int k = kl; k < size() && (*this)[k] < rmarg; k++, n++ );
      for ( int k = kr; k >= minEvent() && (*this)[k] >= lmarg; k--, n++ );
      r = n/width;
    }
    else if ( kl < size() && (*this)[kl] < right &&
	      kr >= minEvent() && (*this)[kr] >= left )
      r = (kr-kl+1)/width;

    rate[i] += ( r - rate[i] )/trials;

  }

}


void EventData::rate( SampleDataD &rate, const Kernel &kernel,
		      double time ) const
{
  rate = 0.0;
  double offs = time + rate.pos( 0 );
  int n = next( offs );
  int p = previous( offs + rate.length() );
  for ( int k=n; k<=p; k++ ) {
    int bin = rate.index( (*this)[k] - time );
    double dt = (*this)[k] - time - rate.pos( bin );
    for ( int i = rate.indices( kernel.left() ); 
	  i<rate.indices( kernel.right() );
	  i++ ) {
      int inx = bin+i;
      if ( inx >= 0 && inx < rate.size() )
	rate[inx] += kernel.value( rate.interval( i ) + dt );
    }
  }
}


void EventData::addRate( SampleDataD &rate, int &trials, const Kernel &kernel,
			 double time ) const
{
  ArrayD rr( rate.size(), 0.0 );

  double offs = time + rate.pos( 0 );
  int n = next( offs );
  int p = previous( offs + rate.length() );
  for ( int k=n; k<=p; k++ ) {
    int bin = rate.index( (*this)[k] - time );
    double dt = (*this)[k] - time - rate.pos( bin );
    for ( int i = rate.indices( kernel.left() ); 
	  i<rate.indices( kernel.right() );
	  i++ ) {
      int inx = bin+i;
      if ( inx >= 0 && inx < rr.size() )
	rr[inx] += kernel.value( rate.interval( i ) + dt );
    }
  }

  trials++;
  for ( int k=0; k<rate.size(); k++ )
    rate[k] += ( rr[k] - rate[k] )/trials;
}


void EventData::cyclicRate( SampleDataD &rate, const Kernel &kernel,
			    double time ) const
{
  rate = 0.0;
  int trials = 0;
  addCyclicRate( rate, trials, kernel, time );
}


void EventData::addCyclicRate( SampleDataD &rate, int &trials,
			       const Kernel &kernel, double time ) const
{
  double rr[ rate.size() ];
  for ( int k=0; k<rate.size(); k++ )
    rr[k] = 0.0;

  int n = next( time + rate.rangeFront() );
  int p = previous( time + rate.rangeBack() );
  for ( int k=n; k<=p; k++ ) {
    int bin = rate.index( (*this)[k] - time );
    double dt = (*this)[k] - time - rate.pos( bin );
    for ( int i = rate.indices( kernel.left() ); 
	  i<rate.indices( kernel.right() );
	  i++ ) {
      int inx = bin+i;
      while ( inx < 0 )
	inx += rate.size();
      while ( inx >= rate.size() )
	inx -= rate.size();
      rr[inx] += kernel.value( rate.interval( i ) + dt );
    }
  }

  trials++;
  for ( int k=0; k<rate.size(); k++ )
    rate[k] += ( rr[k] - rate[k] )/trials;
}


double EventData::interval( double tbegin, double tend, 
			    double *sd ) const
{
  int n = next( tbegin );
  int p = previous( tend );
  
  if ( p <= n || p < 0 || tend <= tbegin ) {
    if ( sd != 0 )
      *sd = 0.0;
    return 0.0;
  }
  else {
    double meanisi = ( (*this)[p] - (*this)[n] )/(p-n);
    if ( sd != 0 ) {
      double var = 0.0;
      double ss;
      int nisi=0;
      for ( int k=n+1; k<=p; k++ ) {
	ss = ( (*this)[k] - (*this)[k-1] ) - meanisi;
	var += ( ss*ss - var )/(++nisi);
      }
      *sd = sqrt( var );
    }
    return meanisi;
  }
}


double EventData::interval( double time, double *sd ) const
{
  return interval( time, Range.back(), sd );
}


double EventData::interval( int n, double *sd ) const
{
  if ( n < 1 || n > size() ) {
    if ( sd != 0 )
      *sd = 0.0;
    return 0.0;
  }

  return interval( back( n ), back(), sd );
}


double EventData::intervalAt( double time ) const
{
  int n = next( time );

  if ( n < Index+R && n > 0 )
    return (*this)[n] - (*this)[n-1];

  return 0.0;
}


void EventData::interval( SampleDataD &intervals, double time ) const
{
  intervals = 0.0;
  int trials = 0;
  addInterval( intervals, trials, time );
}


void EventData::addInterval( SampleDataD &intervals, int &trials,
			     double time ) const
{
  trials++;
  int k = next( time + intervals.rangeFront() );
  for ( int i=0; i<intervals.size(); i++ ) {
    for ( ; k<size() && (*this)[k] < time + intervals.pos( i ); k++ );
    double T = 0.0;
    if ( k<size() && k > minEvent() )
      T = (*this)[k] - (*this)[k-1];
    intervals[i] += ( T - intervals[i] )/trials;
  }
}


void EventData::cyclicInterval( SampleDataD &intervals, double time ) const
{
  intervals = 0.0;
  int trials = 0;
  addCyclicInterval( intervals, trials, time );
}


void EventData::addCyclicInterval( SampleDataD &intervals, int &trials,
				   double time ) const
{
  double lmarg = time + intervals.rangeFront();
  int kl = next( lmarg );
  double rmarg = time + intervals.rangeBack();
  int kr = previous( rmarg );

  trials++;
  int k = kl;
  for ( int i=0; i<intervals.size(); i++ ) {
    for ( ; k<size() && (*this)[k] < time + intervals.pos( i ); k++ );
    double T = intervals.length();
    if ( k>=size() || (*this)[k] > rmarg ) {
      if ( k > minEvent() && (*this)[k-1] >= lmarg ) {
	T = rmarg - (*this)[k-1] + (*this)[kl] - lmarg;
      }
    }
    else if ( k-1 < minEvent() || (*this)[k-1] < lmarg ) {
      if ( k < size() && (*this)[k] <= rmarg ) {
	T = (*this)[k] - lmarg + rmarg - (*this)[kr];
      }
    }
    else if ( k<size() && k > minEvent() )
      T = (*this)[k] - (*this)[k-1];
    intervals[i] += ( T - intervals[i] )/trials;
  }
}


int EventData::intervals( double tbegin, double tend, ArrayD &intervals ) const
{
  intervals.clear();
  return addIntervals( tbegin, tend, intervals );
}


int EventData::addIntervals( double tbegin, double tend, ArrayD &intervals ) const
{
  int n = next( tbegin );
  int p = previous( tend );
  
  if ( p <= n || p < 0 || tend <= tbegin )
    return 0;

  intervals.reserve( intervals.size() + p-n );

  for ( int k=n+1; k<=p; k++ )
    intervals.push( (*this)[k]-(*this)[k-1] );

  return p-n;
}


int EventData::intervals( double tbegin, double tend,
			  MapD &intrvls, int pos ) const
{
  intrvls.clear();
  return addIntervals( tbegin, tend, intrvls, pos );
}


int EventData::addIntervals( double tbegin, double tend,
			     MapD &intrvls, int pos ) const
{
  int n = next( tbegin );
  int p = previous( tend );
  
  if ( p <= n || p < 0 || tend <= tbegin )
    return 0;

  intrvls.reserve( intrvls.size() + p-n );

  if ( pos < 0 ) {
    for ( int k=n+1; k<=p; k++ )
      intrvls.push( (*this)[k-1], (*this)[k]-(*this)[k-1] );
  }
  else if ( pos > 0 ) {
    for ( int k=n+1; k<=p; k++ )
      intrvls.push( (*this)[k], (*this)[k]-(*this)[k-1] );
  }
  else {
    for ( int k=n+1; k<=p; k++ )
      intrvls.push( 0.5*((*this)[k-1]+(*this)[k]), (*this)[k]-(*this)[k-1] );
  }

  return p-n;
}


int EventData::saveIntervals( double tbegin, double tend, ostream &os,
			      int pos, double tfac, int width,
			      int prec, char frmt,
			      const string &noevents ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( width < 0 )
    os.setf( ios::left, ios::adjustfield );
  else
    os.unsetf( ios::adjustfield );
  width = ::abs( width );

  if ( p <= n || p < 0 || tend <= tbegin ) {
    os << setw( width ) << noevents << "  "
       << setw( width ) << noevents << '\n';
    return 0;
  }

  if ( frmt == 'f' || frmt == 'F' )
    os.setf( ios::fixed, ios::floatfield );
  else if ( frmt == 'e' || frmt == 'E' )
    os.setf( ios::scientific, ios::floatfield );
  else
    os.unsetf( ios::floatfield );

  if ( frmt == 'F' || frmt == 'E' || frmt == 'G' )
    os.setf( ios::uppercase );

  os << setprecision( prec ); 

  if ( pos < 0 ) {
    for ( int k=n+1; k<=p; k++ )
      os << setw( width ) << (*this)[k-1] * tfac << "  "
	 << setw( width ) << (*this)[k]-(*this)[k-1] << '\n';
  }
  else if ( pos > 0 ) {
    for ( int k=n+1; k<=p; k++ )
      os << setw( width ) << (*this)[k] * tfac << "  "
	 << setw( width ) << (*this)[k]-(*this)[k-1] << '\n';
  }
  else {
    for ( int k=n+1; k<=p; k++ )
      os << setw( width ) << 0.5*((*this)[k-1]+(*this)[k]) * tfac << "  "
	 << setw( width ) << (*this)[k]-(*this)[k-1] << '\n';
  }

  return p-n;
}


double EventData::frequency( double tbegin, double tend, 
			     double *sd ) const
{
  int n = next( tbegin );
  int p = previous( tend );
  
  if ( p <= n || p < 0 || tend <= tbegin ) {
    if ( sd != 0 )
      *sd = 0.0;
    return 0.0;
  }
  else {
    double meanisi = ( (*this)[p] - (*this)[n] )/(p-n);
    if ( sd != 0 ) {
      double var = 0.0;
      double ss;
      int nisi=0;
      for ( int k=n+1; k<=p; k++ ) {
	ss = ( (*this)[k] - (*this)[k-1] ) - meanisi;
	var += ( ss*ss - var )/(++nisi);
      }
      *sd = sqrt( var )/meanisi/meanisi;
    }
    return 1.0/meanisi;
  }
}


double EventData::frequency( double time, double *sd ) const
{
  return frequency( time, Range.back(), sd );
}


double EventData::frequency( int n, double *sd ) const
{
  if ( n < 1 || n > size() ) {
    if ( sd != 0 )
      *sd = 0.0;
    return 0.0;
  }

  return frequency( back( n ), back(), sd );
}


double EventData::frequencyAt( double time, double defaultfrequency ) const
{
  int n = next( time );

  if ( n < Index+R && n > 0 )
    return 1.0 / ( (*this)[n] - (*this)[n-1] );

  return defaultfrequency;
}


void EventData::frequency( SampleDataD &rate, double time, double defaultfrequency ) const
{
  rate = 0.0;
  int trials = 0;
  addFrequency( rate, trials, time, defaultfrequency );
}


void EventData::addFrequency( SampleDataD &rate, int &trials,
			      double time, double defaultfrequency ) const
{
  trials++;
  int k = next( time + rate.rangeFront() );
  for ( int i=0; i<rate.size(); i++ ) {
    for ( ; k<size() && (*this)[k] < time + rate.pos( i ); k++ );
    double f = defaultfrequency;
    if ( k<size() && k > minEvent() )
      f = 1.0 / ( (*this)[k] - (*this)[k-1] );
    rate[i] += ( f - rate[i] )/trials;
  }
}


void EventData::addFrequency( SampleDataD &rate, SampleDataD &ratesq,
			      int &trials, double time, double defaultfrequency ) const
{
  trials++;
  int k = next( time + rate.rangeFront() );
  for ( int i=0; i<rate.size(); i++ ) {
    for ( ; k<size() && (*this)[k] < time + rate.pos( i ); k++ );
    double f = defaultfrequency;
    if ( k<size() && k > minEvent() )
      f = 1.0 / ( (*this)[k] - (*this)[k-1] );
    rate[i] += ( f - rate[i] )/trials;
    ratesq[i] += ( f*f - ratesq[i])/trials;
  }
}


void EventData::cyclicFrequency( SampleDataD &rate, double time ) const
{
  rate = 0.0;
  int trials = 0;
  addCyclicFrequency( rate, trials, time );
}


void EventData::addCyclicFrequency( SampleDataD &rate, int &trials,
				    double time ) const
{
  double lmarg = time + rate.rangeFront();
  int kl = next( lmarg );
  double rmarg = time + rate.rangeBack();
  int kr = previous( rmarg );

  trials++;
  int k = kl;
  for ( int i=0; i<rate.size(); i++ ) {
    for ( ; k<size() && (*this)[k] < time + rate.pos( i ); k++ );
    double T = rate.length();
    if ( k>=size() || (*this)[k] > rmarg ) {
      if ( k > minEvent() && (*this)[k-1] >= lmarg ) {
	T = rmarg - (*this)[k-1] + (*this)[kl] - lmarg;
      }
    }
    else if ( k-1 < minEvent() || (*this)[k-1] < lmarg ) {
      if ( k < size() && (*this)[k] <= rmarg ) {
	T = (*this)[k] - lmarg + rmarg - (*this)[kr];
      }
    }
    else if ( k<size() && k > minEvent() )
      T = (*this)[k] - (*this)[k-1];
    rate[i] += ( 1.0/T - rate[i] )/trials;
  }
}


void EventData::addCyclicFrequency( SampleDataD &rate, SampleDataD &period,
				    int &trials, double time ) const
{
  double lmarg = time + rate.rangeFront();
  int kl = next( lmarg );
  double rmarg = time + rate.rangeBack();
  int kr = previous( rmarg );

  trials++;
  int k = kl;
  for ( int i=0; i<rate.size(); i++ ) {
    for ( ; k<size() && (*this)[k] < time + rate.pos( i ); k++ );
    double T = rate.length();
    if ( k>=size() || (*this)[k] > rmarg ) {
      if ( k > minEvent() && (*this)[k-1] >= lmarg ) {
	T = rmarg - (*this)[k-1] + (*this)[kl] - lmarg;
      }
    }
    else if ( k-1 < minEvent() || (*this)[k-1] < lmarg ) {
      if ( k < size() && (*this)[k] <= rmarg ) {
	T = (*this)[k] - lmarg + rmarg - (*this)[kr];
      }
    }
    else if ( k<size() && k > minEvent() )
      T = (*this)[k] - (*this)[k-1];
    rate[i] += ( 1.0/T - rate[i] )/trials;
    period[i] += ( T - period[i] )/trials;
  }
}


int EventData::frequencies( double tbegin, double tend,
			    MapD &freqs, int pos ) const
{
  freqs.clear();
  return addFrequencies( tbegin, tend, freqs, pos );
}


int EventData::addFrequencies( double tbegin, double tend,
			       MapD &freqs, int pos ) const
{
  int n = next( tbegin );
  int p = previous( tend );
  
  if ( p <= n || p < 0 || tend <= tbegin )
    return 0;

  freqs.reserve( freqs.size() + p-n );

  if ( pos < 0 ) {
    for ( int k=n+1; k<=p; k++ )
      freqs.push( (*this)[k-1], 1.0/((*this)[k]-(*this)[k-1]) );
  }
  else if ( pos > 0 ) {
    for ( int k=n+1; k<=p; k++ )
      freqs.push( (*this)[k], 1.0/((*this)[k]-(*this)[k-1]) );
  }
  else {
    for ( int k=n+1; k<=p; k++ )
      freqs.push( 0.5*((*this)[k-1]+(*this)[k]), 1.0/((*this)[k]-(*this)[k-1]) );
  }

  return p-n;
}


int EventData::saveFrequencies( double tbegin, double tend, ostream &os,
				int pos, double tfac, int width,
				int prec, char frmt,
				const string &noevents ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( width < 0 )
    os.setf( ios::left, ios::adjustfield );
  else
    os.unsetf( ios::adjustfield );
  width = ::abs( width );

  if ( p <= n || p < 0 || tend <= tbegin ) {
    os << setw( width ) << noevents << "  "
       << setw( width ) << noevents << '\n';
    return 0;
  }

  if ( frmt == 'f' || frmt == 'F' )
    os.setf( ios::fixed, ios::floatfield );
  else if ( frmt == 'e' || frmt == 'E' )
    os.setf( ios::scientific, ios::floatfield );
  else
    os.unsetf( ios::floatfield );

  if ( frmt == 'F' || frmt == 'E' || frmt == 'G' )
    os.setf( ios::uppercase );

  os << setprecision( prec ); 

  if ( pos < 0 ) {
    for ( int k=n+1; k<=p; k++ )
      os << setw( width ) << (*this)[k-1] * tfac << "  "
	 << setw( width ) << 1.0/((*this)[k]-(*this)[k-1]) << '\n';
  }
  else if ( pos > 0 ) {
    for ( int k=n+1; k<=p; k++ )
      os << setw( width ) << (*this)[k] * tfac << "  "
	 << setw( width ) << 1.0/((*this)[k]-(*this)[k-1]) << '\n';
  }
  else {
    for ( int k=n+1; k<=p; k++ )
      os << setw( width ) << 0.5*((*this)[k-1]+(*this)[k]) * tfac << "  "
	 << setw( width ) << 1.0/((*this)[k]-(*this)[k-1]) << '\n';
  }

  return p-n;
}


void EventData::intervalHistogram( double tbegin, double tend, 
				   SampleDataD &hist ) const
{
  hist = 0.0;
  addIntervalHistogram( tbegin, tend, hist );
}


void EventData::addIntervalHistogram( double tbegin, double tend, 
				      SampleDataD &hist ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin )
    return;
  
  for ( int k=n+1; k<=p; k++ ) {
    int inx = hist.index( (*this)[k] - (*this)[k-1] );
    if ( inx >= 0 && inx < hist.size() )
      hist[inx]++;
  }
}


void EventData::serialCorr( double tbegin, double tend, ArrayD &sc ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin ) {
    sc = 0.0;
    sc[0] = 1.0;
    return;
  }

  ArrayD iv;
  iv.reserve( p-n );
  for ( int k=n+1; k<=p; k++ )
    iv.push( (*this)[k] - (*this)[k-1] );

  ::relacs::serialCorr( iv, sc );
}


void EventData::fano( double tbegin, double tend, SampleDataD &ff ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin || ff.size() == 0 ) {
    ff = 0.0;
    return;
  }

  ArrayD count;
  count.reserve( (int)::floor( (tend - tbegin)/ff.pos( 0 ) ) + 1 );
  for ( int j=0; j<ff.size(); j++ ) {
    count.clear();
    int k = 0;
    for ( k=n; k<=p; ) {
      int c=0;
      for ( ;
	    k<=p && operator[]( k ) < tbegin + ff.pos( j )*(count.size()+1);
	    k++, c++ );
      if ( k<=p )
	count.push( double( c ) );
    }
    /*
    // overlapping windows:
    for ( k=n; k<=p && operator[]( k ) < tbegin + ff.pos( j )*0.5; k++ );
    int cs = 1;
    for ( ; k<=p; ) {
      int c=0;
      for ( ;
	    k<=p && operator[]( k ) < tbegin + ff.pos( j )*(cs+0.5);
	    k++, c++ );
      if ( k<=p ) {
	count.push( c );
	cs++;
      }
    }
    */
    double m=0.0, sd=0.0;
    m = count.mean( sd );
    ff[j] = m > 0.0 ? sd*sd/m : 0.0;
  }
}


double EventData::locking( double tbegin, double tend, double period ) const
{
  if ( tend <= tbegin )
    return 0.0;

  // make duration a multiple of period:
  double np = ::floor( (tend - tbegin) / period + 1.0e-6 );
  tend = tbegin + np*period;

  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 )
    return 0.0;

  double ns = p - n + 1;

  return ns/np;
}


double EventData::vectorStrength( double tbegin, double tend, double period ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin )
    return 0.0;

  double sc = 0.0;
  double ss = 0.0;
  int c = 0;
  for ( int k=n; k<=p; k++, c++ ) {
    double phi = 6.28318530717959 * ( (*this)[k] - tbegin ) / period;
    sc += ::cos( phi );
    ss += ::sin( phi );
  }
  return ::sqrt( sc*sc + ss*ss )/c;
}


double EventData::vectorPhase( double tbegin, double tend, double period ) const
{
  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin )
    return 0.0;

  double sc = 0.0;
  double ss = 0.0;
  for ( int k=n; k<=p; k++ ) {
    double phi = 6.28318530717959 * ( (*this)[k] - tbegin ) / period;
    sc += ::cos( phi );
    ss += ::sin( phi );
  }
  return ::atan2( ss, sc );
}


int EventData::average( double tbegin, double tend, const SampleDataD &trace,
			SampleDataD &ave ) const
{
  ave = 0.0;

  // adjust range to trace:
  if ( trace.rangeFront() > tbegin )
    tbegin = trace.rangeFront();
  if ( trace.rangeBack() < tend )
    tend = trace.rangeBack();

  // adjust range to ave:
  if ( ave.rangeFront() < 0.0 )
    tbegin -= ave.rangeFront();
  if ( ave.rangeBack() > 0.0 )
    tend -= ave.rangeBack();

  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin )
    return 0;

  int c = 0;
  for ( int k=n; k<=p; k++ ) {
    c++;
    for ( int j=0; j<ave.size(); j++ )
      ave[j] += ( trace[(*this)[k] + ave.pos(j)] - ave[j] ) / c;
  }
  return c;
}


int EventData::average( double tbegin, double tend, const SampleDataD &trace,
			SampleDataD &ave, SampleDataD &sd ) const
{
  ave = 0.0;
  sd = ave;

  // adjust range to trace:
  if ( trace.rangeFront() > tbegin )
    tbegin = trace.rangeFront();
  if ( trace.rangeBack() < tend )
    tend = trace.rangeBack();

  // adjust range to ave:
  if ( ave.rangeFront() < 0.0 )
    tbegin -= ave.rangeFront();
  if ( ave.rangeBack() > 0.0 )
    tend -= ave.rangeBack();

  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin )
    return 0;

  int c = 0;
  for ( int k=n; k<=p; k++ ) {
    c++;
    for ( int j=0; j<ave.size(); j++ )
      ave[j] += ( trace[(*this)[k] + ave.pos(j)] - ave[j] ) / c;
  }  

  c = 0;
  for ( int k=n; k<=p; k++ ) {
    c++;
    for ( int j=0; j<sd.size(); j++ ) {
      double s = trace[(*this)[k] + ave.pos(j)] - ave[j];
      sd[j] += ( s*s - sd[j] ) / c;
    }
  }  
  for ( int j=0; j<sd.size(); j++ )
    sd[j] = ::sqrt( sd[j] );
  return c;
}


int EventData::average( double tbegin, double tend, const SampleDataD &trace,
			SampleDataD &ave, SampleDataD &sd,
			vector< SampleDataD > &snippets ) const
{
  ave = 0.0;
  sd = ave;
  snippets.clear();

  // adjust range to trace:
  if ( trace.rangeFront() > tbegin )
    tbegin = trace.rangeFront();
  if ( trace.rangeBack() < tend )
    tend = trace.rangeBack();

  // adjust range to ave:
  if ( ave.rangeFront() < 0.0 )
    tbegin -= ave.rangeFront();
  if ( ave.rangeBack() > 0.0 )
    tend -= ave.rangeBack();

  int n = next( tbegin );
  int p = previous( tend );

  if ( p <= n || p < 0 || tend <= tbegin )
    return 0;

  snippets.reserve( p-n+1 );
  int c = 0;
  for ( int k=n; k<=p; k++ ) {
    c++;
    snippets.push_back( SampleDataD( ave.range() ) );
    for ( int j=0; j<ave.size(); j++ ) {
      double v = trace[(*this)[k] + ave.pos(j)];
      snippets.back()[j] = v;
      ave[j] += ( v - ave[j] ) / c;
    }
  }  

  c = 0;
  for ( int k=n; k<=p; k++ ) {
    c++;
    for ( int j=0; j<sd.size(); j++ ) {
      double s = trace[(*this)[k] + ave.pos(j)] - ave[j];
      sd[j] += ( s*s - sd[j] ) / c;
    }
  }  
  for ( int j=0; j<sd.size(); j++ )
    sd[j] = ::sqrt( sd[j] );
  return c;
}


void EventData::spectrum( double tbegin, double tend, double step,
			  SampleDataD &psd,
			  bool overlap, double (*window)( int j, int n ) ) const
{
  SampleDataD rr( tbegin, tend, step );
  rate( rr );
  rr -= mean( rr );
  ::relacs::rPSD( rr, psd, overlap, window );
}


void EventData::coherence( const SampleDataD &stimulus, SampleDataD &c,
			   bool overlap, double (*window)( int j, int n ) ) const
{
  SampleDataD rr( stimulus.range() );
  rate( rr );
  rr -= mean( rr );
  ::relacs::coherence( stimulus, rr, c, overlap, window );
}


void EventData::coherence( const EventData &e, double tbegin, double tend,
			   double step, SampleDataD &c,
			   bool overlap, double (*window)( int j, int n ) ) const
{
  SampleDataD r1( tbegin, tend, step );
  rate( r1 );
  r1 -= mean( r1 );

  SampleDataD r2( tbegin, tend, step );
  e.rate( r2 );
  r2 -= mean( r2 );

  ::relacs::coherence( r1, r2, c, overlap, window );
}


double EventData::latency( double time ) const
{
  // get index to first event:
  int n = next( time );

  // no event:
  if ( n == size() )
    return -1;

  return (*this)[n] - time;
}


void EventData::poisson( double rate, double refract,
			 double duration, RandomBase &random )
{
  double r = 1.0/(1.0/rate - refract);
  clear();
  reserve( 2 * (int)::ceil( r*duration ) );
  double t = refract + random.exponential() / r;
  while ( t < duration ) {
    if ( size() >= capacity() )
      reserve( capacity() + 2 * (int)::ceil( r*(duration-t) ) );
    push( t );
    t += refract + random.exponential() / r;
  }
}


void EventData::saveText( ostream &os, double tfac, 
			  int width, int precision, char format,
			  const string &noevents ) const
{
  if ( format == 'f' || format == 'F' )
    os.setf( ios::fixed, ios::floatfield );
  else if ( format == 'e' || format == 'E' )
    os.setf( ios::scientific, ios::floatfield );
  else
    os.unsetf( ios::floatfield );

  if ( format == 'F' || format == 'E' || format == 'G' )
    os.setf( ios::uppercase );

  if ( width < 0 )
    os.setf( ios::left, ios::adjustfield );
  else
    os.unsetf( ios::adjustfield );
  width = ::abs( width );

  os << setprecision( precision ); 

  int n = 0;
  for ( const_iterator i = begin()+minEvent(); i != end(); ++i, ++n )
    os << setw( width ) << *i * tfac << '\n';

  if ( n <= 0 && noevents.size() > 0 )
    os << setw( width ) << noevents << '\n';
}


void EventData::savePoint( ostream &os, double y, double tfac, 
			   int width, int precision, char format,
			   const string &noevents, double noy ) const
{
  if ( format == 'f' || format == 'F' )
    os.setf( ios::fixed, ios::floatfield );
  else if ( format == 'e' || format == 'E' )
    os.setf( ios::scientific, ios::floatfield );
  else
    os.unsetf( ios::floatfield );

  if ( format == 'F' || format == 'E' || format == 'G' )
    os.setf( ios::uppercase );

  if ( width < 0 )
    os.setf( ios::left, ios::adjustfield );
  else
    os.unsetf( ios::adjustfield );

  os << setprecision( precision ); 

  int n = 0;
  for ( const_iterator i = begin()+minEvent(); i != end(); ++i, ++n ) {
    os << setw( ::abs( width ) ) << *i * tfac << ' ' << y << '\n';
  }

  if ( n <= 0 && noevents.size() > 0 )
    os << setw( width ) << noevents << ' ' << noy << '\n';
}


void EventData::saveStroke( ostream &os, int offs, double tfac, 
			    int width, int precision, char format,
			    double lower, double upper,
			    const string &noevents, double noy ) const
{
  if ( format == 'f' || format == 'F' )
    os.setf( ios::fixed, ios::floatfield );
  else if ( format == 'e' || format == 'E' )
    os.setf( ios::scientific, ios::floatfield );
  else
    os.unsetf( ios::floatfield );

  if ( format == 'F' || format == 'E' || format == 'G' )
    os.setf( ios::uppercase );

  if ( width < 0 )
    os.setf( ios::left, ios::adjustfield );
  else
    os.unsetf( ios::adjustfield );

  os << setprecision( precision ); 

  int nn = 0;
  for ( const_iterator i = begin()+minEvent(); i != end(); ++i, ++nn ) {
    os << setw( ::abs( width ) ) << *i * tfac << ' ' << offs+lower << '\n';
    os << setw( ::abs( width ) ) << *i * tfac << ' ' << offs+upper << '\n';
    os << '\n';
  }

  if ( nn <= 0 && noevents.size() > 0 ) {
    os << setw( width ) << noevents << ' ' << noy << '\n';
    os << '\n';
  }
}


void EventData::saveBox( ostream &os, double bin, int offs, double tfac, 
			 int width, int precision, char format,
			 double lower, double upper,
			 const string &noevents, double noy ) const
{
  if ( format == 'f' || format == 'F' )
    os.setf( ios::fixed, ios::floatfield );
  else if ( format == 'e' || format == 'E' )
    os.setf( ios::scientific, ios::floatfield );
  else
    os.unsetf( ios::floatfield );

  if ( format == 'F' || format == 'E' || format == 'G' )
    os.setf( ios::uppercase );

  if ( width < 0 )
    os.setf( ios::left, ios::adjustfield );
  else
    os.unsetf( ios::adjustfield );

  os << setprecision( precision ); 

  bin *= tfac;
  int nn = 0;
  for ( const_iterator i = begin()+minEvent(); i != end(); ++i, ++nn ) {
    double t = *i * tfac;
    os << setw( ::abs( width ) ) << t << ' ' << offs+lower << '\n';
    os << setw( ::abs( width ) ) << t << ' ' << offs+upper << '\n';
    os << setw( ::abs( width ) ) << t+bin << ' ' << offs+upper << '\n';
    os << setw( ::abs( width ) ) << t+bin << ' ' << offs+lower << '\n';
    os << '\n';
  }

  if ( nn <= 0 && noevents.size() > 0 ) {
    os << setw( width ) << noevents << ' ' << noy << '\n';
    os << '\n';
  }
}


EventIterator &EventIterator::operator=( const EventIterator &p )
{
  if ( &p == this )
    return *this;

  ED = p.ED;
  Index = p.Index;

  return *this;
}


}; /* namespace relacs */

