/*
  base/record.cc
  Simply records data

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

#include <relacs/base/record.h>
using namespace relacs;

namespace base {


Record::Record( void )
  : RePro( "Record", "base", "Jan Benda", "1.0", "Aug 13, 2012" )
{
  // add some options:
  addNumber( "duration", "Duration", 1.0, 0.01, 1000.0, 0.02, "sec", "ms" );
  addInteger( "repeats", "Repeats", 0, 0, 10000, 2 ).setStyle( OptWidget::SpecialInfinite );
}


Record::~Record( void )
{
}


int Record::main( void )
{
  // get options:
  double duration = number( "duration" );
  int repeats = integer( "repeats" );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotContinuous( duration );

  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    sleepWait( duration );
    if ( interrupt() )
      return Aborted;
  }

  return Completed;
}


addRePro( Record, base );

}; /* namespace base */

#include "moc_record.cc"
