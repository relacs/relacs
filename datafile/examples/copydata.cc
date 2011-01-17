/*
  copydata.cc
  

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

#include <iostream>
#include <relacs/datafile.h>
using namespace relacs;


int main( int argc, char *argv[] )
{
  if ( argc <= 1 )
    return 1;

  int stopempty = 1;

  DataFile sf( argv[1] );

  while ( sf.readMetaData() > 0 && sf.good() ) {

    // data file block:
    if ( sf.newDataFile() ) {
      cout << sf.dataFile();
      for ( int j=0; j<sf.emptyLines( DataFile::DataFileLevel ); j++ )
	cout << endl;
    }

    // data key before any new meta data blocks:
    if ( sf.newDataKey() && 
	 sf.dataKeyIndex() == sf.metaDataIndex( sf.newLevels()-1 ) - 1 ) {
      cout << sf.dataKey();
      for ( int j=0; j<sf.emptyLines( DataFile::DataKeyLevel ); j++ )
	cout << endl;
    }

    // meta data blocks:
    for ( int k=sf.newLevels()-1; k>=0; k-- ) {
      cout << sf.metaData( k );
      for ( int j=0; j<sf.emptyLines( k ); j++ )
	cout << endl;

      // data key block following meta data block:
      if ( sf.newDataKey() && 
	   sf.dataKeyIndex() == sf.metaDataIndex( k )+1 ) {
	cout << sf.dataKey();
	for ( int j=0; j<sf.emptyLines( DataFile::DataKeyLevel ); j++ )
	  cout << endl;
      }
    }

    // read data:
    do {
      if ( sf.emptyLine() ) {
	// read in empty lines at once:
	sf.readEmptyLines();
	// print them out:
	for ( int j=0; j<sf.emptyLines(); j++ )
	  cout << endl;
	// end of data block:
	if ( sf.emptyLines() >= stopempty )
	  break;
      }
      // print data and meta data lines:
      cout << sf.line() << endl;
    // read in next line:
    } while ( sf.getline() );

  }

  sf.close();
	
}
