/*
  xinterpolation.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
  noise.whiteNoise( 10.0, 0.005, 0.0, 20.0, rnd );
  cout << "# original data:\n";
  cout << noise << "\n\n";

  // interpolate from SampleData to SampleData:
  SampleDataD isnoise( 1.0, 11.0, 0.001 );
  isnoise.interpolate( noise );
  cout << "# interpolated from sampledata:\n";
  cout << isnoise << "\n\n";

  // interpolate from SampleData with new stepsize:
  SampleDataD ssnoise;
  ssnoise.interpolate( noise, 0.002 );
  cout << "# interpolated from sampledata with new stepsize:\n";
  cout << ssnoise << "\n\n";

  // interpolate from SampleData with new stepsize and offset:
  SampleDataD sosnoise;
  sosnoise.interpolate( noise, 3.0, 0.002 );
  cout << "# interpolated from sampledata with new offset and stepsize:\n";
  cout << sosnoise << "\n\n";

  // interpolate from SampleData with new range:
  SampleDataD rsnoise;
  rsnoise.interpolate( noise, LinearRange( 1.0, 4.0, 0.01 ) );
  cout << "# interpolated from sampledata with new range:\n";
  cout << rsnoise << "\n\n";

  // convert to map:
  MapD mnoise( noise );
  cout << "# as map:\n";
  cout << mnoise << "\n\n";

  // interpolate from Map to SampleData:
  SampleDataD imnoise( 1.0, 11.0, 0.001 );
  imnoise.interpolate( mnoise );
  cout << "# interpolated from map:\n";
  cout << imnoise << "\n\n";

  // interpolate from Map to SampleData with new range:
  SampleDataD rmnoise( 1.0, 11.0, 0.001 );
  rmnoise.interpolate( mnoise, LinearRange( 2.0, 6.0, 0.01 ) );
  cout << "# interpolated from map with new range:\n";
  cout << rmnoise << "\n\n";

  return 0;
}
