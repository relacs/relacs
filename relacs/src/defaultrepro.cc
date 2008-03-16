/*
  defaultrepro.cc
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

#include "defaultrepro.h"


DefaultRePro::DefaultRePro( void )
  : RePro( "DefaultRePro", "DefaultRePro", "Jan Benda", "1.0" )
{
  // parameter:
  Duration = 0.3;

  // add some parameter as options:
  addNumber( "duration", "Duration", Duration, 0.01, 1000.0, 0.02, "sec", "ms" );
}


DefaultRePro::~DefaultRePro( void )
{
}


int DefaultRePro::main( void )
{
  // get options:
  Duration = number( "duration" );

  // don't print repro message:
  noMessage();

  // plot trace:
  plotToggle( true, false, Duration, 0.0 );

  while ( true )
    sleep( Duration );
}

