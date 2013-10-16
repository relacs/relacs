/*
  standardtraces.h
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

#ifndef _RELACS_STANDARDTRACES_H_
#define _RELACS_STANDARDTRACES_H_ 1

#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/relacsplugin.h>
using namespace relacs;

namespace relacs {


/*! 
\class StandardTraces
\author Jan Benda
\brief Infrastructure for providing indices to standard inpput and output traces and events.
*/

class StandardTraces
{
public:

  StandardTraces( void );


protected:

  static void clearIndices( int *traceindices, int n=-1 );

  static void initStandardTraces( const InList &data, int *ntraces, int *traceindices,
				  const string *names, string &namelist, bool alpha=false );
  static void initStandardEventTraces( const InList &data, const EventList &events,
				       int *ntraces, int *traceindices, int *eventindices,
				       const string *tracenames, const string *eventnames,
				       string &tracenamelist, string &eventnamelist, bool alpha=false );
  static void initStandardOutputs( const RELACSPlugin *rp, int *ntraces, int *traceindices,
				   const string *names, string &namelist, bool alpha=false );

  static const int MaxTraces = 10;

};


}; /* namespace relacs */

#endif /* ! _RELACS_STANDARD_TRACES_H_ */
