/*
  base/record.cc
  Simply records data

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

#include <relacs/digitalio.h>
#include <relacs/base/record.h>
using namespace relacs;

namespace base {


Record::Record( void )
  : RePro( "Record", "base", "Jan Benda", "1.2", "Dec 10, 2014" )
{
  // add some options:
  addNumber( "duration", "Duration", 0.0, 0.0, 1000000.0, 1.0, "sec" ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "plotwidth", "Width of plotting window", 0.0, 0.0, 100.0, 0.1, "sec" ).setStyle( OptWidget::SpecialNone );
  addBoolean( "dioout", "Indicate recording on a DIO line", false );
  addText( "diodevice", "Name of the digitial I/O device", "dio-1" ).setActivation( "dioout", "true" );
  addInteger( "dioline", "Output line on the digitial I/O device", 0, 0, 128 ).setActivation( "dioout", "true" );
}


int Record::main( void )
{
  // get options:
  double duration = number( "duration" );
  double plotwidth = number( "plotwidth" );
  bool dioout = boolean( "dioout" );
  string diodevice = text( "diodevice" );
  int dioline = integer( "dioline" );

  // don't print repro message:
  noMessage();

  // plot trace:
  if ( plotwidth > 0.001 )
    tracePlotContinuous( plotwidth );
  else
    tracePlotContinuous();

  // find dio device:
  DigitalIO *dio = 0;
  if ( dioout ) {
    dio = digitalIO( diodevice );
    if ( dio != 0 ) {
      dio->allocateLine( dioline );
      dio->configureLine( dioline, true );
      dio->write( dioline, true );
    }
  }

  double starttime = currentTime();

  do {
    sleepWait( 0.5 );
    if ( interrupt() ) {
      if ( dio != 0 )
	dio->write( dioline, false );
      return duration <= 1e-8 ? Completed : Aborted;
    }
  }  while ( softStop() == 0 &&
	     ( duration <= 1e-8 || currentTime() - starttime < duration ) );

  if ( dio != 0 )
    dio->write( dioline, false );

  return Completed;
}


addRePro( Record, base );

}; /* namespace base */

#include "moc_record.cc"
