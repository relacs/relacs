/*
  xsampledata.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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
#include <iomanip>
#include <vector>
#include <relacs/sampledata.h>
using namespace std;
using namespace relacs;


int main( void )
{

  SampleDataD sa1( 0.0, 1.0, 0.1 );
  SampleDataD sa2( 0.0, 1.0, 0.1 );
  SampleDataF sa3( 0.0, 1.0, 0.1 );
  ArrayD a1( 10 );
  ArrayF a2( 10 );
  vector< double > v1( 10 );
  vector< float > v2( 10 );
  
  cerr << "\nSampleDataD\n";
  sa1 = sa2;
  cerr << sa1 << '\n';
  cerr << "\nSampleDataF\n";
  sa1 = sa3;
  cerr << sa1 << '\n';
  cerr << "\nArrayD\n";
  sa1 = a1;
  cerr << sa1 << '\n';
  cerr << "\nArrayF\n";
  sa1 = a2;
  cerr << sa1 << '\n';
  cerr << "\nvector< double >\n";
  sa1 = v1;
  cerr << sa1 << '\n';
  cerr << "\nvector< float >\n";
  sa1 = v2;
  cerr << sa1 << '\n';


  /*
  for ( double max=0.9; max<=1.1; max+=0.01 ) {
    SampleData< double > data( 0.0, max, 0.1 );
    cout << "\nmax=" << max << "\n";
    data.save( cout );
  }
  */

  /*
  SampleDataD noise;
  //  noise.whiteNoise( 1.0, 0.0001, 0.0, 100.0, rnd );
  noise.ouNoise( 2.0, 0.0001, 0.005, rnd );
  cout << noise << endl;
  */

  /*
  SampleDataD signal;
  signal.load( "signal.dat" );
  cout << signal << endl;
  */

  return 0;
}
