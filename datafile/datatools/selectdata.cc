/*
  selectdata.cc
  

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
#include <string>
#include <getopt.h>
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/datafile.h>
using namespace std;
using namespace relacs;

int stopempty = 100;
bool erase = false;
bool metadata = true;
bool repeatkey = false;
string indices;
Str lines;
string outfile;


int readData( DataFile &sf )
{
  bool failed = true;

  // split indices:
  StrQueue istrs( indices, ":" );
  int n = istrs.size();

  // no indices:
  if ( n==1 && istrs[0].empty() )
    n = 0;

  // read in index ranges:
  vector< vector< int > > linx( n );
  vector< Options > optinx( n );
  for ( int k=0; k<n; k++ ) {
    istrs[k].range( linx[k], ",", "-" );
    if ( linx[k].empty() )
      optinx[k].load( istrs[k], "=", ";" );
  }

  int cinx[ n ];  // current indices for the levels
  for ( int k=0; k<n; k++ )
    cinx[k] = 0;

  int tlinx[n];   // index in linx of next target index
  int tinx[n];    // next target indices
  for ( int k=0; k<n; k++ ) {
    tlinx[k] = 0;
    tinx[k] = linx[k].size() > 0 ? linx[k][0] : -1;
  }

  // read in line ranges:
  vector< int > liner;
  lines.range( liner, ",", "-" );

  int newlevel = 0;

  while ( sf.readMetaData() > 0 && sf.good() ) {

    if ( newlevel < sf.newLevels() )
      newlevel = sf.newLevels();

    // select data block:
    bool out = true;
    for ( int k=0; k<n; k++ ) {
      if ( tinx[k] >= 0 ) {
	if ( cinx[k] != tinx[k] ) {
	  out = false;
	  break;
	}
      }
      else {
	for ( int j=0; j<optinx[k].size(); j++ ) {
	  // XXX: match k with metaData levels!
	  Parameter p = sf.metaDataOptions( k )[ optinx[k][j].ident() ];
	  if ( p.isNotype() ||
	       ( p.isAnyNumber() && 
		 p.number( optinx[k][j].unit() ) != optinx[k][j].number() ) ||
	       ( !p.isAnyNumber() &&
		 p.text() != optinx[k][j].text() ) ) {
	    out = false;
	    break;
	  }
	}
      }
    }

    if ( out )
      failed = false;

    // write out meta data:
    if ( out ) {
      bool keyprinted = false;

      // data key before any new meta data blocks:
      if ( metadata && 
	   sf.newDataKey() && 
	   ( newlevel <= 0 ||
	     sf.dataKeyIndex() == sf.metaDataIndex( newlevel-1 ) - 1 ) ) {
	cout << sf.dataKey() << '\n';
	keyprinted = true;
      }

      // meta data blocks:
      for ( int k=newlevel-1; k>=0; k-- ) {
	if ( metadata )
	  cout << sf.metaData( k );

	bool key = ( sf.newDataKey() && sf.dataKeyIndex() == sf.metaDataIndex( k )+1 );
	if ( metadata && ( key || repeatkey || k>0 ) )
	  cout << '\n';

	// data key block following meta data block:
	if ( metadata && key ) {
	  cout << sf.dataKey();
	  keyprinted = true;
	  if ( k>0 )
	    cout << '\n';
	}
      }
      newlevel = 0;

      if ( repeatkey && ! keyprinted && ! sf.dataKey().empty() )
	cout << sf.dataKey();
    }

    // read and probably write data:
    int linenum = 0;
    unsigned int lineinx = 0;
    sf.initData();
    do {
      if ( out ) {
	if ( !erase ) {
	  for ( int k=0; k<sf.emptyLines(); k++ )
	    cout << '\n';
	}
	if ( liner.empty() || 
	     ( lineinx < liner.size() && liner[lineinx] == linenum ) ) {
	  cout << sf.line() << '\n';
	  lineinx++;
	}
	linenum++;
      }
    } while ( sf.readDataLine( stopempty ) );

    // trailing newlines:
    if ( out ) {
      int se = sf.emptyLines();
      if ( erase )
	se = stopempty;
      for ( int k=0; k<se; k++ )
	cout << '\n';
    }

    // update indices:
    int level = n-(sf.emptyLines()-stopempty)-1;
    if ( level < 0 )
      level = 0;
    if ( level < n ) {
      cinx[level]++;
      level++;
    }
    for ( ; level < n; level++ ) {
      cinx[level] = 0;
    }

    if ( out ) {
      int k = 0;
      for ( k=n-1; k>=0; k-- ) {
	if ( linx[k].size() > 0 ) {
	  tlinx[k]++;
	  if ( tlinx[k] < (int)linx[k].size() )
	    break;
	  else {
	    for ( int i=k; i<n; i++ )
	      tlinx[i] = 0;
	  }
	}
	else
	  break;
      }
      if ( k >= n )
	break;
      for ( k=0; k<n; k++ )
	tinx[k] = linx[k].size() > 0 ? linx[k][tlinx[k]] : -1;
    }

  }

  sf.close();

  return failed ? 1 : 0;
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "selectdata -d # -i xxx -l xxx -e -m -k -o xxx fname\n";
  cerr << '\n';
  cerr << "selects a block of data\n";
  cerr << "-d: the number of empty lines that separate blocks of data.\n";
  cerr << "-i: the indices of the selected blocks of data\n";
  cerr << "    or name-value pairs to be used to select data based on the meta data.\n";
  cerr << "    Separate different levels with a colon ':' and values from their names by '='.\n";
  cerr << "    Multiple name-value pairs on the same level are separated by a semi-colon ';'.\n";
  cerr << "    e.g. -i '2:1,5-8' selects blocks 1, 5, 6, 7, 8\n";
  cerr << "    within the next level block with index 2 only\n";
  cerr << "    e.g. -i '3-5:cutoff=50Hz;stdev=5' selects all blocks with their meta data\n";
  cerr << "    matching 'cutoff=50Hz' and 'stdev=5' within the next level blocks\n";
  cerr << "    with indices 3, 4, 5\n";
  cerr << "-l: select a range of line numbers within the data blocks (first line = 0).\n";
  cerr << "-e: erase empty lines within a data block.\n";
  cerr << "-m: only write out data, no meta data.\n";
  cerr << "-k: write key in front of each selected data block.\n";
  cerr << "-o: write selected data into file ### instead to standard out\n";
  cerr << '\n';
  cerr << "Return values:\n";
  cerr << "0: Success.\n";
  cerr << "1: Failed to open files.\n";
  cerr << "2: Requested data block does not exist.\n";
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
  while ( (c = getopt( argc, argv, "d:i:l:eo:mk" )) >= 0 ) {
    switch ( c ) {
      case 'd': if ( optarg == NULL ||
		     sscanf( optarg, "%d", &stopempty ) == 0 ||
		     stopempty < 1 )
		  stopempty = 1;
                break;
      case 'i': if ( optarg != NULL )
		  indices = optarg;
                break;
      case 'l': if ( optarg != NULL )
		  lines = optarg;
                break;
      case 'e': erase = true;
          	break;
      case 'm': metadata = false;
          	break;
      case 'k': repeatkey = true;
          	break;
      case 'o': if ( optarg != NULL )
		  outfile = optarg;
                break;
      default : WriteUsage();
    }
  }
  if ( optind < argc && argv[optind][0] == '?' )
    WriteUsage();
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
  else
    sf.open( cin );

  // redirect cout:
  streambuf *coutb = 0;
  ofstream df;
  if ( !outfile.empty() ) {
    df.open( outfile.c_str() );
    if ( !df.good() ) {
      cerr << "! can't open file " << outfile << " for writing\n";
      return 1;
    }
    coutb = cout.rdbuf();
    streambuf *sb = df.rdbuf();
    cout.rdbuf( sb );
  }

  int r = readData( sf );

  if ( coutb != 0 ) {
    df.close();
    cout.rdbuf( coutb );
  }

  return 2*r;
}
