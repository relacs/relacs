/*
  syncevents.cc
  

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

#include <iostream>
#include <relacs/eventlist.h>


int main( void )
{
  EventData spikes1( 10 );
  EventData spikes2( 10 );
  spikes1.setStepsize( 0.2 );
  spikes2.setStepsize( 0.2 );

  double t = 0.0;
  for ( int k=0; k<spikes1.capacity(); k++ ) {
    spikes1.push( t );
    spikes2.push( t+k*0.1+3.0 );
    t += 1.0;
  }

  cout << "spikes1 =\n";
  spikes1.saveText( cout );
  cout << '\n';
  cout << "spikes2 =\n";
  spikes2.saveText( cout );
  cout << '\n';

  EventData spikessync;
  //  spikes1.sync( spikes2, spikessync, 0.2 );
  //  spikessync = spikes1*spikes2;
  spikes1.sum( spikes2, spikessync );
  //  spikessync = spikes1+spikes2;
  //  spikes1 += spikes2;
  //  cout << "spikessync =\n";
  //  spikessync.saveText( cout );
  //  spikes1.saveText( cout );
  cout << '\n';

  EventList spikess( 0 );
  spikess.reserve( 2 );
  //  spikess.add( &spikes1 );
  spikess.add( &spikes2 );

  spikess.sync( spikessync, 0.2, 1.0, true );
  //  spikess.sum( spikessync );
  cout << "spikessync =\n";
  spikessync.saveText( cout );
  /*
  cerr << "size: " << spikes2.size() << "  " << spikessync.size() << endl;
  for ( int k=0; k<spikes2.size() && k<spikessync.size(); k++ ) {
    cerr << spikes2[k] << " " << ( spikes2[k] == spikessync[k] ? "equal" : "error" ) << endl;
  }
  */
  //  spikes1.saveText( cout );
  cout << '\n';

  return 0;
}
