/*
  fixdata.cc
  

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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <relacs/datafile.h>
#include <relacs/options.h>
using namespace std;
using namespace relacs;


int stopempty = 1;
bool fixempty = false;


void fixData( DataFile &sf )
{
  Options prevopt;
  int prevemptylines;

  while ( sf.readMetaData() > 0 ) {

    // data file:
    if ( sf.newDataFile() ) {
      cout << sf.dataFile();
      if ( fixempty )
	cout << '\n';
      else {
	for ( int j=0; j<sf.emptyLines( DataFile::DataFileLevel ); j++ )
	  cout << '\n';
      }
    }

    int pageemptylines = prevemptylines;;

    // check for doubled meta data at the beginning of read meta data:
    int firstdoubled = sf.newLevels();
    if ( sf.newMetaData( 0 ) ) {
      for ( int k=sf.newLevels()-1; k>0; k-- ) {
	Options &opt = sf.metaDataOptions( k );
	if ( prevopt.size() != opt.size() )
	  break;
	bool equal = true;
	for ( int j=0; j<opt.size(); j++ )
	  if ( ! ( prevopt[j] == opt[j] ) ) {
	    equal = false;
	    break;
	  }
	if ( equal )
	  firstdoubled = k;
      }
    }

    // check for doubled meta data at the end of the read meta data:
    int lastdoubled = 0;
    if ( sf.newMetaData( 0 ) ) {
      Options &lastopt = sf.metaDataOptions( 0 );
      for ( int k=1; k<sf.levels() && sf.newMetaData( k ); k++ ) {
	Options &opt = sf.metaDataOptions( k );
	if ( lastopt.size() != opt.size() )
	  break;
	bool equal = true;
	for ( int j=0; j<opt.size(); j++ )
	  if ( ! ( lastopt[j] == opt[j] ) ) {
	    equal = false;
	    break;
	  }
	if ( equal ) {
	  lastdoubled = k;
	  if ( sf.dataLine() )
	    pageemptylines = sf.emptyLines( 0 );
	}
      }
    }

    // print meta data:
    if ( sf.newDataKey() && sf.dataKeyIndex() == sf.metaDataIndex( sf.newLevels()-1 ) - 1 ) {
      cout << sf.dataKey();
      int el = fixempty ? ( sf.newLevels()>0 ? 1 : 0 ) : sf.emptyLines( DataFile::DataKeyLevel );
      for ( int j=0; j<el; j++ )
	cout << '\n';
    }
    for ( int k=sf.newLevels()-1; k>=0; k-- ) {

      cout << sf.metaData( k );

      bool doubled = ( ( ( k>0 || ( k==0 && !sf.dataLine() ) ) &&
			 k <= lastdoubled ) ||
		       k >= firstdoubled );

      if ( doubled ) {
	bool key = ( sf.newDataKey() &&
		     sf.dataKeyIndex() == sf.metaDataIndex( 0 )+1 );
	int el = fixempty ? ( key ? 1 : 0 ) : pageemptylines;
	for ( int j=0; j<el; j++ )
	  cout << '\n';
	if ( key ) {
	  cout << sf.dataKey();
	  el = fixempty ? 0 : sf.emptyLines( DataFile::DataKeyLevel );
	  for ( int j=0; j<el; j++ )
	    cout << '\n';
	}
	cout << "-0\n";
      }

      bool key = ( sf.newDataKey() && sf.dataKeyIndex() == sf.metaDataIndex( k )+1 );
      int el = fixempty && !doubled ? 1 : sf.emptyLines( k );
      if ( fixempty && k == 0 && !key )
	el = 0;
      for ( int j=0; j<el; j++ )
	cout << '\n';

      if ( key ) {
	cout << sf.dataKey();
	int el = fixempty ? ( k>0 ? 1 : 0 ) : sf.emptyLines( DataFile::DataKeyLevel );
	for ( int j=0; j<el; j++ )
	  cout << '\n';
      }
    }

    // read data:
    do {
      if ( sf.good() && sf.emptyLine() ) {
	sf.readEmptyLines();
	for ( int j=0; j<sf.emptyLines(); j++ )
	  cout << '\n';
	if ( sf.emptyLines() >= stopempty )
	  break;
      }
      cout << sf.line() << '\n';
    } while ( sf.getline() );

    prevopt = sf.metaDataOptions( 0 );
    prevemptylines = sf.emptyLines( 0 );

  };
}


void writeUsage()
{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "fixdata [-e ##] [-f] [sourcefile [outfile]]\n";
  cerr << '\n';
  cerr << "Reads in <sourcefile> and fixes it for forgetten data\n";
  cerr << "by inserting '-0' data.\n";
  cerr << "If <outfile> or <infile> are not specified,\n";
  cerr << "output is written to stdout, and input is read from stdin.\n";
  cerr << "The fixed file is written to <outfile>.\n";
  cerr << "  -e: the number of empty lines separating sets of data (default 1)\n";
  cerr << "  -f: fix the number of empty lines between blocks of meta data to one.\n";
  cerr << '\n';
  exit( 1 );
}


void readArgs( int argc, char *argv[], int *filec )
{
  int c;

  optind = 0;
  opterr = 0;
  while ( (c = getopt( argc, argv, "e:f" )) >= 0 )
    switch ( c ) {
    case 'e' :
      sscanf( optarg, " %d", &stopempty );
      if ( stopempty < 1 )
	stopempty = 1;
      break;
    case 'f' :
      fixempty = true;
      break;
    default : writeUsage();
    }
  if ( optind < argc && argv[optind][0] == '?' )
    writeUsage();
  *filec = optind;
}


int main( int argc, char *argv[] )
{
  int filec;

  readArgs( argc, argv, &filec );

  // redirect cin:
  DataFile sf;
  if ( argc > filec ) {
    sf.open( argv[filec] );
    if ( !sf.good() ) {
      cerr << "! can't open file " << argv[filec] << " for reading\n";
      return 1;
    }
    filec++;
  }
  else
    sf.open( cin );

  // redirect cout:
  streambuf *coutb = 0;
  ofstream df;
  if ( argc > filec ) {
    df.open( argv[filec] );
    if ( !df.good() ) {
      cerr << "! can't open file " << argv[filec] << " for writing\n";
      return 1;
    }
    coutb = cout.rdbuf();
    streambuf *sb = df.rdbuf();
    cout.rdbuf( sb );
  }

  // process data:
  fixData( sf );

  // close files:
  sf.close();
  if ( coutb != 0 ) {
    df.close();
    cout.rdbuf( coutb );
  }

  return 0;
}
