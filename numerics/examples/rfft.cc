/*
  rfft.cc
  Example for fourier transformation of real numbers.

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

#include <cmath>
#include <iostream>
#include <relacs/sampledata.h>
#include <relacs/spectrum.h>
using namespace std;
using namespace relacs;


int main( int argc, char **argv )
{
  // Create data array with the sum of two sine waves at 50 and 100 Hz:
  SampleDataD data( 4096*16, 0.0, 0.00005 );
  for ( int k=0; k<data.size(); k++ )
    data[k] = sin( 2.0*M_PI*50.0*data.pos( k ) ) + 0.5*sin(  2.0*M_PI*100.0*data.pos( k )  );

  // simple fourier transform:
  SampleDataD fourier( data );
  rFFT( fourier );

  // backwards transformation:
  hcFFT( fourier );
  fourier /= fourier.size();  // normalization!
  for ( int k=0; k<data.size(); k++ )
    cout << data.pos( k ) << " " << data[k] << " " << fourier[k] << '\n';
  cout << "\n\n";

  return 0;
}
