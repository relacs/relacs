/*
  pause.cc
  Does nothing

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

#include <relacs/pause.h>


Pause::Pause( void )
  : RePro( "Pause", "Pause", "Jan Benda", "1.2", "Oct 1, 2004" )
{
  // add some options:
  addNumber( "duration", "Duration", 1.0, 0.01, 1000.0, 0.02, "sec", "ms" );
  addInteger( "repeats", "Repeats", 1, 0, 10000, 2 );
  addBoolean( "savedata", "Save Data", false );
}


int Pause::main( void )
{
  // get options:
  double duration = number( "duration" );
  int repeats = integer( "repeats" );
  bool savedata = boolean( "savedata" );

  // don't print repro message:
  noMessage();

  // don't save files:
  if ( ! savedata )
    noSaving();

  // plot trace:
  plotToggle( true, false, duration, 0.0 );

  for ( int count=0; 
	( repeats <= 0 || count < repeats ) && softStop() == 0; 
	count++ ) {
    sleep( duration );
    if ( interrupt() )
      return Aborted;
  }

  return Completed;
}


addRePro( Pause );

#include "moc_pause.cc"
