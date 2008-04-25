/*
  mergedata.cc
  

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
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <getopt.h>
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/datafile.h>
using namespace std;

int stopempty = 2;
bool metadata = true;
bool truncatelines = false;
bool single = false;
string outfile;
vector<int> col; 
vector<string> cols;
vector<double> colthresh;
vector<int> colthreshmode;
vector<Str> addidents;


void readData( DataFile &sf )
{
  // read file:
  deque< StrQueue > data;
  deque< StrQueue > addvals;
  int blocknum = 0;
  int minlines = -1;
  int maxlines = 0;
  unsigned int linewidth = 0;

  do {

    sf.readMetaData();

    // find data column:
    if ( sf.newDataKey() ) {
      for ( unsigned int k=0; k<cols.size(); k++ ) {
	if ( !cols[k].empty() ) {
	  int c = sf.column( cols[k] );
	  if ( c >= 0 )
	    col[k] = c;
	}
      }
    }

    // get values from meta data:
    addvals.push_back( StrQueue() );
    for ( unsigned int k=0; k<addidents.size(); k++ ) {
      if ( addidents[k] == "l" || addidents[k] == "n" )
	addvals.back().add( Str( blocknum ) );
      else {
	int l = -1;
	string ident = addidents[k];
	if ( addidents[k].size() > 2 && addidents[k][1] == '_' ) {
	  l = (int)addidents[k].number( -1.0 );
	  ident = addidents[k].substr( 2 );
	}
	if ( l >= 0 ) {
	  Options::iterator p = sf.metaDataOptions( l ).find( ident );
	  if ( p != sf.metaDataOptions( l ).end() )
	    addvals.back().add( p->text() );
	  else
	    addvals.back().add( "-" );
	}
	else {
	  bool found = false;
	  for ( l=0; l<sf.levels(); l++ ) {
	    Options::iterator p = sf.metaDataOptions( l ).find( ident );
	    if ( p != sf.metaDataOptions( l ).end() ) {
	      addvals.back().add( p->text() );
	      found = true;
	      break;
	    }
	  }
	  if ( ! found )
	    addvals.back().add( "-" );
	}
      }
    }
    blocknum++;

    // read data:
    data.push_back( StrQueue() );
    sf.initData();
    do {
      data.back().add( sf.line() );
      if ( linewidth < sf.line().size() )
	linewidth = sf.line().size();
    } while ( sf.readDataLine( stopempty ) );
    if ( maxlines < data.back().size() )
      maxlines = data.back().size();
    if ( minlines > data.back().size() || minlines < 0 )
      minlines = data.back().size();

  } while (  sf.good() );

  int lines = truncatelines ? minlines : maxlines;
  if ( lines == 0 )
    return;

  // write meta data:
  if ( metadata ) {
    for ( int k=sf.levels()-1; k>0; k-- ) {
      cout << sf.metaData( k ) << '\n';
    }
    if ( ! sf.dataKey().empty() ) {
      cout << sf.dataKey();
    }
  }

  // with of add meta data:
  vector< int > addwidth( addidents.size(), 0 );
  for ( unsigned int j=0; j<addvals.size(); j++ ) {
    for ( int k=0; k<addvals[j].size(); k++ ) {
      if ( addwidth[k] < addvals[j][k].size() )
	addwidth[k] = addvals[j][k].size();
    }
  }

  // merge data:
  if ( col.empty() ) {
    // loop through lines:
    for ( int k=0; k<lines; k++ ) {
      // loop through blocks:
      for ( unsigned int j=0; j<data.size(); j++ ) {
	if ( k < data[j].size() ) {
	  cout << data[j][k];
	  if ( ! addvals[j].empty() ) {
	    int addspace = linewidth - data[j][k].size();
	    if ( addspace > 0 )
	      cout << Str( ' ', addspace );
	    for ( int i=0; i<addvals[j].size(); i++ )
	      cout << "  " << Str( addvals[j][i], addwidth[i], ' ' );
	  }
	  cout << '\n';
	}
      }
      for ( int j=0; j<stopempty; j++ )
	cout << '\n';
    }
  }
  else {
    // loop through blocks:
    bool stop = false;
    for ( unsigned int b=0; b<data.size() && ! stop; b++ ) {
      // loop through lines:
      while ( ! data[b].empty() && ! stop ) {
	// get values:
	StrQueue vals;
	for ( unsigned int c=0; c<col.size(); c++ )
	  vals.add( data[b].front().word( col[c], Str::WhiteSpace, sf.comment() ) );
	// print lines:
	cout << data[b].front();
	if ( ! addvals[b].empty() ) {
	  int addspace = linewidth - data[b].front().size();
	  if ( addspace > 0 )
	    cout << Str( ' ', addspace );
	  for ( int i=0; i<addvals[b].size(); i++ )
	    cout << "  " << Str( addvals[b][i], addwidth[i], ' ' );
	}
	cout << '\n';
	data[b].erase( data[b].begin() );
	if ( ! single ) {
	  for ( StrQueue::iterator k=data[b].begin(); k != data[b].end(); ) {
	    // print matching lines:
	    bool match = true;
	    for ( unsigned int c=0; c<col.size(); c++ ) {
	      Str val = k->word( col[c], Str::WhiteSpace, sf.comment() );
	      double x1 = 0.0, x2 = 0.0;
	      int mode = colthreshmode[c];
	      double thresh = colthresh[c];
	      if ( mode != -2 ) {
		x1 = val.number( HUGE_VAL );
		x2 = vals[c].number( HUGE_VAL );
	      }
	      if ( ( mode == -2 && val != vals[c] ) ||
		   ( mode == 0 && 
		     ( thresh == 0.0 && x1 != x2 ) ||
		     ( thresh > 0.0 && ::fabs( x1 - x2 ) > thresh ) ) ||
		   ( mode == 1 && ( x1 < x2 || x1 > x2 + thresh ) ) ||
		   ( mode == -1 && ( x1 > x2 || x1 < x2 - thresh ) ) ) {
		match = false;
		break;
	      }
	    }
	    if ( match ) {
	      cout << *k;
	      if ( ! addvals[b].empty() ) {
		int addspace = linewidth - k->size();
		if ( addspace > 0 )
		  cout << Str( ' ', addspace );
		for ( int i=0; i<addvals[b].size(); i++ )
		  cout << "  " << Str( addvals[b][i], addwidth[i], ' ' );
	      }
	      cout << '\n';
	      k = data[b].erase( k );
	    }
	    else
	      ++k;
	  }
	}
	// loop through remaining blocks:
	for ( unsigned int j=b+1; j<data.size(); j++ ) {
	  for ( StrQueue::iterator k=data[j].begin(); k != data[j].end(); ) {
	    // print matching lines:
	    bool match = true;
	    for ( unsigned int c=0; c<col.size(); c++ ) {
	      Str val = k->word( col[c], Str::WhiteSpace, sf.comment() );
	      double x1 = 0.0, x2 = 0.0;
	      int mode = colthreshmode[c];
	      double thresh = colthresh[c];
	      if ( mode != -2 ) {
		x1 = val.number( HUGE_VAL );
		x2 = vals[c].number( HUGE_VAL );
	      }
	      if ( ( mode == -2 && val != vals[c] ) ||
		   ( mode == 0 && 
		     ( thresh == 0.0 && x1 != x2 ) ||
		     ( thresh > 0.0 && ::fabs( x1 - x2 ) > thresh ) ) ||
		   ( mode == 1 && ( x1 < x2 || x1 > x2 + thresh ) ) ||
		   ( mode == -1 && ( x1 > x2 || x1 < x2 - thresh ) ) ) {
		match = false;
		break;
	      }
	    }
	    if ( match ) {
	      cout << *k;
	      if ( ! addvals[j].empty() ) {
		int addspace = linewidth - k->size();
		if ( addspace > 0 )
		  cout << Str( ' ', addspace );
		for ( int i=0; i<addvals[j].size(); i++ )
		  cout << "  " << Str( addvals[j][i], addwidth[i], ' ' );
	      }
	      cout << '\n';
	      k = data[j].erase( k );
	      if ( single )
		break;
	    }
	    else
	      ++k;
	  }
	  if ( truncatelines && data[j].empty() )
	    stop = true;
	}
	for ( int j=0; j<stopempty; j++ )
	  cout << '\n';
      }
	
    }
  }
    
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "mergedata [-d #] [-t] [-c ### [-c ### ...]] [-a ### [-a ### ...]] [-s] [-m] [-o xxx] fname\n";
  cerr << '\n';
  cerr << "Rearrange lines from all blocks of data to new blocks of data.\n";
  cerr << '\n';
  cerr << "If no colums are specified (no -c option), then all first lines\n";
  cerr << "of each block of data are merged as the first data block,\n";
  cerr << "all second lines of each block of data are merged as the second\n";
  cerr << "data block, ...\n";
  cerr << '\n';
  cerr << "If columns are specified with the -c option, then all lines\n";
  cerr << "with the same values in the specified columns as the first line\n";
  cerr << "in the first data block are merged as the first data block,\n";
  cerr << "all lines with the same values in the specified columns as the\n";
  cerr << "next line with values different from the first line are merged\n";
  cerr << "as the second data block, ...\n";
  cerr << '\n';
  cerr << "-d: the number of empty lines that separate blocks of data.\n";
  cerr << "-t: truncate output blocks to the minimum number of lines.\n";
  cerr << "-c: ### specifies a column (number or key) used to select lines to be merged.\n";
  cerr << "    If only a column is specified, field values are compared as strings.\n";
  cerr << "    Alternatively, the column specification can be followed by a colon ':'\n";
  cerr << "    and a threshold value (e.g. 'x:3') to enforce a numerical comparison.\n";
  cerr << "    If the threshold is explicitley positive (e.g. 'x:+3') or negative\n";
  cerr << "    (e.g. 'x:-3'), then matches are within the threshold value above or\n";
  cerr << "    below the comparison value, respectively (in [x,x+thresh] or [x-thresh,x]).\n";
  cerr << "    If the threshold is just a number without a sign, then matching values\n";
  cerr << "    must be in in the range [x-threshold,x+threshold].\n";
  cerr << "    If the threshold equals zero, then the values need to match exactly.\n";
  cerr << "-s: take only a single line from each block of data.\n";
  cerr << "-a: Add value of ident ### from meta data to each data line.\n";
  cerr << "-m: only write out data, no meta data.\n";
  cerr << "-o: write merged data into file ### instead to standard out\n";
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
  while ( (c = getopt( argc, argv, "d:tc:sa:o:m" )) >= 0 ) {
    switch ( c ) {
      case 'd': if ( optarg == NULL ||
		     sscanf( optarg, "%d", &stopempty ) == 0 ||
		     stopempty < 1 )
		  stopempty = 1;
                break;
      case 't': truncatelines = true;
          	break;
      case 's': single = true;
          	break;
      case 'c': if ( optarg != NULL ) {
		  Str ident = Str( optarg ).ident();
		  Str value = Str( optarg ).value();
		  double thresh = value.number( HUGE_VAL );
		  colthresh.push_back( ::fabs( thresh ) );
		  if ( thresh == HUGE_VAL )
		    colthreshmode.push_back( -2 );
		  else if ( ! value.empty() && value[0] == '+' )
		    colthreshmode.push_back( 1 );
		  else if ( ! value.empty() && value[0] == '-' )
		    colthreshmode.push_back( -1 );
		  else
		    colthreshmode.push_back( 0 );
                  int c = (int)ident.number( -1.0 );
	          if ( c <= 0 ) {
		    col.push_back( 0 );
		    cols.push_back( ident );
		  }
                  else {
		    col.push_back( c-1 );
		    cols.push_back( "" );
		  }
                }
                break;
      case 'a': if ( optarg != NULL ) {
		  addidents.push_back( optarg );
                }
                break;
      case 'm': metadata = false;
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

  readData( sf );

  if ( coutb != 0 ) {
    df.close();
    cout.rdbuf( coutb );
  }

  return 0;
}
