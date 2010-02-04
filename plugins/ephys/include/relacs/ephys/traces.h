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
#include <relacs/relacsplugin.h>
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

  static void initialize( const RELACSPlugin *rp,
			  const InList &data, const EventList &events );

  static int StimulusEvents;

  static const int MaxSpikeTraces = 10;
  static int SpikeTraces;
  static int SpikeTrace[MaxSpikeTraces];
  static int SpikeEvents[MaxSpikeTraces];

  static const int MaxNerveTraces = 10;
  static int NerveTraces;
  static int NerveTrace[MaxNerveTraces];
  static int NerveEvents[MaxNerveTraces];

  static const int MaxCurrentOutputs = 10;
  static int CurrentOutputs;
  static int CurrentOutput[MaxCurrentOutputs];

  static const int MaxPotentialOutputs = 10;
  static int PotentialOutputs;
  static int PotentialOutput[MaxPotentialOutputs];


    /*! Returns the names of all input traces with voltages of neurons,
        separated by '|'. Can be passed to a text Parameter. */
  static string spikeTraceNames( void );
    /*! Returns the names of all event traces with spikes of neurons,
        separated by '|'. Can be passed to a text Parameter. */
  static string spikeEventNames( void );
    /*! Returns the names of all input traces with voltages of whole nerves,
        separated by '|'. Can be passed to a text Parameter. */
  static string nerveTraceNames( void );
    /*! Returns the names of all event traces with events detected in nerves,
        separated by '|'. Can be passed to a text Parameter. */
  static string nerveEventNames( void );
    /*! Returns the names of all output traces for current injections,
        separated by '|'. Can be passed to a text Parameter. */
  static string currentOutputNames( void );
    /*! Returns the names of all output traces for voltage-clamp potentials,
        separated by '|'. Can be passed to a text Parameter. */
  static string potentialOutputNames( void );


private:

  static string SpikeTraceName;
  static string SpikeEventsName;
  static string SpikeTraceNames;
  static string SpikeEventsNames;

  static string NerveTraceName;
  static string NerveEventsName;
  static string NerveTraceNames;
  static string NerveEventsNames;

  static string CurrentOutputName;
  static string CurrentOutputNames;

  static string PotentialOutputName;
  static string PotentialOutputNames;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_TRACES_H_ */
