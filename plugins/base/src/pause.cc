/*
  base/pause.cc
  Does nothing

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

#include <relacs/base/pause.h>
using namespace relacs;

namespace base {


Pause::Pause( void )
  : RePro( "Pause", "base", "Jan Benda", "1.4", "Dec 10, 2014" )
{
  // add some options:
  addNumber( "duration", "Duration", 0.0, 0.0, 1000000.0, 1.0, "sec" ).setStyle( OptWidget::SpecialInfinite );
  addBoolean( "savedata", "Save raw data", false );
  addNumber( "plotwidth", "Width of plotting window", 0.0, 0.0, 100.0, 0.1, "sec" ).setStyle( OptWidget::SpecialNone );
}


int Pause::main( void )
{
  // get options:
  double duration = number( "duration" );
  bool savedata = boolean( "savedata" );
  double plotwidth = number( "plotwidth" );

  // don't print repro message:
  noMessage();

  // don't save files:
  if ( ! savedata )
    noSaving();

  // plot trace:
  if ( plotwidth > 0.001 )
    tracePlotContinuous( plotwidth );
  else
    tracePlotContinuous();

  double starttime = currentTime();

  do {
    sleepWait( 0.5 );
    if ( interrupt() )
      return Aborted;
  }  while ( softStop() == 0 &&
	     ( duration <= 0 || currentTime() - starttime < duration ) );

  return Completed;
}


addRePro( Pause, base );

}; /* namespace base */

#include "moc_pause.cc"
