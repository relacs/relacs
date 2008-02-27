/*
  auditorytraces.cc
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

#include "str.h"
#include "inlist.h"
#include "eventlist.h"
#include "outdata.h"
#include "relacsplugin.h"
#include "auditorytraces.h"

string AuditoryTraces::LoudspeakerName = "Speaker";
string AuditoryTraces::SoundTraceName = "Sound";

int AuditoryTraces::Loudspeakers = 0;
int AuditoryTraces::Loudspeaker[AuditoryTraces::MaxLoudspeakers] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int AuditoryTraces::LeftSpeakers = 0;
int AuditoryTraces::LeftSpeaker[AuditoryTraces::MaxLoudspeakers] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int AuditoryTraces::RightSpeakers = 0;
int AuditoryTraces::RightSpeaker[AuditoryTraces::MaxLoudspeakers] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int AuditoryTraces::Speaker[2] = { -1, -1 };

int AuditoryTraces::SoundTraces = 0;
int AuditoryTraces::SoundTrace[AuditoryTraces::MaxSoundTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int AuditoryTraces::LeftSoundTraces = 0;
int AuditoryTraces::LeftSoundTrace[AuditoryTraces::MaxSoundTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int AuditoryTraces::RightSoundTraces = 0;
int AuditoryTraces::RightSoundTrace[AuditoryTraces::MaxSoundTraces] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };


AuditoryTraces::AuditoryTraces( void )
{
}


void AuditoryTraces::initialize( const RELACSPlugin *rp,
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


string AuditoryTraces::loudspeakerName( void )
{
  return LoudspeakerName;
}


void AuditoryTraces::setLoudspeakerName( const string &name )
{
  LoudspeakerName = name;
}


string AuditoryTraces::soundTraceName( void )
{
  return SoundTraceName;
}


void AuditoryTraces::setSoundTraceName( const string &name )
{
  SoundTraceName = name;
}

