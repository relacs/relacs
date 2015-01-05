/*
  ephys/traces.h
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

#ifndef _RELACS_EPHYS_TRACES_H_
#define _RELACS_EPHYS_TRACES_H_ 1

#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/relacsplugin.h>
#include <relacs/standardtraces.h>
using namespace relacs;

namespace ephys {


/*! 
\class Traces
\author Jan Benda
\version 1.1
\brief [lib] Variables for standard input traces and events in electrophysiology.
*/

class Traces : public StandardTraces
{
public:

  Traces( void );

  static void initialize( const RELACSPlugin *rp,
			  const InList &data, const EventList &events );

  static int SpikeTraces;
  static int SpikeTrace[MaxTraces];
  static int SpikeEvents[MaxTraces];

  static int CurrentTraces;
  static int CurrentTrace[MaxTraces];

  static int PotentialTraces;
  static int PotentialTrace[MaxTraces];

  static int NerveTraces;
  static int NerveTrace[MaxTraces];
  static int NerveEvents[MaxTraces];

  static int CurrentOutputs;
  static int CurrentOutput[MaxTraces];

  static int PotentialOutputs;
  static int PotentialOutput[MaxTraces];


    /*! Returns the names of all input traces with voltages of neurons,
        separated by ','. Can be passed to a text Parameter. */
  static string spikeTraceNames( void );
    /*! Returns the names of all event traces with spikes of neurons,
        separated by ','. Can be passed to a text Parameter. */
  static string spikeEventNames( void );
    /*! Returns the names of all input traces with currents for current-clamp,
        separated by ','. Can be passed to a text Parameter. */
  static string currentTraceNames( void );
    /*! Returns the names of all input traces with command potentials for voltage-clamp,
        separated by ','. Can be passed to a text Parameter. */
  static string potentialTraceNames( void );
    /*! Returns the names of all input traces with voltages of whole nerves,
        separated by ','. Can be passed to a text Parameter. */
  static string nerveTraceNames( void );
    /*! Returns the names of all event traces with events detected in nerves,
        separated by ','. Can be passed to a text Parameter. */
  static string nerveEventNames( void );
    /*! Returns the names of all output traces for current injections,
        separated by ','. Can be passed to a text Parameter. */
  static string currentOutputNames( void );
    /*! Returns the names of all output traces for voltage-clamp potentials,
        separated by ','. Can be passed to a text Parameter. */
  static string potentialOutputNames( void );


private:

  static string SpikeTraceIdentifier[2];
  static string SpikeEventsIdentifier[2];
  static string SpikeTraceNames;
  static string SpikeEventsNames;

  static string CurrentTraceIdentifier[2];
  static string CurrentTraceNames;

  static string PotentialTraceIdentifier[2];
  static string PotentialTraceNames;

  static string NerveTraceIdentifier[2];
  static string NerveEventsIdentifier[2];
  static string NerveTraceNames;
  static string NerveEventsNames;

  static string CurrentOutputIdentifier[2];
  static string CurrentOutputNames;

  static string PotentialOutputIdentifier[2];
  static string PotentialOutputNames;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_TRACES_H_ */
