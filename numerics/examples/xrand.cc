/*
  xrand.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
#include <iomanip>
#include <relacs/random.h>
using namespace std;
using namespace relacs;


int main()
{
  Ran3 rnd1;
  RandomGSL rnd2( gsl_rng_taus );
  for ( int k=0; k<10000; k++ ) {
    cout << setw( 10 ) << rnd1();
    //    cout << setw( 10 ) << rnd2();
    cout << '\n';
  }
  return 0;
}
