/*
  mm3a.cc
  Example/test for kleindiek mm3a micromanipulator

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
#include <relacs/misc/kleindiek.h>
using namespace std;
using namespace relacs;
using namespace misc;


int main( void )
{
  Kleindiek k( "/dev/ttyS1" );

  //  cout << k << endl;

  /*
  cout << "countermode\n";
  k.countermode( 0, 0 );
  cout << "counterreset\n";
  k.counterreset();
  cout << "counterread\n";
  k.counterread();
  */
  cout << "speed\n";
  k.speed( 0, 13 );
  cout << "amplitude\n";
  k.amplitudepos( 0, 80 );
  k.amplitudeneg( 0, 80 );
  cout << "coarse\n";
  k.coarse( 0, 2000 );
  /*
  cout << "counterread\n";
  k.counterread();
  */

}
