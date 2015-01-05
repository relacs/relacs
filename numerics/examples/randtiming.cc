/*
  randtiming.cc
  

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

#include <ctime>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <relacs/random.h>
using namespace std;
using namespace relacs;


int main()
{
  time_t t1, t2;
  double r;
  const int max = 1000000000;

  //  Ran3 rnd1;
  Ran3 ran3;
  RandomBase &rnd1 = ran3;
  t1 = time( 0 );
  for ( int k=0; k<max; k++ ) {
    r = rnd1.gaussian();
  }
  t2 = time( 0 );
  cerr << "ran3 gaussian: " << difftime( t2, t1 ) << "sec\n"; 
  t1 = time( 0 );
  for ( int k=0; k<max; k++ ) {
    r = rnd1.uniform();
  }
  t2 = time( 0 );
  cerr << "ran3 uniform: " << difftime( t2, t1 ) << "sec\n"; 

  //  RandomGSL rnd2( gsl_rng_taus );
  RandomGSL rangsl;
  RandomBase &rnd2 = rangsl;
  t1 = time( 0 );
  for ( int k=0; k<max; k++ ) {
    r = rnd2.gaussian();
  }
  t2 = time( 0 );
  cerr << "taus gaussian: " << difftime( t2, t1 ) << "sec\n"; 
  t1 = time( 0 );
  for ( int k=0; k<5*max; k++ ) {
    r = rnd2.uniform();
  }
  t2 = time( 0 );
  cerr << "taus uniform: " << difftime( t2, t1 ) << "sec\n"; 

  /*
  Rand55 rnd3;
  t1 = time( 0 );
  for ( int k=0; k<max; k++ ) {
    r = rnd3.gaussian();
  }
  t2 = time( 0 );
  cerr << "rand55: " << difftime( t2, t1 ) << "sec\n"; 
  */
  /*
other way of time measurement:
#include <sys/time.h>
  timeval tv, tv2;
  gettimeofday(&tv, 0);

  double x = 0.0;
  for ( int k=0; k<10000000; k++ )
    x = static_cast<double>(4);

  gettimeofday(&tv2, 0);
  long long l = (tv2.tv_sec - tv.tv_sec) * 1000000 + tv2.tv_usec - tv.tv_usec;
  cout << l << "us\n";
  */

  return 0;
}

