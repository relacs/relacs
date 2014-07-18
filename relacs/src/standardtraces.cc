/*
  standardtraces.cc
  Infrastructure for providing indices to standard inpput and output traces and events.

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

#include <relacs/str.h>
#include <relacs/standardtraces.h>

namespace relacs {


StandardTraces::StandardTraces( void )
{
}


void StandardTraces::clearIndices( int *traceindices, int n )
{
  if ( n < 0 )
    n = MaxTraces;
  for ( int k=0; k<n; k++ )
    traceindices[k] = -1;
}


void StandardTraces::initStandardTraces( const InList &data, int *ntraces, int *traceindices,
					 const string *names, string &namelist, bool alpha )
{
  for ( int k=0; k<MaxTraces; k++ )
    traceindices[k] = -1;
  *ntraces = 0;
  namelist = "";
  for ( int k=0; k<MaxTraces; k++ ) {
    string ns = "";
    if ( k > 0 ) {
      if ( alpha ) {
	char a = 'A';
	a += k-1;
	ns = "-" + Str( a );
      }
      else
	ns = "-" + Str( k );
    }
    for ( int j=0; j<10 && !names[j].empty() && traceindices[*ntraces]<0; j++ )
      traceindices[*ntraces] = data.index( names[j] + ns );
    if ( traceindices[*ntraces] >= 0 ) {
      if ( ! namelist.empty() )
	namelist += "|";
      namelist += data[ traceindices[*ntraces] ].ident();
      (*ntraces)++;
    }
  }
}


void StandardTraces::initStandardEventTraces( const InList &data, const EventList &events,
					      int *ntraces, int *traceindices, int *eventindices,
					      const string *tracenames, const string *eventnames,
					      string &tracenamelist, string &eventnamelist, bool alpha )
{
  for ( int k=0; k<MaxTraces; k++ ) {
    traceindices[k] = -1;
    eventindices[k] = -1;
  }
  *ntraces = 0;
  tracenamelist = "";
  eventnamelist = "";
  for ( int k=0; k<MaxTraces; k++ ) {
    string ns = "";
    if ( k > 0 ) {
      if ( alpha ) {
	char a = 'A';
	a += k-1;
	ns = "-" + Str( a );
      }
      else
	ns = "-" + Str( k );
    }
    for ( int j=0; j<10 && !tracenames[j].empty() && traceindices[*ntraces]<0; j++ )
      traceindices[*ntraces] = data.index( tracenames[j] + ns );
    for ( int j=0; j<10 && !eventnames[j].empty() && eventindices[*ntraces]<0; j++ )
      eventindices[*ntraces] = events.index( eventnames[j] + ns );
    if ( traceindices[*ntraces] >= 0 ) {
      if ( ! tracenamelist.empty() )
	tracenamelist += "|";
      tracenamelist += data[ traceindices[*ntraces] ].ident();
    }
    if ( eventindices[*ntraces] >= 0 ) {
      if ( ! eventnamelist.empty() )
	eventnamelist += "|";
      eventnamelist += events[ eventindices[*ntraces] ].ident();
    }
    if ( traceindices[*ntraces] >= 0 || eventindices[*ntraces] >= 0 )
      (*ntraces)++;
  }
}


void StandardTraces::initStandardOutputs( const RELACSPlugin *rp, int *ntraces, int *traceindices,
					  const string *names, string &namelist, bool alpha )
{
  for ( int k=0; k<MaxTraces; k++ )
    traceindices[k] = -1;
  *ntraces = 0;
  namelist = "";
  for ( int k=0; k<MaxTraces; k++ ) {
    string ns = "";
    if ( k > 0 ) {
      if ( alpha ) {
	char a = 'A';
	a += k-1;
	ns = "-" + Str( a );
      }
      else
	ns = "-" + Str( k );
    }
    for ( int j=0; j<10 && !names[j].empty() && traceindices[*ntraces]<0; j++ )
      traceindices[*ntraces] = rp->outTraceIndex( names[j] + ns );
    if ( traceindices[*ntraces] >= 0 ) {
      if ( ! namelist.empty() )
	namelist += "|";
      namelist += rp->outTraceName( traceindices[*ntraces] );
      (*ntraces)++;
    }
  }
}


}; /* namespace relacs */
