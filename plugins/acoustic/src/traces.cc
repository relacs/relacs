/*
  acoustictraces.cc
  Variables for standard output traces of acoustic stimuli

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/outdata.h>
#include <relacs/acoustic/traces.h>
using namespace relacs;

namespace acoustic {


string Traces::LoudspeakerIdentifier[2] = { "Speaker", "" };
string Traces::LoudspeakerNames = "";
string Traces::LeftLoudspeakerIdentifier[2] = { "Left-Speaker", "" };
string Traces::LeftLoudspeakerNames = "";
string Traces::RightLoudspeakerIdentifier[2] = { "Right-Speaker", "" };
string Traces::RightLoudspeakerNames = "";

string Traces::SoundTraceIdentifier[4] = { "Sound", "Microphone", "Mic", "" };
string Traces::SoundTraceNames = "";
string Traces::LeftSoundTraceIdentifier[4] = { "Left-Sound", "Left-Microphone", "Left-Mic", "" };
string Traces::LeftSoundTraceNames = "";
string Traces::RightSoundTraceIdentifier[4] = { "Right-Sound", "Right-Microphone", "Right-Mic", "" };
string Traces::RightSoundTraceNames = "";

int Traces::Loudspeakers = 0;
int Traces::Loudspeaker[Traces::MaxTraces] = { -1, -1, -1, -1 };
int Traces::LeftSpeakers = 0;
int Traces::LeftSpeaker[Traces::MaxTraces] = { -1, -1, -1, -1 };
int Traces::RightSpeakers = 0;
int Traces::RightSpeaker[Traces::MaxTraces] = { -1, -1, -1, -1 };
int Traces::Speaker[2] = { -1, -1 };

int Traces::SoundTraces = 0;
int Traces::SoundTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };
int Traces::LeftSoundTraces = 0;
int Traces::LeftSoundTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };
int Traces::RightSoundTraces = 0;
int Traces::RightSoundTrace[Traces::MaxTraces] = { -1, -1, -1, -1 };


Traces::Traces( void )
{
}


void Traces::initialize( const RELACSPlugin *rp,
			 const InList &data, 
			 const EventList &events )
{
  // speaker:
  initStandardOutputs( rp, &Loudspeakers, Loudspeaker,
		       LoudspeakerIdentifier, LoudspeakerNames );
  initStandardOutputs( rp, &LeftSpeakers, LeftSpeaker,
		       LeftLoudspeakerIdentifier, LeftLoudspeakerNames );
  initStandardOutputs( rp, &RightSpeakers, RightSpeaker,
		       RightLoudspeakerIdentifier, RightLoudspeakerNames );
  if ( ! LeftLoudspeakerNames.empty() ) {
    if ( ! LoudspeakerNames.empty() )
      LoudspeakerNames += "|";
    LoudspeakerNames += LeftLoudspeakerNames;
  }
  if ( ! RightLoudspeakerNames.empty() ) {
    if ( ! LoudspeakerNames.empty() )
      LoudspeakerNames += "|";
    LoudspeakerNames += RightLoudspeakerNames;
  }
  Speaker[0] = LeftSpeaker[0];
  Speaker[1] = RightSpeaker[0];

  // sound recordings:
  initStandardTraces( data, &SoundTraces, SoundTrace,
		      SoundTraceIdentifier, SoundTraceNames );
  initStandardTraces( data, &LeftSoundTraces, LeftSoundTrace,
		      LeftSoundTraceIdentifier, LeftSoundTraceNames );
  initStandardTraces( data, &RightSoundTraces, RightSoundTrace,
		      RightSoundTraceIdentifier, RightSoundTraceNames );
}


string Traces::loudspeakerTraceNames( void )
{
  return LoudspeakerNames;
}


string Traces::leftLoudspeakerTraceNames( void )
{
  return LeftLoudspeakerNames;
}


string Traces::rightLoudspeakerTraceNames( void )
{
  return RightLoudspeakerNames;
}


string Traces::soundTraceNames( void )
{
  return SoundTraceNames;
}


string Traces::leftSoundTraceNames( void )
{
  return LeftSoundTraceNames;
}


string Traces::rightSoundTraceNames( void )
{
  return RightSoundTraceNames;
}


}; /* namespace acoustic */
