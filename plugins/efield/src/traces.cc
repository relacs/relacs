/*
  efield/traces.cc
  Variables for standard output traces of electric fields and standard input traces and events of electric fish.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/efield/traces.h>
using namespace relacs;

namespace efield {


int Traces::GlobalEField = -1;
int Traces::LocalEFields = 0;
int Traces::LocalEField[MaxEFields] = { -1, -1, -1, -1, -1, -1 };

int Traces::EODTrace = -1;
int Traces::EODEvents = -1;
int Traces::ChirpEvents = -1;

int Traces::LocalEODTraces = 0;
int Traces::LocalEODTrace[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalEODEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalChirpEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalBeatPeakEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalBeatTroughEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };

int Traces::EFieldSignalTraces = 0;
int Traces::EFieldSignalTrace[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::EFieldSignalEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };


Traces::Traces( void )
{
}


void Traces::initialize( const RELACSPlugin *rp,
			 const InList &data,
			 const EventList &events )
{
  // global stimulation electrode:
  GlobalEField = rp->outTraceIndex( "EField-global" );

  // local stimulation electrodes:
  LocalEFields = 0;
  for ( int k=0; k<MaxEFields; k++ ) {
    LocalEField[LocalEFields] = rp->outTraceIndex( "EField-local-" + Str( k ) );
    if ( LocalEField[LocalEFields] >= 0 )
      LocalEFields++;
  }
				    
  // global EOD:
  EODTrace = data.index( "EOD" );
  EODEvents = events.index( "EOD" );
  ChirpEvents = events.index( "Chirps" );

  // local EODs:
  LocalEODTraces = 0;
  for ( int k=0; k<MaxEFields; k++ ) {
    LocalEODTrace[LocalEODTraces] = data.index( "EOD-" + Str( k ) );
    LocalEODEvents[LocalEODTraces] = events.index( "EOD-" + Str( k ) );
    LocalChirpEvents[LocalEODTraces] = events.index( "Chirps-" + Str( k ) );
    LocalBeatPeakEvents[LocalEODTraces] = events.index( "BeatPeaks-" + Str( k ) );
    LocalBeatTroughEvents[LocalEODTraces] = events.index( "BeatTroughs-" + Str( k ) );
    if ( LocalEODTrace[LocalEODTraces] >= 0 )
      LocalEODTraces++;
  }

  // signals:
  EFieldSignalTraces = 0;
  for ( int k=0; k<MaxEFields; k++ ) {
    EFieldSignalTrace[EFieldSignalTraces] = data.index( "EFieldSignal-" + Str( k ) );
    EFieldSignalEvents[EFieldSignalTraces] = events.index( "EFieldSignal-" + Str( k ) );
    if ( EFieldSignalTrace[EFieldSignalTraces] >= 0 )
      EFieldSignalTraces++;
  }
}


}; /* namespace efield */
