/*
  extractdata.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <cmath>
#include <getopt.h>
#include <vector>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/tablekey.h>

using namespace std;
using namespace relacs;

vector<int> alevel; 
vector<bool> aunit; 
int stopempty = 2;
bool key = false;
bool keyonly = false;
bool numbercols = false;
bool units = true;
bool storemetadata = false;
string destfile = "";
TableKey destkey;


void readData( DataFile &sf )
{
  vector<Parameter*> aparam;
  aparam.reserve( alevel.size() );
  for ( unsigned int k=0; k<alevel.size(); k++ )
    aparam.push_back( &destkey[k] );

  while ( sf.good() ) {
    sf.read( stopempty, 0 );

    // meta data:
    for ( int l=0; l<sf.levels(); l++ ) {
      for ( unsigned int k=0; k<aparam.size(); k++ ) {
	if ( alevel[k] == l || alevel[k] < 0 ) {
	  Options::iterator p = sf.metaDataOptions( l ).find( aparam[k]->ident() );
	  if ( p != sf.metaDataOptions( l ).end() ) {
	    if ( p->isText() ) {
	      aparam[k]->setText( p->text() );
	      if ( ! aunit[k] ) {
		aparam[k]->setType( Parameter::Text );
		aparam[k]->setFormat( "%-10s" );
		aunit[k] = true;
	      }
	    }
	    else {
	      if ( aunit[k] )
		aparam[k]->setNumber( p->number(), p->unit() );
	      else {
		aparam[k]->setNumber( p->number() );
		if ( ! p->unit().empty() )
		  aparam[k]->setUnit( p->unit() );
		aunit[k] = true;
	      }
	    }
	  }
	}
      }
    }

    // store meta data:
    if ( ! keyonly && storemetadata && sf.good() ) {
      for ( int l=sf.levels()-1; l>0; l-- ) {
	if ( sf.newMetaData( l ) ) {
	  if ( ! key )
	    cout << "\n\n";
	  key = true;
	  cout << sf.metaData( l ) << '\n';
	}
      }
    }

    // save key:
    if ( key ) {
      destkey.saveKey( cout, true, numbercols, units );
      key = false;
      if ( keyonly )
	break;
    }

    // save data:
    destkey.saveData( cout );

  }
  sf.close();
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "extractdata [-d ###] [-a aaa] [-k|-K [-U] [-n]] [-m] [-o xxx] fname\n";
  cerr << '\n';
  cerr << "Extract values from the metadata in file <fname> and write them into a table.\n";
  cerr << "-a: value that is added to the table (either column number, column title\n";
  cerr << "    (takes the value from the last line), 'ident:value' (value can be a\n";
  cerr << "    number with an unit or a string), or 'l_ident' (take value of ident\n";
  cerr << "    in meta data level l, l can be ommited))\n";
  cerr << "-k: add key to the output table\n";
  cerr << "-K: just print the key, don't process data\n";
  cerr << "-U: don't print the line with the units in the key\n";
  cerr << "-n: number columns of the key\n";
  cerr << "-m: store higher level meta data in output file\n";
  cerr << "-d: the number of empty lines that separate blocks of data.\n";
  cerr << "-o: write extracted data into file ### instead to standard out\n";
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
  while ( (c = getopt( argc, argv, "d:a:o:kKUnm" )) >= 0 ) {
    switch ( c ) {
    case 'a':
      if ( optarg != NULL ) {
	Str as = optarg;
	string aident = as.ident();
	Str val = as.value();
	if ( ! aident.empty() && ! val.empty() ) {
	  aunit.push_back( true );
	  alevel.push_back( -1 );
	  double e = 0.0;
	  string aunit = "";
	  double aval = val.number( e, aunit, MAXDOUBLE );
	  if ( aval == MAXDOUBLE ) {
	    destkey.addText( aident );
	    destkey.setText( aident, val );
	  }
	  else {
	    destkey.addNumber( aident, aunit, "%7.5g" );
	    destkey.setNumber( aident, aval );
	  }
	}
	else {
	  aunit.push_back( false );
	  alevel.push_back( (int)as.number( -1.0 ) );
	  int pos = as.findFirstNot( "0123456789_" );
	  if ( pos > 0 )
	    as.erase( 0, pos );
	  destkey.addNumber( as, "-", "%7.5g" );
	}
      }
      break;
    case 'k':
      key = true;
      break;
    case 'K':
      key = true;
      keyonly = true;
      break;
    case 'n':
      numbercols = true;
      break;
    case 'U':
      units = false;
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
    case 'm':
      storemetadata = true;
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
  alevel.reserve( 10 );
  aunit.reserve( 10 );
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
