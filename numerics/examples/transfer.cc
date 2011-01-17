/*
  transfer.cc
  Example for computing a transfer function.

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

#include <cmath>
#include <iostream>
#include <relacs/sampledata.h>
#include <relacs/random.h>
#include <relacs/spectrum.h>
using namespace std;
using namespace relacs;


int main( int argc, char **argv )
{
  const int n=4096;

  // the stimulus:
  SampleDataD s;
  s.whiteNoise( n*16, 0.0005, 0.0, 1000.0, rnd );

  // low pass filtered response:
  const double tau = 0.01;
  SampleDataD r( s );
  double x = r[0];
  for ( int k=1; k<r.size(); k++ ) {
    x += (-x + r[k])*r.stepsize()/tau;
    r[k] = x;
  }

  // transfer function:
  SampleDataD h( n );
  transfer( s, r, h );

  // gain and phase:
  SampleDataD gain1( h.size()/2 );
  SampleDataD phase1( h.size()/2 );
  hcMagnitude( h, gain1 );
  hcPhase( h, phase1 );

  // the gain function:
  SampleDataD gain2( h.size()/2 );
  gain( s, r, gain2 );

  for ( int k=0; k<gain1.size(); k++ ) {
    double f = gain1.pos(k);
    double wt = 2.0*M_PI*f*tau;
    cout << f << "  " << gain1[k] << "  " << phase1[k]
	 << "  " << 1.0/::sqrt(1.0+wt*wt) << "  " << ::atan(-wt)
	 << "  " << gain2[k] << '\n';
  }

  return 0;
}
