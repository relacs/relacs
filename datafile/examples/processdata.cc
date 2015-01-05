/*
  processdata.cc
  

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
#include <relacs/datafile.h>
using namespace relacs;


int main( int argc, char *argv[] )
{
  if ( argc <= 1 )
    return 1;

  int stopempty = 1;

  DataFile sf( argv[1] );

  while ( sf.readMetaData() > 0 && sf.good() ) {

    // ... process meta data ...

    // read data:
    sf.initData();
    do {
      // ... process data line ...
    } while ( sf.readDataLine( stopempty ) );

    // ... process data block and meta data ...

  }

  sf.close();
	
}
