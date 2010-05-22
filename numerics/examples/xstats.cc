/*
  xstats.cc
  check whether the functions provided in stats.h compie with various types.

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

#include <cstdio>
#include <vector>
#include <relacs/stats.h>
#include <relacs/array.h>
#include <relacs/sampledata.h>
#include <relacs/map.h>
using std::vector;
using namespace relacs;

template< typename T >
void testfunc( T &x, T &y )
{
  typename T::value_type v = 0;
  typename T::value_type w = 0;
  typename numerical_container_traits<T>::mean_type a = 0;
  typename numerical_container_traits<T>::variance_type s = 0;
  int i = 0;
  int j = 0;

  v = min( x );
  v = min( i, x );
  i = minIndex( x );
  v = max( x );
  v = max( i, x );
  i = maxIndex( x );
  minMax( v, w, x );
  minMax( v, i, w, j, x );
  minMaxIndex( i, j, x );
  v = minAbs( x );
  v = maxAbs( x );
  i = clip( 0.0, 1.0, x );

  a = mean( x );
  a = wmean( x, y );
  a = smean( x, y );
  a = meanStdev( s, x );
  a = wmeanStdev( s, x, y );
  a = smeanStdev( s, x, y );

  s = variance( x );
  s = varianceKnown( a, x );
  s = varianceFixed( a, x );
  s = wvarianceKnown( a, x, y );

  s = stdev( x );
  s = stdevKnown( a, x );
  s = stdevFixed( a, x );
  s = wstdevKnown( a, x, y );

  s = absdev( x );
  s = absdevKnown( a, x );
  s = wabsdev( x, y );
  s = wabsdevKnown( a, x, y );

  s = rms( x );
  s = skewness( x );
  s = kurtosis( x );
  v = sum( x );
  s = squaredSum( x );
  s = magnitude( x );
  s = power( x );
  s = dot( x, y );

  v = median( x );
  v = quantile( 0.25, x );
  v = rank( x );
}

int main( int argc, char **argv )
{
  ArrayD a, b;
  a.rand( 10000, rnd );
  b.rand( 10000, rnd );
  testfunc( a, b );

  SampleDataF c, d;
  c = sin( 0.0, 1.0, 0.001, 100.0 );
  d = cos( 0.0, 1.0, 0.001, 50.0 );
  testfunc( c, d );

  vector< int > e( 100 );
  vector< int > f( 100 );
  for ( unsigned int k=0; k<e.size(); k++ ) {
    e[k] = k+1;
    f[k] = f.size()-k;
  }
  testfunc( e, f );

  Array< unsigned short > g( 100 );
  Array< unsigned short > h( 100 );
  for ( int k=0; k<g.size(); k++ ) {
    g[k] = k+1;
    h[k] = h.size()-k;
  }
  testfunc( g, h );

  return 0;
}
