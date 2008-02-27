/*
  xtranslate.cc
  

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

#include <vector>
#include "str.h"
#include "options.h"
using namespace std;


int main( int argc, char *argv[] )
{
  vector< Options > opt( 3 );
  opt[0].addNumber( "num0", 0.12, "ms" );
  opt[0].addText( "text0", "monkey" );
  opt[1].addNumber( "num1", 1.23, "Hz" );
  opt[1].addText( "text1", "donkey" );
  opt[2].addNumber( "num2", 2.34, "m" );
  opt[2].addText( "text2", "elephant" );

  for ( unsigned int k=0; k<opt.size(); k++ ) {
    cout << "options " << k << ":\n";
    cout << opt[k] << endl;
  }

  while ( true ) {
    Str ss;
    cout << "enter string to be translated: ";
    getline( cin, ss );
    cout << "translated: " 
	 << ss.translated( opt, "pst" ) << endl;
  }

  return 0;
}
