/*
  ramps.cc
  

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
#include <relacs/sampledata.h>
using namespace std;
using namespace relacs;


int main( void )
{
  // some signal:
  SampleDataD signal;
  signal.sin( 0.0, 4.0, 0.01, 20.0 );
  cout << "# original data:\n";
  cout << signal << "\n\n";

  // linear ramps:
  SampleDataD linearramp = signal;
  linearramp.ramp( 1.0 );
  cout << "# linear ramp:\n";
  cout << linearramp << "\n\n";

  // square ramps:
  SampleDataD squareramp = signal;
  squareramp.ramp( 1.0, 1 );
  cout << "# square ramp:\n";
  cout << squareramp << "\n\n";

  // sqrt ramps:
  SampleDataD sqrtramp = signal;
  sqrtramp.ramp( 1.0, 2 );
  cout << "# sqrt ramp:\n";
  cout << sqrtramp << "\n\n";

  // cosine ramps:
  SampleDataD cosineramp = signal;
  cosineramp.ramp( 1.0, 3 );
  cout << "# cosine ramp:\n";
  cout << cosineramp << "\n\n";

  return 0;
}
