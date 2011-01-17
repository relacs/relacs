/*
  indatatimeindex.cc
  

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

#include <cstdio>
#include <relacs/str.h>
#include <relacs/random.h>
#include <relacs/indata.h>
using namespace std;
using namespace relacs;


int countt = 0;
int countl1 = 0;
int countl2 = 0;
int countln = 0;
int countg1 = 0;
int countg2 = 0;
int countgn = 0;


double pos( int i, double step )
{
  return step*i;
}


int index( double pos, double step )
{
  return int( ::floor( pos/step + 1e-6 ) );
}


void checkTime( InData &data, int iindex ) 
{
  if ( iindex < 0 )
    return;

  countt++;
  int inx = iindex;
  //  double t = pos( inx, data.stepsize() );
  double t = data.pos( inx );
  //  cout << Str( inx, 10 ) << " " << Str( t, 8, 4, 'f' ) << '\n';

  int inx1 = inx;
  double t1 = t;
  int inx2 = inx;
  double t2 = t;
  int inxn = inx;
  double tn = t;
  for ( int k=0; k < 5; k++ ) {
    //    inxn = index( tn, data.stepsize() );
    //    tn = pos( inxn, data.stepsize() );
    inxn = data.index( tn );
    tn = data.pos( inxn );
    if ( k==0 ) {
      inx1 = inxn;
      t1 = tn;
      if ( inx1 < inx )
	countl1++;
      else if ( inx1 > inx )
	countg1++;
    }
    else if ( k==1 ) {
      inx2 = inxn;
      t2 = tn;
      if ( inx2 < inx1 )
	countl2++;
      else if ( inx2 > inx1)
	countg2++;
    }
    //    cout << Str( inxn, 10 ) << " " << Str( tn, 8, 4, 'f' ) << '\n';
  }
  if ( inxn < inx2 )
    countln++;
  else if ( inxn > inx2 )
    countgn++;
  //  cout << '\n';
}


void checkSignal( InData &data, int iindex ) 
{
  if ( iindex < 0 )
    return;

  countt++;
  int inx = iindex;
  data.setSignalIndex( iindex );
  double t = data.signalTime();

  int inx1 = inx;
  double t1 = t;
  int inx2 = inx;
  double t2 = t;
  int inxn = inx;
  double tn = t;
  for ( int k=0; k < 5; k++ ) {
    data.setSignalTime( tn );
    inxn = data.signalIndex();
    tn = data.signalTime();
    if ( k==0 ) {
      inx1 = inxn;
      t1 = tn;
      if ( inx1 < inx )
	countl1++;
      else if ( inx1 > inx )
	countg1++;
    }
    else if ( k==1 ) {
      inx2 = inxn;
      t2 = tn;
      if ( inx2 < inx1 )
	countl2++;
      else if ( inx2 > inx1)
	countg2++;
    }
  }
  if ( inxn < inx2 )
    countln++;
  else if ( inxn > inx2 )
    countgn++;
}


int main( int argc, char **argv )
{
  InData data( 1000, 0.001 );

  countt = 0;
  countl1 = 0;
  countl2 = 0;
  countln = 0;
  countg1 = 0;
  countg2 = 0;
  countgn = 0;
  for ( int k=0; k<1000000; k++ )
    checkTime( data, rnd.integer() );
  cout << "time:\n";
  cout << " first: less=" << countl1 << "(" << 100.0*(double)countl1/countt << "%)"
       << " more=" << countg1 << "(" << 100.0*(double)countg1/countt << "%)" << '\n';
  cout << "second: less=" << countl2 << "(" << 100.0*(double)countl2/countt << "%)"
       << " more=" << countg2 << "(" << 100.0*(double)countg2/countt << "%)" << '\n';
  cout << "  last: less=" << countln << "(" << 100.0*(double)countln/countt << "%)"
       << " more=" << countgn << "(" << 100.0*(double)countgn/countt << "%)" << '\n';

  cout << '\n';
  countt = 0;
  countl1 = 0;
  countl2 = 0;
  countln = 0;
  countg1 = 0;
  countg2 = 0;
  countgn = 0;
  for ( int k=0; k<1000000; k++ )
    checkSignal( data, rnd.integer() );
  cout << "signal:\n";
  cout << " first: less=" << countl1 << "(" << 100.0*(double)countl1/countt << "%)"
       << " more=" << countg1 << "(" << 100.0*(double)countg1/countt << "%)" << '\n';
  cout << "second: less=" << countl2 << "(" << 100.0*(double)countl2/countt << "%)"
       << " more=" << countg2 << "(" << 100.0*(double)countg2/countt << "%)" << '\n';
  cout << "  last: less=" << countln << "(" << 100.0*(double)countln/countt << "%)"
       << " more=" << countgn << "(" << 100.0*(double)countgn/countt << "%)" << '\n';

  return 0;
}
