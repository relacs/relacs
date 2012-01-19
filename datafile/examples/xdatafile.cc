/*
  xdatafile.cc
  

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

/*!
g++ -g -Wall -o xdatafile xdatafile.cc -I$HOME/include -L$HOME/lib -loptions
*/

#include <iostream>
#include <relacs/datafile.h>
using namespace std;
using namespace relacs;


int main( void )
{

  DataFile df;
  //  df.open( "/home/efish/data/data/03-04-10-ab/firate.dat" );
  df.open( "test.dat" );
  if ( !df.good() )
    cout << "can't open file" << endl;
  while ( df.read( 2 ) ) {
    if ( df.newMetaData( 1 ) ) {
      cout << df.metaData( 1 );
    }
    for ( int k=0; k<df.levels(); k++ )
      cerr << "level " << k << ": " << df.metaDataCount( k ) << ", " << df.metaDataTotalCount( k ) << endl;
    for ( int k = 0; k<df.data().rows(); k++ ) {
      for ( int j = 0; j<df.data().columns(); j++ ) {
	cout << "  " << df( j, k );
      }
      cout << endl;
    }
    cout << endl;
  };
  cout << "files: " << endl;
  cout << df.dataFile() << endl;
  return 0;
}
