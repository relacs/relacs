/*
  ephys/traces.cc
  Variables for standard input traces and events in electrophysiology.

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

#include <relacs/str.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace ephys {

string Traces::SpikeTraceIdentifier[2] = { "V", "" };
string Traces::SpikeEventsIdentifier[2] = { "Spikes", "" };
string Traces::SpikeTraceNames = "";
string Traces::SpikeEventsNames = "";

int Traces::SpikeTraces = 0;
int Traces::SpikeTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };
int Traces::SpikeEvents[Traces::MaxTraces] = { -1, -1, -1, -1 };

string Traces::CurrentTraceIdentifier[2] = { "Current", "" };
string Traces::CurrentTraceNames = "";
int Traces::CurrentTraces = 0;
int Traces::CurrentTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };

string Traces::PotentialTraceIdentifier[2] = { "Potential", "" };
string Traces::PotentialTraceNames = "";
int Traces::PotentialTraces = 0;
int Traces::PotentialTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };

string Traces::NerveTraceIdentifier[2] = { "Nerve", "" };
string Traces::NerveEventsIdentifier[2] = { "Nerve", "" };
string Traces::NerveTraceNames = "";
string Traces::NerveEventsNames = "";

int Traces::NerveTraces = 0;
int Traces::NerveTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };
int Traces::NerveEvents[Traces::MaxTraces] = { -1, -1, -1, -1 };

string Traces::CurrentOutputIdentifier[2] = { "Current", "" };
string Traces::CurrentOutputNames = "";
int Traces::CurrentOutputs = 0;
int Traces::CurrentOutput[Traces::MaxTraces] = { -1, -1, -1, -1 };

string Traces::PotentialOutputIdentifier[2] = { "Potential", "" };
string Traces::PotentialOutputNames = "";
int Traces::PotentialOutputs = 0;
int Traces::PotentialOutput[Traces::MaxTraces] = { -1, -1, -1, -1 };


Traces::Traces( void )
{
  clear();
}


void Traces::clear( void ) 
{
  clearIndices( SpikeTrace );
  clearIndices( SpikeEvents );
  clearIndices( CurrentTrace );
  clearIndices( PotentialTrace );
  clearIndices( NerveTrace );
  clearIndices( NerveEvents );
  clearIndices( CurrentOutput );
  clearIndices( PotentialOutput );
}


void Traces::initialize( const RELACSPlugin *rp,
			 const InList &data, 
			 const EventList &events )
{
  // spikes:
  initStandardEventTraces( data, events, &SpikeTraces, SpikeTrace, SpikeEvents,
			   SpikeTraceIdentifier, SpikeEventsIdentifier,
			   SpikeTraceNames, SpikeEventsNames );

  // current inputs:
  initStandardTraces( data, &CurrentTraces, CurrentTrace,
		      CurrentTraceIdentifier, CurrentTraceNames );

  // potential inputs:
  initStandardTraces( data, &PotentialTraces, PotentialTrace,
		      PotentialTraceIdentifier, PotentialTraceNames );

  // nerve potentials:
  initStandardEventTraces( data, events, &NerveTraces, NerveTrace, NerveEvents,
			   NerveTraceIdentifier, NerveEventsIdentifier,
			   NerveTraceNames, NerveEventsNames );

  // current output traces:
  initStandardOutputs( rp, &CurrentOutputs, CurrentOutput,
		       CurrentOutputIdentifier, CurrentOutputNames );

  // potential output traces:
  initStandardOutputs( rp, &PotentialOutputs, PotentialOutput,
		       PotentialOutputIdentifier, PotentialOutputNames );

}


string Traces::spikeTraceNames( void )
{
  return SpikeTraceNames;
}


string Traces::spikeEventNames( void )
{
  return SpikeEventsNames;
}


string Traces::nerveTraceNames( void )
{
  return NerveTraceNames;
}


string Traces::nerveEventNames( void )
{
  return NerveEventsNames;
}


string Traces::currentTraceNames( void )
{
  return CurrentTraceNames;
}


string Traces::potentialTraceNames( void )
{
  return PotentialTraceNames;
}


string Traces::currentOutputNames( void )
{
  return CurrentOutputNames;
}


string Traces::potentialOutputNames( void )
{
  return PotentialOutputNames;
}


}; /* namespace ephys */
