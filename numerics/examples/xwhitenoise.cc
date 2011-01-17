/*
  xwhitenoise.cc
  

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
using namespace std;
using namespace relacs;


int main( int argc, char **argv )
{
  // Gaussian white noise:
  SampleDataD noise;
  //  noise.whiteNoise( 1000000, 0.0001, 0.0, 100.0, rnd );
  noise.whiteNoise( 0.5, 0.0001, 0.0, 100.0, rnd );

  cerr << "mean: " << mean( noise ) << endl;
  cerr << "standard deviation: " << stdev( noise ) << endl;
  /*
  // power spectrum:
  SampleDataD power( 4096, 0.0, 0.5/noise.stepsize()/4096 );
  rPSD( noise.begin(), noise.end(), power.begin(), power.end(),
	true, hanning );
  power.save( cout, 0, 5 );
  */

  return 0;
}
