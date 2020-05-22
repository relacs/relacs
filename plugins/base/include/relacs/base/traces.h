/*
  base/traces.h
  Variables for basic input traces and events.

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

#ifndef _RELACS_BASE_TRACES_H_
#define _RELACS_BASE_TRACES_H_ 1

#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/relacsplugin.h>
#include <relacs/standardtraces.h>
using namespace relacs;

namespace base {


/*! 
\class Traces
\author Jan Benda
\version 1.0
\brief [lib] Variables for basic input traces and events.
*/

class Traces : public StandardTraces
{
public:

  Traces( void );

  static void initialize( const RELACSPlugin *rp,
			  const InList &data, const EventList &events );

  static int StimulusEvents;

  static int VoltageTraces;
  static int VoltageTrace[MaxTraces];

  static int VoltageOutputs;
  static int VoltageOutput[MaxTraces];

  static int LEDOutputs;
  static int LEDOutput[MaxTraces];


    /*! Returns the names of all input traces with plain voltages,
        separated by ','. Can be passed to a text Parameter. */
  static string voltageTraceNames( void );
    /*! Returns the names of all output traces for voltages,
        separated by ','. Can be passed to a text Parameter. */
  static string voltageOutputNames( void );
    /*! Returns the names of all output traces for LEDs,
        separated by ','. Can be passed to a text Parameter. */
  static string LEDOutputNames( void );


private:

  static string VoltageTraceIdentifier[3];
  static string VoltageTraceNames;
  static string VoltageOutputIdentifier[3];
  static string VoltageOutputNames;
  static string LedOutputIdentifier[3];
  static string LedOutputNames;

  static void clear( void ); 
};


}; /* namespace base */

#endif /* ! _RELACS_BASE_TRACES_H_ */
