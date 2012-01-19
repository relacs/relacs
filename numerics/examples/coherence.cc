/*
  coherence.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
#include <fstream>
#include <relacs/sampledata.h>
#include <relacs/spectrum.h>
using namespace std;
using namespace relacs;


int main ( void )
{
  int n = 100000;

  SampleDataD signal;
  //  signal.whiteNoise( n, 0.001, 0.0, 400.0, rnd );
  signal.ouNoise( n, 0.001, 0.001, rnd );

  SampleDataD response( signal );

  //response.whiteNoise( n, 0.001, 0.0, 400.0, rnd );

  //  response = 1.0;

  // low pass filter:
  double tau = 0.01;
  double rho = ::exp( -response.stepsize()/tau );
  double sqrho = ::sqrt( 1.0 - rho*rho );
  double x = response[0];
  for ( SampleDataD::iterator iter1 = response.begin()+1;
	iter1 != response.end();
	++iter1 ) {
    x = rho * x + sqrho * (*iter1);
    *iter1 = x;
  }

  // add some noise:
  SampleDataD noise( signal.range() );
  noise.randNorm( n, rnd );
  noise *= 0.5;
  response += noise;
  int nfft = 1024;

  SampleDataD fgain( nfft );
  SampleDataD cohere( nfft );
  SampleDataD crossspec( nfft );
  SampleDataD signalspec( nfft );
  SampleDataD responsespec( nfft );
  spectra( signal, response, fgain, cohere, crossspec, signalspec, responsespec, false );
  //  gain( signal, response, fgain, false );
  //  coherence( signal, response, cohere, false );
  //  rCSD( signal, response, cohere, false );
  //  SampleDataD trans( 2*nfft );
  //  transfer( signal, response, trans, false );
  //  hcMagnitude( trans, fgain );

  for ( int k=0; k<fgain.size(); k++ )
    cout << fgain.pos( k )
	 << "  " << fgain[k]
	 << "  " << cohere[k]
	 << "  " << crossspec[k]
	 << "  " << signalspec[k]
	 << "  " << responsespec[k] << '\n';

  return 0;
}
