/*
  xinterpolation.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <cstdio>
#include <cmath>
#include <relacs/random.h>
#include <relacs/spectrum.h>
#include <relacs/sampledata.h>
#include <relacs/map.h>
using namespace std;
using namespace relacs;


int main( int argc, char **argv )
{
  // some noise:
  SampleDataD noise;
  noise.whiteNoise( 100.0, 0.005, 0.0, 20.0, rnd );
  cout << noise << "\n\n";

  // convert to map:
  MapD mnoise( noise );
  cout << mnoise << "\n\n";

  // interpolate to SampleData:
  SampleDataD inoise( 10.0, 110.0, 0.001 );
  inoise.interpolate( mnoise );
  cout << inoise << "\n\n";

  return 0;
}
