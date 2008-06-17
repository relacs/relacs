/*
  xspectrum.cc
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
  /*
  double f=5.0;
  SampleData data( 4096, 0.0, 0.01 );
  for ( int k=0; k<data.size(); k++ )
    data[k] = cos( 6.28318530717959*f*data.pos( k ) );
  cout << "power signal: " << data.power() << endl;
  data.writeText( "signal.dat" );

  SampleData spec;
  periodogram( data, spec );
  cout << "power periodogram: " << spec.sum() << endl;
  spec.writeText( "period.dat" );

  spectrum( data, spec, WindowData( 1024 ), false );
  cout << "power spectrum: " << spec.sum() << endl;
  spec.writeText( "spec.dat" );
  */

  /*
  // Create data array with the sum of two sine waves at 50 and 100 Hz:
  SampleDataD data( 4096*16, 0.0, 0.00005 );
  for ( int k=0; k<data.size(); k++ )
    data[k] = sin( 2.0*M_PI*50.0*data.pos( k ) ) + 0.5*sin(  2.0*M_PI*100.0*data.pos( k )  );

  // simple fourier transform:
  SampleDataD fourier( data );
  rFFT( fourier );
  cout << 0.0 << " " << fourier[0]*fourier[0] << '\n';
  for ( int k=1; k<data.size()/2; k++ )
    cout << k/data.stepsize()/data.size() << " " << ::sqrt( fourier[k]*fourier[k] + fourier[fourier.size()-k]*fourier[fourier.size()-k] ) << '\n';
  cout << 0.5/data.stepsize() << " " << fourier[data.size()/2]*fourier[data.size()/2] << '\n';
  cout << "\n\n";

  // backwards transformation:
  hcFFT( fourier );
  fourier /= fourier.size();  // normalization!
  for ( int k=0; k<data.size(); k++ )
    cout << data.pos( k ) << " " << data[k] << " " << fourier[k] << '\n';
  cout << "\n\n";

  // power spectrum:
  SampleDataD spec( 4096, 0.0, 0.5/data.stepsize()/4096 );
  psd( data, spec, true, hanning );
  cout << spec << endl;
  */

  // Create data array with a sine wave at 100 Hz:
  SampleDataD data( 4096*16, 0.0, 0.00005 );
  for ( int k=0; k<data.size(); k++ )
    data[k] = 1.0*sin(  2.0*M_PI*100.0*data.pos( k )  );

  // power spectrum:
  SampleDataD power( 4096, 0.0, 0.5/data.stepsize()/4096 );
  rPSD( data.begin(), data.end(), power.begin(), power.end(),
	true, hanning );
  cout << power << endl;

  return 0;
}
