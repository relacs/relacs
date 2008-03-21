/*
  xounoise.cc
  

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

#include <cstdio>
#include <cmath>
#include <relacs/random.h>
#include <relacs/spectrum.h>
#include <relacs/sampledata.h>

using namespace numerics;

int main( int argc, char **argv )
{
  // Ohrnstein Uhlenbeck noise:
  SampleDataD noise;
  noise.ouNoise( 10000, 0.0001, 0.01, rnd );

  // power spectrum:
  SampleDataD power( 4096, 0.0, 0.5/noise.stepsize()/4096 );
  realPSD( noise.begin(), noise.end(), power.begin(), power.end(),
	   true, hanning );
  power.save( cout, 0, 5 );

  return 0;
}
