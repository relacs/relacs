/*
  ephys/traces.cc
  Variables for standard input traces and events in electrophysiology.

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

#include <relacs/str.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace ephys {


int Traces::StimulusEvents = 0;

string Traces::SpikeTraceName = "V";
string Traces::SpikeEventsName = "Spikes";
string Traces::SpikeTraceNames = "";
string Traces::SpikeEventsNames = "";

int Traces::SpikeTraces = 0;
int Traces::SpikeTrace[Traces::MaxSpikeTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int Traces::SpikeEvents[Traces::MaxSpikeTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

string Traces::NerveTraceName = "Nerve";
string Traces::NerveEventsName = "Nerve";
string Traces::NerveTraceNames = "";
string Traces::NerveEventsNames = "";

int Traces::NerveTraces = 0;
int Traces::NerveTrace[Traces::MaxNerveTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int Traces::NerveEvents[Traces::MaxNerveTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

string Traces::CurrentOutputName = "Current";
string Traces::CurrentOutputNames = "";
int Traces::CurrentOutputs = 0;
int Traces::CurrentOutput[Traces::MaxCurrentOutputs] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

string Traces::PotentialOutputName = "Potential";
string Traces::PotentialOutputNames = "";
int Traces::PotentialOutputs = 0;
int Traces::PotentialOutput[Traces::MaxPotentialOutputs] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };


Traces::Traces( void )
{
}


void Traces::initialize( const RELACSPlugin *rp,
			 const InList &data, 
			 const EventList &events )
{
  // get trace indices:
  StimulusEvents = events.index( "Stimulus" );

  // spikes:
  SpikeTraces = 0;
  SpikeTraceNames = "";
  SpikeEventsNames = "";
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    Str ns( k+1 );
    SpikeTrace[k] = data.index( SpikeTraceName + "-" + ns );
    SpikeEvents[k] = events.index( SpikeEventsName + "-" + ns );
    if ( SpikeTrace[k] >= 0 || SpikeEvents[k] >= 0 )
      SpikeTraces++;
    if ( SpikeTrace[k] >= 0 )
      SpikeTraceNames += '|' + SpikeTraceName + "-" + ns;
    if ( SpikeEvents[k] >= 0 )
      SpikeEventsNames += '|' + SpikeEventsName + "-" + ns;
  }
  SpikeTraceNames.erase( 0, 1 );
  SpikeEventsNames.erase( 0, 1 );

  // nerve potentials:
  NerveTraces = 0;
  NerveTraceNames = "";
  NerveEventsNames = "";
  for ( int k=0; k<MaxNerveTraces; k++ ) {
    Str ns( k+1 );
    NerveTrace[k] = data.index( NerveTraceName + "-" + ns );
    NerveEvents[k] = events.index( NerveEventsName + "-" + ns );
    if ( NerveTrace[k] >= 0 || NerveEvents[k] >= 0 )
      NerveTraces++;
    if ( NerveTrace[k] >= 0 )
      NerveTraceNames += '|' + NerveTraceName + "-" + ns;
    if ( NerveEvents[k] >= 0 )
      NerveEventsNames += '|' + NerveEventsName + "-" + ns;
  }
  NerveTraceNames.erase( 0, 1 );
  NerveEventsNames.erase( 0, 1 );

  // current output traces:
  CurrentOutputs = 0;
  CurrentOutputNames = "";
  for ( int k=0; k<MaxCurrentOutputs; k++ ) {
    Str ns( k+1 );
    CurrentOutput[k] = rp->outTraceIndex( CurrentOutputName + "-" + ns );
    if ( CurrentOutput[k] >= 0 ) {
      CurrentOutputs++;
      CurrentOutputNames += '|' + rp->outTraceName( CurrentOutput[k] );
    }
  }
  CurrentOutputNames.erase( 0, 1 );

  // potential output traces:
  PotentialOutputs = 0;
  PotentialOutputNames = "";
  for ( int k=0; k<MaxPotentialOutputs; k++ ) {
    Str ns( k+1 );
    PotentialOutput[k] = rp->outTraceIndex( PotentialOutputName + "-" + ns );
    if ( PotentialOutput[k] >= 0 ) {
      PotentialOutputs++;
      PotentialOutputNames += '|' + rp->outTraceName( PotentialOutput[k] );
    }
  }
  PotentialOutputNames.erase( 0, 1 );

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


string Traces::currentOutputNames( void )
{
  return CurrentOutputNames;
}


string Traces::potentialOutputNames( void )
{
  return PotentialOutputNames;
}


}; /* namespace ephys */
