/*
  efield/traces.h
  Variables for standard output traces of electric fields and standard input traces and events of electric fish.

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

#ifndef _RELACS_EFIELD_TRACES_H_
#define _RELACS_EFIELD_TRACES_H_ 1

#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/relacsplugin.h>
using namespace relacs;

namespace efield {


/*!
\class Traces
\brief [lib] Variables for standard output traces of electric fields and
       standard input traces and events of electric fish.
\author Jan Benda
\version 1.0 (Nov 26, 2009)
*/


class Traces
{

public:

  Traces( void );

  static void initialize( const RELACSPlugin *rp,
			  const InList &data, const EventList &events );


protected:

    /*! Maximum number of supported electric field input or output channels. */
  static const int MaxEFields = 6;

    /*! The index of the global electric field stimulus channel (direct stimulation). */
  static int GlobalEField;
    /*! The index of the global electric field stimulus channel (amplitude modulation). */
  static int GlobalAMEField;
    /*! The number of available local electric field stimulus channels (direct stimulation). */
  static int LocalEFields;
    /*! The indices of local electric field stimulus channels (direct stimulation). */
  static int LocalEField[MaxEFields];
    /*! The number of available local electric field stimulus channels (amplitude modulation). */
  static int LocalAMEFields;
    /*! The indices of local electric field stimulus channels (amplitude modulation). */
  static int LocalAMEField[MaxEFields];

    /*! The index of the trace recording the EOD of a fish. */
  static int EODTrace;
    /*! The index of the events recording the zero crossings of the EOD of a fish. */
  static int EODEvents;
    /*! The index of the events recording the chirps of the EOD of a fish. */
  static int ChirpEvents;

    /*! The number of available recorded local EOD measurements of a fish. */
  static int LocalEODTraces;
    /*! The indices of traces recording local EOD measurements of a fish. */
  static int LocalEODTrace[MaxEFields];
    /*! The indices of events recording the zero crossings of local EOD measurements of a fish. */
  static int LocalEODEvents[MaxEFields];
    /*! The indices of events recording the chirps of local EOD measurements of a fish. */
  static int LocalChirpEvents[MaxEFields];
    /*! The indices of events recording the beat peaks of local EOD measurements of a fish. */
  static int LocalBeatPeakEvents[MaxEFields];
    /*! The indices of events recording the beat troughs of local EOD measurements of a fish. */
  static int LocalBeatTroughEvents[MaxEFields];

    /*! The index of the trace of the recorded global electric field stimulus. */
  static int GlobalEFieldTrace;
    /*! The index of the events of zero crossings of the recorded global electric field stimulus. */
  static int GlobalEFieldEvents;

    /*! The number of available recorded electric field stimuli. */
  static int LocalEFieldTraces;
    /*! The indices of traces of recorded electric field stimuli. */
  static int LocalEFieldTrace[MaxEFields];
    /*! The indices of events of zero crossings of recorded electric field stimuli. */
  static int LocalEFieldEvents[MaxEFields];

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_TRACES_H_ */
