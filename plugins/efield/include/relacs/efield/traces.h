/*
  efield/traces.h
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

#ifndef _RELACS_EFIELD_TRACES_H_
#define _RELACS_EFIELD_TRACES_H_ 1

#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/relacsplugin.h>
#include <relacs/standardtraces.h>
using namespace relacs;

namespace efield {


/*!
\class Traces
\brief [lib] Variables for standard output traces of electric fields and
       standard input traces and events of electric fish.
\author Jan Benda
\version 2.0 (Oct 16, 2013)
*/


class Traces : public StandardTraces
{

public:

  Traces( void );

  static void initialize( const RELACSPlugin *rp,
			  const InList &data, const EventList &events );


protected:

  // The globally recorded electric field:
    /*! The index of the trace recording the EOD of a fish. */
  static int EODTrace;
    /*! The index of the events recording the zero crossings of the EOD of a fish. */
  static int EODEvents;
    /*! The index of the events recording the chirps of the EOD of a fish. */
  static int ChirpEvents;

  // The locally recorded electric field:
    /*! The number of available recorded local EOD measurements of a fish. */
  static int LocalEODTraces;
    /*! The indices of traces recording local EOD measurements of a fish. */
  static int LocalEODTrace[MaxTraces];
    /*! The indices of events recording the zero crossings of local EOD measurements of a fish. */
  static int LocalEODEvents[MaxTraces];
    /*! The indices of events recording the chirps of local EOD measurements of a fish. */
  static int LocalChirpEvents[MaxTraces];
    /*! The indices of events recording the beat peaks of local EOD measurements of a fish. */
  static int LocalBeatPeakEvents[MaxTraces];
    /*! The indices of events recording the beat troughs of local EOD measurements of a fish. */
  static int LocalBeatTroughEvents[MaxTraces];

  // The recorded electric fields in various tanks:
    /*! The number of available tanks with EOD measurements. */
  static int FishEODTanks;
    /*! The number of available recorded EOD measurements in each tank. */
  static int FishEODTraces[MaxTraces];
    /*! The indices of traces recording EOD measurements in each tank. */
  static int FishEODTrace[MaxTraces][MaxTraces];
    /*! The indices of events recording the zero crossings of EOD measurements in each tank. */
  static int FishEODEvents[MaxTraces][MaxTraces];
    /*! The indices of events recording the chirps of EOD measurements in each tank. */
  static int FishChirpEvents[MaxTraces][MaxTraces];

  // Stimulation:
    /*! The index of the global electric field stimulus channel (direct stimulation). */
  static int GlobalEField;
    /*! The index of the global electric field stimulus channel (amplitude modulation). */
  static int GlobalAMEField;

    /*! The number of available local electric field stimulus channels (direct stimulation). */
  static int LocalEFields;
    /*! The indices of local electric field stimulus channels (direct stimulation). */
  static int LocalEField[MaxTraces];
    /*! The number of available local electric field stimulus channels (amplitude modulation). */
  static int LocalAMEFields;
    /*! The indices of local electric field stimulus channels (amplitude modulation). */
  static int LocalAMEField[MaxTraces];
    /*! The number of available electric field stimulus channels simulating a fish in a tank. */
  static int FishEFields;
    /*! The indices of electric field stimulus channels simulating a fish in a tank. */
  static int FishEField[MaxTraces];
    /*! The number of all available electric field stimulus channels. */
  static int EFields;
    /*! The indices of all electric field stimulus channels. */
  static int EField[2*MaxTraces];

  // Recordings of global electric field stimuli:
    /*! The index of the trace of the recorded global electric field stimulus. */
  static int GlobalEFieldTrace;
    /*! The index of the events of zero crossings of the recorded global electric field stimulus. */
  static int GlobalEFieldEvents;

  // Recordings of local electric field stimuli:
    /*! The number of available recorded local electric field stimuli. */
  static int LocalEFieldTraces;
    /*! The indices of traces of recorded local electric field stimuli. */
  static int LocalEFieldTrace[MaxTraces];
    /*! The indices of events of zero crossings of recorded local electric field stimuli. */
  static int LocalEFieldEvents[MaxTraces];

  // Recordings of electric field stimuli simulating a fish in a tank:
    /*! The number of available recorded electric field stimuli simulating a fish in a tank. */
  static int FishEFieldTraces;
    /*! The indices of traces of recorded electric field stimuli simulating a fish in a tank. */
  static int FishEFieldTrace[MaxTraces];
    /*! The indices of events of zero crossings of recorded electric field stimuli 
        simulating a fish in a tank. */
  static int FishEFieldEvents[MaxTraces];


private:

  static string LocalEFieldIdentifier[2];
  static string LocalAMEFieldIdentifier[2];
  static string FishEFieldIdentifier[3];
  static string LocalEFieldTraceIdentifier[2];
  static string LocalEFieldEventsIdentifier[2];
  static string FishEFieldTraceIdentifier[3];
  static string FishEFieldEventsIdentifier[3];

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_TRACES_H_ */
