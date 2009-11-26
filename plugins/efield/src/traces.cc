/*
  efield/traces.cc
  Variables for standard input traces and events of electric fish.

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


int Traces::EODTrace1 = -1;
int Traces::EODEvents1 = -1;
int Traces::ChirpEvents1 = -1;
int Traces::EODTrace2 = -1;
int Traces::EODEvents2 = -1;
int Traces::ChirpEvents2 = -1;
int Traces::BeatPeakEvents2 = -1;
int Traces::BeatTroughEvents2 = -1;

int Traces::SignalTrace1 = -1;
int Traces::SignalEvents1 = -1;


Traces::Traces( void )
{
}


void Traces::initialize( const InList &data, const EventList &events )
{
  // get trace indices:
  EODTrace1 = data.index( "EOD-1" );
  EODTrace2 = data.index( "EOD-2" );
  EODEvents1 = events.index( "EOD-1" );
  ChirpEvents1 = events.index( "Chirps-1" );
  EODEvents2 = events.index( "EOD-2" );
  ChirpEvents2 = events.index( "Chirps-2" );
  BeatPeakEvents2 = events.index( "Beat-2-1" );
  BeatTroughEvents2 = BeatPeakEvents2 + 1;
  SignalTrace1 = data.index( "Signal-1" );
  SignalEvents1 = events.index( "Signal-1" );
}


}; /* namespace efield */

#include "moc_traces.cc"
