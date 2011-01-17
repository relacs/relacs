/*
  xspectrum.cc
  

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
#include <relacs/spectrum.h>
using namespace std;
using namespace relacs;


int main( int argc, char **argv )
{
  // Create data array with a sine wave at 100 Hz:
  const int n=4096;
  SampleDataD data( n*16, 0.0, 0.00005 );
  for ( int k=0; k<data.size(); k++ )
    //    data[k] = 1.0*sin( 2.0*M_PI*100.0*data.pos( k )  );
    data[k] = sin( 2.0*M_PI*50.0*data.pos( k ) ) + 0.5*sin(  2.0*M_PI*100.0*data.pos( k )  );
  cerr << "Power of data (mean squared amplitudes): " << power( data ) << '\n';

  // power spectrum:
  SampleDataD powera( n, 0.0, 0.5/data.stepsize()/n );
  rPSD( data.begin(), data.end(), powera.begin(), powera.end(),
	false, hanning );
  cerr << "Power of powera (sum of power spectrum): " << sum( powera ) << '\n';
  cout << powera << "\n\n";

  // power spectrum of sample data:
  SampleDataD powersd( n );
  rPSD( data, powersd, false, hanning );
  cerr << "Power of powersd (sum of power spectrum): " << sum( powersd ) << '\n';
  cout << powersd << "\n\n";

  // power spectrum directly from fourier transform:
  SampleDataD datafft( data );
  datafft.resize( 2*n );
  cerr << "Size of subset of data: " << datafft.size() << '\n';
  cerr << "Power of subset of data (mean squared amplitudes): " << power( datafft ) << '\n';
  rFFT( datafft );
  SampleDataD powerfft( n );
  hcPower( datafft, powerfft );
  cerr << "Power of powerfft (sum of power spectrum): " << sum( powerfft ) << '\n';
  cout << powerfft << "\n\n";

  return 0;
}
