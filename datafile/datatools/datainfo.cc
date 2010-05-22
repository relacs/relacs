/*
  datainfo.cc
  

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


void dataInfo( DataFile &sf )
{
  int nd = 0;
  int ndb[2] = { 0, 0 };
  cout << "# " << Str( "inx1", 5 ) << "  " << Str( "inx2", 5 );
  cout << "  " << Str( "meta", 5 ) << "  " << Str( "data", 5 ) << "  " << Str( "empty", 5 ) << '\n';
  while ( sf.read( stopempty, 0 ) ) {

    int ml;
    for ( ml=0; ml<sf.levels() && sf.newMetaData( ml ); ml++ );

    cout << "  " << Str( ndb[1], 5 );
    cout << "  " << Str( ndb[0], 5 );
    cout << "  " << Str( ml, 5 );
    cout << "  " << Str( sf.dataLines(), 5 );
    cout << "  " << Str( sf.emptyLines(), 5 );
    cout << '\n';
    for ( int k=stopempty; k<sf.emptyLines(); k++ )
      cout << '\n';

    ndb[0]++;
    if ( sf.emptyLines() > stopempty ) {
      ndb[0] = 0;
      ndb[1]++;
    }
    nd++;

  };
}


void writeUsage()
{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "datainfo [-e ##] [sourcefile [outfile]]\n";
  cerr << '\n';
  cerr << "Reads in <sourcefile> and prints information about meta data.\n";
  cerr << "If <outfile> or <infile> are not specified,\n";
  cerr << "output is written to stdout, and input is read from stdin.\n";
  cerr << "The infos are written to <outfile>.\n";
  cerr << "  -e: the number of empty lines separating sets of data (default 1)\n";
  cerr << '\n';
  exit( 1 );
}


void readArgs( int argc, char *argv[], int *filec )
{
  int c;

  optind = 0;
  opterr = 0;
  while ( (c = getopt( argc, argv, "e:" )) >= 0 )
    switch ( c ) {
    case 'e' :
      sscanf( optarg, " %d", &stopempty );
      if ( stopempty < 1 )
	stopempty = 1;
      break;
    default : 
      writeUsage();
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
  dataInfo( sf );

  // close files:
  sf.close();
  if ( coutb != 0 ) {
    df.close();
    cout.rdbuf( coutb );
  }

  return 0;
}
