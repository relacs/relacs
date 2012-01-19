/*
  datacolumn.cc
  

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

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <getopt.h>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/tablekey.h>

using namespace std;
using namespace relacs;


vector<int> xcol; 
vector<string> xcols;
int offset = 0;


void readData( DataFile &sf )
{
  sf.readMetaData();

  // find columns:
  if ( sf.newDataKey() ) {
    for ( unsigned int k=0; k<xcols.size(); k++ ) {
      if ( !xcols[k].empty() ) {
	int c = sf.column( xcols[k] );
	if ( c >= 0 )
	  xcol[k] = c;
      }
      cout << xcol[k]+offset << '\n';
    }
  }
  else 
    cerr << "error: no key found\n";

  sf.close();
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "datacolumn -c ### [-c ### [-c ### ....]] [-o ###] fname\n";
  cerr << '\n';
  cerr << "prints out the column numbers for the specified column titles\n";
  cerr << "of the data file <fname>.\n";
  cerr << "First column is '0'.\n";
  cerr << "-c: ### specifies column (default is first column).\n";
  cerr << "    the column can be either a number (1,2,...)\n";
  cerr << "    or a string containing the column title.\n";
  cerr << "-o: An offset that is added to the column numbers.\n";
  cerr << '\n';
  exit( 1 );
}


void readArgs( int argc, char *argv[], int &filec )
{
  int c;

  if ( argc <= 1 )
    WriteUsage();
  optind = 0;
  opterr = 0;
  while ( (c = getopt( argc, argv, "c:o:" )) >= 0 ) {
    switch ( c ) {
      case 'c': if ( optarg != NULL ) {
                  int xc = 0;
	          if ( sscanf( optarg, "%d", &xc ) == 0 ) {
		    xcol.push_back( 0 );
		    xcols.push_back( optarg );
		  }
                  else {
		    xcol.push_back( xc-1 );
		    xcols.push_back( "" );
		  }
                }
                break;
      case 'o': if ( optarg == NULL ||
		     sscanf( optarg, "%d", &offset ) == 0 )
		  offset = 0;
                break;
      default : WriteUsage();
    }
  }
  if ( optind < argc && argv[optind][0] == '?' ) {
    WriteUsage();
  }
  filec = optind;
}


int main( int argc, char *argv[] )
{
  int filec = 0;
  readArgs( argc, argv, filec );

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
  else {
    sf.open( cin );
  }

  readData( sf );

  return 0;
}
