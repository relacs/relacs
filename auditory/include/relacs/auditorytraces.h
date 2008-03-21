/*
  auditorytraces.h
  Variables for standard output traces of acoustic stimuli

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _AUDITORYTRACES_H_
#define _AUDITORYTRACES_H_


/*! 
\class AuditoryTraces
\author Jan Benda
\version 1.0
\brief Variables for standard output traces of acoustic stimuli
       and recordings from microphones.
*/


class RELACSPlugin;
class InList;
class EventList;


class AuditoryTraces
{
public:

  AuditoryTraces( void );
  
  static void initialize( const RELACSPlugin *rp,
			  const InList &data, const EventList &events );

    /*! Maximum number of supported loadspeakers. */
  static const int MaxLoudspeakers = 12;
    /*! The number of available loadspeakers. */
  static int Loudspeakers;
    /*! The indices of the available loudspeakers. */
  static int Loudspeaker[MaxLoudspeakers];
    /*! The number of available left loadspeakers. */
  static int LeftSpeakers;
    /*! The indices of the available left loudspeakers. */
  static int LeftSpeaker[MaxLoudspeakers];
    /*! The number of available right loadspeakers. */
  static int RightSpeakers;
    /*! The indices of the available right loudspeakers. */
  static int RightSpeaker[MaxLoudspeakers];
    /*! The indices of the first left and the first right loudspeaker. */
  static int Speaker[2];

    /*! Maximum number of supported sound recording traces. */
  static const int MaxSoundTraces = 12;
    /*! The number of available sound recording traces. */
  static int SoundTraces;
    /*! The indices of the available sound recording traces. */
  static int SoundTrace[MaxSoundTraces];
    /*! The number of available left sound recording traces. */
  static int LeftSoundTraces;
    /*! The indices of the available left sound recording traces. */
  static int LeftSoundTrace[MaxSoundTraces];
    /*! The number of available right sound recording traces. */
  static int RightSoundTraces;
    /*! The indices of the available right sound recording traces. */
  static int RightSoundTrace[MaxSoundTraces];

  static string loudspeakerName( void );
  static void setLoudspeakerName( const string &name );

  static string soundTraceName( void );
  static void setSoundTraceName( const string &name );


private:

  static string LoudspeakerName;
  static string SoundTraceName;

};


#endif
