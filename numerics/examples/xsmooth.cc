/*
  xsmooth.cc
  

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

#include <cstdio>
#include <cmath>
#include <relacs/random.h>
#include <relacs/spectrum.h>
#include <relacs/sampledata.h>
#include <relacs/fitalgorithm.h>
using namespace std;
using namespace relacs;


int main( int argc, char **argv )
{
  // some noise:
  SampleDataD noise;
  noise.whiteNoise( 10.0, 0.005, 0.0, 20.0, rnd );
  cout << "# original data:\n";
  cout << noise << "\n\n";

  // smooth with n=7:
  SampleDataD sn7noise;
  sn7noise.smooth( noise, 7 );
  cout << "# smoothed with n=7:\n";
  cout << sn7noise << "\n\n";

  // smooth with n=7 weights:
  SampleDataD sw7noise;
  ArrayD raweights( 7 );
  raweights = 1.0/7.0;
  sw7noise.smooth( noise, raweights, 3 );
  cout << "# smoothed with n=7 weights:\n";
  cout << sw7noise << "\n\n";

  // smooth with savgol n=7 weights:
  SampleDataD sg7noise;
  ArrayD sgweights;
  savitzkyGolay( sgweights, 7, 3, 0 );
  sg7noise.smooth( noise, sgweights, 3 );
  cout << "# smoothed with savgol n=7 weights:\n";
  cout << sg7noise << "\n\n";

  // smooth with savgol n=7 weights, 2nd order:
  SampleDataD sg72noise;
  ArrayD sg2weights;
  savitzkyGolay( sg2weights, 7, 3, 2 );
  sg72noise.smooth( noise, sg2weights, 3 );
  cout << "# smoothed with savgol n=7 weights, 2nd order:\n";
  cout << sg72noise << "\n\n";

  return 0;
}
