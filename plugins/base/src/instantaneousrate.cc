/*
  base/instantaneousrate.cc
  Computes the instantaneous rate between succeeding events.

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

#include <relacs/base/instantaneousrate.h>
using namespace relacs;

namespace base {


InstantaneousRate::InstantaneousRate( const string &ident, int mode )
  : Filter( ident, mode, SingleEventFilter, 1,
            "InstantaneousRate", "base", "Jan Benda", "1.0", "May 03, 2015" )
{
  // options:
  addNumber( "minrate", "Minimum initial rate", 0.0, 0.0, 1000000.0, 10.0, "Hz" );
  addNumber( "maxrate", "Maximum initial rate", 1000.0, 0.0, 1000000.0, 10.0, "Hz" );
}


int InstantaneousRate::init( const EventData &inevents, InData &outdata )
{
  cerr << "INIT RATE\n";
  Index = inevents.size();
  double minrate = number( "minrate" );
  double maxrate = number( "maxrate" );
  if ( maxrate < minrate ) {
    maxrate = minrate;
    minrate = 0.0;
  }
  outdata.setMinValue( minrate );
  outdata.setMaxValue( maxrate );
  outdata.setUnit( "Hz" );
  return 0;
}


int InstantaneousRate::filter( const EventData &inevents, InData &outdata )
{
  while ( Index < inevents.size() ) {
    float rate = 0.0;
    if ( Index > inevents.minEvent() )
      rate = 1.0/(inevents[Index] - inevents[Index-1]);
    while ( outdata.currentTime() < inevents[Index] )
      outdata.push( rate );
    Index++;
  }
  return 0;
}


addFilter( InstantaneousRate, base );

}; /* namespace base */

#include "moc_instantaneousrate.cc"
