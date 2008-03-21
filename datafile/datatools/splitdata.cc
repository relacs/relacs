/*
  splitdata.cc
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/options.h>
#include <relacs/datafile.h>
#include <relacs/translate.h>
using namespace std;

int stopempty = 2;
bool erase = false;
bool metadata = true;
bool repeatmetadata = false;
bool append = false;
Str outfile;


void readData( DataFile &sf )
{
  ofstream df;
  bool newfile =true;
  int nf = 0;

  do {

    sf.readMetaData();
    
    int level = sf.newLevels();
    if ( repeatmetadata && newfile )
      level = sf.levels();

    if ( newfile ) {
      if ( df.is_open() )
	df.close();
      vector< Options > metaopts;
      metaopts.resize( sf.levels() );
      for ( int k=0; k<sf.levels(); k++ ) {
	metaopts[k].load( sf.metaData( k ).strippedComments( "-#" ) );
      }
      Str filename = translate( outfile, metaopts );
      nf++;
      filename.format( nf, 'n', 'd' );
      if ( append )
	df.open( filename.c_str(), ofstream::out | ofstream::app );
      else
	df.open( filename.c_str() );
      newfile = false;
    }

    // data key before any new meta data blocks:
    if ( metadata && 
	 sf.newDataKey() && 
	 sf.dataKeyIndex() == sf.metaDataIndex( level-1 ) - 1 )
      df << sf.dataKey() << '\n';
    
    // meta data blocks:
    for ( int k=level-1; k>=0; k-- ) {
      if ( metadata )
	df << sf.metaData( k );
      
      bool key = ( sf.newDataKey() && sf.dataKeyIndex() == sf.metaDataIndex( k )+1 );
      if ( metadata && ( key || k>0 ) )
	df << '\n';
      
      // data key block following meta data block:
      if ( metadata && key ) {
	df << sf.dataKey();
	if ( k>0 )
	  df << '\n';
      }
    }

    // read data:
    sf.initData();
    do {
      if ( !erase ) {
	for ( int k=0; k<sf.emptyLines(); k++ )
	  df << '\n';
      }
      df << sf.line() << '\n';
    } while ( sf.readDataLine( stopempty ) );

    int se = sf.emptyLines();
    if ( erase )
      se = stopempty;
    for ( int k=0; k<se; k++ )
      df << '\n';

    // split file:
    if ( se >= stopempty && sf.good() ) {
      newfile =true;
    }

  } while ( sf.good() );

  sf.close();
}


void WriteUsage()

{
  cerr << '\n';
  cerr << "usage:\n";
  cerr << '\n';
  cerr << "splitdata -d # -e -m -r -a -o xxx fname\n";
  cerr << '\n';
  cerr << "split a data file into several files\n";
  cerr << "  -d: the number of empty lines that separate blocks of data (default: 2).\n";
  cerr << "  -e: erase empty lines within a data block.\n";
  cerr << "  -m: only write out data, no meta data.\n";
  cerr << "  -r: repeat all meta data for each new file.\n";
  cerr << "  -a: append data to existing file (instead of overwriting).\n";
  cerr << "  -o: name of the output files.\n";
  cerr << "      A '%n' printf - type format specifier is replaced by the file number.\n";
  cerr << "      Every occurence of '$(xxx)' is replaced by the value\n";
  cerr << "      of the metadata xxx found in the datafile.\n";
  cerr << "      $(l xxx) specifies the level l of meta data where xxx shoulkd be searched.\n";
  cerr << "      $(l%guu xxx) allows to format the found value with a printf-style format specifier %g.\n";
  cerr << "      Numerical values may be converted to the (optional) unit uu.\n";
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
  while ( (c = getopt( argc, argv, "d:eo:mra" )) >= 0 ) {
    switch ( c ) {
      case 'd': if ( optarg == NULL ||
		     sscanf( optarg, "%d", &stopempty ) == 0 ||
		     stopempty < 1 )
		  stopempty = 1;
                break;
      case 'e': erase = true;
          	break;
      case 'm': metadata = false;
          	break;
      case 'r': repeatmetadata = true;
          	break;
      case 'a': append = true;
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

  readData( sf );

  return 0;
}
