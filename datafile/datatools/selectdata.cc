/*
  selectdata.cc
  

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
#include <relacs/strqueue.h>
#include <relacs/datafile.h>
using namespace std;
using namespace relacs;

int stopempty = 2;
bool erase = false;
bool metadata = true;
bool repeatkey = false;
string indices;
Str lines;
vector< Str > selectcols;
vector< int > selectcol;
vector< int > selectop;
vector< double > selectval;
string outfile;

string opcodes[7] = { "==", "=", ">=", ">", "<=", "<", "!=" };

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
  vector< vector< int > > optops( n );
  for ( int k=0; k<n; k++ ) {
    istrs[k].range( linx[k], ",", "-" );
    if ( linx[k].empty() ) {
      optinx[k].load( istrs[k], "=<>!", ";" );
      StrQueue os( istrs[k], ";" );
      for ( int j=0; j<os.size(); j++ ) {
	if ( os[j].find( '<' ) > 0 )
	  optops[k].push_back( 1 );
	else if ( os[j].find( '>' ) > 0 )
	  optops[k].push_back( 2 );
	else if ( os[j].find( '!' ) > 0 )
	  optops[k].push_back( 3 );
	else
	  optops[k].push_back( 0 );
      }
    }
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

  sf.readMetaData();

  while ( sf.good() ) {

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
	  if ( p.isAnyNumber() ) {
	    double v1 = p.number( optinx[k][j].unit() );
	    double v2 = optinx[k][j].number();
	    if ( ( optops[k][j] == 0 && fabs( v1 - v2 ) > 1.0e-8 ) ||
		 ( optops[k][j] == 1 && v1 > v2 ) ||
		 ( optops[k][j] == 2 && v1 < v2 ) ||
		 ( optops[k][j] == 3 && fabs( v1 - v2 ) < 1.0e-8 ) ) {
	      out = false;
	      break;
	    }
	  }
	  else if ( p.isNotype() ||
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

    // update selectcol:
    if ( sf.newDataKey() ) {
      for ( unsigned int k=0; k<selectcols.size(); k++ ) {
	if ( ! selectcols[k].empty() )
	  selectcol[k] = sf.column( selectcols[k] );
      }
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
	  bool lout = true;
	  if ( ! selectcols.empty() ) {
	    sf.scanDataLine();
	    int row = sf.data().rows()-1;
	    for ( unsigned int k=0; k<selectcols.size(); k++ ) {
	      double val = sf.data( selectcol[k], row );
	      double thresh = selectval[k];
	      bool comp = true;
	      switch ( selectop[k] ) {
		case 0: comp = ( fabs( val - thresh ) < 1.0e-8 ); break;
		case 1: comp = ( fabs( val - thresh ) < 1.0e-8 ); break;
		case 2: comp = ( val >= thresh - 1.0e-8 ); break;
		case 3: comp = ( val > thresh ); break;
		case 4: comp = ( val <= thresh + 1.0e-8 ); break;
		case 5: comp = ( val < thresh ); break;
		case 6: comp = ( fabs( val - thresh ) >= 1.0e-8 ); break;
	      default: break;
	      }
	      if ( ! comp ) {
		lout = false;
		break;
	      }
	    }
	  }
	  if ( lout ) {
	    cout << sf.line() << '\n';
	    lineinx++;
	  }
	}
	linenum++;
      }
    } while ( sf.readDataLine( stopempty ) );

    // trailing newlines:
    if ( out ) {
      int se = sf.emptyLines();
      if ( erase || ! sf.good() )
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

    sf.readMetaData();

  }

  sf.close();

  return failed ? 1 : 0;
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "selectdata -d # -i xxx -l xxx -s cc:oovv -e -m -k -o xxx fname\n";
  cerr << '\n';
  cerr << "selects blocks and lines of data\n";
  cerr << "-d: the number of empty lines that separate blocks of data.\n";
  cerr << "-i: the indices of the selected blocks of data\n";
  cerr << "    or name-value pairs to be used to select data based on the meta data.\n";
  cerr << "    Separate different levels with a colon ':'\n";
  cerr << "    and values from their names by '=', '<', '>', or '!'.\n";
  cerr << "    Multiple name-value pairs on the same level are separated\n";
  cerr << "    by a semi-colon ';'.\n";
  cerr << "    Examples:\n";
  cerr << "    -i '2:1,5-8' selects blocks 1, 5, 6, 7, 8\n";
  cerr << "    within the next level block with index 2 only.\n";
  cerr << "    -i '3-5:cutoff=50Hz;stdev>5' selects all blocks with their meta data\n";
  cerr << "    matching 'cutoff=50Hz' and 'stdev' greater than 5 within the next level blocks\n";
  cerr << "    with indices 3, 4, 5\n";
  cerr << "-l: select a range of line numbers within the data blocks (first line = 0).\n";
  cerr << "-s: select lines based on their data values:\n";
  cerr << "    cc:oovv defines the column cc, an operand oo, and a comparison value vvv.\n";
  cerr << "    The column is either a number (first column = 0) or a string.\n";
  cerr << "    The operand can be one of ==, >=, >, <=, <, != .\n";
  cerr << "    Several -s options have to be true together for a line to be selected.\n";
  cerr << "    For example 'temperature:>20' selects all lines with the\n";
  cerr << "    data value in the 'temperature' column greater than 20.\n";
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
  while ( (c = getopt( argc, argv, "d:i:l:s:eo:mk" )) >= 0 ) {
    switch ( c ) {
    case 'd':
      if ( optarg == NULL ||
	   sscanf( optarg, "%d", &stopempty ) == 0 ||
	   stopempty < 1 )
	stopempty = 1;
      break;
    case 'i':
      if ( optarg != NULL )
	indices = optarg;
      break;
    case 'l':
      if ( optarg != NULL )
	lines = optarg;
      break;
    case 's':
      if ( optarg != NULL ) {
	Str s( optarg );
	selectcols.push_back( s.ident() );
	selectcol.push_back( (int)::rint( selectcols.back().number( -1.0 ) ) );
	if ( selectcol.back() >= 0 )
	  selectcols.back() = "";
	Str op = s.value();
	selectop.push_back( -1 );
	for ( int k=0; k<7; k++ ) {
	  if ( op.find( opcodes[k] ) == 0 ) {
	    op.erase( 0, opcodes[k].size() );
	    selectop.back() = k;
	    break;
	  }
	}
	selectval.push_back( op.number( 0.0 ) );
      }
      break;
    case 'e':
      erase = true;
      break;
    case 'm':
      metadata = false;
      break;
    case 'k':
      repeatkey = true;
      break;
    case 'o':
      if ( optarg != NULL )
	outfile = optarg;
      break;
    default :
      WriteUsage();
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
