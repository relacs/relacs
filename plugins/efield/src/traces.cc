/*
  efield/traces.cc
  Variables for standard output traces of electric fields and standard input traces and events of electric fish.

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

#include <relacs/efield/traces.h>
using namespace relacs;

namespace efield {


int Traces::GlobalEField = -1;
int Traces::GlobalAMEField = -1;
int Traces::LocalEFields = 0;
int Traces::LocalEField[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalAMEFields = 0;
int Traces::LocalAMEField[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::EFields = 0;
int Traces::EField[2*MaxEFields] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

int Traces::EODTrace = -1;
int Traces::EODEvents = -1;
int Traces::ChirpEvents = -1;

int Traces::LocalEODTraces = 0;
int Traces::LocalEODTrace[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalEODEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalChirpEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalBeatPeakEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalBeatTroughEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };

int Traces::GlobalEFieldTrace = -1;
int Traces::GlobalEFieldEvents = -1;

int Traces::LocalEFieldTraces = 0;
int Traces::LocalEFieldTrace[MaxEFields] = { -1, -1, -1, -1, -1, -1 };
int Traces::LocalEFieldEvents[MaxEFields] = { -1, -1, -1, -1, -1, -1 };


Traces::Traces( void )
{
}


void Traces::initialize( const RELACSPlugin *rp,
			 const InList &data,
			 const EventList &events )
{
  // global stimulation electrode:
  GlobalEField = rp->outTraceIndex( "GlobalEField" );
  GlobalAMEField = rp->outTraceIndex( "GlobalEFieldAM" );

  // local stimulation electrodes:
  LocalEFields = 0;
  LocalAMEFields = 0;
  for ( int k=1; k<=MaxEFields; k++ ) {
    LocalEField[LocalEFields] = rp->outTraceIndex( "LocalEField-" + Str( k ) );
    if ( LocalEField[LocalEFields] >= 0 )
      LocalEFields++;
    LocalAMEField[LocalAMEFields] = rp->outTraceIndex( "LocalEFieldAM-" + Str( k ) );
    if ( LocalAMEField[LocalAMEFields] >= 0 )
      LocalAMEFields++;
  }

  // all stimulation electrodes:
  if ( GlobalEField >= 0 )
    EField[EFields++] = GlobalEField;
  if ( GlobalAMEField >= 0 )
    EField[EFields++] = GlobalAMEField;
  for ( int k=0; k<LocalEFields; k++ ) {
    if ( LocalEField[k] >= 0 )
      EField[EFields++] = LocalEField[k];
  }
  for ( int k=0; k<LocalAMEFields; k++ ) {
    if ( LocalAMEField[k] >= 0 )
      EField[EFields++] = LocalAMEField[k];
  }
				    
  // global EOD:
  EODTrace = data.index( "EOD" );
  EODEvents = events.index( "EOD" );
  ChirpEvents = events.index( "Chirps" );

  // local EODs:
  LocalEODTraces = 0;
  for ( int k=1; k<=MaxEFields; k++ ) {
    LocalEODTrace[LocalEODTraces] = data.index( "LocalEOD-" + Str( k ) );
    LocalEODEvents[LocalEODTraces] = events.index( "LocalEOD-" + Str( k ) );
    LocalChirpEvents[LocalEODTraces] = events.index( "LocalChirps-" + Str( k ) );
    LocalBeatPeakEvents[LocalEODTraces] = events.index( "LocalBeat-" + Str( k ) + "-1" );
    LocalBeatTroughEvents[LocalEODTraces] = events.index( "LocalBeat-" + Str( k ) + "-2" );
    if ( LocalEODTrace[LocalEODTraces] >= 0 )
      LocalEODTraces++;
  }

  // global stimulation fields:
  GlobalEFieldTrace = data.index( "GlobalEFieldStimulus" );
  GlobalEFieldEvents = events.index( "GlobalEFieldStimulus" );

  // local stimulation fields:
  LocalEFieldTraces = 0;
  for ( int k=1; k<=MaxEFields; k++ ) {
    LocalEFieldTrace[LocalEFieldTraces] = data.index( "LocalEFieldStimulus-" + Str( k ) );
    LocalEFieldEvents[LocalEFieldTraces] = events.index( "LocalEFieldStimulus-" + Str( k ) );
    if ( LocalEFieldTrace[LocalEFieldTraces] >= 0 )
      LocalEFieldTraces++;
  }
}


}; /* namespace efield */
