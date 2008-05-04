/*
  ephystraces.cc
  Variables for standard input traces and events in electrophysiology.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/common/ephystraces.h>
using namespace relacs;


int EPhysTraces::StimulusEvents = 0;

string EPhysTraces::SpikeTraceName = "V";
string EPhysTraces::SpikeEventsName = "Spikes";

int EPhysTraces::SpikeTraces = 0;
int EPhysTraces::SpikeTrace[EPhysTraces::MaxSpikeTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int EPhysTraces::SpikeEvents[EPhysTraces::MaxSpikeTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

string EPhysTraces::NerveTraceName = "Nerve";
string EPhysTraces::NerveEventsName = "Nerve";

int EPhysTraces::NerveTraces = 0;
int EPhysTraces::NerveTrace[EPhysTraces::MaxNerveTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int EPhysTraces::NerveEvents[EPhysTraces::MaxNerveTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };


EPhysTraces::EPhysTraces( void )
{
}


void EPhysTraces::initialize( const InList &data, 
			      const EventList &events )
{
  // get trace indices:
  StimulusEvents = events.index( "Stimulus" );

  SpikeTraces = 0;
  for ( int k=0; k<MaxSpikeTraces; k++ ) {
    Str ns( k+1 );
    SpikeTrace[k] = data.index( SpikeTraceName + "-" + ns );
    SpikeEvents[k] = events.index( SpikeEventsName + "-" + ns );
    if ( SpikeTrace[k] >= 0 && SpikeEvents[k] >= 0 )
      SpikeTraces++;
  }

  NerveTraces = 0;
  for ( int k=0; k<MaxNerveTraces; k++ ) {
    Str ns( k+1 );
    NerveTrace[k] = data.index( NerveTraceName + "-" + ns );
    NerveEvents[k] = events.index( NerveEventsName + "-" + ns );
    if ( NerveTrace[k] >= 0 && NerveEvents[k] >= 0 )
      NerveTraces++;
  }
}

