/*
  ephys/traces.h
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

#ifndef _RELACS_EPHYS_TRACES_H_
#define _RELACS_EPHYS_TRACES_H_ 1

#include <relacs/inlist.h>
#include <relacs/eventlist.h>
using namespace relacs;

namespace ephys {


/*! 
\class Traces
\author Jan Benda
\version 1.0
\brief [lib] Variables for standard input traces and events in electrophysiology.
*/

class Traces
{
public:

  Traces( void );

  static void initialize( const InList &data, const EventList &events );

  static int StimulusEvents;

  static const int MaxSpikeTraces = 10;
  static int SpikeTraces;
  static int SpikeTrace[MaxSpikeTraces];
  static int SpikeEvents[MaxSpikeTraces];

  static const int MaxNerveTraces = 10;
  static int NerveTraces;
  static int NerveTrace[MaxNerveTraces];
  static int NerveEvents[MaxNerveTraces];


  static string spikeTraceName( void ) { return SpikeTraceName; };
  static void setSpikeTraceName( const string &name ) { SpikeTraceName = name; };

  static string spikeEventsName( void ) { return SpikeEventsName; };
  static void setSpikeEventsName( const string &name ) { SpikeEventsName = name; };

  static string nerveTraceName( void ) { return NerveTraceName; };
  static void setNerveTraceName( const string &name ) { NerveTraceName = name; };

  static string nerveEventsName( void ) { return NerveEventsName; };
  static void setNerveEventsName( const string &name ) { NerveEventsName = name; };


private:

  static string SpikeTraceName;
  static string SpikeEventsName;

  static string NerveTraceName;
  static string NerveEventsName;

};


}; /* namespace ephys */

#endif /* ! _RELACS_COMMON_EPHYSTRACES_H_ */
