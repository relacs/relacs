/*
  convertdata.cc
  

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
#include <vector>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/tablekey.h>

using namespace std;
using namespace relacs;

int stopempty = 2;
bool keyonly = false;
bool numbercols = false;
bool storemetadata = false;
string format = "l";
string destfile = "";


void readData( DataFile &sf )
{
  cout << "\\documentclass{article}\n";
  cout << "\n";
  cout << "\\begin{document}\n";
  cout << "\n";
  while ( sf.good() ) {
    sf.readMetaData();

    // write out new meta data:
    for ( int l=sf.levels()-1; l>=0; l-- ) {
      if ( sf.newMetaData( l ) ) {
	cout << "\\begin{tabular}{ll}\n";
	for ( int k=0; k<sf.metaData( l ).size(); k++ ) {
	  Str ml = sf.metaData( l )[k];
	  int p = ml.findFirstNot( "#" );
	  if ( p > 0 )
	    ml.erase( 0, p );
	  string ident = ml.ident().latex();
	  string value = ml.value().latexUnit();
	  if ( ident.empty() || value.empty() )
	    cout << "  \\multicolumn{2}{l}{" << ml.latex() << "}\\\\\n";
	  else
	    cout << "  " << ident << " & " << value << " \\\\\n";
	}
	cout << "\\end{tabular}\n\n";
      }
    }

    // write out key:
    sf.key().saveKeyLaTeX( cout, numbercols );

    // write out data:
    int dcs = sf.dataComments().size();
    do {
      int ndcs = sf.dataComments().size();
      if ( ndcs > dcs ) {
	for ( int k=dcs; k<ndcs; k++ ) {
	  Str ml = sf.dataComments()[k];
	  int p = ml.findFirstNot( "#" );
	  if ( p > 0 )
	    ml.erase( 0, p );
	  cout << "  \\multicolumn{2}{l}{" << ml << "}\\\\\n";
	}
	dcs = ndcs;
      }
      StrQueue items;
      sf.splitLine( items );
      StrQueue latexitems;
      for ( int k=0; k<items.size(); k++ )
	latexitems.add( items[k].latexNum() );
      cout << "  " << latexitems.save( " & " ) << " \\\\\n";
    } while ( sf.readDataLine( stopempty ) );
    cout << "  \\hline\n";
    cout << "\\end{tabular}\n";
    cout << "\\vspace{2ex}\n";
    cout << "\n";

  }
  cout << "\n";
  cout << "\\end{document}\n";
  sf.close();
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "convertdata -d ### -K -n -m -f ### -o xxx fname\n";
  cerr << '\n';
  cerr << "Extract values from the metadata in file <fname> and write them into a table.\n";
  cerr << "-K: just print the key, don't process data\n";
  cerr << "-n: number columns of the key\n";
  cerr << "-m: store meta data in output file\n";
  cerr << "-d: the number of empty lines that separate blocks of data.\n";
  cerr << "-f: format of the converted data:\n";
  cerr << "    l - LaTeX\n";
  cerr << "-o: write converted data into file ### instead to standard out\n";
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
  while ( (c = getopt( argc, argv, "d:o:Knm" )) >= 0 ) {
    switch ( c ) {
    case 'K':
      keyonly = true;
      break;
    case 'd':
      if ( optarg == NULL ||
	   sscanf( optarg, "%d", &stopempty ) == 0 ||
	   stopempty < 1 )
	stopempty = 1;
      break;
    case 'o':
      if ( optarg != NULL )
	destfile = optarg;
      break;
    case 'n':
      numbercols = true;
      break;
    case 'm':
      storemetadata = true;
      break;
    case 'f':
      format = optarg;
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

  // redirect cout:
  streambuf *coutb = 0;
  ofstream df;
  if ( !destfile.empty() ) {
    df.open( destfile.c_str() );
    if ( !df.good() ) {
      cerr << "! can't open file " << destfile << " for writing\n";
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
