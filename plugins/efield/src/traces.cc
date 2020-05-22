/*
  efield/traces.cc
  Variables for standard output traces of electric fields and standard input traces and events of electric fish.

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

#include <relacs/efield/traces.h>
using namespace relacs;

namespace efield {


int Traces::EODTrace = -1;
int Traces::EODEvents = -1;
int Traces::ChirpEvents = -1;

int Traces::LocalEODTraces = 0;
int Traces::LocalEODTrace[MaxTraces] = { -1, -1, -1, -1 };
int Traces::LocalEODEvents[MaxTraces] = { -1, -1, -1, -1 };
int Traces::LocalChirpEvents[MaxTraces] = { -1, -1, -1, -1 };
int Traces::LocalBeatPeakEvents[MaxTraces] = { -1, -1, -1, -1 };
int Traces::LocalBeatTroughEvents[MaxTraces] = { -1, -1, -1, -1 };

int Traces::FishEODTanks = 0;
int Traces::FishEODTraces[MaxTraces] = { 0, 0, 0, 0 };
int Traces::FishEODTrace[MaxTraces][MaxTraces] = { { -1, -1, -1, -1 }, { -1, -1, -1, -1 } };
int Traces::FishEODEvents[MaxTraces][MaxTraces] = { { -1, -1, -1, -1 }, { -1, -1, -1, -1 } };
int Traces::FishChirpEvents[MaxTraces][MaxTraces] = { { -1, -1, -1, -1 }, { -1, -1, -1, -1 } };

int Traces::GlobalEField = -1;
int Traces::GlobalAMEField = -1;
int Traces::LocalEFields = 0;
int Traces::LocalEField[MaxTraces] = { -1, -1, -1, -1 };
int Traces::LocalAMEFields = 0;
int Traces::LocalAMEField[MaxTraces] = { -1, -1, -1, -1 };
int Traces::FishEFields = 0;
int Traces::FishEField[MaxTraces] = { -1, -1, -1, -1 };
int Traces::EFields = 0;
int Traces::EField[2*MaxTraces] = { -1, -1, -1, -1 };

int Traces::GlobalEFieldTrace = -1;
int Traces::GlobalEFieldEvents = -1;

int Traces::LocalEFieldTraces = 0;
int Traces::LocalEFieldTrace[MaxTraces] = { -1, -1, -1, -1 };
int Traces::LocalEFieldEvents[MaxTraces] = { -1, -1, -1, -1 };

int Traces::FishEFieldTraces = 0;
int Traces::FishEFieldTrace[MaxTraces] = { -1, -1, -1, -1 };
int Traces::FishEFieldEvents[MaxTraces] = { -1, -1, -1, -1 };

string Traces::LocalEFieldIdentifier[2] = { "LocalEField", "" };
string Traces::LocalAMEFieldIdentifier[2] = { "LocalAMEField", "" };
string Traces::FishEFieldIdentifier[3] = { "GlobalEField", "EField", "" };
string Traces::LocalEFieldTraceIdentifier[2] = { "LocalEFieldStimulus", "" };
string Traces::LocalEFieldEventsIdentifier[2] = { "LocalEFieldStimulus", "" };
string Traces::FishEFieldTraceIdentifier[3] = { "GlobalEFieldStimulus", "FishEFieldStimulus", "" };
string Traces::FishEFieldEventsIdentifier[3] = { "GlobalEFieldStimulus", "FishEFieldStimulus", "" };


Traces::Traces( void )
{
  clear();
}

void Traces::clear( void )
{
  clearIndices( LocalEODTrace );
  clearIndices( LocalEODEvents );
  clearIndices( LocalChirpEvents );
  clearIndices( LocalBeatPeakEvents );
  clearIndices( LocalBeatTroughEvents );
  for ( int k=0; k<MaxTraces; k++ ) {
    FishEODTraces[k] = 0;
    for ( int j=0; j<MaxTraces; j++ ) {
      FishEODTrace[k][j] = -1;
      FishEODEvents[k][j] = -1;
      FishChirpEvents[k][j] = -1;
    }
  }
  clearIndices( LocalEField );
  clearIndices( LocalAMEField );
  clearIndices( FishEField );
  clearIndices( EField, 2*MaxTraces );
  clearIndices( LocalEFieldTrace );
  clearIndices( LocalEFieldEvents );
  clearIndices( FishEFieldTrace );
  clearIndices( FishEFieldEvents );
}

void Traces::initialize( const RELACSPlugin *rp,
			 const InList &data,
			 const EventList &events )
{
  clear();
  // global EOD:
  string fs[3] = { "", "-1", "-A-1" };
  for ( int k=0; k<3; k++ ) {
    EODTrace = data.index( "EOD"+fs[k] );
    if ( EODTrace >= 0 ) {
      EODEvents = events.index( "EOD"+fs[k] );
      ChirpEvents = events.index( "Chirps"+fs[k] );
      break;
    }
  }

  // local EODs:
  LocalEODTraces = 0;
  for ( int k=0; k<=MaxTraces; k++ ) {
    string ns = "";
    if ( k > 0 )
      ns = "-" + Str( k );
    LocalEODTrace[LocalEODTraces] = data.index( "LocalEOD" + ns );
    LocalEODEvents[LocalEODTraces] = events.index( "LocalEOD" + ns );
    LocalChirpEvents[LocalEODTraces] = events.index( "LocalChirps" + ns );
    LocalBeatPeakEvents[LocalEODTraces] = events.index( "LocalBeat" + ns + "-1" );
    LocalBeatTroughEvents[LocalEODTraces] = events.index( "LocalBeat" + ns + "-2" );
    if ( LocalEODTrace[LocalEODTraces] >= 0 )
      LocalEODTraces++;
  }

  // fish EODs:
  FishEODTanks = 0;
  for ( int k=0; k<=MaxTraces; k++ ) {
    FishEODTraces[FishEODTanks] = 0;
    string ts = "";
    if ( k > 0 ) {
      char a = 'A';
      a += k-1;
      ts = "-" + Str( a );
    }
    for ( int j=0; j<=MaxTraces; j++ ) {
      string ns = "";
      if ( j > 0 )
	ns = "-" + Str( j );
      FishEODTrace[FishEODTanks][FishEODTraces[FishEODTanks]] = data.index( "EOD" + ts + ns );
      FishEODEvents[FishEODTanks][FishEODTraces[FishEODTanks]] = events.index( "EOD" + ts + ns );
      FishChirpEvents[FishEODTanks][FishEODTraces[FishEODTanks]] = events.index( "Chirps" + ts + ns );
      if ( FishEODTrace[FishEODTanks][FishEODTraces[FishEODTanks]] >= 0 )
	FishEODTraces[FishEODTanks]++;
    }
    if ( FishEODTraces[FishEODTanks] > 0 )
      FishEODTanks++;
  }

  // global stimulation electrode:
  GlobalEField = rp->outTraceIndex( "GlobalEField" );
  GlobalAMEField = rp->outTraceIndex( "GlobalEFieldAM" );

  // local stimulation electrodes:
  string namelist;
  initStandardOutputs( rp, &LocalEFields, LocalEField, LocalEFieldIdentifier, namelist );
  initStandardOutputs( rp, &LocalAMEFields, LocalAMEField, LocalAMEFieldIdentifier, namelist );

  // fish stimulation electrodes:
  initStandardOutputs( rp, &FishEFields, FishEField, FishEFieldIdentifier, namelist, true );

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

  // global stimulation fields:
  GlobalEFieldTrace = data.index( "GlobalEFieldStimulus" );
  GlobalEFieldEvents = events.index( "GlobalEFieldStimulus" );

  // local stimulation fields:
  initStandardEventTraces( data, events, &LocalEFieldTraces, LocalEFieldTrace, LocalEFieldEvents,
			   LocalEFieldTraceIdentifier, LocalEFieldEventsIdentifier, namelist, namelist );

  // fish stimulation fields:
  initStandardEventTraces( data, events, &FishEFieldTraces, FishEFieldTrace, FishEFieldEvents,
			   FishEFieldTraceIdentifier, FishEFieldEventsIdentifier, namelist, namelist, true );
  cerr << "Traces: found " << EFields << " EField traces!!" << endl;
}


}; /* namespace efield */
