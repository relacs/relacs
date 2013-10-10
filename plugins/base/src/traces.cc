/*
  base/traces.cc
  Variables for basic input traces and events.

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
#include <relacs/base/traces.h>
using namespace relacs;

namespace base {


int Traces::StimulusEvents = 0;

string Traces::VoltageTraceIdentifier[3] = { "Voltage", "V", "" };
string Traces::VoltageTraceNames = "";
int Traces::VoltageTraces = 0;
int Traces::VoltageTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };

string Traces::VoltageOutputIdentifier[3] = { "Voltage", "V", "" };
string Traces::VoltageOutputNames = "";
int Traces::VoltageOutputs = 0;
int Traces::VoltageOutput[Traces::MaxTraces] = { -1, -1, -1, -1 };

string Traces::LedOutputIdentifier[3] = { "Light", "LED", "" };
string Traces::LedOutputNames = "";
int Traces::LEDOutputs = 0;
int Traces::LEDOutput[Traces::MaxTraces] = { -1, -1, -1, -1 };


Traces::Traces( void )
{
  clearIndices( VoltageTrace );
  clearIndices( VoltageOutput );
  clearIndices( LEDOutput );
}


void Traces::initialize( const RELACSPlugin *rp,
			 const InList &data, 
			 const EventList &events )
{
  // get trace indices:
  StimulusEvents = events.index( "Stimulus" );
  
  // voltage inputs:
  initStandardTraces( data, &VoltageTraces, VoltageTrace,
		      VoltageTraceIdentifier, VoltageTraceNames );
  
  // voltage output traces:
  initStandardOutputs( rp, &VoltageOutputs, VoltageOutput,
		       VoltageOutputIdentifier, VoltageOutputNames );
  
  // LED output traces:
  initStandardOutputs( rp, &LEDOutputs, LEDOutput,
		       LedOutputIdentifier, LedOutputNames );
  
}


string Traces::voltageTraceNames( void )
{
  return VoltageTraceNames;
}


string Traces::voltageOutputNames( void )
{
  return VoltageOutputNames;
}


string Traces::LEDOutputNames( void )
{
  return LedOutputNames;
}


}; /* namespace base */
