/*
  plotdata.cc
  

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

#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/datafile.h>
#include <relacs/translate.h>
using namespace relacs;


bool allpages = false;
bool morepages = false;
string format = "02";
int stopempty = 2;
int dindex = 0;
int dindexincr = 1;
bool save = false;
bool view = false;
bool xplot = false;
string term = "postscript eps enhanced color solid \"Helvetica\" 18";
int xtiles = 1;
int ytiles = 1;
string header = "";
Options paramopts;


FILE* openPlot( const string plotgeo="" )
{
  // check DISPLAY:
  char *ds = getenv( "DISPLAY" );
  if ( xplot && ds == NULL ) {
    cerr << "! no DISPLAY available!\n";
    exit( 1 );
  }

  // no DISPLAY needed:
  if ( ! xplot ) {
    unsetenv( "DISPLAY" );
    unsetenv( "TERM" );
  }

  // start gnuplot as a background process:
  string cs = "gnuplot -bg white -title plotdata";
  if ( ! plotgeo.empty() ) 
    cs += " -geometry " + plotgeo;
  FILE *plt = popen( cs.c_str(), "w" );
  // check for success:
  if ( plt == NULL ) {
    perror( "can't open gnuplot" );
    exit( 1 );
  }

  return plt;
}


void readData( const Str &datafile, const string &plotcommandfile,
	       const Str &plotfile )
{
  // open gnuplot:
  FILE *plt = openPlot();
  //  OpenDataPipes( 1 );

  // read in plot commands:
  ifstream pcf( plotcommandfile.c_str() );
  if ( ! pcf.good() ) {
    cerr << "can't open '" << plotcommandfile << "'\n";
    exit( 1 );
  }
  StrQueue plotcommands;
  plotcommands.load( pcf );
  pcf.close();

  // read in plot header:
  StrQueue plotheader;
  if ( !header.empty() ) {
    ifstream phf( header.c_str() );
    if ( ! phf.good() ) {
      cerr << "can't open '" << header << "'\n";
    }
    plotheader.load( phf );
    phf.close();
  }

  // open data file:
  DataFile sf;
  sf.open( datafile );
  if ( !sf.good() ) {
    cerr << "can't open '" + datafile + "'\n";
    return;
  }

  FILE *TF = view ? stdout : plt;

  // data options:
  Options plotopts;
  plotopts.addText( "file", datafile );
  plotopts.addText( "path", datafile.dir() );
  plotopts.addText( "name", datafile.name() );
  plotopts.addText( "ext", datafile.extension() );
  //  plotopts.addText( "plotfile", save ? DataPipe[0] : datafile );
  plotopts.addText( "plotfile", datafile );
  plotopts.addInteger( "index", 0 );
  plotopts.addInteger( "page", 0 );

  bool multiplot = false;
  double dx = 1.0;
  double dy = 1.0;
  int xc = 0;
  int yc = 0;
  if ( xtiles > 1 || ytiles > 1 ) {
    dx = 1.0/xtiles;
    dy = 1.0/ytiles;
    multiplot = true;
    plotopts.addNumber( "xsize", dx );
    plotopts.addNumber( "ysize", dy );
    plotopts.addNumber( "xorig", 0.0 );
    plotopts.addNumber( "yorig", 0.0 );
    plotopts.addInteger( "cols", xtiles );
    plotopts.addInteger( "rows", ytiles );
    plotopts.addInteger( "cinx", 0 );
    plotopts.addInteger( "rinx", 0 );
  }

  bool plotted = false;
  bool first = true;
  int pindex = 0;
  TableKey tk;
  while ( sf.read( stopempty ) && 
	  !( allpages && plotted ) ) {

    // load key:
    if ( sf.newDataKey() )
      tk.loadKey( sf.dataKey() );
    
    if ( !(morepages && pindex == dindex) && pindex >= dindex ) {
      
      dindex += dindexincr;

      plotopts.setInteger( "index", pindex );
      plotopts.setInteger( "page", pindex+1 );

      // options vector:
      vector< Options > opts;
      opts.resize( 2 + sf.levels() );
      opts[0] = plotopts;
      opts[1] = paramopts;
      for ( int k=0; k<sf.levels(); k++ )
        opts[2+k].load( sf.metaData( k ).strippedComments( "-#" ) );
      
      // output term and file:
      if ( !multiplot || first ) {
	if ( xplot ) {
	  fprintf( TF, "set term X\n" );
	  fprintf( TF, "set out\n" );
	}
	else {
	  fprintf( TF, "set term %s\n", term.c_str() );
	  Str pf = translate( plotfile, opts, "dp123456789", "-" );
	  if ( allpages || multiplot ) {
	    if ( pf.extension().empty() )
	      pf += ".eps";
	  }
	  else {
	    if ( pf.find( '%' ) < 0 ) {
	      pf += "%" + format + "n.eps";
	    }
	    pf.format( pindex+1, 'n', 'd' );
	  }
	  fprintf( TF, "set out '%s'\n", pf.c_str() );
	}
      }

      if ( multiplot ) {
	plotopts.setNumber( "xorig", xc*dx );
	plotopts.setNumber( "yorig", (ytiles-yc-1)*dy );
	plotopts.setInteger( "cinx", xc );
	plotopts.setInteger( "rinx", yc );
	if ( xc == 0 && yc == 0 ) {
	  if ( ! first  )
	    fprintf( TF, "set nomultiplot\n" );
	  fprintf( TF, "set size 1, 1\n" );
	  fprintf( TF, "set multiplot\n" );
	  // plot header:
	  if ( !plotheader.empty() ) {
	    StrQueue sq;
	    translate( plotheader, sq, opts, "dp123456789", "-" );
	    for ( int k=0; k<sq.size(); k++ )
	      fprintf( TF, "%s\n", sq[k].c_str() );
	  }
	}
	fprintf( TF, "set origin %g, %g\n", xc*dx, (ytiles-yc-1)*dy );
	fprintf( TF, "set size %g, %g\n", dx, dy );
	xc++;
	if ( xc >= xtiles ) {
	  xc = 0;
	  yc++;
	  if ( yc >= ytiles )
	    yc = 0;
	}
      }
      else {
	if ( !plotheader.empty() ) {
	  StrQueue sq;
	  translate( plotheader, sq, opts, "dp123456789", "-" );
	  for ( int k=0; k<sq.size(); k++ )
	    fprintf( TF, "%s\n", sq[k].c_str() );
	}
      }

      // plot commands:
      StrQueue sq;
      translate( plotcommands, sq, opts, "dp123456789k", "-", &tk );
      for ( int k=0; k<sq.size(); k++ )
	fprintf( TF, "%s\n", sq[k].c_str() );

      fflush( plt );
      /*
      if ( save ) {
	DF = fopen( "plot.dat", "r" );
	if ( DF != NULL ) {
	  PF = fopen( DataPipe[0], "w" );
	  if ( PF != NULL ) {
	    while ( fgets( s, 1000, DF ) != NULL )
	      fputs( s, PF );
	    fclose( PF );
	  }
	  fclose( DF );
	  sleep( 2 );
	}
      }
      */
      if ( xplot )
	getchar();

      plotted = true;
      first = false;
    }
    pindex++;
  };

  if ( multiplot )
    fprintf( TF, "set nomultiplot\n" );

  sf.close();

  //  CloseDataPipes();
  pclose( plt );
}

 
void writeUsage()
{
  cerr << "\nusage:\n";
  cerr << '\n';
  cerr << "plotpage [-a|-m] [-d ##] [-f ##] [-i ##] [-s] [-p ## [-p ## ...]] [-v] [-x]\n";
  cerr << "         [-t xxx] [-g axb] [-h header] datafile cmdfile plotfile\n";
  cerr << '\n';
  cerr << "Plot the data contained in <datafile>\n";
  cerr << "using <cmdfile> into <plotfile>\n";
  cerr << "<cmdfile> contains gnuplot commands and can access metadata (see below).\n";
  cerr << '\n';
  cerr << "  -a: plot all pages at once. Exits after first set of data.\n";
  cerr << "  -m: plot all pages at once, but only if there is more than one set of data.\n";
  cerr << "  -d: the number of empty lines that separate blocks of data (default: 2)\n";
  cerr << "  -f: format of the file number, i.e. the width. A preceding '0' is possible.\n";
  cerr << "  -i: start plotting set of data with index ##.\n";
  cerr << "  -e: plot every ##-nd set of data.\n";
  cerr << "  -s: save data of a page to an extra file and pass it to the plot.\n";
  cerr << "  -p: add parameter ## to the meta-data information (accessible via $(p XXX) ).\n";
  cerr << "  -v: view translated plot command file, don't plot.\n";
  cerr << "  -t: Define terminal\n";
  cerr << "      (default: postscript eps enhanced color solid \"Helvetica\" 18).\n";
  cerr << "  -x: plot to screen and not into file.\n";
  cerr << "  -g: Put multiple plots on a page: <a> columns, <b> rows.\n";
  cerr << "  -h: A file containing plot commands for printing a header.\n";
  cerr << '\n';
  cerr << "Output terminal and files:\n";
  cerr << "  If <plotfile> does not contain a '%', then an integer formatted\n";
  cerr << "  according to the -f option and the extension '.eps' are appended.\n";
  cerr << "  Otherwise a '%n' printf - type format specifier is replaced by the plot\n";
  cerr << "  number. This is usefull if you specify a terminal (-t) that does not\n";
  cerr << "  produce .eps files.\n";
  cerr << "  In both cases every occurence of '$(xxx)' is replaced by the value\n";
  cerr << "  of the metadata xxx found in the datafile.\n";
  cerr << "  $(l xxx) specifies the level l of meta data where xxx should be searched.\n";
  cerr << "  $(l%guu xxx) allows to format the found value with a printf-style\n";
  cerr << "  format specifier %g. Numerical values may be converted to the (optional)\n";
  cerr << "  unit uu.\n";
  cerr << '\n';
  cerr << "Accessing meta data:\n";
  cerr << "Each occurence of '$(iffuu:dd nn)' in the plot command file\n";
  cerr << "is replaced by the corresponding meta-data value.\n";
  cerr << "'i'  is a specifier of the meta-data level. '1' is the lowest level.\n";
  cerr << "     There are three special levels:\n";
  cerr << "     - the additional parameters from the command line\n";
  cerr << "       (-p option) are stored in 'p'.\n";
  cerr << "     - some special variables are stored in 'd' (see below)\n";
  cerr << "     - the column numbers of the data file are accessible via 'k'\n";
  cerr << "       (see below for details)\n";
  cerr << "     If no level is specified, than 'nn' is searched in all meta-data levels\n";
  cerr << "     except the table key.\n";
  cerr << "'ff' is an optional C-style format specifier, either for floating point\n";
  cerr << "     numbers (%f,%g, %e), their associated errors (%F,%G,%E), for strings (%s),\n";
  cerr << "     and file pathes (%p: path, %d: file name without path, \n";
  cerr << "     %n: file name without path and extenion, %x: extension)\n";
  cerr << "'uu' is an optional unit\n";
  cerr << "'dd' a default value in case 'nn' is not not found.\n";
  cerr << "'nn' is the name that is searched in the meta-data.\n";
  cerr << "\n";
  cerr << "Special variables $(d xxx):\n";
  cerr << "  file: the name of the data file.\n";
  cerr << "  path: the path of the data file (inclusively the trailing '/').\n";
  cerr << "  name: the name of the data file (without path and extension).\n";
  cerr << "  ext: the extension of the data file.\n";
  cerr << "  index: the index of the current block of data.\n";
  cerr << "  page: the index + 1 of the current block of data.\n";
  cerr << "  In case of multiple plots (-g) the following variabels are defined as well:\n";
  cerr << "  xsize: horizontal width of the sub-plot (0..1).\n";
  cerr << "  ysize: vertical width of the sub-plot (0..1).\n";
  cerr << "  xorig: horizontal position of the sub-plot (0..1).\n";
  cerr << "  yorig: vertical position of the sub-plot (0..1).\n";
  cerr << "  cols: number of columns.\n";
  cerr << "  rows: number of rows.\n";
  cerr << "  cinx: current column index.\n";
  cerr << "  rinx: current row index.\n";
  cerr << "\n";
  cerr << "Additional variables $(p xxx):\n";
  cerr << "  Additional variables that were added with the -p option are accessible\n";
  cerr << "  via a 'p' in the $(p xxx) constructs.\n";
  cerr << "\n";
  cerr << "Table columns $(k xxx):\n";
  cerr << "  If the data file contains a table key then it can be used to\n";
  cerr << "  retrieve the corresponding column number via $(k xxx).\n";
  cerr << "  By default, the first column is the column number 0.\n";
  cerr << "  Adding a '+' right after the 'k' sets the first column to\n";
  cerr << "  column number 1 ( $(k+ xxx) ), as it is needed for gnuplot.\n";
  cerr << "  Adding '+nn' right after the 'k', where nn is an integer,\n";
  cerr << "  sets the first column to column number nn ( $(k+2 xxx) ).\n";
  cerr << "\n";
  exit( 1 );
}


void readArgs( int argc, char *argv[], int &filec )
{
  int c;

  if ( argc <= 1 )
    writeUsage();
  optind = 0;
  opterr = 0;
  while ( (c = getopt( argc, argv, "amd:f:i:e:sp:vt:g:h:x" )) >= 0 )
    switch ( c ) {
    case 'a': 
      allpages = true;
      break;
    case 'm': 
      allpages = true;
      morepages = true;
      break;
    case 'd':
      sscanf( optarg, " %d", &stopempty );
      break;
    case 'f':
      format = optarg;
      break;
    case 'i':
      sscanf( optarg, " %d", &dindex );
      break;
    case 'e':
      sscanf( optarg, " %d", &dindexincr );
      break;
    case 's': 
      save = true;
      break;
    case 'p': {
      Parameter p( optarg );
      paramopts.add( p );
      break;}
    case 'v': 
      view = true;
      break;
    case 't': 
      term = optarg;
      break;
    case 'h': 
      header = optarg;
      break;
    case 'g': 
      sscanf( optarg, " %d x %d", &xtiles, &ytiles );
      break;
    case 'x': 
      xplot = true;
      break;
    default : writeUsage();
    }
  if ( optind >= argc || argv[optind][0] == '?' )
    writeUsage();
  filec = optind;
  if ( argc - optind < 3 - xplot )
    writeUsage();
}


int main( int argc, char *argv[] )
{
  int filec = 0;
  readArgs( argc, argv, filec );

  readData( argv[filec], argv[filec+1], xplot ? "" : argv[filec+2] );

  return 0;
}

