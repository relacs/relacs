/*
  acoustic/traces.h
  Variables for standard output traces of acoustic stimuli

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

#ifndef _RELACS_ACOUSTIC_TRACES_H_
#define _RELACS_ACOUSTIC_TRACES_H_ 1

#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/relacsplugin.h>
#include <relacs/standardtraces.h>
using namespace relacs;

namespace acoustic {


/*! 
\class Traces
\author Jan Benda
\version 1.0
\brief [lib] Variables for standard output traces of acoustic stimuli
       and recordings from microphones.
*/

class Traces : public StandardTraces
{
public:

  Traces( void );
  
  static void initialize( const RELACSPlugin *rp,
			  const InList &data, const EventList &events );

    /*! The number of available loadspeakers. */
  static int Loudspeakers;
    /*! The indices of the available loudspeakers. */
  static int Loudspeaker[MaxTraces];
    /*! The number of available left loadspeakers. */
  static int LeftSpeakers;
    /*! The indices of the available left loudspeakers. */
  static int LeftSpeaker[MaxTraces];
    /*! The number of available right loadspeakers. */
  static int RightSpeakers;
    /*! The indices of the available right loudspeakers. */
  static int RightSpeaker[MaxTraces];
    /*! The indices of the first left and the first right loudspeaker. */
  static int Speaker[2];

    /*! The number of available sound recording traces. */
  static int SoundTraces;
    /*! The indices of the available sound recording traces. */
  static int SoundTrace[MaxTraces];
    /*! The number of available left sound recording traces. */
  static int LeftSoundTraces;
    /*! The indices of the available left sound recording traces. */
  static int LeftSoundTrace[MaxTraces];
    /*! The number of available right sound recording traces. */
  static int RightSoundTraces;
    /*! The indices of the available right sound recording traces. */
  static int RightSoundTrace[MaxTraces];

    /*! Returns the names of all output traces connected to loudspeakers,
        separated by ','. Can be passed to a text Parameter. */
  static string loudspeakerTraceNames( void );
    /*! Returns the names of all output traces connected to left loudspeakers,
        separated by ','. Can be passed to a text Parameter. */
  static string leftLoudspeakerTraceNames( void );
    /*! Returns the names of all output traces connected to right loudspeakers,
        separated by ','. Can be passed to a text Parameter. */
  static string rightLoudspeakerTraceNames( void );

    /*! Returns the names of all input traces connected to microphones,
        separated by ','. Can be passed to a text Parameter. */
  static string soundTraceNames( void );
    /*! Returns the names of all input traces connected to left microphones,
        separated by ','. Can be passed to a text Parameter. */
  static string leftSoundTraceNames( void );
    /*! Returns the names of all input traces connected to right microphones,
        separated by ','. Can be passed to a text Parameter. */
  static string rightSoundTraceNames( void );


private:

  static string LoudspeakerIdentifier[2];
  static string LoudspeakerNames;
  static string LeftLoudspeakerIdentifier[2];
  static string LeftLoudspeakerNames;
  static string RightLoudspeakerIdentifier[2];
  static string RightLoudspeakerNames;

  static string SoundTraceIdentifier[4];
  static string SoundTraceNames;
  static string LeftSoundTraceIdentifier[4];
  static string LeftSoundTraceNames;
  static string RightSoundTraceIdentifier[4];
  static string RightSoundTraceNames;

  static void clear( void );

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_TRACES_H_ */
