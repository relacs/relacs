/*
  binrate.cc
  

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

#include <iostream>
#include <fstream>
#include <relacs/eventlist.h>
#include <relacs/sampledata.h>
using namespace relacs;


int main( void )
{
  EventList spikes( 10, 100 );
  for ( int k=0; k<spikes.size(); k++ ) {
    for ( int j=0; j<=k; j++ ) {
      spikes[j].push( 0.0+1.0*k );
    }
  }
  ofstream sf( "spikes.dat" );
  spikes.saveStroke( sf );

  SampleDataD rate ( 0.0, 10.0, 1.0 );
  spikes.rate( rate );
  rate.save( "rate.dat" );

  return 0;
}
