/*
  acoustictraces.cc
  Variables for standard output traces of acoustic stimuli

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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


string Traces::LoudspeakerName = "Speaker";
string Traces::SoundTraceName = "Sound";

int Traces::Loudspeakers = 0;
int Traces::Loudspeaker[Traces::MaxLoudspeakers] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int Traces::LeftSpeakers = 0;
int Traces::LeftSpeaker[Traces::MaxLoudspeakers] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int Traces::RightSpeakers = 0;
int Traces::RightSpeaker[Traces::MaxLoudspeakers] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int Traces::Speaker[2] = { -1, -1 };

int Traces::SoundTraces = 0;
int Traces::SoundTrace[Traces::MaxSoundTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int Traces::LeftSoundTraces = 0;
int Traces::LeftSoundTrace[Traces::MaxSoundTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int Traces::RightSoundTraces = 0;
int Traces::RightSoundTrace[Traces::MaxSoundTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };


Traces::Traces( void )
{
}


void Traces::initialize( const RELACSPlugin *rp,
				 const InList &data, 
				 const EventList &events )
{
  Loudspeakers = 0;
  Loudspeaker[0] = rp->outTraceIndex( LoudspeakerName );
  if ( Loudspeaker[0] >= 0 )
    Loudspeakers++;
  for ( int k=0; k<MaxLoudspeakers; k++ ) {
    Loudspeaker[Loudspeakers] = rp->outTraceIndex( LoudspeakerName + "-" + Str( k ) );
    if ( Loudspeaker[Loudspeakers] >= 0 )
      Loudspeakers++;
  }

  LeftSpeakers = 0;
  LeftSpeaker[0] = rp->outTraceIndex( "Left-" + LoudspeakerName );
  if ( LeftSpeaker[0] >= 0 )
    LeftSpeakers++;
  for ( int k=0; k<MaxLoudspeakers; k++ ) {
    LeftSpeaker[LeftSpeakers] = rp->outTraceIndex( "Left-" + LoudspeakerName + "-" + Str( k ) );
    if ( LeftSpeaker[LeftSpeakers] >= 0 )
      LeftSpeakers++;
  }

  RightSpeakers = 0;
  RightSpeaker[0] = rp->outTraceIndex( "Right-" + LoudspeakerName );
  if ( RightSpeaker[0] >= 0 )
    RightSpeakers++;
  for ( int k=0; k<MaxLoudspeakers; k++ ) {
    RightSpeaker[RightSpeakers] = rp->outTraceIndex( "Right-" + LoudspeakerName + "-" + Str( k ) );
    if ( RightSpeaker[RightSpeakers] >= 0 )
      RightSpeakers++;
  }

  Speaker[0] = LeftSpeaker[0];
  Speaker[1] = RightSpeaker[0];

  SoundTraces = 0;
  SoundTrace[0] = data.index( SoundTraceName );
  if ( SoundTrace[0] >= 0 )
    SoundTraces++;
  for ( int k=0; k<MaxSoundTraces; k++ ) {
    SoundTrace[SoundTraces] = data.index( SoundTraceName + "-" + Str( k ) );
    if ( SoundTrace[SoundTraces] >= 0 )
      SoundTraces++;
  }

  LeftSoundTraces = 0;
  LeftSoundTrace[0] = data.index( "Left-" + SoundTraceName );
  if ( LeftSoundTrace[0] >= 0 )
    LeftSoundTraces++;
  for ( int k=0; k<MaxSoundTraces; k++ ) {
    LeftSoundTrace[LeftSoundTraces] = data.index( "Left-" + SoundTraceName + "-" + Str( k ) );
    if ( LeftSoundTrace[LeftSoundTraces] >= 0 )
      LeftSoundTraces++;
  }

  RightSoundTraces = 0;
  RightSoundTrace[0] = data.index( "Right-" + SoundTraceName );
  if ( RightSoundTrace[0] >= 0 )
    RightSoundTraces++;
  for ( int k=0; k<MaxSoundTraces; k++ ) {
    RightSoundTrace[RightSoundTraces] = data.index( "Right-" + SoundTraceName + "-" + Str( k ) );
    if ( RightSoundTrace[RightSoundTraces] >= 0 )
      RightSoundTraces++;
  }

}


string Traces::loudspeakerName( void )
{
  return LoudspeakerName;
}


void Traces::setLoudspeakerName( const string &name )
{
  LoudspeakerName = name;
}


string Traces::soundTraceName( void )
{
  return SoundTraceName;
}


void Traces::setSoundTraceName( const string &name )
{
  SoundTraceName = name;
}


}; /* namespace acoustic */
