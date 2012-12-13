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
  signal.whiteNoise( n, 0.001, 0.0, 400.0, rnd );
  //  signal.ouNoise( n, 0.001, 0.001, rnd );

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

  SampleDataD fgain1( nfft );
  SampleDataD cohere1( nfft );
  SampleDataD crossspec1( nfft );
  SampleDataD signalspec1( nfft );
  SampleDataD responsespec1( nfft );
  spectra( signal, response, fgain1, cohere1, crossspec1, signalspec1, responsespec1 );

  SampleDataD signalspec2( nfft );
  rPSD( signal, signalspec2 );

  SampleDataD responsespec2( nfft );
  rPSD( response, responsespec2 );

  SampleDataD fgain2( nfft );
  SampleDataD trans2( 2*nfft );
  transfer( signal, response, trans2 );
  hcMagnitude( trans2, fgain2 );

  SampleDataD fgain3( nfft );
  gain( signal, response, fgain3 );

  SampleDataD cohere2( nfft );
  coherence( signal, response, cohere2 );

  SampleDataD crossspec2( nfft );
  rCSD( signal, response, crossspec2 );

  SampleDataD fgain4( nfft );
  SampleDataD cohere4( nfft );
  SampleDataD responsespec4( nfft );
  spectra( signal, response, fgain4, cohere4, responsespec4 );

  SampleDataD crossspec5( 2*nfft );
  SampleDataD signalspec5( nfft );
  SampleDataD responsespec5( nfft );
  crossSpectra( signal, response, crossspec5, signalspec5, responsespec5 );
  SampleDataD crossspec6( nfft );
  hcPower( crossspec5, crossspec6 );
  SampleDataD cohere6( nfft );
  //  cohere6 = crossspec6/signalspec5/responsespec5;
  coherence( crossspec5, signalspec5, responsespec5, cohere6 );

  for ( int k=0; k<fgain1.size(); k++ )
    cout << fgain1.pos( k )
	 << "  " << fgain1[k]
	 << "  " << cohere1[k]
	 << "  " << crossspec1[k]
	 << "  " << signalspec1[k]
	 << "  " << responsespec1[k]
	 << "  " << signalspec2[k]
	 << "  " << responsespec2[k]
	 << "  " << fgain2[k]
	 << "  " << fgain3[k]
	 << "  " << cohere2[k]
	 << "  " << crossspec2[k]
	 << "  " << fgain4[k]
	 << "  " << cohere4[k]
	 << "  " << responsespec4[k]
	 << "  " << crossspec6[k]
	 << "  " << signalspec5[k]
	 << "  " << responsespec5[k]
	 << "  " << cohere6[k]
	 << '\n';

  return 0;
}
